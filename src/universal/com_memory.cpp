#include "com_memory.h"
#include "../qcommon/cmd.h"
#include "../xanim/xanim_public.h"

#define HUNK_MAGIC  0x89537892
#define HUNK_FREE_MAGIC 0x89537893

hunkUsed_t hunk_low, hunk_high;

byte *s_hunkData = NULL;
int s_hunkTotal;

fileData_s* com_hunkData;
fileData_s* com_fileDataHashTable[1024];

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

void *Hunk_AllocAlignInternal(int size, int alignment)
{
	byte* buf;

	hunk_high.permanent += size;
	hunk_high.permanent = ~alignment & (alignment + hunk_high.permanent);
	hunk_high.temp = hunk_high.permanent;

	if (hunk_high.permanent + hunk_low.temp > s_hunkTotal)
	{
		Com_Error(ERR_DROP, "Hunk_AllocAlign failed on %i bytes (total %i MB, low %i MB, high %i MB)",
		          size, s_hunkTotal / 0x100000, hunk_low.temp / 0x100000, hunk_high.temp / 0x100000);
	}

	buf = &s_hunkData[s_hunkTotal - hunk_high.permanent];
	memset(buf, 0, size);

	return buf;
}

void *Hunk_Alloc( int size )
{
	return Hunk_AllocAlignInternal( size, 32 );
}

void* Hunk_AllocateTempMemory(int size)
{
	hunkHeader_t* hdr;
	unsigned char* buf;
	int prev_temp;

	if (!s_hunkData)
	{
		return Z_Malloc(size);
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
		    "\x15" "Hunk_AllocateTempMemory: failed on %i bytes (total %i MB, low %i MB, high %iMB), needs %i more hunk bytes",
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
		Z_Free(buf);
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
			case 2:
				XAnimFreeList((struct XAnim_s*)data);
				break;
			case 4:
				XModelPartsFree((struct XModelPartsLoad*)data);
				break;
			case 6:
				XAnimFree((struct XAnimParts*)data);
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

	for (hash = 0; hash < 0x400; ++hash)
	{
		Hunk_ClearDataFor(com_fileDataHashTable + hash, low, high);
	}
	Hunk_ClearDataFor(&com_hunkData, low, high);
}

void Hunk_Clear()
{
	hunk_low.permanent = 0;
	hunk_low.temp = 0;
	hunk_high.permanent = 0;
	hunk_high.temp = 0;
	Hunk_ClearData();
}

void Hunk_ClearTempMemory()
{
	hunk_low.permanent = 0;
	hunk_high.permanent = 0;
}

void Hunk_ClearTempMemoryHigh()
{
	hunk_low.temp = 0;
	hunk_high.temp = 0;
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

	s_hunkData = (byte *)malloc( s_hunkTotal + 31 );

	if ( !s_hunkData )
	{
		Com_Error( ERR_FATAL, "Hunk data failed to allocate %i megs", s_hunkTotal / ( 1024 * 1024 ) );
	}

	// cacheline align
	s_hunkData = ( byte * )( ( (int)s_hunkData + 31 ) & ~31 );

	Hunk_Clear();

	Cmd_AddCommand( "meminfo", Com_Meminfo_f );
}