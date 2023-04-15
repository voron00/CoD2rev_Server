#include "../qcommon/qcommon.h"
#include "g_shared.h"

void SetClientViewAngle(gentity_s *ent, const float *angle)
{
	float direction;
	float proneDirection;
	float torsoPitch;
	vec3_t angles;
	int i;

	VectorCopy(angle, angles);

	if ( (ent->client->ps.pm_flags & 1) != 0 && (ent->client->ps.eFlags & 0x300) == 0 )
	{
		AngleDelta(ent->client->ps.proneDirection, angles[1]);
		angles[1] = AngleNormalize180(angles[1]);

		if ( angles[1] > 45.0 || (direction = angles[1], angles[1] < -45.0) )
		{
			if ( angles[1] <= 45.0 )
				angles[1] = angles[1] + 45.0;
			else
				angles[1] = angles[1] - 45.0;

			ent->client->ps.delta_angles[1] += ANGLE2SHORT(angles[1]);

			if ( angles[1] <= 0.0 )
				proneDirection = ent->client->ps.proneDirection + 45.0;
			else
				proneDirection = ent->client->ps.proneDirection - 45.0;

			direction = ANGLE2SHORT(proneDirection) * 0.0054931641;
			angles[1] = direction;
		}

		AngleDelta(ent->client->ps.proneTorsoPitch, angles[0]);

		angles[0] = direction;
		angles[0] = AngleNormalize180(angles[0]);

		if ( angles[0] > 45.0 || angles[0] < -15.0 )
		{
			if ( angles[0] <= 45.0 )
				angles[0] = angles[0] + 15.0;
			else
				angles[0] = angles[0] - 45.0;

			ent->client->ps.delta_angles[0] += ANGLE2SHORT(angles[0]);

			if ( angles[0] <= 0.0 )
				torsoPitch = ent->client->ps.proneTorsoPitch + 15.0;
			else
				torsoPitch = ent->client->ps.proneTorsoPitch - 45.0;

			angles[0] = AngleNormalize180(torsoPitch);
		}
	}

	for ( i = 0; i < 3; ++i )
		ent->client->ps.delta_angles[i] = ANGLE2SHORT(angles[i]) - ent->client->sess.cmd.angles[i];

	VectorCopy(angles, ent->r.currentAngles);
	VectorCopy(ent->r.currentAngles, ent->client->ps.viewangles);
}

static void ClientCleanName( const char *in, char *out, int outSize )
{
	int		len, colorlessLen;
	char	ch;
	char	*p;
	int		spaces;

	//save room for trailing null byte
	outSize--;

	len = 0;
	colorlessLen = 0;
	p = out;
	*p = 0;
	spaces = 0;

	while( 1 )
	{
		ch = *in++;

		if( !ch )
		{
			break;
		}

		// don't allow leading spaces
		if( !*p && ch == ' ' )
		{
			continue;
		}

		// check colors
		if( ch == Q_COLOR_ESCAPE )
		{
			// solo trailing carat is not a color prefix
			if( !*in )
			{
				break;
			}

			// don't allow black in a name, period
			if( ColorIndex(*in) == 0 )
			{
				in++;
				continue;
			}

			// make sure room in dest for both chars
			if( len > outSize - 2 )
			{
				break;
			}

			*out++ = ch;
			*out++ = *in++;
			len += 2;
			continue;
		}

		// don't allow too many consecutive spaces
		if( ch == ' ' )
		{
			spaces++;
			if( spaces > 3 )
			{
				continue;
			}
		}
		else
		{
			spaces = 0;
		}

		if( len > outSize - 1 )
		{
			break;
		}

		*out++ = ch;
		colorlessLen++;
		len++;
	}

	*out = 0;

	// don't allow empty names
	if( *p == 0 || colorlessLen == 0 )
	{
		Q_strncpyz( p, "UnnamedPlayer", outSize );
	}
}

#ifdef LIBCOD
extern int codecallback_userinfochanged;
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
#endif

void ClientUserinfoChanged(int clientNum)
{
	char userinfo[MAX_STRING_CHARS];
	char oldname[MAX_STRING_CHARS];
	const char *value;
	gclient_t *client;
	gentity_t *entity;

	entity = &g_entities[clientNum];
	client = entity->client;
	SV_GetUserinfo(clientNum, userinfo, 1024);

	if ( !Info_Validate(userinfo) )
		strcpy(userinfo, "\\name\\badinfo");

	client->sess.localClient = SV_IsLocalClient(clientNum);
	value = Info_ValueForKey(userinfo, "cg_predictItems");

	if ( atoi(value) )
		client->sess.predictItemPickup = 1;
	else
		client->sess.predictItemPickup = 0;

	if ( client->sess.connected == CS_CONNECTED && level.manualNameChange )
	{
		value = Info_ValueForKey(userinfo, "name");
		ClientCleanName(value, client->sess.name, 32);
	}
	else
	{
		I_strncpyz(oldname, client->sess.state.name, 1024);
		value = Info_ValueForKey(userinfo, "name");
		ClientCleanName(value, client->sess.state.name, 32);
		I_strncpyz(client->sess.name, client->sess.state.name, 32);
	}

	level_bgs.clientinfo[clientNum].clientNum = clientNum;
	level_bgs.clientinfo[clientNum].team = client->sess.state.team;

	I_strncpyz(level_bgs.clientinfo[clientNum].name, client->sess.state.name, 32);
}

void G_GetPlayerViewDirection(const gentity_s *ent, float *forward, float *right, float *up)
{
	AngleVectors(ent->client->ps.viewangles, forward, right, up);
}

void G_GetPlayerViewOrigin(gentity_s *ent, float *origin)
{
	float speed;
	float cycle;
	float hFactor;
	float vFactor;
	vec3_t direction;
	gclient_s *client;

	client = ent->client;

	if ( (client->ps.eFlags & 0x300) != 0 )
	{
		if ( !G_DObjGetWorldTagPos(&g_entities[client->ps.viewlocked_entNum], scr_const.tag_player, origin) )
			Com_Error(ERR_DROP, "G_GetPlayerViewOrigin: Couldn't find [tag_player] on turret");
	}
	else
	{
		VectorCopy(client->ps.origin, origin);
		origin[2] = origin[2] + client->ps.viewHeightCurrent;
		cycle = BG_GetBobCycle(client);
		speed = BG_GetSpeed(&client->ps, level.time);
		vFactor = BG_GetVerticalBobFactor(&client->ps, cycle, speed, bg_bobMax->current.decimal);
		origin[2] = origin[2] + vFactor;
		hFactor = BG_GetHorizontalBobFactor(&client->ps, cycle, speed, bg_bobMax->current.decimal);
		G_GetPlayerViewDirection(ent, 0, direction, 0);
		VectorMA(origin, hFactor, direction, origin);
		AddLeanToPosition(origin, client->ps.viewangles[1], client->ps.leanf, 16.0, 20.0);

		if ( client->ps.origin[2] + 8.0 > origin[2] )
			origin[2] = client->ps.origin[2] + 8.0;
	}
}

void ClientDisconnect(int clientNum)
{
	gclient_s *client;
	int i;
	gentity_s *ent;

	client = &level.clients[clientNum];
	ent = &g_entities[clientNum];

	if ( Scr_IsSystemActive() )
	{
		Scr_AddString("disconnect");
		Scr_AddString("-1");
		Scr_Notify(ent, scr_const.menuresponse, 2u);
	}

	for ( i = 0; i < level.maxclients; ++i )
	{
		if ( level.clients[i].sess.connected
		        && level.clients[i].sess.sessionState == STATE_SPECTATOR
		        && level.clients[i].spectatorClient == clientNum )
		{
			StopFollowing(&g_entities[i]);
		}
	}

	HudElem_ClientDisconnect(ent);

	if ( Scr_IsSystemActive() )
		Scr_PlayerDisconnect(ent);

	G_FreeEntity(ent);
	client->sess.connected = CS_FREE;
	memset(&client->sess.state, 0, sizeof(client->sess.state));
	CalculateRanks();
}

void ClientBegin(unsigned int clientNum)
{
	gclient_s *client;

	client = &level.clients[clientNum];
	client->sess.connected = CS_CONNECTED;
	client->ps.pm_type = 4;
	CalculateRanks();
	Scr_Notify(&g_entities[clientNum], scr_const.begin, 0);
}

extern dvar_t *g_password;
const char* ClientConnect(unsigned int clientNum, unsigned int scriptPersId)
{
	XAnimTree_s *tree;
	clientInfo_t *ci;
	gentity_s *ent;
	char userinfo[1024];
	gclient_s *gclient;
	const char *password;
	uint16_t id;

	id = scriptPersId;
	ent = &g_entities[clientNum];
	gclient = &level.clients[clientNum];
	memset(gclient, 0, sizeof(gclient_s));
	ci = &level_bgs.clientinfo[clientNum];
	tree = level_bgs.clientinfo[clientNum].pXAnimTree;
	memset(ci, 0, sizeof(clientInfo_t));
	ci->pXAnimTree = tree;
	ci->infoValid = 1;
	ci->nextValid = 1;
	gclient->sess.connected = CS_ZOMBIE;
	gclient->sess.pers = id;
	gclient->sess.state.team = 3;
	gclient->sess.sessionState = STATE_SPECTATOR;
	gclient->spectatorClient = -1;
	gclient->sess.forceSpectatorClient = -1;
	G_InitGentity(ent);
	ent->handler = 0;
	ent->client = gclient;
	gclient->useHoldEntity = 1023;
	gclient->sess.state.clientIndex = clientNum;
	gclient->ps.clientNum = clientNum;
#ifdef LIBCOD
	hook_ClientUserinfoChanged(clientNum);
#else
	ClientUserinfoChanged(clientNum);
#endif
	SV_GetUserinfo(clientNum, userinfo, 1024);
	if ( gclient->sess.localClient
	        || (password = Info_ValueForKey(userinfo, "password"), !*g_password->current.string)
	        || !I_stricmp(g_password->current.string, "none")
	        || !strcmp(g_password->current.string, password) )
	{
		Scr_PlayerConnect(ent);
		CalculateRanks();
		return 0;
	}
	else
	{
		G_FreeEntity(ent);
		return "GAME_INVALIDPASSWORD";
	}

	return 0;
}

extern dvar_t *voice_global;
extern dvar_t *voice_deadChat;
extern dvar_t *voice_localEcho;
void G_BroadcastVoice(gentity_s *talker, VoicePacket_t *voicePacket)
{
	gentity_s *ent;
	int listener;

	talker->client->lastVoiceTime = level.time;

	for ( listener = 0; listener < MAX_CLIENTS; ++listener )
	{
		ent = &g_entities[listener];

		if ( ent->r.inuse
		        && ent->client
		        && (voice_global->current.boolean || OnSameTeam(talker, ent) || talker->client->sess.state.team == TEAM_NONE)
		        && (ent->client->sess.sessionState == talker->client->sess.sessionState
		            || (ent->client->sess.sessionState == STATE_DEAD || talker->client->sess.sessionState == STATE_DEAD)
		            && voice_deadChat->current.boolean)
		        && (talker != ent || voice_localEcho->current.boolean)
		        && !SV_ClientHasClientMuted(listener, talker->s.number)
		        && SV_ClientWantsVoiceData(listener) )
		{
			SV_QueueVoicePacket(talker->s.number, listener, voicePacket);
		}
	}
}

extern vec3_t playerMins;
extern vec3_t playerMaxs;
extern dvar_t *g_inactivity;
void ClientSpawn(gentity_s *ent, const float *spawn_origin, const float *spawn_angles)
{
	int spawncount;
	clientSession_t session;
	int flags;
	gclient_s *client;
	int num;

	num = ent - g_entities;
	client = ent->client;

	if ( (client->ps.pm_flags & 0x800000) != 0 && (client->ps.eFlags & 0x300) != 0 )
		G_ClientStopUsingTurret(&level.gentities[client->ps.viewlocked_entNum]);

	G_EntUnlink(ent);

	if ( ent->r.linked )
		SV_UnlinkEntity(ent);

	ent->s.groundEntityNum = 1023;
	Scr_SetString(&ent->classname, scr_const.player);
	ent->clipmask = 42008593;
	ent->r.svFlags |= 1u;
	ent->takedamage = 0;
	G_UpdatePlayerContents(ent);
	ent->handler = 10;
	ent->flags = 4096;
	VectorCopy(playerMins, ent->r.mins);
	VectorCopy(playerMaxs, ent->r.maxs);
	flags = client->ps.eFlags & 0x100002;
	memcpy(&session, &client->sess, sizeof(session));
	spawncount = client->ps.stats[5];
	memset(client, 0, sizeof(gclient_s));
	memcpy(&client->sess, &session, sizeof(client->sess));
	client->spectatorClient = -1;
	client->useHoldEntity = 1023;
	client->ps.stats[5] = spawncount + 1;
	client->ps.stats[2] = client->sess.maxHealth;
	client->ps.eFlags = flags;
	client->sess.state.clientIndex = num;
	client->ps.clientNum = num;
	client->ps.viewlocked_entNum = 1023;
	SV_GetUsercmd(client - level.clients, &client->sess.cmd);
	client->ps.eFlags ^= 2u;
	VectorCopy(ent->r.mins, client->ps.mins);
	VectorCopy(ent->r.maxs, client->ps.maxs);
	client->ps.viewHeightTarget = 60;
	client->ps.viewHeightCurrent = 60.0;
	client->ps.viewHeightLerpTime = 0;
	client->ps.viewHeightLerpPosAdj = 0.0;
	G_SetOrigin(ent, spawn_origin);
	VectorCopy(spawn_origin, client->ps.origin);
	client->ps.pm_flags |= 0x1000u;
	SetClientViewAngle(ent, spawn_angles);
	client->inactivityTime = level.time + 1000 * g_inactivity->current.integer;
	client->buttons = client->sess.cmd.buttons;
	level.clientIsSpawning = 1;
	client->lastSpawnTime = level.time;
	client->sess.cmd.serverTime = level.time;
	client->ps.commandTime = level.time - 100;
	ClientEndFrame(ent);
	ClientThink_real(ent, &client->sess.cmd);
	level.clientIsSpawning = 0;
	BG_PlayerStateToEntityState(&client->ps, &ent->s, 1, 1u);
}