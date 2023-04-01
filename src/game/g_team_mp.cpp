#include "../qcommon/qcommon.h"
#include "g_shared.h"

qboolean OnSameTeam(gentity_s *ent1, gentity_s *ent2)
{
	if ( !ent1->client || !ent2->client )
		return 0;

	if ( ent1->client->sess.state.team )
		return ent1->client->sess.state.team == ent2->client->sess.state.team;

	return 0;
}