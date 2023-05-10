#include "../qcommon/qcommon.h"
#include "g_shared.h"

struct pushed_t
{
	gentity_s *ent;
	vec3_t origin;
	vec3_t angles;
	float deltayaw;
};

pushed_t pushed[1024];
pushed_t *pushed_p;

/*
==================
G_TryPushingEntity

Returns qfalse if the move is blocked
==================
*/
qboolean G_TryPushingEntity( gentity_t *check, gentity_t *pusher, vec3_t move, vec3_t amove )
{
	vec3_t org, org2, move2, amove2;
	gentity_t   *block;
	vec3_t matrix[3], transpose[3];
	float x, fx, y, fy, z, fz;
#define JITTER_INC  4
#define JITTER_MAX  ( check->r.maxs[0] / 2.0 )

	// figure movement due to the pusher's amove
	VectorAdd(check->r.currentOrigin, move, org);
	BG_CreateRotationMatrix(amove, transpose);
	BG_TransposeMatrix(transpose, matrix);
	VectorSubtract(org, pusher->r.currentOrigin, amove2);
	VectorCopy(amove2, org2);
	BG_RotatePoint(org2, matrix);
	VectorSubtract(org2, amove2, move2);
	VectorAdd(org, move2, org);

	block = G_TestEntityPosition(check, org);

	if ( !block )
	{
		// pushed ok
		if ( check->s.groundEntityNum != pusher->s.number )
			check->s.groundEntityNum = 1023;

		// try moving the contacted entity
		VectorCopy(org, check->r.currentOrigin);
		VectorCopy(org, check->s.pos.trBase);

		if ( check->client )
		{
			// make sure the client's view rotates when on a rotating mover
			// RF, this is done client-side now
			// ydnar: only do this if player is prone or using set mortar
			check->client->ps.delta_angles[1] += ANGLE2SHORT( amove[YAW] );
			VectorCopy(org, check->client->ps.origin);
		}

		++pushed_p;
		return qtrue;
	}

	// RF, if still not valid, move them around to see if we can find a good spot
	if ( JITTER_MAX > JITTER_INC )
	{
		VectorCopy(org, amove2);

		for ( z = 0; z < JITTER_MAX; z += JITTER_INC )
			for ( fz = -z; fz <= z; fz += 2 * z )
			{
				for ( x = JITTER_INC; x < JITTER_MAX; x += JITTER_INC )
					for ( fx = -x; fx <= x; fx += 2 * x )
					{
						for ( y = JITTER_INC; y < JITTER_MAX; y += JITTER_INC )
							for ( fy = -y; fy <= y; fy += 2 * y )
							{
								VectorSet(move2, fx, fy, fz);
								VectorAdd(amove2, move2, org2);

								//
								// do the test
								block = G_TestEntityPosition( check, org2 );
								if ( !block )
								{
									// pushed ok
									if ( check->s.groundEntityNum != pusher->s.number )
										check->s.groundEntityNum = 1023;

									VectorCopy(org2, check->r.currentOrigin);
									VectorCopy(org2, check->s.pos.trBase);

									if ( check->client )
									{
										check->client->ps.delta_angles[1] += ANGLE2SHORT( amove[YAW] );
										VectorCopy(org2, check->client->ps.origin);
									}

									++pushed_p;
									return qtrue;
								}
							}
					}
				if ( !fz )
				{
					break;
				}
			}
	}

	// if it is ok to leave in the old position, do it
	// this is only relevent for riding entities, not pushed
	// Sliding trapdoors can cause this.
	block = G_TestEntityPosition( check, check->r.currentOrigin );

	if ( !block )
	{
		check->s.groundEntityNum = 1023;
		return 1;
	}

	// blocked
	return qfalse;
}

gentity_s* G_TestEntityPosition(gentity_s *ent, float *origin)
{
	int contentmask;
	trace_t trace;

	if ( ent->clipmask )
	{
		if ( (ent->r.contents & 0x4000000) != 0 )
			return 0;

		contentmask = ent->clipmask;
	}
	else
	{
		contentmask = 2065;
	}

	if ( ent->s.eType == ET_MISSILE )
		G_TraceCapsule(&trace, origin, ent->r.mins, ent->r.maxs, origin, ent->r.ownerNum, contentmask);
	else
		G_TraceCapsule(&trace, origin, ent->r.mins, ent->r.maxs, origin, ent->s.number, contentmask);

	if ( trace.startsolid || trace.allsolid )
		return &g_entities[trace.entityNum];
	else
		return 0;
}

bool G_MoverPush(gentity_s *pusher, vec3_t move, vec3_t amove, gentity_s **obstacle)
{
	bool pushed;
	float radius;
	vec3_t totalMaxs;
	vec3_t totalMins;
	int k;
	int entities;
	int checkEntityList[1024];
	int entityList[1024];
	vec3_t maxs;
	vec3_t mins;
	gentity_s *check;
	int j;
	int i;

	*obstacle = 0;
	pushed = 1;

	if ( pusher->r.currentAngles[0] == 0.0
	        && pusher->r.currentAngles[1] == 0.0
	        && pusher->r.currentAngles[2] == 0.0
	        && *amove == 0.0
	        && amove[1] == 0.0
	        && amove[2] == 0.0 )
	{
		for ( i = 0; i <= 2; ++i )
		{
			mins[i] = pusher->r.absmin[i] + move[i];
			maxs[i] = pusher->r.absmax[i] + move[i];
		}

		VectorCopy(pusher->r.absmin, totalMins);
		VectorCopy(pusher->r.absmax, totalMaxs);
	}
	else
	{
		radius = RadiusFromBounds(pusher->r.mins, pusher->r.maxs);

		for ( i = 0; i <= 2; ++i )
		{
			mins[i] = pusher->r.currentOrigin[i] - radius + move[i];
			maxs[i] = pusher->r.currentOrigin[i] + radius + move[i];
			totalMins[i] = pusher->r.currentOrigin[i] - radius;
			totalMaxs[i] = pusher->r.currentOrigin[i] + radius;
		}
	}

	for ( i = 0; i <= 2; ++i )
	{
		if ( move[i] <= 0.0 )
			totalMins[i] = totalMins[i] + move[i];
		else
			totalMaxs[i] = totalMaxs[i] + move[i];
	}

	SV_UnlinkEntity(pusher);
	entities = CM_AreaEntities(totalMins, totalMaxs, entityList, 1024, 33554816);
	VectorAdd(pusher->r.currentOrigin, move, pusher->r.currentOrigin);
	VectorAdd(pusher->r.currentAngles, amove, pusher->r.currentAngles);
	SV_LinkEntity(pusher);
	k = 0;

	for ( j = 0; j < entities; ++j )
	{
		check = &g_entities[entityList[j]];

		if ( (check->s.eType == ET_MISSILE
		        || check->s.eType == ET_ITEM
		        || check->s.eType == ET_PLAYER
		        || check->physicsObject)
		        && (check->s.groundEntityNum == pusher->s.number
		            || check->r.absmin[0] < maxs[0]
		            && check->r.absmin[1] < maxs[1]
		            && check->r.absmin[2] < maxs[2]
		            && mins[0] < check->r.absmax[0]
		            && mins[1] < check->r.absmax[1]
		            && mins[2] < check->r.absmax[2]
		            && G_TestEntityPosition(check, check->r.currentOrigin) == pusher) )
		{
			checkEntityList[k++] = entityList[j];
		}
	}

	for ( j = 0; j < k; ++j )
	{
		check = &g_entities[checkEntityList[j]];
		SV_UnlinkEntity(check);
	}

	for ( j = 0; j < k; ++j )
	{
		check = &g_entities[checkEntityList[j]];
		pushed_p->ent = check;
		VectorCopy(check->r.currentOrigin, pushed_p->origin);
		pushed_p->deltayaw = amove[1];

		if ( G_TryPushingEntity(check, pusher, move, amove) || check->s.eType == ET_ITEM )
		{
			SV_LinkEntity(check);
		}
		else if ( pusher->s.pos.trType == TR_SINE || pusher->s.apos.trType == TR_SINE )
		{
			G_Damage(check, pusher, pusher, 0, 0, 99999, 0, 9, HITLOC_NONE, 0);
		}
		else
		{
			*obstacle = check;
			pushed = 0;
		}
	}

	for ( j = 0; j < k; ++j )
	{
		check = &g_entities[checkEntityList[j]];
		SV_LinkEntity(check);
	}

	return pushed;
}

void G_MoverTeam(gentity_s *ent)
{
	void (*reached)(struct gentity_s *);
	void (*blocked)(struct gentity_s *, struct gentity_s *);
	pushed_t *push;
	gentity_s *part;
	vec3_t angles;
	vec3_t origin;
	gentity_s *obstacle;
	vec3_t amove;
	vec3_t move;

	obstacle = 0;
	pushed_p = pushed;

	BG_EvaluateTrajectory(&ent->s.pos, level.time, origin);
	BG_EvaluateTrajectory(&ent->s.apos, level.time, angles);

	VectorSubtract(origin, ent->r.currentOrigin, move);
	VectorSubtract(angles, ent->r.currentAngles, amove);

	if ( G_MoverPush(ent, move, amove, &obstacle) )
	{
		if ( ent->s.pos.trType )
		{
			if ( level.time >= ent->s.pos.trTime + ent->s.pos.trDuration )
			{
				reached = entityHandlers[ent->handler].reached;

				if ( reached )
					reached(ent);
			}
		}

		if ( ent->s.apos.trType )
		{
			if ( level.time >= ent->s.apos.trTime + ent->s.apos.trDuration )
			{
				reached = entityHandlers[ent->handler].reached;

				if ( reached )
					reached(ent);
			}
		}
	}
	else
	{
		for ( push = pushed_p - 1; push >= pushed; --push )
		{
			part = push->ent;

			VectorCopy(push->origin, push->ent->r.currentOrigin);
			VectorCopy(push->origin, part->s.pos.trBase);

			if ( part->client )
			{
				part->client->ps.delta_angles[1] -= ANGLE2SHORT(push->deltayaw);
				VectorCopy(push->origin, part->client->ps.origin);
			}

			SV_LinkEntity(part);
		}

		ent->s.pos.trTime += level.time - level.previousTime;
		ent->s.apos.trTime += level.time - level.previousTime;

		BG_EvaluateTrajectory(&ent->s.pos, level.time, ent->r.currentOrigin);
		BG_EvaluateTrajectory(&ent->s.apos, level.time, ent->r.currentAngles);

		SV_LinkEntity(ent);
		blocked = entityHandlers[ent->handler].blocked;

		if ( blocked )
			blocked(ent, obstacle);
	}
}

void G_RunMover(gentity_s *ent)
{
	if ( ent->tagInfo )
	{
		G_GeneralLink(ent);
	}
	else if ( ent->s.pos.trType || ent->s.apos.trType )
	{
		G_MoverTeam(ent);
	}

	G_RunThink(ent);
}