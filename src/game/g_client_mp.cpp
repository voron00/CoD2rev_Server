#include "../qcommon/qcommon.h"
#include "g_shared.h"

#ifdef TESTING_LIBRARY
#define level (*((level_locals_t*)( 0x0859B400 )))
#else
extern level_locals_t level;
#endif

#ifdef TESTING_LIBRARY
#define g_entities ((gentity_t*)( 0x08665480 ))
#else
extern gentity_t g_entities[];
#endif

#ifdef TESTING_LIBRARY
#define g_clients ((gclient_t*)( 0x086F1480 ))
#else
extern gclient_t g_clients[];
#endif

#ifdef TESTING_LIBRARY
#define level_bgs (*((bgs_t*)( 0x0859EA40 )))
#else
extern bgs_t level_bgs;
#endif

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
	ClientUserinfoChanged(clientNum);
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