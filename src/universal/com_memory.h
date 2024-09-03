#pragma once

#include "../qcommon/qcommon.h"
#include "../xanim/xanim_public.h"

#define PAD(base, alignment)	(((base)+(alignment)-1) & ~((alignment)-1))
#define PADP(base, alignment)	((void *) PAD((intptr_t) (base), (alignment)))

#define Com_Memset memset
#define Com_Memcpy memcpy
#define Com_Memcmp memcmp

#define HUNK_MAGIC  0x89537892
#define HUNK_FREE_MAGIC 0x89537893

#define FILEDATA_HASH_SIZE 1024

typedef struct
{
	int permanent;
	int temp;
} hunkUsed_t;

struct hunkHeader_t
{
	unsigned int magic;
	int size;
	const char *name;
	int dummy;
};

#pragma pack(push)
#pragma pack(1)
typedef struct fileData_s
{
	void *data;
	fileData_s *next;
	unsigned char type;
	char name[1];
} fileData_t;
#pragma pack(pop)

enum filedata_type_t
{
	FILEDATA_AITYPE,
	FILEDATA_XMODELPIECES,
	FILEDATA_XMODELSURFS,
	FILEDATA_XMODELPARTS,
	FILEDATA_XMODEL,
	FILEDATA_XANIM,
	FILEDATA_XANIMLIST,
	FILEDATA_STRINGTABLE,
	FILEDATA_VEHICLEDEF
};

void *Z_TryMallocInternal( int size );
void *Z_MallocInternal( int size );
void *Z_MallocGarbageInternal( int size );
void Z_FreeInternal( void *ptr );
char* CopyStringInternal(const char *in);
void* Hunk_FindDataForFile(int type, const char *name);
void Hunk_OverrideDataForFile(int type, const char *name, void *data);
void Hunk_AddData(unsigned char type, void *data, void *(*alloc)(int));
const char* Hunk_SetDataForFile(int type, const char *name, void *data, void *(*alloc)(int));
qboolean Hunk_DataOnHunk(void *data);
void FreeStringInternal(char *str);
void* Hunk_AllocAlignInternal(int size, int aligment);
void *Hunk_AllocInternal( int size );
void* Hunk_AllocateTempMemoryInternal(int size);
void* Hunk_AllocateTempMemoryHighInternal(int size);
void* Hunk_AllocLowAlignInternal(int size, int aligment);
void* Hunk_AllocLowInternal(int n);
void Hunk_ConvertTempToPermLowInternal();
void* Hunk_ReallocateTempMemoryInternal(int size);
void Hunk_FreeTempMemory(void* buf);
void Hunk_ClearData();
void DB_EnumXAssets(XAssetType type, void (*func)(struct XAssetHeader, void *), void *inData, bool includeOverride);
void Hunk_ClearTempMemoryInternal();
void Hunk_ClearTempMemoryHighInternal();
void Hunk_ClearHigh(int memory);
void Hunk_ClearLow(int memory);
void Hunk_Clear();
int Hunk_SetMark();
void Hunk_ClearTempMemoryHigh();
void Hunk_Shutdown();
void* TempMalloc(int size);
char* TempMallocAlign(int size);
char* TempMallocAlignStrict(int size);
void* TempMemorySetPos(char *pos);
void TempMemoryReset();
int Hunk_HideTempMemory();
void Hunk_ShowTempMemory(int memory);
void Com_InitHunkMemory( void );

#define Z_Malloc Z_MallocInternal
#define S_Malloc Z_MallocInternal
#define Z_MallocGarbage Z_MallocGarbageInternal
#define Z_Free Z_FreeInternal

#define Hunk_Alloc Hunk_AllocInternal
#define Hunk_AllocateTempMemory Hunk_AllocateTempMemoryInternal

#define CopyString CopyStringInternal
#define FreeString FreeStringInternal
