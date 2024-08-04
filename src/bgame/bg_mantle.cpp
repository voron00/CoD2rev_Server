#include "../qcommon/qcommon.h"
#include "bg_public.h"

dvar_t *mantle_enable;
dvar_t *mantle_debug;
dvar_t *mantle_check_range;
dvar_t *mantle_check_radius;
dvar_t *mantle_check_angle;
dvar_t *mantle_view_yawcap;

enum MantleAnims
{
	MANTLE_ROOT,
	MANTLE_UP_57,
	MANTLE_UP_51,
	MANTLE_UP_45,
	MANTLE_UP_39,
	MANTLE_UP_33,
	MANTLE_UP_27,
	MANTLE_UP_21,
	MANTLE_OVER_HIGH,
	MANTLE_OVER_MID,
	MANTLE_OVER_LOW,
	MANTLE_ANIM_COUNT
};

static const char *s_mantleAnimNames[] =
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

static MantleAnimTransition s_mantleTrans[] =
{
	{ 1, 8, 57 },
	{ 2, 8, 51 },
	{ 3, 9, 45 },
	{ 4, 9, 39 },
	{ 5, 9, 33 },
	{ 6, 10, 27 },
	{ 7, 10, 21 },
};

static XAnim *s_mantleAnims;

/*
==================
MantleXAnimPrecacheAlloc
==================
*/
void* MantleXAnimPrecacheAlloc( int size )
{
	return Hunk_AllocAlignInternal(size, 4);
}

/*
==================
Mantle_DebugPrint
==================
*/
void Mantle_DebugPrint( const char *msg )
{
	assert(msg);

	if ( !mantle_debug->current.boolean )
	{
		return;
	}

	Com_Printf("%s\n", msg);
}

/*
==================
Mantle_IsWeaponInactive
==================
*/
bool Mantle_IsWeaponInactive( const playerState_t *ps )
{
	return mantle_enable->current.boolean && ps->pm_flags & PMF_MANTLE && s_mantleTrans[ps->mantleState.transIndex].overAnimIndex != MANTLE_OVER_LOW;
}

/*
==================
Mantle_ClearHint
==================
*/
void Mantle_ClearHint( playerState_t *ps )
{
	assert(ps);
	ps->mantleState.flags &= ~8;
}

/*
==================
Mantle_ShutdownAnims
==================
*/
void Mantle_ShutdownAnims()
{
	s_mantleAnims = NULL;
}

/*
==================
Mantle_CapView
==================
*/
void Mantle_CapView( playerState_t *ps )
{
	float value;
	float delta;

	assert(ps);
	assert(ps->pm_flags & PMF_MANTLE);

	if ( !mantle_enable->current.boolean )
	{
		return;
	}

	delta = AngleDelta(ps->mantleState.yaw, ps->viewangles[1]);

	if ( delta < -mantle_view_yawcap->current.decimal || mantle_view_yawcap->current.decimal < delta )
	{
		while ( -mantle_view_yawcap->current.decimal > delta )
		{
			delta = delta + mantle_view_yawcap->current.decimal;
		}

		while ( delta > mantle_view_yawcap->current.decimal )
		{
			delta = delta - mantle_view_yawcap->current.decimal;
		}

		value = mantle_view_yawcap->current.decimal;

		if ( delta > 0.0 )
		{
			value = value * -1.0;
		}

		ps->delta_angles[1] += ANGLE2SHORT(delta);
		ps->viewangles[1] = AngleNormalize360Accurate(ps->mantleState.yaw + value);
	}
}

/*
==================
Mantle_Move
==================
*/
void Mantle_Move( pmove_t *pm, playerState_t *ps, pml_t *pml )
{
	int animIndex;
	int mantleLength;
	int deltaTime;
	int prevTime;
	vec3_t prevTrans;
	vec3_t trans;
	MantleState *mstate;

	assert(ps->pm_flags & PMF_MANTLE);
	assert(pml->msec >= 0);

	if ( !mantle_enable->current.boolean )
	{
		return;
	}

	mstate = &ps->mantleState;
	mstate->flags &= ~8;

	if ( mstate->flags & 2 )
	{
		BG_AddPredictableEventToPlayerstate(EV_STANCE_FORCE_CROUCH, 0, ps);
	}

	mantleLength = Mantle_GetOverLength(mstate) + Mantle_GetUpLength(mstate);
	prevTime = mstate->timer;
	mstate->timer = prevTime + pml->msec;

	if ( mstate->timer > mantleLength )
	{
		mstate->timer = mantleLength;
	}

	deltaTime = mstate->timer - prevTime;

	Mantle_GetAnimDelta(mstate, prevTime, prevTrans);
	Mantle_GetAnimDelta(mstate, mstate->timer, trans);

	animIndex = Mantle_GetAnim(mstate) + ANIM_MT_MANTLE_ROOT;
	BG_AnimScriptAnimation(ps, AISTATE_COMBAT, animIndex, qtrue);

	VectorSubtract(trans, prevTrans, trans);
	VectorAdd(trans, ps->origin, ps->origin);
	VectorScale(trans, 1.0 / (deltaTime * 0.001), ps->velocity);

	if ( mstate->timer != mantleLength )
	{
		return;
	}

	ps->pm_flags &= ~PMF_MANTLE;
	pm->mantleStarted = false;

	if ( mstate->flags & 1 )
	{
		BG_AnimScriptEvent(ps, ANIM_ET_JUMP, qfalse, qtrue);
	}

	if ( mstate->flags & 4 )
	{
		BG_AddPredictableEventToPlayerstate(EV_STANCE_FORCE_STAND, 0, ps);
		ps->eFlags &= ~EF_MANTLE;
	}
}

/*
==================
Mantle_CreateAnims
==================
*/
void Mantle_CreateAnims( void *(*xanimAlloc)(int) )
{
	int animIndex;
	int transIndex;
	vec3_t rot;
	vec3_t delta;

	if ( s_mantleAnims )
	{
		return;
	}

	s_mantleAnims = XAnimCreateAnims("PLAYER_MANTLE", ARRAY_COUNT(s_mantleAnimNames), xanimAlloc);
	assert(s_mantleAnims);

	XAnimBlend(s_mantleAnims, 0, s_mantleAnimNames[0], 1, ARRAY_COUNT(s_mantleAnimNames) - 1, 0);

	for ( animIndex = 1; animIndex < ARRAY_COUNT(s_mantleAnimNames); animIndex++ )
	{
		XAnimPrecache(s_mantleAnimNames[animIndex], MantleXAnimPrecacheAlloc);
		XAnimCreate(s_mantleAnims, animIndex, s_mantleAnimNames[animIndex]);
	}

	for ( transIndex = 0; transIndex < ARRAY_COUNT(s_mantleTrans); transIndex++ )
	{
		animIndex = s_mantleTrans[transIndex].upAnimIndex;
		XAnimGetAbsDelta(s_mantleAnims, animIndex, rot, delta, 1.0);

		if ( I_fabs(delta[0] - 16.0) > 1.0 )
		{
			Com_Error(ERR_DROP, "Mantle anim [%s] has X translation %f, should be %f\n", s_mantleAnimNames[animIndex], delta[0], 16.0);
		}

		if ( I_fabs(delta[1]) > 1.0 )
		{
			Com_Error(ERR_DROP, "Mantle anim [%s] has Y translation %f, should be %f\n", s_mantleAnimNames[animIndex], delta[1], 0.0);
		}

		if ( I_fabs(delta[2] - s_mantleTrans[transIndex].height) > 1.0 )
		{
			Com_Error(ERR_DROP, "Mantle anim [%s] has Z translation %f, should be %f\n", s_mantleAnimNames[animIndex], delta[2], s_mantleTrans[transIndex].height);
		}

		animIndex = s_mantleTrans[transIndex].overAnimIndex;
		XAnimGetAbsDelta(s_mantleAnims, animIndex, rot, delta, 1.0);

		if ( I_fabs(delta[0] - 31.0) > 1.0 )
		{
			Com_Error(ERR_DROP, "Mantle anim [%s] has X translation %f, should be %f\n", s_mantleAnimNames[animIndex], delta[0], 31.0);
		}

		if ( I_fabs(delta[1]) > 1.0 )
		{
			Com_Error(ERR_DROP, "Mantle anim [%s] has Y translation %f, should be %f\n", s_mantleAnimNames[animIndex], delta[1], 0.0);
		}

		if ( I_fabs(delta[2] - -18.0) > 1.0 )
		{
			Com_Error(ERR_DROP, "Mantle anim [%s] has Z translation %f, should be %f\n", s_mantleAnimNames[animIndex], delta[2], -18.0);
		}
	}
}

/*
==================
Mantle_Check
==================
*/
void Mantle_Check( pmove_t *pmove, pml_t *pml )
{
	playerState_t *ps;
	vec3_t mantleDir;
	trace_t trace;
	MantleResults mresults;

	Mantle_DebugPrint("====== Mantle Debug ======");

	if ( !mantle_enable->current.boolean )
	{
		Mantle_DebugPrint("Mantle Failed: Not enabled");
		return;
	}

	ps = pmove->ps;
	assert(ps);

	Mantle_ClearHint(pmove->ps);

	if ( ps->pm_type >= PM_DEAD )
	{
		Mantle_DebugPrint("Mantle Failed: Player is dead");
		return;
	}

	if ( ps->pm_flags & PMF_MANTLE )
	{
		Mantle_DebugPrint("Mantle Failed: Player already mantling");
		return;
	}

	if ( ps->eFlags & (EF_CROUCHING | EF_PRONE) )
	{
		Mantle_DebugPrint("Mantle Failed: Player not standing");
		return;
	}

	if ( ps->weaponstate >= WEAPON_BINOCULARS_INIT && ps->weaponstate <= WEAPON_BINOCULARS_END )
	{
		Mantle_DebugPrint("Mantle Failed: Player using binoculars");
		return;
	}

	if ( !Mantle_FindMantleSurface(pmove, pml, &trace, mantleDir) )
	{
		return;
	}

	memset(&mresults, 0, sizeof(mresults));

	VectorCopy(mantleDir, mresults.dir);
	VectorCopy(ps->origin, mresults.startPos);

	if ( trace.surfaceFlags & SURF_MANTLEOVER )
	{
		mresults.flags |= 1;
	}

	if ( Mantle_CheckLedge(pmove, pml, &mresults, 60.0) )
	{
		return;
	}

	if ( Mantle_CheckLedge(pmove, pml, &mresults, 40.0) )
	{
		return;
	}

	Mantle_CheckLedge(pmove, pml, &mresults, 20.0);
}

/*
==================
Mantle_CalcEndPos
==================
*/
void Mantle_CalcEndPos( pmove_t *pm, MantleResults *mresults )
{
	playerState_t *ps;
	vec3_t maxs;
	vec3_t mins;
	vec3_t end;
	vec3_t start;
	trace_t trace;

	ps = pm->ps;
	assert(ps);

	if ( !(mresults->flags & 1) )
	{
		VectorCopy(mresults->ledgePos, mresults->endPos);
		return;
	}

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
		mresults->flags &= ~1;
		VectorCopy(mresults->ledgePos, mresults->endPos);
	}
	else
	{
		VectorCopy(end, mresults->endPos);
		mresults->endPos[2] = (end[2] - start[2]) * trace.fraction + start[2];
	}
}

/*
==================
Mantle_GetOverLength
==================
*/
int Mantle_GetOverLength( const MantleState *mstate )
{
	if ( !(mstate->flags & 1) )
	{
		return 0;
	}

	return XAnimGetLengthMsec(s_mantleAnims, s_mantleTrans[mstate->transIndex].overAnimIndex);
}

/*
==================
Mantle_GetUpLength
==================
*/
int Mantle_GetUpLength( const MantleState *mstate )
{
	return XAnimGetLengthMsec(s_mantleAnims, s_mantleTrans[mstate->transIndex].upAnimIndex);
}

/*
==================
Mantle_GetAnim
==================
*/
int Mantle_GetAnim( MantleState *mstate )
{
	int upTime;
	int overTime;

	upTime = Mantle_GetUpLength(mstate);
	overTime = Mantle_GetOverLength(mstate);

	assert((mstate->timer >= 0) && (mstate->timer <= (upTime + overTime)));

	if ( mstate->timer > upTime )
		return s_mantleTrans[mstate->transIndex].overAnimIndex;
	else
		return s_mantleTrans[mstate->transIndex].upAnimIndex;
}

/*
==================
Mantle_FindTransition
==================
*/
int Mantle_FindTransition( float curHeight, float goalHeight )
{
	float height;
	int bestIndex;
	int transIndex;
	float bestDiff;

	height = goalHeight - curHeight;
	assert(height > 0);

	bestIndex = 0;
	bestDiff = I_fabs(s_mantleTrans[0].height - height);

	for ( transIndex = 1; transIndex < ARRAY_COUNT(s_mantleTrans); transIndex++ )
	{
		if ( bestDiff > I_fabs(s_mantleTrans[transIndex].height - height) )
		{
			bestIndex = transIndex;
			bestDiff = I_fabs(s_mantleTrans[transIndex].height - height);
		}
	}

	return bestIndex;
}

/*
==================
Mantle_GetAnimDelta
==================
*/
void Mantle_GetAnimDelta( MantleState *mstate, int time, vec3_t delta )
{
	int upTime;
	int overTime;
	vec3_t trans;
	vec2_t rot;

	upTime = Mantle_GetUpLength(mstate);
	overTime = Mantle_GetOverLength(mstate);

	assert((time >= 0) && (time <= (upTime + overTime)));

	if ( time <= upTime )
	{
		XAnimGetAbsDelta(s_mantleAnims, s_mantleTrans[mstate->transIndex].upAnimIndex, rot, delta, (float)time / (float)upTime);
	}
	else
	{
		XAnimGetAbsDelta(s_mantleAnims, s_mantleTrans[mstate->transIndex].upAnimIndex, rot, trans, 1.0);
		XAnimGetAbsDelta(s_mantleAnims, s_mantleTrans[mstate->transIndex].overAnimIndex, rot, delta, (float)(time - upTime) / (float)overTime);

		VectorAdd(delta, trans, delta);
	}

	VectorAngleMultiply(delta, mstate->yaw);
}

/*
==================
Mantle_FindMantleSurface
==================
*/
bool Mantle_FindMantleSurface( pmove_t *pm, pml_t *pml, trace_t *trace, vec3_t mantleDir )
{
	playerState_t *ps;
	vec3_t traceDir;
	vec3_t end;
	vec3_t start;
	vec3_t maxs;
	vec3_t mins;

	ps = pm->ps;
	assert(ps);

	VectorSet(mins, -mantle_check_radius->current.decimal, -mantle_check_radius->current.decimal, ps->mins[2]);
	VectorSet(maxs, mantle_check_radius->current.decimal, mantle_check_radius->current.decimal, ps->maxs[2]);

	assert((maxs[0] - mins[0]) <= (ps->maxs[2] - ps->mins[2]));
	assert((maxs[1] - mins[1]) <= (ps->maxs[2] - ps->mins[2]));

	VectorCopy(pml->forward, traceDir);
	traceDir[2] = 0.0;

	Vec3Normalize(traceDir);

	VectorMA(ps->origin, -(ps->maxs[0] - mantle_check_radius->current.decimal), traceDir, start);
	VectorMA(ps->origin, mantle_check_range->current.decimal + ps->maxs[0] - mantle_check_radius->current.decimal, traceDir, end);

	PM_trace(pm, trace, start, mins, maxs, end, ps->clientNum, CONTENTS_MANTLE);

	if ( trace->startsolid || trace->allsolid )
	{
		Mantle_DebugPrint("Mantle Failed: Mantle brush is too thick");
		return false;
	}

	if ( trace->fraction == 1.0 )
	{
		Mantle_DebugPrint("Mantle Failed: No mantle surface found");
		return false;
	}

	if ( !(trace->surfaceFlags & (SURF_MANTLEON | SURF_MANTLEOVER)) )
	{
		Mantle_DebugPrint("Mantle Failed: No mantle surface with MANTLE_ON or MANTLE_OVER found");
		return false;
	}

	VectorNegate(trace->normal, mantleDir);
	mantleDir[2] = 0.0;

	if ( Vec3Normalize(mantleDir) < 0.000099999997 )
	{
		Mantle_DebugPrint("Mantle Failed: Mantle surface has vertical normal");
		return false;
	}

	if ( acos(DotProduct(traceDir, mantleDir)) * DEGINRAD > mantle_check_angle->current.decimal )
	{
		Mantle_DebugPrint("Mantle Failed: Player is not facing mantle surface");
		return false;
	}

	return true;
}

/*
==================
Mantle_CheckLedge
==================
*/
bool Mantle_CheckLedge( pmove_t *pmove, pml_t *pml, MantleResults *mresults, float units )
{
	playerState_t *ps;
	vec3_t maxs;
	vec3_t mins;
	vec3_t end;
	vec3_t start;
	trace_t trace;

	ps = pmove->ps;
	assert(ps);

	Mantle_DebugPrint(va("Checking for ledge at %f units", units));

	VectorSet(mins, -ps->maxs[0], -ps->maxs[0], 0.0);
	VectorSet(maxs, ps->maxs[0], ps->maxs[0], ps->maxs[0] + ps->maxs[0]);

	VectorCopy(mresults->startPos, start);
	start[2] = start[2] + units;

	VectorMA(start, 16.0, mresults->dir, end);
	PM_trace(pmove, &trace, start, mins, maxs, end, ps->clientNum, pmove->tracemask);

	if ( trace.startsolid || trace.fraction < 1.0 )
	{
		Mantle_DebugPrint("Mantle Failed: Ledge is too far away");
		return false;
	}

	VectorCopy(end, start);
	end[2] = mresults->startPos[2] + 18.0;

	PM_trace(pmove, &trace, start, mins, maxs, end, ps->clientNum, pmove->tracemask);

	if ( trace.startsolid || trace.fraction == 1.0 )
	{
		Mantle_DebugPrint("Mantle Failed: Can't find ledge");
		return false;
	}

	if ( trace.normal[2] < 0.69999999 )
	{
		Mantle_DebugPrint("Mantle Failed: Ledge is too steep");
		return false;
	}

	VectorCopy(end, mresults->ledgePos);
	mresults->ledgePos[2] = (end[2] - start[2]) * trace.fraction + start[2];
	assert((((mresults->ledgePos[2] - mresults->startPos[2]) > 0.0f)));
	maxs[2] = 50.0;

	PM_trace(pmove, &trace, mresults->ledgePos, mins, maxs, mresults->ledgePos, ps->clientNum, pmove->tracemask);

	if ( trace.startsolid )
	{
		Mantle_DebugPrint("Mantle Failed: Player can't fit crouched on ledge");
		return false;
	}

	ps->mantleState.flags |= 8;
	mresults->flags |= 8;

	Mantle_DebugPrint("Mantle available!");

	if ( !(pmove->cmd.buttons & BUTTON_JUMP) )
	{
		return true;
	}

	Mantle_CalcEndPos(pmove, mresults);

	if ( !(ps->eFlags & EF_CROUCHING) )
	{
		PM_trace(pmove, &trace, mresults->ledgePos, ps->mins, ps->maxs, mresults->ledgePos, ps->clientNum, pmove->tracemask);

		if ( trace.startsolid )
		{
			mresults->flags |= 2;
		}

		PM_trace(pmove, &trace, mresults->endPos,  ps->mins, ps->maxs, mresults->endPos, ps->clientNum, pmove->tracemask);

		if ( !trace.startsolid )
		{
			mresults->flags |= 4;
		}
	}

	Mantle_Start(pmove, ps, mresults);
	return true;
}

/*
==================
Mantle_Start
==================
*/
void Mantle_Start( pmove_t *pm, playerState_s *ps, MantleResults *mresults )
{
	int mantleTime;
	vec3_t trans;
	MantleState *mstate;

	mstate = &ps->mantleState;

	ps->mantleState.yaw = vectoyaw(mresults->dir);

	mstate->timer = 0;
	mstate->transIndex = Mantle_FindTransition(mresults->startPos[2], mresults->ledgePos[2]);
	mstate->flags = mresults->flags;

	mantleTime = Mantle_GetOverLength(mstate) + Mantle_GetUpLength(mstate);
	Mantle_GetAnimDelta(mstate, mantleTime, trans);

	VectorSubtract(mresults->endPos, trans, ps->origin);

	ps->pm_flags |= PMF_MANTLE;
	ps->eFlags |= EF_MANTLE;

	VectorCopy(mresults->endPos, pm->mantleEndPos);

	pm->mantleDuration = mantleTime;
	pm->mantleStarted = true;
}

/*
==================
Mantle_RegisterDvars
==================
*/
void Mantle_RegisterDvars()
{
	mantle_enable = Dvar_RegisterBool("mantle_enable", true, DVAR_CHEAT | DVAR_CODINFO | DVAR_CHANGEABLE_RESET);
	mantle_debug = Dvar_RegisterBool("mantle_debug", false, DVAR_CHEAT | DVAR_CODINFO | DVAR_CHANGEABLE_RESET);
	mantle_check_range = Dvar_RegisterFloat("mantle_check_range", 20, 0, 128, DVAR_CHEAT | DVAR_CODINFO | DVAR_CHANGEABLE_RESET);
	mantle_check_radius = Dvar_RegisterFloat("mantle_check_radius", 0.1, 0, 15, DVAR_CHEAT | DVAR_CODINFO | DVAR_CHANGEABLE_RESET);
	mantle_check_angle = Dvar_RegisterFloat("mantle_check_angle", 60, 0, 180, DVAR_CHEAT | DVAR_CODINFO | DVAR_CHANGEABLE_RESET);
	mantle_view_yawcap = Dvar_RegisterFloat("mantle_view_yawcap", 60, 0, 180, DVAR_CHEAT | DVAR_CODINFO | DVAR_CHANGEABLE_RESET);
}