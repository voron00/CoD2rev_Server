#include "../qcommon/qcommon.h"
#include "bg_public.h"

WeaponDef* bg_weaponDefs[MAX_WEAPONS];
int bg_iNumWeapons;

WeaponDef *bg_weapAmmoTypes[MAX_WEAPONS];
WeaponDef *bg_weapClips[MAX_WEAPONS];
WeaponDef *bg_sharedAmmoCaps[MAX_WEAPONS];

int bg_iNumAmmoTypes;
int bg_iNumWeapClips;
int bg_iNumSharedAmmoCaps;

/*
===============
BG_GetBobCycle
===============
*/
float BG_GetBobCycle( const playerState_t *ps )
{
	assert(ps);
	return (ps->bobCycle & 255) / 255 * M_PI + (ps->bobCycle & 255) / 255 * M_PI + (M_PI * 2);
}

/*
===============
PM_IsBinocularsADS
===============
*/
bool PM_IsBinocularsADS( const playerState_t *ps )
{
	assert(ps);

	if ( ps->weaponstate == WEAPON_BINOCULARS_HOLD )
	{
		return true;
	}

	if ( ps->weaponstate == WEAPON_BINOCULARS_EXIT_ADS )
	{
		return true;
	}

	return false;
}

/*
===============
BG_IsAnyEmptyPrimaryWeaponSlot
===============
*/
bool BG_IsAnyEmptyPrimaryWeaponSlot( const playerState_t *ps )
{
	assert(ps);

	if ( !ps->weaponslots[SLOT_PRIMARY] )
	{
		return true;
	}

	if ( !ps->weaponslots[SLOT_PRIMARYB] )
	{
		return true;
	}

	return false;
}

/*
===============
BG_GetAmmoClipName
===============
*/
const char *BG_GetAmmoClipName( int iClipIndex )
{
	return bg_weapClips[iClipIndex]->clipName;
}

/*
===============
BG_GetAmmoTypeName
===============
*/
const char *BG_GetAmmoTypeName( int iAmmoIndex )
{
	return bg_weapAmmoTypes[iAmmoIndex]->ammoName;
}

/*
===============
BG_GetSharedAmmoCapName
===============
*/
const char *BG_GetSharedAmmoCapName( int iAmmoIndex )
{
	return bg_sharedAmmoCaps[iAmmoIndex]->sharedAmmoCapName;
}

/*
===============
BG_GetSharedAmmoCapSize
===============
*/
int BG_GetSharedAmmoCapSize( int iAmmoIndex )
{
	return bg_sharedAmmoCaps[iAmmoIndex]->sharedAmmoCap;
}

/*
===============
BG_GetAmmoClipSize
===============
*/
int BG_GetAmmoClipSize( int iClipIndex )
{
	return bg_weapClips[iClipIndex]->clipSize;
}

/*
===============
BG_GetNumAmmoClips
===============
*/
int BG_GetNumAmmoClips()
{
	return bg_iNumWeapClips;
}

/*
===============
BG_GetAmmoTypeMax
===============
*/
int BG_GetAmmoTypeMax( int iAmmoIndex )
{
	return bg_weapAmmoTypes[iAmmoIndex]->maxAmmo;
}

/*
===============
BG_GetNumAmmoTypes
===============
*/
int BG_GetNumAmmoTypes()
{
	return bg_iNumAmmoTypes;
}

/*
===============
BG_GetNumWeapons
===============
*/
int BG_GetNumWeapons()
{
	return bg_iNumWeapons;
}

/*
===============
BG_GetWeaponDef
===============
*/
WeaponDef *BG_GetWeaponDef( int iWeapon )
{
	return bg_weaponDefs[iWeapon];
}

/*
===============
PM_StartWeaponAnim
===============
*/
void PM_StartWeaponAnim( playerState_t *ps, int anim )
{
	assert(ps);

	if ( ps->pm_type >= PM_DEAD )
	{
		return;
	}

	ps->weapAnim = ( anim | ( ps->weapAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT );
}

/*
===============
BG_WeaponIsClipOnly
===============
*/
int BG_WeaponIsClipOnly( int weapon )
{
	return BG_GetWeaponDef(weapon)->clipOnly;
}

/*
===============
BG_AmmoForWeapon
===============
*/
int BG_AmmoForWeapon( int weapon )
{
	return BG_GetWeaponDef(weapon)->ammoIndex;
}

/*
===============
BG_ClipForWeapon
===============
*/
int BG_ClipForWeapon( int weapon )
{
	return BG_GetWeaponDef(weapon)->clipIndex;
}

/*
===============
BG_GetSpreadForWeapon
===============
*/
void BG_GetSpreadForWeapon( const playerState_t *ps, int weaponIndex, float *minSpread, float *maxSpread )
{
	float frac;
	WeaponDef *weapDef;

	assert(ps);
	weapDef = BG_GetWeaponDef(weaponIndex);

	if ( ps->viewHeightCurrent > CROUCH_VIEWHEIGHT )
	{
		frac = (ps->viewHeightCurrent - CROUCH_VIEWHEIGHT) / 20.0;

		*minSpread = (weapDef->hipSpreadStandMin - weapDef->hipSpreadDuckedMin) * frac + weapDef->hipSpreadDuckedMin;
		*maxSpread = (weapDef->hipSpreadMax - weapDef->hipSpreadDuckedMax) * frac + weapDef->hipSpreadDuckedMax;
	}
	else
	{
		frac = (ps->viewHeightCurrent - PRONE_VIEWHEIGHT) / 29.0;

		*minSpread = (weapDef->hipSpreadDuckedMin - weapDef->hipSpreadProneMin) * frac + weapDef->hipSpreadProneMin;
		*maxSpread = (weapDef->hipSpreadDuckedMax - weapDef->hipSpreadProneMax) * frac + weapDef->hipSpreadProneMax;
	}
}

/*
===============
BG_GetTotalAmmoReserve
===============
*/
int BG_GetTotalAmmoReserve( const playerState_t *ps, int weaponIndex )
{
	qboolean clipCounted[MAX_WEAPONS];
	qboolean ammoCounted[MAX_WEAPONS];
	int iClipIndex;
	int iAmmoIndex;
	int ammo;
	int curWeaponIndex;
	WeaponDef *weapDef;

	assert(ps);
	ammo = 0;

	iAmmoIndex = BG_AmmoForWeapon(weaponIndex);
	iClipIndex = BG_ClipForWeapon(weaponIndex);

	memset(ammoCounted, 0, sizeof(ammoCounted));
	memset(clipCounted, 0, sizeof(clipCounted));

	weapDef = BG_GetWeaponDef(weaponIndex);

	if ( weapDef->sharedAmmoCapIndex < 0 )
	{
		if ( BG_WeaponIsClipOnly(weaponIndex) )
		{
			return ps->ammoclip[iClipIndex];
		}
		else
		{
			return ps->ammo[iAmmoIndex];
		}
	}

	for ( curWeaponIndex = 1; curWeaponIndex <= bg_iNumWeapons; curWeaponIndex++ )
	{
		if ( !Com_BitCheck(ps->weapons, curWeaponIndex) )
		{
			continue;
		}

		if ( BG_GetWeaponDef(curWeaponIndex)->sharedAmmoCapIndex != weapDef->sharedAmmoCapIndex )
		{
			continue;
		}

		if ( BG_WeaponIsClipOnly(curWeaponIndex) )
		{
			if ( !clipCounted[BG_ClipForWeapon(curWeaponIndex)] )
			{
				clipCounted[BG_ClipForWeapon(curWeaponIndex)] = qtrue;
				ammo += ps->ammoclip[BG_ClipForWeapon(curWeaponIndex)];
			}
		}
		else
		{
			if ( !ammoCounted[BG_AmmoForWeapon(curWeaponIndex)] )
			{
				ammoCounted[BG_AmmoForWeapon(curWeaponIndex)] = qtrue;
				ammo += ps->ammo[BG_AmmoForWeapon(curWeaponIndex)];
			}
		}
	}

	return ammo;
}

/*
===============
BG_GetMaxPickupableAmmo
===============
*/
int BG_GetMaxPickupableAmmo( const playerState_t *ps, int weaponIndex )
{
	qboolean clipCounted[MAX_WEAPONS];
	qboolean ammoCounted[MAX_WEAPONS];
	int iClipIndex;
	int iAmmoIndex;
	int ammo;
	int curWeaponIndex;
	WeaponDef *weapDef;

	assert(ps);

	memset(ammoCounted, 0, sizeof(ammoCounted));
	memset(clipCounted, 0, sizeof(clipCounted));

	weapDef = BG_GetWeaponDef(weaponIndex);

	iAmmoIndex = BG_AmmoForWeapon(weaponIndex);
	iClipIndex = BG_ClipForWeapon(weaponIndex);

	if ( weapDef->sharedAmmoCapIndex < 0 )
	{
		if ( BG_WeaponIsClipOnly(weaponIndex) )
		{
			return BG_GetAmmoClipSize(iClipIndex) - ps->ammoclip[iClipIndex];
		}
		else
		{
			return BG_GetAmmoTypeMax(iAmmoIndex) - ps->ammo[iAmmoIndex];
		}
	}

	ammo = BG_GetSharedAmmoCapSize(weapDef->sharedAmmoCapIndex);

	for ( curWeaponIndex = 1; curWeaponIndex <= bg_iNumWeapons; curWeaponIndex++ )
	{
		if ( !Com_BitCheck(ps->weapons, curWeaponIndex) )
		{
			continue;
		}

		if ( BG_GetWeaponDef(curWeaponIndex)->sharedAmmoCapIndex != weapDef->sharedAmmoCapIndex )
		{
			continue;
		}

		if ( BG_WeaponIsClipOnly(curWeaponIndex) )
		{
			if ( !clipCounted[BG_ClipForWeapon(curWeaponIndex)] )
			{
				clipCounted[BG_ClipForWeapon(curWeaponIndex)] = qtrue;
				ammo -= ps->ammoclip[BG_ClipForWeapon(curWeaponIndex)];
			}
		}
		else
		{
			if ( !ammoCounted[BG_AmmoForWeapon(curWeaponIndex)] )
			{
				ammoCounted[BG_AmmoForWeapon(curWeaponIndex)] = qtrue;
				ammo -= ps->ammo[BG_AmmoForWeapon(curWeaponIndex)];
			}
		}
	}

	return ammo;
}

/*
===============
BG_GetStackSlotForWeapon
===============
*/
int BG_GetStackSlotForWeapon( const playerState_t *pPS, int iWeaponIndex, unsigned int preferedSlot )
{
	WeaponDef *weapDef;

	assert(pPS);
	weapDef = BG_GetWeaponDef(iWeaponIndex);

	if ( !weapDef->slotStackable )
	{
		return SLOT_NONE;
	}

	if ( weapDef->weaponSlot - 1 > 1 )
	{
		return SLOT_NONE;
	}

	if ( preferedSlot == SLOT_PRIMARY || preferedSlot == SLOT_PRIMARYB )
	{
		if ( !pPS->weaponslots[preferedSlot] )
		{
			return preferedSlot;
		}

		if ( BG_GetWeaponDef(pPS->weaponslots[preferedSlot])->slotStackable )
		{
			return preferedSlot;
		}
	}

	if ( !pPS->weaponslots[SLOT_PRIMARY] )
	{
		return SLOT_PRIMARY;
	}

	if ( BG_GetWeaponDef(pPS->weaponslots[SLOT_PRIMARY])->slotStackable )
	{
		return SLOT_PRIMARY;
	}

	if ( !pPS->weaponslots[SLOT_PRIMARYB] )
	{
		return SLOT_PRIMARYB;
	}

	if ( BG_GetWeaponDef(pPS->weaponslots[SLOT_PRIMARYB])->slotStackable )
	{
		return SLOT_PRIMARYB;
	}

	return SLOT_NONE;
}

/*
===============
BG_GetEmptySlotForWeapon
===============
*/
int BG_GetEmptySlotForWeapon( playerState_t *pPS, int iWeaponIndex )
{
	assert(pPS);

	if ( BG_GetWeaponDef(iWeaponIndex)->weaponSlot - 1 > 1 )
	{
		return SLOT_NONE;
	}

	if ( !pPS->weaponslots[SLOT_PRIMARY] )
	{
		return SLOT_PRIMARY;
	}

	if ( !pPS->weaponslots[SLOT_PRIMARYB] )
	{
		return SLOT_PRIMARYB;
	}

	return SLOT_NONE;
}

/*
===============
BG_IsPlayerWeaponInSlot
===============
*/
int BG_IsPlayerWeaponInSlot( playerState_t *pPS, int iWeaponIndex, qboolean bAnyMode )
{
	WeaponDef *weapDef;
	int index;

	assert(pPS);

	if ( !Com_BitCheck(pPS->weapons, iWeaponIndex) )
	{
		return SLOT_NONE;
	}

	index = iWeaponIndex;

	do
	{
		weapDef = BG_GetWeaponDef(index);

		if ( weapDef->weaponSlot - 1 > 1 )
		{
			return SLOT_NONE;
		}

		if ( pPS->weaponslots[SLOT_PRIMARY] == index )
		{
			return SLOT_PRIMARY;
		}

		if ( pPS->weaponslots[SLOT_PRIMARYB] == index )
		{
			return SLOT_PRIMARYB;
		}

		if ( bAnyMode )
		{
			if ( weapDef->altWeaponIndex )
			{
				index = weapDef->altWeaponIndex;
			}
		}
	}
	while ( index != iWeaponIndex );

	return SLOT_NONE;
}

/*
===============
BG_SetPlayerWeaponForSlot
===============
*/
qboolean BG_SetPlayerWeaponForSlot( playerState_t *ps, unsigned int slot, int iWeaponIndex )
{
	assert(ps);

	if ( !Com_BitCheck(ps->weapons, iWeaponIndex) )
	{
		return qfalse;
	}

	if ( BG_GetWeaponDef(iWeaponIndex)->weaponSlot - 1 > 1 )
	{
		return qfalse;
	}

	if ( slot != SLOT_PRIMARY && slot != SLOT_PRIMARYB )
	{
		return qfalse;
	}

	if ( iWeaponIndex != ps->weaponslots[slot] && iWeaponIndex != ps->weapon )
	{
		ps->weapon = WP_NONE;
	}

	ps->weaponslots[slot] = iWeaponIndex;
	return qtrue;
}

/*
===============
BG_TakePlayerWeapon
===============
*/
int BG_TakePlayerWeapon( playerState_t *pPS, int iWeaponIndex )
{
	WeaponDef *weapCheck;
	WeaponDef *weapDef;
	int slot;
	int curWeaponIndex;

	assert(pPS);

	if ( !Com_BitCheck(pPS->weapons, iWeaponIndex) )
	{
		return qfalse;
	}

	weapDef = BG_GetWeaponDef(iWeaponIndex);
	slot = BG_IsPlayerWeaponInSlot(pPS, iWeaponIndex, qtrue);

	if ( slot )
	{
		if ( weapDef->slotStackable )
		{
			for ( curWeaponIndex = 1; curWeaponIndex <= bg_iNumWeapons; curWeaponIndex++ )
			{
				weapCheck = BG_GetWeaponDef(iWeaponIndex);

				if ( !weapCheck->slotStackable )
				{
					continue;
				}

				if ( weapCheck->weaponSlot != weapDef->weaponSlot )
				{
					continue;
				}

				if ( !Com_BitCheck(pPS->weapons, curWeaponIndex) )
				{
					continue;
				}

				if ( BG_IsPlayerWeaponInSlot(pPS, curWeaponIndex, qtrue) )
				{
					continue;
				}

				pPS->weaponslots[slot] = curWeaponIndex;
				break;
			}

			if ( curWeaponIndex > bg_iNumWeapons )
			{
				pPS->weaponslots[slot] = WP_NONE;
			}
		}
		else
		{
			pPS->weaponslots[slot] = WP_NONE;
		}
	}

	Com_BitClear(pPS->weapons, iWeaponIndex);

	for ( curWeaponIndex = weapDef->altWeaponIndex; curWeaponIndex && Com_BitCheck(pPS->weapons, curWeaponIndex); curWeaponIndex = BG_GetWeaponDef(curWeaponIndex)->altWeaponIndex )
	{
		Com_BitClear(pPS->weapons, curWeaponIndex);
	}

	if ( iWeaponIndex == pPS->weapon )
	{
		pPS->weapon = WP_NONE;
	}

	return qtrue;
}

/*
===============
BG_DoesWeaponNeedSlot
===============
*/
bool BG_DoesWeaponNeedSlot( int weaponIndex )
{
	WeaponDef *weapDef;

	weapDef = BG_GetWeaponDef(weaponIndex);
	return weapDef->weaponClass != WEAPCLASS_ITEM && weapDef->offhandClass == OFFHAND_CLASS_NONE;
}

/*
===============
BG_DoesWeaponRequireSlot
===============
*/
bool BG_DoesWeaponRequireSlot( int weaponIndex )
{
	WeaponDef *weapDef;

	weapDef = BG_GetWeaponDef(weaponIndex);
	return weapDef->weaponSlot == SLOT_PRIMARY || weapDef->weaponSlot == SLOT_PRIMARYB;
}

/*
===============
BG_ValidateWeaponNumber
===============
*/
bool BG_ValidateWeaponNumber( int weaponIndex )
{
	return weaponIndex >= WP_NONE && weaponIndex <= BG_GetNumWeapons();
}

/*
===============
BG_IsAimDownSightWeapon
===============
*/
qboolean BG_IsAimDownSightWeapon( int iWeapon )
{
	return BG_GetWeaponDef(iWeapon)->aimDownSight;
}

/*
===============
BG_GetFirstEquippedOffhand
===============
*/
int BG_GetFirstEquippedOffhand( const playerState_t *ps, int offhandClass )
{
	int numWeaps;
	int weapIndex;

	assert(ps);

	numWeaps = BG_GetNumWeapons();

	for ( weapIndex = 1; weapIndex <= numWeaps; weapIndex++ )
	{
		if ( BG_GetWeaponDef(weapIndex)->offhandClass != offhandClass )
		{
			continue;
		}

		if ( !Com_BitCheck(ps->weapons, weapIndex) )
		{
			continue;
		}

		return weapIndex;
	}

	return WP_NONE;
}

/*
===============
BG_GetViewmodelWeaponIndex
===============
*/
int BG_GetViewmodelWeaponIndex( const playerState_t *ps )
{
	assert(ps);

	if ( ps->pm_flags & PMF_BINOCULARS )
	{
		return BG_GetBinocularWeaponIndex();
	}

	if ( ps->pm_flags & PMF_FRAG )
	{
		return ps->offHandIndex;
	}

	return ps->weapon;
}

/*
===============
BG_ShutdownWeaponDefFiles
===============
*/
void BG_ShutdownWeaponDefFiles()
{
	bg_iNumWeapons = 0;
}

/*
===============
BG_GetHorizontalBobFactor
===============
*/
float BG_GetHorizontalBobFactor( const playerState_t *ps, float cycle, float speed, float maxAmp )
{
	float amplitude;

	assert(ps);

	if ( ps->viewHeightTarget == PRONE_VIEWHEIGHT )
	{
		amplitude = speed * bg_bobAmplitudeProne->current.decimal;
	}
	else
	{
		if ( ps->viewHeightTarget == CROUCH_VIEWHEIGHT )
			amplitude = speed * bg_bobAmplitudeDucked->current.decimal;
		else
			amplitude = speed * bg_bobAmplitudeStanding->current.decimal;
	}

	if ( amplitude > maxAmp )
	{
		amplitude = maxAmp;
	}

	return sin(cycle) * amplitude;
}

/*
===============
BG_GetVerticalBobFactor
===============
*/
float BG_GetVerticalBobFactor( const playerState_t *ps, float cycle, float speed, float maxAmp )
{
	float amplitude;

	assert(ps);

	if ( ps->viewHeightTarget == PRONE_VIEWHEIGHT )
	{
		amplitude = speed * bg_bobAmplitudeProne->current.decimal;
	}
	else
	{
		if ( ps->viewHeightTarget == CROUCH_VIEWHEIGHT )
			amplitude = speed * bg_bobAmplitudeDucked->current.decimal;
		else
			amplitude = speed * bg_bobAmplitudeStanding->current.decimal;
	}

	if ( amplitude > maxAmp )
	{
		amplitude = maxAmp;
	}

	return (0.75 * (sin(cycle * 4.0 + asin(1)) * 0.2 + sin(cycle + cycle))) * amplitude;
}

/*
===============
BG_WeaponFireRecoil
===============
*/
void BG_WeaponFireRecoil( playerState_t *ps, vec3_t vGunSpeed, vec3_t kickAVel )
{
	float fReducePercent;
	int weapIndex;
	float fPosLerp;
	float fYawKick;
	float fPitchKick;
	WeaponDef *weapDef;

	assert(ps);

	weapIndex = BG_GetViewmodelWeaponIndex(ps);
	weapDef = BG_GetWeaponDef(weapIndex);

	fPosLerp = ps->fWeaponPosFrac;
	fReducePercent = 1.0;

	if ( ps->weaponRestrictKickTime > 0 )
	{
		if ( fPosLerp == 1.0 )
		{
			fReducePercent = weapDef->adsGunKickReducedKickPercent * 0.0099999998;
		}
		else
		{
			fReducePercent = weapDef->hipGunKickReducedKickPercent * 0.0099999998;
		}
	}

	if ( fPosLerp == 1.0 )
	{
		fPitchKick = G_random() * (weapDef->adsViewKickPitchMax - weapDef->adsViewKickPitchMin) + weapDef->adsViewKickPitchMin;
		fYawKick = G_random() * (weapDef->adsViewKickYawMax - weapDef->adsViewKickYawMin) + weapDef->adsViewKickYawMin;
	}
	else
	{
		fPitchKick = G_random() * (weapDef->hipViewKickPitchMax - weapDef->hipViewKickPitchMin) + weapDef->hipViewKickPitchMin;
		fYawKick = G_random() * (weapDef->hipViewKickYawMax - weapDef->hipViewKickYawMin) + weapDef->hipViewKickYawMin;
	}

	kickAVel[0] = -(fPitchKick * fReducePercent);
	kickAVel[1] = fYawKick * fReducePercent;
	kickAVel[2] = kickAVel[1] * -0.5;

	if ( fPosLerp > 0 )
	{
		fPitchKick = G_random() * (weapDef->adsGunKickPitchMax - weapDef->adsGunKickPitchMin) + weapDef->adsGunKickPitchMin;
		fYawKick = G_random() * (weapDef->adsGunKickYawMax - weapDef->adsGunKickYawMin) + weapDef->adsGunKickYawMin;
	}
	else
	{
		fPitchKick = G_random() * (weapDef->hipGunKickPitchMax - weapDef->hipGunKickPitchMin) + weapDef->hipGunKickPitchMin;
		fYawKick = G_random() * (weapDef->hipGunKickYawMax - weapDef->hipGunKickYawMin) + weapDef->hipGunKickYawMin;
	}

	vGunSpeed[0] = vGunSpeed[0] + (fPitchKick * fReducePercent);
	vGunSpeed[1] = vGunSpeed[1] + (fYawKick * fReducePercent);
}

/*
===============
PM_WeaponAmmoAvailable
===============
*/
int PM_WeaponAmmoAvailable( playerState_t *ps )
{
	assert(ps);
	return ps->ammoclip[BG_ClipForWeapon(ps->weapon)];
}

/*
===============
BG_WeaponAmmo
===============
*/
int BG_WeaponAmmo( const playerState_t *ps, int weapon )
{
	assert(ps);
	return ps->ammo[BG_AmmoForWeapon(weapon)] + ps->ammoclip[BG_ClipForWeapon(weapon)];
}

/*
===============
PM_ExitAimDownSight
===============
*/
void PM_ExitAimDownSight( playerState_t *ps )
{
	assert(ps);
	PM_AddEvent(ps, EV_RESET_ADS);
	ps->pm_flags &= ~PMF_ADS;
}

/*
===============
BG_IsWeaponValid
===============
*/
bool BG_IsWeaponValid( const playerState_t *ps, int weaponIndex )
{
	WeaponDef *weapDef;

	assert(ps);

	if ( !BG_ValidateWeaponNumber(weaponIndex) )
	{
		return false;
	}

	if ( !Com_BitCheck(ps->weapons, weaponIndex) )
	{
		return false;
	}

	weapDef = BG_GetWeaponDef(weaponIndex);
	assert(weapDef);

	return weapDef->offhandClass
	       || weaponIndex == ps->weaponslots[SLOT_PRIMARY]
	       || weaponIndex == ps->weaponslots[SLOT_PRIMARYB]
	       || weapDef->altWeaponIndex == weaponIndex;
}

/*
===============
BG_GetFirstAvailableOffhand
===============
*/
int BG_GetFirstAvailableOffhand( const playerState_t *ps, int offhandSlot )
{
	int numWeps;
	int weapIndex;

	assert(ps);

	numWeps = BG_GetNumWeapons();

	for ( weapIndex = 1; weapIndex <= numWeps; weapIndex++ )
	{
		if ( BG_GetWeaponDef(weapIndex)->offhandClass != offhandSlot )
		{
			continue;
		}

		if ( !Com_BitCheck(ps->weapons, weapIndex) )
		{
			continue;
		}

		if ( !BG_WeaponAmmo(ps, weapIndex) )
		{
			continue;
		}

		return weapIndex;
	}

	return WP_NONE;
}

/*
===============
BG_FindWeaponIndexForName
===============
*/
int BG_FindWeaponIndexForName( const char *name )
{
	int curWeaponIndex;

	for ( curWeaponIndex = 1; curWeaponIndex <= bg_iNumWeapons; curWeaponIndex++ )
	{
		if ( !I_stricmp(name, bg_weaponDefs[curWeaponIndex]->szInternalName) )
		{
			return curWeaponIndex;
		}
	}

	return WP_NONE;
}

/*
===============
BG_GetAmmoClipForName
===============
*/
int BG_GetAmmoClipForName( const char *name )
{
	int curClipIndex;

	for ( curClipIndex = 0; curClipIndex < bg_iNumWeapClips; curClipIndex++ )
	{
		if ( !I_stricmp(bg_weapClips[curClipIndex]->clipName, name) )
		{
			return curClipIndex;
		}
	}

	Com_DPrintf("Couldn't find ammo clip \"%s\"\n", name);
	return 0;
}

/*
===============
BG_GetAmmoTypeForName
===============
*/
int BG_GetAmmoTypeForName( const char *name )
{
	int curAmmoIndex;

	for ( curAmmoIndex = 0; curAmmoIndex < bg_iNumAmmoTypes; curAmmoIndex++ )
	{
		if ( !I_stricmp(bg_weapAmmoTypes[curAmmoIndex]->ammoName, name) )
		{
			return curAmmoIndex;
		}
	}

	Com_DPrintf("Couldn't find ammo type \"%s\"\n", name);
	return 0;
}

/*
===============
PM_AdjustAimSpreadScale
===============
*/
void PM_AdjustAimSpreadScale( pmove_t *pm, pml_t *pml )
{
	playerState_t *ps;
	float wpnScale;
	float viewchange;
	float decrease;
	float increase;
	int i;
	WeaponDef *weapDef;

	ps = pm->ps;
	assert(ps);

	weapDef = BG_GetWeaponDef(pm->ps->weapon);
	wpnScale = weapDef->hipSpreadDecayRate;

	if ( wpnScale == 0 )
	{
		increase = 0;
		decrease = 1;
	}
	else
	{
		if ( ps->groundEntityNum == ENTITYNUM_NONE && ps->pm_type != PM_NORMAL_LINKED )
		{
			wpnScale *= 0.5;
		}
		else if ( ps->eFlags & EF_PRONE )
		{
			wpnScale *= weapDef->hipSpreadProneDecay;
		}
		else if ( ps->eFlags & EF_CROUCH )
		{
			wpnScale *= weapDef->hipSpreadDuckedDecay;
		}

		decrease = wpnScale * pml->frametime;

		if ( ps->fWeaponPosFrac == 1 )
		{
			increase = 0;
		}
		else
		{
			viewchange = 0;

			if ( weapDef->hipSpreadTurnAdd != 0 )
			{
				for ( i = 0; i < 2; i++ )
				{
					viewchange = I_fabs(AngleSubtract(SHORT2ANGLE( pm->cmd.angles[i] ), SHORT2ANGLE( pm->oldcmd.angles[i] )))
					             * 0.0099999998 * weapDef->hipSpreadTurnAdd / pml->frametime + viewchange;
				}
			}

			if ( weapDef->hipSpreadMoveAdd != 0 && (pm->cmd.forwardmove || pm->cmd.rightmove) )
			{
				if ( Vec2LengthSq(ps->velocity) > Square(bg_aimSpreadMoveSpeedThreshold->current.decimal) )
				{
					viewchange = viewchange + weapDef->hipSpreadMoveAdd;
				}
			}

			if ( ps->groundEntityNum == ENTITYNUM_NONE && ps->pm_type != PM_NORMAL_LINKED )
			{
				for ( i = 0; i < 2; i++ )
				{
					viewchange = viewchange + 1.28;
				}
			}

			increase = viewchange * pml->frametime;
		}
	}

	ps->aimSpreadScale = (increase - decrease) * 255 + ps->aimSpreadScale;

	if ( ps->aimSpreadScale < 0 )
	{
		ps->aimSpreadScale = 0;
	}
	else if ( ps->aimSpreadScale > 255 )
	{
		ps->aimSpreadScale = 255;
	}
}

/*
===============
PM_InteruptWeaponWithProneMove
===============
*/
qboolean PM_InteruptWeaponWithProneMove( playerState_t *ps )
{
	assert(ps);

	if ( ps->weaponstate < WEAPON_FIRING )
	{
		return qtrue;
	}

	if ( ps->weaponstate == WEAPON_RELOADING )
	{
		return qtrue;
	}

	if ( ps->weaponstate == WEAPON_RELOAD_START )
	{
		return qtrue;
	}

	if ( ps->weaponstate == WEAPON_RELOAD_END )
	{
		return qtrue;
	}

	if ( ps->weaponstate == WEAPON_RELOAD_START_INTERUPT )
	{
		return qtrue;
	}

	if ( ps->weaponstate == WEAPON_RELOADING_INTERUPT )
	{
		return qtrue;
	}

	if ( ps->weaponstate == WEAPON_RECHAMBERING )
	{
		return qtrue;
	}

	if ( ps->weaponstate == WEAPON_FIRING )
	{
		return qfalse;
	}

	if ( ps->weaponstate == WEAPON_MELEE_FIRE )
	{
		return qfalse;
	}

	if ( ps->weaponstate >= WEAPON_OFFHAND_INIT && ps->weaponstate <= WEAPON_OFFHAND_END )
	{
		return qfalse;
	}

	PM_Weapon_Idle(ps);
	return qtrue;
}

/*
===============
PM_UpdateAimDownSightLerp
===============
*/
void PM_UpdateAimDownSightLerp( pmove_t *pm, pml_t *pml )
{
	playerState_t *ps;
	bool adsRequested;
	int weapIndex;
	WeaponDef *weapDef;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	weapIndex = BG_GetViewmodelWeaponIndex(pm->ps);
	weapDef = BG_GetWeaponDef(weapIndex);

	if ( player_scopeExitOnDamage->current.boolean && ps->damageCount && weapDef->adsOverlayReticle )
	{
		PM_ExitAimDownSight(ps);
		ps->fWeaponPosFrac = 0;
		ps->adsDelayTime = 0;
		return;
	}

	if ( !weapDef->aimDownSight )
	{
		ps->fWeaponPosFrac = 0;
		ps->adsDelayTime = 0;
		return;
	}

	adsRequested = false;

	if ( !weapDef->segmentedReload
	        && ps->weaponstate == WEAPON_RELOADING
	        && ps->weaponTime - weapDef->adsReloadTransTime > 0
	        || weapDef->segmentedReload
	        && (ps->weaponstate == WEAPON_RELOADING
	            || ps->weaponstate == WEAPON_RELOADING_INTERUPT
	            || ps->weaponstate == WEAPON_RELOAD_START
	            || ps->weaponstate == WEAPON_RELOAD_START_INTERUPT
	            || ps->weaponstate == WEAPON_RELOAD_END && ps->weaponTime - weapDef->adsReloadTransTime > 0)
	        || !weapDef->rechamberWhileAds && ps->weaponstate == WEAPON_RECHAMBERING )
	{
		adsRequested = false;
	}
	else if ( ps->pm_flags & PMF_ADS )
	{
		adsRequested = true;
	}

	if ( weapDef->adsFire && ps->weaponDelay && ps->weaponstate == WEAPON_FIRING )
	{
		adsRequested = true;
	}

	if ( ps->fWeaponPosFrac == 1 && !adsRequested && player_adsExitDelay->current.integer > 0 )
	{
		if ( !ps->adsDelayTime )
		{
			ps->adsDelayTime = pm->cmd.serverTime + player_adsExitDelay->current.integer;
		}

		if ( ps->adsDelayTime > pm->cmd.serverTime )
			adsRequested = true;
		else
			ps->adsDelayTime = 0;
	}
	else
	{
		ps->adsDelayTime = 0;
	}

	if ( adsRequested && ps->fWeaponPosFrac != 1 || !adsRequested && ps->fWeaponPosFrac != 0 )
	{
		if ( adsRequested )
			ps->fWeaponPosFrac += pml->msec * weapDef->OOPosAnimLength[0];
		else
			ps->fWeaponPosFrac -= pml->msec * weapDef->OOPosAnimLength[1];

		ps->fWeaponPosFrac = I_fclamp(ps->fWeaponPosFrac, 0, 1);
	}
}

/*
===============
PM_UpdateAimDownSightFlag
===============
*/
void PM_UpdateAimDownSightFlag( pmove_t *pm, pml_t *pml )
{
	bool adsRequested;
	bool adsAllowed;
	playerState_t *ps;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	ps->pm_flags &= ~PMF_ADS;

	adsAllowed = PM_IsAdsAllowed(ps, pml);
	adsRequested = false;

	if ( pm->cmd.buttons & BUTTON_ADS || PM_IsBinocularsADS(ps) )
	{
		adsRequested = true;
	}

	if ( adsRequested && adsAllowed )
	{
		if ( ps->pm_flags & PMF_PRONE )
		{
			if ( !(pm->oldcmd.buttons & BUTTON_ADS) || !pm->cmd.forwardmove && !pm->cmd.rightmove )
			{
				ps->pm_flags |= PMF_ADS;
				ps->pm_flags |= PMF_ADS_OVERRIDE;
			}
		}
		else
		{
			ps->pm_flags |= PMF_ADS;
		}
	}

	if ( ps->pm_flags & PMF_ADS )
		BG_UpdateConditionValue(ps->clientNum, ANIM_COND_WEAPON_POSITION, qtrue, qtrue);
	else
		BG_UpdateConditionValue(ps->clientNum, ANIM_COND_WEAPON_POSITION, qfalse, qtrue);
}

/*
===============
BG_CalculateWeaponPosition_Sway
===============
*/
void BG_CalculateWeaponPosition_Sway( const playerState_t *ps, vec3_t swayViewAngles, vec3_t swayOffset, vec3_t swayAngles, float ssSwayScale, int frametime )
{
	int weapon;
	float dt;
	float fWeaponPosFrac;
	float swayVertScale;
	float swayHorizScale;
	float swayYawScale;
	float swayPitchScale;
	float swayLerpSpeed;
	float swayMaxAngle;
	vec3_t deltaAngles;
	WeaponDef *weapDef;

	assert(ps);
	fWeaponPosFrac = ps->fWeaponPosFrac;

	if ( !frametime )
	{
		return;
	}

	weapon = BG_GetViewmodelWeaponIndex(ps);
	weapDef = BG_GetWeaponDef(weapon);

	dt = frametime * 0.001;

	if ( !BG_IsAimDownSightWeapon(weapon) )
	{
		swayMaxAngle = weapDef->swayMaxAngle;
		swayLerpSpeed = weapDef->swayLerpSpeed;
		swayPitchScale = weapDef->swayPitchScale;
		swayYawScale = weapDef->swayYawScale;
		swayHorizScale = weapDef->swayHorizScale;
		swayVertScale = weapDef->swayVertScale;

		swayPitchScale *= ssSwayScale;
		swayYawScale *= ssSwayScale;
		swayHorizScale *= ssSwayScale;
		swayVertScale *= ssSwayScale;

		AnglesSubtract(ps->viewangles, swayViewAngles, deltaAngles);

		assert(dt);
		VectorScale(deltaAngles, 1.0 / (dt * 60.0), deltaAngles);

		deltaAngles[0] = I_fclamp(deltaAngles[0], -swayMaxAngle, swayMaxAngle);
		deltaAngles[1] = I_fclamp(deltaAngles[1], -swayMaxAngle, swayMaxAngle);

		swayOffset[1] = DiffTrack(deltaAngles[1] * swayHorizScale, swayOffset[1], swayLerpSpeed, dt);
		swayOffset[2] = DiffTrack(deltaAngles[0] * swayVertScale, swayOffset[2], swayLerpSpeed, dt);

		deltaAngles[0] = deltaAngles[0] * swayPitchScale;
		deltaAngles[1] = deltaAngles[1] * swayYawScale;

		swayAngles[0] = DiffTrackAngle(deltaAngles[0], swayAngles[0], swayLerpSpeed, dt);
		swayAngles[1] = DiffTrackAngle(deltaAngles[1], swayAngles[1], swayLerpSpeed, dt);

		VectorCopy(ps->viewangles, swayViewAngles);
		return;
	}

	if ( fWeaponPosFrac > 0 && weapDef->adsOverlayReticle )
	{
		return;
	}

	swayMaxAngle = (weapDef->adsSwayMaxAngle - weapDef->swayMaxAngle) * fWeaponPosFrac + weapDef->swayMaxAngle;
	swayLerpSpeed = (weapDef->adsSwayLerpSpeed - weapDef->swayLerpSpeed) * fWeaponPosFrac + weapDef->swayLerpSpeed;
	swayPitchScale = (weapDef->adsSwayPitchScale - weapDef->swayPitchScale) * fWeaponPosFrac + weapDef->swayPitchScale;
	swayYawScale = (weapDef->adsSwayYawScale - weapDef->swayYawScale) * fWeaponPosFrac + weapDef->swayYawScale;
	swayHorizScale = (weapDef->adsSwayHorizScale - weapDef->swayHorizScale) * fWeaponPosFrac + weapDef->swayHorizScale;
	swayVertScale = (weapDef->adsSwayVertScale - weapDef->swayVertScale) * fWeaponPosFrac + weapDef->swayVertScale;

	swayPitchScale *= ssSwayScale;
	swayYawScale *= ssSwayScale;
	swayHorizScale *= ssSwayScale;
	swayVertScale *= ssSwayScale;

	AnglesSubtract(ps->viewangles, swayViewAngles, deltaAngles);

	assert(dt);
	VectorScale(deltaAngles, 1.0 / (dt * 60.0), deltaAngles);

	deltaAngles[0] = I_fclamp(deltaAngles[0], -swayMaxAngle, swayMaxAngle);
	deltaAngles[1] = I_fclamp(deltaAngles[1], -swayMaxAngle, swayMaxAngle);

	swayOffset[1] = DiffTrack(deltaAngles[1] * swayHorizScale, swayOffset[1], swayLerpSpeed, dt);
	swayOffset[2] = DiffTrack(deltaAngles[0] * swayVertScale, swayOffset[2], swayLerpSpeed, dt);

	deltaAngles[0] = deltaAngles[0] * swayPitchScale;
	deltaAngles[1] = deltaAngles[1] * swayYawScale;

	swayAngles[0] = DiffTrackAngle(deltaAngles[0], swayAngles[0], swayLerpSpeed, dt);
	swayAngles[1] = DiffTrackAngle(deltaAngles[1], swayAngles[1], swayLerpSpeed, dt);

	VectorCopy(ps->viewangles, swayViewAngles);
}

/*
===============
BG_CalculateViewAngles
===============
*/
void BG_CalculateViewAngles(viewState_t *vs, vec3_t angles)
{
	VectorClear(angles);

	BG_CalculateView_DamageKick(vs, angles);
	BG_CalculateView_IdleAngles(vs, angles);
	BG_CalculateView_BobAngles(vs, angles);
	BG_CalculateView_Velocity(vs, angles);
}

/*
===============
PM_ResetWeaponState
===============
*/
void PM_ResetWeaponState( playerState_t *ps )
{
	assert(ps);
	PM_Weapon_Idle(ps);
}

/*
===============
BG_CalculateWeaponAngles
===============
*/
void BG_CalculateWeaponAngles( weaponState_t *ws, vec3_t angles )
{
	playerState_t *ps;

	ps = ws->ps;
	assert(ps);

	VectorClear(angles);

	if ( ps->leanf != 0 )
	{
		angles[2] = angles[2] - (GetLeanFraction(ps->leanf) + GetLeanFraction(ps->leanf));
	}

	BG_CalculateWeaponPosition_BaseAngles(ws, angles);
	BG_CalculateWeaponPosition_IdleAngles(ws, angles);
	BG_CalculateWeaponPosition_BobOffset(ws, angles);
	BG_CalculateWeaponPosition_DamageKick(ws, angles);
	BG_CalculateWeaponPosition_GunRecoil(ws, angles);

	angles[0] = AngleSubtract(angles[0], ws->swayAngles[0]);
	angles[1] = AngleSubtract(angles[1], ws->swayAngles[1]);
}

/*
===============
BG_LoadWeaponDef
===============
*/
WeaponDef *BG_LoadWeaponDef( const char *folder, const char *name )
{
	WeaponDef *weapDef;

	if ( !name[0] )
	{
		return NULL;
	}

	weapDef = BG_LoadWeaponDefInternal(folder, name);

	if ( weapDef )
	{
		return weapDef;
	}

	weapDef = BG_LoadWeaponDefInternal(folder, "defaultweapon_mp");

	if ( !weapDef )
	{
		Com_Error(ERR_DROP, "BG_LoadWeaponDef: Could not find default weapon");
	}

	SetConfigString((char **)&weapDef->szInternalName, name);
	return weapDef;
}

/*
===============
PM_Weapon
===============
*/
void PM_Weapon( pmove_t *pm, pml_t *pml )
{
	playerState_t *ps;
	qboolean delayedAction;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	if ( pm->ps->pm_flags & PMF_RESPAWNED )
	{
		return;
	}

	if ( ps->pm_type >= PM_DEAD )
	{
		ps->weapon = WP_NONE;
		return;
	}

	if ( ps->eFlags & EF_TURRET_ACTIVE )
	{
		return;
	}

	PM_UpdateAimDownSightLerp(pm, pml);
	PM_UpdateHoldBreath(pm, pml);

	if ( PM_UpdateGrenadeThrow(ps, pml) )
	{
		return;
	}

	delayedAction = PM_Weapon_WeaponTimeAdjust(pm, pml);

	PM_Weapon_CheckForBinoculars(pm);
	PM_Weapon_CheckForOffHand(pm);
	PM_Weapon_CheckForChangeWeapon(pm);
	PM_Weapon_CheckForReload(pm);
	PM_Weapon_CheckForMelee(pm, delayedAction);

	if ( PM_Weapon_CheckForRechamber(ps, delayedAction) )
	{
		return;
	}

	if ( ps->pm_flags & PMF_PRONE && (pm->cmd.forwardmove || pm->cmd.rightmove)
	        || ps->weaponstate == WEAPON_MELEE_INIT
	        || ps->weaponstate == WEAPON_MELEE_FIRE )
	{
		ps->aimSpreadScale = 255;
	}

	assert((ps->weaponTime >= 0) && (ps->weaponDelay >= 0));

	if ( !delayedAction && ps->weaponTime || ps->weaponDelay )
	{
		return;
	}

	switch ( ps->weaponstate )
	{
	case WEAPON_RAISING:
		PM_Weapon_FinishWeaponRaise(ps);
		return;

	case WEAPON_DROPPING:
		PM_Weapon_FinishWeaponChange(pm);
		return;

	case WEAPON_RELOADING:
	case WEAPON_RELOADING_INTERUPT:
		PM_Weapon_FinishReload(ps, delayedAction);
		return;

	case WEAPON_RELOAD_START:
	case WEAPON_RELOAD_START_INTERUPT:
		PM_Weapon_FinishReloadStart(ps, delayedAction);
		return;

	case WEAPON_RELOAD_END:
		PM_Weapon_FinishReloadEnd(ps);
		return;

	case WEAPON_MELEE_INIT:
		PM_Weapon_FireMelee(ps);
		return;

	case WEAPON_MELEE_FIRE:
		PM_Weapon_FinishMelee(ps);
		return;

	case WEAPON_OFFHAND_INIT:
		PM_Weapon_OffHandPrepare(ps);
		return;

	case WEAPON_OFFHAND_PREPARE:
		PM_Weapon_OffHandHold(ps);
		return;

	case WEAPON_OFFHAND_HOLD:
		PM_Weapon_OffHand(pm);
		return;

	case WEAPON_OFFHAND:
		PM_Weapon_OffHandEnd(ps);
		return;

	case WEAPON_OFFHAND_END:
	case WEAPON_BINOCULARS_END:
		PM_Weapon_Idle(ps);
		return;

	case WEAPON_BINOCULARS_INIT:
		Binocular_State_Raise(ps);
		return;

	case WEAPON_BINOCULARS_RAISE:
		Binocular_State_Hold(ps);
		return;

	case WEAPON_BINOCULARS_HOLD:
		Binocular_State_Exit_ADS(ps);
		return;

	case WEAPON_BINOCULARS_EXIT_ADS:
		return;

	case WEAPON_BINOCULARS_DROP:
		Binocular_State_End(pm);
		return;
	}

	if ( !ps->weapon )
	{
		return;
	}

	if ( PM_Weapon_FinishFiring(pm, delayedAction) )
	{
		return;
	}

	PM_Weapon_FireWeapon(ps, delayedAction);
	assert((ps->weaponTime >= 0) && (ps->weaponDelay >= 0));
}

/*
===============
BG_FillInAllWeaponItems
===============
*/
void BG_FillInAllWeaponItems()
{
	int weaponCount;
	int weaponIndex;

	weaponCount = BG_GetNumWeapons();

	for ( weaponIndex = 1; weaponIndex <= weaponCount; weaponIndex++ )
	{
		BG_SetupWeaponIndex(weaponIndex);
	}
}

/*
===============
BG_GetWeaponIndexForName
===============
*/
int BG_GetWeaponIndexForName( const char *name, int (*regWeap)(int) )
{
	WeaponDef *weapDef;
	int weapIndex;

	if ( !name[0] || !strcasecmp(name, "none") )
	{
		return WP_NONE;
	}

	weapIndex = BG_FindWeaponIndexForName(name);

	if ( weapIndex )
	{
		return weapIndex;
	}

	weapDef = BG_LoadWeaponDef("mp", name);

	if ( weapDef )
	{
		return BG_SetupWeaponDef(weapDef, regWeap);
	}

	Com_DPrintf("Couldn't find weapon \"%s\"\n", name);
	return WP_NONE;
}

/*
===============
CG_GetWeaponIndexForName
===============
*/
int CG_GetWeaponIndexForName( const char *name )
{
	return BG_GetWeaponIndexForName(name, WP_NONE);
}

/*
===============
CG_SetWeaponDefToDefaultWeapon
===============
*/
void CG_SetWeaponDefToDefaultWeapon( int iWeapon )
{
	int weapIndex;
	int animIter;

	weapIndex = CG_GetWeaponIndexForName("defaultweapon_mp");

	if ( !weapIndex )
	{
		Com_Error(ERR_DROP, "CG_SetWeaponDefToDefaultWeapon: weapon def for default weapon could not be found!");
	}

	bg_weaponDefs[iWeapon]->gunModel = bg_weaponDefs[weapIndex]->gunModel;
	bg_weaponDefs[iWeapon]->handModel = bg_weaponDefs[weapIndex]->handModel;

	for ( animIter = 0; animIter < ARRAY_COUNT(bg_weaponDefs[iWeapon]->XAnims); animIter++ )
	{
		bg_weaponDefs[iWeapon]->XAnims[animIter] = bg_weaponDefs[weapIndex]->XAnims[animIter];
	}
}

/*
===============
BG_FillInAmmoItems
===============
*/
void BG_FillInAmmoItems( int (*regWeap)(int) )
{
	char name[MAX_QPATH];
	char *c;
	gitem_t *item;
	WeaponDef *weapDef;
	int weapIndex;
	int itemIdx;

	for ( itemIdx = bg_numWeaponItems; itemIdx < bg_numItems; itemIdx++ )
	{
		item = &bg_itemlist[itemIdx];

		if ( item->giType != IT_AMMO )
		{
			continue;
		}

		I_strncpyz(name, item->pickup_name, sizeof(name));

		for ( c = name; *c; c++ )
		{
			if ( *c == ' ' )
			{
				*c = 0;
				break;
			}
		}

		weapIndex = BG_GetWeaponIndexForName(name, regWeap);

		if ( weapIndex )
		{
			weapDef = BG_GetWeaponDef(weapIndex);

			item->giTag = weapIndex;
			item->giAmmoIndex = weapDef->ammoIndex;
			item->giClipIndex = weapDef->clipIndex;
		}
		else
		{
			Com_Printf("^3WARNING^7: Could not find weapon for ammo item %s\n", item->pickup_name);
			weapIndex = BG_GetWeaponIndexForName("defaultweapon_mp", regWeap);

			if ( !weapIndex )
			{
				Com_Error(ERR_DROP, "BG_FillInAmmoItems: weapon def for default weapon could not be found!");
			}

			weapDef = BG_GetWeaponDef(weapIndex);

			item->giTag = WP_DEFAULTWEAPON;
			item->giAmmoIndex = weapDef->ammoIndex;
			item->giClipIndex = weapDef->clipIndex;
		}
	}
}

/*
===============
BG_ClearWeaponDef
===============
*/
void BG_ClearWeaponDef()
{
	int itemIdx;

	bg_weaponDefs[0] = BG_LoadDefaultWeaponDef();

	bg_weapAmmoTypes[0] = bg_weaponDefs[0];
	bg_iNumAmmoTypes = 1;

	bg_sharedAmmoCaps[0] = bg_weaponDefs[0];
	bg_iNumSharedAmmoCaps = 1;

	bg_weapClips[0] = bg_weaponDefs[0];
	bg_iNumWeapClips = 1;

	for ( itemIdx = 1; itemIdx < bg_numWeaponItems; itemIdx++ )
	{
		bg_itemlist[itemIdx].giType = IT_BAD;
	}

	BG_LoadPlayerAnimTypes();
	BG_InitWeaponStrings();
}

/*
===============
Binocular_State_Hold
===============
*/
void Binocular_State_Hold( playerState_t *ps )
{
	int weapIndex;
	WeaponDef *weapDef;

	assert(ps);

	weapIndex = BG_GetBinocularWeaponIndex();
	weapDef = BG_GetWeaponDef(weapIndex);

	ps->weaponstate = WEAPON_BINOCULARS_HOLD;
	ps->weaponTime = weapDef->adsTransInTime;
	ps->weaponDelay = 0;

	PM_EnterBinoculars(ps);
	PM_StartWeaponAnim(ps, WEAP_IDLE);
}

/*
===============
PM_EndHoldBreath
===============
*/
void PM_EndHoldBreath( playerState_t *ps )
{
	assert(ps);
	ps->pm_flags &= ~PMF_BREATH;
}

/*
===============
PM_StartHoldBreath
===============
*/
void PM_StartHoldBreath( playerState_t *ps )
{
	assert(ps);
	ps->pm_flags |= PMF_BREATH;
}

/*
===============
PM_IsBinocularADSAllowed
===============
*/
bool PM_IsBinocularADSAllowed( int iWeaponState )
{
	return iWeaponState == WEAPON_BINOCULARS_HOLD || iWeaponState == WEAPON_BINOCULARS_EXIT_ADS;
}

/*
===============
BG_GetBinocularWeaponIndex
===============
*/
int BG_GetBinocularWeaponIndex()
{
	int curWeaponIndex;

	for ( curWeaponIndex = 1; curWeaponIndex <= bg_iNumWeapons; curWeaponIndex++ )
	{
		if ( bg_weaponDefs[curWeaponIndex]->weaponType == WEAPTYPE_BINOCULARS )
		{
			return curWeaponIndex;
		}
	}

	return WP_NONE;
}

/*
===============
BG_FillInWeaponItems
===============
*/
void BG_FillInWeaponItems( int weapIndex )
{
	WeaponDef *weapDef;
	gitem_t *item;

	weapDef = BG_GetWeaponDef(weapIndex);
	item = &bg_itemlist[weapIndex];

	item->pickup_sound = weapDef->pickupSound;

	item->world_model[0] = weapDef->worldModel;
	item->world_model[1] = NULL;

	item->icon = weapDef->hudIcon;
	item->pickup_name = weapDef->displayName;
	item->quantity = weapDef->startAmmo;

	item->giType = IT_WEAPON;
	item->giTag = weapIndex;

	item->giAmmoIndex = weapDef->ammoIndex;
	item->giClipIndex = weapDef->clipIndex;
}

/*
===============
Binocular_State_Init
===============
*/
void Binocular_State_Init( playerState_t *ps )
{
	WeaponDef *weapDef;

	assert(ps);
	weapDef = BG_GetWeaponDef(ps->weapon);

	PM_ExitAimDownSight(ps);

	ps->weaponTime = 0;
	ps->weaponDelay = weapDef->adsTransOutTime + player_adsExitDelay->current.integer;
	ps->weaponstate = WEAPON_BINOCULARS_INIT;
}

/*
===============
PM_Weapon_OffHandEnd
===============
*/
void PM_Weapon_OffHandEnd( playerState_t *ps )
{
	assert(ps);

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
	ps->pm_flags &= ~(PMF_FRAG | PMF_ADS_OVERRIDE);
}

/*
===============
PM_Weapon_OffHandHold
===============
*/
void PM_Weapon_OffHandHold( playerState_t *ps )
{
	WeaponDef *weapDef;

	assert(ps);
	assert(ps->offHandIndex != WP_NONE);

	weapDef = BG_GetWeaponDef(ps->offHandIndex);

	ps->weaponstate = WEAPON_OFFHAND_HOLD;
	ps->weaponTime = 0;
	ps->weaponDelay = 0;
	ps->pm_flags |= PMF_FRAG;
	ps->grenadeTimeLeft = weapDef->fuseTime;
}

/*
===============
PM_Weapon_OffHandPrepare
===============
*/
void PM_Weapon_OffHandPrepare( playerState_t *ps )
{
	WeaponDef *weapDef;

	assert(ps);
	assert(ps->offHandIndex != WP_NONE);

	weapDef = BG_GetWeaponDef(ps->offHandIndex);

	ps->weaponstate = WEAPON_OFFHAND_PREPARE;
	ps->weaponTime = weapDef->holdFireTime;
	ps->weaponDelay = 0;
	ps->pm_flags |= PMF_FRAG;

	BG_AddPredictableEventToPlayerstate(EV_PREP_OFFHAND, ps->offHandIndex, ps);
	PM_StartWeaponAnim(ps, WEAP_HOLD_FIRE);

	PM_SetProneMovementOverride(ps);
}

/*
===============
PM_Weapon_AddFiringAimSpreadScale
===============
*/
void PM_Weapon_AddFiringAimSpreadScale( playerState_t *ps )
{
	WeaponDef *weapDef;

	assert(ps);
	weapDef = BG_GetWeaponDef(ps->weapon);

	if ( ps->fWeaponPosFrac == 1.0 )
	{
		return;
	}

	ps->aimSpreadScale = weapDef->hipSpreadFireAdd * 255 + ps->aimSpreadScale;

	if ( ps->aimSpreadScale > 255 )
	{
		ps->aimSpreadScale = 255;
	}
}

/*
===============
PM_Weapon_FinishReloadEnd
===============
*/
void PM_Weapon_FinishReloadEnd( playerState_t *ps )
{
	assert(ps);
	assert(ps->weaponstate == WEAPON_RELOAD_END);

	ps->weaponstate = WEAPON_READY;
	PM_StartWeaponAnim(ps, WEAP_IDLE);
}

/*
===============
PM_Weapon_FinishWeaponRaise
===============
*/
void PM_Weapon_FinishWeaponRaise( playerState_t *ps )
{
	assert(ps);
	assert(ps->weaponstate == WEAPON_RAISING);

	ps->weaponstate = WEAPON_READY;
	PM_StartWeaponAnim(ps, WEAP_IDLE);
}

/*
===============
PM_ContinueWeaponAnim
===============
*/
void PM_ContinueWeaponAnim( playerState_t *ps, int anim )
{
	assert(ps);

	if ( (ps->weapAnim & ~ANIM_TOGGLEBIT) != anim )
	{
		PM_StartWeaponAnim(ps, anim);
	}
}

/*
===============
BG_CalculateView_IdleAngles
===============
*/
void BG_CalculateView_IdleAngles( viewState_t *vs, vec3_t angles )
{
	float idleSpeed;
	int weapIndex;
	playerState_t *ps;
	float fTargFactor;
	float fTargScale;
	WeaponDef *weapDef;

	ps = vs->ps;
	assert(ps);

	weapIndex = BG_GetViewmodelWeaponIndex(vs->ps);
	weapDef = BG_GetWeaponDef(weapIndex);

	if ( !weapDef->adsOverlayReticle )
	{
		return;
	}

	if ( BG_IsAimDownSightWeapon(weapIndex) )
	{
		fTargScale = (weapDef->adsIdleAmount - weapDef->hipIdleAmount) * ps->fWeaponPosFrac + weapDef->hipIdleAmount;
		idleSpeed =  (weapDef->adsIdleSpeed  - weapDef->hipIdleSpeed)  * ps->fWeaponPosFrac + weapDef->hipIdleSpeed;
	}
	else if ( weapDef->hipIdleAmount == 0 )
	{
		fTargScale = 80;
		idleSpeed = 1.0;
	}
	else
	{
		fTargScale = weapDef->hipIdleAmount;
		idleSpeed = weapDef->hipIdleSpeed;
	}

	if ( ps->eFlags & EF_PRONE )
	{
		fTargFactor = weapDef->idleProneFactor;
	}
	else if ( ps->eFlags & EF_CROUCH )
	{
		fTargFactor = weapDef->idleCrouchFactor;
	}
	else
	{
		fTargFactor = 1.0;
	}

	if ( weapDef->adsOverlayReticle && ps->fWeaponPosFrac != 0 && fTargFactor != vs->fLastIdleFactor )
	{
		if ( fTargFactor > vs->fLastIdleFactor )
		{
			vs->fLastIdleFactor += vs->frametime * 0.5;

			if ( vs->fLastIdleFactor > fTargFactor )
			{
				vs->fLastIdleFactor = fTargFactor;
			}
		}
		else
		{
			vs->fLastIdleFactor -= vs->frametime * 0.5;

			if ( fTargFactor > vs->fLastIdleFactor )
			{
				vs->fLastIdleFactor = fTargFactor;
			}
		}
	}

	fTargScale *= vs->fLastIdleFactor;
	fTargScale *= ps->fWeaponPosFrac;
	fTargScale *= ps->holdBreathScale;

	*vs->weapIdleTime += ps->holdBreathScale * vs->frametime * 1000 * idleSpeed;

	// Requires conversion to long double for higher precision
	angles[1] += fTargScale * sin((long double)*vs->weapIdleTime * 0.00069999998) * 0.0099999998;
	angles[0] += fTargScale * sin((long double)*vs->weapIdleTime * 0.001)         * 0.0099999998;
}

/*
===============
BG_CalculateWeaponPosition_IdleAngles
===============
*/
void BG_CalculateWeaponPosition_IdleAngles( weaponState_t *ws, vec3_t angles )
{
	float idleSpeed;
	int weapIndex;
	playerState_t *ps;
	float fTargFactor;
	float fTargScale;
	WeaponDef *weapDef;

	ps = ws->ps;
	assert(ps);

	weapIndex = BG_GetViewmodelWeaponIndex(ws->ps);
	weapDef = BG_GetWeaponDef(weapIndex);

	if ( BG_IsAimDownSightWeapon(weapIndex) )
	{
		fTargScale = (weapDef->adsIdleAmount - weapDef->hipIdleAmount) * ps->fWeaponPosFrac + weapDef->hipIdleAmount;
		idleSpeed =  (weapDef->adsIdleSpeed  - weapDef->hipIdleSpeed)  * ps->fWeaponPosFrac + weapDef->hipIdleSpeed;
	}
	else if ( weapDef->hipIdleAmount == 0 )
	{
		fTargScale = 80;
		idleSpeed = 1.0;
	}
	else
	{
		fTargScale = weapDef->hipIdleAmount;
		idleSpeed = weapDef->hipIdleSpeed;
	}

	if ( ps->eFlags & EF_PRONE )
	{
		fTargFactor = weapDef->idleProneFactor;
	}
	else if ( ps->eFlags & EF_CROUCH )
	{
		fTargFactor = weapDef->idleCrouchFactor;
	}
	else
	{
		fTargFactor = 1.0;
	}

	if ( (!weapDef->adsOverlayReticle || ps->fWeaponPosFrac == 0) && fTargFactor != ws->fLastIdleFactor )
	{
		if ( fTargFactor > ws->fLastIdleFactor )
		{
			ws->fLastIdleFactor += ws->frametime * 0.5;

			if ( ws->fLastIdleFactor > fTargFactor )
			{
				ws->fLastIdleFactor = fTargFactor;
			}
		}
		else
		{
			ws->fLastIdleFactor -= ws->frametime * 0.5;

			if ( fTargFactor > ws->fLastIdleFactor )
			{
				ws->fLastIdleFactor = fTargFactor;
			}
		}
	}

	fTargScale *= ws->fLastIdleFactor;

	if ( weapDef->adsOverlayReticle )
	{
		fTargScale *= 1.0 - ps->fWeaponPosFrac;
	}

	*ws->weapIdleTime += ws->frametime * 1000 * idleSpeed;

	// Requires conversion to long double for higher precision
	angles[2] += fTargScale * sin((long double)*ws->weapIdleTime * 0.00050000002) * 0.0099999998;
	angles[1] += fTargScale * sin((long double)*ws->weapIdleTime * 0.00069999998) * 0.0099999998;
	angles[0] += fTargScale * sin((long double)*ws->weapIdleTime * 0.001)         * 0.0099999998;
}

/*
===============
BG_CalculateWeaponPosition_BasePosition_angles
===============
*/
void BG_CalculateWeaponPosition_BasePosition_angles( weaponState_t *ws, vec3_t angles )
{
	int weapIndex;
	WeaponDef *weapDef;
	playerState_t *ps;
	vec3_t targetAngles;
	float minSpeed;
	float speedFrac;
	int i;

	ps = ws->ps;
	assert(ps);

	weapIndex = BG_GetViewmodelWeaponIndex(ws->ps);
	weapDef = BG_GetWeaponDef(weapIndex);

	if ( ps->eFlags & EF_PRONE )
	{
		minSpeed = weapDef->proneRotMinSpeed;
	}
	else if ( ps->eFlags & EF_CROUCH )
	{
		minSpeed = weapDef->duckedRotMinSpeed;
	}
	else
	{
		minSpeed = weapDef->standRotMinSpeed;
	}

	assert(minSpeed >= 0.f);

	if ( ws->xyspeed > minSpeed && ps->weaponstate != WEAPON_RELOADING )
	{
		speedFrac = (ws->xyspeed - minSpeed) / (ps->speed - minSpeed);
		speedFrac = I_fmin(1.0f, I_fmax(0.f, speedFrac));

		if ( ps->eFlags & EF_PRONE )
		{
			VectorScale(&weapDef->proneRotP, speedFrac, targetAngles);
		}
		else if ( ps->eFlags & EF_CROUCH )
		{
			VectorScale(&weapDef->duckedRotP, speedFrac, targetAngles);
		}
		else
		{
			VectorScale(&weapDef->standRotP, speedFrac, targetAngles);
		}
	}
	else
	{
		VectorClear(targetAngles);
	}

	if ( ps->fWeaponPosFrac != 0 )
	{
		speedFrac = 1.0 - ps->fWeaponPosFrac;
		VectorScale(targetAngles, speedFrac, targetAngles);
	}

	for ( i = 0; i < 3; ++i )
	{
		if ( ws->vLastMoveAng[i] == targetAngles[i] )
		{
			continue;
		}

		if ( ps->viewHeightCurrent == PRONE_VIEWHEIGHT )
		{
			speedFrac = (targetAngles[i] - ws->vLastMoveAng[i]) * ws->frametime * weapDef->posProneRotRate;
		}
		else
		{
			speedFrac = (targetAngles[i] - ws->vLastMoveAng[i]) * ws->frametime * weapDef->posRotRate;
		}

		if ( targetAngles[i] > ws->vLastMoveAng[i] )
		{
			if ( ws->frametime * 0.1 > speedFrac )
			{
				speedFrac = ws->frametime * 0.1;
			}

			ws->vLastMoveAng[i] = ws->vLastMoveAng[i] + speedFrac;

			if ( ws->vLastMoveAng[i] > targetAngles[i] )
			{
				ws->vLastMoveAng[i] = targetAngles[i];
			}
		}
		else
		{
			if ( speedFrac > ws->frametime * -0.1 )
			{
				speedFrac = ws->frametime * -0.1;
			}

			ws->vLastMoveAng[i] = ws->vLastMoveAng[i] + speedFrac;

			if ( targetAngles[i] > ws->vLastMoveAng[i] )
			{
				ws->vLastMoveAng[i] = targetAngles[i];
			}
		}
	}

	if ( ps->fWeaponPosFrac == 0 )
	{
		VectorAdd(angles, ws->vLastMoveAng, angles);
	}
	else if ( ps->fWeaponPosFrac < 0.5 )
	{
		speedFrac = 1.0 - (ps->fWeaponPosFrac + ps->fWeaponPosFrac);
		VectorMA(angles, speedFrac, ws->vLastMoveAng, angles);
	}
}

/*
===============
Binocular_State_Drop
===============
*/
void Binocular_State_Drop( playerState_t *ps )
{
	int weapIndex;
	WeaponDef *weapDef;

	assert(ps);

	weapIndex = BG_GetBinocularWeaponIndex();
	weapDef = BG_GetWeaponDef(weapIndex);

	ps->weaponstate = WEAPON_BINOCULARS_DROP;
	ps->weaponTime = weapDef->adsTransOutTime + player_adsExitDelay->current.integer;
	ps->weaponDelay = 0;

	PM_AddEvent(ps, EV_BINOCULAR_DROP);
	PM_StartWeaponAnim(ps, WEAP_IDLE);
}

/*
===============
PM_ExitBinoculars
===============
*/
void PM_ExitBinoculars( playerState_t *ps )
{
	assert(ps);
	ps->pm_flags &= ~PMF_BINOCULARS;
	PM_AddEvent(ps, EV_BINOCULAR_EXIT);
}

/*
===============
PM_EnterBinoculars
===============
*/
void PM_EnterBinoculars( playerState_t *ps )
{
	assert(ps);
	ps->pm_flags |= PMF_BINOCULARS;
	PM_AddEvent(ps, EV_BINOCULAR_ENTER);
}

/*
===============
PM_SendEmtpyOffhandEvent
===============
*/
void PM_SendEmtpyOffhandEvent( playerState_t *ps )
{
	assert(ps);
	PM_AddEvent(ps, EV_EMPTY_OFFHAND);
}

/*
===============
PM_Weapon_FinishMelee
===============
*/
void PM_Weapon_FinishMelee( playerState_t *ps )
{
	assert(ps);
	PM_ContinueWeaponAnim(ps, WEAP_IDLE);
	ps->weaponstate = WEAPON_READY;
}

/*
===============
PM_Weapon_FireMelee
===============
*/
void PM_Weapon_FireMelee( playerState_t *ps )
{
	WeaponDef *weapDef;

	assert(ps);
	weapDef = BG_GetWeaponDef(ps->weapon);

	if ( ps->weaponTime < weapDef->meleeTime - weapDef->meleeDelay )
	{
		ps->weaponTime = weapDef->meleeTime - weapDef->meleeDelay;
	}

	PM_AddEvent(ps, EV_FIRE_MELEE);
	ps->weaponstate = WEAPON_MELEE_FIRE;

	PM_SetProneMovementOverride(ps);
}

/*
===============
PM_Weapon_FinishFiring
===============
*/
qboolean PM_Weapon_FinishFiring( pmove_t *pm, qboolean delayedAction )
{
	playerState_t *ps;

	assert(pm);

	if ( pm->cmd.buttons & BUTTON_ATTACK || delayedAction )
	{
		return qfalse;
	}

	ps = pm->ps;
	assert(ps);

	if ( pm->ps->weaponstate == WEAPON_FIRING )
	{
		PM_ContinueWeaponAnim(ps, WEAP_IDLE);
	}

	ps->weaponstate = WEAPON_READY;
	return qtrue;
}

/*
===============
PM_SwitchIfEmpty
===============
*/
void PM_SwitchIfEmpty( playerState_t *ps )
{
	assert(ps);

	if ( !BG_WeaponIsClipOnly(ps->weapon) )
	{
		return;
	}

	if ( ps->ammoclip[BG_ClipForWeapon(ps->weapon)] )
	{
		return;
	}

	if ( ps->ammo[BG_AmmoForWeapon(ps->weapon)] )
	{
		return;
	}

	BG_TakePlayerWeapon(ps, ps->weapon);
	PM_AddEvent(ps, EV_NOAMMO);
}

/*
===============
PM_Weapon_AllowReload
===============
*/
qboolean PM_Weapon_AllowReload( playerState_t *ps )
{
	int weapIndex;
	WeaponDef *weapDef;

	assert(ps);

	weapDef = BG_GetWeaponDef(ps->weapon);
	weapIndex = BG_ClipForWeapon(ps->weapon);

	if ( !ps->ammo[BG_AmmoForWeapon(ps->weapon)] || ps->ammoclip[weapIndex] >= BG_GetAmmoClipSize(weapIndex) )
	{
		return qfalse;
	}

	if ( !weapDef->noPartialReload )
	{
		return qtrue;
	}

	if ( weapDef->reloadAmmoAdd && weapDef->reloadAmmoAdd < BG_GetAmmoClipSize(weapIndex) )
	{
		return BG_GetAmmoClipSize(weapIndex) - ps->ammoclip[weapIndex] >= weapDef->reloadAmmoAdd;
	}

	return !ps->ammoclip[weapIndex];
}

/*
===============
PM_SetWeaponReloadAddAmmoDelay
===============
*/
void PM_SetWeaponReloadAddAmmoDelay( playerState_t *ps )
{
	int iReloadAddTime;
	WeaponDef *weapDef;

	assert(ps);

	weapDef = BG_GetWeaponDef(ps->weapon);

	if ( ps->weaponstate == WEAPON_RELOAD_START || ps->weaponstate == WEAPON_RELOAD_START_INTERUPT )
	{
		if ( weapDef->reloadStartAddTime )
		{
			if ( weapDef->reloadStartAddTime < weapDef->reloadStartTime )
				iReloadAddTime = weapDef->reloadStartAddTime;
			else
				iReloadAddTime = weapDef->reloadStartTime;
		}
		else
		{
			iReloadAddTime = 0;
		}
	}
	else
	{
		if ( ps->ammoclip[BG_ClipForWeapon(ps->weapon)] || weapDef->weaponType )
			iReloadAddTime = weapDef->reloadTime;
		else
			iReloadAddTime = weapDef->reloadEmptyTime;

		if ( weapDef->reloadAddTime && weapDef->reloadAddTime < iReloadAddTime )
			iReloadAddTime = weapDef->reloadAddTime;
	}

	if ( weapDef->boltAction && Com_BitCheck(ps->weaponrechamber, ps->weapon) )
	{
		if ( !iReloadAddTime )
			iReloadAddTime = ps->weaponTime;

		if ( weapDef->rechamberBoltTime < iReloadAddTime )
			iReloadAddTime = weapDef->rechamberBoltTime;

		if ( !iReloadAddTime )
			iReloadAddTime = 1;

		ps->weaponDelay = iReloadAddTime;
	}
	else if ( iReloadAddTime )
	{
		ps->weaponDelay = iReloadAddTime;
	}
}

/*
===============
PM_Weapon_FinishRechamber
===============
*/
void PM_Weapon_FinishRechamber( playerState_t *ps )
{
	assert(ps);
	PM_ContinueWeaponAnim(ps, WEAP_IDLE);
	ps->weaponstate = WEAPON_READY;
}

/*
===============
PM_WeaponClipEmpty
===============
*/
qboolean PM_WeaponClipEmpty( playerState_t *ps )
{
	assert(ps);
	return ps->ammoclip[BG_ClipForWeapon(ps->weapon)] == 0;
}

/*
===============
PM_WeaponUseAmmo
===============
*/
void PM_WeaponUseAmmo( playerState_t *ps, int weaponIndex, int amount )
{
	assert(ps);
	ps->ammoclip[BG_ClipForWeapon(weaponIndex)] -= amount;
}

/*
===============
PM_ReloadClip
===============
*/
void PM_ReloadClip( playerState_t *ps )
{
	int ammoAdd;
	int ammoInClip;
	int ammoReserve;
	int clipIndex;
	int ammoIndex;
	WeaponDef *weapDef;

	assert(ps);
	weapDef = BG_GetWeaponDef(ps->weapon);

	if ( ps->weaponstate == WEAPON_RELOAD_START || ps->weaponstate == WEAPON_RELOAD_START_INTERUPT )
	{
		if ( !weapDef->reloadStartAdd )
		{
			return;
		}
	}

	ammoIndex = BG_AmmoForWeapon(ps->weapon);
	clipIndex = BG_ClipForWeapon(ps->weapon);

	ammoReserve = ps->ammo[ammoIndex];
	ammoInClip = ps->ammoclip[clipIndex];

	ammoAdd = BG_GetAmmoClipSize(clipIndex) - ammoInClip;

	if ( ammoAdd > ammoReserve )
	{
		ammoAdd = ammoReserve;
	}

	if ( ps->weaponstate == WEAPON_RELOAD_START || ps->weaponstate == WEAPON_RELOAD_START_INTERUPT )
	{
		if ( weapDef->reloadStartAdd < BG_GetAmmoClipSize(clipIndex) && ammoAdd > weapDef->reloadStartAdd )
		{
			ammoAdd = weapDef->reloadStartAdd;
		}
	}
	else
	{
		if ( weapDef->reloadAmmoAdd && weapDef->reloadAmmoAdd < BG_GetAmmoClipSize(clipIndex) && ammoAdd > weapDef->reloadAmmoAdd )
		{
			ammoAdd = weapDef->reloadAmmoAdd;
		}
	}

	if ( ammoAdd )
	{
		ps->ammo[ammoIndex] -= ammoAdd;
		ps->ammoclip[clipIndex] += ammoAdd;
	}
}

/*
===============
PM_HoldBreathFire
===============
*/
void PM_HoldBreathFire( playerState_t *ps )
{
	int weapIndex;
	WeaponDef *weapDef;
	int breathHoldTime;

	assert(ps);

	weapIndex = BG_GetViewmodelWeaponIndex(ps);
	weapDef = BG_GetWeaponDef(weapIndex);

	if ( ps->fWeaponPosFrac != 1.0 )
	{
		return;
	}

	if ( !weapDef->adsOverlayReticle )
	{
		return;
	}

	if ( weapDef->weaponClass == WEAPCLASS_ITEM )
	{
		return;
	}

	breathHoldTime = player_breath_hold_time->current.decimal * 1000;

	if ( ps->holdBreathTimer < breathHoldTime )
	{
		ps->holdBreathTimer += player_breath_fire_delay->current.decimal * 1000;

		if ( ps->holdBreathTimer > breathHoldTime )
		{
			ps->holdBreathTimer = breathHoldTime;
		}
	}

	PM_EndHoldBreath(ps);
}

/*
===============
PM_IsAdsAllowed
===============
*/
bool PM_IsAdsAllowed( playerState_t *ps, pml_t *pml )
{
	int weapIndex;

	assert(ps);
	assert(pml);

	if ( ps->pm_type == PM_NORMAL_LINKED )
	{
		if ( pml->almostGroundPlane )
		{
			return false;
		}
	}

	if ( ps->pm_type >= PM_NORMAL_LINKED && ps->pm_type <= PM_DEAD_LINKED )
	{
		return false;
	}

	weapIndex = BG_GetViewmodelWeaponIndex(ps);

	if ( !BG_GetWeaponDef(weapIndex)->aimDownSight )
	{
		return false;
	}

	if ( ps->weaponstate >= WEAPON_OFFHAND_INIT && ps->weaponstate <= WEAPON_OFFHAND_END )
	{
		return false;
	}

	if ( ps->weaponstate == WEAPON_MELEE_INIT || ps->weaponstate == WEAPON_MELEE_FIRE )
	{
		return false;
	}

	if ( ps->weaponstate == WEAPON_RAISING || ps->weaponstate == WEAPON_DROPPING )
	{
		return false;
	}

	if ( ps->weaponstate >= WEAPON_BINOCULARS_INIT && ps->weaponstate <= WEAPON_BINOCULARS_END )
	{
		return PM_IsBinocularADSAllowed(ps->weaponstate);
	}

	return true;
}

/*
===============
BG_CalculateView_Velocity
===============
*/
void BG_CalculateView_Velocity( viewState_t *vs, vec3_t angles )
{
	int weapIndex;
	WeaponDef *weapDef;
	playerState_t *ps;
	float bobCycle;

	ps = vs->ps;
	assert(ps);

	weapIndex = BG_GetViewmodelWeaponIndex(vs->ps);
	weapDef = BG_GetWeaponDef(weapIndex);

	if ( ps->eFlags & EF_TURRET_ACTIVE )
	{
		return;
	}

	if ( ps->fWeaponPosFrac == 0 )
	{
		return;
	}

	if ( weapDef->adsViewBobMult == 0 )
	{
		return;
	}

	bobCycle = BG_GetBobCycle(ps);

	angles[0] -= ps->fWeaponPosFrac * weapDef->adsViewBobMult * BG_GetVerticalBobFactor(ps, bobCycle, vs->xyspeed, 45);
	angles[1] -= ps->fWeaponPosFrac * weapDef->adsViewBobMult * BG_GetHorizontalBobFactor(ps, bobCycle, vs->xyspeed, 45);
}

/*
===============
BG_CalculateView_BobAngles
===============
*/
void BG_CalculateView_BobAngles( viewState_t *vs, vec3_t angles )
{
	int weapIndex;
	WeaponDef *weapDef;
	playerState_t *ps;
	vec3_t vAngOfs;
	float speed;
	float cycle;
	float fPositionLerp;

	ps = vs->ps;
	assert(ps);

	weapIndex = BG_GetViewmodelWeaponIndex(vs->ps);
	weapDef = BG_GetWeaponDef(weapIndex);

	if ( !weapDef->adsOverlayReticle )
	{
		return;
	}

	cycle = BG_GetBobCycle(ps) + 0.7853981633974483 + (M_PI * 2);
	speed = vs->xyspeed * 0.16;

	vAngOfs[0] = -BG_GetVerticalBobFactor(ps, cycle, speed, 10);
	vAngOfs[1] = -BG_GetHorizontalBobFactor(ps, cycle, speed, 10);

	cycle -= 0.4712389167638204;
	speed *= 1.5;

	vAngOfs[2] = I_fmin(BG_GetHorizontalBobFactor(ps, cycle, speed, 10), 0.f);

	fPositionLerp = ps->fWeaponPosFrac;

	if ( fPositionLerp != 0 )
	{
		speed = 1.0 - (1.0 - weapDef->adsBobFactor) * fPositionLerp;

		vAngOfs[0] *= speed;
		vAngOfs[1] *= speed;
		vAngOfs[2] *= speed;
	}

	VectorScale(vAngOfs, fPositionLerp, vAngOfs);
	VectorAdd(angles, vAngOfs, angles);
}

/*
===============
BG_CalculateView_DamageKick
===============
*/
void BG_CalculateView_DamageKick( viewState_t *vs, vec3_t angles )
{
	int weapIndex;
	WeaponDef *weapDef;
	playerState_t *ps;
	float fFactor;
	float fRatio;

	if ( !vs->damageTime )
	{
		return;
	}

	ps = vs->ps;
	assert(ps);

	weapIndex = BG_GetViewmodelWeaponIndex(vs->ps);
	weapDef = BG_GetWeaponDef(weapIndex);

	fFactor = 1.0 - ps->fWeaponPosFrac * 0.5;

	if ( ps->fWeaponPosFrac != 0 && weapDef->adsOverlayReticle )
	{
		fFactor *= (ps->fWeaponPosFrac * 0.5 + 1.0);
	}

	fRatio = vs->time - vs->damageTime;

	if ( fRatio < 100 )
	{
		angles[0] += ((GetLeanFraction(fRatio / 100)) * fFactor) * vs->v_dmg_pitch;
		angles[2] += ((GetLeanFraction(fRatio / 100)) * fFactor) * vs->v_dmg_roll;
	}
	else
	{
		fRatio = 1.0 - (fRatio - 100) / 400;

		if ( fRatio > 0 )
		{
			angles[0] += ((1.0 - GetLeanFraction(1.0 - fRatio)) * fFactor) * vs->v_dmg_pitch;
			angles[2] += ((1.0 - GetLeanFraction(1.0 - fRatio)) * fFactor) * vs->v_dmg_roll;
		}
	}
}

/*
===============
BG_CalculateWeaponPosition_GunRecoil_SingleAngle
===============
*/
qboolean BG_CalculateWeaponPosition_GunRecoil_SingleAngle( float *fOffset, float *speed, float fTimeStep,
        float fOfsCap, float fGunKickAccel, float fGunKickSpeedMax, float fGunKickSpeedDecay, float fGunKickStaticDecay )
{
	if ( I_fabs(*fOffset) < 0.25 && I_fabs(*speed) < 1.0 )
	{
		*fOffset = 0;
		*speed = 0;
		return qtrue;
	}

	*fOffset = *speed * fTimeStep + *fOffset;

	if ( *fOffset > fOfsCap )
	{
		*fOffset = fOfsCap;

		if ( *speed > 0 )
		{
			*speed = 0;
		}
	}
	else if ( -fOfsCap > *fOffset )
	{
		*fOffset = -fOfsCap;

		if ( *speed < 0 )
		{
			*speed = 0;
		}
	}

	if ( *fOffset > 0 )
	{
		*speed = *speed - fGunKickAccel * fTimeStep;
	}
	else if ( *fOffset < 0 )
	{
		*speed = fGunKickAccel * fTimeStep + *speed;
	}

	*speed = *speed - *speed * fGunKickSpeedDecay * fTimeStep;

	if ( *speed > 0 )
	{
		*speed = *speed - fGunKickStaticDecay * fTimeStep;

		if ( *speed < 0 )
		{
			*speed = 0;
		}
	}
	else
	{
		*speed = fGunKickStaticDecay * fTimeStep + *speed;

		if ( *speed > 0 )
		{
			*speed = 0;
		}
	}

	if ( *speed > fGunKickSpeedMax )
	{
		*speed = fGunKickSpeedMax;
	}
	else if ( -fGunKickSpeedMax > *speed )
	{
		*speed = -fGunKickSpeedMax;
	}

	return qfalse;
}

/*
===============
BG_CalculateWeaponPosition_DamageKick
===============
*/
void BG_CalculateWeaponPosition_DamageKick( weaponState_t *ws, vec3_t angles )
{
	int weapIndex;
	WeaponDef *weapDef;
	playerState_t *ps;
	float fReturnTime;
	float fDeflectTime;
	float fFactor;
	float fRatio;

	if ( !ws->damageTime )
	{
		return;
	}

	ps = ws->ps;
	assert(ps);

	weapIndex = BG_GetViewmodelWeaponIndex(ws->ps);
	weapDef = BG_GetWeaponDef(weapIndex);

	fFactor = ps->fWeaponPosFrac * 0.5 + 0.5;

	fDeflectTime = fFactor * 100;
	fReturnTime = fFactor * 400;

	if ( ps->fWeaponPosFrac != 0 && weapDef->adsOverlayReticle )
	{
		fFactor *= (1.0 - ps->fWeaponPosFrac * 0.75);
	}

	fRatio = ws->time - ws->damageTime;

	if ( fDeflectTime > fRatio )
	{
		angles[0] += (GetLeanFraction(fRatio / fDeflectTime) * fFactor) * ws->v_dmg_pitch * 0.5;
		angles[1] -= (GetLeanFraction(fRatio / fDeflectTime) * fFactor) * ws->v_dmg_roll;
		angles[2] += (GetLeanFraction(fRatio / fDeflectTime) * fFactor) * ws->v_dmg_roll * 0.5;
	}
	else
	{
		fRatio = 1.0 - (fRatio - fDeflectTime) / fReturnTime;

		if ( fRatio > 0 )
		{
			angles[0] += ((1.0 - GetLeanFraction(1.0 - fRatio)) * fFactor) * ws->v_dmg_pitch * 0.5;
			angles[1] -= ((1.0 - GetLeanFraction(1.0 - fRatio)) * fFactor) * ws->v_dmg_roll;
			angles[2] += ((1.0 - GetLeanFraction(1.0 - fRatio)) * fFactor) * ws->v_dmg_roll * 0.5;
		}
	}
}

/*
===============
BG_CalculateWeaponPosition_BobOffset
===============
*/
void BG_CalculateWeaponPosition_BobOffset( weaponState_t *ws, vec3_t angles )
{
	int weapIndex;
	WeaponDef *weapDef;
	playerState_t *ps;
	vec3_t vAngOfs;
	float speed;
	float cycle;
	float fPositionLerp;

	ps = ws->ps;
	assert(ps);

	weapIndex = BG_GetViewmodelWeaponIndex(ws->ps);
	weapDef = BG_GetWeaponDef(weapIndex);

	cycle = BG_GetBobCycle(ps) + 0.7853981633974483 + (M_PI * 2);
	speed = ws->xyspeed * 0.16;

	vAngOfs[0] = -BG_GetVerticalBobFactor(ps, cycle, speed, 10);
	vAngOfs[1] = -BG_GetHorizontalBobFactor(ps, cycle, speed, 10);

	cycle -= 0.4712389167638204;
	speed *= 1.5;

	vAngOfs[2] = I_fmin(BG_GetHorizontalBobFactor(ps, cycle, speed, 10), 0.f);

	fPositionLerp = ps->fWeaponPosFrac;

	if ( fPositionLerp != 0 )
	{
		speed = 1.0 - (1.0 - weapDef->adsBobFactor) * fPositionLerp;

		vAngOfs[0] *= speed;
		vAngOfs[1] *= speed;
		vAngOfs[2] *= speed;
	}

	if ( weapDef->adsOverlayReticle )
	{
		VectorScale(vAngOfs, 1.0 - fPositionLerp, vAngOfs);
	}

	VectorAdd(angles, vAngOfs, angles);
}

/*
===============
BG_CalculateWeaponPosition_BaseAngles
===============
*/
void BG_CalculateWeaponPosition_BaseAngles( weaponState_t *ws, vec3_t angles )
{
	int weapIndex;
	WeaponDef *weapDef;
	playerState_t *ps;

	ps = ws->ps;
	assert(ps);

	weapIndex = BG_GetViewmodelWeaponIndex(ws->ps);
	weapDef = BG_GetWeaponDef(weapIndex);

	if ( BG_IsAimDownSightWeapon(weapIndex) )
	{
		angles[0] += ps->fWeaponPosFrac * weapDef->adsAimPitch;
	}

	BG_CalculateWeaponPosition_BasePosition_angles(ws, angles);
}

/*
===============
Binocular_State_End
===============
*/
void Binocular_State_End( pmove_t *pm )
{
	playerState_t *ps;

	assert(pm);
	ps = pm->ps;
	assert(ps);

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

/*
===============
Binocular_State_Raise
===============
*/
void Binocular_State_Raise( playerState_t *ps )
{
	WeaponDef *weapDef;

	assert(ps);
	weapDef = BG_GetWeaponDef(ps->weapon);

	ps->weaponstate = WEAPON_BINOCULARS_RAISE;
	ps->weaponTime = weapDef->quickDropTime;
	ps->weaponDelay = 0;

	PM_StartWeaponAnim(ps, WEAP_EMPTY_DROP);
}

/*
===============
Binocular_State_Exit_ADS
===============
*/
void Binocular_State_Exit_ADS( playerState_t *ps )
{
	assert(ps);

	ps->weaponstate = WEAPON_BINOCULARS_EXIT_ADS;
	ps->weaponTime = 0;
	ps->weaponDelay = 0;
}

/*
===============
PM_ExitBinocularsQuick
===============
*/
void PM_ExitBinocularsQuick( playerState_t *ps )
{
	assert(ps);

	ps->fWeaponPosFrac = 0;

	PM_ExitAimDownSight(ps);
	PM_ExitBinoculars(ps);
}

/*
===============
PM_UpdateGrenadeThrow
===============
*/
bool PM_UpdateGrenadeThrow( playerState_t *ps, pml_t *pml )
{
	int weapIndex;
	WeaponDef *weapDef;

	assert(ps);
	assert(pml);

	if ( !(ps->pm_flags & PMF_FRAG) )
	{
		return false;
	}

	weapIndex = ps->offHandIndex;
	weapDef = BG_GetWeaponDef(weapIndex);

	if ( weapDef->weaponType != WEAPTYPE_GRENADE )
	{
		return false;
	}

	if ( ps->grenadeTimeLeft <= 0 )
	{
		return false;
	}

	if ( weapDef->cookOffHold )
	{
		ps->grenadeTimeLeft -= pml->msec;
	}

	if ( ps->grenadeTimeLeft > 0 )
	{
		return false;
	}

	PM_WeaponUseAmmo(ps, weapIndex, 1);
	PM_AddEvent(ps, EV_GRENADE_SUICIDE);

	return true;
}

/*
===============
PM_Weapon_OffHand
===============
*/
void PM_Weapon_OffHand( pmove_t *pm )
{
	playerState_t *ps;
	WeaponDef *weapDef;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	assert(ps->offHandIndex != WP_NONE);

	if ( pm->oldcmd.buttons & (BUTTON_FRAG | BUTTON_SMOKE) && pm->cmd.buttons & (BUTTON_FRAG | BUTTON_SMOKE) )
	{
		ps->weaponDelay = 1;
		return;
	}

	weapDef = BG_GetWeaponDef(ps->offHandIndex);

	ps->weaponstate = WEAPON_OFFHAND;
	ps->weaponTime = weapDef->fireTime;
	ps->weaponDelay = 0;

	ps->pm_flags |= PMF_FRAG;

	BG_AddPredictableEventToPlayerstate(EV_USE_OFFHAND, ps->offHandIndex, ps);
	PM_StartWeaponAnim(ps, WEAP_ATTACK);

	PM_WeaponUseAmmo(ps, ps->offHandIndex, 1);
	BG_AnimScriptEvent(ps, ANIM_ET_FIREWEAPON, qfalse, qtrue);

	if ( !BG_WeaponAmmo(ps, ps->offHandIndex) )
	{
		PM_AddEvent(ps, EV_EMPTY_OFFHAND);
	}
}

/*
===============
PM_Weapon_OffHandInit
===============
*/
void PM_Weapon_OffHandInit( playerState_t *ps )
{
	WeaponDef *weapDef;

	assert(ps);
	assert(ps->offHandIndex != WP_NONE);

	weapDef = BG_GetWeaponDef(ps->weapon);

	ps->weaponstate = WEAPON_OFFHAND_INIT;
	ps->weaponTime = weapDef->quickDropTime;
	ps->weaponDelay = 0;

	ps->pm_flags &= ~PMF_FRAG;

	PM_ExitAimDownSight(ps);
	PM_StartWeaponAnim(ps, WEAP_EMPTY_DROP);
}

/*
===============
PM_Weapon_Idle
===============
*/
void PM_Weapon_Idle( playerState_t *ps )
{
	assert(ps);

	ps->pm_flags &= ~(PMF_FRAG | PMF_ADS_OVERRIDE);

	if ( ps->weaponstate >= WEAPON_BINOCULARS_INIT && ps->weaponstate <= WEAPON_BINOCULARS_END )
	{
		PM_ExitBinocularsQuick(ps);
	}

	ps->weaponTime = 0;
	ps->weaponDelay = 0;
	ps->weaponstate = WEAPON_READY;

	PM_StartWeaponAnim(ps, WEAP_IDLE);
}

/*
===============
PM_Weapon_CheckForMelee
===============
*/
void PM_Weapon_CheckForMelee( pmove_t *pm, qboolean delayedAction )
{
	playerState_t *ps;
	WeaponDef *weapDef;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	weapDef = BG_GetWeaponDef(pm->ps->weapon);

	// Using binoculars
	if ( ps->weaponstate >= WEAPON_BINOCULARS_INIT && ps->weaponstate <= WEAPON_BINOCULARS_END )
	{
		return;
	}

	// Using grenade
	if ( ps->weaponstate >= WEAPON_OFFHAND_INIT && ps->weaponstate <= WEAPON_OFFHAND_END )
	{
		return;
	}

	if ( !weapDef->meleeDamage )
	{
		return;
	}

	if ( delayedAction )
	{
		return;
	}

	// Reloading
	if ( ps->weaponDelay )
	{
		if ( ps->weaponstate < WEAPON_RELOADING && ps->weaponstate > WEAPON_RELOAD_END )
		{
			return;
		}
	}

	if ( !(pm->cmd.buttons & BUTTON_MELEE) )
	{
		ps->pm_flags &= ~PMF_MELEE;
		return;
	}

	if ( ps->pm_flags & PMF_MELEE )
	{
		return;
	}

	ps->pm_flags |= PMF_MELEE;

	if ( ps->fWeaponPosFrac > 0 && weapDef->adsOverlayReticle )
	{
		return;
	}

	if ( ps->weaponstate >= WEAPON_RAISING && ps->weaponstate <= WEAPON_DROPPING )
	{
		return;
	}

	// Already bashing
	if ( ps->weaponstate >= WEAPON_MELEE_INIT )
	{
		return;
	}

	BG_AnimScriptEvent(ps, ANIM_ET_MELEEATTACK, qfalse, qtrue);
	PM_StartWeaponAnim(ps, WEAP_MELEE_ATTACK);

	PM_AddEvent(ps, EV_MELEE_SWIPE);

	if ( !weapDef->meleeDelay )
	{
		PM_Weapon_FireMelee(ps);
		return;
	}

	ps->weaponTime = weapDef->meleeTime;
	ps->weaponDelay = weapDef->meleeDelay;
	ps->weaponstate = WEAPON_MELEE_INIT;

	PM_SetProneMovementOverride(ps);
}

/*
===============
PM_Weapon_SetFPSFireAnim
===============
*/
void PM_Weapon_SetFPSFireAnim( playerState_t *ps )
{
	assert(ps);

	if ( ps->fWeaponPosFrac > 0.75 )
	{
		if ( PM_WeaponClipEmpty(ps) )
			PM_StartWeaponAnim(ps, WEAP_ADS_ATTACK_LASTSHOT);
		else
			PM_StartWeaponAnim(ps, WEAP_ADS_ATTACK);
	}
	else
	{
		if ( PM_WeaponClipEmpty(ps) )
			PM_StartWeaponAnim(ps, WEAP_ATTACK_LASTSHOT);
		else
			PM_StartWeaponAnim(ps, WEAP_ATTACK);
	}
}

/*
===============
PM_Weapon_StartFiring
===============
*/
void PM_Weapon_StartFiring( playerState_t *ps, qboolean delayedAction )
{
	WeaponDef *weapDef;

	assert(ps);

	weapDef = BG_GetWeaponDef(ps->weapon);

	if ( weapDef->weaponType == WEAPTYPE_GRENADE )
	{
		if ( delayedAction )
		{
			return;
		}

		if ( PM_WeaponAmmoAvailable(ps) )
		{
			ps->grenadeTimeLeft = weapDef->fuseTime;

			PM_StartWeaponAnim(ps, WEAP_HOLD_FIRE);
			PM_AddEvent(ps, EV_PULLBACK_WEAPON);
		}

		ps->weaponDelay = weapDef->holdFireTime;
		ps->weaponTime = 0;
		return;
	}

	ps->weaponDelay = weapDef->fireDelay;
	ps->weaponTime = weapDef->fireTime;

	if ( weapDef->adsFire )
	{
		ps->weaponDelay = (int)((1.0 - ps->fWeaponPosFrac) * (1.0 / weapDef->OOPosAnimLength[0]));
	}

	BG_AnimScriptEvent(ps, ANIM_ET_FIREWEAPON, qfalse, qtrue);

	if ( weapDef->boltAction )
	{
		Com_BitSet(ps->weaponrechamber, ps->weapon);
	}

	if ( ps->weaponstate != WEAPON_FIRING )
	{
		if ( ps->fWeaponPosFrac >= 1.0 )
			ps->weaponRestrictKickTime = weapDef->fireDelay + weapDef->adsGunKickReducedKickBullets * weapDef->fireTime;
		else
			ps->weaponRestrictKickTime = weapDef->fireDelay + weapDef->hipGunKickReducedKickBullets * weapDef->fireTime;
	}

	ps->weaponstate = WEAPON_FIRING;

	PM_SetProneMovementOverride(ps);
}

/*
===============
PM_Weapon_WeaponTimeAdjust
===============
*/
qboolean PM_Weapon_WeaponTimeAdjust( pmove_t *pm, pml_t *pml )
{
	playerState_t *ps;
	WeaponDef *weapDef;

	ps = pm->ps;
	assert(ps);

	weapDef = BG_GetWeaponDef(pm->ps->weapon);

	if ( ps->weaponRestrictKickTime > 0 )
	{
		ps->weaponRestrictKickTime -= pml->msec;
		ps->weaponRestrictKickTime = I_max(ps->weaponRestrictKickTime, 0);
	}

	if ( ps->weaponTime )
	{
		ps->weaponTime -= pml->msec;

		if ( ps->weaponTime <= 0 )
		{
			if ( ( ps->weaponstate < WEAPON_OFFHAND_INIT || ps->weaponstate > WEAPON_OFFHAND_END )
			        && weapDef->semiAuto
			        && pm->cmd.buttons & BUTTON_ATTACK
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
				        && weapDef->segmentedReload
				        && weapDef->reloadStartTime )
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
	{
		return qfalse;
	}

	ps->weaponDelay -= pml->msec;

	if ( ps->weaponDelay > 0 )
	{
		return qfalse;
	}

	ps->weaponDelay = 0;

	return qtrue;
}

/*
===============
PM_Weapon_ReloadDelayedAction
===============
*/
void PM_Weapon_ReloadDelayedAction( playerState_t *ps )
{
	int reloadTime;
	WeaponDef *weapDef;

	assert(ps);
	weapDef = BG_GetWeaponDef(ps->weapon);

	if ( !weapDef->boltAction || !Com_BitCheck(ps->weaponrechamber, ps->weapon) )
	{
		PM_ReloadClip(ps);
		return;
	}

	Com_BitClear(ps->weaponrechamber, ps->weapon);
	PM_AddEvent(ps, EV_EJECT_BRASS);

	if ( ps->weaponstate == WEAPON_RELOAD_START || ps->weaponstate == WEAPON_RELOAD_START_INTERUPT )
	{
		if ( !weapDef->reloadStartAddTime )
		{
			return;
		}
	}

	if ( !ps->weaponTime )
	{
		PM_ReloadClip(ps);
		return;
	}

	if ( ps->weaponstate == WEAPON_RELOAD_START || ps->weaponstate == WEAPON_RELOAD_START_INTERUPT )
	{
		reloadTime = weapDef->reloadStartAddTime >= weapDef->reloadStartTime ? weapDef->reloadStartTime : weapDef->reloadStartAddTime;
	}
	else
	{
		reloadTime = ps->ammoclip[BG_ClipForWeapon(ps->weapon)] || weapDef->weaponType ? weapDef->reloadTime : weapDef->reloadEmptyTime;

		if ( weapDef->reloadAddTime && weapDef->reloadAddTime < reloadTime )
		{
			reloadTime = weapDef->reloadAddTime;
		}
	}

	reloadTime -= weapDef->rechamberBoltTime >= reloadTime ? qtrue : weapDef->rechamberBoltTime;

	if ( reloadTime > 0 )
	{
		ps->weaponDelay = reloadTime;
		return;
	}

	PM_ReloadClip(ps);
}

/*
===============
PM_Weapon_FinishWeaponChange
===============
*/
void PM_Weapon_FinishWeaponChange( pmove_t *pm )
{
	bool bHasWeapon;
	qboolean bAltRaise;
	WeaponDef *weapDef;
	playerState_t *ps;
	int newweapon;
	int oldweapon;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	assert( ps->weaponstate == WEAPON_DROPPING );

	//BG_GetWeaponDef(pm->ps->weapon);

	if ( Mantle_IsWeaponInactive(ps) || ps->pm_flags & PMF_LADDER || ps->pm_flags & PMF_DISABLEWEAPON || !Com_BitCheck(ps->weapons, pm->cmd.weapon) )
	{
		newweapon = WP_NONE;
		bHasWeapon = Com_BitCheck(ps->weapons, newweapon);
	}
	else
	{
		newweapon = pm->cmd.weapon;

		if ( newweapon > BG_GetNumWeapons() )
		{
			newweapon = WP_NONE;
		}

		bHasWeapon = Com_BitCheck(ps->weapons, newweapon);
	}

	if ( !bHasWeapon )
	{
		newweapon = WP_NONE;
	}

	oldweapon = ps->weapon;
	ps->weapon = (byte)newweapon;
	assert((byte)ps->weapon == newweapon);

	//BG_GetWeaponDef(ps->weapon);

	if ( oldweapon == newweapon )
	{
		ps->weaponstate = WEAPON_READY;
		PM_StartWeaponAnim(ps, WEAP_IDLE);
	}
	else if ( oldweapon )
	{
		ps->weaponstate = WEAPON_RAISING;
		PM_SetProneMovementOverride(ps);

		bAltRaise = qfalse;

		if ( newweapon )
		{
			bAltRaise = newweapon == BG_GetWeaponDef(oldweapon)->altWeaponIndex;
		}

		if ( bAltRaise )
		{
			ps->weaponTime = BG_GetWeaponDef(newweapon)->altRaiseTime;
		}
		else
		{
			PM_AddEvent(ps, EV_RAISE_WEAPON);
			ps->weaponTime = BG_GetWeaponDef(newweapon)->raiseTime;
		}

		weapDef = BG_GetWeaponDef(newweapon);
		assert(weapDef);

		BG_UpdateConditionValue(ps->clientNum, ANIM_COND_PLAYERANIMTYPE, weapDef->playerAnimType, qtrue);
		BG_UpdateConditionValue(ps->clientNum, ANIM_COND_WEAPONCLASS, weapDef->weaponClass, qtrue);

		if ( newweapon && newweapon == BG_GetWeaponDef(oldweapon)->altWeaponIndex )
		{
			if ( ps->aimSpreadScale < 128 )
			{
				ps->aimSpreadScale = 128;
			}

			PM_StartWeaponAnim(ps, WEAP_ALTSWITCHTO);
		}
		else
		{
			BG_AnimScriptEvent(ps, ANIM_ET_RAISEWEAPON, qfalse, qfalse);
			ps->aimSpreadScale = 255;

			PM_StartWeaponAnim(ps, WEAP_RAISE);
		}
	}
	else
	{
		ps->weaponstate = WEAPON_RAISING;
		ps->weaponTime = BG_GetWeaponDef(newweapon)->raiseTime;
		ps->aimSpreadScale = 255;

		PM_StartWeaponAnim(ps, WEAP_RAISE);
		PM_SetProneMovementOverride(ps);
	}
}

/*
===============
PM_BeginWeaponChange
===============
*/
void PM_BeginWeaponChange( playerState_t *ps, int newweapon )
{
	qboolean altswitch;
	int slot;
	int oldweapon;
	WeaponDef *weapDef;
	qboolean oldWeaponNoAmmo;

	assert(ps);

	if ( newweapon && !Com_BitCheck(ps->weapons, newweapon))
	{
		return;
	}

	if ( ps->weaponstate == WEAPON_DROPPING )
	{
		return;
	}

	ps->weaponDelay = 0;
	oldweapon = ps->weapon;

	if ( !oldweapon || !Com_BitCheck(ps->weapons, oldweapon) || ps->grenadeTimeLeft > 0 )
	{
		ps->weaponTime = 0;
		ps->weaponstate = WEAPON_DROPPING;
		ps->grenadeTimeLeft = 0;
		PM_SetProneMovementOverride(ps);
		return;
	}

	weapDef = BG_GetWeaponDef(oldweapon);
	altswitch = qfalse;

	if ( newweapon )
	{
		altswitch = newweapon == weapDef->altWeaponIndex;
	}

	oldWeaponNoAmmo = qtrue;

	if ( BG_WeaponIsClipOnly(oldweapon) )
	{
		oldWeaponNoAmmo = ps->ammoclip[BG_ClipForWeapon(oldweapon)] != 0;
	}

	ps->grenadeTimeLeft = 0;

	if ( altswitch )
	{
		PM_AddEvent(ps, EV_WEAPON_ALT);
		PM_StartWeaponAnim(ps, WEAP_ALTSWITCHFROM);
	}
	else if ( oldWeaponNoAmmo )
	{
		PM_AddEvent(ps, EV_PUTAWAY_WEAPON);
		PM_StartWeaponAnim(ps, WEAP_DROP);
	}

	if ( !altswitch && !(ps->pm_flags & PMF_MANTLE) )
	{
		BG_AnimScriptEvent(ps, ANIM_ET_DROPWEAPON, qfalse, qfalse);
	}

	ps->weaponstate = WEAPON_DROPPING;
	PM_SetProneMovementOverride(ps);

	if ( altswitch )
		ps->weaponTime = weapDef->altDropTime;
	else
		ps->weaponTime = weapDef->dropTime;

	if ( altswitch )
	{
		slot = BG_IsPlayerWeaponInSlot(ps, oldweapon, qtrue);

		if ( slot )
		{
			BG_SetPlayerWeaponForSlot(ps, slot, newweapon);
		}
	}

}

/*
===============
PM_SetReloadingState
===============
*/
void PM_SetReloadingState( playerState_t *ps )
{
	WeaponDef *weapDef;

	assert(ps);
	weapDef = BG_GetWeaponDef(ps->weapon);

	if ( ps->ammoclip[BG_ClipForWeapon(ps->weapon)] || weapDef->weaponType )
	{
		PM_StartWeaponAnim(ps, WEAP_RELOAD);
		ps->weaponTime = weapDef->reloadTime;
		PM_AddEvent(ps, EV_RELOAD);
	}
	else
	{
		PM_StartWeaponAnim(ps, WEAP_RELOAD_EMPTY);
		ps->weaponTime = weapDef->reloadEmptyTime;
		PM_AddEvent(ps, EV_RELOAD_FROM_EMPTY);
	}

	if ( ps->weaponstate == WEAPON_RELOAD_START_INTERUPT )
		ps->weaponstate = WEAPON_RELOADING_INTERUPT;
	else
		ps->weaponstate = WEAPON_RELOADING;

	PM_SetWeaponReloadAddAmmoDelay(ps);
}

/*
===============
PM_Weapon_CheckForRechamber
===============
*/
qboolean PM_Weapon_CheckForRechamber( playerState_t *ps, qboolean delayedAction )
{
	WeaponDef *weapDef;

	assert(ps);
	weapDef = BG_GetWeaponDef(ps->weapon);

	if ( ps->weaponstate >= WEAPON_BINOCULARS_INIT && ps->weaponstate <= WEAPON_BINOCULARS_END )
	{
		return qfalse;
	}

	if ( ps->weaponstate >= WEAPON_OFFHAND_INIT && ps->weaponstate <= WEAPON_OFFHAND_END )
	{
		return qfalse;
	}

	if ( !weapDef->boltAction || !Com_BitCheck(ps->weaponrechamber, ps->weapon) )
	{
		return qfalse;
	}

	if ( ps->weaponstate == WEAPON_RECHAMBERING )
	{
		if ( delayedAction )
		{
			Com_BitClear(ps->weaponrechamber, ps->weapon);
			PM_AddEvent(ps, EV_EJECT_BRASS);

			if ( ps->weaponTime )
			{
				return qtrue;
			}
		}
	}

	if ( ps->weaponTime )
	{
		if ( ps->weaponstate == WEAPON_FIRING || ps->weaponstate == WEAPON_RECHAMBERING
		        || ps->weaponstate == WEAPON_MELEE_INIT ||
		        ps->weaponstate == WEAPON_MELEE_FIRE || ps->weaponDelay )
		{
			return qfalse;
		}
	}

	if ( ps->weaponstate == WEAPON_RECHAMBERING )
	{
		PM_Weapon_FinishRechamber(ps);
		return qfalse;
	}

	if ( ps->weaponstate != WEAPON_READY )
	{
		return qfalse;
	}

	if ( ps->fWeaponPosFrac > 0.75 )
		PM_StartWeaponAnim(ps, WEAP_ADS_RECHAMBER);
	else
		PM_StartWeaponAnim(ps, WEAP_RECHAMBER);

	ps->weaponstate = WEAPON_RECHAMBERING;
	ps->weaponTime = weapDef->rechamberTime;

	if ( weapDef->rechamberBoltTime && weapDef->rechamberBoltTime < weapDef->rechamberTime )
		ps->weaponDelay = weapDef->rechamberBoltTime;
	else
		ps->weaponDelay = 1;

	PM_AddEvent(ps, EV_RECHAMBER_WEAPON);

	return qfalse;
}

/*
===============
PM_UpdateHoldBreath
===============
*/
void PM_UpdateHoldBreath( pmove_t *pm, pml_t *pml )
{
	playerState_t *ps;
	int breathGaspTime;
	int breathHoldTime;
	float lerp;
	float targetScale;
	int weapIndex;
	WeaponDef *weapDef;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	weapIndex = BG_GetViewmodelWeaponIndex(pm->ps);
	weapDef = BG_GetWeaponDef(weapIndex);

	breathHoldTime = player_breath_hold_time->current.decimal * 1000;
	breathGaspTime = player_breath_gasp_time->current.decimal * 1000;

	if ( breathHoldTime <= 0 )
	{
		PM_EndHoldBreath(ps);
		ps->holdBreathScale = 1.0;
		ps->holdBreathTimer = 0;
		return;
	}

	if ( ps->fWeaponPosFrac == 1.0 && weapDef->adsOverlayReticle && weapDef->weaponClass != WEAPCLASS_ITEM && pm->cmd.buttons & BUTTON_HOLDBREATH )
	{
		if ( !ps->holdBreathTimer )
		{
			PM_StartHoldBreath(ps);
		}
	}
	else
	{
		PM_EndHoldBreath(ps);
	}

	if ( ps->pm_flags & PMF_BREATH )
		ps->holdBreathTimer += pml->msec;
	else
		ps->holdBreathTimer -= pml->msec;

	if ( ps->holdBreathTimer < 0 )
	{
		ps->holdBreathTimer = 0;
	}

	if ( ps->pm_flags & PMF_BREATH )
	{
		if ( ps->holdBreathTimer > breathHoldTime )
		{
			ps->holdBreathTimer = breathHoldTime + breathGaspTime;
			PM_EndHoldBreath(ps);
		}
	}

	if ( ps->pm_flags & PMF_BREATH )
	{
		targetScale = 0;
		lerp = player_breath_hold_lerp->current.decimal;
	}
	else
	{
		targetScale = (player_breath_gasp_scale->current.decimal - 1.0) * (ps->holdBreathTimer / (float)(breathHoldTime + breathGaspTime)) + 1.0;
		lerp = player_breath_gasp_lerp->current.decimal;
	}

	ps->holdBreathScale = DiffTrack((targetScale - 1.0) * ps->fWeaponPosFrac + 1.0, ps->holdBreathScale, lerp, pml->frametime);
}

/*
===============
BG_CalculateWeaponPosition_GunRecoil
===============
*/
void BG_CalculateWeaponPosition_GunRecoil( weaponState_t *ws, vec3_t angles )
{
	int weapIndex;
	WeaponDef *weapDef;
	playerState_t *ps;
	float fPosLerp;
	float fGunKickStaticDecay;
	float fGunKickSpeedDecay;
	float fGunKickSpeedMax;
	float fGunKickAccel;
	qboolean bCanStop;
	float fTimeStep;
	float fTotalTime;

	ps = ws->ps;
	assert(ps);

	weapIndex = BG_GetViewmodelWeaponIndex(ws->ps);
	weapDef = BG_GetWeaponDef(weapIndex);

	if ( !BG_IsAimDownSightWeapon(weapIndex) )
	{
		return;
	}

	fPosLerp = ps->fWeaponPosFrac;

	fGunKickAccel = (weapDef->adsGunKickAccel - weapDef->hipGunKickAccel) * fPosLerp + weapDef->hipGunKickAccel;
	fGunKickSpeedMax = (weapDef->adsGunKickSpeedMax - weapDef->hipGunKickSpeedMax) * fPosLerp + weapDef->hipGunKickSpeedMax;
	fGunKickSpeedDecay = (weapDef->adsGunKickSpeedDecay - weapDef->hipGunKickSpeedDecay) * fPosLerp + weapDef->hipGunKickSpeedDecay;
	fGunKickStaticDecay = (weapDef->adsGunKickStaticDecay - weapDef->hipGunKickStaticDecay) * fPosLerp + weapDef->hipGunKickStaticDecay;

	fTotalTime = ws->frametime;

	while ( fTotalTime > 0 )
	{
		if ( fTotalTime > 0.005 )
		{
			fTimeStep = 0.0049999999;
			fTotalTime -= 0.005;

			bCanStop = BG_CalculateWeaponPosition_GunRecoil_SingleAngle(
			               ws->vGunOffset,
			               ws->vGunSpeed,
			               0.0049999999,
			               weapDef->gunMaxPitch,
			               fGunKickAccel,
			               fGunKickSpeedMax,
			               fGunKickSpeedDecay,
			               fGunKickStaticDecay);
		}
		else
		{
			fTimeStep = fTotalTime;
			fTotalTime = 0;

			bCanStop = BG_CalculateWeaponPosition_GunRecoil_SingleAngle(
			               ws->vGunOffset,
			               ws->vGunSpeed,
			               fTimeStep,
			               weapDef->gunMaxPitch,
			               fGunKickAccel,
			               fGunKickSpeedMax,
			               fGunKickSpeedDecay,
			               fGunKickStaticDecay);
		}

		if ( BG_CalculateWeaponPosition_GunRecoil_SingleAngle(
		            &ws->vGunOffset[YAW],
		            &ws->vGunSpeed[YAW],
		            fTimeStep,
		            weapDef->gunMaxYaw,
		            fGunKickAccel,
		            fGunKickSpeedMax,
		            fGunKickSpeedDecay,
		            fGunKickStaticDecay) )
		{
			if ( bCanStop )
			{
				break;
			}
		}
	}

	VectorAdd(angles, ws->vGunOffset, angles);
}

/*
===============
PM_Weapon_CheckForBinoculars
===============
*/
void PM_Weapon_CheckForBinoculars( pmove_t *pm )
{
	playerState_t *ps;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	// Disable binoculars while using scope
	if ( !( ps->weaponstate >= WEAPON_BINOCULARS_INIT && ps->weaponstate <= WEAPON_BINOCULARS_END
	        || (ps->fWeaponPosFrac <= 0 || !BG_GetWeaponDef(ps->weapon)->adsOverlayReticle)
	        && !(pm->cmd.buttons & ( BUTTON_ATTACK | BUTTON_MELEE | BUTTON_RELOAD | BUTTON_FRAG | BUTTON_SMOKE ) ) ) )
	{
		return;
	}

	// Optimization: no states except READY and binoculars are allowed
	if ( ps->weaponstate != WEAPON_READY )
	{
		if ( !( ps->weaponstate >= WEAPON_BINOCULARS_INIT && ps->weaponstate <= WEAPON_BINOCULARS_END ) )
		{
			return;
		}
	}

	// Switching weapon ?
	if ( ps->weaponstate >= WEAPON_BINOCULARS_INIT && ps->weaponstate <= WEAPON_BINOCULARS_END )
	{
		if ( ( ps->damageCount || pm->oldcmd.weapon != pm->cmd.weapon
		        || pm->oldcmd.offHandIndex != pm->cmd.offHandIndex
		        || ( pm->cmd.buttons & ( BUTTON_ATTACK | BUTTON_MELEE | BUTTON_RELOAD | BUTTON_FRAG | BUTTON_SMOKE ) ) ) )
		{
			PM_Weapon_Idle(ps);

			if ( pm->cmd.buttons & BUTTON_ATTACK )
			{
				ps->weaponTime = 1;
			}

			return;
		}
	}

	if ( pm->cmd.buttons & BUTTON_ATTACK )
	{
		return;
	}

	// Fire or release, used by Scr_Notify system
	if ( ps->weaponstate == WEAPON_BINOCULARS_EXIT_ADS )
	{
		if ( !( pm->oldcmd.buttons & ( BUTTON_USE | BUTTON_USERELOAD) ) && ( pm->cmd.buttons & ( BUTTON_USE | BUTTON_USERELOAD) ) )
		{
			PM_AddEvent(ps, EV_BINOCULAR_FIRE);
		}
		else if ( ( pm->oldcmd.buttons & ( BUTTON_USE | BUTTON_USERELOAD) ) && !( pm->cmd.buttons & ( BUTTON_USE | BUTTON_USERELOAD) ) )
		{
			PM_AddEvent(ps, EV_BINOCULAR_RELEASE);
		}
	}

	// default mode is toggle
	if ( player_toggleBinoculars->current.boolean )
	{
		if ( ( !( pm->oldcmd.buttons & BUTTON_BINOCULARS ) && pm->cmd.buttons & BUTTON_BINOCULARS ) && ( ps->weaponstate < WEAPON_BINOCULARS_INIT || ps->weaponstate > WEAPON_BINOCULARS_END ) )
		{
			// Pressed binoculars button - switch to binoculars
			if ( !BG_GetBinocularWeaponIndex() )
			{
				return; // No binoculars weapon file loaded
			}

			if ( pm->cmd.weapon )
			{
				if ( pm->cmd.buttons & BUTTON_ADS )
				{
					Binocular_State_Init(ps);
				}
				else
				{
					Binocular_State_Raise(ps);
				}
			}
			else
			{
				Binocular_State_Hold(ps);
			}
		}
		else if ( ( !( pm->oldcmd.buttons & BUTTON_BINOCULARS ) && pm->cmd.buttons & BUTTON_BINOCULARS ) && ( ps->weaponstate > WEAPON_BINOCULARS_INIT && ps->weaponstate < WEAPON_BINOCULARS_DROP ) )
		{
			// Pressed binoculars button while using binoculars - drop them
			if ( ps->weaponstate == WEAPON_BINOCULARS_RAISE )
			{
				Binocular_State_End(pm);
			}
			else
			{
				Binocular_State_Drop(ps);
			}
		}
	}
	else
	{
		if ( pm->cmd.buttons & BUTTON_BINOCULARS && ( ps->weaponstate < WEAPON_BINOCULARS_INIT || ps->weaponstate > WEAPON_BINOCULARS_END ) )
		{
			// Player is holding binoculars button
			if ( !BG_GetBinocularWeaponIndex() )
			{
				return; // No binoculars weapon file loaded
			}

			if ( pm->cmd.weapon )
			{
				if ( pm->cmd.buttons & BUTTON_ADS )
				{
					Binocular_State_Init(ps);
				}
				else
				{
					Binocular_State_Raise(ps);
				}
			}
			else
			{
				Binocular_State_Hold(ps);
			}
		}
		else if ( !( pm->cmd.buttons & BUTTON_BINOCULARS ) && ( ps->weaponstate > WEAPON_BINOCULARS_INIT && ps->weaponstate < WEAPON_BINOCULARS_DROP ) )
		{
			// Player released binoculars button
			if ( ps->weaponstate == WEAPON_BINOCULARS_RAISE )
			{
				Binocular_State_End(pm);
			}
			else
			{
				Binocular_State_Drop(ps);
			}
		}
	}
}

/*
===============
PM_Weapon_CheckForChangeWeapon
===============
*/
void PM_Weapon_CheckForChangeWeapon( pmove_t *pm )
{
	playerState_t *ps;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	// Using binoculars
	if ( ps->weaponstate >= WEAPON_BINOCULARS_INIT && ps->weaponstate <= WEAPON_BINOCULARS_END )
	{
		return;
	}

	// Using offhand
	if ( ps->weaponstate >= WEAPON_OFFHAND_INIT && ps->weaponstate <= WEAPON_OFFHAND_END )
	{
		return;
	}

	if ( !( !ps->weaponTime
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
	        && !ps->weaponDelay ) )
	{
		return;
	}

	if ( Mantle_IsWeaponInactive(ps) )
	{
		if ( !ps->weapon )
		{
			return;
		}

		PM_BeginWeaponChange(ps, WP_NONE);
		return;
	}

	if ( ps->pm_flags & PMF_LADDER )
	{
		if ( !ps->weapon )
		{
			return;
		}

		PM_BeginWeaponChange(ps, WP_NONE);
		return;
	}

	if ( ps->pm_flags & PMF_DISABLEWEAPON )
	{
		if ( !ps->weapon )
		{
			return;
		}

		PM_BeginWeaponChange(ps, WP_NONE);
		return;
	}

	if ( ps->weapon != pm->cmd.weapon )
	{
		if ( ( !(ps->pm_flags & PMF_UNKNOWN_8000) || !ps->weapon ) )
		{
			if ( !pm->cmd.weapon || BG_IsWeaponValid(ps, pm->cmd.weapon) )
			{
				PM_BeginWeaponChange(ps, pm->cmd.weapon);
				return;
			}
		}
	}

	if ( ps->weapon )
	{
		if ( !Com_BitCheck(ps->weapons, ps->weapon) )
		{
			PM_BeginWeaponChange(ps, WP_NONE);
		}
	}
}

/*
===============
PM_Weapon_FinishReload
===============
*/
void PM_Weapon_FinishReload( playerState_t *ps, qboolean delayedAction )
{
	WeaponDef *weapDef;

	assert(ps);
	assert(ps->weaponstate == WEAPON_RELOADING || ps->weaponstate == WEAPON_RELOADING_INTERUPT);

	weapDef = BG_GetWeaponDef(ps->weapon);

	if ( delayedAction )
	{
		PM_Weapon_ReloadDelayedAction(ps);

		if ( ps->weaponTime )
		{
			return;
		}
	}

	if ( ps->weaponTime )
	{
		return;
	}

	Com_BitClear(ps->weaponrechamber, ps->weapon);

	if ( !weapDef->segmentedReload )
	{
		ps->weaponstate = WEAPON_READY;
		PM_StartWeaponAnim(ps, WEAP_IDLE);
		return;
	}

	if ( ps->weaponstate != WEAPON_RELOADING_INTERUPT )
	{
		if ( PM_Weapon_AllowReload(ps) )
		{
			PM_SetReloadingState(ps);
			return;
		}
	}

	if ( weapDef->reloadEndTime )
	{
		ps->weaponstate = WEAPON_RELOAD_END;
		PM_StartWeaponAnim(ps, WEAP_RELOAD_END);

		ps->weaponTime = weapDef->reloadEndTime;
		PM_AddEvent(ps, EV_RELOAD_END);
	}
	else
	{
		ps->weaponstate = WEAPON_READY;
		PM_StartWeaponAnim(ps, WEAP_IDLE);
	}
}

/*
===============
PM_Weapon_FinishReloadStart
===============
*/
void PM_Weapon_FinishReloadStart( playerState_t *ps, qboolean delayedAction )
{
	WeaponDef *weapDef;

	assert(ps);
	weapDef = BG_GetWeaponDef(ps->weapon);
	assert((ps->weaponstate == WEAPON_RELOAD_START) || (ps->weaponstate == WEAPON_RELOAD_START_INTERUPT));

	if ( delayedAction )
	{
		PM_Weapon_ReloadDelayedAction(ps);
	}

	if ( ps->weaponTime )
	{
		return;
	}

	if ( ps->weaponstate != WEAPON_RELOAD_START_INTERUPT || !ps->ammoclip[BG_ClipForWeapon(ps->weapon)] )
	{
		if ( PM_Weapon_AllowReload(ps) )
		{
			PM_SetReloadingState(ps);
			return;
		}
	}

	Com_BitClear(ps->weaponrechamber, ps->weapon);

	if ( weapDef->reloadEndTime )
	{
		ps->weaponstate = WEAPON_RELOAD_END;
		PM_StartWeaponAnim(ps, WEAP_RELOAD_END);

		ps->weaponTime = weapDef->reloadEndTime;
		PM_AddEvent(ps, EV_RELOAD_END);
	}
	else
	{
		ps->weaponstate = WEAPON_READY;
		PM_StartWeaponAnim(ps, WEAP_IDLE);
	}
}

/*
===============
PM_BeginWeaponReload
===============
*/
void PM_BeginWeaponReload( playerState_t *ps )
{
	WeaponDef *weapDef;

	assert(ps);
	weapDef = BG_GetWeaponDef(ps->weapon);

	if ( ps->weaponstate != WEAPON_READY && ps->weaponstate != WEAPON_FIRING && ps->weaponstate != WEAPON_RECHAMBERING )
	{
		return;
	}

	if ( !BG_ValidateWeaponNumber(ps->weapon) )
	{
		return;
	}

	if ( !BG_WeaponIsClipOnly(ps->weapon) )
	{
		BG_AnimScriptEvent(ps, ANIM_ET_RELOAD, qfalse, qtrue);
	}

	PM_AddEvent(ps, EV_RESET_ADS);

	if ( !weapDef->segmentedReload || !weapDef->reloadStartTime )
	{
		PM_SetReloadingState(ps);
		return;
	}

	PM_StartWeaponAnim(ps, WEAP_RELOAD_START);

	ps->weaponTime = weapDef->reloadStartTime;
	ps->weaponstate = WEAPON_RELOAD_START;

	PM_AddEvent(ps, EV_RELOAD_START);
	PM_SetWeaponReloadAddAmmoDelay(ps);
}

/*
===============
PM_Weapon_CheckFiringAmmo
===============
*/
qboolean PM_Weapon_CheckFiringAmmo( playerState_t *ps )
{
	WeaponDef *weapDef;

	assert(ps);
	assert(ps->weapon != WP_NONE);

	weapDef = BG_GetWeaponDef(ps->weapon);

	if ( PM_WeaponAmmoAvailable(ps) > 0 )
	{
		return qtrue;
	}

	if ( weapDef->weaponType != WEAPTYPE_GRENADE )
	{
		if ( ps->ammo[BG_AmmoForWeapon(ps->weapon)] < 1 )
		{
			PM_AddEvent(ps, EV_NOAMMO);
		}
	}

	if ( ps->ammo[BG_AmmoForWeapon(ps->weapon)] > 0 )
	{
		PM_BeginWeaponReload(ps);
	}
	else
	{
		Com_BitClear(ps->weaponrechamber, ps->weapon);
		PM_ContinueWeaponAnim(ps, WEAP_IDLE);
		ps->weaponTime += 500;
	}

	return qfalse;
}

/*
===============
PM_Weapon_CheckForReload
===============
*/
void PM_Weapon_CheckForReload( pmove_t *pm )
{
	playerState_t *ps;
	qboolean doReload;
	qboolean reloadRequested;
	WeaponDef *weapDef;

	doReload = qfalse;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	weapDef = BG_GetWeaponDef(pm->ps->weapon);

	// Using binoculars
	if ( ps->weaponstate >= WEAPON_BINOCULARS_INIT && ps->weaponstate <= WEAPON_BINOCULARS_END )
	{
		return;
	}

	// Using offhand
	if ( ps->weaponstate >= WEAPON_OFFHAND_INIT && ps->weaponstate <= WEAPON_OFFHAND_END )
	{
		return;
	}

	// Using melee
	if ( ps->weaponstate >= WEAPON_MELEE_INIT && ps->weaponstate <= WEAPON_MELEE_FIRE )
	{
		return;
	}

	reloadRequested = pm->cmd.buttons & BUTTON_RELOAD;

	// Deprecated flag, not used
	if ( ps->pm_flags & PMF_RELOAD )
	{
		ps->pm_flags &= ~PMF_RELOAD;
		reloadRequested = qtrue;
	}

	if ( weapDef->segmentedReload && ( ps->weaponstate == WEAPON_RELOAD_START || ps->weaponstate == WEAPON_RELOADING ) )
	{
		if ( pm->cmd.buttons & BUTTON_ATTACK && !( pm->oldcmd.buttons & BUTTON_ATTACK ) )
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
			{
				BG_AnimScriptEvent(ps, ANIM_ET_RELOAD, qfalse, qtrue);
			}
		}
		return;
	}

	if ( reloadRequested )
	{
		if ( PM_Weapon_AllowReload(ps) )
		{
			doReload = qtrue;
		}
	}

	if ( !ps->ammoclip[BG_ClipForWeapon(ps->weapon)] && ps->ammo[BG_AmmoForWeapon(ps->weapon)] )
	{
		if ( ps->weaponstate != WEAPON_FIRING )
		{
			// No ammo and player is proning and moving, reload not allowed
			if ( !(ps->pm_flags & PMF_PRONE) || !pm->cmd.forwardmove && !pm->cmd.rightmove )
			{
				doReload = qtrue;
			}
		}
	}

	if ( doReload )
	{
		PM_BeginWeaponReload(ps);
	}
}

/*
===============
PM_Weapon_FireWeapon
===============
*/
void PM_Weapon_FireWeapon( playerState_t *ps, qboolean delayedAction )
{
	WeaponDef *weapDef;

	assert(ps);
	weapDef = BG_GetWeaponDef(ps->weapon);

	PM_Weapon_StartFiring(ps, delayedAction);

	if ( !PM_Weapon_CheckFiringAmmo(ps) )
	{
		return;
	}

	if ( ps->weaponDelay )
	{
		return;
	}

	if ( PM_WeaponAmmoAvailable(ps) != -1 && !(ps->eFlags & EF_TURRET_ACTIVE) )
	{
		PM_WeaponUseAmmo(ps, ps->weapon, 1);
	}

	if ( weapDef->weaponType == WEAPTYPE_GRENADE )
	{
		ps->weaponTime = weapDef->fireTime;
	}

	PM_Weapon_SetFPSFireAnim(ps);

	if ( PM_WeaponClipEmpty(ps) )
		PM_AddEvent(ps, EV_FIRE_WEAPON_LASTSHOT);
	else
		PM_AddEvent(ps, EV_FIRE_WEAPON);

	PM_HoldBreathFire(ps);
	PM_Weapon_AddFiringAimSpreadScale(ps);
	PM_SwitchIfEmpty(ps);
}

/*
===============
PM_Weapon_CheckForOffHand
===============
*/
void PM_Weapon_CheckForOffHand( pmove_t *pm )
{
	WeaponDef *weapDef;
	int offhandSlot;
	int offHandIndex;
	playerState_t *ps;

	assert(pm);
	ps = pm->ps;
	assert(ps);

	if ( pm->ps->eFlags & EF_TURRET_ACTIVE )
	{
		return;
	}

	if ( ps->pm_flags & PMF_DISABLEWEAPON )
	{
		return;
	}

	// Using binoculars
	if ( ps->weaponstate >= WEAPON_BINOCULARS_INIT && ps->weaponstate <= WEAPON_BINOCULARS_END )
	{
		return;
	}

	// Using offhand
	if ( ps->weaponstate >= WEAPON_OFFHAND_INIT && ps->weaponstate <= WEAPON_OFFHAND ) // No OFFHAND_END ? Edit: yes, this is correct
	{
		return;
	}

	if ( Com_BitCheck(ps->weapons, pm->cmd.offHandIndex) )
	{
		ps->offHandIndex = pm->cmd.offHandIndex;
	}

	assert( ps->offHandIndex == WP_NONE || BG_GetWeaponDef( ps->offHandIndex )->offhandClass != OFFHAND_CLASS_NONE );

	if ( pm->cmd.buttons & BUTTON_FRAG )
	{
		offhandSlot = OFFHAND_SLOT_FRAG_GRENADE;
	}
	else
	{
		if ( !( pm->cmd.buttons & BUTTON_SMOKE ) )
		{
			return;
		}

		offhandSlot = OFFHAND_SLOT_SMOKE_GRENADE;
	}

	offHandIndex = BG_GetFirstAvailableOffhand(ps, offhandSlot);
	assert( offHandIndex == WP_NONE || BG_GetWeaponDef( offHandIndex )->offhandClass != OFFHAND_CLASS_NONE );

	if ( !offHandIndex )
	{
		PM_SendEmtpyOffhandEvent(ps);
		return;
	}

	BG_AddPredictableEventToPlayerstate(EV_SWITCH_OFFHAND, offHandIndex, ps);

	ps->offHandIndex = offHandIndex;
	weapDef = BG_GetWeaponDef(ps->offHandIndex);

	if ( weapDef->weaponType != WEAPTYPE_GRENADE )
	{
		Com_Error(ERR_DROP, "[%s] Only grenades are currently supported for off hand use\n", weapDef->szInternalName);
	}

	if ( !weapDef->offhandClass )
	{
		Com_Error(ERR_DROP, "[%s] No offhand class set\n", weapDef->szInternalName);
	}

	if ( !ps->weapon || ps->weaponstate == WEAPON_OFFHAND_END )
		PM_Weapon_OffHandPrepare(ps);
	else
		PM_Weapon_OffHandInit(ps);
}

/*
===============
BG_SetupWeaponAlts
===============
*/
void BG_SetupWeaponAlts( int weapIndex, int (*regWeap)(int) )
{
	WeaponDef *altWeapDef;
	WeaponDef *weapDef;
	int altWeapIndex;

	weapDef = BG_GetWeaponDef(weapIndex);
	assert(weapDef);
	weapDef->altWeaponIndex = 0;

	if ( !*weapDef->altWeapon )
	{
		return;
	}

	altWeapIndex = BG_GetWeaponIndexForName(weapDef->altWeapon, regWeap);

	if ( !altWeapIndex )
	{
		Com_Error(ERR_DROP, "could not find altWeapon '%s' for weapon '%s'", weapDef->altWeapon, weapDef->szInternalName);
	}

	weapDef->altWeaponIndex = altWeapIndex;
	altWeapDef = bg_weaponDefs[altWeapIndex];

	if ( weapDef->weaponSlot != altWeapDef->weaponSlot )
	{
		Com_Error(ERR_DROP, "weapon '%s' does not have same weaponSlot setting as its alt weapon '%s'", weapDef->szInternalName, altWeapDef->szInternalName);
	}

	if ( weapDef->slotStackable != altWeapDef->slotStackable )
	{
		Com_Error(ERR_DROP, "weapon '%s' does not have same slotStackable setting as its alt weapon '%s'", weapDef->szInternalName, altWeapDef->szInternalName);
	}
}

/*
===============
BG_SetupClipIndexes
===============
*/
void BG_SetupClipIndexes( int weapIndex )
{
	WeaponDef *weapDef;
	WeaponDef *altWeapDef;
	int clipIndex;
	int altWeapIndex;

	weapDef = BG_GetWeaponDef(weapIndex);
	assert(weapDef);

	for ( clipIndex = 0; ; clipIndex++ )
	{
		if ( clipIndex >= bg_iNumWeapClips )
		{
			bg_weapClips[clipIndex] = weapDef;
			weapDef->clipIndex = clipIndex;
			bg_iNumWeapClips++;
			return;
		}

		if ( !strcmp(bg_weapClips[clipIndex]->clipName, weapDef->clipName) )
		{
			break;
		}
	}

	weapDef->clipIndex = clipIndex;

	if ( bg_weapClips[clipIndex]->clipSize == weapDef->clipSize )
	{
		return;
	}

	if ( !clipIndex )
	{
		return;
	}

	for ( altWeapIndex = 1; altWeapIndex < weapIndex; altWeapIndex++ )
	{
		altWeapDef = bg_weaponDefs[altWeapIndex];

		if ( I_stricmp(bg_weapClips[clipIndex]->clipName, altWeapDef->clipName) )
		{
			continue;
		}

		if ( altWeapDef->clipSize != bg_weapClips[clipIndex]->clipSize )
		{
			continue;
		}

		Com_Error(ERR_DROP,
		          "Clip Size mismatch for \"%s\" clip: '%s\" set it to %i, but \"%s\" already set it to %i.\n",
		          weapDef->ammoName,
		          weapDef->szInternalName,
		          weapDef->clipSize,
		          altWeapDef->szInternalName,
		          altWeapDef->clipSize);
	}
}

/*
===============
BG_SetupSharedAmmoIndexes
===============
*/
void BG_SetupSharedAmmoIndexes( int weapIndex )
{
	WeaponDef *weapDef;
	WeaponDef *altWeapDef;
	int sharedAmmoIndex;
	int altWeapIndex;

	weapDef = BG_GetWeaponDef(weapIndex);
	assert(weapDef);

	weapDef->sharedAmmoCapIndex = -1;

	if ( !*weapDef->sharedAmmoCapName )
	{
		return;
	}

	Com_DPrintf("%s: %s\n", weapDef->szInternalName, weapDef->sharedAmmoCapName);

	for ( sharedAmmoIndex = 0; ; sharedAmmoIndex++ )
	{
		if ( sharedAmmoIndex >= bg_iNumSharedAmmoCaps )
		{
			bg_sharedAmmoCaps[sharedAmmoIndex] = weapDef;
			weapDef->sharedAmmoCapIndex = sharedAmmoIndex;
			bg_iNumSharedAmmoCaps++;
			return;
		}

		if ( !I_stricmp(bg_sharedAmmoCaps[sharedAmmoIndex]->sharedAmmoCapName, weapDef->sharedAmmoCapName) )
		{
			break;
		}
	}

	weapDef->sharedAmmoCapIndex = sharedAmmoIndex;

	if ( bg_sharedAmmoCaps[sharedAmmoIndex]->sharedAmmoCap == weapDef->sharedAmmoCap )
	{
		return;
	}

	if ( !sharedAmmoIndex )
	{
		return;
	}

	for ( altWeapIndex = 1; altWeapIndex < weapIndex; altWeapIndex++ )
	{
		altWeapDef = bg_weaponDefs[altWeapIndex];

		if ( I_stricmp(bg_sharedAmmoCaps[sharedAmmoIndex]->sharedAmmoCapName, altWeapDef->sharedAmmoCapName) )
		{
			continue;
		}

		if ( altWeapDef->sharedAmmoCap != bg_sharedAmmoCaps[sharedAmmoIndex]->sharedAmmoCap )
		{
			continue;
		}

		Com_Error(ERR_DROP,
		          "Shared ammo cap mismatch for \"%s\" shared ammo cap: '%s\" set it to %i, but \"%s\" already set it to %i.\n",
		          weapDef->sharedAmmoCapName,
		          weapDef->szInternalName,
		          weapDef->sharedAmmoCap,
		          altWeapDef->szInternalName,
		          altWeapDef->sharedAmmoCap);
	}
}

/*
===============
BG_SetupAmmoIndexes
===============
*/
void BG_SetupAmmoIndexes( int weapIndex )
{
	WeaponDef *weapDef;
	WeaponDef *altWeapDef;
	int ammoIndex;
	int altWeapIndex;

	weapDef = BG_GetWeaponDef(weapIndex);
	assert(weapDef);

	for ( ammoIndex = 0; ; ammoIndex++ )
	{
		if ( ammoIndex >= bg_iNumAmmoTypes )
		{
			bg_weapAmmoTypes[ammoIndex] = weapDef;
			weapDef->ammoIndex = ammoIndex;
			bg_iNumAmmoTypes++;
			return;
		}

		if ( !strcmp(bg_weapAmmoTypes[ammoIndex]->ammoName, weapDef->ammoName) )
		{
			break;
		}
	}

	weapDef->ammoIndex = ammoIndex;

	if ( bg_weapAmmoTypes[ammoIndex]->maxAmmo == weapDef->maxAmmo )
	{
		return;
	}

	if ( !ammoIndex )
	{
		return;
	}

	for ( altWeapIndex = 1; altWeapIndex < weapIndex; altWeapIndex++ )
	{
		altWeapDef = bg_weaponDefs[altWeapIndex];

		if ( I_stricmp(bg_weapAmmoTypes[ammoIndex]->ammoName, altWeapDef->ammoName) )
		{
			continue;
		}

		if ( altWeapDef->maxAmmo != bg_weapAmmoTypes[ammoIndex]->maxAmmo )
		{
			continue;
		}

		Com_Error(
		    ERR_DROP,
		    "Max ammo mismatch for \"%s\" ammo: '%s\" set it to %i, but \"%s\" already set it to %i.\n",
		    weapDef->ammoName,
		    weapDef->szInternalName,
		    weapDef->maxAmmo,
		    altWeapDef->szInternalName,
		    altWeapDef->maxAmmo);
	}
}

/*
===============
BG_SetupWeaponIndex
===============
*/
void BG_SetupWeaponIndex( int weapIndex )
{
	BG_SetupAmmoIndexes(weapIndex);
	BG_SetupSharedAmmoIndexes(weapIndex);
	BG_SetupClipIndexes(weapIndex);
	BG_FillInWeaponItems(weapIndex);
}

/*
===============
BG_SetupWeaponDef
===============
*/
int BG_SetupWeaponDef( WeaponDef *weapDef, int (*regWeap)(int) )
{
	int weapIndex;

	bg_iNumWeapons++;
	weapIndex = bg_iNumWeapons;

	bg_weaponDefs[bg_iNumWeapons] = weapDef;

	BG_SetupWeaponIndex(weapIndex);
	BG_SetupWeaponAlts(weapIndex, regWeap);

	if ( regWeap )
	{
		regWeap(weapIndex);
	}

	return weapIndex;
}
