#pragma once

#include "../zlib/unzip.h"

#define MAX_IWDFILES 1024
#define MAX_FILEHASH_SIZE 1024
#define MAX_FILE_HANDLES  64
#define MAX_ZPATH 256
#define NUM_IW_IWDS 25

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

long FS_HashFileName( const char *fname, int hashSize );
size_t FS_FileRead(void *ptr, size_t size, size_t n, FILE *stream);
size_t FS_FileWrite(void *ptr, size_t size, size_t n, FILE *s);
FILE* FS_FileOpen(const char *filename, const char *modes);
int FS_FileSeek(FILE *stream, int off, int whence);
int FS_FileClose(FILE *stream);
int FS_LanguageHasAssets(int iLanguage);
void FS_BuildOSPath(const char *base, const char *game, const char *qpath, char* ospath);
int FS_Seek(int f, int offset, int origin);
int FS_Read(void *buffer, int len, int h);

#ifdef __cplusplus
extern "C" {
#endif

int QDECL FS_ReadFile(const char* qpath, void** buffer);
void QDECL FS_FreeFile(void* buffer);

#ifdef __cplusplus
}
#endif

int FS_FOpenTextFileWrite(const char* filename);
int FS_FOpenFileWrite( const char *filename );
int FS_FOpenFileByMode( const char *qpath, fileHandle_t *f, fsMode_t mode );
void FS_Printf( fileHandle_t h, const char *fmt, ... );
void FS_Flush(int f);
int FS_Write(const void *buffer, int len, int h);
void FS_CopyFile(char *fromOSPath, char *toOSPath);
void FS_FCloseFile( fileHandle_t f );
void FS_Shutdown();
qboolean FS_Initialized();
void FS_InitFilesystem();
int FS_LoadStack();
void FS_ShutdownServerIwdNames();
void FS_ShutdownServerReferencedIwds();
int FS_PureServerSetLoadedIwds(const char *paksums, const char *paknames);
int FS_FOpenFileRead(const char *filename, fileHandle_t *file, qboolean uniqueFILE);
int FS_FOpenFileReadStream(const char *filename, fileHandle_t *file, qboolean uniqueFILE);
int FS_SV_FOpenFileRead( const char *filename, fileHandle_t *fp );
fileHandle_t FS_SV_FOpenFileWrite( const char *filename );
int FS_FilenameCompare(const char *s1, const char *s2);
int FS_GetFileList(const char *path, const char *extension, FsListBehavior behavior, char *listbuf, int bufsize);
char **FS_ListFilteredFiles(searchpath_t *searchPath, const char *path, const char *extension, const char *filter, FsListBehavior behavior, int *numfiles);
char** FS_ListFiles(const char* path, const char* extension, FsListBehavior behavior, int* numfiles);
void FS_FreeFileList( char **list );
void FS_ConvertPath(char *s);
qboolean FS_iwIwd(char *iwd, const char *base);
const char *FS_LoadedIwdNames();
const char *FS_ReferencedIwdNames();
const char *FS_ReferencedIwdChecksums();
const char *FS_LoadedIwdChecksums();
const char *FS_LoadedIwdPureChecksums();
char *FS_GetMapBaseName(const char *mapname);
void FS_Startup(const char *gameName);
void FS_ClearIwdReferences();
void FS_Restart(int checksumFeed);
void FS_ResetFiles();