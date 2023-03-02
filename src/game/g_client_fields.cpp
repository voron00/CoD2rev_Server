#include "../qcommon/qcommon.h"
#include "../server/server.h"
#include "g_shared.h"

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
#define level (*((level_locals_t*)( 0x0859B400 )))
#else
extern level_locals_t level;
#endif

#ifdef TESTING_LIBRARY
#define scr_const (*((scr_const_t*)( 0x087A22A0 )))
#else
extern scr_const_t scr_const;
#endif

game_client_field_t g_client_fields[] =
{
	{ "name", 10116, 2, ClientScr_ReadOnly, NULL },
	{ "sessionteam", 0, 3, ClientScr_SetSessionTeam, ClientScr_GetSessionTeam },
	{ "sessionstate", 0, 3, ClientScr_SetSessionState, ClientScr_GetSessionState },
	{ "maxhealth", 10024, 0, ClientScr_SetMaxHealth, NULL },
	{ "score", 9912, 0, ClientScr_SetScore, NULL },
	{ "deaths", 9916, 0, NULL, NULL },
	{ "statusicon", 0, 3, ClientScr_SetStatusIcon, ClientScr_GetStatusIcon },
	{ "headicon", 0, 3, ClientScr_SetHeadIcon, ClientScr_GetHeadIcon },
	{ "headiconteam", 0, 3, ClientScr_SetHeadIconTeam, ClientScr_GetHeadIconTeam },
	{ "spectatorclient", 9900, 0, ClientScr_SetSpectatorClient, NULL },
	{ "archivetime", 9908, 1, ClientScr_SetArchiveTime, ClientScr_GetArchiveTime },
	{ "psoffsettime", 10148, 0, ClientScr_SetPSOffsetTime, ClientScr_GetPSOffsetTime },
	{ "pers", 9920, 7, ClientScr_ReadOnly, NULL }

};

void ClientScr_ReadOnly(gclient_s *pSelf, const game_client_field_t *pField)
{
	Scr_Error(va("player field %s is read-only", pField->name));
}

void ClientScr_SetSessionTeam(gclient_s *pSelf, const game_client_field_t *pField)
{
	unsigned short str;

	str = Scr_GetConstString(0);

	if ( str == scr_const.axis )
	{
		pSelf->sess.state.team = TEAM_AXIS;
	}
	else if ( str == scr_const.allies )
	{
		pSelf->sess.state.team = TEAM_ALLIES;
	}
	else if ( str == scr_const.spectator )
	{
		pSelf->sess.state.team = TEAM_SPECTATOR;
	}
	else if ( str == scr_const.none )
	{
		pSelf->sess.state.team = TEAM_NONE;
	}
	else
	{
		Scr_Error(va("'%s' is an illegal sessionteam string. Must be allies, axis, none, or spectator.", SL_ConvertToString(str)));
	}

	ClientUserinfoChanged(pSelf - level.clients);
	CalculateRanks();
}

void ClientScr_SetSessionState(gclient_s *pSelf, const game_client_field_t *pField)
{
	unsigned short str;

	str = Scr_GetConstString(0);

	if ( str == scr_const.playing )
	{
		pSelf->sess.sessionState = STATE_PLAYING;
	}
	else if ( str == scr_const.dead )
	{
		pSelf->sess.sessionState = STATE_DEAD;
	}
	else if ( str == scr_const.spectator )
	{
		pSelf->sess.sessionState = STATE_SPECTATOR;
	}
	else if ( str == scr_const.intermission )
	{
		pSelf->ps.eFlags ^= 2u;
		pSelf->sess.sessionState = STATE_INTERMISSION;
	}
	else
	{
		Scr_Error(va("'%s' is an illegal sessionstate string. Must be playing, dead, spectator, or intermission.", SL_ConvertToString(str)));
	}
}

void ClientScr_SetMaxHealth(gclient_s *pSelf, const game_client_field_t *pField)
{
	pSelf->sess.maxHealth = Scr_GetInt(0);

	if ( pSelf->sess.maxHealth <= 0 )
		pSelf->sess.maxHealth = 1;

	if ( pSelf->ps.stats[0] > pSelf->sess.maxHealth )
		pSelf->ps.stats[0] = pSelf->sess.maxHealth;

	g_entities[pSelf - level.clients].health = pSelf->ps.stats[0];
	pSelf->ps.stats[2] = pSelf->sess.maxHealth;
}

void ClientScr_SetScore(gclient_s *pSelf, const game_client_field_t *pField)
{
	pSelf->sess.score = Scr_GetInt(0);
	CalculateRanks();
}

void ClientScr_SetStatusIcon(gclient_s *pSelf, const game_client_field_t *pField)
{
	pSelf->sess.statusIcon = GScr_GetStatusIconIndex(Scr_GetString(0));
}

void ClientScr_SetHeadIcon(gclient_s *pSelf, const game_client_field_t *pField)
{
	g_entities[pSelf - level.clients].s.iHeadIcon = GScr_GetHeadIconIndex(Scr_GetString(0));
}

void ClientScr_SetHeadIconTeam(gclient_s *pSelf, const game_client_field_t *pField)
{
	unsigned short str;

	str = Scr_GetConstString(0);

	if ( scr_const.none == str )
	{
		g_entities[pSelf - level.clients].s.iHeadIconTeam = 0;
	}
	else if ( scr_const.allies == str )
	{
		g_entities[pSelf - level.clients].s.iHeadIconTeam = 2;
	}
	else if ( scr_const.axis == str )
	{
		g_entities[pSelf - level.clients].s.iHeadIconTeam = 1;
	}
	else if ( scr_const.spectator == str )
	{
		Scr_Error(va("'%s' is an illegal head icon team string. Must be none, allies, axis, or spectator.", SL_ConvertToString(str)));
	}
	else
	{
		g_entities[pSelf - level.clients].s.iHeadIconTeam = 3;
	}
}

void ClientScr_SetSpectatorClient(gclient_s *pSelf, const game_client_field_t *pField)
{
	int clientNum;

	clientNum = Scr_GetInt(0);

	if ( clientNum < -1 || clientNum > 63 )
		Scr_Error("spectatorclient can only be set to -1, or a valid client number");

	pSelf->sess.forceSpectatorClient = clientNum;
}

void ClientScr_SetArchiveTime(gclient_s *pSelf, const game_client_field_t *pField)
{
	pSelf->sess.archiveTime = (int)(Scr_GetFloat(0) * 1000.0);
}

void ClientScr_SetPSOffsetTime(gclient_s *pSelf, const game_client_field_t *pField)
{
	pSelf->sess.psOffsetTime = Scr_GetInt(0);
}

void ClientScr_GetSessionTeam(gclient_s *pSelf, const game_client_field_t *pField)
{
	int team;

	team = pSelf->sess.state.team;

	if ( team == TEAM_AXIS )
	{
		Scr_AddConstString(scr_const.axis);
	}
	else if ( team > TEAM_AXIS )
	{
		if ( team == TEAM_ALLIES )
		{
			Scr_AddConstString(scr_const.allies);
		}
		else if ( team == TEAM_SPECTATOR )
		{
			Scr_AddConstString(scr_const.spectator);
		}
	}
	else if ( team == TEAM_NONE )
	{
		Scr_AddConstString(scr_const.none);
	}
}

void ClientScr_GetSessionState(gclient_s *pSelf, const game_client_field_t *pField)
{
	int sessionState;

	sessionState = pSelf->sess.sessionState;

	if ( sessionState == STATE_DEAD )
	{
		Scr_AddConstString(scr_const.dead);
	}
	else if ( sessionState > STATE_DEAD )
	{
		if ( sessionState == STATE_SPECTATOR )
		{
			Scr_AddConstString(scr_const.spectator);
		}
		else if ( sessionState == STATE_INTERMISSION )
		{
			Scr_AddConstString(scr_const.intermission);
		}
	}
	else if ( sessionState == STATE_PLAYING )
	{
		Scr_AddConstString(scr_const.playing);
	}
}

void ClientScr_GetStatusIcon(gclient_s *pSelf, const game_client_field_t *pField)
{
	char string[MAX_STRING_CHARS];

	if ( pSelf->sess.statusIcon )
	{
		SV_GetConfigstring(pSelf->sess.statusIcon + 22, string, sizeof(string));
		Scr_AddString(string);
	}
	else
	{
		Scr_AddString("");
	}
}

void ClientScr_GetHeadIcon(gclient_s *pSelf, const game_client_field_t *pField)
{
	int iHeadIcon;
	char string[MAX_STRING_CHARS];

	iHeadIcon = g_entities[pSelf - level.clients].s.iHeadIcon;

	if ( iHeadIcon )
	{
		if ( iHeadIcon < 16 )
		{
			SV_GetConfigstring(iHeadIcon + 30, string, 0);
			Scr_AddString(string);
		}
	}
	else
	{
		Scr_AddString("");
	}
}

void ClientScr_GetHeadIconTeam(gclient_s *pSelf, const game_client_field_t *pField)
{
	int icon;
	unsigned int str;

	icon = g_entities[pSelf - level.clients].s.iHeadIconTeam;

	switch ( icon )
	{
	case 1:
		str = scr_const.axis;
		break;

	case 2:
		str = scr_const.allies;
		break;

	case 3:
		str = scr_const.spectator;
		break;

	default:
		str = scr_const.none;
		break;
	}

	Scr_AddConstString(str);
}

void ClientScr_GetArchiveTime(gclient_s *pSelf, const game_client_field_t *pField)
{
	Scr_AddFloat((float)pSelf->sess.archiveTime * 0.001);
}

void ClientScr_GetPSOffsetTime(gclient_s *pSelf, const game_client_field_t *pField)
{
	Scr_AddInt(pSelf->sess.archiveTime);
}

void Scr_GetClientField(gclient_s *client, int offset)
{
	game_client_field_t *field;

	field = &g_client_fields[offset];

	if ( field->getter )
		field->getter(client, field);
	else
		Scr_GetGenericField((byte *)client, field->type, field->ofs);
}

void Scr_SetClientField(gclient_s *client, int offset)
{
	game_client_field_t *field;

	field = &g_client_fields[offset];

	if ( field->setter )
		field->setter(client, field);
	else
		Scr_SetGenericField((byte *)client, field->type, field->ofs);
}

void GScr_AddFieldsForClient()
{
	game_client_field_t *i;

	for ( i = g_client_fields; i->name; ++i )
		Scr_AddClassField(0, i->name, (i - g_client_fields) | 0xC000);
}