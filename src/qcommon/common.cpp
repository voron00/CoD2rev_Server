#include "qcommon.h"
#include "cm_local.h"
#include "cmd.h"
#include "sys_thread.h"

#include "../clientscript/clientscript_public.h"
#include "../universal/universal_public.h"
#include "../sound/sound_public.h"
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
dvar_t *sv_running;
dvar_t *com_introPlayed;
dvar_t *com_animCheck;
dvar_t *com_sv_running;

dvar_t *ui_errorMessage;
dvar_t *ui_errorTitle;

static fileHandle_t logfile;

int com_frameTime;
int com_lastFrameTime;

#define MAX_CONSOLE_LINES 32

int com_numConsoleLines;
char *com_consoleLines[MAX_CONSOLE_LINES];

qboolean com_errorEntered;
qboolean com_fullyInitialized = qfalse;

errorParm_t com_errorType;
char com_errorMessage[MAXPRINTMSG];

int com_fixedConsolePosition;

static const char *completionString;
static char shortestMatch[MAX_TOKEN_CHARS];
static int	matchCount;
static field_t *completionField;

static void FindMatches( const char *s )
{
	int		i;

	if ( Q_stricmpn( s, completionString, strlen( completionString ) ) )
	{
		return;
	}
	matchCount++;
	if ( matchCount == 1 )
	{
		Q_strncpyz( shortestMatch, s, sizeof( shortestMatch ) );
		return;
	}

	// cut shortestMatch to the amount common with s
	for ( i = 0 ; s[i] ; i++ )
	{
		if ( tolower(shortestMatch[i]) != tolower(s[i]) )
		{
			shortestMatch[i] = 0;
		}
	}
}

static void PrintMatches( const char *s )
{
	if ( !Q_stricmpn( s, shortestMatch, strlen( shortestMatch ) ) )
	{
		Com_Printf( "    %s\n", s );
	}
}

static void keyConcatArgs( void )
{
	int		i;
	const char	*arg;

	for ( i = 1 ; i < Cmd_Argc() ; i++ )
	{
		Q_strcat( completionField->buffer, sizeof( completionField->buffer ), " " );
		arg = Cmd_Argv( i );
		while (*arg)
		{
			if (*arg == ' ')
			{
				Q_strcat( completionField->buffer, sizeof( completionField->buffer ),  "\"");
				break;
			}
			arg++;
		}
		Q_strcat( completionField->buffer, sizeof( completionField->buffer ),  Cmd_Argv( i ) );
		if (*arg == ' ')
		{
			Q_strcat( completionField->buffer, sizeof( completionField->buffer ),  "\"");
		}
	}
}

static void ConcatRemaining( char *src, const char *start )
{
	char *str;

	str = strstr(src, start);
	if (!str)
	{
		keyConcatArgs();
		return;
	}

	str += strlen(start);
	Q_strcat( completionField->buffer, sizeof( completionField->buffer ), str);
}

void Field_CompleteCommand( field_t *field )
{
	field_t		temp;

	completionField = field;

	// only look at the first token for completion purposes
	Cmd_TokenizeString( completionField->buffer );

	completionString = Cmd_Argv(0);
	if ( completionString[0] == '\\' || completionString[0] == '/' )
	{
		completionString++;
	}
	matchCount = 0;
	shortestMatch[0] = 0;

	if ( strlen( completionString ) == 0 )
	{
		return;
	}

	Cmd_CommandCompletion( FindMatches );
	Dvar_CommandCompletion( FindMatches );

	if ( matchCount == 0 )
	{
		return;	// no matches
	}

	Com_Memcpy(&temp, completionField, sizeof(field_t));

	if ( matchCount == 1 )
	{
		Com_sprintf( completionField->buffer, sizeof( completionField->buffer ), "\\%s", shortestMatch );
		if ( Cmd_Argc() == 1 )
		{
			Q_strcat( completionField->buffer, sizeof( completionField->buffer ), " " );
		}
		else
		{
			ConcatRemaining( temp.buffer, completionString );
		}
		completionField->cursor = strlen( completionField->buffer );
		return;
	}

	// multiple matches, complete to shortest
	Com_sprintf( completionField->buffer, sizeof( completionField->buffer ), "\\%s", shortestMatch );
	completionField->cursor = strlen( completionField->buffer );
	ConcatRemaining( temp.buffer, completionString );

	Com_Printf( "]%s\n", completionField->buffer );

	// run through again, printing matches
	Cmd_CommandCompletion( PrintMatches );
	Dvar_CommandCompletion( PrintMatches );
}

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

static char		*rd_buffer;
static size_t	rd_buffersize;
static void		(*rd_flush)( char *buffer );
void Com_BeginRedirect (char *buffer, int buffersize, void (*flush)( char *) )
{
	if (!buffer || !buffersize || !flush)
		return;
	rd_buffer = buffer;
	rd_buffersize = buffersize;
	rd_flush = flush;

	*rd_buffer = 0;
}

void Com_EndRedirect (void)
{
	if ( rd_flush )
	{
		rd_flush(rd_buffer);
	}

	rd_buffer = NULL;
	rd_buffersize = 0;
	rd_flush = NULL;
}

void Com_PrintMessage( conChannel_t channel, const char *fmt, ... )
{
	va_list		argptr;
	char		msg[MAXPRINTMSG];
	static qboolean opening_qconsole = qfalse;

	va_start (argptr,fmt);
	Q_vsnprintf (msg, sizeof(msg), fmt, argptr);
	va_end (argptr);

	if ( rd_buffer )
	{
		if ((strlen (msg) + strlen(rd_buffer)) > (rd_buffersize - 1))
		{
			rd_flush(rd_buffer);
			*rd_buffer = 0;
		}
		Q_strcat(rd_buffer, rd_buffersize, msg);
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

	// echo to dedicated console and early console
	Sys_Print( msg );

	// logfile
	if ( com_logfile && com_logfile->current.integer )
	{
		// TTimo: only open the qconsole.log if the filesystem is in an initialized state
		//   also, avoid recursing in the qconsole.log opening (i.e. if fs_debug is on)
		if ( !logfile && FS_Initialized() && !opening_qconsole)
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
		if ( logfile && FS_Initialized())
		{
			FS_Write(msg, strlen(msg), logfile);

			if ( com_logfile->current.integer > 1 )
				FS_Flush(logfile);
		}
	}
}

void Com_Printf( const char *fmt, ...)
{
	va_list		argptr;
	char		msg[MAXPRINTMSG];

	va_start (argptr,fmt);
	Q_vsnprintf (msg, sizeof(msg), fmt, argptr);
	va_end (argptr);

	Com_PrintMessage(CON_CHANNEL_DONT_FILTER, "%s", msg);
}

void Com_DPrintf( const char *fmt, ...)
{
	va_list		argptr;
	char		msg[MAXPRINTMSG];

	if ( !com_developer || !com_developer->current.integer )
	{
		return;			// don't confuse non-developers with techie stuff...
	}

	va_start (argptr,fmt);
	Q_vsnprintf (msg, sizeof(msg), fmt, argptr);
	va_end (argptr);

	Com_PrintMessage(CON_CHANNEL_DONT_FILTER, "%s", msg);
}

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

void Com_Error(errorParm_t code, const char *format, ...)
{
	va_list		argptr;
	jmp_buf*	abortframe;

	if ( com_errorEntered )
		Sys_Error("recursive error after: %s", com_errorMessage);

	com_errorEntered = 1;

	va_start(argptr, format);
	Q_vsnprintf(com_errorMessage, 0x1000, format, argptr);
	va_end (argptr);

	if ( code != ERR_SCRIPT && code != ERR_LOCALIZATION )
	{
		if ( code == ERR_SCRIPT_DROP )
			code = ERR_FATAL;
		else
			com_fixedConsolePosition = 0;
	}
	else
	{
		code = ERR_FATAL;
	}

	com_errorType = code;
	abortframe = (jmp_buf*)Sys_GetValue(THREAD_VALUE_COM_ERROR);
	longjmp(*abortframe, -1);
}

void Com_SetErrorMessage(const char *errorMessage)
{
	ui_errorMessage = Dvar_RegisterString("com_errorMessage", "", 4160);
	ui_errorTitle = Dvar_RegisterString("com_errorTitle", "", 4160);
	Dvar_SetString(ui_errorMessage, errorMessage);
}

void Com_Restart()
{
	SV_ShutdownGameProgs();
	Com_ShutdownDObj();
	DObjShutdown();
	XAnimShutdown();
	CM_Shutdown();
	SND_ShutdownChannels();
	Hunk_Clear();
	Scr_Init();
	Scr_Settings(com_logfile->current.integer || com_developer->current.integer, com_developer_script->current.integer, com_developer->current.integer);
	com_fixedConsolePosition = 0;
	XAnimInit();
	DObjInit();
	Com_InitDObj();
}

void Com_Close()
{
	Com_ShutdownDObj();
	DObjShutdown();
	XAnimShutdown();
	CM_Shutdown();
	SND_ShutdownChannels();
	Hunk_Clear();
	Scr_Shutdown();
}

void Com_ShutdownInternal(const char *reason)
{
#ifndef DEDICATED
	CL_SwitchToLocalClient();
	CL_Disconnect();
	CL_SwitchToLocalClient();
	CL_ShutdownAll();
	CL_ShutdownDemo();
#endif
	SV_Shutdown(reason);
	Com_Restart();
}

void Com_Shutdown(const char *reason)
{
	Com_ShutdownInternal(reason);
}

void Com_CloseLogFile()
{
	if ( logfile )
	{
		FS_FCloseFile(logfile);
		logfile = 0;
	}
}

void Com_Quit_f()
{
	Com_Printf("quitting...\n");

	if ( !com_errorEntered )
	{
		Hunk_ClearTempMemory();
		SV_Shutdown("EXE_SERVERQUIT");
		Com_Close();
		Com_CloseLogFile();
		FS_Shutdown();
		FS_ShutdownServerIwdNames();
		FS_ShutdownServerReferencedIwds();
	}

	Sys_Quit();
}

void Com_ErrorCleanup( void )
{
	static int	lastErrorTime;
	static int	errorCount;
	int			currentTime;
	char msgBuf[MAXPRINTMSG];

	LargeLocalReset();
	Dvar_SetInAutoExec(0);
	Hunk_ClearTempMemory();
	Hunk_ClearTempMemoryHigh();
	Dvar_SetIntByName("cl_paused", 0);
	FS_PureServerSetLoadedIwds("", "");

	strcpy(msgBuf, com_errorMessage);

	if ( com_errorType != ERR_DISCONNECT )
	{
		Com_SetErrorMessage(com_errorMessage);
		Scr_Abort();
	}

	SND_ErrorCleanup();
	Com_CleanupBsp();
	CM_Cleanup();
	Com_ResetParseSessions();

	if ( com_errorType == ERR_DROP )
		Cbuf_Init();

	lastErrorTime = Sys_MilliSeconds();

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

void Com_RunAutoExec()
{
	Dvar_SetInAutoExec(1);
	Cbuf_Execute();
	Dvar_SetInAutoExec(0);
}

void Com_WriteConfiguration( void )
{
	// if we are quiting without fully initializing, make sure
	// we don't write out anything
	if ( !com_fullyInitialized )
		return;

	if ( !(dvar_modifiedFlags & DVAR_ARCHIVE ) )
		return;

	dvar_modifiedFlags &= ~DVAR_ARCHIVE;
	Com_WriteConfigToFile( CONFIG_NAME );

}

void Com_ExecStartupConfigs(const char *configFile)
{
	Cbuf_AddText("exec default_mp.cfg\n");
	Cbuf_AddText("exec language.cfg\n");
	if ( configFile )
		Cbuf_AddText(va("exec %s\n", configFile));
	Cbuf_Execute();
	Com_RunAutoExec();
	if ( Com_SafeMode() )
		Cbuf_AddText("exec safemode_mp_server.cfg\n");
	Cbuf_Execute();
}

void Com_InitPlayerProfiles()
{
	Com_ExecStartupConfigs(CONFIG_NAME);
}

void Field_Clear( field_t *edit )
{
	memset(edit->buffer, 0, MAX_EDIT_LINE);
	edit->cursor = 0;
	edit->scroll = 0;
}

// bk001129 - here we go again: upped from 64
#define MAX_PUSHED_EVENTS               256
// bk001129 - init, also static
static int com_pushedEventsHead = 0;
static int com_pushedEventsTail = 0;
// bk001129 - static
static sysEvent_t com_pushedEvents[MAX_PUSHED_EVENTS];

sysEvent_t Com_GetEvent( void )
{
	if ( com_pushedEventsHead > com_pushedEventsTail )
	{
		com_pushedEventsTail++;
		return com_pushedEvents[ ( com_pushedEventsTail - 1 ) & ( MAX_PUSHED_EVENTS - 1 ) ];
	}
	return Sys_GetEvent();
}

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
#ifndef DEDICATED
			// manually send packet events for the loopback channel
			while ( NET_GetLoopPacket( NS_CLIENT, &evFrom, &buf ) )
			{
				CL_PacketEvent( evFrom, &buf );
			}
#endif
			while ( NET_GetLoopPacket( NS_SERVER, &evFrom, &buf ) )
			{
				// if the server just shut down, flush the events
				if ( com_sv_running && com_sv_running->current.integer )
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
			break;
		case SE_KEY:
#ifndef DEDICATED
			CL_KeyEvent( ev.evValue, ev.evValue2, ev.evTime );
#endif
			break;
		case SE_CHAR:
#ifndef DEDICATED
			CL_CharEvent( ev.evValue );
#endif
			break;
		case SE_MOUSE:
#ifndef DEDICATED
			CL_MouseEvent( ev.evValue, ev.evValue2, ev.evTime );
#endif
			break;
		case SE_JOYSTICK_AXIS:
#ifndef DEDICATED
			CL_JoystickEvent( ev.evValue, ev.evValue2, ev.evTime );
#endif
			break;
		case SE_CONSOLE:
			Cbuf_AddText( (char *)ev.evPtr );
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
				Com_Printf("Com_EventLoop: oversize packet\n");
				continue;
			}
			Com_Memcpy( buf.data, (byte *)((netadr_t *)ev.evPtr + 1), buf.cursize );
			if ( com_sv_running && com_sv_running->current.integer )
			{
				SV_PacketEvent( evFrom, &buf );
			}
			else
			{
#ifndef DEDICATED
				CL_PacketEvent( evFrom, &buf );
#endif
			}
			break;
		}

		// free any block data
		if ( ev.evPtr )
		{
			Z_Free( ev.evPtr );
		}
	}

	return 0;	// never reached
}

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

static void Com_Crash_f( void )
{
	* ( int * ) 0 = 0x12345678;
}

void Com_StartHunkUsers()
{
	jmp_buf* abortframe = (jmp_buf*)Sys_GetValue(THREAD_VALUE_COM_ERROR);

	if (setjmp(*abortframe))
		Sys_Error("Error during initialization");

#ifndef DEDICATED
	UI_SetMap("", "");
	CL_StartHunkUsers();
#endif
	Com_EventLoop();
#ifndef DEDICATED
	if ( com_dedicated )
	{
		if ( !com_dedicated->current.integer )
		{
			UI_SetActiveMenu(1);
		}
	}
#endif
}

int Com_ModifyMsec( int msec )
{
	int clampTime;

	//
	// modify time for debugging values
	//
	if ( com_fixedtime->current.integer )
	{
		msec = com_fixedtime->current.integer;
	}
	else if ( com_timescale->current.decimal )
	{
		msec *= com_timescale->current.decimal;
	}

	// don't let it scale below 1 msec
	if ( msec < 1 && com_timescale->current.decimal )
	{
		msec = 1;
	}

	if ( com_dedicated->current.integer )
	{
		// dedicated servers don't want to clamp for a much longer
		// period, because it would mess up all the client's views
		// of time.
		if ( msec > 500 && msec < 500000 )
		{
			Com_Printf( "Hitch warning: %i msec frame time\n", msec );
		}
		clampTime = 5000;
	}
	else if ( !com_sv_running->current.integer )
	{
		// clients of remote servers do not want to clamp time, because
		// it would skew their view of the server's time temporarily
		clampTime = 5000;
	}
	else
	{
		// for local single player gaming
		// we may want to clamp the time to prevent players from
		// flying off edges when something hitches.
		clampTime = 200;
	}

	if ( msec > clampTime )
	{
		msec = clampTime;
	}

	return msec;
}

void Com_UpdateViewLog()
{
	if ( com_viewlog->modified )
	{
		if ( !com_dedicated->current.integer )
		{
			Sys_ShowConsole( com_viewlog->current.integer, qfalse );
		}

		Dvar_ClearModified(com_viewlog);
	}
}

void Com_Frame_Try_Block_Function()
{
	int msec, minMsec;

	Com_WriteConfiguration();
	Com_UpdateViewLog();
	SetAnimCheck(com_animCheck->current.boolean);

	// we may want to spin here if things are going too fast
	if ( !com_dedicated->current.integer && com_maxfps->current.integer > 0 )
	{
		minMsec = 1000 / com_maxfps->current.integer;
	}
	else
	{
		minMsec = 1;
	}
	do
	{
		com_frameTime = Com_EventLoop();

		if ( com_lastFrameTime > com_frameTime )
		{
			com_lastFrameTime = com_frameTime;
		}

		msec = com_frameTime - com_lastFrameTime;
		NET_Sleep(0);
	}
	while ( msec < minMsec );

	Cbuf_Execute();
	com_lastFrameTime = com_frameTime;
	msec = Com_ModifyMsec( msec );
#ifndef DEDICATED
	CL_SwitchToLocalClient();
#endif
	SV_Frame( msec );
}

void Com_Frame( void )
{
	static unsigned int com_frameNumber;

	jmp_buf* abortframe = (jmp_buf*)Sys_GetValue(THREAD_VALUE_COM_ERROR);

	if (!setjmp(*abortframe))
	{
		Com_Frame_Try_Block_Function();
		++com_frameNumber;
	}

	if ( com_errorEntered )
	{
		Com_ErrorCleanup();
		Com_StartHunkUsers();
	}
}

void Com_SetCinematic()
{
	if ( !com_dedicated->current.integer && !com_introPlayed->current.integer )
	{
		Cbuf_AddText("cinematic atvi\n");
		Dvar_SetString(nextmap, "cinematic IW_logo; set nextmap cinematic cod_intro");
		Dvar_SetBool(com_introPlayed, 1);
	}
}

void Com_WriteConfigToFile(const char *contents)
{
	fileHandle_t file;

	file = FS_FOpenFileWrite(contents);

	if ( file )
	{
		FS_Printf(file, "// generated by Call of Duty, do not modify\n");
		Dvar_WriteVariables(file);
		FS_FCloseFile(file);
	}
	else
	{
		Com_Printf("Couldn't write %s.\n", contents);
	}
}

void Com_WriteDefaultConfigToFile(char *contents)
{
	fileHandle_t file;

	file = FS_FOpenFileWrite(contents);

	if ( file )
	{
		FS_Printf(file, "// generated by Call of Duty, do not modify\n");
		Dvar_WriteDefaults(file);
		FS_FCloseFile(file);
	}
	else
	{
		Com_Printf("Couldn't write %s.\n", contents);
	}
}

void Com_WriteConfig_f()
{
	char buf[MAX_QPATH];

	if ( Cmd_Argc() == 2 )
	{
		const char *cmd = Cmd_Argv(1u);
		I_strncpyz(buf, cmd, sizeof(buf));
		Com_DefaultExtension(buf, MAX_QPATH, ".cfg");
		Com_Printf("Writing %s.\n", buf);
		Com_WriteConfigToFile(buf);
	}
	else
	{
		Com_Printf("Usage: writeconfig <filename>\n");
	}
}

void Com_WriteDefaults_f()
{
	char buf[MAX_QPATH];

	if ( Cmd_Argc() == 2 )
	{
		const char *cmd = Cmd_Argv(1u);
		I_strncpyz(buf, cmd, sizeof(buf));
		Com_DefaultExtension(buf, MAX_QPATH, ".cfg");
		Com_Printf("Writing %s.\n", buf);
		Com_WriteDefaultConfigToFile(buf);
	}
	else
	{
		Com_Printf("Usage: writeconfig <filename>\n");
	}
}

void Com_Init_Try_Block_Function(char *commandLine)
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
#ifndef DEDICATED
	CL_SwitchToLocalClient();
#endif
	Com_InitPlayerProfiles();
#ifndef DEDICATED
	CL_SwitchToLocalClient();
#endif
	Cbuf_Execute();
	Com_StartupVariable(0);
	Com_InitHunkMemory();
	Com_InitParse();
	dvar_modifiedFlags &= 0xFFFFFFFE;
	// com_codeTimeScale = 1.0;
	if ( com_developer->current.integer )
	{
		Cmd_AddCommand("error", Com_Error_f);
		Cmd_AddCommand("crash", Com_Crash_f);
		Cmd_AddCommand("freeze", Com_Freeze_f);
	}
	Cmd_AddCommand("quit", Com_Quit_f);
	Cmd_AddCommand("writeconfig", Com_WriteConfig_f);
	Cmd_AddCommand("writedefaults", Com_WriteDefaults_f);
	Dvar_RegisterString("version", va("%s %s build %s %s", GAME_STRING,PRODUCT_VERSION,CPUSTRING, __DATE__), 4160);
	Dvar_RegisterString("shortversion", PRODUCT_VERSION, 4164);
#ifndef DEDICATED
	FxMem_Init();
#endif
	Sys_Init();
	Netchan_Init(Com_Milliseconds());
	Scr_Init();
	Scr_Settings(com_logfile->current.integer || com_developer->current.integer, com_developer_script->current.integer, com_developer->current.integer);
	XAnimInit();
	DObjInit();
	SV_Init();
	NET_Init();
	com_frameTime  = Com_Milliseconds();
	Com_AddStartupCommands();
	Com_SetCinematic();
	com_fullyInitialized = 1;
	Com_Printf("--- Common Initialization Complete ---\n");
	Cbuf_Execute();
}

void Com_Init(char* commandLine)
{
	jmp_buf* abortframe = (jmp_buf*)Sys_GetValue(THREAD_VALUE_COM_ERROR);

	if (setjmp(*abortframe))
		Sys_Error(va("Error during Initialization:\n%s\n", com_errorMessage));

	Com_Init_Try_Block_Function(commandLine);
}