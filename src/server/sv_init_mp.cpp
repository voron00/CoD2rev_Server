#include "../qcommon/qcommon.h"

serverStatic_t svs;
server_t sv;

int sv_serverId_value;

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

#if PROTOCOL_VERSION > 115
dvar_t *sv_wwwDownload;
dvar_t *sv_wwwBaseURL;
dvar_t *sv_wwwDlDisconnected;
#endif

dvar_t *sv_master[MAX_MASTER_SERVERS];     // master server ip address

#ifdef LIBCOD
dvar_t *sv_allowRcon;
dvar_t *sv_downloadMessage;
dvar_t *sv_cracked;
#endif

bool SV_Loaded()
{
	return sv.state == SS_GAME;
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

#if PROTOCOL_VERSION > 115
	sv_wwwDownload = Dvar_RegisterBool("sv_wwwDownload", 0, 4097);
	sv_wwwBaseURL = Dvar_RegisterString("sv_wwwBaseURL", "", 4097);
	sv_wwwDlDisconnected = Dvar_RegisterBool("sv_wwwDlDisconnected", 0, 4097);
#endif

	sv_master[0] = Dvar_RegisterString("sv_master1", MASTER_SERVER_NAME, DVAR_ARCHIVE);
	sv_master[1] = Dvar_RegisterString("sv_master2", "", DVAR_ARCHIVE);
	sv_master[2] = Dvar_RegisterString("sv_master3", "", DVAR_ARCHIVE);
	sv_master[3] = Dvar_RegisterString("sv_master4", "", DVAR_ARCHIVE);
	sv_master[4] = Dvar_RegisterString("sv_master5", "", DVAR_ARCHIVE);

#ifdef LIBCOD
	sv_allowRcon = Dvar_RegisterBool("sv_allowRcon", qtrue, DVAR_ARCHIVE);
	sv_downloadMessage = Dvar_RegisterString("sv_downloadMessage", "", DVAR_ARCHIVE);
	sv_cracked = Dvar_RegisterBool("sv_cracked", qfalse, DVAR_ARCHIVE);
#endif
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

void SV_BoundMaxClients(int minimum)
{
	sv_maxclients = Dvar_RegisterInt("sv_maxclients", 20, 1, 64, 0x1025u);
	Dvar_ClearModified(sv_maxclients);

	if ( sv_maxclients->current.integer < minimum )
		Dvar_SetInt(sv_maxclients, minimum);
}

/*
==================
SV_ChangeMaxClients
==================
*/
void SV_ChangeMaxClients( void )
{
	int oldMaxClients;
	int i;
	client_t    *oldClients;
	int count;

	// get the highest client number in use
	count = 0;
	for ( i = 0 ; i < sv_maxclients->current.integer ; i++ )
	{
		if ( svs.clients[i].state >= CS_CONNECTED )
		{
			if ( i > count )
			{
				count = i;
			}
		}
	}
	count++;

	oldMaxClients = sv_maxclients->current.integer;
	// never go below the highest client number in use
	SV_BoundMaxClients( count );
	// if still the same
	if ( sv_maxclients->current.integer == oldMaxClients )
	{
		return;
	}

	oldClients = (client_t *)Hunk_AllocateTempMemory( count * sizeof( client_t ) );
	// copy the clients to hunk memory
	for ( i = 0 ; i < count ; i++ )
	{
		if ( svs.clients[i].state >= CS_CONNECTED )
		{
			oldClients[i] = svs.clients[i];
		}
		else
		{
			Com_Memset( &oldClients[i], 0, sizeof( client_t ) );
		}
	}

	// free old clients arrays
	//Z_Free( svs.clients );
	free( svs.clients );    // RF, avoid trying to allocate large chunk on a fragmented zone

	// allocate new clients
	// RF, avoid trying to allocate large chunk on a fragmented zone
	svs.clients = (client_t *)malloc( sizeof( client_t ) * sv_maxclients->current.integer );
	if ( !svs.clients )
	{
		Com_Error( ERR_FATAL, "SV_Startup: unable to allocate svs.clients" );
	}

	Com_Memset( svs.clients, 0, sv_maxclients->current.integer * sizeof( client_t ) );

	// copy the clients over
	for ( i = 0 ; i < count ; i++ )
	{
		if ( oldClients[i].state >= CS_CONNECTED )
		{
			svs.clients[i] = oldClients[i];
		}
	}

	// free the old clients on the hunk
	Hunk_FreeTempMemory( oldClients );

	// allocate new snapshot entities
	if ( com_dedicated->current.integer )
	{
		svs.numSnapshotEntities = sv_maxclients->current.integer * PACKET_BACKUP * 64;
		svs.numSnapshotClients = PACKET_BACKUP * sv_maxclients->current.integer * sv_maxclients->current.integer;
	}
	else
	{
		// we don't need nearly as many when playing locally
		svs.numSnapshotEntities = sv_maxclients->current.integer * 4 * 64;
		svs.numSnapshotClients = 4 * sv_maxclients->current.integer * sv_maxclients->current.integer;
	}
}

/*
================
SV_ClearServer
================
*/
void SV_ClearServer( void )
{
	int i;

	for ( i = 0 ; i < MAX_CONFIGSTRINGS ; i++ )
	{
		if ( sv.configstrings[i] )
		{
			Z_Free( sv.configstrings[i] );
		}
	}
	Com_Memset( &sv, 0, sizeof( sv ) );
}

/*
===============
SV_Startup
Called when a host starts a map when it wasn't running
one before.  Successive map or map_restart commands will
NOT cause this to be called, unless the game is exited to
the menu system first.
===============
*/
extern dvar_t *com_sv_running;
void SV_Startup( void )
{
	if ( svs.initialized )
	{
		Com_Error( ERR_FATAL, "SV_Startup: svs.initialized" );
	}

	SV_BoundMaxClients( 1 );

	// RF, avoid trying to allocate large chunk on a fragmented zone
	svs.clients = (client_t *)malloc( sizeof( client_t ) * sv_maxclients->current.integer );
	if ( !svs.clients )
	{
		Com_Error( ERR_FATAL, "SV_Startup: unable to allocate svs.clients" );
	}

	if ( com_dedicated->current.integer )
	{
		svs.numSnapshotEntities = sv_maxclients->current.integer * PACKET_BACKUP * 64;
		svs.numSnapshotClients = PACKET_BACKUP * sv_maxclients->current.integer * sv_maxclients->current.integer;
	}
	else
	{
		// we don't need nearly as many when playing locally
		svs.numSnapshotEntities = sv_maxclients->current.integer * 4 * 64;
		svs.numSnapshotClients = 4 * sv_maxclients->current.integer * sv_maxclients->current.integer;
	}
	svs.initialized = qtrue;

	Dvar_SetBool( com_sv_running, 1 );
}

/*
====================
SV_SetExpectedHunkUsage
  Sets com_expectedhunkusage, so the client knows how to draw the percentage bar
====================
*/
void SV_SetExpectedHunkUsage( char *mapname )
{
	int handle;
	const char *memlistfile = "hunkusage.dat";
	char *buf;
	const char *buftrav;
	char *token;
	int len;

	len = FS_FOpenFileByMode( memlistfile, &handle, FS_READ );
	if ( len >= 0 )   // the file exists, so read it in, strip out the current entry for this map, and save it out, so we can append the new value
	{

		buf = (char *)Z_Malloc( len + 1 );
		memset( buf, 0, len + 1 );

		FS_Read( (void *)buf, len, handle );
		FS_FCloseFile( handle );

		// now parse the file, filtering out the current map
		buftrav = buf;
		while ( ( token = Com_Parse( &buftrav ) ) != NULL && token[0] )
		{
			if ( !Q_stricmp( token, mapname ) )
			{
				// found a match
				token = Com_Parse( &buftrav );  // read the size
				if ( token && token[0] )
				{
					// this is the usage
					Dvar_SetInt(com_expectedHunkUsage, atoi( token ));
					Z_Free( buf );
					return;
				}
			}
		}

		Z_Free( buf );
	}

	// just set it to a negative number,so the cgame knows not to draw the percent bar
	//com_expectedhunkusage = -1;
}

void SV_FinalMessage(const char *message)
{
	client_s *client;
	int i;
	int j;

	for ( i = 0; i < 2; ++i )
	{
		j = 0;
		client = svs.clients;

		while ( j < sv_maxclients->current.integer )
		{
			if ( client->state > CS_ZOMBIE )
			{
				if ( client->netchan.remoteAddress.type != NA_LOOPBACK )
				{
					SV_SendServerCommand(client, 0, "%c \"%s\"", 101, message);
					SV_SendServerCommand(client, 1, "%c \"%s\"", 119, message);
				}

				client->nextSnapshotTime = -1;
				SV_SendClientSnapshot(client);
			}

			++j;
			++client;
		}
	}
}

void SV_DropAllClients()
{
	client_s *client;
	int i;

	i = 0;
	client = svs.clients;

	while ( i < sv_maxclients->current.integer )
	{
		if ( client->state > CS_ZOMBIE )
			SV_DropClient(client, "EXE_DISCONNECTED");

		++i;
		++client;
	}
}

void SV_FreeClients()
{
	client_s *client;
	int i;

	i = 0;
	client = svs.clients;

	while ( i < sv_maxclients->current.integer )
	{
		if ( client->state > CS_ZOMBIE )
			SV_FreeClient(client);

		++i;
		++client;
	}

	Z_Free(svs.clients);
}


void SV_Shutdown(const char *finalmsg)
{
	bool loading;

	if ( com_sv_running && com_sv_running->current.boolean )
	{
		Com_Printf("----- Server Shutdown -----\n");
		loading = sv.state == SS_LOADING;

		if ( svs.clients )
			SV_FinalMessage(finalmsg);

		SV_RemoveOperatorCommands();
		SV_MasterShutdown();
		SV_ShutdownGameProgs();
		SV_DropAllClients();
		SV_ClearServer();

		if ( svs.clients )
			SV_FreeClients();

		SV_ShutdownArchivedSnapshot();
		memset(&svs, 0, sizeof(svs));
#ifndef DEDICATED
		if ( com_dedicated->current.integer )
			FX_FreeSystem();
#endif
		Dvar_SetBool(com_sv_running, 0);
		Com_Printf("---------------------------\n");

		if ( loading )
		{
			Com_AbortDObj();
			DObjAbort();
			XAnimAbort();
			Scr_Abort();
			bgs = 0;
		}
	}
}

void SV_SaveSystemInfo()
{
	char info[8192];

	I_strncpyz(info, Dvar_InfoString_Big(8u), 0x2000);
	dvar_modifiedFlags &= ~8u;
	SV_SetConfigstring(1u, info);
	SV_SetConfigstring(0, Dvar_InfoString(0x404u));
	dvar_modifiedFlags &= 0xFFFFFBFB;
	SV_SetConfig(142, 96, 0x100u);
	dvar_modifiedFlags &= ~0x100u;
}

extern dvar_t *cl_paused;
void SV_SpawnServer(char *server)
{
	char mapname[64];
	int persist;
	client_t *cl;
	const char* dropreason;
	const char* iwdChecksums;
	const char* iwdNames;
	int checksum;
	int index;
	int i;

#if COMPILE_SQLITE == 1
	free_sqlite_db_stores_and_tasks();
#endif

	Scr_ParseGameTypeList();
	SV_SetGametype();

	if ( com_sv_running->current.boolean )
	{
		persist = G_GetSavePersist();
		index = 0;
		cl = svs.clients;

		while ( index < sv_maxclients->current.integer )
		{
			if ( cl->state > CS_CONNECTED )
			{
				Com_sprintf(mapname, sizeof(mapname), "loadingnewmap\n%s\n%s", server, sv_gametype->current.string);
				NET_OutOfBandPrint(NS_SERVER, cl->netchan.remoteAddress, mapname);
			}

			++index;
			++cl;
		}

		NET_Sleep(250);
	}
	else
	{
		persist = 0;
	}

	Dvar_SetStringByName("mapname", server);
	SV_ShutdownGameProgs();
	Com_Printf("------ Server Initialization ------\n");
	Com_Printf("Server: %s\n", server);
	SV_ClearServer();

#ifndef DEDICATED
	if ( com_dedicated->current.integer )
		FX_FreeSystem();
#endif

	FS_Shutdown();
	FS_ClearIwdReferences();
	Com_Restart();

	if ( com_sv_running->current.boolean )
		SV_ChangeMaxClients();
	else
		SV_Startup();

	I_strncpyz(sv.gametype, sv_gametype->current.string, 64);

	srand(Sys_MillisecondsRaw());
	sv.checksumFeed = rand() ^ rand() << 16 ^ Sys_MilliSeconds();

	FS_Restart(sv.checksumFeed);
	Com_sprintf(mapname, 64, "maps/mp/%s.%s", server, GetBspExtension());
	SV_SetExpectedHunkUsage(mapname);

	for ( index = 0; index < MAX_CONFIGSTRINGS; ++index )
	{
		sv.configstrings[index] = CopyStringInternal("");
	}

	Dvar_ResetScriptInfo();

	svs.snapshotEntities = (entityState_t *)Hunk_AllocInternal(sizeof(entityState_t) * svs.numSnapshotEntities);
	svs.nextSnapshotEntities = 0;
	svs.snapshotClients = (clientState_t *)Hunk_AllocInternal(sizeof(clientState_t) * svs.numSnapshotClients);
	svs.nextSnapshotClients = 0;

	SV_InitArchivedSnapshot();

	svs.snapFlagServerBit ^= 4u;
	Dvar_SetString(nextmap, "map_restart");
	Dvar_SetInt(cl_paused, 0);

	Com_sprintf(mapname, 64, "maps/mp/%s.%s", server, GetBspExtension());

	Com_LoadBsp(mapname);
	CM_LoadMap(mapname, &checksum);
	Com_UnloadBsp();
	CM_LinkWorld();

	sv_serverId_value = (byte)(sv_serverId_value + 16);

	if ( (sv_serverId_value & 0xF0) == 0 )
		sv_serverId_value += 16;

	Dvar_SetInt(sv_serverid, sv_serverId_value);
	sv.start_frameTime = com_frameTime;
	sv.state = SS_LOADING;

#ifndef DEDICATED
	Com_sprintf(mapname, 64, "maps/mp/%s.%s", server, GetBspExtension());
	Com_LoadSoundAliases(mapname, "all_mp", SASYS_GAME);
#endif

	SV_InitGameProgs(persist);

	for(i = 0; i < 3; ++i)
	{
		svs.time += 100;
		SV_RunFrame();
	}

	SV_CreateBaseline();

	for(i = 0, cl = svs.clients; i < sv_maxclients->current.integer; ++i, ++cl)
	{
		if ( cl->state < CS_CONNECTED )
		{
			continue;
		}

		if (cl->bot)
		{
			SV_DropClient(cl, "EXE_DISCONNECTED");
			continue;
		}

		dropreason = ClientConnect(i, cl->clscriptid);

		if ( dropreason )
		{
			SV_DropClient(cl, dropreason);
			Com_Printf("SV_SpawnServer: dropped client %i - denied!\n", i);
		}
		else
		{
			cl->state = CS_CONNECTED;
		}
	}

	if ( sv_pure->current.boolean )
	{
		iwdChecksums = FS_LoadedIwdChecksums();
		Dvar_SetString(sv_iwds, iwdChecksums);

		if ( !*iwdChecksums )
			Com_Printf("WARNING: sv_pure set but no IWD files loaded\n");

		iwdNames = FS_LoadedIwdNames();
		Dvar_SetString(sv_iwdNames, iwdNames);
	}
	else
	{
		Dvar_SetString(sv_iwds, "");
		Dvar_SetString(sv_iwdNames, "");
	}

	Dvar_SetString(sv_referencedIwds, FS_ReferencedIwdChecksums());
	Dvar_SetString(sv_referencedIwdNames, FS_ReferencedIwdNames());

	SV_SaveSystemInfo();

	sv.state = SS_GAME;
	SV_Heartbeat_f();
	Com_Printf("-----------------------------------\n");
}