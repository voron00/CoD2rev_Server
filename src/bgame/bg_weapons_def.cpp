#include "../qcommon/qcommon.h"
#include "bg_public.h"

#ifdef TESTING_LIBRARY
#define bg_weaponDefs (((WeaponDef**)( 0x08576160 )))
#define bg_iNumWeapons (*((unsigned int*)( 0x08576140 )))
#else
WeaponDef* bg_weaponDefs[128];
unsigned int bg_iNumWeapons;
#endif

unsigned int BG_GetNumWeapons()
{
	return bg_iNumWeapons;
}

WeaponDef* QDECL BG_GetWeaponDef(int weaponIndex)
{
	return bg_weaponDefs[weaponIndex];
}