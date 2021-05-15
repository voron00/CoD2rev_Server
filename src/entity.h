#pragma once

#include "player.h"

typedef struct gclient_s gclient_t;
typedef struct gentity_s gentity_t;
typedef int scr_entref_t;

typedef enum team_s
{
	TEAM_FREE,
	TEAM_RED,
	TEAM_BLUE,
	TEAM_SPEC,
	TEAM_NUM_TEAMS
} team_t;

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
	trType_t trType;
	int trTime;
	int trDuration;
	vec3_t trBase;
	vec3_t trDelta;
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
	int iHeadIcon;
	int iHeadIconTeam;
	int solid;
	int eventParm;
	int eventSequence;
	vec4_t events;
	vec4_t eventParms;
	int weapon;
	int legsAnim;
	int torsoAnim;
	int stage;
	int hintstring;
	int cursorhint;
	int animMovetype;
	vec3_t angles2;
} entityState_t; // verified

typedef struct
{
	byte linked;
	byte bmodel;
	byte svFlags;
	byte pad1;
	int clientMask[2];
	byte inuse;
	byte pad2[3];
	int broadcastTime;
	vec3_t mins;
	vec3_t maxs;
	int contents;
	vec3_t absmin;
	vec3_t absmax;
	vec3_t currentOrigin;
	vec3_t currentAngles;
	u_int16_t ownerNum;
	u_int16_t pad3;
	int eventTime;
} entityShared_t; // verified

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

typedef struct
{
	byte slot_none;
	byte slot_primary;
	byte slot_primaryb;
	byte pad;
} weapSlot_t;

typedef struct playerState_s
{
	int commandTime;
	int pm_type;
	int bobCycle;
	int pm_flags;
	int pm_time;
	vec3_t origin;
	vec3_t velocity;
	vec2_t oldVelocity;
	int weaponTime;
	int weaponDelay;
	int grenadeTimeLeft;
	int weaponRestrictKickTime;
	int foliageSoundTime;
	int gravity;
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
	int legsAnimDuration;
	int torsoAnimDuration;
	int damageTimer;
	int damageDuration;
	int flinchYawAnim;
	int movementDir;
	int eFlags;
	int eventSequence;
	int events[4];
	unsigned int eventParms[4];
	int oldEventSequence;
	int clientNum;
	int offHandIndex;
	unsigned int weapon;
	int weaponstate;
	float fWeaponPosFrac;
	int adsDelayTime;
	int viewmodelIndex;
	vec3_t viewangles;
	int viewHeightTarget;
	float viewHeightCurrent;
	int viewHeightLerpTime;
	int viewHeightLerpTarget;
	int viewHeightLerpDown;
	vec2_t		viewAngleClampBase;
	vec2_t		viewAngleClampRange;
	int unknown;
	int damageCount;
	int damageYaw;
	int damagePitch;
	int damageEvent;
	int health;
	int deadYaw;
	int maxhealth;
	int teamPlayInfoEntNum;
	struct gclient_s *teamPlayInfoClient;
	int clientSpawnCounter;
	int ammo[128];
	int ammoclip[128];
	int weapFlags;
	int weapFlags2;
	int otherFlags;
	int otherFlags2;
	weapSlot_t weaponSlot;
	int nonPVSFriendlyFlags;
	int weaponRechamber;
	vec3_t unknownVector;
	vec3_t mins;
	vec3_t maxs;
	float proneDirection;
	float proneDirectionPitch;
	float proneTorsoPitch;
	ViewLockTypes_t viewlocked;
	int viewlocked_entNum;
	int cursorHint;
	int cursorHintString;
	int cursorHintEntIndex;
	int unknownInteger;
	vec3_t angles2;
	float holdBreathScale;
	int holdBreathTime;
	mantleState_t mantleState;
	int entityEventSequence;
	int weaponSequenceFlags;
	float aimSpreadScale;
	int shellshockIndex;
	int shellshockTime;
	int shellshockDuration;
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
	int clientIndex;
	int team;
	int modelindex;
	int attachModelIndex[6];
	int attachTagIndex[6];
	char name[32];
} clientState_t;

typedef struct
{
	sessionState_t state;
	int forceSpectatorClient;
	int statusIcon;
	int archiveTime;
	int score;
	int deaths;
	u_int16_t scriptPersId;
	byte pad2;
	byte pad;
	int connected;
	usercmd_s cmd;
	usercmd_s oldcmd;
	int localClient;
	int predictItemPickup;
	char name[32];
	int maxHealth;
	int enterTime;
	int voteCount;
	int teamVoteCount;
	float unknown;
	int viewmodelIndex;
	int noSpectate;
	int teamInfo;
	clientState_t clState;
	int psOffsetTime;
} clientSession_t; // verified

struct gclient_s
{
	playerState_t ps;
	clientSession_t sess;
	int spectatorClient;
	int noclip;
	int ufo;
	int bFrozen;
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
	int damage_fromWorld;
	int accurateCount; // N/A
	int accuracy_shots; // N/A
	int accuracy_hits; // N/A
	int inactivityTime;
	int inactivityWarning;
	int playerTalkTime;
	int rewardTime; // N/A
	float currentAimSpreadScale;
	int sniperRifleFiredTime; // N/A
	float sniperRifleMuzzleYaw; // N/A
	int unknownClientEndFrameVar;
	vec3_t unknownVector;  // N/A
	gentity_s *lookAtEntity;
	int activateEntNumber;
	int activateTime;
	int nonPVSFriendlyEntNum;
	int pingPlayerTime;
	int damageFeedBackTime;
	vec2_t damageFeedBackDir;
	vec3_t swayViewAngles;
	vec3_t swayOffset;
	vec3_t swayAngles;
	vec3_t vLastMoveAng;
	vec3_t vGunOffset;
	vec3_t vGunSpeed;
	int vGunAngle[2];
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

struct tagInfo_s
{
	struct gentity_s *parent;
	struct gentity_s *next;
	u_int16_t name;
	u_int16_t pad;
	int index;
	float axis[4][3];
	float parentInvAxis[4][3];
};

struct trigger_ent_t
{
	int threshold;
	int accumulate;
	int timestamp;
	int singleUserEntIndex;
	byte requireLookAt;
};

struct item_ent_t
{
	int ammoCount;
	int clipAmmoCount;
	int index;
};

struct mover_ent_t
{
	float decelTime;
	float aDecelTime;
	float speed;
	float aSpeed;
	float midTime;
	float aMidTime;
	vec3_t pos1;
	vec3_t pos2;
	vec3_t pos3;
	vec3_t apos1;
	vec3_t apos2;
	vec3_t apos3;
};

struct corpse_ent_t
{
	int deathAnimStartTime;
};

enum MissileStage
{
	MISSILESTAGE_SOFTLAUNCH = 0x0,
	MISSILESTAGE_ASCENT = 0x1,
	MISSILESTAGE_DESCENT = 0x2,
};

enum MissileFlightMode
{
	MISSILEFLIGHTMODE_TOP = 0x0,
	MISSILEFLIGHTMODE_DIRECT = 0x1,
};

struct missile_ent_t
{
	float time;
	int timeOfBirth;
	float travelDist;
	vec3_t surfaceNormal;
	enum team_s team;
	vec3_t curvature;
	int targetEntNum;
	vec3_t targetOffset;
	enum MissileStage stage;
	enum MissileFlightMode flightMode;
};

struct gentity_s
{
	entityState_t s;
	entityShared_t r;
	struct gclient_s *client;
	turretInfo_s *pTurretInfo;
	byte physicsObject;
	byte takedamage;
	byte active;
	byte nopickup;
	byte model;
	byte dobjbits;
	byte handler;
	byte team;
	u_int16_t classname;
	u_int16_t target;
	u_int16_t targetname;
	u_int16_t padding;
	int spawnflags;
	int flags;
	int eventTime;
	int freeAfterEvent;
	int unlinkAfterEvent;
	int clipmask;
	int framenum;
	gentity_s *parent;
	int nextthink;
	int healthPoints;
	int reservedHealth;
	int damage;
	int count;
	int unknown;
	union
	{
		struct item_ent_t item[2];
		struct trigger_ent_t trigger;
		struct mover_ent_t mover;
		struct corpse_ent_t corpse;
		struct missile_ent_t missile;
	};
	tagInfo_s *tagInfo;
	gentity_s *tagChildren;
	u_int16_t attachModelNames[6];
	u_int16_t attachTagNames[6];
	int useCount;
	gentity_s *nextFree;
}; // verified
