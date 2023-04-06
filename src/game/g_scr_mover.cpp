#include "../qcommon/qcommon.h"
#include "g_shared.h"

#ifdef TESTING_LIBRARY
#define g_entities ((gentity_t*)( 0x08665480 ))
#else
extern gentity_t g_entities[];
#endif

#ifdef TESTING_LIBRARY
#define level (*((level_locals_t*)( 0x0859B400 )))
#else
extern level_locals_t level;
#endif

scr_method_t scriptent_methods[] =
{
	{ "moveto", ScriptEntCmd_MoveTo, 0, },
	{ "movex", ScriptEntCmd_MoveX, 0, },
	{ "movey", ScriptEntCmd_MoveY, 0, },
	{ "movez", ScriptEntCmd_MoveZ, 0, },
	{ "movegravity", ScriptEntCmd_GravityMove, 0, },
	{ "rotateto", ScriptEntCmd_RotateTo, 0, },
	{ "rotatepitch", ScriptEntCmd_RotatePitch, 0, },
	{ "rotateyaw", ScriptEntCmd_RotateYaw, 0, },
	{ "rotateroll", ScriptEntCmd_RotateRoll, 0, },
	{ "rotatevelocity", ScriptEntCmd_RotateVelocity, 0, },
	{ "solid", ScriptEntCmd_Solid, 0, },
	{ "notsolid", ScriptEntCmd_NotSolid, 0, },
};

void ScriptEntCmdGetCommandTimes(float *pfTotalTime, float *pfAccelTime, float *pfDecelTime)
{
	signed int paramNum;

	*pfTotalTime = Scr_GetFloat(1u);

	if ( *pfTotalTime <= 0.0 )
		Scr_ParamError(1, "total time must be positive");

	paramNum = Scr_GetNumParam();

	if ( paramNum <= 2 )
	{
		*pfAccelTime = 0.0;
		*pfDecelTime = 0.0;
	}
	else
	{
		*pfAccelTime = Scr_GetFloat(2u);

		if ( *pfAccelTime < 0.0 )
			Scr_ParamError(2, "accel time must be nonnegative");

		if ( paramNum <= 3 )
		{
			*pfDecelTime = 0.0;
		}
		else
		{
			*pfDecelTime = Scr_GetFloat(3u);

			if ( *pfDecelTime < 0.0 )
				Scr_ParamError(3, "decel time must be nonnegative");
		}
	}

	if ( *pfAccelTime + *pfDecelTime > *pfTotalTime )
		Scr_Error("accel time plus decel time is greater than total time");
}

int ScriptMover_UpdateMove(trajectory_t *pTr, float *vCurrPos, float fSpeed, float fMidTime, float fDecelTime, float *vPos1, float *vPos2, float *vPos3)
{
	int trDuration;
	vec3_t vMove;
	float fDelta;

	trDuration = (int)(fMidTime * 1000.0);

	if ( pTr->trType == TR_ACCELERATE && trDuration > 0 )
	{
		pTr->trTime = level.time;
		pTr->trDuration = trDuration;
		VectorCopy(vPos1, pTr->trBase);
		VectorSubtract(vPos2, vPos1, vMove);
		fDelta = 1000.0 / (long double)trDuration;
		VectorScale(vMove, fDelta, pTr->trDelta);
		pTr->trType = TR_LINEAR_STOP;

		return 0;
	}

	if ( (pTr->trType == TR_ACCELERATE && trDuration <= 0 || pTr->trType == TR_LINEAR_STOP) && fDecelTime > 0.0 )
	{
		pTr->trTime = level.time;
		pTr->trDuration = (int)(fDecelTime * 1000.0);
		VectorCopy(vPos2, pTr->trBase);
		VectorSubtract(vPos3, vPos2, vMove);
		Vec3Normalize(vMove);
		VectorScale(vMove, fSpeed, vMove);
		VectorCopy(vMove, pTr->trDelta);
		pTr->trType = TR_DECCELERATE;

		return 0;
	}

	if ( pTr->trType == TR_GRAVITY )
		BG_EvaluateTrajectory(pTr, level.time, pTr->trBase);
	else
		VectorCopy(vPos3, pTr->trBase);

	pTr->trTime = level.time;
	pTr->trType = TR_STATIONARY;

	return 1;
}

void Reached_ScriptMover(gentity_s *pEnt)
{
	int bMoveFinished;

	if ( pEnt->s.pos.trType )
	{
		if ( pEnt->s.pos.trTime + pEnt->s.pos.trDuration <= level.time )
		{
			bMoveFinished = ScriptMover_UpdateMove(
			                    &pEnt->s.pos,
			                    pEnt->r.currentOrigin,
			                    pEnt->mover.speed,
			                    pEnt->mover.midTime,
			                    pEnt->mover.decelTime,
			                    pEnt->mover.pos1,
			                    pEnt->mover.pos2,
			                    pEnt->mover.pos3);

			BG_EvaluateTrajectory(&pEnt->s.pos, level.time, pEnt->r.currentOrigin);
			SV_LinkEntity(pEnt);

			if ( bMoveFinished )
				Scr_Notify(pEnt, scr_const.movedone, 0);
		}
	}

	if ( pEnt->s.apos.trType && pEnt->s.apos.trTime + pEnt->s.apos.trDuration <= level.time )
	{
		bMoveFinished = ScriptMover_UpdateMove(
		                    &pEnt->s.apos,
		                    pEnt->r.currentAngles,
		                    pEnt->mover.aSpeed,
		                    pEnt->mover.aMidTime,
		                    pEnt->mover.aDecelTime,
		                    pEnt->mover.apos1,
		                    pEnt->mover.apos2,
		                    pEnt->mover.apos3);

		BG_EvaluateTrajectory(&pEnt->s.apos, level.time, pEnt->r.currentAngles);
		SV_LinkEntity(pEnt);

		if ( bMoveFinished )
		{
			pEnt->r.currentAngles[0] = AngleNormalize180(pEnt->r.currentAngles[0]);
			pEnt->r.currentAngles[1] = AngleNormalize360(pEnt->r.currentAngles[1]);
			pEnt->r.currentAngles[2] = AngleNormalize180(pEnt->r.currentAngles[2]);

			Scr_Notify(pEnt, scr_const.rotatedone, 0);
		}
	}
}

void ScriptMover_SetupMove(trajectory_t *pTr, float *vPos, float fTotalTime, float fAccelTime, float fDecelTime, float *vCurrPos, float *pfSpeed, float *pfMidTime, float *pfDecelTime, float *vPos1, float *vPos2, float *vPos3)
{
	vec3_t vMaxSpeed;
	vec3_t vMove;
	float fDelta;
	float fDist;

	VectorSubtract(vPos, vCurrPos, vMove);

	if ( pTr->trType )
		BG_EvaluateTrajectory(pTr, level.time, vCurrPos);

	if ( fAccelTime == 0.0 && fDecelTime == 0.0 )
	{
		pTr->trTime = level.time;
		pTr->trDuration = (int)(fTotalTime * 1000.0);
		*pfMidTime = fTotalTime;
		*pfDecelTime = 0.0;
		VectorCopy(vPos, vPos3);
		VectorCopy(vCurrPos, pTr->trBase);
		fDelta = 1000.0 / (long double)pTr->trDuration;
		VectorScale(vMove, fDelta, pTr->trDelta);
		pTr->trType = TR_LINEAR_STOP;
		BG_EvaluateTrajectory(pTr, level.time, vCurrPos);
	}
	else
	{
		*pfMidTime = fTotalTime - fAccelTime - fDecelTime;
		*pfDecelTime = fDecelTime;
		fDist = VectorLength(vMove);
		*pfSpeed = (fDist + fDist) / (fTotalTime + fTotalTime - fAccelTime - fDecelTime);
		Vec3NormalizeTo(vMove, vMaxSpeed);
		VectorScale(vMaxSpeed, *pfSpeed, vMaxSpeed);

		if ( fAccelTime == 0.0 )
		{
			VectorCopy(vCurrPos, vPos1);

			if ( *pfMidTime == 0.0 )
			{
				pTr->trTime = level.time;
				pTr->trDuration = (int)(*pfDecelTime * 1000.0);
				VectorCopy(vCurrPos, pTr->trBase);
				VectorCopy(vMaxSpeed, pTr->trDelta);
				pTr->trType = TR_DECCELERATE;
			}
			else
			{
				pTr->trTime = level.time;
				pTr->trDuration = (int)(*pfMidTime * 1000.0);
				VectorCopy(vCurrPos, pTr->trBase);
				VectorScale(vMaxSpeed, *pfMidTime, vMove);
				fDelta = 1000.0 / (long double)pTr->trDuration;
				VectorScale(vMove, fDelta, pTr->trDelta);
				pTr->trType = TR_LINEAR_STOP;
			}
		}
		else
		{
			pTr->trTime = level.time;
			pTr->trDuration = (int)(fAccelTime * 1000.0);
			VectorCopy(vCurrPos, pTr->trBase);
			VectorCopy(vMaxSpeed, pTr->trDelta);
			pTr->trType = TR_ACCELERATE;
			BG_EvaluateTrajectory(pTr, level.time + pTr->trDuration, vPos1);
		}

		VectorMA(vPos1, *pfMidTime, vMaxSpeed, vPos2);
		VectorCopy(vPos, vPos3);
		BG_EvaluateTrajectory(pTr, level.time, vCurrPos);
	}
}

void ScriptMover_SetupMoveSpeed(trajectory_t *pTr, float *vSpeed, float fTotalTime, float fAccelTime, float fDecelTime, float *vCurrPos, float *pfSpeed, float *pfMidTime, float *pfDecelTime, float *vPos1, float *vPos2, float *vPos3)
{
	trajectory_t tr;

	if ( pTr->trType )
		BG_EvaluateTrajectory(pTr, level.time, vCurrPos);

	if ( fAccelTime == 0.0 && fDecelTime == 0.0 )
	{
		pTr->trTime = level.time;
		pTr->trDuration = (int)(fTotalTime * 1000.0);
		*pfMidTime = fTotalTime;
		*pfDecelTime = 0.0;
		VectorCopy(vCurrPos, pTr->trBase);
		VectorCopy(vSpeed, pTr->trDelta);
		pTr->trType = TR_LINEAR_STOP;
		BG_EvaluateTrajectory(pTr, level.time, vCurrPos);
		BG_EvaluateTrajectory(pTr, level.time + pTr->trDuration, vPos3);
	}
	else
	{
		*pfMidTime = fTotalTime - fAccelTime - fDecelTime;
		*pfDecelTime = fDecelTime;
		*pfSpeed = VectorLength(vSpeed);

		if ( fAccelTime == 0.0 )
		{
			VectorCopy(vCurrPos, vPos1);

			if ( *pfMidTime == 0.0 )
			{
				pTr->trTime = level.time;
				pTr->trDuration = (int)(*pfDecelTime * 1000.0);
				VectorCopy(vCurrPos, pTr->trBase);
				VectorCopy(vSpeed, pTr->trDelta);
				pTr->trType = TR_DECCELERATE;
			}
			else
			{
				pTr->trTime = level.time;
				pTr->trDuration = (int)(*pfMidTime * 1000.0);
				VectorCopy(vCurrPos, pTr->trBase);
				VectorCopy(vSpeed, pTr->trDelta);
				pTr->trType = TR_LINEAR_STOP;
			}
		}
		else
		{
			pTr->trTime = level.time;
			pTr->trDuration = (int)(fAccelTime * 1000.0);
			VectorCopy(vCurrPos, pTr->trBase);
			VectorCopy(vSpeed, pTr->trDelta);
			pTr->trType = TR_ACCELERATE;
			BG_EvaluateTrajectory(pTr, level.time + pTr->trDuration, vPos1);
		}

		VectorMA(vPos1, *pfMidTime, vSpeed, vPos2);

		if ( *pfDecelTime == 0.0 )
		{
			VectorCopy(vPos2, vPos3);
		}
		else
		{
			tr.trType = TR_DECCELERATE;
			tr.trTime = level.time;
			tr.trDuration = (int)(*pfDecelTime * 1000.0);
			VectorCopy(vPos2, tr.trBase);
			VectorCopy(vSpeed, tr.trDelta);
			BG_EvaluateTrajectory(&tr, level.time + tr.trDuration, vPos3);
		}

		BG_EvaluateTrajectory(pTr, level.time, vCurrPos);
	}
}

void ScriptMover_Move(gentity_s *pEnt, float *vPos, const float fTotalTime, const float fAccelTime, const float fDecelTime)
{
	ScriptMover_SetupMove(
	    &pEnt->s.pos,
	    vPos,
	    fTotalTime,
	    fAccelTime,
	    fDecelTime,
	    pEnt->r.currentOrigin,
	    &pEnt->mover.speed,
	    &pEnt->mover.midTime,
	    &pEnt->mover.decelTime,
	    pEnt->mover.pos1,
	    pEnt->mover.pos2,
	    pEnt->mover.pos3);

	SV_LinkEntity(pEnt);
}

void ScriptMover_Rotate(gentity_s *pEnt, float *vRot, float fTotalTime, float fAccelTime, float fDecelTime)
{
	ScriptMover_SetupMove(
	    &pEnt->s.apos,
	    vRot,
	    fTotalTime,
	    fAccelTime,
	    fDecelTime,
	    pEnt->r.currentAngles,
	    &pEnt->mover.aSpeed,
	    &pEnt->mover.aMidTime,
	    &pEnt->mover.aDecelTime,
	    pEnt->mover.apos1,
	    pEnt->mover.apos2,
	    pEnt->mover.apos3);

	SV_LinkEntity(pEnt);
}

void ScriptMover_RotateSpeed(gentity_s *pEnt, float *vRotSpeed, float fTotalTime, float fAccelTime, float fDecelTime)
{
	ScriptMover_SetupMoveSpeed(
	    &pEnt->s.apos,
	    vRotSpeed,
	    fTotalTime,
	    fAccelTime,
	    fDecelTime,
	    pEnt->r.currentAngles,
	    &pEnt->mover.aSpeed,
	    &pEnt->mover.aMidTime,
	    &pEnt->mover.aDecelTime,
	    pEnt->mover.apos1,
	    pEnt->mover.apos2,
	    pEnt->mover.apos3);

	SV_LinkEntity(pEnt);
}

void ScriptEntCmd_MoveTo(scr_entref_t entref)
{
	gentity_s *pEnt;
	vec3_t vPos;
	float pfDecelTime;
	float pfAccelTime;
	float fTotalTime;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		pEnt = 0;
	}
	else
	{
		pEnt = &g_entities[entref.entnum];

		if ( pEnt->classname != scr_const.script_brushmodel
		        && pEnt->classname != scr_const.script_model
		        && pEnt->classname != scr_const.script_origin )
		{
			Scr_ObjectError(va("entity %i is not a script_brushmodel, script_model, or script_origin", entref.entnum));
		}
	}

	Scr_GetVector(0, vPos);
	ScriptEntCmdGetCommandTimes(&fTotalTime, &pfAccelTime, &pfDecelTime);
	ScriptMover_Move(pEnt, vPos, fTotalTime, pfAccelTime, pfDecelTime);
}

void ScriptEnt_MoveAxis(scr_entref_t entref, int iAxis)
{
	gentity_s *pEnt;
	vec3_t vPos;
	float pfDecelTime;
	float pfAccelTime;
	float pfTotalTime;
	float fMove;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		pEnt = 0;
	}
	else
	{
		pEnt = &g_entities[entref.entnum];

		if ( pEnt->classname != scr_const.script_brushmodel
		        && pEnt->classname != scr_const.script_model
		        && pEnt->classname != scr_const.script_origin )
		{
			Scr_ObjectError(va("entity %i is not a script_brushmodel, script_model, or script_origin", entref.entnum));
		}
	}

	fMove = Scr_GetFloat(0);
	ScriptEntCmdGetCommandTimes(&pfTotalTime, &pfAccelTime, &pfDecelTime);
	VectorCopy(pEnt->r.currentOrigin, vPos);
	vPos[iAxis] = vPos[iAxis] + fMove;
	ScriptMover_Move(pEnt, vPos, pfTotalTime, pfAccelTime, pfDecelTime);
}

void ScriptEntCmd_MoveX(scr_entref_t entref)
{
	ScriptEnt_MoveAxis(entref, 0);
}

void ScriptEntCmd_MoveY(scr_entref_t entref)
{
	ScriptEnt_MoveAxis(entref, 1);
}

void ScriptEntCmd_MoveZ(scr_entref_t entref)
{
	ScriptEnt_MoveAxis(entref, 2);
}

void Scr_MoveGravity(gentity_s *ent, float *vEnd, float time)
{
	ent->s.pos.trTime = level.time;
	ent->s.pos.trDuration = (int)(time * 1000.0);
	VectorCopy(ent->r.currentOrigin, ent->s.pos.trBase);
	VectorCopy(vEnd, ent->s.pos.trDelta);
	ent->s.pos.trType = TR_GRAVITY;
	BG_EvaluateTrajectory(&ent->s.pos, level.time, ent->r.currentOrigin);
	SV_LinkEntity(ent);
}

void ScriptEntCmd_GravityMove(scr_entref_t entRef)
{
	gentity_s *ent;
	vec3_t vEnd;
	float time;

	if ( entRef.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entRef.entnum];

		if ( ent->classname != scr_const.script_brushmodel
		        && ent->classname != scr_const.script_model
		        && ent->classname != scr_const.script_origin )
		{
			Scr_ObjectError(va("entity %i is not a script_brushmodel, script_model, or script_origin", entRef.entnum));
		}
	}

	Scr_GetVector(0, vEnd);
	time = Scr_GetFloat(1u);
	Scr_MoveGravity(ent, vEnd, time);
}

void ScriptEntCmd_RotateTo(scr_entref_t entref)
{
	gentity_s *pEnt;
	vec3_t vRot;
	vec3_t vDest;
	float pfDecelTime;
	float pfAccelTime;
	float pfTotalTime;
	int i;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		pEnt = 0;
	}
	else
	{
		pEnt = &g_entities[entref.entnum];

		if ( pEnt->classname != scr_const.script_brushmodel
		        && pEnt->classname != scr_const.script_model
		        && pEnt->classname != scr_const.script_origin )
		{
			Scr_ObjectError(va("entity %i is not a script_brushmodel, script_model, or script_origin", entref.entnum));
		}
	}

	Scr_GetVector(0, vDest);
	ScriptEntCmdGetCommandTimes(&pfTotalTime, &pfAccelTime, &pfDecelTime);

	for ( i = 0; i < 3; ++i )
	{
		vRot[i] = AngleSubtract(vDest[i], pEnt->r.currentAngles[i]) + pEnt->r.currentAngles[i];
	}

	ScriptMover_Rotate(pEnt, vRot, pfTotalTime, pfAccelTime, pfDecelTime);
}

void ScriptEnt_RotateAxis(scr_entref_t entref, int iAxis)
{
	gentity_s *pEnt;
	vec3_t vRot;
	float pfDecelTime;
	float pfAccelTime;
	float pfTotalTime;
	float fMove;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		pEnt = 0;
	}
	else
	{
		pEnt = &g_entities[entref.entnum];

		if ( pEnt->classname != scr_const.script_brushmodel
		        && pEnt->classname != scr_const.script_model
		        && pEnt->classname != scr_const.script_origin )
		{
			Scr_ObjectError(va("entity %i is not a script_brushmodel, script_model, or script_origin", entref.entnum));
		}
	}

	fMove = Scr_GetFloat(0);
	ScriptEntCmdGetCommandTimes(&pfTotalTime, &pfAccelTime, &pfDecelTime);
	VectorCopy(pEnt->r.currentAngles, vRot);
	vRot[iAxis] = vRot[iAxis] + fMove;
	ScriptMover_Rotate(pEnt, vRot, pfTotalTime, pfAccelTime, pfDecelTime);
}

void ScriptEntCmd_RotatePitch(scr_entref_t entref)
{
	ScriptEnt_RotateAxis(entref, 0);
}

void ScriptEntCmd_RotateYaw(scr_entref_t entref)
{
	ScriptEnt_RotateAxis(entref, 1);
}

void ScriptEntCmd_RotateRoll(scr_entref_t entref)
{
	ScriptEnt_RotateAxis(entref, 2);
}

void ScriptEntCmd_RotateVelocity(scr_entref_t entref)
{
	gentity_s *pEnt;
	vec3_t vSpeed;
	float pfDecelTime;
	float pfAccelTime;
	float fTotalTime;

	if ( entref.classnum )
	{
		Scr_ObjectError("not an entity");
		pEnt = 0;
	}
	else
	{
		pEnt = &g_entities[entref.entnum];

		if ( pEnt->classname != scr_const.script_brushmodel
		        && pEnt->classname != scr_const.script_model
		        && pEnt->classname != scr_const.script_origin )
		{
			Scr_ObjectError(va("entity %i is not a script_brushmodel, script_model, or script_origin", entref.entnum));
		}
	}

	Scr_GetVector(0, vSpeed);
	ScriptEntCmdGetCommandTimes(&fTotalTime, &pfAccelTime, &pfDecelTime);
	ScriptMover_RotateSpeed(pEnt, vSpeed, fTotalTime, pfAccelTime, pfDecelTime);
}

void ScriptEntCmd_Solid(scr_entref_t entRef)
{
	gentity_s *ent;

	if ( entRef.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entRef.entnum];
		if ( ent->classname != scr_const.script_brushmodel
		        && ent->classname != scr_const.script_model
		        && ent->classname != scr_const.script_origin )
		{
			Scr_ObjectError(va("entity %i is not a script_brushmodel, script_model, or script_origin", entRef.entnum));
		}
	}

	if ( ent->classname == scr_const.script_origin )
	{
		Com_DPrintf("cannot use the solid/notsolid commands on a script_origin entity( number %i )\n", ent->s.number);
	}
	else
	{
		if ( ent->classname == scr_const.script_model )
		{
			ent->r.contents = 8320;
		}
		else
		{
			ent->r.contents = 1;
			ent->s.eFlags &= ~1u;
		}

		SV_LinkEntity(ent);
	}
}

void ScriptEntCmd_NotSolid(scr_entref_t entRef)
{
	gentity_s *ent;

	if ( entRef.classnum )
	{
		Scr_ObjectError("not an entity");
		ent = 0;
	}
	else
	{
		ent = &g_entities[entRef.entnum];

		if ( ent->classname != scr_const.script_brushmodel
		        && ent->classname != scr_const.script_model
		        && ent->classname != scr_const.script_origin )
		{
			Scr_ObjectError(va("entity %i is not a script_brushmodel, script_model, or script_origin", entRef.entnum));
		}
	}

	if ( ent->classname == scr_const.script_origin )
	{
		Com_DPrintf("cannot use the solid/notsolid commands on a script_origin entity( number %i )\n", ent->s.number);
	}
	else
	{
		ent->r.contents = 0;

		if ( ent->classname != scr_const.script_model )
			ent->s.eFlags |= 1u;

		SV_LinkEntity(ent);
	}
}

void (*ScriptEnt_GetMethod(const char **pName))(scr_entref_t)
{
	const char *name;
	unsigned int i;

	name = *pName;

	for ( i = 0; i < COUNT_OF(scriptent_methods); ++i )
	{
		if ( !strcmp(name, scriptent_methods[i].name) )
		{
			*pName = scriptent_methods[i].name;
			return scriptent_methods[i].call;
		}
	}

	return NULL;
}

void InitScriptMover(gentity_s *pSelf)
{
	int v1;
	int v2;
	int v3;
	int v4;
	int v5;
	int v6;
	vec3_t color;
	float light;

	if ( level.spawnVars.spawnVarsValid )
	{
		v6 = G_SpawnFloat("light", "100", &light);
		v5 = G_SpawnVector("color", "1 1 1", color);

		if ( v6 || v5 )
		{
			v4 = (int)(color[0] * 255.0);

			if ( v4 > 255 )
				v4 = 255;

			v3 = (int)(color[1] * 255.0);

			if ( v3 > 255 )
				v3 = 255;

			v2 = (int)(color[2] * 255.0);

			if ( v2 > 255 )
				v2 = 255;

			v1 = (int)(light / 4.0);

			if ( v1 > 255 )
				v1 = 255;

			pSelf->s.constantLight = (v2 << 16) | v4 | (v3 << 8) | (v1 << 24);
		}
	}

	pSelf->handler = 5;
	pSelf->r.svFlags = 0;
	pSelf->s.eType = ET_SCRIPTMOVER;
	VectorCopy(pSelf->r.currentOrigin, pSelf->s.pos.trBase);
	pSelf->s.pos.trType = TR_STATIONARY;
	VectorCopy(pSelf->r.currentAngles, pSelf->s.apos.trBase);
	pSelf->s.apos.trType = TR_STATIONARY;
	pSelf->flags |= 0x1000u;
}

void SP_script_brushmodel(gentity_s *ent)
{
	SV_SetBrushModel(ent);
	InitScriptMover(ent);
	ent->r.contents = 1;
	SV_LinkEntity(ent);
}

void SP_script_model(gentity_s *ent)
{
	G_DObjUpdate(ent);
	InitScriptMover(ent);
	ent->r.svFlags |= 4u;
	ent->r.contents = 8320;
	SV_LinkEntity(ent);
}

void SP_script_origin(gentity_s *ent)
{
	InitScriptMover(ent);
	ent->r.contents = 0;
	SV_LinkEntity(ent);

	if ( ent->s.constantLight )
		ent->s.eFlags |= 0x20u;
	else
		ent->r.svFlags |= 1u;
}