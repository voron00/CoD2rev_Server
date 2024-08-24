#include "../qcommon/qcommon.h"
#include "bg_public.h"

static int g_playerAnimTypeNamesCount;
static const char *g_playerAnimTypeNames[32];
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
	{ WEF( displayName ), CSPFT_STRING, },
	{ WEF( overlayName ), CSPFT_STRING, },
	{ WEF( modeName ), CSPFT_STRING, },
	{ WEF( playerAnimType ), WFT_ANIMTYPE, },
	{ WEF( gunModel ), CSPFT_STRING, },
	{ WEF( handModel ), CSPFT_STRING, },
	{ WEF( idleAnim ), CSPFT_STRING, },
	{ WEF( emptyIdleAnim ), CSPFT_STRING, },
	{ WEF( fireAnim ), CSPFT_STRING, },
	{ WEF( holdFireAnim ), CSPFT_STRING, },
	{ WEF( lastShotAnim ), CSPFT_STRING, },
	{ WEF( rechamberAnim ), CSPFT_STRING, },
	{ WEF( meleeAnim ), CSPFT_STRING, },
	{ WEF( reloadAnim ), CSPFT_STRING, },
	{ WEF( reloadEmptyAnim ), CSPFT_STRING, },
	{ WEF( reloadStartAnim ), CSPFT_STRING, },
	{ WEF( reloadEndAnim ), CSPFT_STRING, },
	{ WEF( raiseAnim ), CSPFT_STRING, },
	{ WEF( dropAnim ), CSPFT_STRING, },
	{ WEF( altRaiseAnim ), CSPFT_STRING, },
	{ WEF( altDropAnim ), CSPFT_STRING, },
	{ WEF( quickRaiseAnim ), CSPFT_STRING, },
	{ WEF( quickDropAnim ), CSPFT_STRING, },
	{ WEF( adsFireAnim ), CSPFT_STRING, },
	{ WEF( adsLastShotAnim ), CSPFT_STRING, },
	{ WEF( adsRechamberAnim ), CSPFT_STRING, },
	{ WEF( adsUpAnim ), CSPFT_STRING, },
	{ WEF( adsDownAnim ), CSPFT_STRING, },
	{ WEF( script ), CSPFT_STRING, },
	{ WEF( weaponType ), WFT_WEAPONTYPE, },
	{ WEF( weaponClass ), WFT_WEAPONCLASS, },
	{ WEF( weaponSlot ), WFT_SLOT, },
	{ WEF( slotStackable ), CSPFT_QBOOLEAN, },
	{ WEF( offhandClass ), WFT_OFFHAND_CLASS, },
	{ WEF( viewFlashEffect ), CSPFT_STRING, },
	{ WEF( worldFlashEffect ), CSPFT_STRING, },
	{ WEF( pickupSound ), CSPFT_STRING, },
	{ WEF( ammoPickupSound ), CSPFT_STRING, },
	{ WEF( projectileSound ), CSPFT_STRING, },
	{ WEF( pullbackSound ), CSPFT_STRING, },
	{ WEF( fireSound ), CSPFT_STRING, },
	{ WEF( fireSoundPlayer ), CSPFT_STRING, },
	{ WEF( loopFireSound ), CSPFT_STRING, },
	{ WEF( loopFireSoundPlayer ), CSPFT_STRING, },
	{ WEF( stopFireSound ), CSPFT_STRING, },
	{ WEF( stopFireSoundPlayer ), CSPFT_STRING, },
	{ WEF( lastShotSound ), CSPFT_STRING, },
	{ WEF( lastShotSoundPlayer ), CSPFT_STRING, },
	{ WEF( meleeSwipeSound ), CSPFT_STRING, },
	{ WEF( rechamberSound ), CSPFT_STRING, },
	{ WEF( rechamberSoundPlayer ), CSPFT_STRING, },
	{ WEF( reloadSound ), CSPFT_STRING, },
	{ WEF( reloadSoundPlayer ), CSPFT_STRING, },
	{ WEF( reloadEmptySound ), CSPFT_STRING, },
	{ WEF( reloadEmptySoundPlayer ), CSPFT_STRING, },
	{ WEF( reloadStartSound ), CSPFT_STRING, },
	{ WEF( reloadStartSoundPlayer ), CSPFT_STRING, },
	{ WEF( reloadEndSound ), CSPFT_STRING, },
	{ WEF( reloadEndSoundPlayer ), CSPFT_STRING, },
	{ WEF( raiseSound ), CSPFT_STRING, },
	{ WEF( altSwitchSound ), CSPFT_STRING, },
	{ WEF( putawaySound ), CSPFT_STRING, },
	{ WEF( noteTrackSoundA ), CSPFT_STRING, },
	{ WEF( noteTrackSoundB ), CSPFT_STRING, },
	{ WEF( noteTrackSoundC ), CSPFT_STRING, },
	{ WEF( noteTrackSoundD ), CSPFT_STRING, },
	{ WEF( shellEjectEffect ), CSPFT_STRING, },
	{ WEF( lastShotEjectEffect ), CSPFT_STRING, },
	{ WEF( reticleCenter ), CSPFT_STRING, },
	{ WEF( reticleSide ), CSPFT_STRING, },
	{ WEF( reticleCenterSize ), CSPFT_INT, },
	{ WEF( reticleSideSize ), CSPFT_INT, },
	{ WEF( reticleMinOfs ), CSPFT_INT, },
	{ WEF( standMoveF ), CSPFT_FLOAT, },
	{ WEF( standMoveR ), CSPFT_FLOAT, },
	{ WEF( standMoveU ), CSPFT_FLOAT, },
	{ WEF( standRotP ), CSPFT_FLOAT, },
	{ WEF( standRotY ), CSPFT_FLOAT, },
	{ WEF( standRotR ), CSPFT_FLOAT, },
	{ WEF( duckedOfsF ), CSPFT_FLOAT, },
	{ WEF( duckedOfsR ), CSPFT_FLOAT, },
	{ WEF( duckedOfsU ), CSPFT_FLOAT, },
	{ WEF( duckedMoveF ), CSPFT_FLOAT, },
	{ WEF( duckedMoveR ), CSPFT_FLOAT, },
	{ WEF( duckedMoveU ), CSPFT_FLOAT, },
	{ WEF( duckedRotP ), CSPFT_FLOAT, },
	{ WEF( duckedRotY ), CSPFT_FLOAT, },
	{ WEF( duckedRotR ), CSPFT_FLOAT, },
	{ WEF( proneOfsF ), CSPFT_FLOAT, },
	{ WEF( proneOfsR ), CSPFT_FLOAT, },
	{ WEF( proneOfsU ), CSPFT_FLOAT, },
	{ WEF( proneMoveF ), CSPFT_FLOAT, },
	{ WEF( proneMoveR ), CSPFT_FLOAT, },
	{ WEF( proneMoveU ), CSPFT_FLOAT, },
	{ WEF( proneRotP ), CSPFT_FLOAT, },
	{ WEF( proneRotY ), CSPFT_FLOAT, },
	{ WEF( proneRotR ), CSPFT_FLOAT, },
	{ WEF( posMoveRate ), CSPFT_FLOAT, },
	{ WEF( posProneMoveRate ), CSPFT_FLOAT, },
	{ WEF( standMoveMinSpeed ), CSPFT_FLOAT, },
	{ WEF( duckedMoveMinSpeed ), CSPFT_FLOAT, },
	{ WEF( proneMoveMinSpeed ), CSPFT_FLOAT, },
	{ WEF( posRotRate ), CSPFT_FLOAT, },
	{ WEF( posProneRotRate ), CSPFT_FLOAT, },
	{ WEF( standRotMinSpeed ), CSPFT_FLOAT, },
	{ WEF( duckedRotMinSpeed ), CSPFT_FLOAT, },
	{ WEF( proneRotMinSpeed ), CSPFT_FLOAT, },
	{ WEF( worldModel ), CSPFT_STRING, },
	{ WEF( hudIcon ), CSPFT_STRING, },
	{ WEF( modeIcon ), CSPFT_STRING, },
	{ WEF( startAmmo ), CSPFT_INT, },
	{ WEF( ammoName ), CSPFT_STRING, },
	{ WEF( clipName ), CSPFT_STRING, },
	{ WEF( maxAmmo ), CSPFT_INT, },
	{ WEF( clipSize ), CSPFT_INT, },
	{ WEF( shotCount ), CSPFT_INT, },
	{ WEF( sharedAmmoCapName ), CSPFT_STRING, },
	{ WEF( sharedAmmoCap ), CSPFT_INT, },
	{ WEF( damage ), CSPFT_INT, },
	{ WEF( playerDamage ), CSPFT_INT, },
	{ WEF( meleeDamage ), CSPFT_INT, },
	{ WEF( minDamage ), CSPFT_INT, },
	{ WEF( minPlayerDamage ), CSPFT_INT, },
	{ WEF( maxDamageRange ), CSPFT_FLOAT, },
	{ WEF( minDamageRange ), CSPFT_FLOAT, },
	{ WEF( destabilizationBaseTime ), CSPFT_FLOAT, },
	{ WEF( destabilizationTimeReductionRatio ), CSPFT_FLOAT, },
	{ WEF( destabilizationAngleMax ), CSPFT_FLOAT, },
	{ WEF( destabilizeDistance ), CSPFT_INT, },
	{ WEF( fireDelay ), CSPFT_MILLISECONDS, },
	{ WEF( meleeDelay ), CSPFT_MILLISECONDS, },
	{ WEF( fireTime ), CSPFT_MILLISECONDS, },
	{ WEF( rechamberTime ), CSPFT_MILLISECONDS, },
	{ WEF( rechamberBoltTime ), CSPFT_MILLISECONDS, },
	{ WEF( holdFireTime ), CSPFT_MILLISECONDS, },
	{ WEF( meleeTime ), CSPFT_MILLISECONDS, },
	{ WEF( reloadTime ), CSPFT_MILLISECONDS, },
	{ WEF( reloadEmptyTime ), CSPFT_MILLISECONDS, },
	{ WEF( reloadAddTime ), CSPFT_MILLISECONDS, },
	{ WEF( reloadStartTime ), CSPFT_MILLISECONDS, },
	{ WEF( reloadStartAddTime ), CSPFT_MILLISECONDS, },
	{ WEF( reloadEndTime ), CSPFT_MILLISECONDS, },
	{ WEF( dropTime ), CSPFT_MILLISECONDS, },
	{ WEF( raiseTime ), CSPFT_MILLISECONDS, },
	{ WEF( altDropTime ), CSPFT_MILLISECONDS, },
	{ WEF( altRaiseTime ), CSPFT_MILLISECONDS, },
	{ WEF( quickDropTime ), CSPFT_MILLISECONDS, },
	{ WEF( quickRaiseTime ), CSPFT_MILLISECONDS, },
	{ WEF( fuseTime ), CSPFT_MILLISECONDS, },
	{ WEF( autoAimRange ), CSPFT_FLOAT, },
	{ WEF( slowdownAimRange ), CSPFT_FLOAT, },
	{ WEF( slowdownAimRangeAds ), CSPFT_FLOAT, },
	{ WEF( lockonAimRange ), CSPFT_FLOAT, },
	{ WEF( lockonAimRangeAds ), CSPFT_FLOAT, },
	{ WEF( enemyCrosshairRange ), CSPFT_FLOAT, },
	{ WEF( moveSpeedScale ), CSPFT_FLOAT, },
	{ WEF( idleCrouchFactor ), CSPFT_FLOAT, },
	{ WEF( idleProneFactor ), CSPFT_FLOAT, },
	{ WEF( gunMaxPitch ), CSPFT_FLOAT, },
	{ WEF( gunMaxYaw ), CSPFT_FLOAT, },
	{ WEF( swayMaxAngle ), CSPFT_FLOAT, },
	{ WEF( swayLerpSpeed ), CSPFT_FLOAT, },
	{ WEF( swayPitchScale ), CSPFT_FLOAT, },
	{ WEF( swayYawScale ), CSPFT_FLOAT, },
	{ WEF( swayHorizScale ), CSPFT_FLOAT, },
	{ WEF( swayVertScale ), CSPFT_FLOAT, },
	{ WEF( swayShellShockScale ), CSPFT_FLOAT, },
	{ WEF( adsSwayMaxAngle ), CSPFT_FLOAT, },
	{ WEF( adsSwayLerpSpeed ), CSPFT_FLOAT, },
	{ WEF( adsSwayPitchScale ), CSPFT_FLOAT, },
	{ WEF( adsSwayYawScale ), CSPFT_FLOAT, },
	{ WEF( adsSwayHorizScale ), CSPFT_FLOAT, },
	{ WEF( adsSwayVertScale ), CSPFT_FLOAT, },
	{ WEF( rifleBullet ), CSPFT_QBOOLEAN, },
	{ WEF( armorPiercing ), CSPFT_QBOOLEAN, },
	{ WEF( semiAuto ), CSPFT_QBOOLEAN, },
	{ WEF( boltAction ), CSPFT_QBOOLEAN, },
	{ WEF( aimDownSight ), CSPFT_QBOOLEAN, },
	{ WEF( rechamberWhileAds ), CSPFT_QBOOLEAN, },
	{ WEF( adsViewErrorMin ), CSPFT_FLOAT, },
	{ WEF( adsViewErrorMax ), CSPFT_FLOAT, },
	{ WEF( clipOnly ), CSPFT_QBOOLEAN, },
	{ WEF( cookOffHold ), CSPFT_QBOOLEAN, },
	{ WEF( wideListIcon ), CSPFT_QBOOLEAN, },
	{ WEF( adsFire ), CSPFT_QBOOLEAN, },
	{ WEF( killIcon ), CSPFT_STRING, },
	{ WEF( wideKillIcon ), CSPFT_QBOOLEAN, },
	{ WEF( flipKillIcon ), CSPFT_QBOOLEAN, },
	{ WEF( noPartialReload ), CSPFT_QBOOLEAN, },
	{ WEF( segmentedReload ), CSPFT_QBOOLEAN, },
	{ WEF( reloadAmmoAdd ), CSPFT_INT, },
	{ WEF( reloadStartAdd ), CSPFT_INT, },
	{ WEF( altWeapon ), CSPFT_STRING, },
	{ WEF( dropAmmoMin ), CSPFT_INT, },
	{ WEF( dropAmmoMax ), CSPFT_INT, },
	{ WEF( explosionRadius ), CSPFT_INT, },
	{ WEF( explosionInnerDamage ), CSPFT_INT, },
	{ WEF( explosionOuterDamage ), CSPFT_INT, },
	{ WEF( projectileSpeed ), CSPFT_INT, },
	{ WEF( projectileSpeedUp ), CSPFT_INT, },
	{ WEF( projectileModel ), CSPFT_STRING, },
	{ WEF( projExplosionType ), WFT_PROJ_EXPLOSION, },
	{ WEF( projExplosionEffect ), CSPFT_STRING, },
	{ WEF( projExplosionSound ), CSPFT_STRING, },
	{ WEF( projImpactExplode ), CSPFT_QBOOLEAN, },
	{ WEF( parallelDefaultBounce ), CSPFT_FLOAT, },
	{ WEF( parallelBarkBounce ), CSPFT_FLOAT, },
	{ WEF( parallelBrickBounce ), CSPFT_FLOAT, },
	{ WEF( parallelCarpetBounce ), CSPFT_FLOAT, },
	{ WEF( parallelClothBounce ), CSPFT_FLOAT, },
	{ WEF( parallelConcreteBounce ), CSPFT_FLOAT, },
	{ WEF( parallelDirtBounce ), CSPFT_FLOAT, },
	{ WEF( parallelFleshBounce ), CSPFT_FLOAT, },
	{ WEF( parallelFoliageBounce ), CSPFT_FLOAT, },
	{ WEF( parallelGlassBounce ), CSPFT_FLOAT, },
	{ WEF( parallelGrassBounce ), CSPFT_FLOAT, },
	{ WEF( parallelGravelBounce ), CSPFT_FLOAT, },
	{ WEF( parallelIceBounce ), CSPFT_FLOAT, },
	{ WEF( parallelMetalBounce ), CSPFT_FLOAT, },
	{ WEF( parallelMudBounce ), CSPFT_FLOAT, },
	{ WEF( parallelPaperBounce ), CSPFT_FLOAT, },
	{ WEF( parallelPlasterBounce ), CSPFT_FLOAT, },
	{ WEF( parallelRockBounce ), CSPFT_FLOAT, },
	{ WEF( parallelSandBounce ), CSPFT_FLOAT, },
	{ WEF( parallelSnowBounce ), CSPFT_FLOAT, },
	{ WEF( parallelWaterBounce ), CSPFT_FLOAT, },
	{ WEF( parallelWoodBounce ), CSPFT_FLOAT, },
	{ WEF( parallelAsphaltBounce ), CSPFT_FLOAT, },
	{ WEF( perpendicularDefaultBounce ), CSPFT_FLOAT, },
	{ WEF( perpendicularBarkBounce ), CSPFT_FLOAT, },
	{ WEF( perpendicularBrickBounce ), CSPFT_FLOAT, },
	{ WEF( perpendicularCarpetBounce ), CSPFT_FLOAT, },
	{ WEF( perpendicularClothBounce ), CSPFT_FLOAT, },
	{ WEF( perpendicularConcreteBounce ), CSPFT_FLOAT, },
	{ WEF( perpendicularDirtBounce ), CSPFT_FLOAT, },
	{ WEF( perpendicularFleshBounce ), CSPFT_FLOAT, },
	{ WEF( perpendicularFoliageBounce ), CSPFT_FLOAT, },
	{ WEF( perpendicularGlassBounce ), CSPFT_FLOAT, },
	{ WEF( perpendicularGrassBounce ), CSPFT_FLOAT, },
	{ WEF( perpendicularGravelBounce ), CSPFT_FLOAT, },
	{ WEF( perpendicularIceBounce ), CSPFT_FLOAT, },
	{ WEF( perpendicularMetalBounce ), CSPFT_FLOAT, },
	{ WEF( perpendicularMudBounce ), CSPFT_FLOAT, },
	{ WEF( perpendicularPaperBounce ), CSPFT_FLOAT, },
	{ WEF( perpendicularPlasterBounce ), CSPFT_FLOAT, },
	{ WEF( perpendicularRockBounce ), CSPFT_FLOAT, },
	{ WEF( perpendicularSandBounce ), CSPFT_FLOAT, },
	{ WEF( perpendicularSnowBounce ), CSPFT_FLOAT, },
	{ WEF( perpendicularWaterBounce ), CSPFT_FLOAT, },
	{ WEF( perpendicularWoodBounce ), CSPFT_FLOAT, },
	{ WEF( perpendicularAsphaltBounce ), CSPFT_FLOAT, },
	{ WEF( projTrailEffect ), CSPFT_STRING, },
	{ WEF( projectileDLight ), CSPFT_INT, },
	{ WEF( projectileRed ), CSPFT_FLOAT, },
	{ WEF( projectileGreen ), CSPFT_FLOAT, },
	{ WEF( projectileBlue ), CSPFT_FLOAT, },
	{ WEF( adsTransInTime ), CSPFT_MILLISECONDS, },
	{ WEF( adsTransOutTime ), CSPFT_MILLISECONDS, },
	{ WEF( adsIdleAmount ), CSPFT_FLOAT, },
	{ WEF( adsIdleSpeed ), CSPFT_FLOAT, },
	{ WEF( adsZoomFov ), CSPFT_FLOAT, },
	{ WEF( adsZoomInFrac ), CSPFT_FLOAT, },
	{ WEF( adsZoomOutFrac ), CSPFT_FLOAT, },
	{ WEF( adsOverlayShader ), CSPFT_STRING, },
	{ WEF( adsOverlayReticle ), WFT_OVERLAYRETICLE, },
	{ WEF( adsOverlayWidth ), CSPFT_FLOAT, },
	{ WEF( adsOverlayHeight ), CSPFT_FLOAT, },
	{ WEF( adsBobFactor ), CSPFT_FLOAT, },
	{ WEF( adsViewBobMult ), CSPFT_FLOAT, },
	{ WEF( adsAimPitch ), CSPFT_FLOAT, },
	{ WEF( adsCrosshairInFrac ), CSPFT_FLOAT, },
	{ WEF( adsCrosshairOutFrac ), CSPFT_FLOAT, },
	{ WEF( adsReloadTransTime ), CSPFT_MILLISECONDS, },
	{ WEF( adsGunKickReducedKickBullets ), CSPFT_INT, },
	{ WEF( adsGunKickReducedKickPercent ), CSPFT_FLOAT, },
	{ WEF( adsGunKickPitchMin ), CSPFT_FLOAT, },
	{ WEF( adsGunKickPitchMax ), CSPFT_FLOAT, },
	{ WEF( adsGunKickYawMin ), CSPFT_FLOAT, },
	{ WEF( adsGunKickYawMax ), CSPFT_FLOAT, },
	{ WEF( adsGunKickAccel ), CSPFT_FLOAT, },
	{ WEF( adsGunKickSpeedMax ), CSPFT_FLOAT, },
	{ WEF( adsGunKickSpeedDecay ), CSPFT_FLOAT, },
	{ WEF( adsGunKickStaticDecay ), CSPFT_FLOAT, },
	{ WEF( adsViewKickPitchMin ), CSPFT_FLOAT, },
	{ WEF( adsViewKickPitchMax ), CSPFT_FLOAT, },
	{ WEF( adsViewKickYawMin ), CSPFT_FLOAT, },
	{ WEF( adsViewKickYawMax ), CSPFT_FLOAT, },
	{ WEF( adsViewKickCenterSpeed ), CSPFT_FLOAT, },
	{ WEF( adsSpread ), CSPFT_FLOAT, },
	{ WEF( hipSpreadStandMin ), CSPFT_FLOAT, },
	{ WEF( hipSpreadDuckedMin ), CSPFT_FLOAT, },
	{ WEF( hipSpreadProneMin ), CSPFT_FLOAT, },
	{ WEF( hipSpreadMax ), CSPFT_FLOAT, },
	{ WEF( hipSpreadDuckedMax ), CSPFT_FLOAT, },
	{ WEF( hipSpreadProneMax ), CSPFT_FLOAT, },
	{ WEF( hipSpreadDecayRate ), CSPFT_FLOAT, },
	{ WEF( hipSpreadFireAdd ), CSPFT_FLOAT, },
	{ WEF( hipSpreadTurnAdd ), CSPFT_FLOAT, },
	{ WEF( hipSpreadMoveAdd ), CSPFT_FLOAT, },
	{ WEF( hipSpreadDuckedDecay ), CSPFT_FLOAT, },
	{ WEF( hipSpreadProneDecay ), CSPFT_FLOAT, },
	{ WEF( hipReticleSidePos ), CSPFT_FLOAT, },
	{ WEF( hipIdleAmount ), CSPFT_FLOAT, },
	{ WEF( hipIdleSpeed ), CSPFT_FLOAT, },
	{ WEF( hipGunKickReducedKickBullets ), CSPFT_INT, },
	{ WEF( hipGunKickReducedKickPercent ), CSPFT_FLOAT, },
	{ WEF( hipGunKickPitchMin ), CSPFT_FLOAT, },
	{ WEF( hipGunKickPitchMax ), CSPFT_FLOAT, },
	{ WEF( hipGunKickYawMin ), CSPFT_FLOAT, },
	{ WEF( hipGunKickYawMax ), CSPFT_FLOAT, },
	{ WEF( hipGunKickAccel ), CSPFT_FLOAT, },
	{ WEF( hipGunKickSpeedMax ), CSPFT_FLOAT, },
	{ WEF( hipGunKickSpeedDecay ), CSPFT_FLOAT, },
	{ WEF( hipGunKickStaticDecay ), CSPFT_FLOAT, },
	{ WEF( hipViewKickPitchMin ), CSPFT_FLOAT, },
	{ WEF( hipViewKickPitchMax ), CSPFT_FLOAT, },
	{ WEF( hipViewKickYawMin ), CSPFT_FLOAT, },
	{ WEF( hipViewKickYawMax ), CSPFT_FLOAT, },
	{ WEF( hipViewKickCenterSpeed ), CSPFT_FLOAT, },
	{ WEF( leftArc ), CSPFT_FLOAT, },
	{ WEF( rightArc ), CSPFT_FLOAT, },
	{ WEF( topArc ), CSPFT_FLOAT, },
	{ WEF( bottomArc ), CSPFT_FLOAT, },
	{ WEF( accuracy ), CSPFT_FLOAT, },
	{ WEF( aiSpread ), CSPFT_FLOAT, },
	{ WEF( playerSpread ), CSPFT_FLOAT, },
	{ WEF( maxVertTurnSpeed ), CSPFT_FLOAT, },
	{ WEF( maxHorTurnSpeed ), CSPFT_FLOAT, },
	{ WEF( minVertTurnSpeed ), CSPFT_FLOAT, },
	{ WEF( minHorTurnSpeed ), CSPFT_FLOAT, },
	{ WEF( pitchConvergenceTime ), CSPFT_FLOAT, },
	{ WEF( yawConvergenceTime ), CSPFT_FLOAT, },
	{ WEF( suppressionTime ), CSPFT_FLOAT, },
	{ WEF( maxRange ), CSPFT_FLOAT, },
	{ WEF( animHorRotateInc ), CSPFT_FLOAT, },
	{ WEF( playerPositionDist ), CSPFT_FLOAT, },
	{ WEF( stance ), WFT_STANCE, },
	{ WEF( useHintString ), CSPFT_STRING, },
	{ WEF( dropHintString ), CSPFT_STRING, },
	{ WEF( horizViewJitter ), CSPFT_FLOAT, },
	{ WEF( vertViewJitter ), CSPFT_FLOAT, },
	{ WEF( fightDist ), CSPFT_FLOAT, },
	{ WEF( maxDist ), CSPFT_FLOAT, },
	{ WEF( aiVsAiAccuracyGraph ), CSPFT_STRING, },
	{ WEF( aiVsPlayerAccuracyGraph ), CSPFT_STRING, },
	{ WEF( locNone ), CSPFT_FLOAT, },
	{ WEF( locHelmet ), CSPFT_FLOAT, },
	{ WEF( locHead ), CSPFT_FLOAT, },
	{ WEF( locNeck ), CSPFT_FLOAT, },
	{ WEF( locTorsoUpper ), CSPFT_FLOAT, },
	{ WEF( locTorsoLower ), CSPFT_FLOAT, },
	{ WEF( locRightArmUpper ), CSPFT_FLOAT, },
	{ WEF( locRightArmLower ), CSPFT_FLOAT, },
	{ WEF( locRightHand ), CSPFT_FLOAT, },
	{ WEF( locLeftArmUpper ), CSPFT_FLOAT, },
	{ WEF( locLeftArmLower ), CSPFT_FLOAT, },
	{ WEF( locLeftHand ), CSPFT_FLOAT, },
	{ WEF( locRightLegUpper ), CSPFT_FLOAT, },
	{ WEF( locRightLegLower ), CSPFT_FLOAT, },
	{ WEF( locRightFoot ), CSPFT_FLOAT, },
	{ WEF( locLeftLegUpper ), CSPFT_FLOAT, },
	{ WEF( locLeftLegLower ), CSPFT_FLOAT, },
	{ WEF( locLeftFoot ), CSPFT_FLOAT, },
	{ WEF( locGun ), CSPFT_FLOAT, },
	{ WEF( fireRumble ), CSPFT_STRING, },
	{ WEF( meleeImpactRumble ), CSPFT_STRING, },
};

/*
===============
BG_GetWeaponSlotNameForIndex
===============
*/
const char *BG_GetWeaponSlotNameForIndex( int iSlot )
{
	assert( iSlot >= SLOT_NONE && iSlot < SLOT_COUNT );
	return szWeapSlotNames[iSlot];
}

/*
===============
BG_GetWeaponSlotForName
===============
*/
int BG_GetWeaponSlotForName( const char *name )
{
	int slot;

	for ( slot = SLOT_NONE; slot < SLOT_COUNT; slot++ )
	{
		if ( !strcasecmp(name, szWeapSlotNames[slot]) )
		{
			return slot;
		}
	}

	return SLOT_NONE;
}

/*
===============
BG_GetWeaponTypeName
===============
*/
const char *BG_GetWeaponTypeName( int weapType )
{
	return szWeapTypeNames[weapType];
}

/*
===============
BG_LoadWeaponStrings
===============
*/
void BG_LoadWeaponStrings()
{
	for ( int i = 0; i < g_playerAnimTypeNamesCount; i++ )
	{
		BG_InitWeaponString(i, g_playerAnimTypeNames[i]);
	}
}

/*
===============
BG_LoadPlayerAnimTypes
===============
*/
void BG_LoadPlayerAnimTypes()
{
	int len;
	fileHandle_t f;
	char buf[4096];
	const char *token;
	const char *buftrav;

	g_playerAnimTypeNamesCount = 0;

	len = FS_FOpenFileByMode(globalTypesFilename, &f, FS_READ);

	if ( len < 0 )
	{
		Com_Error(ERR_DROP, "Couldn't load file ", globalTypesFilename);
	}

	if ( len >= sizeof(buf) )
	{
		FS_FCloseFile(f);
		Com_Error(ERR_DROP, "%s max size exceeded", globalTypesFilename);
	}

	FS_Read(buf, len, f);
	buf[len] = 0;

	FS_FCloseFile(f);
	buftrav = buf;

	Com_BeginParseSession("BG_AnimParseAnimScript");

	while ( 1 )
	{
		token = Com_Parse(&buftrav);

		if ( !token || !token[0] )
		{
			break;
		}

		if ( g_playerAnimTypeNamesCount >= 64 )
		{
			Com_Error(ERR_DROP, "Player anim type array size exceeded");
		}

		g_playerAnimTypeNames[g_playerAnimTypeNamesCount] = (const char *)Hunk_AllocLowInternal(strlen(token) + 1);
		strcpy((char *)g_playerAnimTypeNames[g_playerAnimTypeNamesCount], token);
		g_playerAnimTypeNamesCount++;
	}

	Com_EndParseSession();
}

/*
===============
InitWeaponDef
===============
*/
static void InitWeaponDef( WeaponDef *weapDef )
{
	const cspField_t *pField;
	int iField;

	weapDef->szInternalName = "";

	for ( iField = 0, pField = weaponDefFields; iField < ARRAY_COUNT(weaponDefFields); iField++, pField++ )
	{
		if ( pField->iFieldType == CSPFT_STRING )
		{
			*(const char **)((char *)&weapDef->szInternalName + pField->iOffset) = "";
		}
	}
}

/*
===============
Weapon_GetStringArrayIndex
===============
*/
static int Weapon_GetStringArrayIndex( const char *value, const char **stringArray, int arraySize )
{
	assert(value);
	assert(stringArray);

	for ( int arrayIndex = 0; arrayIndex < arraySize; arrayIndex++ )
	{
		if ( !strcasecmp(value, stringArray[arrayIndex]) )
		{
			return arrayIndex;
		}
	}

	return -1;
}

/*
===============
BG_ParseWeaponDefSpecificFieldType
===============
*/
static qboolean BG_ParseWeaponDefSpecificFieldType( unsigned char *pStruct, const char *pValue, int iFieldType )
{
	int index;
	WeaponDef *weapDef;

	assert(pStruct);
	assert(pValue);

	weapDef = (WeaponDef *)pStruct;

	switch ( iFieldType )
	{
	case WFT_WEAPONTYPE:
		index = Weapon_GetStringArrayIndex(pValue, szWeapTypeNames, ARRAY_COUNT(szWeapTypeNames));
		if ( index < 0 )
			Com_Error(ERR_DROP, "Unknown weapon type \"%s\" in \"%s\"\n", pValue, weapDef->szInternalName);
		weapDef->weaponType = index;
		break;

	case WFT_WEAPONCLASS:
		index = Weapon_GetStringArrayIndex(pValue, szWeapClassNames, ARRAY_COUNT(szWeapClassNames));
		if ( index < 0 )
			Com_Error(ERR_DROP, "Unknown weapon class \"%s\" in \"%s\"\n", pValue, weapDef->szInternalName);
		weapDef->weaponClass = index;
		break;

	case WFT_OVERLAYRETICLE:
		index = Weapon_GetStringArrayIndex(pValue, szWeapOverlayReticleNames, ARRAY_COUNT(szWeapOverlayReticleNames));
		if ( index < 0 )
			Com_Error(ERR_DROP, "Unknown weapon overlay reticle \"%s\" in \"%s\"\n", pValue, weapDef->szInternalName);
		weapDef->adsOverlayReticle = index;
		break;

	case WFT_SLOT:
		index = Weapon_GetStringArrayIndex(pValue, szWeapSlotNames, ARRAY_COUNT(szWeapSlotNames));
		if ( index < 0 )
			Com_Error(ERR_DROP, "Unknown weapon slot \"%s\" in \"%s\"\n", pValue, weapDef->szInternalName);
		weapDef->weaponSlot = index;
		break;

	case WFT_STANCE:
		index = Weapon_GetStringArrayIndex(pValue, szWeapStanceNames, ARRAY_COUNT(szWeapStanceNames));
		if ( index < 0 )
			Com_Error(ERR_DROP, "Unknown weapon stance \"%s\" in \"%s\"\n", pValue, weapDef->szInternalName);
		weapDef->stance = index;
		break;

	case WFT_PROJ_EXPLOSION:
		index = Weapon_GetStringArrayIndex(pValue, szProjectileExplosionNames, ARRAY_COUNT(szProjectileExplosionNames));
		if ( index < 0 )
			Com_Error(ERR_DROP, "Unknown projectile explosion \"%s\" in \"%s\"\n", pValue, weapDef->szInternalName);
		weapDef->projExplosionType = index;
		break;

	case WFT_OFFHAND_CLASS:
		index = Weapon_GetStringArrayIndex(pValue, offhandClassNames, ARRAY_COUNT(offhandClassNames));
		if ( index < 0 )
			Com_Error(ERR_DROP, "Unknown offhand class \"%s\" in \"%s\"\n", pValue, weapDef->szInternalName);
		weapDef->offhandClass = index;
		break;

	case WFT_ANIMTYPE:
		index = Weapon_GetStringArrayIndex(pValue, g_playerAnimTypeNames, g_playerAnimTypeNamesCount);
		if ( index < 0 )
			Com_Error(ERR_DROP, "Unknown playerAnimType \"%s\" in \"%s\"\n", pValue, weapDef->szInternalName);
		weapDef->playerAnimType = index;
		break;

	default:
		Com_Error(ERR_DROP, "Bad field type %i in %s\n", iFieldType, weapDef->szInternalName);
		return qfalse;
	}

	return qtrue;
}

/*
===============
BG_SetupTransitionTimes
===============
*/
static void BG_SetupTransitionTimes( WeaponDef *weapDef )
{
	if ( weapDef->adsTransInTime > 0 )
		weapDef->OOPosAnimLength[0] = 1.0 / weapDef->adsTransInTime;
	else
		weapDef->OOPosAnimLength[0] = 0.0033333334;

	if ( weapDef->adsTransOutTime > 0 )
		weapDef->OOPosAnimLength[1] = 1.0 / weapDef->adsTransOutTime;
	else
		weapDef->OOPosAnimLength[1] = 0.0020000001;
}

/*
===============
BG_CheckWeaponDamageRanges
===============
*/
static void BG_CheckWeaponDamageRanges( WeaponDef *weapDef )
{
	if ( weapDef->maxDamageRange <= 0.0 )
		weapDef->maxDamageRange = 999999.0;

	if ( weapDef->minDamageRange <= 0.0 )
		weapDef->minDamageRange = 999999.12;
}

/*
===============
BG_CheckProjectileValues
===============
*/
static void BG_CheckProjectileValues( WeaponDef *weapDef )
{
	assert(weapDef->weaponType == WEAPTYPE_PROJECTILE);

	if ( (float)weapDef->projectileSpeed <= 0 )
	{
		Com_Error(ERR_DROP, "Projectile speed for WeapType %s must be greater than 0.0", weapDef->displayName);
	}

	if ( weapDef->destabilizationAngleMax >= 45 || weapDef->destabilizationAngleMax < 0 )
	{
		Com_Error(ERR_DROP, "Destabilization angle for for WeapType %s must be between 0 and 45 degrees", weapDef->displayName);
	}

	if ( weapDef->destabilizationBaseTime <= 0 )
	{
		Com_Error(ERR_DROP, "Destabilization base time for for WeapType %s must be positive", weapDef->displayName);
	}

	if ( weapDef->destabilizationTimeReductionRatio <= 0 )
	{
		Com_Error(ERR_DROP, "Destabilization reduction ratio for for WeapType %s must be positive", weapDef->displayName);
	}
}

/*
===============
BG_LoadDefaultWeaponDef
===============
*/
WeaponDef bg_defaultWeaponDefs;
WeaponDef *BG_LoadDefaultWeaponDef()
{
	InitWeaponDef(&bg_defaultWeaponDefs);

	bg_defaultWeaponDefs.szInternalName = "none";

	bg_defaultWeaponDefs.accuracyGraphName[0] = "noweapon.accu";
	bg_defaultWeaponDefs.accuracyGraphName[1] = "noweapon.accu";

	G_ParseWeaponAccurayGraphs(&bg_defaultWeaponDefs);

	return &bg_defaultWeaponDefs;
}

/*
===============
BG_LoadWeaponDefInternal
===============
*/
WeaponDef* BG_LoadWeaponDefInternal( const char *folder, const char *name )
{
#define SOURCE_FILE_NAME "WEAPONFILE"
	int sourceLen;
	fileHandle_t f;
	WeaponDef *weapDef;
	char szBuffer[8192];
	char outputName[MAX_QPATH];
	int len;

	sourceLen = strlen(SOURCE_FILE_NAME);
	weapDef = (WeaponDef *)Hunk_AllocLowInternal(sizeof(WeaponDef));

	InitWeaponDef(weapDef);
	Com_sprintf(outputName, sizeof(outputName), "weapons/%s/%s", folder, name);

	len = FS_FOpenFileByMode(outputName, &f, FS_READ);

	if ( len < 0 )
	{
		Com_Printf("^3WARNING: Could not load weapon file '%s'\n", outputName);
		return NULL;
	}

	FS_Read(szBuffer, sourceLen, f);
	szBuffer[sourceLen] = 0;

	if ( strncmp(szBuffer, SOURCE_FILE_NAME, sourceLen) )
	{
		Com_Printf("^3WARNING: \"%s\" does not appear to be a weapon file\n", outputName);
		FS_FCloseFile(f);
		return NULL;
	}

	if ( len - sourceLen >= sizeof(szBuffer) )
	{
		Com_Printf("^3WARNING: \"%s\" Is too long of a weapon file to parse\n", outputName);
		FS_FCloseFile(f);
		return NULL;
	}

	memset(szBuffer, 0, sizeof(szBuffer));

	FS_Read(szBuffer, len - sourceLen, f);
	szBuffer[len - sourceLen] = 0;

	FS_FCloseFile(f);

	if ( !Info_Validate(szBuffer) )
	{
		Com_Printf("^3WARNING: \"%s\" is not a valid weapon file\n", outputName);
		return NULL;
	}

	SetConfigString((char **)&weapDef->szInternalName, name);

	if ( !ParseConfigStringToStruct( (unsigned char *)weapDef, weaponDefFields, ARRAY_COUNT(weaponDefFields), szBuffer, WFT_NUM_FIELD_TYPES, BG_ParseWeaponDefSpecificFieldType, SetConfigString2) )
	{
		return NULL;
	}

	BG_SetupTransitionTimes(weapDef);
	BG_CheckWeaponDamageRanges(weapDef);

	if ( weapDef->enemyCrosshairRange > 15000.0 )
	{
		Com_Error(ERR_DROP, "Enemy crosshair ranges should be less than %f ", 15000.0);
	}

	if ( weapDef->weaponType == WEAPTYPE_PROJECTILE )
	{
		BG_CheckProjectileValues(weapDef);
	}

	if ( !G_ParseWeaponAccurayGraphs(weapDef) )
	{
		return NULL;
	}

	I_strlwr(weapDef->ammoName);
	I_strlwr(weapDef->clipName);

	return weapDef;
}
