#pragma once

#include "i_shared.h"

#define Com_Memset memset
#define Com_Memcpy memcpy

typedef enum
{
	TAG_FREE,
	TAG_GENERAL,
	TAG_BOTLIB,
	TAG_RENDERER,
	TAG_SMALL,
	TAG_STATIC,
	TAG_XZONE,
	TAG_UNZIP,
	TAG_STRINGED,
	TAG_SCRIPTSTRING,
	TAG_SCRIPTDEBUGGER
} memtag_t;

void *Hunk_AllocInternal( int size );
void *Hunk_AllocLowInternal( int size );
void Com_InitHunkMemory( void );

#define Hunk_Alloc Hunk_AllocInternal
#define Hunk_AllocLow Hunk_AllocLowInternal

void *Z_TryMallocInternal( int size );
void *Z_MallocInternal( int size );
void Z_FreeInternal( void *ptr );
char* CopyStringInternal(const char *in);
void FreeStringInternal(char *str);

#define Z_Malloc Z_MallocInternal
#define S_Malloc Z_MallocInternal
#define Z_Free Z_FreeInternal

#define CopyString CopyStringInternal
#define FreeString FreeStringInternal
