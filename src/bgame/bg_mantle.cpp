#include "../qcommon/qcommon.h"
#include "bg_public.h"

dvar_t *mantle_enable;
dvar_t *mantle_debug;
dvar_t *mantle_check_range;
dvar_t *mantle_check_radius;
dvar_t *mantle_check_angle;
dvar_t *mantle_view_yawcap;

const char *s_mantleAnimNames[] =
{
	"mp_mantle_root",
	"mp_mantle_up_57",
	"mp_mantle_up_51",
	"mp_mantle_up_45",
	"mp_mantle_up_39",
	"mp_mantle_up_33",
	"mp_mantle_up_27",
	"mp_mantle_up_21",
	"mp_mantle_over_high",
	"mp_mantle_over_mid",
	"player_mantle_over_low",
};

MantleAnimTransition s_mantleTrans[] =
{
	{ 1, 8, 57.000000 },
	{ 2, 8, 51.000000 },
	{ 3, 9, 45.000000 },
	{ 4, 9, 39.000000 },
	{ 5, 9, 33.000000 },
	{ 6, 10, 27.000000 },
	{ 7, 10, 21.000000 },
};

XAnim_s *s_mantleAnims;

void* MantleXAnimPrecacheAlloc(int size)
{
	return Hunk_AllocAlignInternal(size, 4);
}

bool Mantle_IsWeaponInactive(const playerState_s *ps)
{
	return mantle_enable->current.boolean
	       && (ps->pm_flags & 4) != 0
	       && s_mantleTrans[ps->mantleState.transIndex].overAnimIndex != 10;
}

int Mantle_GetUpLength(const MantleState *mstate)
{
	return XAnimGetLengthMsec(s_mantleAnims, s_mantleTrans[mstate->transIndex].upAnimIndex);
}

int Mantle_GetOverLength(const MantleState *mstate)
{
	if ( ((LOBYTE(mstate->flags) ^ 1) & 1) != 0 )
		return 0;
	else
		return XAnimGetLengthMsec(s_mantleAnims, s_mantleTrans[mstate->transIndex].overAnimIndex);
}

void Mantle_DebugPrint(const char *msg)
{
	if ( mantle_debug->current.boolean )
		Com_Printf("%s\n", msg);
}

void Mantle_GetAnimDelta(MantleState *mstate, int time, float *delta)
{
	float frac;
	int upTime;
	float trans[3];
	float rot[2];
	int overTime;

	upTime = Mantle_GetUpLength(mstate);
	overTime = Mantle_GetOverLength(mstate);

	if ( time > upTime )
	{
		frac = (float)(time - upTime) / (float)overTime;

		XAnimGetAbsDelta(s_mantleAnims, s_mantleTrans[mstate->transIndex].upAnimIndex, rot, trans, 1.0);
		XAnimGetAbsDelta(s_mantleAnims, s_mantleTrans[mstate->transIndex].overAnimIndex, rot, delta, frac);

		delta[0] = delta[0] + trans[0];
		delta[1] = delta[1] + trans[1];
		delta[2] = delta[2] + trans[2];
	}
	else
	{
		XAnimGetAbsDelta(s_mantleAnims, s_mantleTrans[mstate->transIndex].upAnimIndex, rot, delta, (float)time / (float)upTime);
	}

	VectorAngleMultiply(delta, mstate->yaw);
}

int Mantle_GetAnim(MantleState *mstate)
{
	int length;

	length = Mantle_GetUpLength(mstate);

	if ( mstate->timer > length )
		return s_mantleTrans[mstate->transIndex].overAnimIndex;
	else
		return s_mantleTrans[mstate->transIndex].upAnimIndex;
}

void Mantle_CalcEndPos(pmove_t *pm, MantleResults *mresults)
{
	playerState_s *ps;
	vec3_t maxs;
	vec3_t mins;
	vec3_t end;
	vec3_t start;
	trace_t trace;

	ps = pm->ps;

	if ( ((LOBYTE(mresults->flags) ^ 1) & 1) != 0 )
	{
		VectorCopy(mresults->ledgePos, mresults->endPos);
	}
	else
	{
		VectorCopy(ps->mins, mins);
		VectorCopy(ps->maxs, maxs);
		maxs[2] = 50.0;

		VectorCopy(mresults->ledgePos, start);
		VectorMA(start, 31.0, mresults->dir, start);
		VectorCopy(start, end);
		end[2] = end[2] - 18.0;

		PM_trace(pm, &trace, start, mins, maxs, end, ps->clientNum, pm->tracemask);

		if ( trace.startsolid || trace.fraction < 1.0 )
		{
			mresults->flags &= ~1u;
			VectorCopy(mresults->ledgePos, mresults->endPos);
		}
		else
		{
			VectorCopy(end, mresults->endPos);
			mresults->endPos[2] = (end[2] - start[2]) * trace.fraction + start[2];
		}
	}
}

int Mantle_FindTransition(float curHeight, float goalHeight)
{
	float height;
	int bestIndex;
	int transIndex;
	float bestDiff;

	height = goalHeight - curHeight;
	bestIndex = 0;
	bestDiff = fabs(s_mantleTrans[0].height - height);

	for ( transIndex = 1; transIndex < COUNT_OF(s_mantleTrans); ++transIndex )
	{
		if ( bestDiff > fabs(s_mantleTrans[transIndex].height - height) )
		{
			bestIndex = transIndex;
			bestDiff = fabs(s_mantleTrans[transIndex].height - height);
		}
	}

	return bestIndex;
}

void Mantle_Move(pmove_t *pm, playerState_s *ps, pml_t *pml)
{
	int upLength;
	float scale;
	int anim;
	int overLength;
	int prevTime;
	int time;
	vec3_t prevTrans;
	vec3_t trans;
	MantleState *mstate;

	if ( mantle_enable->current.boolean )
	{
		mstate = &ps->mantleState;
		ps->mantleState.flags &= ~8u;

		if ( (mstate->flags & 2) != 0 )
			BG_AddPredictableEventToPlayerstate(EV_STANCE_FORCE_CROUCH, 0, ps);

		upLength = Mantle_GetUpLength(mstate);
		overLength = Mantle_GetOverLength(mstate) + upLength;
		time = mstate->timer;
		mstate->timer = time + pml->msec;

		if ( mstate->timer > overLength )
			mstate->timer = overLength;

		prevTime = mstate->timer - time;
		Mantle_GetAnimDelta(mstate, time, prevTrans);
		Mantle_GetAnimDelta(mstate, mstate->timer, trans);
		anim = Mantle_GetAnim(mstate) + 20;
		BG_AnimScriptAnimation(ps, AISTATE_COMBAT, (scriptAnimMoveTypes_t)anim, 1);
		VectorSubtract(trans, prevTrans, trans);
		VectorAdd(trans, ps->origin, ps->origin);
		scale = 1.0 / ((float)prevTime * 0.001);
		VectorScale(trans, scale, ps->velocity);

		if ( mstate->timer == overLength )
		{
			ps->pm_flags &= ~4u;
			pm->mantleStarted = 0;

			if ( (mstate->flags & 1) != 0 )
				BG_AnimScriptEvent(ps, ANIM_ET_JUMP, 0, 1);

			if ( (mstate->flags & 4) != 0 )
			{
				BG_AddPredictableEventToPlayerstate(EV_STANCE_FORCE_STAND, 0, ps);
				ps->eFlags &= ~0x4000u;
			}
		}
	}
}

void Mantle_Start(pmove_t *pm, playerState_s *ps, MantleResults *mresults)
{
	int length;
	int time;
	vec3_t trans;
	MantleState *mstate;

	mstate = &ps->mantleState;
	ps->mantleState.yaw = vectoyaw(mresults->dir);
	mstate->timer = 0;
	mstate->transIndex = Mantle_FindTransition(mresults->startPos[2], mresults->ledgePos[2]);
	mstate->flags = mresults->flags;
	length = Mantle_GetUpLength(mstate);
	time = Mantle_GetOverLength(mstate) + length;
	Mantle_GetAnimDelta(mstate, time, trans);
	VectorSubtract(mresults->endPos, trans, ps->origin);
	ps->pm_flags |= 4u;
	ps->eFlags |= 0x4000u;
	VectorCopy(mresults->endPos, pm->mantleEndPos);
	pm->mantleDuration = time;
	pm->mantleStarted = 1;
}

bool Mantle_CheckLedge(pmove_t *pmove, pml_t *pml, MantleResults *mresults, float units)
{
	float target;
	playerState_s *ps;
	vec3_t maxs;
	vec3_t mins;
	vec3_t end;
	vec3_t start;
	trace_t trace;
	float direction;

	ps = pmove->ps;
	direction = pmove->ps->maxs[0];
	Mantle_DebugPrint(va("Checking for ledge at %f units", units));
	VectorSet(mins, -direction, -direction, 0.0);
	target = direction + direction;
	VectorSet(maxs, direction, direction, target);
	VectorCopy(mresults->startPos, start);
	start[2] = start[2] + units;
	VectorMA(start, 16.0, mresults->dir, end);
	PM_trace(pmove, &trace, start, mins, maxs, end, ps->clientNum, pmove->tracemask);

	if ( trace.startsolid || trace.fraction < 1.0 )
	{
		Mantle_DebugPrint("Mantle Failed: Ledge is too far away");
		return 0;
	}
	else
	{
		VectorCopy(end, start);
		end[2] = mresults->startPos[2] + 18.0;

		PM_trace(pmove, &trace, start, mins, maxs, end, ps->clientNum, pmove->tracemask);

		if ( trace.startsolid || trace.fraction == 1.0 )
		{
			Mantle_DebugPrint("Mantle Failed: Can't find ledge");
			return 0;
		}
		else if ( trace.normal[2] >= 0.69999999 )
		{
			VectorCopy(end, mresults->ledgePos);
			mresults->ledgePos[2] = (end[2] - start[2]) * trace.fraction + start[2];
			maxs[2] = 50.0;

			PM_trace(pmove, &trace, mresults->ledgePos, mins, maxs, mresults->ledgePos, ps->clientNum, pmove->tracemask);

			if ( trace.startsolid )
			{
				Mantle_DebugPrint("Mantle Failed: Player can't fit crouched on ledge");
				return 0;
			}
			else
			{
				ps->mantleState.flags |= 8u;
				mresults->flags |= 8u;
				Mantle_DebugPrint("Mantle available!");

				if ( (pmove->cmd.buttons & 0x400) != 0 )
				{
					Mantle_CalcEndPos(pmove, mresults);

					if ( (ps->eFlags & 4) == 0 )
					{
						PM_trace(
						    pmove,
						    &trace,
						    mresults->ledgePos,
						    ps->mins,
						    ps->maxs,
						    mresults->ledgePos,
						    ps->clientNum,
						    pmove->tracemask);

						if ( trace.startsolid )
							mresults->flags |= 2u;

						PM_trace(
						    pmove,
						    &trace,
						    mresults->endPos,
						    ps->mins,
						    ps->maxs,
						    mresults->endPos,
						    ps->clientNum,
						    pmove->tracemask);

						if ( !trace.startsolid )
							mresults->flags |= 4u;
					}

					Mantle_Start(pmove, ps, mresults);
					return 1;
				}
				else
				{
					return 1;
				}
			}
		}
		else
		{
			Mantle_DebugPrint("Mantle Failed: Ledge is too steep");
			return 0;
		}
	}
}

bool Mantle_FindMantleSurface(pmove_t *pm, pml_t *pml, trace_t *trace, float *mantleDir)
{
	playerState_s *ps;
	float normal;
	float angle;
	float radiusCheck;
	float range;
	vec3_t forward;
	vec3_t end;
	vec3_t start;
	vec3_t maxs;
	vec3_t mins;
	float radius;

	ps = pm->ps;
	radius = pm->ps->maxs[0];
	VectorSet(mins, -mantle_check_radius->current.decimal, -mantle_check_radius->current.decimal, ps->mins[2]);
	VectorSet(maxs, mantle_check_radius->current.decimal, mantle_check_radius->current.decimal, ps->maxs[2]);
	radiusCheck = radius - mantle_check_radius->current.decimal;
	range = mantle_check_range->current.decimal + radiusCheck;
	VectorCopy(pml->forward, forward);
	forward[2] = 0.0;
	Vec3Normalize(forward);
	VectorMA(ps->origin, -radiusCheck, forward, start);
	VectorMA(ps->origin, range, forward, end);

	PM_trace(pm, trace, start, mins, maxs, end, ps->clientNum, 0x1000000);

	if ( trace->startsolid || trace->allsolid )
	{
		Mantle_DebugPrint("Mantle Failed: Mantle brush is too thick");
		return 0;
	}
	else if ( trace->fraction == 1.0 )
	{
		Mantle_DebugPrint("Mantle Failed: No mantle surface found");
		return 0;
	}
	else if ( (trace->surfaceFlags & 0x6000000) != 0 )
	{
		mantleDir[0] = -trace->normal[0];
		mantleDir[1] = -trace->normal[1];
		mantleDir[2] = 0.0;

		normal = Vec3Normalize(mantleDir);

		if ( normal >= 0.000099999997 )
		{
			angle = VectorsLengthSquared(forward, mantleDir);

			if ( acos(angle) * 57.29577951308232 <= mantle_check_angle->current.decimal )
			{
				return 1;
			}
			else
			{
				Mantle_DebugPrint("Mantle Failed: Player is not facing mantle surface");
				return 0;
			}
		}
		else
		{
			Mantle_DebugPrint("Mantle Failed: Mantle surface has vertical normal");
			return 0;
		}
	}
	else
	{
		Mantle_DebugPrint("Mantle Failed: No mantle surface with MANTLE_ON or MANTLE_OVER found");
		return 0;
	}
}

void Mantle_ClearHint(playerState_s *ps)
{
	ps->mantleState.flags &= ~8u;
}

void Mantle_Check(pmove_t *pmove, pml_t *pml)
{
	playerState_s *ps;
	vec3_t mantleDir;
	trace_t trace;
	MantleResults mresults;

	Mantle_DebugPrint("====== Mantle Debug ======");

	if ( mantle_enable->current.boolean )
	{
		ps = pmove->ps;
		Mantle_ClearHint(pmove->ps);

		if ( ps->pm_type <= 5 )
		{
			if ( (ps->pm_flags & 4) != 0 )
			{
				Mantle_DebugPrint("Mantle Failed: Player already mantling");
			}
			else if ( (ps->eFlags & 0xC) != 0 )
			{
				Mantle_DebugPrint("Mantle Failed: Player not standing");
			}
			else if ( ps->weaponstate < WEAPON_BINOCULARS_INIT || ps->weaponstate > WEAPON_BINOCULARS_END )
			{
				if ( Mantle_FindMantleSurface(pmove, pml, &trace, mantleDir) )
				{
					memset(&mresults, 0, sizeof(mresults));
					VectorCopy(mantleDir, mresults.dir);
					VectorCopy(ps->origin, mresults.startPos);

					if ( (trace.surfaceFlags & 0x4000000) != 0 )
						mresults.flags |= 1u;

					if ( !Mantle_CheckLedge(pmove, pml, &mresults, 60.0) && !Mantle_CheckLedge(pmove, pml, &mresults, 40.0) )
						Mantle_CheckLedge(pmove, pml, &mresults, 20.0);
				}
			}
			else
			{
				Mantle_DebugPrint("Mantle Failed: Player using binoculars");
			}
		}
		else
		{
			Mantle_DebugPrint("Mantle Failed: Player is dead");
		}
	}
	else
	{
		Mantle_DebugPrint("Mantle Failed: Not enabled");
	}
}

void Mantle_CreateAnims(void *(*Alloc)(int))
{
	float length;
	int animIndex;
	vec3_t rot;
	vec3_t delta;
	int i;

	if ( !s_mantleAnims )
	{
		s_mantleAnims = XAnimCreateAnims("PLAYER_MANTLE", COUNT_OF(s_mantleAnimNames), Alloc);
		XAnimBlend(s_mantleAnims, 0, s_mantleAnimNames[0], 1, COUNT_OF(s_mantleAnimNames) - 1, 0);

		for ( animIndex = 1; animIndex < COUNT_OF(s_mantleAnimNames); ++animIndex )
		{
			XAnimPrecache(s_mantleAnimNames[animIndex], MantleXAnimPrecacheAlloc);
			XAnimCreate(s_mantleAnims, animIndex, s_mantleAnimNames[animIndex]);
		}

		for ( i = 0; i < COUNT_OF(s_mantleTrans); ++i )
		{
			animIndex = s_mantleTrans[i].upAnimIndex;
			XAnimGetAbsDelta(s_mantleAnims, animIndex, rot, delta, 1.0);
			length = delta[0] - 16.0;

			if ( fabs(length) > 1.0 )
				Com_Error(
				    ERR_DROP,
				    "Mantle anim [%s] has X translation %f, should be %f\n",
				    s_mantleAnimNames[animIndex],
				    delta[0],
				    16.0);

			if ( fabs(delta[1]) > 1.0 )
				Com_Error(
				    ERR_DROP,
				    "Mantle anim [%s] has Y translation %f, should be %f\n",
				    s_mantleAnimNames[animIndex],
				    delta[1],
				    0.0);

			length = delta[2] - s_mantleTrans[i].height;

			if ( fabs(length) > 1.0 )
				Com_Error(
				    ERR_DROP,
				    "Mantle anim [%s] has Z translation %f, should be %f\n",
				    s_mantleAnimNames[animIndex],
				    delta[2],
				    s_mantleTrans[i].height);

			animIndex = s_mantleTrans[i].overAnimIndex;
			XAnimGetAbsDelta(s_mantleAnims, animIndex, rot, delta, 1.0);
			length = delta[0] - 31.0;

			if ( fabs(length) > 1.0 )
				Com_Error(
				    ERR_DROP,
				    "Mantle anim [%s] has X translation %f, should be %f\n",
				    s_mantleAnimNames[animIndex],
				    delta[0],
				    31.0);

			if ( fabs(delta[1]) > 1.0 )
				Com_Error(
				    ERR_DROP,
				    "Mantle anim [%s] has Y translation %f, should be %f\n",
				    s_mantleAnimNames[animIndex],
				    delta[1],
				    0.0);

			length = delta[2] - -18.0;

			if ( fabs(length) > 1.0 )
				Com_Error(
				    ERR_DROP,
				    "Mantle anim [%s] has Z translation %f, should be %f\n",
				    s_mantleAnimNames[animIndex],
				    delta[2],
				    -18.0);
		}
	}
}

void Mantle_ShutdownAnims()
{
	s_mantleAnims = 0;
}

void Mantle_RegisterDvars()
{
	mantle_enable = Dvar_RegisterBool("mantle_enable", 1, 0x1180u);
	mantle_debug = Dvar_RegisterBool("mantle_debug", 0, 0x1180u);
	mantle_check_range = Dvar_RegisterFloat("mantle_check_range", 20.0, 0.0, 128.0, 0x1180u);
	mantle_check_radius = Dvar_RegisterFloat("mantle_check_radius", 0.1, 0.0, 15.0, 0x1180u);
	mantle_check_angle = Dvar_RegisterFloat("mantle_check_angle", 60.0, 0.0, 180.0, 0x1180u);
	mantle_view_yawcap = Dvar_RegisterFloat("mantle_view_yawcap", 60.0, 0.0, 180.0, 0x1180u);
}