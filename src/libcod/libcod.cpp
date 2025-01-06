#include "gsc.hpp"

#ifdef LIBCOD

dvar_t *sv_master[MAX_MASTER_SERVERS];
dvar_t *fs_library;
dvar_t *g_playerCollision;
dvar_t *g_playerEject;
dvar_t *sv_allowRcon;
dvar_t *sv_downloadMessage;
dvar_t *sv_fastDownload;
dvar_t *sv_cracked;
dvar_t *sv_kickbots;
dvar_t *jump_bounceEnable;
dvar_t *g_mantleBlockEnable;
dvar_t *g_fixedWeaponSpreads;
dvar_t *g_dropGrenadeOnDeath;

int codecallback_playercommand = 0;
int codecallback_userinfochanged = 0;
int codecallback_fire_grenade = 0;
int codecallback_vid_restart = 0;

void RegisterLibcodDvars()
{
	sv_master[0] = Dvar_RegisterString("sv_master1", MASTER_SERVER_NAME, DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);
	sv_master[1] = Dvar_RegisterString("sv_master2", "", DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);
	sv_master[2] = Dvar_RegisterString("sv_master3", "", DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);
	sv_master[3] = Dvar_RegisterString("sv_master4", "", DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);
	sv_master[4] = Dvar_RegisterString("sv_master5", "", DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);

	fs_library = Dvar_RegisterString("fs_library", "", DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);

	g_playerCollision = Dvar_RegisterBool("g_playerCollision", true, DVAR_CHANGEABLE_RESET);
	g_playerEject = Dvar_RegisterBool("g_playerEject", true, DVAR_CHANGEABLE_RESET);

	sv_allowRcon = Dvar_RegisterBool("sv_allowRcon", true, DVAR_CHANGEABLE_RESET);
	sv_downloadMessage = Dvar_RegisterString("sv_downloadMessage", "", DVAR_CHANGEABLE_RESET);
	sv_fastDownload = Dvar_RegisterBool("sv_fastDownload", false, DVAR_CHANGEABLE_RESET);
	sv_cracked = Dvar_RegisterBool("sv_cracked", false, DVAR_ARCHIVE | DVAR_CHANGEABLE_RESET);
	sv_kickbots = Dvar_RegisterBool("sv_kickbots", false, DVAR_CHANGEABLE_RESET);

	jump_bounceEnable = Dvar_RegisterBool("jump_bounceEnable", false, DVAR_CHEAT | DVAR_CODINFO | DVAR_CHANGEABLE_RESET);
	g_mantleBlockEnable = Dvar_RegisterBool("g_mantleBlockEnable", true, DVAR_CHANGEABLE_RESET);

	g_fixedWeaponSpreads = Dvar_RegisterBool("g_fixedWeaponSpreads", false, DVAR_CHANGEABLE_RESET);
	g_dropGrenadeOnDeath = Dvar_RegisterBool("g_dropGrenadeOnDeath", true, DVAR_CHANGEABLE_RESET);
}

void InitLibcodCallbacks()
{
	codecallback_playercommand = Scr_GetFunctionHandle("maps/mp/gametypes/_callbacksetup", "CodeCallback_PlayerCommand", 0);
	codecallback_userinfochanged = Scr_GetFunctionHandle("maps/mp/gametypes/_callbacksetup", "CodeCallback_UserInfoChanged", 0);
	codecallback_fire_grenade = Scr_GetFunctionHandle("maps/mp/gametypes/_callbacksetup", "CodeCallback_FireGrenade", 0);
	codecallback_vid_restart = Scr_GetFunctionHandle("maps/mp/gametypes/_callbacksetup", "CodeCallback_VidRestart", 0);
}

static void Com_Assert_f( void )
{
	assert(0);
}

void SV_AddLibcodCommands()
{
	if ( com_developer->current.integer )
	{
		Cmd_AddCommand("assert", Com_Assert_f);
	}
}

const char* NET_AdrToStringNoPort( netadr_t a )
{
	static char s[64];

	if ( a.type == NA_LOOPBACK )
	{
		Com_sprintf( s, sizeof( s ), "loopback" );
	}
	else if ( a.type == NA_BOT )
	{
		Com_sprintf( s, sizeof( s ), "bot" );
	}
	else if ( a.type == NA_IP )
	{
		Com_sprintf( s, sizeof( s ), "%i.%i.%i.%i",
		             a.ip[0], a.ip[1], a.ip[2], a.ip[3] );
	}
	else
	{
		Com_sprintf( s, sizeof( s ), "%02x%02x%02x%02x.%02x%02x%02x%02x%02x%02x",
		             a.ipx[0], a.ipx[1], a.ipx[2], a.ipx[3], a.ipx[4], a.ipx[5], a.ipx[6], a.ipx[7], a.ipx[8], a.ipx[9] );
	}

	return s;
}

void manymaps_prepare(const char *mapname, int read)
{
	char map_check[MAX_OSPATH];
	char library_path[MAX_OSPATH];

	dvar_t *fs_homepath = Dvar_FindVar("fs_homepath");
	dvar_t *fs_game = Dvar_FindVar("fs_game");
	dvar_t *map = Dvar_FindVar("mapname");

	if (strlen(fs_library->current.string))
		strncpy(library_path, fs_library->current.string, sizeof(library_path));
	else
		snprintf(library_path, sizeof(library_path), "%s/%s/Library", fs_homepath->current.string, fs_game->current.string);

	Com_sprintf(map_check, MAX_OSPATH, "%s/%s.iwd", library_path, mapname);

#if PROTOCOL_VERSION == 115
	const char *stock_maps[] = { "mp_breakout", "mp_brecourt", "mp_burgundy", "mp_carentan", "mp_dawnville", "mp_decoy", "mp_downtown", "mp_farmhouse", "mp_leningrad", "mp_matmata", "mp_railyard", "mp_toujane", "mp_trainstation" };
#else
	const char *stock_maps[] = { "mp_breakout", "mp_brecourt", "mp_burgundy", "mp_carentan", "mp_dawnville", "mp_decoy", "mp_downtown", "mp_farmhouse", "mp_leningrad", "mp_matmata", "mp_railyard", "mp_toujane", "mp_trainstation", "mp_rhine", "mp_harbor" };
#endif

	bool map_found = false, from_stock_map = false;

	for (int i = 0; i < int( sizeof(stock_maps) / sizeof(stock_maps[0]) ); i++)
	{
		if (strcmp(map->current.string, stock_maps[i]) == 0)
		{
			from_stock_map = true;
			break;
		}
	}

	for (int i = 0; i < int( sizeof(stock_maps) / sizeof(stock_maps[0]) ); i++)
	{
		if (strcmp(mapname, stock_maps[i]) == 0)
		{
			map_found = true;

			if (from_stock_map) // When changing from stock map to stock map do not trigger manymap
				return;
			else
				break;
		}
	}

	int map_exists = access(map_check, F_OK) != -1;

	if (!map_exists && !map_found)
		return;

	DIR *dir;
	struct dirent *dir_ent;

	dir = opendir(library_path);

	if (!dir)
		return;

	while ((dir_ent = readdir(dir)) != NULL)
	{
		if (strcmp(dir_ent->d_name, ".") == 0 || strcmp(dir_ent->d_name, "..") == 0)
			continue;

		char fileDelete[MAX_OSPATH];
		Com_sprintf(fileDelete, MAX_OSPATH, "%s/%s/%s", fs_homepath->current.string, fs_game->current.string, dir_ent->d_name);

		if (access(fileDelete, F_OK) != -1)
		{
#ifdef _WIN32
			char delcmd[COD2_MAX_STRINGLENGTH];
			Com_sprintf(delcmd, sizeof(delcmd), "del /F %s", fileDelete);
			int unlink_success = system(delcmd) == 0;
#else
			int unlink_success = unlink(fileDelete) == 0;
#endif
			Com_Printf("manymaps> REMOVED OLD LINK: %s result of unlink: %s\n", fileDelete, unlink_success?"success":"failed");
		}
	}

	closedir(dir);

	if (map_exists)
	{
		char src[MAX_OSPATH];
		char dst[MAX_OSPATH];

		Com_sprintf(src, MAX_OSPATH, "%s/%s.iwd", library_path, mapname);
		Com_sprintf(dst, MAX_OSPATH, "%s/%s/%s.iwd", fs_homepath->current.string, fs_game->current.string, mapname);

		if (access(src, F_OK) != -1)
		{
#ifdef _WIN32
			char linkcmd[COD2_MAX_STRINGLENGTH];
			Com_sprintf(linkcmd, sizeof(linkcmd), "mklink /h %s %s", dst, src);
			int link_success = system(linkcmd) == 0;
#else
			int link_success = symlink(src, dst) == 0;
#endif
			Com_Printf("manymaps> NEW LINK: src=%s dst=%s result of link: %s\n", src, dst, link_success?"success":"failed");

			if (link_success && read == -1) // FS_LoadDir is needed when empty.iwd is missing (then .d3dbsp isn't referenced anywhere)
				FS_AddIwdFilesForGameDirectory(fs_homepath->current.string, fs_game->current.string);
		}
	}
}

// ioquake3 rate limit connectionless requests
// https://github.com/ioquake/ioq3/blob/master/code/server/sv_main.c
// This is deliberately quite large to make it more of an effort to DoS
#define MAX_BUCKETS	16384
#define MAX_HASHES 1024

static leakyBucket_t buckets[ MAX_BUCKETS ];
static leakyBucket_t* bucketHashes[ MAX_HASHES ];
leakyBucket_t outboundLeakyBucket;

static long SVC_HashForAddress( netadr_t address )
{
	unsigned char *ip = address.ip;
	int	i;
	long hash = 0;

	for ( i = 0; i < 4; i++ )
	{
		hash += (long)( ip[ i ] ) * ( i + 119 );
	}

	hash = ( hash ^ ( hash >> 10 ) ^ ( hash >> 20 ) );
	hash &= ( MAX_HASHES - 1 );

	return hash;
}

static leakyBucket_t *SVC_BucketForAddress( netadr_t address, int burst, int period )
{
	leakyBucket_t *bucket = NULL;
	int	i;
	long hash = SVC_HashForAddress( address );
	uint64_t now = Sys_Milliseconds64();

	for ( bucket = bucketHashes[ hash ]; bucket; bucket = bucket->next )
	{
		if ( memcmp( bucket->adr, address.ip, 4 ) == 0 )
		{
			return bucket;
		}
	}

	for ( i = 0; i < MAX_BUCKETS; i++ )
	{
		int interval;

		bucket = &buckets[ i ];
		interval = now - bucket->lastTime;

		// Reclaim expired buckets
		if ( bucket->lastTime > 0 && ( interval > ( burst * period ) ||
		                               interval < 0 ) )
		{
			if ( bucket->prev != NULL )
			{
				bucket->prev->next = bucket->next;
			}
			else
			{
				bucketHashes[ bucket->hash ] = bucket->next;
			}

			if ( bucket->next != NULL )
			{
				bucket->next->prev = bucket->prev;
			}

			memset( bucket, 0, sizeof( leakyBucket_t ) );
		}

		if ( bucket->type == 0 )
		{
			bucket->type = address.type;
			memcpy( bucket->adr, address.ip, 4 );

			bucket->lastTime = now;
			bucket->burst = 0;
			bucket->hash = hash;

			// Add to the head of the relevant hash chain
			bucket->next = bucketHashes[ hash ];
			if ( bucketHashes[ hash ] != NULL )
			{
				bucketHashes[ hash ]->prev = bucket;
			}

			bucket->prev = NULL;
			bucketHashes[ hash ] = bucket;

			return bucket;
		}
	}

	// Couldn't allocate a bucket for this address
	return NULL;
}

bool SVC_RateLimit( leakyBucket_t *bucket, int burst, int period )
{
	if ( bucket != NULL )
	{
		uint64_t now = Sys_Milliseconds64();
		int interval = now - bucket->lastTime;
		int expired = interval / period;
		int expiredRemainder = interval % period;

		if ( expired > bucket->burst || interval < 0 )
		{
			bucket->burst = 0;
			bucket->lastTime = now;
		}
		else
		{
			bucket->burst -= expired;
			bucket->lastTime = now - expiredRemainder;
		}

		if ( bucket->burst < burst )
		{
			bucket->burst++;

			return false;
		}
	}

	return true;
}

bool SVC_RateLimitAddress( netadr_t from, int burst, int period )
{
	if (Sys_IsLANAddress(from))
		return false;

	leakyBucket_t *bucket = SVC_BucketForAddress( from, burst, period );

	return SVC_RateLimit( bucket, burst, period );
}

int hook_findMap(const char *qpath, void **buffer)
{
	int read = FS_ReadFile(qpath, buffer);
	manymaps_prepare(Cmd_Argv(1), read);

	if (read != -1)
		return read;
	else
		return FS_ReadFile(qpath, buffer);
}

bool hook_SV_MapExists(const char *mapname)
{
	bool map_exists = SV_MapExists(mapname);

	if (map_exists)
	{
		return map_exists;
	}
	else
	{
		char map_check[MAX_OSPATH];
		char library_path[MAX_OSPATH];

		dvar_t *fs_homepath = Dvar_FindVar("fs_homepath");
		dvar_t *fs_game = Dvar_FindVar("fs_game");

		if (strlen(fs_library->current.string))
			strncpy(library_path, fs_library->current.string, sizeof(library_path));
		else
			snprintf(library_path, sizeof(library_path), "%s/%s/Library", fs_homepath->current.string, fs_game->current.string);

		Com_sprintf(map_check, MAX_OSPATH, "%s/%s.iwd", library_path, mapname);

		return access(map_check, F_OK) != -1;
	}
}

#define IP_BAN_LIST_NAME "iplist.txt"
bool SV_IsBannedIp(netadr_t adr)
{
	bool banned;
	char *file;
	const char *token;
	const char *text;

	if ( FS_ReadFile(IP_BAN_LIST_NAME, (void **)&file) < 0 )
		return false;

	text = file;
	banned = false;

	while ( 1 )
	{
		token = Com_Parse(&text);

		if ( !token[0] )
			break;

		if ( strcmp(token, NET_AdrToStringNoPort(adr)) == 0 )
		{
			banned = true;
			break;
		}

		Com_SkipRestOfLine(&text);
	}

	FS_FreeFile(file);

	return banned;
}

int clientfps[MAX_CLIENTS] = {0};
int clientframes[MAX_CLIENTS] = {0};
uint64_t clientframetime[MAX_CLIENTS] = {0};
void SV_ClientThink_libcod(client_t *cl, usercmd_t *cmd)
{
	cl->lastUsercmd = *cmd;

	if ( cl->state != CS_ACTIVE )
	{
		return;     // may have been kicked during the last usercmd
	}

	int clientnum = cl - svs.clients;
	clientframes[clientnum]++;

	if (Sys_Milliseconds64() - clientframetime[clientnum] >= 1000)
	{
		if (clientframes[clientnum] > 1000)
			clientframes[clientnum] = 1000;

		clientfps[clientnum] = clientframes[clientnum];
		clientframetime[clientnum] = Sys_Milliseconds64();
		clientframes[clientnum] = 0;
	}

	G_SetLastServerTime(cl - svs.clients, cmd->serverTime);
	ClientThink(cl - svs.clients);
}

int bot_buttons[MAX_CLIENTS] = {0};
int bot_weapon[MAX_CLIENTS] = {0};
char bot_forwardmove[MAX_CLIENTS] = {0};
char bot_rightmove[MAX_CLIENTS] = {0};
void SV_BotUserMove_libcod(client_t *client)
{
	int num;
	usercmd_t ucmd = {0};

	if (client->gentity == NULL)
		return;

	num = client - svs.clients;
	ucmd.serverTime = svs.time;

	playerState_t *ps = SV_GameClientNum(num);
	gentity_t *ent = SV_GentityNum(num);

	if (bot_weapon[num])
		ucmd.weapon = (byte)(bot_weapon[num] & 0xFF);
	else
		ucmd.weapon = (byte)(ps->weapon & 0xFF);

	if (ent->client == NULL)
		return;

	if (ent->client->sess.archiveTime == 0)
	{
		ucmd.buttons = bot_buttons[num];

		ucmd.forwardmove = bot_forwardmove[num];
		ucmd.rightmove = bot_rightmove[num];

		ucmd.angles[0] = ent->client->sess.cmd.angles[0];
		ucmd.angles[1] = ent->client->sess.cmd.angles[1];
		ucmd.angles[2] = ent->client->sess.cmd.angles[2];
	}

	client->deltaMessage = client->netchan.outgoingSequence - 1;
	SV_ClientThink_libcod(client, &ucmd);
}

void SV_ResetPureClient_libcod(client_t *cl)
{
	cl->pureAuthentic = 0;

	if (codecallback_vid_restart)
	{
		if (!Scr_IsSystemActive())
			return;

		if (cl->gentity == NULL)
			return;

		stackPushInt(cl - svs.clients);
		short ret = Scr_ExecEntThread(cl->gentity, codecallback_vid_restart, 1);
		Scr_FreeThread(ret);
	}
}

void hook_ClientCommand(int clientNum)
{
	if ( ! codecallback_playercommand)
	{
		ClientCommand(clientNum);
		return;
	}

	if (!Scr_IsSystemActive())
		return;

	stackPushArray();
	int args = Cmd_Argc();
	for (int i = 0; i < args; i++)
	{
		char tmp[COD2_MAX_STRINGLENGTH];
		SV_Cmd_ArgvBuffer(i, tmp, sizeof(tmp));
		if(i == 1 && tmp[0] >= 20 && tmp[0] <= 22)
		{
			char *part = strtok(tmp + 1, " ");
			while(part != NULL)
			{
				stackPushString(part);
				stackPushArrayLast();
				part = strtok(NULL, " ");
			}
		}
		else
		{
			stackPushString(tmp);
			stackPushArrayLast();
		}
	}

	short ret = Scr_ExecEntThread(&g_entities[clientNum], codecallback_playercommand, 1);
	Scr_FreeThread(ret);
}

void hook_ClientUserinfoChanged(int clientNum)
{
	if ( ! codecallback_userinfochanged)
	{
		ClientUserinfoChanged(clientNum);
		return;
	}

	if (!Scr_IsSystemActive())
		return;

	stackPushInt(clientNum); // one parameter is required
	short ret = Scr_ExecEntThread(&g_entities[clientNum], codecallback_userinfochanged, 1);
	Scr_FreeThread(ret);
}

void SV_MasterHeartbeat_libcod(const char *hbname)
{
	static netadr_t	adr[MAX_MASTER_SERVERS];
	char heartbeat[32];
	int	i;

	// "dedicated 1" is for lan play, "dedicated 2" is for inet public play
	if ( !com_dedicated || com_dedicated->current.integer != 2 )
	{
		return;     // only dedicated servers send heartbeats
	}

	// if not time yet, don't send anything
	if ( svs.time >= svs.nextHeartbeatTime )
	{
		svs.nextHeartbeatTime = svs.time + HEARTBEAT_MSEC;

		// send to group masters
		for ( i = 0 ; i < MAX_MASTER_SERVERS ; i++ )
		{
			if ( !sv_master[i]->current.string[0] )
			{
				continue;
			}

			// see if we haven't already resolved the name
			// do it when needed
			if ( sv_master[i]->modified || !adr[i].type )
			{
				sv_master[i]->modified = qfalse;

				Com_Printf( "Resolving %s\n", sv_master[i]->current.string );
				if ( !NET_StringToAdr( sv_master[i]->current.string, &adr[i] ) )
				{
					// if the address failed to resolve, clear it
					// so we don't take repeated dns hits
					Com_Printf( "Couldn't resolve address: %s\n", sv_master[i]->current.string );
					Dvar_SetString(sv_master[i], "");
					sv_master[i]->modified = qfalse;
					continue;
				}
				if ( !strstr( ":", sv_master[i]->current.string ) )
				{
					adr[i].port = BigShort( PORT_MASTER );
				}
				Com_Printf( "%s resolved to %i.%i.%i.%i:%i\n", sv_master[i]->current.string,
				            adr[i].ip[0], adr[i].ip[1], adr[i].ip[2], adr[i].ip[3],
				            BigShort( adr[i].port ) );
			}

			if (strlen(sv_master[i]->current.string))
			{
				Com_Printf( "Sending heartbeat to %s\n", sv_master[i]->current.string );
				sprintf(heartbeat, "heartbeat %s\n", hbname);
				NET_OutOfBandPrint( NS_SERVER, adr[i], heartbeat );
			}
		}
	}

	// if not time yet, don't send anything
	if ( svs.time >= svs.nextStatusResponseTime )
	{
		svs.nextStatusResponseTime = svs.time + STATUS_MSEC;

		// send to group masters
		for ( i = 0 ; i < MAX_MASTER_SERVERS ; i++ )
		{
			if ( !sv_master[i]->current.string[0] )
			{
				continue;
			}

			// see if we haven't already resolved the name
			// do it when needed
			if ( sv_master[i]->modified || !adr[i].type )
			{
				sv_master[i]->modified = qfalse;

				Com_Printf( "Resolving %s\n", sv_master[i]->current.string );
				if ( !NET_StringToAdr( sv_master[i]->current.string, &adr[i] ) )
				{
					// if the address failed to resolve, clear it
					// so we don't take repeated dns hits
					Com_Printf( "Couldn't resolve address: %s\n", sv_master[i]->current.string );
					Dvar_SetString(sv_master[i], "");
					sv_master[i]->modified = qfalse;
					continue;
				}
				if ( !strstr( ":", sv_master[i]->current.string ) )
				{
					adr[i].port = BigShort( PORT_MASTER );
				}
				Com_Printf( "%s resolved to %i.%i.%i.%i:%i\n", sv_master[i]->current.string,
				            adr[i].ip[0], adr[i].ip[1], adr[i].ip[2], adr[i].ip[3],
				            BigShort( adr[i].port ) );
			}

			if (strlen(sv_master[i]->current.string))
			{
				SVC_Status(adr[i]);
			}
		}
	}
}

// Adds bot pings and removes spam on 1.2 and 1.3
void SV_CalcPings_libcod( void )
{
	int i, j;
	client_t *cl;
	int total, count;
	int delta;

	for ( i = 0 ; i < sv_maxclients->current.integer ; i++ )
	{
		cl = &svs.clients[i];

		if ( cl->state != CS_ACTIVE )
		{
			cl->ping = -1;
			continue;
		}

		if ( !cl->gentity )
		{
			cl->ping = -1;
			continue;
		}

		if ( cl->netchan.remoteAddress.type == NA_BOT )
		{
			cl->ping = 0;
			cl->lastPacketTime = svs.time;
			continue;
		}

		total = 0;
		count = 0;

		for ( j = 0 ; j < PACKET_BACKUP ; j++ )
		{
			if ( cl->frames[j].messageAcked == 0xFFFFFFFF )
				continue;

			delta = cl->frames[j].messageAcked - cl->frames[j].messageSent;
			count++;
			total += delta;
		}

		if ( !count )
			cl->ping = 999;
		else
		{
			cl->ping = total / count;

			if ( cl->ping > 999 )
				cl->ping = 999;
		}
	}
}

void SV_CheckTimeouts_libcod( void )
{
	int	i;
	client_t *cl;
	int	droppoint;
	int	zombiepoint;

	droppoint = svs.time - 1000 * sv_timeout->current.integer;
	zombiepoint = svs.time - 1000 * sv_zombietime->current.integer;

	for ( i = 0, cl = svs.clients; i < sv_maxclients->current.integer; i++, cl++ )
	{
		// message times may be wrong across a changelevel
		if (cl->lastPacketTime > svs.time)
			cl->lastPacketTime = svs.time;

		if (cl->state == CS_ZOMBIE && cl->lastPacketTime < zombiepoint)
		{
			cl->state = CS_FREE; // can now be reused
			continue;
		}

		if (cl->state >= CS_CONNECTED && cl->lastPacketTime < droppoint)
		{
			// wait several frames so a debugger session doesn't
			// cause a timeout
			if ( ++cl->timeoutCount > 5 )
			{
				SV_DropClient(cl, "EXE_TIMEDOUT");
				cl->state = CS_FREE; // don't bother with zombie state
			}
		}
		else
			cl->timeoutCount = 0;
	}
}

void PM_ProjectVelocity(const float *velIn, const float *normal, float *velOut)
{
	float lengthSq2D;
	float adjusted;
	float newZ;
	float lengthScale;

	if (!jump_bounceEnable->current.boolean)
	{
		PM_ClipVelocity(velIn, normal, velOut);
		return;
	}

	lengthSq2D = (float)(velIn[0] * velIn[0]) + (float)(velIn[1] * velIn[1]);

	if ( I_fabs(normal[2]) < 0.001 || lengthSq2D == 0.0 )
	{
		velOut[0] = velIn[0];
		velOut[1] = velIn[1];
		velOut[2] = velIn[2];
	}
	else
	{
		newZ = (float)-(float)((float)(velIn[0] * normal[0]) + (float)(velIn[1] * normal[1])) / normal[2];
		adjusted = velIn[1];
		lengthScale = I_sqrt((float)((float)(velIn[2] * velIn[2]) + lengthSq2D) / (float)((float)(newZ * newZ) + lengthSq2D));

		if ( lengthScale < 1.0 || newZ < 0.0 || velIn[2] > 0.0 )
		{
			velOut[0] = lengthScale * velIn[0];
			velOut[1] = lengthScale * adjusted;
			velOut[2] = lengthScale * newZ;
		}
	}
}

const char *SV_ModifyConfigstringIwdChkSum( client_t *client, int index )
{
	char tmp[MAX_STRING_CHARS];
	static char info[MAX_STRING_CHARS];

	if ( index == CS_SYSTEMINFO && client->netchan.protocol == 119 )
	{
		strcpy(tmp, sv.configstrings[index]);

		char *part = strtok(tmp, " ");

		while(part != NULL)
		{
			if ( strcmp(part, "1053665859") == 0 ) // iw_06.iwd
				I_strncat( info, sizeof( info ), va( "%s ", "-141992458" ) );
			else if ( strcmp(part, "1046874969") == 0 ) // iw_07.iwd
				I_strncat( info, sizeof( info ), va( "%s ", "840608716" ) );
			else
				I_strncat( info, sizeof( info ), va( "%s ", part ) );

			part = strtok(NULL, " ");
		}

		return info;
	}

	return sv.configstrings[index];
}

#endif