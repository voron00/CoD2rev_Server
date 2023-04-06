#include "../qcommon/qcommon.h"
#include "bg_public.h"

#ifdef TESTING_LIBRARY
#define bg_weaponDefs (((WeaponDef**)( 0x08576160 )))
#define bg_iNumWeapons (*((unsigned int*)( 0x08576140 )))
#else
WeaponDef* bg_weaponDefs[128];
unsigned int bg_iNumWeapons;
#endif

#ifdef TESTING_LIBRARY
#define bg_itemlist ((gitem_t*)( 0x08164C20 ))
#else
extern gitem_t bg_itemlist[];
#endif

#ifdef TESTING_LIBRARY
#define bg_weapAmmoTypes (((WeaponDef**)( 0x08576360 )))
#define bg_weapClips (((WeaponDef**)( 0x085767A0 )))
#define bg_sharedAmmoCaps (((WeaponDef**)( 0x08576580 )))
#else
WeaponDef *bg_weapAmmoTypes[128];
WeaponDef *bg_weapClips[128];
WeaponDef *bg_sharedAmmoCaps[128];
#endif

#ifdef TESTING_LIBRARY
#define bg_iNumAmmoTypes (*((unsigned int*)( 0x08576560 )))
#define bg_iNumWeapClips (*((unsigned int*)( 0x085769A0 )))
#define bg_iNumSharedAmmoCaps (*((unsigned int*)( 0x08576780 )))
#else
unsigned int bg_iNumAmmoTypes;
unsigned int bg_iNumWeapClips;
unsigned int bg_iNumSharedAmmoCaps;
#endif

unsigned int BG_GetNumWeapons()
{
	return bg_iNumWeapons;
}

WeaponDef* QDECL BG_GetWeaponDef(int weaponIndex)
{
	return bg_weaponDefs[weaponIndex];
}

int BG_FindWeaponIndexForName(const char *name)
{
	int i;

	for ( i = 1; i <= bg_iNumWeapons; ++i )
	{
		if ( !I_stricmp(name, bg_weaponDefs[i]->szInternalName) )
			return i;
	}

	return 0;
}

int BG_GetWeaponIndexForName(const char *name, int (*regWeap)(int))
{
	WeaponDef *weaponDef;
	int index;

	if ( !*name || !strcasecmp(name, "none") )
		return 0;

	index = BG_FindWeaponIndexForName(name);

	if ( index )
		return index;

	weaponDef = BG_LoadWeaponDef("mp", name);

	if ( weaponDef )
		return BG_SetupWeaponDef(weaponDef, regWeap);

	Com_DPrintf("Couldn't find weapon \"%s\"\n", name);
	return 0;
}

void BG_SetupWeaponAlts(int weapIndex, int (*regWeap)(int))
{
	WeaponDef *altWeaponDef;
	WeaponDef *weaponDef;
	int index;

	weaponDef = BG_GetWeaponDef(weapIndex);
	weaponDef->altWeaponIndex = 0;

	if ( *weaponDef->altWeapon )
	{
		index = BG_GetWeaponIndexForName(weaponDef->altWeapon, regWeap);

		if ( !index )
			Com_Error(ERR_DROP, "could not find altWeapon '%s' for weapon '%s'", weaponDef->altWeapon, weaponDef->szInternalName);

		weaponDef->altWeaponIndex = index;
		altWeaponDef = bg_weaponDefs[index];

		if ( weaponDef->weaponSlot != altWeaponDef->weaponSlot )
			Com_Error(ERR_DROP, "weapon '%s' does not have same weaponSlot setting as its alt weapon '%s'", weaponDef->szInternalName, altWeaponDef->szInternalName);

		if ( weaponDef->slotStackable != altWeaponDef->slotStackable )
			Com_Error(ERR_DROP, "weapon '%s' does not have same slotStackable setting as its alt weapon '%s'", weaponDef->szInternalName, altWeaponDef->szInternalName);
	}
}

void BG_SetupAmmoIndexes(int weapIndex)
{
	WeaponDef *altWeaponDef;
	int j;
	int i;
	WeaponDef *weaponDef;

	weaponDef = BG_GetWeaponDef(weapIndex);

	for ( i = 0; ; ++i )
	{
		if ( i >= bg_iNumAmmoTypes )
		{
			bg_weapAmmoTypes[i] = weaponDef;
			weaponDef->ammoIndex = i;
			++bg_iNumAmmoTypes;
			return;
		}

		if ( !strcmp(bg_weapAmmoTypes[i]->ammoName, weaponDef->ammoName) )
			break;
	}

	weaponDef->ammoIndex = i;

	if ( bg_weapAmmoTypes[i]->maxAmmo != weaponDef->maxAmmo && i )
	{
		for ( j = 1; j < weapIndex; ++j )
		{
			altWeaponDef = bg_weaponDefs[j];

			if ( !I_stricmp(bg_weapAmmoTypes[i]->ammoName, altWeaponDef->ammoName)
			        && altWeaponDef->maxAmmo == bg_weapAmmoTypes[i]->maxAmmo )
			{
				Com_Error(
				    ERR_DROP,
				    "Max ammo mismatch for \"%s\" ammo: '%s\" set it to %i, but \"%s\" already set it to %i.\n",
				    weaponDef->ammoName,
				    weaponDef->szInternalName,
				    weaponDef->maxAmmo,
				    altWeaponDef->szInternalName,
				    altWeaponDef->maxAmmo);
			}
		}
	}
}

void BG_SetupSharedAmmoIndexes(int weapIndex)
{
	WeaponDef *altWeaponDef;
	int j;
	WeaponDef *weaponDef;
	int i;

	weaponDef = BG_GetWeaponDef(weapIndex);
	weaponDef->sharedAmmoCapIndex = -1;

	if ( *weaponDef->sharedAmmoCapName )
	{
		Com_DPrintf("%s: %s\n", weaponDef->szInternalName, weaponDef->sharedAmmoCapName);

		for ( i = 0; ; ++i )
		{
			if ( i >= bg_iNumSharedAmmoCaps )
			{
				bg_sharedAmmoCaps[i] = weaponDef;
				weaponDef->sharedAmmoCapIndex = i;
				++bg_iNumSharedAmmoCaps;
				return;
			}

			if ( !I_stricmp(bg_sharedAmmoCaps[i]->sharedAmmoCapName, weaponDef->sharedAmmoCapName) )
				break;
		}

		weaponDef->sharedAmmoCapIndex = i;

		if ( bg_sharedAmmoCaps[i]->sharedAmmoCap != weaponDef->sharedAmmoCap && i )
		{
			for ( j = 1; j < weapIndex; ++j )
			{
				altWeaponDef = bg_weaponDefs[j];

				if ( !I_stricmp(bg_sharedAmmoCaps[i]->sharedAmmoCapName, altWeaponDef->sharedAmmoCapName)
				        && altWeaponDef->sharedAmmoCap == bg_sharedAmmoCaps[i]->sharedAmmoCap )
				{
					Com_Error(
					    ERR_DROP,
					    "Shared ammo cap mismatch for \"%s\" shared ammo cap: '%s\" set it to %i, but \"%s\" already set it to %i.\n",
					    weaponDef->sharedAmmoCapName,
					    weaponDef->szInternalName,
					    weaponDef->sharedAmmoCap,
					    altWeaponDef->szInternalName,
					    altWeaponDef->sharedAmmoCap);
				}
			}
		}
	}
}

void BG_SetupClipIndexes(int weapIndex)
{
	WeaponDef *altWeaponDef;
	int j;
	WeaponDef *weaponDef;
	int i;

	weaponDef = BG_GetWeaponDef(weapIndex);

	for ( i = 0; ; ++i )
	{
		if ( i >= bg_iNumWeapClips )
		{
			bg_weapClips[i] = weaponDef;
			weaponDef->clipIndex = i;
			++bg_iNumWeapClips;
			return;
		}

		if ( !strcmp(bg_weapClips[i]->clipName, weaponDef->clipName) )
			break;
	}

	weaponDef->clipIndex = i;

	if ( bg_weapClips[i]->clipSize != weaponDef->clipSize && i )
	{
		for ( j = 1; j < weapIndex; ++j )
		{
			altWeaponDef = bg_weaponDefs[j];

			if ( !I_stricmp(bg_weapClips[i]->clipName, altWeaponDef->clipName)
			        && altWeaponDef->clipSize == bg_weapClips[i]->clipSize )
			{
				Com_Error(
				    ERR_DROP,
				    "Clip Size mismatch for \"%s\" clip: '%s\" set it to %i, but \"%s\" already set it to %i.\n",
				    weaponDef->ammoName,
				    weaponDef->szInternalName,
				    weaponDef->clipSize,
				    altWeaponDef->szInternalName,
				    altWeaponDef->clipSize);
			}
		}
	}
}

void BG_FillInWeaponItems(int weapIndex)
{
	WeaponDef *weaponDef;
	gitem_s *item;

	weaponDef = BG_GetWeaponDef(weapIndex);
	item = &bg_itemlist[weapIndex];
	item->pickup_sound = weaponDef->pickupSound;
	item->world_model = weaponDef->worldModel;
	item->view_model = 0;
	item->icon = weaponDef->hudIcon;
	item->pickup_name = weaponDef->szDisplayName;
	item->quantity = weaponDef->startAmmo;
	item->giType = IT_WEAPON;
	item->giTag = weapIndex;
	item->giAmmoIndex = weaponDef->ammoIndex;
	item->giClipIndex = weaponDef->clipIndex;
}

void BG_SetupWeaponIndex(int weapIndex)
{
	BG_SetupAmmoIndexes(weapIndex);
	BG_SetupSharedAmmoIndexes(weapIndex);
	BG_SetupClipIndexes(weapIndex);
	BG_FillInWeaponItems(weapIndex);
}

int BG_SetupWeaponDef(WeaponDef *weapDef, int (*regWeap)(int))
{
	int num;

	num = ++bg_iNumWeapons;

	bg_weaponDefs[bg_iNumWeapons] = weapDef;
	BG_SetupWeaponIndex(num);
	BG_SetupWeaponAlts(num, regWeap);

	if ( regWeap )
		regWeap(num);

	return num;
}

// Don't hook that
void BG_ClearWeaponDef()
{
	int i;

	bg_weaponDefs[0] = BG_LoadDefaultWeaponDef();
	bg_weapAmmoTypes[0] = bg_weaponDefs[0];
	bg_iNumAmmoTypes = 1;
	bg_sharedAmmoCaps[0] = bg_weaponDefs[0];
	bg_iNumSharedAmmoCaps = 1;
	bg_weapClips[0] = bg_weaponDefs[0];
	bg_iNumWeapClips = 1;

	for ( i = 1; i <= 128; ++i )
		bg_itemlist[i].giType = IT_BAD;

	BG_LoadPlayerAnimTypes();
	BG_InitWeaponStrings();
}