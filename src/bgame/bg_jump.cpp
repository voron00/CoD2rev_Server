#include "../qcommon/qcommon.h"
#include "bg_public.h"

dvar_t *jump_height;
dvar_t *jump_stepSize;
dvar_t *jump_slowdownEnable;
dvar_t *jump_ladderPushVel;
dvar_t *jump_spreadAdd;
dvar_t *jump_bounceEnable;

#if LIBCOD_COMPILE_PLAYER == 1
bool player_jump_slowdownenable_enabled[MAX_CLIENTS] = {0};
bool player_jump_slowdownenable[MAX_CLIENTS] = {0};
#endif

bool getJumpSlowdownEnable(playerState_t* ps)
{
#if LIBCOD_COMPILE_PLAYER == 1
	int clientid = playerstateToClientNum(ps);
	if(player_jump_slowdownenable_enabled[clientid])
		return player_jump_slowdownenable[clientid];
#endif
	return jump_slowdownEnable->current.boolean;
}

void Jump_ClearState(playerState_s *ps)
{
	ps->pm_flags &= ~0x80000u;
	ps->jumpOriginZ = 0.0;
}

bool Jump_GetStepHeight(playerState_s *ps, const float *origin, float *stepSize)
{
	if ( ps->jumpOriginZ + jump_height->current.decimal <= (float)origin[2] )
		return 0;

	*stepSize = jump_stepSize->current.decimal;

	if ( origin[2] + *stepSize > ps->jumpOriginZ + jump_height->current.decimal )
		*stepSize = ps->jumpOriginZ + jump_height->current.decimal - origin[2];

	return 1;
}

bool Jump_IsPlayerAboveMax(playerState_s *ps)
{
	return ps->origin[2] >= ps->jumpOriginZ + jump_height->current.decimal;
}

void Jump_ActivateSlowdown(playerState_s *ps)
{
	if ( !ps->pm_time )
	{
		ps->pm_flags |= 0x80000u;
		ps->pm_time = 1800;
	}
}

void Jump_ApplySlowdown(playerState_s *ps)
{
	float scale;

	scale = 1.0;

	if ( ps->pm_time <= 1800 )
	{
		if ( !ps->pm_time )
		{
			if ( ps->jumpOriginZ + 18.0 <= ps->origin[2] )
			{
				ps->pm_time = 1200;
				scale = 0.5;
			}
			else
			{
				ps->pm_time = 1800;
				scale = 0.64999998;
			}
		}
	}
	else
	{
		Jump_ClearState(ps);
		scale = 0.64999998;
	}

	if ( !getJumpSlowdownEnable(ps) )
		scale = 1.0;

	VectorScale(ps->velocity, scale, ps->velocity);
}

float Jump_ReduceFriction(playerState_s *ps)
{
	if ( getJumpSlowdownEnable(ps) )
	{
		if ( ps->pm_time <= 1699 )
			return (float)ps->pm_time * 1.5 * 0.00058823527 + 1.0;
		else
			return 2.5;
	}
	else
	{
		return 1.0;
	}
}

void Jump_ClampVelocity(playerState_s *ps, const float *origin)
{
	float temp;
	float maxJumpVel;
	float heightDiff;

	if ( ps->origin[2] - origin[2] > 0.0 )
	{
		heightDiff = ps->jumpOriginZ + jump_height->current.decimal - ps->origin[2];

		if ( heightDiff >= 0.1 )
		{
			temp = (heightDiff + heightDiff) * (float)ps->gravity;
			maxJumpVel = sqrt(temp);

			if ( ps->velocity[2] > (float)maxJumpVel )
				ps->velocity[2] = maxJumpVel;
		}
		else
		{
			ps->velocity[2] = 0.0;
		}
	}
}

float Jump_GetLandFactor(playerState_s *ps)
{
	if ( getJumpSlowdownEnable(ps) )
	{
		if ( ps->pm_time <= 1699 )
			return (float)ps->pm_time * 1.5 * 0.00058823527 + 1.0;
		else
			return 2.5;
	}
	else
	{
		return 1.0;
	}
}

void Jump_AddSurfaceEvent(playerState_s *ps, pml_t *pml)
{
	int surfType;

	if ( (ps->pm_flags & 0x20) != 0 )
	{
		PM_AddEvent(ps, EV_JUMP_WOOD);
	}
	else
	{
		surfType = PM_GroundSurfaceType(pml);

		if ( surfType )
			PM_AddEvent(ps, surfType + 70);
	}
}

void Jump_PushOffLadder(playerState_s *ps, pml_t *pml)
{
	float scale;
	float len;
	float dot;
	vec3_t flatForward;
	vec3_t pushOffDir;

	ps->velocity[2] = ps->velocity[2] * 0.75;
	VectorSet(flatForward, pml->forward[0], pml->forward[1], 0.0);
	Vec3Normalize(flatForward);

	len = DotProduct(ps->vLadderVec, pml->forward);

	if ( len >= 0.0 )
	{
		VectorCopy(flatForward, pushOffDir);
	}
	else
	{
		dot = DotProduct(flatForward, ps->vLadderVec);
		scale = dot * -2.0;
		VectorMA(flatForward, scale, ps->vLadderVec, pushOffDir);
		Vec3Normalize(pushOffDir);
	}

	Vec2Scale(pushOffDir, jump_ladderPushVel->current.decimal, ps->velocity);
	ps->pm_flags &= ~0x20u;
}

void Jump_Start(pmove_t *pm, pml_t *pml, float height)
{
	playerState_s *ps;
	float factor;
	float velocitySqrd;

	ps = pm->ps;
	velocitySqrd = (height + height) * (float)pm->ps->gravity;

	if ( (pm->ps->pm_flags & 0x80000) != 0 && ps->pm_time <= 1800 )
	{
		factor = Jump_GetLandFactor(ps);
		velocitySqrd = velocitySqrd / factor;
	}

	pml->groundPlane = 0;
	pml->almostGroundPlane = 0;
	pml->walking = 0;
	ps->groundEntityNum = 1023;
	ps->jumpTime = pm->cmd.serverTime;
	ps->jumpOriginZ = ps->origin[2];
	ps->pm_flags |= 0x80000u;
	ps->pm_time = 0;
	ps->velocity[2] = sqrt(velocitySqrd);
	ps->aimSpreadScale = ps->aimSpreadScale + jump_spreadAdd->current.decimal;

	if ( ps->aimSpreadScale > 255.0 )
		ps->aimSpreadScale = 255.0;
}

bool Jump_Check(pmove_t *pm, pml_t *pml)
{
	playerState_s *ps;

	ps = pm->ps;

	if ( pm->cmd.serverTime - pm->ps->jumpTime <= 499 )
		return 0;

	if ( (ps->pm_flags & 0x1000) != 0 )
		return 0;

	if ( (ps->pm_flags & 4) != 0 )
		return 0;

	if ( ps->pm_type > PM_INTERMISSION )
		return 0;

	if ( PM_GetEffectiveStance(ps) )
		return 0;

	if ( (pm->cmd.buttons & 0x400) == 0 )
		return 0;

	if ( (pm->oldcmd.buttons & 0x400) != 0 )
	{
		pm->cmd.buttons &= ~0x400u;
		return 0;
	}
	else
	{
		Jump_Start(pm, pml, jump_height->current.decimal);
		Jump_AddSurfaceEvent(ps, pml);

		if ( (ps->pm_flags & 0x20) != 0 )
			Jump_PushOffLadder(ps, pml);

		if ( pm->cmd.forwardmove < 0 )
			BG_AnimScriptEvent(ps, ANIM_ET_JUMPBK, 0, 1);
		else
			BG_AnimScriptEvent(ps, ANIM_ET_JUMP, 0, 1);

		return 1;
	}
}

void Jump_RegisterDvars()
{
	jump_height = Dvar_RegisterFloat("jump_height", 39.0, 0.0, 128.0, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	jump_stepSize = Dvar_RegisterFloat("jump_stepSize", 18.0, 0.0, 64.0, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	jump_slowdownEnable = Dvar_RegisterBool("jump_slowdownEnable", 1, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	jump_ladderPushVel = Dvar_RegisterFloat("jump_ladderPushVel", 128.0, 0.0, 1024.0, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	jump_spreadAdd = Dvar_RegisterFloat("jump_spreadAdd", 64.0, 0.0, 512.0, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
	jump_bounceEnable = Dvar_RegisterBool("jump_bounceEnable", 0, DVAR_CHEAT | DVAR_DEVELOPER | DVAR_CHANGEABLE_RESET);
}