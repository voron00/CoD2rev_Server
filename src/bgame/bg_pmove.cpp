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
int PM_GetViewHeightLerpTime( const playerState_t *ps, int iTarget, int bDown )
{
	if ( iTarget == PRONE_VIEWHEIGHT )
	{
		return 400;
	}

	if ( iTarget != CROUCH_VIEWHEIGHT )
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
	if ( ps->viewHeightTarget == CROUCH_VIEWHEIGHT )
	{
		return PM_EFF_STANCE_CROUCH;
	}

	if ( ps->viewHeightTarget == PRONE_VIEWHEIGHT )
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

		ps->viewangles[i] = SHORT2ANGLE( temp );
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
			// DHM - Nerve :: Allow players to look around while 'wounded' or lock to a medic if nearby
			short temp = cmd->angles[YAW] + ps->delta_angles[YAW];
			// rain - always allow this.  viewlocking will take precedence
			// if a medic is found
			// rain - using a full short and converting on the client so that
			// we get >1 degree resolution
			ps->stats[STAT_DEAD_YAW] = SHORT2ANGLE ( temp );
		}

		PM_UpdateLean(ps, msec, cmd, pmoveHandlers[handler].trace);
		return; // no view changes at all
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

	if ( iFromHeight != -1 && iToHeight != -1 && (iToHeight != ps->viewHeightLerpTarget || iToHeight == CROUCH_VIEWHEIGHT
	        && (iFromHeight != PRONE_VIEWHEIGHT || ps->viewHeightLerpDown) && (iFromHeight != DEFAULT_VIEWHEIGHT || !ps->viewHeightLerpDown)) )
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
#ifdef EXTRA_DEBUG
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
#ifdef EXTRA_DEBUG
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
#ifdef EXTRA_DEBUG
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
#ifdef EXTRA_DEBUG
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
#ifdef EXTRA_DEBUG
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
	float scale;
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

			if ( !(ps->pm_flags & PMF_ADS_WALK) && ps->leanf == 0 )
				bobmove = (fLadderSpeed / (fMaxSpeed * 1.0)) * 0.44999999;
			else
				bobmove = (fLadderSpeed / (fMaxSpeed * 0.40000001)) * 0.34999999;

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

		if ( ps->viewHeightTarget == PRONE_VIEWHEIGHT )
		{
			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_IDLEPRONE, qtrue);
		}
		else if ( ps->viewHeightTarget == CROUCH_VIEWHEIGHT )
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
			if ( ps->viewHeightTarget == CROUCH_VIEWHEIGHT )
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

	scale = ps->speed;

	if ( pm->cmd.forwardmove )
	{
		if ( pm->cmd.rightmove )
		{
			scale *= ((player_strafeSpeedScale->current.decimal - 1.0) * 0.75 + 1.0 + 1.0) * 0.5;

			if ( pm->cmd.forwardmove < 0 )
			{
				scale *= (player_backSpeedScale->current.decimal + 1.0) * 0.5;
			}
		}
		else if ( pm->cmd.forwardmove < 0 )
		{
			scale *= player_backSpeedScale->current.decimal;
		}

		BG_UpdateConditionValue(ps->clientNum, ANIM_COND_STRAFING, LEANING_NOT, qtrue);
	}
	else if ( pm->cmd.rightmove )
	{
		scale *= ((player_strafeSpeedScale->current.decimal - 1.0) * 0.75 + 1.0);

		if ( pm->cmd.rightmove > 0 )
			BG_UpdateConditionValue(ps->clientNum, ANIM_COND_STRAFING, LEANING_RIGHT, qtrue);
		else
			BG_UpdateConditionValue(ps->clientNum, ANIM_COND_STRAFING, LEANING_LEFT, qtrue);
	}

	if ( iswalking )
	{
		scale *= 0.40000001;
	}

	lerpFrac = PM_GetViewHeightLerp(pm, CROUCH_VIEWHEIGHT, PRONE_VIEWHEIGHT);

	if ( lerpFrac == 0 )
	{
		lerpFrac = PM_GetViewHeightLerp(pm, PRONE_VIEWHEIGHT, CROUCH_VIEWHEIGHT);

		if ( lerpFrac == 0 )
		{
			if ( iStance == PM_EFF_STANCE_PRONE )
			{
				scale *= 0.15000001;
			}
			else if ( iStance == PM_EFF_STANCE_CROUCH )
			{
				scale *= 0.64999998;
			}
		}
		else
		{
			scale *= (lerpFrac * 0.64999998 + (1.0 - lerpFrac) * 0.15000001);
		}
	}
	else
	{
		scale *= (lerpFrac * 0.15000001 + (1.0 - lerpFrac) * 0.64999998);
	}

	if ( iStance == PM_EFF_STANCE_PRONE )
	{
		if ( iswalking )
			bobmove = pm->xyspeed / scale * 0.23999999;
		else
			bobmove = pm->xyspeed / scale * 0.25;

		if ( !(ps->pm_flags & PMF_BACKWARDS_RUN) )
			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_WALKPRONE, qtrue);
		else
			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_WALKPRONEBK, qtrue);
	}
	else if ( iStance == PM_EFF_STANCE_CROUCH )
	{
		if ( iswalking )
			bobmove = pm->xyspeed / scale * 0.315;
		else
			bobmove = pm->xyspeed / scale * 0.34;

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
			bobmove = pm->xyspeed / scale * 0.30500001;

			if ( ps->damageTimer > stumble_end_time )
				animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_STUMBLE_WALK_FORWARD, qtrue);
			else
				animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_WALK, qtrue);
		}
		else
		{
			bobmove = pm->xyspeed / scale * 0.33500001;

			if ( ps->damageTimer > stumble_end_time )
				animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_STUMBLE_FORWARD, qtrue);
			else
				animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_RUN, qtrue);
		}
	}
	else if ( iswalking )
	{
		bobmove = pm->xyspeed / scale * 0.32499999;

		if ( ps->damageTimer > stumble_end_time )
			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_STUMBLE_WALK_BACKWARD, qtrue);
		else
			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_WALKBK, qtrue);
	}
	else
	{
		bobmove = pm->xyspeed / scale * 0.36000001;

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

		if ( ps->viewHeightTarget == PRONE_VIEWHEIGHT )
		{
			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_IDLEPRONE, qtrue);
		}
		else if ( ps->viewHeightTarget == CROUCH_VIEWHEIGHT )
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

/*
=============
PM_GroundTraceMissed

The ground trace didn't hit a surface, so we are in freefall
=============
*/
static void PM_GroundTraceMissed( pmove_t *pm, pml_t *pml )
{
	trace_t trace;
	vec3_t point;
	playerState_t *ps;

	ps = pm->ps;

	if ( pm->ps->groundEntityNum != ENTITYNUM_NONE )
	{
		// if they aren't in a jumping animation and the ground is a ways away, force into it
		// if we didn't do the trace, the player would be backflipping down staircases
		VectorCopy( ps->origin, point );
		point[2] -= 64;

		pmoveHandlers[pm->handler].trace(&trace, ps->origin, pm->mins, pm->maxs, point, ps->clientNum, pm->tracemask);
		if ( trace.fraction == 1.0 )
		{
			if ( pm->cmd.forwardmove >= 0 )
			{
				BG_AnimScriptEvent( ps, ANIM_ET_JUMP, qfalse, qtrue );
			}
			else
			{
				BG_AnimScriptEvent( ps, ANIM_ET_JUMPBK, qfalse, qtrue );
			}
			pml->almostGroundPlane = qfalse;
		}
		else
		{
			pml->almostGroundPlane = trace.fraction < 0.015625;
		}
	}
	else
	{
		VectorCopy(ps->origin, point);
		point[2] = point[2] - 1.0;

		pmoveHandlers[pm->handler].trace(&trace, ps->origin, pm->mins, pm->maxs, point, ps->clientNum, pm->tracemask);
		pml->almostGroundPlane = 1.0 != trace.fraction;
	}

	// Signify that we're in mid-air
	ps->groundEntityNum = ENTITYNUM_NONE;
	pml->groundPlane = qfalse;
	pml->walking = qfalse;
}

/*
============
PM_CmdScale_Walk

Returns the scale factor to apply to cmd movements
This allows the clients to use axial -127 to 127 values for all directions
without getting a sqrt(2) distortion in speed.
============
*/
static float PM_CmdScale_Walk( pmove_t *pm, usercmd_t *cmd )
{
	int iStance;
	float lerpFrac;
	playerState_t *ps;
	float total;
	float scale;
	float max;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	total = I_sqrt( Square(cmd->rightmove) + Square(cmd->forwardmove) );

	if ( cmd->forwardmove >= 0 )
		max = I_fmax(I_fabs(cmd->forwardmove), I_fabs(cmd->rightmove * player_strafeSpeedScale->current.decimal));
	else
		max = I_fmax(I_fabs(cmd->forwardmove * player_backSpeedScale->current.decimal), I_fabs(cmd->rightmove * player_strafeSpeedScale->current.decimal));

	if ( max == 0 )
	{
		return 0;
	}

	scale = (float)ps->speed * max / ( 127.0 * total );

	if ( ps->pm_flags & PMF_ADS_WALK || ps->leanf != 0 )
	{
		scale *= 0.40000001;
	}

	if ( ps->pm_type == PM_NOCLIP )
	{
		scale *= 3;
	}
	else if ( ps->pm_type == PM_UFO )
	{
		scale *= 6;
	}
	else
	{
		iStance = PM_GetEffectiveStance(ps);
		lerpFrac = PM_GetViewHeightLerp(pm, CROUCH_VIEWHEIGHT, PRONE_VIEWHEIGHT);

		if ( lerpFrac == 0 )
		{
			lerpFrac = PM_GetViewHeightLerp(pm, PRONE_VIEWHEIGHT, CROUCH_VIEWHEIGHT);

			if ( lerpFrac == 0 )
			{
				if ( iStance == PM_EFF_STANCE_PRONE )
				{
					scale *= 0.15000001;
				}
				else if ( iStance == PM_EFF_STANCE_CROUCH )
				{
					scale *= 0.64999998;
				}
			}
			else
			{
				scale *= (lerpFrac * 0.64999998 + (1.0 - lerpFrac) * 0.15000001);
			}
		}
		else
		{
			scale *= (lerpFrac * 0.15000001 + (1.0 - lerpFrac) * 0.64999998);
		}
	}

	if ( ps->weapon && BG_GetWeaponDef(ps->weapon)->moveSpeedScale > 0 )
	{
		scale *= BG_GetWeaponDef(ps->weapon)->moveSpeedScale;
	}

	if ( cmd->buttons & BUTTON_ADS_LEGACY )
	{
		scale *= 0.40000001;
	}

	return scale;
}

/*
============
BG_CheckProneTurned
============
*/
qboolean BG_CheckProneTurned( playerState_t *ps, float newProneYaw, byte handler )
{
	float feet_dist;
	float fraction;
	float testYaw;
	float delta;

	delta = AngleDelta(newProneYaw, ps->viewangles[YAW]);
	fraction = I_fabs(delta) / 240.0;
	testYaw = AngleNormalize360Accurate(newProneYaw - (1.0 - fraction) * delta);
	feet_dist = fraction * 45.0 + (1.0 - fraction) * 66.0;

	return BG_CheckProne( ps->clientNum, ps->origin, ps->maxs[0], 30.0, testYaw,
	                      &ps->fTorsoHeight, &ps->fTorsoPitch, &ps->fWaistPitch,
	                      qtrue, ps->groundEntityNum != ENTITYNUM_NONE, NULL, handler, PCT_CLIENT, feet_dist );
}

/*
============
PM_ViewHeightAdjust
============
*/
void PM_ViewHeightAdjust( pmove_t *pm, pml_t *pml )
{
	vec3_t vDir;
	vec3_t vel;
	float scale;
	float fNewPosOfs;
	int iLerpTime;
	int iLerpFrac;
	playerState_t *ps;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	if ( !ps->viewHeightTarget || ps->viewHeightCurrent == 0 )
	{
		if ( ps->pm_type == PM_SPECTATOR )
		{
			ps->viewHeightCurrent = 0;
		}
		else
		{
			ps->viewHeightCurrent = (float)ps->viewHeightTarget;
		}

		return;
	}

	if ( (ps->viewHeightCurrent == ps->viewHeightTarget) && !ps->viewHeightLerpTime )
	{
		return;
	}

	iLerpFrac = 0;
	fNewPosOfs = 0;

	if ( ps->viewHeightTarget != PRONE_VIEWHEIGHT && ps->viewHeightTarget != CROUCH_VIEWHEIGHT && ps->viewHeightTarget != DEFAULT_VIEWHEIGHT )
	{
		ps->viewHeightLerpTime = 0;

		if ( ps->viewHeightTarget > ps->viewHeightCurrent )
		{
			ps->viewHeightCurrent = pml->frametime * 180 + ps->viewHeightCurrent;

			if ( ps->viewHeightCurrent >= ps->viewHeightTarget )
			{
				ps->viewHeightCurrent = (float)ps->viewHeightTarget;
			}
		}
		else
		{
			ps->viewHeightCurrent = ps->viewHeightCurrent - pml->frametime * 180;

			if ( ps->viewHeightTarget >= ps->viewHeightCurrent )
			{
				ps->viewHeightCurrent = (float)ps->viewHeightTarget;
			}
		}

		return;
	}

	if ( ps->viewHeightLerpTime )
	{
		iLerpTime = PM_GetViewHeightLerpTime(ps, ps->viewHeightLerpTarget, ps->viewHeightLerpDown);
		iLerpFrac = 100 * (pm->cmd.serverTime - ps->viewHeightLerpTime) / iLerpTime;

		if ( iLerpFrac < 0 )
		{
			iLerpFrac = 0;
		}
		else
		{
			if ( iLerpFrac > 100 )
			{
				iLerpFrac = 100;
			}
		}

		if ( iLerpFrac == 100 )
		{
			ps->viewHeightCurrent = (float)ps->viewHeightLerpTarget;
			ps->viewHeightLerpTime = 0;
			ps->viewHeightLerpPosAdj = 0;
		}
		else
		{
			if ( ps->viewHeightLerpTarget == PRONE_VIEWHEIGHT )
			{
				ps->viewHeightCurrent = PM_ViewHeightTableLerp(iLerpFrac, viewLerp_CrouchProne, &fNewPosOfs);
			}
			else if ( ps->viewHeightLerpTarget == CROUCH_VIEWHEIGHT )
			{
				if ( ps->viewHeightLerpDown )
					ps->viewHeightCurrent = PM_ViewHeightTableLerp(iLerpFrac, viewLerp_StandCrouch, &fNewPosOfs);
				else
					ps->viewHeightCurrent = PM_ViewHeightTableLerp(iLerpFrac, viewLerp_ProneCrouch, &fNewPosOfs);
			}
			else
			{
				ps->viewHeightCurrent = PM_ViewHeightTableLerp(iLerpFrac, viewLerp_CrouchStand, &fNewPosOfs);
			}

			if ( I_fabs(ps->viewHeightLerpPosAdj - fNewPosOfs) > 0.050000001 )
			{
				VectorCopy(ps->velocity, vel);
				scale = fNewPosOfs - ps->viewHeightLerpPosAdj;

				if ( ps->groundEntityNum == ENTITYNUM_NONE )
				{
					scale *= 0.5;
				}

				scale = scale / pml->frametime;

				vDir[0] = pml->forward[0];
				vDir[1] = pml->forward[1];
				vDir[2] = 0.0;

				Vec3Normalize(vDir);
				VectorScale(vDir, scale, ps->velocity);

				PM_StepSlideMove(pm, pml, qtrue);
				VectorCopy(vel, ps->velocity);

				ps->viewHeightLerpPosAdj = fNewPosOfs;
			}
		}
	}

	if ( ps->viewHeightLerpTime )
	{
		if ( ps->viewHeightTarget != ps->viewHeightLerpTarget && (ps->viewHeightTarget < ps->viewHeightLerpTarget
		        && !ps->viewHeightLerpDown || ps->viewHeightTarget > ps->viewHeightLerpTarget && ps->viewHeightLerpDown) )
		{
			iLerpFrac = 100 - iLerpFrac;
			ps->viewHeightLerpDown ^= qtrue;

			if ( ps->viewHeightLerpDown )
			{
				if ( ps->viewHeightLerpTarget == DEFAULT_VIEWHEIGHT )
				{
					ps->viewHeightLerpTarget = CROUCH_VIEWHEIGHT;
				}
				else if ( ps->viewHeightLerpTarget == CROUCH_VIEWHEIGHT )
				{
					ps->viewHeightLerpTarget = PRONE_VIEWHEIGHT;
				}
			}
			else if ( ps->viewHeightLerpTarget == PRONE_VIEWHEIGHT )
			{
				ps->viewHeightLerpTarget = CROUCH_VIEWHEIGHT;
			}
			else if ( ps->viewHeightLerpTarget == CROUCH_VIEWHEIGHT )
			{
				ps->viewHeightLerpTarget = DEFAULT_VIEWHEIGHT;
			}

			if ( iLerpFrac == 100 )
			{
				ps->viewHeightCurrent = (float)ps->viewHeightLerpTarget;
				ps->viewHeightLerpTime = 0;
				ps->viewHeightLerpPosAdj = 0;
			}
			else
			{
				iLerpTime = PM_GetViewHeightLerpTime(ps, ps->viewHeightLerpTarget, ps->viewHeightLerpDown);
				ps->viewHeightLerpTime = pm->cmd.serverTime - iLerpFrac * 0.0099999998 * iLerpTime;

				if ( ps->viewHeightLerpTarget == PRONE_VIEWHEIGHT )
				{
					PM_ViewHeightTableLerp(iLerpFrac, viewLerp_CrouchProne, &fNewPosOfs);
				}
				else if ( ps->viewHeightLerpTarget == CROUCH_VIEWHEIGHT )
				{
					if ( ps->viewHeightLerpDown )
						PM_ViewHeightTableLerp(iLerpFrac, viewLerp_StandCrouch, &fNewPosOfs);
					else
						PM_ViewHeightTableLerp(iLerpFrac, viewLerp_ProneCrouch, &fNewPosOfs);
				}
				else
				{
					PM_ViewHeightTableLerp(iLerpFrac, viewLerp_CrouchStand, &fNewPosOfs);
				}

				ps->viewHeightLerpPosAdj = fNewPosOfs;
			}
		}
	}
	else if ( ps->viewHeightCurrent != ps->viewHeightTarget )
	{
		ps->viewHeightLerpTime = pm->cmd.serverTime;

		switch ( ps->viewHeightTarget )
		{
		case PRONE_VIEWHEIGHT:
			ps->viewHeightLerpDown = qtrue;
			if ( ps->viewHeightCurrent <= CROUCH_VIEWHEIGHT )
				ps->viewHeightLerpTarget = PRONE_VIEWHEIGHT;
			else
				ps->viewHeightLerpTarget = CROUCH_VIEWHEIGHT;
			break;

		case CROUCH_VIEWHEIGHT:
			ps->viewHeightLerpDown = ps->viewHeightCurrent > ps->viewHeightTarget;
			ps->viewHeightLerpTarget = CROUCH_VIEWHEIGHT;
			break;

		case DEFAULT_VIEWHEIGHT:
			ps->viewHeightLerpDown = qfalse;
			if ( ps->viewHeightCurrent >= CROUCH_VIEWHEIGHT )
				ps->viewHeightLerpTarget = DEFAULT_VIEWHEIGHT;
			else
				ps->viewHeightLerpTarget = CROUCH_VIEWHEIGHT;
			break;

		default:
			assert(va( "View height lerp to %i reached bad place\n", ps->viewHeightTarget ));
			break;
		}
	}
}

/*
==============
PM_CheckDuck

Sets mins, maxs, and pm->ps->viewheight
==============
*/
void PM_CheckDuck( pmove_t *pm, pml_t *pml )
{
	vec3_t vEnd;
	float delta;
	vec3_t vPoint;
	playerState_t *ps;
	int iStance;
	trace_t trace;
	qboolean bWasStanding;
	qboolean bWasProne;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	pm->proneChange = qfalse;

	if ( ps->pm_type == PM_SPECTATOR )
	{
		// Ridah, modified this for configurable bounding boxes
		pm->mins[0] = -8.0;
		pm->mins[1] = -8.0;
		pm->mins[2] = -8.0;

		pm->maxs[0] = 8.0;
		pm->maxs[1] = 8.0;
		pm->maxs[2] = 16.0;

		ps->pm_flags &= ~(PMF_PRONE | PMF_DUCKED);

		if ( pm->cmd.buttons & BUTTON_PRONE )
		{
			pm->cmd.buttons &= ~BUTTON_PRONE;
			BG_AddPredictableEventToPlayerstate(EV_STANCE_FORCE_STAND, 0, ps);
		}

		ps->viewHeightTarget = 0;
		ps->viewHeightCurrent = 0;

		return;
	}

	bWasProne = (ps->pm_flags & PMF_PRONE) != 0;
	bWasStanding = !(ps->pm_flags & (PMF_PRONE | PMF_DUCKED));

	// Ridah, modified this for configurable bounding boxes
	pm->mins[0] = ps->mins[0];
	pm->mins[1] = ps->mins[1];

	pm->maxs[0] = ps->maxs[0];
	pm->maxs[1] = ps->maxs[1];

	pm->mins[2] = ps->mins[2];

	if ( ps->pm_type >= PM_DEAD )
	{
		pm->maxs[2] = ps->maxs[2];  // NOTE: must set death bounding box in game code
		ps->viewHeightTarget = DEAD_VIEWHEIGHT;
		PM_ViewHeightAdjust(pm, pml);
		return;
	}

	if ( ps->eFlags & EF_TURRET_ACTIVE )
	{
		if ( ps->eFlags & EF_TURRET_PRONE && !(ps->eFlags & EF_TURRET_DUCK) )
		{
			ps->pm_flags |= PMF_PRONE;
			ps->pm_flags &= ~PMF_DUCKED;
		}
		else
		{
			if ( ps->eFlags & EF_TURRET_DUCK && !(ps->eFlags & EF_TURRET_PRONE) )
			{
				ps->pm_flags |= PMF_DUCKED;
				ps->pm_flags &= ~PMF_PRONE;
			}
			else
			{
				ps->pm_flags &= ~(PMF_PRONE | PMF_DUCKED);
			}
		}
	}
	else if ( !(ps->pm_flags & PMF_UNKNOWN_8000) )
	{
		if ( ps->pm_flags & PMF_LADDER && pm->cmd.buttons & (BUTTON_PRONE | BUTTON_CROUCH) )
		{
			pm->cmd.buttons &= ~(BUTTON_PRONE | BUTTON_CROUCH);
			BG_AddPredictableEventToPlayerstate(EV_STANCE_FORCE_STAND, 0, ps);
		}

		if ( pm->cmd.buttons & BUTTON_PRONE )
		{
			if ( ps->pm_flags & PMF_PRONE || ps->groundEntityNum != ENTITYNUM_NONE
			        && BG_CheckProne( ps->clientNum, ps->origin, pm->maxs[0], 30.0,
			                          ps->viewangles[YAW], &ps->fTorsoHeight, &ps->fTorsoPitch, &ps->fWaistPitch,
			                          qfalse, ps->groundEntityNum != ENTITYNUM_NONE, NULL, pm->handler, PCT_CLIENT, 66.0) )
			{
				ps->pm_flags |= PMF_PRONE;
				ps->pm_flags &= ~PMF_DUCKED;
			}
			else if ( ps->groundEntityNum != ENTITYNUM_NONE )
			{
				ps->pm_flags |= PMF_PRONE_BLOCKED;

				if ( !(pm->cmd.buttons & BUTTON_CANNOT_PRONE) )
				{
					if ( ps->pm_flags & PMF_DUCKED)
						BG_AddPredictableEventToPlayerstate(EV_STANCE_FORCE_CROUCH, 0, ps);
					else
						BG_AddPredictableEventToPlayerstate(EV_STANCE_FORCE_STAND, 0, ps);
				}
			}
		}
		else if ( pm->cmd.buttons & BUTTON_CROUCH )
		{
			if ( ps->pm_flags & PMF_PRONE )
			{
				pm->maxs[2] = 50.0;
				pmoveHandlers[pm->handler].trace(&trace, ps->origin, pm->mins, pm->maxs, ps->origin, ps->clientNum, pm->tracemask & ~CONTENTS_BODY);

				if ( trace.allsolid )
				{
					if ( !(pm->cmd.buttons & BUTTON_CANNOT_PRONE) )
					{
						BG_AddPredictableEventToPlayerstate(EV_STANCE_FORCE_PRONE, 2, ps);
					}
				}
				else
				{
					BG_AnimScriptEvent(ps, ANIM_ET_PRONE_TO_CROUCH, qfalse, qfalse);

					ps->pm_flags &= ~PMF_PRONE;
					ps->pm_flags |= PMF_DUCKED;
				}
			}
			else
			{
				BG_AnimScriptEvent(ps, ANIM_ET_STAND_TO_CROUCH, qfalse, qfalse);
				ps->pm_flags |= PMF_DUCKED;
			}
		}
		else if ( ps->pm_flags & PMF_PRONE )
		{
			pm->maxs[2] = ps->maxs[2];
			pmoveHandlers[pm->handler].trace(&trace, ps->origin, pm->mins, pm->maxs, ps->origin, ps->clientNum, pm->tracemask & ~CONTENTS_BODY);

			if ( trace.allsolid )
			{
				pm->maxs[2] = 50.0;
				pmoveHandlers[pm->handler].trace(&trace, ps->origin, pm->mins, pm->maxs, ps->origin, ps->clientNum, pm->tracemask & ~CONTENTS_BODY);

				if ( trace.allsolid )
				{
					if ( !(pm->cmd.buttons & BUTTON_CANNOT_PRONE) )
					{
						BG_AddPredictableEventToPlayerstate(EV_STANCE_FORCE_PRONE, 1, ps);
					}
				}
				else
				{
					ps->pm_flags &= ~PMF_PRONE;
					ps->pm_flags |= PMF_DUCKED;
				}
			}
			else
			{
				BG_AnimScriptEvent(ps, ANIM_ET_PRONE_TO_STAND, qfalse, qfalse);
				ps->pm_flags &= ~(PMF_PRONE | PMF_DUCKED);
			}
		}
		else if ( ps->pm_flags & PMF_DUCKED )
		{
			pm->maxs[2] = ps->maxs[2];
			pmoveHandlers[pm->handler].trace(&trace, ps->origin, pm->mins, pm->maxs, ps->origin, ps->clientNum, pm->tracemask & ~CONTENTS_BODY);

			if ( trace.allsolid )
			{
				if ( !(pm->cmd.buttons & BUTTON_CANNOT_PRONE) )
				{
					BG_AddPredictableEventToPlayerstate(EV_STANCE_FORCE_CROUCH, 1, ps);
				}
			}
			else
			{
				BG_AnimScriptEvent(ps, ANIM_ET_CROUCH_TO_STAND, qfalse, qfalse);
				ps->pm_flags &= ~PMF_DUCKED;
			}
		}
	}

	if ( !ps->viewHeightLerpTime )
	{
		if ( ps->pm_flags & PMF_PRONE )
		{
			if ( ps->viewHeightTarget == DEFAULT_VIEWHEIGHT )
			{
				ps->viewHeightTarget = CROUCH_VIEWHEIGHT;
			}
			else if ( ps->viewHeightTarget != PRONE_VIEWHEIGHT )
			{
				ps->viewHeightTarget = PRONE_VIEWHEIGHT;
				pm->proneChange = qtrue;

				BG_PlayAnim(ps, ANIM_MT_UNUSED, ANIM_BP_TORSO, 0, qfalse, qtrue, qtrue);
				Jump_ActivateSlowdown(ps);
			}
		}
		else if ( ps->viewHeightTarget == PRONE_VIEWHEIGHT )
		{
			ps->viewHeightTarget = CROUCH_VIEWHEIGHT;
			pm->proneChange = qtrue;

			BG_PlayAnim(ps, ANIM_MT_UNUSED, ANIM_BP_TORSO, 0, qfalse, qtrue, qtrue);
		}
		else if ( ps->pm_flags & PMF_DUCKED )
		{
			ps->viewHeightTarget = CROUCH_VIEWHEIGHT;
		}
		else
		{
			ps->viewHeightTarget = DEFAULT_VIEWHEIGHT;
		}
	}

	PM_ViewHeightAdjust(pm, pml);
	iStance = PM_GetEffectiveStance(ps);

	if ( iStance == PM_EFF_STANCE_PRONE )
	{
		pm->maxs[2] = 30.0;

		ps->eFlags |= EF_PRONE;
		ps->eFlags &= ~EF_CROUCH;

		ps->pm_flags |= PMF_PRONE;
		ps->pm_flags &= ~PMF_DUCKED;
	}
	else
	{
		if ( iStance == PM_EFF_STANCE_CROUCH )
		{
			pm->maxs[2] = 50.0;

			ps->eFlags |= EF_CROUCH;
			ps->eFlags &= ~EF_PRONE;

			ps->pm_flags |= PMF_DUCKED;
			ps->pm_flags &= ~PMF_PRONE;
		}
		else
		{
			pm->maxs[2] = ps->maxs[2];

			ps->eFlags &= ~(EF_CROUCH | EF_PRONE);
			ps->pm_flags = ps->pm_flags & ~(PMF_PRONE | PMF_DUCKED);
		}
	}

	if ( ps->pm_flags & PMF_PRONE && !bWasProne )
	{
		if ( pm->cmd.forwardmove || pm->cmd.rightmove )
		{
			ps->pm_flags &= ~PMF_ADS_OVERRIDE;
			PM_ExitAimDownSight(ps);
		}

		VectorCopy(ps->origin, vEnd);
		vEnd[2] = vEnd[2] + 10.0;

		pmoveHandlers[pm->handler].trace(&trace, ps->origin, pm->mins, pm->maxs, vEnd, ps->clientNum, pm->tracemask & ~CONTENTS_BODY);

		Vec3Lerp(ps->origin, vEnd, trace.fraction, vEnd);
		pmoveHandlers[pm->handler].trace(&trace, vEnd, pm->mins, pm->maxs, ps->origin, ps->clientNum, pm->tracemask & ~CONTENTS_BODY);

		Vec3Lerp(vEnd, ps->origin, trace.fraction, ps->origin);
		ps->proneDirection = ps->viewangles[YAW];

		VectorCopy(ps->origin, vPoint);
		vPoint[2] = vPoint[2] - 0.25;

		pmoveHandlers[pm->handler].trace(&trace, ps->origin, pm->mins, pm->maxs, vPoint, ps->clientNum, pm->tracemask & ~CONTENTS_BODY);

		if ( trace.startsolid || trace.fraction >= 1.0 )
			ps->proneDirectionPitch = 0;
		else
			ps->proneDirectionPitch = PitchForYawOnNormal(ps->proneDirection, trace.normal);

		delta = AngleDelta(ps->proneDirectionPitch, ps->viewangles[PITCH]);

		if ( delta < -45.0 )
			ps->proneTorsoPitch = ps->viewangles[PITCH] - 45.0;
		else if ( delta > 45.0 )
			ps->proneTorsoPitch = ps->viewangles[PITCH] + 45.0;
		else
			ps->proneTorsoPitch = ps->proneDirectionPitch;
	}
}

/*
=================
PM_CrashLand

Check for hard landings that generate sound events
=================
*/
static void PM_CrashLand( playerState_t *ps, pml_t *pml )
{
	float dist;
	float vel, acc;
	float t;
	float a, b, c, den;
	int viewDip;
	float landVel;
	float fallHeight;
	float fSpeedMult;
	int stunTime;
	int damage;

	// calculate the exact velocity on landing
	dist = pml->previous_origin[2] - ps->origin[2];
	vel = pml->previous_velocity[2];
	acc = -ps->gravity;

	a = acc * 0.5;
	b = vel;
	c = dist;

	den =  b * b - 4 * a * c;
	if ( den < 0 )
	{
		return;
	}
	t = ( -b - I_sqrt( den ) ) / ( 2 * a );

	landVel = ((t * acc) + vel) * -1;
	fallHeight = (Square(landVel)) / (ps->gravity * 2);

	if ( bg_fallDamageMinHeight->current.decimal >= bg_fallDamageMaxHeight->current.decimal )
	{
		Com_Printf("bg_fallDamageMaxHeight must be greater than bg_fallDamageMinHeight\n");
		damage = 0;
	}
	else
	{
		// SURF_NODAMAGE is used for bounce pads where you don't ever
		// want to take damage or play a crunch sound
		if ( bg_fallDamageMinHeight->current.decimal >= fallHeight || pml->groundTrace.surfaceFlags & SURF_NODAMAGE || ps->pm_type >= PM_DEAD )
		{
			damage = 0;
		}
		else if ( fallHeight >= bg_fallDamageMaxHeight->current.decimal )
		{
			damage = 100;
		}
		else
		{
			damage = I_clamp(((fallHeight - bg_fallDamageMinHeight->current.decimal) / (bg_fallDamageMaxHeight->current.decimal - bg_fallDamageMinHeight->current.decimal) * 100), 0, 100);
		}
	}

	if ( fallHeight <= 12 )
	{
		viewDip = 0;
	}
	else
	{
		viewDip = (int)((fallHeight - 12) / 26 * 4 + 4);

		if ( viewDip > 24 )
		{
			viewDip = 24;
		}

		BG_AnimScriptEvent(ps, ANIM_ET_LAND, qfalse, qtrue);
	}

	if ( damage )
	{
		if ( damage >= 100 || pml->groundTrace.surfaceFlags & SURF_SLICK )
		{
			VectorScale(ps->velocity, 0.67000002, ps->velocity);
		}
		else
		{
			stunTime = 35 * damage + 500;

			if ( stunTime > 2000 )
			{
				stunTime = 2000;
			}

			if ( stunTime <= 500 )
			{
				fSpeedMult = 0.5;
			}
			else
			{
				if ( stunTime >= 1500 )
					fSpeedMult = 0.2;
				else
					fSpeedMult = 0.5 - ((stunTime - 500.0) / 1000.0) * 0.30000001;
			}

			ps->pm_time = stunTime;
			ps->pm_flags |= PMF_TIME_SLIDE;

			VectorScale(ps->velocity, fSpeedMult, ps->velocity);
		}

		BG_AddPredictableEventToPlayerstate(PM_DamageLandingForSurface(pml), damage, ps);
	}
	else
	{
		if ( fallHeight <= 4 )
		{
			return;
		}
		else if ( fallHeight < 8 )
		{
			PM_AddEvent(ps, PM_MediumLandingForSurface(pml));
		}
		else if ( fallHeight < 12 )
		{
			PM_AddEvent(ps, PM_LightLandingForSurface(pml));
		}
		else
		{
			VectorScale(ps->velocity, 0.67000002, ps->velocity);
			BG_AddPredictableEventToPlayerstate(PM_HardLandingForSurface(pml), viewDip, ps);
		}
	}
}

/*
=============
PM_GroundTrace
=============
*/
void PM_GroundTrace( pmove_t *pm, pml_t *pml )
{
	playerState_t *ps;
	trace_t trace;
	vec3_t point;
	vec3_t start;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	start[0] = pm->ps->origin[0];
	start[1] = ps->origin[1];

	point[0] = ps->origin[0];
	point[1] = ps->origin[1];

	if ( ps->eFlags & EF_TURRET_ACTIVE )
	{
		start[2] = ps->origin[2];
		point[2] = ps->origin[2] - 1.0;
	}
	else
	{
		start[2] = ps->origin[2] + 0.25;
		point[2] = ps->origin[2] - 0.25;
	}

	PM_playerTrace(pm, &trace, start, pm->mins, pm->maxs, point, ps->clientNum, pm->tracemask);
	pml->groundTrace = trace;

	// do something corrective if the trace starts in a solid...
	if ( trace.allsolid )
	{
		if ( !PM_CorrectAllSolid( pm, pml, &trace ) )
		{
			return;
		}
	}

	if ( trace.startsolid )
	{
		start[2] = ps->origin[2] - 0.001;
		PM_playerTrace(pm, &trace, start, pm->mins, pm->maxs, point, ps->clientNum, pm->tracemask);

		if ( trace.startsolid )
		{
			ps->groundEntityNum = ENTITYNUM_NONE;
			pml->groundPlane = qfalse;
			pml->almostGroundPlane = qfalse;
			pml->walking = qfalse;
			return;
		}

		pml->groundTrace = trace;
	}

	// if the trace didn't hit anything, we are in free fall
	if ( trace.fraction == 1.0 )
	{
		PM_GroundTraceMissed(pm, pml);
		return;
	}

	assert(trace.normal[0] || trace.normal[1] || trace.normal[2]);

	// check if getting thrown off the ground
	if ( ps->velocity[2] > 0 && DotProduct( ps->velocity, trace.normal ) > 10 && !(ps->pm_flags & PMF_PRONE ) )
	{
		// go into jump animation
		if ( pm->cmd.forwardmove >= 0 )
		{
			BG_AnimScriptEvent(ps, ANIM_ET_JUMP, qfalse, qfalse);
		}
		else
		{
			BG_AnimScriptEvent(ps, ANIM_ET_JUMPBK, qfalse, qfalse);
		}

		pml->almostGroundPlane = qfalse;
		ps->groundEntityNum = ENTITYNUM_NONE;
		pml->groundPlane = qfalse;
		pml->walking = qfalse;
		return;
	}

	// slopes that are too steep will not be considered onground
	if ( trace.normal[2] < MIN_WALK_NORMAL )
	{
		ps->groundEntityNum = ENTITYNUM_NONE;
		pml->groundPlane = qtrue;
		pml->almostGroundPlane = qtrue;
		pml->walking = qfalse;
		Jump_ClearState(ps);
		return;
	}

	pml->groundPlane = qtrue;
	pml->almostGroundPlane = qtrue;
	pml->walking = qtrue;

	if ( ps->groundEntityNum == ENTITYNUM_NONE )
	{
		// just hit the ground
		PM_CrashLand(ps, pml);
	}

	ps->groundEntityNum = trace.entityNum;

	// don't reset the z velocity for slopes
	//	pm->ps->velocity[2] = 0;

	PM_AddTouchEnt(pm, trace.entityNum);
}

/*
=============
PM_DoPlayerInertia
=============
*/
bool PM_DoPlayerInertia( playerState_t *ps, float accelspeed, const vec3_t wishdir )
{
	vec2_t vel;
	vec2_t oldVel;
	float angle;

	VectorMA2(ps->velocity, accelspeed, wishdir, vel);
	Vector2Copy(ps->oldVelocity, oldVel);

	Vec2Normalize(oldVel);
	Vec2Normalize(vel);

	angle = Dot2Product(oldVel, vel);

	if ( angle >= inertiaAngle->current.decimal )
	{
		return false;
	}

	if ( inertiaDebug->current.boolean )
	{
		Com_Printf("angle is %f (oldVel is (%f,%f), vel is (%f, %f))\n", angle, oldVel[0], oldVel[1], vel[0], vel[1]);
		Com_Printf("clamping acceleration from %f to %f\n", accelspeed, inertiaMax->current.decimal);
	}

	return true;
}

/*
=============
PM_PlayerInertia
=============
*/
float PM_PlayerInertia( playerState_t *ps, float accelspeed, const vec3_t wishdir )
{
	if ( ps->pm_type == PM_NOCLIP )
	{
		return accelspeed;
	}

	if ( inertiaMax->current.decimal >= accelspeed )
	{
		return accelspeed;
	}

	if ( Vec2LengthSq(ps->oldVelocity) < 0.0001 )
	{
		return accelspeed;
	}

	if ( !PM_DoPlayerInertia(ps, accelspeed, wishdir) )
	{
		return accelspeed;
	}

	return inertiaMax->current.decimal;
}

/*
==============
PM_Accelerate

Handles user intended acceleration
==============
*/
void PM_Accelerate( playerState_t *ps, const pml_t *pml, vec3_t wishdir, float wishspeed, float accel )
{
	vec3_t pushDir;
	vec3_t wishVelocity;
	float pushLen;
	float canPush;
	float currentspeed;
	float accelspeed;
	float addspeed;
	float control;

	if ( ps->pm_flags & PMF_LADDER )
	{
		VectorScale(wishdir, wishspeed, wishVelocity);
		VectorSubtract(wishVelocity, ps->velocity, pushDir);
		pushLen = Vec3Normalize(pushDir);
		canPush = accel * pml->frametime * wishspeed;

		if ( canPush > pushLen )
		{
			canPush = pushLen;
		}

		VectorMA(ps->velocity, canPush, pushDir, ps->velocity);
		return;
	}

	currentspeed = DotProduct(ps->velocity, wishdir);
	addspeed = wishspeed - currentspeed;

	if ( addspeed <= 0 )
	{
		return;
	}

	if ( stopspeed->current.decimal > wishspeed )
		control = stopspeed->current.decimal;
	else
		control = wishspeed;

	accelspeed = accel * pml->frametime * control;

	if ( accelspeed > addspeed )
	{
		accelspeed = addspeed;
	}

	canPush = PM_PlayerInertia(ps, accelspeed, wishdir);
	VectorMA(ps->velocity, canPush, wishdir, ps->velocity);
}

/*
==============
PM_UFOMove
==============
*/
void PM_UFOMove( pmove_t *pm, pml_t *pml )
{
	vec3_t forward;
	vec3_t up;
	int i;
	playerState_t *ps;
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

	assert(pm);
	ps = pm->ps;
	assert(ps);

	ps->viewHeightTarget = DEFAULT_VIEWHEIGHT;

	fmove = pm->cmd.forwardmove;
	rmove = pm->cmd.rightmove;
	umove = 0;

	if ( pm->cmd.buttons & BUTTON_LEANRIGHT )
		umove += 127;

	if ( pm->cmd.buttons & BUTTON_LEANLEFT )
		umove -= 127;

	// friction

	if ( fmove == 0 && rmove == 0 && umove == 0 )
		speed = 0;
	else
		speed = VectorLength(ps->velocity);

	if ( speed < 1 )
	{
		VectorCopy(vec3_origin, ps->velocity);
	}
	else
	{
		drop = 0;
		curFriction = friction->current.decimal * 1.5; // extra friction

		if ( stopspeed->current.decimal > speed )
			control = stopspeed->current.decimal;
		else
			control = speed;

		drop += control * curFriction * pml->frametime;

		// scale the velocity
		newspeed = speed - drop;
		if ( newspeed < 0 )
			newspeed = 0;
		newspeed /= speed;

		VectorScale(ps->velocity, newspeed, ps->velocity);
	}

	// accelerate
	scale = PM_MoveScale(ps, fmove, rmove, umove);

	up[1] = 0;
	up[0] = 0;
	up[2] = 1;

	Vec3Cross(up, pml->right, forward);

	for ( i = 0; i < 3; i++ )
		wishvel[i] = forward[i] * fmove + pml->right[i] * rmove + up[i] * umove;

	VectorCopy(wishvel, wishdir);

	wishspeed = Vec3Normalize(wishdir);
	wishspeed *= scale;

	PM_Accelerate(ps, pml, wishdir, wishspeed, 9);

	// move
	VectorMA(ps->origin, pml->frametime, ps->velocity, ps->origin);
}

/*
===============
PM_NoclipMove
===============
*/
void PM_NoclipMove( pmove_t *pm, pml_t *pml )
{
	playerState_t *ps;
	float scale;
	float wishspeed;
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
	float speed;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	ps->viewHeightTarget = DEFAULT_VIEWHEIGHT;

	// friction

	speed = VectorLength(ps->velocity);

	if ( speed < 1 )
	{
		VectorCopy(vec3_origin, ps->velocity);
	}
	else
	{
		drop = 0;
		curFriction = friction->current.decimal * 1.5; // extra friction

		if ( stopspeed->current.decimal > speed )
			control = stopspeed->current.decimal;
		else
			control = speed;

		drop += control * curFriction * pml->frametime;

		// scale the velocity
		newspeed = speed - drop;
		if ( newspeed < 0 )
			newspeed = 0;
		newspeed /= speed;

		VectorScale(ps->velocity, newspeed, ps->velocity);
	}

	fmove = pm->cmd.forwardmove;
	rmove = pm->cmd.rightmove;
	umove = 0;

	if ( pm->cmd.buttons & BUTTON_LEANRIGHT )
		umove += 127;

	if ( pm->cmd.buttons & BUTTON_LEANLEFT )
		umove -= 127;

	// accelerate
	scale = PM_MoveScale(ps, fmove, rmove, umove);

	for ( i = 0; i < 3; i++ )
		wishvel[i] = pml->forward[i] * fmove + pml->right[i] * rmove + pml->up[i] * umove;

	VectorCopy(wishvel, wishdir);

	wishspeed = Vec3Normalize(wishdir);
	wishspeed *= scale;

	PM_Accelerate(ps, pml, wishdir, wishspeed, 9);

	// move
	VectorMA(ps->origin, pml->frametime, ps->velocity, ps->origin);
}

/*
===================
PM_AirMove
===================
*/
void PM_AirMove( pmove_t *pm, pml_t *pml )
{
	playerState_t *ps;
	usercmd_t cmd;
	float scale;
	float wishspeed;
	vec3_t wishdir;
	float rmove;
	float fmove;
	vec3_t wishvel;
	int i;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	PM_Friction(pm->ps, pml);

	fmove = pm->cmd.forwardmove;
	rmove = pm->cmd.rightmove;

	cmd = pm->cmd;

	scale = PM_CmdScale(ps, &cmd);

	pml->forward[2] = 0;
	pml->right[2] = 0;

	Vec3Normalize(pml->forward);
	Vec3Normalize(pml->right);

	for ( i = 0; i < 2; i++ )
		wishvel[i] = pml->forward[i] * fmove + pml->right[i] * rmove;
	wishvel[2] = 0;

	VectorCopy(wishvel, wishdir);

	wishspeed = Vec3Normalize(wishdir);
	wishspeed *= scale;

	// not on ground, so little effect on velocity
	PM_Accelerate(ps, pml, wishdir, wishspeed, 1);

	// we may have a ground plane that is very steep, even
	// though we don't have a groundentity
	// slide along the steep plane
	if ( pml->groundPlane )
	{
		PM_ClipVelocity(ps->velocity, pml->groundTrace.normal, ps->velocity);
	}

	PM_StepSlideMove(pm, pml, qtrue);

	// Ridah, moved this down, so we use the actual movement direction
	// set the movementDir so clients can rotate the legs for strafing
	PM_SetMovementDir(pm, pml);
}

/*
===================
PM_FlyMove
===================
*/
void PM_FlyMove( pmove_t *pm, pml_t *pml )
{
	playerState_t *ps;
	float scale;
	vec3_t wishdir;
	float wishspeed;
	vec3_t wishvel;
	int i;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	// normal slowdown
	PM_Friction(pm->ps, pml);

	scale = PM_CmdScale(ps, &pm->cmd);

	//
	// user intentions
	//
	if ( !scale )
	{
		wishvel[0] = 0;
		wishvel[1] = 0;
		wishvel[2] = 0;
	}
	else
	{
		for ( i = 0; i < 3; ++i )
		{
			wishvel[i] = scale * pml->forward[i] * pm->cmd.forwardmove + scale * pml->right[i] * pm->cmd.rightmove;
		}
	}

	if ( ps->speed )
	{
		scale = PM_MoveScale(ps, 0, 0, 127);

		if ( pm->cmd.buttons & BUTTON_LEANLEFT )
			wishvel[2] -= scale * 127;

		if ( pm->cmd.buttons & BUTTON_LEANRIGHT )
			wishvel[2] += scale * 127;
	}

	VectorCopy(wishvel, wishdir);
	wishspeed = Vec3Normalize(wishdir);

	PM_Accelerate(ps, pml, wishdir, wishspeed, 8);

	PM_StepSlideMove(pm, pml, qfalse);
}

/*
============
PM_LadderMove
============
*/
void PM_LadderMove( pmove_t *pm, pml_t *pml )
{
	playerState_t *ps;
	int moveyaw;
	float fSpeedDrop;
	float fSideSpeed;
	vec2_t vSideDir;
	float upscale;
	vec3_t vTempRight;
	vec3_t wishvel;
	vec3_t wishdir;
	float scale;
	float wishspeed;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	if ( Jump_Check(pm, pml) )
	{
		// jumped away
		PM_AirMove(pm, pml);
		return;
	}

	upscale = ( pml->forward[2] + 0.25 ) * 2.5;
	if ( upscale > 1.0 )
	{
		upscale = 1.0;
	}
	else if ( upscale < -1.0 )
	{
		upscale = -1.0;
	}

	// forward/right should be horizontal only
	pml->forward[2] = 0;
	Vec3Normalize(pml->forward);
	pml->right[2] = 0;
	Vec3NormalizeTo(pml->right, vTempRight);

	ProjectPointOnPlane(vTempRight, ps->vLadderVec, pml->right);

	// move depending on the view, if view is straight forward, then go up
	// if view is down more then X degrees, start going down
	// if they are back pedalling, then go in reverse of above
	scale = PM_CmdScale(ps, &pm->cmd);
	VectorClear(wishvel);

	if ( pm->cmd.forwardmove )
	{
		wishvel[2] = upscale * 0.5 * scale * pm->cmd.forwardmove;
	}

	if ( pm->cmd.rightmove )
	{
		VectorMA(wishvel, scale * 0.2 * pm->cmd.rightmove, pml->right, wishvel);
	}

	wishspeed = Vec3NormalizeTo(wishvel, wishdir);
	PM_Accelerate(ps, pml, wishdir, wishspeed, 9);

	if ( !pm->cmd.forwardmove )
	{
		if ( ps->velocity[2] > 0 )
		{
			ps->velocity[2] = ps->velocity[2] - ps->gravity * pml->frametime;

			if ( ps->velocity[2] < 0 )
			{
				ps->velocity[2] = 0;
			}
		}
		else
		{
			ps->velocity[2] = ps->gravity * pml->frametime + ps->velocity[2];

			if ( ps->velocity[2] > 0 )
			{
				ps->velocity[2] = 0;
			}
		}
	}

	if ( !pm->cmd.rightmove )
	{
		Vector2Copy(pml->right, vSideDir);
		Vec2Normalize(vSideDir);

		fSideSpeed = Dot2Product(vSideDir, ps->velocity);

		if ( fSideSpeed != 0 )
		{
			VectorMA2(ps->velocity, -fSideSpeed, vSideDir, ps->velocity);
			fSpeedDrop = fSideSpeed * pml->frametime * 16;

			if ( I_fabs(fSideSpeed) > I_fabs(fSpeedDrop) )
			{
				if ( I_fabs(fSpeedDrop) < 1 )
				{
					fSpeedDrop = FloatSign(fSpeedDrop);
				}

				VectorMA2(ps->velocity, fSideSpeed - fSpeedDrop, vSideDir, ps->velocity);
			}
		}
	}

	if ( !pml->walking )
	{
		VectorMA2(ps->velocity, -Dot2Product(ps->vLadderVec, ps->velocity), ps->vLadderVec, ps->velocity);

		if ( Square(ps->velocity[2]) >= Vec2LengthSq(ps->velocity) )
		{
			VectorMA2(ps->velocity, -50, ps->vLadderVec, ps->velocity);
		}
	}

	PM_StepSlideMove(pm, pml, qfalse);  // no gravity while going up ladder

	scale = vectoyaw(ps->vLadderVec) + 180;
	moveyaw = (int)AngleDelta(scale, ps->viewangles[YAW]);

	if ( abs( moveyaw ) > 75 )
	{
		if ( moveyaw > 0 )
		{
			moveyaw = 75;
		}
		else
		{
			moveyaw = -75;
		}
	}

	// always point legs forward
	ps->movementDir = (signed char)moveyaw;
}

/*
===================
PM_WalkMove
===================
*/
void PM_WalkMove( pmove_t *pm, pml_t *pml )
{
	playerState_t *ps;
	int iStance;
	float vel;
	float acceleration;
	usercmd_t cmd;
	float scale;
	float wishspeed;
	vec3_t wishdir;
	float rmove;
	float fmove;
	vec3_t wishvel;
	vec3_t dir;
	int i;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	if ( pm->ps->pm_flags & PMF_TIME_LAND )
	{
		Jump_ApplySlowdown(ps);
	}

	if ( Jump_Check(pm, pml) )
	{
		// jumped away
		PM_AirMove(pm, pml);
		return;
	}

	PM_Friction(ps, pml);

	fmove = pm->cmd.forwardmove;
	rmove = pm->cmd.rightmove;

	cmd = pm->cmd;
	scale = PM_CmdScale_Walk(pm, &cmd);

	scale *= PM_DamageScale_Walk(ps->damageTimer);
	ps->damageTimer -= (int)(pml->frametime * 1000.0);

	if ( ps->damageTimer <= 0 )
	{
		ps->damageTimer = 0;
	}

	// project moves down to flat plane
	pml->forward[2] = 0;
	pml->right[2] = 0;

	// project the forward and right directions onto the ground plane
	PM_ClipVelocity(pml->forward, pml->groundTrace.normal, pml->forward);
	PM_ClipVelocity(pml->right, pml->groundTrace.normal, pml->right);
	//
	Vec3Normalize(pml->forward);
	Vec3Normalize(pml->right);

	for ( i = 0; i < 3; i++ )
		dir[i] = pml->forward[i] * fmove + pml->right[i] * rmove;
	// when going up or down slopes the wish velocity should Not be zero
	//	wishvel[2] = 0;

	VectorCopy(dir, wishdir);
	wishspeed = Vec3Normalize(wishdir);
	wishspeed *= scale;

	iStance = PM_GetEffectiveStance(ps);

	// when a player gets hit, they temporarily lose
	// full control, which allows them to be moved a bit
	if ( pml->groundTrace.surfaceFlags & SURF_SLICK || ps->pm_flags & PMF_TIME_KNOCKBACK )
	{
		acceleration = 1;
	}
	else if ( iStance == PM_EFF_STANCE_PRONE )
	{
		acceleration = 19;
	}
	else if ( iStance == PM_EFF_STANCE_CROUCH )
	{
		acceleration = 12;
	}
	else
	{
		acceleration = 9;
	}

	if ( ps->pm_flags & PMF_TIME_SLIDE )
	{
		acceleration *= 0.25;
	}

	PM_Accelerate(ps, pml, wishdir, wishspeed, acceleration);

	//Com_Printf("velocity = %1.1f %1.1f %1.1f\n", pm->ps->velocity[0], pm->ps->velocity[1], pm->ps->velocity[2]);
	//Com_Printf("velocity1 = %1.1f\n", VectorLength(pm->ps->velocity));

	if ( pml->groundTrace.surfaceFlags & SURF_SLICK || ps->pm_flags & PMF_TIME_KNOCKBACK )
	{
		ps->velocity[2] -= ps->gravity * pml->frametime;
	}
	else
	{
		// don't reset the z velocity for slopes
		//pm->ps->velocity[2] = 0;
	}

	vel = VectorLength(ps->velocity);

	VectorCopy(ps->velocity, wishvel);
	// slide along the ground plane
	PM_ClipVelocity(ps->velocity, pml->groundTrace.normal, ps->velocity);

	if ( DotProduct(ps->velocity, wishvel) > 0 )
	{
		Vec3Normalize(ps->velocity);
		VectorScale(ps->velocity, vel, ps->velocity);
	}

	// don't do anything if standing still
	if ( ps->velocity[0] || ps->velocity[1] )
	{
		PM_StepSlideMove(pm, pml, qfalse);
	}

	PM_SetMovementDir(pm, pml);
}

/*
================
PmoveSingle
================
*/
void PmoveSingle( pmove_t *pm )
{
	vec3_t move;
	pml_t pml;
	playerState_t *ps;
	vec2_t oldVel;
	int iStance;

	ps = pm->ps;
	assert(ps);

	// RF, update conditional values for anim system
	BG_AnimUpdatePlayerStateConditions(pm);

	if ( ps->pm_flags & PMF_UNKNOWN_8000 )
	{
		pm->cmd.buttons &= (BUTTON_PRONE | BUTTON_CROUCH | BUTTON_CANNOT_PRONE);
		pm->cmd.forwardmove = 0;
		pm->cmd.rightmove = 0;

		VectorSet(ps->velocity, 0, 0, 0);
	}

	// if talk button is down, dissallow all other input
	// this is to prevent any possible intercept proxy from
	// adding fake talk balloons
	if ( pm->cmd.buttons & BUTTON_TALK )
	{
		pm->cmd.buttons &= (BUTTON_PRONE | BUTTON_CROUCH | BUTTON_ADS | BUTTON_CANNOT_PRONE | BUTTON_TALK);
		pm->cmd.forwardmove = 0;
		pm->cmd.rightmove = 0;
	}

	ps->pm_flags &= ~PMF_PRONE_BLOCKED;

	if ( ps->pm_type >= PM_DEAD )
	{
		pm->tracemask &= ~CONTENTS_BODY; // corpses can fly through bodies
	}

	// make sure walking button is clear if they are running, to avoid
	// proxy no-footsteps cheats
	if ( ps->pm_flags & PMF_PRONE )
	{
		if ( ( pm->cmd.forwardmove == pm->oldcmd.forwardmove || I_fabs(pm->cmd.forwardmove) <= I_fabs(pm->oldcmd.forwardmove) )
		        && ( pm->cmd.rightmove == pm->oldcmd.rightmove || I_fabs(pm->cmd.rightmove) <= I_fabs(pm->oldcmd.rightmove) ) )
		{
			if ( !(ps->pm_flags & PMF_ADS) && (ps->weaponstate < WEAPON_FIRING || ps->weaponstate == WEAPON_RELOADING) )
			{
				ps->pm_flags &= ~PMF_ADS_OVERRIDE;
			}
		}
		else if ( PM_InteruptWeaponWithProneMove(ps) )
		{
			ps->pm_flags &= ~PMF_ADS_OVERRIDE;
			PM_ExitAimDownSight(ps);
		}
	}
	else
	{
		ps->pm_flags &= ~PMF_ADS_OVERRIDE;
	}

	iStance = PM_GetEffectiveStance(ps);

	if ( ps->pm_flags & PMF_ADS && iStance == PM_EFF_STANCE_PRONE )
	{
		pm->cmd.forwardmove = 0;
		pm->cmd.rightmove = 0;
	}

	// set the talk balloon flag
	if ( pm->cmd.buttons & BUTTON_TALK)
		ps->eFlags |= EF_TALK;
	else
		ps->eFlags &= ~EF_TALK;

	// set the firing flag for continuous beam weapons
	ps->eFlags &= ~EF_FIRING;

	if ( ( ps->pm_type != PM_INTERMISSION ) && !( ps->pm_flags & PMF_RESPAWNED ) )
	{
		if ( ps->weaponstate == WEAPON_READY || ps->weaponstate == WEAPON_FIRING )
		{
			// check for ammo
			if ( PM_WeaponAmmoAvailable( ps ) )
			{
				// all clear, fire!
				if ( pm->cmd.buttons & BUTTON_ATTACK )
				{
					ps->eFlags |= EF_FIRING;
				}
			}
		}
	}

	// clear the respawned flag if attack and use are cleared
	if ( ps->pm_type < PM_DEAD
	        && !(pm->cmd.buttons & (BUTTON_BINOCULARS | BUTTON_ATTACK)) )
	{
		ps->pm_flags &= ~PMF_RESPAWNED;
	}

	// clear all pmove local vars
	memset(&pml, 0, sizeof(pml));

	// determine the time
	pml.msec = pm->cmd.serverTime - ps->commandTime;
	if ( pml.msec < 1 )
	{
		pml.msec = 1;
	}
	else if ( pml.msec > 200 )
	{
		pml.msec = 200;
	}
	ps->commandTime = pm->cmd.serverTime;

	// save old org in case we get stuck
	VectorCopy(ps->origin, pml.previous_origin);

	// save old velocity for crashlanding
	VectorCopy(ps->velocity, pml.previous_velocity);

	pml.frametime = pml.msec * 0.001;

	PM_AdjustAimSpreadScale(pm, &pml);

	// update the viewangles
	PM_UpdateViewAngles(ps, pml.msec, &pm->cmd, pm->handler);
	AngleVectors(ps->viewangles, pml.forward, pml.right, pml.up);

	// decide if backpedaling animations should be used
	if ( pm->cmd.forwardmove < 0 )
	{
		ps->pm_flags |= PMF_BACKWARDS_RUN;
	}
	else
	{
		if ( pm->cmd.forwardmove > 0 || !pm->cmd.forwardmove && pm->cmd.rightmove )
		{
			ps->pm_flags &= ~PMF_BACKWARDS_RUN;
		}
	}

	if ( ps->pm_type >= PM_DEAD ) // DHM - Nerve
	{
		pm->cmd.forwardmove = 0;
		pm->cmd.rightmove = 0;
	}

	if ( iStance == PM_EFF_STANCE_PRONE && ps->pm_flags & PMF_ADS_OVERRIDE )
	{
		pm->cmd.forwardmove = 0;
		pm->cmd.rightmove = 0;
	}

	Mantle_ClearHint(ps);

	switch ( ps->pm_type )
	{
	case PM_NORMAL_LINKED:
	case PM_DEAD_LINKED:
		PM_ClearLadderFlag(ps);
		ps->groundEntityNum = ENTITYNUM_NONE;
		pml.walking = qfalse;
		pml.groundPlane = qfalse;
		pml.almostGroundPlane = qfalse;
		VectorClear(ps->velocity);
		PM_UpdateAimDownSightFlag(pm, &pml);
		PM_UpdatePlayerWalkingFlag(pm);
		PM_CheckDuck(pm, &pml);
		PM_DropTimers(ps, &pml);
		PM_Footsteps(pm, &pml);
		PM_Weapon(pm, &pml);
		return;

	case PM_NOCLIP:
		PM_ClearLadderFlag(ps);
		PM_UpdateAimDownSightFlag(pm, &pml);
		PM_UpdatePlayerWalkingFlag(pm);
		PM_NoclipMove(pm, &pml);
		PM_DropTimers(ps, &pml);
		PM_UpdateAimDownSightLerp(pm, &pml);
		return;

	case PM_UFO:
		PM_ClearLadderFlag(ps);
		PM_UpdateAimDownSightFlag(pm, &pml);
		PM_UpdatePlayerWalkingFlag(pm);
		PM_UFOMove(pm, &pml);
		PM_DropTimers(ps, &pml);
		PM_UpdateAimDownSightLerp(pm, &pml);
		return;

	case PM_SPECTATOR:
		PM_ClearLadderFlag(ps);
		PM_UpdateAimDownSightFlag(pm, &pml);
		PM_UpdatePlayerWalkingFlag(pm);
		PM_CheckDuck(pm, &pml);
		PM_FlyMove(pm, &pml);
		PM_DropTimers(ps, &pml);
		PM_UpdateAimDownSightLerp(pm, &pml);
		return;

	case PM_INTERMISSION:
		PM_ClearLadderFlag(ps);
		PM_UpdateAimDownSightFlag(pm, &pml);
		PM_UpdateAimDownSightLerp(pm, &pml);
		return;

	default:
		if ( ps->eFlags & EF_TURRET_ACTIVE )
		{
			PM_ClearLadderFlag(ps);
			ps->groundEntityNum = ENTITYNUM_NONE;
			pml.walking = qfalse;
			pml.groundPlane = qfalse;
			pml.almostGroundPlane = qfalse;
			VectorClear(ps->velocity);
			PM_UpdateAimDownSightFlag(pm, &pml);
			PM_UpdatePlayerWalkingFlag(pm);
			PM_CheckDuck(pm, &pml);
			PM_DropTimers(ps, &pml);
			PM_Footsteps(pm, &pml);
			PM_ResetWeaponState(ps);
			return;
		}

		if ( !(ps->pm_flags & PMF_MANTLE) )
		{
			PM_CheckDuck(pm, &pml);
			PM_GroundTrace(pm, &pml);
		}

		Mantle_Check(pm, &pml);

		if ( ps->pm_flags & PMF_MANTLE)
		{
			PM_ClearLadderFlag(ps);
			ps->groundEntityNum = ENTITYNUM_NONE;
			pml.groundPlane = qfalse;
			pml.walking = qfalse;
			PM_UpdateAimDownSightFlag(pm, &pml);
			PM_UpdatePlayerWalkingFlag(pm);
			PM_CheckDuck(pm, &pml);
			Mantle_Move(pm, ps, &pml);
			PM_Weapon(pm, &pml);
			return;
		}

		PM_UpdateAimDownSightFlag(pm, &pml);
		PM_UpdatePlayerWalkingFlag(pm);
		PM_UpdatePronePitch(pm, &pml);

		if ( ps->pm_type == PM_DEAD )
		{
			PM_DeadMove(ps, &pml);
		}

		PM_CheckLadderMove(pm, &pml);
		PM_DropTimers(ps, &pml);

		if ( ps->pm_flags & PMF_LADDER )
		{
			PM_LadderMove(pm, &pml);
		}
		else if ( pml.walking )
		{
			PM_WalkMove(pm, &pml);
		}
		else
		{
			PM_AirMove(pm, &pml);
		}

		PM_GroundTrace(pm, &pml);
		PM_Footsteps(pm, &pml);
		PM_Weapon(pm, &pml);
		PM_FoliageSounds(pm);

		VectorSubtract(ps->origin, pml.previous_origin, move);

		if ( VectorLengthSquared(ps->velocity) * 0.25 > VectorLengthSquared(move) / (pml.frametime * pml.frametime) )
		{
			VectorScale(move, 1.0 / pml.frametime, ps->velocity);
		}

		Vector2Subtract(ps->velocity, ps->oldVelocity, oldVel);
		Vec2Scale(oldVel, I_fmin(pml.frametime, 1.0), oldVel);
		Vector2Add(ps->oldVelocity, oldVel, ps->oldVelocity);

		// snap some parts of playerstate to save network bandwidth
		Sys_SnapVector(ps->velocity);
	}
}