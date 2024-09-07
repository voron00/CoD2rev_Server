#include "../qcommon/qcommon.h"
#include "g_shared.h"

unsigned int G_GetNonPVSFriendlyInfo(gentity_s *pSelf, float *vPosition, int iLastUpdateEnt)
{
	unsigned int index;
	int iNext;
	int iNum;
	int team;
	gentity_s *pEnt;
	float fScale;
	float fScale_4;
	vec2_t fPos;
	int iPos;
	int iPos_4;
	int num;
	int iCurrentEnt;
	int iEntCount;
	int iBaseEnt;

	team = pSelf->client->sess.state.team;

	if ( team == TEAM_FREE || team == TEAM_SPECTATOR )
		return 0;

	if ( iLastUpdateEnt == 1023 )
		iBaseEnt = 0;
	else
		iBaseEnt = iLastUpdateEnt + 1;

	for ( iEntCount = 0; ; ++iEntCount )
	{
		if ( iEntCount > 63 )
			return 0;

		iNum = iEntCount + iBaseEnt;
		iNext = iEntCount + iBaseEnt + (iEntCount + iBaseEnt < 0 ? 0x3F : 0);
		iCurrentEnt = (iEntCount + iBaseEnt) % 64;
		pEnt = &g_entities[iCurrentEnt];

		if ( pEnt->r.inuse )
		{
			if ( pEnt->client
			        && pEnt->client->sess.sessionState == SESS_STATE_PLAYING
			        && pEnt->client->sess.state.team == team
			        && pSelf != pEnt
			        && !SV_inSnapshot(vPosition, pEnt->s.number) )
			{
				break;
			}
		}
	}

	num = pEnt->s.number;
	Vector2Subtract(pEnt->r.currentOrigin, vPosition, fPos);

	iPos = (int)(fPos[0] + 0.5);
	iPos_4 = (int)(fPos[1] + 0.5);

	fScale = 1.0;
	fScale_4 = 1.0;

	if ( iPos <= 1024 )
	{
		if ( iPos < -1022 )
			fScale = -1022.0 / iPos;
	}
	else
	{
		fScale = 1024.0 / iPos;
	}

	if ( iPos_4 <= 1024 )
	{
		if ( iPos_4 < -1022 )
			fScale_4 = -1022.0 / iPos_4;
	}
	else
	{
		fScale_4 = 1024.0 / iPos_4;
	}

	if ( fScale < 1.0 || fScale_4 < 1.0 )
	{
		if ( fScale_4 <= fScale )
		{
			if ( fScale > fScale_4 )
				iPos = (int)(iPos * fScale_4);
		}
		else
		{
			iPos_4 = (int)(iPos_4 * fScale);
		}
	}

	if ( iPos <= 1024 )
	{
		if ( iPos < -1022 )
			iPos = -1022;
	}
	else
	{
		iPos = 1024;
	}

	if ( iPos_4 <= 1024 )
	{
		if ( iPos_4 < -1022 )
			iPos_4 = -1022;
	}
	else
	{
		iPos_4 = 1024;
	}

	index = num & 0xFFFF803F;
	num = num & 0xFF00003F | ((((unsigned short)((iPos + 2) / 4) + 255) & 0x1FF) << 6) & 0x7FFF | ((((unsigned short)((iPos_4 + 2) / 4) + 255) & 0x1FF) << 15);

	return (index | ((((unsigned short)((iPos + 2) / 4) + 255) & 0x1FF) << 6)) & 0x7FFF | ((((unsigned short)((iPos_4 + 2) / 4) + 255) & 0x1FF) << 15) & 0xFFFFFF | ((unsigned char)(int)(g_entities[iNum - (iNext >> 6 << 6)].r.currentAngles[1] * 0.71111113) << 24);
}



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
		cycle = BG_GetBobCycle(&client->ps);
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
		        && level.clients[i].sess.sessionState == SESS_STATE_SPECTATOR
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
	client->ps.pm_type = PM_SPECTATOR;
	CalculateRanks();
	Scr_Notify(&g_entities[clientNum], scr_const.begin, 0);
}

#if LIBCOD_COMPILE_PLAYER == 1
char proxy_realip[MAX_CLIENTS][16] = {0};
#endif
extern dvar_t *g_password;
const char* ClientConnect(unsigned int clientNum, unsigned short scriptPersId)
{
	XAnimTree_s *pXAnimTree;
	clientInfo_t *ci;
	gentity_t *ent;
	char userinfo[MAX_INFO_STRING];
	gclient_t *client;
	const char *password;

	ent = &g_entities[clientNum];
	client = &level.clients[clientNum];
	memset(client, 0, sizeof(gclient_s));

	ci = &level_bgs.clientinfo[clientNum];
	pXAnimTree = level_bgs.clientinfo[clientNum].pXAnimTree;
	memset(ci, 0, sizeof(clientInfo_t));

	ci->pXAnimTree = pXAnimTree;

	ci->infoValid = 1;
	ci->nextValid = 1;

	client->sess.connected = CON_CONNECTING;
	client->sess.pers = scriptPersId;
	client->sess.state.team = TEAM_SPECTATOR;
	client->sess.sessionState = SESS_STATE_SPECTATOR;
	client->spectatorClient = -1;
	client->sess.forceSpectatorClient = -1;

	G_InitGentity(ent);

	ent->handler = 0;
	ent->client = client;

	client->useHoldEntity = 1023;
	client->sess.state.clientIndex = clientNum;
	client->ps.clientNum = clientNum;

	ClientUserinfoChanged(clientNum);
	SV_GetUserinfo(clientNum, userinfo, sizeof(userinfo));

	if ( client->sess.localClient
	        || (password = Info_ValueForKey(userinfo, "password"), !*g_password->current.string)
	        || !I_stricmp(g_password->current.string, "none")
	        || !strcmp(g_password->current.string, password) )
	{
#if LIBCOD_COMPILE_PLAYER == 1
		char realIP[16];
		strncpy(realIP, Info_ValueForKey(userinfo, "ip"), sizeof(proxy_realip[clientNum]) - 1);

		if (strlen(realIP) != 0)
		{
			strncpy(proxy_realip[clientNum], realIP, sizeof(proxy_realip[clientNum]) - 1);
			Com_DPrintf("realip = %s\n", proxy_realip[clientNum]);
		}
#endif
		Scr_PlayerConnect(ent);
		CalculateRanks();
		return NULL;
	}
	else
	{
		G_FreeEntity(ent);
		return "GAME_INVALIDPASSWORD";
	}

	return NULL;
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
		            || (ent->client->sess.sessionState == SESS_STATE_DEAD || talker->client->sess.sessionState == SESS_STATE_DEAD)
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
	G_SetClientContents(ent);
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