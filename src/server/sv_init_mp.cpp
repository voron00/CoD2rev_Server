#include "../qcommon/qcommon.h"

#ifdef TESTING_LIBRARY
#define svs (*((serverStatic_t*)( 0x0841FB00 )))
#define sv (*((server_t*)( 0x0842BC80 )))
#else
serverStatic_t svs;
server_t sv;
#endif

#ifdef TESTING_LIBRARY
#define sv_serverId_value (*((int*)( 0x0841FA88 )))
#else
int sv_serverId_value;
#endif

dvar_t *sv_gametype;
dvar_t *sv_mapname;
dvar_t *sv_privateClients;
dvar_t *sv_maxclients;
dvar_t *sv_hostname;
dvar_t *sv_maxRate;
dvar_t *sv_minPing;
dvar_t *sv_maxPing;
dvar_t *sv_floodProtect;
dvar_t *sv_allowAnonymous;
dvar_t *sv_showCommands;
dvar_t *sv_disableClientConsole;
dvar_t *sv_voice;
dvar_t *sv_voiceQuality;
dvar_t *sv_cheats;
dvar_t *sv_serverid;
dvar_t *sv_pure;
dvar_t *sv_iwds;
dvar_t *sv_iwdNames;
dvar_t *sv_referencedIwds;
dvar_t *sv_referencedIwdNames;
dvar_t *rcon_password;
dvar_t *sv_privatePassword;
dvar_t *sv_fps;
dvar_t *sv_timeout;
dvar_t *sv_zombietime;
dvar_t *sv_allowDownload;
dvar_t *sv_reconnectlimit;
dvar_t *sv_padPackets;
dvar_t *sv_allowedClan1;
dvar_t *sv_allowedClan2;
dvar_t *sv_packet_info;
dvar_t *sv_showAverageBPS;
dvar_t *sv_kickBanTime;
dvar_t *sv_mapRotation;
dvar_t *sv_mapRotationCurrent;
dvar_t *sv_debugRate;
dvar_t *sv_debugReliableCmds;
dvar_t *nextmap;
dvar_t *com_expectedHunkUsage;

bool SV_Loaded()
{
	return sv.state == SS_GAME;
}

void SV_AddOperatorCommands()
{
	UNIMPLEMENTED(__FUNCTION__);
}

void SV_Init()
{
	SV_AddOperatorCommands();
	sv_gametype = Dvar_RegisterString("g_gametype", "dm", 0x1024u);
	Dvar_RegisterString("sv_keywords", "", 0x1004u);
	Dvar_RegisterInt("protocol", PROTOCOL_VERSION, PROTOCOL_VERSION, PROTOCOL_VERSION, 0x1044u);
	sv_mapname = Dvar_RegisterString("mapname", "", 0x1044u);
	sv_privateClients = Dvar_RegisterInt("sv_privateClients", 0, 0, 64, 0x1004u);
	sv_maxclients = Dvar_RegisterInt("sv_maxclients", 20, 1, 64, 0x1025u);
	sv_hostname = Dvar_RegisterString("sv_hostname", "CoD2Host", 0x1005u);
	sv_maxRate = Dvar_RegisterInt("sv_maxRate", 0, 0, 25000, 0x1005u);
	sv_minPing = Dvar_RegisterInt("sv_minPing", 0, 0, 999, 0x1005u);
	sv_maxPing = Dvar_RegisterInt("sv_maxPing", 0, 0, 999, 0x1005u);
	sv_floodProtect = Dvar_RegisterBool("sv_floodProtect", 1, 4101);
	sv_allowAnonymous = Dvar_RegisterBool("sv_allowAnonymous", 0, 4100);
	sv_showCommands = Dvar_RegisterBool("sv_showCommands", 0, 4096);
	sv_disableClientConsole = Dvar_RegisterBool("sv_disableClientConsole", 0, 4104);
	sv_voice = Dvar_RegisterBool("sv_voice", 0, 4109);
	sv_voiceQuality = Dvar_RegisterInt("sv_voiceQuality", 1, 0, 9, 0x1008u);
	sv_cheats = Dvar_RegisterBool("sv_cheats", 0, 4120);
	sv_serverid = Dvar_RegisterInt("sv_serverid", 0, 0x80000000, 0x7FFFFFFF, 0x1048u);
	sv_pure = Dvar_RegisterBool("sv_pure", 1, 4108);
	sv_iwds = Dvar_RegisterString("sv_iwds", "", 0x1048u);
	sv_iwdNames = Dvar_RegisterString("sv_iwdNames", "", 0x1048u);
	sv_referencedIwds = Dvar_RegisterString("sv_referencedIwds", "", 0x1048u);
	sv_referencedIwdNames = Dvar_RegisterString("sv_referencedIwdNames", "", 0x1048u);
	rcon_password = Dvar_RegisterString("rcon_password", "", 0x1000u);
	sv_privatePassword = Dvar_RegisterString("sv_privatePassword", "", 0x1000u);
	sv_fps = Dvar_RegisterInt("sv_fps", 20, 10, 1000, 0x1000u);
	sv_timeout = Dvar_RegisterInt("sv_timeout", 240, 0, 1800, 0x1000u);
	sv_zombietime = Dvar_RegisterInt("sv_zombietime", 2, 0, 1800, 0x1000u);
	sv_allowDownload = Dvar_RegisterBool("sv_allowDownload", 1, 4097);
	sv_reconnectlimit = Dvar_RegisterInt("sv_reconnectlimit", 3, 0, 1800, 0x1001u);
	sv_padPackets = Dvar_RegisterInt("sv_padPackets", 0, 0, 0x7FFFFFFF, 0x1000u);
	//*(_BYTE *)(legacyHacks + 221) = 0;
	sv_allowedClan1 = Dvar_RegisterString("sv_allowedClan1", "", 0x1000u);
	sv_allowedClan2 = Dvar_RegisterString("sv_allowedClan2", "", 0x1000u);
	sv_packet_info = Dvar_RegisterBool("sv_packet_info", 0, 4096);
	sv_showAverageBPS = Dvar_RegisterBool("sv_showAverageBPS", 0, 4096);
	sv_kickBanTime = Dvar_RegisterFloat("sv_kickBanTime", 300.0, 0.0, 3600.0, 0x1000u);
	sv_mapRotation = Dvar_RegisterString("sv_mapRotation", "", 0x1000u);
	sv_mapRotationCurrent = Dvar_RegisterString("sv_mapRotationCurrent", "", 0x1000u);
	sv_debugRate = Dvar_RegisterBool("sv_debugRate", 0, 4096);
	sv_debugReliableCmds = Dvar_RegisterBool("sv_debugReliableCmds", 0, 4096);
	nextmap = Dvar_RegisterString("nextmap", "", 0x1000u);
	com_expectedHunkUsage = Dvar_RegisterInt("com_expectedHunkUsage", 0, 0, 0x7FFFFFFF, 0x1040u);
}

void SV_SetConfigstring(unsigned int index, const char *val)
{
	int len, i;
	int maxChunkSize;
	char buf[MAX_STRING_CHARS];
	client_t *client;
	char cmd;

	if ( index < 0 || index >= MAX_CONFIGSTRINGS )
	{
		Com_Error (ERR_DROP, "SV_SetConfigstring: bad index %i\n", index);
	}

	if ( !val )
	{
		val = "";
	}

	if ( !sv.configstrings[ index ] )
		return;

	// don't bother broadcasting an update if no change
	if ( !strcmp( val, sv.configstrings[ index ] ) )
	{
		return;
	}

	ReplaceString((char **)&sv.configstrings[ index ], (char *)val);

	// send it to all the clients if we aren't
	// spawning a new server
	if ( sv.state != SS_GAME && !sv.restarting )
	{
		return;
	}

	len = strlen( val );
	sprintf(buf, "%i", index);
	maxChunkSize = MAX_STRING_CHARS - 4 - strlen(buf);

	for ( i = 0, client = svs.clients; i < sv_maxclients->current.integer ; i++, client++ )
	{
		if ( client->state < CS_PRIMED )
		{
			continue;
		}

		if ( len >= maxChunkSize )
		{
			int sent = 0;
			int remaining = len;

			while ( remaining > 0 )
			{
				if ( sent == 0 )
				{
					cmd = 'x';
				}
				else if ( remaining < maxChunkSize )
				{
					cmd = 'z';
				}
				else
				{
					cmd = 'y';
				}

				Q_strncpyz( buf, &val[sent], maxChunkSize );
				SV_SendServerCommand( client, 1, "%c %i %s", cmd, index, buf );

				sent += ( maxChunkSize - 1 );
				remaining -= ( maxChunkSize - 1 );
			}
		}
		else
		{
			// standard cs, just send it
			cmd = 'd';
			SV_SendServerCommand( client, 1, "%c %i %s", cmd, index, val );
		}
	}
}

void SV_SetConfigValueForKey(int start, int max, const char *key, const char *value)
{
	char *configstring;
	int i;

	for ( i = 0; i < max; ++i )
	{
		configstring = (&sv.configstrings[start])[i];

		if ( !*configstring )
		{
			SV_SetConfigstring(start + i, key);
			break;
		}

		if ( !strcasecmp(key, configstring) )
			break;
	}

	if ( i == max )
		Com_Error(ERR_DROP, "\x15SV_SetConfigValueForKey: overflow");

	SV_SetConfigstring(i + start + max, value);
}

void SV_GetConfigstring( int index, char *buffer, int bufferSize )
{
	if ( bufferSize < 1 )
	{
		Com_Error( ERR_DROP, "SV_GetConfigstring: bufferSize == %i", bufferSize );
	}

	if ( index < 0 || index >= MAX_CONFIGSTRINGS )
	{
		Com_Error( ERR_DROP, "SV_GetConfigstring: bad index %i\n", index );
	}

	if ( !sv.configstrings[index] )
	{
		buffer[0] = 0;
		return;
	}

	Q_strncpyz( buffer, sv.configstrings[index], bufferSize );
}

const char* SV_GetConfigstringConst(int index)
{
	const char *configstring;

	configstring = sv.configstrings[index];

	if ( !configstring )
		return "";

	return configstring;
}

void SV_GetUserinfo( int index, char *buffer, int bufferSize )
{
	if ( bufferSize < 1 )
	{
		Com_Error( ERR_DROP, "SV_GetUserinfo: bufferSize == %i", bufferSize );
	}

	if ( index < 0 || index >= sv_maxclients->current.integer )
	{
		Com_Error( ERR_DROP, "SV_GetUserinfo: bad index %i\n", index );
	}

	Q_strncpyz( buffer, svs.clients[ index ].userinfo, bufferSize );
}

void SV_SetUserinfo( int index, const char *val )
{
	if ( index < 0 || index >= sv_maxclients->current.integer )
	{
		Com_Error( ERR_DROP, "SV_SetUserinfo: bad index %i\n", index );
	}

	if ( !val )
	{
		val = "";
	}

	Q_strncpyz( svs.clients[index].userinfo, val, sizeof( svs.clients[ index ].userinfo ) );
	Q_strncpyz( svs.clients[index].name, Info_ValueForKey( val, "name" ), sizeof( svs.clients[index].name ) );
}