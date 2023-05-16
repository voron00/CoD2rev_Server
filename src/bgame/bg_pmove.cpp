#include "../qcommon/qcommon.h"
#include "bg_public.h"

pmoveHandler_t pmoveHandlers[] =
{
	{ NULL, NULL, NULL },
	{ G_TraceCapsule, SV_PointContents, G_PlayerEvent },
};

viewLerpWaypoint_s viewLerp_CrouchProne[] =
{
	{ 0, 40.000000, 0},
	{ 11, 38.000000, 0},
	{ 22, 33.000000, 0},
	{ 34, 25.000000, 0},
	{ 45, 16.000000, 0},
	{ 50, 15.000000, 0},
	{ 55, 16.000000, 0},
	{ 70, 18.000000, 0},
	{ 90, 17.000000, 0},
	{ 100, 11.000000, 0},
	{ -1, 0.000000, 0},
};

viewLerpWaypoint_s viewLerp_StandCrouch[] =
{
	{ 0, 60.000000, 0},
	{ 1, 59.500000, 0},
	{ 4, 58.500000, 0},
	{ 30, 56.000000, 0},
	{ 80, 44.000000, 0},
	{ 90, 41.500000, 0},
	{ 95, 40.500000, 0},
	{ 100, 40.000000, 0},
	{ -1, 0.000000, 0},
};

viewLerpWaypoint_s viewLerp_ProneCrouch[] =
{
	{ 0, 11.000000, 0},
	{ 5, 10.000000, 0},
	{ 30, 21.000000, 0},
	{ 50, 25.000000, 0},
	{ 67, 31.000000, 0},
	{ 83, 34.000000, 0},
	{ 100, 40.000000, 0},
	{ -1, 0.000000, 0},
};

viewLerpWaypoint_s viewLerp_CrouchStand[] =
{
	{ 0, 40.000000, 0},
	{ 5, 40.500000, 0},
	{ 10, 41.500000, 0},
	{ 20, 44.000000, 0},
	{ 70, 56.000000, 0},
	{ 96, 58.500000, 0},
	{ 99, 59.500000, 0},
	{ 100, 60.000000, 0},
	{ -1, 0.000000, 0},
};

const vec3_t CorrectSolidDeltas[] =
{
	{ 0.000000, 0.000000, 1.000000 },
	{ -1.000000, 0.000000, 1.000000 },
	{ 0.000000, -1.000000, 1.000000 },
	{ 1.000000, 0.000000, 1.000000 },
	{ 0.000000, 1.000000, 1.000000 },
	{ -1.000000, 0.000000, 0.000000 },
	{ 0.000000, -1.000000, 0.000000 },
	{ 1.000000, 0.000000, 0.000000 },
	{ 0.000000, 1.000000, 0.000000 },
	{ 0.000000, 0.000000, -1.000000 },
	{ -1.000000, 0.000000, -1.000000 },
	{ 0.000000, -1.000000, -1.000000 },
	{ 1.000000, 0.000000, -1.000000 },
	{ 0.000000, 1.000000, -1.000000 },
	{ -1.000000, -1.000000, 1.000000 },
	{ 1.000000, -1.000000, 1.000000 },
	{ 1.000000, 1.000000, 1.000000 },
	{ -1.000000, 1.000000, 1.000000 },
	{ -1.000000, -1.000000, 0.000000 },
	{ 1.000000, -1.000000, 0.000000 },
	{ 1.000000, 1.000000, 0.000000 },
	{ -1.000000, 1.000000, 0.000000 },
	{ -1.000000, -1.000000, -1.000000 },
	{ 1.000000, -1.000000, -1.000000 },
	{ 1.000000, 1.000000, -1.000000 },
	{ -1.000000, 1.000000, -1.000000 },
};

int PM_GetViewHeightLerpTime(const playerState_s *ps, int iTarget, int bDown)
{
	if ( iTarget == 11 )
		return 400;

	if ( iTarget != 40 )
		return 200;

	if ( bDown )
		return 200;

	return 400;
}

int QDECL PM_GetEffectiveStance(const playerState_s *ps)
{
	if ( ps->viewHeightTarget == 40 )
		return 2;
	else
		return ps->viewHeightTarget == 11;
}

void QDECL PM_AddEvent(playerState_s *ps, int newEvent)
{
	BG_AddPredictableEventToPlayerstate(newEvent, 0, ps);
}

int CL_LocalClient_GetActiveCount()
{
	return 1;
}

void PM_trace(pmove_t *pm,trace_t *results, const float *start, const float *mins, const float *maxs, const float *end, int passEntityNum, int contentMask)
{
	pmoveHandlers[pm->handler].trace(results, start, mins, maxs, end, passEntityNum, contentMask);
}

void QDECL PM_AddTouchEnt(pmove_t *pm, int entityNum)
{
	int i;

	if ( entityNum != 1022 && pm->numtouch != 32 )
	{
		for ( i = 0; i < pm->numtouch; ++i )
		{
			if ( pm->touchents[i] == entityNum )
				return;
		}

		pm->touchents[pm->numtouch++] = entityNum;
	}
}

void QDECL PM_playerTrace(pmove_t *pm, trace_t *results, const float *start, const float *mins, const float *maxs, const float *end, int passEntityNum, int contentMask)
{
	pmoveHandlers[pm->handler].trace(results, start, mins, maxs, end, passEntityNum, contentMask);

	if ( results->startsolid )
	{
		if ( (results->contents & 0x2000000) != 0 )
		{
			PM_AddTouchEnt(pm, results->entityNum);
			pm->tracemask &= ~0x2000000u;
			pmoveHandlers[pm->handler].trace(results, start, mins, maxs, end, passEntityNum, contentMask & 0xFDFFFFFF);
		}
	}
}

int QDECL PM_GroundSurfaceType(pml_t *pml)
{
	if ( (pml->groundTrace.surfaceFlags & 0x2000) != 0 )
		return 0;
	else
		return (pml->groundTrace.surfaceFlags & 0x1F00000) >> 20;
}

void QDECL PM_Accelerate(playerState_s *ps, const pml_t *pml, float *wishdir, float wishspeed, float accel)
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

int PM_FootstepType(playerState_s *ps, pml_t *pml)
{
	int iSurfType;

	iSurfType = PM_GroundSurfaceType(pml);

	if ( !iSurfType )
		return 0;

	if ( (ps->pm_flags & 1) != 0 )
		return iSurfType + 47;

	if ( (ps->pm_flags & 0x100) != 0 || ps->leanf != 0.0 )
		return iSurfType + 24;

	return iSurfType + 1;
}

void QDECL PM_FootstepEvent(pmove_t *pm, pml_t *pml, int iOldBobCycle, int iNewBobCycle, int bFootStep)
{
	int footstepType;
	trace_t trace;
	vec3_t end;
	vec3_t maxs;
	vec3_t mins;
	// float fTraceDist;
	int contentMask;
	int iSurfaceType;
	playerState_s *ps;

	ps = pm->ps;

	if ( (((iNewBobCycle + 64) ^ (iOldBobCycle + 64)) & 0x80u) != 0 )
	{
		if ( ps->groundEntityNum == 1023 )
		{
			if ( bFootStep && (ps->pm_flags & 0x20) != 0 )
			{
				VectorCopy(pm->mins, mins);

				mins[0] = mins[0] + 6.0;
				mins[1] = mins[1] + 6.0;
				mins[2] = 8.0;

				VectorCopy(pm->maxs, maxs);

				maxs[0] = maxs[0] - 6.0;
				maxs[1] = maxs[1] - 6.0;

				if ( mins[2] > maxs[2] )
					maxs[2] = mins[2];

				contentMask = pm->tracemask & 0xFDFEFFFF;
				// fTraceDist = -31.0;
				VectorMA(ps->origin, -31.0, ps->vLadderVec, end);
				PM_playerTrace(pm, &trace, ps->origin, mins, maxs, end, ps->clientNum, contentMask);
				iSurfaceType = (trace.surfaceFlags & 0x1F00000) >> 20;

				if ( trace.fraction == 1.0 || !iSurfaceType )
					iSurfaceType = 21;

				PM_AddEvent(ps, iSurfaceType + 1);
			}
		}
		else if ( bFootStep )
		{
			footstepType = PM_FootstepType(ps, pml);
			PM_AddEvent(ps, (entity_event_t)footstepType);
		}
	}
}

qboolean QDECL PM_ShouldMakeFootsteps(pmove_t *pm)
{
	playerState_s *ps;
	int flags;
	int stance;

	ps = pm->ps;
	flags = pm->ps->pm_flags & 0x100;
	stance = PM_GetEffectiveStance(pm->ps);

	if ( stance == 1 || stance == 2 )
		return 0;

	if ( SLOBYTE(ps->pm_flags) >= 0 )
	{
		if ( !flags )
			return pm->xyspeed >= (float)player_footstepsThreshhold->current.decimal;

		return 0;
	}

	if ( flags )
		return 0;

	return pm->xyspeed >= (float)player_footstepsThreshhold->current.decimal;
}

int QDECL PM_VerifyPronePosition(pmove_t *pm, float *vFallbackOrg, float *vFallbackVel)
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

float PM_GetReducedFriction(playerState_s *ps)
{
	if ( ps->pm_time > 1800 )
	{
		Jump_ClearState(ps);
		return 1.0;
	}
	else
	{
		return Jump_ReduceFriction(ps);
	}
}

void PM_Friction(playerState_s *ps, pml_t *pml)
{
	float current;
	float drop;
	float control;
	float newspeed;
	float scale;
	float speed;
	vec_t *vel;
	vec3_t vec;

	vel = ps->velocity;
	VectorCopy(ps->velocity, vec);

	if ( pml->walking )
		vec[2] = 0.0;

	speed = VectorLength(vec);

	if ( speed >= 1.0 )
	{
		drop = 0.0;

		if ( pml->walking && (pml->groundTrace.surfaceFlags & 2) == 0 && (ps->pm_flags & 0x400) == 0 )
		{
			if ( stopspeed->current.decimal <= speed )
				current = speed;
			else
				current = stopspeed->current.decimal;

			control = current;

			if ( (ps->pm_flags & 0x200) != 0 )
			{
				control = current * 0.30000001;
			}
			else if ( (ps->pm_flags & 0x80000) != 0 )
			{
				control = PM_GetReducedFriction(ps) * current;
			}

			drop = control * friction->current.decimal * pml->frametime + 0.0;
		}

		if ( ps->pm_type == 4 )
			drop = speed * 5.0 * pml->frametime + drop;

		newspeed = speed - drop;

		if ( newspeed < 0.0 )
			newspeed = 0.0;

		scale = newspeed / speed;
		*vel = *vel * scale;
		ps->velocity[1] = ps->velocity[1] * scale;
		ps->velocity[2] = ps->velocity[2] * scale;
	}
	else
	{
		VectorClear(vel);
	}
}

float PM_MoveScale(playerState_s *ps, float fmove, float rmove, float umove)
{
	float temp;
	float scale;
	float speed;
	float max;

	max = fabs(fmove);

	if ( fabs(rmove) > max )
		max = fabs(rmove);

	if ( fabs(umove) > max )
		max = fabs(umove);

	if ( max == 0.0 )
	{
		return 0.0;
	}
	else
	{
		temp = fmove * fmove + rmove * rmove + umove * umove;
		speed = sqrt(temp);
		scale = (float)ps->speed * max / (speed * 127.0);

		if ( (ps->pm_flags & 0x100) != 0 || ps->leanf != 0.0 )
			scale = scale * 0.40000001;

		if ( ps->pm_type == 2 )
			scale = scale * 3.0;

		if ( ps->pm_type == 3 )
			scale = scale * 6.0;

		if ( ps->pm_type == 4 )
			return scale * player_spectateSpeedScale->current.decimal;

		return scale;
	}
}

float QDECL PM_CmdScale(playerState_s *ps, usercmd_s *cmd)
{
	float temp;
	int set;
	int rightmove;
	int forwardmove;
	float scale;
	float total;
	int value;

	temp = (float)(cmd->rightmove * cmd->rightmove + cmd->forwardmove * cmd->forwardmove);
	total = sqrt(temp);
	forwardmove = cmd->forwardmove;

	if ( forwardmove < 0 )
		forwardmove = -forwardmove;

	value = forwardmove;
	rightmove = cmd->rightmove;

	if ( rightmove < 0 )
		rightmove = -rightmove;

	if ( rightmove > forwardmove )
	{
		set = cmd->rightmove;

		if ( set < 0 )
			set = -set;

		value = set;
	}

	if ( value )
	{
		scale = (float)ps->speed * (float)value / (total * 127.0);

		if ( (ps->pm_flags & 0x100) != 0 || ps->leanf != 0.0 )
			scale = scale * 0.40000001;

		if ( ps->pm_type == 2 )
			scale = scale * 3.0;

		if ( ps->pm_type == 3 )
			scale = scale * 6.0;

		if ( ps->pm_type == 4 )
			return scale * player_spectateSpeedScale->current.decimal;

		return scale;
	}
	else
	{
		return 0.0;
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

float PM_ViewHeightTableLerp(int iFrac, viewLerpWaypoint_s *pTable, float *pfPosOfs)
{
	float fEntryFrac;
	viewLerpWaypoint_s *pCurr;
	int i;

	if ( iFrac )
	{
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
				fEntryFrac = (float)(iFrac - pTable[i - 1].iFrac) / (float)(pCurr->iFrac - pTable[i - 1].iFrac);
				*pfPosOfs = (float)pTable[i - 1].iOffset
				            + (float)(pCurr->iOffset - pTable[i - 1].iOffset) * fEntryFrac;
				return (pCurr->fViewHeight - pTable[i - 1].fViewHeight) * fEntryFrac + pTable[i - 1].fViewHeight;
			}
			pCurr = &pTable[++i];
		}
		while ( pCurr->iFrac != -1 );
		*pfPosOfs = (float)pTable->iOffset;
		return pTable->fViewHeight;
	}
	else
	{
		*pfPosOfs = (float)pTable->iOffset;
		return pTable->fViewHeight;
	}
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

						if ( fabs(height) > 0.050000001 )
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
	else if ( ps->pm_type == 4 )
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

	if ( ps->pm_type == 4 )
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

		if ( ps->pm_type <= 5 )
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

void PM_DropTimers(playerState_s *ps, pml_t *pml)
{
	if ( ps->pm_time )
	{
		if ( pml->msec < ps->pm_time )
		{
			ps->pm_time -= pml->msec;
		}
		else
		{
			ps->pm_flags &= 0xFFF7F9FF;
			ps->pm_time = 0;
		}
	}

	if ( ps->legsTimer > 0 )
	{
		ps->legsTimer -= pml->msec;

		if ( ps->legsTimer < 0 )
			ps->legsTimer = 0;
	}

	if ( ps->torsoTimer > 0 )
	{
		ps->torsoTimer -= pml->msec;

		if ( ps->torsoTimer < 0 )
			ps->torsoTimer = 0;
	}
}

void PM_UpdatePlayerWalkingFlag(pmove_t *pm)
{
	playerState_s *ps;

	ps = pm->ps;
	ps->pm_flags &= ~0x100u;

	if ( ps->pm_type <= 5
	        && (pm->cmd.buttons & 0x1000) != 0
	        && (ps->pm_flags & 1) == 0
	        && (ps->pm_flags & 0x40) != 0
	        && ps->weaponstate != WEAPON_RELOADING
	        && ps->weaponstate != WEAPON_RELOAD_START
	        && ps->weaponstate != WEAPON_RELOAD_END
	        && ps->weaponstate != WEAPON_RELOAD_START_INTERUPT
	        && ps->weaponstate != WEAPON_RELOADING_INTERUPT )
	{
		ps->pm_flags |= 0x100u;
	}
}

void PM_ClearLadderFlag(playerState_s *ps)
{
	if ( (ps->pm_flags & 0x20) != 0 )
		ps->pm_flags |= 0x40000u;

	ps->pm_flags &= ~0x20u;
}

void PM_SetLadderFlag(playerState_s *ps)
{
	ps->pm_flags |= 0x20u;
}

void PM_SetMovementDir(pmove_t *pm, pml_t *pml)
{
	float dir[3];
	int moveyaw;
	float moved[3];
	float speed;
	playerState_s *ps;

	ps = pm->ps;

	if ( (ps->pm_flags & 1) == 0 || (ps->eFlags & 0x300) != 0 )
	{
		if ( (ps->pm_flags & 0x20) != 0 )
		{
			speed = vectoyaw(ps->vLadderVec) + 180.0;
			moveyaw = (int)AngleDelta(speed, ps->viewangles[1]);

			if ( (int)abs32(moveyaw) > 90 )
			{
				if ( moveyaw <= 0 )
					moveyaw = -90;
				else
					moveyaw = 90;
			}

			ps->movementDir = (char)moveyaw;
		}
		else
		{
			moved[0] = ps->origin[0] - pml->previous_origin[0];
			moved[1] = ps->origin[1] - pml->previous_origin[1];
			moved[2] = ps->origin[2] - pml->previous_origin[2];

			if ( !pm->cmd.forwardmove && !pm->cmd.rightmove
			        || ps->groundEntityNum == 1023
			        || (speed = Abs(moved), speed == 0.0)
			        || speed <= (float)(pml->frametime * 5.0) )
			{
				ps->movementDir = 0;
			}
			else
			{
				Vec3NormalizeTo(moved, dir);
				vectoangles(dir, dir);
				moveyaw = (int)AngleDelta(dir[1], ps->viewangles[1]);

				if ( pm->cmd.forwardmove < 0 )
					moveyaw = (int)AngleNormalize180((float)moveyaw + 180.0);

				if ( (int)abs32(moveyaw) > 90 )
				{
					if ( moveyaw <= 0 )
						moveyaw = -90;
					else
						moveyaw = 90;
				}

				ps->movementDir = (char)moveyaw;
			}
		}
	}
	else
	{
		moveyaw = (int)AngleDelta(ps->proneDirection, ps->viewangles[1]);

		if ( (int)abs32(moveyaw) > 90 )
		{
			if ( moveyaw <= 0 )
				moveyaw = -90;
			else
				moveyaw = 90;
		}

		ps->movementDir = (char)moveyaw;
	}
}

void QDECL PM_ClipVelocity(const float *in, const float *normal, float *out)
{
	float backoff;
	float change;

	backoff = DotProduct(in, normal);
	change = backoff - fabs(backoff) * 0.001;

	VectorMA(in, -change, normal, out);
}

extern dvar_t *jump_bounceEnable;
void QDECL PM_ProjectVelocity(const float *velIn, const float *normal, float *velOut)
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

	if ( fabs(normal[2]) < 0.001 || lengthSq2D == 0.0 )
	{
		velOut[0] = velIn[0];
		velOut[1] = velIn[1];
		velOut[2] = velIn[2];
	}
	else
	{
		newZ = (float)-(float)((float)(velIn[0] * normal[0]) + (float)(velIn[1] * normal[1])) / normal[2];
		adjusted = velIn[1];
		lengthScale = sqrt((float)((float)(velIn[2] * velIn[2]) + lengthSq2D) / (float)((float)(newZ * newZ) + lengthSq2D));

		if ( lengthScale < 1.0 || newZ < 0.0 || velIn[2] > 0.0 )
		{
			velOut[0] = lengthScale * velIn[0];
			velOut[1] = lengthScale * adjusted;
			velOut[2] = lengthScale * newZ;
		}
	}
}

void QDECL PM_AirMove(pmove_t *pm, pml_t *pml)
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

float PM_DamageScale_Walk(int damage_timer)
{
	float timer_min;
	float timer_max;

	if ( damage_timer )
	{
		timer_max = player_dmgtimer_maxTime->current.decimal;

		if ( timer_max == 0.0 )
		{
			return 1.0;
		}
		else
		{
			timer_min = -player_dmgtimer_minScale->current.decimal / timer_max;
			return (float)damage_timer * timer_min + 1.0;
		}
	}
	else
	{
		return 1.0;
	}
}

float PM_CmdScaleForStance(const pmove_t *pm, int iTarget, int bDown)
{
	float time;
	playerState_s *ps;

	ps = pm->ps;
	if ( pm->ps->viewHeightLerpTime )
	{
		if ( iTarget == -1
		        || bDown == -1
		        || bDown == ps->viewHeightLerpTarget
		        && (bDown != 40 || iTarget == 11 && !ps->viewHeightLerpDown || iTarget == 60 && ps->viewHeightLerpDown) )
		{
			time = (float)(pm->cmd.serverTime - ps->viewHeightLerpTime)
			       / (float)PM_GetViewHeightLerpTime(ps, ps->viewHeightLerpTarget, ps->viewHeightLerpDown);
			if ( time >= 0.0 )
			{
				if ( time > 1.0 )
					return 1.0;
			}
			else
			{
				return 0.0;
			}
			return time;
		}
		else
		{
			return 0.0;
		}
	}
	else
	{
		return 0.0;
	}
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
	forwardSquared = sqrt(totalForward);

	if ( cmd->forwardmove >= 0 )
		forward = (float)cmd->forwardmove;
	else
		forward = (float)cmd->forwardmove * player_backSpeedScale->current.decimal;

	absTotal = fabs(forward);
	totalRight = (float)cmd->rightmove * player_strafeSpeedScale->current.decimal;
	rightSquared = fabs(totalRight);
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

		if ( ps->pm_type == 2 )
		{
			totalSpeed = totalSpeed * 3.0;
		}
		else if ( ps->pm_type == 3 )
		{
			totalSpeed = totalSpeed * 6.0;
		}
		else
		{
			stance = PM_GetEffectiveStance(ps);
			scale = PM_CmdScaleForStance(pm, 40, 11);

			if ( scale == 0.0 )
			{
				scale2 = PM_CmdScaleForStance(pm, 11, 40);

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

int PM_CorrectAllSolid(pmove_t *pm, pml_t *pml, trace_t *trace)
{
	playerState_s *ps;
	vec3_t point;
	unsigned int i;

	ps = pm->ps;

	for ( i = 0; i < COUNT_OF(CorrectSolidDeltas); ++i )
	{
		VectorAdd(ps->origin, CorrectSolidDeltas[i], point);
		PM_playerTrace(pm, trace, point, pm->mins, pm->maxs, point, ps->clientNum, pm->tracemask);

		if ( !trace->startsolid )
		{
			VectorCopy(point, ps->origin);

			point[0] = ps->origin[0];
			point[1] = ps->origin[1];
			point[2] = ps->origin[2] - 1.0;

			PM_playerTrace(pm, trace, ps->origin, pm->mins, pm->maxs, point, ps->clientNum, pm->tracemask);
			pml->groundTrace = *trace;
			Vec3Lerp(ps->origin, point, trace->fraction, ps->origin);

			return 1;
		}
	}

	ps->groundEntityNum = 1023;
	pml->groundPlane = 0;
	pml->almostGroundPlane = 0;
	pml->walking = 0;
	Jump_ClearState(ps);

	return 0;
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

void PM_FoliageSounds(pmove_t *pm)
{
	playerState_s *ps;
	vec3_t maxs;
	vec3_t mins;
	trace_t trace;
	int interval;
	float speedFrac;

	ps = pm->ps;

	if ( bg_foliagesnd_minspeed->current.decimal <= (float)pm->xyspeed )
	{
		speedFrac = (pm->xyspeed - bg_foliagesnd_minspeed->current.decimal)
		            / (bg_foliagesnd_maxspeed->current.decimal - bg_foliagesnd_minspeed->current.decimal);
		if ( speedFrac > 1.0 )
			speedFrac = 1.0;
		interval = (int)((float)(bg_foliagesnd_fastinterval->current.integer
		                         - bg_foliagesnd_slowinterval->current.integer)
		                 * speedFrac
		                 + (float)bg_foliagesnd_slowinterval->current.integer);
		if ( interval + ps->foliageSoundTime < pm->cmd.serverTime )
		{
			VectorScale(pm->mins, 0.75, mins);
			VectorScale(pm->maxs, 0.75, maxs);
			maxs[2] = pm->maxs[2] * 0.89999998;
			PM_playerTrace(pm, &trace, ps->origin, mins, maxs, ps->origin, ps->clientNum, 2);

			if ( trace.startsolid )
			{
				PM_AddEvent(ps, EV_FOLIAGE_SOUND);
				ps->foliageSoundTime = pm->cmd.serverTime;
			}
		}
	}
	else if ( bg_foliagesnd_resetinterval->current.integer + ps->foliageSoundTime < pm->cmd.serverTime )
	{
		ps->foliageSoundTime = 0;
	}
}

int PM_GetFlinchAnimType(float yaw)
{
	if ( yaw < 0.0 )
		yaw = yaw + 360.0;

	if ( yaw >= 315.0 || yaw < 45.0 )
		return 31;

	if ( yaw < 135.0 )
		return 33;

	if ( yaw >= 225.0 )
		return 34;

	return 32;
}

void PM_Footsteps(pmove_t *pm, pml_t *pml)
{
	float newSpeedScale;
	float xyzSpeedScale;
	float velocitySquared;
	float flinchYaw;
	int animType;
	int animWalking;
	float height;
	int animScriptResult;
	int animResult;
	int turnAnimType;
	clientInfo_t *ci;
	int damageTime;
	playerState_s *ps;
	float currentStanceScale;
	float newStanceScale;
	float speed;
	int stance;
	qboolean bFootStep;
	int currentBobCycle;
	int iOldBobCycle;
	float bobCycleScale;
	float newspeed;

	ps = pm->ps;
	if ( pm->ps->pm_type > 5 )
		return;
	if ( ps->clientNum > 63 )
		ci = 0;
	else
		ci = &level_bgs.clientinfo[ps->clientNum];
	damageTime = ps->damageDuration - player_dmgtimer_stumbleTime->current.integer;
	if ( damageTime < 0 )
		damageTime = 0;
	velocitySquared = ps->velocity[0] * ps->velocity[0] + ps->velocity[1] * ps->velocity[1];
	pm->xyspeed = sqrt(velocitySquared);
	if ( (ps->eFlags & 0x300) != 0 )
	{
		if ( (ps->pm_flags & 1) != 0 )
		{
			BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_IDLEPRONE, 1);
			return;
		}
		if ( (ps->pm_flags & 2) != 0 )
			goto LABEL_11;
		goto LABEL_145;
	}
	stance = PM_GetEffectiveStance(ps);
	if ( ps->groundEntityNum == 1023 && ps->pm_type != 1 )
	{
		if ( (ps->pm_flags & 0x20) != 0 )
		{
			if ( pm->cmd.serverTime - ps->jumpTime <= 299 )
				return;
			height = ps->velocity[2];
			if ( (ps->pm_flags & 0x100) != 0 || ps->leanf != 0.0 )
				bobCycleScale = height / (95.25 * 0.40000001) * 0.34999999;
			else
				bobCycleScale = height / 95.25 * 0.44999999;
			if ( height < 0.0 )
				BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_CLIMBDOWN, 1);
			else
				BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_CLIMBUP, 1);
			currentBobCycle = ps->bobCycle;
			ps->bobCycle = (unsigned char)(int)((float)currentBobCycle + (float)pml->msec * bobCycleScale);
			PM_FootstepEvent(pm, pml, currentBobCycle, ps->bobCycle, 1);
		}
		if ( stance == (ps->pm_flags & 3) )
			return;
	}
	animWalking = 0;
	if ( (ps->pm_flags & 0x100) != 0 || ps->leanf != 0.0 )
		animWalking = 1;
	if ( player_moveThreshhold->current.decimal > (float)pm->xyspeed || ps->pm_type == 1 )
	{
		if ( pm->xyspeed < 1.0 )
			ps->bobCycle = 0;
		turnAnimType = 0;
		if ( ci && player_turnAnims->current.boolean )
		{
			if ( ci->turnAnimType && ci->turnAnimEndTime )
				Com_DPrintf("turn anim end time is %i, time is %i\n", ci->turnAnimEndTime, level_bgs.time);
			if ( ci->legs.yawing )
			{
				if ( ci->legs.yawAngle <= (float)ci->torso.yawAngle )
					turnAnimType = 15;
				else
					turnAnimType = 14;
				ci->legs.yawAngle = ci->torso.yawAngle;
				ci->turnAnimType = turnAnimType;
				if ( ci->turnAnimEndTime < level_bgs.time )
					ci->turnAnimEndTime = 0;
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
		if ( ps->viewHeightTarget == 11 )
		{
			animScriptResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_IDLEPRONE, 1);
		}
		else if ( ps->viewHeightTarget == 40 )
		{
			if ( turnAnimType == 14 )
			{
				animScriptResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_TURNRIGHTCR, 1);
				if ( animScriptResult > 0 && !ci->turnAnimEndTime )
					ci->turnAnimEndTime = level_bgs.time + ps->legsAnimDuration;
			}
			else if ( turnAnimType == 15 )
			{
				animScriptResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_TURNLEFTCR, 1);
				if ( animScriptResult > 0 && !ci->turnAnimEndTime )
					ci->turnAnimEndTime = level_bgs.time + ps->legsAnimDuration;
			}
			else
			{
				animScriptResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_IDLECR, 1);
			}
		}
		else if ( turnAnimType == 14 )
		{
			animScriptResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_TURNRIGHT, 1);
			if ( animScriptResult > 0 && !ci->turnAnimEndTime )
				ci->turnAnimEndTime = level_bgs.time + ps->legsAnimDuration;
		}
		else if ( turnAnimType == 15 )
		{
			animScriptResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_TURNLEFT, 1);
			if ( animScriptResult > 0 && !ci->turnAnimEndTime )
				ci->turnAnimEndTime = level_bgs.time + ps->legsAnimDuration;
		}
		else
		{
			if ( ps->damageTimer > damageTime )
			{
				flinchYaw = (float)ps->flinchYaw;
				animType = PM_GetFlinchAnimType(flinchYaw);
				BG_AnimScriptAnimation(ps, AISTATE_COMBAT, animType, 1);
				return;
			}
			animScriptResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_IDLE, 1);
		}
		if ( animScriptResult < 0 )
		{
			if ( ps->viewHeightTarget == 40 )
			{
LABEL_11:
				BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_IDLECR, 1);
				return;
			}
			if ( ps->damageTimer <= damageTime )
				goto LABEL_145;
			goto LABEL_74;
		}
		return;
	}
	speed = (float)ps->speed;
	if ( pm->cmd.forwardmove )
	{
		if ( pm->cmd.rightmove )
		{
			speed = ((player_strafeSpeedScale->current.decimal - 1.0) * 0.75 + 1.0 + 1.0) * 0.5 * speed;
			if ( pm->cmd.forwardmove < 0 )
				speed = (player_backSpeedScale->current.decimal + 1.0) * 0.5 * speed;
		}
		else if ( pm->cmd.forwardmove < 0 )
		{
			speed = speed * player_backSpeedScale->current.decimal;
		}
		BG_UpdateConditionValue(ps->clientNum, ANIM_COND_STRAFING, 0, 1);
	}
	else if ( pm->cmd.rightmove )
	{
		speed = ((player_strafeSpeedScale->current.decimal - 1.0) * 0.75 + 1.0) * speed;
		if ( pm->cmd.rightmove <= 0 )
			BG_UpdateConditionValue(ps->clientNum, ANIM_COND_STRAFING, 1, 1);
		else
			BG_UpdateConditionValue(ps->clientNum, ANIM_COND_STRAFING, 2, 1);
	}
	if ( animWalking )
		speed = speed * 0.40000001;
	currentStanceScale = PM_CmdScaleForStance(pm, 40, 11);
	if ( currentStanceScale == 0.0 )
	{
		newStanceScale = PM_CmdScaleForStance(pm, 11, 40);
		if ( newStanceScale == 0.0 )
		{
			if ( stance == 1 )
			{
				speed = speed * 0.15000001;
			}
			else if ( stance == 2 )
			{
				speed = speed * 0.64999998;
			}
		}
		else
		{
			speed = (newStanceScale * 0.64999998 + (1.0 - newStanceScale) * 0.15000001) * speed;
		}
	}
	else
	{
		speed = (currentStanceScale * 0.15000001 + (1.0 - currentStanceScale) * 0.64999998) * speed;
	}
	if ( stance == 1 )
	{
		if ( animWalking )
			newSpeedScale = pm->xyspeed / speed * 0.23999999;
		else
			newSpeedScale = pm->xyspeed / speed * 0.25;
		newspeed = newSpeedScale;
		if ( SLOBYTE(ps->pm_flags) >= 0 )
			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_WALKPRONE, 1);
		else
			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_WALKPRONEBK, 1);
	}
	else if ( stance == 2 )
	{
		if ( animWalking )
			xyzSpeedScale = pm->xyspeed / speed * 0.315;
		else
			xyzSpeedScale = pm->xyspeed / speed * 0.34;
		newspeed = xyzSpeedScale;
		if ( SLOBYTE(ps->pm_flags) >= 0 )
		{
			if ( animWalking )
			{
				if ( ps->damageTimer <= damageTime )
				{
					animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_WALKCR, 1);
					goto LABEL_136;
				}
			}
			else if ( ps->damageTimer <= damageTime )
			{
				animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_RUNCR, 1);
				goto LABEL_136;
			}
			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_STUMBLE_CROUCH_FORWARD, 1);
		}
		else
		{
			if ( animWalking )
			{
				if ( ps->damageTimer <= damageTime )
				{
					animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_WALKCRBK, 1);
					goto LABEL_136;
				}
			}
			else if ( ps->damageTimer <= damageTime )
			{
				animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_RUNCRBK, 1);
				goto LABEL_136;
			}
			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_STUMBLE_CROUCH_BACKWARD, 1);
		}
	}
	else if ( SLOBYTE(ps->pm_flags) >= 0 )
	{
		if ( animWalking )
		{
			newspeed = pm->xyspeed / speed * 0.30500001;
			if ( ps->damageTimer <= damageTime )
				animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_WALK, 1);
			else
				animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_STUMBLE_WALK_FORWARD, 1);
		}
		else
		{
			newspeed = pm->xyspeed / speed * 0.33500001;
			if ( ps->damageTimer <= damageTime )
				animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_RUN, 1);
			else
				animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_STUMBLE_FORWARD, 1);
		}
	}
	else if ( animWalking )
	{
		newspeed = pm->xyspeed / speed * 0.32499999;
		if ( ps->damageTimer <= damageTime )
			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_WALKBK, 1);
		else
			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_STUMBLE_WALK_BACKWARD, 1);
	}
	else
	{
		newspeed = pm->xyspeed / speed * 0.36000001;
		if ( ps->damageTimer <= damageTime )
			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_RUNBK, 1);
		else
			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_STUMBLE_BACKWARD, 1);
	}
LABEL_136:
	bFootStep = PM_ShouldMakeFootsteps(pm);
	iOldBobCycle = ps->bobCycle;
	ps->bobCycle = (unsigned char)(int)((float)iOldBobCycle + (float)pml->msec * newspeed);
	if ( pm->cmd.forwardmove || pm->cmd.rightmove )
	{
		if ( animResult < 0 )
			BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_IDLE, 1);
		PM_FootstepEvent(pm, pml, iOldBobCycle, ps->bobCycle, bFootStep);
	}
	else if ( pm->xyspeed <= 120.0 )
	{
		if ( ps->viewHeightTarget == 11 )
		{
			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_IDLEPRONE, 1);
		}
		else if ( ps->viewHeightTarget == 40 )
		{
			animResult = BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_IDLECR, 1);
		}
		if ( animResult < 0 )
		{
			if ( ps->damageTimer <= damageTime )
			{
LABEL_145:
				BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_IDLE, 1);
				return;
			}
LABEL_74:
			BG_AnimScriptAnimation(ps, AISTATE_COMBAT, ANIM_MT_FLINCH_FORWARD, 1);
		}
	}
}

void PM_DeadMove(playerState_s *ps, pml_t *pml)
{
	float length;
	float forward;

	if ( pml->walking )
	{
		length = VectorLength(ps->velocity);
		forward = length - 20.0;

		if ( forward > 0.0 )
		{
			Vec3Normalize(ps->velocity);
			VectorScale(ps->velocity, forward, ps->velocity);
		}
		else
		{
			VectorClear(ps->velocity);
		}
	}
}

void PM_CheckLadderMove(pmove_t *pm, pml_t *pml)
{
	int fellOffLadderInAir;
	playerState_s *ps;
	float tracedist;
	trace_t trace;
	vec3_t maxs;
	vec3_t mins;
	vec3_t vLadderCheckDir;
	vec3_t spot;

	ps = pm->ps;

	if ( pml->walking )
		ps->pm_flags &= ~0x40000u;

	if ( !ps->pm_time || (ps->pm_flags & 0x600) == 0 )
	{
		if ( pml->walking )
			tracedist = 8.0;
		else
			tracedist = 30.0;

		fellOffLadderInAir = 0;

		if ( (ps->pm_flags & 0x20) != 0 && ps->groundEntityNum == 1023 )
			fellOffLadderInAir = 1;

		if ( fellOffLadderInAir )
		{
			VectorCopyInverse(ps->vLadderVec, vLadderCheckDir);
		}
		else
		{
			vLadderCheckDir[0] = pml->forward[0];
			vLadderCheckDir[1] = pml->forward[1];
			vLadderCheckDir[2] = 0.0;

			Vec3Normalize(vLadderCheckDir);
		}
		if ( ps->pm_type <= 5 )
		{
			if ( (ps->pm_flags & 0x40000) != 0 || PM_GetEffectiveStance(ps) == 1 || pm->cmd.serverTime - ps->jumpTime <= 299 )
			{
				PM_ClearLadderFlag(ps);
			}
			else
			{
				VectorCopy(pm->mins, mins);

				mins[0] = mins[0] + 6.0;
				mins[1] = mins[1] + 6.0;
				mins[2] = 8.0;

				VectorCopy(pm->maxs, maxs);
				maxs[0] = maxs[0] - 6.0;
				maxs[1] = maxs[1] - 6.0;

				if ( mins[2] > maxs[2] )
					maxs[2] = mins[2];

				VectorMA(ps->origin, tracedist, vLadderCheckDir, spot);
				PM_playerTrace(pm, &trace, ps->origin, mins, maxs, spot, ps->clientNum, pm->tracemask);

				if ( trace.fraction < 1.0
				        && (trace.surfaceFlags & 8) != 0
				        && (!pml->walking || pm->cmd.forwardmove > 0)
				        && ((ps->pm_flags & 0x20) != 0
				            || (VectorCopy(trace.normal, ps->vLadderVec),
				                VectorCopyInverse(ps->vLadderVec, vLadderCheckDir),
				                VectorMA(ps->origin, tracedist, vLadderCheckDir, spot),
				                PM_playerTrace(pm, &trace, ps->origin, mins, maxs, spot, ps->clientNum, pm->tracemask),
				                trace.fraction < 1.0)
				            && (trace.surfaceFlags & 8) != 0) )
				{
					PM_SetLadderFlag(ps);
				}
				else
				{
					PM_ClearLadderFlag(ps);

					if ( fellOffLadderInAir )
						BG_AnimScriptEvent(ps, ANIM_ET_JUMP, 0, 1);
				}
			}
		}
		else
		{
			ps->groundEntityNum = 1023;
			pml->groundPlane = 0;
			pml->almostGroundPlane = 0;
			pml->walking = 0;
			PM_ClearLadderFlag(ps);
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

	if ( ps->pm_type > 5 )
		pmove->tracemask &= ~0x2000000u;

	if ( (ps->pm_flags & 1) != 0 )
	{
		if ( (pmove->cmd.forwardmove == pmove->oldcmd.forwardmove
		        || (forwardmove = (float)pmove->cmd.forwardmove,
		            absFmove = fabs(forwardmove),
		            oldFmove = (float)pmove->oldcmd.forwardmove,
		            absFmove <= fabs(oldFmove)))
		        && (pmove->cmd.rightmove == pmove->oldcmd.rightmove
		            || (rightmove = (float)pmove->cmd.rightmove,
		                absRmove = fabs(rightmove),
		                oldRmove = (float)pmove->oldcmd.rightmove,
		                absRmove <= fabs(oldRmove))) )
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

	if ( ps->pm_type != 5
	        && (ps->pm_flags & 0x1000) == 0
	        && (ps->weaponstate == WEAPON_READY || ps->weaponstate == WEAPON_FIRING)
	        && PM_WeaponAmmoAvailable(ps)
	        && (pmove->cmd.buttons & 1) != 0 )
	{
		ps->eFlags |= 0x40u;
	}

	if ( ps->pm_type <= 5 && (pmove->cmd.buttons & 0x4001) == 0 )
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
	if ( ps->pm_type > 5 )
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
	case 1:
	case 7:
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
	case 2:
		PM_ClearLadderFlag(ps);
		PM_UpdateAimDownSightFlag(pmove, &pml);
		PM_UpdatePlayerWalkingFlag(pmove);
		PM_NoclipMove(pmove, &pml);
		PM_DropTimers(ps, &pml);
		PM_UpdateAimDownSightLerp(pmove, &pml);
		break;
	case 3:
		PM_ClearLadderFlag(ps);
		PM_UpdateAimDownSightFlag(pmove, &pml);
		PM_UpdatePlayerWalkingFlag(pmove);
		PM_UFOMove(pmove, &pml);
		PM_DropTimers(ps, &pml);
		PM_UpdateAimDownSightLerp(pmove, &pml);
		break;
	case 4:
		PM_ClearLadderFlag(ps);
		PM_UpdateAimDownSightFlag(pmove, &pml);
		PM_UpdatePlayerWalkingFlag(pmove);
		PM_CheckDuck(pmove, &pml);
		PM_FlyMove(pmove, &pml);
		PM_DropTimers(ps, &pml);
		PM_UpdateAimDownSightLerp(pmove, &pml);
		break;
	case 5:
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
				if ( ps->pm_type == 6 )
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

void Pmove(pmove_t *pmove)
{
	int msec;
	playerState_s *ps;
	int time;

	ps = pmove->ps;
	time = pmove->cmd.serverTime;

	if ( time >= pmove->ps->commandTime )
	{
		if ( time > ps->commandTime + 1000 )
			ps->commandTime = time - 1000;

		pmove->numtouch = 0;

		while ( ps->commandTime != time )
		{
			msec = time - ps->commandTime;

			if ( msec > 66 )
				msec = 66;

			pmove->cmd.serverTime = ps->commandTime + msec;
			PmoveSingle(pmove);
			pmove->oldcmd = pmove->cmd;
		}
	}
}

float BG_GetSpeed(const playerState_s *ps, int time)
{
	if ( (ps->pm_flags & 0x20) != 0 )
	{
		if ( time - ps->jumpTime > 499 )
			return ps->velocity[2];
		else
			return 0.0;
	}
	else
	{
		return VectorLength(ps->velocity);
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

int PM_DamageLandingForSurface(pml_t *pml)
{
	return PM_GroundSurfaceType(pml) + 116;
}

int PM_HardLandingForSurface(pml_t *pml)
{
	return PM_GroundSurfaceType(pml) + 93;
}

int PM_MediumLandingForSurface(pml_t *pml)
{
	int iSurfType;

	iSurfType = PM_GroundSurfaceType(pml);

	if ( iSurfType )
		return iSurfType + 1;
	else
		return 0;
}

int PM_LightLandingForSurface(pml_t *pml)
{
	int iSurfType;

	iSurfType = PM_GroundSurfaceType(pml);

	if ( iSurfType )
		return iSurfType + 24;
	else
		return 0;
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
		t = (-vel - sqrt(den)) / (acc + acc);
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
					medDmg = PM_MediumLandingForSurface(pml);
					PM_AddEvent(pm, medDmg);
				}
			}
			else
			{
				lightDmg = PM_LightLandingForSurface(pml);
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
				absSpeedDrop = fabs(fSideSpeedTemp);

				if ( absSpeedDrop > fabs(fSpeedDrop) )
				{
					if ( fabs(fSpeedDrop) < 1.0 )
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