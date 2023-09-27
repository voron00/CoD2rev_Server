#pragma once

#include "../clientscript/clientscript_public.h"
#include "../xanim/xanim_public.h"

//==================================================================
// New Animation Scripting Defines

#define MAX_ANIMSCRIPT_MODELS               32
#define MAX_ANIMSCRIPT_ITEMS_PER_MODEL      2048
#define MAX_MODEL_ANIMATIONS                512     // animations per model
#define MAX_ANIMSCRIPT_ANIMCOMMANDS         8
#define MAX_ANIMSCRIPT_ITEMS                128
// NOTE: these must all be in sync with string tables in bg_animation.c

#define MAX_ANIMSCRIPT_ANIMCOMMANDS         8

#define ANIM_BITS           10
#define ANIM_TOGGLEBIT      ( 1 << ( ANIM_BITS - 1 ) )

#define DEFAULT_GRAVITY     800

typedef enum
{
	ANIM_BP_UNUSED,
	ANIM_BP_LEGS,
	ANIM_BP_TORSO,
	ANIM_BP_BOTH,

	NUM_ANIM_BODYPARTS
} animBodyPart_t;

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

typedef enum
{
	PARSEMODE_DEFINES,
	PARSEMODE_ANIMATION,
	PARSEMODE_CANNED_ANIMATIONS,
	PARSEMODE_STATECHANGES,
	PARSEMODE_EVENTS
} animScriptParseMode_t;

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
	int stance;
	int noteType;
} animation_t;
// static_assert((sizeof(animation_t) == 0x60), "ERROR: animation_t size is invalid!");

typedef struct
{
	int index;
	int value[2];
} animScriptCondition_t;

typedef struct snd_alias_list_s
{
	const char *aliasName;
	struct snd_alias_t *head;
	int count;
	snd_alias_list_s *next;
} snd_alias_list_t;

typedef struct
{
	short bodyPart[2];
	short animIndex[2];
	unsigned short animDuration[2];
	snd_alias_list_t *soundAlias;
} animScriptCommand_t;
// static_assert((sizeof(animScriptCommand_t) == 0x10), "ERROR: animScriptCommand_t size is invalid!");

typedef struct
{
	int numConditions;
	animScriptCondition_t conditions[NUM_ANIM_CONDITIONS];
	int numCommands;
	animScriptCommand_t commands[MAX_ANIMSCRIPT_ANIMCOMMANDS];
} animScriptItem_t;
// static_assert((sizeof(animScriptItem_t) == 0xF4), "ERROR: animScriptItem_t size is invalid!");

typedef struct
{
	int numItems;
	animScriptItem_t *items[128];
} animScript_t;

typedef struct animScriptData_s
{
	animation_s animations[MAX_MODEL_ANIMATIONS];
	int numAnimations;
	animScript_t scriptAnims[MAX_AISTATES][NUM_ANIM_MOVETYPES];
	animScript_t scriptCannedAnims[MAX_AISTATES][NUM_ANIM_MOVETYPES];
	animScript_t scriptStateChange[MAX_AISTATES][MAX_AISTATES];
	animScript_t scriptEvents[NUM_ANIM_EVENTTYPES];
	animScriptItem_t scriptItems[MAX_ANIMSCRIPT_ITEMS_PER_MODEL];
	int numScriptItems;
	scr_animtree_t animTree;
	unsigned short torsoAnim;
	unsigned short legsAnim;
	unsigned short turningAnim;
	unsigned short rootAnim;
	snd_alias_list_t *(*soundAlias)(const char *);
	void (*playSoundAlias)(int, snd_alias_list_t *);
} animScriptData_t;
// static_assert((sizeof(animScriptData_t) == 736200), "ERROR: animScriptData_t size is invalid!");

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
	animation_s *animation;
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
	int leftHandGun;
	int dobjDirty;
	clientControllers_t control;
	int clientConditions[NUM_ANIM_CONDITIONS][2];
	XAnimTree_s *pXAnimTree;
	int iDObjWeapon;
	int stanceTransitionTime;
	int turnAnimEndTime;
	char turnAnimType;
} clientInfo_t;
// static_assert((sizeof(clientInfo_t) == 0x4B8), "ERROR: clientInfo_t size is invalid!");

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
	void (*CreateDObj)(DObjModel_s *, unsigned short, XAnimTree_s *, int);
	void (*SafeDObjFree)(int);
	void *(*AllocXAnim)(int);
	clientInfo_t clientinfo[64];
} bgs_t;
// static_assert((sizeof(bgs_t) == 0xC6A00), "ERROR: bgs_t size is invalid!");

extern bgs_t level_bgs;

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

enum weapFieldType_t
{
	WFT_WEAPONTYPE = 0x8,
	WFT_WEAPONCLASS = 0x9,
	WFT_OVERLAYRETICLE = 0xA,
	WFT_SLOT = 0xB,
	WFT_STANCE = 0xC,
	WFT_PROJ_EXPLOSION = 0xD,
	WFT_OFFHAND_CLASS = 0xE,
	WFT_ANIMTYPE = 0xF,
	WFT_NUM_FIELD_TYPES = 0x10,
};

typedef struct
{
	const char *szInternalName;
	const char *displayName;
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
	int weaponType;
	int weaponClass;
	int weaponSlot;
	int offhandClass;
	int slotStackable;
	int stance;
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
	int adsOverlayReticle;
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
	int projExplosionType;
	const char *projExplosionEffect;
	const char *projExplosionSound;
	int projImpactExplode;
	float parallelBounce[23];
	float perpendicularBounce[23];
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
	const char *accuracyGraphName[2];
	intptr_t accuracyGraphKnotCount[2];
	intptr_t originalAccuracyGraphKnotCount[2];
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
	float locationDamageMultipliers[19];
	const char *fireRumble;
	const char *meleeImpactRumble;
} WeaponDef;
// static_assert((sizeof(WeaponDef) == 0x604), "ERROR: WeaponDef size is invalid!");

typedef struct
{
	const char *szInternalName;
	const char *displayName;
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
	int weaponType;
	int weaponClass;
	int weaponSlot;
	int offhandClass;
	int slotStackable;
	int stance;
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
	int adsOverlayReticle;
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
	int projExplosionType;
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
	intptr_t accuracyGraphKnotCount[2];
	intptr_t originalAccuracyGraphKnotCount[2];
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
	float locNone;
	float locHelmet;
	float locHead;
	float locNeck;
	float locTorsoUpper;
	float locTorsoLower;
	float locRightArmUpper;
	float locRightArmLower;
	float locRightHand;
	float locLeftArmUpper;
	float locLeftArmLower;
	float locLeftHand;
	float locRightLegUpper;
	float locRightLegLower;
	float locRightFoot;
	float locLeftLegUpper;
	float locLeftLegLower;
	float locLeftFoot;
	float locGun;
	const char *fireRumble;
	const char *meleeImpactRumble;
} WeaponFullDef;
// static_assert((sizeof(WeaponFullDef) == 0x604), "ERROR: WeaponFullDef size is invalid!");

extern WeaponDef* bg_weaponDefs[];
extern unsigned int bg_iNumWeapons;

extern WeaponDef *bg_weapAmmoTypes[];
extern WeaponDef *bg_weapClips[];
extern WeaponDef *bg_sharedAmmoCaps[];

enum hitLocation_t
{
	HITLOC_NONE = 0x0,
	HITLOC_HELMET = 0x1,
	HITLOC_HEAD = 0x2,
	HITLOC_NECK = 0x3,
	HITLOC_TORSO_UPR = 0x4,
	HITLOC_TORSO_LWR = 0x5,
	HITLOC_R_ARM_UPR = 0x6,
	HITLOC_L_ARM_UPR = 0x7,
	HITLOC_R_ARM_LWR = 0x8,
	HITLOC_L_ARM_LWR = 0x9,
	HITLOC_R_HAND = 0xA,
	HITLOC_L_HAND = 0xB,
	HITLOC_R_LEG_UPR = 0xC,
	HITLOC_L_LEG_UPR = 0xD,
	HITLOC_R_LEG_LWR = 0xE,
	HITLOC_L_LEG_LWR = 0xF,
	HITLOC_R_FOOT = 0x10,
	HITLOC_L_FOOT = 0x11,
	HITLOC_GUN = 0x12,
	HITLOC_NUM = 0x13,
};

struct pmoveHandler_t
{
	void (*trace)(trace_t *, const float *, const float *, const float *, const float *, int, int);
	int (*isEntWalkable)(const float *, int, int);
	void (*playerEvent)(int, int);
};

struct pmove_t
{
	playerState_s *ps;
	usercmd_s cmd;
	usercmd_s oldcmd;
	int tracemask;
	int numtouch;
	int touchents[32];
	vec3_t mins;
	vec3_t maxs;
	float xyspeed;
	int proneChange;
	byte handler;
	byte mantleStarted;
	vec3_t mantleEndPos;
	int mantleDuration;
};

struct pml_t
{
	vec3_t forward;
	vec3_t right;
	vec3_t up;
	float frametime;
	int msec;
	int walking;
	int groundPlane;
	int almostGroundPlane;
	trace_t groundTrace;
	float impactSpeed;
	vec3_t previous_origin;
	vec3_t previous_velocity;
	unsigned int holdrand;
};

enum weaponstate_t
{
	WEAPON_READY = 0x0,
	WEAPON_RAISING = 0x1,
	WEAPON_DROPPING = 0x2,
	WEAPON_FIRING = 0x3,
	WEAPON_RECHAMBERING = 0x4,
	WEAPON_RELOADING = 0x5,
	WEAPON_RELOADING_INTERUPT = 0x6,
	WEAPON_RELOAD_START = 0x7,
	WEAPON_RELOAD_START_INTERUPT = 0x8,
	WEAPON_RELOAD_END = 0x9,
	WEAPON_MELEE_INIT = 0xA,
	WEAPON_MELEE_FIRE = 0xB,
	WEAPON_OFFHAND_INIT = 0xC,
	WEAPON_OFFHAND_PREPARE = 0xD,
	WEAPON_OFFHAND_HOLD = 0xE,
	WEAPON_OFFHAND = 0xF,
	WEAPON_OFFHAND_END = 0x10,
	WEAPON_BINOCULARS_INIT = 0x11,
	WEAPON_BINOCULARS_PREPARE = 0x12,
	WEAPON_BINOCULARS_HOLD = 0x13,
	WEAPON_BINOCULARS_START = 0x14,
	WEAPON_BINOCULARS_DROP = 0x15,
	WEAPON_BINOCULARS_END = 0x16,
	WEAPONSTATES_NUM = 0x17,
};

enum weapAnimNumber_t
{
	WEAP_IDLE = 0x0,
	WEAP_FORCE_IDLE = 0x1,
	WEAP_ATTACK = 0x2,
	WEAP_ATTACK_LASTSHOT = 0x3,
	WEAP_RECHAMBER = 0x4,
	WEAP_ADS_ATTACK = 0x5,
	WEAP_ADS_ATTACK_LASTSHOT = 0x6,
	WEAP_ADS_RECHAMBER = 0x7,
	WEAP_MELEE_ATTACK = 0x8,
	WEAP_DROP = 0x9,
	WEAP_RAISE = 0xA,
	WEAP_RELOAD = 0xB,
	WEAP_RELOAD_EMPTY = 0xC,
	WEAP_RELOAD_START = 0xD,
	WEAP_RELOAD_END = 0xE,
	WEAP_ALTSWITCH = 0xF,
	WEAP_EMPTY_DROP = 0x11,
	WEAP_EMPTY_RAISE = 0x12,
	WEAP_HOLD_FIRE = 0x13,
	MAX_WP_ANIMATIONS = 0x14,
};

#define PMF_PRONE 				1
#define PMF_CROUCH 				2
#define PMF_MANTLE 				4
#define PMF_FRAG				16
#define PMF_LADDER 				32
#define PMF_BACKWARDS_RUN 		128
#define PMF_SLIDING 			512
#define PMF_MELEE 				8192
#define PMF_JUMPING 			524288
#define PMF_SPECTATING 			16777216
#define PMF_DISABLEWEAPON 		67108864

extern dvar_t *player_view_pitch_up;
extern dvar_t *player_view_pitch_down;
extern dvar_t *bg_ladder_yawcap;
extern dvar_t *bg_prone_yawcap;
extern dvar_t *bg_foliagesnd_minspeed;
extern dvar_t *bg_foliagesnd_maxspeed;
extern dvar_t *bg_foliagesnd_slowinterval;
extern dvar_t *bg_foliagesnd_fastinterval;
extern dvar_t *bg_foliagesnd_resetinterval;
extern dvar_t *bg_fallDamageMinHeight;
extern dvar_t *bg_fallDamageMaxHeight;
extern dvar_t *inertiaMax;
extern dvar_t *inertiaDebug;
extern dvar_t *inertiaAngle;
extern dvar_t *friction;
extern dvar_t *stopspeed;
extern dvar_t *bg_swingSpeed;
extern dvar_t *bg_bobAmplitudeStanding;
extern dvar_t *bg_bobAmplitudeDucked;
extern dvar_t *bg_bobAmplitudeProne;
extern dvar_t *bg_bobMax;
extern dvar_t *bg_aimSpreadMoveSpeedThreshold;
extern dvar_t *player_breath_hold_time;
extern dvar_t *player_breath_gasp_time;
extern dvar_t *player_breath_fire_delay;
extern dvar_t *player_breath_gasp_scale;
extern dvar_t *player_breath_hold_lerp;
extern dvar_t *player_breath_gasp_lerp;
extern dvar_t *player_breath_snd_lerp;
extern dvar_t *player_breath_snd_delay;
extern dvar_t *player_toggleBinoculars;
extern dvar_t *player_scopeExitOnDamage;
extern dvar_t *player_adsExitDelay;
extern dvar_t *player_moveThreshhold;
extern dvar_t *player_footstepsThreshhold;
extern dvar_t *player_strafeSpeedScale;
extern dvar_t *player_backSpeedScale;
extern dvar_t *player_spectateSpeedScale;
extern dvar_t *player_turnAnims;
extern dvar_t *player_dmgtimer_timePerPoint;
extern dvar_t *player_dmgtimer_maxTime;
extern dvar_t *player_dmgtimer_minScale;
extern dvar_t *player_dmgtimer_stumbleTime;
extern dvar_t *player_dmgtimer_flinchTime;

struct MantleAnimTransition
{
	int upAnimIndex;
	int overAnimIndex;
	float height;
};

struct MantleResults
{
	vec3_t dir;
	vec3_t startPos;
	vec3_t ledgePos;
	vec3_t endPos;
	int flags;
	int duration;
};

struct viewLerpWaypoint_s
{
	int iFrac;
	float fViewHeight;
	int iOffset;
};

struct weaponParms
{
	float forward[3];
	float right[3];
	float up[3];
	float muzzleTrace[3];
	float gunForward[3];
	const WeaponDef *weapDef;
};

struct viewState_t
{
	playerState_s *ps;
	int damageTime;
	int time;
	float v_dmg_pitch;
	float v_dmg_roll;
	float xyspeed;
	float frametime;
	float fLastIdleFactor;
	int *weapIdleTime;
};

struct weaponState_t
{
	const playerState_s *ps;
	float xyspeed;
	float frametime;
	float vLastMoveAng[3];
	float fLastIdleFactor;
	int time;
	int damageTime;
	float v_dmg_pitch;
	float v_dmg_roll;
	float recoilAngles[3];
	float recoilSpeed[3];
	float swayAngles[3];
	int *weapIdleTime;
};

enum pmtype_t
{
	PM_NORMAL = 0x0,
	PM_NORMAL_LINKED = 0x1,
	PM_NOCLIP = 0x2,
	PM_UFO = 0x3,
	PM_SPECTATOR = 0x4,
	PM_INTERMISSION = 0x5,
	PM_DEAD = 0x6,
	PM_DEAD_LINKED = 0x7,
};

extern pmoveHandler_t pmoveHandlers[];
extern int singleClientEvents[];

typedef struct gitem_s
{
	const char *classname;
	const char *pickup_sound;
	const char *world_model;
	const char *view_model;
	const char *icon;
	const char *pickup_name;
	int quantity;
	int giType;
	int giTag;
	int giAmmoIndex;
	int giClipIndex;
} gitem_t;

extern gitem_t bg_itemlist[];

#ifdef __cplusplus
extern "C" {
#endif

unsigned int QDECL BG_GetConditionValue(clientInfo_t *ci, int condition, qboolean checkConversion);
void QDECL PM_Accelerate(playerState_s *ps, const pml_t *pml, float *wishdir, float wishspeed, float accel);
void QDECL PM_FootstepEvent(pmove_t *pm, pml_t *pml, int iOldBobCycle, int iNewBobCycle, int bFootStep);
qboolean QDECL PM_ShouldMakeFootsteps(pmove_t *pm);
int QDECL BG_CheckProne(int passEntityNum, float const * const vPos, float fSize, float fHeight, float fYaw, float * pfTorsoHeight, float * pfTorsoPitch, float * pfWaistPitch, int bAlreadyProne, int bOnGround, float * const vGroundNormal, unsigned char handler, int proneCheckType, float prone_feet_dist);
int QDECL PM_VerifyPronePosition(pmove_t *pm, float *vFallbackOrg, float *vFallbackVel);
void QDECL PM_playerTrace(pmove_t *pm, trace_t *results, const float *start, const float *mins, const float *maxs, const float *end, int passEntityNum, int contentMask);
void QDECL PM_AddTouchEnt(pmove_t *pm, int entityNum);
void QDECL PM_StepSlideMove(pmove_t *pm, pml_t *pml, int gravity);
void QDECL PM_ClipVelocity(const float *in, const float *normal, float *out);
void QDECL PM_AirMove(pmove_t *pm, pml_t *pml);
float QDECL PM_CmdScale(playerState_s *ps, usercmd_s *cmd);
int QDECL PM_GroundSurfaceType(pml_t *pml);
void QDECL PM_AddEvent(playerState_s *ps, int newEvent);
int QDECL PM_GetEffectiveStance(const playerState_s *ps);
void QDECL PM_UpdateViewAngles(playerState_s *ps, float msec, usercmd_s *cmd, unsigned char handler);
void QDECL PM_UpdatePronePitch(pmove_t *pm, pml_t *pml);

void QDECL Jump_ClearState(playerState_s *ps);
bool QDECL Jump_GetStepHeight(playerState_s *ps, const float *origin, float *stepSize);
bool QDECL Jump_IsPlayerAboveMax(playerState_s *ps);
void QDECL Jump_ClampVelocity(playerState_s *ps, const float *origin);
void QDECL Jump_ActivateSlowdown(playerState_s *ps);
bool QDECL Jump_Check(pmove_t *pm, pml_t *pml);

void QDECL BG_AddPredictableEventToPlayerstate( int newEvent, int eventParm, playerState_t *ps );
int QDECL BG_AnimScriptEvent( playerState_s *ps, scriptAnimEventTypes_t event, qboolean isContinue, qboolean force );

int QDECL BG_IsAimDownSightWeapon(int weapon);
unsigned int QDECL BG_GetViewmodelWeaponIndex(const playerState_s *ps);
WeaponDef* QDECL BG_GetWeaponDef(int weaponIndex);
void QDECL BG_CalculateWeaponPosition_Sway(const struct playerState_s *ps, float *swayViewAngles, float *swayOffset, float *swayAngles, float ssSwayScale, int frametime);

float QDECL BG_GetVerticalBobFactor(const struct playerState_s *ps, float cycle, float speed, float maxAmp);
float QDECL BG_GetHorizontalBobFactor(const struct playerState_s *ps, float cycle, float speed, float maxAmp);

void QDECL BG_CalculateViewMovementAngles(viewState_t *vs, float *angles);
void QDECL BG_CalculateWeaponMovement(weaponState_t *ws, float *angles);

void QDECL PM_ProjectVelocity(const float *velIn, const float *normal, float *velOut);
float QDECL PM_PermuteRestrictiveClipPlanes(const float *velocity, int planeCount, const float (*planes)[3], int *permutation);
int QDECL PM_SlideMove(pmove_t *pm, pml_t *pml, int gravity);
void QDECL PM_UpdateLean(playerState_s *ps, float msec, usercmd_s *cmd, void (*capsuleTrace)(trace_t *, const float *, const float *, const float *, const float *, int, int));
void QDECL Mantle_CapView(playerState_s *ps);
int QDECL BG_CheckProneTurned(playerState_s *ps, float newProneYaw, unsigned char handler);

#ifdef __cplusplus
}
#endif

void PM_ExitAimDownSight(playerState_s *ps);
void BG_RunLerpFrameRate(clientInfo_t *ci, lerpFrame_t *lf, int newAnimation, const entityState_s *es);
void BG_SwingAngles( float destination,  float swingTolerance, float clampTolerance, float speed, float *angle, qboolean *swinging );
void BG_PlayerAnimation(const DObj_s *pDObj, entityState_s *es, clientInfo_t *ci);
int BG_PlayAnim(playerState_s *ps, int animNum, int bodyPart, int forceDuration, qboolean setTimer, qboolean isContinue, qboolean force);
void BG_SetNewAnimation(clientInfo_t *ci, lerpFrame_t *lf, int newAnimation, qboolean isComplete);
int BG_ExecuteCommand( playerState_t *ps, animScriptCommand_t *scriptCommand, qboolean setTimer, qboolean isContinue, qboolean force );
void BG_AnimPlayerConditions(entityState_s *es, clientInfo_t *ci);
void BG_UpdateConditionValue(int client, int condition, int value, qboolean checkConversion);
void BG_UpdatePlayerDObj(DObj_s *pDObj, entityState_s *es, clientInfo_t *ci, int attachIgnoreCollision);
int BG_AnimScriptAnimation(playerState_s *ps, int state, int movetype, qboolean force);
void BG_Player_DoControllers(DObj_s *obj, const gentity_s *ent, int *partBits, clientInfo_t *ci, int frametime);
void BG_AnimParseAnimScript(animScriptData_t *scriptData, loadAnim_t *pLoadAnims, unsigned int *piNumAnims);
void BG_FindAnimTrees();
void BG_FindAnims();
void BG_AnimUpdatePlayerStateConditions(pmove_t *pmove);
unsigned int BG_AnimationIndexForString(const char *string);
void BG_FinalizePlayerAnims();
void BG_LoadAnim();
void BG_PostLoadAnim();

void BG_LoadPlayerAnimTypes();
void BG_InitWeaponStrings();

int CL_LocalClient_GetActiveCount();
void PM_AdjustAimSpreadScale(pmove_t *pm, pml_t *pml);
void PM_ViewHeightAdjust(pmove_t *pm, pml_t *pml);
void PM_CheckDuck(pmove_t *pm, pml_t *pml);
void PM_SetMovementDir(pmove_t *pm, pml_t *pml);
void PM_WalkMove(pmove_t *pm, pml_t *pml);
void PM_NoclipMove(pmove_t *pm, pml_t *pml);
void PM_UFOMove(pmove_t *pm, pml_t *pml);
void PM_GroundTrace(pmove_t *pm, pml_t *pml);
void PM_Footsteps(pmove_t *pm, pml_t *pml);
void PM_CheckLadderMove(pmove_t *pm, pml_t *pml);
void PM_CrashLand(playerState_s *pm, pml_t *pml);
void PM_LadderMove(pmove_t *pm, pml_t *pml);

void Pmove(pmove_t *pmove);

qboolean BG_PlayerTouchesItem(const playerState_s *ps, const entityState_s *item, int atTime);
void BG_EvaluateTrajectory( const trajectory_t *tr, int atTime, vec3_t result );
void BG_EvaluateTrajectoryDelta( const trajectory_t *tr, int atTime, vec3_t result);
int PM_WeaponAmmoAvailable(playerState_s *ps);
int BG_WeaponAmmo(const playerState_s *ps, int weaponIndex);
int BG_WeaponIsClipOnly(int weapon);
int BG_AmmoForWeapon(int weapon);
int BG_ClipForWeapon(int weapon);
int BG_GetClipSize(int weaponIndex);
int BG_GetMaxAmmo(int weaponIndex);
int BG_GetSharedAmmoCapSize(int weaponIndex);
qboolean PM_WeaponClipEmpty(playerState_s *ps);
int PM_InteruptWeaponWithProneMove(playerState_s *ps);
void PM_ResetWeaponState(playerState_s *ps);
void PM_UpdateAimDownSightLerp(pmove_t *pm, pml_t *pml);
void PM_UpdateAimDownSightFlag(pmove_t *pm, pml_t *pml);
float BG_GetBobCycle(gclient_s *client);
float BG_GetSpeed(const playerState_s *ps, int time);
void BG_GetSpreadForWeapon(const playerState_s *ps, int weaponIndex, float *minSpread, float *maxSpread);
int BG_GetFirstAvailableOffhand(playerState_s *ps, int offhandSlot);
int BG_GetMaxPickupableAmmo(playerState_s *ps, unsigned int weaponIndex);
bool BG_DoesWeaponNeedSlot(int weapon);
int BG_PlayerHasWeapon(playerState_s *ps, int weaponIndex, int altWeaponIndex);
int BG_GetStackableSlot(gclient_s *client, int weapon, int slot);
int BG_GetEmptySlotForWeapon(playerState_s *ps, int weapon);
qboolean BG_CanItemBeGrabbed(entityState_s *ent, playerState_s *ps, int touched);
int BG_GetWeaponSlotForName(const char *name);
const char* BG_GetWeaponSlotNameForIndex(int index);

void BG_CreateRotationMatrix( const vec3_t angles, vec3_t matrix[3] );
void BG_TransposeMatrix( const vec3_t matrix[3], vec3_t transpose[3] );
void BG_RotatePoint( vec3_t point, const vec3_t matrix[3] );

void Jump_ApplySlowdown(playerState_s *ps);
void Mantle_ClearHint(playerState_s *ps);
void BG_StringCopy(unsigned char *member, const char *keyValue);
bool BG_IsWeaponValid(playerState_t *ps, int weaponIndex);
int BG_SetupWeaponDef(WeaponDef *weapDef, int (*regWeap)(int));
void BG_FillInAmmoItems(int (*regWeap)(int));
WeaponDef *BG_LoadDefaultWeaponDef();
void BG_ClearWeaponDef();
void BG_ShutdownWeaponDefFiles();

int BG_GetWeaponIndexForName(const char *name, int (*regWeap)(int));
int BG_FindWeaponIndexForName(const char *name);
int BG_TakePlayerWeapon(playerState_s *ps, int weaponIndex);
bool BG_DoesWeaponRequireSlot(int weaponIndex);
bool BG_IsAnyEmptyPrimaryWeaponSlot(gclient_s *client);

long BG_StringHashValue( const char *fname );
long BG_StringHashValue_Lwr( const char *fname );

bool Mantle_IsWeaponInactive(const playerState_s *ps);
float Jump_ReduceFriction(playerState_s *ps);
void PM_FlyMove(pmove_t *pm, pml_t *pml);

void PM_Weapon(pmove_t *pm, pml_t *pml);
void PM_trace(pmove_t *pm,trace_t *results, const float *start, const float *mins, const float *maxs, const float *end, int passEntityNum, int contentMask);

void Mantle_GetAnimDelta(MantleState *mstate, int time, float *delta);
int Mantle_GetAnim(MantleState *mstate);
void Mantle_Move(pmove_t *pm, playerState_s *ps, pml_t *pml);
void Mantle_Start(pmove_t *pm, playerState_s *ps, MantleResults *mresults);
void Mantle_Check(pmove_t *pmove, pml_t *pml);
void Mantle_CreateAnims(void *(*Alloc)(int));
void Mantle_ShutdownAnims();

unsigned int BG_GetNumWeapons();
void BG_WeaponFireRecoil(playerState_s *ps, float *recoilSpeed, float *kickAVel);
void PM_WeaponUseAmmo(playerState_s *ps, int weaponIndex, int amount);

WeaponDef* BG_LoadWeaponDef(const char *folderName, const char *weaponName);
gitem_s* BG_FindItemForWeapon(int weapon);;

int PM_GetViewHeightLerpTime(const playerState_s *ps, int iTarget, int bDown);
void BG_PlayerStateToEntityState(playerState_s *ps, entityState_s *s, int snap, byte handler);

void Mantle_RegisterDvars();
void Jump_RegisterDvars();
void BG_RegisterDvars();