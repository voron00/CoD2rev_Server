#pragma once

#define GAMEVERSION "Call of Duty 2"

#define MAX_HUDELEMENTS 31
#define MAX_HUDELEMS_ARCHIVAL MAX_HUDELEMENTS
#define MAX_HUDELEMS_CURRENT MAX_HUDELEMENTS
#define MAX_OBJECTIVES 16

typedef struct gclient_s gclient_t;
typedef struct gentity_s gentity_t;

typedef struct
{
	const char *key;
	const char *value;
} keyValueStr_t;

typedef struct
{
	byte spawnVarsValid;
	int numSpawnVars;
	keyValueStr_t spawnVars[64];
	int numSpawnVarChars;
	char spawnVarChars[2048];
} SpawnVar;

typedef struct
{
	uint16_t entnum;
	uint16_t otherEntnum;
	int useCount;
	int otherUseCount;
} trigger_info_t;

typedef enum
{
	TEAM_FREE = 0x0,
	TEAM_NONE = 0x0,
	TEAM_BAD = 0x0,
	TEAM_AXIS = 0x1,
	TEAM_ALLIES = 0x2,
	TEAM_SPECTATOR = 0x3,
	TEAM_NUM_TEAMS = 0x4,
} team_t;

typedef enum
{
	CON_DISCONNECTED,
	CON_CONNECTING,
	CON_CONNECTED
} clientConnected_t;

typedef enum
{
	SESS_STATE_PLAYING,
	SESS_STATE_DEAD,
	SESS_STATE_SPECTATOR,
	SESS_STATE_INTERMISSION,
} sessionState_t;

typedef enum
{
	TR_STATIONARY,
	TR_INTERPOLATE,
	TR_LINEAR,
	TR_LINEAR_STOP,
	TR_SINE,
	TR_GRAVITY,
	TR_GRAVITY_PAUSED,
	TR_ACCELERATE,
	TR_DECCELERATE
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
	vec3_t origin2;
	vec3_t angles2;
	int otherEntityNum;
	int attackerEntityNum;
	int groundEntityNum;
	int constantLight;
	int loopSound;
	int surfType;
	int index;
	int clientNum;
	int iHeadIcon;
	int iHeadIconTeam;
	int solid;
	int eventParm;
	int eventSequence;
	int events[4];
	int eventParms[4];
	int weapon;
	int legsAnim;
	int torsoAnim;
	float leanf;
	int scale;
	int dmgFlags;
	int animMovetype;
	float fTorsoHeight;
	float fTorsoPitch;
	float fWaistPitch;
} entityState_t;

typedef struct
{
	byte linked;
	byte bmodel;
	byte svFlags;
	int clientMask[2];
	byte inuse;
	int broadcastTime;
	vec3_t mins;
	vec3_t maxs;
	int contents;
	vec3_t absmin;
	vec3_t absmax;
	vec3_t currentOrigin;
	vec3_t currentAngles;
	int ownerNum;
	int eventTime;
} entityShared_t;

enum objectiveState_t
{
	OBJST_EMPTY = 0x0,
	OBJST_ACTIVE = 0x1,
	OBJST_INVISIBLE = 0x2,
	OBJST_DONE = 0x3,
	OBJST_CURRENT = 0x4,
	OBJST_FAILED = 0x5,
	OBJST_NUMSTATES = 0x6,
};

typedef struct objective_s
{
	int state;
	vec3_t origin;
	int entNum;
	int teamNum;
	int icon;
} objective_t;

typedef enum
{
	HE_TYPE_FREE,
	HE_TYPE_TEXT,
	HE_TYPE_VALUE,
	HE_TYPE_PLAYERNAME,
	HE_TYPE_MAPNAME,
	HE_TYPE_GAMETYPE,
	HE_TYPE_MATERIAL,
	HE_TYPE_TIMER_DOWN,
	HE_TYPE_TIMER_UP,
	HE_TYPE_TENTHS_TIMER_DOWN,
	HE_TYPE_TENTHS_TIMER_UP,
	HE_TYPE_CLOCK_DOWN,
	HE_TYPE_CLOCK_UP,
	HE_TYPE_WAYPOINT,
	HE_TYPE_COUNT
} he_type_t;

enum hudelem_update_t
{
	HUDELEM_UPDATE_ARCHIVAL = 0x1,
	HUDELEM_UPDATE_CURRENT = 0x2,
	HUDELEM_UPDATE_ARCHIVAL_AND_CURRENT = 0x3,
};

typedef struct
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
} hudelem_colorsplit_t;

typedef union
{
	hudelem_colorsplit_t split;
	int rgba;
} hudelem_color_t;

typedef struct hudelem_s
{
	int type;
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
	hudelem_color_t foreground;
} hudelem_t;

typedef struct hudElemState_s
{
	hudelem_t current[MAX_HUDELEMS_CURRENT];
	hudelem_t archival[MAX_HUDELEMS_ARCHIVAL];
} hudElemState_t;

typedef struct
{
	float yaw;
	int timer;
	int transIndex;
	int flags;
} MantleState;

enum statIndex_t
{
	STAT_HEALTH = 0x0,
	STAT_DEAD_YAW = 0x1,
	STAT_MAX_HEALTH = 0x2,
	STAT_IDENT_CLIENT_NUM = 0x3,
	STAT_IDENT_CLIENT_HEALTH = 0x4,
	STAT_SPAWN_COUNT = 0x5,
	MAX_STATS = 0x6,
};

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
	int delta_angles[3];
	int groundEntityNum;
	vec3_t vLadderVec;
	int jumpTime;
	float jumpOriginZ;
	int legsTimer;
	int legsAnim;
	int torsoTimer;
	int torsoAnim;
	int legsAnimDuration;
	int torsoAnimDuration;
	int damageTimer;
	int damageDuration;
	int flinchYaw;
	int movementDir;
	int eFlags;
	int eventSequence;
	int events[4];
	int eventParms[4];
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
	float viewHeightLerpPosAdj;
	vec2_t viewAngleClampBase;
	vec2_t viewAngleClampRange;
	int damageEvent;
	int damageYaw;
	int damagePitch;
	int damageCount;
	int stats[MAX_STATS];
	int ammo[128];
	int ammoclip[128];
	int weapons[2];
	int oldweapons[2];
	char weaponslots[8];
	int weaponrechamber[2];
	int oldweaponrechamber[2];
	vec3_t mins;
	vec3_t maxs;
	float proneDirection;
	float proneDirectionPitch;
	float proneTorsoPitch;
	int viewlocked;
	int viewlocked_entNum;
	int cursorHint;
	int cursorHintString;
	int cursorHintEntIndex;
	int iCompassFriendInfo;
	float fTorsoHeight;
	float fTorsoPitch;
	float fWaistPitch;
	float holdBreathScale;
	int holdBreathTimer;
	MantleState mantleState;
	int entityEventSequence;
	int weapAnim;
	float aimSpreadScale;
	int shellshockIndex;
	int shellshockTime;
	int shellshockDuration;
	objective_t objective[MAX_OBJECTIVES];
	int deltaTime;
	hudElemState_t hud;
} playerState_t;

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
	sessionState_t sessionState;
	int forceSpectatorClient;
	int statusIcon;
	int archiveTime;
	int score;
	int deaths;
	uint16_t pers;
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
	float moveSpeedScaleMultiplier;
	int viewmodelIndex;
	int noSpectate;
	int teamInfo;
	clientState_t state;
	int psOffsetTime;
} clientSession_t;

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
	int accurateCount;
	int accuracy_shots;
	int accuracy_hits;
	int inactivityTime;
	qboolean inactivityWarning;
	int lastVoiceTime;
	int switchTeamTime;
	float currentAimSpreadScale;
	gentity_s *persistantPowerup;
	int portalID;
	int dropWeaponTime;
	int sniperRifleFiredTime;
	float sniperRifleMuzzleYaw;
	int PCSpecialPickedUpCount;
	gentity_s *pLookatEnt;
	int useHoldEntity;
	int useHoldTime;
	int iLastCompassFriendlyInfoEnt;
	int compassPingTime;
	int damageTime;
	float v_dmg_roll;
	float v_dmg_pitch;
	vec3_t swayViewAngles;
	vec3_t swayOffset;
	vec3_t swayAngles;
	vec3_t vLastMoveAng;
	float fLastIdleFactor;
	vec3_t recoilAngles;
	vec3_t recoilSpeed;
	int weapIdleTime;
	int lastServerTime;
	int lastSpawnTime;
};
static_assert((sizeof(gclient_t) == 0x28A4), "ERROR: gclient_t size is invalid!");

extern gclient_t g_clients[];

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
	float triggerDown;
	char fireSnd;
	char fireSndPlayer;
	char stopSnd;
	char stopSndPlayer;
};
static_assert((sizeof(turretInfo_s) == 0x44), "ERROR: turretInfo_s size is invalid!");

struct tagInfo_s
{
	struct gentity_s *parent;
	struct gentity_s *next;
	uint16_t name;
	int index;
	float axis[4][3];
	float parentInvAxis[4][3];
};
static_assert((sizeof(tagInfo_s) == 112), "ERROR: tagInfo_s size is invalid!");

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
	int clipAmmoCount;
	unsigned short index;
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
	int team;
	vec3_t curvature;
	int targetEntNum;
	vec3_t targetOffset;
	int stage;
	int flightMode;
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
	byte attachIgnoreCollision;
	byte handler;
	byte team;
	uint16_t classname;
	uint16_t target;
	uint16_t targetname;
	uint16_t padding;
	int spawnflags;
	int flags;
	int eventTime;
	int freeAfterEvent;
	int unlinkAfterEvent;
	int clipmask;
	int framenum;
	gentity_s *parent;
	int nextthink;
	int health;
	int maxHealth;
	int dmg;
	int count;
	gentity_s *chain;
	union
	{
		struct item_ent_t item;
		struct trigger_ent_t trigger;
		struct mover_ent_t mover;
		struct corpse_ent_t corpse;
		struct missile_ent_t missile;
	};
	tagInfo_s *tagInfo;
	gentity_s *tagChildren;
	byte attachModelNames[8];
	uint16_t attachTagNames[8];
	int useCount;
	gentity_s *nextFree;
};
static_assert((sizeof(gentity_t) == 560), "ERROR: gentity_t size is invalid!");

extern gentity_t g_entities[];

typedef struct
{
	struct gclient_s *clients;
	struct gentity_s *gentities;
	int gentitySize;
	int num_entities;
	struct gentity_s *firstFreeEnt;
	struct gentity_s *lastFreeEnt;
	fileHandle_t logFile;
	int initializing;
	int clientIsSpawning;
	objective_t objectives[16];
	int maxclients;
	int framenum;
	int time;
	int previousTime;
	int frameTime;
	int startTime;
	int teamScores[TEAM_NUM_TEAMS];
	int lastTeammateHealthTime;
	qboolean bUpdateScoresForIntermission;
	int manualNameChange;
	int numConnectedClients;
	int sortedClients[MAX_CLIENTS];
	char voteString[1024];
	char voteDisplayString[1024];
	int voteTime; // 711
	int voteExecuteTime;
	int voteYes;
	int voteNo;
	int numVotingClients;
	byte gap[2072];
	SpawnVar spawnVars;
	int savePersist;
	struct gentity_s *droppedWeaponCue[32];
	float fFogOpaqueDist;
	float fFogOpaqueDistSqrd;
	int remapCount;
	int currentPlayerClone;
	trigger_info_t pendingTriggerList[256];
	trigger_info_t currentTriggerList[256];
	int pendingTriggerListSize;
	int currentTriggerListSize;
	int finished;
	int bPlayerIgnoreRadiusDamage;
	int bPlayerIgnoreRadiusDamageLatched;
	int registerWeapons;
	int bRegisterItems;
	int currentEntityThink;
	int openScriptIOFileHandles[1];
	char *openScriptIOFileBuffers[1];
	com_parse_mark_t currentScriptIOLineMark[1];
} level_locals_t;
static_assert((sizeof(level_locals_t) == 0x3624), "ERROR: level_locals_t size is invalid!");

extern level_locals_t level;

enum fieldtype_t
{
	F_INT,
	F_FLOAT,
	F_LSTRING,
	F_STRING,
	F_VECTOR,
	F_ENTITY,
	F_VECTORHACK,
	F_OBJECT,
	F_MODEL
};

typedef struct game_hudelem_s
{
	hudelem_s elem;
	int clientNum;
	int team;
	int archived;
} game_hudelem_t;

extern game_hudelem_t g_hudelems[];

typedef struct game_hudelem_field_s
{
	const char *name;
	int ofs;
	int type;
	int size;
	byte shift;
	void (*setter)(game_hudelem_s *, int);
	void (*getter)(game_hudelem_s *, int);
} game_hudelem_field_t;

typedef struct game_entity_field_s
{
	const char *name;
	int ofs;
	int type;
	void (*setter)(gentity_s *, int);
} game_entity_field_t;

typedef struct game_client_field_s
{
	const char *name;
	int ofs;
	int type;
	void (*setter)(gclient_s *, const game_client_field_s *);
	void (*getter)(gclient_s *, const game_client_field_s *);
} game_client_field_t;

enum g_class_num_t
{
	CLASS_NUM_ENTITY,
	CLASS_NUM_HUDELEM,
	CLASS_NUM_PATHNODE,
	CLASS_NUM_VEHICLENODE,
	CLASS_NUM_COUNT
};

struct scr_entref_t
{
	uint16_t entnum;
	uint16_t classnum;
};

struct gameTypeScript_t
{
	char pszScript[64];
	char pszName[64];
	int bTeamBased;
};

struct scr_gametype_data_t
{
	int main;
	int startupgametype;
	int playerconnect;
	int playerdisconnect;
	int playerdamage;
	int playerkilled;
	int votecalled;
	int playervote;
	int iNumGameTypes;
	gameTypeScript_t list[32];
};

#include "../bgame/bg_public.h"
typedef struct
{
	int levelscript;
	int gametypescript;
	scr_gametype_data_t gametype;
	int deletestruct;
	int initstructs;
	int createstruct;
	corpseInfo_t playerCorpseInfo[8];
} scr_data_t;

extern scr_data_t g_scr_data;

typedef struct
{
	unsigned short emptystring;
	unsigned short allies;
	unsigned short axis;
	unsigned short current;
	unsigned short damage;
	unsigned short death;
	unsigned short dlight;
	unsigned short done;
	unsigned short empty;
	unsigned short entity;
	unsigned short failed;
	unsigned short fraction;
	unsigned short goal;
	unsigned short grenade;
	unsigned short info_notnull;
	unsigned short invisible;
	unsigned short key1;
	unsigned short key2;
	unsigned short killanimscript;
	unsigned short left;
	unsigned short movedone;
	unsigned short noclass;
	unsigned short normal;
	unsigned short pistol;
	unsigned short plane_waypoint;
	unsigned short player;
	unsigned short position;
	unsigned short primary;
	unsigned short primaryb;
	unsigned short prone;
	unsigned short right;
	unsigned short rocket;
	unsigned short rotatedone;
	unsigned short script_brushmodel;
	unsigned short script_model;
	unsigned short script_origin;
	unsigned short spectator;
	unsigned short stand;
	unsigned short surfacetype;
	unsigned short target_script_trigger;
	unsigned short tempEntity;
	unsigned short touch;
	unsigned short trigger;
	unsigned short trigger_use;
	unsigned short trigger_use_touch;
	unsigned short trigger_damage;
	unsigned short trigger_lookat;
	unsigned short truck_cam;
	unsigned short worldspawn;
	unsigned short binocular_enter;
	unsigned short binocular_exit;
	unsigned short binocular_fire;
	unsigned short binocular_release;
	unsigned short binocular_drop;
	unsigned short begin;
	unsigned short intermission;
	unsigned short menuresponse;
	unsigned short playing;
	unsigned short none;
	unsigned short dead;
	unsigned short auto_change;
	unsigned short manual_change;
	unsigned short freelook;
	unsigned short call_vote;
	unsigned short vote;
	unsigned short snd_enveffectsprio_level;
	unsigned short snd_enveffectsprio_shellshock;
	unsigned short snd_channelvolprio_holdbreath;
	unsigned short snd_channelvolprio_pain;
	unsigned short snd_channelvolprio_shellshock;
	unsigned short tag_flash;
	unsigned short tag_flash_11;
	unsigned short tag_flash_2;
	unsigned short tag_flash_22;
	unsigned short tag_brass;
	unsigned short j_head;
	unsigned short tag_weapon;
	unsigned short tag_player;
	unsigned short tag_camera;
	unsigned short tag_aim;
	unsigned short tag_aim_animated;
	unsigned short tag_origin;
	unsigned short tag_butt;
	unsigned short tag_weapon_right;
	unsigned short back_low;
	unsigned short back_mid;
	unsigned short back_up;
	unsigned short neck;
	unsigned short head;
	unsigned short pelvis;
} scr_const_t;

extern scr_const_t scr_const;

struct entityHandler_t
{
	void (*think)(struct gentity_s *);
	void (*reached)(struct gentity_s *);
	void (*blocked)(struct gentity_s *, struct gentity_s *);
	void (*touch)(struct gentity_s *, struct gentity_s *, int);
	void (*use)(struct gentity_s *, struct gentity_s *, struct gentity_s *);
	void (*pain)(struct gentity_s *, struct gentity_s *, int, const float *, int, const float *, int);
	void (*die)(struct gentity_s *, struct gentity_s *, struct gentity_s *, int, int, const int, const float *, int, int);
	void (*controller)(struct gentity_s *, int *);
	int methodOfDeath;
	int splashMethodOfDeath;
};

extern entityHandler_t entityHandlers[];

extern char riflePriorityMap[];
extern char bulletPriorityMap[];

typedef enum
{
	EV_NONE,
	EV_FOOTSTEP_RUN_DEFAULT,
	EV_FOOTSTEP_RUN_BARK,
	EV_FOOTSTEP_RUN_BRICK,
	EV_FOOTSTEP_RUN_CARPET,
	EV_FOOTSTEP_RUN_CLOTH,
	EV_FOOTSTEP_RUN_CONCRETE,
	EV_FOOTSTEP_RUN_DIRT,
	EV_FOOTSTEP_RUN_FLESH,
	EV_FOOTSTEP_RUN_FOLIAGE,
	EV_FOOTSTEP_RUN_GLASS,
	EV_FOOTSTEP_RUN_GRASS,
	EV_FOOTSTEP_RUN_GRAVEL,
	EV_FOOTSTEP_RUN_ICE,
	EV_FOOTSTEP_RUN_METAL,
	EV_FOOTSTEP_RUN_MUD,
	EV_FOOTSTEP_RUN_PAPER,
	EV_FOOTSTEP_RUN_PLASTER,
	EV_FOOTSTEP_RUN_ROCK,
	EV_FOOTSTEP_RUN_SAND,
	EV_FOOTSTEP_RUN_SNOW,
	EV_FOOTSTEP_RUN_WATER,
	EV_FOOTSTEP_RUN_WOOD,
	EV_FOOTSTEP_RUN_ASPHALT,
	EV_FOOTSTEP_WALK_DEFAULT,
	EV_FOOTSTEP_WALK_BARK,
	EV_FOOTSTEP_WALK_BRICK,
	EV_FOOTSTEP_WALK_CARPET,
	EV_FOOTSTEP_WALK_CLOTH,
	EV_FOOTSTEP_WALK_CONCRETE,
	EV_FOOTSTEP_WALK_DIRT,
	EV_FOOTSTEP_WALK_FLESH,
	EV_FOOTSTEP_WALK_FOLIAGE,
	EV_FOOTSTEP_WALK_GLASS,
	EV_FOOTSTEP_WALK_GRASS,
	EV_FOOTSTEP_WALK_GRAVEL,
	EV_FOOTSTEP_WALK_ICE,
	EV_FOOTSTEP_WALK_METAL,
	EV_FOOTSTEP_WALK_MUD,
	EV_FOOTSTEP_WALK_PAPER,
	EV_FOOTSTEP_WALK_PLASTER,
	EV_FOOTSTEP_WALK_ROCK,
	EV_FOOTSTEP_WALK_SAND,
	EV_FOOTSTEP_WALK_SNOW,
	EV_FOOTSTEP_WALK_WATER,
	EV_FOOTSTEP_WALK_WOOD,
	EV_FOOTSTEP_WALK_ASPHALT,
	EV_FOOTSTEP_PRONE_DEFAULT,
	EV_FOOTSTEP_PRONE_BARK,
	EV_FOOTSTEP_PRONE_BRICK,
	EV_FOOTSTEP_PRONE_CARPET,
	EV_FOOTSTEP_PRONE_CLOTH,
	EV_FOOTSTEP_PRONE_CONCRETE,
	EV_FOOTSTEP_PRONE_DIRT,
	EV_FOOTSTEP_PRONE_FLESH,
	EV_FOOTSTEP_PRONE_FOLIAGE,
	EV_FOOTSTEP_PRONE_GLASS,
	EV_FOOTSTEP_PRONE_GRASS,
	EV_FOOTSTEP_PRONE_GRAVEL,
	EV_FOOTSTEP_PRONE_ICE,
	EV_FOOTSTEP_PRONE_METAL,
	EV_FOOTSTEP_PRONE_MUD,
	EV_FOOTSTEP_PRONE_PAPER,
	EV_FOOTSTEP_PRONE_PLASTER,
	EV_FOOTSTEP_PRONE_ROCK,
	EV_FOOTSTEP_PRONE_SAND,
	EV_FOOTSTEP_PRONE_SNOW,
	EV_FOOTSTEP_PRONE_WATER,
	EV_FOOTSTEP_PRONE_WOOD,
	EV_FOOTSTEP_PRONE_ASPHALT,
	EV_JUMP_DEFAULT,
	EV_JUMP_BARK,
	EV_JUMP_BRICK,
	EV_JUMP_CARPET,
	EV_JUMP_CLOTH,
	EV_JUMP_CONCRETE,
	EV_JUMP_DIRT,
	EV_JUMP_FLESH,
	EV_JUMP_FOLIAGE,
	EV_JUMP_GLASS,
	EV_JUMP_GRASS,
	EV_JUMP_GRAVEL,
	EV_JUMP_ICE,
	EV_JUMP_METAL,
	EV_JUMP_MUD,
	EV_JUMP_PAPER,
	EV_JUMP_PLASTER,
	EV_JUMP_ROCK,
	EV_JUMP_SAND,
	EV_JUMP_SNOW,
	EV_JUMP_WATER,
	EV_JUMP_WOOD,
	EV_JUMP_ASPHALT,
	EV_LANDING_DEFAULT,
	EV_LANDING_BARK,
	EV_LANDING_BRICK,
	EV_LANDING_CARPET,
	EV_LANDING_CLOTH,
	EV_LANDING_CONCRETE,
	EV_LANDING_DIRT,
	EV_LANDING_FLESH,
	EV_LANDING_FOLIAGE,
	EV_LANDING_GLASS,
	EV_LANDING_GRASS,
	EV_LANDING_GRAVEL,
	EV_LANDING_ICE,
	EV_LANDING_METAL,
	EV_LANDING_MUD,
	EV_LANDING_PAPER,
	EV_LANDING_PLASTER,
	EV_LANDING_ROCK,
	EV_LANDING_SAND,
	EV_LANDING_SNOW,
	EV_LANDING_WATER,
	EV_LANDING_WOOD,
	EV_LANDING_ASPHALT,
	EV_LANDING_PAIN_DEFAULT,
	EV_LANDING_PAIN_BARK,
	EV_LANDING_PAIN_BRICK,
	EV_LANDING_PAIN_CARPET,
	EV_LANDING_PAIN_CLOTH,
	EV_LANDING_PAIN_CONCRETE,
	EV_LANDING_PAIN_DIRT,
	EV_LANDING_PAIN_FLESH,
	EV_LANDING_PAIN_FOLIAGE,
	EV_LANDING_PAIN_GLASS,
	EV_LANDING_PAIN_GRASS,
	EV_LANDING_PAIN_GRAVEL,
	EV_LANDING_PAIN_ICE,
	EV_LANDING_PAIN_METAL,
	EV_LANDING_PAIN_MUD,
	EV_LANDING_PAIN_PAPER,
	EV_LANDING_PAIN_PLASTER,
	EV_LANDING_PAIN_ROCK,
	EV_LANDING_PAIN_SAND,
	EV_LANDING_PAIN_SNOW,
	EV_LANDING_PAIN_WATER,
	EV_LANDING_PAIN_WOOD,
	EV_LANDING_PAIN_ASPHALT,
	EV_FOLIAGE_SOUND,
	EV_STANCE_FORCE_STAND,
	EV_STANCE_FORCE_CROUCH,
	EV_STANCE_FORCE_PRONE,
	EV_STEP_VIEW,
	EV_ITEM_PICKUP,
	EV_AMMO_PICKUP,
	EV_NOAMMO,
	EV_EMPTYCLIP,
	EV_EMPTY_OFFHAND,
	EV_RESET_ADS,
	EV_RELOAD,
	EV_RELOAD_FROM_EMPTY,
	EV_RELOAD_START,
	EV_RELOAD_END,
	EV_RAISE_WEAPON,
	EV_PUTAWAY_WEAPON,
	EV_WEAPON_ALT,
	EV_PULLBACK_WEAPON,
	EV_FIRE_WEAPON,
	EV_FIRE_WEAPONB,
	EV_FIRE_WEAPON_LASTSHOT,
	EV_RECHAMBER_WEAPON,
	EV_EJECT_BRASS,
	EV_MELEE_SWIPE,
	EV_FIRE_MELEE,
	EV_PREP_OFFHAND,
	EV_USE_OFFHAND,
	EV_SWITCH_OFFHAND,
	EV_BINOCULAR_ENTER,
	EV_BINOCULAR_EXIT,
	EV_BINOCULAR_FIRE,
	EV_BINOCULAR_RELEASE,
	EV_BINOCULAR_DROP,
	EV_MELEE_HIT,
	EV_MELEE_MISS,
	EV_FIRE_WEAPON_MG42,
	EV_FIRE_QUADBARREL_1,
	EV_FIRE_QUADBARREL_2,
	EV_BULLET_TRACER,
	EV_SOUND_ALIAS,
	EV_SOUND_ALIAS_AS_MASTER,
	EV_BULLET_HIT_SMALL,
	EV_BULLET_HIT_LARGE,
	EV_SHOTGUN_HIT,
	EV_BULLET_HIT_AP,
	EV_BULLET_HIT_CLIENT_SMALL,
	EV_BULLET_HIT_CLIENT_LARGE,
	EV_GRENADE_BOUNCE,
	EV_GRENADE_EXPLODE,
	EV_ROCKET_EXPLODE,
	EV_ROCKET_EXPLODE_NOMARKS,
	EV_CUSTOM_EXPLODE,
	EV_CUSTOM_EXPLODE_NOMARKS,
	EV_BULLET,
	EV_PLAY_FX,
	EV_PLAY_FX_ON_TAG,
	EV_EARTHQUAKE,
	EV_GRENADE_SUICIDE,
	EV_OBITUARY,
	EV_MAX_EVENTS
} entity_event_t;

enum entityType_t
{
	ET_GENERAL = 0x0,
	ET_PLAYER = 0x1,
	ET_PLAYER_CORPSE = 0x2,
	ET_ITEM = 0x3,
	ET_MISSILE = 0x4,
	ET_INVISIBLE = 0x5,
	ET_SCRIPTMOVER = 0x6,
	ET_FX = 0x7,
	ET_LOOP_FX = 0x8,
	ET_TURRET = 0x9,
	ET_EVENTS = 0xA,
};

enum meansOfDeath_t
{
	MOD_UNKNOWN = 0x0,
	MOD_PISTOL_BULLET = 0x1,
	MOD_RIFLE_BULLET = 0x2,
	MOD_GRENADE = 0x3,
	MOD_GRENADE_SPLASH = 0x4,
	MOD_PROJECTILE = 0x5,
	MOD_PROJECTILE_SPLASH = 0x6,
	MOD_MELEE = 0x7,
	MOD_HEAD_SHOT = 0x8,
	MOD_CRUSH = 0x9,
	MOD_TELEFRAG = 0xA,
	MOD_FALLING = 0xB,
	MOD_SUICIDE = 0xC,
	MOD_TRIGGER_HURT = 0xD,
	MOD_EXPLOSIVE = 0xE,
	MOD_NUM = 0xF,
};

struct AntilagClientStore
{
	vec3_t realClientPositions[64];
	bool clientMoved[64];
};

enum itemType_t
{
	IT_BAD = 0x0,
	IT_WEAPON = 0x1,
	IT_AMMO = 0x2,
	IT_HEALTH = 0x3,
	IT_HOLDABLE = 0x4,
};

struct spawn_t
{
	const char *name;
	void (*spawn)(gentity_s *ent);
};

struct useList_t
{
	gentity_s *ent;
	float score;
};

#define KEY_MASK_NONE        	0

#define KEY_MASK_FORWARD        127
#define KEY_MASK_BACK           -127
#define KEY_MASK_MOVERIGHT      127
#define KEY_MASK_MOVELEFT       -127

#define KEY_MASK_FIRE           1
#define KEY_MASK_MELEE          4
#define KEY_MASK_USE            8
#define KEY_MASK_RELOAD         16
#define KEY_MASK_USERELOAD      32
#define KEY_MASK_LEANLEFT       64
#define KEY_MASK_LEANRIGHT      128
#define KEY_MASK_PRONE          256
#define KEY_MASK_CROUCH         512
#define KEY_MASK_JUMP           1024
#define KEY_MASK_ADS_MODE       4096
#define KEY_MASK_MELEE_BREATH   32772
#define KEY_MASK_HOLDBREATH     32768
#define KEY_MASK_FRAG           65536
#define KEY_MASK_SMOKE          131072

#define EF_VOTED 0x00100000
#define EF_TALK 0x00200000
#define EF_TAUNT 0x00400000
#define EF_FIRING 0x00000020
#define EF_MANTLE 0x00004000
#define EF_CROUCHING 0x00000004
#define EF_PRONE 0x00000008
#define EF_DEAD 0x00020000
#define EF_USETURRET 0x00000300
#define EF_AIMDOWNSIGHT 0x00040000

#define DFLAGS_RADIUS				 1
#define DFLAGS_NO_ARMOR				 2
#define DFLAGS_NO_KNOCKBACK			 4
#define DFLAGS_NO_TEAM_PROTECTION	 8
#define DFLAGS_NO_PROTECTION		 16
#define DFLAGS_PASSTHRU				 32

void HudElem_SetEnumString(game_hudelem_t *hud, const game_hudelem_field_t *f, const char **names, int nameCount);
void HudElem_SetFontScale(game_hudelem_t *hud, int offset);
void HudElem_SetFont(game_hudelem_t *hud, int offset);
void HudElem_SetAlignX(game_hudelem_t *hud, int offset);
void HudElem_SetAlignY(game_hudelem_t *hud, int offset);
void HudElem_SetHorzAlign(game_hudelem_t *hud, int offset);
void HudElem_SetVertAlign(game_hudelem_t *hud, int offset);
void HudElem_SetColor(game_hudelem_t *hud, int offset);
void HudElem_SetAlpha(game_hudelem_t *hud, int offset);
void HudElem_SetLocalizedString(game_hudelem_t *hud, int offset);
void HudElem_SetBoolean(game_hudelem_t *hud, int offset);
void HudElem_AddString(game_hudelem_t *hud, const game_hudelem_field_t *field, const char **names);
void HudElem_GetFont(game_hudelem_t *hud, int offset);
void HudElem_GetAlignX(game_hudelem_t *hud, int offset);
void HudElem_GetAlignY(game_hudelem_t *hud, int offset);
void HudElem_GetHorzAlign(game_hudelem_t *hud, int offset);
void HudElem_GetVertAlign(game_hudelem_t *hud, int offset);
void HudElem_GetColor(game_hudelem_t *hud, int offset);
void HudElem_GetAlpha(game_hudelem_t *hud, int offset);
void HudElem_ClearTypeSettings(game_hudelem_t *hud);
void HudElem_SetDefaults(game_hudelem_t *hud);
void HudElem_UpdateClient(gclient_s *client, int clientNum, byte which);
void HudElem_DestroyAll();
void HudElem_ClientDisconnect(gentity_t *ent);
void HudElem_Free(game_hudelem_t *hud);
game_hudelem_t *HudElem_Alloc(int clientNum, int teamNum);
void Scr_GetHudElemField(int entnum, int offset);
void Scr_SetHudElemField(int entnum, int offset);
void Scr_FreeHudElemConstStrings(game_hudelem_s *hud);
void GScr_AddFieldsForHudElems();

void HECmd_SetText(scr_entref_t entRef);
void HECmd_SetPlayerNameString(scr_entref_t entRef);
void HECmd_SetGameTypeString(scr_entref_t entRef);
void HECmd_SetMapNameString(scr_entref_t entRef);
void HECmd_SetShader(scr_entref_t entRef);
void HECmd_SetTimer(scr_entref_t entRef);
void HECmd_SetTimerUp(scr_entref_t entRef);
void HECmd_SetTenthsTimer(scr_entref_t entRef);
void HECmd_SetTenthsTimerUp(scr_entref_t entRef);
void HECmd_SetClock(scr_entref_t entRef);
void HECmd_SetClockUp(scr_entref_t entRef);
void HECmd_SetValue(scr_entref_t entRef);
void HECmd_SetWaypoint(scr_entref_t entRef);
void HECmd_FadeOverTime(scr_entref_t entRef);
void HECmd_ScaleOverTime(scr_entref_t entRef);
void HECmd_MoveOverTime(scr_entref_t entRef);
void HECmd_Reset(scr_entref_t entRef);
void HECmd_Destroy(scr_entref_t entRef);

char *ConcatArgs( int start );
qboolean ConsoleCommand();

void Scr_LocalizationError(int iParm, const char *pszErrorMessage);
void Scr_ConstructMessageString(int firstParmIndex, int lastParmIndex, const char *errorContext, char *string, unsigned int stringLimit);
void CalculateRanks();
float G_GetFogOpaqueDistSqrd();
int G_FindConfigstringIndex(const char *name, int start, int max, int create, const char *errormsg);
const char* G_ModelName(int modelIndex);
int G_LocalizedStringIndex(const char *string);
int G_ShaderIndex(const char *string);
int G_TagIndex(const char *name);
int G_SoundAliasIndex(const char *name);
int G_ShellShockIndex(const char *name);
int G_EffectIndex(const char *name);
XModel* G_GetModel(int modelIndex);
void G_SafeDObjFree(gentity_s *ent);
void G_DObjUpdate(gentity_s *ent);
unsigned int G_ModelIndex(const char *name);
void G_OverrideModel(int modelIndex, const char *defaultModelName);
void G_RunThink(gentity_s *ent);
void G_CheckHitTriggerDamage(gentity_s *pActivator, float *vStart, float *vEnd, int iDamage, unsigned int iMOD);
qboolean OnSameTeam(gentity_s *ent1, gentity_s *ent2);
void G_AntiLagRewindClientPos(int gameTime, AntilagClientStore *antilagStore);
void G_AntiLag_RestoreClientPos(AntilagClientStore *antilagStore);
void G_GrenadeTouchTriggerDamage(gentity_s *pActivator, float *vStart, float *vEnd, int iDamage, int iMOD);

void FireWeaponMelee(gentity_s *ent);
void G_UseOffHand(gentity_s *ent);
int G_RadiusDamage(float *origin, gentity_s *inflictor, gentity_s *attacker, float fInnerDamage, float fOuterDamage, float radius, gentity_s *ignore, int mod);
void G_RunItem(gentity_s *ent);
void G_SetFixedLink(gentity_s *ent, int eAngles);
void G_RunClient(gentity_s *ent);
void G_RunMover(gentity_s *ent);
void G_RunMissile(gentity_s *ent);
void G_RunFrameForEntity(gentity_s *ent);

gentity_s* G_TestEntityPosition(gentity_s *ent, float *origin);
qboolean G_TryPushingEntity( gentity_t *check, gentity_t *pusher, vec3_t move, vec3_t amove );

int G_CallSpawnEntity(gentity_s *ent);
void G_CallSpawn(void);

void ClientEvents(gentity_s *ent, int oldEventSequence);
void G_AddPlayerMantleBlockage(float *endPos, int duration, pmove_t *pm);
int G_DObjGetWorldTagMatrix(gentity_s *ent, unsigned int tagName, float (*tagMat)[3]);
gentity_s* Drop_Weapon(gentity_s *entity, int weapon, unsigned int tag);
gitem_s* G_GetItemForClassname(const char *name);
gitem_s* G_FindItem(const char *pickupName);
void G_SpawnItem(gentity_s *ent, const gitem_s *item);

void G_DObjCalcPose(gentity_s *ent);
void G_DObjCalcBone(gentity_s *ent, int boneIndex);

qboolean G_UpdateClientInfo(gentity_s *ent);
void G_PlayerStateToEntityStateExtrapolate(playerState_s *ps, entityState_s *s, int time, int snap);
void G_FreeEntityDelay(gentity_s *ent);

gentity_t* GetEntity(scr_entref_t entref);
game_hudelem_t* HECmd_GetHudElem(scr_entref_t entref);
gentity_t* Scr_GetEntity(unsigned int index);
void Scr_AddEntity(gentity_t *ent);
void Scr_AddHudElem(game_hudelem_t *hud);
void Scr_FreeHudElem(game_hudelem_s *hud);

char* Scr_GetGameTypeNameForScript(const char *pszGameTypeScript);
qboolean Scr_IsValidGameType(const char *pszGameType);

void ClientCommand( int clientNum );
bool G_ParseWeaponAccurayGraphs(WeaponDef *weaponDef);
int G_XModelBad(int index);

const char *vtos( const vec3_t v );
const char *vtosf( const vec3_t v );

void QDECL G_LogPrintf( const char *fmt, ... );

int IsItemRegistered(unsigned int iItemIndex);
void RegisterItem(unsigned int index, int global);
void SaveRegisteredItems();
void SaveRegisteredWeapons();
void ClearRegisteredItems();
int G_RegisterWeapon(int weapIndex);
void G_SetEquippedOffHand(int clientNum, int offHandIndex);
int G_GivePlayerWeapon(playerState_s *ps, int weapon);
int G_GetWeaponIndexForName(const char *name);
void ExitLevel();

int Add_Ammo(gentity_s *pSelf, int weaponIndex, int count, int fillClip);
void G_AddPredictableEvent(gentity_s *ent, int event, int eventParm);
void Touch_Item(gentity_s *ent, gentity_s *other, int touched);
void G_UpdateTeamScoresForIntermission();
void CheckTeamStatus();
int G_DObjUpdateServerTime(gentity_s *ent, int bNotify);
void G_RunFrame(int time);

void Scr_LoadGameType();
void Scr_LoadLevel();
void Scr_StartupGameType();

void G_LoadStructs();
int G_GetEntityToken(char *buffer, int bufferSize);
char* G_AddSpawnVarToken(char *string, SpawnVar *spawnVar);
int G_ParseSpawnVars(SpawnVar *spawnVar);
void G_SpawnEntitiesFromString();

void G_InitObjectives();
void G_ProcessIPBans( void );
void G_SetupWeaponDef();

void G_InitGame(int levelTime, int randomSeed, int restart, int registerDvars);
void G_ShutdownGame(int freeScripts);

snd_alias_list_t* Com_FindSoundAlias(const char *name);
void G_AnimScriptSound(int clientNum, snd_alias_list_t *aliasList);

void Scr_ReadOnlyField(gentity_s *ent, int offset);
void Scr_SetOrigin(gentity_s *ent, int offset);
void Scr_SetAngles(gentity_s *ent, int offset);
void Scr_SetHealth(gentity_s *ent, int offset);
unsigned int GScr_AllocString(const char *string);
int GScr_GetHeadIconIndex(const char *pszIcon);
int GScr_GetStatusIconIndex(const char *pszIcon);
int G_GetHintStringIndex(int *piIndex, const char *pszString);
int Scr_SetObjectField(unsigned int classnum, int entnum, int offset);
void Scr_GetObjectField(unsigned int classnum, int entnum, int offset);
int Scr_SetEntityField(int entnum, int offset);
void Scr_GetEntityField(int entnum, int offset);
void Scr_Notify(gentity_s *ent, unsigned short stringValue, unsigned int paramcount);
unsigned short Scr_ExecEntThread(gentity_s *ent, int handle, unsigned int paramcount);
void Scr_PlayerConnect(gentity_s *self);
void G_InitGentity(gentity_s *ent);
void G_PrintEntities();
gentity_s* G_Spawn(void);
gentity_s* G_TempEntity(vec3_t origin, int event);
void Bullet_Fire(gentity_s *attacker, float spread, weaponParms *wp, const gentity_s *ent, int gameTime);
gentity_s* fire_grenade(gentity_s *parent, float *start, float *dir, int grenadeWPID, int time);
void Weapon_Throw_Grenade(gentity_s *ent, int grenType, weaponParms *wp);
void gunrandom(float *x, float *y);
gentity_s* fire_rocket(gentity_s *parent, float *start, float *dir);
void Weapon_RocketLauncher_Fire(gentity_s *ent, float spread, weaponParms *targetOffset);
void G_SetOrigin(gentity_s *ent, const float *origin);
void G_SetAngle(gentity_s *ent, const float *angle);
void G_PlayerEvent(int clientNum, int event);
int GetFollowPlayerState(int clientNum, playerState_s *ps);
clientState_t* G_GetClientState(int num);
int G_GetClientArchiveTime(int clientindex);
void G_SetClientArchiveTime(int clindex, int time);

void G_TraceCapsule(trace_t *results, const float *start, const float *mins, const float *maxs, const float *end, int passEntityNum, int contentmask);
int G_TraceCapsuleComplete(const float *start, const float *mins, const float *maxs, const float *end, int passEntityNum, int contentmask);
void G_LocationalTrace(trace_t *results, const float *start, const float *end, int passentitynum, int contentmask, char *priorityMap);
int G_LocationalTracePassed(const float *start, const float *end, int passEntityNum, int contentmask);
void G_SightTrace(int *hitNum, const float *start, const float *end, int passEntityNum, int contentmask);
gentity_s* G_FX_VisibilityTrace(trace_t *trace, const float *start, const float *end, int passentitynum, int contentmask, char *priorityMap, float *forwardAngles);
void G_Trigger(gentity_s *self, gentity_s *other);
void Player_UpdateLookAtEntity(gentity_s *ent);
void Player_UpdateCursorHints(gentity_s *ent);
void P_DamageFeedback(gentity_s *player);

void Scr_ParseGameTypeList();

void SetClientViewAngle(gentity_s *ent, const float *angle);
void hook_ClientUserinfoChanged(int clientNum);
void ClientUserinfoChanged(int clientNum);
void ClientImpacts(gentity_s *ent, pmove_t *pm);
void G_TouchTriggers(gentity_s *ent);
DObjAnimMat* G_DObjGetLocalTagMatrix(gentity_s *ent, unsigned int tagName);
int G_DObjGetWorldTagPos(gentity_s *ent, unsigned int tagName, float *pos);
void G_GetPlayerViewDirection(const gentity_s *ent, float *forward, float *right, float *up);
void G_GetPlayerViewOrigin(gentity_s *ent, float *origin);
void FireWeapon(gentity_s *ent, int gametime);
void Weapon_Melee(gentity_s *ent, weaponParms *wp, float range, float width, float height);

float G_GetWeaponHitLocationMultiplier(int hitLoc, int weapon);
unsigned short G_GetHitLocationString(int hitLoc);
int G_GetHitLocationIndexFromString(unsigned short sString);
void G_ParseHitLocDmgTable();
void StopFollowing(gentity_s *ent);
void ClientDisconnect(int clientNum);
int ClientInactivityTimer(gclient_s *client);
void ClientIntermissionThink(gentity_s *ent);
qboolean G_ClientCanSpectateTeam(gclient_s *client, int team);
int Cmd_FollowCycle_f(gentity_s *ent, int dir);
void SpectatorThink(gentity_s *ent, usercmd_s *ucmd);
void ClientThink_real(gentity_s *ent, usercmd_s *ucmd);
void ClientThink(int clientNum);
void G_SetLastServerTime(int clientNum, int lastServerTime);
void ClientBegin(unsigned int clientNum);
const char* ClientConnect(unsigned int clientNum, unsigned int scriptPersId);
int G_GetClientScore(int clientNum);
int G_GetSavePersist();
void G_SetSavePersist(int savepersist);
void G_FreeAnimTreeInstances();

#include "../server/server.h"
void G_BroadcastVoice(gentity_s *talker, VoicePacket_t *voicePacket);

void GScr_AddFieldsForEntity();
void GScr_AddFieldsForRadiant();

void Scr_PlayerDamage(gentity_s *self, gentity_s *inflictor, gentity_s *attacker, int damage, int dflags, unsigned int meansOfDeath, int iWeapon, const float *vPoint, const float *vDir, int hitLoc, int timeOffset);
void Scr_PlayerKilled(gentity_s *self, gentity_s *inflictor, gentity_s *attacker, int damage, unsigned int meansOfDeath, int iWeapon, const float *vDir, int hitLoc, int psTimeOffset, int deathAnimDuration);
void Scr_PlayerDisconnect(gentity_s *self);

void Scr_PlayerVote(gentity_s *self, const char *option);
void Scr_VoteCalled(gentity_s *self, const char *command, const char *param1, const char *param2);

void G_Damage(gentity_s *self, gentity_s *inflictor, gentity_s *ent, const float *vDir,const float *vPoint, int value, int dflags, int meansOfDeath, int hitLoc, int timeOffset);

float G_random();
float G_crandom();

void G_AddEvent(gentity_s *ent, int event, unsigned int eventParm);
void TeleportPlayer(gentity_s *player, float *origin, float *angles);
void G_EntUnlink(gentity_s *ent);
void G_CorpseFree(gentity_s *ent);
void G_FreeEntity(gentity_s *ent);

void G_EntDetachAll(gentity_s *ent);
int G_EntDetach(gentity_s *ent, const char *modelName, unsigned int tagName);
int G_EntAttach(gentity_s *ent, const char *modelName, unsigned int tagName, int ignoreCollision);
int G_EntLinkToWithOffset(gentity_s *ent, gentity_s *parent, unsigned int tagName, const float *originOffset, const float *anglesOffset);
int G_EntLinkTo(gentity_s *ent, gentity_s *parent, unsigned int tagName);

void G_RunCorpseMove(gentity_s *ent);
void G_RunCorpse(gentity_s *ent);
void G_SetModel(gentity_s *ent, const char *modelName);
void G_SetConstString(unsigned short *to, const char *from);
void G_setfog(const char *fogstring);

void Player_UpdateActivate(gentity_s *ent);
void LookAtKiller(gentity_s *self, gentity_s *inflictor, gentity_s *attacker);
void DeathmatchScoreboardMessage(gentity_s *ent);
void Cmd_Score_f(gentity_s *ent);
void player_die(gentity_s *self, gentity_s *inflictor, gentity_s *attacker, int damage, int meansOfDeath, int iWeapon, const float *vDir, int hitLoc, int psTimeOffset);
gentity_s* LaunchItem(const gitem_s *item, float *origin, float *angles, int ownerNum);
unsigned int G_NewString(const char *string);
void G_ParseEntityFields(gentity_s *ent);
int G_SpawnInt(const char *key, const char *defaultString, int *out);
int G_SpawnFloat(const char *key, const char *defaultString, float *out);
int G_SpawnVector(const char *key, const char *defaultString, float *out);
int G_SpawnString(const char *key, const char *defaultString, const char **out);

void G_PlaySoundAlias(gentity_s *ent, byte alias);
qboolean G_IsPlaying(gentity_s *ent);

void G_InitTurrets();
void G_SpawnTurret(gentity_s *ent, const char *weaponName);
void turret_think_client(gentity_s *self);

void trigger_use_shared(gentity_s *self);
qboolean LogAccuracyHit(gentity_s *target, gentity_s *attacker);

void SP_info_null(gentity_s *self);
void SP_info_notnull(gentity_s *self);
void SP_trigger_multiple(gentity_s *self);
void SP_trigger_radius(gentity_s *ent);
void SP_trigger_disk(gentity_s *ent);
void SP_trigger_hurt(gentity_s *self);
void SP_trigger_once(gentity_s *ent);
void SP_light(gentity_s *self);
void SP_misc_model(gentity_s *self);
void SP_turret(gentity_s *ent);
void SP_corona(gentity_s *self);
void SP_trigger_use(gentity_s *self);
void SP_trigger_use_touch(gentity_s *self);
void SP_trigger_damage(gentity_s *ent);
void SP_trigger_lookat(gentity_s *ent);
void SP_script_brushmodel(gentity_s *ent);
void SP_script_model(gentity_s *ent);
void SP_script_origin(gentity_s *ent);

qboolean G_IsTurretUsable(gentity_s *useEnt, gentity_s *playerEnt);
void G_ClientStopUsingTurret(gentity_s *self);
void G_FreeTurret(gentity_s *ent);
void G_PlayerTurretPositionAndBlend(gentity_s *ent, gentity_s *pTurretEnt);
void G_GeneralLink(gentity_s *ent);

void (*Player_GetMethod(const char **pName))(scr_entref_t);
void (*ScriptEnt_GetMethod(const char **pName))(scr_entref_t);
void (*BuiltIn_GetMethod(const char **pName, int *type))(scr_entref_t);
void (*HudElem_GetMethod(const char **pName))(scr_entref_t);
void (*Scr_GetMethod(const char **pName, int *type))(scr_entref_t);
void (*Scr_GetFunction(const char **pName, int *type))();

void ClientScr_ReadOnly(gclient_s *pSelf, const game_client_field_t *pField);
void ClientScr_SetSessionTeam(gclient_s *pSelf, const game_client_field_t *pField);
void ClientScr_SetSessionState(gclient_s *pSelf, const game_client_field_t *pField);
void ClientScr_SetMaxHealth(gclient_s *pSelf, const game_client_field_t *pField);
void ClientScr_SetScore(gclient_s *pSelf, const game_client_field_t *pField);
void ClientScr_SetStatusIcon(gclient_s *pSelf, const game_client_field_t *pField);
void ClientScr_SetHeadIcon(gclient_s *pSelf, const game_client_field_t *pField);
void ClientScr_SetHeadIconTeam(gclient_s *pSelf, const game_client_field_t *pField);
void ClientScr_SetSpectatorClient(gclient_s *pSelf, const game_client_field_t *pField);
void ClientScr_SetArchiveTime(gclient_s *pSelf, const game_client_field_t *pField);
void ClientScr_SetPSOffsetTime(gclient_s *pSelf, const game_client_field_t *pField);
void ClientScr_GetSessionTeam(gclient_s *pSelf, const game_client_field_t *pField);
void ClientScr_GetSessionState(gclient_s *pSelf, const game_client_field_t *pField);
void ClientScr_GetStatusIcon(gclient_s *pSelf, const game_client_field_t *pField);
void ClientScr_GetHeadIcon(gclient_s *pSelf, const game_client_field_t *pField);
void ClientScr_GetHeadIconTeam(gclient_s *pSelf, const game_client_field_t *pField);
void ClientScr_GetArchiveTime(gclient_s *pSelf, const game_client_field_t *pField);
void ClientScr_GetPSOffsetTime(gclient_s *pSelf, const game_client_field_t *pField);
void Scr_GetClientField(gclient_s *client, int offset);
void Scr_SetClientField(gclient_s *client, int offset);
void Scr_FreeEntity(gentity_s *ent);
void GScr_AddFieldsForClient();

void GScr_LoadConsts();
void GScr_LoadScripts();
void GScr_FreeScripts();

void G_RegisterDvars();

void Touch_Multi(gentity_s *self, gentity_s *other, int touched);
void hurt_use(gentity_s *self, gentity_s *ent, gentity_s *other);
void Use_trigger_damage(gentity_s *pEnt, gentity_s *pOther, gentity_s *pActivator);
void Pain_trigger_damage(gentity_s *pSelf, gentity_s *pAttacker, int iDamage, const float *vPoint, int iMod, const float *vDir, int hitLoc);
void Die_trigger_damage(gentity_s *pSelf, gentity_s *pInflictor, gentity_s *pAttacker, int iDamage, int iMod, int iWeapon, const float *vDir, int hitLoc, int psTimeOffset);
void Reached_ScriptMover(gentity_s *pEnt);
void G_ExplodeMissile(gentity_s *ent);
void Touch_Item_Auto(gentity_s *ent, gentity_s *other, int touched);
void G_PlayerController(gentity_s *self, int *partBits);
void BodyEnd(gentity_s *ent);
void turret_think(gentity_s *self);
void G_SelectWeaponIndex(int clientNum, int iWeaponIndex);
void G_Say(gentity_s *ent, gentity_s *target, int mode, const char *chatText);
void GScr_AddEntity(gentity_s *pEnt);
gentity_s* Drop_Item(gentity_s *ent, const gitem_s *item, float angle, int novelocity);
int G_IndexForMeansOfDeath(const char *name);
int GScr_GetScriptMenuIndex(const char *pszMenu);
void Scr_MakeGameMessage(int iClientNum, const char *pszCmd);
void hurt_touch(gentity_s *self, gentity_s *other, int touched);
void turret_think_init(gentity_s *ent);
void turret_use(gentity_s *self, gentity_s *owner, gentity_s *other);
void use_trigger_use(gentity_s *pSelf, gentity_s *pEnt, gentity_s *pOther);
void turret_controller(gentity_s *self, int *partBits);
void FinishSpawningItem(gentity_s *ent);
void DroppedItemClearOwner(gentity_s *pSelf);


void SpectatorClientEndFrame(gentity_s *ent);
int StuckInClient(gentity_s *self);
void ClientEndFrame(gentity_s *entity);
void ClientSpawn(gentity_s *ent, const float *spawn_origin, const float *spawn_angles);
void G_UpdatePlayerContents(gentity_s *ent);
gentity_s* G_SpawnPlayerClone();
int G_GetFreePlayerCorpseIndex();
void G_GetItemClassname(const gitem_s *item, unsigned short *out);



void print();
void println();
void iprintln();
void iprintlnbold();
void print3d();
void GScr_line();
void Scr_GetEnt();
void Scr_GetEntArray();
void GScr_Spawn();
void GScr_SpawnTurret();
void GScr_PrecacheTurret();
void Scr_AddStruct();
void assertCmd();
void assertexCmd();
void assertmsgCmd();
void GScr_IsDefined();
void GScr_IsString();
void GScr_IsAlive();
void GScr_GetDvar();
void GScr_GetDvarInt();
void GScr_GetDvarFloat();
void GScr_SetDvar();
void GScr_GetTime();
void Scr_GetEntByNum();
void Scr_GetWeaponModel();
void GScr_GetAnimLength();
void GScr_AnimHasNotetrack();
void GScr_GetBrushModelCenter();
void Scr_Objective_Add();
void Scr_Objective_Delete();
void Scr_Objective_State();
void Scr_Objective_Icon();
void Scr_Objective_Position();
void Scr_Objective_OnEntity();
void Scr_Objective_Current();
void Scr_BulletTrace();
void Scr_BulletTracePassed();
void Scr_SightTracePassed();
void Scr_PhysicsTrace();
void GScr_GetMoveDelta();
void GScr_GetAngleDelta();
void GScr_GetNorthYaw();
void Scr_RandomInt();
void Scr_RandomFloat();
void Scr_RandomIntRange();
void Scr_RandomFloatRange();
void GScr_sin();
void GScr_cos();
void GScr_tan();
void GScr_asin();
void GScr_acos();
void GScr_atan();
void GScr_CastInt();
void Scr_Distance();
void Scr_DistanceSquared();
void Scr_Length();
void Scr_LengthSquared();
void Scr_Closer();
void Scr_VectorDot();
void Scr_VectorNormalize();
void Scr_VectorToAngles();
void Scr_AnglesToUp();
void Scr_AnglesToRight();
void Scr_AnglesToForward();
void Scr_IsSubStr();
void Scr_GetSubStr();
void Scr_ToLower();
void Scr_StrTok();
void Scr_MusicPlay();
void Scr_MusicStop();
void Scr_SoundFade();
void Scr_AmbientPlay();
void Scr_AmbientStop();
void Scr_PrecacheModel();
void Scr_PrecacheShellShock();
void Scr_PrecacheItem();
void Scr_PrecacheShader();
void Scr_PrecacheString();
void Scr_PrecacheRumble();
void Scr_LoadFX();
void Scr_PlayFX();
void Scr_PlayFXOnTag();
void Scr_PlayLoopedFX();
void Scr_SetLinearFog();
void Scr_SetExponentialFog();
void Scr_GrenadeExplosionEffect();
void GScr_RadiusDamage();
void GScr_SetPlayerIgnoreRadiusDamage();
void GScr_GetNumParts();
void GScr_GetPartName();
void GScr_Earthquake();
void GScr_NewHudElem();
void GScr_NewClientHudElem();
void GScr_NewTeamHudElem();
void GScr_IsPlayer();
void GScr_IsPlayerNumber();
void GScr_SetWinningPlayer();
void GScr_SetWinningTeam();
void GScr_Announcement();
void GScr_ClientAnnouncement();
void GScr_GetTeamScore();
void GScr_SetTeamScore();
void GScr_SetClientNameMode();
void GScr_UpdateClientNames();
void GScr_GetTeamPlayersAlive();
void GScr_Objective_Team();
void GScr_LogPrint();
void GScr_WorldEntNumber();
void GScr_Obituary();
void GScr_positionWouldTelefrag();
void GScr_getStartTime();
void GScr_PrecacheMenu();
void GScr_PrecacheStatusIcon();
void GScr_PrecacheHeadIcon();
void GScr_MapRestart();
void GScr_ExitLevel();
void GScr_AddTestClient();
void GScr_MakeDvarServerInfo();
void GScr_SetArchive();
void GScr_AllClientsPrint();
void GScr_ClientPrint();
void GScr_MapExists();
void GScr_IsValidGameType();
void GScr_MatchEnd();
void GScr_SetPlayerTeamRank();
void GScr_SendXboxLiveRanks();
void GScr_SetVoteString();
void GScr_SetVoteTime();
void GScr_SetVoteYesCount();
void GScr_SetVoteNoCount();
void GScr_OpenFile();
void GScr_CloseFile();
void GScr_FPrintln();
void GScr_FReadLn();
void GScr_FGetArg();
void GScr_KickPlayer();
void GScr_BanPlayer();
void GScr_LoadMap();
void Scr_PlayRumbleOnPos();
void Scr_PlayLoopRumbleOnPos();
void Scr_StopAllRumbles();
void ScrCmd_SoundExists();
void Scr_IsSplitscreen();
void GScr_EndXboxLiveLobby();


void ScrCmd_attach(scr_entref_t entref);
void ScrCmd_detach(scr_entref_t entref);
void ScrCmd_detachAll(scr_entref_t entref);
void ScrCmd_GetAttachSize(scr_entref_t entref);
void ScrCmd_GetAttachModelName(scr_entref_t entref);
void ScrCmd_GetAttachTagName(scr_entref_t entref);
void ScrCmd_GetAttachIgnoreCollision(scr_entref_t entref);
void GScr_GetAmmoCount(scr_entref_t entref);
void ScrCmd_GetClanId(scr_entref_t entref);
void ScrCmd_GetClanName(scr_entref_t entref);
void ScrCmd_GetClanDescription(scr_entref_t entref);
void ScrCmd_GetClanMotto(scr_entref_t entref);
void ScrCmd_GetClanURL(scr_entref_t entref);
void ScrCmd_LinkTo(scr_entref_t entref);
void ScrCmd_Unlink(scr_entref_t entref);
void ScrCmd_EnableLinkTo(scr_entref_t entref);
void ScrCmd_GetOrigin(scr_entref_t entref);
void ScrCmd_GetEye(scr_entref_t entref);
void ScrCmd_UseBy(scr_entref_t entref);
void Scr_SetStableMissile(scr_entref_t entref);
void ScrCmd_IsTouching(scr_entref_t entref);
void ScrCmd_PlaySound(scr_entref_t entref);
void ScrCmd_PlaySoundAsMaster(scr_entref_t entref);
void ScrCmd_PlayLoopSound(scr_entref_t entref);
void ScrCmd_StopLoopSound(scr_entref_t entref);
void ScrCmd_PlayRumble(scr_entref_t entref);
void ScrCmd_PlayLoopRumble(scr_entref_t entref);
void ScrCmd_StopRumble(scr_entref_t entref);
void ScrCmd_Delete(scr_entref_t entref);
void ScrCmd_SetModel(scr_entref_t entref);
void ScrCmd_GetNormalHealth(scr_entref_t entref);
void ScrCmd_SetNormalHealth(scr_entref_t entref);
void ScrCmd_Show(scr_entref_t entref);
void ScrCmd_Hide(scr_entref_t entref);
void ScrCmd_ShowToPlayer(scr_entref_t entref);
void ScrCmd_SetContents(scr_entref_t entRef);
void GScr_SetCursorHint(scr_entref_t entref);
void GScr_SetHintString(scr_entref_t entref);
void GScr_ShellShock(scr_entref_t entref);
void GScr_StopShellShock(scr_entref_t entref);
void GScr_ViewKick(scr_entref_t entref);
void GScr_LocalToWorldCoords(scr_entref_t entref);
void GScr_SetRightArc(scr_entref_t entref);
void GScr_SetLeftArc(scr_entref_t entref);
void GScr_SetTopArc(scr_entref_t entref);
void GScr_SetBottomArc(scr_entref_t entref);
void GScr_GetEntityNumber(scr_entref_t entref);
void GScr_EnableGrenadeTouchDamage(scr_entref_t entref);
void GScr_DisableGrenadeTouchDamage(scr_entref_t entref);
void GScr_EnableGrenadeBounce(scr_entref_t entref);
void GScr_DisableGrenadeBounce(scr_entref_t entref);
void GScr_EnableAimAssist(scr_entref_t entref);
void GScr_DisableAimAssist(scr_entref_t entref);
void GScr_PlaceSpawnPoint(scr_entref_t entref);
void GScr_UpdateScores(scr_entref_t entref);
void GScr_SetTeamForTrigger(scr_entref_t entref);
void GScr_ClientClaimTrigger(scr_entref_t entref);
void GScr_ClientReleaseTrigger(scr_entref_t entref);
void GScr_ReleaseClaimedTrigger(scr_entref_t entref);



void ScriptEntCmd_MoveTo(scr_entref_t entref);
void ScriptEntCmd_MoveX(scr_entref_t entref);
void ScriptEntCmd_MoveY(scr_entref_t entref);
void ScriptEntCmd_MoveZ(scr_entref_t entref);
void ScriptEntCmd_GravityMove(scr_entref_t entRef);
void ScriptEntCmd_RotateTo(scr_entref_t entref);
void ScriptEntCmd_RotatePitch(scr_entref_t entref);
void ScriptEntCmd_RotateYaw(scr_entref_t entref);
void ScriptEntCmd_RotateRoll(scr_entref_t entref);
void ScriptEntCmd_RotateVelocity(scr_entref_t entref);
void ScriptEntCmd_Solid(scr_entref_t entRef);
void ScriptEntCmd_NotSolid(scr_entref_t entRef);





void PlayerCmd_giveWeapon(scr_entref_t entRef);
void PlayerCmd_takeWeapon(scr_entref_t entref);
void PlayerCmd_takeAllWeapons(scr_entref_t entref);
void PlayerCmd_getCurrentWeapon(scr_entref_t entref);
void PlayerCmd_getCurrentOffhand(scr_entref_t entRef);
void PlayerCmd_hasWeapon(scr_entref_t entref);
void PlayerCmd_switchToWeapon(scr_entref_t entRef);
void PlayerCmd_switchToOffhand(scr_entref_t entref);
void PlayerCmd_giveStartAmmo(scr_entref_t entref);
void PlayerCmd_giveMaxAmmo(scr_entref_t entRef);
void PlayerCmd_getFractionStartAmmo(scr_entref_t entRef);
void PlayerCmd_getFractionMaxAmmo(scr_entref_t entRef);
void PlayerCmd_setOrigin(scr_entref_t entRef);
void PlayerCmd_setAngles(scr_entref_t entref);
void PlayerCmd_getAngles(scr_entref_t entref);
void PlayerCmd_useButtonPressed(scr_entref_t entref);
void PlayerCmd_attackButtonPressed(scr_entref_t entref);
void PlayerCmd_meleeButtonPressed(scr_entref_t entref);
void PlayerCmd_playerADS(scr_entref_t entref);
void PlayerCmd_isOnGround(scr_entref_t entref);
void PlayerCmd_pingPlayer(scr_entref_t entRef);
void PlayerCmd_SetViewmodel(scr_entref_t entRef);
void PlayerCmd_GetViewmodel(scr_entref_t entRef);
void PlayerCmd_SayAll(scr_entref_t entref);
void PlayerCmd_SayTeam(scr_entref_t entref);
void PlayerCmd_showScoreboard(scr_entref_t entref);
void PlayerCmd_setSpawnWeapon(scr_entref_t entRef);
void PlayerCmd_dropItem(scr_entref_t entref);
void PlayerCmd_finishPlayerDamage(scr_entref_t entRef);
void PlayerCmd_Suicide(scr_entref_t entref);
void PlayerCmd_OpenMenu(scr_entref_t entref);
void PlayerCmd_OpenMenuNoMouse(scr_entref_t entref);
void PlayerCmd_CloseMenu(scr_entref_t entref);
void PlayerCmd_CloseInGameMenu(scr_entref_t entref);
void PlayerCmd_FreezeControls(scr_entref_t entref);
void PlayerCmd_DisableWeapon(scr_entref_t entref);
void PlayerCmd_EnableWeapon(scr_entref_t entref);
void PlayerCmd_SetReverb(scr_entref_t entref);
void PlayerCmd_DeactivateReverb(scr_entref_t entref);
void PlayerCmd_SetChannelVolumes(scr_entref_t entref);
void PlayerCmd_DeactivateChannelVolumes(scr_entref_t entref);
void PlayerCmd_GetWeaponSlotWeapon(scr_entref_t entref);
void PlayerCmd_SetWeaponSlotWeapon(scr_entref_t entref);
void PlayerCmd_GetWeaponSlotAmmo(scr_entref_t entref);
void PlayerCmd_SetWeaponSlotAmmo(scr_entref_t entref);
void PlayerCmd_GetWeaponSlotClipAmmo(scr_entref_t entref);
void PlayerCmd_SetWeaponSlotClipAmmo(scr_entref_t entref);
void PlayerCmd_SetWeaponClipAmmo(scr_entref_t entref);
void iclientprintln(scr_entref_t entref);
void iclientprintlnbold(scr_entref_t entref);
void PlayerCmd_spawn(scr_entref_t entref);
void PlayerCmd_setEnterTime(scr_entref_t entref);
void PlayerCmd_ClonePlayer(scr_entref_t entRef);
void PlayerCmd_SetClientDvar(scr_entref_t entref);
void ScrCmd_IsLookingAt(scr_entref_t entref);
void ScrCmd_PlayLocalSound(scr_entref_t entref);
void PlayerCmd_IsTalking(scr_entref_t entref);
void PlayerCmd_AllowSpectateTeam(scr_entref_t entref);
void PlayerCmd_GetGuid(scr_entref_t entref);