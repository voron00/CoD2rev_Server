#include "../qcommon/qcommon.h"
#include "bg_public.h"

/*

input: origin, velocity, bounds, groundPlane, trace function

output: origin, velocity, impacts, stairup boolean

*/

#define MAX_CLIP_PLANES 8

/*
==================
PM_StepSlideMove
==================
*/
void PM_StepSlideMove( pmove_t *pm, pml_t *pml, qboolean gravity )
{
	int old;
	int iDelta;
	playerState_t *ps;
	qboolean jumping;
	vec3_t endpos;
	int bHadGround;
	float fStepAmount;
	int iBumps;
	float fStepSize;
	qboolean bCloser;
	vec2_t vel;
	vec2_t flatDelta;
	vec3_t down;
	vec3_t up;
	trace_t trace;
	vec3_t down_v;
	vec3_t down_o;
	vec3_t start_v;
	vec3_t start_o;

	fStepAmount = 0;

	ps = pm->ps;
	assert(ps);

	jumping = qfalse;

	if ( pm->ps->pm_flags & PMF_LADDER )
	{
		bHadGround = qfalse;
		Jump_ClearState(ps);
	}
	else if ( pml->groundPlane )
	{
		bHadGround = qtrue;
	}
	else
	{
		bHadGround = qfalse;

		if ( ps->pm_flags & PMF_TIME_LAND && ps->pm_time )
		{
			Jump_ClearState(ps);
		}
	}

	// try slidemove from this position
	VectorCopy(ps->origin, start_o);
	VectorCopy(ps->velocity, start_v);

	iBumps = PM_SlideMove(pm, pml, gravity);

	if ( ps->pm_flags & PMF_PRONE )
		fStepSize = STEPSIZE_PRONE;
	else
		fStepSize = STEPSIZE;

	if ( ps->groundEntityNum == ENTITYNUM_NONE )
	{
		if ( ps->pm_flags & PMF_TIME_LAND && ps->pm_time )
		{
			Jump_ClearState(ps);
		}

		if ( iBumps && ps->pm_flags & PMF_TIME_LAND )
		{
			if ( Jump_GetStepHeight(ps, start_o, &fStepSize) )
			{
				if ( fStepSize < 1 )
				{
					return;
				}

				jumping = qtrue;
			}
		}

		// never step up when you still have up velocity
		if ( !jumping && !( ps->pm_flags & PMF_LADDER && ps->velocity[2] > 0 ) )
		{
			return;
		}
	}

	// push down the final amount
	VectorCopy(ps->origin, down_o);
	VectorCopy(ps->velocity, down_v);

	Vector2Subtract(down_o, start_o, flatDelta);

	if ( iBumps )
	{
		VectorCopy(start_o, up);
		up[2] += fStepSize + 1.0;

		PM_playerTrace(pm, &trace, start_o, pm->mins, pm->maxs, up, ps->clientNum, pm->tracemask);

		fStepAmount = (fStepSize + 1.0) * trace.fraction - 1.0;

		if ( fStepAmount < 1 )
		{
			fStepAmount = 0;
		}
		else
		{
			VectorSet(ps->origin, up[0], up[1], start_o[2] + fStepAmount);
			VectorCopy(start_v, ps->velocity);

			PM_SlideMove(pm, pml, gravity);
		}
	}

	if ( bHadGround || fStepAmount != 0 )
	{
		VectorCopy(ps->origin, down);
		down[2] -= fStepAmount;

		if ( bHadGround )
		{
			down[2] -= 9.0;
		}

		PM_playerTrace(pm, &trace, ps->origin, pm->mins, pm->maxs, down, ps->clientNum, pm->tracemask);

		if ( trace.entityNum < MAX_CLIENTS )
		{
			VectorCopy(down_o, ps->origin);
			VectorCopy(down_v, ps->velocity);
			return;
		}

		if ( trace.fraction < 1 )
		{
			if ( trace.normal[2] < 0.30000001 )
			{
				VectorCopy(down_o, ps->origin);
				VectorCopy(down_v, ps->velocity);
				return;
			}

			Vec3Lerp(ps->origin, down, trace.fraction, ps->origin);
#ifdef LIBCOD
			PM_ProjectVelocity(ps->velocity, trace.normal, ps->velocity); // bounce jump
#else
			PM_ClipVelocity(ps->velocity, trace.normal, ps->velocity);
#endif
		}
		else
		{
			if ( fStepAmount != 0 )
			{
				ps->origin[2] = ps->origin[2] - fStepAmount;
			}
		}
	}

	Vector2Subtract(ps->origin, start_o, vel);
	bCloser = Dot2Product(flatDelta, ps->velocity) + 0.001 >= Dot2Product(vel, ps->velocity);

	if ( bCloser || jumping && Jump_IsPlayerAboveMax(ps) )
	{
		VectorCopy(down_o, ps->origin);
		VectorCopy(down_v, ps->velocity);

		fStepAmount = 0;

		if ( bHadGround )
		{
			VectorCopy(ps->origin, down);
			down[2] -= 9.0;

			// if the down trace can trace back to the original position directly, don't step
			PM_playerTrace(pm, &trace, ps->origin, pm->mins, pm->maxs, down, ps->clientNum, pm->tracemask);

			if ( trace.fraction < 1.0 )
			{
				Vec3Lerp(ps->origin, down, trace.fraction, endpos);
				fStepAmount = endpos[2] - ps->origin[2];
				VectorCopy(endpos, ps->origin);
				PM_ClipVelocity(ps->velocity, trace.normal, ps->velocity);
			}
		}
	}

	if ( jumping )
	{
		Jump_ClampVelocity(ps, down_o);
	}

	if ( !bHadGround )
	{
		return;
	}

	if ( ps->pm_type >= PM_DEAD )
	{
		return;
	}

	if ( !PM_VerifyPronePosition(pm, start_o, start_v) )
	{
		return;
	}

	if ( I_fabs(ps->origin[2] - down_o[2]) <= 0.5 )
	{
		return;
	}

	// use the step move
	iDelta = Q_rint(ps->origin[2] - down_o[2]);

	if ( !iDelta )
	{
		return;
	}

	if ( iDelta < -16 )
	{
		iDelta = -16;
	}
	else if ( iDelta > 24 )
	{
		iDelta = 24;
	}

	iDelta += 128;
	BG_AddPredictableEventToPlayerstate(EV_STEP_VIEW, iDelta, ps);
	VectorScale(ps->velocity, (1.0 - I_fabs(ps->origin[2] - start_o[2]) / fStepSize) * 0.80000001 + 0.19999999, ps->velocity);
	iDelta -= 128;

	if ( abs(iDelta) < 4 )
	{
		return;
	}

	if ( ps->groundEntityNum == ENTITYNUM_NONE )
	{
		return;
	}

	if ( !PM_ShouldMakeFootsteps(pm) )
	{
		return;
	}

	iDelta = abs(iDelta) / 2;

	if ( iDelta > 4 )
	{
		iDelta = 4;
	}

	old = ps->bobCycle;
	ps->bobCycle = (int)( old + (iDelta * 1.25 + 7.0) ) & 255;
	PM_FootstepEvent(pm, pml, old, ps->bobCycle, qtrue);
	//Com_Printf( "%i:stepped\n", iDelta );
}

/*
==================
PM_PermuteRestrictiveClipPlanes
==================
*/
static float PM_PermuteRestrictiveClipPlanes( const vec3_t velocity, int planeCount, const vec3_t *planes, int *permutation )
{
	int permutedIndex;
	float parallel[MAX_CLIP_PLANES];
	int planeIndex;

	assert(velocity);
	assert(planeCount > 0 && planeCount <= MAX_CLIP_PLANES);
	assert(planes);
	assert(permutation);

	for ( planeIndex = 0; planeIndex < planeCount; planeIndex++ )
	{
		parallel[planeIndex] = DotProduct(velocity, planes[planeIndex]);

		for ( permutedIndex = planeIndex; permutedIndex && parallel[planeIndex] <= parallel[permutation[permutedIndex - 1]]; permutedIndex-- )
		{
			permutation[permutedIndex] = permutation[permutedIndex - 1];
		}

		permutation[permutedIndex] = planeIndex;
	}

	return parallel[*permutation];
}

/*
==================
PM_SlideMove
==================
*/
qboolean PM_SlideMove( pmove_t *pm, pml_t *pml, qboolean gravity )
{
	int bumpcount, numbumps;
	vec3_t dir;
	float d;
	int numplanes;
	vec3_t planes[MAX_CLIP_PLANES];
	vec3_t primal_velocity;
	vec3_t clipVelocity;
	int i, j, k;
	trace_t trace;
	vec3_t end;
	float time_left;
	float into;
	vec3_t endVelocity;
	vec3_t endClipVelocity;
	playerState_t *ps;
	int permutation[MAX_CLIP_PLANES];

	ps = pm->ps;
	assert(ps);

	numbumps = 4;

	VectorCopy(ps->velocity, primal_velocity);
	VectorCopy(ps->velocity, endVelocity);

	if ( gravity )
	{
		endVelocity[2] = endVelocity[2] - ps->gravity * pml->frametime;
		ps->velocity[2] = (ps->velocity[2] + endVelocity[2]) * 0.5;
		primal_velocity[2] = endVelocity[2];

		if ( pml->groundPlane )
		{
			// slide along the ground plane
			PM_ClipVelocity(ps->velocity, pml->groundTrace.normal, ps->velocity);
		}
	}

	time_left = pml->frametime;

	// never turn against the ground plane
	if ( pml->groundPlane )
	{
		VectorCopy(pml->groundTrace.normal, planes[0]);
		numplanes = 1;
	}
	else
	{
		numplanes = 0;
	}

	// never turn against original velocity
	Vec3NormalizeTo(ps->velocity, planes[numplanes]);
	numplanes++;

	for ( bumpcount = 0; bumpcount < numbumps; bumpcount++ )
	{
		// calculate position we are trying to move to
		VectorMA(ps->origin, time_left, ps->velocity, end);

		// see if we can make it there
		PM_playerTrace(pm, &trace, ps->origin, pm->mins, pm->maxs, end, ps->clientNum, pm->tracemask);

		if ( trace.allsolid )
		{
			// entity is completely trapped in another solid
			ps->velocity[2] = 0;
			return qtrue;
		}

		if ( trace.fraction > 0 )
		{
			// actually covered some distance
			Vec3Lerp(ps->origin, end, trace.fraction, ps->origin);
		}

		if ( trace.fraction == 1 )
		{
			break;  // moved the entire distance
		}

		// save entity for contact
		PM_AddTouchEnt(pm, trace.entityNum);

		time_left -= time_left * trace.fraction;

		if ( numplanes >= MAX_CLIP_PLANES )
		{
			// this shouldn't really happen
			VectorClear(ps->velocity);
			return qtrue;
		}

		//
		// if this is the same plane we hit before, nudge velocity
		// out along it, which fixes some epsilon issues with
		// non-axial planes
		//
		for ( i = 0; i < numplanes; i++ )
		{
			if ( DotProduct(trace.normal, planes[i]) > 0.99900001 )
			{
				// zinx - if it happens again, nudge the origin instead,
				// and trace it, to make sure we don't end up in a solid
				PM_ClipVelocity(ps->velocity, trace.normal, ps->velocity);
				VectorAdd(trace.normal, ps->velocity, ps->velocity);
				break;
			}
		}

		if ( i < numplanes )
		{
			continue;
		}
		VectorCopy(trace.normal, planes[numplanes]);
		numplanes++;

		//
		// modify velocity so it parallels all of the clip planes
		//

		// find a plane that it enters
		into = PM_PermuteRestrictiveClipPlanes(ps->velocity, numplanes, planes, permutation);

		if ( into >= 0.1 )
		{
			continue;       // move doesn't interact with the plane
		}

		// see how hard we are hitting things
		if ( -into > pml->impactSpeed )
		{
			pml->impactSpeed = -into;
		}

		// slide along the plane
		PM_ClipVelocity(ps->velocity, planes[permutation[0]], clipVelocity);

		// slide along the plane
		PM_ClipVelocity(endVelocity, planes[permutation[0]], endClipVelocity);

		for ( j = 1; j < numplanes; j++ )
		{
			if ( DotProduct( clipVelocity, planes[permutation[j]] ) >= 0.1 )
			{
				continue;       // move doesn't interact with the plane
			}

			// try clipping the move to the plane
			PM_ClipVelocity(clipVelocity, planes[permutation[j]], clipVelocity);
			PM_ClipVelocity(endClipVelocity, planes[permutation[j]], endClipVelocity);

			// see if it goes back into the first clip plane
			if ( DotProduct( clipVelocity, planes[permutation[0]] ) >= 0 )
			{
				continue;
			}

			// slide the original velocity along the crease
			Vec3Cross(planes[permutation[0]], planes[permutation[j]], dir);
			Vec3Normalize(dir);
			d = DotProduct(dir, ps->velocity);
			VectorScale(dir, d, clipVelocity);

			d = DotProduct(dir, endVelocity);
			VectorScale(dir, d, endClipVelocity);

			// see if there is a third plane the the new move enters
			for ( k = 1; k < numplanes; k++ )
			{
				if ( k == j )
				{
					continue;
				}

				if ( DotProduct( clipVelocity, planes[permutation[k]] ) >= 0.1 )
				{
					continue;       // move doesn't interact with the plane
				}

				// stop dead at a tripple plane interaction
				VectorClear(ps->velocity);
				return qtrue;
			}
		}

		// if we have fixed all interactions, try another move
		VectorCopy(clipVelocity, ps->velocity);
		VectorCopy(endClipVelocity, endVelocity);
	}

	if ( gravity )
	{
		VectorCopy(endVelocity, ps->velocity);
	}

	// don't change velocity if in a timer (FIXME: is this correct?)
	if ( ps->pm_time )
	{
		VectorCopy(primal_velocity, ps->velocity);
	}

	return ( bumpcount != 0 );
}

/*
==================
PM_VerifyPronePosition
==================
*/
qboolean PM_VerifyPronePosition( pmove_t *pm, const vec3_t vFallbackOrg, const vec3_t vFallbackVel )
{
	qboolean bProneOK;
	playerState_t *ps;

	ps = pm->ps;
	assert(ps);

	if ( !(pm->ps->pm_flags & PMF_PRONE) )
	{
		return qtrue;
	}

	bProneOK = BG_CheckProne( ps->clientNum, ps->origin, ps->maxs[0], 30.0,
	                          ps->proneDirection, &ps->fTorsoHeight, &ps->fTorsoPitch, &ps->fWaistPitch,
	                          qtrue, qtrue, NULL, pm->handler, PCT_CLIENT, 66.0 );

	if ( !bProneOK )
	{
		VectorCopy(vFallbackOrg, ps->origin);
		VectorCopy(vFallbackVel, ps->velocity);
	}

	return bProneOK;
}
