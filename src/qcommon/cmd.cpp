#include "cmd.h"

#define	MAX_CMD_BUFFER	0x10000
#define	MAX_CMD_LINE	0x1000

typedef struct
{
	byte	*data;
	int		maxsize;
	int		cursize;
} cmd_t;

int			cmd_wait;
cmd_t		cmd_text;
byte		cmd_text_buf[MAX_CMD_BUFFER];

typedef struct cmd_function_s
{
	struct cmd_function_s   *next;
	char                    *name;
	xcommand_t function;
} cmd_function_t;

static int cmd_argc;
static char *cmd_argv[MAX_STRING_TOKENS];        // points into cmd_tokenized
static char cmd_tokenized[BIG_INFO_STRING + MAX_STRING_TOKENS];         // will have 0 bytes inserted
static char cmd_cmd[BIG_INFO_STRING];         // the original command we received (no token processing)
static cmd_function_t  *cmd_functions;      // possible commands to execute

/*
============
Cmd_Argc
============
*/
int Cmd_Argc( void )
{
	return cmd_argc;
}

/*
============
Cmd_Argv
============
*/
const char *Cmd_Argv( int arg )
{
	if ( arg >= cmd_argc )
	{
		return "";
	}
	return cmd_argv[arg];
}

/*
============
Cmd_CommandCompletion
============
*/
void Cmd_CommandCompletion( void(*callback)(const char *s) )
{
	cmd_function_t	*cmd;

	for (cmd=cmd_functions ; cmd ; cmd=cmd->next)
	{
		callback( cmd->name );
	}
}

/*
============
Cbuf_InsertText
Adds command text immediately after the current command
Adds a \n to the text
============
*/
void Cbuf_InsertText( const char *text )
{
	int len;
	int i;

	len = strlen( text ) + 1;
	if ( len + cmd_text.cursize > cmd_text.maxsize )
	{
		Com_Printf( "Cbuf_InsertText overflowed\n" );
		return;
	}

	// move the existing command text
	for ( i = cmd_text.cursize - 1 ; i >= 0 ; i-- )
	{
		cmd_text.data[ i + len ] = cmd_text.data[ i ];
	}

	// copy the new text in
	memcpy( cmd_text.data, text, len - 1 );

	// add a \n
	cmd_text.data[ len - 1 ] = '\n';

	cmd_text.cursize += len;
}

/*
============
Cmd_ExecuteString
A complete command line has been parsed, so try to execute it
============
*/
void Cmd_ExecuteString( const char *text )
{
	cmd_function_t  *cmd, **prev;

	// execute the command line
	Cmd_TokenizeString( text );

	if ( !Cmd_Argc() )
	{
		return;     // no tokens
	}

	// check registered command functions
	for ( prev = &cmd_functions ; *prev ; prev = &cmd->next )
	{
		cmd = *prev;
		if ( !I_stricmp( cmd_argv[0],cmd->name ) )
		{
			// rearrange the links so that the command will be
			// near the head of the list next time it is used
			*prev = cmd->next;
			cmd->next = cmd_functions;
			cmd_functions = cmd;

			// perform the action
			if ( !cmd->function )
			{
				// let the cgame or game handle it
				break;
			}
			else
			{
				cmd->function();
			}
			return;
		}
	}

	// check dvars
	if ( Dvar_Command() )
	{
		return;
	}

#ifndef DEDICATED
	// check client game commands
	if ( com_cl_running && com_cl_running->current.integer && CL_GameCommand() )
	{
		return;
	}
#endif

	// check server game commands
	if ( com_sv_running && com_sv_running->current.integer && SV_GameCommand() )
	{
		return;
	}

#ifndef DEDICATED
	// check ui commands
	if ( com_cl_running && com_cl_running->current.integer && UI_GameCommand() )
	{
		return;
	}

	// send it as a server command if we are connected
	// this will usually result in a chat message
	CL_ForwardCommandToServer( text );
#endif
}

/*
============
Cbuf_AddText
Adds command text at the end of the buffer, does NOT add a final \n
============
*/
void Cbuf_AddText( const char *text )
{
	int l;

	l = strlen( text );

	if ( cmd_text.cursize + l >= cmd_text.maxsize )
	{
		Com_Printf( "Cbuf_AddText: overflow\n" );
		return;
	}
	memcpy( &cmd_text.data[cmd_text.cursize], text, l );
	cmd_text.cursize += l;
}

/*
============
Cbuf_Execute
============
*/
void Cbuf_Execute( void )
{
	int i;
	char    *text;
	char line[MAX_CMD_LINE];
	int quotes;

	while ( cmd_text.cursize )
	{
		if ( cmd_wait )
		{
			// skip out while text still remains in buffer, leaving it
			// for next frame
			cmd_wait--;
			break;
		}

		// find a \n or ; line break
		text = (char *)cmd_text.data;

		quotes = 0;
		for ( i = 0 ; i < cmd_text.cursize ; i++ )
		{
			if ( text[i] == '"' )
			{
				quotes++;
			}
			if ( !( quotes & 1 ) &&  text[i] == ';' )
			{
				break;  // don't break if inside a quoted string
			}
			if ( text[i] == '\n' || text[i] == '\r' )
			{
				break;
			}
		}

		if ( i >= ( MAX_CMD_LINE - 1 ) )
		{
			i = MAX_CMD_LINE - 1;
		}

		memcpy( line, text, i );
		line[i] = 0;

// delete the text from the command buffer and move remaining commands down
// this is necessary because commands (exec) can insert data at the
// beginning of the text buffer

		if ( i == cmd_text.cursize )
		{
			cmd_text.cursize = 0;
		}
		else
		{
			i++;
			cmd_text.cursize -= i;
			memmove( text, text + i, cmd_text.cursize );
		}

// execute the command line

		Cmd_ExecuteString( line );
	}
}

/*
============
Cbuf_ExecuteText
============
*/
void Cbuf_ExecuteText( int exec_when, const char *text )
{
	switch ( exec_when )
	{
	case EXEC_NOW:
		if ( text && strlen( text ) > 0 )
		{
			Cmd_ExecuteString( text );
		}
		else
		{
			Cbuf_Execute();
		}
		break;
	case EXEC_INSERT:
		Cbuf_InsertText( text );
		break;
	case EXEC_APPEND:
		Cbuf_AddText( text );
		break;
	default:
		Com_Error( ERR_FATAL, "Cbuf_ExecuteText: bad exec_when" );
	}
}

/*
============
Cmd_TokenizeString
Parses the given string into command line tokens.
The text is copied to a seperate buffer and 0 characters
are inserted in the apropriate place, The argv array
will point into this temporary buffer.
============
*/
void Cmd_TokenizeString( const char *text_in )
{
	const char  *text;
	char    *textOut;

	// clear previous args
	cmd_argc = 0;

	if ( !text_in )
	{
		return;
	}

	I_strncpyz( cmd_cmd, text_in, sizeof( cmd_cmd ) );

	text = text_in;
	textOut = cmd_tokenized;

	while ( 1 )
	{
		if ( cmd_argc == MAX_STRING_TOKENS )
		{
			return;         // this is usually something malicious
		}

		while ( 1 )
		{
			// skip whitespace
			while ( *text && *text <= ' ' )
			{
				text++;
			}
			if ( !*text )
			{
				return;         // all tokens parsed
			}

			// skip // comments
			if ( text[0] == '/' && text[1] == '/' )
			{
				//bani - lets us put 'http://' in commandlines
				if ( text == text_in || ( text > text_in && text[-1] != ':' ) )
				{
					return;         // all tokens parsed
				}
			}

			// skip /* */ comments
			if ( text[0] == '/' && text[1] == '*' )
			{
				while ( *text && ( text[0] != '*' || text[1] != '/' ) )
				{
					text++;
				}
				if ( !*text )
				{
					return;     // all tokens parsed
				}
				text += 2;
			}
			else
			{
				break;          // we are ready to parse a token
			}
		}

		// handle quoted strings
		if ( *text == '"' )
		{
			cmd_argv[cmd_argc] = textOut;
			cmd_argc++;
			text++;
			while ( *text && *text != '"' )
			{
				*textOut++ = *text++;
			}
			*textOut++ = 0;
			if ( !*text )
			{
				return;     // all tokens parsed
			}
			text++;
			continue;
		}

		// regular token
		cmd_argv[cmd_argc] = textOut;
		cmd_argc++;

		// skip until whitespace, quote, or command
		while ( *text > ' ' )
		{
			if ( text[0] == '"' )
			{
				break;
			}

			if ( text[0] == '/' && text[1] == '/' )
			{
				//bani - lets us put 'http://' in commandlines
				if ( text == text_in || ( text > text_in && text[-1] != ':' ) )
				{
					break;
				}
			}

			// skip /* */ comments
			if ( text[0] == '/' && text[1] == '*' )
			{
				break;
			}

			*textOut++ = *text++;
		}

		*textOut++ = 0;

		if ( !*text )
		{
			return;     // all tokens parsed
		}
	}

}

/*
==============
Cmd_FindCommand
==============
*/
cmd_function_t* Cmd_FindCommand(const char *cmdName)
{
	cmd_function_t *cmd;

	for (cmd = cmd_functions; cmd; cmd = cmd->next)
	{
		if (!strcmp(cmdName, cmd->name))
		{
			return cmd;
		}
	}

	return NULL;
}

/*
============
Cmd_AddCommand
============
*/
void Cmd_AddCommand( const char *cmd_name, xcommand_t function )
{
	cmd_function_t  *cmd;

	// fail if the command already exists
	cmd = Cmd_FindCommand(cmd_name);

	if (cmd)
	{
		if (function != NULL)
		{
			Com_Printf("Cmd_AddCommand: %s already defined\n", cmd_name);
		}
		return;
	}

	// use a small malloc to avoid zone fragmentation
	cmd = (cmd_function_t *)S_Malloc( sizeof( cmd_function_t ) );
	cmd->name = CopyString( cmd_name );
	cmd->function = function;
	cmd->next = cmd_functions;
	cmd_functions = cmd;
}

/*
============
Cmd_RemoveCommand
============
*/
void Cmd_RemoveCommand( const char *cmd_name )
{
	cmd_function_t  *cmd, **back;

	back = &cmd_functions;
	while ( 1 )
	{
		cmd = *back;
		if ( !cmd )
		{
			// command wasn't active
			return;
		}
		if ( !strcmp( cmd_name, cmd->name ) )
		{
			*back = cmd->next;
			if ( cmd->name )
			{
				Z_Free( cmd->name );
			}
			Z_Free( cmd );
			return;
		}
		back = &cmd->next;
	}
}

/*
============
Cmd_List_f
============
*/
void Cmd_List_f( void )
{
	cmd_function_t  *cmd;
	int i;
	const char            *match;

	if ( Cmd_Argc() > 1 )
	{
		match = Cmd_Argv( 1 );
	}
	else
	{
		match = NULL;
	}

	i = 0;
	for ( cmd = cmd_functions ; cmd ; cmd = cmd->next )
	{
		if ( match && !Com_Filter( match, cmd->name, qfalse ) )
		{
			continue;
		}

		Com_Printf( "%s\n", cmd->name );
		i++;
	}
	Com_Printf( "%i commands\n", i );
}

/*
===============
Cmd_Exec_f
===============
*/
void Cmd_Exec_f( void )
{
	char    *f;
	char filename[MAX_QPATH];

	if ( Cmd_Argc() != 2 )
	{
		Com_Printf( "exec <filename> : execute a script file\n" );
		return;
	}

	I_strncpyz( filename, Cmd_Argv( 1 ), sizeof( filename ) );
	Com_DefaultExtension( filename, sizeof( filename ), ".cfg" );
	FS_ReadFile( filename, (void **)&f );

	if ( !f )
	{
		Com_Printf( "couldn't exec %s\n",Cmd_Argv( 1 ) );
		return;
	}

	Com_Printf( "execing %s\n",Cmd_Argv( 1 ) );
	Cbuf_InsertText( f );
	FS_FreeFile( f );
}

/*
===============
Cmd_Vstr_f
Inserts the current value of a variable as command text
===============
*/
void Cmd_Vstr_f( void )
{
	const char *cmd;
	dvar_t *dvar;

	if ( Cmd_Argc() == 2 )
	{
		cmd = Cmd_Argv(1);
		dvar = Dvar_FindVar(cmd);

		if ( dvar )
		{
			if ( dvar->type == DVAR_TYPE_STRING )
			{
				Cbuf_InsertText( va( "%s\n", Dvar_GetString(cmd) ) );
			}
			else
			{
				Com_Printf("%s is not a string-based dvar\n", cmd);
			}
		}
		else
		{
			Com_Printf("%s doesn't exist\n", cmd);
		}
	}
	else
	{
		Com_Printf("vstr <variablename> : execute a variable command\n");
	}
}

/*
============
Cmd_Wait_f
Causes execution of the remainder of the command buffer to be delayed until
next frame.  This allows commands like:
bind g "cmd use rocket ; +attack ; wait ; -attack ; cmd use blaster"
============
*/
void Cmd_Wait_f( void )
{
	if ( Cmd_Argc() == 2 )
	{
		cmd_wait = atoi( Cmd_Argv( 1 ) );
	}
	else
	{
		cmd_wait = 1;
	}
}

/*
============
Cbuf_Init
============
*/
void Cbuf_Init( void )
{
	cmd_text.data = cmd_text_buf;
	cmd_text.maxsize = MAX_CMD_BUFFER;
	cmd_text.cursize = 0;
}

/*
============
Cmd_Init
============
*/
void Cmd_Init( void )
{
	Cmd_AddCommand("cmdlist", Cmd_List_f);
	Cmd_AddCommand("exec", Cmd_Exec_f);
	Cmd_AddCommand("vstr", Cmd_Vstr_f);
	Cmd_AddCommand("wait", Cmd_Wait_f);
}