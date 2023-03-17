#include "../qcommon/qcommon.h"
#include "g_shared.h"

#ifdef TESTING_LIBRARY
#define level (*((level_locals_t*)( 0x0859B400 )))
#else
level_locals_t level;
#endif

#ifdef TESTING_LIBRARY
#define g_entities ((gentity_t*)( 0x08665480 ))
#else
gentity_t g_entities[MAX_GENTITIES];
#endif

#ifdef TESTING_LIBRARY
#define g_clients ((gclient_t*)( 0x086F1480 ))
#else
gclient_t g_clients[MAX_CLIENTS];
#endif

void Scr_LocalizationError(int iParm, const char *pszErrorMessage)
{
	Com_Error(ERR_LOCALIZATION, pszErrorMessage);
}

static signed int SortRanks(const void *num1, const void *num2)
{
	gclient_t *client1;
	gclient_t *client2;

	client2 = &level.clients[*(int *)num1];
	client1 = &level.clients[*(int *)num2];

	if ( client2->sess.connected == CS_ZOMBIE )
		return 1;

	if ( client1->sess.connected == CS_ZOMBIE )
		return -1;

	if ( client2->sess.state.team == TEAM_SPECTATOR && client1->sess.state.team == TEAM_SPECTATOR )
	{
		if ( client2 >= client1 )
			return client2 > client1;
		else
			return -1;
	}
	else if ( client2->sess.state.team == TEAM_SPECTATOR )
	{
		return 1;
	}
	else if ( client1->sess.state.team == TEAM_SPECTATOR )
	{
		return -1;
	}
	else if ( client2->sess.score <= client1->sess.score )
	{
		if ( client2->sess.score >= client1->sess.score )
		{
			if ( client2->sess.deaths >= client1->sess.deaths )
				return client2->sess.deaths > client1->sess.deaths;
			else
				return -1;
		}
		else
		{
			return 1;
		}
	}
	else
	{
		return -1;
	}
}

void CalculateRanks()
{
	int i;

	level.numConnectedClients = 0;
	level.numVotingClients = 0;

	for ( i = 0; i < level.maxclients; ++i )
	{
		if ( level.clients[i].sess.connected )
		{
			level.sortedClients[level.numConnectedClients++] = i;

			if ( level.clients[i].sess.state.team != TEAM_SPECTATOR && level.clients[i].sess.connected == CS_CONNECTED )
				++level.numVotingClients;
		}
	}

	qsort(level.sortedClients, level.numConnectedClients, 4, SortRanks);
	level.bUpdateScoresForIntermission = 1;
}

float G_GetFogOpaqueDistSqrd()
{
	return level.fFogOpaqueDistSqrd;
}