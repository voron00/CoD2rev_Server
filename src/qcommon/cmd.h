#pragma once

#include "qcommon.h"

// paramters for command buffer stuffing
typedef enum
{
	EXEC_NOW,			// don't return until completed, a VM should NEVER use this,
	// because some commands might cause the VM to be unloaded...
	EXEC_INSERT,		// insert at current position, but don't run yet
	EXEC_APPEND			// add to end of the command buffer (normal case)
} cbufExec_t;

void Cbuf_Init (void);
void Cmd_Init( void );
int Cmd_Argc( void );
const char *Cmd_Argv( int arg );
void Cmd_CommandCompletion( void(*callback)(const char *s) );
void Cbuf_Execute( void );
void Cbuf_ExecuteText( int exec_when, const char *text );
void Cbuf_AddText( const char *text );
void Cmd_TokenizeString( const char *text_in );
void Cmd_ExecuteString( const char *text );
void Cmd_AddCommand( const char *cmd_name, xcommand_t function );
void Cmd_RemoveCommand( const char *cmd_name );

#define SV_Cmd_Argc Cmd_Argc
#define SV_Cmd_Argv Cmd_Argv
#define SV_Cmd_ExecuteString Cmd_ExecuteString
#define SV_Cmd_TokenizeString Cmd_TokenizeString
