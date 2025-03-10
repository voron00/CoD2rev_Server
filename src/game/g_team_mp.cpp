#include "../qcommon/qcommon.h"
#include "g_shared.h"

extern dvar_t *g_maxclients;

qboolean OnSameTeam(gentity_s *ent1, gentity_s *ent2)
{
	if ( !ent1->client || !ent2->client )
		return 0;

	if ( ent1->client->sess.state.team )
		return ent1->client->sess.state.team == ent2->client->sess.state.team;

	return 0;
}

void TeamplayInfoMessage(gentity_s *ent)
{
	trace_t trace;
	vec3_t vEnd;
	vec3_t vStart;
	vec3_t vForward;
	int ident_health;
	int ident_num;

	if ( ent->client->sess.sessionState )
	{
		G_GetPlayerViewOrigin(ent, vStart);
		G_GetPlayerViewDirection(ent, vForward, 0, 0);

		if ( ent->client->ps.viewHeightCurrent < 8.0 )
			vStart[2] = 8.0 - ent->client->ps.viewHeightCurrent + vStart[2];

		VectorMA(vStart, 8192.0, vForward, vEnd);
	}
	else
	{
		if ( ent->client->sess.state.team == TEAM_FREE )
		{
			ent->client->ps.stats[STAT_IDENT_CLIENT_NUM] = -1;
#if PROTOCOL_VERSION != 119
			ent->client->ps.stats[STAT_IDENT_CLIENT_HEALTH] = 0;
#endif
			return;
		}

		G_GetPlayerViewOrigin(ent, vStart);
		G_GetPlayerViewDirection(ent, vForward, 0, 0);
		VectorMA(vStart, 8192.0, vForward, vEnd);
	}

	G_TraceCapsule(&trace, vStart, vec3_origin, vec3_origin, vEnd, ent->client->ps.clientNum, 33554433);
	ident_num = trace.entityNum;

	if ( trace.entityNum <= 0x3Fu
	        && g_entities[ident_num].client
	        && (!G_IsPlaying(ent) || g_entities[ident_num].client->sess.state.team == ent->client->sess.state.team) )
	{
		ident_health = g_entities[ident_num].health;
	}
	else
	{
		ident_num = -1;
		ident_health = 0;
	}

	ent->client->ps.stats[STAT_IDENT_CLIENT_NUM] = ident_num;
#if PROTOCOL_VERSION != 119
	ent->client->ps.stats[STAT_IDENT_CLIENT_HEALTH] = ident_health;
#endif
}

void CheckTeamStatus()
{
	gentity_s *ent;
	int i;

	if ( level.time - level.lastTeammateHealthTime > 0 )
	{
		level.lastTeammateHealthTime = level.time;

		for ( i = 0; i < g_maxclients->current.integer; ++i )
		{
			ent = &g_entities[i];

			if ( ent->r.inuse )
			{
				if ( (ent->client->ps.pm_flags & 0x400000) == 0 )
					TeamplayInfoMessage(ent);
			}
		}
	}
}