#include "i_shared.h"
#include "server.h"
#include "dvar.h"
#include "sys_main.h"
#include "sys_thread.h"

#define MAX_QUED_EVENTS 256
#define MAX_CMD 1024

static char binaryPath[ MAX_OSPATH ] = { 0 };
static char installPath[ MAX_OSPATH ] = { 0 };
static char exeFilename[ MAX_OSPATH ] = { 0 };
static char exeFilenameShort[ MAX_OSPATH ] = { 0 };

#ifndef MAXPRINTMSG
#define MAXPRINTMSG 1024
#endif

static char homePath[MAX_OSPATH];

#define MAX_FOUND_FILES 0x1000

uid_t saved_euid;
qboolean stdin_active = qtrue;

#define MAX_EDIT_LINE   256
typedef struct
{
	int cursor;
	int scroll;
	int widthInChars;
	char buffer[MAX_EDIT_LINE];
} field_t;

// bk000306: upped this from 64
#define	MAX_QUED_EVENTS		256
#define	MASK_QUED_EVENTS	( MAX_QUED_EVENTS - 1 )

sysEvent_t	eventQue[MAX_QUED_EVENTS];
// bk000306: initialize
int		eventHead = 0;
int             eventTail = 0;
byte		sys_packetReceived[MAX_MSGLEN];

/*
================
Sys_Milliseconds
================
*/
/* base time in seconds, that's our origin
   timeval:tv_sec is an int:
   assuming this wraps every 0x7fffffff - ~68 years since the Epoch (1970) - we're safe till 2038
   using unsigned long data type to work right with Sys_XTimeToSysTime */
unsigned long sys_timeBase = 0;
/* current time in ms, using sys_timeBase as origin
   NOTE: sys_timeBase*1000 + curtime -> ms since the Epoch
	 0x7fffffff ms - ~24 days
   although timeval:tv_usec is an int, I'm not sure wether it is actually used as an unsigned int
	 (which would affect the wrap period) */
int curtime;
int Sys_Milliseconds( void )
{
	struct timeval tp;

	gettimeofday( &tp, NULL );

	if ( !sys_timeBase )
	{
		sys_timeBase = tp.tv_sec;
		return tp.tv_usec / 1000;
	}

	curtime = ( tp.tv_sec - sys_timeBase ) * 1000 + tp.tv_usec / 1000;

	return curtime;
}

/*
==================
Sys_ListFilteredFiles
==================
*/
void Sys_ListFilteredFiles( const char *basedir, const char *subdirs, char *filter, char **list, int *numfiles )
{
	char          search[MAX_OSPATH], newsubdirs[MAX_OSPATH];
	char          filename[MAX_OSPATH];
	DIR           *fdir;
	struct dirent *d;
	struct stat   st;

	if ( *numfiles >= MAX_FOUND_FILES - 1 )
	{
		return;
	}

	if (strlen(subdirs))
	{
		Com_sprintf( search, sizeof(search), "%s/%s", basedir, subdirs );
	}
	else
	{
		Com_sprintf( search, sizeof(search), "%s", basedir );
	}

	if ((fdir = opendir(search)) == NULL)
	{
		return;
	}

	while ((d = readdir(fdir)) != NULL)
	{
		Com_sprintf(filename, sizeof(filename), "%s/%s", search, d->d_name);
		if (stat(filename, &st) == -1)
			continue;

		if (st.st_mode & S_IFDIR)
		{
			if (I_stricmp(d->d_name, ".") && I_stricmp(d->d_name, ".."))
			{
				if (strlen(subdirs))
				{
					Com_sprintf( newsubdirs, sizeof(newsubdirs), "%s/%s", subdirs, d->d_name);
				}
				else
				{
					Com_sprintf( newsubdirs, sizeof(newsubdirs), "%s", d->d_name);
				}
				Sys_ListFilteredFiles( basedir, newsubdirs, filter, list, numfiles );
			}
		}
		if ( *numfiles >= MAX_FOUND_FILES - 1 )
		{
			break;
		}
		Com_sprintf( filename, sizeof(filename), "%s/%s", subdirs, d->d_name );
		if (!Com_FilterPath( filter, filename, qfalse ))
			continue;
		list[ *numfiles ] = CopyString( filename );
		(*numfiles)++;
	}

	closedir(fdir);
}

/*
==================
Sys_ListFiles
==================
*/
char **Sys_ListFiles( const char *directory, const char *extension, char *filter, int *numfiles, qboolean wantsubs )
{
	/*Looks like the includefile sys/stat.h has a wrong type size. Bugfix */
	struct stat_size_fix
	{
		struct stat st;
		int guard1;
		int guard2;
	};

	struct dirent *d;
	DIR           *fdir;
	qboolean      dironly = wantsubs;
	char          search[MAX_OSPATH];
	int           nfiles;
	char          **listCopy;
	char          *list[MAX_FOUND_FILES];
	int           i;
	struct        stat_size_fix st;
	unsigned int           extLen;

	if (filter)
	{

		nfiles = 0;
		Sys_ListFilteredFiles( directory, "", filter, list, &nfiles );

		list[ nfiles ] = NULL;
		*numfiles = nfiles;

		if (!nfiles)
			return NULL;

		listCopy = (char **)Z_Malloc( ( nfiles + 1 ) * sizeof( *listCopy ) );
		for ( i = 0 ; i < nfiles ; i++ )
		{
			listCopy[i] = list[i];
		}
		listCopy[i] = NULL;

		return listCopy;
	}

	if ( !extension)
		extension = "";

	if ( extension[0] == '/' && extension[1] == 0 )
	{
		extension = "";
		dironly = qtrue;
	}

	extLen = strlen( extension );

	// search
	nfiles = 0;

	if ((fdir = opendir(directory)) == NULL)
	{
		*numfiles = 0;
		return NULL;
	}
	while ((d = readdir(fdir)) != NULL)
	{

		Com_sprintf(search, sizeof(search), "%s/%s", directory, d->d_name);

		if (stat(search, &st.st) == -1)
			continue;

		if ((dironly && !(st.st.st_mode & S_IFDIR)) ||
		        (!dironly && (st.st.st_mode & S_IFDIR)))
			continue;

		if (*extension)
		{
			if ( strlen( d->d_name ) < extLen ||
			        I_stricmp(
			            d->d_name + strlen( d->d_name ) - extLen,
			            extension ) )
			{
				continue; // didn't match
			}
		}
		if ( nfiles == MAX_FOUND_FILES - 1 )
			break;
		list[ nfiles ] = CopyString( d->d_name );
		nfiles++;
	}

	list[ nfiles ] = NULL;

	closedir(fdir);

	// return a copy of the list
	*numfiles = nfiles;

	if ( !nfiles )
	{
		return NULL;
	}

	listCopy = (char **)S_Malloc( ( nfiles + 1 ) * sizeof( *listCopy ) );
	for ( i = 0 ; i < nfiles ; i++ )
	{
		listCopy[i] = list[i];
	}
	listCopy[i] = NULL;

	return listCopy;
}

/*
==================
Sys_DirectoryHasContents
==================
*/
qboolean Sys_DirectoryHasContents(const char *dir)
{
	DIR *hdir;
	struct dirent *hfiles;

	hdir = opendir(dir);

	if ( hdir )
	{
		hfiles = readdir(hdir);
		while ( hfiles )
		{
			if ( hfiles->d_reclen != 4 || hfiles->d_name[0] != '.' )
			{
				closedir(hdir);
				return qtrue;
			}
			hfiles = readdir(hdir);
		}
		closedir(hdir);
	}

	return qfalse;
}

/*
==================
Sys_FreeFileList
==================
*/
void Sys_FreeFileList( char **list )
{
	int i;

	if ( !list )
	{
		return;
	}

	for ( i = 0 ; list[i] ; i++ )
	{
		FreeString( list[i] );
	}

	Z_Free( list );
}

/*
==================
Sys_Cwd
==================
*/
char *Sys_Cwd( void )
{
	static char cwd[MAX_OSPATH];

	char *result = getcwd( cwd, sizeof( cwd ) - 1 );
	if( result != cwd )
		return NULL;

	cwd[MAX_OSPATH-1] = 0;

	return cwd;
}

/*
==================
Sys_DefaultHomePath
==================
*/
const char *Sys_DefaultHomePath(void)
{
	char *p;

	if( !*homePath )
	{
		if( ( p = getenv( "HOME" ) ) != NULL )
		{
			Com_sprintf(homePath, sizeof(homePath), "%s%c", p, PATH_SEP);
			I_strncat(homePath, sizeof(homePath), HOMEPATH_NAME_UNIX);
		}
	}

	return homePath;
}

/*
=================
Sys_SetBinaryPath
=================
*/
void Sys_SetBinaryPath(const char *path)
{
	I_strncpyz(binaryPath, path, sizeof(binaryPath));
}

/*
=================
Sys_SetExeFile
=================
*/
void Sys_SetExeFile(const char *filepath)
{
	I_strncpyz(exeFilename, filepath, sizeof(exeFilename));
}

void Sys_SetExeFileShort(const char *filepath)
{
	I_strncpyz(exeFilenameShort, filepath, sizeof(exeFilenameShort));
}

/*
=================
Sys_ExeFile
=================
*/
const char* Sys_ExeFile( void )
{
	return exeFilename;
}

const char* Sys_ExeFileShort( void )
{
	return exeFilenameShort;
}

/*
=================
Sys_BinaryPath
=================
*/
const char *Sys_BinaryPath(void)
{
	return binaryPath;
}

/*
=================
Sys_SetDefaultInstallPath
=================
*/
void Sys_SetDefaultInstallPath(const char *path)
{
	I_strncpyz(installPath, path, sizeof(installPath));
}

/*
=================
Sys_DefaultInstallPath
=================
*/
char *Sys_DefaultInstallPath(void)
{
	if (*installPath)
		return installPath;
	else
		return Sys_Cwd();
}

/*
=================
Sys_DefaultCDPath
=================
*/
const char *Sys_DefaultCDPath(void)
{
	return "";
}

/*
=================
Sys_DefaultAppPath
=================
*/
const char *Sys_DefaultAppPath(void)
{
	return Sys_BinaryPath();
}

void Sys_OutOfMemErrorInternal(const char *filename, int line)
{
	fprintf(stderr, "OUT OF MEMORY! ABORTING!!! (%s:%d)\n", filename, line);
	exit(-1);
}

// bk010104 - added for abstraction
void Sys_Exit( int ex )
{
#ifndef NDEBUG // regular behavior
	// We can't do this
	//  as long as GL DLL's keep installing with atexit...
	//exit(ex);
	_exit(ex);
#else
	// Give me a backtrace on error exits.
	assert( ex == 0 );
	exit(ex);
#endif
}

void Sys_PrintBinVersion( const char* name )
{
	const char* date = __DATE__;
	const char* time = __TIME__;
	const char* sep = "==============================================================";
	fprintf( stdout, "\n\n%s\n", sep );
#ifdef DEDICATED
	fprintf( stdout, "Linux Quake3 Dedicated Server [%s %s]\n", date, time );
#else
	fprintf( stdout, "Linux Quake3 Full Executable  [%s %s]\n", date, time );
#endif
	fprintf( stdout, " local install: %s\n", name );
	fprintf( stdout, "%s\n\n", sep );
}

void Sys_ParseArgs( int argc, char* argv[] )
{
	if ( argc==2 )
	{
		if ( (!strcmp( argv[1], "--version" ))
		        || ( !strcmp( argv[1], "-v" )) )
		{
			Sys_PrintBinVersion( argv[0] );
			Sys_Exit(0);
		}
	}
}

void  Sys_Print( const char *msg )
{
	fputs( msg, stderr );
}

char *Sys_ConsoleInput( void )
{
	// we use this when sending back commands
	static char text[256];

	int len;
	fd_set fdset;
	struct timeval timeout;

	if ( !stdin_active )
	{
		return NULL;
	}

	FD_ZERO( &fdset );
	FD_SET( 0, &fdset ); // stdin
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;
	if ( select( 1, &fdset, NULL, NULL, &timeout ) == -1 || !FD_ISSET( 0, &fdset ) )
	{
		return NULL;
	}

	len = read( 0, text, sizeof( text ) );
	if ( len == 0 )   // eof!
	{
		stdin_active = qfalse;
		return NULL;
	}

	if ( len < 1 )
	{
		return NULL;
	}
	text[len - 1] = 0; // rip off the \n and terminate

	return text;
}

/*
================
Sys_QueEvent
A time of 0 will get the current time
Ptr should either be null, or point to a block of data that can
be freed by the game later.
================
*/
void Sys_QueEvent( int time, sysEventType_t type, int value, int value2, int ptrLength, void *ptr )
{
	sysEvent_t  *ev;

	ev = &eventQue[ eventHead & MASK_QUED_EVENTS ];

	// bk000305 - was missing
	if ( eventHead - eventTail >= MAX_QUED_EVENTS )
	{
		Com_Printf( "Sys_QueEvent: overflow\n" );
		// we are discarding an event, but don't leak memory
		if ( ev->evPtr )
		{
			Z_Free( ev->evPtr );
		}
		eventTail++;
	}

	eventHead++;

	if ( time == 0 )
	{
		time = Sys_Milliseconds();
	}

	ev->evTime = time;
	ev->evType = type;
	ev->evValue = value;
	ev->evValue2 = value2;
	ev->evPtrLength = ptrLength;
	ev->evPtr = ptr;
}

/*
================
Sys_GetEvent
================
*/
sysEvent_t Sys_GetEvent( void )
{
	sysEvent_t ev;
	char    *s;
	msg_t netmsg;
	netadr_t adr;

	// return if we have data
	if ( eventHead > eventTail )
	{
		eventTail++;
		return eventQue[ ( eventTail - 1 ) & MASK_QUED_EVENTS ];
	}

	// pump the message loop
	// in vga this calls KBD_Update, under X, it calls GetEvent
	//Sys_SendKeyEvents();

	// check for console commands
	s = Sys_ConsoleInput();
	if ( s )
	{
		char  *b;
		int len;

		len = strlen( s ) + 1;
		b = (char *)Z_Malloc( len );
		strcpy( b, s );
		Sys_QueEvent( 0, SE_CONSOLE, 0, 0, len, b );
	}

	// check for other input devices
	//IN_Frame();

	// check for network packets
	MSG_Init( &netmsg, sys_packetReceived, sizeof( sys_packetReceived ) );
	if ( Sys_GetPacket( &adr, &netmsg ) )
	{
		netadr_t    *buf;
		int len;

		// copy out to a seperate buffer for qeueing
		len = sizeof( netadr_t ) + netmsg.cursize;
		buf = (netadr_t *)Z_Malloc( len );
		*buf = adr;
		memcpy( buf + 1, netmsg.data, netmsg.cursize );
		Sys_QueEvent( 0, SE_PACKET, 0, 0, len, buf );
	}

	// return if we have data
	if ( eventHead > eventTail )
	{
		eventTail++;
		return eventQue[ ( eventTail - 1 ) & MASK_QUED_EVENTS ];
	}

	// create an empty event to return

	memset( &ev, 0, sizeof( ev ) );
	ev.evTime = Sys_Milliseconds();

	return ev;
}

void Sys_InitStreamThread( void )
{
}

void Sys_ShutdownStreamThread( void )
{
}

void Sys_BeginStreamedFile( fileHandle_t f, int readAhead )
{
}

void Sys_EndStreamedFile( fileHandle_t f )
{
}

void Sys_Error( const char *error, ... )
{
	va_list argptr;
	char string[1024];

	// change stdin to non blocking
	// NOTE TTimo not sure how well that goes with tty console mode
	fcntl( 0, F_SETFL, fcntl( 0, F_GETFL, 0 ) & ~FNDELAY );

	va_start( argptr,error );
	I_vsnprintf( string, sizeof( string ), error, argptr );
	va_end( argptr );
	fprintf( stderr, "Sys_Error: %s\n", string );

	Sys_Exit( 1 ); // bk010104 - use single exit point.
}

#ifndef DEFAULT_BASEDIR
#	ifdef MACOS_X
#		define DEFAULT_BASEDIR Sys_StripAppBundle(Sys_BinaryPath())
#	else
#		define DEFAULT_BASEDIR Sys_BinaryPath()
#	endif
#endif

int main( int argc, char* argv[] )
{
	int		len, i;
	char	*cmdline;

	// go back to real user for config loads
	saved_euid = geteuid();
	seteuid(getuid());

	Sys_InitMainThread();
	Dvar_Init();
	Sys_ParseArgs( argc, argv );
	Sys_SetDefaultInstallPath( DEFAULT_BASEDIR );

	// merge the command line, this is kinda silly
	for (len = 1, i = 1; i < argc; i++)
		len += strlen(argv[i]) + 1;
	cmdline = (char *)malloc(len);
	*cmdline = 0;
	for (i = 1; i < argc; i++)
	{
		if (i > 1)
			strcat(cmdline, " ");
		strcat(cmdline, argv[i]);
	}

	// bk000306 - clear queues
	memset( &eventQue[0], 0, MAX_QUED_EVENTS*sizeof(sysEvent_t) );
	memset( &sys_packetReceived[0], 0, MAX_MSGLEN*sizeof(byte) );

	Sys_Milliseconds();

	Com_Init(cmdline);

	fcntl(0, F_SETFL, fcntl (0, F_GETFL, 0) | FNDELAY);

	while ( 1 )
	{
		usleep(5000);
		Com_Frame();
	}

	return 1;
}