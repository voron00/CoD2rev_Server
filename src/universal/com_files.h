#pragma once

#include "../zlib/unzip.h"

#define MAX_IWDFILES 1024
#define MAX_FILEHASH_SIZE 1024
#define MAX_FILE_HANDLES  64
#define MAX_ZPATH 256

#define BASEGAME "main"

#define DEFAULT_CONFIG "default_mp.cfg"

#ifdef DEDICATED
#define CONFIG_NAME "config_mp_server.cfg"
#else
#define CONFIG_NAME "config_mp.cfg"
#endif

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
static_assert((sizeof(iwd_t) == 800), "ERROR: iwd_t size is invalid!");

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
static_assert((sizeof(fileHandleData_t) == 284), "ERROR: fileHandleData_t size is invalid!");

struct searchpath_t
{
	searchpath_t *next;
	iwd_t *iwd;
	directory_t *dir;
	qboolean localized;
	int language;
};
static_assert((sizeof(searchpath_t) == 20), "ERROR: searchpath_t size is invalid!");

extern dvar_t* fs_ignoreLocalized;

int FS_LanguageHasAssets(int iLanguage);
void FS_BuildOSPath(const char *base, const char *game, const char *qpath, char* ospath);
int FS_Seek(int f, int offset, int origin);
int FS_Read(void *buffer, int len, int h);
int FS_ReadFile(const char* qpath, void** buffer);
int FS_FOpenTextFileWrite(const char* filename);
int FS_FOpenFileWrite( const char *filename );
int FS_FOpenFileByMode( const char *qpath, fileHandle_t *f, fsMode_t mode );
void FS_Printf( fileHandle_t h, const char *fmt, ... );
void FS_Flush(int f);
int FS_Write(const void *buffer, int len, int h);
void FS_FreeFile(void* buffer);
void FS_FCloseFile( fileHandle_t f );
void FS_Shutdown();
qboolean FS_Initialized();
void FS_InitFilesystem();
int FS_LoadStack();
void FS_ShutdownServerIwdNames();
void FS_ShutdownServerReferencedIwds();
int FS_PureServerSetLoadedIwds(const char *paksums, const char *paknames);
int FS_FOpenFileRead(const char *filename, fileHandle_t *file, qboolean uniqueFILE);
void FS_ResetFiles();