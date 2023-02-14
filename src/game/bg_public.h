#pragma once

#include "../clientscript/clientscript_public.h"
#include "../xanim/xanim_public.h"

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
	unsigned int value;
} animScriptCondition_t;

typedef struct
{
	short bodyPart[2];
	short animIndex[2];
	unsigned short animDuration[2];
	struct snd_alias_list_t *soundAlias;
	unsigned short tagName;
	unsigned short flags;
} animScriptCommand_t;
static_assert((sizeof(animScriptCommand_t) == 0x14), "ERROR: animScriptCommand_t size is invalid!");

typedef struct
{
	int numConditions;
	animScriptCondition_t conditions[NUM_ANIM_CONDITIONS];
	int numCommands;
	animScriptCommand_t commands[8];
	int unk;
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
	clientControllers_t control;
	unsigned int clientConditions[10][2];
	XAnimTree_s *pXAnimTree;
	char weaponModel;
	int stanceTransitionTime;
	int turnAnimEndTime;
	char turnAnimType;
} clientInfo_t;
static_assert((sizeof(clientInfo_t) == 0x4B8), "ERROR: clientInfo_t size is invalid!");

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