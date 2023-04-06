#include "../qcommon/qcommon.h"
#include "bg_public.h"

#ifdef TESTING_LIBRARY
#define bg_weaponDefs (((WeaponDef**)( 0x08576160 )))
#define bg_iNumWeapons (*((unsigned int*)( 0x08576140 )))
#else
extern WeaponDef* bg_weaponDefs[];
extern unsigned int bg_iNumWeapons;
#endif

bool BG_ValidateWeaponNumber(int weaponIndex)
{
	if ( weaponIndex >= 0 && weaponIndex <= BG_GetNumWeapons() )
		return true;

	return false;
}

int QDECL BG_IsAimDownSightWeapon(int weapon)
{
	return BG_GetWeaponDef(weapon)->aimDownSight;
}

bool BG_IsWeaponValid(playerState_t *ps, int weaponIndex)
{
	WeaponDef *WeaponDef;
	bool valid;

	valid = BG_ValidateWeaponNumber(weaponIndex);

	if ( !COM_BitTest(ps->weapons, weaponIndex) )
		return false;

	WeaponDef = BG_GetWeaponDef(weaponIndex);

	if (!WeaponDef)
		return false;

	if ( !WeaponDef->offhandClass
	        && ps->weaponslots[1] != weaponIndex
	        && ps->weaponslots[2] != weaponIndex
	        && WeaponDef->altWeaponIndex != weaponIndex )
	{
		return false;
	}

	return valid;
}

bool BG_DoesWeaponNeedSlot(int weapon)
{
	WeaponDef *weaponDef;

	weaponDef = BG_GetWeaponDef(weapon);

	return weaponDef->weaponClass != WEAPCLASS_ITEM && weaponDef->offhandClass == 0;
}

int BG_GetBinocularsIndex()
{
	int i;

	for ( i = 1; i <= bg_iNumWeapons; ++i )
	{
		if ( bg_weaponDefs[i]->weaponType == WEAPTYPE_BINOCULARS )
			return i;
	}

	return 0;
}

unsigned int QDECL BG_GetViewmodelWeaponIndex(const playerState_s *ps)
{
	if ( (ps->pm_flags & 0x20000) != 0 )
		return BG_GetBinocularsIndex();

	if ( (ps->pm_flags & 0x10) != 0 )
		return ps->offHandIndex;

	return ps->weapon;
}

int BG_GetStackableSlot(gclient_s *client, int weapon, int slot)
{
	WeaponDef *weaponDef;

	weaponDef = BG_GetWeaponDef(weapon);

	if ( weaponDef->slotStackable )
	{
		if ( (unsigned int)(weaponDef->weaponSlot - 1) <= 1 )
		{
			if ( slot == 1 || slot == 2 )
			{
				if ( !client->ps.weaponslots[slot] )
					return slot;

				if ( BG_GetWeaponDef(client->ps.weaponslots[slot])->slotStackable )
					return slot;
			}

			if ( !client->ps.weaponslots[1] )
				return 1;

			if ( BG_GetWeaponDef(client->ps.weaponslots[1])->slotStackable )
				return 1;

			if ( !client->ps.weaponslots[2] )
				return 2;

			if ( BG_GetWeaponDef(client->ps.weaponslots[2])->slotStackable )
				return 2;
		}

		return 0;
	}

	return 0;
}

int BG_GetEmptySlotForWeapon(playerState_s *ps, int weapon)
{
	if ( (unsigned int)(BG_GetWeaponDef(weapon)->weaponSlot - 1) > 1 )
		return 0;

	if ( ps->weaponslots[1] )
	{
		if ( !ps->weaponslots[2] )
			return 2;

		return 0;
	}

	return 1;
}

int BG_PlayerHasWeapon(playerState_s *ps, int weaponIndex, int altWeaponIndex)
{
	WeaponDef *weaponDef;
	int weapon;

	if ( !COM_BitTest(ps->weapons, weaponIndex) )
		return 0;

	weapon = weaponIndex;
	do
	{
		weaponDef = BG_GetWeaponDef(weapon);

		if ( (unsigned int)(weaponDef->weaponSlot - 1) > 1 )
			return 0;

		if ( ps->weaponslots[1] == weapon )
			return 1;

		if ( ps->weaponslots[2] == weapon )
			return 2;

		if ( altWeaponIndex && weaponDef->altWeaponIndex )
			weapon = weaponDef->altWeaponIndex;
	}
	while ( weapon != weaponIndex );

	return 0;
}

int BG_GetFirstAvailableOffhand(playerState_s *ps, int offhandSlot)
{
	int weapIndex;
	int slot;

	weapIndex = BG_GetNumWeapons();

	for ( slot = 1; slot <= weapIndex; ++slot )
	{
		if ( BG_GetWeaponDef(slot)->offhandClass == offhandSlot && COM_BitTest(ps->weapons, slot) && BG_WeaponAmmo(ps, slot) )
			return slot;
	}

	return 0;
}

int BG_IsAltSwitch(playerState_s *ps, unsigned int lastWeapon, unsigned int weapon)
{
	if ( !COM_BitTest(ps->weapons, weapon) )
		return 0;

	if ( (unsigned int)(BG_GetWeaponDef(weapon)->weaponSlot - 1) > 1 )
		return 0;

	if ( lastWeapon != 1 && lastWeapon != 2 )
		return 0;

	if ( weapon != ps->weaponslots[lastWeapon] && weapon != ps->weapon )
		ps->weapon = 0;

	ps->weaponslots[lastWeapon] = weapon;
	return 1;
}

void PM_AdjustAimSpreadScale(pmove_t *pm, pml_t *pml)
{
	float temp1;
	float temp2;
	float temp3;
	float velocity;
	playerState_s *ps;
	float hipSpreadDecayRate;
	float sppedSquared;
	float decrease;
	float increase;
	int i;
	int j;
	WeaponDef *weaponDef;

	ps = pm->ps;
	weaponDef = BG_GetWeaponDef(pm->ps->weapon);
	hipSpreadDecayRate = weaponDef->hipSpreadDecayRate;

	if ( hipSpreadDecayRate == 0.0 )
	{
		increase = 0.0;
		decrease = 1.0;
	}
	else
	{
		if ( ps->groundEntityNum != 1023 || ps->pm_type == 1 )
		{
			if ( (ps->eFlags & 8) != 0 )
			{
				hipSpreadDecayRate = hipSpreadDecayRate * weaponDef->hipSpreadProneDecay;
			}
			else if ( (ps->eFlags & 4) != 0 )
			{
				hipSpreadDecayRate = hipSpreadDecayRate * weaponDef->hipSpreadDuckedDecay;
			}
		}
		else
		{
			hipSpreadDecayRate = hipSpreadDecayRate * 0.5;
		}

		decrease = hipSpreadDecayRate * pml->frametime;

		if ( ps->fWeaponPosFrac == 1.0 )
		{
			increase = 0.0;
		}
		else
		{
			sppedSquared = 0.0;

			if ( weaponDef->hipSpreadTurnAdd != 0.0 )
			{
				for ( i = 0; i <= 1; ++i )
				{
					temp3 = (long double)pm->oldcmd.angles[i] * 0.0054931641;
					temp1 = (long double)pm->cmd.angles[i] * 0.0054931641;
					temp2 = AngleSubtract(temp1, temp3);

					sppedSquared = fabs(temp2) * 0.0099999998 * weaponDef->hipSpreadTurnAdd / pml->frametime + sppedSquared;
				}
			}

			if ( weaponDef->hipSpreadMoveAdd != 0.0 && (pm->cmd.forwardmove || pm->cmd.rightmove) )
			{
				velocity = Vec2LengthSq(ps->velocity);

				if ( velocity > Square(bg_aimSpreadMoveSpeedThreshold->current.decimal) )
					sppedSquared = sppedSquared + weaponDef->hipSpreadMoveAdd;
			}

			if ( ps->groundEntityNum == 1023 && ps->pm_type != 1 )
			{
				for ( j = 0; j <= 1; ++j )
					sppedSquared = sppedSquared + 1.28;
			}

			increase = sppedSquared * pml->frametime;
		}
	}

	ps->aimSpreadScale = (increase - decrease) * 255.0 + ps->aimSpreadScale;

	if ( ps->aimSpreadScale >= 0.0 )
	{
		if ( ps->aimSpreadScale > 255.0 )
			ps->aimSpreadScale = 255.0;
	}
	else
	{
		ps->aimSpreadScale = 0.0;
	}
}

void BG_WeaponFireRecoil(playerState_s *ps, float *recoilSpeed, float *kickAVel)
{
	float hipKickYaw;
	float hipKickPitch;
	float adsKickYaw;
	float adsKickPitch;
	float hipViewKickYaw;
	float hipViewKickPitch;
	float adsViewKickYaw;
	float adsViewKickPitch;
	float fReduce;
	unsigned int index;
	float frac;
	float adsViewKickYawMax;
	float adsViewKickYawReduce;
	float adsGunKickYaw;
	float fReducePercent2;
	float adsViewKickPitchMax;
	float adsViewKickPitchReduce;
	float adsGunKickPitch;
	float fReducePercent;
	WeaponDef *WeaponDef;

	index = BG_GetViewmodelWeaponIndex(ps);
	WeaponDef = BG_GetWeaponDef(index);
	frac = ps->fWeaponPosFrac;
	fReduce = 1.0;

	if ( ps->weaponRestrictKickTime > 0 )
	{
		if ( frac == 1.0 )
			fReduce = WeaponDef->adsGunKickReducedKickPercent * 0.0099999998;
		else
			fReduce = WeaponDef->hipGunKickReducedKickPercent * 0.0099999998;
	}

	if ( frac == 1.0 )
	{
		adsViewKickPitch = WeaponDef->adsViewKickPitchMax - WeaponDef->adsViewKickPitchMin;
		adsViewKickPitchMax = G_random() * adsViewKickPitch + WeaponDef->adsViewKickPitchMin;
		adsViewKickYaw = WeaponDef->adsViewKickYawMax - WeaponDef->adsViewKickYawMin;
		adsViewKickYawMax = G_random() * adsViewKickYaw + WeaponDef->adsViewKickYawMin;
	}
	else
	{
		hipViewKickPitch = WeaponDef->hipViewKickPitchMax - WeaponDef->hipViewKickPitchMin;
		adsViewKickPitchMax = G_random() * hipViewKickPitch + WeaponDef->hipViewKickPitchMin;
		hipViewKickYaw = WeaponDef->hipViewKickYawMax - WeaponDef->hipViewKickYawMin;
		adsViewKickYawMax = G_random() * hipViewKickYaw + WeaponDef->hipViewKickYawMin;
	}

	adsViewKickPitchReduce = adsViewKickPitchMax * fReduce;
	adsViewKickYawReduce = adsViewKickYawMax * fReduce;

	kickAVel[0] = -adsViewKickPitchReduce;
	kickAVel[1] = adsViewKickYawReduce;
	kickAVel[2] = kickAVel[1] * -0.5;

	if ( frac <= 0.0 )
	{
		hipKickPitch = WeaponDef->hipGunKickPitchMax - WeaponDef->hipGunKickPitchMin;
		adsGunKickPitch = G_random() * hipKickPitch + WeaponDef->hipGunKickPitchMin;
		hipKickYaw = WeaponDef->hipGunKickYawMax - WeaponDef->hipGunKickYawMin;
		adsGunKickYaw = G_random() * hipKickYaw + WeaponDef->hipGunKickYawMin;
	}
	else
	{
		adsKickPitch = WeaponDef->adsGunKickPitchMax - WeaponDef->adsGunKickPitchMin;
		adsGunKickPitch = G_random() * adsKickPitch + WeaponDef->adsGunKickPitchMin;
		adsKickYaw = WeaponDef->adsGunKickYawMax - WeaponDef->adsGunKickYawMin;
		adsGunKickYaw = G_random() * adsKickYaw + WeaponDef->adsGunKickYawMin;
	}

	fReducePercent = adsGunKickPitch * fReduce;
	fReducePercent2 = adsGunKickYaw * fReduce;

	recoilSpeed[0] = recoilSpeed[0] + fReducePercent;
	recoilSpeed[1] = recoilSpeed[1] + fReducePercent2;
}

void PM_ExitAimDownSight(playerState_s *ps)
{
	PM_AddEvent(ps, EV_RESET_ADS);
	ps->pm_flags &= ~0x40u;
}

void PM_UpdateAimDownSightLerp(pmove_t *pm, pml_t *pml)
{
	long double lerpRate;
	playerState_s *ps;
	bool adsRequested;
	unsigned int weaponIndex;
	WeaponDef *weaponDef;

	ps = pm->ps;
	weaponIndex = BG_GetViewmodelWeaponIndex(pm->ps);
	weaponDef = BG_GetWeaponDef(weaponIndex);

	if ( player_scopeExitOnDamage->current.boolean && ps->damageCount && weaponDef->adsOverlayReticle )
	{
		PM_ExitAimDownSight(ps);
		ps->fWeaponPosFrac = 0.0;
		ps->adsDelayTime = 0;
	}
	else if ( weaponDef->aimDownSight )
	{
		adsRequested = 0;

		if ( !weaponDef->segmentedReload
		        && ps->weaponstate == WEAPON_RELOADING
		        && ps->weaponTime - weaponDef->adsReloadTransTime > 0
		        || weaponDef->segmentedReload
		        && (ps->weaponstate == WEAPON_RELOADING
		            || ps->weaponstate == WEAPON_RELOADING_INTERUPT
		            || ps->weaponstate == WEAPON_RELOAD_START
		            || ps->weaponstate == WEAPON_RELOAD_START_INTERUPT
		            || ps->weaponstate == WEAPON_RELOAD_END && ps->weaponTime - weaponDef->adsReloadTransTime > 0)
		        || !weaponDef->rechamberWhileAds && ps->weaponstate == WEAPON_RECHAMBERING )
		{
			adsRequested = 0;
		}
		else if ( (ps->pm_flags & 0x40) != 0 )
		{
			adsRequested = 1;
		}

		if ( weaponDef->adsFire && ps->weaponDelay && ps->weaponstate == WEAPON_FIRING )
			adsRequested = 1;

		if ( 1.0 != ps->fWeaponPosFrac || adsRequested || player_adsExitDelay->current.integer <= 0 )
		{
			ps->adsDelayTime = 0;
		}
		else
		{
			if ( !ps->adsDelayTime )
				ps->adsDelayTime = pm->cmd.serverTime + player_adsExitDelay->current.integer;

			if ( ps->adsDelayTime <= pm->cmd.serverTime )
				ps->adsDelayTime = 0;
			else
				adsRequested = 1;
		}

		if ( adsRequested && ps->fWeaponPosFrac != 1.0 || !adsRequested && ps->fWeaponPosFrac != 0.0 )
		{
			if ( adsRequested )
				lerpRate = (long double)pml->msec * weaponDef->OOPosAnimLength[0] + ps->fWeaponPosFrac;
			else
				lerpRate = ps->fWeaponPosFrac - (long double)pml->msec * weaponDef->OOPosAnimLength[1];
			ps->fWeaponPosFrac = lerpRate;

			ps->fWeaponPosFrac = I_fclamp(ps->fWeaponPosFrac, 0.0, 1.0);
		}
	}
	else
	{
		ps->fWeaponPosFrac = 0.0;
		ps->adsDelayTime = 0;
	}
}

void PM_StartHoldBreath(playerState_s *ps)
{
	ps->pm_flags |= 0x4000u;
}

void PM_EndHoldBreath(playerState_s *ps)
{
	ps->pm_flags &= ~0x4000u;
}

void PM_UpdateHoldBreath(pmove_t *pm, pml_t *pml)
{
	int timerMsec;
	playerState_s *ps;
	int gasp_time;
	int hold_time;
	float lerp;
	float time;
	float targetScale;
	float target;
	unsigned int index;
	WeaponDef *weaponDef;

	ps = pm->ps;
	index = BG_GetViewmodelWeaponIndex(pm->ps);
	weaponDef = BG_GetWeaponDef(index);
	hold_time = (int)(player_breath_hold_time->current.decimal * 1000.0);
	gasp_time = (int)(player_breath_gasp_time->current.decimal * 1000.0);

	if ( hold_time > 0 )
	{
		if ( ps->fWeaponPosFrac == 1.0
		        && weaponDef->adsOverlayReticle
		        && weaponDef->weaponClass != 9
		        && SLOWORD(pm->cmd.buttons) < 0 )
		{
			if ( !ps->holdBreathTimer )
				PM_StartHoldBreath(ps);
		}
		else
		{
			PM_EndHoldBreath(ps);
		}

		if ( (ps->pm_flags & 0x4000) != 0 )
			timerMsec = ps->holdBreathTimer + pml->msec;
		else
			timerMsec = ps->holdBreathTimer - pml->msec;

		ps->holdBreathTimer = timerMsec;

		if ( ps->holdBreathTimer < 0 )
			ps->holdBreathTimer = 0;

		if ( (ps->pm_flags & 0x4000) != 0 && ps->holdBreathTimer > hold_time )
		{
			ps->holdBreathTimer = hold_time + gasp_time;
			PM_EndHoldBreath(ps);
		}

		if ( (ps->pm_flags & 0x4000) != 0 )
		{
			targetScale = 0.0;
			lerp = player_breath_hold_lerp->current.decimal;
		}
		else
		{
			time = (long double)ps->holdBreathTimer / (long double)(hold_time + gasp_time);
			targetScale = (player_breath_gasp_scale->current.decimal - 1.0) * time + 1.0;
			lerp = player_breath_gasp_lerp->current.decimal;
		}

		target = (targetScale - 1.0) * ps->fWeaponPosFrac + 1.0;
		ps->holdBreathScale = DiffTrack(target, ps->holdBreathScale, lerp, pml->frametime);
	}
	else
	{
		PM_EndHoldBreath(ps);
		ps->holdBreathScale = 1.0;
		ps->holdBreathTimer = 0;
	}
}

bool PM_UpdateGrenadeThrow(playerState_s *ps, pml_t *pml)
{
	int index;
	WeaponDef *weaponDef;

	if ( (ps->pm_flags & 0x10) == 0 )
		return 0;

	index = ps->offHandIndex;
	weaponDef = BG_GetWeaponDef(index);

	if ( weaponDef->weaponType != WEAPTYPE_GRENADE )
		return 0;

	if ( ps->grenadeTimeLeft <= 0 )
		return 0;

	if ( weaponDef->cookOffHold )
		ps->grenadeTimeLeft -= pml->msec;

	if ( ps->grenadeTimeLeft > 0 )
		return 0;

	PM_WeaponUseAmmo(ps, index, 1);
	PM_AddEvent(ps, EV_GRENADE_SUICIDE);

	return 1;
}

void PM_StartWeaponAnim(playerState_s *ps, int anim)
{
	if ( ps->pm_type <= 5 )
		ps->weapAnim = anim | ps->weapAnim & 0x200 ^ 0x200;
}

void PM_ContinueWeaponAnim(playerState_s *ps, int anim)
{
	if ( (ps->weapAnim & 0xFFFFFDFF) != anim )
		PM_StartWeaponAnim(ps, anim);
}

void PM_Weapon_FinishRechamber(playerState_s *ps)
{
	PM_ContinueWeaponAnim(ps, WEAP_IDLE);
	ps->weaponstate = WEAPON_READY;
}

int PM_Weapon_WeaponTimeAdjust(pmove_t *pm, pml_t *pml)
{
	playerState_s *ps;
	WeaponDef *weaponDef;

	ps = pm->ps;
	weaponDef = BG_GetWeaponDef(pm->ps->weapon);

	if ( ps->weaponRestrictKickTime > 0 )
	{
		ps->weaponRestrictKickTime -= pml->msec;
		ps->weaponRestrictKickTime = I_min(ps->weaponRestrictKickTime, 0);
	}

	if ( ps->weaponTime )
	{
		ps->weaponTime -= pml->msec;

		if ( ps->weaponTime <= 0 )
		{
			if ( (ps->weaponstate <= WEAPON_MELEE_FIRE || ps->weaponstate > WEAPON_OFFHAND_END)
			        && weaponDef->semiAuto
			        && (pm->cmd.buttons & 1) != 0
			        && ps->weapon == pm->cmd.weapon
			        && PM_WeaponAmmoAvailable(ps) )
			{
				ps->weaponTime = 1;

				if ( ps->weaponstate == WEAPON_RELOADING
				        || ps->weaponstate == WEAPON_RELOAD_START
				        || ps->weaponstate == WEAPON_RELOAD_END
				        || ps->weaponstate == WEAPON_RELOAD_START_INTERUPT
				        || ps->weaponstate == WEAPON_RELOADING_INTERUPT )
				{
					ps->weaponTime = 0;
				}

				if ( (ps->weaponstate == WEAPON_RELOADING
				        || ps->weaponstate == WEAPON_RELOAD_START
				        || ps->weaponstate == WEAPON_RELOAD_END
				        || ps->weaponstate == WEAPON_RELOAD_START_INTERUPT
				        || ps->weaponstate == WEAPON_RELOADING_INTERUPT)
				        && ps->weaponstate != WEAPON_RELOAD_END
				        && weaponDef->segmentedReload
				        && weaponDef->reloadStartTime )
				{
					ps->weaponTime = 0;
				}
				else if ( ps->weaponstate == WEAPON_RECHAMBERING )
				{
					PM_Weapon_FinishRechamber(ps);
				}
				else if ( ps->weaponstate == WEAPON_FIRING
				          || ps->weaponstate == WEAPON_RECHAMBERING
				          || ps->weaponstate == WEAPON_MELEE_INIT
				          || ps->weaponstate == WEAPON_MELEE_FIRE )
				{
					PM_ContinueWeaponAnim(ps, WEAP_IDLE);
					ps->weaponstate = WEAPON_READY;
				}
			}
			else
			{
				ps->weaponTime = 0;
			}
		}
	}

	if ( !ps->weaponDelay )
		return 0;

	ps->weaponDelay -= pml->msec;

	if ( ps->weaponDelay > 0 )
		return 0;

	ps->weaponDelay = 0;
	return 1;
}

void PM_ExitBinoculars(playerState_s *ps)
{
	ps->pm_flags &= ~0x20000u;
	PM_AddEvent(ps, EV_BINOCULAR_EXIT);
}

void PM_ResetAds(playerState_s *ps)
{
	ps->fWeaponPosFrac = 0.0;
	PM_ExitAimDownSight(ps);
	PM_ExitBinoculars(ps);
}

void PM_Weapon_Idle(playerState_s *ps)
{
	ps->pm_flags &= 0xFFFFF7EF;

	if ( ps->weaponstate > WEAPON_OFFHAND_END && ps->weaponstate <= WEAPON_BINOCULARS_END )
		PM_ResetAds(ps);

	ps->weaponTime = 0;
	ps->weaponDelay = 0;
	ps->weaponstate = WEAPON_READY;
	PM_StartWeaponAnim(ps, WEAP_IDLE);
}

int PM_InteruptWeaponWithProneMove(playerState_s *ps)
{
	if ( ps->weaponstate <  WEAPON_FIRING
	        || ps->weaponstate == WEAPON_RELOADING
	        || ps->weaponstate == WEAPON_RELOAD_START
	        || ps->weaponstate == WEAPON_RELOAD_END
	        || ps->weaponstate == WEAPON_RELOAD_START_INTERUPT
	        || ps->weaponstate == WEAPON_RELOADING_INTERUPT
	        || ps->weaponstate == WEAPON_RECHAMBERING )
	{
		return 1;
	}

	if ( ps->weaponstate == WEAPON_FIRING
	        || ps->weaponstate == WEAPON_MELEE_FIRE
	        || ps->weaponstate > WEAPON_MELEE_FIRE && ps->weaponstate <= WEAPON_OFFHAND_END )
	{
		return 0;
	}

	PM_Weapon_Idle(ps);
	return 1;
}

void PM_ResetWeaponState(playerState_s *ps)
{
	PM_Weapon_Idle(ps);
}

void PM_Weapon_BinocularsInit(playerState_s *ps)
{
	WeaponDef *weaponDef;

	weaponDef = BG_GetWeaponDef(ps->weapon);
	PM_ExitAimDownSight(ps);
	ps->weaponTime = 0;
	ps->weaponDelay = weaponDef->adsTransOutTime + player_adsExitDelay->current.integer;
	ps->weaponstate = WEAPON_BINOCULARS_INIT;
}

void PM_Weapon_BinocularsPrepare(playerState_s *ps)
{
	WeaponDef *weaponDef;

	weaponDef = BG_GetWeaponDef(ps->weapon);
	ps->weaponstate = WEAPON_BINOCULARS_PREPARE;
	ps->weaponTime = weaponDef->quickDropTime;
	ps->weaponDelay = 0;
	PM_StartWeaponAnim(ps, WEAP_EMPTY_DROP);
}

void PM_EnterBinoculars(playerState_s *ps)
{
	ps->pm_flags |= 0x20000u;
	PM_AddEvent(ps, EV_BINOCULAR_ENTER);
}

void PM_Weapon_BinocularsHold(playerState_s *ps)
{
	int weapon;
	WeaponDef *weaponDef;

	weapon = BG_GetBinocularsIndex();
	weaponDef = BG_GetWeaponDef(weapon);
	ps->weaponstate = WEAPON_BINOCULARS_HOLD;
	ps->weaponTime = weaponDef->adsTransInTime;
	ps->weaponDelay = 0;
	PM_EnterBinoculars(ps);
	PM_StartWeaponAnim(ps, WEAP_IDLE);
}

void PM_Weapon_BinocularsDrop(playerState_s *ps)
{
	int weapon;
	WeaponDef *weaponDef;

	weapon = BG_GetBinocularsIndex();
	weaponDef = BG_GetWeaponDef(weapon);
	ps->weaponstate = WEAPON_BINOCULARS_DROP;
	ps->weaponTime = weaponDef->adsTransOutTime + player_adsExitDelay->current.integer;
	ps->weaponDelay = 0;
	PM_AddEvent(ps, EV_BINOCULAR_DROP);
	PM_StartWeaponAnim(ps, WEAP_IDLE);
}

void PM_Weapon_BinocularsEnd(pmove_t *pm)
{
	playerState_s *ps;

	ps = pm->ps;
	ps->weaponstate = WEAPON_BINOCULARS_END;
	PM_ExitBinoculars(ps);

	if ( pm->cmd.weapon )
	{
		ps->weaponTime = BG_GetWeaponDef(ps->weapon)->quickRaiseTime;
		ps->weaponDelay = 0;
		PM_StartWeaponAnim(ps, WEAP_EMPTY_RAISE);
	}
	else
	{
		ps->weaponTime = 0;
		ps->weaponDelay = 1;
	}
}

void PM_Weapon_CheckForBinoculars(pmove_t *pm)
{
	char buttons1;
	char buttons2;
	char buttons3;
	char buttons4;
	char buttons5;
	char buttons6;
	char buttons7;
	char buttons8;
	char buttons9;
	char buttons10;
	playerState_s *ps;

	ps = pm->ps;

	if ( (pm->ps->weaponstate > WEAPON_OFFHAND_END && ps->weaponstate <= WEAPON_BINOCULARS_END
	        || (ps->fWeaponPosFrac <= 0.0 || !BG_GetWeaponDef(ps->weapon)->adsOverlayReticle)
	        && (pm->cmd.buttons & 0x30015) == 0)
	        && ps->weaponstate != WEAPON_RELOADING
	        && ps->weaponstate != WEAPON_RELOAD_START
	        && ps->weaponstate != WEAPON_RELOAD_END
	        && ps->weaponstate != WEAPON_RELOAD_START_INTERUPT
	        && ps->weaponstate != WEAPON_RELOADING_INTERUPT
	        && ps->weaponstate != WEAPON_FIRING
	        && ps->weaponstate != WEAPON_RECHAMBERING
	        && ps->weaponstate != WEAPON_MELEE_INIT
	        && ps->weaponstate != WEAPON_MELEE_FIRE
	        && ps->weaponstate != WEAPON_MELEE_INIT
	        && ps->weaponstate != WEAPON_MELEE_FIRE
	        && ps->weaponstate != WEAPON_RAISING
	        && ps->weaponstate != WEAPON_DROPPING
	        && (ps->weaponstate <= WEAPON_MELEE_FIRE || ps->weaponstate > WEAPON_OFFHAND_END) )
	{
		buttons8 = 0;
		if ( ps->damageCount
		        || pm->oldcmd.weapon != pm->cmd.weapon
		        || pm->oldcmd.offHandIndex != pm->cmd.offHandIndex
		        || (pm->cmd.buttons & 0x30015) != 0 )
		{
			buttons8 = 1;
		}

		if ( ps->weaponstate > WEAPON_OFFHAND_END && ps->weaponstate <= WEAPON_BINOCULARS_END && buttons8 )
		{
			PM_Weapon_Idle(ps);
			if ( (pm->cmd.buttons & 1) != 0 )
				ps->weaponTime = 1;
		}
		else if ( (pm->cmd.buttons & 1) == 0 )
		{
			if ( ps->weaponstate == WEAPON_BINOCULARS_START )
			{
				buttons7 = 0;
				if ( (pm->oldcmd.buttons & 0x28) == 0 && (pm->cmd.buttons & 0x28) != 0 )
					buttons7 = 1;

				buttons6 = 0;
				if ( (pm->oldcmd.buttons & 0x28) != 0 && (pm->cmd.buttons & 0x28) == 0 )
					buttons6 = 1;

				if ( buttons7 )
				{
					PM_AddEvent(ps, EV_BINOCULAR_FIRE);
				}
				else if ( buttons6 )
				{
					PM_AddEvent(ps, EV_BINOCULAR_RELEASE);
				}
			}
			if ( player_toggleBinoculars->current.boolean )
			{
				buttons5 = 0;
				if ( (pm->oldcmd.buttons & 0x4000) == 0 && (pm->cmd.buttons & 0x4000) != 0 )
					buttons5 = 1;

				buttons4 = 0;
				if ( buttons5 && (ps->weaponstate <= WEAPON_OFFHAND_END || ps->weaponstate > WEAPON_BINOCULARS_END) )
					buttons4 = 1;

				buttons10 = buttons4;
				buttons3 = 0;

				if ( buttons5 && ps->weaponstate > WEAPON_BINOCULARS_INIT && ps->weaponstate <= WEAPON_BINOCULARS_START )
					buttons3 = 1;

				buttons9 = buttons3;
			}
			else
			{
				buttons2 = 0;

				if ( (pm->cmd.buttons & 0x4000) != 0
				        && (ps->weaponstate <= WEAPON_OFFHAND_END || ps->weaponstate > WEAPON_BINOCULARS_END) )
				{
					buttons2 = 1;
				}

				buttons10 = buttons2;
				buttons1 = 0;

				if ( (pm->cmd.buttons & 0x4000) == 0
				        && ps->weaponstate > WEAPON_BINOCULARS_INIT
				        && ps->weaponstate <= WEAPON_BINOCULARS_START )
				{
					buttons1 = 1;
				}

				buttons9 = buttons1;
			}
			if ( buttons10 )
			{
				if ( BG_GetBinocularsIndex() )
				{
					if ( pm->cmd.weapon )
					{
						if ( (pm->cmd.buttons & 0x1000) != 0 )
							PM_Weapon_BinocularsInit(ps);
						else
							PM_Weapon_BinocularsPrepare(ps);
					}
					else
					{
						PM_Weapon_BinocularsHold(ps);
					}
				}
			}
			else if ( buttons9 )
			{
				if ( ps->weaponstate == WEAPON_BINOCULARS_PREPARE )
					PM_Weapon_BinocularsEnd(pm);
				else
					PM_Weapon_BinocularsDrop(ps);
			}
		}
	}
}

void PM_SetProneMovementOverride(playerState_s *ps)
{
	if ( (ps->pm_flags & 1) != 0 )
		ps->pm_flags |= 0x800u;
}

void PM_Weapon_OffHandInit(playerState_s *ps)
{
	WeaponDef *weaponDef;

	weaponDef = BG_GetWeaponDef(ps->weapon);
	ps->weaponstate = WEAPON_OFFHAND_INIT;
	ps->weaponTime = weaponDef->quickDropTime;
	ps->weaponDelay = 0;
	ps->pm_flags &= ~0x10u;
	PM_ExitAimDownSight(ps);
	PM_StartWeaponAnim(ps, WEAP_EMPTY_DROP);
}

void PM_Weapon_OffHandPrepare(playerState_s *ps)
{
	WeaponDef *weaponDef;

	weaponDef = BG_GetWeaponDef(ps->offHandIndex);
	ps->weaponstate = WEAPON_OFFHAND_PREPARE;
	ps->weaponTime = weaponDef->holdFireTime;
	ps->weaponDelay = 0;
	ps->pm_flags |= 0x10u;
	BG_AddPredictableEventToPlayerstate(EV_PREP_OFFHAND, ps->offHandIndex, ps);
	PM_StartWeaponAnim(ps, WEAP_HOLD_FIRE);
	PM_SetProneMovementOverride(ps);
}

void PM_SendEmtpyOffhandEvent(playerState_s *ps)
{
	PM_AddEvent(ps, EV_EMPTY_OFFHAND);
}

void PM_Weapon_CheckForOffHand(pmove_t *pm)
{
	WeaponDef *weaponDef;
	int slot;
	int index;
	playerState_s *ps;

	ps = pm->ps;

	if ( (pm->ps->eFlags & 0x300) == 0
	        && (ps->pm_flags & 0x4000000) == 0
	        && (ps->weaponstate <= WEAPON_OFFHAND_END || ps->weaponstate > WEAPON_BINOCULARS_END)
	        && (ps->weaponstate <= WEAPON_MELEE_FIRE || ps->weaponstate >= WEAPON_OFFHAND_END) )
	{
		if ( COM_BitTest(ps->weapons, pm->cmd.offHandIndex) )
			ps->offHandIndex = pm->cmd.offHandIndex;

		if ( (pm->cmd.buttons & 0x10000) != 0 )
		{
			slot = 1;
		}
		else
		{
			if ( (pm->cmd.buttons & 0x20000) == 0 )
				return;
			slot = 2;
		}

		index = BG_GetFirstAvailableOffhand(ps, slot);

		if ( index )
		{
			BG_AddPredictableEventToPlayerstate(EV_SWITCH_OFFHAND, index, ps);
			ps->offHandIndex = index;
			weaponDef = BG_GetWeaponDef(ps->offHandIndex);

			if ( weaponDef->weaponType != WEAPTYPE_GRENADE )
				Com_Error(ERR_DROP, "[%s] Only grenades are currently supported for off hand use\n", weaponDef->szInternalName);

			if ( !weaponDef->offhandClass )
				Com_Error(ERR_DROP, "[%s] No offhand class set\n", weaponDef->szInternalName);

			if ( ps->weapon && ps->weaponstate != WEAPON_OFFHAND_END )
				PM_Weapon_OffHandInit(ps);
			else
				PM_Weapon_OffHandPrepare(ps);
		}
		else
		{
			PM_SendEmtpyOffhandEvent(ps);
		}
	}
}

void PM_BeginWeaponChange(playerState_s *pm, unsigned int newweapon)
{
	int dropTime;
	int altWeapon;
	int lastWeapon;
	unsigned int weaponIndex;
	WeaponDef *weaponDef;
	int onlyClip;

	if ( (!newweapon || COM_BitTest(pm->weapons, newweapon)) && pm->weaponstate != WEAPON_DROPPING )
	{
		pm->weaponDelay = 0;
		weaponIndex = pm->weapon;

		if ( weaponIndex && COM_BitTest(pm->weapons, weaponIndex) && pm->grenadeTimeLeft <= 0 )
		{
			weaponDef = BG_GetWeaponDef(weaponIndex);
			altWeapon = 0;

			if ( newweapon && newweapon == weaponDef->altWeaponIndex )
				altWeapon = 1;

			onlyClip = 1;

			if ( BG_WeaponIsClipOnly(weaponIndex) && !pm->ammoclip[BG_ClipForWeapon(weaponIndex)] )
				onlyClip = 0;

			pm->grenadeTimeLeft = 0;

			if ( altWeapon )
			{
				PM_AddEvent(pm, EV_WEAPON_ALT);
				PM_StartWeaponAnim(pm, WEAP_ALTSWITCH);
			}
			else if ( onlyClip )
			{
				PM_AddEvent(pm, EV_PUTAWAY_WEAPON);
				PM_StartWeaponAnim(pm, WEAP_DROP);
			}

			if ( !altWeapon && (pm->pm_flags & 4) == 0 )
				BG_AnimScriptEvent(pm, ANIM_ET_DROPWEAPON, 0, 0);

			pm->weaponstate = WEAPON_DROPPING;
			PM_SetProneMovementOverride(pm);

			if ( altWeapon )
				dropTime = weaponDef->altDropTime;
			else
				dropTime = weaponDef->dropTime;

			pm->weaponTime = dropTime;

			if ( altWeapon )
			{
				lastWeapon = BG_PlayerHasWeapon(pm, weaponIndex, 1);

				if ( lastWeapon )
					BG_IsAltSwitch(pm, lastWeapon, newweapon);
			}
		}
		else
		{
			pm->weaponTime = 0;
			pm->weaponstate = WEAPON_DROPPING;
			pm->grenadeTimeLeft = 0;
			PM_SetProneMovementOverride(pm);
		}
	}
}

void PM_Weapon_CheckForChangeWeapon(pmove_t *pm)
{
	playerState_s *ps;

	ps = pm->ps;

	if ( (pm->ps->weaponstate <= WEAPON_OFFHAND_END || ps->weaponstate > WEAPON_BINOCULARS_END)
	        && (ps->weaponstate <= WEAPON_MELEE_FIRE || ps->weaponstate > WEAPON_OFFHAND_END)
	        && (!ps->weaponTime
	            || ps->weaponstate == WEAPON_RELOADING
	            || ps->weaponstate == WEAPON_RELOAD_START
	            || ps->weaponstate == WEAPON_RELOAD_END
	            || ps->weaponstate == WEAPON_RELOAD_START_INTERUPT
	            || ps->weaponstate == WEAPON_RELOADING_INTERUPT
	            || ps->weaponstate == WEAPON_RECHAMBERING
	            || ps->weaponstate != WEAPON_FIRING
	            && ps->weaponstate != WEAPON_RECHAMBERING
	            && ps->weaponstate != WEAPON_MELEE_INIT
	            && ps->weaponstate != WEAPON_MELEE_FIRE
	            && !ps->weaponDelay) )
	{
		if ( Mantle_IsWeaponInactive(ps) )
		{
			if ( !ps->weapon )
				return;
changeWeapon:
			PM_BeginWeaponChange(ps, 0);
			return;
		}

		if ( (ps->pm_flags & 0x20) != 0 )
		{
			if ( !ps->weapon )
				return;
			goto changeWeapon;
		}

		if ( (ps->pm_flags & 0x4000000) != 0 )
		{
			if ( !ps->weapon )
				return;
			goto changeWeapon;
		}

		if ( ps->weapon != pm->cmd.weapon
		        && (SLOWORD(ps->pm_flags) >= 0 || !ps->weapon)
		        && (!pm->cmd.weapon || BG_IsWeaponValid(ps, pm->cmd.weapon)) )
		{
			PM_BeginWeaponChange(ps, pm->cmd.weapon);
			return;
		}

		if ( ps->weapon && !COM_BitTest(ps->weapons, ps->weapon) )
			goto changeWeapon;
	}
}

void PM_SetWeaponReloadAddAmmoDelay(playerState_s *ps)
{
	int iReloadAddTime;
	WeaponDef *weaponDef;

	weaponDef = BG_GetWeaponDef(ps->weapon);

	if ( ps->weaponstate == WEAPON_RELOAD_START || ps->weaponstate == WEAPON_RELOAD_START_INTERUPT )
	{
		if ( weaponDef->reloadStartAddTime )
		{
			if ( weaponDef->reloadStartAddTime >= weaponDef->reloadStartTime )
				iReloadAddTime = weaponDef->reloadStartTime;
			else
				iReloadAddTime = weaponDef->reloadStartAddTime;
		}
		else
		{
			iReloadAddTime = 0;
		}
	}
	else
	{
		if ( ps->ammoclip[BG_ClipForWeapon(ps->weapon)] || weaponDef->weaponType )
			iReloadAddTime = weaponDef->reloadTime;
		else
			iReloadAddTime = weaponDef->reloadEmptyTime;

		if ( weaponDef->reloadAddTime && weaponDef->reloadAddTime < iReloadAddTime )
			iReloadAddTime = weaponDef->reloadAddTime;
	}

	if ( weaponDef->boltAction && COM_BitTest(ps->weaponrechamber, ps->weapon) )
	{
		if ( !iReloadAddTime )
			iReloadAddTime = ps->weaponTime;

		if ( weaponDef->rechamberBoltTime < iReloadAddTime )
			iReloadAddTime = weaponDef->rechamberBoltTime;

		if ( !iReloadAddTime )
			iReloadAddTime = 1;

		ps->weaponDelay = iReloadAddTime;
	}
	else if ( iReloadAddTime )
	{
		ps->weaponDelay = iReloadAddTime;
	}
}

void PM_SetReloadingState(playerState_s *ps)
{
	WeaponDef *weaponDef;

	weaponDef = BG_GetWeaponDef(ps->weapon);

	if ( ps->ammoclip[BG_ClipForWeapon(ps->weapon)] || weaponDef->weaponType )
	{
		PM_StartWeaponAnim(ps, WEAP_RELOAD);
		ps->weaponTime = weaponDef->reloadTime;
		PM_AddEvent(ps, EV_RELOAD);
	}
	else
	{
		PM_StartWeaponAnim(ps, WEAP_RELOAD_EMPTY);
		ps->weaponTime = weaponDef->reloadEmptyTime;
		PM_AddEvent(ps, EV_RELOAD_FROM_EMPTY);
	}

	if ( ps->weaponstate == WEAPON_RELOAD_START_INTERUPT )
		ps->weaponstate = WEAPON_RELOADING_INTERUPT;
	else
		ps->weaponstate = WEAPON_RELOADING;

	PM_SetWeaponReloadAddAmmoDelay(ps);
}

void PM_BeginWeaponReload(playerState_s *ps)
{
	WeaponDef *weaponDef;

	weaponDef = BG_GetWeaponDef(ps->weapon);

	if ( (ps->weaponstate == WEAPON_READY || ps->weaponstate == WEAPON_FIRING || ps->weaponstate == WEAPON_RECHAMBERING)
	        && ps->weapon
	        && (signed int)ps->weapon <= BG_GetNumWeapons() )
	{
		if ( !BG_WeaponIsClipOnly(ps->weapon) )
			BG_AnimScriptEvent(ps, ANIM_ET_RELOAD, 0, 1);

		PM_AddEvent(ps, EV_RESET_ADS);

		if ( weaponDef->segmentedReload && weaponDef->reloadStartTime )
		{
			PM_StartWeaponAnim(ps, WEAP_RELOAD_START);
			ps->weaponTime = weaponDef->reloadStartTime;
			ps->weaponstate = WEAPON_RELOAD_START;
			PM_AddEvent(ps, EV_RELOAD_START);
			PM_SetWeaponReloadAddAmmoDelay(ps);
		}
		else
		{
			PM_SetReloadingState(ps);
		}
	}
}

int PM_Weapon_AllowReload(const playerState_s *ps)
{
	int weapon;
	WeaponDef *weaponDef;

	weaponDef = BG_GetWeaponDef(ps->weapon);
	weapon = BG_ClipForWeapon(ps->weapon);

	if ( !ps->ammo[BG_AmmoForWeapon(ps->weapon)] || ps->ammoclip[weapon] >= BG_GetClipSize(weapon) )
		return 0;

	if ( !weaponDef->noPartialReload )
		return 1;

	if ( weaponDef->reloadAmmoAdd && weaponDef->reloadAmmoAdd < BG_GetClipSize(weapon) )
		return BG_GetClipSize(weapon) - ps->ammoclip[weapon] >= weaponDef->reloadAmmoAdd;

	return !ps->ammoclip[weapon];
}

void PM_Weapon_CheckForReload(pmove_t *pm)
{
	playerState_s *ps;
	int ammoIndex;
	int clipIndex;
	int allowReload;
	int reloadButtonPressed;
	WeaponDef *weaponDef;

	allowReload = 0;
	ps = pm->ps;
	weaponDef = BG_GetWeaponDef(pm->ps->weapon);

	if ( (ps->weaponstate <= WEAPON_OFFHAND_END || ps->weaponstate > WEAPON_BINOCULARS_END)
	        && (ps->weaponstate <= WEAPON_MELEE_FIRE || ps->weaponstate > WEAPON_OFFHAND_END) )
	{
		reloadButtonPressed = (pm->cmd.buttons & 0x10) != 0;

		if ( (ps->pm_flags & 8) != 0 )
		{
			ps->pm_flags &= ~8u;
			reloadButtonPressed = 1;
		}

		if ( weaponDef->segmentedReload
		        && (ps->weaponstate == WEAPON_RELOAD_START || ps->weaponstate == WEAPON_RELOADING)
		        && (pm->cmd.buttons & 1) != 0
		        && ((LOBYTE(pm->oldcmd.buttons) ^ 1) & 1) != 0 )
		{
			if ( ps->weaponstate == WEAPON_RELOAD_START )
			{
				ps->weaponstate = WEAPON_RELOAD_START_INTERUPT;
			}
			else if ( ps->weaponstate == WEAPON_RELOADING )
			{
				ps->weaponstate = WEAPON_RELOADING_INTERUPT;
			}
		}

		switch ( ps->weaponstate )
		{
		case WEAPON_RAISING:
		case WEAPON_DROPPING:
		case WEAPON_MELEE_INIT:
		case WEAPON_MELEE_FIRE:
			return;
		case WEAPON_RELOADING:
		case WEAPON_RELOADING_INTERUPT:
		case WEAPON_RELOAD_START:
		case WEAPON_RELOAD_START_INTERUPT:
		case WEAPON_RELOAD_END:
			if ( pm->proneChange )
			{
				if ( !BG_WeaponIsClipOnly(ps->weapon) )
					BG_AnimScriptEvent(ps, ANIM_ET_RELOAD, 0, 1);
			}
			break;
		default:
			clipIndex = BG_ClipForWeapon(ps->weapon);
			ammoIndex = BG_AmmoForWeapon(ps->weapon);
			if ( reloadButtonPressed && PM_Weapon_AllowReload(ps) )
				allowReload = 1;
			if ( !ps->ammoclip[clipIndex]
			        && ps->ammo[ammoIndex]
			        && ps->weaponstate != WEAPON_FIRING
			        && (((LOBYTE(ps->pm_flags) ^ 1) & 1) != 0 || !pm->cmd.forwardmove && !pm->cmd.rightmove) )
			{
				allowReload = 1;
			}
			if ( allowReload )
				PM_BeginWeaponReload(ps);
			break;
		}
	}
}

void PM_Weapon_MeleeFire(playerState_s *ps)
{
	WeaponDef *weaponDef;

	weaponDef = BG_GetWeaponDef(ps->weapon);

	if ( ps->weaponTime < weaponDef->meleeTime - weaponDef->meleeDelay )
		ps->weaponTime = weaponDef->meleeTime - weaponDef->meleeDelay;

	PM_AddEvent(ps, EV_FIRE_MELEE);
	ps->weaponstate = WEAPON_MELEE_FIRE;
	PM_SetProneMovementOverride(ps);
}

void PM_Weapon_CheckForMelee(pmove_t *pm, int delayedAction)
{
	int weaponstate;
	playerState_s *ps;
	WeaponDef *weaponDef;

	ps = pm->ps;
	weaponDef = BG_GetWeaponDef(pm->ps->weapon);

	if ( (ps->weaponstate <= WEAPON_OFFHAND_END || ps->weaponstate > WEAPON_BINOCULARS_END)
	        && (ps->weaponstate <= WEAPON_MELEE_FIRE || ps->weaponstate > WEAPON_OFFHAND_END)
	        && weaponDef->meleeDamage
	        && !delayedAction
	        && (!ps->weaponDelay
	            || ps->weaponstate == WEAPON_RELOADING
	            || ps->weaponstate == WEAPON_RELOAD_START
	            || ps->weaponstate == WEAPON_RELOAD_END
	            || ps->weaponstate == WEAPON_RELOAD_START_INTERUPT
	            || ps->weaponstate == WEAPON_RELOADING_INTERUPT) )
	{
		if ( (pm->cmd.buttons & 4) != 0 )
		{
			if ( (ps->pm_flags & 0x2000) == 0 )
			{
				ps->pm_flags |= 0x2000u;

				if ( ps->fWeaponPosFrac <= 0.0 || !weaponDef->adsOverlayReticle )
				{
					weaponstate = ps->weaponstate;

					if ( weaponstate < WEAPON_RAISING || weaponstate > WEAPON_DROPPING && (unsigned int)(weaponstate - 10) > 1 )
					{
						BG_AnimScriptEvent(ps, ANIM_ET_MELEEATTACK, 0, 1);
						PM_StartWeaponAnim(ps, WEAP_MELEE_ATTACK);
						PM_AddEvent(ps, EV_MELEE_SWIPE);

						if ( weaponDef->meleeDelay )
						{
							ps->weaponTime = weaponDef->meleeTime;
							ps->weaponDelay = weaponDef->meleeDelay;
							ps->weaponstate = WEAPON_MELEE_INIT;
							PM_SetProneMovementOverride(ps);
						}
						else
						{
							PM_Weapon_MeleeFire(ps);
						}
					}
				}
			}
		}
		else
		{
			ps->pm_flags &= ~0x2000u;
		}
	}
}

int PM_Weapon_CheckForRechamber(playerState_s *ps, int delayedAction)
{
	WeaponDef *weaponDef;

	weaponDef = BG_GetWeaponDef(ps->weapon);

	if ( ps->weaponstate > WEAPON_OFFHAND_END && ps->weaponstate <= WEAPON_BINOCULARS_END )
		return 0;

	if ( ps->weaponstate <= WEAPON_MELEE_FIRE || ps->weaponstate > WEAPON_OFFHAND_END )
	{
		if ( weaponDef->boltAction && COM_BitTest(ps->weaponrechamber, ps->weapon) )
		{
			if ( ps->weaponstate == WEAPON_RECHAMBERING )
			{
				if ( delayedAction )
				{
					COM_BitClear(ps->weaponrechamber, ps->weapon);
					PM_AddEvent(ps, EV_EJECT_BRASS);

					if ( ps->weaponTime )
						return 1;
				}
			}

			if ( !ps->weaponTime
			        || ps->weaponstate != WEAPON_FIRING
			        && ps->weaponstate != WEAPON_RECHAMBERING
			        && ps->weaponstate != WEAPON_MELEE_INIT
			        && ps->weaponstate != WEAPON_MELEE_FIRE
			        && !ps->weaponDelay )
			{
				if ( ps->weaponstate == WEAPON_RECHAMBERING )
				{
					PM_Weapon_FinishRechamber(ps);
				}
				else if ( ps->weaponstate == WEAPON_READY )
				{
					if ( ps->fWeaponPosFrac <= 0.75 )
						PM_StartWeaponAnim(ps, WEAP_RECHAMBER);
					else
						PM_StartWeaponAnim(ps, WEAP_ADS_RECHAMBER);

					ps->weaponstate = WEAPON_RECHAMBERING;
					ps->weaponTime = weaponDef->rechamberTime;

					if ( weaponDef->rechamberBoltTime && weaponDef->rechamberBoltTime < weaponDef->rechamberTime )
						ps->weaponDelay = weaponDef->rechamberBoltTime;
					else
						ps->weaponDelay = 1;

					PM_AddEvent(ps, EV_RECHAMBER_WEAPON);
				}
			}
		}

		return 0;
	}

	return 0;
}

void PM_Weapon_FinishWeaponRaise(playerState_s *ps)
{
	ps->weaponstate = WEAPON_READY;
	PM_StartWeaponAnim(ps, WEAP_IDLE);
}

void PM_Weapon_FinishWeaponChange(pmove_t *pm)
{
	bool exist;
	int altWeapon;
	WeaponDef *weaponDef;
	playerState_s *ps;
	int weapon;
	int weaponIndex;

	ps = pm->ps;
	BG_GetWeaponDef(pm->ps->weapon);

	if ( Mantle_IsWeaponInactive(ps)
	        || (ps->pm_flags & 0x20) != 0
	        || !COM_BitTest(ps->weapons, pm->cmd.weapon)
	        || (ps->pm_flags & 0x4000000) != 0
	        || !COM_BitTest(ps->weapons, pm->cmd.weapon) )
	{
		weapon = 0;
		exist = COM_BitTest(ps->weapons, 0);
	}
	else
	{
		weapon = pm->cmd.weapon;

		if ( weapon > BG_GetNumWeapons() )
			weapon = 0;

		exist = COM_BitTest(ps->weapons, weapon);
	}

	if ( !exist )
		weapon = 0;

	weaponIndex = ps->weapon;
	ps->weapon = (unsigned char)weapon;
	BG_GetWeaponDef(ps->weapon);

	if ( weaponIndex == weapon )
	{
		ps->weaponstate = WEAPON_READY;
		PM_StartWeaponAnim(ps, WEAP_IDLE);
	}
	else if ( weaponIndex )
	{
		ps->weaponstate = WEAPON_RAISING;
		PM_SetProneMovementOverride(ps);
		altWeapon = 0;

		if ( weapon && weapon == BG_GetWeaponDef(weaponIndex)->altWeaponIndex )
			altWeapon = 1;

		if ( altWeapon )
		{
			ps->weaponTime = BG_GetWeaponDef(weapon)->altRaiseTime;
		}
		else
		{
			PM_AddEvent(ps, EV_RAISE_WEAPON);
			ps->weaponTime = BG_GetWeaponDef(weapon)->raiseTime;
		}

		weaponDef = BG_GetWeaponDef(weapon);
		BG_UpdateConditionValue(ps->clientNum, ANIM_COND_PLAYERANIMTYPE, weaponDef->playerAnimType, 1);
		BG_UpdateConditionValue(ps->clientNum, ANIM_COND_WEAPONCLASS, weaponDef->weaponClass, 1);

		if ( weapon && weapon == BG_GetWeaponDef(weaponIndex)->altWeaponIndex )

		{
			if ( ps->aimSpreadScale < 128.0 )
				ps->aimSpreadScale = 128.0;

			PM_StartWeaponAnim(ps, (weapAnimNumber_t)16);
		}
		else
		{
			BG_AnimScriptEvent(ps, ANIM_ET_RAISEWEAPON, 0, 0);
			ps->aimSpreadScale = 255.0;
			PM_StartWeaponAnim(ps, WEAP_RAISE);
		}
	}
	else
	{
		ps->weaponstate = WEAPON_RAISING;
		ps->weaponTime = BG_GetWeaponDef(weapon)->raiseTime;
		ps->aimSpreadScale = 255.0;
		PM_StartWeaponAnim(ps, WEAP_RAISE);
		PM_SetProneMovementOverride(ps);
	}
}

void PM_ReloadClip(playerState_s *ps)
{
	int clipSize;
	int ammoInClip;
	int ammo;
	int weaponIndex;
	int ammoForWeapon;
	WeaponDef *weaponDef;

	weaponDef = BG_GetWeaponDef(ps->weapon);

	if ( ps->weaponstate != WEAPON_RELOAD_START && ps->weaponstate != WEAPON_RELOAD_START_INTERUPT
	        || weaponDef->reloadStartAdd )
	{
		ammoForWeapon = BG_AmmoForWeapon(ps->weapon);
		weaponIndex = BG_ClipForWeapon(ps->weapon);
		ammo = ps->ammo[ammoForWeapon];
		ammoInClip = ps->ammoclip[weaponIndex];
		clipSize = BG_GetClipSize(weaponIndex) - ammoInClip;

		if ( clipSize > ammo )
			clipSize = ammo;

		if ( ps->weaponstate == WEAPON_RELOAD_START || ps->weaponstate == WEAPON_RELOAD_START_INTERUPT )
		{
			if ( weaponDef->reloadStartAdd < BG_GetClipSize(weaponIndex) && clipSize > weaponDef->reloadStartAdd )
				clipSize = weaponDef->reloadStartAdd;
		}
		else if ( weaponDef->reloadAmmoAdd
		          && weaponDef->reloadAmmoAdd < BG_GetClipSize(weaponIndex)
		          && clipSize > weaponDef->reloadAmmoAdd )
		{
			clipSize = weaponDef->reloadAmmoAdd;
		}

		if ( clipSize )
		{
			ps->ammo[ammoForWeapon] -= clipSize;
			ps->ammoclip[weaponIndex] += clipSize;
		}
	}
}

void PM_Weapon_ReloadDelayedAction(playerState_s *ps)
{
	int rechamberTime;
	int reloadAddTime;
	int delay;
	WeaponDef *weaponDef;

	weaponDef = BG_GetWeaponDef(ps->weapon);

	if ( !weaponDef->boltAction || !COM_BitTest(ps->weaponrechamber, ps->weapon) )
	{
reload_clip:
		PM_ReloadClip(ps);
		return;
	}

	COM_BitClear(ps->weaponrechamber, ps->weapon);
	PM_AddEvent(ps, EV_EJECT_BRASS);

	if ( ps->weaponstate != WEAPON_RELOAD_START && ps->weaponstate != WEAPON_RELOAD_START_INTERUPT
	        || weaponDef->reloadStartAddTime )
	{
		if ( ps->weaponTime )
		{
			if ( ps->weaponstate == WEAPON_RELOAD_START || ps->weaponstate == WEAPON_RELOAD_START_INTERUPT )
			{
				reloadAddTime = weaponDef->reloadStartAddTime >= weaponDef->reloadStartTime
				                ? weaponDef->reloadStartTime
				                : weaponDef->reloadStartAddTime;
			}
			else
			{
				reloadAddTime = ps->ammoclip[BG_ClipForWeapon(ps->weapon)] || weaponDef->weaponType
				                ? weaponDef->reloadTime
				                : weaponDef->reloadEmptyTime;

				if ( weaponDef->reloadAddTime && weaponDef->reloadAddTime < reloadAddTime )
					reloadAddTime = weaponDef->reloadAddTime;
			}

			rechamberTime = weaponDef->rechamberBoltTime >= reloadAddTime ? 1 : weaponDef->rechamberBoltTime;
			delay = reloadAddTime - rechamberTime;

			if ( delay > 0 )
			{
				ps->weaponDelay = delay;
				return;
			}
		}

		goto reload_clip;
	}
}

void PM_Weapon_FinishReload(playerState_s *ps, int delayedAction)
{
	WeaponDef *weaponDef;

	weaponDef = BG_GetWeaponDef(ps->weapon);

	if ( !delayedAction || (PM_Weapon_ReloadDelayedAction(ps), !ps->weaponTime) )
	{
		if ( !ps->weaponTime )
		{
			COM_BitClear(ps->weaponrechamber, ps->weapon);

			if ( !weaponDef->segmentedReload )
				goto ready;

			if ( ps->weaponstate != WEAPON_RELOADING_INTERUPT && PM_Weapon_AllowReload(ps) )
			{
				PM_SetReloadingState(ps);
				return;
			}

			if ( weaponDef->reloadEndTime )
			{
				ps->weaponstate = WEAPON_RELOAD_END;
				PM_StartWeaponAnim(ps, WEAP_RELOAD_END);
				ps->weaponTime = weaponDef->reloadEndTime;
				PM_AddEvent(ps, EV_RELOAD_END);
			}
			else
			{
ready:
				ps->weaponstate = WEAPON_READY;
				PM_StartWeaponAnim(ps, WEAP_IDLE);
			}
		}
	}
}

void PM_Weapon_FinishReloadStart(playerState_s *ps, int delayedAction)
{
	WeaponDef *weaponDef;

	weaponDef = BG_GetWeaponDef(ps->weapon);

	if ( delayedAction )
		PM_Weapon_ReloadDelayedAction(ps);

	if ( !ps->weaponTime )
	{
		if ( ps->weaponstate == WEAPON_RELOAD_START_INTERUPT && ps->ammoclip[BG_ClipForWeapon(ps->weapon)]
		        || !PM_Weapon_AllowReload(ps) )
		{
			COM_BitClear(ps->weaponrechamber, ps->weapon);

			if ( weaponDef->reloadEndTime )
			{
				ps->weaponstate = WEAPON_RELOAD_END;
				PM_StartWeaponAnim(ps, WEAP_RELOAD_END);
				ps->weaponTime = weaponDef->reloadEndTime;
				PM_AddEvent(ps, EV_RELOAD_END);
			}
			else
			{
				ps->weaponstate = WEAPON_READY;
				PM_StartWeaponAnim(ps, WEAP_IDLE);
			}
		}
		else
		{
			PM_SetReloadingState(ps);
		}
	}
}

void PM_Weapon_FinishReloadEnd(playerState_s *ps)
{
	ps->weaponstate = WEAPON_READY;
	PM_StartWeaponAnim(ps, WEAP_IDLE);
}

void PM_Weapon_MeleeEnd(playerState_s *ps)
{
	PM_ContinueWeaponAnim(ps, WEAP_IDLE);
	ps->weaponstate = WEAPON_READY;
}

void PM_Weapon_OffHandHold(playerState_s *ps)
{
	WeaponDef *weaponDef;

	weaponDef = BG_GetWeaponDef(ps->offHandIndex);
	ps->weaponstate = WEAPON_OFFHAND_HOLD;
	ps->weaponTime = 0;
	ps->weaponDelay = 0;
	ps->pm_flags |= 0x10u;
	ps->grenadeTimeLeft = weaponDef->fuseTime;
}

bool BG_IsUsingBinoculars(playerState_s *ps)
{
	if ( ps->weaponstate == WEAPON_BINOCULARS_HOLD || ps->weaponstate == WEAPON_BINOCULARS_START )
		return 1;

	return 0;
}

bool BG_IsUsingBinoculars2(int state)
{
	if ( state == WEAPON_BINOCULARS_HOLD || state == WEAPON_BINOCULARS_START )
		return 1;

	return 0;
}

bool PM_IsAdsAllowed(playerState_s *ps, pml_t *pml)
{
	unsigned int weapon;

	if ( ps->pm_type == 1 )
	{
		if ( pml->almostGroundPlane )
			return 0;
	}
	else if ( ps->pm_type >= 1 && ps->pm_type <= 7 )
	{
		return 0;
	}

	weapon = BG_GetViewmodelWeaponIndex(ps);

	if ( !BG_GetWeaponDef(weapon)->aimDownSight )
		return 0;

	if ( ps->weaponstate > WEAPON_MELEE_FIRE && ps->weaponstate <= WEAPON_OFFHAND_END )
		return 0;

	if ( ps->weaponstate == WEAPON_MELEE_INIT || ps->weaponstate == WEAPON_MELEE_FIRE )
		return 0;

	if ( ps->weaponstate == WEAPON_RAISING || ps->weaponstate == WEAPON_DROPPING )
		return 0;

	return ps->weaponstate <= WEAPON_OFFHAND_END
	       || ps->weaponstate > WEAPON_BINOCULARS_END
	       || BG_IsUsingBinoculars2(ps->weaponstate);
}

void PM_UpdateAimDownSightFlag(pmove_t *pm, pml_t *pml)
{
	bool adsRequested;
	bool IsAdsAllowed;
	playerState_s *ps;

	ps = pm->ps;
	ps->pm_flags &= ~0x40u;
	IsAdsAllowed = PM_IsAdsAllowed(ps, pml);
	adsRequested = 0;

	if ( (pm->cmd.buttons & 0x1000) != 0 || BG_IsUsingBinoculars(ps) )
		adsRequested = 1;

	if ( adsRequested && IsAdsAllowed )
	{
		if ( (ps->pm_flags & 1) != 0 )
		{
			if ( (pm->oldcmd.buttons & 0x1000) == 0 || !pm->cmd.forwardmove && !pm->cmd.rightmove )
			{
				ps->pm_flags |= 0x40u;
				ps->pm_flags |= 0x800u;
			}
		}
		else
		{
			ps->pm_flags |= 0x40u;
		}
	}

	if ( (ps->pm_flags & 0x40) != 0 )
		BG_UpdateConditionValue(ps->clientNum, ANIM_COND_WEAPON_POSITION, 1, 1);
	else
		BG_UpdateConditionValue(ps->clientNum, ANIM_COND_WEAPON_POSITION, 0, 1);
}

void PM_Weapon_OffHand(pmove_t *pm)
{
	playerState_s *ps;
	WeaponDef *weaponDef;

	ps = pm->ps;

	if ( (pm->oldcmd.buttons & 0x30000) != 0 && (pm->cmd.buttons & 0x30000) != 0 )
	{
		ps->weaponDelay = 1;
	}
	else
	{
		weaponDef = BG_GetWeaponDef(ps->offHandIndex);
		ps->weaponstate = WEAPON_OFFHAND;
		ps->weaponTime = weaponDef->fireTime;
		ps->weaponDelay = 0;
		ps->pm_flags |= 0x10u;
		BG_AddPredictableEventToPlayerstate(EV_USE_OFFHAND, ps->offHandIndex, ps);
		PM_StartWeaponAnim(ps, WEAP_ATTACK);
		PM_WeaponUseAmmo(ps, ps->offHandIndex, 1);
		BG_AnimScriptEvent(ps, ANIM_ET_FIREWEAPON, 0, 1);

		if ( !BG_WeaponAmmo(ps, ps->offHandIndex) )
			PM_AddEvent(ps, EV_EMPTY_OFFHAND);
	}
}

void PM_Weapon_OffHandEnd(playerState_s *ps)
{
	if ( ps->weapon )
	{
		ps->weaponTime = BG_GetWeaponDef(ps->weapon)->quickRaiseTime;
		ps->weaponDelay = 0;
		PM_StartWeaponAnim(ps, WEAP_EMPTY_RAISE);
	}
	else
	{
		ps->weaponTime = 0;
		ps->weaponDelay = 1;
	}

	ps->weaponstate = WEAPON_OFFHAND_END;
	ps->pm_flags &= 0xFFFFF7EF;
}

void PM_Weapon_Binoculars(playerState_s *ps)
{
	ps->weaponstate = WEAPON_BINOCULARS_START;
	ps->weaponTime = 0;
	ps->weaponDelay = 0;
}

int PM_Weapon_ShouldBeFiring(pmove_t *pm, int delayedAction)
{
	playerState_s *ps;

	if ( ((LOBYTE(pm->cmd.buttons) ^ 1) & 1) == 0 || delayedAction )
		return 0;

	ps = pm->ps;

	if ( pm->ps->weaponstate == WEAPON_FIRING )
		PM_ContinueWeaponAnim(ps, WEAP_IDLE);

	ps->weaponstate = WEAPON_READY;
	return 1;
}

void PM_Weapon_SetFPSFireAnim(playerState_s *ps)
{
	if ( ps->fWeaponPosFrac <= 0.75 )
	{
		if ( PM_WeaponClipEmpty(ps) )
			PM_StartWeaponAnim(ps, WEAP_ATTACK_LASTSHOT);
		else
			PM_StartWeaponAnim(ps, WEAP_ATTACK);
	}
	else if ( PM_WeaponClipEmpty(ps) )
	{
		PM_StartWeaponAnim(ps, WEAP_ADS_ATTACK_LASTSHOT);
	}
	else
	{
		PM_StartWeaponAnim(ps, WEAP_ADS_ATTACK);
	}
}

int PM_Weapon_CheckFiringAmmo(playerState_s *ps)
{
	int reloadingW;
	int ammoNeeded;
	int activeCount;
	WeaponDef *weaponDef;

	reloadingW = 1;
	weaponDef = BG_GetWeaponDef(ps->weapon);
	activeCount = CL_LocalClient_GetActiveCount();

	if ( activeCount <= PM_WeaponAmmoAvailable(ps) )
		return 1;

	ammoNeeded = activeCount <= ps->ammo[BG_AmmoForWeapon(ps->weapon)];

	if ( weaponDef->weaponType == WEAPTYPE_GRENADE )
		reloadingW = 0;

	if ( reloadingW && !ammoNeeded )
		PM_AddEvent(ps, EV_NOAMMO);

	if ( ammoNeeded )
	{
		PM_BeginWeaponReload(ps);
	}
	else
	{
		COM_BitClear(ps->weaponrechamber, ps->weapon);
		PM_ContinueWeaponAnim(ps, WEAP_IDLE);
		ps->weaponTime += 500;
	}

	return 0;
}

void PM_HoldBreathFire(playerState_s *ps)
{
	unsigned int weapon;
	WeaponDef *weaponDef;
	int breathHoldTime;

	weapon = BG_GetViewmodelWeaponIndex(ps);
	weaponDef = BG_GetWeaponDef(weapon);

	if ( ps->fWeaponPosFrac == 1.0 && weaponDef->adsOverlayReticle && weaponDef->weaponClass != 9 )
	{
		breathHoldTime = (int)(player_breath_hold_time->current.decimal * 1000.0);

		if ( ps->holdBreathTimer < breathHoldTime )
		{
			ps->holdBreathTimer += (int)(player_breath_fire_delay->current.decimal * 1000.0);

			if ( ps->holdBreathTimer > breathHoldTime )
				ps->holdBreathTimer = breathHoldTime;
		}

		PM_EndHoldBreath(ps);
	}
}

void PM_Weapon_AddFiringAimSpreadScale(playerState_s *ps)
{
	WeaponDef *weaponDef;

	weaponDef = BG_GetWeaponDef(ps->weapon);

	if ( ps->fWeaponPosFrac != 1.0 )
	{
		ps->aimSpreadScale = weaponDef->hipSpreadFireAdd * 255.0 + ps->aimSpreadScale;

		if ( ps->aimSpreadScale > 255.0 )
			ps->aimSpreadScale = 255.0;
	}
}

int BG_TakePlayerWeapon(playerState_s *ps, int weaponIndex)
{
	WeaponDef *altWeaponDef;
	WeaponDef *weaponDef;
	int slot;
	int index;
	int altIndex;

	if ( !COM_BitTest(ps->weapons, weaponIndex) )
		return 0;

	weaponDef = BG_GetWeaponDef(weaponIndex);
	slot = BG_PlayerHasWeapon(ps, weaponIndex, 1);

	if ( slot )
	{
		if ( weaponDef->slotStackable )
		{
			for ( index = 1; index <= bg_iNumWeapons; ++index )
			{
				altWeaponDef = BG_GetWeaponDef(weaponIndex);

				if ( altWeaponDef->slotStackable
				        && altWeaponDef->weaponSlot == weaponDef->weaponSlot
				        && COM_BitTest(ps->weapons, index)
				        && !BG_PlayerHasWeapon(ps, index, 1) )
				{
					ps->weaponslots[slot] = index;
					break;
				}
			}

			if ( index > bg_iNumWeapons )
				ps->weaponslots[slot] = 0;
		}
		else
		{
			ps->weaponslots[slot] = 0;
		}
	}

	COM_BitClear(ps->weapons, weaponIndex);

	for ( altIndex = weaponDef->altWeaponIndex;
	        altIndex && COM_BitTest(ps->weapons, altIndex);
	        altIndex = BG_GetWeaponDef(altIndex)->altWeaponIndex )
	{
		COM_BitClear(ps->weapons, altIndex);
	}

	if ( weaponIndex == ps->weapon )
		ps->weapon = 0;

	return 1;
}

void BG_SwitchWeaponsIfEmpty(playerState_s *ps)
{
	if ( BG_WeaponIsClipOnly(ps->weapon)
	        && !ps->ammoclip[BG_ClipForWeapon(ps->weapon)]
	        && !ps->ammo[BG_AmmoForWeapon(ps->weapon)] )
	{
		BG_TakePlayerWeapon(ps, ps->weapon);
		PM_AddEvent(ps, EV_NOAMMO);
	}
}

void PM_Weapon_StartFiring(playerState_s *ps, int delayedAction)
{
	WeaponDef *weaponDef;

	weaponDef = BG_GetWeaponDef(ps->weapon);

	if ( weaponDef->weaponType == WEAPTYPE_GRENADE )
	{
		if ( !delayedAction )
		{
			if ( PM_WeaponAmmoAvailable(ps) )
			{
				ps->grenadeTimeLeft = weaponDef->fuseTime;
				PM_StartWeaponAnim(ps, WEAP_HOLD_FIRE);
				PM_AddEvent(ps, EV_PULLBACK_WEAPON);
			}
			ps->weaponDelay = weaponDef->holdFireTime;
			ps->weaponTime = 0;
		}
	}
	else
	{
		ps->weaponDelay = weaponDef->fireDelay;
		ps->weaponTime = weaponDef->fireTime;

		if ( weaponDef->adsFire )
			ps->weaponDelay = (int)((1.0 - ps->fWeaponPosFrac) * (1.0 / weaponDef->OOPosAnimLength[0]));

		BG_AnimScriptEvent(ps, ANIM_ET_FIREWEAPON, 0, 1);

		if ( weaponDef->boltAction )
			COM_BitSet(ps->weaponrechamber, ps->weapon);

		if ( ps->weaponstate != WEAPON_FIRING )
		{
			if ( ps->fWeaponPosFrac < 1.0 )
				ps->weaponRestrictKickTime = weaponDef->fireDelay
				                             + weaponDef->hipGunKickReducedKickBullets * weaponDef->fireTime;
			else
				ps->weaponRestrictKickTime = weaponDef->fireDelay
				                             + weaponDef->adsGunKickReducedKickBullets * weaponDef->fireTime;
		}
	}

	ps->weaponstate = WEAPON_FIRING;
	PM_SetProneMovementOverride(ps);
}

void PM_Weapon_FireWeapon(playerState_s *ps, int delayedAction)
{
	int activeCount;
	WeaponDef *weaponDef;

	weaponDef = BG_GetWeaponDef(ps->weapon);
	PM_Weapon_StartFiring(ps, delayedAction);

	if ( PM_Weapon_CheckFiringAmmo(ps) && !ps->weaponDelay )
	{
		if ( PM_WeaponAmmoAvailable(ps) != -1 && (ps->eFlags & 0x300) == 0 )
		{
			activeCount = CL_LocalClient_GetActiveCount();
			PM_WeaponUseAmmo(ps, ps->weapon, activeCount);
		}

		if ( weaponDef->weaponType == WEAPTYPE_GRENADE )
			ps->weaponTime = weaponDef->fireTime;

		PM_Weapon_SetFPSFireAnim(ps);

		if ( PM_WeaponClipEmpty(ps) )
			PM_AddEvent(ps, EV_FIRE_WEAPON_LASTSHOT);
		else
			PM_AddEvent(ps, EV_FIRE_WEAPON);

		PM_HoldBreathFire(ps);
		PM_Weapon_AddFiringAimSpreadScale(ps);
		BG_SwitchWeaponsIfEmpty(ps);
	}
}

void PM_Weapon(pmove_t *pm, pml_t *pml)
{
	playerState_s *ps;
	int delayedAction;

	ps = pm->ps;

	if ( (pm->ps->pm_flags & 0x1000) == 0 )
	{
		if ( ps->pm_type <= 5 )
		{
			if ( (ps->eFlags & 0x300) == 0 )
			{
				PM_UpdateAimDownSightLerp(pm, pml);
				PM_UpdateHoldBreath(pm, pml);

				if ( !PM_UpdateGrenadeThrow(ps, pml) )
				{
					delayedAction = PM_Weapon_WeaponTimeAdjust(pm, pml);

					PM_Weapon_CheckForBinoculars(pm);
					PM_Weapon_CheckForOffHand(pm);
					PM_Weapon_CheckForChangeWeapon(pm);
					PM_Weapon_CheckForReload(pm);
					PM_Weapon_CheckForMelee(pm, delayedAction);

					if ( !PM_Weapon_CheckForRechamber(ps, delayedAction) )
					{
						if ( (ps->pm_flags & 1) != 0 && (pm->cmd.forwardmove || pm->cmd.rightmove)
						        || ps->weaponstate == WEAPON_MELEE_INIT
						        || ps->weaponstate == WEAPON_MELEE_FIRE )
						{
							ps->aimSpreadScale = 255.0;
						}

						if ( delayedAction || !ps->weaponTime && !ps->weaponDelay )
						{
							switch ( ps->weaponstate )
							{
							case WEAPON_RAISING:
								PM_Weapon_FinishWeaponRaise(ps);
								break;

							case WEAPON_DROPPING:
								PM_Weapon_FinishWeaponChange(pm);
								break;

							case WEAPON_RELOADING:
							case WEAPON_RELOADING_INTERUPT:
								PM_Weapon_FinishReload(ps, delayedAction);
								break;

							case WEAPON_RELOAD_START:
							case WEAPON_RELOAD_START_INTERUPT:
								PM_Weapon_FinishReloadStart(ps, delayedAction);
								break;

							case WEAPON_RELOAD_END:
								PM_Weapon_FinishReloadEnd(ps);
								break;

							case WEAPON_MELEE_INIT:
								PM_Weapon_MeleeFire(ps);
								break;

							case WEAPON_MELEE_FIRE:
								PM_Weapon_MeleeEnd(ps);
								break;

							case WEAPON_OFFHAND_INIT:
								PM_Weapon_OffHandPrepare(ps);
								break;

							case WEAPON_OFFHAND_PREPARE:
								PM_Weapon_OffHandHold(ps);
								break;

							case WEAPON_OFFHAND_HOLD:
								PM_Weapon_OffHand(pm);
								break;

							case WEAPON_OFFHAND:
								PM_Weapon_OffHandEnd(ps);
								break;

							case WEAPON_OFFHAND_END:
							case WEAPON_BINOCULARS_END:
								PM_Weapon_Idle(ps);
								break;

							case WEAPON_BINOCULARS_INIT:
								PM_Weapon_BinocularsPrepare(ps);
								break;

							case WEAPON_BINOCULARS_PREPARE:
								PM_Weapon_BinocularsHold(ps);
								break;

							case WEAPON_BINOCULARS_HOLD:
								PM_Weapon_Binoculars(ps);
								break;

							case WEAPON_BINOCULARS_START:
								return;

							case WEAPON_BINOCULARS_DROP:
								PM_Weapon_BinocularsEnd(pm);
								break;

							default:
								if ( ps->weapon )
								{
									if ( !PM_Weapon_ShouldBeFiring(pm, delayedAction) )
										PM_Weapon_FireWeapon(ps, delayedAction);
								}
								break;
							}
						}
					}
				}
			}
		}
		else
		{
			ps->weapon = 0;
		}
	}
}

float BG_GetBobCycle(gclient_s *client)
{
	return (long double)LOBYTE(client->ps.bobCycle) / 255.0 * 3.141592653589793
	       + (long double)LOBYTE(client->ps.bobCycle) / 255.0 * 3.141592653589793
	       + 6.283185307179586;
}

float QDECL BG_GetVerticalBobFactor(const struct playerState_s *ps, float cycle, float speed, float maxAmp)
{
	long double v4;
	float v6;
	float v7;
	float v8;
	float v10;
	float amplitude;

	if ( ps->viewHeightTarget == 11 )
	{
		amplitude = speed * bg_bobAmplitudeProne->current.decimal;
	}
	else
	{
		if ( ps->viewHeightTarget == 40 )
			v4 = speed * bg_bobAmplitudeDucked->current.decimal;
		else
			v4 = speed * bg_bobAmplitudeStanding->current.decimal;
		amplitude = v4;
	}
	if ( amplitude > (long double)maxAmp )
		amplitude = maxAmp;
	v6 = sin(cycle + cycle);
	v10 = sin(cycle * 4.0 + 1.570796326794897);
	v7 = v10 * 0.2 + v6;
	v8 = 0.75 * v7;
	return v8 * amplitude;
}

float QDECL BG_GetHorizontalBobFactor(const struct playerState_s *ps, float cycle, float speed, float maxAmp)
{
	long double v4;
	float v6;
	float amplitude;

	if ( ps->viewHeightTarget == 11 )
	{
		amplitude = speed * bg_bobAmplitudeProne->current.decimal;
	}
	else
	{
		if ( ps->viewHeightTarget == 40 )
			v4 = speed * bg_bobAmplitudeDucked->current.decimal;
		else
			v4 = speed * bg_bobAmplitudeStanding->current.decimal;
		amplitude = v4;
	}
	if ( amplitude > (long double)maxAmp )
		amplitude = maxAmp;
	v6 = sin(cycle);
	return v6 * amplitude;
}

void BG_GetSpreadForWeapon(const playerState_s *ps, int weaponIndex, float *minSpread, float *maxSpread)
{
	float frac;
	WeaponDef *weaponDef;

	weaponDef = BG_GetWeaponDef(weaponIndex);

	if ( ps->viewHeightCurrent <= 40.0 )
	{
		frac = (ps->viewHeightCurrent - 11.0) / 29.0;

		*minSpread = (weaponDef->hipSpreadDuckedMin - weaponDef->hipSpreadProneMin) * frac + weaponDef->hipSpreadProneMin;
		*maxSpread = (weaponDef->hipSpreadDuckedMax - weaponDef->hipSpreadProneMax) * frac + weaponDef->hipSpreadProneMax;
	}
	else
	{
		frac = (ps->viewHeightCurrent - 40.0) / 20.0;

		*minSpread = (weaponDef->hipSpreadStandMin - weaponDef->hipSpreadDuckedMin) * frac + weaponDef->hipSpreadDuckedMin;
		*maxSpread = (weaponDef->hipSpreadMax - weaponDef->hipSpreadDuckedMax) * frac + weaponDef->hipSpreadDuckedMax;
	}
}