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
	uint16_t ownerNum;
	uint16_t pad3;
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
	vec3_t delta_angles;
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
	byte weaponslots[8];
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
	uint16_t scriptPersId;
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
	byte dobjbits;
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
	uint16_t attachModelNames[6];
	uint16_t attachTagNames[6];
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
	void *openScriptIOFileHandles[1];
	char *openScriptIOFileBuffers[1];
	com_parse_mark_t currentScriptIOLineMark[1];
} level_locals_t;
static_assert((sizeof(level_locals_t) == 0x3624), "ERROR: level_locals_t size is invalid!");

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
	unsigned int constId;
	int ofs;
	int size;
	byte shift;
	void (*setter)(game_hudelem_s *, int);
	void (*getter)(game_hudelem_s *, int);
} game_hudelem_field_t;

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

#include "bg_public.h"
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

void (*HudElem_GetMethod(const char **pName))(scr_entref_t);

void Scr_LocalizationError(int iParm, const char *pszErrorMessage);
void Scr_ConstructMessageString(int firstParmIndex, int lastParmIndex, const char *errorContext, char *string, unsigned int stringLimit);
void CalculateRanks();
const char* G_ModelName(int modelIndex);
int G_LocalizedStringIndex(const char *string);
int G_ShaderIndex(const char *string);

gentity_t* Scr_EntityForRef(scr_entref_t entref);
game_hudelem_t* Scr_HudElemForRef(scr_entref_t entref);
gentity_t* Scr_GetEntity(unsigned int index);
void Scr_AddEntity(gentity_t *ent);
void Scr_AddHudElem(game_hudelem_t *hud);
void Scr_FreeHudElem(game_hudelem_s *hud);

char* Scr_GetGameTypeNameForScript(const char *pszGameTypeScript);