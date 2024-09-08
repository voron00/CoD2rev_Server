#include "../qcommon/qcommon.h"
#include "../server/server.h"
#include "g_shared.h"

// VoroN: A bit more cleaner approach
#define GCSF_NONE 0
#define GCSF( x ) (intptr_t)&( (gclient_t*)0 )->x

game_client_field_t g_client_fields[] =
{
	{ "name", GCSF( sess.state.name ), F_LSTRING, ClientScr_ReadOnly, NULL },
	{ "sessionteam", GCSF_NONE, F_STRING, ClientScr_SetSessionTeam, ClientScr_GetSessionTeam },
	{ "sessionstate", GCSF_NONE, F_STRING, ClientScr_SetSessionState, ClientScr_GetSessionState },
	{ "maxhealth", GCSF( sess.maxHealth ), F_INT, ClientScr_SetMaxHealth, NULL },
	{ "score", GCSF( sess.score ), F_INT, ClientScr_SetScore, NULL },
	{ "deaths", GCSF( sess.deaths ), F_INT, NULL, NULL },
	{ "statusicon", GCSF_NONE, F_STRING, ClientScr_SetStatusIcon, ClientScr_GetStatusIcon },
	{ "headicon", GCSF_NONE, F_STRING, ClientScr_SetHeadIcon, ClientScr_GetHeadIcon },
	{ "headiconteam", GCSF_NONE, F_STRING, ClientScr_SetHeadIconTeam, ClientScr_GetHeadIconTeam },
	{ "spectatorclient", GCSF( sess.forceSpectatorClient ), F_INT, ClientScr_SetSpectatorClient, NULL },
	{ "archivetime", GCSF( sess.archiveTime ), F_FLOAT, ClientScr_SetArchiveTime, ClientScr_GetArchiveTime },
	{ "psoffsettime", GCSF( sess.psOffsetTime ), F_INT, ClientScr_SetPSOffsetTime, ClientScr_GetPSOffsetTime },
	{ "pers", GCSF( sess.pers ), F_OBJECT, ClientScr_ReadOnly, NULL },
	{ NULL, GCSF_NONE, F_INT, NULL, NULL } // field terminator
};

/*
===============
GScr_AddFieldsForClient
===============
*/
void GScr_AddFieldsForClient()
{
	const game_client_field_t *f;

	for ( f = g_client_fields; f->name; f++ )
	{
		assert(!((f - g_client_fields) & ENTFIELD_MASK));
		assert((f - g_client_fields) == (unsigned short)( f - g_client_fields ));

		Scr_AddClassField(CLASS_NUM_ENTITY, f->name, (unsigned short)(f - g_client_fields) | ENTFIELD_MASK);
	}
}

/*
===============
Scr_SetClientField
===============
*/
void Scr_SetClientField( gclient_t *client, int offset )
{
	const game_client_field_t *f;

	assert(client);
	assert(static_cast<unsigned int>( offset ) < ARRAY_COUNT( g_client_fields ) - 1);
	assert(offset >= 0);

	f = &g_client_fields[offset];

	if ( f->setter )
	{
		f->setter(client, f);
	}
	else
	{
		assert(f->ofs);
		Scr_SetGenericField((byte *)client, f->type, f->ofs);
	}
}

/*
===============
Scr_GetClientField
===============
*/
void Scr_GetClientField( gclient_t *client, int offset )
{
	const game_client_field_t *f;

	assert(client);
	assert(static_cast<unsigned int>( offset ) < ARRAY_COUNT( g_client_fields ) - 1);
	assert(offset >= 0);

	f = &g_client_fields[offset];

	if ( f->getter )
	{
		f->getter(client, f);
	}
	else
	{
		assert(f->ofs);
		Scr_GetGenericField((byte *)client, f->type, f->ofs);
	}
}

/*
===============
ClientScr_SetPSOffsetTime
===============
*/
void ClientScr_SetPSOffsetTime( gclient_t *pSelf, const game_client_field_t *pField )
{
	assert(pSelf);
	pSelf->sess.psOffsetTime = Scr_GetInt(0);
}

/*
===============
ClientScr_SetArchiveTime
===============
*/
void ClientScr_SetArchiveTime( gclient_t *pSelf, const game_client_field_t *pField )
{
	assert(pSelf);
	pSelf->sess.archiveTime = (int)(Scr_GetFloat(0) * 1000.0);
}

/*
===============
ClientScr_SetSpectatorClient
===============
*/
void ClientScr_SetSpectatorClient( gclient_t *pSelf, const game_client_field_t *pField )
{
	int iNewSpectatorClient;

	assert(pSelf);
	iNewSpectatorClient = Scr_GetInt(0);

	if ( iNewSpectatorClient < -1 || iNewSpectatorClient >= MAX_CLIENTS )
		Scr_Error("spectatorclient can only be set to -1, or a valid client number");

	pSelf->sess.forceSpectatorClient = iNewSpectatorClient;
}

/*
===============
ClientScr_SetScore
===============
*/
void ClientScr_SetScore( gclient_t *pSelf, const game_client_field_t *pField )
{
	assert(pSelf);
	pSelf->sess.score = Scr_GetInt(0);
	CalculateRanks();
}

/*
===============
ClientScr_SetMaxHealth
===============
*/
void ClientScr_SetMaxHealth( gclient_t *pSelf, const game_client_field_t *pField )
{
	assert(pSelf);
	pSelf->sess.maxHealth = Scr_GetInt(0);

	if ( pSelf->sess.maxHealth < 1 )
		pSelf->sess.maxHealth = 1;

	if ( pSelf->ps.stats[STAT_HEALTH] > pSelf->sess.maxHealth )
		pSelf->ps.stats[STAT_HEALTH] = pSelf->sess.maxHealth;

	g_entities[pSelf - level.clients].health = pSelf->ps.stats[STAT_HEALTH];
	pSelf->ps.stats[STAT_MAX_HEALTH] = pSelf->sess.maxHealth;
}

/*
===============
ClientScr_ReadOnly
===============
*/
void ClientScr_ReadOnly( gclient_t *pSelf, const game_client_field_t *pField )
{
	assert(pSelf);
	assert(pField);
	Scr_Error(va("player field %s is read-only", pField->name));
}

/*
===============
ClientScr_SetHeadIconTeam
===============
*/
void ClientScr_SetHeadIconTeam( gclient_t *pSelf, const game_client_field_t *pField )
{
	gentity_t *pEnt;
	unsigned short sTeam;

	assert(pSelf);
	assert(pSelf - level.clients >= 0 && pSelf - level.clients < level.maxclients);

	pEnt = &g_entities[pSelf - level.clients];
	sTeam = Scr_GetConstString(0);

	if ( sTeam == scr_const.none )
	{
		pEnt->s.iHeadIconTeam = TEAM_FREE;
	}
	else if ( sTeam == scr_const.allies )
	{
		pEnt->s.iHeadIconTeam = TEAM_ALLIES;
	}
	else if ( sTeam == scr_const.axis )
	{
		pEnt->s.iHeadIconTeam = TEAM_AXIS;
	}
	else if ( sTeam == scr_const.spectator )
	{
		pEnt->s.iHeadIconTeam = TEAM_SPECTATOR;
	}
	else
	{
		Scr_Error(va("'%s' is an illegal head icon team string. Must be none, allies, axis, or spectator.", SL_ConvertToString(sTeam)));
	}
}

/*
===============
ClientScr_SetSessionState
===============
*/
void ClientScr_SetSessionState( gclient_t *pSelf, const game_client_field_t *pField )
{
	unsigned short newState;

	assert(pSelf);
	assert(pSelf->sess.connected != CON_DISCONNECTED);

	newState = Scr_GetConstString(0);

	if ( newState == scr_const.playing )
	{
		pSelf->sess.sessionState = SESS_STATE_PLAYING;
	}
	else if ( newState == scr_const.dead )
	{
		pSelf->sess.sessionState = SESS_STATE_DEAD;
	}
	else if ( newState == scr_const.spectator )
	{
		pSelf->sess.sessionState = SESS_STATE_SPECTATOR;
	}
	else if ( newState == scr_const.intermission )
	{
		pSelf->ps.eFlags ^= EF_TELEPORT_BIT;
		pSelf->sess.sessionState = SESS_STATE_INTERMISSION;
	}
	else
	{
		Scr_Error(va("'%s' is an illegal sessionstate string. Must be playing, dead, spectator, or intermission.", SL_ConvertToString(newState)));
	}
}

/*
===============
ClientScr_SetHeadIcon
===============
*/
void ClientScr_SetHeadIcon( gclient_t *pSelf, const game_client_field_t *pField )
{
	gentity_t *pEnt;
	const char *pszIcon;

	assert(pSelf);
	assert(pSelf - level.clients >= 0 && pSelf - level.clients < level.maxclients);

	pEnt = &g_entities[pSelf - level.clients];
	pszIcon = Scr_GetString(0);
	pEnt->s.iHeadIcon = GScr_GetHeadIconIndex(pszIcon);
}

/*
===============
ClientScr_SetStatusIcon
===============
*/
void ClientScr_SetStatusIcon( gclient_t *pSelf, const game_client_field_t *pField )
{
	const char *pszIcon;

	assert(pSelf);
	pszIcon = Scr_GetString(0);
	pSelf->sess.statusIcon = GScr_GetStatusIconIndex(pszIcon);
}

/*
===============
ClientScr_SetSessionTeam
===============
*/
void ClientScr_SetSessionTeam( gclient_t *pSelf, const game_client_field_t *pField )
{
	unsigned short newTeam;

	assert(pSelf);
	newTeam = Scr_GetConstString(0);

	if ( newTeam == scr_const.axis )
	{
		pSelf->sess.state.team = TEAM_AXIS;
	}
	else if ( newTeam == scr_const.allies )
	{
		pSelf->sess.state.team = TEAM_ALLIES;
	}
	else if ( newTeam == scr_const.spectator )
	{
		pSelf->sess.state.team = TEAM_SPECTATOR;
	}
	else if ( newTeam == scr_const.none )
	{
		pSelf->sess.state.team = TEAM_FREE;
	}
	else
	{
		Scr_Error(va("'%s' is an illegal sessionteam string. Must be allies, axis, none, or spectator.", SL_ConvertToString(newTeam)));
	}

	if ( pSelf - level.clients >= level.maxclients )
		Scr_Error("client is not pointing to the level.clients array");

	ClientUserinfoChanged(pSelf - level.clients);
	CalculateRanks();
}

/*
===============
ClientScr_GetPSOffsetTime
===============
*/
void ClientScr_GetPSOffsetTime( gclient_t *pSelf, const game_client_field_t *pField )
{
	assert(pSelf);
	Scr_AddInt(pSelf->sess.archiveTime);
}

/*
===============
ClientScr_GetArchiveTime
===============
*/
void ClientScr_GetArchiveTime( gclient_t *pSelf, const game_client_field_t *pField )
{
	assert(pSelf);
	Scr_AddFloat((float)pSelf->sess.archiveTime * 0.001);
}

/*
===============
ClientScr_GetHeadIconTeam
===============
*/
void ClientScr_GetHeadIconTeam( gclient_t *pSelf, const game_client_field_t *pField )
{
	int iHeadIcon;

	assert(pSelf);
	assert(pSelf - level.clients >= 0 && pSelf - level.clients < level.maxclients);

	iHeadIcon = g_entities[pSelf - level.clients].s.iHeadIconTeam;

	switch ( iHeadIcon )
	{
	case TEAM_AXIS:
		Scr_AddConstString(scr_const.axis);
		break;
	case TEAM_ALLIES:
		Scr_AddConstString(scr_const.allies);
		break;
	case TEAM_SPECTATOR:
		Scr_AddConstString(scr_const.spectator);
		break;
	default:
		Scr_AddConstString(scr_const.none);
		break;
	}
}

/*
===============
ClientScr_GetHeadIcon
===============
*/
void ClientScr_GetHeadIcon( gclient_t *pSelf, const game_client_field_t *pField )
{
	int iHeadIcon;
	char szConfigString[MAX_STRING_CHARS];

	assert(pSelf);
	assert(pSelf - level.clients >= 0 && pSelf - level.clients < level.maxclients);

	iHeadIcon = g_entities[pSelf - level.clients].s.iHeadIcon;

	if ( !iHeadIcon )
	{
		Scr_AddString("");
		return;
	}

	if ( iHeadIcon >= MAX_HEADICONS )
	{
		return;
	}

	SV_GetConfigstring(CS_HEAD_ICONS - 1 + iHeadIcon, szConfigString, sizeof(szConfigString));
	Scr_AddString(szConfigString);
}

/*
===============
ClientScr_GetStatusIcon
===============
*/
void ClientScr_GetStatusIcon( gclient_t *pSelf, const game_client_field_t *pField )
{
	char szConfigString[MAX_STRING_CHARS];

	assert(pSelf);
	assert(pSelf->sess.statusIcon >= 0 && pSelf->sess.statusIcon <= MAX_STATUS_ICONS);

	if ( !pSelf->sess.statusIcon )
	{
		Scr_AddString("");
		return;
	}

	SV_GetConfigstring(CS_STATUS_ICONS - 1 + pSelf->sess.statusIcon, szConfigString, sizeof(szConfigString));
	Scr_AddString(szConfigString);
}

/*
===============
ClientScr_GetSessionState
===============
*/
void ClientScr_GetSessionState( gclient_t *pSelf, const game_client_field_t *pField )
{
	assert(pSelf);
	assert(pSelf->sess.connected != CON_DISCONNECTED);

	switch ( pSelf->sess.sessionState )
	{
	case SESS_STATE_PLAYING:
		Scr_AddConstString(scr_const.playing);
		break;
	case SESS_STATE_DEAD:
		Scr_AddConstString(scr_const.dead);
		break;
	case SESS_STATE_SPECTATOR:
		Scr_AddConstString(scr_const.spectator);
		break;
	case SESS_STATE_INTERMISSION:
		Scr_AddConstString(scr_const.intermission);
		break;
	default:
		break;
	}
}

/*
===============
ClientScr_GetSessionTeam
===============
*/
void ClientScr_GetSessionTeam( gclient_t *pSelf, const game_client_field_t *pField )
{
	assert(pSelf);

	switch ( pSelf->sess.state.team )
	{
	case TEAM_FREE:
		Scr_AddConstString(scr_const.none);
		break;
	case TEAM_AXIS:
		Scr_AddConstString(scr_const.axis);
		break;
	case TEAM_ALLIES:
		Scr_AddConstString(scr_const.allies);
		break;
	case TEAM_SPECTATOR:
		Scr_AddConstString(scr_const.spectator);
		break;
	default:
		break;
	}
}
