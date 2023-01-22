#pragma once

#include "../qcommon/qcommon.h"

#define Com_Memset memset
#define Com_Memcpy memcpy
#define Com_Memcmp memcmp

#define MIN_DEDICATED_COMHUNKMEGS 1

#define MIN_COMHUNKMEGS 80
#define DEF_COMHUNKMEGS 160
#define MAX_COMHUNKMEGS 160

#define HUNK_MAGIC  0x89537892
#define HUNK_FREE_MAGIC 0x89537893

typedef struct
{
	int permanent;
	int temp;
} hunkUsed_t;

typedef struct
{
	int magic;
	int size;
} hunkHeader_t;

extern byte *s_hunkData;
extern hunkUsed_t hunk_low, hunk_high;
extern int s_hunkTotal;

struct fileData_s
{
	void *data;
	fileData_s *next;
	unsigned char type;
	char name[1];
};

void *Z_TryMallocInternal( int size );
void *Z_MallocInternal( int size );
void *Z_MallocGarbageInternal( int size );
void Z_FreeInternal( void *ptr );
char* CopyStringInternal(const char *in);
void FreeStringInternal(char *str);
void* Hunk_AllocAlignInternal(size_t size, int aligment);
void *Hunk_AllocInternal( int size );
void* Hunk_AllocateTempMemoryInternal(int size);
void* Hunk_AllocateTempMemoryHighInternal(int size);
void* Hunk_AllocLowAlignInternal(size_t size, int aligment);
void* Hunk_AllocLowInternal(size_t n);
void Hunk_ConvertTempToPermLowInternal();
void* Hunk_ReallocateTempMemoryInternal(int size);
void Hunk_FreeTempMemory(void* buf);
void Hunk_ClearData();
void Hunk_ClearTempMemoryInternal();
void Hunk_ClearTempMemoryHighInternal();
void Hunk_ClearHigh(int memory);
void Hunk_ClearLow(int memory);
void Hunk_Clear();
int Hunk_SetMark();
void Hunk_ClearTempMemoryHigh();
void Hunk_ClearTempMemory();
void Hunk_Shutdown();
void* TempMalloc(int size);
void TempMemoryReset();
void Hunk_UserCreate();
void Hunk_UserDestroy();
void Com_InitHunkMemory( void );

#define Z_Malloc Z_MallocInternal
#define S_Malloc Z_MallocInternal
#define Z_MallocGarbage Z_MallocGarbageInternal
#define Z_Free Z_FreeInternal

#define Hunk_Alloc Hunk_AllocInternal
#define Hunk_AllocateTempMemory Hunk_AllocateTempMemoryInternal

#define TempMallocAlign TempMalloc
#define TempMallocAlignStrict TempMallocAlign

#define CopyString CopyStringInternal
#define FreeString FreeStringInternal

struct LargeLocal
{
	LargeLocal(int sizeParam);
	~LargeLocal();

	void* GetBuf();

	int startPos;
	int size;
};

void LargeLocalEnd(int startPos);
void LargeLocalReset();
int LargeLocalBegin(int size);
void* LargeLocalGetBuf(int startPos, int size);
unsigned int LargeLocalRoundSize(int size);