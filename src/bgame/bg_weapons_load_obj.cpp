#include "../qcommon/qcommon.h"
#include "bg_public.h"

unsigned int g_playerAnimTypeNamesCount;
const char *g_playerAnimTypeNames[32];
WeaponDef bg_defaultWeaponDefs;

static const char *globalTypesFilename = "mp/playeranimtypes.txt";    // to prevent redundant params

const char *szWeapTypeNames[] =
{
	"bullet",
	"grenade",
	"projectile",
	"binoculars"
};

const char *szWeapSlotNames[] =
{
	"none",
	"primary",
	"primaryb"
};

const char *szProjectileExplosionNames[] =
{
	"grenade",
	"rocket",
	"none"
};

const char *szWeapOverlayReticleNames[] =
{
	"none",
	"crosshair",
	"FG42",
	"Springfield",
	"binoculars"
};

const char *szWeapClassNames[] =
{
	"rifle",
	"mg",
	"smg",
	"spread",
	"pistol",
	"grenade",
	"rocketlauncher",
	"turret",
	"non-player",
	"item"
};

const char *szWeapStanceNames[] =
{
	"stand",
	"duck",
	"prone"
};

const char *offhandClassNames[] =
{
	"None",
	"Frag Grenade",
	"Smoke Grenade"
};

#define WEF( x ) # x,(intptr_t)&( (WeaponFullDef*)0 )->x
cspField_t weaponDefFields[] =
{
	{ WEF( displayName ), 0, },
	{ WEF( AIOverlayDescription ), 0, },
	{ WEF( modeName ), 0, },
	{ WEF( playerAnimType ), 15, },
	{ WEF( gunModel ), 0, },
	{ WEF( handModel ), 0, },
	{ WEF( idleAnim ), 0, },
	{ WEF( emptyIdleAnim ), 0, },
	{ WEF( fireAnim ), 0, },
	{ WEF( holdFireAnim ), 0, },
	{ WEF( lastShotAnim ), 0, },
	{ WEF( rechamberAnim ), 0, },
	{ WEF( meleeAnim ), 0, },
	{ WEF( reloadAnim ), 0, },
	{ WEF( reloadEmptyAnim ), 0, },
	{ WEF( reloadStartAnim ), 0, },
	{ WEF( reloadEndAnim ), 0, },
	{ WEF( raiseAnim ), 0, },
	{ WEF( dropAnim ), 0, },
	{ WEF( altRaiseAnim ), 0, },
	{ WEF( altDropAnim ), 0, },
	{ WEF( quickRaiseAnim ), 0, },
	{ WEF( quickDropAnim ), 0, },
	{ WEF( adsFireAnim ), 0, },
	{ WEF( adsLastShotAnim ), 0, },
	{ WEF( adsRechamberAnim ), 0, },
	{ WEF( adsUpAnim ), 0, },
	{ WEF( adsDownAnim ), 0, },
	{ WEF( script ), 0, },
	{ WEF( weaponType ), 8, },
	{ WEF( weaponClass ), 9, },
	{ WEF( weaponSlot ), 11, },
	{ WEF( slotStackable ), 5, },
	{ WEF( offhandClass ), 14, },
	{ WEF( viewFlashEffect ), 0, },
	{ WEF( worldFlashEffect ), 0, },
	{ WEF( pickupSound ), 0, },
	{ WEF( ammoPickupSound ), 0, },
	{ WEF( projectileSound ), 0, },
	{ WEF( pullbackSound ), 0, },
	{ WEF( fireSound ), 0, },
	{ WEF( fireSoundPlayer ), 0, },
	{ WEF( loopFireSound ), 0, },
	{ WEF( loopFireSoundPlayer ), 0, },
	{ WEF( stopFireSound ), 0, },
	{ WEF( stopFireSoundPlayer ), 0, },
	{ WEF( lastShotSound ), 0, },
	{ WEF( lastShotSoundPlayer ), 0, },
	{ WEF( meleeSwipeSound ), 0, },
	{ WEF( rechamberSound ), 0, },
	{ WEF( rechamberSoundPlayer ), 0, },
	{ WEF( reloadSound ), 0, },
	{ WEF( reloadSoundPlayer ), 0, },
	{ WEF( reloadEmptySound ), 0, },
	{ WEF( reloadEmptySoundPlayer ), 0, },
	{ WEF( reloadStartSound ), 0, },
	{ WEF( reloadStartSoundPlayer ), 0, },
	{ WEF( reloadEndSound ), 0, },
	{ WEF( reloadEndSoundPlayer ), 0, },
	{ WEF( raiseSound ), 0, },
	{ WEF( altSwitchSound ), 0, },
	{ WEF( putawaySound ), 0, },
	{ WEF( noteTrackSoundA ), 0, },
	{ WEF( noteTrackSoundB ), 0, },
	{ WEF( noteTrackSoundC ), 0, },
	{ WEF( noteTrackSoundD ), 0, },
	{ WEF( shellEjectEffect ), 0, },
	{ WEF( lastShotEjectEffect ), 0, },
	{ WEF( reticleCenter ), 0, },
	{ WEF( reticleSide ), 0, },
	{ WEF( reticleCenterSize ), 4, },
	{ WEF( reticleSideSize ), 4, },
	{ WEF( reticleMinOfs ), 4, },
	{ WEF( standMoveF ), 6, },
	{ WEF( standMoveR ), 6, },
	{ WEF( standMoveU ), 6, },
	{ WEF( standRotP ), 6, },
	{ WEF( standRotY ), 6, },
	{ WEF( standRotR ), 6, },
	{ WEF( duckedOfsF ), 6, },
	{ WEF( duckedOfsR ), 6, },
	{ WEF( duckedOfsU ), 6, },
	{ WEF( duckedMoveF ), 6, },
	{ WEF( duckedMoveR ), 6, },
	{ WEF( duckedMoveU ), 6, },
	{ WEF( duckedRotP ), 6, },
	{ WEF( duckedRotY ), 6, },
	{ WEF( duckedRotR ), 6, },
	{ WEF( proneOfsF ), 6, },
	{ WEF( proneOfsR ), 6, },
	{ WEF( proneOfsU ), 6, },
	{ WEF( proneMoveF ), 6, },
	{ WEF( proneMoveR ), 6, },
	{ WEF( proneMoveU ), 6, },
	{ WEF( proneRotP ), 6, },
	{ WEF( proneRotY ), 6, },
	{ WEF( proneRotR ), 6, },
	{ WEF( posMoveRate ), 6, },
	{ WEF( posProneMoveRate ), 6, },
	{ WEF( standMoveMinSpeed ), 6, },
	{ WEF( duckedMoveMinSpeed ), 6, },
	{ WEF( proneMoveMinSpeed ), 6, },
	{ WEF( posRotRate ), 6, },
	{ WEF( posProneRotRate ), 6, },
	{ WEF( standRotMinSpeed ), 6, },
	{ WEF( duckedRotMinSpeed ), 6, },
	{ WEF( proneRotMinSpeed ), 6, },
	{ WEF( worldModel ), 0, },
	{ WEF( hudIcon ), 0, },
	{ WEF( modeIcon ), 0, },
	{ WEF( startAmmo ), 4, },
	{ WEF( ammoName ), 0, },
	{ WEF( clipName ), 0, },
	{ WEF( maxAmmo ), 4, },
	{ WEF( clipSize ), 4, },
	{ WEF( shotCount ), 4, },
	{ WEF( sharedAmmoCapName ), 0, },
	{ WEF( sharedAmmoCap ), 4, },
	{ WEF( damage ), 4, },
	{ WEF( playerDamage ), 4, },
	{ WEF( meleeDamage ), 4, },
	{ WEF( minDamage ), 4, },
	{ WEF( minPlayerDamage ), 4, },
	{ WEF( maxDamageRange ), 6, },
	{ WEF( minDamageRange ), 6, },
	{ WEF( destabilizationBaseTime ), 6, },
	{ WEF( destabilizationTimeReductionRatio ), 6, },
	{ WEF( destabilizationAngleMax ), 6, },
	{ WEF( destabilizeDistance ), 4, },
	{ WEF( fireDelay ), 7, },
	{ WEF( meleeDelay ), 7, },
	{ WEF( fireTime ), 7, },
	{ WEF( rechamberTime ), 7, },
	{ WEF( rechamberBoltTime ), 7, },
	{ WEF( holdFireTime ), 7, },
	{ WEF( meleeTime ), 7, },
	{ WEF( reloadTime ), 7, },
	{ WEF( reloadEmptyTime ), 7, },
	{ WEF( reloadAddTime ), 7, },
	{ WEF( reloadStartTime ), 7, },
	{ WEF( reloadStartAddTime ), 7, },
	{ WEF( reloadEndTime ), 7, },
	{ WEF( dropTime ), 7, },
	{ WEF( raiseTime ), 7, },
	{ WEF( altDropTime ), 7, },
	{ WEF( altRaiseTime ), 7, },
	{ WEF( quickDropTime ), 7, },
	{ WEF( quickRaiseTime ), 7, },
	{ WEF( fuseTime ), 7, },
	{ WEF( autoAimRange ), 6, },
	{ WEF( slowdownAimRange ), 6, },
	{ WEF( slowdownAimRangeAds ), 6, },
	{ WEF( lockonAimRange ), 6, },
	{ WEF( lockonAimRangeAds ), 6, },
	{ WEF( enemyCrosshairRange ), 6, },
	{ WEF( moveSpeedScale ), 6, },
	{ WEF( idleCrouchFactor ), 6, },
	{ WEF( idleProneFactor ), 6, },
	{ WEF( gunMaxPitch ), 6, },
	{ WEF( gunMaxYaw ), 6, },
	{ WEF( swayMaxAngle ), 6, },
	{ WEF( swayLerpSpeed ), 6, },
	{ WEF( swayPitchScale ), 6, },
	{ WEF( swayYawScale ), 6, },
	{ WEF( swayHorizScale ), 6, },
	{ WEF( swayVertScale ), 6, },
	{ WEF( swayShellShockScale ), 6, },
	{ WEF( adsSwayMaxAngle ), 6, },
	{ WEF( adsSwayLerpSpeed ), 6, },
	{ WEF( adsSwayPitchScale ), 6, },
	{ WEF( adsSwayYawScale ), 6, },
	{ WEF( adsSwayHorizScale ), 6, },
	{ WEF( adsSwayVertScale ), 6, },
	{ WEF( rifleBullet ), 5, },
	{ WEF( armorPiercing ), 5, },
	{ WEF( semiAuto ), 5, },
	{ WEF( boltAction ), 5, },
	{ WEF( aimDownSight ), 5, },
	{ WEF( rechamberWhileAds ), 5, },
	{ WEF( adsViewErrorMin ), 6, },
	{ WEF( adsViewErrorMax ), 6, },
	{ WEF( clipOnly ), 5, },
	{ WEF( cookOffHold ), 5, },
	{ WEF( wideListIcon ), 5, },
	{ WEF( adsFire ), 5, },
	{ WEF( killIcon ), 0, },
	{ WEF( wideKillIcon ), 5, },
	{ WEF( flipKillIcon ), 5, },
	{ WEF( noPartialReload ), 5, },
	{ WEF( segmentedReload ), 5, },
	{ WEF( reloadAmmoAdd ), 4, },
	{ WEF( reloadStartAdd ), 4, },
	{ WEF( altWeapon ), 0, },
	{ WEF( dropAmmoMin ), 4, },
	{ WEF( dropAmmoMax ), 4, },
	{ WEF( explosionRadius ), 4, },
	{ WEF( explosionInnerDamage ), 4, },
	{ WEF( explosionOuterDamage ), 4, },
	{ WEF( projectileSpeed ), 4, },
	{ WEF( projectileSpeedUp ), 4, },
	{ WEF( projectileModel ), 0, },
	{ WEF( projExplosionType ), 13, },
	{ WEF( projExplosionEffect ), 0, },
	{ WEF( projExplosionSound ), 0, },
	{ WEF( projImpactExplode ), 5, },
	{ WEF( parallelDefaultBounce ), 6, },
	{ WEF( parallelBarkBounce ), 6, },
	{ WEF( parallelBrickBounce ), 6, },
	{ WEF( parallelCarpetBounce ), 6, },
	{ WEF( parallelClothBounce ), 6, },
	{ WEF( parallelConcreteBounce ), 6, },
	{ WEF( parallelDirtBounce ), 6, },
	{ WEF( parallelFleshBounce ), 6, },
	{ WEF( parallelFoliageBounce ), 6, },
	{ WEF( parallelGlassBounce ), 6, },
	{ WEF( parallelGrassBounce ), 6, },
	{ WEF( parallelGravelBounce ), 6, },
	{ WEF( parallelIceBounce ), 6, },
	{ WEF( parallelMetalBounce ), 6, },
	{ WEF( parallelMudBounce ), 6, },
	{ WEF( parallelPaperBounce ), 6, },
	{ WEF( parallelPlasterBounce ), 6, },
	{ WEF( parallelRockBounce ), 6, },
	{ WEF( parallelSandBounce ), 6, },
	{ WEF( parallelSnowBounce ), 6, },
	{ WEF( parallelWaterBounce ), 6, },
	{ WEF( parallelWoodBounce ), 6, },
	{ WEF( parallelAsphaltBounce ), 6, },
	{ WEF( perpendicularDefaultBounce ), 6, },
	{ WEF( perpendicularBarkBounce ), 6, },
	{ WEF( perpendicularBrickBounce ), 6, },
	{ WEF( perpendicularCarpetBounce ), 6, },
	{ WEF( perpendicularClothBounce ), 6, },
	{ WEF( perpendicularConcreteBounce ), 6, },
	{ WEF( perpendicularDirtBounce ), 6, },
	{ WEF( perpendicularFleshBounce ), 6, },
	{ WEF( perpendicularFoliageBounce ), 6, },
	{ WEF( perpendicularGlassBounce ), 6, },
	{ WEF( perpendicularGrassBounce ), 6, },
	{ WEF( perpendicularGravelBounce ), 6, },
	{ WEF( perpendicularIceBounce ), 6, },
	{ WEF( perpendicularMetalBounce ), 6, },
	{ WEF( perpendicularMudBounce ), 6, },
	{ WEF( perpendicularPaperBounce ), 6, },
	{ WEF( perpendicularPlasterBounce ), 6, },
	{ WEF( perpendicularRockBounce ), 6, },
	{ WEF( perpendicularSandBounce ), 6, },
	{ WEF( perpendicularSnowBounce ), 6, },
	{ WEF( perpendicularWaterBounce ), 6, },
	{ WEF( perpendicularWoodBounce ), 6, },
	{ WEF( perpendicularAsphaltBounce ), 6, },
	{ WEF( projTrailEffect ), 0, },
	{ WEF( projectileDLight ), 4, },
	{ WEF( projectileRed ), 6, },
	{ WEF( projectileGreen ), 6, },
	{ WEF( projectileBlue ), 6, },
	{ WEF( adsTransInTime ), 7, },
	{ WEF( adsTransOutTime ), 7, },
	{ WEF( adsIdleAmount ), 6, },
	{ WEF( adsIdleSpeed ), 6, },
	{ WEF( adsZoomFov ), 6, },
	{ WEF( adsZoomInFrac ), 6, },
	{ WEF( adsZoomOutFrac ), 6, },
	{ WEF( adsOverlayShader ), 0, },
	{ WEF( adsOverlayReticle ), 10, },
	{ WEF( adsOverlayWidth ), 6, },
	{ WEF( adsOverlayHeight ), 6, },
	{ WEF( adsBobFactor ), 6, },
	{ WEF( adsViewBobMult ), 6, },
	{ WEF( adsAimPitch ), 6, },
	{ WEF( adsCrosshairInFrac ), 6, },
	{ WEF( adsCrosshairOutFrac ), 6, },
	{ WEF( adsReloadTransTime ), 7, },
	{ WEF( adsGunKickReducedKickBullets ), 4, },
	{ WEF( adsGunKickReducedKickPercent ), 6, },
	{ WEF( adsGunKickPitchMin ), 6, },
	{ WEF( adsGunKickPitchMax ), 6, },
	{ WEF( adsGunKickYawMin ), 6, },
	{ WEF( adsGunKickYawMax ), 6, },
	{ WEF( adsGunKickAccel ), 6, },
	{ WEF( adsGunKickSpeedMax ), 6, },
	{ WEF( adsGunKickSpeedDecay ), 6, },
	{ WEF( adsGunKickStaticDecay ), 6, },
	{ WEF( adsViewKickPitchMin ), 6, },
	{ WEF( adsViewKickPitchMax ), 6, },
	{ WEF( adsViewKickYawMin ), 6, },
	{ WEF( adsViewKickYawMax ), 6, },
	{ WEF( adsViewKickCenterSpeed ), 6, },
	{ WEF( adsSpread ), 6, },
	{ WEF( hipSpreadStandMin ), 6, },
	{ WEF( hipSpreadDuckedMin ), 6, },
	{ WEF( hipSpreadProneMin ), 6, },
	{ WEF( hipSpreadMax ), 6, },
	{ WEF( hipSpreadDuckedMax ), 6, },
	{ WEF( hipSpreadProneMax ), 6, },
	{ WEF( hipSpreadDecayRate ), 6, },
	{ WEF( hipSpreadFireAdd ), 6, },
	{ WEF( hipSpreadTurnAdd ), 6, },
	{ WEF( hipSpreadMoveAdd ), 6, },
	{ WEF( hipSpreadDuckedDecay ), 6, },
	{ WEF( hipSpreadProneDecay ), 6, },
	{ WEF( hipReticleSidePos ), 6, },
	{ WEF( hipIdleAmount ), 6, },
	{ WEF( hipIdleSpeed ), 6, },
	{ WEF( hipGunKickReducedKickBullets ), 4, },
	{ WEF( hipGunKickReducedKickPercent ), 6, },
	{ WEF( hipGunKickPitchMin ), 6, },
	{ WEF( hipGunKickPitchMax ), 6, },
	{ WEF( hipGunKickYawMin ), 6, },
	{ WEF( hipGunKickYawMax ), 6, },
	{ WEF( hipGunKickAccel ), 6, },
	{ WEF( hipGunKickSpeedMax ), 6, },
	{ WEF( hipGunKickSpeedDecay ), 6, },
	{ WEF( hipGunKickStaticDecay ), 6, },
	{ WEF( hipViewKickPitchMin ), 6, },
	{ WEF( hipViewKickPitchMax ), 6, },
	{ WEF( hipViewKickYawMin ), 6, },
	{ WEF( hipViewKickYawMax ), 6, },
	{ WEF( hipViewKickCenterSpeed ), 6, },
	{ WEF( leftArc ), 6, },
	{ WEF( rightArc ), 6, },
	{ WEF( topArc ), 6, },
	{ WEF( bottomArc ), 6, },
	{ WEF( accuracy ), 6, },
	{ WEF( aiSpread ), 6, },
	{ WEF( playerSpread ), 6, },
	{ WEF( maxVertTurnSpeed ), 6, },
	{ WEF( maxHorTurnSpeed ), 6, },
	{ WEF( minVertTurnSpeed ), 6, },
	{ WEF( minHorTurnSpeed ), 6, },
	{ WEF( pitchConvergenceTime ), 6, },
	{ WEF( yawConvergenceTime ), 6, },
	{ WEF( suppressionTime ), 6, },
	{ WEF( maxRange ), 6, },
	{ WEF( animHorRotateInc ), 6, },
	{ WEF( playerPositionDist ), 6, },
	{ WEF( stance ), 12, },
	{ WEF( useHintString ), 0, },
	{ WEF( dropHintString ), 0, },
	{ WEF( horizViewJitter ), 6, },
	{ WEF( vertViewJitter ), 6, },
	{ WEF( fightDist ), 6, },
	{ WEF( maxDist ), 6, },
	{ WEF( aiVsAiAccuracyGraph ), 0, },
	{ WEF( aiVsPlayerAccuracyGraph ), 0, },
	{ WEF( locNone ), 6, },
	{ WEF( locHelmet ), 6, },
	{ WEF( locHead ), 6, },
	{ WEF( locNeck ), 6, },
	{ WEF( locTorsoUpper ), 6, },
	{ WEF( locTorsoLower ), 6, },
	{ WEF( locRightArmUpper ), 6, },
	{ WEF( locRightArmLower ), 6, },
	{ WEF( locRightHand ), 6, },
	{ WEF( locLeftArmUpper ), 6, },
	{ WEF( locLeftArmLower ), 6, },
	{ WEF( locLeftHand ), 6, },
	{ WEF( locRightLegUpper ), 6, },
	{ WEF( locRightLegLower ), 6, },
	{ WEF( locRightFoot ), 6, },
	{ WEF( locLeftLegUpper ), 6, },
	{ WEF( locLeftLegLower ), 6, },
	{ WEF( locLeftFoot ), 6, },
	{ WEF( locGun ), 6, },
	{ WEF( fireRumble ), 0, },
	{ WEF( meleeImpactRumble ), 0, },
};

int BG_GetWeaponSlotForName(const char *name)
{
	int i;

	for ( i = 0; i < COUNT_OF(szWeapSlotNames); ++i )
	{
		if ( !strcasecmp(name, szWeapSlotNames[i]) )
			return i;
	}

	return 0;
}

const char* BG_GetWeaponSlotNameForIndex(int index)
{
	return szWeapSlotNames[index];
}

void BG_LoadPlayerAnimTypes()
{
	unsigned int count;
	int len;
	fileHandle_t f;
	char buffer[4096];
	char *token;
	const char *data_p;

	g_playerAnimTypeNamesCount = 0;
	len = FS_FOpenFileByMode(globalTypesFilename, &f, FS_READ);

	if ( len < 0 )
		Com_Error(ERR_DROP, "Couldn't load file ", globalTypesFilename);

	if ( len > 4095 )
	{
		FS_FCloseFile(f);
		Com_Error(ERR_DROP, "%s max size exceeded", globalTypesFilename);
	}

	FS_Read(buffer, len, f);
	buffer[len] = 0;
	FS_FCloseFile(f);
	data_p = buffer;

	Com_BeginParseSession("BG_AnimParseAnimScript");

	while ( 1 )
	{
		token = Com_Parse(&data_p);

		if ( !token || !*token )
			break;

		if ( g_playerAnimTypeNamesCount > 63 )
			Com_Error(ERR_DROP, "Player anim type array size exceeded");

		count = g_playerAnimTypeNamesCount;
		g_playerAnimTypeNames[count] = (char *)Hunk_AllocLowInternal(strlen(token) + 1);
		strcpy((char *)g_playerAnimTypeNames[g_playerAnimTypeNamesCount], token);
		++g_playerAnimTypeNamesCount;
	}

	Com_EndParseSession();
}

void BG_LoadWeaponStrings()
{
	int index;

	for ( index = 0; index < g_playerAnimTypeNamesCount; ++index )
		BG_InitWeaponString(index, g_playerAnimTypeNames[index]);
}

void InitWeaponDef(WeaponDef *weapon)
{
	cspField_t *field;
	int count;

	weapon->szInternalName = "";
	count = 0;
	field = weaponDefFields;

	while ( count < sizeof( weaponDefFields ) / sizeof( weaponDefFields[0] ) )
	{
		if ( !field->iFieldType )
			*(const char **)((char *)&weapon->szInternalName + field->iOffset) = "";

		++count;
		++field;
	}
}

int Weapon_GetStringArrayIndex(const char *value, const char **stringArray, int arraySize)
{
	int i;

	for ( i = 0; i < arraySize; ++i )
	{
		if ( !strcasecmp(value, stringArray[i]) )
			return i;
	}

	return -1;
}

int BG_ParseWeaponDefSpecificFieldType(unsigned char *pStruct, const char *pValue, int iFieldType)
{
	int value;
	WeaponDef* weapon;

	weapon = (WeaponDef*)pStruct;

	switch ( iFieldType )
	{
	case WFT_WEAPONTYPE:
		value = Weapon_GetStringArrayIndex(pValue, szWeapTypeNames, COUNT_OF(szWeapTypeNames));
		if ( value < 0 )
			Com_Error(ERR_DROP, "Unknown weapon type \"%s\" in \"%s\"\n", pValue, weapon->szInternalName);
		weapon->weaponType = value;
		break;

	case WFT_WEAPONCLASS:
		value = Weapon_GetStringArrayIndex(pValue, szWeapClassNames, COUNT_OF(szWeapClassNames));
		if ( value < 0 )
			Com_Error(ERR_DROP, "Unknown weapon class \"%s\" in \"%s\"\n", pValue, weapon->szInternalName);
		weapon->weaponClass = value;
		break;

	case WFT_OVERLAYRETICLE:
		value = Weapon_GetStringArrayIndex(pValue, szWeapOverlayReticleNames, COUNT_OF(szWeapOverlayReticleNames));
		if ( value < 0 )
			Com_Error(ERR_DROP, "Unknown weapon overlay reticle \"%s\" in \"%s\"\n", pValue, weapon->szInternalName);
		weapon->adsOverlayReticle = value;
		break;

	case WFT_SLOT:
		value = Weapon_GetStringArrayIndex(pValue, szWeapSlotNames, COUNT_OF(szWeapSlotNames));
		if ( value < 0 )
			Com_Error(ERR_DROP, "Unknown weapon slot \"%s\" in \"%s\"\n", pValue, weapon->szInternalName);
		weapon->weaponSlot = value;
		break;

	case WFT_STANCE:
		value = Weapon_GetStringArrayIndex(pValue, szWeapStanceNames, COUNT_OF(szWeapStanceNames));
		if ( value < 0 )
			Com_Error(ERR_DROP, "Unknown weapon stance \"%s\" in \"%s\"\n", pValue, weapon->szInternalName);
		weapon->stance = value;
		break;

	case WFT_PROJ_EXPLOSION:
		value = Weapon_GetStringArrayIndex(pValue, szProjectileExplosionNames, COUNT_OF(szProjectileExplosionNames));
		if ( value < 0 )
			Com_Error(ERR_DROP, "Unknown projectile explosion \"%s\" in \"%s\"\n", pValue, weapon->szInternalName);
		weapon->projExplosionType = value;
		break;

	case WFT_OFFHAND_CLASS:
		value = Weapon_GetStringArrayIndex(pValue, offhandClassNames, COUNT_OF(offhandClassNames));
		if ( value < 0 )
			Com_Error(ERR_DROP, "Unknown offhand class \"%s\" in \"%s\"\n", pValue, weapon->szInternalName);
		weapon->offhandClass = value;
		break;

	case WFT_ANIMTYPE:
		value = Weapon_GetStringArrayIndex(pValue, g_playerAnimTypeNames, g_playerAnimTypeNamesCount);
		if ( value < 0 )
			Com_Error(ERR_DROP, "Unknown playerAnimType \"%s\" in \"%s\"\n", pValue, weapon->szInternalName);
		weapon->playerAnimType = value;
		break;

	default:
		Com_Error(ERR_DROP, "Bad field type %i in %s\n", iFieldType, weapon->szInternalName);
		return 0;
	}

	return 1;
}

void BG_SetupTransitionTimes(WeaponDef *weapon)
{
	if ( weapon->adsTransInTime <= 0 )
		weapon->OOPosAnimLength[0] = 0.0033333334;
	else
		weapon->OOPosAnimLength[0] = 1.0 / (float)weapon->adsTransInTime;

	if ( weapon->adsTransOutTime <= 0 )
		weapon->OOPosAnimLength[1] = 0.0020000001;
	else
		weapon->OOPosAnimLength[1] = 1.0 / (float)weapon->adsTransOutTime;
}

void BG_CheckWeaponDamageRanges(WeaponDef *weapon)
{
	if ( weapon->maxDamageRange <= 0.0 )
		weapon->maxDamageRange = 999999.0;

	if ( weapon->minDamageRange <= 0.0 )
		weapon->minDamageRange = 999999.12;
}

void BG_CheckProjectileValues(WeaponDef *weapon)
{
	if ( (float)weapon->projectileSpeed <= 0.0 )
		Com_Error(ERR_DROP, "Projectile speed for WeapType %s must be greater than 0.0", weapon->displayName);

	if ( weapon->destabilizationAngleMax >= 45.0 || weapon->destabilizationAngleMax < 0.0 )
		Com_Error( ERR_DROP, "Destabilization angle for for WeapType %s must be between 0 and 45 degrees", weapon->displayName);

	if ( weapon->destabilizationBaseTime <= 0.0 )
		Com_Error(ERR_DROP, "Destabilization base time for for WeapType %s must be positive", weapon->displayName);

	if ( weapon->destabilizationTimeReductionRatio <= 0.0 )
		Com_Error(ERR_DROP, "Destabilization reduction ratio for for WeapType %s must be positive", weapon->displayName);
}

WeaponDef* BG_LoadWeaponDefInternal(const char *folder, const char *fileName)
{
	size_t n;
	fileHandle_t f;
	WeaponDef *newWeaponDef;
	char buf[8192];
	char name[64];
	int len;

	n = strlen("WEAPONFILE");
	newWeaponDef = (WeaponDef *)Hunk_AllocLowInternal(sizeof(WeaponDef));
	InitWeaponDef(newWeaponDef);
	Com_sprintf(name, sizeof(name), "weapons/%s/%s", folder, fileName);
	len = FS_FOpenFileByMode(name, &f, FS_READ);

	if ( len >= 0 )
	{
		FS_Read(buf, n, f);
		buf[n] = 0;

		if ( !strncmp(buf, "WEAPONFILE", n) )
		{
			if ( (int)(len - n) <= 0x1FFF )
			{
				memset(buf, 0, sizeof(buf));
				FS_Read(buf, len - n, f);
				buf[len - n] = 0;
				FS_FCloseFile(f);

				if ( Info_Validate(buf) )
				{
					SetConfigString((char **)newWeaponDef, fileName);

					if ( ParseConfigStringToStruct(
					            (unsigned char *)newWeaponDef,
					            weaponDefFields,
					            sizeof( weaponDefFields ) / sizeof( weaponDefFields[0] ),
					            buf,
					            16,
					            BG_ParseWeaponDefSpecificFieldType,
					            SetConfigString2) )
					{
						BG_SetupTransitionTimes(newWeaponDef);
						BG_CheckWeaponDamageRanges(newWeaponDef);

						if ( newWeaponDef->enemyCrosshairRange > 15000.0 )
							Com_Error(ERR_DROP, "Enemy crosshair ranges should be less than %f ", 15000.0);

						if ( newWeaponDef->weaponType == 2 )
							BG_CheckProjectileValues(newWeaponDef);

						if ( G_ParseWeaponAccurayGraphs(newWeaponDef) )
						{
							I_strlwr((char *)newWeaponDef->ammoName);
							I_strlwr((char *)newWeaponDef->clipName);

							return newWeaponDef;
						}
						else
						{
							return 0;
						}
					}
					else
					{
						return 0;
					}
				}
				else
				{
					Com_Printf("^3WARNING: \"%s\" is not a valid weapon file\n", name);
					return 0;
				}
			}
			else
			{
				Com_Printf("^3WARNING: \"%s\" Is too long of a weapon file to parse\n", name);
				FS_FCloseFile(f);
				return 0;
			}
		}
		else
		{
			Com_Printf("^3WARNING: \"%s\" does not appear to be a weapon file\n", name);
			FS_FCloseFile(f);
			return 0;
		}
	}
	else
	{
		Com_Printf("^3WARNING: Could not load weapon file '%s'\n", name);
		return 0;
	}

	return 0;
}

WeaponDef* BG_LoadWeaponDef(const char *folderName, const char *weaponName)
{
	WeaponDef *weapon;

	if ( !*weaponName )
		return 0;

	weapon = BG_LoadWeaponDefInternal(folderName, weaponName);

	if ( weapon )
		return weapon;

	weapon = BG_LoadWeaponDefInternal(folderName, "defaultweapon_mp");

	if ( !weapon )
		Com_Error(ERR_DROP, "BG_LoadWeaponDef: Could not find default weapon");

	SetConfigString((char **)weapon, weaponName);
	return weapon;
}

WeaponDef *BG_LoadDefaultWeaponDef()
{
	InitWeaponDef(&bg_defaultWeaponDefs);

	bg_defaultWeaponDefs.szInternalName = "none";
	bg_defaultWeaponDefs.accuracyGraphName[0] = "noweapon.accu";
	bg_defaultWeaponDefs.accuracyGraphName[1] = "noweapon.accu";

	G_ParseWeaponAccurayGraphs(&bg_defaultWeaponDefs);

	return &bg_defaultWeaponDefs;
}