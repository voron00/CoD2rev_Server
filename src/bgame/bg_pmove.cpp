#include "../qcommon/qcommon.h"
#include "bg_public.h"

pmoveHandler_t pmoveHandlers[] =
{
	{ NULL, NULL, NULL },
	{ G_TraceCapsule, SV_PointContents, G_PlayerEvent },
};

viewLerpWaypoint_s viewLerp_CrouchProne[] =
{
	{ 0, 40, 0},
	{ 11, 38, 0},
	{ 22, 33, 0},
	{ 34, 25, 0},
	{ 45, 16, 0},
	{ 50, 15, 0},
	{ 55, 16, 0},
	{ 70, 18, 0},
	{ 90, 17, 0},
	{ 100, 11, 0},
	{ -1, 0, 0},
};

viewLerpWaypoint_s viewLerp_StandCrouch[] =
{
	{ 0, 60, 0},
	{ 1, 59.5, 0},
	{ 4, 58.5, 0},
	{ 30, 56, 0},
	{ 80, 44, 0},
	{ 90, 41.5, 0},
	{ 95, 40.5, 0},
	{ 100, 40, 0},
	{ -1, 0, 0},
};

viewLerpWaypoint_s viewLerp_CrouchStand[] =
{
	{ 0, 40, 0},
	{ 5, 40.5, 0},
	{ 10, 41.5, 0},
	{ 20, 44, 0},
	{ 70, 56, 0},
	{ 96, 58.5, 0},
	{ 99, 59.5, 0},
	{ 100, 60, 0},
	{ -1, 0, 0},
};

viewLerpWaypoint_s viewLerp_ProneCrouch[] =
{
	{ 0, 11, 0},
	{ 5, 10, 0},
	{ 30, 21, 0},
	{ 50, 25, 0},
	{ 67, 31, 0},
	{ 83, 34, 0},
	{ 100, 40, 0},
	{ -1, 0, 0},
};

const vec3_t CorrectSolidDeltas[] =
{
	{ 0, 0, 1 },
	{ -1, 0, 1 },
	{ 0, -1, 1 },
	{ 1, 0, 1 },
	{ 0, 1, 1 },
	{ -1, 0, 0 },
	{ 0, -1, 0 },
	{ 1, 0, 0 },
	{ 0, 1, 0 },
	{ 0, 0, -1 },
	{ -1, 0, -1 },
	{ 0, -1, -1 },
	{ 1, 0, -1 },
	{ 0, 1, -1 },
	{ -1, -1, 1 },
	{ 1, -1, 1 },
	{ 1, 1, 1 },
	{ -1, 1, 1 },
	{ -1, -1, 0 },
	{ 1, -1, 0 },
	{ 1, 1, 0 },
	{ -1, 1, 0 },
	{ -1, -1, -1 },
	{ 1, -1, -1 },
	{ 1, 1, -1 },
	{ -1, 1, -1 },
};

/*
===============
PM_SetProneMovementOverride
===============
*/
void PM_SetProneMovementOverride( playerState_t *ps )
{
	if ( ps->pm_flags & PMF_PRONE )
	{
		ps->pm_flags |= PMF_ADS_OVERRIDE;
	}
}

/*
===============
PM_GetViewHeightLerpTime
===============
*/
int PM_GetViewHeightLerpTime( const playerState_s *ps, int iTarget, int bDown )
{
	if ( iTarget == 11 )
	{
		return 400;
	}

	if ( iTarget != 40 )
	{
		return 200;
	}

	if ( bDown )
	{
		return 200;
	}

	return 400;
}

/*
===============
PM_GroundSurfaceType
===============
*/
int PM_GroundSurfaceType( pml_t *pml )
{
	int iSurfType;

	assert(pml);

	if ( pml->groundTrace.surfaceFlags & SURF_NOSTEPS )
	{
		return SURF_TYPE_NONE;
	}

	iSurfType = SURF_TYPEINDEX( pml->groundTrace.surfaceFlags );
	assert(iSurfType >= SURF_TYPE_NONE && iSurfType < SURF_TYPECOUNT);

	return iSurfType;
}

/*
===============
PM_DamageScale_Walk
===============
*/
float PM_DamageScale_Walk( int damage_timer )
{
	float timer_max;

	if ( !damage_timer )
	{
		return 1;
	}

	timer_max = player_dmgtimer_maxTime->current.decimal;

	if ( timer_max == 0 )
	{
		return 1;
	}

	return damage_timer * (-player_dmgtimer_minScale->current.decimal / timer_max) + 1;
}

/*
===============
PM_GetEffectiveStance
===============
*/
int PM_GetEffectiveStance( const playerState_t *ps )
{
	if ( ps->viewHeightTarget == 40 )
	{
		return PM_EFF_STANCE_CROUCH;
	}

	if ( ps->viewHeightTarget == 11 )
	{
		return PM_EFF_STANCE_PRONE;
	}

	return PM_EFF_STANCE_STAND;
}

/*
===============
PM_AddTouchEnt
===============
*/
void PM_AddTouchEnt( pmove_t *pm, int entityNum )
{
	int i;

	if ( entityNum == ENTITYNUM_WORLD )
	{
		return;
	}
	if ( pm->numtouch == MAXTOUCH )
	{
		return;
	}

	// see if it is already added
	for ( i = 0 ; i < pm->numtouch ; i++ )
	{
		if ( pm->touchents[ i ] == entityNum )
		{
			return;
		}
	}

	// add it
	pm->touchents[pm->numtouch] = entityNum;
	pm->numtouch++;
}

/*
===============
PM_playerTrace
===============
*/
void PM_playerTrace( pmove_t *pm, trace_t *results, const vec3_t start,
                     const vec3_t mins, const vec3_t maxs, const vec3_t end,
                     int passEntityNum, int contentMask )
{
	pmoveHandlers[pm->handler].trace(results, start, mins, maxs, end, passEntityNum, contentMask);

	if ( !results->startsolid )
	{
		return;
	}

	if ( !(results->contents & CONTENTS_BODY) )
	{
		return;
	}

	PM_AddTouchEnt(pm, results->entityNum);
	pm->tracemask &= ~CONTENTS_BODY;
	pmoveHandlers[pm->handler].trace(results, start, mins, maxs, end, passEntityNum, contentMask & ~CONTENTS_BODY);
}

/*
===============
PM_trace
===============
*/
void PM_trace( pmove_t *pm, trace_t *results, const vec3_t start,
               const vec3_t mins, const vec3_t maxs, const vec3_t end,
               int passEntityNum, int contentMask )
{
	pmoveHandlers[pm->handler].trace(results, start, mins, maxs, end, passEntityNum, contentMask);
}

/*
===============
PM_ShouldMakeFootsteps
===============
*/
qboolean PM_ShouldMakeFootsteps( pmove_t *pm )
{
	playerState_t *ps;
	int bCanWalk;
	int stance;

	ps = pm->ps;
	assert(ps);

	bCanWalk = pm->ps->pm_flags & PMF_ADS_WALK;
	stance = PM_GetEffectiveStance(pm->ps);

	if ( stance == PM_EFF_STANCE_PRONE || stance == PM_EFF_STANCE_CROUCH )
	{
		return qfalse;
	}

	return !bCanWalk && pm->xyspeed >= player_footstepsThreshhold->current.decimal; // optimized some bs here
}

/*
===============
PM_AddEvent
===============
*/
void PM_AddEvent( playerState_t *ps, int newEvent )
{
	BG_AddPredictableEventToPlayerstate(newEvent, 0, ps);
}

/*
===============
BG_GetSpeed
===============
*/
float BG_GetSpeed( const playerState_t *ps, int iTime )
{
	if ( !(ps->pm_flags & PMF_LADDER) )
	{
		return VectorLength(ps->velocity);
	}

	if ( iTime - ps->jumpTime < 500 )
	{
		return 0;
	}

	return ps->velocity[2];
}

/*
===============
PM_FootstepEvent
===============
*/
void PM_FootstepEvent( pmove_t *pm, pml_t *pml, int iOldBobCycle, int iNewBobCycle, qboolean bFootStep )
{
	trace_t trace;
	vec3_t vEnd;
	vec3_t maxs;
	vec3_t mins;
	float fTraceDist;
	int iClipMask;
	int iSurfaceType;
	playerState_t *ps;

	ps = pm->ps;
	assert(ps);

	if ( !( ( (iNewBobCycle + 64) ^ (iOldBobCycle + 64) ) & 128 ) )
	{
		return;
	}

	if ( ps->groundEntityNum != ENTITYNUM_NONE )
	{
		if ( bFootStep )
		{
			PM_AddEvent(ps, PM_FootstepForSurface(ps, pml));
		}

		return;
	}

	if ( !bFootStep )
	{
		return;
	}

	if ( !(ps->pm_flags & PMF_LADDER) )
	{
		return;
	}

	VectorCopy(pm->mins, mins);

	mins[0] = mins[0] + 6.0;
	mins[1] = mins[1] + 6.0;
	mins[2] = 8.0;

	VectorCopy(pm->maxs, maxs);

	maxs[0] = maxs[0] - 6.0;
	maxs[1] = maxs[1] - 6.0;

	if ( mins[2] > maxs[2] )
	{
		maxs[2] = mins[2];
	}

	assert(maxs[0] >= mins[0]);
	assert(maxs[1] >= mins[1]);
	assert(maxs[2] >= mins[2]);

	iClipMask = pm->tracemask & ~( CONTENTS_BODY | CONTENTS_PLAYERCLIP );
	fTraceDist = -31.0;

	VectorMA(ps->origin, fTraceDist, ps->vLadderVec, vEnd);

	PM_playerTrace(pm, &trace, ps->origin, mins, maxs, vEnd, ps->clientNum, iClipMask);
	iSurfaceType = SURF_TYPEINDEX( trace.surfaceFlags );

	if ( trace.fraction == 1.0 || !iSurfaceType )
	{
		iSurfaceType = SURF_TYPE_WOOD;
	}

	PM_AddEvent(ps, iSurfaceType + 1);
}

/*
==================
PM_ClipVelocity

Slide off of the impacting surface
==================
*/
void PM_ClipVelocity( const vec3_t in, const vec3_t normal, vec3_t out )
{
	float backoff;
	float change;

	backoff = DotProduct(in, normal);
	change = backoff - I_fabs(backoff) * 0.001;

	VectorMA(in, -change, normal, out);
}

/*
==================
PM_UpdateLean
==================
*/
void PM_UpdateLean( playerState_t *ps, float msec, usercmd_t *cmd,
                    void (*capsuleTrace)(trace_t *, const float *, const float *, const float *, const float *, int, int) )
{
	float fLeanFrac;
	float fLean;
	float fLeanMax;
	trace_t trace;
	float leanofs;
	int leaning;
	vec3_t tmaxs;
	vec3_t tmins;
	vec3_t end;
	vec3_t start;

	leaning = 0;
	leanofs = 0;

	if ( cmd->buttons & ( BUTTON_LEANLEFT | BUTTON_LEANRIGHT ) && !(ps->pm_flags & PMF_UNKNOWN_8000) && ps->pm_type < PM_DEAD )
	{
		if ( ps->groundEntityNum != ENTITYNUM_NONE || ps->pm_type == PM_NORMAL_LINKED )
		{
			if ( cmd->buttons & BUTTON_LEANLEFT )
			{
				leaning--;
			}

			if ( cmd->buttons & BUTTON_LEANRIGHT )
			{
				leaning++;
			}
		}
	}

	if ( ps->eFlags & EF_TURRET_ACTIVE )
	{
		leaning = 0;
	}

	if ( PM_GetEffectiveStance(ps) == PM_EFF_STANCE_PRONE )
	{
		fLeanMax = 0.25;
	}
	else
	{
		fLeanMax = 0.5;
	}

	leanofs = ps->leanf;

	if ( leaning )
	{
		if ( leaning > 0 )
		{
			if ( fLeanMax > leanofs )
			{
				leanofs = msec / 350.0 * fLeanMax + leanofs;
			}

			if ( leanofs > fLeanMax )
			{
				leanofs = fLeanMax;
			}
		}
		else
		{
			if ( leanofs > -fLeanMax )
			{
				leanofs = leanofs - msec / 350.0 * fLeanMax;
			}

			if ( -fLeanMax > leanofs )
			{
				leanofs = -fLeanMax;
			}
		}
	}
	else if ( leanofs > 0 )
	{
		leanofs = leanofs - msec / 280.0 * fLeanMax;

		if ( leanofs < 0 )
		{
			leanofs = 0;
		}
	}
	else if ( leanofs < 0 )
	{
		leanofs = msec / 280.0 * fLeanMax + leanofs;

		if ( leanofs > 0 )
		{
			leanofs = 0;
		}
	}

	ps->leanf = leanofs;

	if ( ps->leanf == 0 )
	{
		return;
	}

	fLeanFrac = FloatSign(ps->leanf);

	VectorCopy(ps->origin, start);
	start[2] = start[2] + ps->viewHeightCurrent;
	VectorCopy(start, end);

	AddLeanToPosition(end, ps->viewangles[1], fLeanFrac, 16.0, 20.0);

	VectorSet(tmins, -8.0, -8.0, -8.0);
	VectorSet(tmaxs, 8.0, 8.0, 8.0);

	capsuleTrace(&trace, start, tmins, tmaxs, end, ps->clientNum, 42008593);

	fLean = UnGetLeanFraction(trace.fraction);

	if ( I_fabs(ps->leanf) > fLean )
	{
		ps->leanf = FloatSign(ps->leanf) * fLean;
	}
}

/*
==================
PM_UpdatePronePitch
==================
*/
void PM_UpdatePronePitch( pmove_t *pm, pml_t *pml )
{
	qboolean bProneOK;
	playerState_t *ps;
	float delta;
	float fTargPitch;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	if ( !( ps->pm_flags & PMF_PRONE ) )
	{
		return;
	}

	if ( ps->groundEntityNum == ENTITYNUM_NONE )
	{
		if ( pml->groundPlane )
		{
			bProneOK = BG_CheckProne( ps->clientNum, ps->origin, ps->maxs[0], 30.0, ps->proneDirection, &ps->fTorsoHeight, &ps->fTorsoPitch,
			                          &ps->fWaistPitch, qtrue, ps->groundEntityNum != ENTITYNUM_NONE, pml->groundTrace.normal, pm->handler, PCT_CLIENT, 66.0);
		}
		else
		{
			bProneOK = BG_CheckProne( ps->clientNum, ps->origin, ps->maxs[0], 30.0, ps->proneDirection, &ps->fTorsoHeight, &ps->fTorsoPitch,
			                          &ps->fWaistPitch, qtrue, ps->groundEntityNum != ENTITYNUM_NONE, NULL, pm->handler, PCT_CLIENT, 66.0);
		}

		if ( !bProneOK )
		{
			BG_AddPredictableEventToPlayerstate(EV_STANCE_FORCE_CROUCH, 0, ps);
			ps->pm_flags |= PMF_PRONE_BLOCKED;
		}
	}
	else
	{
		if ( pml->groundPlane && pml->groundTrace.normal[2] < 0.69999999 )
		{
			BG_AddPredictableEventToPlayerstate(EV_STANCE_FORCE_CROUCH, 0, ps);
		}
	}

	if ( pml->groundPlane )
	{
		fTargPitch = PitchForYawOnNormal(ps->proneDirection, pml->groundTrace.normal);
	}
	else
	{
		fTargPitch = 0;
	}

	delta = AngleDelta(fTargPitch, ps->proneDirectionPitch);

	if ( delta != 0 )
	{
		if ( I_fabs(delta) > pml->frametime * 70.0 )
		{
			ps->proneDirectionPitch = FloatSign(delta) * (pml->frametime * 70.0) + ps->proneDirectionPitch;
		}
		else
		{
			ps->proneDirectionPitch = ps->proneDirectionPitch + delta;
		}

		ps->proneDirectionPitch = AngleNormalize180Accurate(ps->proneDirectionPitch);
	}

	if ( pml->groundPlane )
	{
		fTargPitch = PitchForYawOnNormal(ps->viewangles[1], pml->groundTrace.normal);
	}
	else
	{
		fTargPitch = 0;
	}

	delta = AngleDelta(fTargPitch, ps->proneTorsoPitch);

	if ( delta != 0 )
	{
		if ( I_fabs(delta) > pml->frametime * 70.0 )
		{
			ps->proneTorsoPitch = FloatSign(delta) * (pml->frametime * 70.0) + ps->proneTorsoPitch;
		}
		else
		{
			ps->proneTorsoPitch = ps->proneTorsoPitch + delta;
		}

		ps->proneTorsoPitch = AngleNormalize180Accurate(ps->proneTorsoPitch);
	}
}

/*
==================
PM_UpdateViewAngles_Clamp
==================
*/
void PM_UpdateViewAngles_Clamp( playerState_t *ps, usercmd_t *cmd, byte handler )
{
	int minPitch = ANGLE2SHORT(player_view_pitch_up->current.decimal);
	int maxPitch = ANGLE2SHORT(player_view_pitch_down->current.decimal);

	// circularly clamp the angles with deltas
	for ( int i = 0; i < 3; i++ )
	{
		short temp = cmd->angles[i] + ps->delta_angles[i];

		if ( i == PITCH )
		{
			// don't let the player look up or down more than 90 degrees
			if ( temp > maxPitch )
			{
				ps->delta_angles[PITCH] = maxPitch - cmd->angles[PITCH];
				temp = maxPitch;
			}
			else
			{
				if ( temp < -minPitch )
				{
					ps->delta_angles[PITCH] = -minPitch - cmd->angles[PITCH];
					temp = -minPitch;
				}
			}
		}

		ps->viewangles[i] = temp * 0.0054931641;
	}
}

/*
==================
PM_UpdateViewAngles_RangeLimited
==================
*/
void PM_UpdateViewAngles_RangeLimited( playerState_t *ps )
{
	for ( int i = 0; i < 2; i++ )
	{
		float delta = AngleDelta(ps->viewAngleClampBase[i], ps->viewangles[i]);

		if ( delta > ps->viewAngleClampRange[i] || -ps->viewAngleClampRange[i] > delta )
		{
			if ( delta > ps->viewAngleClampRange[i] )
				delta -= ps->viewAngleClampRange[i];
			else
				delta += ps->viewAngleClampRange[i];

			ps->delta_angles[i] += ANGLE2SHORT(delta);

			if ( delta > 0 )
				ps->viewangles[i] = AngleNormalize360Accurate(ps->viewAngleClampBase[i] - ps->viewAngleClampRange[i]);
			else
				ps->viewangles[i] = AngleNormalize360Accurate(ps->viewAngleClampBase[i] + ps->viewAngleClampRange[i]);
		}
	}
}

/*
==================
PM_UpdateViewAngles_LadderClamp
==================
*/
void PM_UpdateViewAngles_LadderClamp( playerState_t *ps )
{
	float ladderFacing = vectoyaw(ps->vLadderVec) + 180;
	float delta = AngleDelta(ladderFacing, ps->viewangles[YAW]);

	if ( delta > bg_ladder_yawcap->current.decimal || -bg_ladder_yawcap->current.decimal > delta )
	{
		if ( delta > bg_ladder_yawcap->current.decimal )
			delta -= bg_ladder_yawcap->current.decimal;
		else
			delta += bg_ladder_yawcap->current.decimal;

		ps->delta_angles[YAW] += ANGLE2SHORT(delta);

		if ( delta > 0 )
			ps->viewangles[YAW] = AngleNormalize360Accurate(ladderFacing - bg_ladder_yawcap->current.decimal);
		else
			ps->viewangles[YAW] = AngleNormalize360Accurate(ladderFacing + bg_ladder_yawcap->current.decimal);
	}
}

/*
==================
PM_UpdateViewAngles_ProneYawClamp
==================
*/
void PM_UpdateViewAngles_ProneYawClamp( playerState_t *ps, float delta, qboolean proneBlocked, float oldViewYaw, float newViewYaw )
{
	float deltaYaw1;
	float deltaYaw2;

	if ( delta > bg_prone_yawcap->current.decimal || -bg_prone_yawcap->current.decimal > delta )
	{
		if ( delta > bg_prone_yawcap->current.decimal )
			delta -= bg_prone_yawcap->current.decimal;
		else
			delta += bg_prone_yawcap->current.decimal;

		ps->delta_angles[YAW] += ANGLE2SHORT(delta);

		if ( delta > 0 )
			ps->viewangles[YAW] = AngleNormalize360Accurate(ps->proneDirection - bg_prone_yawcap->current.decimal);
		else
			ps->viewangles[YAW] = AngleNormalize360Accurate(ps->proneDirection + bg_prone_yawcap->current.decimal);
	}

	if ( !proneBlocked )
	{
		return;
	}

	ps->pm_flags |= PMF_PRONE_BLOCKED;
	deltaYaw1 = AngleDelta(oldViewYaw, ps->viewangles[YAW]);

	if ( I_fabs(deltaYaw1) > 1 )
	{
		return;
	}

	deltaYaw2 = AngleDelta(newViewYaw, ps->viewangles[YAW]);

	if ( deltaYaw1 * deltaYaw2 > 0 )
	{
		ps->viewangles[YAW] = AngleNormalize360Accurate(ps->viewangles[YAW] + (deltaYaw1 * 0.98000002));
		ps->delta_angles[YAW] += ANGLE2SHORT(deltaYaw1 * 0.98000002);
	}
}

/*
==================
PM_UpdateViewAngles_PronePitchClamp
==================
*/
void PM_UpdateViewAngles_PronePitchClamp( playerState_t *ps )
{
	float delta = AngleDelta(ps->proneTorsoPitch, ps->viewangles[PITCH]);

	if ( delta > 45 || delta < -45 )
	{
		if ( delta > 45 )
			delta -= 45;
		else
			delta += 45;

		ps->delta_angles[PITCH] += ANGLE2SHORT(delta);

		if ( delta > 0 )
			ps->viewangles[PITCH] = AngleNormalize180Accurate(ps->proneTorsoPitch - 45);
		else
			ps->viewangles[PITCH] = AngleNormalize180Accurate(ps->proneTorsoPitch + 45);
	}
}

/*
==================
PM_UpdateViewAngles_Prone
==================
*/
void PM_UpdateViewAngles_Prone( playerState_t *ps, float msec, usercmd_t *cmd, byte handler, float oldViewYaw )
{
	qboolean bProneOK;
	qboolean bRetry;
	qboolean bSkip;
	qboolean proneBlocked;
	float newProneYaw;
	float newViewYaw;
	float delta;

	newViewYaw = ps->viewangles[YAW];
	proneBlocked = qfalse;

	delta = AngleDelta(ps->proneDirection, ps->viewangles[YAW]);

	if ( delta > bg_prone_yawcap->current.decimal - 5.0 || -(bg_prone_yawcap->current.decimal - 5.0) > delta || (cmd->forwardmove || cmd->rightmove) && delta != 0 )
	{
		if ( msec * 55.0 * 0.001 > I_fabs(delta) )
		{
			newProneYaw = ps->viewangles[YAW];
		}
		else
		{
			if ( delta > 0 )
				newProneYaw = ps->proneDirection - msec * 55.0 * 0.001;
			else
				newProneYaw = msec * 55.0 * 0.001 + ps->proneDirection;
		}

		bRetry = qtrue;
		bSkip = qfalse;

		while ( 1 )
		{
			if ( BG_CheckProneTurned(ps, newProneYaw, handler) )
			{
				break;
			}

			if ( !bRetry )
			{
				bSkip = qtrue;
				break;
			}

			delta = AngleDelta(ps->proneDirection, newProneYaw);

			bRetry = I_fabs(delta) > 1;

			if ( bRetry )
			{
				if ( delta > 0 )
					delta = 1;
				else
					delta = -1;
			}
			else
			{
				proneBlocked = qtrue;
			}

			newProneYaw = AngleNormalize360Accurate(newProneYaw + delta);
		}

		if ( !bSkip )
		{
			bProneOK = BG_CheckProne(ps->clientNum, ps->origin, ps->maxs[0], 30.0, ps->viewangles[YAW],
			                         NULL, NULL, NULL, qtrue, ps->groundEntityNum != ENTITYNUM_NONE, NULL, handler, PCT_CLIENT, PRONE_FEET_DIST_TURNED);
			if ( bProneOK )
			{
				bProneOK = BG_CheckProne(ps->clientNum, ps->origin, ps->maxs[0], 30.0, newProneYaw,
				                         NULL, NULL, NULL, qtrue, ps->groundEntityNum != ENTITYNUM_NONE, NULL, handler, PCT_CLIENT, PRONE_FEET_DIST_TURNED);
				if ( bProneOK )
				{
					ps->proneDirection = newProneYaw;
				}
			}

			if ( !bProneOK )
			{
				proneBlocked = qtrue;
			}
		}
	}

	delta = AngleDelta(ps->proneDirection, ps->viewangles[YAW]);

	if ( delta != 0 )
	{
		newProneYaw = ps->proneDirection;
		bRetry = qtrue;

		while ( 1 )
		{
			bProneOK = BG_CheckProne(ps->clientNum, ps->origin, ps->maxs[0], 30.0, newProneYaw,
			                         NULL, NULL, NULL, qtrue, ps->groundEntityNum != ENTITYNUM_NONE, NULL, handler, PCT_CLIENT, PRONE_FEET_DIST_TURNED);
			if ( bProneOK )
			{
				if ( BG_CheckProneTurned(ps, newProneYaw, handler) )
				{
					break;
				}
			}

			if ( !bRetry )
			{
				break;
			}

			bRetry = I_fabs(delta) > 1;

			if ( bRetry )
			{
				if ( delta > 0 )
					delta = 1;
				else
					delta = -1;
			}

			proneBlocked = qtrue;

			ps->delta_angles[YAW] += ANGLE2SHORT(delta);
			ps->viewangles[YAW] = AngleNormalize360Accurate(ps->viewangles[YAW] + delta);

			delta = AngleDelta(ps->proneDirection, ps->viewangles[YAW]);

			if ( !bProneOK )
			{
				newProneYaw = AngleNormalize360Accurate(newProneYaw + delta);
			}
		}

		ps->proneDirection = newProneYaw;
	}

	PM_UpdateViewAngles_ProneYawClamp(ps, delta, proneBlocked, oldViewYaw, newViewYaw);
	PM_UpdateViewAngles_PronePitchClamp(ps);
}

/*
================
PM_UpdateViewAngles

This can be used as another entry point when only the viewangles
are being updated isntead of a full move

	!! NOTE !! Any changes to mounted/prone view should be duplicated in BotEntityWithinView()
================
*/
// rain - take a tracemask as well - we can't use anything out of pm
void PM_UpdateViewAngles( playerState_t *ps, float msec, usercmd_t *cmd, byte handler )
{
	float oldViewYaw;

	if ( ps->pm_type == PM_INTERMISSION )
	{
		return; // no view changes at all
	}

	if ( ps->pm_type >= PM_DEAD )
	{
		if ( ps->stats[STAT_DEAD_YAW] == 999 )
		{
			ps->stats[STAT_DEAD_YAW] = ps->delta_angles[YAW] + cmd->angles[YAW] * 0.0054931641;
		}

		PM_UpdateLean(ps, msec, cmd, pmoveHandlers[handler].trace);
		return;
	}

	oldViewYaw = ps->viewangles[YAW];
	PM_UpdateViewAngles_Clamp(ps, cmd, handler);

	if ( ps->eFlags & EF_TURRET_ACTIVE )
	{
		PM_UpdateViewAngles_RangeLimited(ps);
		return;
	}

	if ( ps->pm_flags & PMF_MANTLE )
	{
		Mantle_CapView(ps);
		return;
	}

	if ( ps->pm_flags & PMF_LADDER && ps->groundEntityNum == ENTITYNUM_NONE && bg_ladder_yawcap->current.decimal != 0 )
	{
		PM_UpdateViewAngles_LadderClamp(ps);
	}

	if ( ps->pm_flags & PMF_PRONE )
	{
		assert(!(ps->eFlags & EF_TURRET_ACTIVE));
		PM_UpdateViewAngles_Prone(ps, msec, cmd, handler, oldViewYaw);
	}

	if ( ps->pm_type != PM_UFO && ps->pm_type != PM_NOCLIP && ps->pm_type != PM_SPECTATOR )
	{
		PM_UpdateLean(ps, msec, cmd, pmoveHandlers[handler].trace);
	}
}

/*
================
Pmove

Can be called by either the server or the client
================
*/
void Pmove( pmove_t *pmove )
{
	int msec;
	playerState_t *ps;
	int finalTime;

	ps = pmove->ps;
	assert(ps);

	finalTime = pmove->cmd.serverTime;

	if ( finalTime < ps->commandTime )
	{
		return; // should not happen
	}

	if ( finalTime > ps->commandTime + 1000 )
	{
		ps->commandTime = finalTime - 1000;
	}

	pmove->numtouch = 0;

	// chop the move up if it is too long, to prevent framerate
	// dependent behavior
	while ( ps->commandTime != finalTime )
	{
		msec = finalTime - ps->commandTime;

		// rain - this was 66 (15fps), but I've changed it to
		// 50 (20fps, max rate of mg42) to alleviate some of the
		// framerate dependency with the mg42.
		// in reality, this should be split according to sv_fps,
		// and pmove() shouldn't handle weapon firing
		if ( msec > 66 )
		{
			msec = 66;
		}

		pmove->cmd.serverTime = ps->commandTime + msec;
		PmoveSingle(pmove);

		pmove->oldcmd = pmove->cmd;
	}
}

/*
================
PM_ClearLadderFlag
================
*/
void PM_ClearLadderFlag( playerState_t *ps )
{
	if ( ps->pm_flags & PMF_LADDER )
	{
		ps->pm_flags |= PMF_LADDER_END;
	}

	ps->pm_flags &= ~PMF_LADDER;
}

/*
================
PM_SetLadderFlag
================
*/
void PM_SetLadderFlag( playerState_t *ps )
{
	ps->pm_flags |= PMF_LADDER;
}

/*
================
PM_UpdatePlayerWalkingFlag
================
*/
void PM_UpdatePlayerWalkingFlag( pmove_t *pm )
{
	playerState_t *ps;

	ps = pm->ps;
	assert(ps);

	ps->pm_flags &= ~PMF_ADS_WALK;

	if ( ps->pm_type >= PM_DEAD )
	{
		return;
	}

	if ( !(pm->cmd.buttons & BUTTON_ADS) )
	{
		return;
	}

	if ( ps->pm_flags & PMF_PRONE )
	{
		return;
	}

	if ( !(ps->pm_flags & PMF_ADS) )
	{
		return;
	}

	if ( ps->weaponstate == WEAPON_RELOADING )
	{
		return;
	}

	if ( ps->weaponstate == WEAPON_RELOAD_START )
	{
		return;
	}

	if ( ps->weaponstate == WEAPON_RELOAD_END )
	{
		return;
	}

	if ( ps->weaponstate == WEAPON_RELOAD_START_INTERUPT )
	{
		return;
	}

	if ( ps->weaponstate == WEAPON_RELOADING_INTERUPT )
	{
		return;
	}

	ps->pm_flags |= PMF_ADS_WALK;
}

/*
================
PM_DropTimers
================
*/
static void PM_DropTimers( playerState_t *ps, pml_t *pml )
{
	// drop misc timing counter
	if ( ps->pm_time )
	{
		if ( pml->msec >= ps->pm_time )
		{
			ps->pm_flags &= ~PMF_ALL_TIMES;
			ps->pm_time = 0;
		}
		else
		{
			ps->pm_time -= pml->msec;
		}
	}

	// drop animation counter
	if ( ps->legsTimer > 0 )
	{
		ps->legsTimer -= pml->msec;
		if ( ps->legsTimer < 0 )
		{
			ps->legsTimer = 0;
		}
	}

	if ( ps->torsoTimer > 0 )
	{
		ps->torsoTimer -= pml->msec;
		if ( ps->torsoTimer < 0 )
		{
			ps->torsoTimer = 0;
		}
	}
}

/*
================
PM_GetFlinchAnim
================
*/
int PM_GetFlinchAnim( float yaw )
{
	if ( yaw < 0 )
	{
		yaw += 360;
	}

	if ( yaw >= 315 || yaw < 45 )
	{
		return ANIM_MT_FLINCH_FORWARD;
	}

	if ( yaw < 135 )
	{
		return ANIM_MT_FLINCH_LEFT;
	}

	if ( yaw < 225 )
	{
		return ANIM_MT_FLINCH_BACKWARD;
	}

	return ANIM_MT_FLINCH_RIGHT;
}

/*
================
PM_ViewHeightTableLerp
================
*/
float PM_ViewHeightTableLerp( int iFrac, viewLerpWaypoint_s *pTable, float *pfPosOfs )
{
	float fEntryFrac;
	viewLerpWaypoint_s *pCurr;
	viewLerpWaypoint_s *pPrev;
	int i;

	if ( !iFrac )
	{
		*pfPosOfs = (float)pTable->iOffset;
		return pTable->fViewHeight;
	}

	assert(iFrac < 100);
	pCurr = pTable + 1;
	i = 1;

	do
	{
		if ( iFrac == pCurr->iFrac )
		{
			*pfPosOfs = (float)pCurr->iOffset;
			return pCurr->fViewHeight;
		}

		if ( pCurr->iFrac > iFrac )
		{
			pPrev = &pTable[i - 1];
			assert((pCurr->iFrac - pPrev->iFrac) > 0);
			fEntryFrac = (float)(iFrac - pPrev->iFrac) / (float)(pCurr->iFrac - pPrev->iFrac);
			*pfPosOfs = (float)pPrev->iOffset + (float)((float)(pCurr->iOffset - pPrev->iOffset) * fEntryFrac);
			return (pCurr->fViewHeight - pPrev->fViewHeight) * fEntryFrac + pPrev->fViewHeight;
		}

		pCurr = &pTable[++i];
	}
	while ( pCurr->iFrac != -1 );

	assert(va( "No encapsulating table entries found for fraction %i", iFrac ));
	*pfPosOfs = (float)pTable->iOffset;

	return pTable->fViewHeight;
}

/*
================
PM_GetViewHeightLerp
================
*/
float PM_GetViewHeightLerp( const pmove_t *pm, int iFromHeight, int iToHeight )
{
	float fLerpFrac;
	playerState_t *ps;

	ps = pm->ps;

	if ( !pm->ps->viewHeightLerpTime )
	{
		return 0;
	}

	if ( iFromHeight != -1 && iToHeight != -1 && (iToHeight != ps->viewHeightLerpTarget || iToHeight == VIEW_HEIGHT_CROUCHED
	        && (iFromHeight != VIEW_HEIGHT_PRONE || ps->viewHeightLerpDown) && (iFromHeight != VIEW_HEIGHT_STANDING || !ps->viewHeightLerpDown)) )
	{
		return 0;
	}

	fLerpFrac = (float)(pm->cmd.serverTime - ps->viewHeightLerpTime) / (float)PM_GetViewHeightLerpTime(ps, ps->viewHeightLerpTarget, ps->viewHeightLerpDown);

	if ( fLerpFrac < 0 )
	{
		return 0;
	}

	if ( fLerpFrac > 1 )
	{
		return 1;
	}

	return fLerpFrac;
}

/*
=============
PM_CorrectAllSolid
=============
*/
static qboolean PM_CorrectAllSolid( pmove_t *pm, pml_t *pml, trace_t *trace )
{
	playerState_t *ps;
	vec3_t point;
	unsigned int i;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	// jitter around
	for ( i = 0; i < ARRAY_COUNT(CorrectSolidDeltas); i++ )
	{
		VectorAdd(ps->origin, CorrectSolidDeltas[i], point);
		PM_playerTrace(pm, trace, point, pm->mins, pm->maxs, point, ps->clientNum, pm->tracemask);

		if ( trace->startsolid )
		{
			continue;
		}

		VectorCopy(point, ps->origin);

		point[0] = ps->origin[0];
		point[1] = ps->origin[1];
		point[2] = ps->origin[2] - 1.0;

		PM_playerTrace(pm, trace, ps->origin, pm->mins, pm->maxs, point, ps->clientNum, pm->tracemask);
		pml->groundTrace = *trace;
		Vec3Lerp(ps->origin, point, trace->fraction, ps->origin);

		return qtrue;
	}

	ps->groundEntityNum = ENTITYNUM_NONE;
	pml->groundPlane = qfalse;
	pml->almostGroundPlane = qfalse;
	pml->walking = qfalse;

	Jump_ClearState(ps);

	return qfalse;
}

/*
=============
PM_DamageLandingForSurface
=============
*/
int PM_DamageLandingForSurface( pml_t *pml )
{
	int iSurfType;

	iSurfType = PM_GroundSurfaceType(pml);

	if ( !iSurfType )
	{
#ifndef NDEBUG
		Com_Printf(
		    "PM_DamageLandingForSurface has been called with a ground surface of type 'NONE' near (%.2f %.2f %.2f). \n"
		    "This means a player has landed on a surface that wasn't properly setup to be used as a ground surface. \n"
		    "Use a different material which has a surface type set.\n",
		    pml->previous_origin[0],
		    pml->previous_origin[1],
		    pml->previous_origin[2]);
#endif
	}

	return EV_LANDING_PAIN_DEFAULT + iSurfType;
}

/*
=============
PM_HardLandingForSurface
=============
*/
int PM_HardLandingForSurface( pml_t *pml )
{
	int iSurfType;

	iSurfType = PM_GroundSurfaceType(pml);

	if ( !iSurfType )
	{
#ifndef NDEBUG
		Com_Printf(
		    "PM_HardLandingForSurface has been called with a ground surface of type 'NONE' near (%.2f %.2f %.2f). \n"
		    "This means a player has landed on a surface that wasn't properly setup to be used as a ground surface. \n"
		    "Use a different material which has a surface type set.\n",
		    pml->previous_origin[0],
		    pml->previous_origin[1],
		    pml->previous_origin[2]);
#endif
	}

	return EV_LANDING_DEFAULT + iSurfType;
}

/*
=============
PM_MediumLandingForSurface
=============
*/
int PM_MediumLandingForSurface( pml_t *pml )
{
	int iSurfType;

	iSurfType = PM_GroundSurfaceType(pml);

	if ( !iSurfType )
	{
#ifndef NDEBUG
		Com_Printf(
		    "PM_MediumLandingForSurface has been called with a ground surface of type 'NONE' near (%.2f %.2f %.2f). \n"
		    "This means a player has landed on a surface that wasn't properly setup to be used as a ground surface. \n"
		    "Use a different material which has a surface type set.\n",
		    pml->previous_origin[0],
		    pml->previous_origin[1],
		    pml->previous_origin[2]);
#endif
		return EV_NONE;
	}

	return EV_FOOTSTEP_WALK_DEFAULT + iSurfType;
}

/*
=============
PM_LightLandingForSurface
=============
*/
int PM_LightLandingForSurface( pml_t *pml )
{
	int iSurfType;

	iSurfType = PM_GroundSurfaceType(pml);

	if ( !iSurfType )
	{
#ifndef NDEBUG
		Com_Printf(
		    "PM_LightLandingForSurface has been called with a ground surface of type 'NONE' near (%.2f %.2f %.2f). \n"
		    "This means a player has landed on a surface that wasn't properly setup to be used as a ground surface. \n"
		    "Use a different material which has a surface type set.\n",
		    pml->previous_origin[0],
		    pml->previous_origin[1],
		    pml->previous_origin[2]);
#endif
		return EV_NONE;
	}

	return EV_FOOTSTEP_RUN_DEFAULT + iSurfType;
}

/*
================
PM_FootstepForSurface

Returns an event number apropriate for the groundsurface
================
*/
int PM_FootstepForSurface( playerState_t *ps, pml_t *pml )
{
	int iSurfType;

	iSurfType = PM_GroundSurfaceType(pml);

	if ( !iSurfType )
	{
#ifndef NDEBUG
		Com_Printf(
		    "PM_FootstepForSurface has been called with a ground surface of type 'NONE' near (%.2f %.2f %.2f). \n"
		    "This means a player has landed on a surface that wasn't properly setup to be used as a ground surface. \n"
		    "Use a different material which has a surface type set.\n",
		    pml->previous_origin[0],
		    pml->previous_origin[1],
		    pml->previous_origin[2]);
#endif
		return EV_NONE;
	}

	if ( ps->pm_flags & PMF_PRONE )
	{
		return EV_FOOTSTEP_PRONE_DEFAULT + iSurfType;
	}

	if ( !(ps->pm_flags & PMF_ADS_WALK) && ps->leanf == 0 )
	{
		return EV_FOOTSTEP_RUN_DEFAULT + iSurfType;
	}

	return EV_FOOTSTEP_WALK_DEFAULT + iSurfType;
}

/*
============
PM_CmdScale

Returns the scale factor to apply to cmd movements
This allows the clients to use axial -127 to 127 values for all directions
without getting a sqrt(2) distortion in speed.
============
*/
static float PM_CmdScale( playerState_t *ps, usercmd_t *cmd )
{
	int max;
	float total;
	float scale;

	assert(ps);

	max = abs( cmd->forwardmove );
	if ( abs( cmd->rightmove ) > max )
	{
		max = abs( cmd->rightmove );
	}
	if ( !max )
	{
		return 0;
	}

	total = I_sqrt( Square(cmd->forwardmove) + Square(cmd->rightmove) );
	scale = (float)ps->speed * max / ( 127.0 * total );

	if ( ps->pm_flags & PMF_ADS_WALK || ps->leanf != 0 )
	{
		scale *= 0.40000001;
	}

	if ( ps->pm_type == PM_NOCLIP )
	{
		scale *= 3;
	}

	if ( ps->pm_type == PM_UFO )
	{
		scale *= 6;
	}

	if ( ps->pm_type == PM_SPECTATOR )
	{
		scale *= player_spectateSpeedScale->current.decimal;
	}

	return scale;
}

/*
============
PM_FoliageSounds
============
*/
void PM_FoliageSounds( pmove_t *pm )
{
	playerState_t *ps;
	vec3_t maxs;
	vec3_t mins;
	trace_t trace;
	int interval;
	float speedFrac;

	ps = pm->ps;
	assert(ps);

	if ( bg_foliagesnd_minspeed->current.decimal > pm->xyspeed )
	{
		if ( bg_foliagesnd_resetinterval->current.integer + ps->foliageSoundTime < pm->cmd.serverTime )
		{
			ps->foliageSoundTime = 0;
		}

		return;
	}

	assert(bg_foliagesnd_maxspeed->current.decimal - bg_foliagesnd_minspeed->current.decimal > 0);
	speedFrac = (pm->xyspeed - bg_foliagesnd_minspeed->current.decimal) / (bg_foliagesnd_maxspeed->current.decimal - bg_foliagesnd_minspeed->current.decimal);

	if ( speedFrac > 1 )
	{
		speedFrac = 1;
	}

	interval = (bg_foliagesnd_fastinterval->current.integer - bg_foliagesnd_slowinterval->current.integer) * speedFrac + bg_foliagesnd_slowinterval->current.integer;

	if ( interval + ps->foliageSoundTime >= pm->cmd.serverTime )
	{
		return;
	}

	VectorScale(pm->mins, 0.75, mins);
	VectorScale(pm->maxs, 0.75, maxs);

	maxs[2] = pm->maxs[2] * 0.89999998;

	PM_playerTrace(pm, &trace, ps->origin, mins, maxs, ps->origin, ps->clientNum, CONTENTS_FOILAGE);

	if ( trace.startsolid )
	{
		PM_AddEvent(ps, EV_FOLIAGE_SOUND);
		ps->foliageSoundTime = pm->cmd.serverTime;
	}
}

/*
==============
PM_DeadMove
==============
*/
static void PM_DeadMove( playerState_t *ps, pml_t *pml )
{
	float forward;

	if ( !pml->walking )
	{
		return;
	}

	// extra friction

	forward = VectorLength( ps->velocity );
	forward -= 20;
	if ( forward <= 0 )
	{
		VectorClear( ps->velocity );
	}
	else
	{
		Vec3Normalize( ps->velocity );
		VectorScale( ps->velocity, forward, ps->velocity );
	}
}

/*
================
PM_SetMovementDir

Determine the rotation of the legs reletive
to the facing dir
================
*/
static void PM_SetMovementDir( pmove_t *pm, pml_t *pml )
{
	// Ridah, changed this for more realistic angles (at the cost of more network traffic?)
	float speed;
	vec3_t moved;
	int moveyaw;
	playerState_t *ps;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	// prone move
	if ( ps->pm_flags & PMF_PRONE && !(ps->eFlags & EF_TURRET_ACTIVE) )
	{
		moveyaw = (int)AngleDelta(ps->proneDirection, ps->viewangles[YAW]);

		if ( abs( moveyaw ) > 90 )
		{
			if ( moveyaw > 0 )
			{
				moveyaw = 90;
			}
			else
			{
				moveyaw = -90;
			}
		}

		ps->movementDir = (signed char)moveyaw;
		return;
	}

	// ladder move
	if ( ps->pm_flags & PMF_LADDER )
	{
		speed = vectoyaw(ps->vLadderVec) + 180;
		moveyaw = (int)AngleDelta(speed, ps->viewangles[YAW]);

		if ( abs( moveyaw ) > 90 )
		{
			if ( moveyaw > 0 )
			{
				moveyaw = 90;
			}
			else
			{
				moveyaw = -90;
			}
		}

		ps->movementDir = (signed char)moveyaw;
		return;
	}

	VectorSubtract( ps->origin, pml->previous_origin, moved );

	if (    ( pm->cmd.forwardmove || pm->cmd.rightmove )
	        &&  ( ps->groundEntityNum != ENTITYNUM_NONE )
	        &&  ( speed = VectorLength( moved ) )
	        &&  ( speed > pml->frametime * 5 ) )   // if moving slower than 20 units per second, just face head angles
	{
		vec3_t dir;

		Vec3NormalizeTo( moved, dir );
		vectoangles( dir, dir );

		moveyaw = (int)AngleDelta( dir[YAW], ps->viewangles[YAW] );

		if ( pm->cmd.forwardmove < 0 )
		{
			moveyaw = (int)AngleNormalize180( moveyaw + 180 );
		}

		if ( abs( moveyaw ) > 90 )
		{
			if ( moveyaw > 0 )
			{
				moveyaw = 90;
			}
			else
			{
				moveyaw = -90;
			}
		}

		ps->movementDir = (signed char)moveyaw;
	}
	else
	{
		ps->movementDir = 0;
	}
}

/*
============
PM_MoveScale

Returns the scale factor to apply to movements
This allows the clients to use axial -127 to 127 values for all directions
without getting a sqrt(2) distortion in speed.
============
*/
static float PM_MoveScale( playerState_t *ps, float fmove, float rmove, float umove )
{
	int max;
	float total;
	float scale;

	assert(ps);

	max = I_fabs( fmove );
	if ( I_fabs( rmove ) > max )
	{
		max = I_fabs( rmove );
	}
	if ( I_fabs( umove ) > max )
	{
		max = I_fabs( umove );
	}
	if ( !max )
	{
		return 0;
	}

	total = I_sqrt( Square(fmove) + Square(rmove) + Square(umove) );
	scale = (float)ps->speed * max / ( 127.0 * total );

	if ( ps->pm_flags & PMF_ADS_WALK || ps->leanf != 0 )
	{
		scale *= 0.40000001;
	}

	if ( ps->pm_type == PM_NOCLIP )
	{
		scale *= 3;
	}

	if ( ps->pm_type == PM_UFO )
	{
		scale *= 6;
	}

	if ( ps->pm_type == PM_SPECTATOR )
	{
		scale *= player_spectateSpeedScale->current.decimal;
	}

	return scale;
}

/*
==================
PM_Friction

Handles both ground friction and water friction
==================
*/
static void PM_Friction( playerState_t *ps, pml_t *pml )
{
	vec3_t vec;
	float   *vel;
	float speed, newspeed, control;
	float drop;

	vel = ps->velocity;

	VectorCopy( vel, vec );
	if ( pml->walking )
	{
		vec[2] = 0; // ignore slope movement
	}

	speed = VectorLength( vec );
	// rain - #179 don't do this for PM_SPECTATOR/PM_NOCLIP, we always want them to stop
	if ( speed < 1  )
	{
		VectorClear(vel);     // allow sinking underwater
		// FIXME: still have z friction underwater?
		return;
	}

	drop = 0;

	// apply ground friction
	if ( pml->walking && !( pml->groundTrace.surfaceFlags & SURF_SLICK ) )
	{
		// if getting knocked back, no friction
		if ( !( ps->pm_flags & PMF_TIME_KNOCKBACK ) )
		{
			control = speed < stopspeed->current.decimal ? stopspeed->current.decimal : speed;

			if ( ps->pm_flags & PMF_TIME_SLIDE )
			{
				control *= 0.30000001;
			}
			else if ( ps->pm_flags & PMF_TIME_LAND )
			{
				control *= Jump_ReduceFriction(ps);
			}

			drop += control * friction->current.decimal * pml->frametime;
		}
	}

	if ( ps->pm_type == PM_SPECTATOR )
	{
		drop += speed * 5 * pml->frametime;
	}

	// scale the velocity
	newspeed = speed - drop;
	if ( newspeed < 0 )
	{
		newspeed = 0;
	}
	newspeed /= speed;

	// rain - used VectorScale instead of multiplying by hand
	VectorScale( vel, newspeed, vel );
}

/*
================
PM_CheckLadderMove

  Checks to see if we are on a ladder
================
*/
void PM_CheckLadderMove( pmove_t *pm, pml_t *pml )
{
	playerState_t *ps;
	float tracedist;
#define TRACE_LADDER_DIST   30.0
	qboolean fellOffLadderInAir;
	vec3_t vLadderCheckDir;
	vec3_t spot;
	trace_t trace;
	vec3_t mins;
	vec3_t maxs;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	if ( pml->walking )
	{
		ps->pm_flags &= ~PMF_LADDER_END;
	}

	if ( ps->pm_time && ps->pm_flags & ( PMF_TIME_SLIDE | PMF_TIME_KNOCKBACK) )
	{
		return;
	}

	if ( pml->walking )
	{
		tracedist = 8.0;
	}
	else
	{
		tracedist = TRACE_LADDER_DIST;
	}

	fellOffLadderInAir = ( ( pm->ps->pm_flags & PMF_LADDER ) != 0 ) && ps->groundEntityNum == ENTITYNUM_NONE;

	if ( fellOffLadderInAir )
	{
		VectorNegate(ps->vLadderVec, vLadderCheckDir);
	}
	else
	{
		// check for ladder
		vLadderCheckDir[0] = pml->forward[0];
		vLadderCheckDir[1] = pml->forward[1];
		vLadderCheckDir[2] = 0.0;

		Vec3Normalize(vLadderCheckDir);
	}

	if ( ps->pm_type >= PM_DEAD )
	{
		ps->groundEntityNum = ENTITYNUM_NONE;
		pml->groundPlane = qfalse;
		pml->almostGroundPlane = qfalse;
		pml->walking = qfalse;
		PM_ClearLadderFlag(ps);
		return;
	}

	if ( ps->pm_flags & PMF_LADDER_END || PM_GetEffectiveStance(ps) == PM_EFF_STANCE_PRONE || pm->cmd.serverTime - ps->jumpTime < 300 )
	{
		PM_ClearLadderFlag(ps);
		return;
	}

	VectorCopy(pm->mins, mins);

	mins[0] = mins[0] + 6.0;
	mins[1] = mins[1] + 6.0;
	mins[2] = 8.0;

	VectorCopy(pm->maxs, maxs);

	maxs[0] = maxs[0] - 6.0;
	maxs[1] = maxs[1] - 6.0;

	if ( mins[2] > maxs[2] )
	{
		maxs[2] = mins[2];
	}

	assert(maxs[0] >= mins[0]);
	assert(maxs[1] >= mins[1]);
	assert(maxs[2] >= mins[2]);

	VectorMA(ps->origin, tracedist, vLadderCheckDir, spot);
	PM_playerTrace(pm, &trace, ps->origin, mins, maxs, spot, ps->clientNum, pm->tracemask);

	if ( ( trace.fraction < 1 ) && ( trace.surfaceFlags & SURF_LADDER ) )
	{
		if ( !pml->walking || pm->cmd.forwardmove > 0 )
		{
			if ( ps->pm_flags & PMF_LADDER )
			{
				PM_SetLadderFlag(ps); // set ladder bit
				return;
			}

			// if we are only just on the ladder, don't do this yet, or it may throw us back off the ladder
			VectorCopy(trace.normal, ps->vLadderVec);
			VectorNegate(ps->vLadderVec, vLadderCheckDir);

			VectorMA(ps->origin, tracedist, vLadderCheckDir, spot);
			PM_playerTrace(pm, &trace, ps->origin, mins, maxs, spot, ps->clientNum, pm->tracemask);

			if ( ( trace.fraction < 1 ) && ( trace.surfaceFlags & SURF_LADDER ) )
			{
				PM_SetLadderFlag(ps); // set ladder bit
				return;
			}
		}
	}

	PM_ClearLadderFlag(ps);

	if ( fellOffLadderInAir )
	{
		BG_AnimScriptEvent(ps, ANIM_ET_JUMP, qfalse, qtrue);
	}
}

/*
===============
PM_Footsteps
===============
*/
void PM_Footsteps( pmove_t *pm, pml_t *pml )
{
	qboolean iswalking;
	int turnAdjust;
	clientInfo_t *ci;
	int stumble_end_time;
	playerState_t *ps;
	float lerpFrac;
	float fMaxSpeed;
	int iStance;
	qboolean footstep;
	int old;
	float bobmove;
	int animResult = -1;

	ps = pm->ps;

	if ( pm->ps->pm_type >= PM_DEAD )
	{
		return;
	}

	if ( ps->clientNum >= MAX_CLIENTS )
		ci = NULL;
	else
		ci = &level_bgs.clientinfo[ps->clientNum];

	stumble_end_time = ps->damageDuration - player_dmgtimer_stumbleTime->current.integer;

	if ( stumble_end_time < 0 )
	{
		stumble_end_time = 0;
	}

	//
	// calculate speed and cycle to be used for
	// all cyclic walking effects
	//
	pm->xyspeed = I_sqrt( Square(ps->velocity[0]) + Square(ps->velocity[1]) );

	// mg42, always idle
	if ( ps->eFlags & EF_TURRET_ACTIVE )
	{
		if ( ps->pm_flags & PMF_PRONE )
		{
			BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_IDLEPRONE, qtrue);
			return;
		}

		if ( ps->pm_flags & PMF_DUCKED )
		{
			BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_IDLECR, qtrue);
			return;
		}

		BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_IDLE, qtrue);
		return;
	}

	iStance = PM_GetEffectiveStance(ps);
	iswalking = ps->pm_flags & PMF_ADS_WALK || ps->leanf != 0;

	// in the air
	if ( ps->groundEntityNum == ENTITYNUM_NONE && ps->pm_type != PM_NORMAL_LINKED )
	{
		if ( ps->pm_flags & PMF_LADDER ) // on ladder
		{
			float fLadderSpeed;
			float fMaxSpeed = (0.5 * 1.5) * 127.0;

			if ( pm->cmd.serverTime - ps->jumpTime < 300 )
			{
				return;
			}

			fLadderSpeed = ps->velocity[2];

			if ( !iswalking )
				bobmove = fLadderSpeed / (fMaxSpeed * 0.40000001) * 0.34999999;
			else
				bobmove = fLadderSpeed / (fMaxSpeed * 0.44999999);

			if ( fLadderSpeed >= 0.0 )
				BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_CLIMBUP, qtrue);
			else
				BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_CLIMBDOWN, qtrue);

			// check for footstep / splash sounds
			old = ps->bobCycle;
			ps->bobCycle = (int)( old + bobmove * pml->msec ) & 255;
			PM_FootstepEvent(pm, pml, old, ps->bobCycle, qtrue);
		}

		assert( iStance == (ps->pm_flags & ( PMF_DUCKED | PMF_PRONE) ) );
		return;
	}

	// if not trying to move
	if ( player_moveThreshhold->current.decimal > pm->xyspeed || ps->pm_type == PM_NORMAL_LINKED )
	{
		if ( pm->xyspeed < 1 )
		{
			ps->bobCycle = 0; // start at beginning of cycle again
		}

		turnAdjust = ANIM_MT_UNUSED;

		if ( ci && player_turnAnims->current.boolean )
		{
			if ( ci->turnAnimType && ci->turnAnimEndTime )
			{
				Com_DPrintf("turn anim end time is %i, time is %i\n", ci->turnAnimEndTime, level_bgs.time);
			}

			if ( ci->legs.yawing )
			{
				if ( ci->legs.yawAngle > ci->torso.yawAngle )
					turnAdjust = ANIM_MT_TURNRIGHT;
				else
					turnAdjust = ANIM_MT_TURNLEFT;

				ci->legs.yawAngle = ci->torso.yawAngle;
				ci->turnAnimType = turnAdjust;

				if ( ci->turnAnimEndTime < level_bgs.time )
				{
					ci->turnAnimEndTime = 0;
				}
			}
			else
			{
				if ( ci->turnAnimEndTime > level_bgs.time )
				{
					ci->legs.yawAngle = ci->torso.yawAngle;
					return;
				}

				if ( ci->turnAnimEndTime )
				{
					ci->turnAnimEndTime = 0;
					ci->legs.yawAngle = ci->torso.yawAngle;
				}
			}
		}

		if ( ps->viewHeightTarget == VIEW_HEIGHT_PRONE )
		{
			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_IDLEPRONE, qtrue);
		}
		else if ( ps->viewHeightTarget == VIEW_HEIGHT_CROUCHED )
		{
			if ( turnAdjust == ANIM_MT_TURNRIGHT )
			{
				animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_TURNRIGHTCR, qtrue);

				if ( animResult > 0 && !ci->turnAnimEndTime )
				{
					ci->turnAnimEndTime = level_bgs.time + ps->legsAnimDuration;
				}
			}
			else if ( turnAdjust == ANIM_MT_TURNLEFT )
			{
				animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_TURNLEFTCR, qtrue);

				if ( animResult > 0 && !ci->turnAnimEndTime )
				{
					ci->turnAnimEndTime = level_bgs.time + ps->legsAnimDuration;
				}
			}
			else
			{
				animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_IDLECR, qtrue);
			}
		}
		else if ( turnAdjust == ANIM_MT_TURNRIGHT )
		{
			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_TURNRIGHT, qtrue);

			if ( animResult > 0 && !ci->turnAnimEndTime )
			{
				ci->turnAnimEndTime = level_bgs.time + ps->legsAnimDuration;
			}
		}
		else if ( turnAdjust == ANIM_MT_TURNLEFT )
		{
			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_TURNLEFT, qtrue);

			if ( animResult > 0 && !ci->turnAnimEndTime )
			{
				ci->turnAnimEndTime = level_bgs.time + ps->legsAnimDuration;
			}
		}
		else
		{
			if ( ps->damageTimer > stumble_end_time )
			{
				BG_AnimScriptAnimation(ps, AISTATE_COMBAT, PM_GetFlinchAnim(ps->flinchYaw), qtrue);
				return;
			}

			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_IDLE, qtrue);
		}

		if ( animResult < 0 )
		{
			if ( ps->viewHeightTarget == VIEW_HEIGHT_CROUCHED )
			{
				BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_IDLECR, qtrue);
				return;
			}

			if ( ps->damageTimer <= stumble_end_time )
			{
				BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_IDLE, qtrue);
				return;
			}

			BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_FLINCH_FORWARD, qtrue);
		}

		return;
	}

	fMaxSpeed = ps->speed;

	if ( pm->cmd.forwardmove )
	{
		if ( pm->cmd.rightmove )
		{
			fMaxSpeed = ((player_strafeSpeedScale->current.decimal - 1.0) * 0.75 + 1.0 + 1.0) * 0.5 * fMaxSpeed;

			if ( pm->cmd.forwardmove < 0 )
			{
				fMaxSpeed = (player_backSpeedScale->current.decimal + 1.0) * 0.5 * fMaxSpeed;
			}
		}
		else if ( pm->cmd.forwardmove < 0 )
		{
			fMaxSpeed = fMaxSpeed * player_backSpeedScale->current.decimal;
		}

		BG_UpdateConditionValue(ps->clientNum, ANIM_COND_STRAFING, LEANING_NOT, qtrue);
	}
	else if ( pm->cmd.rightmove )
	{
		fMaxSpeed = ((player_strafeSpeedScale->current.decimal - 1.0) * 0.75 + 1.0) * fMaxSpeed;

		if ( pm->cmd.rightmove > 0 )
			BG_UpdateConditionValue(ps->clientNum, ANIM_COND_STRAFING, LEANING_RIGHT, qtrue);
		else
			BG_UpdateConditionValue(ps->clientNum, ANIM_COND_STRAFING, LEANING_LEFT, qtrue);
	}

	if ( iswalking )
	{
		fMaxSpeed *= 0.40000001;
	}

	lerpFrac = PM_GetViewHeightLerp(pm, VIEW_HEIGHT_CROUCHED, VIEW_HEIGHT_PRONE);

	if ( lerpFrac == 0 )
	{
		lerpFrac = PM_GetViewHeightLerp(pm, VIEW_HEIGHT_PRONE, VIEW_HEIGHT_CROUCHED);

		if ( lerpFrac == 0 )
		{
			if ( iStance == PM_EFF_STANCE_PRONE )
			{
				fMaxSpeed = fMaxSpeed * 0.15000001;
			}
			else if ( iStance == PM_EFF_STANCE_CROUCH )
			{
				fMaxSpeed = fMaxSpeed * 0.64999998;
			}
		}
		else
		{
			fMaxSpeed = (lerpFrac * 0.64999998 + (1.0 - lerpFrac) * 0.15000001) * fMaxSpeed;
		}
	}
	else
	{
		fMaxSpeed = (lerpFrac * 0.15000001 + (1.0 - lerpFrac) * 0.64999998) * fMaxSpeed;
	}

	if ( iStance == PM_EFF_STANCE_PRONE )
	{
		if ( iswalking )
			bobmove = pm->xyspeed / fMaxSpeed * 0.23999999;
		else
			bobmove = pm->xyspeed / fMaxSpeed * 0.25;

		if ( !(ps->pm_flags & PMF_BACKWARDS_RUN) )
			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_WALKPRONE, qtrue);
		else
			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_WALKPRONEBK, qtrue);
	}
	else if ( iStance == PM_EFF_STANCE_CROUCH )
	{
		if ( iswalking )
			bobmove = pm->xyspeed / fMaxSpeed * 0.315;
		else
			bobmove = pm->xyspeed / fMaxSpeed * 0.34;

		if ( !(ps->pm_flags & PMF_BACKWARDS_RUN) )
		{
			if ( iswalking )
			{
				if ( ps->damageTimer <= stumble_end_time )
				{
					animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_WALKCR, qtrue);
				}
			}
			else
			{
				if ( ps->damageTimer > stumble_end_time )
				{
					animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_STUMBLE_CROUCH_FORWARD, qtrue);
				}
				else
				{
					animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_RUNCR, qtrue);
				}
			}
		}
		else
		{
			if ( iswalking )
			{
				if ( ps->damageTimer <= stumble_end_time )
				{
					animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_WALKCRBK, qtrue);
				}
			}
			else
			{
				if ( ps->damageTimer > stumble_end_time )
				{
					animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_STUMBLE_CROUCH_BACKWARD, qtrue);
				}
				else
				{
					animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_RUNCRBK, qtrue);
				}
			}
		}
	}
	else if ( !(ps->pm_flags & PMF_BACKWARDS_RUN) )
	{
		if ( iswalking )
		{
			bobmove = pm->xyspeed / fMaxSpeed * 0.30500001;

			if ( ps->damageTimer > stumble_end_time )
				animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_STUMBLE_WALK_FORWARD, qtrue);
			else
				animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_WALK, qtrue);
		}
		else
		{
			bobmove = pm->xyspeed / fMaxSpeed * 0.33500001;

			if ( ps->damageTimer > stumble_end_time )
				animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_STUMBLE_FORWARD, qtrue);
			else
				animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_RUN, qtrue);
		}
	}
	else if ( iswalking )
	{
		bobmove = pm->xyspeed / fMaxSpeed * 0.32499999;

		if ( ps->damageTimer > stumble_end_time )
			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_STUMBLE_WALK_BACKWARD, qtrue);
		else
			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_WALKBK, qtrue);
	}
	else
	{
		bobmove = pm->xyspeed / fMaxSpeed * 0.36000001;

		if ( ps->damageTimer > stumble_end_time )
			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_STUMBLE_BACKWARD, qtrue);
		else
			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_RUNBK, qtrue);
	}

	// check for footstep / splash sounds
	footstep = PM_ShouldMakeFootsteps(pm);
	old = ps->bobCycle;
	ps->bobCycle = (int)( old + bobmove * pml->msec ) & 255;

	// if not trying to move
	if ( !pm->cmd.forwardmove && !pm->cmd.rightmove )
	{
		if ( pm->xyspeed > 120 )
		{
			return; // continue what they were doing last frame, until we stop
		}

		if ( ps->viewHeightTarget == VIEW_HEIGHT_PRONE )
		{
			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_IDLEPRONE, qtrue);
		}
		else if ( ps->viewHeightTarget == VIEW_HEIGHT_CROUCHED )
		{
			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_IDLECR, qtrue);
		}

		if ( animResult < 0 )
		{
			if ( ps->damageTimer > stumble_end_time )
			{
				BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_FLINCH_FORWARD, qtrue);
			}
			else
			{
				BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_IDLE, qtrue);
			}
		}
		//
		return;
	}

	// if no anim found yet, then just use the idle as default
	if ( animResult < 0 )
	{
		BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_IDLE, qtrue);
	}

	PM_FootstepEvent(pm, pml, old, ps->bobCycle, footstep);
}











































































void PM_Accelerate(playerState_s *ps, const pml_t *pml, float *wishdir, float wishspeed, float accel)
{
	vec2_t velocity;
	vec3_t pushDir;
	float pushLen;
	float canPush;
	float control;
	float addspeed;
	float currentspeed;
	float accelspeed;

	if ( (ps->pm_flags & 0x20) != 0 )
	{
		velocity[0] = wishspeed * wishdir[1];
		velocity[1] = wishspeed * wishdir[2];

		pushDir[0] = (float)(wishspeed * *wishdir) - ps->velocity[0];
		pushDir[1] = velocity[0] - ps->velocity[1];
		pushDir[2] = velocity[1] - ps->velocity[2];

		pushLen = Vec3Normalize(pushDir);
		canPush = (float)(accel * pml->frametime) * wishspeed;

		if ( canPush > pushLen )
			canPush = pushLen;

		ps->velocity[0] = (float)(canPush * pushDir[0]) + ps->velocity[0];
		ps->velocity[1] = (float)(canPush * pushDir[1]) + ps->velocity[1];
		ps->velocity[2] = (float)(canPush * pushDir[2]) + ps->velocity[2];
	}
	else
	{
		currentspeed = (float)((float)(ps->velocity[0] * *wishdir) + (float)(ps->velocity[1] * wishdir[1]))
		               + (float)(ps->velocity[2] * wishdir[2]);

		addspeed = wishspeed - currentspeed;

		if ( (float)(wishspeed - currentspeed) > 0.0 )
		{
			if ( stopspeed->current.decimal <= wishspeed )
				control = wishspeed;
			else
				control = stopspeed->current.decimal;

			accelspeed = (float)(accel * pml->frametime) * control;

			if ( accelspeed > addspeed )
				accelspeed = addspeed;

			ps->velocity[0] = (float)(accelspeed * wishdir[0]) + ps->velocity[0];
			ps->velocity[1] = (float)(accelspeed * wishdir[1]) + ps->velocity[1];
			ps->velocity[2] = (float)(accelspeed * wishdir[2]) + ps->velocity[2];
		}
	}
}















void PM_FlyMove(pmove_t *pm, pml_t *pml)
{
	playerState_s *ps;
	float scale;
	float movescale;
	vec3_t wishdir;
	float wishspeed;
	vec3_t wishvel;
	int i;

	ps = pm->ps;
	PM_Friction(pm->ps, pml);
	scale = PM_CmdScale(ps, &pm->cmd);

	if ( scale == 0.0 )
	{
		memset(wishvel, 0, sizeof(wishvel));
	}
	else
	{
		for ( i = 0; i < 3; ++i )
			wishvel[i] = scale * pml->forward[i] * (float)pm->cmd.forwardmove
			             + scale * pml->right[i] * (float)pm->cmd.rightmove;
	}

	if ( ps->speed )
	{
		movescale = PM_MoveScale(ps, 0.0, 0.0, 127.0);

		if ( (pm->cmd.buttons & 0x40) != 0 )
			wishvel[2] = wishvel[2] - movescale * 127.0;

		if ( SLOBYTE(pm->cmd.buttons) < 0 )
			wishvel[2] = movescale * 127.0 + wishvel[2];
	}

	VectorCopy(wishvel, wishdir);
	wishspeed = Vec3Normalize(wishdir);
	PM_Accelerate(ps, pml, wishdir, wishspeed, 8.0);
	PM_StepSlideMove(pm, pml, 0);
}



void PM_ViewHeightAdjust(pmove_t *pm, pml_t *pml)
{
	float height;
	vec3_t direction;
	vec3_t velocity;
	float adjust;
	float pfPosOfs;
	int time;
	int iFrac;
	playerState_s *ps;

	ps = pm->ps;

	if ( ps->viewHeightTarget && ps->viewHeightCurrent != 0.0 )
	{
		if ( ps->viewHeightCurrent != (float)ps->viewHeightTarget || ps->viewHeightLerpTime )
		{
			iFrac = 0;

			if ( ps->viewHeightTarget == 11 || ps->viewHeightTarget == 40 || ps->viewHeightTarget == 60 )
			{
				if ( ps->viewHeightLerpTime )
				{
					time = PM_GetViewHeightLerpTime(ps, ps->viewHeightLerpTarget, ps->viewHeightLerpDown);
					iFrac = 100 * (pm->cmd.serverTime - ps->viewHeightLerpTime) / time;

					if ( iFrac >= 0 )
					{
						if ( iFrac > 100 )
							iFrac = 100;
					}
					else
					{
						iFrac = 0;
					}

					if ( iFrac == 100 )
					{
						ps->viewHeightCurrent = (float)ps->viewHeightLerpTarget;
						ps->viewHeightLerpTime = 0;
						ps->viewHeightLerpPosAdj = 0.0;
					}
					else
					{
						if ( ps->viewHeightLerpTarget == 11 )
						{
							ps->viewHeightCurrent = PM_ViewHeightTableLerp(iFrac, viewLerp_CrouchProne, &pfPosOfs);
						}
						else if ( ps->viewHeightLerpTarget == 40 )
						{
							if ( ps->viewHeightLerpDown )
								ps->viewHeightCurrent = PM_ViewHeightTableLerp(iFrac, viewLerp_StandCrouch, &pfPosOfs);
							else
								ps->viewHeightCurrent = PM_ViewHeightTableLerp(iFrac, viewLerp_ProneCrouch, &pfPosOfs);
						}
						else
						{
							ps->viewHeightCurrent = PM_ViewHeightTableLerp(iFrac, viewLerp_CrouchStand, &pfPosOfs);
						}

						height = ps->viewHeightLerpPosAdj - pfPosOfs;

						if ( I_fabs(height) > 0.050000001 )
						{
							VectorCopy(ps->velocity, velocity);
							adjust = pfPosOfs - ps->viewHeightLerpPosAdj;

							if ( ps->groundEntityNum == 1023 )
								adjust = adjust * 0.5;

							adjust = adjust / pml->frametime;

							direction[0] = pml->forward[0];
							direction[1] = pml->forward[1];
							direction[2] = 0.0;

							Vec3Normalize(direction);
							VectorScale(direction, adjust, ps->velocity);

							PM_StepSlideMove(pm, pml, 1);

							VectorCopy(velocity, ps->velocity);
							ps->viewHeightLerpPosAdj = pfPosOfs;
						}
					}
				}
				if ( ps->viewHeightLerpTime )
				{
					if ( ps->viewHeightTarget != ps->viewHeightLerpTarget
					        && (ps->viewHeightTarget < ps->viewHeightLerpTarget && !ps->viewHeightLerpDown
					            || ps->viewHeightTarget > ps->viewHeightLerpTarget && ps->viewHeightLerpDown) )
					{
						iFrac = 100 - iFrac;
						ps->viewHeightLerpDown ^= 1u;

						if ( ps->viewHeightLerpDown )
						{
							if ( ps->viewHeightLerpTarget == 60 )
							{
								ps->viewHeightLerpTarget = 40;
							}
							else if ( ps->viewHeightLerpTarget == 40 )
							{
								ps->viewHeightLerpTarget = 11;
							}
						}
						else if ( ps->viewHeightLerpTarget == 11 )
						{
							ps->viewHeightLerpTarget = 40;
						}
						else if ( ps->viewHeightLerpTarget == 40 )
						{
							ps->viewHeightLerpTarget = 60;
						}

						if ( iFrac == 100 )
						{
							ps->viewHeightCurrent = (float)ps->viewHeightLerpTarget;
							ps->viewHeightLerpTime = 0;
							ps->viewHeightLerpPosAdj = 0.0;
						}
						else
						{
							time = PM_GetViewHeightLerpTime(ps, ps->viewHeightLerpTarget, ps->viewHeightLerpDown);
							ps->viewHeightLerpTime = pm->cmd.serverTime - (int)((float)iFrac * 0.0099999998 * (float)time);

							if ( ps->viewHeightLerpTarget == 11 )
							{
								PM_ViewHeightTableLerp(iFrac, viewLerp_CrouchProne, &pfPosOfs);
							}
							else if ( ps->viewHeightLerpTarget == 40 )
							{
								if ( ps->viewHeightLerpDown )
									PM_ViewHeightTableLerp(iFrac, viewLerp_StandCrouch, &pfPosOfs);
								else
									PM_ViewHeightTableLerp(iFrac, viewLerp_ProneCrouch, &pfPosOfs);
							}
							else
							{
								PM_ViewHeightTableLerp(iFrac, viewLerp_CrouchStand, &pfPosOfs);
							}

							ps->viewHeightLerpPosAdj = pfPosOfs;
						}
					}
				}
				else if ( ps->viewHeightCurrent != (float)ps->viewHeightTarget )
				{
					ps->viewHeightLerpTime = pm->cmd.serverTime;

					switch ( ps->viewHeightTarget )
					{
					case 0xB:
						ps->viewHeightLerpDown = 1;
						if ( ps->viewHeightCurrent <= 40.0 )
							ps->viewHeightLerpTarget = 11;
						else
							ps->viewHeightLerpTarget = 40;
						break;

					case 0x28:
						ps->viewHeightLerpDown = ps->viewHeightCurrent > (float)ps->viewHeightTarget;
						ps->viewHeightLerpTarget = 40;
						break;

					case 0x3C:
						ps->viewHeightLerpDown = 0;
						if ( ps->viewHeightCurrent >= 40.0 )
							ps->viewHeightLerpTarget = 60;
						else
							ps->viewHeightLerpTarget = 40;
						break;
					}
				}
			}
			else
			{
				ps->viewHeightLerpTime = 0;

				if ( (float)ps->viewHeightTarget <= ps->viewHeightCurrent )
				{
					ps->viewHeightCurrent = ps->viewHeightCurrent - pml->frametime * 180.0;

					if ( (float)ps->viewHeightTarget >= ps->viewHeightCurrent )
						ps->viewHeightCurrent = (float)ps->viewHeightTarget;
				}
				else
				{
					ps->viewHeightCurrent = pml->frametime * 180.0 + ps->viewHeightCurrent;

					if ( ps->viewHeightCurrent >= (float)ps->viewHeightTarget )
						ps->viewHeightCurrent = (float)ps->viewHeightTarget;
				}
			}
		}
	}
	else if ( ps->pm_type == PM_SPECTATOR )
	{
		ps->viewHeightCurrent = 0.0;
	}
	else
	{
		ps->viewHeightCurrent = (float)ps->viewHeightTarget;
	}
}

void PM_CheckDuck(pmove_t *pm, pml_t *pml)
{
	int flags;
	vec3_t vEnd;
	float delta;
	vec3_t vPoint;
	playerState_s *ps;
	int stance;
	trace_t trace;
	int bWasStanding;
	int bWasProne;

	ps = pm->ps;
	pm->proneChange = 0;

	if ( ps->pm_type == PM_SPECTATOR )
	{
		pm->mins[0] = -8.0;
		pm->mins[1] = -8.0;
		pm->mins[2] = -8.0;
		pm->maxs[0] = 8.0;
		pm->maxs[1] = 8.0;
		pm->maxs[2] = 16.0;
		ps->pm_flags &= 0xFFFFFFFC;

		if ( (pm->cmd.buttons & 0x100) != 0 )
		{
			pm->cmd.buttons &= ~0x100u;
			BG_AddPredictableEventToPlayerstate(EV_STANCE_FORCE_STAND, 0, ps);
		}

		ps->viewHeightTarget = 0;
		ps->viewHeightCurrent = 0.0;
	}
	else
	{
		bWasProne = (ps->pm_flags & 1) != 0;
		bWasStanding = (ps->pm_flags & 3) == 0;

		pm->mins[0] = ps->mins[0];
		pm->mins[1] = ps->mins[1];
		pm->maxs[0] = ps->maxs[0];
		pm->maxs[1] = ps->maxs[1];
		pm->mins[2] = ps->mins[2];

		if ( ps->pm_type <= PM_INTERMISSION )
		{
			if ( (ps->eFlags & 0x300) != 0 )
			{
				if ( (ps->eFlags & 0x100) == 0 || (ps->eFlags & 0x200) != 0 )
				{
					if ( (ps->eFlags & 0x200) == 0 || (ps->eFlags & 0x100) != 0 )
					{
						ps->pm_flags &= 0xFFFFFFFC;
					}
					else
					{
						ps->pm_flags |= 2u;
						ps->pm_flags &= ~1u;
					}
				}
				else
				{
					ps->pm_flags |= 1u;
					ps->pm_flags &= ~2u;
				}
			}
			else if ( SLOWORD(ps->pm_flags) >= 0 )
			{
				if ( (ps->pm_flags & 0x20) != 0 && (pm->cmd.buttons & 0x300) != 0 )
				{
					pm->cmd.buttons &= 0xFFFFFCFF;
					BG_AddPredictableEventToPlayerstate(EV_STANCE_FORCE_STAND, 0, ps);
				}
				if ( (pm->cmd.buttons & 0x100) != 0 )
				{
					if ( (ps->pm_flags & 1) != 0
					        || ps->groundEntityNum != 1023
					        && BG_CheckProne(
					            ps->clientNum,
					            ps->origin,
					            pm->maxs[0],
					            30.0,
					            ps->viewangles[1],
					            &ps->fTorsoHeight,
					            &ps->fTorsoPitch,
					            &ps->fWaistPitch,
					            0,
					            ps->groundEntityNum != 1023,
					            0,
					            pm->handler,
					            0,
					            66.0) )
					{
						ps->pm_flags |= 1u;
						ps->pm_flags &= ~2u;
					}
					else if ( ps->groundEntityNum != 1023 )
					{
						ps->pm_flags |= 0x10000u;

						if ( (pm->cmd.buttons & 0x2000) == 0 )
						{
							if ( (ps->pm_flags & 2) != 0 )
								BG_AddPredictableEventToPlayerstate(EV_STANCE_FORCE_CROUCH, 0, ps);
							else
								BG_AddPredictableEventToPlayerstate(EV_STANCE_FORCE_STAND, 0, ps);
						}
					}
				}
				else if ( (pm->cmd.buttons & 0x200) != 0 )
				{
					if ( (ps->pm_flags & 1) != 0 )
					{
						pm->maxs[2] = 50.0;
						pmoveHandlers[pm->handler].trace(
						    &trace,
						    ps->origin,
						    pm->mins,
						    pm->maxs,
						    ps->origin,
						    ps->clientNum,
						    pm->tracemask & 0xFDFFFFFF);
						if ( trace.allsolid )
						{
							if ( (pm->cmd.buttons & 0x2000) == 0 )
								BG_AddPredictableEventToPlayerstate(EV_STANCE_FORCE_PRONE, 2u, ps);
						}
						else
						{
							BG_AnimScriptEvent(ps, ANIM_ET_PRONE_TO_CROUCH, 0, 0);
							ps->pm_flags &= ~1u;
							ps->pm_flags |= 2u;
						}
					}
					else
					{
						BG_AnimScriptEvent(ps, ANIM_ET_STAND_TO_CROUCH, 0, 0);
						ps->pm_flags |= 2u;
					}
				}
				else if ( (ps->pm_flags & 1) != 0 )
				{
					pm->maxs[2] = ps->maxs[2];
					pmoveHandlers[pm->handler].trace(
					    &trace,
					    ps->origin,
					    pm->mins,
					    pm->maxs,
					    ps->origin,
					    ps->clientNum,
					    pm->tracemask & 0xFDFFFFFF);
					if ( trace.allsolid )
					{
						pm->maxs[2] = 50.0;
						pmoveHandlers[pm->handler].trace(
						    &trace,
						    ps->origin,
						    pm->mins,
						    pm->maxs,
						    ps->origin,
						    ps->clientNum,
						    pm->tracemask & 0xFDFFFFFF);
						if ( trace.allsolid )
						{
							if ( (pm->cmd.buttons & 0x2000) == 0 )
								BG_AddPredictableEventToPlayerstate(EV_STANCE_FORCE_PRONE, 1u, ps);
						}
						else
						{
							ps->pm_flags &= ~1u;
							ps->pm_flags |= 2u;
						}
					}
					else
					{
						BG_AnimScriptEvent(ps, ANIM_ET_PRONE_TO_STAND, 0, 0);
						ps->pm_flags &= 0xFFFFFFFC;
					}
				}
				else if ( (ps->pm_flags & 2) != 0 )
				{
					pm->maxs[2] = ps->maxs[2];
					pmoveHandlers[pm->handler].trace(
					    &trace,
					    ps->origin,
					    pm->mins,
					    pm->maxs,
					    ps->origin,
					    ps->clientNum,
					    pm->tracemask & 0xFDFFFFFF);
					if ( trace.allsolid )
					{
						if ( (pm->cmd.buttons & 0x2000) == 0 )
							BG_AddPredictableEventToPlayerstate(EV_STANCE_FORCE_CROUCH, 1u, ps);
					}
					else
					{
						BG_AnimScriptEvent(ps, ANIM_ET_CROUCH_TO_STAND, 0, 0);
						ps->pm_flags &= ~2u;
					}
				}
			}
			if ( !ps->viewHeightLerpTime )
			{
				if ( (ps->pm_flags & 1) != 0 )
				{
					if ( ps->viewHeightTarget == 60 )
					{
						ps->viewHeightTarget = 40;
					}
					else if ( ps->viewHeightTarget != 11 )
					{
						ps->viewHeightTarget = 11;
						pm->proneChange = 1;
						BG_PlayAnim(ps, 0, ANIM_BP_TORSO, 0, 0, 1, 1);
						Jump_ActivateSlowdown(ps);
					}
				}
				else if ( ps->viewHeightTarget == 11 )
				{
					ps->viewHeightTarget = 40;
					pm->proneChange = 1;
					BG_PlayAnim(ps, 0, ANIM_BP_TORSO, 0, 0, 1, 1);
				}
				else if ( (ps->pm_flags & 2) != 0 )
				{
					ps->viewHeightTarget = 40;
				}
				else
				{
					ps->viewHeightTarget = 60;
				}
			}

			PM_ViewHeightAdjust(pm, pml);
			stance = PM_GetEffectiveStance(ps);

			if ( stance == 1 )
			{
				pm->maxs[2] = 30.0;
				ps->eFlags |= 8u;
				ps->eFlags &= ~4u;
				ps->pm_flags |= 1u;
				ps->pm_flags &= ~2u;
			}
			else
			{
				if ( stance == 2 )
				{
					pm->maxs[2] = 50.0;
					ps->eFlags |= 4u;
					ps->eFlags &= ~8u;
					ps->pm_flags |= 2u;
					flags = ps->pm_flags & 0xFFFFFFFE;
				}
				else
				{
					pm->maxs[2] = ps->maxs[2];
					ps->eFlags &= 0xFFFFFFF3;
					flags = ps->pm_flags & 0xFFFFFFFC;
				}

				ps->pm_flags = flags;
			}

			if ( (ps->pm_flags & 1) != 0 && !bWasProne )
			{
				if ( pm->cmd.forwardmove || pm->cmd.rightmove )
				{
					ps->pm_flags &= ~0x800u;
					PM_ExitAimDownSight(ps);
				}
				VectorCopy(ps->origin, vEnd);
				vEnd[2] = vEnd[2] + 10.0;
				pmoveHandlers[pm->handler].trace(
				    &trace,
				    ps->origin,
				    pm->mins,
				    pm->maxs,
				    vEnd,
				    ps->clientNum,
				    pm->tracemask & 0xFDFFFFFF);
				Vec3Lerp(ps->origin, vEnd, trace.fraction, vEnd);
				pmoveHandlers[pm->handler].trace(
				    &trace,
				    vEnd,
				    pm->mins,
				    pm->maxs,
				    ps->origin,
				    ps->clientNum,
				    pm->tracemask & 0xFDFFFFFF);
				Vec3Lerp(vEnd, ps->origin, trace.fraction, ps->origin);
				ps->proneDirection = ps->viewangles[1];
				VectorCopy(ps->origin, vPoint);
				vPoint[2] = vPoint[2] - 0.25;
				pmoveHandlers[pm->handler].trace(
				    &trace,
				    ps->origin,
				    pm->mins,
				    pm->maxs,
				    vPoint,
				    ps->clientNum,
				    pm->tracemask & 0xFDFFFFFF);
				if ( trace.startsolid || trace.fraction >= 1.0 )
				{
					ps->proneDirectionPitch = 0.0;
				}
				else
				{
					ps->proneDirectionPitch = PitchForYawOnNormal(ps->proneDirection, trace.normal);
				}

				delta = AngleDelta(ps->proneDirectionPitch, ps->viewangles[0]);

				if ( delta >= -45.0 )
				{
					if ( delta <= 45.0 )
						ps->proneTorsoPitch = ps->proneDirectionPitch;
					else
						ps->proneTorsoPitch = ps->viewangles[0] + 45.0;
				}
				else
				{
					ps->proneTorsoPitch = ps->viewangles[0] - 45.0;
				}
			}
		}
		else
		{
			pm->maxs[2] = ps->maxs[2];
			ps->viewHeightTarget = 8;
			PM_ViewHeightAdjust(pm, pml);
		}
	}
}












#ifdef LIBCOD
void PM_ProjectVelocity(const float *velIn, const float *normal, float *velOut)
{
	float lengthSq2D;
	float adjusted;
	float newZ;
	float lengthScale;

	if (!jump_bounceEnable->current.boolean)
	{
		PM_ClipVelocity(velIn, normal, velOut);
		return;
	}

	lengthSq2D = (float)(velIn[0] * velIn[0]) + (float)(velIn[1] * velIn[1]);

	if ( I_fabs(normal[2]) < 0.001 || lengthSq2D == 0.0 )
	{
		velOut[0] = velIn[0];
		velOut[1] = velIn[1];
		velOut[2] = velIn[2];
	}
	else
	{
		newZ = (float)-(float)((float)(velIn[0] * normal[0]) + (float)(velIn[1] * normal[1])) / normal[2];
		adjusted = velIn[1];
		lengthScale = I_sqrt((float)((float)(velIn[2] * velIn[2]) + lengthSq2D) / (float)((float)(newZ * newZ) + lengthSq2D));

		if ( lengthScale < 1.0 || newZ < 0.0 || velIn[2] > 0.0 )
		{
			velOut[0] = lengthScale * velIn[0];
			velOut[1] = lengthScale * adjusted;
			velOut[2] = lengthScale * newZ;
		}
	}
}
#endif

void PM_AirMove(pmove_t *pm, pml_t *pml)
{
	playerState_s *ps;
	usercmd_s cmd;
	float scale;
	float wishspeed;
	vec3_t wishdir;
	float rightmove;
	float forwardmove;
	vec3_t wishvel;
	int i;

	ps = pm->ps;
	PM_Friction(pm->ps, pml);
	forwardmove = (float)pm->cmd.forwardmove;
	rightmove = (float)pm->cmd.rightmove;
	cmd = pm->cmd;
	scale = PM_CmdScale(ps, &cmd);
	pml->forward[2] = 0.0;
	pml->right[2] = 0.0;
	Vec3Normalize(pml->forward);
	Vec3Normalize(pml->right);

	for ( i = 0; i < 2; ++i )
		wishvel[i] = pml->forward[i] * forwardmove + pml->right[i] * rightmove;

	wishvel[2] = 0.0;
	VectorCopy(wishvel, wishdir);
	wishspeed = Vec3Normalize(wishdir);
	wishspeed = wishspeed * scale;
	PM_Accelerate(ps, pml, wishdir, wishspeed, 1.0);

	if ( pml->groundPlane )
		PM_ClipVelocity(ps->velocity, pml->groundTrace.normal, ps->velocity);

	PM_StepSlideMove(pm, pml, 1);
	PM_SetMovementDir(pm, pml);
}



float PM_CmdScale_Walk(pmove_t *pm, usercmd_s *cmd)
{
	float totalForward;
	float forward;
	float totalRight;
	int stance;
	float scale;
	float scale2;
	playerState_s *ps;
	float forwardSquared;
	float totalSpeed;
	float rightSquared;
	float absTotal;
	float maxTotal;

	ps = pm->ps;
	totalForward = (float)(cmd->rightmove * cmd->rightmove + cmd->forwardmove * cmd->forwardmove);
	forwardSquared = I_sqrt(totalForward);

	if ( cmd->forwardmove >= 0 )
		forward = (float)cmd->forwardmove;
	else
		forward = (float)cmd->forwardmove * player_backSpeedScale->current.decimal;

	absTotal = I_fabs(forward);
	totalRight = (float)cmd->rightmove * player_strafeSpeedScale->current.decimal;
	rightSquared = I_fabs(totalRight);
	maxTotal = I_fmax(absTotal, rightSquared);

	if ( maxTotal == 0.0 )
	{
		return 0.0;
	}
	else
	{
		totalSpeed = (float)ps->speed * maxTotal / (forwardSquared * 127.0);

		if ( (ps->pm_flags & 0x100) != 0 || ps->leanf != 0.0 )
			totalSpeed = totalSpeed * 0.40000001;

		if ( ps->pm_type == PM_NOCLIP )
		{
			totalSpeed = totalSpeed * 3.0;
		}
		else if ( ps->pm_type == PM_UFO )
		{
			totalSpeed = totalSpeed * 6.0;
		}
		else
		{
			stance = PM_GetEffectiveStance(ps);
			scale = PM_GetViewHeightLerp(pm, 40, 11);

			if ( scale == 0.0 )
			{
				scale2 = PM_GetViewHeightLerp(pm, 11, 40);

				if ( scale2 == 0.0 )
				{
					if ( stance == 1 )
					{
						totalSpeed = totalSpeed * 0.15000001;
					}
					else if ( stance == 2 )
					{
						totalSpeed = totalSpeed * 0.64999998;
					}
				}
				else
				{
					totalSpeed = (scale2 * 0.64999998 + (1.0 - scale2) * 0.15000001) * totalSpeed;
				}
			}
			else
			{
				totalSpeed = (scale * 0.15000001 + (1.0 - scale) * 0.64999998) * totalSpeed;
			}
		}
		if ( ps->weapon && BG_GetWeaponDef(ps->weapon)->moveSpeedScale > 0.0 )
			totalSpeed = totalSpeed * BG_GetWeaponDef(ps->weapon)->moveSpeedScale;

		if ( (cmd->buttons & 0x800) != 0 )
			return totalSpeed * 0.40000001;

		return totalSpeed;
	}
}

void PM_WalkMove(pmove_t *pm, pml_t *pml)
{
	float dmgscale;
	playerState_s *ps;
	int stance;
	float len;
	float accel;
	usercmd_s cmd;
	float cmdscale;
	float wishspeed;
	vec3_t wishdir;
	float rightmove;
	float forwardmove;
	vec3_t wishvel;
	vec3_t dir;
	int i;

	ps = pm->ps;

	if ( (pm->ps->pm_flags & 0x80000) != 0 )
		Jump_ApplySlowdown(ps);

	if ( Jump_Check(pm, pml) )
	{
		PM_AirMove(pm, pml);
	}
	else
	{
		PM_Friction(ps, pml);
		forwardmove = (float)pm->cmd.forwardmove;
		rightmove = (float)pm->cmd.rightmove;
		cmd = pm->cmd;
		cmdscale = PM_CmdScale_Walk(pm, &cmd);
		dmgscale = PM_DamageScale_Walk(ps->damageTimer);
		cmdscale = dmgscale * cmdscale;
		ps->damageTimer -= (int)(pml->frametime * 1000.0);

		if ( ps->damageTimer <= 0 )
			ps->damageTimer = 0;

		pml->forward[2] = 0.0;
		pml->right[2] = 0.0;

		PM_ClipVelocity(pml->forward, pml->groundTrace.normal, pml->forward);
		PM_ClipVelocity(pml->right, pml->groundTrace.normal, pml->right);

		Vec3Normalize(pml->forward);
		Vec3Normalize(pml->right);

		for ( i = 0; i < 3; ++i )
			dir[i] = pml->forward[i] * forwardmove + pml->right[i] * rightmove;

		VectorCopy(dir, wishdir);
		wishspeed = Vec3Normalize(wishdir);
		wishspeed = wishspeed * cmdscale;
		stance = PM_GetEffectiveStance(ps);

		if ( (pml->groundTrace.surfaceFlags & 2) != 0 || (ps->pm_flags & 0x400) != 0 )
		{
			accel = 1.0;
		}
		else if ( stance == 1 )
		{
			accel = 19.0;
		}
		else if ( stance == 2 )
		{
			accel = 12.0;
		}
		else
		{
			accel = 9.0;
		}

		if ( (ps->pm_flags & 0x200) != 0 )
			accel = accel * 0.25;

		PM_Accelerate(ps, pml, wishdir, wishspeed, accel);

		if ( (pml->groundTrace.surfaceFlags & 2) != 0 || (ps->pm_flags & 0x400) != 0 )
			ps->velocity[2] = ps->velocity[2] - (float)ps->gravity * pml->frametime;

		len = VectorLength(ps->velocity);
		VectorCopy(ps->velocity, wishvel);
		PM_ClipVelocity(ps->velocity, pml->groundTrace.normal, ps->velocity);

		if ( DotProduct(ps->velocity, wishvel) > 0.0 )
		{
			Vec3Normalize(ps->velocity);
			VectorScale(ps->velocity, len, ps->velocity);
		}

		if ( ps->velocity[0] != 0.0 || ps->velocity[1] != 0.0 )
			PM_StepSlideMove(pm, pml, 0);

		PM_SetMovementDir(pm, pml);
	}
}

void PM_NoclipMove(pmove_t *pm, pml_t *pml)
{
	float value;
	playerState_s *ps;
	float scale;
	float wishspeed;
	float speed;
	vec3_t wishdir;
	float umove;
	float rmove;
	float fmove;
	vec3_t wishvel;
	int i;
	float newspeed;
	float control;
	float curFriction;
	float drop;
	float curSpeed;

	ps = pm->ps;
	ps->viewHeightTarget = 60;
	curSpeed = VectorLength(ps->velocity);

	if ( curSpeed >= 1.0 )
	{
		drop = 0.0;
		curFriction = friction->current.decimal * 1.5;

		if ( stopspeed->current.decimal <= curSpeed )
			value = curSpeed;
		else
			value = stopspeed->current.decimal;

		control = value;
		drop = value * curFriction * pml->frametime + drop;
		newspeed = curSpeed - drop;

		if ( newspeed < 0.0 )
			newspeed = 0.0;

		newspeed = newspeed / curSpeed;
		VectorScale(ps->velocity, newspeed, ps->velocity);
	}
	else
	{
		VectorCopy(vec3_origin, ps->velocity);
	}

	fmove = (float)pm->cmd.forwardmove;
	rmove = (float)pm->cmd.rightmove;
	umove = 0.0;

	if ( SLOBYTE(pm->cmd.buttons) < 0 )
		umove = umove + 127.0;

	if ( (pm->cmd.buttons & 0x40) != 0 )
		umove = umove - 127.0;

	scale = PM_MoveScale(ps, fmove, rmove, umove);

	for ( i = 0; i < 3; ++i )
		wishvel[i] = pml->forward[i] * fmove + pml->right[i] * rmove + pml->up[i] * umove;

	VectorCopy(wishvel, wishdir);
	wishspeed = Vec3Normalize(wishdir);
	speed = wishspeed * scale;
	PM_Accelerate(ps, pml, wishdir, speed, 9.0);
	VectorMA(ps->origin, pml->frametime, ps->velocity, ps->origin);
}

void PM_UFOMove(pmove_t *pm, pml_t *pml)
{
	float value;
	vec3_t right;
	vec3_t up;
	int i;
	playerState_s *ps;
	float scale;
	float wishspeed;
	vec3_t wishdir;
	float umove;
	float rmove;
	float fmove;
	vec3_t wishvel;
	float newspeed;
	float control;
	float curFriction;
	float drop;
	float speed;

	ps = pm->ps;
	ps->viewHeightTarget = 60;

	fmove = (float)pm->cmd.forwardmove;
	rmove = (float)pm->cmd.rightmove;
	umove = 0.0;

	if ( SLOBYTE(pm->cmd.buttons) < 0 )
		umove = umove + 127.0;

	if ( (pm->cmd.buttons & 0x40) != 0 )
		umove = umove - 127.0;

	if ( fmove == 0.0 && rmove == 0.0 && umove == 0.0 )
		speed = 0.0;
	else
		speed = VectorLength(ps->velocity);

	if ( speed >= 1.0 )
	{
		drop = 0.0;
		curFriction = friction->current.decimal * 1.5;

		if ( stopspeed->current.decimal <= speed )
			value = speed;
		else
			value = stopspeed->current.decimal;

		control = value;
		drop = value * curFriction * pml->frametime + drop;
		newspeed = speed - drop;

		if ( newspeed < 0.0 )
			newspeed = 0.0;

		newspeed = newspeed / speed;
		VectorScale(ps->velocity, newspeed, ps->velocity);
	}
	else
	{
		VectorCopy(vec3_origin, ps->velocity);
	}

	scale = PM_MoveScale(ps, fmove, rmove, umove);

	up[1] = 0.0;
	up[0] = 0.0;
	up[2] = 1.0;

	Vec3Cross(up, pml->right, right);

	for ( i = 0; i < 3; ++i )
		wishvel[i] = right[i] * fmove + pml->right[i] * rmove + up[i] * umove;

	VectorCopy(wishvel, wishdir);
	wishspeed = Vec3Normalize(wishdir);
	wishspeed = wishspeed * scale;
	PM_Accelerate(ps, pml, wishdir, wishspeed, 9.0);
	VectorMA(ps->origin, pml->frametime, ps->velocity, ps->origin);
}

void PM_GroundTraceMissed(pmove_t *pm, pml_t *pml)
{
	playerState_s *ps;
	vec3_t point;
	trace_t trace;

	ps = pm->ps;

	if ( pm->ps->groundEntityNum == 1023 )
	{
		VectorCopy(ps->origin, point);
		point[2] = point[2] - 1.0;
		pmoveHandlers[pm->handler].trace(&trace, ps->origin, pm->mins, pm->maxs, point, ps->clientNum, pm->tracemask);
		pml->almostGroundPlane = 1.0 != trace.fraction;
	}
	else
	{
		VectorCopy(ps->origin, point);
		point[2] = point[2] - 64.0;
		pmoveHandlers[pm->handler].trace(&trace, ps->origin, pm->mins, pm->maxs, point, ps->clientNum, pm->tracemask);

		if ( trace.fraction == 1.0 )
		{
			if ( pm->cmd.forwardmove < 0 )
				BG_AnimScriptEvent(ps, ANIM_ET_JUMPBK, 0, 1);
			else
				BG_AnimScriptEvent(ps, ANIM_ET_JUMP, 0, 1);

			pml->almostGroundPlane = 0;
		}
		else
		{
			pml->almostGroundPlane = trace.fraction < 0.015625;
		}
	}

	ps->groundEntityNum = 1023;
	pml->groundPlane = 0;
	pml->walking = 0;
}

void PM_GroundTrace(pmove_t *pm, pml_t *pml)
{
	float height;
	playerState_s *ps;
	trace_t trace;
	vec3_t end;
	vec3_t start;

	ps = pm->ps;

	start[0] = pm->ps->origin[0];
	start[1] = ps->origin[1];

	end[0] = ps->origin[0];
	end[1] = ps->origin[1];

	if ( (ps->eFlags & 0x300) != 0 )
	{
		start[2] = ps->origin[2];
		height = ps->origin[2] - 1.0;
	}
	else
	{
		start[2] = ps->origin[2] + 0.25;
		height = ps->origin[2] - 0.25;
	}

	end[2] = height;
	PM_playerTrace(pm, &trace, start, pm->mins, pm->maxs, end, ps->clientNum, pm->tracemask);
	pml->groundTrace = trace;

	if ( !trace.allsolid || PM_CorrectAllSolid(pm, pml, &trace) )
	{
		if ( trace.startsolid )
		{
			start[2] = ps->origin[2] - 0.001;
			PM_playerTrace(pm, &trace, start, pm->mins, pm->maxs, end, ps->clientNum, pm->tracemask);

			if ( trace.startsolid )
			{
				ps->groundEntityNum = 1023;
				pml->groundPlane = 0;
				pml->almostGroundPlane = 0;
				pml->walking = 0;
				return;
			}

			pml->groundTrace = trace;
		}

		if ( trace.fraction == 1.0 )
		{
			PM_GroundTraceMissed(pm, pml);
		}
		else if ( (ps->pm_flags & 0x20) != 0
		          || ps->velocity[2] <= 0.0
		          || DotProduct(ps->velocity, trace.normal) <= 10.0 )
		{
			if ( trace.normal[2] >= 0.69999999 )
			{
				pml->groundPlane = 1;
				pml->almostGroundPlane = 1;
				pml->walking = 1;

				if ( ps->groundEntityNum == 1023 )
					PM_CrashLand(ps, pml);

				ps->groundEntityNum = trace.entityNum;
				PM_AddTouchEnt(pm, trace.entityNum);
			}
			else
			{
				ps->groundEntityNum = 1023;
				pml->groundPlane = 1;
				pml->almostGroundPlane = 1;
				pml->walking = 0;
				Jump_ClearState(ps);
			}
		}
		else
		{
			if ( pm->cmd.forwardmove < 0 )
				BG_AnimScriptEvent(ps, ANIM_ET_JUMPBK, 0, 0);
			else
				BG_AnimScriptEvent(ps, ANIM_ET_JUMP, 0, 0);

			pml->almostGroundPlane = 0;
			ps->groundEntityNum = 1023;
			pml->groundPlane = 0;
			pml->walking = 0;
		}
	}
}









void PmoveSingle(pmove_t *pmove)
{
	int flags;
	float length;
	float forwardmove;
	float oldFmove;
	float rightmove;
	float oldRmove;
	float msec;
	float frametime;
	float newFrame;
	float absRmove;
	float absFmove;
	float lengthSquared;
	float nextFrameTime;
	vec3_t velocity;
	pml_t pml;
	playerState_s *ps;
	vec3_t newVelocity;
	int stance;

	ps = pmove->ps;
	BG_AnimUpdatePlayerStateConditions(pmove);

	if ( SLOWORD(ps->pm_flags) < 0 )
	{
		pmove->cmd.buttons &= 0x2300u;
		pmove->cmd.forwardmove = 0;
		pmove->cmd.rightmove = 0;
		VectorSet(ps->velocity, 0.0, 0.0, 0.0);
	}

	if ( (pmove->cmd.buttons & 0x40000) != 0 )
	{
		pmove->cmd.buttons &= 0x43300u;
		pmove->cmd.forwardmove = 0;
		pmove->cmd.rightmove = 0;
	}

	ps->pm_flags &= ~0x10000u;

	if ( ps->pm_type > PM_INTERMISSION )
		pmove->tracemask &= ~0x2000000u;

	if ( (ps->pm_flags & 1) != 0 )
	{
		if ( (pmove->cmd.forwardmove == pmove->oldcmd.forwardmove
		        || (forwardmove = (float)pmove->cmd.forwardmove,
		            absFmove = I_fabs(forwardmove),
		            oldFmove = (float)pmove->oldcmd.forwardmove,
		            absFmove <= I_fabs(oldFmove)))
		        && (pmove->cmd.rightmove == pmove->oldcmd.rightmove
		            || (rightmove = (float)pmove->cmd.rightmove,
		                absRmove = I_fabs(rightmove),
		                oldRmove = (float)pmove->oldcmd.rightmove,
		                absRmove <= I_fabs(oldRmove))) )
		{
			if ( (ps->pm_flags & 0x40) == 0
			        && (ps->weaponstate < (unsigned int)WEAPON_FIRING || ps->weaponstate == WEAPON_RELOADING) )
			{
				ps->pm_flags &= ~0x800u;
			}
		}
		else if ( PM_InteruptWeaponWithProneMove(ps) )
		{
			ps->pm_flags &= ~0x800u;
			PM_ExitAimDownSight(ps);
		}
	}
	else
	{
		ps->pm_flags &= ~0x800u;
	}

	stance = PM_GetEffectiveStance(ps);

	if ( (ps->pm_flags & 0x40) != 0 && stance == 1 )
	{
		pmove->cmd.forwardmove = 0;
		pmove->cmd.rightmove = 0;
	}

	if ( (pmove->cmd.buttons & 0x40000) != 0 )
		flags = ps->eFlags | 0x200000;
	else
		flags = ps->eFlags & 0xFFDFFFFF;

	ps->eFlags = flags;
	ps->eFlags &= ~0x40u;

	if ( ps->pm_type != PM_INTERMISSION
	        && (ps->pm_flags & 0x1000) == 0
	        && (ps->weaponstate == WEAPON_READY || ps->weaponstate == WEAPON_FIRING)
	        && PM_WeaponAmmoAvailable(ps)
	        && (pmove->cmd.buttons & 1) != 0 )
	{
		ps->eFlags |= 0x40u;
	}

	if ( ps->pm_type <= PM_INTERMISSION && (pmove->cmd.buttons & 0x4001) == 0 )
		ps->pm_flags &= ~0x1000u;

	memset(&pml, 0, sizeof(pml));
	pml.msec = pmove->cmd.serverTime - ps->commandTime;

	if ( pml.msec > 0 )
	{
		if ( pml.msec > 200 )
			pml.msec = 200;
	}
	else
	{
		pml.msec = 1;
	}

	ps->commandTime = pmove->cmd.serverTime;
	VectorCopy(ps->origin, pml.previous_origin);
	VectorCopy(ps->velocity, pml.previous_velocity);
	pml.frametime = (float)pml.msec * 0.001;
	PM_AdjustAimSpreadScale(pmove, &pml);
	msec = (float)pml.msec;
	PM_UpdateViewAngles(ps, msec, &pmove->cmd, pmove->handler);
	AngleVectors(ps->viewangles, pml.forward, pml.right, pml.up);

	if ( pmove->cmd.forwardmove >= 0 )
	{
		if ( pmove->cmd.forwardmove > 0 || !pmove->cmd.forwardmove && pmove->cmd.rightmove )
			ps->pm_flags &= ~0x80u;
	}
	else
	{
		ps->pm_flags |= 0x80u;
	}
	if ( ps->pm_type > PM_INTERMISSION )
	{
		pmove->cmd.forwardmove = 0;
		pmove->cmd.rightmove = 0;
	}
	if ( stance == 1 && (ps->pm_flags & 0x800) != 0 )
	{
		pmove->cmd.forwardmove = 0;
		pmove->cmd.rightmove = 0;
	}
	Mantle_ClearHint(ps);
	switch ( ps->pm_type )
	{
	case PM_NORMAL_LINKED:
	case PM_DEAD_LINKED:
		PM_ClearLadderFlag(ps);
		ps->groundEntityNum = 1023;
		memset(&pml.walking, 0, 12);
		VectorClear(ps->velocity);
		PM_UpdateAimDownSightFlag(pmove, &pml);
		PM_UpdatePlayerWalkingFlag(pmove);
		PM_CheckDuck(pmove, &pml);
		PM_DropTimers(ps, &pml);
		PM_Footsteps(pmove, &pml);
		PM_Weapon(pmove, &pml);
		break;
	case PM_NOCLIP:
		PM_ClearLadderFlag(ps);
		PM_UpdateAimDownSightFlag(pmove, &pml);
		PM_UpdatePlayerWalkingFlag(pmove);
		PM_NoclipMove(pmove, &pml);
		PM_DropTimers(ps, &pml);
		PM_UpdateAimDownSightLerp(pmove, &pml);
		break;
	case PM_UFO:
		PM_ClearLadderFlag(ps);
		PM_UpdateAimDownSightFlag(pmove, &pml);
		PM_UpdatePlayerWalkingFlag(pmove);
		PM_UFOMove(pmove, &pml);
		PM_DropTimers(ps, &pml);
		PM_UpdateAimDownSightLerp(pmove, &pml);
		break;
	case PM_SPECTATOR:
		PM_ClearLadderFlag(ps);
		PM_UpdateAimDownSightFlag(pmove, &pml);
		PM_UpdatePlayerWalkingFlag(pmove);
		PM_CheckDuck(pmove, &pml);
		PM_FlyMove(pmove, &pml);
		PM_DropTimers(ps, &pml);
		PM_UpdateAimDownSightLerp(pmove, &pml);
		break;
	case PM_INTERMISSION:
		PM_ClearLadderFlag(ps);
		PM_UpdateAimDownSightFlag(pmove, &pml);
		PM_UpdateAimDownSightLerp(pmove, &pml);
		break;
	default:
		if ( (ps->eFlags & 0x300) != 0 )
		{
			PM_ClearLadderFlag(ps);
			ps->groundEntityNum = 1023;
			memset(&pml.walking, 0, 12);
			VectorClear(ps->velocity);
			PM_UpdateAimDownSightFlag(pmove, &pml);
			PM_UpdatePlayerWalkingFlag(pmove);
			PM_CheckDuck(pmove, &pml);
			PM_DropTimers(ps, &pml);
			PM_Footsteps(pmove, &pml);
			PM_ResetWeaponState(ps);
		}
		else
		{
			if ( (ps->pm_flags & 4) == 0 )
			{
				PM_CheckDuck(pmove, &pml);
				PM_GroundTrace(pmove, &pml);
			}
			Mantle_Check(pmove, &pml);
			if ( (ps->pm_flags & 4) != 0 )
			{
				PM_ClearLadderFlag(ps);
				ps->groundEntityNum = 1023;
				pml.groundPlane = 0;
				pml.walking = 0;
				PM_UpdateAimDownSightFlag(pmove, &pml);
				PM_UpdatePlayerWalkingFlag(pmove);
				PM_CheckDuck(pmove, &pml);
				Mantle_Move(pmove, ps, &pml);
				PM_Weapon(pmove, &pml);
			}
			else
			{
				PM_UpdateAimDownSightFlag(pmove, &pml);
				PM_UpdatePlayerWalkingFlag(pmove);
				PM_UpdatePronePitch(pmove, &pml);
				if ( ps->pm_type == PM_DEAD )
					PM_DeadMove(ps, &pml);
				PM_CheckLadderMove(pmove, &pml);
				PM_DropTimers(ps, &pml);
				if ( (ps->pm_flags & 0x20) != 0 )
				{
					PM_LadderMove(pmove, &pml);
				}
				else if ( pml.walking )
				{
					PM_WalkMove(pmove, &pml);
				}
				else
				{
					PM_AirMove(pmove, &pml);
				}
				PM_GroundTrace(pmove, &pml);
				PM_Footsteps(pmove, &pml);
				PM_Weapon(pmove, &pml);
				PM_FoliageSounds(pmove);
				VectorSubtract(ps->origin, pml.previous_origin, velocity);
				length = VectorLengthSquared(velocity);
				nextFrameTime = length / (pml.frametime * pml.frametime);
				lengthSquared = VectorLengthSquared(ps->velocity);
				if ( lengthSquared * 0.25 > nextFrameTime )
				{
					frametime = 1.0 / pml.frametime;
					VectorScale(velocity, frametime, ps->velocity);
				}
				Vector2Subtract(ps->velocity, ps->oldVelocity, newVelocity);
				newFrame = I_fmin(pml.frametime, 1.0);
				Vec2Scale(newVelocity, newFrame, newVelocity);
				Vector2Add(ps->oldVelocity, newVelocity, ps->oldVelocity);
				VectorRint(ps->velocity);
			}
		}
		break;
	}
}





int PM_ClampFallDamageMax(int x, int y, int z)
{
	if ( x < 0 )
		return z;
	return y;
}

int PM_ClampFallDamage(int val, int min, int max)
{
	int x;

	x = PM_ClampFallDamageMax(val - max, max, val);
	return PM_ClampFallDamageMax(min - val, min, x);
}







void PM_CrashLand(playerState_s *pm, pml_t *pml)
{
	int dmg;
	int hardDmg;
	int lightDmg;
	int medDmg;
	float gravity;
	float landVel;
	int stunTime;
	int damage;
	int viewDip;
	float fSpeedMult;
	float den;
	float acc;
	float t;
	float vel;
	float dist;
	float fallHeight;

	dist = pml->previous_origin[2] - pm->origin[2];
	vel = pml->previous_velocity[2];
	gravity = (float)pm->gravity;
	acc = -gravity * 0.5;
	den = vel * vel - acc * 4.0 * dist;

	if ( den >= 0.0 )
	{
		t = (-vel - I_sqrt(den)) / (acc + acc);
		landVel = t * -gravity + vel;
		fallHeight = -landVel * -landVel / ((float)pm->gravity + (float)pm->gravity);

		if ( bg_fallDamageMinHeight->current.decimal < (float)bg_fallDamageMaxHeight->current.decimal )
		{
			if ( bg_fallDamageMinHeight->current.decimal >= (float)fallHeight
			        || (pml->groundTrace.surfaceFlags & 1) != 0
			        || pm->pm_type > PM_INTERMISSION )
			{
				damage = 0;
			}
			else if ( fallHeight < (float)bg_fallDamageMaxHeight->current.decimal )
			{
				damage = PM_ClampFallDamage(
				             (int)((fallHeight - bg_fallDamageMinHeight->current.decimal)
				                   / (bg_fallDamageMaxHeight->current.decimal - bg_fallDamageMinHeight->current.decimal)
				                   * 100.0),
				             0,
				             100);
			}
			else
			{
				damage = 100;
			}
		}
		else
		{
			Com_Printf("bg_fallDamageMaxHeight must be greater than bg_fallDamageMinHeight\n");
			damage = 0;
		}
		if ( fallHeight > 12.0 )
		{
			viewDip = (int)((fallHeight - 12.0) / 26.0 * 4.0 + 4.0);

			if ( viewDip > 24 )
				viewDip = 24;

			BG_AnimScriptEvent(pm, ANIM_ET_LAND, 0, 1);
		}
		else
		{
			viewDip = 0;
		}

		if ( damage )
		{
			if ( damage > 99 || (pml->groundTrace.surfaceFlags & 2) != 0 )
			{
				VectorScale(pm->velocity, 0.67000002, pm->velocity);
			}
			else
			{
				stunTime = 35 * damage + 500;

				if ( stunTime > 2000 )
					stunTime = 2000;

				if ( stunTime > 500 )
				{
					if ( stunTime <= 1499 )
						fSpeedMult = 0.5 - ((float)stunTime - 500.0) / 1000.0 * 0.30000001;
					else
						fSpeedMult = 0.2;
				}
				else
				{
					fSpeedMult = 0.5;
				}

				pm->pm_time = stunTime;
				pm->pm_flags |= 0x200u;

				VectorScale(pm->velocity, fSpeedMult, pm->velocity);
			}

			dmg = PM_DamageLandingForSurface(pml);
			BG_AddPredictableEventToPlayerstate(dmg, damage, pm);
		}
		else if ( fallHeight > 4.0 )
		{
			if ( fallHeight >= 8.0 )
			{
				if ( fallHeight >= 12.0 )
				{
					VectorScale(pm->velocity, 0.67000002, pm->velocity);
					hardDmg = PM_HardLandingForSurface(pml);
					BG_AddPredictableEventToPlayerstate(hardDmg, viewDip, pm);
				}
				else
				{
					medDmg = PM_LightLandingForSurface(pml);
					PM_AddEvent(pm, medDmg);
				}
			}
			else
			{
				lightDmg = PM_MediumLandingForSurface(pml);
				PM_AddEvent(pm, lightDmg);
			}
		}
	}
}

void PM_LadderMove(pmove_t *pm, pml_t *pml)
{
	float sc;
	int move;
	float sidelenSqTemp;
	float absSpeedDrop;
	playerState_s *ps;
	int moveyaw;
	float fSpeedDrop;
	float fSideSpeedTemp;
	float fSideSpeed;
	float sidelenSq;
	vec2_t vSideDir;
	float upscale;
	vec3_t vTempRight;
	vec3_t wishvel;
	vec3_t out;
	float scale;
	float wishspeed;

	ps = pm->ps;

	if ( Jump_Check(pm, pml) )
	{
		PM_AirMove(pm, pml);
	}
	else
	{
		upscale = (pml->forward[2] + 0.25) * 2.5;

		if ( upscale <= 1.0 )
		{
			if ( upscale < -1.0 )
				upscale = -1.0;
		}
		else
		{
			upscale = 1.0;
		}

		pml->forward[2] = 0.0;
		Vec3Normalize(pml->forward);
		pml->right[2] = 0.0;
		Vec3NormalizeTo(pml->right, vTempRight);
		ProjectPointOnPlane(vTempRight, ps->vLadderVec, pml->right);
		scale = PM_CmdScale(ps, &pm->cmd);
		VectorClear(wishvel);

		if ( pm->cmd.forwardmove )
			wishvel[2] = upscale * 0.5 * scale * (float)pm->cmd.forwardmove;

		if ( pm->cmd.rightmove )
		{
			sc = scale * 0.2 * (float)pm->cmd.rightmove;
			VectorMA(wishvel, sc, pml->right, wishvel);
		}

		wishspeed = Vec3NormalizeTo(wishvel, out);
		PM_Accelerate(ps, pml, out, wishspeed, 9.0);

		if ( !pm->cmd.forwardmove )
		{
			if ( ps->velocity[2] <= 0.0 )
			{
				ps->velocity[2] = (float)ps->gravity * pml->frametime + ps->velocity[2];

				if ( ps->velocity[2] > 0.0 )
					ps->velocity[2] = 0.0;
			}
			else
			{
				ps->velocity[2] = ps->velocity[2] - (float)ps->gravity * pml->frametime;

				if ( ps->velocity[2] < 0.0 )
					ps->velocity[2] = 0.0;
			}
		}

		if ( !pm->cmd.rightmove )
		{
			Vector2Copy(pml->right, vSideDir);
			Vec2Normalize(vSideDir);
			fSideSpeedTemp = Dot2Product(vSideDir, ps->velocity);

			if ( fSideSpeedTemp != 0.0 )
			{
				VectorMA2(ps->velocity, -fSideSpeedTemp, vSideDir, ps->velocity);
				fSpeedDrop = fSideSpeedTemp * pml->frametime * 16.0;
				absSpeedDrop = I_fabs(fSideSpeedTemp);

				if ( absSpeedDrop > I_fabs(fSpeedDrop) )
				{
					if ( I_fabs(fSpeedDrop) < 1.0 )
					{
						if ( fSpeedDrop < 0.0 )
							fSpeedDrop = -1.0;
						else
							fSpeedDrop = 1.0;
					}

					fSideSpeed = fSideSpeedTemp - fSpeedDrop;
					VectorMA2(ps->velocity, fSideSpeed, vSideDir, ps->velocity);
				}
			}
		}

		if ( !pml->walking )
		{
			sidelenSq = Dot2Product(ps->vLadderVec, ps->velocity);
			VectorMA2(ps->velocity, -sidelenSq, ps->vLadderVec, ps->velocity);
			sidelenSqTemp = Vec2LengthSq(ps->velocity);

			if ( Square(ps->velocity[2]) >= sidelenSqTemp )
				VectorMA2(ps->velocity, -50.0, ps->vLadderVec, ps->velocity);
		}

		PM_StepSlideMove(pm, pml, 0);
		scale = vectoyaw(ps->vLadderVec) + 180.0;
		moveyaw = (int)AngleDelta(scale, ps->viewangles[1]);
		move = moveyaw;

		if ( moveyaw < 0 )
			move = -moveyaw;

		if ( move > 75 )
		{
			if ( moveyaw <= 0 )
				LOBYTE(moveyaw) = -75;
			else
				LOBYTE(moveyaw) = 75;
		}

		ps->movementDir = (char)moveyaw;
	}
}




int BG_CheckProneTurned(playerState_s *ps, float newProneYaw, unsigned char handler)
{
	float fraction;
	float dist;
	float abs;
	float testYaw;
	float delta;

	delta = AngleDelta(newProneYaw, ps->viewangles[1]);
	abs = I_fabs(delta) / 240.0;
	fraction = newProneYaw - (1.0 - abs) * delta;
	testYaw = AngleNormalize360Accurate(fraction);
	dist = abs * 45.0 + (1.0 - abs) * 66.0;

	return BG_CheckProne(
	           ps->clientNum,
	           ps->origin,
	           ps->maxs[0],
	           30.0,
	           testYaw,
	           &ps->fTorsoHeight,
	           &ps->fTorsoPitch,
	           &ps->fWaistPitch,
	           1,
	           ps->groundEntityNum != 1023,
	           0,
	           handler,
	           0,
	           dist);
}