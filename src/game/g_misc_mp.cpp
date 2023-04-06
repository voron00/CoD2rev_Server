#include "../qcommon/qcommon.h"
#include "g_shared.h"

void TeleportPlayer(gentity_s *player, float *origin, float *angles)
{
	SV_UnlinkEntity(player);
	VectorCopy(origin, player->client->ps.origin);
	player->client->ps.origin[2] = player->client->ps.origin[2] + 1.0;
	player->client->ps.eFlags ^= 2u;
	SetClientViewAngle(player, angles);
	BG_PlayerStateToEntityState(&player->client->ps, &player->s, 1, 1u);
	VectorCopy(player->client->ps.origin, player->r.currentOrigin);

	if ( player->r.linked )
		SV_LinkEntity(player);
}

void SP_info_null(gentity_s *self)
{
	G_FreeEntity(self);
}

void SP_info_notnull(gentity_s *self)
{
	G_SetOrigin(self, self->r.currentOrigin);
}

void SP_light(gentity_s *self)
{
	G_FreeEntity(self);
}

void SP_misc_model(gentity_s *self)
{
	G_FreeEntity(self);
}

void SP_corona(gentity_s *self)
{
	G_FreeEntity(self);
}