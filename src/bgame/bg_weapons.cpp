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

unsigned int BG_GetViewmodelWeaponIndex(const playerState_s *ps)
{
	if ( (ps->pm_flags & 0x20000) != 0 )
		return BG_GetBinocularsIndex();

	if ( (ps->pm_flags & 0x10) != 0 )
		return ps->offHandIndex;

	return ps->weapon;
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