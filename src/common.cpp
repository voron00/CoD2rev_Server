#include "i_shared.h"
#include "i_common.h"
#include "sys_main.h"
#include "cmd.h"
#include "dvar.h"
#include "filesystem.h"
#include "server.h"
#include "entity.h"
#include "sys_thread.h"

dvar_t *com_dedicated;
dvar_t *com_maxfps;
dvar_t *com_developer;
dvar_t *com_developer_script;
dvar_t *com_logfile;
dvar_t *com_timescale;
dvar_t *com_fixedtime;
dvar_t *com_viewlog;
dvar_t *sv_paused;
dvar_t *cl_paused;
dvar_t *sv_running;
dvar_t *com_introPlayed;
dvar_t *com_animCheck;
dvar_t *com_sv_running;

float com_codeTimeScale;
qboolean com_fullyInitialized = qfalse;

jmp_buf abortframe;     // an ERR_DROP occured, exit the entire frame

#define		MAX_CONSOLE_LINES	32
int			com_numConsoleLines;
char		*com_consoleLines[MAX_CONSOLE_LINES];

#define MIN_DEDICATED_COMHUNKMEGS 1
#define MIN_COMHUNKMEGS 80 // JPW NERVE changed this to 42 for MP, was 56 for team arena and 75 for wolfSP
#define DEF_COMHUNKMEGS 160 // RF, increased this, some maps are exceeding 56mb // JPW NERVE changed this for multiplayer back to 42, 56 for depot/mp_cpdepot, 42 for everything else

#define HUNK_MAGIC  0x89537892
#define HUNK_FREE_MAGIC 0x89537893

typedef struct
{
	int permanent;
	int temp;
} hunkUsed_t;

static byte *s_hunkData = NULL;
static hunkUsed_t hunk_low, hunk_high;
static int s_hunkTotal;

void Com_ErrorMessage(int err, const char* fmt,...)
{
	char buf[MAX_STRING_CHARS];

	va_list		argptr;

	va_start (argptr,fmt);
	I_vsnprintf(buf, sizeof(buf), fmt, argptr);
	va_end (argptr);

	fputs(buf, stdout);
	exit(1);
}

/*
==================
COM_DefaultExtension
==================
*/
void COM_DefaultExtension( char *path, int maxSize, const char *extension )
{
	char oldPath[MAX_QPATH];
	char    *src;

//
// if path doesn't have a .EXT, append extension
// (extension should include the .)
//
	src = path + strlen( path ) - 1;

	while ( *src != '/' && src != path )
	{
		if ( *src == '.' )
		{
			return;                 // it has an extension
		}
		src--;
	}

	I_strncpyz( oldPath, path, sizeof( oldPath ) );
	Com_sprintf( path, maxSize, "%s%s", oldPath, extension );
}

/*
============
Com_StringContains
============
*/
const char *Com_StringContains( const char *str1, const char *str2, int casesensitive)
{
	int len, i, j;

	len = strlen(str1) - strlen(str2);
	for (i = 0; i <= len; i++, str1++)
	{
		for (j = 0; str2[j]; j++)
		{
			if (casesensitive)
			{
				if (str1[j] != str2[j])
				{
					break;
				}
			}
			else
			{
				if (toupper(str1[j]) != toupper(str2[j]))
				{
					break;
				}
			}
		}
		if (!str2[j])
		{
			return str1;
		}
	}
	return NULL;
}

/*
============
Com_Filter
============
*/
int Com_Filter( const char *filter, const char *name, int casesensitive)
{
	char buf[MAX_TOKEN_CHARS];
	const char *ptr;
	int i, found;

	while(*filter)
	{
		if (*filter == '*')
		{
			filter++;
			for (i = 0; *filter; i++)
			{
				if (*filter == '*' || *filter == '?') break;
				buf[i] = *filter;
				filter++;
			}
			buf[i] = '\0';
			if (strlen(buf))
			{
				ptr = Com_StringContains(name, buf, casesensitive);
				if (!ptr) return qfalse;
				name = ptr + strlen(buf);
			}
		}
		else if (*filter == '?')
		{
			filter++;
			name++;
		}
		else if (*filter == '[' && *(filter+1) == '[')
		{
			filter++;
		}
		else if (*filter == '[')
		{
			filter++;
			found = qfalse;
			while(*filter && !found)
			{
				if (*filter == ']' && *(filter+1) != ']') break;
				if (*(filter+1) == '-' && *(filter+2) && (*(filter+2) != ']' || *(filter+3) == ']'))
				{
					if (casesensitive)
					{
						if (*name >= *filter && *name <= *(filter+2)) found = qtrue;
					}
					else
					{
						if (toupper(*name) >= toupper(*filter) &&
						        toupper(*name) <= toupper(*(filter+2))) found = qtrue;
					}
					filter += 3;
				}
				else
				{
					if (casesensitive)
					{
						if (*filter == *name) found = qtrue;
					}
					else
					{
						if (toupper(*filter) == toupper(*name)) found = qtrue;
					}
					filter++;
				}
			}
			if (!found) return qfalse;
			while(*filter)
			{
				if (*filter == ']' && *(filter+1) != ']') break;
				filter++;
			}
			filter++;
			name++;
		}
		else
		{
			if (casesensitive)
			{
				if (*filter != *name) return qfalse;
			}
			else
			{
				if (toupper(*filter) != toupper(*name)) return qfalse;
			}
			filter++;
			name++;
		}
	}
	return qtrue;
}

/*
============
Com_FilterPath
============
*/
int Com_FilterPath(const char *filter, const char *name, int casesensitive)
{
	int i;
	char new_filter[MAX_QPATH];
	char new_name[MAX_QPATH];

	for (i = 0; i < MAX_QPATH-1 && filter[i]; i++)
	{
		if ( filter[i] == '\\' || filter[i] == ':' )
		{
			new_filter[i] = '/';
		}
		else
		{
			new_filter[i] = filter[i];
		}
	}
	new_filter[i] = '\0';
	for (i = 0; i < MAX_QPATH-1 && name[i]; i++)
	{
		if ( name[i] == '\\' || name[i] == ':' )
		{
			new_name[i] = '/';
		}
		else
		{
			new_name[i] = name[i];
		}
	}
	new_name[i] = '\0';
	return Com_Filter(new_filter, new_name, casesensitive);
}

// bk001129 - here we go again: upped from 64
#define MAX_PUSHED_EVENTS               256
// bk001129 - init, also static
static int com_pushedEventsHead = 0;
static int com_pushedEventsTail = 0;
// bk001129 - static
static sysEvent_t com_pushedEvents[MAX_PUSHED_EVENTS];

/*
=================
Com_GetEvent
=================
*/
sysEvent_t  Com_GetEvent( void )
{
	if ( com_pushedEventsHead > com_pushedEventsTail )
	{
		com_pushedEventsTail++;
		return com_pushedEvents[ ( com_pushedEventsTail - 1 ) & ( MAX_PUSHED_EVENTS - 1 ) ];
	}
	return Sys_GetEvent();
}

/*
=================
Com_EventLoop
=================
*/
int Com_EventLoop( void )
{
	sysEvent_t ev;
	//netadr_t evFrom;
	//byte bufData[MAX_MSGLEN];
	//msg_t buf;

	//MSG_Init( &buf, bufData, sizeof( bufData ) );

	while ( 1 )
	{
		ev = Com_GetEvent();

		// if no more events are available
		if ( ev.evType == SE_NONE )
		{
			/*
			while ( NET_GetLoopPacket( NS_SERVER, &evFrom, &buf ) )
			{
				// if the server just shut down, flush the events
				if ( com_sv_running->integer )
				{
					Com_RunAndTimeServerPacket( &evFrom, &buf );
				}
			}
			*/

			return ev.evTime;
		}

		switch ( ev.evType )
		{
		default:
			// bk001129 - was ev.evTime
			Com_Error( ERR_FATAL, "Com_EventLoop: bad event type %i", ev.evType );
			break;
		case SE_NONE:
			break;
		case SE_CONSOLE:
			Cbuf_AddText( (char *)ev.evPtr );
			Cbuf_AddText( "\n" );
			break;
		case SE_PACKET:
			/*
				// this cvar allows simulation of connections that
				// drop a lot of packets.  Note that loopback connections
				// don't go through here at all.
				if ( com_dropsim->value > 0 )
				{
					static int seed;

					if ( Q_random( &seed ) < com_dropsim->value )
					{
						break;      // drop this packet
					}
				}

				evFrom = *(netadr_t *)ev.evPtr;
				buf.cursize = ev.evPtrLength - sizeof( evFrom );

				// we must copy the contents of the message out, because
				// the event buffers are only large enough to hold the
				// exact payload, but channel messages need to be large
				// enough to hold fragment reassembly
				if ( (unsigned)buf.cursize > buf.maxsize )
				{
					Com_Printf( "Com_EventLoop: oversize packet\n" );
					continue;
				}
				memcpy( buf.data, ( byte * )( (netadr_t *)ev.evPtr + 1 ), buf.cursize );
				if ( com_sv_running->integer )
				{
					//Com_RunAndTimeServerPacket( &evFrom, &buf );
				}
				else
				{
					//CL_PacketEvent( evFrom, &buf );
				}
			*/
			break;
		}

		// free any block data
		if ( ev.evPtr )
		{
			Z_Free( ev.evPtr );
		}
	}

	return 0;   // never reached
}

/*
=================
Com_PushEvent
=================
*/
void Com_PushEvent( sysEvent_t *event )
{
	sysEvent_t      *ev;
	static int printedWarning = 0; // bk001129 - init, bk001204 - explicit int

	ev = &com_pushedEvents[ com_pushedEventsHead & ( MAX_PUSHED_EVENTS - 1 ) ];

	if ( com_pushedEventsHead - com_pushedEventsTail >= MAX_PUSHED_EVENTS )
	{

		// don't print the warning constantly, or it can give time for more...
		if ( !printedWarning )
		{
			printedWarning = qtrue;
			Com_Printf( "WARNING: Com_PushEvent overflow\n" );
		}

		if ( ev->evPtr )
		{
			Z_Free( ev->evPtr );
		}
		com_pushedEventsTail++;
	}
	else
	{
		printedWarning = qfalse;
	}

	*ev = *event;
	com_pushedEventsHead++;
}

/*
=================
Com_InitPushEvent
=================
*/
// bk001129 - added
void Com_InitPushEvent( void )
{
	// clear the static buffer array
	// this requires SE_NONE to be accepted as a valid but NOP event
	memset( com_pushedEvents, 0, sizeof( com_pushedEvents ) );
	// reset counters while we are at it
	// beware: GetEvent might still return an SE_NONE from the buffer
	com_pushedEventsHead = 0;
	com_pushedEventsTail = 0;
}

/*
===============
Com_StartupVariable
Searches for command line parameters that are set commands.
If match is not NULL, only that cvar will be looked for.
That is necessary because cddir and basedir need to be set
before the filesystem is started, but all other sets should
be after execing the config and default.
===============
*/
void Com_StartupVariable( const char *match )
{
	int		i;
	for (i=0 ; i < com_numConsoleLines ; i++)
	{
		Cmd_TokenizeString( com_consoleLines[i] );

		if(!match || !strcmp(Cmd_Argv(1), match))
		{
			if ( !strcmp( Cmd_Argv(0), "set" ))
			{
				Dvar_Set_f();
				continue;
			}
			else if( !strcmp( Cmd_Argv(0), "seta" ) )
			{
				Dvar_SetA_f();
			}
		}
	}
}

/*
=================
Com_AddStartupCommands
Adds command line parameters as script statements
Commands are seperated by + signs
Returns qtrue if any late commands were added, which
will keep the demoloop from immediately starting
=================
*/
qboolean Com_AddStartupCommands( void )
{
	int		i;
	qboolean	added;
	char		buf[1024];
	added = qfalse;
	// quote every token, so args with semicolons can work
	for (i=0 ; i < com_numConsoleLines ; i++)
	{
		if ( !com_consoleLines[i] || !com_consoleLines[i][0] )
		{
			continue;
		}

		// set commands already added with Com_StartupVariable
		if ( !I_stricmpn( com_consoleLines[i], "set", 3 ))
		{
			continue;
		}

		added = qtrue;
		Com_sprintf(buf,sizeof(buf),"%s\n",com_consoleLines[i]);
		Cbuf_ExecuteText( EXEC_APPEND, buf);
	}

	return added;
}

/*
==================
Com_ParseCommandLine
Break it up into multiple console lines
==================
*/
void Com_ParseCommandLine( char *commandLine )
{

	com_consoleLines[0] = commandLine;
	com_numConsoleLines = 1;
	char* line;
	int numQuotes, i;

	while ( *commandLine )
	{

		// look for a + seperating character
		// if commandLine came from a file, we might have real line seperators
		if ( (*commandLine == '+') || *commandLine == '\n'  || *commandLine == '\r' )
		{
			if ( com_numConsoleLines == MAX_CONSOLE_LINES )
			{
				return;
			}
			if(*(commandLine +1) != '\n')
			{
				com_consoleLines[com_numConsoleLines] = commandLine + 1;
				com_numConsoleLines = (com_numConsoleLines)+1;
			}
			*commandLine = 0;
		}
		commandLine++;
	}

	for (i = 0; i < com_numConsoleLines; i++)
	{
		line = com_consoleLines[i];
		/* Remove trailling spaces and / or bad quotes */
		while ( (*line == ' ' || *line == '\"') && *line != '\0')
		{
			line++;
		}

		memmove(com_consoleLines[i], line, strlen(line) +1);

		numQuotes = 0;

		/* Now verify quotes */
		while (*line)
		{

			while (*line != '\"' && *line != '\0')
			{
				line ++;
			}
			if(*line == '\"' && *(line -1) != ' ')
				break;

			if(*line == '\"')
				numQuotes++;

			if(*line != '\0')
			{
				line ++;
			}

			while (*line != '\"' && *line != '\0')
			{
				line ++;
			}
			if(*line == '\"' && *(line +1) != ' ' && *(line +1) != '\0'  )
				break;

			if(*line == '\"')
				numQuotes++;

			if(*line != '\0')
			{
				line ++;
			}
		}

		/* if we have bad quotes or an odd number of quotes we replace them all with ' ' */
		if(*line != '\0' || numQuotes & 1)
		{
			line = com_consoleLines[i];
			while (*line != '\0')
			{
				if(*line == '\"')
				{
					*line = ' ';

				}
				line++;
			}
		}

	}
}


/*
===================
Com_SafeMode
Check for "safe" on the command line, which will
skip loading of wolfconfig.cfg
===================
*/
qboolean Com_SafeMode( void )
{
	int i;

	for ( i = 0 ; i < com_numConsoleLines ; i++ )
	{
		Cmd_TokenizeString( com_consoleLines[i] );
		if ( !I_stricmp( Cmd_Argv( 0 ), "safe" ) || !I_stricmp( Cmd_Argv( 0 ), "dvar_restart" ) )
		{
			com_consoleLines[i][0] = 0;
			return qtrue;
		}
	}
	return qfalse;
}

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
		Com_Error( ERR_FATAL, "Hunk data failed to allocate %i megs", s_hunkTotal / ( 1024 * 1024 ) );
	}

	// cacheline align
	s_hunkData = ( byte * )( ( (int)s_hunkData + 31 ) & ~31 );

	Hunk_Clear();

	Cmd_AddCommand( "meminfo", Com_Meminfo_f );
}

void Com_InitDvars()
{
	com_dedicated = Dvar_RegisterInt("dedicated", 2, 0, 2, 4160);
	com_maxfps = Dvar_RegisterInt("com_maxfps", 85, 0, 1000, 4097);
	com_developer = Dvar_RegisterInt("developer", 0, 0, 2, 4096);
	com_developer_script = Dvar_RegisterBool("developer_script", 0, 4096);
	com_logfile = Dvar_RegisterInt("logfile", 0, 0, 2, 4096);
	com_timescale = Dvar_RegisterFloat("timescale", 1.0, 0.0, 1000.0, 4232);
	com_fixedtime = Dvar_RegisterInt("fixedtime", 0, 0, 1000, 4224);
	com_viewlog = Dvar_RegisterInt("viewlog", 0, 0, 2, 4224);
	sv_paused = Dvar_RegisterInt("sv_paused", 0, 0, 2, 4160);
	cl_paused = Dvar_RegisterInt("cl_paused", 0, 0, 2, 4160);
	sv_running = Dvar_RegisterBool("sv_running", 0, 4160);
	com_introPlayed = Dvar_RegisterBool("com_introPlayed", 0, 4097);
	com_animCheck = Dvar_RegisterBool("com_animCheck", 0, 4096);
}

/*
=================
Com_Frame
=================
*/
void Com_Frame( void )
{
	Cbuf_Execute();
	Com_EventLoop();
}

/*
=================
Com_Init
The games main initialization
=================
*/
void Com_Init(char* commandLine)
{
	Com_Printf("%s %s build %s %s\n", GAME_STRING,PRODUCT_VERSION,PLATFORM_STRING, __DATE__);

	if (sizeof(client_t) != 0x78F14)
	{
		Com_Error( ERR_FATAL, "Error: client_t struct has invalid size %d\n", sizeof(client_t) );
	}

	if (sizeof(gentity_t) != 560)
	{
		Com_Error( ERR_FATAL, "Error: gentity_t struct has invalid size %d\n", sizeof(gentity_t) );
	}

	if (sizeof(gclient_t) != 0x28A4)
	{
		Com_Error( ERR_FATAL, "Error: gclient_t struct has invalid size %d\n", sizeof(gclient_t) );
	}

	// bk001129 - do this before anything else decides to push events
	Com_InitPushEvent();

	// prepare enough of the subsystems to handle
	// cvar and command buffer management
	Com_ParseCommandLine( commandLine );

	Swap_Init();
	Cbuf_Init();

	//Com_InitZoneMemory(); // Not present in CoD2
	Cmd_Init();

	// override anything from the config files with command line args
	Com_StartupVariable( NULL );

	// get the developer cvar set as early as possible
	Com_StartupVariable( "developer" );

	FS_InitFilesystem();
	Com_InitDvars();

	Cbuf_Execute();

	// allocate the stack based hunk allocator
	Com_InitHunkMemory();

	CM_InitThreadData(THREAD_CONTEXT_MAIN);

	dvar_modifiedFlags &= ~DVAR_ARCHIVE;
	com_codeTimeScale = 1.0;

	com_fullyInitialized = qtrue;

	Com_Printf( "--- Common Initialization Complete ---\n" );
}