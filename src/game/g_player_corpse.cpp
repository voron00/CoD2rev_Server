#include "../qcommon/qcommon.h"
#include "g_shared.h"

int G_GetPlayerCorpseIndex(gentity_s *ent)
{
	int i;

	for ( i = 0; i < 8; ++i )
	{
		if ( g_scr_data.playerCorpseInfo[i].entnum == ent->s.number )
			return i;
	}

	return 0;
}

void G_GetAnimDeltaForCorpse(gentity_s *ent, corpseInfo_t *ci, float *originChange)
{
	vec3_t rot;

	XAnimCalcDelta(ci->tree, 0, rot, originChange, 1);
}

void G_BounceCorpse(gentity_s *ent, corpseInfo_t *corpseInfo, trace_t *trace, float *endpos)
{
	float vAngles[3];
	float vAxis[3][3];

	VectorClear(ent->s.pos.trDelta);

	if ( trace->allsolid || trace->normal[2] > 0.0 )
	{
		corpseInfo->falling = 0;
		ent->s.pos.trType = TR_INTERPOLATE;
		VectorCopy(endpos, ent->s.pos.trBase);
		ent->s.pos.trTime = 0;
		ent->s.pos.trDuration = 0;
		VectorClear(ent->s.pos.trDelta);
		ent->s.groundEntityNum = trace->hitId;

		if ( trace->allsolid )
		{
			G_SetAngle(ent, ent->r.currentAngles);
		}
		else
		{
			VectorCopy(trace->normal, vAxis[2]);
			AngleVectors(ent->r.currentAngles, vAxis[0], 0, 0);
			Vec3Cross(vAxis[2], vAxis[0], vAxis[1]);
			Vec3Cross(vAxis[1], vAxis[2], vAxis[0]);
			AxisToAngles(vAxis, vAngles);
			G_SetAngle(ent, vAngles);
		}

		SV_LinkEntity(ent);
	}
	else
	{
		VectorAdd(ent->r.currentOrigin, trace->normal, ent->r.currentOrigin);
		VectorCopy(ent->r.currentOrigin, ent->s.pos.trBase);
		ent->s.pos.trTime = level.time;
	}
}

void G_RunCorpseMove(gentity_s *ent)
{
	bool isBounce;
	vec3_t delta;
	vec3_t right;
	vec3_t forward;
	vec3_t deltaChange;
	corpseInfo_t *corpseInfo;
	int index;
	vec3_t start;
	vec3_t endpos;
	int contentmask;
	int contents;
	trace_t trace;
	vec3_t endPos;

	index = G_GetPlayerCorpseIndex(ent);
	corpseInfo = &g_scr_data.playerCorpseInfo[index];
	G_GetAnimDeltaForCorpse(ent, corpseInfo, deltaChange);
	isBounce = 0;

	if ( !corpseInfo->falling && VectorLengthSquared(deltaChange) > 1.0 )
		isBounce = 1;

	if ( corpseInfo->falling || isBounce )
	{
		BG_EvaluateTrajectory(&ent->s.pos, level.time, endPos);

		if ( isBounce )
		{
			AngleVectors(ent->r.currentAngles, forward, right, 0);
			VectorScale(right, -1.0, delta);
			Vec3Normalize(forward);
			Vec3Normalize(delta);
			VectorMA(endPos, deltaChange[0], forward, endPos);
			VectorMA(endPos, deltaChange[1], delta, endPos);
		}

		contentmask = ent->clipmask;
		G_TraceCapsule(&trace, ent->r.currentOrigin, ent->r.mins, ent->r.maxs, endPos, ent->r.ownerNum, contentmask);
		Vec3Lerp(ent->r.currentOrigin, endPos, trace.fraction, endpos);
		VectorCopy(endpos, ent->r.currentOrigin);

		if ( trace.startsolid )
			trace.fraction = 0.0;

		SV_LinkEntity(ent);
		G_RunThink(ent);

		if ( ent->r.inuse )
		{
			if ( trace.fraction == 1.0 )
			{
				if ( !corpseInfo->falling && isBounce )
				{
					ent->s.pos.trType = TR_INTERPOLATE;
					VectorCopy(endpos, ent->s.pos.trBase);
					ent->s.pos.trTime = 0;
					ent->s.pos.trDuration = 0;
					VectorClear(ent->s.pos.trDelta);
					endPos[2] = endPos[2] - 1.0;

					if ( G_TraceCapsuleComplete(
					            ent->r.currentOrigin,
					            ent->r.mins,
					            ent->r.maxs,
					            endPos,
					            ent->r.ownerNum,
					            contentmask) )
					{
						corpseInfo->falling = 1;
						ent->s.pos.trType = TR_GRAVITY;
						VectorCopy(endpos, ent->s.pos.trBase);
						VectorClear(ent->s.pos.trDelta);
						VectorMA(ent->s.pos.trDelta, deltaChange[0], forward, ent->s.pos.trDelta);
						VectorMA(ent->s.pos.trDelta, deltaChange[1], delta, ent->s.pos.trDelta);
						ent->s.pos.trTime = level.time;
						ent->s.pos.trDuration = 0;
					}
				}
			}
			else
			{
				contents = SV_PointContents(ent->r.currentOrigin, -1, 0x80000000);

				if ( contents )
				{
					G_FreeEntity(ent);
				}
				else if ( corpseInfo->falling )
				{
					if ( trace.allsolid )
					{
						VectorCopy(ent->r.currentOrigin, start);
						start[2] = start[2] + 64.0;

						G_TraceCapsule(
						    &trace,
						    start,
						    ent->r.mins,
						    ent->r.maxs,
						    ent->r.currentOrigin,
						    ent->r.ownerNum,
						    contentmask & 0xFFFEFFFF);

						if ( !trace.allsolid )
						{
							Vec3Lerp(start, ent->r.currentOrigin, trace.fraction, endpos);
							VectorCopy(endpos, ent->r.currentOrigin);
						}
					}

					G_BounceCorpse(ent, corpseInfo, &trace, endpos);
				}
			}
		}
	}
}

void G_RunCorpseAnimate(gentity_s *ent)
{
	DObj_s *obj;
	corpseInfo_t *corpseInfo;

	corpseInfo = &g_scr_data.playerCorpseInfo[G_GetPlayerCorpseIndex(ent)];
	obj = Com_GetServerDObj(ent->s.number);
	BG_UpdatePlayerDObj(obj, &ent->s, &corpseInfo->ci, 0);
	obj = Com_GetServerDObj(ent->s.number);

	if ( obj )
		BG_PlayerAnimation(obj, &ent->s, &corpseInfo->ci);
}

void G_RunCorpse(gentity_s *ent)
{
	G_RunCorpseMove(ent);
	G_RunCorpseAnimate(ent);
	G_RunThink(ent);
}

void G_CorpseFree(gentity_s *ent)
{
	g_scr_data.playerCorpseInfo[G_GetPlayerCorpseIndex(ent)].entnum = -1;
}