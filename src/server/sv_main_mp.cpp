#include "../qcommon/qcommon.h"

#ifdef TESTING_LIBRARY
#define svs (*((serverStatic_t*)( 0x0841FB00 )))
#define sv (*((server_t*)( 0x0842BC80 )))
#else
extern server_t sv;
extern serverStatic_t svs;
#endif

void SV_PacketEvent( netadr_t from, msg_t *msg )
{
	UNIMPLEMENTED(__FUNCTION__);
}

void SV_Frame(int msec)
{
	// UNIMPLEMENTED(__FUNCTION__);
}

void SV_Shutdown( const char* finalmsg )
{
	UNIMPLEMENTED(__FUNCTION__);
}

int SV_CopyReliableCommands(client_t *client)
{
	int sequence;
	int i;

	sequence = client->reliableSent + 1;

	for ( i = client->reliableSent; i < client->reliableSequence + 1; ++i )
	{
		if ( client->reliableCommands[i & (MAX_RELIABLE_COMMANDS - 1)].cmdType )
		{
			if ( (sequence & (MAX_RELIABLE_COMMANDS - 1)) != (i & (MAX_RELIABLE_COMMANDS - 1)) )
			{
				Com_Memcpy(&client->reliableCommands[sequence & (MAX_RELIABLE_COMMANDS - 1)], &client->reliableCommands[i & (MAX_RELIABLE_COMMANDS - 1)], sizeof(reliableCommands_t));
			}
			++sequence;
		}
	}

	client->reliableSequence = sequence - 1;
	return sequence - 1;
}

int SV_GetReliableSequence(client_t *client, const char *command)
{
	int i;

	for ( i = client->reliableSent + 1; i < client->reliableSequence + 1; ++i )
	{
		if ( client->reliableCommands[i & (MAX_RELIABLE_COMMANDS - 1)].cmdType == 0 )
			continue;

		if ( client->reliableCommands[i & (MAX_RELIABLE_COMMANDS - 1)].command[0] != command[0] )
			continue;

		if ( command[0] >= 120 && command[0] <= 122 )
			continue;

		if ( !strcmp(&command[1], &client->reliableCommands[i & (MAX_RELIABLE_COMMANDS - 1)].command[1]) )
			return i;

		switch ( command[0] )
		{
		case 'C':
		case 'D':
		case 'a':
		case 'b':
		case 'o':
		case 'p':
		case 'q':
		case 'r':
		case 't':
			return i;
		case 'd':
		case 'v':
			if ( !I_IsEqualUnitWSpace( (char*)&command[2], &client->reliableCommands[i & (MAX_RELIABLE_COMMANDS - 1)].command[2]) )
				continue;
			return i;
		default:
			continue;
		}
	}

	return -1;
}

void SV_AddServerCommand(client_t *client, int type, const char *cmd)
{
	int sequence;
	int i;
	int j;
	int index;

	if ( client->bot )
		return;

	if ( client->reliableSequence - client->reliableAcknowledge < MAX_RELIABLE_COMMANDS / 2 && client->state == CS_ACTIVE || (SV_CopyReliableCommands(client), type) )
	{
		sequence = SV_GetReliableSequence(client, cmd);

		if ( sequence < 0 )
		{
			++client->reliableSequence;
		}
		else
		{
			for ( i = sequence + 1; i < client->reliableSequence + 1; ++sequence )
			{
				Com_Memcpy(&client->reliableCommands[sequence & (MAX_RELIABLE_COMMANDS - 1)], &client->reliableCommands[i++ & (MAX_RELIABLE_COMMANDS - 1)], sizeof(reliableCommands_t));
			}
		}

		if ( client->reliableSequence - client->reliableAcknowledge == (MAX_RELIABLE_COMMANDS + 1) )
		{
			Com_Printf("Client: %i lost reliable commands\n", client - svs.clients);
			Com_Printf("===== pending server commands =====\n");

			for ( j = client->reliableAcknowledge + 1; j <= client->reliableSequence; ++j )
			{
				Com_Printf("cmd %5d: %8d: %s\n", j, client->reliableCommands[j & (MAX_RELIABLE_COMMANDS - 1)].cmdTime, &client->reliableCommands[j & (MAX_RELIABLE_COMMANDS - 1)].command);
			}

			Com_Printf("cmd %5d: %8d: %s\n", j, svs.time, cmd);

			NET_OutOfBandPrint( NS_SERVER, client->netchan.remoteAddress, "disconnect" );
			SV_DelayDropClient(client, "EXE_SERVERCOMMANDOVERFLOW");

			type = 1;
			cmd = va("%c \"EXE_SERVERCOMMANDOVERFLOW\"", 119);
		}

		index = client->reliableSequence & ( MAX_RELIABLE_COMMANDS - 1 );
		MSG_WriteReliableCommandToBuffer(cmd, client->reliableCommands[ index ].command, sizeof( client->reliableCommands[ index ].command ));

		client->reliableCommands[ index ].cmdTime = svs.time;
		client->reliableCommands[ index ].cmdType = type;
	}
}

char *SV_ExpandNewlines( char *in )
{
	static	char string[1024];
	int l;

	l = 0;

	while ( *in && l < sizeof(string) - 3 )
	{
		if ( *in == '\n' )
		{
			string[l++] = '\\';
			string[l++] = 'n';
		}
		else
		{
			string[l++] = *in;
		}
		in++;
	}

	string[l] = 0;

	return string;
}

void SV_SendServerCommand( client_t *cl, int type, const char *fmt, ... )
{
	va_list argptr;
	byte message[MAX_MSGLEN];
	client_t    *client;
	int j;

	va_start( argptr,fmt );
	Q_vsnprintf( (char *)message, sizeof( message ), fmt, argptr );
	va_end( argptr );

	if ( cl != NULL )
	{
		SV_AddServerCommand( cl, type, (char *)message );
		return;
	}

	// hack to echo broadcast prints to console
	if ( com_dedicated->current.integer && !strncmp( (char *)message, "print", 5 ) )
	{
		Com_Printf( "broadcast: %s\n", SV_ExpandNewlines( (char *)message ) );
	}

	// send the data to all relevent clients
	for ( j = 0, client = svs.clients; j < sv_maxclients->current.integer ; j++, client++ )
	{
		if ( client->state < CS_PRIMED )
		{
			continue;
		}
		// done.
		SV_AddServerCommand( client, type, (char *)message );
	}
}