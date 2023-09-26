#include "../qcommon/qcommon.h"
#include "bg_public.h"

void BG_CalculateViewMovement_DamageKick(viewState_t *vs, float *angles)
{
	float tmp;
	unsigned int weapIndex;
	WeaponDef *weapDef;
	playerState_s *ps;
	float fFactor;
	float fTime;
	float fRatio;
	float fRatioa;
	float fRatiob;
	float scale;
	float fFrac;
	float fRatioc;

	if ( vs->damageTime )
	{
		ps = vs->ps;
		weapIndex = BG_GetViewmodelWeaponIndex(vs->ps);
		weapDef = BG_GetWeaponDef(weapIndex);
		fFactor = 1.0 - ps->fWeaponPosFrac * 0.5;
		if ( ps->fWeaponPosFrac != 0.0 && weapDef->adsOverlayReticle )
			fFactor = (ps->fWeaponPosFrac * 0.5 + 1.0) * fFactor;
		fTime = (float)(vs->time - vs->damageTime);
		if ( fTime >= 100.0 )
		{
			scale = 1.0 - (fTime - 100.0) / 400.0;
			if ( scale > 0.0 )
			{
				tmp = 1.0 - scale;
				fFrac = 1.0 - GetLeanFraction(tmp);
				fRatioc = fFrac * fFactor;
				*angles = fRatioc * vs->v_dmg_pitch + *angles;
				angles[2] = fRatioc * vs->v_dmg_roll + angles[2];
			}
		}
		else
		{
			fRatio = fTime / 100.0;
			fRatioa = GetLeanFraction(fRatio);
			fRatiob = fRatioa * fFactor;
			*angles = fRatiob * vs->v_dmg_pitch + *angles;
			angles[2] = fRatiob * vs->v_dmg_roll + angles[2];
		}
	}
}

void BG_CalculateViewMovement_IdleAngles(viewState_t *vs, float *angles)
{
	float hipIdleSpeed;
	unsigned int weapIndex;
	playerState_s *ps;
	float idleProneFactor;
	float hipIdleAmount;
	float v7;
	float v8;
	float fTargScale;
	float v10;
	float v11;
	WeaponDef *weapDef;

	ps = vs->ps;
	weapIndex = BG_GetViewmodelWeaponIndex(vs->ps);
	weapDef = BG_GetWeaponDef(weapIndex);

	if ( weapDef->adsOverlayReticle )
	{
		if ( BG_IsAimDownSightWeapon(weapIndex) )
		{
			hipIdleAmount = (weapDef->adsIdleAmount - weapDef->hipIdleAmount) * ps->fWeaponPosFrac + weapDef->hipIdleAmount;
			hipIdleSpeed = (weapDef->adsIdleSpeed - weapDef->hipIdleSpeed) * ps->fWeaponPosFrac + weapDef->hipIdleSpeed;
		}
		else if ( weapDef->hipIdleAmount == 0.0 )
		{
			hipIdleAmount = 80.0;
			hipIdleSpeed = 1.0;
		}
		else
		{
			hipIdleAmount = weapDef->hipIdleAmount;
			hipIdleSpeed = weapDef->hipIdleSpeed;
		}
		if ( (ps->eFlags & 8) != 0 )
		{
			idleProneFactor = weapDef->idleProneFactor;
		}
		else if ( (ps->eFlags & 4) != 0 )
		{
			idleProneFactor = weapDef->idleCrouchFactor;
		}
		else
		{
			idleProneFactor = 1.0;
		}
		if ( weapDef->adsOverlayReticle && ps->fWeaponPosFrac != 0.0 && idleProneFactor != vs->fLastIdleFactor )
		{
			if ( idleProneFactor <= (float)vs->fLastIdleFactor )
			{
				vs->fLastIdleFactor = vs->fLastIdleFactor - vs->frametime * 0.5;
				if ( idleProneFactor > (float)vs->fLastIdleFactor )
					vs->fLastIdleFactor = idleProneFactor;
			}
			else
			{
				vs->fLastIdleFactor = vs->frametime * 0.5 + vs->fLastIdleFactor;
				if ( vs->fLastIdleFactor > (float)idleProneFactor )
					vs->fLastIdleFactor = idleProneFactor;
			}
		}
		v7 = hipIdleAmount * vs->fLastIdleFactor;
		v8 = v7 * ps->fWeaponPosFrac;
		fTargScale = v8 * ps->holdBreathScale;
		*vs->weapIdleTime += (int)(ps->holdBreathScale * vs->frametime * 1000.0 * hipIdleSpeed);
		v10 = sin((float)*vs->weapIdleTime * 0.00069999998);
		angles[1] = fTargScale * v10 * 0.0099999998 + angles[1];
		v11 = sin((float)*vs->weapIdleTime * 0.001);
		*angles = fTargScale * v11 * 0.0099999998 + *angles;
	}
}

void BG_CalculateViewMovement_BobAngles(viewState_t *vs, float *angles)
{
	float tempFactor;
	float VerticalBobFactor;
	float HorizontalBobFactor;
	unsigned int weapIndex;
	WeaponDef *weapDef;
	playerState_s *ps;
	float fBobCycle;
	vec3_t vAngOfs;
	float speed;
	float cycle;
	float fWeaponPosFrac;

	ps = vs->ps;
	weapIndex = BG_GetViewmodelWeaponIndex(vs->ps);
	weapDef = BG_GetWeaponDef(weapIndex);

	if ( weapDef->adsOverlayReticle )
	{
		fBobCycle = (float)LOBYTE(ps->bobCycle) / 255.0 * 3.141592653589793
		            + (float)LOBYTE(ps->bobCycle) / 255.0 * 3.141592653589793
		            + 6.283185307179586;
		cycle = fBobCycle + 0.7853981633974483 + 6.283185307179586;
		speed = vs->xyspeed * 0.16;
		VerticalBobFactor = BG_GetVerticalBobFactor(ps, cycle, speed, 10.0);
		vAngOfs[0] = -VerticalBobFactor;
		HorizontalBobFactor = BG_GetHorizontalBobFactor(ps, cycle, speed, 10.0);
		vAngOfs[1] = -HorizontalBobFactor;
		cycle = cycle - 0.4712389167638204;
		speed = speed * 1.5;
		tempFactor = BG_GetHorizontalBobFactor(ps, cycle, speed, 10.0);
		vAngOfs[2] = I_fmin(tempFactor, 0.0);
		fWeaponPosFrac = ps->fWeaponPosFrac;

		if ( fWeaponPosFrac != 0.0 )
		{
			speed = 1.0 - (1.0 - weapDef->adsBobFactor) * fWeaponPosFrac;
			vAngOfs[0] = vAngOfs[0] * speed;
			vAngOfs[1] = vAngOfs[1] * speed;
			vAngOfs[2] = vAngOfs[2] * speed;
		}

		VectorScale(vAngOfs, fWeaponPosFrac, vAngOfs);
		VectorAdd(angles, vAngOfs, angles);
	}
}

void BG_CalculateViewMovement_AdsBob(viewState_t *vs, float *angles)
{
	unsigned int weapIndex;
	WeaponDef *weapDef;
	playerState_s *ps;
	float bobCycle;
	float HorizontalBobFactor;
	float viewBob;
	float VerticalBobFactor;
	float adsBobFactor;

	ps = vs->ps;
	weapIndex = BG_GetViewmodelWeaponIndex(vs->ps);
	weapDef = BG_GetWeaponDef(weapIndex);

	if ( (ps->eFlags & 0x300) == 0 && ps->fWeaponPosFrac != 0.0 && weapDef->adsViewBobMult != 0.0 )
	{
		bobCycle = (float)LOBYTE(ps->bobCycle) / 255.0 * 3.141592653589793
		           + (float)LOBYTE(ps->bobCycle) / 255.0 * 3.141592653589793
		           + 6.283185307179586;
		VerticalBobFactor = BG_GetVerticalBobFactor(ps, bobCycle, vs->xyspeed, 45.0);
		adsBobFactor = ps->fWeaponPosFrac * weapDef->adsViewBobMult * VerticalBobFactor;
		*angles = *angles - adsBobFactor;
		HorizontalBobFactor = BG_GetHorizontalBobFactor(ps, bobCycle, vs->xyspeed, 45.0);
		viewBob = ps->fWeaponPosFrac * weapDef->adsViewBobMult * HorizontalBobFactor;
		angles[1] = angles[1] - viewBob;
	}
}

void BG_CalculateViewMovementAngles(viewState_t *vs, float *angles)
{
	VectorClear(angles);
	BG_CalculateViewMovement_DamageKick(vs, angles);
	BG_CalculateViewMovement_IdleAngles(vs, angles);
	BG_CalculateViewMovement_BobAngles(vs, angles);
	BG_CalculateViewMovement_AdsBob(vs, angles);
}

void BG_CalculateWeaponMovement_Base_Internal(weaponState_t *ws, float *angles)
{
	float max;
	unsigned int weapIndex;
	WeaponDef *weapDef;
	const playerState_s *ps;
	vec3_t targetPos;
	float proneRotMinSpeed;
	float frac;
	float speed;
	int i;

	ps = ws->ps;
	weapIndex = BG_GetViewmodelWeaponIndex(ws->ps);
	weapDef = BG_GetWeaponDef(weapIndex);

	if ( (ps->eFlags & 8) != 0 )
	{
		proneRotMinSpeed = weapDef->proneRotMinSpeed;
	}
	else if ( (ps->eFlags & 4) != 0 )
	{
		proneRotMinSpeed = weapDef->duckedRotMinSpeed;
	}
	else
	{
		proneRotMinSpeed = weapDef->standRotMinSpeed;
	}
	if ( ws->xyspeed <= (float)proneRotMinSpeed || ps->weaponstate == WEAPON_RELOADING )
	{
		VectorClear(targetPos);
	}
	else
	{
		speed = (ws->xyspeed - proneRotMinSpeed) / ((float)ps->speed - proneRotMinSpeed);
		max = I_fmax(0.0, speed);
		speed = I_fmin(1.0, max);
		if ( (ps->eFlags & 8) != 0 )
		{
			VectorScale(&weapDef->proneRotP, speed, targetPos);
		}
		else if ( (ps->eFlags & 4) != 0 )
		{
			VectorScale(&weapDef->duckedRotP, speed, targetPos);
		}
		else
		{
			VectorScale(&weapDef->standRotP, speed, targetPos);
		}
	}
	if ( ps->fWeaponPosFrac != 0.0 )
	{
		frac = 1.0 - ps->fWeaponPosFrac;
		VectorScale(targetPos, frac, targetPos);
	}
	for ( i = 0; i <= 2; ++i )
	{
		if ( ws->vLastMoveAng[i] != targetPos[i] )
		{
			if ( ps->viewHeightCurrent == 11.0 )
				frac = (targetPos[i] - ws->vLastMoveAng[i]) * ws->frametime * weapDef->posProneRotRate;
			else
				frac = (targetPos[i] - ws->vLastMoveAng[i]) * ws->frametime * weapDef->posRotRate;
			if ( targetPos[i] <= (float)ws->vLastMoveAng[i] )
			{
				if ( frac > ws->frametime * -0.1 )
					frac = ws->frametime * -0.1;
				ws->vLastMoveAng[i] = ws->vLastMoveAng[i] + frac;
				if ( targetPos[i] > (float)ws->vLastMoveAng[i] )
					ws->vLastMoveAng[i] = targetPos[i];
			}
			else
			{
				if ( ws->frametime * 0.1 > frac )
					frac = ws->frametime * 0.1;
				ws->vLastMoveAng[i] = ws->vLastMoveAng[i] + frac;
				if ( ws->vLastMoveAng[i] > (float)targetPos[i] )
					ws->vLastMoveAng[i] = targetPos[i];
			}
		}
	}
	if ( ps->fWeaponPosFrac == 0.0 )
	{
		VectorAdd(angles, ws->vLastMoveAng, angles);
	}
	else if ( ps->fWeaponPosFrac < 0.5 )
	{
		frac = 1.0 - (ps->fWeaponPosFrac + ps->fWeaponPosFrac);
		VectorMA(angles, frac, ws->vLastMoveAng, angles);
	}
}

void BG_CalculateWeaponMovement_Base(weaponState_t *ws, float *angles)
{
	unsigned int weapIndex;
	WeaponDef *weapDef;
	const playerState_s *ps;

	ps = ws->ps;
	weapIndex = BG_GetViewmodelWeaponIndex(ws->ps);
	weapDef = BG_GetWeaponDef(weapIndex);

	if ( BG_IsAimDownSightWeapon(weapIndex) )
		*angles = ps->fWeaponPosFrac * weapDef->adsAimPitch + *angles;

	BG_CalculateWeaponMovement_Base_Internal(ws, angles);
}

void BG_CalculateWeaponMovement_IdleAngles(weaponState_t *ws, float *angles)
{
	float hipIdleSpeed;
	unsigned int weapIndex;
	const playerState_s *ps;
	float idleProneFactor;
	float hipIdleAmount;
	float fTargScale;
	float v8;
	float v9;
	float v10;
	WeaponDef *weapDef;

	ps = ws->ps;
	weapIndex = BG_GetViewmodelWeaponIndex(ws->ps);
	weapDef = BG_GetWeaponDef(weapIndex);
	if ( BG_IsAimDownSightWeapon(weapIndex) )
	{
		hipIdleAmount = (weapDef->adsIdleAmount - weapDef->hipIdleAmount) * ps->fWeaponPosFrac + weapDef->hipIdleAmount;
		hipIdleSpeed = (weapDef->adsIdleSpeed - weapDef->hipIdleSpeed) * ps->fWeaponPosFrac + weapDef->hipIdleSpeed;
	}
	else if ( weapDef->hipIdleAmount == 0.0 )
	{
		hipIdleAmount = 80.0;
		hipIdleSpeed = 1.0;
	}
	else
	{
		hipIdleAmount = weapDef->hipIdleAmount;
		hipIdleSpeed = weapDef->hipIdleSpeed;
	}
	if ( (ps->eFlags & 8) != 0 )
	{
		idleProneFactor = weapDef->idleProneFactor;
	}
	else if ( (ps->eFlags & 4) != 0 )
	{
		idleProneFactor = weapDef->idleCrouchFactor;
	}
	else
	{
		idleProneFactor = 1.0;
	}
	if ( (!weapDef->adsOverlayReticle || ps->fWeaponPosFrac == 0.0) && idleProneFactor != ws->fLastIdleFactor )
	{
		if ( idleProneFactor <= (float)ws->fLastIdleFactor )
		{
			ws->fLastIdleFactor = ws->fLastIdleFactor - ws->frametime * 0.5;
			if ( idleProneFactor > (float)ws->fLastIdleFactor )
				ws->fLastIdleFactor = idleProneFactor;
		}
		else
		{
			ws->fLastIdleFactor = ws->frametime * 0.5 + ws->fLastIdleFactor;
			if ( ws->fLastIdleFactor > (float)idleProneFactor )
				ws->fLastIdleFactor = idleProneFactor;
		}
	}
	fTargScale = hipIdleAmount * ws->fLastIdleFactor;
	if ( weapDef->adsOverlayReticle )
		fTargScale = (1.0 - ps->fWeaponPosFrac) * fTargScale;
	*ws->weapIdleTime += (int)(ws->frametime * 1000.0 * hipIdleSpeed);
	v8 = sin((float)*ws->weapIdleTime * 0.00050000002);
	angles[2] = fTargScale * v8 * 0.0099999998 + angles[2];
	v9 = sin((float)*ws->weapIdleTime * 0.00069999998);
	angles[1] = fTargScale * v9 * 0.0099999998 + angles[1];
	v10 = sin((float)*ws->weapIdleTime * 0.001);
	*angles = fTargScale * v10 * 0.0099999998 + *angles;
}

void BG_CalculateWeaponMovement_BobAngles(weaponState_t *ws, float *angles)
{
	float v2;
	float v3;
	float VerticalBobFactor;
	float HorizontalBobFactor;
	unsigned int weapIndex;
	WeaponDef *weapDef;
	const playerState_s *ps;
	float bobCycle;
	float v10;
	float v11;
	float v12;
	float speed;
	float cycle;
	float fWeaponPosFrac;

	ps = ws->ps;
	weapIndex = BG_GetViewmodelWeaponIndex(ws->ps);
	weapDef = BG_GetWeaponDef(weapIndex);
	bobCycle = (float)LOBYTE(ps->bobCycle) / 255.0 * 3.141592653589793
	           + (float)LOBYTE(ps->bobCycle) / 255.0 * 3.141592653589793
	           + 6.283185307179586;
	cycle = bobCycle + 0.7853981633974483 + 6.283185307179586;
	speed = ws->xyspeed * 0.16;
	VerticalBobFactor = BG_GetVerticalBobFactor(ps, cycle, speed, 10.0);
	v10 = -VerticalBobFactor;
	HorizontalBobFactor = BG_GetHorizontalBobFactor(ps, cycle, speed, 10.0);
	v11 = -HorizontalBobFactor;
	cycle = cycle - 0.4712389167638204;
	speed = speed * 1.5;
	v2 = BG_GetHorizontalBobFactor(ps, cycle, speed, 10.0);
	v12 = I_fmin(v2, 0.0);
	fWeaponPosFrac = ps->fWeaponPosFrac;
	if ( fWeaponPosFrac != 0.0 )
	{
		speed = 1.0 - (1.0 - weapDef->adsBobFactor) * fWeaponPosFrac;
		v10 = v10 * speed;
		v11 = v11 * speed;
		v12 = v12 * speed;
	}
	if ( weapDef->adsOverlayReticle )
	{
		v3 = 1.0 - fWeaponPosFrac;
		VectorScale(&v10, v3, &v10);
	}
	VectorAdd(angles, &v10, angles);
}

void BG_CalculateWeaponMovement_DamageKickAngles(weaponState_t *ws, float *angles)
{
	float fFrac;
	unsigned int weapIndex;
	WeaponDef *weapDef;
	const playerState_s *ps;
	float fReturnTime;
	float fDeflectTime;
	float fFactor;
	float fRatio;
	float fRatioa;
	float fRatiob;
	float fRatioc;
	float fRatiof;
	float fRatiod;
	float fRatioe;

	if ( ws->damageTime )
	{
		ps = ws->ps;
		weapIndex = BG_GetViewmodelWeaponIndex(ws->ps);
		weapDef = BG_GetWeaponDef(weapIndex);
		fFactor = ps->fWeaponPosFrac * 0.5 + 0.5;
		fDeflectTime = fFactor * 100.0;
		fReturnTime = fFactor * 400.0;
		if ( ps->fWeaponPosFrac != 0.0 && weapDef->adsOverlayReticle )
			fFactor = (1.0 - ps->fWeaponPosFrac * 0.75) * fFactor;
		fRatio = (float)(ws->time - ws->damageTime);
		if ( fDeflectTime <= (float)fRatio )
		{
			fRatiof = 1.0 - (fRatio - fDeflectTime) / fReturnTime;
			if ( fRatiof > 0.0 )
			{
				fFrac = 1.0 - fRatiof;
				fRatiod = 1.0 - GetLeanFraction(fFrac);
				fRatioe = fRatiod * fFactor;
				*angles = fRatioe * ws->v_dmg_pitch * 0.5 + *angles;
				angles[1] = angles[1] - fRatioe * ws->v_dmg_roll;
				angles[2] = fRatioe * ws->v_dmg_roll * 0.5 + angles[2];
			}
		}
		else
		{
			fRatioa = fRatio / fDeflectTime;
			fRatiob = GetLeanFraction(fRatioa);
			fRatioc = fRatiob * fFactor;
			*angles = fRatioc * ws->v_dmg_pitch * 0.5 + *angles;
			angles[1] = angles[1] - fRatioc * ws->v_dmg_roll;
			angles[2] = fRatioc * ws->v_dmg_roll * 0.5 + angles[2];
		}
	}
}

int BG_CalculateWeaponMovement_GunRecoil_SingleAngle(float *fOffset, float *speed, float fTimeStep, float fOfsCap, float fGunKickAccel, float fGunKickSpeedMax, float fGunKickSpeedDecay, float fGunKickStaticDecay)
{
	int bCanStop;

	bCanStop = 0;

	if ( fabs(*fOffset) >= 0.25 || fabs(*speed) >= 1.0 )
	{
		*fOffset = *speed * fTimeStep + *fOffset;
		if ( *fOffset <= (float)fOfsCap )
		{
			if ( -fOfsCap > (float)*fOffset )
			{
				*fOffset = -fOfsCap;
				if ( *speed < 0.0 )
					*speed = 0.0;
			}
		}
		else
		{
			*fOffset = fOfsCap;
			if ( *speed > 0.0 )
				*speed = 0.0;
		}
		if ( *fOffset <= 0.0 )
		{
			if ( *fOffset < 0.0 )
				*speed = fGunKickAccel * fTimeStep + *speed;
		}
		else
		{
			*speed = *speed - fGunKickAccel * fTimeStep;
		}
		*speed = *speed - *speed * fGunKickSpeedDecay * fTimeStep;
		if ( *speed <= 0.0 )
		{
			*speed = fGunKickStaticDecay * fTimeStep + *speed;
			if ( *speed > 0.0 )
				*speed = 0.0;
		}
		else
		{
			*speed = *speed - fGunKickStaticDecay * fTimeStep;
			if ( *speed < 0.0 )
				*speed = 0.0;
		}
		if ( *speed <= (float)fGunKickSpeedMax )
		{
			if ( -fGunKickSpeedMax > (float)*speed )
				*speed = -fGunKickSpeedMax;
		}
		else
		{
			*speed = fGunKickSpeedMax;
		}
	}
	else
	{
		*fOffset = 0.0;
		*speed = 0.0;
		return 1;
	}

	return bCanStop;
}

void BG_CalculateWeaponMovement_Recoil(weaponState_t *ws, float *angles)
{
	int stop;
	unsigned int weapIndex;
	WeaponDef *weapDef;
	const playerState_s *ps;
	float fWeaponPosFrac;
	float fGunKickStaticDecay;
	float fGunKickSpeedDecay;
	float fGunKickSpeedMax;
	float fGunKickAccel;
	int bCanStop;
	float fTimeStep;
	float frametime;

	ps = ws->ps;
	weapIndex = BG_GetViewmodelWeaponIndex(ws->ps);
	weapDef = BG_GetWeaponDef(weapIndex);
	if ( BG_IsAimDownSightWeapon(weapIndex) )
	{
		fWeaponPosFrac = ps->fWeaponPosFrac;
		fGunKickAccel = (weapDef->adsGunKickAccel - weapDef->hipGunKickAccel) * fWeaponPosFrac + weapDef->hipGunKickAccel;
		fGunKickSpeedMax = (weapDef->adsGunKickSpeedMax - weapDef->hipGunKickSpeedMax) * fWeaponPosFrac
		                   + weapDef->hipGunKickSpeedMax;
		fGunKickSpeedDecay = (weapDef->adsGunKickSpeedDecay - weapDef->hipGunKickSpeedDecay) * fWeaponPosFrac
		                     + weapDef->hipGunKickSpeedDecay;
		fGunKickStaticDecay = (weapDef->adsGunKickStaticDecay - weapDef->hipGunKickStaticDecay) * fWeaponPosFrac
		                      + weapDef->hipGunKickStaticDecay;
		frametime = ws->frametime;
		while ( frametime > 0.0 )
		{
			if ( frametime <= 0.005 )
			{
				fTimeStep = frametime;
				frametime = 0.0;
				stop = BG_CalculateWeaponMovement_GunRecoil_SingleAngle(
				           ws->recoilAngles,
				           ws->recoilSpeed,
				           fTimeStep,
				           weapDef->gunMaxPitch,
				           fGunKickAccel,
				           fGunKickSpeedMax,
				           fGunKickSpeedDecay,
				           fGunKickStaticDecay);
			}
			else
			{
				fTimeStep = 0.0049999999;
				frametime = frametime - 0.005;
				stop = BG_CalculateWeaponMovement_GunRecoil_SingleAngle(
				           ws->recoilAngles,
				           ws->recoilSpeed,
				           0.0049999999,
				           weapDef->gunMaxPitch,
				           fGunKickAccel,
				           fGunKickSpeedMax,
				           fGunKickSpeedDecay,
				           fGunKickStaticDecay);
			}
			bCanStop = stop;
			if ( BG_CalculateWeaponMovement_GunRecoil_SingleAngle(
			            &ws->recoilAngles[1],
			            &ws->recoilSpeed[1],
			            fTimeStep,
			            weapDef->gunMaxYaw,
			            fGunKickAccel,
			            fGunKickSpeedMax,
			            fGunKickSpeedDecay,
			            fGunKickStaticDecay) )
			{
				if ( bCanStop )
					break;
			}
		}
		VectorAdd(angles, ws->recoilAngles, angles);
	}
}

void BG_CalculateWeaponMovement(weaponState_t *ws, float *angles)
{
	const playerState_s *ps;
	float LeanFraction;

	ps = ws->ps;
	VectorClear(angles);

	if ( ps->leanf != 0.0 )
	{
		LeanFraction = GetLeanFraction(ps->leanf);
		angles[2] = angles[2] - (LeanFraction + LeanFraction);
	}

	BG_CalculateWeaponMovement_Base(ws, angles);
	BG_CalculateWeaponMovement_IdleAngles(ws, angles);
	BG_CalculateWeaponMovement_BobAngles(ws, angles);
	BG_CalculateWeaponMovement_DamageKickAngles(ws, angles);
	BG_CalculateWeaponMovement_Recoil(ws, angles);

	angles[0] = AngleSubtract(angles[0], ws->swayAngles[0]);
	angles[1] = AngleSubtract(angles[1], ws->swayAngles[1]);
}