#include "i_common.h"
#include "i_shared.h"
#include "com_memory.h"
#include "sys_main.h"
#include "cmd.h"
#include "filesystem.h"

#define MIN_DEDICATED_COMHUNKMEGS 1
#define MIN_COMHUNKMEGS 80
#define DEF_COMHUNKMEGS 160

#define HUNK_MAGIC  0x89537892
#define HUNK_FREE_MAGIC 0x89537893

typedef struct
{
	int permanent;
	int temp;
} hunkUsed_t;

#if TESTING_LIBRARY == 1
#define s_hunkData (*((byte**)( 0x08515294 )))
#define hunk_low (*((hunkUsed_t*)( 0x08515284 )))
#define hunk_high (*((hunkUsed_t*)( 0x0851528C )))
#define s_hunkTotal (*((int*)( 0x0851529C )))
#else
static byte *s_hunkData = NULL;
static hunkUsed_t hunk_low, hunk_high;
static int s_hunkTotal;
#endif

/*
=================
Hunk_Clear
The server calls this before shutting down or loading a new map
=================
*/
void Hunk_Clear( void )
{
	hunk_low.permanent = 0;
	hunk_low.temp = 0;

	hunk_high.permanent = 0;
	hunk_high.temp = 0;
}

/*
=================
Hunk_AllocInternal
=================
*/
void *Hunk_AllocInternal( int size )
{
	void    *buf;

	// round to cacheline
	size = ( size + 31 ) & ~31;

	hunk_high.permanent += size;
	hunk_high.temp = hunk_high.permanent;

	buf = ( void * )( s_hunkData + s_hunkTotal - hunk_high.permanent );

	if ( hunk_low.temp + hunk_high.temp + size > s_hunkTotal )
	{
		Com_Error( ERR_DROP, "Hunk_AllocInternal failed on %i", size );
	}

	memset( buf, 0, size );
	return buf;
}

/*
=================
Hunk_AllocLowInternal
=================
*/
void *Hunk_AllocLowInternal( int size )
{
	void    *buf;

	// round to cacheline
	size = ( size + 31 ) & ~31;

	hunk_low.permanent += size;
	hunk_low.temp = hunk_low.permanent;

	buf = ( void * )( s_hunkData + s_hunkTotal - hunk_low.permanent );

	if ( hunk_low.temp + hunk_high.temp + size > s_hunkTotal )
	{
		Com_Error( ERR_DROP, "Hunk_AllocLowInternal failed on %i", size );
	}

	memset( buf, 0, size );
	return buf;
}

/*
=================
Com_Meminfo_f
=================
*/
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

/*
=================
Com_InitHunkMemory
=================
*/
void Com_InitHunkMemory( void )
{
	dvar_t  *cv;
	int nMinAlloc;
	const char *pMsg;

	// make sure the file system has allocated and "not" freed any temp blocks
	// this allows the config and product id files ( journal files too ) to be loaded
	// by the file system without redunant routines in the file system utilizing different
	// memory systems
	if ( FS_LoadStack() != 0 )
	{
		Com_Error( ERR_FATAL, "Hunk initialization failed. File system load stack not zero" );
	}

	// allocate the stack based hunk allocator
	cv = Dvar_RegisterInt( "com_hunkMegs", DEF_COMHUNKMEGS, 1, 512, DVAR_LATCH | DVAR_ARCHIVE );

	// if we are not dedicated min allocation is 56, otherwise min is 1
	if ( com_dedicated && com_dedicated->current.integer )
	{
		nMinAlloc = MIN_DEDICATED_COMHUNKMEGS;
		pMsg = "Minimum com_hunkMegs for a dedicated server is %i, allocating %i megs.\n";
	}
	else
	{
		nMinAlloc = MIN_COMHUNKMEGS;
		pMsg = "Minimum com_hunkMegs is %i, allocating %i megs.\n";
	}

	if ( cv->current.integer < nMinAlloc )
	{
		s_hunkTotal = 1024 * 1024 * nMinAlloc;
		Com_Printf( pMsg, nMinAlloc, s_hunkTotal / ( 1024 * 1024 ) );
	}
	else
	{
		s_hunkTotal = cv->current.integer * 1024 * 1024;
	}

	s_hunkData = (byte *)malloc( s_hunkTotal + 31 );

	if ( !s_hunkData )
	{
		Sys_OutOfMemErrorInternal(__FILE__, __LINE__);
	}

	// cacheline align
	s_hunkData = ( byte * )( ( (int)s_hunkData + 31 ) & ~31 );

	Hunk_Clear();

	Cmd_AddCommand( "meminfo", Com_Meminfo_f );
}

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
