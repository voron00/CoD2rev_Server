#pragma once

#include "../zlib/unzip.h"

#define MAX_IWDFILES 1024
#define MAX_FILEHASH_SIZE 1024
#define MAX_FILE_HANDLES  64
#define MAX_ZPATH 256
#define NUM_IW_IWDS 25

#define BASEGAME "main"
#define DEMOGAME "demomain"

#define CONFIG_NAME "config_mp_server.cfg"
#define DEFAULT_CONFIG "default_mp.cfg"

enum FsThread
{
	FS_THREAD_MAIN = 0x0,
	FS_THREAD_STREAM = 0x1,
};

enum FsListBehavior
{
	FS_LIST_PURE_ONLY = 0x0,
	FS_LIST_ALL = 0x1,
};

enum fsMode_t
{
	FS_READ,
	FS_WRITE,
	FS_APPEND,
	FS_APPEND_SYNC
};

struct directory_t
{
	char path[MAX_OSPATH];
	char gamedir[MAX_OSPATH];
};

union qfile_gus
{
	FILE* o;
	unzFile z;
};

struct qfile_us
{
	qfile_gus file;
	qboolean unique;
};

struct fileInIwd_t
{
	unsigned long pos;
	char *name;
	fileInIwd_t *next;
};

struct iwd_t
{
	char iwdFilename[MAX_OSPATH];
	char iwdBasename[MAX_OSPATH];
	char iwdGamename[MAX_OSPATH];
	unzFile handle;
	int checksum;
	int pure_checksum;
	int numFiles;
	int referenced;
	int hashSize;
	fileInIwd_t **hashTable;
	fileInIwd_t *buildBuffer;
};
#if defined(__i386__)
static_assert((sizeof(iwd_t) == 800), "ERROR: iwd_t size is invalid!");
#endif

struct fileHandleData_t
{
	qfile_us handleFiles;
	qboolean handleSync;
	int fileSize;
	int zipFilePos;
	qboolean zipFile;
	qboolean streamed;
	char name[MAX_ZPATH];
};
#if defined(__i386__)
static_assert((sizeof(fileHandleData_t) == 284), "ERROR: fileHandleData_t size is invalid!");
#endif

struct searchpath_t
{
	searchpath_t *next;
	iwd_t *iwd;
	directory_t *dir;
	qboolean localized;
	int language;
};
#if defined(__i386__)
static_assert((sizeof(searchpath_t) == 20), "ERROR: searchpath_t size is invalid!");
#endif

/*
==============
FileWrapper_Open
==============
*/
inline FILE *FileWrapper_Open(const char *ospath, const char *mode)
{
	FILE* file;

	file = fopen(ospath, mode);

	if (file != (FILE*)-1)
	{
		return file;
	}

	return 0;
}

/*
==============
FileWrapper_Seek
==============
*/
inline int FileWrapper_Seek(FILE *h, int offset, int origin)
{
	switch (origin)
	{
	case SEEK_SET:
		return fseek(h, offset, SEEK_SET);
	case SEEK_CUR:
		return fseek(h, offset, SEEK_CUR);
	case SEEK_END:
		return fseek(h, offset, SEEK_END);
	}

	return 0;
}

/*
==============
FileWrapper_GetFileSize
==============
*/
inline int FileWrapper_GetFileSize(FILE *h)
{
	int startPos;
	int fileSize;

	startPos = ftell(h);
	fseek(h, 0, SEEK_END);

	fileSize = ftell(h);
	fseek(h, startPos, SEEK_SET);

	return fileSize;
}

extern dvar_t* fs_ignoreLocalized;
extern int fs_numServerIwds;

bool FS_Initialized();
long FS_HashFileName( const char *fname, int hashSize );
int FS_Seek(int f, int offset, int origin);
int FS_Read(void *buffer, int len, fileHandle_t h);
int FS_ReadFile(const char* qpath, void** buffer);
void FS_FreeFile(void* buffer);
fileHandle_t FS_FOpenFileWrite(const char *filename);
fileHandle_t FS_FOpenTextFileWrite(const char* filename);
fileHandle_t FS_FOpenFileAppend(const char *filename);
int FS_FOpenFileByMode(const char *qpath, fileHandle_t *f, fsMode_t mode);
void FS_Printf( fileHandle_t h, const char *fmt, ... );
void FS_Flush(fileHandle_t f);
int FS_Write(const void *buffer, int len, fileHandle_t h);
void FS_FCloseFile( fileHandle_t f );
void FS_Shutdown();
void FS_InitFilesystem();
int FS_LoadStack();
void FS_ShutdownServerIwdNames();
void FS_ShutdownServerReferencedIwds();
void FS_PureServerSetLoadedIwds(const char *paksums, const char *paknames);
int FS_FOpenFileRead(const char *filename, fileHandle_t *file, qboolean uniqueFILE);
int FS_SV_FOpenFileRead( const char *filename, fileHandle_t *fp );
int FS_SV_FOpenFileWrite( const char *filename );
void FS_AddIwdFilesForGameDirectory(const char *path, const char *dir);
int FS_GetFileList(const char *path, const char *extension, FsListBehavior behavior, char *listbuf, int bufsize);
char** FS_ListFiles(const char* path, const char* extension, FsListBehavior behavior, int* numfiles);
void FS_FreeFileList( char **list );
void FS_ConvertPath(char *s);
int FS_WriteFile(const char* filename, const void* buffer, int size);
qboolean FS_iwIwd(char *iwd, const char *base);
const char *FS_LoadedIwdNames();
const char *FS_ReferencedIwdNames();
const char *FS_ReferencedIwdChecksums();
const char *FS_LoadedIwdChecksums();
const char *FS_LoadedIwdPureChecksums();
qboolean FS_VerifyIwd( const char *iwd );
char *FS_GetMapBaseName(const char *mapname);
void FS_ClearIwdReferences();
void FS_Restart(int checksumFeed);
void FS_ResetFiles();