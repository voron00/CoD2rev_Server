#include "../qcommon/qcommon.h"
#include "bg_public.h"

dvar_t *jump_height;
dvar_t *jump_stepSize;
dvar_t *jump_slowdownEnable;
dvar_t *jump_ladderPushVel;
dvar_t *jump_spreadAdd;

#define JUMP_LAND_SLOWDOWN_TIME 1800

/*
==================
Jump_ActivateSlowdown
==================
*/
void Jump_ActivateSlowdown( playerState_t *ps )
{
	if ( ps->pm_time )
	{
		return;
	}

	ps->pm_flags |= PMF_JUMPING;
	ps->pm_time = JUMP_LAND_SLOWDOWN_TIME;
}

/*
==================
Jump_IsPlayerAboveMax
==================
*/
bool Jump_IsPlayerAboveMax( playerState_t *ps )
{
	assert(ps->pm_flags & PMF_JUMPING);
	return ps->origin[2] >= ps->jumpOriginZ + jump_height->current.decimal;
}

/*
==================
Jump_GetStepHeight
==================
*/
bool Jump_GetStepHeight( playerState_t *ps, const vec3_t origin, float *stepSize )
{
	assert(ps->pm_flags & PMF_JUMPING);
	assert(origin);
	assert(stepSize);

	if ( ps->jumpOriginZ + jump_height->current.decimal <= origin[2] )
	{
		return false;
	}

	*stepSize = jump_stepSize->current.decimal;

	if ( origin[2] + *stepSize > ps->jumpOriginZ + jump_height->current.decimal )
	{
		*stepSize = ps->jumpOriginZ + jump_height->current.decimal - origin[2];
	}

	return true;
}

/*
==================
Jump_ClearState
==================
*/
void Jump_ClearState( playerState_t *ps )
{
	ps->pm_flags &= ~PMF_JUMPING;
	ps->jumpOriginZ = 0;
}

/*
==================
Jump_ClampVelocity
==================
*/
void Jump_ClampVelocity( playerState_t *ps, const vec3_t origin )
{
	float maxJumpVel;
	float heightDiff;

	assert(ps->pm_flags & PMF_JUMPING);
	assert(origin);

	if ( ps->origin[2] - origin[2] <= 0 )
	{
		return;
	}

	heightDiff = ps->jumpOriginZ + jump_height->current.decimal - ps->origin[2];

	if ( heightDiff < 0.1 )
	{
		ps->velocity[2] = 0;
		return;
	}

	maxJumpVel = I_sqrt((heightDiff + heightDiff) * ps->gravity);

	if ( ps->velocity[2] > maxJumpVel )
	{
		ps->velocity[2] = maxJumpVel;
	}
}

/*
==================
Jump_ReduceFriction
==================
*/
float Jump_ReduceFriction( playerState_t *ps )
{
	assert(ps->pm_flags & PMF_JUMPING);

	if ( !jump_slowdownEnable->current.boolean )
	{
		return 1.0;
	}

	if ( ps->pm_time >= JUMP_LAND_SLOWDOWN_TIME - FRAMETIME )
	{
		return 2.5;
	}

	return ps->pm_time * 1.5 * 0.00058823527 + 1.0;
}

/*
==================
Jump_GetLandFactor
==================
*/
float Jump_GetLandFactor( playerState_t *ps )
{
	assert(ps->pm_flags & PMF_JUMPING);
	assert(ps->pm_time <= JUMP_LAND_SLOWDOWN_TIME);

	if ( !jump_slowdownEnable->current.boolean )
	{
		return 1.0;
	}

	if ( ps->pm_time >= JUMP_LAND_SLOWDOWN_TIME - FRAMETIME )
	{
		return 2.5;
	}

	return ps->pm_time * 1.5 * 0.00058823527 + 1.0;
}

/*
==================
Jump_GetSlowdownFriction
==================
*/
float Jump_GetSlowdownFriction( playerState_t *ps )
{
	assert(ps->pm_flags & PMF_JUMPING);
	assert(ps->pm_time <= JUMP_LAND_SLOWDOWN_TIME);

	if ( !jump_slowdownEnable->current.boolean )
	{
		return 1.0;
	}

	if ( ps->pm_time >= JUMP_LAND_SLOWDOWN_TIME - FRAMETIME )
	{
		return 2.5;
	}

	return ps->pm_time * 1.5 * 0.00058823527 + 1.0;
}

/*
==================
Jump_ApplySlowdown
==================
*/
void Jump_ApplySlowdown( playerState_t *ps )
{
	assert(ps->pm_flags & PMF_JUMPING);

	if ( !jump_slowdownEnable->current.boolean )
	{
		VectorScale(ps->velocity, 1.0, ps->velocity);
		return;
	}

	if ( ps->pm_time > JUMP_LAND_SLOWDOWN_TIME )
	{
		Jump_ClearState(ps);
		VectorScale(ps->velocity, 0.64999998, ps->velocity);
		return;
	}

	if ( ps->pm_time )
	{
		VectorScale(ps->velocity, 1.0, ps->velocity);
		return;
	}

	if ( ps->jumpOriginZ + 18.0 > ps->origin[2] )
	{
		ps->pm_time = JUMP_LAND_SLOWDOWN_TIME;
		VectorScale(ps->velocity, 0.64999998, ps->velocity);
		return;
	}

	ps->pm_time = JUMP_LAND_SLOWDOWN_TIME - FRAMETIME - 500;
	VectorScale(ps->velocity, 0.5, ps->velocity);
}

/*
==================
Jump_AddSurfaceEvent
==================
*/
void Jump_AddSurfaceEvent( playerState_t *ps, pml_t *pml )
{
	if ( (ps->pm_flags & PMF_LADDER) )
	{
		PM_AddEvent(ps, EV_JUMP_WOOD);
		return;
	}

	int surfType = PM_GroundSurfaceType(pml);

	if ( !surfType )
	{
		return;
	}

	PM_AddEvent(ps, surfType + EV_JUMP_DEFAULT);
}

/*
==================
Jump_PushOffLadder
==================
*/
void Jump_PushOffLadder( playerState_t *ps, pml_t *pml )
{
	float dot;
	vec3_t flatForward;
	vec3_t pushOffDir;

	assert(ps->pm_flags & PMF_LADDER);

	ps->velocity[2] = ps->velocity[2] * 0.75;

	VectorSet(flatForward, pml->forward[0], pml->forward[1], 0.0);
	Vec3Normalize(flatForward);

	dot = DotProduct(ps->vLadderVec, pml->forward);

	if ( dot < 0.0 )
	{
		dot = DotProduct(flatForward, ps->vLadderVec);

		VectorMA(flatForward, dot * -2.0, ps->vLadderVec, pushOffDir);
		Vec3Normalize(pushOffDir);
	}
	else
	{
		VectorCopy(flatForward, pushOffDir);
	}

	Vec2Scale(pushOffDir, jump_ladderPushVel->current.decimal, ps->velocity);
	ps->pm_flags &= ~PMF_LADDER;
}

/*
==================
Jump_Start
==================
*/
void Jump_Start( pmove_t *pm, pml_t *pml, float height )
{
	playerState_t *ps;
	float factor;
	float velocitySqrd;

	ps = pm->ps;
	assert(ps);

	velocitySqrd = (height + height) * pm->ps->gravity;

	if ( pm->ps->pm_flags & PMF_JUMPING && ps->pm_time <= JUMP_LAND_SLOWDOWN_TIME )
	{
		factor = Jump_GetLandFactor(ps);
		assert(factor);
		velocitySqrd = velocitySqrd / factor;
	}

	pml->groundPlane = qfalse;
	pml->almostGroundPlane = qfalse;
	pml->walking = qfalse;
	ps->groundEntityNum = ENTITYNUM_NONE;
	ps->jumpTime = pm->cmd.serverTime;
	ps->jumpOriginZ = ps->origin[2];
	ps->pm_flags |= PMF_JUMPING;
	ps->pm_time = 0;
	ps->velocity[2] = I_sqrt(velocitySqrd);
	ps->aimSpreadScale = ps->aimSpreadScale + jump_spreadAdd->current.decimal;

	if ( ps->aimSpreadScale > 255 )
	{
		ps->aimSpreadScale = 255;
	}
}

/*
==================
Jump_Start
==================
*/
bool Jump_Check( pmove_t *pm, pml_t *pml )
{
	playerState_t *ps;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	if ( pm->cmd.serverTime - pm->ps->jumpTime < 500 )
	{
		return false;
	}

	if ( ps->pm_flags & PMF_RESPAWNED )
	{
		return false;
	}

	if ( ps->pm_flags & PMF_MANTLE )
	{
		return false;
	}

	if ( ps->pm_type >= PM_DEAD )
	{
		return false;
	}

	if ( PM_GetEffectiveStance(ps) )
	{
		return false;
	}

	if ( !(pm->cmd.buttons & BUTTON_JUMP) )
	{
		return false;
	}

	if ( pm->oldcmd.buttons & BUTTON_JUMP )
	{
		pm->cmd.buttons &= ~BUTTON_JUMP;
		return false;
	}

	Jump_Start(pm, pml, jump_height->current.decimal);
	Jump_AddSurfaceEvent(ps, pml);

	if ( ps->pm_flags & PMF_LADDER )
	{
		Jump_PushOffLadder(ps, pml);
	}

	if ( pm->cmd.forwardmove >= 0 )
	{
		BG_AnimScriptEvent(ps, ANIM_ET_JUMP, qfalse, qtrue);
	}
	else
	{
		BG_AnimScriptEvent(ps, ANIM_ET_JUMPBK, qfalse, qtrue);
	}

	return true;
}

/*
==================
Jump_RegisterDvars
==================
*/
void Jump_RegisterDvars()
{
	jump_height = Dvar_RegisterFloat("jump_height", 39, 0, 128, DVAR_CHEAT | DVAR_CODINFO | DVAR_CHANGEABLE_RESET);
	jump_stepSize = Dvar_RegisterFloat("jump_stepSize", 18, 0, 64, DVAR_CHEAT | DVAR_CODINFO | DVAR_CHANGEABLE_RESET);
	jump_slowdownEnable = Dvar_RegisterBool("jump_slowdownEnable", true, DVAR_CHEAT | DVAR_CODINFO | DVAR_CHANGEABLE_RESET);
	jump_ladderPushVel = Dvar_RegisterFloat("jump_ladderPushVel", 128, 0, 1024, DVAR_CHEAT | DVAR_CODINFO | DVAR_CHANGEABLE_RESET);
	jump_spreadAdd = Dvar_RegisterFloat("jump_spreadAdd", 64, 0, 512, DVAR_CHEAT | DVAR_CODINFO | DVAR_CHANGEABLE_RESET);
}