#include "../qcommon/qcommon.h"
#include "bg_public.h"

#ifdef TESTING_LIBRARY
#define bg_weapClips (((WeaponDef**)( 0x085767A0 )))
#else
WeaponDef* bg_weapClips[128];
#endif

int BG_GetClipSize(int weaponIndex)
{
	return bg_weapClips[weaponIndex]->clipSize;
}

int BG_AmmoForWeapon(int weapon)
{
	return BG_GetWeaponDef(weapon)->ammoIndex;
}

int BG_ClipForWeapon(int weapon)
{
	return BG_GetWeaponDef(weapon)->clipIndex;
}

int BG_WeaponIsClipOnly(int weapon)
{
	return BG_GetWeaponDef(weapon)->clipOnly;
}

void PM_WeaponUseAmmo(playerState_s *ps, int weaponIndex, int amount)
{
	int index;

	index = BG_ClipForWeapon(weaponIndex);
	ps->ammoclip[index] -= amount;
}

int PM_WeaponAmmoAvailable(playerState_s *ps)
{
	return ps->ammoclip[BG_ClipForWeapon(ps->weapon)];
}

int BG_WeaponAmmo(const playerState_s *ps, int weaponIndex)
{
	int index;

	index = BG_AmmoForWeapon(weaponIndex);
	return ps->ammo[index] + ps->ammoclip[BG_ClipForWeapon(weaponIndex)];
}

qboolean PM_WeaponClipEmpty(playerState_s *ps)
{
  return ps->ammoclip[BG_ClipForWeapon(ps->weapon)] == 0;
}