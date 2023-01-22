#include "com_memory.h"
#include "../qcommon/cmd.h"
#include "../xanim/xanim_public.h"

#define HUNK_MAGIC  0x89537892
#define HUNK_FREE_MAGIC 0x89537893

#define FILEDATA_HASH_SIZE 1024

hunkUsed_t hunk_low, hunk_high;

byte *s_hunkData = NULL;
byte *s_origHunkData = NULL;
int s_hunkTotal;

fileData_s* com_hunkData;
fileData_s* com_fileDataHashTable[FILEDATA_HASH_SIZE];

int currentPos;

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

void FreeStringInternal(char *str)
{
	if (str)
		Z_FreeInternal(str);
}

void *Hunk_FindDataForFileInternal(int type, const char *name, int hash)
{
	fileData_s* searchFileData;

	for (searchFileData = com_fileDataHashTable[hash]; searchFileData; searchFileData = searchFileData->next)
	{
		if (searchFileData->type == type && !I_stricmp(searchFileData->name, name))
		{
			return searchFileData->data;
		}
	}

	return 0;
}

void* Hunk_AllocAlignInternal(size_t size, int aligment)
{
	byte *buf;

	hunk_high.permanent += size;
	hunk_high.permanent = (aligment - 1 + hunk_high.permanent) & ~(aligment - 1);
	buf = &s_hunkData[s_hunkTotal - hunk_high.permanent];
	hunk_high.temp = hunk_high.permanent;

	if ( hunk_low.temp + hunk_high.permanent > s_hunkTotal )
	{
		Com_Error(
		    ERR_DROP,
		    "\x15Hunk_AllocAlign failed on %i bytes (total %i MB, low %i MB, high %i MB)",
		    size,
		    s_hunkTotal / 0x100000,
		    hunk_low.temp / 0x100000,
		    hunk_high.temp / 0x100000);
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
	unsigned char* buf;
	int prev_temp;

	if (!s_hunkData)
	{
		return Z_MallocInternal(size);
	}

	size += 16;
	prev_temp = hunk_low.temp;
	hunk_low.temp = (hunk_low.temp + 15) & ~15;

	buf = &s_hunkData[hunk_low.temp];
	hunk_low.temp += size;

	if (hunk_high.temp + hunk_low.temp > s_hunkTotal)
	{
		Com_Error(
		    ERR_DROP,
		    "\x15Hunk_AllocateTempMemory: failed on %i bytes (total %i MB, low %i MB, high %i MB), needs %i more hunk bytes",
		    size,
		    s_hunkTotal / 0x100000,
		    hunk_low.temp / 0x100000,
		    hunk_high.temp / 0x100000,
		    hunk_high.temp + hunk_low.temp - s_hunkTotal);
	}

	hdr = (hunkHeader_t*)buf;
	buf = (buf + 16);

	hdr->magic = HUNK_MAGIC;
	hdr->size = hunk_low.temp - prev_temp;

	return (void*)buf;
}

void* Hunk_AllocateTempMemoryHighInternal(int size)
{
	hunk_high.temp += size;
	hunk_high.temp = (hunk_high.temp + 15) & ~15;

	if (hunk_low.temp + hunk_high.temp > s_hunkTotal)
	{
		Com_Error(
		    ERR_DROP,
		    "\x15Hunk_AllocateTempMemoryHigh: failed on %i bytes (total %i MB, low %i MB, high %i MB)",
		    size,
		    s_hunkTotal / 0x100000,
		    hunk_low.temp / 0x100000,
		    hunk_high.temp / 0x100000);
	}

	return &s_hunkData[s_hunkTotal - hunk_high.temp];
}

void* Hunk_AllocLowAlignInternal(size_t size, int aligment)
{
	byte *buf;

	hunk_low.permanent = (aligment - 1 + hunk_low.permanent) & ~(aligment - 1);
	buf = &s_hunkData[hunk_low.permanent];
	hunk_low.permanent += size;
	hunk_low.temp = hunk_low.permanent;

	if ( hunk_low.permanent + hunk_high.temp > s_hunkTotal )
	{
		Com_Error(
		    ERR_DROP,
		    "\x15Hunk_AllocLowAlign failed on %i bytes (total %i MB, low %i MB, high %i MB)",
		    size,
		    s_hunkTotal / 0x100000,
		    hunk_low.temp / 0x100000,
		    hunk_high.temp / 0x100000);
	}

	Com_Memset(buf, 0, size);

	return buf;
}

void Hunk_ConvertTempToPermLowInternal()
{
	hunk_low.permanent = hunk_low.temp;
}

void* Hunk_AllocLowInternal(size_t n)
{
	return Hunk_AllocLowAlignInternal(n, 32);
}

void* Hunk_ReallocateTempMemoryInternal(int size)
{
	hunk_low.temp = (hunk_low.permanent + 31) & ~31;
	hunk_low.temp += size;

	if ( hunk_low.temp + hunk_high.temp > s_hunkTotal )
	{
		Com_Error(
		    ERR_DROP,
		    "\x15Hunk_ReallocateTempMemory: failed on %i bytes (total %i MB, low %i MB, high %i MB)",
		    size,
		    s_hunkTotal / 0x100000,
		    hunk_low.temp / 0x100000,
		    hunk_high.temp / 0x100000);
	}

	return &s_hunkData[(hunk_low.permanent + 31) & ~31];
}

void Hunk_FreeTempMemory(void* buf)
{
	hunkHeader_t* hdr;

	if (s_hunkData)
	{
		hdr = (hunkHeader_t*)((char*)buf - 16);

		if (*((DWORD*)buf - 4) != HUNK_MAGIC)
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

void Hunk_ClearDataFor(fileData_s **pFileData, unsigned char *low, unsigned char *high)
{
	void* data;
	fileData_s* fileData;

	while (*pFileData)
	{
		fileData = *pFileData;

		if (*pFileData >= (fileData_s*)low && fileData < (fileData_s*)high)
		{
			*pFileData = fileData->next;
			data = fileData->data;

			switch (fileData->type)
			{
			case 3:
				XModelPartsFree((struct XModelPartsLoad*)data);
				break;
			case 4:
				XModelFree((struct XModel*)data);
				break;
			case 5:
				XAnimFree((struct XAnimParts*)data);
				break;
			case 6:
				XAnimFreeList((struct XAnim_s*)data);
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
	unsigned char* low;
	unsigned int hash;
	unsigned char* high;

	low = &s_hunkData[hunk_low.permanent];
	high = &s_hunkData[s_hunkTotal - hunk_high.permanent];

	for (hash = 0; hash < FILEDATA_HASH_SIZE; ++hash)
	{
		Hunk_ClearDataFor(com_fileDataHashTable + hash, low, high);
	}

	Hunk_ClearDataFor(&com_hunkData, low, high);
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
	;
}

void Hunk_ClearTempMemory()
{
	Hunk_ClearTempMemoryInternal();
	Hunk_ClearTempMemoryHighInternal();
}

void Hunk_Shutdown()
{
	Z_FreeInternal(s_origHunkData);
	s_hunkData = 0;
	s_origHunkData = 0;
	s_hunkTotal = 0;
	Com_Memset(&hunk_low, 0, sizeof(hunk_low));
	Com_Memset(&hunk_high, 0, sizeof(hunk_high));
}

void* TempMalloc(int size)
{
	int pos;
	byte *buf;

	pos = currentPos + size;
	buf = (byte*)Hunk_ReallocateTempMemoryInternal(currentPos + size) + currentPos;
	currentPos = pos;

	return buf;
}

void* TempMemorySetPos(int pos)
{
	currentPos -= (int)TempMalloc(0) - pos;
	return Hunk_ReallocateTempMemoryInternal(currentPos);
}

void TempMemoryReset()
{
	currentPos = 0;
}

void Hunk_UserCreate()
{
	;
}

void Hunk_UserDestroy()
{
	Hunk_ClearTempMemoryInternal();
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

	s_hunkData = (byte *)malloc( s_hunkTotal );

	if ( !s_hunkData )
	{
		Sys_OutOfMemErrorInternal(__FILE__, __LINE__);
	}

	s_origHunkData = s_hunkData;

	Hunk_Clear();

	Cmd_AddCommand( "meminfo", Com_Meminfo_f );
}