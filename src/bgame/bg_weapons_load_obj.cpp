#include "../qcommon/qcommon.h"
#include "bg_public.h"

unsigned int g_playerAnimTypeNamesCount;
const char *g_playerAnimTypeNames[32];
animStringItem_t weaponStrings[128];
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

#define WEF( x ) # x,(intptr_t)&( (WeaponDef*)0 )->x
cspField_t weaponDefFields[] =
{
	{ "displayName", 4, 0, },
	{ "AIOverlayDescription", 8, 0, },
	{ "modeName", 112, 0, },
	{ "playerAnimType", 116, 15, },
	{ "gunModel", 12, 0, },
	{ "handModel", 16, 0, },
	{ "idleAnim", 24, 0, },
	{ "emptyIdleAnim", 28, 0, },
	{ "fireAnim", 32, 0, },
	{ "holdFireAnim", 36, 0, },
	{ "lastShotAnim", 40, 0, },
	{ "rechamberAnim", 44, 0, },
	{ "meleeAnim", 48, 0, },
	{ "reloadAnim", 52, 0, },
	{ "reloadEmptyAnim", 56, 0, },
	{ "reloadStartAnim", 60, 0, },
	{ "reloadEndAnim", 64, 0, },
	{ "raiseAnim", 68, 0, },
	{ "dropAnim", 72, 0, },
	{ "altRaiseAnim", 76, 0, },
	{ "altDropAnim", 80, 0, },
	{ "quickRaiseAnim", 84, 0, },
	{ "quickDropAnim", 88, 0, },
	{ "adsFireAnim", 92, 0, },
	{ "adsLastShotAnim", 96, 0, },
	{ "adsRechamberAnim", 100, 0, },
	{ "adsUpAnim", 104, 0, },
	{ "adsDownAnim", 108, 0, },
	{ "script", 1412, 0, },
	{ "weaponType", 120, 8, },
	{ "weaponClass", 124, 9, },
	{ "weaponSlot", 128, 11, },
	{ "slotStackable", 136, 5, },
	{ "offhandClass", 132, 14, },
	{ "viewFlashEffect", 144, 0, },
	{ "worldFlashEffect", 148, 0, },
	{ "pickupSound", 152, 0, },
	{ "ammoPickupSound", 156, 0, },
	{ "projectileSound", 160, 0, },
	{ "pullbackSound", 164, 0, },
	{ "fireSound", 168, 0, },
	{ "fireSoundPlayer", 172, 0, },
	{ "loopFireSound", 176, 0, },
	{ "loopFireSoundPlayer", 180, 0, },
	{ "stopFireSound", 184, 0, },
	{ "stopFireSoundPlayer", 188, 0, },
	{ "lastShotSound", 192, 0, },
	{ "lastShotSoundPlayer", 196, 0, },
	{ "meleeSwipeSound", 200, 0, },
	{ "rechamberSound", 204, 0, },
	{ "rechamberSoundPlayer", 208, 0, },
	{ "reloadSound", 212, 0, },
	{ "reloadSoundPlayer", 216, 0, },
	{ "reloadEmptySound", 220, 0, },
	{ "reloadEmptySoundPlayer", 224, 0, },
	{ "reloadStartSound", 228, 0, },
	{ "reloadStartSoundPlayer", 232, 0, },
	{ "reloadEndSound", 236, 0, },
	{ "reloadEndSoundPlayer", 240, 0, },
	{ "raiseSound", 244, 0, },
	{ "altSwitchSound", 248, 0, },
	{ "putawaySound", 252, 0, },
	{ "noteTrackSoundA", 256, 0, },
	{ "noteTrackSoundB", 260, 0, },
	{ "noteTrackSoundC", 264, 0, },
	{ "noteTrackSoundD", 268, 0, },
	{ "shellEjectEffect", 272, 0, },
	{ "lastShotEjectEffect", 276, 0, },
	{ "reticleCenter", 280, 0, },
	{ "reticleSide", 284, 0, },
	{ "reticleCenterSize", 288, 4, },
	{ "reticleSideSize", 292, 4, },
	{ "reticleMinOfs", 296, 4, },
	{ "standMoveF", 300, 6, },
	{ "standMoveR", 304, 6, },
	{ "standMoveU", 308, 6, },
	{ "standRotP", 312, 6, },
	{ "standRotY", 316, 6, },
	{ "standRotR", 320, 6, },
	{ "duckedOfsF", 324, 6, },
	{ "duckedOfsR", 328, 6, },
	{ "duckedOfsU", 332, 6, },
	{ "duckedMoveF", 336, 6, },
	{ "duckedMoveR", 340, 6, },
	{ "duckedMoveU", 344, 6, },
	{ "duckedRotP", 348, 6, },
	{ "duckedRotY", 352, 6, },
	{ "duckedRotR", 356, 6, },
	{ "proneOfsF", 360, 6, },
	{ "proneOfsR", 364, 6, },
	{ "proneOfsU", 368, 6, },
	{ "proneMoveF", 372, 6, },
	{ "proneMoveR", 376, 6, },
	{ "proneMoveU", 380, 6, },
	{ "proneRotP", 384, 6, },
	{ "proneRotY", 388, 6, },
	{ "proneRotR", 392, 6, },
	{ "posMoveRate", 396, 6, },
	{ "posProneMoveRate", 400, 6, },
	{ "standMoveMinSpeed", 404, 6, },
	{ "duckedMoveMinSpeed", 408, 6, },
	{ "proneMoveMinSpeed", 412, 6, },
	{ "posRotRate", 416, 6, },
	{ "posProneRotRate", 420, 6, },
	{ "standRotMinSpeed", 424, 6, },
	{ "duckedRotMinSpeed", 428, 6, },
	{ "proneRotMinSpeed", 432, 6, },
	{ "worldModel", 436, 0, },
	{ "hudIcon", 440, 0, },
	{ "modeIcon", 444, 0, },
	{ "startAmmo", 448, 4, },
	{ "ammoName", 452, 0, },
	{ "clipName", 460, 0, },
	{ "maxAmmo", 468, 4, },
	{ "clipSize", 472, 4, },
	{ "shotCount", 476, 4, },
	{ "sharedAmmoCapName", 480, 0, },
	{ "sharedAmmoCap", 488, 4, },
	{ "damage", 492, 4, },
	{ "playerDamage", 496, 4, },
	{ "meleeDamage", 500, 4, },
	{ "minDamage", 1424, 4, },
	{ "minPlayerDamage", 1428, 4, },
	{ "maxDamageRange", 1432, 6, },
	{ "minDamageRange", 1436, 6, },
	{ "destabilizationBaseTime", 1440, 6, },
	{ "destabilizationTimeReductionRatio", 1444, 6, },
	{ "destabilizationAngleMax", 1448, 6, },
	{ "destabilizeDistance", 1452, 4, },
	{ "fireDelay", 508, 7, },
	{ "meleeDelay", 512, 7, },
	{ "fireTime", 516, 7, },
	{ "rechamberTime", 520, 7, },
	{ "rechamberBoltTime", 524, 7, },
	{ "holdFireTime", 528, 7, },
	{ "meleeTime", 532, 7, },
	{ "reloadTime", 536, 7, },
	{ "reloadEmptyTime", 540, 7, },
	{ "reloadAddTime", 544, 7, },
	{ "reloadStartTime", 548, 7, },
	{ "reloadStartAddTime", 552, 7, },
	{ "reloadEndTime", 556, 7, },
	{ "dropTime", 560, 7, },
	{ "raiseTime", 564, 7, },
	{ "altDropTime", 568, 7, },
	{ "altRaiseTime", 572, 7, },
	{ "quickDropTime", 576, 7, },
	{ "quickRaiseTime", 580, 7, },
	{ "fuseTime", 584, 7, },
	{ "autoAimRange", 588, 6, },
	{ "slowdownAimRange", 592, 6, },
	{ "slowdownAimRangeAds", 596, 6, },
	{ "lockonAimRange", 600, 6, },
	{ "lockonAimRangeAds", 604, 6, },
	{ "enemyCrosshairRange", 608, 6, },
	{ "moveSpeedScale", 612, 6, },
	{ "idleCrouchFactor", 728, 6, },
	{ "idleProneFactor", 732, 6, },
	{ "gunMaxPitch", 736, 6, },
	{ "gunMaxYaw", 740, 6, },
	{ "swayMaxAngle", 744, 6, },
	{ "swayLerpSpeed", 748, 6, },
	{ "swayPitchScale", 752, 6, },
	{ "swayYawScale", 756, 6, },
	{ "swayHorizScale", 760, 6, },
	{ "swayVertScale", 764, 6, },
	{ "swayShellShockScale", 768, 6, },
	{ "adsSwayMaxAngle", 772, 6, },
	{ "adsSwayLerpSpeed", 776, 6, },
	{ "adsSwayPitchScale", 780, 6, },
	{ "adsSwayYawScale", 784, 6, },
	{ "adsSwayHorizScale", 788, 6, },
	{ "adsSwayVertScale", 792, 6, },
	{ "rifleBullet", 796, 5, },
	{ "armorPiercing", 800, 5, },
	{ "semiAuto", 804, 5, },
	{ "boltAction", 808, 5, },
	{ "aimDownSight", 812, 5, },
	{ "rechamberWhileAds", 816, 5, },
	{ "adsViewErrorMin", 820, 6, },
	{ "adsViewErrorMax", 824, 6, },
	{ "clipOnly", 832, 5, },
	{ "cookOffHold", 828, 5, },
	{ "wideListIcon", 836, 5, },
	{ "adsFire", 840, 5, },
	{ "killIcon", 844, 0, },
	{ "wideKillIcon", 848, 5, },
	{ "flipKillIcon", 852, 5, },
	{ "noPartialReload", 856, 5, },
	{ "segmentedReload", 860, 5, },
	{ "reloadAmmoAdd", 864, 4, },
	{ "reloadStartAdd", 868, 4, },
	{ "altWeapon", 872, 0, },
	{ "dropAmmoMin", 880, 4, },
	{ "dropAmmoMax", 884, 4, },
	{ "explosionRadius", 888, 4, },
	{ "explosionInnerDamage", 892, 4, },
	{ "explosionOuterDamage", 896, 4, },
	{ "projectileSpeed", 900, 4, },
	{ "projectileSpeedUp", 904, 4, },
	{ "projectileModel", 908, 0, },
	{ "projExplosionType", 912, 13, },
	{ "projExplosionEffect", 916, 0, },
	{ "projExplosionSound", 920, 0, },
	{ "projImpactExplode", 924, 5, },
	{ "parallelDefaultBounce", 928, 6, },
	{ "parallelBarkBounce", 932, 6, },
	{ "parallelBrickBounce", 936, 6, },
	{ "parallelCarpetBounce", 940, 6, },
	{ "parallelClothBounce", 944, 6, },
	{ "parallelConcreteBounce", 948, 6, },
	{ "parallelDirtBounce", 952, 6, },
	{ "parallelFleshBounce", 956, 6, },
	{ "parallelFoliageBounce", 960, 6, },
	{ "parallelGlassBounce", 964, 6, },
	{ "parallelGrassBounce", 968, 6, },
	{ "parallelGravelBounce", 972, 6, },
	{ "parallelIceBounce", 976, 6, },
	{ "parallelMetalBounce", 980, 6, },
	{ "parallelMudBounce", 984, 6, },
	{ "parallelPaperBounce", 988, 6, },
	{ "parallelPlasterBounce", 992, 6, },
	{ "parallelRockBounce", 996, 6, },
	{ "parallelSandBounce", 1000, 6, },
	{ "parallelSnowBounce", 1004, 6, },
	{ "parallelWaterBounce", 1008, 6, },
	{ "parallelWoodBounce", 1012, 6, },
	{ "parallelAsphaltBounce", 1016, 6, },
	{ "perpendicularDefaultBounce", 1020, 6, },
	{ "perpendicularBarkBounce", 1024, 6, },
	{ "perpendicularBrickBounce", 1028, 6, },
	{ "perpendicularCarpetBounce", 1032, 6, },
	{ "perpendicularClothBounce", 1036, 6, },
	{ "perpendicularConcreteBounce", 1040, 6, },
	{ "perpendicularDirtBounce", 1044, 6, },
	{ "perpendicularFleshBounce", 1048, 6, },
	{ "perpendicularFoliageBounce", 1052, 6, },
	{ "perpendicularGlassBounce", 1056, 6, },
	{ "perpendicularGrassBounce", 1060, 6, },
	{ "perpendicularGravelBounce", 1064, 6, },
	{ "perpendicularIceBounce", 1068, 6, },
	{ "perpendicularMetalBounce", 1072, 6, },
	{ "perpendicularMudBounce", 1076, 6, },
	{ "perpendicularPaperBounce", 1080, 6, },
	{ "perpendicularPlasterBounce", 1084, 6, },
	{ "perpendicularRockBounce", 1088, 6, },
	{ "perpendicularSandBounce", 1092, 6, },
	{ "perpendicularSnowBounce", 1096, 6, },
	{ "perpendicularWaterBounce", 1100, 6, },
	{ "perpendicularWoodBounce", 1104, 6, },
	{ "perpendicularAsphaltBounce", 1108, 6, },
	{ "projTrailEffect", 1112, 0, },
	{ "projectileDLight", 1116, 4, },
	{ "projectileRed", 1120, 6, },
	{ "projectileGreen", 1124, 6, },
	{ "projectileBlue", 1128, 6, },
	{ "adsTransInTime", 704, 7, },
	{ "adsTransOutTime", 708, 7, },
	{ "adsIdleAmount", 712, 6, },
	{ "adsIdleSpeed", 720, 6, },
	{ "adsZoomFov", 616, 6, },
	{ "adsZoomInFrac", 620, 6, },
	{ "adsZoomOutFrac", 624, 6, },
	{ "adsOverlayShader", 628, 0, },
	{ "adsOverlayReticle", 632, 10, },
	{ "adsOverlayWidth", 636, 6, },
	{ "adsOverlayHeight", 640, 6, },
	{ "adsBobFactor", 644, 6, },
	{ "adsViewBobMult", 648, 6, },
	{ "adsAimPitch", 1132, 6, },
	{ "adsCrosshairInFrac", 1136, 6, },
	{ "adsCrosshairOutFrac", 1140, 6, },
	{ "adsReloadTransTime", 1316, 7, },
	{ "adsGunKickReducedKickBullets", 1144, 4, },
	{ "adsGunKickReducedKickPercent", 1148, 6, },
	{ "adsGunKickPitchMin", 1152, 6, },
	{ "adsGunKickPitchMax", 1156, 6, },
	{ "adsGunKickYawMin", 1160, 6, },
	{ "adsGunKickYawMax", 1164, 6, },
	{ "adsGunKickAccel", 1168, 6, },
	{ "adsGunKickSpeedMax", 1172, 6, },
	{ "adsGunKickSpeedDecay", 1176, 6, },
	{ "adsGunKickStaticDecay", 1180, 6, },
	{ "adsViewKickPitchMin", 1184, 6, },
	{ "adsViewKickPitchMax", 1188, 6, },
	{ "adsViewKickYawMin", 1192, 6, },
	{ "adsViewKickYawMax", 1196, 6, },
	{ "adsViewKickCenterSpeed", 1200, 6, },
	{ "adsSpread", 1212, 6, },
	{ "hipSpreadStandMin", 652, 6, },
	{ "hipSpreadDuckedMin", 656, 6, },
	{ "hipSpreadProneMin", 660, 6, },
	{ "hipSpreadMax", 664, 6, },
	{ "hipSpreadDuckedMax", 668, 6, },
	{ "hipSpreadProneMax", 672, 6, },
	{ "hipSpreadDecayRate", 676, 6, },
	{ "hipSpreadFireAdd", 680, 6, },
	{ "hipSpreadTurnAdd", 684, 6, },
	{ "hipSpreadMoveAdd", 688, 6, },
	{ "hipSpreadDuckedDecay", 692, 6, },
	{ "hipSpreadProneDecay", 696, 6, },
	{ "hipReticleSidePos", 700, 6, },
	{ "hipIdleAmount", 716, 6, },
	{ "hipIdleSpeed", 724, 6, },
	{ "hipGunKickReducedKickBullets", 1216, 4, },
	{ "hipGunKickReducedKickPercent", 1220, 6, },
	{ "hipGunKickPitchMin", 1224, 6, },
	{ "hipGunKickPitchMax", 1228, 6, },
	{ "hipGunKickYawMin", 1232, 6, },
	{ "hipGunKickYawMax", 1236, 6, },
	{ "hipGunKickAccel", 1240, 6, },
	{ "hipGunKickSpeedMax", 1244, 6, },
	{ "hipGunKickSpeedDecay", 1248, 6, },
	{ "hipGunKickStaticDecay", 1252, 6, },
	{ "hipViewKickPitchMin", 1256, 6, },
	{ "hipViewKickPitchMax", 1260, 6, },
	{ "hipViewKickYawMin", 1264, 6, },
	{ "hipViewKickYawMax", 1268, 6, },
	{ "hipViewKickCenterSpeed", 1272, 6, },
	{ "leftArc", 1320, 6, },
	{ "rightArc", 1324, 6, },
	{ "topArc", 1328, 6, },
	{ "bottomArc", 1332, 6, },
	{ "accuracy", 1336, 6, },
	{ "aiSpread", 1340, 6, },
	{ "playerSpread", 1344, 6, },
	{ "maxVertTurnSpeed", 1356, 6, },
	{ "maxHorTurnSpeed", 1360, 6, },
	{ "minVertTurnSpeed", 1348, 6, },
	{ "minHorTurnSpeed", 1352, 6, },
	{ "pitchConvergenceTime", 1364, 6, },
	{ "yawConvergenceTime", 1368, 6, },
	{ "suppressionTime", 1372, 6, },
	{ "maxRange", 1376, 6, },
	{ "animHorRotateInc", 1380, 6, },
	{ "playerPositionDist", 1384, 6, },
	{ "stance", 140, 12, },
	{ "useHintString", 1388, 0, },
	{ "dropHintString", 1392, 0, },
	{ "horizViewJitter", 1404, 6, },
	{ "vertViewJitter", 1408, 6, },
	{ "fightDist", 1284, 6, },
	{ "maxDist", 1288, 6, },
	{ "aiVsAiAccuracyGraph", 1292, 0, },
	{ "aiVsPlayerAccuracyGraph", 1296, 0, },
	{ "locNone", 1456, 6, },
	{ "locHelmet", 1460, 6, },
	{ "locHead", 1464, 6, },
	{ "locNeck", 1468, 6, },
	{ "locTorsoUpper", 1472, 6, },
	{ "locTorsoLower", 1476, 6, },
	{ "locRightArmUpper", 1480, 6, },
	{ "locRightArmLower", 1488, 6, },
	{ "locRightHand", 1496, 6, },
	{ "locLeftArmUpper", 1484, 6, },
	{ "locLeftArmLower", 1492, 6, },
	{ "locLeftHand", 1500, 6, },
	{ "locRightLegUpper", 1504, 6, },
	{ "locRightLegLower", 1512, 6, },
	{ "locRightFoot", 1520, 6, },
	{ "locLeftLegUpper", 1508, 6, },
	{ "locLeftLegLower", 1516, 6, },
	{ "locLeftFoot", 1524, 6, },
	{ "locGun", 1528, 6, },
	{ "fireRumble", 1532, 0, },
	{ "meleeImpactRumble", 1536, 0, }
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

void BG_InitWeaponString(int index, const char *name)
{
	weaponStrings[index].string = name;
	weaponStrings[index].hash = BG_StringHashValue_Lwr(name);
}

void BG_LoadWeaponStrings()
{
	int index;

	for ( index = 0; index < g_playerAnimTypeNamesCount; ++index )
		BG_InitWeaponString(index, g_playerAnimTypeNames[index]);
}

void BG_InitWeaponStrings()
{
	memset(weaponStrings, 0, sizeof(weaponStrings));
	BG_LoadWeaponStrings();
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
		Com_Error(ERR_DROP, "Projectile speed for WeapType %s must be greater than 0.0", weapon->szDisplayName);

	if ( weapon->destabilizationAngleMax >= 45.0 || weapon->destabilizationAngleMax < 0.0 )
		Com_Error( ERR_DROP, "Destabilization angle for for WeapType %s must be between 0 and 45 degrees", weapon->szDisplayName);

	if ( weapon->destabilizationBaseTime <= 0.0 )
		Com_Error(ERR_DROP, "Destabilization base time for for WeapType %s must be positive", weapon->szDisplayName);

	if ( weapon->destabilizationTimeReductionRatio <= 0.0 )
		Com_Error(ERR_DROP, "Destabilization reduction ratio for for WeapType %s must be positive", weapon->szDisplayName);
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