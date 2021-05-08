#include "i_functions.h"
#include "i_common.h"
#include "i_shared.h"
#include "entity.h"
#include "netchan.h"
#include "server.h"
#include "filesystem.h"
#include "cmd.h"

const char *svc_strings[256] =
{
	"svc_nop",
	"svc_gamestate",
	"svc_configstring",
	"svc_baseline",
	"svc_serverCommand",
	"svc_download",
	"svc_snapshot",
	"svc_EOF"
};

typedef struct
{
	fileHandle_t demofile;
	int demoMessageSequence;
	int gameStatesParsed;
	int demoCompleted;
} demoFileState_t;

demoFileState_t	demo;

void CL_DemoCompleted(demoFileState_t demo)
{
	demo.demoCompleted = 1;
}

void SHOWNET( msg_t *msg, const char *s )
{
	Com_Printf( "%3i:%s\n", msg->readcount - 1, s );
}

gameState_t gameState;
entityState_t entityBaselines[MAX_GENTITIES];
void CL_ParseGamestate( msg_t *msg )
{
	int i;
	entityState_t   *es;
	int newnum;
	entityState_t nullstate;
	int cmd;
	char            *s;

	// a gamestate always marks a server command sequence
	MSG_ReadLong( msg );

	// parse all the configstrings and baselines
	gameState.dataCount = 1; // leave a 0 at the beginning for uninitialized configstrings

	while ( 1 )
	{
		cmd = MSG_ReadByte( msg );

		if ( cmd == svc_EOF )
		{
			break;
		}

		if ( cmd == svc_configstring )
		{
			int len;

			i = MSG_ReadShort( msg );
			if ( i < 0 || i >= MAX_CONFIGSTRINGS )
			{
				Com_Error( ERR_DROP, "configstring > MAX_CONFIGSTRINGS" );
			}
			s = MSG_ReadBigString( msg );

			len = strlen( s );
			//Com_Printf("CL_ParseGamestate: svc_configstring\n");
			//Com_Printf("%s\n", s);

			if ( len + 1 + gameState.dataCount > MAX_GAMESTATE_CHARS )
			{
				Com_Error( ERR_DROP, "MAX_GAMESTATE_CHARS exceeded" );
			}

			// append it to the gameState string buffer
			gameState.stringOffsets[ i ] = gameState.dataCount;
			memcpy( gameState.stringData + gameState.dataCount, s, len + 1 );
			gameState.dataCount += len + 1;
		}
		else if ( cmd == svc_baseline )
		{
			//Com_Printf("CL_ParseGamestate: svc_baseline\n");
			newnum = MSG_ReadBits( msg, GENTITYNUM_BITS );
			if ( newnum < 0 || newnum >= MAX_GENTITIES )
			{
				Com_Error( ERR_DROP, "Baseline number out of range: %i", newnum );
			}
			memset( &nullstate, 0, sizeof( nullstate ) );
			es = &entityBaselines[ newnum ];
			MSG_ReadDeltaEntity( msg, &nullstate, es, newnum );
		}
		else
		{
			Com_Error( ERR_DROP, "CL_ParseGamestate: bad command byte %d", cmd);
		}
	}

	// read the client number
	MSG_ReadLong( msg );
	// read the checksum feed
	MSG_ReadLong( msg );
}

void CL_ParseServerMessage( msg_t *msg )
{
	int cmd;
	byte buffer[MAX_MSGLEN];
	msg_t decompressMsg;

	// get the reliable sequence acknowledge number
	MSG_ReadLong( msg );

	MSG_Init(&decompressMsg, buffer, sizeof(buffer));
	decompressMsg.cursize = MSG_ReadBitsCompress(msg->data + msg->readcount, msg->cursize - msg->readcount, decompressMsg.data, decompressMsg.maxsize);

	//
	// parse the message
	//
	while ( 1 )
	{
		if ( decompressMsg.readcount > decompressMsg.cursize )
		{
			Com_Error( ERR_DROP,"CL_ParseServerMessage: read past end of server message" );
			break;
		}

		cmd = MSG_ReadByte( &decompressMsg );

		if ( cmd == svc_EOF )
		{
			SHOWNET( &decompressMsg, "END OF MESSAGE\n" );
			break;
		}

		if ( !svc_strings[cmd] )
		{
			Com_Printf( "%3i:BAD CMD %i\n", decompressMsg.readcount - 1, cmd );
		}
		else
		{
			SHOWNET( &decompressMsg, svc_strings[cmd] );
		}

		// other commands
		switch ( cmd )
		{
		default:
			Com_Error( ERR_DROP,"CL_ParseServerMessage: Illegible server message %d\n", cmd );
			break;
		case svc_nop:
			Com_Printf("svc_nop\n");
			break;
		case svc_serverCommand:
			Com_Printf("svc_serverCommand\n");
			//CL_ParseCommandString( &decompressMsg );
			break;
		case svc_gamestate:
			Com_Printf("svc_gamestate\n");
			CL_ParseGamestate( &decompressMsg );
			break;
		case svc_snapshot:
			Com_Printf("svc_snapshot\n");
			//CL_ParseSnapshot( &decompressMsg );
			break;
		case svc_download:
			Com_Printf("svc_download\n");
			break;
		}
	}
}

void CL_ReadDemoMessage( void )
{
	int r;
	msg_t buf;
	byte bufData[ MAX_MSGLEN ];
	int s;

	if ( !demo.demofile )
	{
		CL_DemoCompleted(demo);
		return;
	}

	// get the sequence number
	r = FS_Read( &s, 4, demo.demofile );
	if ( r != 4 )
	{
		CL_DemoCompleted(demo);
		return;
	}

	// init the message
	MSG_Init( &buf, bufData, sizeof( bufData ) );

	// get the length
	r = FS_Read( &buf.cursize, 4, demo.demofile );

	if ( r != 4 )
	{
		CL_DemoCompleted(demo);
		return;
	}
	buf.cursize = LittleLong( buf.cursize );
	if ( buf.cursize == -1 )
	{
		CL_DemoCompleted(demo);
		return;
	}
	if ( buf.cursize > buf.maxsize )
	{
		Com_Error( ERR_DROP, "CL_ReadDemoMessage: demoMsglen > MAX_MSGLEN" );
	}
	r = FS_Read( buf.data, buf.cursize, demo.demofile );
	if ( r != buf.cursize )
	{
		Com_Printf( "Demo file was truncated.\n" );
		CL_DemoCompleted(demo);
		return;
	}

	CL_ParseServerMessage( &buf );
}

void SV_ParseDemo_f()
{
	const char *demoname;

	if (SV_Cmd_Argc() < 2)
	{
		Com_Printf("Usage: parsedemo [demofile]\n");
		return;
	}

	demoname = SV_Cmd_Argv(1);

	if (FS_SV_FOpenFileRead(demoname, &demo.demofile))
	{
		while (!demo.demoCompleted)
			CL_ReadDemoMessage();

		FS_FCloseFile( demo.demofile );
	}
	else
	{
		Com_Printf("Couldn't open demo file %s\n", demoname);
	}
}