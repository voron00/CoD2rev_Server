#include "../qcommon/qcommon.h"
#include "bg_public.h"

int PM_VerifyPronePosition(pmove_t *pm, float *vFallbackOrg, float *vFallbackVel)
{
	int bProneOK;
	playerState_s *ps;

	ps = pm->ps;

	if ( (pm->ps->pm_flags & 1) == 0 )
		return 1;

	bProneOK = BG_CheckProne(
	               ps->clientNum,
	               ps->origin,
	               ps->maxs[0],
	               30.0,
	               ps->proneDirection,
	               &ps->fTorsoHeight,
	               &ps->fTorsoPitch,
	               &ps->fWaistPitch,
	               1,
	               1,
	               0,
	               pm->handler,
	               0,
	               66.0);

	if ( !bProneOK )
	{
		VectorCopy(vFallbackOrg, ps->origin);
		VectorCopy(vFallbackVel, ps->velocity);
	}

	return bProneOK;
}

float PM_PermuteRestrictiveClipPlanes(const float *velocity, int planeCount, const float (*planes)[3], int *permutation)
{
	int permutedIndex;
	float parallel[8];
	int planeIndex;

	for ( planeIndex = 0; planeIndex < planeCount; ++planeIndex )
	{
		parallel[planeIndex] = (float)((float)(*velocity * (float)(*planes)[3 * planeIndex])
		                               + (float)(velocity[1] * (float)(*planes)[3 * planeIndex + 1]))
		                       + (float)(velocity[2] * (float)(*planes)[3 * planeIndex + 2]);
		for ( permutedIndex = planeIndex;
		        permutedIndex && parallel[planeIndex] <= parallel[permutation[permutedIndex - 1]];
		        --permutedIndex )
		{
			permutation[permutedIndex] = permutation[permutedIndex - 1];
		}

		permutation[permutedIndex] = planeIndex;
	}

	return parallel[*permutation];
}

int PM_SlideMove(pmove_t *pm, pml_t *pml, int gravity)
{
	int j;
	float dir[3];
	float d;
	int numbumps;
	float endClipVelocity[3];
	int k;
	float planes[8][3];
	int permutation[8];
	float time_left;
	float end[3];
	int numplanes;
	int bumpcount;
	float primal_velocity[3];
	trace_t trace;
	float endVelocity[3];
	float clipVelocity[3];
	int i;
	playerState_s *ps;
	float into;
	float *plane;

	//memset(&trace, 0, sizeof(trace_t));
	ps = pm->ps;
	numbumps = 4;

	primal_velocity[0] = ps->velocity[0];
	primal_velocity[1] = ps->velocity[1];
	primal_velocity[2] = ps->velocity[2];

	endVelocity[0] = ps->velocity[0];
	endVelocity[1] = ps->velocity[1];
	endVelocity[2] = ps->velocity[2];

	if ( gravity )
	{
		endVelocity[2] = endVelocity[2] - (float)((float)ps->gravity * pml->frametime);
		ps->velocity[2] = (float)(ps->velocity[2] + endVelocity[2]) * 0.5;
		primal_velocity[2] = endVelocity[2];

		if ( pml->groundPlane )
			PM_ClipVelocity(ps->velocity, pml->groundTrace.normal, ps->velocity);
	}

	time_left = pml->frametime;

	if ( pml->groundPlane )
	{
		planes[0][0] = pml->groundTrace.normal[0];
		planes[0][1] = pml->groundTrace.normal[1];
		planes[0][2] = pml->groundTrace.normal[2];

		numplanes = 1;
	}
	else
	{
		numplanes = 0;
	}

	Vec3NormalizeTo(ps->velocity, planes[numplanes++]);

	for ( bumpcount = 0; bumpcount < numbumps; ++bumpcount )
	{
		end[0] = (float)(time_left * ps->velocity[0]) + ps->origin[0];
		end[1] = (float)(time_left * ps->velocity[1]) + ps->origin[1];
		end[2] = (float)(time_left * ps->velocity[2]) + ps->origin[2];

		PM_playerTrace(pm, &trace, ps->origin, pm->mins, pm->maxs, end, ps->clientNum, pm->tracemask);

		if ( trace.allsolid )
		{
			ps->velocity[2] = 0.0;
			return 1;
		}

		/*
		if ( trace.startsolid && do_push )
		{
			ps->origin[0] = ps->origin[0] + trace.normal[0];
			ps->origin[1] = ps->origin[1] + trace.normal[1];
			ps->origin[2] = ps->origin[2] + trace.normal[2];
		}
		*/

		if ( trace.fraction > 0.0 )
			Vec3Lerp(ps->origin, end, trace.fraction, ps->origin);

		if ( trace.fraction == 1.0 )
			break;

		PM_AddTouchEnt(pm, trace.entityNum);
		time_left = time_left - (float)(time_left * trace.fraction);

		if ( numplanes >= 8 )
		{
			ps->velocity[0] = 0.0;
			ps->velocity[1] = 0.0;
			ps->velocity[2] = 0.0;

			return 1;
		}

		for ( i = 0; i < numplanes; ++i )
		{
			if ( (float)((float)((float)(trace.normal[0] * planes[i][0])
			                     + (float)(trace.normal[1] * planes[i][1]))
			             + (float)(trace.normal[2] * planes[i][2])) > 0.99900001 )
			{
				PM_ClipVelocity(ps->velocity, trace.normal, ps->velocity);

				ps->velocity[0] = trace.normal[0] + ps->velocity[0];
				ps->velocity[1] = trace.normal[1] + ps->velocity[1];
				ps->velocity[2] = trace.normal[2] + ps->velocity[2];

				break;
			}
		}

		if ( i >= numplanes )
		{
			plane = planes[numplanes];

			plane[0] = trace.normal[0];
			plane[1] = trace.normal[1];
			plane[2] = trace.normal[2];

			into = PM_PermuteRestrictiveClipPlanes(ps->velocity, ++numplanes, planes, permutation);

			if ( into < 0.1 )
			{
				if ( -into > pml->impactSpeed )
					pml->impactSpeed = -into;

				PM_ClipVelocity(ps->velocity, planes[permutation[0]], clipVelocity);
				PM_ClipVelocity(endVelocity, planes[permutation[0]], endClipVelocity);

				for ( j = 1; j < numplanes; ++j )
				{
					plane = planes[permutation[j]];

					if ( (float)((float)((float)(clipVelocity[0] * *plane) + (float)(clipVelocity[1] * plane[1]))
					             + (float)(clipVelocity[2] * plane[2])) < 0.1 )
					{
						PM_ClipVelocity(clipVelocity, planes[permutation[j]], clipVelocity);
						PM_ClipVelocity(endClipVelocity, planes[permutation[j]], endClipVelocity);

						if ( (float)((float)((float)(clipVelocity[0] * planes[permutation[0]][0])
						                     + (float)(clipVelocity[1] * planes[permutation[0]][1]))
						             + (float)(clipVelocity[2] * planes[permutation[0]][2])) < 0.0 )
						{
							Vec3Cross(planes[permutation[0]], planes[permutation[j]], dir);
							Vec3Normalize(dir);

							d = (float)((float)(dir[0] * ps->velocity[0]) + (float)(dir[1] * ps->velocity[1]))
							    + (float)(dir[2] * ps->velocity[2]);

							clipVelocity[0] = d * dir[0];
							clipVelocity[1] = d * dir[1];
							clipVelocity[2] = d * dir[2];

							d = (float)((float)(dir[0] * endVelocity[0]) + (float)(dir[1] * endVelocity[1]))
							    + (float)(dir[2] * endVelocity[2]);

							endClipVelocity[0] = d * dir[0];
							endClipVelocity[1] = d * dir[1];
							endClipVelocity[2] = d * dir[2];

							for ( k = 1; k < numplanes; ++k )
							{
								if ( k != j )
								{
									plane = planes[permutation[k]];

									if ( (float)((float)((float)(clipVelocity[0] * *plane) + (float)(clipVelocity[1] * plane[1]))
									             + (float)(clipVelocity[2] * plane[2])) < 0.1 )
									{
										ps->velocity[0] = 0.0;
										ps->velocity[1] = 0.0;
										ps->velocity[2] = 0.0;

										return 1;
									}
								}
							}
						}
					}
				}

				ps->velocity[0] = clipVelocity[0];
				ps->velocity[1] = clipVelocity[1];
				ps->velocity[2] = clipVelocity[2];

				endVelocity[0] = endClipVelocity[0];
				endVelocity[1] = endClipVelocity[1];
				endVelocity[2] = endClipVelocity[2];
			}
		}
	}

	if ( gravity )
	{
		ps->velocity[0] = endVelocity[0];
		ps->velocity[1] = endVelocity[1];
		ps->velocity[2] = endVelocity[2];
	}

	if ( ps->pm_time )
	{
		ps->velocity[0] = primal_velocity[0];
		ps->velocity[1] = primal_velocity[1];
		ps->velocity[2] = primal_velocity[2];
	}

	return bumpcount != 0;
}

void PM_StepSlideMove(pmove_t *pm, pml_t *pml, int gravity)
{
	float dot2;
	float deltaPos;
	float deltaPos2;
	float deltaPos3;
	float upPoS;
	int iParm4;
	int iDelta;
	float dot1;
	float iDeltaResult;
	int iOldBobCycle;
	float endPos;
	float absPos;
	int iParm;
	int iParm2;
	int iParm3;
	int iDelta2;
	playerState_s *ps;
	int jumping;
	vec3_t endpos;
	int bHadGround;
	float fStepAmount;
	int iBumps;
	float fStepSize;
	// int bCloser;
	vec2_t vel;
	vec2_t flatDelta;
	vec3_t down;
	vec3_t up;
	trace_t trace;
	vec3_t down_v;
	vec3_t down_o;
	vec3_t start_v;
	vec3_t start_o;

	fStepAmount = 0.0;
	ps = pm->ps;
	jumping = 0;

	if ( (pm->ps->pm_flags & 0x20) != 0 )
	{
		bHadGround = 0;
		Jump_ClearState(ps);
	}
	else if ( pml->groundPlane )
	{
		bHadGround = 1;
	}
	else
	{
		bHadGround = 0;

		if ( (ps->pm_flags & 0x80000) != 0 && ps->pm_time )
			Jump_ClearState(ps);
	}

	VectorCopy(ps->origin, start_o);
	VectorCopy(ps->velocity, start_v);

	iBumps = PM_SlideMove(pm, pml, gravity);

	if ( (ps->pm_flags & 1) != 0 )
		fStepSize = 10.0;
	else
		fStepSize = 18.0;

	if ( ps->groundEntityNum != 1023 )
		goto LABEL_23;

	if ( (ps->pm_flags & 0x80000) != 0 && ps->pm_time )
		Jump_ClearState(ps);

	if ( iBumps && (ps->pm_flags & 0x80000) != 0 && Jump_GetStepHeight(ps, start_o, &fStepSize) )
	{
		if ( fStepSize < 1.0 )
			return;

		jumping = 1;
	}

	if ( jumping || (ps->pm_flags & 0x20) != 0 && ps->velocity[2] > 0.0 )
	{
LABEL_23:
		VectorCopy(ps->origin, down_o);
		VectorCopy(ps->velocity, down_v);
		Vector2Subtract(down_o, start_o, flatDelta);

		if ( iBumps )
		{
			VectorCopy(start_o, up);
			up[2] = fStepSize + 1.0 + up[2];

			PM_playerTrace(pm, &trace, start_o, pm->mins, pm->maxs, up, ps->clientNum, pm->tracemask);
			fStepAmount = (fStepSize + 1.0) * trace.fraction - 1.0;

			if ( fStepAmount >= 1.0 )
			{
				upPoS = start_o[2] + fStepAmount;
				VectorSet(ps->origin, up[0], up[1], upPoS);
				VectorCopy(start_v, ps->velocity);
				PM_SlideMove(pm, pml, gravity);
			}
			else
			{
				fStepAmount = 0.0;
			}
		}

		if ( bHadGround || fStepAmount != 0.0 )
		{
			VectorCopy(ps->origin, down);
			down[2] = down[2] - fStepAmount;

			if ( bHadGround )
				down[2] = down[2] - 9.0;

			PM_playerTrace(pm, &trace, ps->origin, pm->mins, pm->maxs, down, ps->clientNum, pm->tracemask);

			if ( trace.entityNum <= 0x3Fu )
				goto LABEL_32;

			if ( trace.fraction >= 1.0 )
			{
				if ( fStepAmount != 0.0 )
					ps->origin[2] = ps->origin[2] - fStepAmount;
			}
			else
			{
				if ( trace.normal[2] < 0.30000001 )
				{
LABEL_32:
					VectorCopy(down_o, ps->origin);
					VectorCopy(down_v, ps->velocity);
					return;
				}

				Vec3Lerp(ps->origin, down, trace.fraction, ps->origin);
#ifdef LIBCOD
				PM_ProjectVelocity(ps->velocity, trace.normal, ps->velocity);
#else
				PM_ClipVelocity(ps->velocity, trace.normal, ps->velocity);
#endif
			}
		}

		Vector2Subtract(ps->origin, start_o, vel);

		dot1 = Dot2Product(vel, ps->velocity);
		dot2 = Dot2Product(flatDelta, ps->velocity) + 0.001;

		// bCloser = dot2 >= dot1;

		if ( dot2 >= dot1 || jumping && Jump_IsPlayerAboveMax(ps) )
		{
			VectorCopy(down_o, ps->origin);
			VectorCopy(down_v, ps->velocity);
			fStepAmount = 0.0;

			if ( bHadGround )
			{
				VectorCopy(ps->origin, down);
				down[2] = down[2] - 9.0;

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
			Jump_ClampVelocity(ps, down_o);

		if ( bHadGround )
		{
			if ( ps->pm_type <= PM_INTERMISSION )
			{
				if ( PM_VerifyPronePosition(pm, start_o, start_v) )
				{
					deltaPos = ps->origin[2] - down_o[2];

					if ( I_fabs(deltaPos) > 0.5 )
					{
						deltaPos2 = ps->origin[2] - down_o[2];
						iParm = floor(deltaPos2);

						if ( iParm )
						{
							if ( iParm >= -16 )
							{
								if ( iParm > EV_FOOTSTEP_WALK_DEFAULT )
									iParm = EV_FOOTSTEP_WALK_DEFAULT;
							}
							else
							{
								iParm = -16;
							}

							iParm2 = iParm + 128;
							BG_AddPredictableEventToPlayerstate(EV_STEP_VIEW, iParm2, ps);
							deltaPos3 = ps->origin[2] - start_o[2];
							absPos = I_fabs(deltaPos3);
							endPos = (1.0 - absPos / fStepSize) * 0.80000001 + 0.19999999;
							VectorScale(ps->velocity, endPos, ps->velocity);
							iParm3 = iParm2 - 128;
							iDelta = iParm3;

							if ( iParm3 < 0 )
								iDelta = -iParm3;

							if ( iDelta > 3 && ps->groundEntityNum != 1023 && PM_ShouldMakeFootsteps(pm) )
							{
								iParm4 = iParm3;

								if ( iParm3 < 0 )
									iParm4 = -iParm3;

								iDelta2 = iParm4 / 2;

								if ( iParm4 / 2 > 4 )
									iDelta2 = 4;

								iDeltaResult = (float)iDelta2 * 1.25 + 7.0;
								iOldBobCycle = ps->bobCycle;
								ps->bobCycle = (unsigned char)(int)((float)iOldBobCycle + iDeltaResult);
								PM_FootstepEvent(pm, pml, iOldBobCycle, ps->bobCycle, 1);
							}
						}
					}
				}
			}
		}
	}
}