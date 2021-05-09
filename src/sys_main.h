#pragma once

typedef enum {
	// bk001129 - make sure SE_NONE is zero
	SE_NONE = 0,    // evTime is still valid
	SE_KEY,     // evValue is a key code, evValue2 is the down flag
	SE_CHAR,    // evValue is an ascii char
	SE_MOUSE,   // evValue and evValue2 are reletive signed x / y moves
	SE_CONSOLE, // evPtr is a char*
	SE_PACKET   // evPtr is a netadr_t followed by data bytes to evPtrLength
} sysEventType_t;

typedef struct
{
	int evTime;
	sysEventType_t evType;
	int evValue, evValue2;
	int evPtrLength;                // bytes of data pointed to by evPtr, for journaling
	void            *evPtr;         // this must be manually freed if not NULL
} sysEvent_t;

int Sys_Milliseconds( void );
void Sys_ListFilteredFiles( const char *basedir, const char *subdirs, char *filter, char **list, int *numfiles );
char **Sys_ListFiles( const char *directory, const char *extension, char *filter, int *numfiles, qboolean wantsubs );
qboolean Sys_DirectoryHasContents(const char *dir);
void Sys_SetDefaultInstallPath(const char *path);
char *Sys_DefaultInstallPath(void);
const char *Sys_DefaultAppPath(void);
const char *Sys_DefaultCDPath( void );
const char *Sys_DefaultHomePath(void);
void Sys_FreeFileList( char **list );
char *Sys_Cwd( void );
void Sys_OutOfMemErrorInternal(const char *filename, int line);

# define assert(var) { }

void Sys_Error( const char *error, ... );
void  Sys_Print( const char *msg );
sysEvent_t Sys_GetEvent( void );

void Sys_EndStreamedFile( fileHandle_t f );
