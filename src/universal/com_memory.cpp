#include "com_memory.h"
#include "com_files.h"

static hunkUsed_t hunk_low, hunk_high;
static byte *s_hunkData = NULL;
static int s_hunkTotal;

static fileData_t* com_hunkData;
static fileData_t* com_fileDataHashTable[FILEDATA_HASH_SIZE];

static int currentPos;

void *Z_TryMallocInternal( int size )
{
	void *buf;

	buf = malloc(size);
	if (buf)
		Com_Memset(buf, 0, size);

	return buf;
}

void *Z_MallocInternal( int size )
{
	void *buf;

	buf = malloc(size);
	if (!buf)
		Sys_OutOfMemErrorInternal(__FILE__, __LINE__);
	Com_Memset(buf, 0, size);

	return buf;
}

void *Z_MallocGarbageInternal( int size )
{
	void *buf;

	buf = malloc(size);
	if (!buf)
		Sys_OutOfMemErrorInternal(__FILE__, __LINE__);

	return buf;
}

void Z_FreeInternal( void *ptr )
{
	free(ptr);
}

char* CopyStringInternal(const char *in)
{
	char *copy = (char *)malloc(strlen(in) + 1);
	if (!copy)
		Sys_OutOfMemErrorInternal(__FILE__, __LINE__);
	strcpy(copy, in);

	return copy;
}

void ReplaceStringInternal(char **string, char *replacement)
{
	unsigned int length;
	char *str;

	length = I_strlen(replacement);
	str = *string;

	if ( *string && strlen(str) < length )
	{
		Z_FreeInternal(str);
		str = 0;
	}

	if ( !str )
	{
		str = (char *)Z_MallocInternal(length + 1);
		*string = str;
	}

	strcpy(str, replacement);
}

void FreeStringInternal(char *str)
{
	if (str)
		Z_FreeInternal(str);
}

void *Hunk_FindDataForFileInternal(int type, const char *name, int hash)
{
	fileData_t* searchFileData;

	for (searchFileData = com_fileDataHashTable[hash]; searchFileData; searchFileData = searchFileData->next)
	{
		if (searchFileData->type == type && !I_stricmp(searchFileData->name, name))
		{
			return searchFileData->data;
		}
	}

	return 0;
}

void* Hunk_FindDataForFile(int type, const char *name)
{
	return Hunk_FindDataForFileInternal(type, name, FS_HashFileName(name, FILEDATA_HASH_SIZE));
}

void Hunk_OverrideDataForFile(int type, const char *name, void *data)
{
	fileData_t *i;

	for ( i = com_fileDataHashTable[FS_HashFileName(name, FILEDATA_HASH_SIZE)]; i; i = i->next )
	{
		if ( i->type == type && !strcasecmp(i->name, name) )
		{
			i->data = data;
			return;
		}
	}
}

void Hunk_AddData(unsigned char type, void *data, void *(*alloc)(int))
{
	fileData_t *fileData;

	fileData = (fileData_t *)alloc(sizeof(fileData_t) - 1);
	fileData->data = data;
	fileData->type = type;
	fileData->next = com_hunkData;

	com_hunkData = fileData;
}

const char* Hunk_SetDataForFile(int type, const char *name, void *data, void *(*alloc)(int))
{
	unsigned int size;
	fileData_s *fileData;
	int hash;

	hash = FS_HashFileName(name, FILEDATA_HASH_SIZE);
	size = I_strlen(name) + sizeof(fileData_t);
	fileData = (fileData_s *)alloc(size);
	fileData->data = data;
	fileData->type = type;
	strcpy(fileData->name, name);
	fileData->next = com_fileDataHashTable[hash];
	com_fileDataHashTable[hash] = fileData;

	return fileData->name;
}

qboolean Hunk_DataOnHunk(void *data)
{
	return data >= s_hunkData && data < &s_hunkData[s_hunkTotal];
}

void* Hunk_AllocAlignInternal(int size, int aligment)
{
	void *buf;

	hunk_high.permanent += size;
	hunk_high.permanent = PAD(hunk_high.permanent, aligment);
	buf = &s_hunkData[s_hunkTotal - hunk_high.permanent];
	hunk_high.temp = hunk_high.permanent;

	if ( hunk_low.temp + hunk_high.permanent > s_hunkTotal )
	{
		Com_Error(
		    ERR_DROP,
		    "\x15Hunk_AllocAlign failed on %i bytes (total %i MB, low %i MB, high %i MB)",
		    size,
		    s_hunkTotal / (1024*1024),
		    hunk_low.temp / (1024*1024),
		    hunk_high.temp / (1024*1024));
	}

	Com_Memset(buf, 0, size);

	return buf;
}

void *Hunk_AllocInternal( int size )
{
	return Hunk_AllocAlignInternal( size, 32 );
}

void* Hunk_AllocateTempMemoryInternal(int size)
{
	hunkHeader_t* hdr;
	void* buf;

	if (!s_hunkData)
	{
		return Z_MallocInternal(size);
	}

	size = PAD(size, sizeof(intptr_t)) + sizeof( hunkHeader_t );

	buf = &s_hunkData[hunk_low.temp];
	hunk_low.temp += size;

	if (hunk_high.temp + hunk_low.temp > s_hunkTotal)
	{
		Com_Error(
		    ERR_DROP,
		    "\x15Hunk_AllocateTempMemory: failed on %i bytes (total %i MB, low %i MB, high %i MB), needs %i more hunk bytes",
		    size,
		    s_hunkTotal / (1024*1024),
		    hunk_low.temp / (1024*1024),
		    hunk_high.temp / (1024*1024),
		    hunk_high.temp + hunk_low.temp - s_hunkTotal);
	}

	hdr = (hunkHeader_t *)buf;
	buf = (void *)(hdr+1);

	hdr->magic = HUNK_MAGIC;
	hdr->size = size;

	return buf;
}

void* Hunk_AllocateTempMemoryHighInternal(int size)
{
	hunk_high.temp += size;

	if (hunk_low.temp + hunk_high.temp > s_hunkTotal)
	{
		Com_Error(
		    ERR_DROP,
		    "\x15Hunk_AllocateTempMemoryHigh: failed on %i bytes (total %i MB, low %i MB, high %i MB)",
		    size,
		    s_hunkTotal / (1024*1024),
		    hunk_low.temp / (1024*1024),
		    hunk_high.temp / (1024*1024));
	}

	return &s_hunkData[s_hunkTotal - hunk_high.temp];
}

void* Hunk_AllocLowAlignInternal(int size, int aligment)
{
	void *buf;

	hunk_low.permanent = PAD(hunk_low.permanent, aligment);
	buf = &s_hunkData[hunk_low.permanent];
	hunk_low.permanent += size;
	hunk_low.temp = hunk_low.permanent;

	if ( hunk_low.permanent + hunk_high.temp > s_hunkTotal )
	{
		Com_Error(
		    ERR_DROP,
		    "\x15Hunk_AllocLowAlign failed on %i bytes (total %i MB, low %i MB, high %i MB)",
		    size,
		    s_hunkTotal / (1024*1024),
		    hunk_low.temp / (1024*1024),
		    hunk_high.temp / (1024*1024));
	}

	Com_Memset(buf, 0, size);

	return buf;
}

void Hunk_ConvertTempToPermLowInternal()
{
	hunk_low.permanent = hunk_low.temp;
}

void* Hunk_AllocLowInternal(int size)
{
	return Hunk_AllocLowAlignInternal(size, 32);
}

void* Hunk_ReallocateTempMemoryInternal(int size)
{
	hunk_low.temp = hunk_low.permanent;
	hunk_low.temp += size;

	if ( hunk_low.temp + hunk_high.temp > s_hunkTotal )
	{
		Com_Error(
		    ERR_DROP,
		    "\x15Hunk_ReallocateTempMemory: failed on %i bytes (total %i MB, low %i MB, high %i MB)",
		    size,
		    s_hunkTotal / (1024*1024),
		    hunk_low.temp / (1024*1024),
		    hunk_high.temp / (1024*1024));
	}

	return &s_hunkData[hunk_low.permanent];
}

void Hunk_FreeTempMemory(void* buf)
{
	hunkHeader_t* hdr;

	if (s_hunkData)
	{
		hdr = ( (hunkHeader_t *)buf ) - 1;

		if ( hdr->magic != HUNK_MAGIC )
		{
			Com_Error(ERR_FATAL, "Hunk_FreeTempMemory: bad magic");
		}

		hdr->magic = HUNK_FREE_MAGIC;
		hunk_low.temp -= hdr->size;
	}
	else
	{
		Z_FreeInternal(buf);
	}
}

void Hunk_ClearDataFor(fileData_t **pFileData, unsigned char *low, unsigned char *high)
{
	void* data;
	fileData_t* fileData;

	while (*pFileData)
	{
		fileData = *pFileData;

		if (*pFileData >= (fileData_t*)low && fileData < (fileData_t*)high)
		{
			*pFileData = fileData->next;
			data = fileData->data;

			switch (fileData->type)
			{
			case FILEDATA_XMODELPARTS:
				XModelPartsFree((XModelParts *)data);
				break;
			case FILEDATA_XMODEL:
				XModelFree((XModel *)data);
				break;
			case FILEDATA_XANIM:
				XAnimFree((XAnimParts *)data);
				break;
			case FILEDATA_XANIMLIST:
				XAnimFreeList((XAnim *)data);
				break;
			}
		}
		else
		{
			pFileData = &fileData->next;
		}
	}
}

void Hunk_ClearData()
{
	unsigned char *low;
	unsigned char *high;
	unsigned int hash;

	low = &s_hunkData[hunk_low.permanent];
	high = &s_hunkData[s_hunkTotal - hunk_high.permanent];

	for (hash = 0; hash < FILEDATA_HASH_SIZE; ++hash)
	{
		Hunk_ClearDataFor(com_fileDataHashTable + hash, low, high);
	}

	Hunk_ClearDataFor(&com_hunkData, low, high);
}

void DB_EnumXAssetsFor(fileData_s *fileData, int fileDataType, void (*func)(struct XAssetHeader, void *), void *inData)
{
	while ( fileData )
	{
		if ( fileData->type == fileDataType && fileData->type == FILEDATA_XMODEL )
			((void (*)(void *, void *))func)(fileData->data, inData);

		fileData = fileData->next;
	}
}

void DB_EnumXAssets(XAssetType type, void (*func)(struct XAssetHeader, void *), void *inData, bool includeOverride)
{
	unsigned int i;

	if ( type == ASSET_TYPE_XMODEL )
	{
		for ( i = 0; i < FILEDATA_HASH_SIZE; ++i )
			DB_EnumXAssetsFor(com_fileDataHashTable[i], FILEDATA_XMODEL, func, inData);
	}
}

void Hunk_ClearTempMemoryInternal()
{
	if ( s_hunkData )
		hunk_low.temp = hunk_low.permanent;
}

void Hunk_ClearTempMemoryHighInternal()
{
	hunk_high.temp = hunk_high.permanent;
}

void Hunk_ClearHigh(int memory)
{
	hunk_high.temp = memory;
	hunk_high.permanent = memory;
	Hunk_ClearData();
}

void Hunk_ClearLow(int memory)
{
	hunk_low.temp = memory;
	hunk_low.permanent = memory;
	Hunk_ClearData();
}

void Hunk_Clear()
{
	hunk_low.permanent = 0;
	hunk_low.temp = 0;
	hunk_high.permanent = 0;
	hunk_high.temp = 0;
	Hunk_ClearData();
}

int Hunk_SetMark()
{
	return hunk_high.permanent;
}

void Hunk_ClearTempMemoryHigh()
{
	Hunk_ClearTempMemoryHighInternal();
}

void Hunk_ClearTempMemory()
{
	Hunk_ClearTempMemoryInternal();
	Hunk_ClearTempMemoryHighInternal();
}

void Hunk_Shutdown()
{
	Z_FreeInternal(s_hunkData);
	s_hunkData = 0;
	s_hunkTotal = 0;
	Com_Memset(&hunk_low, 0, sizeof(hunk_low));
	Com_Memset(&hunk_high, 0, sizeof(hunk_high));
}

void* TempMalloc(int size)
{
	int pos;
	char *buf;

	pos = currentPos + size;
	buf = (char *)Hunk_ReallocateTempMemoryInternal(currentPos + size) + currentPos;
	currentPos = pos;

	return buf;
}

char* TempMallocAlign(int size)
{
	return (char *)TempMalloc(size);
}

char* TempMallocAlignStrict(int size)
{
	return (char *)TempMalloc(size);
}

void* TempMemorySetPos(char *pos)
{
	currentPos -= (char *)TempMalloc(0) - pos;
	return Hunk_ReallocateTempMemoryInternal(currentPos);
}

void TempMemoryReset()
{
	currentPos = 0;
}

int Hunk_HideTempMemory()
{
	int permanent;

	permanent = hunk_low.permanent;
	hunk_low.permanent = hunk_low.temp;

	return permanent;
}

void Hunk_ShowTempMemory(int memory)
{
	hunk_low.permanent = memory;
}

void Com_Meminfo_f(void)
{
	Com_Printf("%8i bytes total hunk\n", s_hunkTotal);
	Com_Printf("\n");
	Com_Printf("%8i low permanent\n", hunk_low.permanent);
	if ( hunk_low.temp != hunk_low.permanent )
		Com_Printf("%8i low temp\n", hunk_low.temp);
	Com_Printf("\n");
	Com_Printf("%8i high permanent\n", hunk_high.permanent);
	if ( hunk_high.temp != hunk_high.permanent )
		Com_Printf("%8i high temp\n", hunk_high.temp);
	Com_Printf("\n");
	Com_Printf("%8i total hunk in use\n", hunk_high.permanent + hunk_low.permanent);
	Com_Printf("\n");
}

void Com_InitHunkMemory( void )
{
	dvar_t *com_hunkMegs;

	// make sure the file system has allocated and "not" freed any temp blocks
	// this allows the config and product id files ( journal files too ) to be loaded
	// by the file system without redunant routines in the file system utilizing different
	// memory systems
	if ( FS_LoadStack() != 0 )
	{
		Com_Error( ERR_FATAL, "Hunk initialization failed. File system load stack not zero" );
	}

	// allocate the stack based hunk allocator
	com_hunkMegs = Dvar_RegisterInt( "com_hunkMegs", DEF_COMHUNKMEGS, MIN_DEDICATED_COMHUNKMEGS, MAX_COMHUNKMEGS, DVAR_LATCH | DVAR_ARCHIVE );

	if ( com_hunkMegs->current.integer < MIN_DEDICATED_COMHUNKMEGS )
	{
		s_hunkTotal = 1024 * 1024 * MIN_DEDICATED_COMHUNKMEGS;
	}
	else
	{
		s_hunkTotal = com_hunkMegs->current.integer * 1024 * 1024;
	}

	s_hunkData = (byte *)Z_MallocInternal( s_hunkTotal );

	Hunk_Clear();

	Cmd_AddCommand( "meminfo", Com_Meminfo_f );
}