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

/*
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
	ci = (clientInfo_t *)(1208 * clientNum + 140846652);
	tree = level_bgs.clientinfo[clientNum].pXAnimTree;
	memset(ci, 0, sizeof(clientInfo_t));
	ci->pXAnimTree = tree;
	ci->infoValid = 1;
	ci->nextValid = 1;
	gclient->sess.connected = CS_ZOMBIE;
	gclient->sess.pers = id;
	gclient->sess.state.team = TEAM_SPECTATOR;
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
	        || !I_stricmp(g_password->current.integer, "none")
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
}
*/