#pragma once

#include "../clientscript/clientscript_public.h"
#include "../xanim/xanim_public.h"

#define MAX_ANIMSCRIPT_ANIMCOMMANDS         8

typedef enum
{
	AISTATE_RELAXED,
	AISTATE_QUERY,
	AISTATE_ALERT,
	AISTATE_COMBAT,

	MAX_AISTATES
} aistateEnum_t;

typedef enum
{
	ANIM_MT_UNUSED,
	ANIM_MT_IDLE,
	ANIM_MT_IDLECR,
	ANIM_MT_IDLEPRONE,
	ANIM_MT_WALK,
	ANIM_MT_WALKBK,
	ANIM_MT_WALKCR,
	ANIM_MT_WALKCRBK,
	ANIM_MT_WALKPRONE,
	ANIM_MT_WALKPRONEBK,
	ANIM_MT_RUN,
	ANIM_MT_RUNBK,
	ANIM_MT_RUNCR,
	ANIM_MT_RUNCRBK,
	ANIM_MT_TURNRIGHT,
	ANIM_MT_TURNLEFT,
	ANIM_MT_TURNRIGHTCR,
	ANIM_MT_TURNLEFTCR,
	ANIM_MT_CLIMBUP,
	ANIM_MT_CLIMBDOWN,
	ANIM_MT_MANTLE_ROOT,
	ANIM_MT_MANTLE_UP_57,
	ANIM_MT_MANTLE_UP_51,
	ANIM_MT_MANTLE_UP_45,
	ANIM_MT_MANTLE_UP_39,
	ANIM_MT_MANTLE_UP_33,
	ANIM_MT_MANTLE_UP_27,
	ANIM_MT_MANTLE_UP_21,
	ANIM_MT_MANTLE_OVER_HIGH,
	ANIM_MT_MANTLE_OVER_MID,
	ANIM_MT_MANTLE_OVER_LOW,
	ANIM_MT_FLINCH_FORWARD,
	ANIM_MT_FLINCH_BACKWARD,
	ANIM_MT_FLINCH_LEFT,
	ANIM_MT_FLINCH_RIGHT,
	ANIM_MT_STUMBLE_FORWARD,
	ANIM_MT_STUMBLE_BACKWARD,
	ANIM_MT_STUMBLE_WALK_FORWARD,
	ANIM_MT_STUMBLE_WALK_BACKWARD,
	ANIM_MT_STUMBLE_CROUCH_FORWARD,
	ANIM_MT_STUMBLE_CROUCH_BACKWARD,

	ANIM_MT_SNEAK,
	ANIM_MT_AFTERBATTLE,

	NUM_ANIM_MOVETYPES
} scriptAnimMoveTypes_t;

typedef enum
{
	ANIM_ET_PAIN,
	ANIM_ET_DEATH,
	ANIM_ET_FIREWEAPON,
	ANIM_ET_JUMP,
	ANIM_ET_JUMPBK,
	ANIM_ET_LAND,
	ANIM_ET_DROPWEAPON,
	ANIM_ET_RAISEWEAPON,
	ANIM_ET_CLIMBMOUNT,
	ANIM_ET_CLIMBDISMOUNT,
	ANIM_ET_RELOAD,
	ANIM_ET_CROUCH_TO_PRONE,
	ANIM_ET_PRONE_TO_CROUCH,
	ANIM_ET_STAND_TO_CROUCH,
	ANIM_ET_CROUCH_TO_STAND,
	ANIM_ET_STAND_TO_PRONE,
	ANIM_ET_PRONE_TO_STAND,
	ANIM_ET_MELEEATTACK,
	ANIM_ET_SHELLSHOCK,

	NUM_ANIM_EVENTTYPES
} scriptAnimEventTypes_t;

typedef enum
{
	ANIM_COND_PLAYERANIMTYPE,
	ANIM_COND_WEAPONCLASS,
	ANIM_COND_MOUNTED,
	ANIM_COND_MOVETYPE,
	ANIM_COND_UNDERHAND,
	ANIM_COND_CROUCHING,
	ANIM_COND_FIRING,
	ANIM_COND_WEAPON_POSITION,
	ANIM_COND_STRAFING,

	NUM_ANIM_CONDITIONS
} scriptAnimConditions_t;

typedef struct
{
	const char *string;
	int hash;
} animStringItem_t;

typedef struct animation_s
{
	char name[64];
	int initialLerp;
	float moveSpeed;
	int duration;
	int nameHash;
	int flags;
	int32_t movetype;
	int movestatus;
	int noteType;
} animation_t;
static_assert((sizeof(animation_t) == 0x60), "ERROR: animation_t size is invalid!");

typedef struct
{
	int index;
	int value[2];
} animScriptCondition_t;

typedef struct
{
	short bodyPart[2];
	short animIndex[2];
	unsigned short animDuration[2];
	struct snd_alias_list_t *soundAlias;
} animScriptCommand_t;
static_assert((sizeof(animScriptCommand_t) == 0x10), "ERROR: animScriptCommand_t size is invalid!");

typedef struct
{
	int numConditions;
	animScriptCondition_t conditions[NUM_ANIM_CONDITIONS];
	int numCommands;
	animScriptCommand_t commands[MAX_ANIMSCRIPT_ANIMCOMMANDS];
} animScriptItem_t;
static_assert((sizeof(animScriptItem_t) == 0xF4), "ERROR: animScriptItem_t size is invalid!");

typedef struct
{
	int numItems;
	animScriptItem_t *items[128];
} animScript_t;

typedef struct animScriptData_s
{
	animation_s animations[512];
	unsigned int numAnimations;
	animScript_t scriptAnims[MAX_AISTATES][NUM_ANIM_MOVETYPES];
	animScript_t scriptCannedAnims[MAX_AISTATES][NUM_ANIM_MOVETYPES];
	animScript_t scriptEvents[NUM_ANIM_EVENTTYPES];
	animScriptItem_t scriptItems[2048];
	int numScriptItems;
	scr_animtree_t animTree;
	unsigned short torsoAnim;
	unsigned short legsAnim;
	unsigned short turningAnim;
	unsigned short rootAnim;
	struct snd_alias_list_t *(*soundAlias)(const char *);
	void (*playSoundAlias)(int, struct snd_alias_list_t *);
} animScriptData_t;
static_assert((sizeof(animScriptData_t) == 736200), "ERROR: animScriptData_t size is invalid!");

typedef struct
{
	scr_anim_t anim;
	int nameHash;
	char name[64];
} loadAnim_t;

struct lerpFrame_t
{
	float yawAngle;
	int yawing;
	float pitchAngle;
	int pitching;
	int animationNumber;
	loadAnim_t *animation;
	int animationTime;
	vec3_t oldFramePos;
	float animSpeedScale;
	int oldFrameSnapshotTime;
};

struct clientControllers_t
{
	vec3_t angles[6];
	vec3_t tag_origin_angles;
	vec3_t tag_origin_offset;
};

typedef struct clientInfo_s
{
	int infoValid;
	int nextValid;
	int clientNum;
	char name[32];
	int team;
	int oldteam;
	int score;
	int location;
	int health;
	char model[64];
	char attachModelNames[6][64];
	char attachTagNames[6][64];
	lerpFrame_t legs;
	lerpFrame_t torso;
	float lerpMoveDir;
	float lerpLean;
	vec3_t playerAngles;
	int dobjDirty;
	int weaponState;
	clientControllers_t control;
	int clientConditions[9][2];
	struct XAnimTree_s *pXAnimTree;
	char weaponModel;
	int stanceTransitionTime;
	int turnAnimEndTime;
	char turnAnimType;
} clientInfo_t;
static_assert((sizeof(clientInfo_t) == 0x4B8), "ERROR: clientInfo_t size is invalid!");

struct corpseInfo_t
{
	XAnimTree_s *tree;
	int entnum;
	int time;
	clientInfo_t ci;
	byte falling;
};

struct bgsAnim_t
{
	animScriptData_t animScriptData;
	struct
	{
		scr_animtree_t tree;
		scr_anim_t     root;
		scr_anim_t     torso;
		scr_anim_t     legs;
		scr_anim_t     turning;
	} generic_human;
};

typedef struct __attribute__((aligned(8))) bgs_s
{
	bgsAnim_t animData;
	int time;
	int latestSnapshotTime;
	int frametime;
	int anim_user;
	XModel *(*GetXModel)(const char *);
	void (*CreateDObj)(DObjModel_s *dobjModels, unsigned short numModels, XAnimTree_s *tree, int handle);
	void (*SafeDObjFree)(int);
	void *(*AllocXAnim)(size_t);
	clientInfo_t clientinfo[64];
} bgs_t;
static_assert((sizeof(bgs_t) == 0xC6A00), "ERROR: bgs_t size is invalid!");

struct cspField_t
{
	const char *szName;
	int iOffset;
	int iFieldType;
};

enum weapType_t
{
	WEAPTYPE_BULLET = 0x0,
	WEAPTYPE_GRENADE = 0x1,
	WEAPTYPE_PROJECTILE = 0x2,
	WEAPTYPE_BINOCULARS = 0x3,
	WEAPTYPE_NUM = 0x4
};

enum weapClass_t
{
	WEAPCLASS_RIFLE = 0x0,
	WEAPCLASS_MG = 0x1,
	WEAPCLASS_SMG = 0x2,
	WEAPCLASS_SPREAD = 0x3,
	WEAPCLASS_PISTOL = 0x4,
	WEAPCLASS_GRENADE = 0x5,
	WEAPCLASS_ROCKETLAUNCHER = 0x6,
	WEAPCLASS_TURRET = 0x7,
	WEAPCLASS_NON_PLAYER = 0x8,
	WEAPCLASS_ITEM = 0x9,
	WEAPCLASS_NUM = 0xA
};

enum weapSlot_t
{
	SLOT_NONE = 0x0,
	SLOT_PRIMARY = 0x1,
	SLOT_PRIMARYB = 0x2,
	SLOT_COUNT = 0x3
};

enum OffhandClass_t
{
	OFFHAND_CLASS_NONE = 0x0,
	OFFHAND_CLASS_FRAG_GRENADE = 0x1,
	OFFHAND_CLASS_SMOKE_GRENADE = 0x2,
	OFFHAND_CLASS_COUNT = 0x3
};

enum weapStance_t
{
	WEAPSTANCE_STAND = 0x0,
	WEAPSTANCE_DUCK = 0x1,
	WEAPSTANCE_PRONE = 0x2,
	WEAPSTANCE_NUM = 0x3
};

enum weapOverlayReticle_t
{
	WEAPOVERLAYRETICLE_NONE = 0x0,
	WEAPOVERLAYRETICLE_CROSSHAIR = 0x1,
	WEAPOVERLAYRETICLE_NUM = 0x2
};

enum weapProjExposion_t
{
	WEAPPROJEXP_GRENADE = 0x0,
	WEAPPROJEXP_ROCKET = 0x1,
	WEAPPROJEXP_NONE = 0x2,
	WEAPPROJEXP_NUM = 0x3
};

typedef struct
{
	const char *szInternalName;
	const char *szDisplayName;
	const char *AIOverlayDescription;
	const char *gunModel;
	const char *handModel;
	const char *defaultAnim;
	const char *idleAnim;
	const char *emptyIdleAnim;
	const char *fireAnim;
	const char *holdFireAnim;
	const char *lastShotAnim;
	const char *rechamberAnim;
	const char *meleeAnim;
	const char *reloadAnim;
	const char *reloadEmptyAnim;
	const char *reloadStartAnim;
	const char *reloadEndAnim;
	const char *raiseAnim;
	const char *dropAnim;
	const char *altRaiseAnim;
	const char *altDropAnim;
	const char *quickRaiseAnim;
	const char *quickDropAnim;
	const char *adsFireAnim;
	const char *adsLastShotAnim;
	const char *adsRechamberAnim;
	const char *adsUpAnim;
	const char *adsDownAnim;
	const char *modeName;
	int playerAnimType;
	weapType_t weaponType;
	weapClass_t weaponClass;
	weapSlot_t weaponSlot;
	OffhandClass_t offhandClass;
	int slotStackable;
	weapStance_t stance;
	const char *viewFlashEffect;
	const char *worldFlashEffect;
	const char *pickupSound;
	const char *ammoPickupSound;
	const char *projectileSound;
	const char *pullbackSound;
	const char *fireSound;
	const char *fireSoundPlayer;
	const char *loopFireSound;
	const char *loopFireSoundPlayer;
	const char *stopFireSound;
	const char *stopFireSoundPlayer;
	const char *lastShotSound;
	const char *lastShotSoundPlayer;
	const char *meleeSwipeSound;
	const char *rechamberSound;
	const char *rechamberSoundPlayer;
	const char *reloadSound;
	const char *reloadSoundPlayer;
	const char *reloadEmptySound;
	const char *reloadEmptySoundPlayer;
	const char *reloadStartSound;
	const char *reloadStartSoundPlayer;
	const char *reloadEndSound;
	const char *reloadEndSoundPlayer;
	const char *raiseSound;
	const char *altSwitchSound;
	const char *putawaySound;
	const char *noteTrackSoundA;
	const char *noteTrackSoundB;
	const char *noteTrackSoundC;
	const char *noteTrackSoundD;
	const char *shellEjectEffect;
	const char *lastShotEjectEffect;
	const char *reticleCenter;
	const char *reticleSide;
	int reticleCenterSize;
	int reticleSideSize;
	int reticleMinOfs;
	float standMoveF;
	float standMoveR;
	float standMoveU;
	float standRotP;
	float standRotY;
	float standRotR;
	float duckedOfsF;
	float duckedOfsR;
	float duckedOfsU;
	float duckedMoveF;
	float duckedMoveR;
	float duckedMoveU;
	float duckedRotP;
	float duckedRotY;
	float duckedRotR;
	float proneOfsF;
	float proneOfsR;
	float proneOfsU;
	float proneMoveF;
	float proneMoveR;
	float proneMoveU;
	float proneRotP;
	float proneRotY;
	float proneRotR;
	float posMoveRate;
	float posProneMoveRate;
	float standMoveMinSpeed;
	float duckedMoveMinSpeed;
	float proneMoveMinSpeed;
	float posRotRate;
	float posProneRotRate;
	float standRotMinSpeed;
	float duckedRotMinSpeed;
	float proneRotMinSpeed;
	const char *worldModel;
	const char *hudIcon;
	const char *modeIcon;
	int startAmmo;
	const char *ammoName;
	int ammoIndex;
	const char *clipName;
	int clipIndex;
	int maxAmmo;
	int clipSize;
	int shotCount;
	const char *sharedAmmoCapName;
	int sharedAmmoCapIndex;
	int sharedAmmoCap;
	int damage;
	int playerDamage;
	int meleeDamage;
	int damageType;
	int fireDelay;
	int meleeDelay;
	int fireTime;
	int rechamberTime;
	int rechamberBoltTime;
	int holdFireTime;
	int meleeTime;
	int reloadTime;
	int reloadEmptyTime;
	int reloadAddTime;
	int reloadStartTime;
	int reloadStartAddTime;
	int reloadEndTime;
	int dropTime;
	int raiseTime;
	int altDropTime;
	int altRaiseTime;
	int quickDropTime;
	int quickRaiseTime;
	int fuseTime;
	float autoAimRange;
	float slowdownAimRange;
	float slowdownAimRangeAds;
	float lockonAimRange;
	float lockonAimRangeAds;
	float enemyCrosshairRange;
	float moveSpeedScale;
	float adsZoomFov;
	float adsZoomInFrac;
	float adsZoomOutFrac;
	const char *adsOverlayShader;
	weapOverlayReticle_t adsOverlayReticle;
	float adsOverlayWidth;
	float adsOverlayHeight;
	float adsBobFactor;
	float adsViewBobMult;
	float hipSpreadStandMin;
	float hipSpreadDuckedMin;
	float hipSpreadProneMin;
	float hipSpreadMax;
	float hipSpreadDuckedMax;
	float hipSpreadProneMax;
	float hipSpreadDecayRate;
	float hipSpreadFireAdd;
	float hipSpreadTurnAdd;
	float hipSpreadMoveAdd;
	float hipSpreadDuckedDecay;
	float hipSpreadProneDecay;
	float hipReticleSidePos;
	int adsTransInTime;
	int adsTransOutTime;
	float adsIdleAmount;
	float hipIdleAmount;
	float adsIdleSpeed;
	float hipIdleSpeed;
	float idleCrouchFactor;
	float idleProneFactor;
	float gunMaxPitch;
	float gunMaxYaw;
	float swayMaxAngle;
	float swayLerpSpeed;
	float swayPitchScale;
	float swayYawScale;
	float swayHorizScale;
	float swayVertScale;
	float swayShellShockScale;
	float adsSwayMaxAngle;
	float adsSwayLerpSpeed;
	float adsSwayPitchScale;
	float adsSwayYawScale;
	float adsSwayHorizScale;
	float adsSwayVertScale;
	int rifleBullet;
	int armorPiercing;
	int semiAuto;
	int boltAction;
	int aimDownSight;
	int rechamberWhileAds;
	float adsViewErrorMin;
	float adsViewErrorMax;
	int cookOffHold;
	int clipOnly;
	int wideListIcon;
	int adsFire;
	const char *killIcon;
	int wideKillIcon;
	int flipKillIcon;
	int noPartialReload;
	int segmentedReload;
	int reloadAmmoAdd;
	int reloadStartAdd;
	const char *altWeapon;
	unsigned int altWeaponIndex;
	int dropAmmoMin;
	int dropAmmoMax;
	int explosionRadius;
	int explosionInnerDamage;
	int explosionOuterDamage;
	int projectileSpeed;
	int projectileSpeedUp;
	const char *projectileModel;
	weapProjExposion_t projExplosionType;
	const char *projExplosionEffect;
	const char *projExplosionSound;
	int projImpactExplode;
	float parallelDefaultBounce;
	float parallelBarkBounce;
	float parallelBrickBounce;
	float parallelCarpetBounce;
	float parallelClothBounce;
	float parallelConcreteBounce;
	float parallelDirtBounce;
	float parallelFleshBounce;
	float parallelFoliageBounce;
	float parallelGlassBounce;
	float parallelGrassBounce;
	float parallelGravelBounce;
	float parallelIceBounce;
	float parallelMetalBounce;
	float parallelMudBounce;
	float parallelPaperBounce;
	float parallelPlasterBounce;
	float parallelRockBounce;
	float parallelSandBounce;
	float parallelSnowBounce;
	float parallelWaterBounce;
	float parallelWoodBounce;
	float parallelAsphaltBounce;
	float perpendicularDefaultBounce;
	float perpendicularBarkBounce;
	float perpendicularBrickBounce;
	float perpendicularCarpetBounce;
	float perpendicularClothBounce;
	float perpendicularConcreteBounce;
	float perpendicularDirtBounce;
	float perpendicularFleshBounce;
	float perpendicularFoliageBounce;
	float perpendicularGlassBounce;
	float perpendicularGrassBounce;
	float perpendicularGravelBounce;
	float perpendicularIceBounce;
	float perpendicularMetalBounce;
	float perpendicularMudBounce;
	float perpendicularPaperBounce;
	float perpendicularPlasterBounce;
	float perpendicularRockBounce;
	float perpendicularSandBounce;
	float perpendicularSnowBounce;
	float perpendicularWaterBounce;
	float perpendicularWoodBounce;
	float perpendicularAsphaltBounce;
	const char *projTrailEffect;
	int projectileDLight;
	float projectileRed;
	float projectileGreen;
	float projectileBlue;
	float adsAimPitch;
	float adsCrosshairInFrac;
	float adsCrosshairOutFrac;
	int adsGunKickReducedKickBullets;
	float adsGunKickReducedKickPercent;
	float adsGunKickPitchMin;
	float adsGunKickPitchMax;
	float adsGunKickYawMin;
	float adsGunKickYawMax;
	float adsGunKickAccel;
	float adsGunKickSpeedMax;
	float adsGunKickSpeedDecay;
	float adsGunKickStaticDecay;
	float adsViewKickPitchMin;
	float adsViewKickPitchMax;
	float adsViewKickYawMin;
	float adsViewKickYawMax;
	float adsViewKickCenterSpeed;
	float adsViewScatterMin;
	float adsViewScatterMax;
	float adsSpread;
	int hipGunKickReducedKickBullets;
	float hipGunKickReducedKickPercent;
	float hipGunKickPitchMin;
	float hipGunKickPitchMax;
	float hipGunKickYawMin;
	float hipGunKickYawMax;
	float hipGunKickAccel;
	float hipGunKickSpeedMax;
	float hipGunKickSpeedDecay;
	float hipGunKickStaticDecay;
	float hipViewKickPitchMin;
	float hipViewKickPitchMax;
	float hipViewKickYawMin;
	float hipViewKickYawMax;
	float hipViewKickCenterSpeed;
	float hipViewScatterMin;
	float hipViewScatterMax;
	float fightDist;
	float maxDist;
	const char *aiVsAiAccuracyGraph;
	const char *aiVsPlayerAccuracyGraph;
	int accuracyGraphKnotCount[2];
	int originalAccuracyGraphKnotCount[2];
	int adsReloadTransTime;
	float leftArc;
	float rightArc;
	float topArc;
	float bottomArc;
	float accuracy;
	float aiSpread;
	float playerSpread;
	float minVertTurnSpeed;
	float minHorTurnSpeed;
	float maxVertTurnSpeed;
	float maxHorTurnSpeed;
	float pitchConvergenceTime;
	float yawConvergenceTime;
	float suppressionTime;
	float maxRange;
	float animHorRotateInc;
	float playerPositionDist;
	const char *useHintString;
	const char *dropHintString;
	int useHintStringIndex;
	int dropHintStringIndex;
	float horizViewJitter;
	float vertViewJitter;
	const char *script;
	float OOPosAnimLength[2];
	int minDamage;
	int minPlayerDamage;
	float maxDamageRange;
	float minDamageRange;
	float destabilizationBaseTime;
	float destabilizationTimeReductionRatio;
	float destabilizationAngleMax;
	int destabilizeDistance;
	int locNone;
	int locHelmet;
	int locHead;
	int locNeck;
	int locTorsoUpper;
	int locTorsoLower;
	int locRightArmUpper;
	int locLeftArmUpper;
	int locRightArmLower;
	int locLeftArmLower;
	int locRightHand;
	int locLeftHand;
	int locRightLegUpper;
	int locLeftLegUpper;
	int locRightLegLower;
	int locLeftLegLower;
	int locRightFoot;
	int locLeftFoot;
	int locGun;
	const char *fireRumble;
	const char *meleeImpactRumble;
} WeaponDef;
static_assert((sizeof(WeaponDef) == 0x604), "ERROR: WeaponDef size is invalid!");

unsigned int BG_GetNumWeapons();
WeaponDef* BG_GetWeaponDef(int weaponIndex);