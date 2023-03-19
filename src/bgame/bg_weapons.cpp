#include "../qcommon/qcommon.h"
#include "bg_public.h"

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
