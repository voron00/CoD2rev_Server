#include "qcommon.h"
#include "cmd.h"
#include "sys_thread.h"

// common.c -- misc functions used in client and server

#include "../script/script_public.h"
#include "../stringed/stringed_public.h"
#include "../universal/universal_public.h"
#include "../xanim/xanim_public.h"

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
dvar_t *com_introPlayed;
dvar_t *com_animCheck;
dvar_t *com_sv_running;

dvar_t *ui_errorMessage;
dvar_t *ui_errorTitle;

static fileHandle_t logfile;

int com_frameTime;
int com_lastFrameTime;

qboolean com_errorEntered;
qboolean com_fullyInitialized = qfalse;

errorParm_t com_errorType;
char com_errorMessage[MAXPRINTMSG];

static char		*rd_buffer;
static size_t	rd_buffersize;
static void		(*rd_flush)( char *buffer );

int com_fixedConsolePosition;
int iWeaponInfoSource;

qboolean com_safemode = qfalse;
int com_frameNumber;
float com_timescaleValue;

/*
==================
Com_HashKey
==================
*/
int Com_HashKey( const char *string, int maxlen )
{
	int hash, i;

	hash = 0;

	for ( i = 0; i < maxlen && string[i] != '\0'; i++ )
	{
		hash += string[i] * ( 119 + i );
	}

	hash = ( hash ^ ( hash >> 10 ) ^ ( hash >> 20 ) );
	return hash;
}

/*
==================
Com_GetDecimalDelimiter
==================
*/
char Com_GetDecimalDelimiter()
{
	int lang = loc_language->current.integer;

	if ( lang == LANGUAGE_FRENCH || lang == LANGUAGE_GERMAN || lang == LANGUAGE_ITALIAN || lang == LANGUAGE_SPANISH || lang == LANGUAGE_RUSSIAN || lang == LANGUAGE_POLISH )
	{
		return ',';
	}

	return '.';
}

/*
==============
Com_AddToString
==============
*/
int Com_AddToString( const char *add, char *msg, int len, int maxlen, int mayAddQuotes )
{
	int addQuotes;
	int i;

	addQuotes = 0;
	if (mayAddQuotes)
	{
		if (*add)
		{
			for (i = 0; i < maxlen - len && add[i]; ++i)
			{
				if (add[i] <= 32)
				{
					addQuotes = qtrue;
					break;
				}
			}
		}
		else
		{
			addQuotes = qtrue;
		}
	}
	if (addQuotes && len < maxlen)
	{
		msg[len++] = '"';
	}
	for (i = 0; len < maxlen && add[i]; ++i)
	{
		msg[len++] = add[i];
	}
	if (addQuotes && len < maxlen)
	{
		msg[len++] = '"';
	}
	return len;
}

/*
==============
Com_SetWeaponInfoMemory
==============
*/
void Com_SetWeaponInfoMemory( int iSource )
{
	iWeaponInfoSource = iSource;
}

/*
==============
Com_CheckSyncFrame
==============
*/
void Com_CheckSyncFrame( void )
{
	UNIMPLEMENTED(__FUNCTION__);
}

/*
==============
Com_GetTimescaleForSnd
==============
*/
float Com_GetTimescaleForSnd()
{
	if ( com_fixedtime->current.integer )
		return (float)com_fixedtime->current.integer;

	return com_timescale->current.decimal;
}

/*
==============
Com_SetScriptSettings
==============
*/
void Com_SetScriptSettings()
{
	Scr_Settings(com_developer->current.integer || com_logfile->current.integer, com_developer_script->current.boolean, com_developer->current.integer);
}

/*
==============
Com_ShutdownEvents
==============
*/
void Com_ShutdownEvents()
{
	UNIMPLEMENTED(__FUNCTION__);
}

// bk001129 - here we go again: upped from 64
#define MAX_PUSHED_EVENTS               256
// bk001129 - init, also static
static int com_pushedEventsHead = 0;
static int com_pushedEventsTail = 0;
// bk001129 - static
static sysEvent_t com_pushedEvents[MAX_PUSHED_EVENTS];

/*
==============
Com_PushEvent
==============
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
=================
Com_ForceSafeMode
=================
*/
void Com_ForceSafeMode()
{
	com_safemode = qtrue;
}

/*
============================================================================

COMMAND LINE FUNCTIONS

+ characters seperate the commandLine string into multiple console
command lines.

All of these are valid:

quake3 +set test blah +map test
quake3 set test blah+map test
quake3 set test blah + map test

============================================================================
*/

#define MAX_CONSOLE_LINES   32
int com_numConsoleLines;
char    *com_consoleLines[MAX_CONSOLE_LINES];

/*
==================
Com_ParseCommandLine

Break it up into multiple console lines
==================
*/
void Com_ParseCommandLine( char *commandLine )
{
	int inq = 0;
	com_consoleLines[0] = commandLine;
	com_numConsoleLines = 1;

	while ( *commandLine )
	{
		if ( *commandLine == '"' )
		{
			inq = !inq;
		}
		// look for a + seperating character
		// if commandLine came from a file, we might have real line seperators
		if ( *commandLine == '+' || *commandLine == '\n' )
		{
			if ( com_numConsoleLines == MAX_CONSOLE_LINES )
			{
				return;
			}
			com_consoleLines[com_numConsoleLines] = commandLine + 1;
			com_numConsoleLines++;
			*commandLine = 0;
		}
		commandLine++;
	}
}

float com_codeTimeScale;

/*
==================
Com_SetTimeScale
==================
*/
void Com_SetTimeScale( float timescale )
{
	com_codeTimeScale = timescale;
}

/*
================
Com_DPrintf

A Com_Printf that only shows up if the "developer" cvar is set
================
*/
void Com_DPrintf( const char *fmt, ... )
{
	va_list		argptr;
	char		msg[MAXPRINTMSG];

	if ( !com_developer || !com_developer->current.integer )
	{
		return;			// don't confuse non-developers with techie stuff...
	}

	va_start( argptr, fmt );
	Q_vsnprintf( msg, sizeof(msg), fmt, argptr );
	va_end( argptr );

	msg[MAXPRINTMSG - 1] = 0;

	Com_Printf( "%s", msg );
}

/*
================
Com_LocalizedFloatToString
================
*/
void Com_LocalizedFloatToString( float f, char *buffer, unsigned int maxlen, unsigned int numDecimalPlaces )
{
	unsigned int charPos;
	char delimiter;

	snprintf(buffer, maxlen - 1, "%.*f", numDecimalPlaces, f);
	buffer[maxlen - 1] = 0;

	delimiter = Com_GetDecimalDelimiter();

	if ( delimiter == '.' )
	{
		return;
	}

	for ( charPos = 0; charPos < maxlen; charPos++ )
	{
		if ( buffer[charPos] == '.' )
		{
			buffer[charPos] = delimiter;
			return;
		}
	}
}

/*
================
Com_CloseLogfiles
================
*/
void Com_CloseLogfiles()
{
	if ( logfile )
	{
		FS_FCloseFile(logfile);
		logfile = 0;
	}
}

/*
================
Com_ResetFrametime
================
*/
void Com_ResetFrametime()
{
	com_lastFrameTime = Sys_MilliSeconds();
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
	int i;
	qboolean added;

	added = qfalse;
	// quote every token, so args with semicolons can work
	for ( i = 0 ; i < com_numConsoleLines ; i++ )
	{
		if ( !com_consoleLines[i] || !com_consoleLines[i][0] )
		{
			continue;
		}

		// set commands won't override menu startup
		if ( I_strnicmp( com_consoleLines[i], "set", 3 ) )
		{
			added = qtrue;
		}
		Cbuf_AddText( com_consoleLines[i] );
		Cbuf_AddText( "\n" );
	}

	return added;
}

/*
================
Com_FreeWeaponInfoMemory
================
*/
void Com_FreeWeaponInfoMemory( int iSource )
{
	if ( iSource == iWeaponInfoSource )
	{
		iWeaponInfoSource = 0;
		BG_ShutdownWeaponDefFiles();
	}
}

/*
================
Com_PumpMessageLoop
================
*/
void Com_PumpMessageLoop()
{
	sysEvent_t ev;

	while ( 1 )
	{
		ev = Sys_GetEvent();
		if ( ev.evType == SE_NONE )
		{
			break;
		}
		Com_PushEvent(&ev);
	}
}

/*
================
Com_Milliseconds
================
*/
int Com_Milliseconds( void )
{
	sysEvent_t	ev;

	// get events and push them until we get a null event with the current time
	do
	{
		ev = Sys_GetEvent();
		if ( ev.evType != SE_NONE )
		{
			Com_PushEvent( &ev );
		}
	}
	while ( ev.evType != SE_NONE );

	return ev.evTime;
}

/*
================
Com_GetEvent
================
*/
sysEvent_t Com_GetEvent( void )
{
	if ( com_pushedEventsHead > com_pushedEventsTail )
	{
		com_pushedEventsTail++;
		return com_pushedEvents[ ( com_pushedEventsTail - 1 ) & ( MAX_PUSHED_EVENTS - 1 ) ];
	}
	return Sys_GetEvent();
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
	return com_safemode;
}

/*
===================
MT_AllocAnimTree
===================
*/
void* MT_AllocAnimTree( int numBytes )
{
	return MT_Alloc(numBytes);
}

/*
===================
Com_XAnimCreateSmallTree
===================
*/
XAnimTree* Com_XAnimCreateSmallTree( XAnim *anims )
{
	return XAnimCreateTree(anims, MT_AllocAnimTree);
}

/*
===================
Com_XAnimFreeSmallTree
===================
*/
void Com_XAnimFreeSmallTree( XAnimTree *animtree )
{
	XAnimFreeTree(animtree, MT_Free);
}

/*
===================
Com_RunAutoExec
===================
*/
void Com_RunAutoExec()
{
	Dvar_SetInAutoExec(qtrue);
	Cbuf_Execute();
	Dvar_SetInAutoExec(qfalse);
}

/*
===================
Com_ExecStartupConfigs
===================
*/
void Com_ExecStartupConfigs( const char *configFile )
{
	Cbuf_AddText("exec default_mp.cfg\n");
	Cbuf_AddText("exec language.cfg\n");

	if ( configFile )
	{
		Cbuf_AddText(va("exec %s\n", configFile));
	}

	Cbuf_Execute();

	Com_RunAutoExec();

	if ( Com_SafeMode() )
	{
		Cbuf_AddText("exec safemode_mp_server.cfg\n");
	}

	Cbuf_Execute();
}

/*
===================
Debug_EventLoop
===================
*/
int Debug_EventLoop()
{
	sysEvent_t	ev;

	while ( 1 )
	{
		ev = Com_GetEvent();

		switch ( ev.evType )
		{
		default:
			// bk001129 - was ev.evTime
			Com_Error( ERR_FATAL, "Com_EventLoop: bad event type %i", ev.evType );
			break;
		case SE_NONE:
		case SE_KEY:
		case SE_CHAR:
		case SE_MOUSE:
			break;
		case SE_JOYSTICK_AXIS:
			Cbuf_AddText((char*)ev.evPtr);
			Z_Free(ev.evPtr);
			Cbuf_AddText("\n");
			break;
		case SE_CONSOLE:
			break;
		case SE_PACKET:
			Z_Free( ev.evPtr );
			break;
		}
	}

	return 0;	// never reached
}

/*
===================
Com_ReadCDKey
===================
*/
void Com_ReadCDKey()
{
	UNIMPLEMENTED(__FUNCTION__);
}

/*
===================
Com_PrintMessage
===================
*/
void Com_PrintMessage( conChannel_t channel, const char *msg )
{
	va_list		argptr;
	static qboolean opening_qconsole = qfalse;

	if ( rd_buffer )
	{
		if (channel != CON_CHANNEL_LOGFILEONLY)
		{
			if ((strlen (msg) + strlen(rd_buffer)) > (rd_buffersize - 1))
			{
				rd_flush(rd_buffer);
				*rd_buffer = 0;
			}
			I_strncat(rd_buffer, rd_buffersize, msg);
		}
		// TTimo nooo .. that would defeat the purpose
		//rd_flush(rd_buffer);
		//*rd_buffer = 0;
		return;
	}

#ifndef DEDICATED
	// echo to console if we're not a dedicated server
	if ( com_dedicated && !com_dedicated->current.integer )
	{
		CL_ConsolePrint( msg );
	}
#endif

	if ( msg[0] == '^' && msg[1] )
		msg += 2;

	if ( channel != CON_CHANNEL_LOGFILEONLY )
	{
		// echo to dedicated console and early console
		Sys_Print( msg );
	}

	// logfile
	if ( com_logfile && com_logfile->current.integer && FS_Initialized() )
	{
		// TTimo: only open the qconsole.log if the filesystem is in an initialized state
		//   also, avoid recursing in the qconsole.log opening (i.e. if fs_debug is on)
		if ( !logfile && !opening_qconsole)
		{
			struct tm *newtime;
			time_t aclock;

			opening_qconsole = qtrue;

			time( &aclock );
			newtime = localtime( &aclock );

#ifdef DEDICATED
			logfile = FS_FOpenTextFileWrite( "console_mp_server.log" );
#else
			logfile = FS_FOpenTextFileWrite( "console_mp.log" );
#endif
			Com_Printf( "logfile opened on %s\n", asctime( newtime ) );

			opening_qconsole = qfalse;
		}

		if ( logfile )
		{
			FS_Write(msg, strlen(msg), logfile);

			if ( com_logfile->current.integer > 1 )
				FS_Flush(logfile);
		}
	}
}

/*
=============
Com_Printf

Both client and server can use this, and it will output
to the apropriate place.

A raw string should NEVER be passed as fmt, because of "%f" type crashers.
=============
*/
void Com_Printf( const char *fmt, ... )
{
	va_list		argptr;
	char		msg[MAXPRINTMSG];

	va_start (argptr,fmt);
	Q_vsnprintf (msg, sizeof(msg), fmt, argptr);
	va_end (argptr);

	msg[MAXPRINTMSG - 1] = 0;
	Com_PrintMessage(CON_CHANNEL_DONT_FILTER, msg);
}

/*
=============
Com_WriteDefaultsToFile
=============
*/
void Com_WriteDefaultsToFile( const char *filename )
{
	int f = FS_FOpenFileWrite(filename);

	if ( !f )
	{
		Com_Printf("Couldn't write %s.\n", filename);
		return;
	}

	FS_Printf(f, "// generated by Call of Duty, do not modify\n");
	Dvar_WriteDefaults(f);
	FS_FCloseFile(f);
}

/*
=============
Com_WriteCDKey
=============
*/
void Com_WriteCDKey()
{
	UNIMPLEMENTED(__FUNCTION__);
}

/*
=============
Info_Print
=============
*/
void Info_Print( const char *s )
{
	char key[512];
	char value[512];
	char    *o;
	int l;

	if ( *s == '\\' )
	{
		s++;
	}
	while ( *s )
	{
		o = key;
		while ( *s && *s != '\\' )
			*o++ = *s++;

		l = o - key;
		if ( l < 20 )
		{
			memset( o, ' ', 20 - l );
			key[20] = 0;
		}
		else
		{
			*o = 0;
		}
		Com_Printf( "%s", key );

		if ( !*s )
		{
			Com_Printf( "MISSING VALUE\n" );
			return;
		}

		o = value;
		s++;
		while ( *s && *s != '\\' )
			*o++ = *s++;
		*o = 0;

		if ( *s )
		{
			s++;
		}
		Com_Printf( "%s\n", value );
	}
}

/*
=============
Com_WriteDefaults_f
=============
*/
static void Com_WriteDefaults_f( void )
{
	char filename[MAX_QPATH];

	if ( Cmd_Argc() != 2 )
	{
		Com_Printf("Usage: writedefaults <filename>\n");
		return;
	}

	I_strncpyz(filename, Cmd_Argv(1), sizeof(filename));

	Com_DefaultExtension(filename, sizeof(filename), ".cfg");
	Com_Printf("Writing %s.\n", filename);
	Com_WriteDefaultsToFile(filename);
}

/*
=============
Com_WriteConfig_f
=============
*/
static void Com_WriteConfig_f( void )
{
	char filename[MAX_QPATH];

	if ( Cmd_Argc() != 2 )
	{
		Com_Printf("Usage: writeconfig <filename>\n");
		return;
	}

	I_strncpyz(filename, Cmd_Argv(1), sizeof(filename));

	Com_DefaultExtension(filename, sizeof(filename), ".cfg");
	Com_Printf("Writing %s.\n", filename);
	Com_WriteConfigToFile(filename);
}

/*
===============
Com_WriteConfiguration

Writes key bindings and archived cvars to config file if modified
===============
*/
void Com_WriteConfiguration( void )
{
	char filename[MAX_QPATH];

	// if we are quiting without fully initializing, make sure
	// we don't write out anything
	if ( !com_fullyInitialized )
		return;

	if ( !(dvar_modifiedFlags & DVAR_ARCHIVE ) )
		return;

	dvar_modifiedFlags &= ~DVAR_ARCHIVE;
	I_strncpyz(filename, CONFIG_NAME, sizeof(filename));
	Com_WriteConfigToFile( filename );
}

/*
===============
Com_StartupVariable

Searches for command line parameters that are set commands.
If match is not NULL, only that cvar will be looked for.
That is necessary because cddir and basedir need to be set
before the filesystem is started, but all other sets shouls
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
			if ( !strcasecmp( Cmd_Argv(0), "set" ))
			{
				Dvar_Set_f();
				continue;
			}
			else if( !strcasecmp( Cmd_Argv(0), "seta" ) )
			{
				Dvar_SetA_f();
			}
		}
	}
}

/*
===============
Com_Close
===============
*/
void Com_Close()
{
	Com_ShutdownDObj();
	DObjShutdown();
	XAnimShutdown();
	CM_Shutdown();
#ifndef DEDICATED
	SND_ShutdownChannels();
#endif
	Hunk_Clear();
	Scr_Shutdown();
}

/*
===============
Com_SyncThreads
===============
*/
void Com_SyncThreads( void )
{
	UNIMPLEMENTED(__FUNCTION__);
}

/*
===============
Com_Restart
===============
*/
void Com_Restart()
{
	SV_ShutdownGameProgs();
	Com_ShutdownDObj();
	DObjShutdown();
	XAnimShutdown();
	CM_Shutdown();
#ifndef DEDICATED
	SND_ShutdownChannels();
#endif
	Hunk_Clear();
	Scr_Init();
	Com_SetScriptSettings();
	com_fixedConsolePosition = 0;
	XAnimInit();
	DObjInit();
	Com_InitDObj();
}

/*
===============
Com_AssetLoadUI
===============
*/
void Com_AssetLoadUI( void )
{
	UNIMPLEMENTED(__FUNCTION__);
}

/*
=============
Com_Quit_f

Both client and server can use this, and it will
do the apropriate things.
=============
*/
void Com_Quit_f()
{
	Com_Printf("quitting...\n");

	// don't try to shutdown if we are in a recursive error
	if ( !com_errorEntered )
	{
		Com_ClearTempMemory();
		SV_Shutdown("EXE_SERVERQUIT");
		Com_Close();
		Com_CloseLogfiles();
		FS_Shutdown();
		FS_ShutdownServerIwdNames();
		FS_ShutdownServerReferencedIwds();
	}

	Sys_Quit();
}

/*
=============
Com_ShutdownInternal
=============
*/
static void Com_ShutdownInternal( const char *finalmsg )
{
#ifndef DEDICATED
	CL_Disconnect();
	CL_ShutdownAll();
	CL_ShutdownDemo();
#endif
	SV_Shutdown(finalmsg);
	Com_Restart();
}

/*
=================
Com_Shutdown
=================
*/
void Com_Shutdown( const char *finalmsg )
{
	Com_ShutdownInternal(finalmsg);
}

/*
=================
Com_EventLoop

Returns last event time
=================
*/
int Com_EventLoop( void )
{
	sysEvent_t	ev;
	netadr_t	evFrom;
	byte		bufData[MAX_MSGLEN];
	msg_t		buf;

	MSG_Init( &buf, bufData, sizeof( bufData ) );

	while ( 1 )
	{
		ev = Com_GetEvent();

		// if no more events are available
		if ( ev.evType == SE_NONE )
		{
			while ( NET_GetLoopPacket( NS_SERVER, &evFrom, &buf ) )
			{
				// if the server just shut down, flush the events
				if ( com_sv_running->current.boolean )
				{
					SV_PacketEvent( evFrom, &buf );
				}
			}

			return ev.evTime;
		}

		switch ( ev.evType )
		{
		default:
			// bk001129 - was ev.evTime
			Com_Error( ERR_FATAL, "Com_EventLoop: bad event type %i", ev.evType );
			break;
		case SE_NONE:
		case SE_KEY:
		case SE_CHAR:
		case SE_MOUSE:
		case SE_JOYSTICK_AXIS:
			break;
		case SE_CONSOLE:
			Cbuf_AddText( (char *)ev.evPtr );
			Z_Free( ev.evPtr );
			Cbuf_AddText( "\n" );
			break;
		case SE_PACKET:
			evFrom = *(netadr_t *)ev.evPtr;
			buf.cursize = ev.evPtrLength - sizeof( evFrom );

			// we must copy the contents of the message out, because
			// the event buffers are only large enough to hold the
			// exact payload, but channel messages need to be large
			// enough to hold fragment reassembly
			if ( buf.cursize > buf.maxsize )
			{
				Z_Free( ev.evPtr );
				Com_Printf("Com_EventLoop: oversize packet\n");
				continue;
			}
			Com_Memcpy( buf.data, (byte *)((netadr_t *)ev.evPtr + 1), buf.cursize );
			Z_Free( ev.evPtr );
			if ( com_sv_running->current.boolean )
			{
				SV_PacketEvent( evFrom, &buf );
			}
			break;
		}
	}

	return 0;	// never reached
}

/*
=============
Com_Error_f

Just throw a fatal error to
test error shutdown procedures
=============
*/
static void Com_Error_f( void )
{
	if ( Cmd_Argc() > 1 )
	{
		Com_Error( ERR_DROP, "Testing drop error" );
	}
	else
	{
		Com_Error( ERR_FATAL, "Testing fatal error" );
	}
}

/*
=============
Com_Freeze_f

Just freeze in place for a given number of seconds to test
error recovery
=============
*/
static void Com_Freeze_f( void )
{
	float	s;
	int		start, now;

	if ( Cmd_Argc() != 2 )
	{
		Com_Printf( "freeze <seconds>\n" );
		return;
	}

	s = atof( Cmd_Argv(1) );

	start = Com_Milliseconds();

	while ( 1 )
	{
		now = Com_Milliseconds();
		if ( ( now - start ) * 0.001 > s )
		{
			break;
		}
	}
}

/*
=================
Com_Crash_f

A way to force a bus error for development reasons
=================
*/
static void Com_Crash_f( void )
{
	* ( int * ) 0 = 0x12345678;
}

/*
=================
Com_Frame_Try_Block_Function
=================
*/
static void Com_Frame_Try_Block_Function()
{
	int msec, minMsec;

	Com_WriteConfiguration();
	Com_DedicatedModified();
	SetAnimCheck(com_animCheck->current.boolean);

	// we may want to spin here if things are going too fast
	minMsec = 1;
	if ( com_maxfps->current.integer > 0 && !com_dedicated->current.integer )
	{
		minMsec = 1000 / com_maxfps->current.integer;
		if ( !minMsec )
			minMsec = 1;
	}

	while (1)
	{
		com_frameTime = Com_EventLoop();

		if ( com_lastFrameTime > com_frameTime )
			com_lastFrameTime = com_frameTime;

		msec = com_frameTime - com_lastFrameTime;

		if ( msec >= minMsec )
			break;

		NET_Sleep(0);
	}

	Cbuf_Execute();
	com_lastFrameTime = com_frameTime;
	msec = Com_ModifyMsec( msec );
	SV_Frame( msec );
}

/*
=================
Com_Frame
=================
*/
void Com_Frame( void )
{
	jmp_buf* abortframe = (jmp_buf*)Sys_GetValue(THREAD_VALUE_COM_ERROR);

	if (!setjmp(*abortframe))
	{
		Com_Frame_Try_Block_Function();
		com_frameNumber++;
	}

	if ( com_errorEntered )
	{
		Com_ErrorCleanup();
	}
}

/*
=================
Com_Init_Try_Block_Function
=================
*/
static void Com_Init_Try_Block_Function( char *commandLine )
{
	Com_Printf("%s %s build %s %s\n", GAME_STRING,PRODUCT_VERSION,CPUSTRING, __DATE__);
	Com_InitPushEvent();
	Com_ParseCommandLine(commandLine);
	Swap_Init();
	Cbuf_Init();
	Cmd_Init();
	Com_StartupVariable(0);
	Com_StartupVariable("developer");
	FS_InitFilesystem();
	Com_InitDvars();
	Com_StartupConfigs();
	Cbuf_Execute();
	Com_StartupVariable(0);
	Com_InitHunkMemory();
	dvar_modifiedFlags &= ~DVAR_ARCHIVE;
	com_codeTimeScale = 1.0;
	if ( com_developer->current.integer )
	{
		Cmd_AddCommand("error", Com_Error_f);
		Cmd_AddCommand("crash", Com_Crash_f);
		Cmd_AddCommand("freeze", Com_Freeze_f);
	}
	Cmd_AddCommand("quit", Com_Quit_f);
	Cmd_AddCommand("writeconfig", Com_WriteConfig_f);
	Cmd_AddCommand("writedefaults", Com_WriteDefaults_f);
	Dvar_RegisterString("version", va("%s %s build %s %s", GAME_STRING,PRODUCT_VERSION,CPUSTRING, __DATE__), DVAR_ROM | DVAR_CHANGEABLE_RESET);
	Dvar_RegisterString("shortversion", PRODUCT_VERSION, DVAR_SERVERINFO | DVAR_ROM | DVAR_CHANGEABLE_RESET);
#ifndef DEDICATED
	FxMem_Init();
#endif
	Sys_Init();
	Netchan_Init();
	Scr_Init();
	Com_SetScriptSettings();
	XAnimInit();
	DObjInit();
	SV_Init();
	NET_Init();
	com_frameTime  = Com_Milliseconds();
	Com_AddStartupCommands();
	COM_PlayIntroMovies();
	com_fullyInitialized = 1;
	Com_Printf("--- Common Initialization Complete ---\n");
	Cbuf_Execute();
}

/*
=================
Com_Init
=================
*/
void Com_Init( char* commandLine )
{
	jmp_buf* abortframe = (jmp_buf*)Sys_GetValue(THREAD_VALUE_COM_ERROR);

	if (setjmp(*abortframe))
		Sys_Error(va("Error during Initialization:\n%s\n", com_errorMessage));

	Com_Init_Try_Block_Function(commandLine);
}

/*
=================
Com_ClearCDKey
=================
*/
void Com_ClearCDKey( void )
{
	UNIMPLEMENTED(__FUNCTION__);
}

/*
=================
Com_ClearGamePadCheat
=================
*/
void Com_ClearGamePadCheat( struct GamePadCheat * )
{
	UNIMPLEMENTED(__FUNCTION__);
}

/*
=================
Com_ClearTempMemory
=================
*/
void Com_ClearTempMemory()
{
	Hunk_ClearTempMemoryInternal();
	Hunk_ClearTempMemoryHighInternal();
}

/*
==================
COM_PlayIntroMovies
==================
*/
void COM_PlayIntroMovies()
{
	if ( com_dedicated->current.integer )
		return;

	if ( com_introPlayed->current.boolean )
		return;

	Cbuf_AddText("cinematic atvi\n");
	Dvar_SetString(nextmap, "cinematic IW_logo; set nextmap cinematic cod_intro");
	Dvar_SetBool(com_introPlayed, true);
}

/*
=================
Com_UpdateGamePadCheat
=================
*/
void Com_UpdateGamePadCheat( struct GamePadCheat * )
{
	UNIMPLEMENTED(__FUNCTION__);
}

/*
=================
Com_UpdateAllGamePadCheats
=================
*/
void Com_UpdateAllGamePadCheats( void )
{
	UNIMPLEMENTED(__FUNCTION__);
}

/*
=================
Com_SetErrorMessage
=================
*/
void Com_SetErrorMessage( const char *errorMessage )
{
	ui_errorMessage = Dvar_RegisterString("com_errorMessage", "", DVAR_ROM | DVAR_CHANGEABLE_RESET);
	ui_errorTitle = Dvar_RegisterString("com_errorTitle", "", DVAR_ROM | DVAR_CHANGEABLE_RESET);

	Dvar_SetString(ui_errorMessage, errorMessage);
}

/*
==================
Com_StartupConfigs
==================
*/
void Com_StartupConfigs()
{
	Com_ExecStartupConfigs(CONFIG_NAME);
}

/*
=================
Com_CheckStartupMessage
=================
*/
void Com_CheckStartupMessage( void )
{
	UNIMPLEMENTED(__FUNCTION__);
}

/*
=================
Com_Statmon
=================
*/
void Com_Statmon( void )
{
	UNIMPLEMENTED(__FUNCTION__);
}

/*
=================
Com_InitDvars
=================
*/
void Com_InitDvars()
{
	com_dedicated = Dvar_RegisterInt("dedicated", 2, 0, 2, DVAR_ROM | DVAR_CHANGEABLE_RESET);
	com_maxfps = Dvar_RegisterInt("com_maxfps", 85, 0, 1000, DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);
	com_developer = Dvar_RegisterInt("developer", 0, 0, 2, DVAR_CHANGEABLE_RESET);
	com_developer_script = Dvar_RegisterBool("developer_script", false, DVAR_CHANGEABLE_RESET);
	com_logfile = Dvar_RegisterInt("logfile", 0, 0, 2, DVAR_CHANGEABLE_RESET);
	com_timescale = Dvar_RegisterFloat("timescale", 1.0, 0.001, 1000.0, DVAR_SYSTEMINFO | DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	com_fixedtime = Dvar_RegisterInt("fixedtime", 0, 0, 1000, DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	com_viewlog = Dvar_RegisterInt("viewlog", 0, 0, 2, DVAR_CHEAT | DVAR_CHANGEABLE_RESET);
	sv_paused = Dvar_RegisterInt("sv_paused", 0, 0, 2, DVAR_ROM | DVAR_CHANGEABLE_RESET);
	cl_paused = Dvar_RegisterInt("cl_paused", 0, 0, 2, DVAR_ROM | DVAR_CHANGEABLE_RESET);
	com_sv_running = Dvar_RegisterBool("sv_running", false, DVAR_ROM | DVAR_CHANGEABLE_RESET);
	com_introPlayed = Dvar_RegisterBool("com_introPlayed", false, DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);
	com_animCheck = Dvar_RegisterBool("com_animCheck", false, DVAR_CHANGEABLE_RESET);

	if ( com_dedicated->current.integer )
	{
		if ( !com_viewlog->current.integer )
			Dvar_SetInt(com_viewlog, 1);
	}
}

/*
================
Com_ModifyMsec
================
*/
int Com_ModifyMsec( int msec )
{
	int originalMsec;
	bool useTimescale;
	int clampTime;

	originalMsec = msec;

	//
	// modify time for debugging values
	//
	if ( com_fixedtime->current.integer )
	{
		msec = com_fixedtime->current.integer;
		useTimescale = true;
	}
	else if ( com_timescale->current.decimal == 1.0 && com_codeTimeScale == 1.0 )
	{
		useTimescale = false;
	}
	else
	{
		msec = Q_rint( msec * com_timescale->current.decimal * com_codeTimeScale );
		useTimescale = true;
	}

	// don't let it scale below 1 msec
	if ( msec < 1 )
		msec = 1;

	if ( com_dedicated->current.integer )
	{
		// dedicated servers don't want to clamp for a much longer
		// period, because it would mess up all the client's views
		// of time.
		if ( msec > 500 && msec < 500000 )
		{
			Com_Printf("Hitch warning: %i msec frame time\n", msec);
		}
		clampTime = 5000;
	}
	else if ( com_sv_running->current.boolean )
	{
		// clients of remote servers do not want to clamp time, because
		// it would skew their view of the server's time temporarily
		clampTime = 200;
	}
	else
	{
		// for local single player gaming
		// we may want to clamp the time to prevent players from
		// flying off edges when something hitches.
		clampTime = 5000;
	}

	if ( msec > clampTime )
		msec = clampTime;

	if ( useTimescale && originalMsec )
	{
		com_timescaleValue = (float)msec / (float)originalMsec;
	}
	else
	{
		com_timescaleValue = 1.0;
	}

	return msec;
}

/*
================
Com_WriteConfigToFile
================
*/
void Com_WriteConfigToFile( const char *filename )
{
	fileHandle_t f = FS_FOpenFileWrite(filename);

	if ( !f )
	{
		Com_Printf("Couldn't write %s.\n", filename);
		return;
	}

	FS_Printf(f, "// generated by Call of Duty, do not modify\n");
	Dvar_WriteVariables(f);
	FS_FCloseFile(f);
}

/*
================
Com_AttractMode
================
*/
void Com_AttractMode( void )
{
	UNIMPLEMENTED(__FUNCTION__);
}

/*
==================
Com_DedicatedModified
==================
*/
void Com_DedicatedModified()
{
	if ( !com_viewlog->modified )
		return;

	if ( !com_dedicated->current.integer )
		Sys_ShowConsole( com_viewlog->current.integer, qfalse );

	Dvar_ClearModified( com_viewlog );
}

/*
================
Com_ErrorCleanup
================
*/
void Com_ErrorCleanup( void )
{
	static int	lastErrorTime;
	static int	errorCount;
	int			currentTime;
	char msgBuf[MAXPRINTMSG];

	Dvar_SetInAutoExec(0);
	Com_ClearTempMemory();
	Hunk_ClearTempMemoryHigh();
	Dvar_SetIntByName("cl_paused", 0);
	FS_PureServerSetLoadedIwds("", "");

	strcpy(msgBuf, com_errorMessage);

	if ( com_errorType != ERR_DISCONNECT )
		Com_SetErrorMessage(com_errorMessage);

	if ( com_errorType != ERR_DISCONNECT )
		Scr_Abort();

	Com_CleanupBsp();
	CM_Cleanup();
	Com_ResetParseSessions();
	FS_ResetFiles();

	if ( com_errorType == ERR_DROP )
		Cbuf_Init();

	// if we are getting a solid stream of ERR_DROP, do an ERR_FATAL
	currentTime = Sys_Milliseconds();
	if ( currentTime - lastErrorTime < 100 )
	{
		if ( ++errorCount > 3 )
		{
			com_errorType = ERR_FATAL;
		}
	}
	else
	{
		errorCount = 0;
	}
	lastErrorTime = currentTime;

	if ( com_errorType != ERR_SERVERDISCONNECT && com_errorType != ERR_DROP && com_errorType != ERR_DISCONNECT )
		Sys_Error("%s", com_errorMessage);

	if ( com_errorType == ERR_SERVERDISCONNECT )
	{
		Com_ShutdownInternal("EXE_DISCONNECTEDFROMOWNLISTENSERVER");
	}
	else
	{
		Com_Printf("********************\nERROR: %s\n********************\n", com_errorMessage);
		Com_ShutdownInternal(msgBuf);

		if ( com_errorType == ERR_DROP )
		{
			if ( QuitOnError() )
				Com_Quit_f();
		}
	}

	com_fixedConsolePosition = 0;
	com_errorEntered = 0;
}

/*
=============
Com_Error

Both client and server can use this, and it will
do the apropriate things.
=============
*/
void Com_Error( errorParm_t code, const char *fmt, ... )
{
	va_list		argptr;
	jmp_buf*	abortframe;

	if ( com_errorEntered )
		Sys_Error("recursive error after: %s", com_errorMessage);

	com_errorEntered = 1;

	va_start(argptr, fmt);
	Q_vsnprintf(com_errorMessage, sizeof(com_errorMessage), fmt, argptr);
	va_end (argptr);

	if ( code == ERR_SCRIPT || code == ERR_LOCALIZATION )
	{
		code = ERR_DROP;
	}
	else if ( code == ERR_SCRIPT_DROP )
	{
		code = ERR_DROP;
	}
	else
	{
		com_fixedConsolePosition = 0;
	}

	com_errorType = code;
	abortframe = (jmp_buf*)Sys_GetValue(THREAD_VALUE_COM_ERROR);
	longjmp(*abortframe, -1);
}

/*
================
Com_StartHunkUsers
================
*/
void Com_StartHunkUsers( void )
{
	UNIMPLEMENTED(__FUNCTION__);
}

/*
================
Com_BeginRedirect
================
*/
void Com_BeginRedirect( char *buffer, int buffersize, void (*flush)( char *) )
{
	if (!buffer || !buffersize || !flush)
		return;
	rd_buffer = buffer;
	rd_buffersize = buffersize;
	rd_flush = flush;

	*rd_buffer = 0;
}

/*
================
Com_EndRedirect
================
*/
void Com_EndRedirect( void )
{
	if ( rd_flush )
	{
		rd_flush(rd_buffer);
	}

	rd_buffer = NULL;
	rd_buffersize = 0;
	rd_flush = NULL;
}

/*
================
Com_SetRecommended
================
*/
void Com_SetRecommended( int restart )
{
	UNIMPLEMENTED(__FUNCTION__);
}

/*
================
Com_CheckSetRecommended
================
*/
void Com_CheckSetRecommended()
{
	UNIMPLEMENTED(__FUNCTION__);
}
