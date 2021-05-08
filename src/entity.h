#pragma once

#include "player.h"

typedef struct gclient_s gclient_t;
typedef struct gentity_s gentity_t;
typedef int scr_entref_t;

typedef enum
{
	CON_DISCONNECTED,
	CON_CONNECTING,
	CON_CONNECTED
} clientConnected_t;

typedef enum
{
	STATE_PLAYING,
	STATE_DEAD,
	STATE_SPECTATOR,
	STATE_INTERMISSION
} sessionState_t;

typedef enum
{
	TR_STATIONARY,
	TR_INTERPOLATE,
	TR_LINEAR,
	TR_LINEAR_STOP,
	TR_SINE,
	TR_GRAVITY
} trType_t;

typedef struct
{
	trType_t	trType;
	int			trTime;
	int			trDuration;
	vec3_t		trBase;
	vec3_t		trDelta;
} trajectory_t;

typedef struct entityState_s
{
	int number;
	int eType;
	int eFlags;
	trajectory_t pos;
	trajectory_t apos;
	int time;
	int time2;
	vec3_t origin;
	vec3_t angles;
	int otherEntityNum;
	int attackerEntityNum;
	int groundEntityNum;
	int constantLight;
	int loopSound;
	int surfaceFlags;
	int modelindex;
	int clientNum;
	int	iHeadIcon;
	int	iHeadIconTeam;
	int	solid;
	int	eventParm;
	int	eventSequence;
	vec4_t events;
	vec4_t eventParms;
	int weapon;
	int legsAnim;
	int torsoAnim;
	int stage;
	int loopfxid;
	int hintstring;
	int animMovetype;
	vec3_t unkAngles;
} entityState_t; // verified

typedef struct
{
	byte		linked;
	byte		bmodel;
	byte		svFlags;
	byte		pad1;
	int			clientMask[2];
	byte		inuse;
	byte		pad2[3];
	int			broadcastTime;
	vec3_t		mins, maxs;
	int			contents;
	vec3_t		absmin, absmax;
	vec3_t		currentOrigin;
	vec3_t		currentAngles;
	u_int16_t	ownerNum;
	u_int16_t	pad3;
	int			eventTime;
} entityShared_t; // verified

typedef enum
{
	STAT_HEALTH,
	STAT_DEAD_YAW,
	STAT_MAX_HEALTH,
	STAT_FRIENDLY_LOOKAT_CLIENTNUM,
	STAT_FRIENDLY_LOOKAT_HEALTH,
	STAT_SPAWN_COUNT
} statIndex_t;

typedef enum
{
	PLAYERVIEWLOCK_NONE = 0x0,
	PLAYERVIEWLOCK_FULL = 0x1,
	PLAYERVIEWLOCK_WEAPONJITTER = 0x2,
	PLAYERVIEWLOCKCOUNT = 0x3,
} ViewLockTypes_t;

typedef enum
{
	OBJST_EMPTY = 0x0,
	OBJST_ACTIVE = 0x1,
	OBJST_INVISIBLE = 0x2,
	OBJST_DONE = 0x3,
	OBJST_CURRENT = 0x4,
	OBJST_FAILED = 0x5,
	OBJST_NUMSTATES = 0x6,
} objectiveState_t;

typedef struct objective_s
{
	objectiveState_t state;
	vec3_t origin;
	int entNum;
	int teamNum;
	int icon;
} objective_t;

typedef enum
{
	HE_TYPE_FREE = 0x0,
	HE_TYPE_TEXT = 0x1,
	HE_TYPE_VALUE = 0x2,
	HE_TYPE_PLAYERNAME = 0x3,
	HE_TYPE_MAPNAME = 0x4,
	HE_TYPE_GAMETYPE = 0x5,
	HE_TYPE_MATERIAL = 0x6,
	HE_TYPE_TIMER_DOWN = 0x7,
	HE_TYPE_TIMER_UP = 0x8,
	HE_TYPE_TENTHS_TIMER_DOWN = 0x9,
	HE_TYPE_TENTHS_TIMER_UP = 0xA,
	HE_TYPE_CLOCK_DOWN = 0xB,
	HE_TYPE_CLOCK_UP = 0xC,
	HE_TYPE_WAYPOINT = 0xD,
	HE_TYPE_COUNT = 0xE,
} he_type_t;

typedef struct
{
	char r;
	char g;
	char b;
	char a;
} hudelem_colorsplit_t;

typedef union
{
	hudelem_colorsplit_t split;
	int rgba;
} hudelem_color_t;

typedef struct hudelem_s
{
	he_type_t type;
	float x;
	float y;
	float z;
	float fontScale;
	int font;
	int alignOrg;
	int alignScreen;
	hudelem_color_t color;
	hudelem_color_t fromColor;
	int fadeStartTime;
	int fadeTime;
	int label;
	int width;
	int height;
	int materialIndex;
	int fromWidth;
	int fromHeight;
	int scaleStartTime;
	int scaleTime;
	float fromX;
	float fromY;
	int fromAlignOrg;
	int fromAlignScreen;
	int moveStartTime;
	int moveTime;
	int time;
	int duration;
	float value;
	int text;
	float sort;
	hudelem_color_t glowColor;
} hudelem_t; // verified

typedef struct hudElemState_s
{
	hudelem_t current[31];
	hudelem_t archival[31];
} hudElemState_t;

typedef struct
{
	float	yaw;
	int	timer;
	int	transIndex;
	int	flags;
} mantleState_t;

typedef struct playerState_s
{
	int	commandTime;
	int pm_type;
	int bobCycle;
	int pm_flags;
	int pm_time;
	vec3_t origin;
	vec3_t velocity;
	vec2_t oldVelocity; // 48
	int	weaponTime;
	int weaponDelay;
	int	grenadeTimeLeft;
	int	throwBackGrenadeOwner;
	int	throwBackGrenadeTimeLeft;
	int	gravity;
	float leanf;
	int speed;
	vec3_t delta_angles;
	int groundEntityNum;
	vec3_t vLadderVec;
	int jumpTime;
	float jumpOriginZ;
	int legsTime;
	int legsAnim;
	int torsoTime;
	int torsoAnim;
	int	legsAnimDuration;
	int	torsoAnimDuration;
	int	damageTimer;
	int	damageDuration;
	int	flinchYawAnim;
	int	movementDir;
	int	eFlags;
	int	eventSequence;
	int events[4];
	unsigned int eventParms[4];
	int	oldEventSequence;
	int	clientNum;
	int	offHandIndex;
	unsigned int weapon;
	int	weaponstate;
	float fWeaponPosFrac;
	int adsDelayTime;
	int	viewmodelIndex;
	vec3_t viewangles;
	int viewHeightTarget;
	float viewHeightCurrent;
	int viewHeightLerpTime;
	int viewHeightLerpTarget;
	int viewHeightLerpDown;
	int unknown[5];
	int	damageEvent;
	int	damageYaw;
	int	damagePitch;
	int	damageCount;
	int	stats[6];
	int	ammo[128];
	int	ammoclip[128]; // 836
	int weapFlags;
	int weapFlags2;
	int unknown2[2];
	byte slot_none;
	byte slot_primary;
	byte slot_primaryb;
	int unknown3[5];
	vec3_t mins;
	vec3_t maxs;
	float proneDirection;
	float proneDirectionPitch;
	float proneTorsoPitch;
	ViewLockTypes_t viewlocked;
	int viewlocked_entNum;
	int	cursorHint;
	int	cursorHintString;
	int	cursorHintEntIndex;
	int unknown1;
	vec3_t unkAngles;
	float holdBreathScale;
	int holdBreathTimer;
	mantleState_t mantleState;
	int entityEventSequence;
	int	weapAnim;
	float aimSpreadScale;
	int	shellshockIndex;
	int	shellshockTime;
	int	shellshockDuration;
	objective_t objective[16];
	int archiveTime;
	hudElemState_t hud;
} playerState_t;

typedef enum
{
	TEAM_NONE,
	TEAM_AXIS,
	TEAM_ALLIES,
	TEAM_SPECTATOR
} sessionTeam_t;

typedef struct
{
	sessionState_t state;
	int forceSpectatorClient;
	int statusIcon;
	int archiveTime;
	int	score;
	int deaths;
	u_int16_t scriptPersId;
	byte pad2;
	byte pad;
	clientConnected_t connected;
	usercmd_t cmd;
	usercmd_t oldcmd;
	qboolean localClient;
	qboolean predictItemPickup;
	char name[32];
	int maxHealth;
	int enterTime;
	int voteCount;
	int teamVoteCount;
	float unknown;
	int viewmodelIndex;
	qboolean noSpectate;
	int teamInfo;
	int clientId;
	sessionTeam_t team;
	int model;
	int attachedModels[6];
	int attachedModelsTags[6];
	char manualModeName[32];
	int psOffsetTime;
} clientSession_t; // verified

struct gclient_s
{
	playerState_t ps;
	clientSession_t sess;
	int spectatorClient;
	qboolean noclip;
	qboolean ufo;
	qboolean bFrozen;
	int lastCmdTime;
	int buttons;
	int oldbuttons;
	int latched_buttons;
	int buttonsSinceLastFrame;
	vec3_t oldOrigin;
	float fGunPitch;
	float fGunYaw;
	int damage_blood;
	vec3_t damage_from;
	qboolean damage_fromWorld;
	int accurateCount; // N/A
	int accuracy_shots; // N/A
	int accuracy_hits; // N/A
	int inactivityTime;
	qboolean inactivityWarning;
	int playerTalkTime;
	int rewardTime; // N/A
	float currentAimSpreadScale; // 10256
	int unknown_space[2];
	int unknownClientEndFrameVar;
	int unknown_space2[3];
	gentity_t *lookAtEntity; // needs a NULL check, otherwise crash.
	int activateEntNumber;
	int activateTime;
	int nonPVSFriendlyEntNum;
	int pingPlayerTime;
	int damageFeedBackTime;
	vec2_t damageFeedBackDir;
	vec3_t swayViewAngles; // 10316
	vec3_t swayOffset;
	vec3_t swayAngles;
	int unknown_space3[7];
	float weaponRecoil; // 10380
	int unknown_space4[3];
	int lastServerTime;
	int lastActivateTime;
}; // verified

struct turretInfo_s
{
	int inuse;
	int flags;
	int fireTime;
	vec2_t arcmin;
	vec2_t arcmax;
	float dropPitch;
	int stance;
	int prevStance;
	int fireSndDelay;
	vec3_t userOrigin;
	float playerSpread;
	int triggerDown;
	char fireSnd;
	char fireSndPlayer;
	char stopSnd;
	char stopSndPlayer;
};

struct gentity_s
{
	entityState_t s;
	entityShared_t r;
	struct gclient_s *client; // 344
	turretInfo_s *pTurretInfo; // 348
	byte physicsObject; // 352
	byte takedamage; // 353
	byte active; // 354
	byte nopickup; // 355 ?
	byte model; // 356
	byte dobjbits; // 357 ?
	byte handler; // 358
	byte team; // 359
	u_int16_t classname; // 360
	u_int16_t target;
	u_int16_t targetname;
	u_int16_t padding;
	int spawnflags;
	int flags;
	int eventTime;
	qboolean freeAfterEvent; // 380
	qboolean unlinkAfterEvent; // 384
	int clipmask; // 388
	int framenum; // 392
	gentity_t *parent; // 396
	int nextthink; // 400
	int healthPoints; // 404
	int reservedHealth; // 408 ?
	int damage; // 412
	int splashDamage; // 416 ?
	int splashRadius; // 420 ?
	float pfDecelTimeMove; // 424
	float pfDecelTimeRotate; // 428
	float pfSpeedMove; // 432
	float pfSpeedRotate; // 436
	float pfMidTimeMove; // 440
	float pfMidTimeRotate; // 444
	vec3_t vPos1Move; // 448 ?
	vec3_t vPos2Move; // 460
	vec3_t vPos3Move; // 472
	vec3_t vPos1Rotate; // 484 ?
	vec3_t vPos2Rotate; // 496
	vec3_t vPos3Rotate; // 508
	int moverState; // 520 ?
	gentity_t** linkedEntities; // 524 ??
	byte attachedModels[6]; // 528
	u_int16_t attachedModelsIndexes; // 536 ?
	u_int16_t numAttachedModels; // 538 ?
	int animTree; // 540 ?
	vec4_t color; // ?
}; // verified

#if __GNUC__ >= 6
static_assert((sizeof(gentity_t) == 560), "ERROR: gentity_t size is invalid!");
static_assert((sizeof(gclient_t) == 0x28A4), "ERROR: gclient_t size is invalid!");
#endif
