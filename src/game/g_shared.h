#pragma once

#define MAX_HUDELEMENTS 31
#define MAX_HUDELEMS_ARCHIVAL MAX_HUDELEMENTS
#define MAX_HUDELEMS_CURRENT MAX_HUDELEMENTS
#define MAX_STATS 6
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
	float scale;
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
} mantleState_t;

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
	mantleState_t mantleState;
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
	sessionState_t sessionState;
	int forceSpectatorClient;
	int statusIcon;
	int archiveTime;
	int score;
	int deaths;
	uint16_t pers;
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
static_assert((sizeof(turretInfo_s) == 0x44), "ERROR: turretInfo_s size is invalid!");

struct tagInfo_s
{
	struct gentity_s *parent;
	struct gentity_s *next;
	uint16_t name;
	uint16_t pad;
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
	int index;
	unsigned short ammoCount;
	unsigned short clipAmmoCount;
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
	void (*pain)(struct gentity_s *, struct gentity_s *, int, const float *, const int, const float *, int);
	void (*die)(struct gentity_s *, struct gentity_s *, struct gentity_s *, int, int, const int, const float *, int, int);
	void (*controller)(struct gentity_s *, int *);
	int methodOfDeath;
	int splashMethodOfDeath;
};

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
void GScr_NewHudElem();

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

void Scr_LocalizationError(int iParm, const char *pszErrorMessage);
void Scr_ConstructMessageString(int firstParmIndex, int lastParmIndex, const char *errorContext, char *string, unsigned int stringLimit);
void CalculateRanks();
float G_GetFogOpaqueDistSqrd();
const char* G_ModelName(int modelIndex);
int G_LocalizedStringIndex(const char *string);
int G_ShaderIndex(const char *string);
int G_TagIndex(const char *name);
XModel* G_CachedModelForIndex(int modelIndex);
unsigned int G_ModelIndex(const char *name);
void G_OverrideModel(int modelIndex, const char *defaultModelName);
void G_RunThink(gentity_s *ent);

void G_DObjCalcPose(gentity_s *ent);
void G_DObjCalcBone(gentity_s *ent, int boneIndex);

qboolean G_UpdateClientInfo(gentity_s *ent);
void G_PlayerStateToEntityStateExtrapolate(playerState_s *ps, entityState_s *s, int time, int snap);

gentity_t* Scr_EntityForRef(scr_entref_t entref);
game_hudelem_t* Scr_HudElemForRef(scr_entref_t entref);
gentity_t* Scr_GetEntity(unsigned int index);
void Scr_AddEntity(gentity_t *ent);
void Scr_AddHudElem(game_hudelem_t *hud);
void Scr_FreeHudElem(game_hudelem_s *hud);

char* Scr_GetGameTypeNameForScript(const char *pszGameTypeScript);
bool G_ParseWeaponAccurayGraphs(WeaponDef *weaponDef);

void Scr_ReadOnlyField(gentity_s *ent, int offset);
void Scr_SetOrigin(gentity_s *ent, int offset);
void Scr_SetAngles(gentity_s *ent, int offset);
void Scr_SetHealth(gentity_s *ent, int offset);
unsigned int GScr_AllocString(const char *string);
int GScr_GetHeadIconIndex(const char *pszIcon);
int GScr_GetStatusIconIndex(const char *pszIcon);
int Scr_SetObjectField(unsigned int classnum, int entnum, int offset);
void Scr_GetObjectField(unsigned int classnum, int entnum, int offset);
int Scr_SetEntityField(int entnum, int offset);
void Scr_GetEntityField(int entnum, int offset);
void Scr_Notify(gentity_s *ent, unsigned short stringValue, unsigned int paramcount);
unsigned short Scr_ExecEntThread(gentity_s *ent, int handle, unsigned int paramcount);
void Scr_PlayerConnect(gentity_s *self);
void G_InitGentity(gentity_s *ent);

void G_SetOrigin(gentity_s *ent, const float *origin);
void G_SetAngle(gentity_s *ent, const float *angle);
void G_PlayerEvent(int clientNum, int event);

void G_TraceCapsule(trace_t *results, const float *start, const float *mins, const float *maxs, const float *end, int passEntityNum, int contentmask);
int G_TraceCapsuleComplete(const float *start, const float *mins, const float *maxs, const float *end, int passEntityNum, int contentmask);
void G_LocationalTrace(trace_t *results, const float *start, const float *end, int passentitynum, int contentmask, char *priorityMap);
int G_LocationalTracePassed(const float *start, const float *end, int passEntityNum, int contentmask);
void G_SightTrace(int *hitNum, const float *start, const float *end, int passEntityNum, int contentmask);

void Scr_BulletTrace();

void SetClientViewAngle(gentity_s *ent, const float *angle);
void ClientUserinfoChanged(int clientNum);

void GScr_AddFieldsForEntity();
void GScr_AddFieldsForRadiant();

float G_random();
float G_crandom();

void G_AddEvent(gentity_s *ent, int event, unsigned int eventParm);
void TeleportPlayer(gentity_s *player, float *origin, float *angles);
void G_EntUnlink(gentity_s *ent);
void G_CorpseFree(gentity_s *ent);
void G_FreeEntity(gentity_s *ent);

void G_RunCorpseMove(gentity_s *ent);
void G_RunCorpse(gentity_s *ent);

void G_ClientStopUsingTurret(gentity_s *self);
void G_FreeTurret(gentity_s *ent);

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

void G_RegisterDvars();