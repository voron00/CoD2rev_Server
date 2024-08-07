#include "../qcommon/qcommon.h"
#include "bg_public.h"

int BG_GetClipSize(int weaponIndex)
{
	return bg_weapClips[weaponIndex]->clipSize;
}

int BG_GetMaxAmmo(int weaponIndex)
{
	return bg_weapAmmoTypes[weaponIndex]->maxAmmo;
}

int BG_GetSharedAmmoCapSize(int weaponIndex)
{
	return bg_sharedAmmoCaps[weaponIndex]->sharedAmmoCap;
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

int BG_GetMaxPickupableAmmo(const playerState_s *ps, unsigned int weaponIndex)
{
	int altClip;
	int altAmmo;
	int clipSize;
	int maxAmmo;
	int weapClipPool[128];
	int weapAmmoPool[128];
	int clipForWeapon;
	int ammoForWeapon;
	int sharedAmmo;
	WeaponDef *altWeaponDef;
	WeaponDef *weaponDef;
	int index;

	memset(weapAmmoPool, 0, sizeof(weapAmmoPool));
	memset(weapClipPool, 0, sizeof(weapClipPool));

	weaponDef = BG_GetWeaponDef(weaponIndex);

	ammoForWeapon = BG_AmmoForWeapon(weaponIndex);
	clipForWeapon = BG_ClipForWeapon(weaponIndex);

	if ( weaponDef->sharedAmmoCapIndex < 0 )
	{
		if ( BG_WeaponIsClipOnly(weaponIndex) )
		{
			clipSize = BG_GetClipSize(clipForWeapon);
			return clipSize - ps->ammoclip[clipForWeapon];
		}
		else
		{
			maxAmmo = BG_GetMaxAmmo(ammoForWeapon);
			return maxAmmo - ps->ammo[ammoForWeapon];
		}
	}
	else
	{
		sharedAmmo = BG_GetSharedAmmoCapSize(weaponDef->sharedAmmoCapIndex);

		for ( index = 1; index <= bg_iNumWeapons; ++index )
		{
			if ( Com_BitCheck(ps->weapons, index) )
			{
				altWeaponDef = BG_GetWeaponDef(index);

				if ( altWeaponDef->sharedAmmoCapIndex == weaponDef->sharedAmmoCapIndex )
				{
					if ( BG_WeaponIsClipOnly(index) )
					{
						if ( !weapClipPool[BG_ClipForWeapon(index)] )
						{
							weapClipPool[BG_ClipForWeapon(index)] = 1;
							altClip = BG_ClipForWeapon(index);
							sharedAmmo -= ps->ammoclip[altClip];
						}
					}
					else if ( !weapAmmoPool[BG_AmmoForWeapon(index)] )
					{
						weapAmmoPool[BG_AmmoForWeapon(index)] = 1;
						altAmmo = BG_AmmoForWeapon(index);
						sharedAmmo -= ps->ammo[altAmmo];
					}
				}
			}
		}
	}

	return sharedAmmo;
}