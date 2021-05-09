#pragma once

#include "i_shared.h"
#include "zlib/unzip.h"

#define FS_GENERAL_REF	0x01
#define FS_UI_REF		0x02
#define FS_CGAME_REF	0x04
#define FS_QAGAME_REF	0x08

#define MAX_ZPATH	256
#define	MAX_SEARCH_PATHS	4096
#define MAX_FILEHASH_SIZE	1024
#define MAX_FILE_HANDLES 64

#define DEFAULT_CONFIG "default_mp.cfg"

// referenced flags
// these are in loop specific order so don't change the order
#define FS_GENERAL_REF	0x01
#define FS_UI_REF		0x02
#define FS_CGAME_REF	0x04
#define FS_QAGAME_REF	0x08

typedef struct fileInPack_s
{
	unsigned long		pos;		// file info position in zip
	char			*name;		// name of the file
	struct	fileInPack_s*	next;		// next file in the hash
} fileInPack_t;

typedef struct  	//Verified
{
	char			pakFilename[MAX_OSPATH];	// c:\quake3\baseq3\pak0.pk3
	char			pakBasename[MAX_OSPATH];	// pak0
	char			pakGamename[MAX_OSPATH];	// baseq3
	unzFile			handle;						// handle to zip file +0x300
	int			checksum;					// regular checksum
	int			pure_checksum;				// checksum for pure
	int			hasOpenFile;
	int			numfiles;					// number of files in pk3
	int			referenced;					// referenced file flags
	int			hashSize;					// hash table size (power of 2)		+0x318
	fileInPack_t*	*hashTable;					// hash table	+0x31c
	fileInPack_t*	buildBuffer;				// buffer with the filenames etc. +0x320
} pack_t;

typedef struct  	//Verified
{
	char		path[MAX_OSPATH];		// c:\quake3
	char		gamedir[MAX_OSPATH];	// baseq3
} directory_t;

typedef struct searchpath_s  	//Verified
{
	struct searchpath_s *next;
	pack_t		*pack;		// only one of pack / dir will be non NULL
	directory_t	*dir;
	qboolean	localized;
	int		langIndex;
} searchpath_t;

typedef union qfile_gus
{
	FILE*		o;
	unzFile	z;
} qfile_gut;

typedef struct qfile_us
{
	qfile_gut	file;
	qboolean	unique;
} qfile_ut;

//Added manual buffering as the stdio file buffering has corrupted the written files
typedef struct
{
	qfile_ut	handleFiles;
	qboolean	handleSync;
	int		fileSize;
	int		zipFilePos;
	qboolean	zipFile;
	qboolean	streamed;
	char		name[MAX_ZPATH];
	//Not used by filesystem api
	void*		writebuffer;
	int		bufferSize;
	int		bufferPos; //For buffered writes, next write position for logfile buffering
	int		rbufferPos; //next read position
} fileHandleData_t; //0x11C (284) Size

extern int dvar_modifiedFlags;

void FS_InitFilesystem();
long FS_FOpenFileRead(const char *filename, fileHandle_t *file);
long FS_SV_FOpenFileRead( const char *filename, fileHandle_t *fp );
int FS_Read( void *buffer, int len, fileHandle_t f );
int FS_ReadFile( const char *qpath, void **buffer );
int FS_SV_ReadFile( const char *qpath, void **buffer );
void FS_FCloseFile( fileHandle_t f );
void FS_FreeFile( void *buffer );
int FS_LoadStack();
