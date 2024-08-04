#include "../qcommon/qcommon.h"
#include "bg_public.h"

uint16_t* controller_names[] =
{
	&scr_const.back_low,
	&scr_const.back_mid,
	&scr_const.back_up,
	&scr_const.neck,
	&scr_const.head,
	&scr_const.pelvis
};

animStringItem_t animStateStr[] =
{
	{"RELAXED", -1},
	{"QUERY", -1},
	{"ALERT", -1},
	{"COMBAT", -1},

	{NULL, -1},
};

animStringItem_t animMoveTypesStr[] =
{
	{"** UNUSED **", -1},
	{"IDLE", -1},
	{"IDLECR", -1},
	{"IDLEPRONE", -1},
	{"WALK", -1},
	{"WALKBK", -1},
	{"WALKCR", -1},
	{"WALKCRBK", -1},
	{"WALKPRONE", -1},
	{"WALKPRONEBK", -1},
	{"RUN", -1},
	{"RUNBK", -1},
	{"RUNCR", -1},
	{"RUNCRBK", -1},
	{"TURNRIGHT", -1},
	{"TURNLEFT", -1},
	{"TURNRIGHTCR", -1},
	{"TURNLEFTCR", -1},
	{"CLIMBUP", -1},
	{"CLIMBDOWN", -1},
	{"MANTLE_ROOT", -1},
	{"MANTLE_UP_57", -1},
	{"MANTLE_UP_51", -1},
	{"MANTLE_UP_45", -1},
	{"MANTLE_UP_39", -1},
	{"MANTLE_UP_33", -1},
	{"MANTLE_UP_27", -1},
	{"MANTLE_UP_21", -1},
	{"MANTLE_OVER_HIGH", -1},
	{"MANTLE_OVER_MID", -1},
	{"MANTLE_OVER_LOW", -1},
	{"FLINCH_FORWARD", -1},
	{"FLINCH_BACKWARD", -1},
	{"FLINCH_LEFT", -1},
	{"FLINCH_RIGHT", -1},
	{"STUMBLE_FORWARD", -1},
	{"STUMBLE_BACKWARD", -1},
	{"STUMBLE_WALK_FORWARD", -1},
	{"STUMBLE_WALK_BACKWARD", -1},
	{"STUMBLE_CROUCH_FORWARD", -1},
	{"STUMBLE_CROUCH_BACKWARD", -1},

	{NULL, -1},
};

animStringItem_t animEventTypesStr[] =
{
	{"PAIN", -1},
	{"DEATH", -1},
	{"FIREWEAPON", -1},
	{"JUMP", -1},
	{"JUMPBK", -1},
	{"LAND", -1},
	{"DROPWEAPON", -1},
	{"RAISEWEAPON", -1},
	{"CLIMBMOUNT", -1},
	{"CLIMBDISMOUNT", -1},
	{"RELOAD", -1},
	{"CROUCH_TO_PRONE", -1},
	{"PRONE_TO_CROUCH", -1},
	{"STAND_TO_CROUCH", -1},
	{"CROUCH_TO_STAND", -1},
	{"STAND_TO_PRONE", -1},
	{"PRONE_TO_STAND", -1},
	{"MELEEATTACK", -1},
	{"SHELLSHOCK", -1},

	{NULL, -1},
};

static animStringItem_t animConditionsStr[] =
{
	{"PLAYERANIMTYPE", -1},
	{"WEAPONCLASS", -1},
	{"MOUNTED", -1},
	{"MOVETYPE", -1},
	{"UNDERHAND", -1},
	{"CROUCHING", -1},
	{"FIRING", -1},
	{"WEAPON_POSITION", -1},
	{"STRAFING", -1},

	{NULL, -1},
};

animStringItem_t animBodyPartsStr[] =
{
	{"** UNUSED **", -1},
	{"LEGS", -1},
	{"TORSO", -1},
	{"BOTH", -1},

	{NULL, -1},
};

static animStringItem_t animParseModesStr[] =
{
	{"defines", -1},
	{"animations", -1},
	{"canned_animations", -1},
	{"statechanges", -1},
	{"events", -1},

	{NULL, -1},
};

static animStringItem_t weaponTypeStr[] =
{
	{ "RIFLE", -1, },
	{ "MG", -1, },
	{ "SMG", -1, },
	{ "SPREAD", -1, },
	{ "PISTOL", -1, },
	{ "GRENADE", -1, },
	{ "ROCKETLAUNCHER", -1, },
	{ "TURRET", -1, },
	{ "NON-PLAYER", -1, },
	{ "ITEM", -1, },

	{NULL, -1},
};

static animStringItem_t animConditionMountedStr[] =
{
	{"** UNUSED **", -1},
	{"MG42", -1},

	{NULL, -1},
};

static animStringItem_t animConditionImpactPointsStr[] =
{
	{ "HIP", -1, },
	{ "ADS", -1, },

	{NULL, -1},
};

static animStringItem_t animConditionLeaningStr[] =
{
	{ "NOT", -1, },
	{ "LEFT", -1, },
	{ "RIGHT", -1, },

	{NULL, -1},
};

animStringItem_t weaponStrings[128];

typedef enum
{
	ANIM_CONDTYPE_BITFLAGS,
	ANIM_CONDTYPE_VALUE,

	NUM_ANIM_CONDTYPES
} animScriptConditionTypes_t;

typedef struct
{
	animScriptConditionTypes_t type;
	animStringItem_t            *values;
} animConditionTable_t;

static animConditionTable_t animConditionsTable[NUM_ANIM_CONDITIONS] =
{
	{ ANIM_CONDTYPE_BITFLAGS, weaponStrings },
	{ ANIM_CONDTYPE_BITFLAGS, weaponTypeStr },
	{ ANIM_CONDTYPE_VALUE,    animConditionMountedStr },
	{ ANIM_CONDTYPE_BITFLAGS, animMoveTypesStr },
	{ ANIM_CONDTYPE_VALUE,    NULL },
	{ ANIM_CONDTYPE_VALUE,    NULL },
	{ ANIM_CONDTYPE_VALUE,    NULL },
	{ ANIM_CONDTYPE_VALUE,    animConditionImpactPointsStr },
	{ ANIM_CONDTYPE_VALUE,    animConditionLeaningStr },
};

// this is used globally within this file to reduce redundant params
static animScriptData_t *globalScriptData = NULL;

#define MAX_ANIM_DEFINES    16

static const char *globalFilename = "mp/playeranim.script";  		  // to prevent redundant params

#define DEFAULT_ANIM_TREE_NAME "multiplayer"

static loadAnim_t *g_pLoadAnims;
static int *g_piNumLoadAnims;

static int parseMovetype;
static int parseEvent;

// these are used globally during script parsing
static int numDefines[NUM_ANIM_CONDITIONS];
static char defineStrings[10000];       // stores the actual strings
static int defineStringsOffset;
static animStringItem_t defineStr[NUM_ANIM_CONDITIONS][MAX_ANIM_DEFINES];
static int defineBits[NUM_ANIM_CONDITIONS][MAX_ANIM_DEFINES][2];

#if LIBCOD_COMPILE_PLAYER == 1
int custom_animation[MAX_CLIENTS] = {0};
#endif

/*
===============
BG_PlayAnim
===============
*/
int BG_PlayAnim( playerState_t *ps, int animNum, int bodyPart, int forceDuration, qboolean setTimer, qboolean isContinue, qboolean force )
{
	int duration;
	qboolean wasSet = qfalse;

#if LIBCOD_COMPILE_PLAYER == 1
	if (custom_animation[ps->clientNum])
		animNum = custom_animation[ps->clientNum];
#endif

	if ( forceDuration )
	{
		duration = forceDuration;
	}
	else
	{
		duration = globalScriptData->animations[animNum].duration + 50;   // account for lerping between anims
	}

	switch ( bodyPart )
	{
	case ANIM_BP_BOTH:
	case ANIM_BP_LEGS:

		if ( ( ps->legsTimer < 50 ) || force )
		{
			if ( !isContinue || !( ( ps->legsAnim & ~ANIM_TOGGLEBIT ) == animNum ) )
			{
				wasSet = qtrue;
				ps->legsAnimDuration = duration;
				ps->legsAnim = ( ( ps->legsAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | animNum;
				if ( setTimer )
				{
					ps->legsTimer = duration;
				}
			}
			else if ( setTimer && globalScriptData->animations[animNum].flags & 0x80 )
			{
				ps->legsTimer = duration;
			}
		}

		if ( bodyPart == ANIM_BP_LEGS )
		{
			break;
		}

		if ( bodyPart == ANIM_BP_BOTH )
		{
			animNum = 0;
		}

	case ANIM_BP_TORSO:

		if ( ( ps->torsoTimer < 50 ) || force )
		{
			if ( !isContinue || !( ( ps->torsoAnim & ~ANIM_TOGGLEBIT ) == animNum ) )
			{
				ps->torsoAnim = ( ( ps->torsoAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | animNum;
				if ( setTimer )
				{
					ps->torsoTimer = duration;
				}
				ps->torsoAnimDuration = duration;
			}
			else if ( setTimer && globalScriptData->animations[animNum].flags & 0x80 )
			{
				ps->torsoTimer = duration;
			}
		}

		break;

	default: // TTimo default ANIM_BP_UNUSED NUM_ANIM_BODYPARTS not handled
		break;
	}

	if ( !wasSet )
	{
		return -1;
	}

	return duration;
}

/*
===============
BG_PlayAnim
===============
*/
void BG_LerpOffset( vec3_t offset_goal, float maxOffsetChange, vec3_t offset )
{
	float error;
	vec3_t diff;

	VectorSubtract(offset_goal, offset, diff);

	error = VectorLengthSquared(diff);

	if ( error == 0.0 )
	{
		return;
	}

	error = I_rsqrt(error) * maxOffsetChange;

	if ( error < 1.0 )
		VectorMA(offset, error, diff, offset);
	else
		VectorCopy(offset_goal, offset);
}

/*
==============
BG_UpdateConditionValue
==============
*/
void BG_UpdateConditionValue( int client, int condition, int value, qboolean checkConversion )
{
	// rain - fixed checkConversion brained-damagedness, which would try
	// to BitSet an insane value if checkConversion was false but this
	// anim was ANIM_CONDTYPE_BITFLAGS
	if ( checkConversion )
	{
		if ( animConditionsTable[condition].type == ANIM_CONDTYPE_BITFLAGS )
		{
			// we may need to convert to bitflags
			// DHM - Nerve :: We want to set the ScriptData to the explicit value passed in.
			//				Com_BitSet will OR values on top of each other, so clear it first.
			level_bgs.clientinfo[client].clientConditions[condition][0] = 0;
			level_bgs.clientinfo[client].clientConditions[condition][1] = 0;
			// dhm - end

			Com_BitSet( level_bgs.clientinfo[client].clientConditions[condition], value );
			return;
		}
		// rain - we must fall through here because a bunch of non-bitflag
		// conditions are set with checkConversion == qtrue
	}
	level_bgs.clientinfo[client].clientConditions[condition][0] = value;
}

/*
===============
BG_ExecuteCommand
  returns the duration of the animation, -1 if no anim was set
===============
*/
int BG_ExecuteCommand( playerState_t *ps, animScriptCommand_t *scriptCommand, qboolean setTimer, qboolean isContinue, qboolean force )
{
	int duration = -1;
	qboolean playedLegsAnim = qfalse;

	if ( scriptCommand->bodyPart[0] )
	{
		duration = scriptCommand->animDuration[0] + 50;
		// FIXME: how to sync torso/legs anims accounting for transition blends, etc
		if ( scriptCommand->bodyPart[0] == ANIM_BP_LEGS || scriptCommand->bodyPart[0] == ANIM_BP_BOTH )
		{
			playedLegsAnim = ( BG_PlayAnim( ps, scriptCommand->animIndex[0], scriptCommand->bodyPart[0], duration, setTimer, isContinue, force ) > -1 );
		}
		else
		{
			BG_PlayAnim( ps, scriptCommand->animIndex[0], scriptCommand->bodyPart[0], duration, setTimer, isContinue, force );
		}
	}

	if ( scriptCommand->bodyPart[1] )
	{
		duration = scriptCommand->animDuration[0] + 50;
		// FIXME: how to sync torso/legs anims accounting for transition blends, etc
		// just play the animation for the torso
		if ( scriptCommand->bodyPart[0] == ANIM_BP_LEGS || scriptCommand->bodyPart[0] == ANIM_BP_BOTH )
		{
			playedLegsAnim = ( BG_PlayAnim( ps, scriptCommand->animIndex[1], scriptCommand->bodyPart[1], duration, setTimer, isContinue, force ) > -1 );
		}
		else
		{
			BG_PlayAnim( ps, scriptCommand->animIndex[1], scriptCommand->bodyPart[1], duration, setTimer, isContinue, force );
		}
	}

	if ( scriptCommand->soundAlias )
	{
		globalScriptData->playSoundAlias( ps->clientNum, scriptCommand->soundAlias );
	}

	if ( !playedLegsAnim )
	{
		return -1;
	}

	return duration;
}

/*
===============
BG_InitWeaponString
===============
*/
void BG_InitWeaponString( int index, const char *name )
{
	weaponStrings[index].string = name;
	weaponStrings[index].hash = BG_StringHashValue(name);
}

/*
=================
BG_AnimUpdatePlayerStateConditions
=================
*/
void BG_AnimUpdatePlayerStateConditions( pmove_t *pmove )
{
	WeaponDef *weaponDef;
	playerState_s *ps;

	ps = pmove->ps;

	weaponDef = BG_GetWeaponDef(BG_GetViewmodelWeaponIndex(ps));

	// WEAPON
	BG_UpdateConditionValue(ps->clientNum, ANIM_COND_PLAYERANIMTYPE, weaponDef->playerAnimType, qtrue);
	BG_UpdateConditionValue(ps->clientNum, ANIM_COND_WEAPONCLASS, weaponDef->weaponClass, qtrue);

	if ( ps->eFlags & EF_AIMDOWNSIGHT )
		BG_UpdateConditionValue(ps->clientNum, ANIM_COND_WEAPON_POSITION, qtrue, qtrue);
	else
		BG_UpdateConditionValue(ps->clientNum, ANIM_COND_WEAPON_POSITION, qfalse, qtrue);

	// MOUNTED
	if ( ps->eFlags & EF_TURRET_ACTIVE )
		BG_UpdateConditionValue(ps->clientNum, ANIM_COND_MOUNTED, MOUNTED_MG42, qtrue);
	else
		BG_UpdateConditionValue(ps->clientNum, ANIM_COND_MOUNTED, MOUNTED_UNUSED, qtrue);

	// UNDERHAND
	BG_UpdateConditionValue(ps->clientNum, ANIM_COND_UNDERHAND, ps->viewangles[0] > 0.0, qtrue);

	if ( pmove->cmd.buttons & BUTTON_ATTACK )
		BG_UpdateConditionValue(ps->clientNum, ANIM_COND_FIRING, qtrue, qtrue);
	else
		BG_UpdateConditionValue(ps->clientNum, ANIM_COND_FIRING, qfalse, qtrue);
}

/*
================
BG_GetAnimScriptEvent

  returns the animation index for this event
================
*/
int BG_GetAnimScriptEvent( playerState_t *ps, scriptAnimEventTypes_t event )
{
	animScript_t        *script;
	animScriptItem_t    *scriptItem;
	animScriptCommand_t *scriptCommand;

	if ( event != ANIM_ET_DEATH && ps->pm_type >= PM_DEAD )
	{
		return -1;
	}

	script = &globalScriptData->scriptEvents[ event ];
	if ( !script->numItems )
	{
		return -1;
	}
	// find the first script item, that passes all the conditions for this event
	scriptItem = BG_FirstValidItem( ps->clientNum, script );
	if ( !scriptItem )
	{
		return -1;
	}
	if ( !scriptItem->numCommands )
	{
		return -1;
	}
	// pick a random command
	scriptCommand = &scriptItem->commands[ rand() % scriptItem->numCommands ];

	// return the animation
	return scriptCommand->animIndex[0];
}

/*
================
BG_AnimScriptEvent

  returns the duration in milliseconds that this model should be paused. -1 if no event found
================
*/
int BG_AnimScriptEvent( playerState_t *ps, scriptAnimEventTypes_t event, qboolean isContinue, qboolean force )
{
	animScript_t        *script;
	animScriptItem_t    *scriptItem;
	animScriptCommand_t *scriptCommand;

	if ( event != ANIM_ET_DEATH && ps->pm_type >= PM_DEAD )
	{
		return -1;
	}

#ifdef DBGANIMEVENTS
	Com_Printf( "script event: cl %i, ev %s, ", ps->clientNum, animEventTypesStr[event] );
#endif

	script = &globalScriptData->scriptEvents[ event ];
	if ( !script->numItems )
	{
#ifdef DBGANIMEVENTS
		Com_Printf( "no entry\n" );
#endif
		return -1;
	}
	// find the first script item, that passes all the conditions for this event
	scriptItem = BG_FirstValidItem( ps->clientNum, script );
	if ( !scriptItem )
	{
#ifdef DBGANIMEVENTS
		Com_Printf( "no valid conditions\n" );
#endif
		return -1;
	}
	if ( !scriptItem->numCommands )
	{
#ifdef DBGANIMEVENTS
		Com_Printf( "no commands\n" );
#endif
		return -1;
	}
	// pick a random command
	scriptCommand = &scriptItem->commands[ rand() % scriptItem->numCommands ];

#ifdef DBGANIMEVENTS
	if ( scriptCommand->bodyPart[0] )
	{
		Com_Printf( "anim0 (%s): %s", animBodyPartsStr[scriptCommand->bodyPart[0]].string, globalScriptData->animations[scriptCommand->animIndex[0]]->name );
	}
	if ( scriptCommand->bodyPart[1] )
	{
		Com_Printf( "anim1 (%s): %s", animBodyPartsStr[scriptCommand->bodyPart[1]].string, globalScriptData->animations[scriptCommand->animIndex[1]]->name );
	}
	Com_Printf( "\n" );
#endif

	// run it
	return BG_ExecuteCommand( ps, scriptCommand, qtrue, isContinue, force );
}

/*
================
BG_AnimScriptStateChange

  returns the duration in milliseconds that this model should be paused. -1 if no anim found
================
*/
int BG_AnimScriptStateChange( playerState_t *ps, aistateEnum_t newState, aistateEnum_t oldState )
{
	animScript_t        *script;
	animScriptItem_t    *scriptItem;
	animScriptCommand_t *scriptCommand;

	if ( ps->pm_type >= PM_DEAD )
	{
		return -1;
	}

	script = &globalScriptData->scriptStateChange[ oldState ][ newState ];
	if ( !script->numItems )
	{
		return -1;
	}
	// find the first script item, that passes all the conditions for this event
	scriptItem = BG_FirstValidItem( ps->clientNum, script );
	if ( !scriptItem )
	{
		return -1;
	}
	if ( !scriptItem->numCommands )
	{
		return -1;
	}
	// pick a random command
	scriptCommand = &scriptItem->commands[ rand() % scriptItem->numCommands ];
	// run it
	return BG_ExecuteCommand( ps, scriptCommand, qtrue, qfalse, qfalse );
}

/*
================
BG_AnimScriptAnimation

  runs the normal locomotive animations

  returns 1 if an animation was set, -1 if no animation was found, 0 otherwise
================
*/
int BG_AnimScriptAnimation( playerState_t *ps, int state, int movetype, qboolean isContinue )
{
	animScript_t        *script = NULL;
	animScriptItem_t    *scriptItem = NULL;
	animScriptCommand_t *scriptCommand = NULL;

	// Allow fallen movetype while dead
	if ( ps->pm_type >= PM_DEAD )
	{
		return -1;
	}

#ifdef DBGANIMS
	Com_Printf( "script anim: cl %i, mt %s, ", ps->clientNum, animMoveTypesStr[movetype] );
#endif

	// xkan, 1/10/2003 - adapted from original SP source
	// try finding a match in all states ABOVE the given state
	while ( !scriptItem && state >= AISTATE_RELAXED )
	{
		script = &globalScriptData->scriptAnims[ state ][ movetype ];
		if ( !script->numItems )
		{
			state--;
			continue;
		}
		// find the first script item, that passes all the conditions for this event
		scriptItem = BG_FirstValidItem( ps->clientNum, script );
		if ( !scriptItem )
		{
			state--;
			continue;
		}
	}

	//
	if ( !scriptItem )
	{
#ifdef DBGANIMS
		Com_Printf( "no valid conditions\n" );
#endif
		return -1;
	}
	if ( !scriptItem->numCommands )
	{
#ifdef DBGANIMS
		Com_Printf( "no commands\n" );
#endif
		return -1;
	}
	// save this as our current movetype
	BG_UpdateConditionValue( ps->clientNum, ANIM_COND_MOVETYPE, movetype, qtrue );
	// pick the correct animation for this character (animations must be constant for each character, otherwise they'll constantly change)
	scriptCommand = &scriptItem->commands[ ps->clientNum % scriptItem->numCommands ];

#ifdef DBGANIMS
	if ( scriptCommand->bodyPart[0] )
	{
		Com_Printf( "anim0 (%s): %s", animBodyPartsStr[scriptCommand->bodyPart[0]].string, animModelInfo->animations[scriptCommand->animIndex[0]]->name );
	}
	if ( scriptCommand->bodyPart[1] )
	{
		Com_Printf( "anim1 (%s): %s", animBodyPartsStr[scriptCommand->bodyPart[1]].string, animModelInfo->animations[scriptCommand->animIndex[1]]->name );
	}
	Com_Printf( "\n" );
#endif

	// run it
	return( BG_ExecuteCommand( ps, scriptCommand, qfalse, isContinue, qfalse ) != -1 );
}

/*
============
BG_InitWeaponStrings

  Builds the list of weapon names from the item list. This is done here rather
  than hardcoded to ease the process of modifying the weapons.
============
*/
void BG_InitWeaponStrings()
{
	memset( weaponStrings, 0, sizeof( weaponStrings ) );
	BG_LoadWeaponStrings();
}

/*
============
BG_Player_DoControllersInternal
============
*/
static void BG_Player_DoControllersInternal( DObj *obj, const entityState_t *es, int *partBits, clientInfo_t *ci, controller_info_t *info )
{
	int i;
	float c;
	float s;
	int clientNum;
	vec3_t vHeadAngles;
	vec3_t vTorsoAngles;
	float fLeanFrac;
	vec3_t tag_origin_offset;
	vec3_t tag_origin_angles;
	vec3_t angles[CONTROL_ANGLES_COUNT];

	if ( es->eFlags & EF_TURRET_ACTIVE )
	{
		memset(info, 0, sizeof(controller_info_t));
		return;
	}

	clientNum = es->clientNum;

	VectorClear(tag_origin_angles);
	VectorClear(vTorsoAngles);

	VectorCopy(ci->playerAngles, vHeadAngles);

	tag_origin_angles[1] = ci->legs.yawAngle;
	vTorsoAngles[1] = ci->torso.yawAngle;

	if ( !( BG_GetConditionValue(ci, ANIM_COND_MOVETYPE, qfalse) & ((1<<ANIM_MT_CLIMBUP)|(1<<ANIM_MT_CLIMBDOWN)) ) )
	{
		vTorsoAngles[0] = ci->torso.pitchAngle;

		if ( es->eFlags & EF_PRONE )
		{
			vTorsoAngles[0] = AngleNormalize180(vTorsoAngles[0]);

			if ( vTorsoAngles[0] > 0.0 )
			{
				vTorsoAngles[0] = vTorsoAngles[0] * 0.5;
			}
			else
			{
				vTorsoAngles[0] = vTorsoAngles[0] * 0.25;
			}
		}
	}

	AnglesSubtract(vHeadAngles, vTorsoAngles, vHeadAngles);
	AnglesSubtract(vTorsoAngles, tag_origin_angles, vTorsoAngles);

	VectorSet(tag_origin_offset, 0.0, 0.0, es->fTorsoHeight);

	fLeanFrac = GetLeanFraction(ci->lerpLean);

	vTorsoAngles[2] = fLeanFrac * 50.0 * 0.92500001;
	vHeadAngles[2] = vTorsoAngles[2];

	if ( fLeanFrac != 0.0 )
	{
		if ( es->eFlags & EF_CROUCHING )
		{
			if ( fLeanFrac > 0.0 )
			{
				tag_origin_offset[1] = -fLeanFrac * 2.5 + tag_origin_offset[1];
			}
			else
			{
				tag_origin_offset[1] = -fLeanFrac * 12.5 + tag_origin_offset[1];
			}
		}
		else if ( fLeanFrac > 0.0 )
		{
			tag_origin_offset[1] = -fLeanFrac * 2.5 + tag_origin_offset[1];
		}
		else
		{
			tag_origin_offset[1] = -fLeanFrac * 5.0 + tag_origin_offset[1];
		}
	}

	if ( !( es->eFlags & EF_DEAD ) )
	{
		tag_origin_angles[1] = AngleSubtract(tag_origin_angles[1], ci->playerAngles[1]);
	}

	if ( es->eFlags & EF_PRONE )
	{
		if ( fLeanFrac != 0.0 )
		{
			vHeadAngles[2] = vHeadAngles[2] * 0.5;
		}

		tag_origin_angles[0] = tag_origin_angles[0] + es->fTorsoPitch;

		FastSinCos(vTorsoAngles[1] * 0.0174532925199433, &s, &c);

		tag_origin_offset[0] = (1.0 - c) * -24.0 + tag_origin_offset[0];
		tag_origin_offset[1] = s * -12.0 + tag_origin_offset[1];

		if ( fLeanFrac * s > 0.0 )
		{
			tag_origin_offset[1] = -fLeanFrac * (1.0 - c) * 16.0 + tag_origin_offset[1];
		}

		angles[CONTROL_ANGLES_BACK_LOW][0] = 0.0;
		angles[CONTROL_ANGLES_BACK_LOW][1] = vTorsoAngles[2] * -1.2;
		angles[CONTROL_ANGLES_BACK_LOW][2] = vTorsoAngles[2] * 0.30000001;

		if ( es->fTorsoPitch != 0.0 || es->fWaistPitch != 0.0 )
		{
			angles[CONTROL_ANGLES_BACK_LOW][0] = AngleSubtract(es->fTorsoPitch, es->fWaistPitch) + angles[CONTROL_ANGLES_BACK_LOW][0];
		}

		angles[CONTROL_ANGLES_BACK_MID][0] = 0.0;
		angles[CONTROL_ANGLES_BACK_MID][1] = vTorsoAngles[1] * 0.1 - vTorsoAngles[2] * 0.2;
		angles[CONTROL_ANGLES_BACK_MID][2] = vTorsoAngles[2] * 0.2;

		angles[CONTROL_ANGLES_BACK_UP][0] = vTorsoAngles[0];
		angles[CONTROL_ANGLES_BACK_UP][1] = vTorsoAngles[1] * 0.80000001 + vTorsoAngles[2];
		angles[CONTROL_ANGLES_BACK_UP][2] = vTorsoAngles[2] * -0.2;
	}
	else
	{
		if ( fLeanFrac != 0.0 )
		{
			if ( es->eFlags & EF_CROUCHING )
			{
				if ( fLeanFrac <= 0.0 )
				{
					vTorsoAngles[2] = vTorsoAngles[2] * 1.25;
					vHeadAngles[2] = vHeadAngles[2] * 1.25;
				}
			}
			else
			{
				vTorsoAngles[2] = vTorsoAngles[2] * 1.25;
				vHeadAngles[2] = vHeadAngles[2] * 1.25;
			}
		}

		tag_origin_angles[2] = fLeanFrac * 50.0 * 0.075000003 + tag_origin_angles[2];

		angles[CONTROL_ANGLES_BACK_LOW][0] = vTorsoAngles[0] * 0.2;
		angles[CONTROL_ANGLES_BACK_LOW][1] = vTorsoAngles[1] * 0.40000001;
		angles[CONTROL_ANGLES_BACK_LOW][2] = vTorsoAngles[2] * 0.5;

		if ( es->fTorsoPitch != 0.0 || es->fWaistPitch != 0.0 )
		{
			angles[CONTROL_ANGLES_BACK_LOW][0] = AngleSubtract(es->fTorsoPitch, es->fWaistPitch) + angles[CONTROL_ANGLES_BACK_LOW][0];
		}

		angles[CONTROL_ANGLES_BACK_MID][0] = vTorsoAngles[0] * 0.30000001;
		angles[CONTROL_ANGLES_BACK_MID][1] = vTorsoAngles[1] * 0.40000001;
		angles[CONTROL_ANGLES_BACK_MID][2] = vTorsoAngles[2] * 0.5;

		angles[CONTROL_ANGLES_BACK_UP][0] = vTorsoAngles[0] * 0.5;
		angles[CONTROL_ANGLES_BACK_UP][1] = vTorsoAngles[1] * 0.2;
		angles[CONTROL_ANGLES_BACK_UP][2] = vTorsoAngles[2] * -0.60000002;
	}

	angles[CONTROL_ANGLES_NECK][0] = vHeadAngles[0] * 0.30000001;
	angles[CONTROL_ANGLES_NECK][1] = vHeadAngles[1] * 0.30000001;
	angles[CONTROL_ANGLES_NECK][2] = 0.0;

	angles[CONTROL_ANGLES_HEAD][0] = vHeadAngles[0] * 0.69999999;
	angles[CONTROL_ANGLES_HEAD][1] = vHeadAngles[1] * 0.69999999;
	angles[CONTROL_ANGLES_HEAD][2] = vHeadAngles[2] * -0.30000001;

	VectorClear(angles[CONTROL_ANGLES_PELVIS]);

	if ( es->fWaistPitch != 0.0 || es->fTorsoPitch != 0.0 )
	{
		angles[CONTROL_ANGLES_PELVIS][0] = AngleSubtract(es->fWaistPitch, es->fTorsoPitch);
	}

	for ( i = 0; i < CONTROL_ANGLES_COUNT; ++i )
	{
		VectorCopy(angles[i], info->angles[i]);
	}

	VectorCopy(tag_origin_angles, info->tag_origin_angles);
	VectorCopy(tag_origin_offset, info->tag_origin_offset);
}

/*
============
BG_Player_DoControllers
============
*/
void BG_Player_DoControllers( DObj *obj, const gentity_s *ent, int *partBits, clientInfo_t *ci, int frametime )
{
	controller_info_t info;
	float maxAngleChange;
	int i;

	BG_Player_DoControllersInternal(obj, &ent->s, partBits, ci, &info);

	maxAngleChange = (float)frametime * 0.36000001;

	for ( i = 0; i < CONTROL_ANGLES_COUNT; ++i )
	{
		BG_LerpAngles(info.angles[i], maxAngleChange, ci->control.angles[i]);
		DObjSetControlTagAngles(obj, partBits, *controller_names[i], ci->control.angles[i]);
	}

	BG_LerpAngles(info.tag_origin_angles, maxAngleChange, ci->control.tag_origin_angles);
	BG_LerpOffset(info.tag_origin_offset, (float)frametime * 0.1, ci->control.tag_origin_offset);

	DObjSetLocalTag(obj, partBits, scr_const.tag_origin, ci->control.tag_origin_offset, ci->control.tag_origin_angles);
}

/*
============
BG_UpdatePlayerDObj
============
*/
void BG_UpdatePlayerDObj( DObj *pDObj, entityState_t *es, clientInfo_t *ci, qboolean attachIgnoreCollision )
{
	int iNumModels;
	DObjModel_s dobjModels[DOBJ_MAX_SUBMODELS];
	XAnimTree_s *pAnimTree;
	int i;
	int iClientWeapon;

	iClientWeapon = es->weapon;

	if ( es->eFlags & EF_TURRET_ACTIVE )
	{
		iClientWeapon = 0;
	}

	pAnimTree = ci->pXAnimTree;

	if ( !ci->infoValid || !ci->model[0] )
	{
		XAnimClearTree(pAnimTree);
		level_bgs.SafeDObjFree(es->number);
		return;
	}

	if ( pDObj )
	{
		if ( ci->iDObjWeapon == iClientWeapon && !ci->dobjDirty )
		{
			assert(pAnimTree == DObjGetTree( pDObj ));
			return;
		}

		level_bgs.SafeDObjFree(es->number);
	}

	dobjModels[0].model = level_bgs.GetXModel(ci->model);
	dobjModels[0].boneName = 0;
	dobjModels[0].ignoreCollision = 0;

	iNumModels = 1;

	for ( i = 0; i < MAX_MODEL_ATTACHMENTS; i++ )
	{
		if ( !ci->attachModelNames[i][0] )
			continue;

		assert(iNumModels < DOBJ_MAX_SUBMODELS);
		dobjModels[iNumModels].model = level_bgs.GetXModel(ci->attachModelNames[i]);
		assert(dobjModels[iNumModels].model);
		dobjModels[iNumModels].boneName = ci->attachTagNames[i];
		dobjModels[iNumModels].ignoreCollision = (attachIgnoreCollision >> i) & 1;
		iNumModels++;
	}

	ci->iDObjWeapon = iClientWeapon;

	level_bgs.CreateDObj(dobjModels, iNumModels, pAnimTree, es->number);

	ci->dobjDirty = 0;
}

/*
===============
BG_GetAnimationForIndex

  returns the animation_t for the given index
===============
*/
animation_t *BG_GetAnimationForIndex( int client, unsigned int index )
{
	if ( index >= globalScriptData->numAnimations )
	{
		Com_Error(ERR_DROP, "BG_GetAnimationForIndex: index out of bounds");
	}

	return &globalScriptData->animations[index];
}

/*
=================
BG_AnimationForString
=================
*/
animation_t *BG_AnimationForString( const char *string )
{
	int i, hash;
	animation_t *anim;

	hash = BG_StringHashValue( string );

	for ( i = 0; i < globalScriptData->numAnimations; i++ )
	{
		anim = &globalScriptData->animations[i];
		if ( ( hash == anim->nameHash ) && !Q_stricmp( string, anim->name ) )
		{
			// found a match
			return anim;
		}
	}
	// no match found
	Com_Error( ERR_DROP, "BG_AnimationForString: unknown animation '%s'", string );
	return NULL;    // shutup compiler
}

/*
=================
BG_AnimParseError
=================
*/
void BG_AnimParseError( const char *msg, ... )
{
	va_list argptr;
	char text[1024];

	va_start( argptr, msg );
	Q_vsnprintf( text, sizeof( text ), msg, argptr );
	va_end( argptr );

	if ( globalFilename )
	{
		Com_Error( ERR_DROP,  "%s: (%s, line %i)", text, globalFilename, Com_GetCurrentParseLine() + 1 );
	}
	else
	{
		Com_Error( ERR_DROP,  "%s", text );
	}
}

/*
=================
BG_IsMantleAnim
=================
*/
qboolean BG_IsMantleAnim( const clientInfo_t *ci, int animNum )
{
	animation_t *anim = BG_GetAnimationForIndex(ci->clientNum, animNum & ~ANIM_TOGGLEBIT);

	return anim->movetype & ((1<<ANIM_MT_MANTLE_ROOT)|(1<<ANIM_MT_MANTLE_UP_57)|(1<<ANIM_MT_MANTLE_UP_51)
	                         |(1<<ANIM_MT_MANTLE_UP_45)|(1<<ANIM_MT_MANTLE_UP_39)|(1<<ANIM_MT_MANTLE_UP_33)
	                         |(1<<ANIM_MT_MANTLE_UP_27)|(1<<ANIM_MT_MANTLE_UP_21)|(1<<ANIM_MT_MANTLE_OVER_HIGH)
	                         |(1<<ANIM_MT_MANTLE_OVER_MID)|(1<<ANIM_MT_MANTLE_OVER_LOW));
}

/*
=================
BG_IsProneAnim
=================
*/
qboolean BG_IsProneAnim( const clientInfo_t *ci, int animNum )
{
	animation_t *anim = BG_GetAnimationForIndex(ci->clientNum, animNum & ~ANIM_TOGGLEBIT);

	return anim->movetype & ((1<<ANIM_MT_IDLEPRONE)|(1<<ANIM_MT_WALKPRONE)|(1<<ANIM_MT_WALKPRONEBK));
}

/*
=================
BG_IsCrouchingAnim
=================
*/
qboolean BG_IsCrouchingAnim( const clientInfo_t *ci, int animNum ) // not sure about this
{
	animation_t *anim = BG_GetAnimationForIndex(ci->clientNum, animNum & ~ANIM_TOGGLEBIT);

	return anim->movetype & ((1<<ANIM_MT_IDLECR)|(1<<ANIM_MT_WALKCR)|(1<<ANIM_MT_WALKCRBK)
	                         |(1<<ANIM_MT_RUNCR)|(1<<ANIM_MT_RUNCRBK)|(1<<ANIM_MT_TURNRIGHTCR)
	                         |(1<<ANIM_MT_TURNLEFTCR));
}

/*
===============
BG_GetAnimString
===============
*/
char *BG_GetAnimString( unsigned int anim )
{
	if ( anim >= globalScriptData->numAnimations )
	{
		BG_AnimParseError( "BG_GetAnimString: anim index is out of range" );
	}

	return globalScriptData->animations[anim].name;
}

/*
===============
BG_CopyStringIntoBuffer
===============
*/
char *BG_CopyStringIntoBuffer( const char *string, char *buffer, int bufSize, int *offset )
{
	char *pch;

	// check for overloaded buffer
	if ( *offset + strlen( string ) + 1 >= bufSize )
	{
		BG_AnimParseError( "BG_CopyStringIntoBuffer: out of buffer space" );
	}

	pch = &buffer[*offset];

	// safe to do a strcpy since we've already checked for overrun
	strcpy( pch, string );

	// move the offset along
	*offset += strlen( string ) + 1;

	return pch;
}

/*
=================
BG_IndexForString
  errors out if no match found
=================
*/
int BG_IndexForString( const char *token, animStringItem_t *strings, qboolean allowFail )
{
	int i, hash;
	animStringItem_t *strav;

	hash = BG_StringHashValue( token );

	for ( i = 0, strav = strings; strav->string; strav++, i++ )
	{
		if ( strav->hash == -1 )
		{
			strav->hash = BG_StringHashValue( strav->string );
		}
		if ( ( hash == strav->hash ) && !Q_stricmp( token, strav->string ) )
		{
			// found a match
			return i;
		}
	}
	// no match found
	if ( !allowFail )
	{
		BG_AnimParseError( "BG_IndexForString: unknown token '%s'", token );
	}
	//
	return -1;
}

/*
=================
BG_AnimationIndexForString
=================
*/
int BG_AnimationIndexForString( const char *string )
{
	int i, hash;
	animation_t *anim;
	loadAnim_t *loadAnim;

	if ( g_pLoadAnims )
	{
		hash = BG_StringHashValue( string );

		for ( i = 0, loadAnim = g_pLoadAnims; i < *g_piNumLoadAnims; i++, loadAnim++ )
		{
			if ( ( hash == loadAnim->nameHash ) && !Q_stricmp( string, loadAnim->name ) )
			{
				// found a match
				return i;
			}
		}

		loadAnim = &g_pLoadAnims[*g_piNumLoadAnims];
		Scr_FindAnim(DEFAULT_ANIM_TREE_NAME, string, &loadAnim->anim, level_bgs.anim_user);

		strcpy(loadAnim->name, string);
		loadAnim->nameHash = hash;

		return (*g_piNumLoadAnims)++;
	}

	hash = BG_StringHashValue( string );

	for ( i = 0; i < globalScriptData->numAnimations; i++ )
	{
		anim = &globalScriptData->animations[i];
		if ( ( hash == anim->nameHash ) && !Q_stricmp( string, anim->name ) )
		{
			// found a match
			return i;
		}
	}
	// no match found
	BG_AnimParseError( "BG_AnimationIndexForString: unknown player animation '%s'", string );
	return -1;  // shutup compiler
}

/*
===============
BG_PlayAnimName
===============
*/
int BG_PlayAnimName( playerState_t *ps, char *animName, animBodyPart_t bodyPart, qboolean setTimer, qboolean isContinue, qboolean force )
{
	return BG_PlayAnim( ps, BG_AnimationIndexForString( animName ), bodyPart, 0, setTimer, isContinue, force );
}

/*
=================
BG_ParseCommands
=================
*/
void BG_ParseCommands( const char **input, animScriptItem_t *scriptItem, animScriptData_t *scriptData )
{
	char    *token;
	// TTimo gcc: might be used uninitialized
	animScriptCommand_t *command = NULL;
	int partIndex = 0;
	qboolean bCommandFound;

	while ( 1 )
	{
		// parse the body part
		token = partIndex > 0 ? Com_ParseOnLine(input) : Com_Parse(input);
		if ( !token || !*token )
		{
			break;
		}
		if ( !Q_stricmp( token, "}" ) )
		{
			// unget the bracket and get out of here
			*input -= strlen( token );
			break;
		}

		// new command?
		if ( partIndex == 0 )
		{
			// have we exceeded the maximum number of commands?
			if ( scriptItem->numCommands >= MAX_ANIMSCRIPT_ANIMCOMMANDS )
			{
				BG_AnimParseError( "BG_ParseCommands: exceeded maximum number of animations (%i)", MAX_ANIMSCRIPT_ANIMCOMMANDS );
			}
			command = &scriptItem->commands[scriptItem->numCommands++];
			memset( command, 0, sizeof( command ) );
		}

		command->bodyPart[partIndex] = BG_IndexForString( token, animBodyPartsStr, qtrue );
		if ( command->bodyPart[partIndex] > 0 )
		{
			// parse the animation
			token = Com_ParseOnLine( input );
			if ( !token || !token[0] )
			{
				BG_AnimParseError( "BG_ParseCommands: expected animation" );
			}
			command->animIndex[partIndex] = BG_AnimationIndexForString( token );
			command->animDuration[partIndex] = scriptData->animations[command->animIndex[partIndex]].duration;
			if ( !g_pLoadAnims )
			{
				// if this is a locomotion, set the movetype of the animation so we can reverse engineer the movetype from the animation, on the client
				if ( parseMovetype != ANIM_MT_UNUSED && command->bodyPart[partIndex] != ANIM_BP_TORSO )
				{
					scriptData->animations[command->animIndex[partIndex]].movetype |= ( 1LL << parseMovetype );

					// if this is a mantle event, then this is a mantle animation
					if ( (parseMovetype == ANIM_MT_CLIMBUP || parseMovetype == ANIM_MT_CLIMBDOWN)
					        && scriptData->animations[command->animIndex[partIndex]].moveSpeed != 0 )
					{
						scriptData->animations[command->animIndex[partIndex]].flags |= 2;
					}

					for ( int i = 0; i < scriptItem->numConditions; i++ )
					{
						switch ( scriptItem->conditions[i].index )
						{
						case ANIM_COND_STRAFING:
							break;
						default:
							switch ( scriptItem->conditions[i].value[0] )
							{
							case 1:
								scriptData->animations[command->animIndex[partIndex]].flags |= 0x10;
								break;
							case 2:
								scriptData->animations[command->animIndex[partIndex]].flags |= 0x20;
								break;
							default:
								break;
							}
							break;
						}
					}
				}

				switch ( parseEvent )
				{
				case ANIM_ET_FIREWEAPON:
					scriptData->animations[command->animIndex[partIndex]].flags |= 8;
					scriptData->animations[command->animIndex[partIndex]].initialLerp = 30;
					break;
				case ANIM_ET_DEATH:
					scriptData->animations[command->animIndex[partIndex]].moveSpeed = 0;
					scriptData->animations[command->animIndex[partIndex]].flags |= 0x40;
					break;
				case ANIM_ET_RELOAD:
					scriptData->animations[command->animIndex[partIndex]].moveSpeed = 0;
					break;
				default:
					if ( parseMovetype >= ANIM_MT_MANTLE_UP_57 && parseMovetype <= ANIM_MT_MANTLE_OVER_LOW )
					{
						scriptData->animations[command->animIndex[partIndex]].moveSpeed = 0;
					}
					break;
				}
			}
			do
			{
				// check for a duration for this animation instance
				bCommandFound = qfalse;
				token = Com_ParseOnLine( input );
				if ( token && token[0] )
				{
					if ( !Q_stricmp( token, "duration" ) )
					{
						// read the duration
						bCommandFound = qtrue;
						token = Com_ParseOnLine( input );
						if ( !token || !token[0] )
						{
							BG_AnimParseError( "BG_ParseCommands: expected duration value" );
						}
						command->animDuration[partIndex] = atoi( token );
					}
					else if ( !Q_stricmp( token, "turretanim" ) )
					{
						// read the turretanim
						bCommandFound = qtrue;
						if ( !g_pLoadAnims )
						{
							scriptData->animations[command->animIndex[partIndex]].flags |= 4;
						}
						if ( command->bodyPart[partIndex] != ANIM_BP_BOTH )
						{
							BG_AnimParseError("BG_ParseCommands: Turret animations can only be played on the 'both' body part");
						}
					}
					else if ( !Q_stricmp( token, "blendtime" ) )
					{
						// read the blendtime
						bCommandFound = qtrue;
						token = Com_ParseOnLine( input );
						if ( !token || !token[0] )
						{
							BG_AnimParseError("BG_ParseCommands: expected blendtime value");
						}
						if ( !g_pLoadAnims )
						{
							scriptData->animations[command->animIndex[partIndex]].initialLerp = atoi( token );
						}
					}
					else
					{
						Com_UngetToken( );  // unget the token
					}
				}
				else
				{
					Com_UngetToken( );
				}
			}
			while ( bCommandFound );
			if ( command->bodyPart[partIndex] != ANIM_BP_BOTH && partIndex++ < 1 )
			{
				continue;   // allow parsing of another bodypart
			}
		}
		else
		{
			// unget the token
			*input -= strlen( token );
		}

		// parse optional parameters (sounds, etc)
		while ( 1 )
		{
			token = Com_ParseOnLine( input );
			if ( !token || !token[0] )
			{
				break;
			}

			if ( !Q_stricmp( token, "sound" ) )
			{
				token = Com_ParseOnLine( input );
				if ( !token || !token[0] )
				{
					BG_AnimParseError( "BG_ParseCommands: expected sound" );
				}
				// NOTE: only sound script are supported at this stage
				if ( strstr( token, ".wav" ) )
				{
					BG_AnimParseError( "BG_ParseCommands: wav files not supported, only sound scripts" );    // RF mod
				}
				// ydnar: this was crashing because soundIndex wasn't initialized
				// FIXME: find the reason
				// Gordon: hmmm, soundindex is setup on both client and server :/
				//	cgs.animScriptData.soundIndex = CG_SoundScriptPrecache;
				//	level.animScriptData.soundIndex = G_SoundIndex;
				command->soundAlias = globalScriptData->soundAlias( token );
			}
			else
			{
				// unknown??
				BG_AnimParseError( "BG_ParseCommands: unknown parameter '%s'", token );
			}
		}

		partIndex = 0;
	}
}

/*
=================
BG_PlayerAnimation
=================
*/
void BG_PlayerAnimation( const DObj *pDObj, entityState_t *es, clientInfo_t *ci )
{
	XAnimTree_s *pAnimTree;

	BG_PlayerAngles(es, ci);
	BG_AnimPlayerConditions(es, ci);

	pAnimTree = ci->pXAnimTree;

	BG_PlayerAnimation_VerifyAnim(pAnimTree, &ci->legs);
	BG_PlayerAnimation_VerifyAnim(pAnimTree, &ci->torso);

	if ( ci->leftHandGun && !( ci->torso.animationNumber & ~ANIM_TOGGLEBIT ) )
	{
		ci->leftHandGun = qfalse;
		ci->dobjDirty = qtrue;
	}

	BG_RunLerpFrameRate(ci, &ci->legs, es->legsAnim, es);
	BG_RunLerpFrameRate(ci, &ci->torso, es->torsoAnim, es);
}

/*
=================
BG_LoadAnim
=================
*/
void BG_LoadAnim()
{
	loadAnim_t playerAnims[MAX_MODEL_ANIMATIONS];
	int iNumPlayerAnims = 0;

	BG_FindAnims();
	BG_AnimParseAnimScript(&level_bgs.animData.animScriptData, playerAnims, &iNumPlayerAnims);
	Scr_PrecacheAnimTrees(level_bgs.AllocXAnim, level_bgs.anim_user);
	BG_FindAnimTrees();
	Scr_EndLoadAnimTrees();
	BG_FinalizePlayerAnims();
}

/*
=================
BG_LoadAnim
=================
*/
void BG_LerpAngles( vec3_t angles_goal, float maxAngleChange, vec3_t angles )
{
	int i;
	float diff;

	for ( i = 0; i < 3; i++ )
	{
		diff = angles_goal[i] - angles[i];

		if ( diff > maxAngleChange )
		{
			angles[i] = angles[i] + maxAngleChange;
			continue;
		}

		if ( -maxAngleChange > diff )
		{
			angles[i] = angles[i] - maxAngleChange;
			continue;
		}

		angles[i] = angles_goal[i];
	}
}

/*
=================
BG_PlayerAnimation_VerifyAnim
=================
*/
void BG_PlayerAnimation_VerifyAnim( XAnimTree *pAnimTree, lerpFrame_t *lf )
{
	if ( !lf->animationNumber )
	{
		return;
	}

	if ( XAnimGetWeight(pAnimTree, lf->animationNumber & ~ANIM_TOGGLEBIT))
	{
		return;
	}

	lf->animationNumber = 0;
	lf->animation = NULL;
	lf->animationTime = 150;
}

/*
===============
BG_EvaluateConditions
  returns qfalse if the set of conditions fails, qtrue otherwise
===============
*/
qboolean BG_EvaluateConditions( clientInfo_t *ci, animScriptItem_t *scriptItem )
{
	int i;
	animScriptCondition_t *cond;

	for ( i = 0, cond = scriptItem->conditions; i < scriptItem->numConditions; i++, cond++ )
	{
		switch ( animConditionsTable[cond->index].type )
		{
		case ANIM_CONDTYPE_BITFLAGS:
			if ( !( ci->clientConditions[cond->index][0] & cond->value[0] ) &&
			        !( ci->clientConditions[cond->index][1] & cond->value[1] ) )
			{
				return qfalse;
			}
			break;
		case ANIM_CONDTYPE_VALUE:
			if ( !( ci->clientConditions[cond->index][0] == cond->value[0] ) )
			{
				return qfalse;
			}
			break;
		default: // TTimo NUM_ANIM_CONDTYPES not handled
			break;
		}
	}
	//
	// all conditions must have passed
	return qtrue;
}

/*
===============
BG_SetupAnimNoteTypes
===============
*/
void BG_SetupAnimNoteTypes( animScriptData_t *scriptData )
{
	int cmdIndex;
	int animIndex;
	int itemIndex;
	animScriptItem_t *scriptItem;
	animScript_t *script;

	for ( animIndex = 0; animIndex < scriptData->numAnimations; animIndex++ )
	{
		scriptData->animations[animIndex].noteType = 0;
	}

	if ( level_bgs.anim_user )
	{
		return;
	}

	script = &scriptData->scriptEvents[ANIM_ET_RELOAD];

	for ( itemIndex = 0; itemIndex < script->numItems; itemIndex++ )
	{
		scriptItem = script->items[itemIndex];

		for ( cmdIndex = 0; cmdIndex < scriptItem->numCommands; cmdIndex++ )
		{
			if ( scriptItem->commands[cmdIndex].bodyPart[0] )
			{
				scriptData->animations[scriptItem->commands[cmdIndex].animIndex[0]].noteType = 1;
			}

			if ( scriptItem->commands[cmdIndex].bodyPart[1] )
			{
				scriptData->animations[scriptItem->commands[cmdIndex].animIndex[1]].noteType = 1;
			}
		}
	}
}

/*
================
BG_StringHashValue
  return a hash value for the given string
================
*/
long BG_StringHashValue( const char *fname )
{
	int i;
	long hash;

	hash = 0;
	i = 0;
	while ( fname[i] != '\0' )
	{
		hash += (long)( tolower(fname[i]) ) * ( i + 119 );
		i++;
	}
	if ( hash == -1 )
	{
		hash = 0;   // never return -1
	}
	return hash;
}

/*
==============
BG_GetConditionValue
==============
*/
int BG_GetConditionValue( clientInfo_t *ci, int condition, qboolean checkConversion )
{
	int i;

	if ( checkConversion && animConditionsTable[condition].type == ANIM_CONDTYPE_BITFLAGS )
	{
		// we may need to convert to a value
		//if (!value)
		//	return 0;
		for ( i = 0; i < 8 * sizeof( ci->clientConditions[0] ); i++ )
		{
			if ( Com_BitCheck( ci->clientConditions[condition], i ) )
			{
				return i;
			}
		}
		// nothing found
		return 0;
	}
	else
	{
		// xkan, 1/14/2003 - must use COM_BitCheck on the result.
		return ci->clientConditions[condition][0];
	}
	//BG_AnimParseError( "BG_GetConditionValue: internal error" );
}

/*
===============
BG_FirstValidItem
  scroll through the script items, returning the first script found to pass all conditions
  returns NULL if no match found
===============
*/
animScriptItem_t *BG_FirstValidItem( int client, animScript_t *script )
{
	animScriptItem_t **ppScriptItem;

	int i;

	for ( i = 0, ppScriptItem = script->items; i < script->numItems; i++, ppScriptItem++ )
	{
		if ( BG_EvaluateConditions( &level_bgs.clientinfo[client], *ppScriptItem ) )
		{
			return *ppScriptItem;
		}
	}
	//
	return NULL;
}

void BG_AnimPlayerConditions( entityState_t *es, clientInfo_t *ci )
{
	WeaponDef *weaponDef;
	int legsAnim;

	weaponDef = BG_GetWeaponDef(es->weapon);

	BG_UpdateConditionValue(es->clientNum, ANIM_COND_PLAYERANIMTYPE, weaponDef->playerAnimType, qtrue);
	BG_UpdateConditionValue(es->clientNum, ANIM_COND_WEAPONCLASS, weaponDef->weaponClass, qtrue);

	if ( es->eFlags & EF_AIMDOWNSIGHT )
		BG_UpdateConditionValue(es->clientNum, ANIM_COND_WEAPON_POSITION, qtrue, qtrue);
	else
		BG_UpdateConditionValue(es->clientNum, ANIM_COND_WEAPON_POSITION, qfalse, qtrue);

	if ( es->eFlags & EF_TURRET_ACTIVE )
		BG_UpdateConditionValue(es->clientNum, ANIM_COND_MOUNTED, MOUNTED_MG42, qtrue);
	else
		BG_UpdateConditionValue(es->clientNum, ANIM_COND_MOUNTED, MOUNTED_UNUSED, qtrue);

	BG_UpdateConditionValue(es->clientNum, ANIM_COND_UNDERHAND, ci->playerAngles[0] > 0.0, qtrue);

	if ( es->eFlags & EF_CROUCHING )
		BG_UpdateConditionValue(es->clientNum, ANIM_COND_CROUCHING, qtrue, qtrue);
	else
		BG_UpdateConditionValue(es->clientNum, ANIM_COND_CROUCHING, qfalse, qtrue);

	if ( es->eFlags & EF_FIRING )
		BG_UpdateConditionValue(es->clientNum, ANIM_COND_FIRING, qtrue, qtrue);
	else
		BG_UpdateConditionValue(es->clientNum, ANIM_COND_FIRING, qfalse, qtrue);

	legsAnim = es->legsAnim & ~ANIM_TOGGLEBIT;

	if ( level_bgs.animData.animScriptData.animations[legsAnim].movetype )
	{
		if ( BG_GetConditionValue(ci, ANIM_COND_MOVETYPE, qfalse) != level_bgs.animData.animScriptData.animations[legsAnim].movetype )
		{
			BG_UpdateConditionValue( es->clientNum, ANIM_COND_MOVETYPE, level_bgs.animData.animScriptData.animations[legsAnim].movetype, qfalse);
		}
	}

	if ( level_bgs.animData.animScriptData.animations[legsAnim].flags & 0x10 )
	{
		BG_UpdateConditionValue(es->clientNum, ANIM_COND_STRAFING, LEANING_LEFT, qtrue);
	}
	else if ( level_bgs.animData.animScriptData.animations[legsAnim].flags & 0x20 )
	{
		BG_UpdateConditionValue(es->clientNum, ANIM_COND_STRAFING, LEANING_RIGHT, qtrue);
	}
	else
	{
		BG_UpdateConditionValue(es->clientNum, ANIM_COND_STRAFING, LEANING_NOT, qtrue);
	}
}

/*
==================
BG_SwingAngles
==================
*/
void BG_SwingAngles( float destination,  float swingTolerance, float clampTolerance, float speed, float *angle, qboolean *swinging )
{
	float	swing;
	float	move;
	float	scale;

	if ( !*swinging )
	{
		// see if a swing should be started
		swing = AngleSubtract( *angle, destination );
		if ( swing > swingTolerance || swing < -swingTolerance )
		{
			*swinging = qtrue;
		}
	}

	if ( !*swinging )
	{
		return;
	}

	// modify the speed depending on the delta
	// so it doesn't seem so linear
	swing = AngleSubtract( destination, *angle );
	scale = I_fabs( swing ) * 0.050000001;
	if ( scale < 0.5 )
	{
		scale = 0.5;
	}
	// swing towards the destination angle
	if ( swing >= 0 )
	{
		move = level_bgs.frametime * scale * speed;
		if ( move >= swing )
		{
			move = swing;
			*swinging = qfalse;
		}
		else
		{
			*swinging = qtrue;
		}
	}
	else
	{
		move = level_bgs.frametime * scale * -speed;
		if ( swing >= move )
		{
			move = swing;
			*swinging = qfalse;
		}
		else
		{
			*swinging = qtrue;
		}
	}

	*angle = AngleMod( *angle + move );
	// clamp to no more than tolerance
	swing = AngleSubtract( destination, *angle );
	if ( swing > clampTolerance )
	{
		*angle = AngleMod(destination - clampTolerance);
	}
	else
	{
		if ( -clampTolerance > swing )
		{
			*angle = AngleMod(destination + clampTolerance);
		}
	}
}

/*
===============
BG_PlayerAngles

Handles seperate torso motion

  legs pivot based on direction of movement

  head always looks exactly at cent->lerpAngles

  if motion < 20 degrees, show in head only
  if < 45 degrees, also show in torso
===============
*/
void BG_PlayerAngles( const entityState_s *es, clientInfo_t *ci ) // not sure about this
{
	vec3_t vHeadAngles;
	vec3_t vTorsoAngles;
	vec3_t vLegsAngles;
	float moveDir;
	float fraction;
	float clampTolerance;
	float destination;

	fraction = GetLeanFraction(ci->lerpLean);
	moveDir = ci->lerpMoveDir;

	VectorCopy(ci->playerAngles, vHeadAngles);
	vHeadAngles[YAW] = AngleMod(vHeadAngles[YAW]);
	VectorClear(vLegsAngles);
	VectorClear(vTorsoAngles);

	// --------- yaw -------------

	// allow yaw to drift a bit, unless these conditions don't allow them
	if ( es->eFlags & EF_TURRET_ACTIVE )
	{
		// if not standing still, always point all in the same direction
		ci->torso.yawing = qtrue;
		ci->torso.pitching = qtrue;
		ci->legs.yawing = qtrue;
	}
	else if ( (BG_GetConditionValue(ci, ANIM_COND_MOVETYPE, qfalse) & ((1<<ANIM_MT_CLIMBUP)|(1<<ANIM_MT_CLIMBDOWN)) ) )
	{
		ci->torso.yawing = qtrue;
		ci->torso.pitching = qtrue;
		ci->legs.yawing = qtrue;
	}
	else if ( es->eFlags & EF_MANTLE )
	{
		ci->torso.yawing = qtrue;
		ci->torso.pitching = qtrue;
		ci->legs.yawing = qtrue;
	}
	else if ( (BG_GetConditionValue(ci, ANIM_COND_MOVETYPE, qfalse) & ((1<<ANIM_MT_IDLE)|(1<<ANIM_MT_IDLECR)) ) )
	{
		// if firing, make sure torso and head are always aligned
		if ( BG_GetConditionValue(ci, ANIM_COND_FIRING, qtrue) )
		{
			ci->torso.yawing = qtrue;
			ci->torso.pitching = qtrue;
		}
	}
	else
	{
		ci->torso.yawing = qtrue;
		ci->torso.pitching = qtrue;
		ci->legs.yawing = qtrue;
	}

	// adjust legs for movement dir
	vLegsAngles[YAW] = vHeadAngles[YAW] + moveDir;

	if ( es->eFlags & EF_DEAD )
	{
		// don't let dead bodies twitch
		vLegsAngles[YAW] = vHeadAngles[YAW];
		vTorsoAngles[YAW] = vHeadAngles[YAW];
		clampTolerance = 90.0;
	}
	else if ( (BG_GetConditionValue(ci, ANIM_COND_MOVETYPE, qfalse) & ((1<<ANIM_MT_CLIMBUP)|(1<<ANIM_MT_CLIMBDOWN)) ) )
	{
		vTorsoAngles[YAW] = vLegsAngles[YAW];
		clampTolerance = 0.0;
	}
	else if ( es->eFlags & EF_MANTLE )
	{
		vTorsoAngles[YAW] = vHeadAngles[YAW];
		vLegsAngles[YAW] = vHeadAngles[YAW];
		vTorsoAngles[PITCH] = 90.0;
		clampTolerance = 90.0;
	}
	else if ( es->eFlags & EF_PRONE )
	{
		vTorsoAngles[YAW] = vHeadAngles[YAW];
		clampTolerance = 90.0;
	}
	else if ( es->eFlags & EF_FIRING )
	{
		vTorsoAngles[YAW] = vHeadAngles[YAW];
		clampTolerance = 45.0;
	}
	else if ( es->eFlags & EF_AIMDOWNSIGHT )
	{
		vTorsoAngles[YAW] = vHeadAngles[YAW];
		clampTolerance = 90.0;
	}
	else
	{
		vTorsoAngles[YAW] = moveDir * 0.30000001 + vHeadAngles[YAW];
		clampTolerance = 90.0;
	}

	// torso
	BG_SwingAngles(vTorsoAngles[YAW], 0.0, clampTolerance, bg_swingSpeed->current.decimal, &ci->torso.yawAngle, &ci->torso.yawing);

	assert(level_bgs.animData.animScriptData.animations);

	// if the legs are yawing (facing heading direction), allow them to rotate a bit, so we don't keep calling
	// the legs_turn animation while an AI is firing, and therefore his angles will be randomizing according to their accuracy

	clampTolerance = 150.0;

	if ( es->eFlags & EF_DEAD )
	{
		BG_SwingAngles(vLegsAngles[YAW], 0.0, clampTolerance, bg_swingSpeed->current.decimal, &ci->legs.yawAngle, &ci->legs.yawing);
	}
	else if ( es->eFlags & EF_PRONE )
	{
		ci->legs.yawing = qfalse;
		ci->legs.yawAngle = vHeadAngles[YAW] + moveDir;
	}
	else if ( level_bgs.animData.animScriptData.animations[es->legsAnim & ~ANIM_TOGGLEBIT].flags & 0x30 )
	{
		ci->legs.yawing = qfalse; // set it if they really need to swing
		vLegsAngles[YAW] = vHeadAngles[YAW];
		BG_SwingAngles(vHeadAngles[YAW], 0.0, clampTolerance, bg_swingSpeed->current.decimal, &ci->legs.yawAngle, &ci->legs.yawing);
	}
	else if ( ci->legs.yawing )
	{
		BG_SwingAngles(vLegsAngles[YAW], 0.0, clampTolerance, bg_swingSpeed->current.decimal, &ci->legs.yawAngle, &ci->legs.yawing);
	}
	else
	{
		BG_SwingAngles(vLegsAngles[YAW], 40.0, clampTolerance, bg_swingSpeed->current.decimal, &ci->legs.yawAngle, &ci->legs.yawing);
	}

	if ( es->eFlags & EF_TURRET_ACTIVE )
	{
		ci->torso.yawAngle = vHeadAngles[YAW];
		ci->legs.yawAngle = vHeadAngles[YAW];
	}
	else if ( (BG_GetConditionValue(ci, ANIM_COND_MOVETYPE, qfalse) & ((1<<ANIM_MT_CLIMBUP)|(1<<ANIM_MT_CLIMBDOWN)) ) )
	{
		ci->torso.yawAngle = vHeadAngles[YAW] + moveDir;
		ci->legs.yawAngle = vHeadAngles[YAW] + moveDir;
	}

	if ( es->eFlags & EF_DEAD )
	{
		destination = 0.0;
	}
	else if ( es->eFlags & EF_TURRET_ACTIVE )
	{
		destination = 0.0;
	}
	else if ( (BG_GetConditionValue(ci, ANIM_COND_MOVETYPE, qfalse) & ((1<<ANIM_MT_CLIMBUP)|(1<<ANIM_MT_CLIMBDOWN)) ) )
	{
		destination = 0.0;
	}
	else if ( es->eFlags == EF_MANTLE )
	{
		destination = 0.0;
	}
	else if ( vHeadAngles[PITCH] <= 180.0 )
	{
		destination = vHeadAngles[0] * 0.60000002;
	}
	else
	{
		destination = (vHeadAngles[0] + -360.0) * 0.60000002;
	}

	BG_SwingAngles(destination, 0.0, 45.0, 0.15000001, &ci->torso.pitchAngle, &ci->torso.pitching);
}

/*
===============
BG_LoadAnimForAnimIndex
===============
*/
loadAnim_t* BG_LoadAnimForAnimIndex( unsigned int index )
{
	loadAnim_t *pAnim;
	int i;

	if ( index >= globalScriptData->numAnimations )
	{
		Com_Error(ERR_DROP, "Player animation index %i out of 0 to %i range", index, globalScriptData->numAnimations);
	}

	for ( i = 0, pAnim = g_pLoadAnims; i < *g_piNumLoadAnims; i++, pAnim++ )
	{
		if ( index == pAnim->anim.index )
		{
			return pAnim;
		}
	}

	return NULL;
}

/*
===============
BG_FindAnimTree
===============
*/
void BG_FindAnimTree( scr_animtree_t *pAnimTree, const char *filename, qboolean bEnforceExists )
{
	scr_animtree_t localTree;

	Scr_FindAnimTree(&localTree, filename);
	pAnimTree->anims = localTree.anims;

	if ( !pAnimTree->anims )
	{
		if ( bEnforceExists )
			Com_Error(ERR_DROP, "Could not find animation tree %s", filename);
	}
}

/*
=================
BG_ParseConditionBits
  convert the string into a single int containing bit flags, stopping at a ',' or end of line
=================
*/
void BG_ParseConditionBits( const char **text_pp, animStringItem_t *stringTable, int condIndex, int result[2] )
{
	qboolean endFlag = qfalse;
	int indexFound;
	int tempBits[2];
	char currentString[MAX_QPATH];
	qboolean minus = qfalse;
	char *token;

	currentString[0] = '\0';
	memset( result, 0, sizeof( result ) );
	memset( tempBits, 0, sizeof( tempBits ) );

	while ( !endFlag )
	{
		token = Com_ParseOnLine( text_pp );
		if ( !token || !token[0] )
		{
			Com_UngetToken(); // go back to the previous token
			endFlag = qtrue;    // done parsing indexes
			if ( !strlen( currentString ) )
			{
				break;
			}
		}

		if ( !Q_stricmp( token, "," ) )
		{
			endFlag = qtrue;    // end of indexes
		}

		if ( !Q_stricmp( token, "none" ) )   // first bit is always the "unused" bit
		{
			Com_BitSet( result, 0 );
			continue;
		}

		if ( !Q_stricmp( token, "none," ) )      // first bit is always the "unused" bit
		{
			Com_BitSet( result, 0 );
			endFlag = qtrue;    // end of indexes
			continue;
		}

		if ( !Q_stricmp( token, "NOT" ) )
		{
			token = "MINUS"; // NOT is equivalent to MINUS
		}

		if ( !endFlag && Q_stricmp( token, "AND" ) && Q_stricmp( token, "MINUS" ) )   // must be a index
		{
			// check for a comma (end of indexes)
			if ( token[strlen( token ) - 1] == ',' )
			{
				endFlag = qtrue;
				token[strlen( token ) - 1] = '\0';
			}
			// append this to the currentString
			if ( strlen( currentString ) )
			{
				Q_strncat( currentString, sizeof( currentString ), " " );
			}
			Q_strncat( currentString, sizeof( currentString ), token );
		}

		if ( !Q_stricmp( token, "AND" ) || !Q_stricmp( token, "MINUS" ) || endFlag )
		{
			// process the currentString
			if ( !strlen( currentString ) )
			{
				if ( endFlag )
				{
					BG_AnimParseError( "BG_AnimParseAnimScript: unexpected end of condition" );
				}
				else
				{
					// check for minus indexes to follow
					if ( !Q_stricmp( token, "MINUS" ) )
					{
						minus = qtrue;
						continue;
					}
					BG_AnimParseError( "BG_AnimParseAnimScript: unexpected '%s'", token );
				}
			}
			if ( !Q_stricmp( currentString, "all" ) )
			{
				tempBits[0] = ~0x0;
				tempBits[1] = ~0x0;
			}
			else
			{
				// first check this string with our defines
				indexFound = BG_IndexForString( currentString, defineStr[condIndex], qtrue );
				if ( indexFound >= 0 )
				{
					// we have precalculated the bitflags for the defines
					tempBits[0] = defineBits[condIndex][indexFound][0];
					tempBits[1] = defineBits[condIndex][indexFound][1];
				}
				else
				{
					// convert the string into an index
					indexFound = BG_IndexForString( currentString, stringTable, qfalse );
					// convert the index into a bitflag
					Com_BitSet( tempBits, indexFound );
				}
			}
			// perform operation
			if ( minus )      // subtract
			{
				result[0] &= ~tempBits[0];
				result[1] &= ~tempBits[1];
			}
			else            // add
			{
				result[0] |= tempBits[0];
				result[1] |= tempBits[1];
			}
			// clear the currentString
			currentString[0] = '\0';
			// check for minus indexes to follow
			if ( !Q_stricmp( token, "MINUS" ) )
			{
				minus = qtrue;
			}
		}
	}
}

/*
=================
BG_FindAnimTrees
=================
*/
void BG_FindAnimTrees()
{
	scr_animtree_t loadAnimTree;

	BG_FindAnimTree(&loadAnimTree, DEFAULT_ANIM_TREE_NAME, qtrue);

	level_bgs.animData.generic_human.tree = loadAnimTree;

	level_bgs.animData.animScriptData.animTree = level_bgs.animData.generic_human.tree;
	level_bgs.animData.animScriptData.torsoAnim = level_bgs.animData.generic_human.torso.index;
	level_bgs.animData.animScriptData.legsAnim = level_bgs.animData.generic_human.legs.index;
	level_bgs.animData.animScriptData.turningAnim = level_bgs.animData.generic_human.turning.index;
}

/*
=================
BG_FindAnims
=================
*/
void BG_FindAnims()
{
	Scr_FindAnim(DEFAULT_ANIM_TREE_NAME, "root", &level_bgs.animData.generic_human.root, level_bgs.anim_user);
	Scr_FindAnim(DEFAULT_ANIM_TREE_NAME, "torso", &level_bgs.animData.generic_human.torso, level_bgs.anim_user);
	Scr_FindAnim(DEFAULT_ANIM_TREE_NAME, "legs", &level_bgs.animData.generic_human.legs, level_bgs.anim_user);
	Scr_FindAnim(DEFAULT_ANIM_TREE_NAME, "turning", &level_bgs.animData.generic_human.turning, level_bgs.anim_user);
}

/*
=================
BG_SetNewAnimation
=================
*/
void BG_SetNewAnimation( clientInfo_t *ci, lerpFrame_t *lf, int newAnimation, qboolean bNew )
{
	qboolean IsCrouchingAnim;
	qboolean IsProneAnim;
	qboolean bNoWeight;
	qboolean proneMatch;
	qboolean crouchMatch;
	unsigned int animIndex;
	unsigned int oldAnimIndex;
	int cycleLen;
	XAnim *pXAnims;
	XAnimTree *pAnimTree;
	float fStartTime;
	int oldAnimNum;
	int transitionMin;
	animation_t *anim;
	animation_t *oldanim;

	transitionMin = -1;
	fStartTime = 0.0;

	oldanim = lf->animation;
	oldAnimNum = lf->animationNumber;

	lf->animationNumber = newAnimation;
	animIndex = newAnimation & ~ANIM_TOGGLEBIT;

	if ( animIndex >= level_bgs.animData.animScriptData.numAnimations )
	{
		Com_Error(ERR_DROP, "Player animation index out of range (%i): %i", level_bgs.animData.animScriptData.numAnimations, animIndex);
	}

	pAnimTree = ci->pXAnimTree;
	pXAnims = level_bgs.animData.animScriptData.animTree.anims;

	if ( animIndex )
	{
		anim = &level_bgs.animData.animScriptData.animations[animIndex];

		lf->animation = anim;
		lf->animationTime = anim->initialLerp;

		IsCrouchingAnim = BG_IsCrouchingAnim(ci, animIndex);
		crouchMatch = IsCrouchingAnim == BG_IsCrouchingAnim(ci, oldAnimNum);

		IsProneAnim = BG_IsProneAnim(ci, animIndex);
		proneMatch = IsProneAnim == BG_IsProneAnim(ci, oldAnimNum);

		if ( lf == &ci->legs && (!crouchMatch || !proneMatch) )
		{
			ci->stanceTransitionTime = level_bgs.time + 400;
		}
	}
	else
	{
		anim = NULL;

		lf->animation = NULL;
		lf->animationTime = 200;
	}

	if ( oldanim || lf != &ci->legs )
	{
		if ( !anim || lf->animationTime <= 0 )
		{
			if ( !anim || anim->moveSpeed == 0.0 )
			{
				if ( !oldanim || oldanim->moveSpeed == 0.0 )
				{
					transitionMin = 170;
				}
				else
				{
					transitionMin = 250;
				}
			}
			else
			{
				transitionMin = 120;
			}
		}

		if ( ci->stanceTransitionTime - level_bgs.time > transitionMin )
		{
			transitionMin = ci->stanceTransitionTime - level_bgs.time;
		}

		if ( lf->animationTime < transitionMin )
		{
			lf->animationTime = transitionMin;
		}
	}
	else
	{
		lf->animationTime = 0;
	}

	if ( anim && anim->moveSpeed != 0.0 && XAnimIsLooped(pXAnims, animIndex) )
	{
		oldAnimIndex = oldAnimNum & ~ANIM_TOGGLEBIT;

		if ( oldanim && oldanim->moveSpeed != 0.0 && XAnimIsLooped(pXAnims, oldAnimIndex) )
		{
			fStartTime = XAnimGetTime(pAnimTree, oldAnimIndex);
		}
		else
		{
			if ( XAnimIsPrimitive(pXAnims, oldAnimIndex) )
			{
				cycleLen = XAnimGetLengthMsec(pXAnims, oldAnimIndex) + 200;
			}
			else
			{
				cycleLen = 1000;
			}

			fStartTime = (level_bgs.time % cycleLen) / cycleLen + ci->clientNum * 0.36000001;
			fStartTime = fStartTime - (int)fStartTime;
		}
	}

	if ( oldanim )
	{
		XAnimClearGoalWeight(pAnimTree, oldAnimNum & ~ANIM_TOGGLEBIT, lf->animationTime * 0.001);
	}

	if ( animIndex )
	{
		if ( lf != &ci->legs )
		{
			ci->leftHandGun = qfalse;
			ci->dobjDirty = qtrue;
		}

		if ( anim->flags & 0x40 )
		{
			if ( XAnimIsLooped(pXAnims, animIndex) )
			{
				Com_Error(ERR_DROP, "death animation '%s' is looping", anim->name);
			}

			if ( bNew )
			{
				XAnimSetCompleteGoalWeight(pAnimTree, animIndex, 1.0, lf->animationTime * 0.001, 1.0, 0, 0, 0);
			}
			else
			{
				XAnimSetCompleteGoalWeightKnobAll(pAnimTree, animIndex, level_bgs.animData.generic_human.root.index, 1.0, 0.0, 1.0, 0, 0);
				XAnimSetTime(pAnimTree, animIndex, 1.0);
			}
		}
		else
		{
			bNoWeight = qfalse;

			if ( anim->moveSpeed != 0.0 )
			{
				bNoWeight = XAnimGetWeight(pAnimTree, animIndex) == 0.0;
			}

			XAnimSetCompleteGoalWeight(pAnimTree, animIndex, 1.0, lf->animationTime * 0.001, 1.0, 0, anim->noteType, lf != &ci->legs);

			if ( bNoWeight )
			{
				XAnimSetTime(pAnimTree, animIndex, fStartTime);
			}
		}

		if ( lf != &ci->legs )
		{
			XAnimSetCompleteGoalWeight(pAnimTree, level_bgs.animData.animScriptData.torsoAnim, 1.0, lf->animationTime * 0.001, 1.0, 0, anim->noteType, 0);
			XAnimSetCompleteGoalWeight(pAnimTree, level_bgs.animData.animScriptData.legsAnim, 0.0099999998, lf->animationTime * 0.001, 1.0, 0, anim->noteType, 0);
		}
	}
	else if ( lf != &ci->legs )
	{
		XAnimSetCompleteGoalWeight(pAnimTree, level_bgs.animData.animScriptData.torsoAnim, 0.0, lf->animationTime * 0.001, 1.0, 0, 0, 0);
		XAnimSetCompleteGoalWeight(pAnimTree, level_bgs.animData.animScriptData.legsAnim, 1.0, lf->animationTime * 0.001, 1.0, 0, 0, 0);
	}
}

/*
=================
BG_ParseConditions
  returns qtrue if everything went ok, error drops otherwise
=================
*/
qboolean BG_ParseConditions( const char **text_pp, animScriptItem_t *scriptItem )
{
	int conditionIndex, conditionValue[2];
	char    *token;

	memset(conditionValue, 0, sizeof(conditionValue));

	while ( 1 )
	{
		token = Com_ParseOnLine( text_pp );
		if ( !token || !token[0] )
		{
			break;
		}

		// special case, "default" has no conditions
		if ( !Q_stricmp( token, "default" ) )
		{
			return qtrue;
		}

		conditionIndex = BG_IndexForString( token, animConditionsStr, qfalse );

		switch ( animConditionsTable[conditionIndex].type )
		{
		case ANIM_CONDTYPE_BITFLAGS:
			BG_ParseConditionBits( text_pp, animConditionsTable[conditionIndex].values, conditionIndex, conditionValue );
			break;
		case ANIM_CONDTYPE_VALUE:
			if ( animConditionsTable[conditionIndex].values )
			{
				token = Com_ParseOnLine( text_pp );
				if ( !token || !token[0] )
				{
					BG_AnimParseError( "BG_AnimParseAnimScript: expected condition value, found end of line" );  // RF modification
				}
				// check for a comma (condition divider)
				if ( token[strlen( token ) - 1] == ',' )
				{
					token[strlen( token ) - 1] = '\0';
				}
				conditionValue[0] = BG_IndexForString( token, animConditionsTable[conditionIndex].values, qfalse );
			}
			else
			{
				conditionValue[0] = 1;  // not used, just check for a positive condition
			}
			break;
		default: // TTimo gcc: NUM_ANIM_CONDTYPES not handled in switch
			break;
		}

		// now append this condition to the item
		scriptItem->conditions[scriptItem->numConditions].index = conditionIndex;
		scriptItem->conditions[scriptItem->numConditions].value[0] = conditionValue[0];
		scriptItem->conditions[scriptItem->numConditions].value[1] = conditionValue[1];
		scriptItem->numConditions++;
	}

	if ( scriptItem->numConditions == 0 )
	{
		BG_AnimParseError( "BG_ParseConditions: no conditions found" );  // RF mod
	}

	return qtrue;
}

/*
=================
BG_RunLerpFrameRate
=================
*/
void BG_RunLerpFrameRate( clientInfo_t *ci, lerpFrame_t *lf, int newAnimation, const entityState_t *es )
{
	qboolean bNewAnim = qfalse;
	float fScaleMax;
	float moveSpeed;
	XAnimTree *pAnimTree;
	animation_t *anim;

	if ( lf->animation )
	{
		bNewAnim = lf->animation->flags & 2;
	}

	pAnimTree = ci->pXAnimTree;

	if ( newAnimation != lf->animationNumber || !lf->animation && newAnimation & ~ANIM_TOGGLEBIT )
	{
		BG_SetNewAnimation(ci, lf, newAnimation, es->eFlags & EF_BODY_START);
	}

	if ( !(newAnimation & ~ANIM_TOGGLEBIT) )
	{
		return;
	}

	anim = lf->animation;

	if ( anim->moveSpeed == 0.0 || !lf->oldFrameSnapshotTime )
	{
		lf->animSpeedScale = 1.0;
		lf->oldFrameSnapshotTime = level_bgs.latestSnapshotTime;

		VectorCopy(es->pos.trBase, lf->oldFramePos);
	}
	else if ( level_bgs.latestSnapshotTime != lf->oldFrameSnapshotTime )
	{
		if ( bNewAnim )
		{
			moveSpeed = I_fabs(lf->oldFramePos[2] - es->pos.trBase[2]);
		}
		else
		{
			moveSpeed = Vec3Distance(lf->oldFramePos, es->pos.trBase);
		}

		lf->animSpeedScale = (moveSpeed / ((level_bgs.latestSnapshotTime - lf->oldFrameSnapshotTime) * 0.001)) / anim->moveSpeed;
		lf->oldFrameSnapshotTime = level_bgs.latestSnapshotTime;

		VectorCopy(es->pos.trBase, lf->oldFramePos);

		if ( lf->animSpeedScale >= 0.1 )
		{
			if ( lf->animSpeedScale > 2.0 )
			{
				if ( anim->flags & 2 )
				{
					if ( lf->animSpeedScale > 4.0 )
					{
						lf->animSpeedScale = 4.0;
					}
				}
				else if ( anim->moveSpeed <= 150.0 )
				{
					if ( anim->moveSpeed >= 20.0 )
					{
						fScaleMax = 3.0 - (anim->moveSpeed - 20.0) / 130.0;

						if ( lf->animSpeedScale > fScaleMax )
						{
							lf->animSpeedScale = fScaleMax;
						}
					}
					else if ( lf->animSpeedScale > 3.0 )
					{
						lf->animSpeedScale = 3.0;
					}
				}
				else
				{
					lf->animSpeedScale = 2.0;
				}
			}
		}
		else if ( lf->animSpeedScale < 0.0099999998 && bNewAnim )
		{
			lf->animSpeedScale = 0.0;
		}
		else
		{
			lf->animSpeedScale = 0.1;
		}
	}

	if ( lf->animationNumber )
	{
		XAnimSetAnimRate(pAnimTree, lf->animationNumber & ~ANIM_TOGGLEBIT, lf->animSpeedScale);
	}
}

/*
=================
BG_AnimParseAnimScript
  Parse the animation script for this model, converting it into run-time structures
=================
*/
void BG_AnimParseAnimScript( animScriptData_t *scriptData, loadAnim_t *pLoadAnims, int *piNumAnims )
{
#define MAX_INDENT_LEVELS   3

	const char    *text_p, *token;
	int parseMode;
	animScript_t        *currentScript;
	animScriptItem_t tempScriptItem;
	// TTimo gcc: might be used unitialized
	animScriptItem_t *currentScriptItem = NULL;
	int indexes[MAX_INDENT_LEVELS], indentLevel, oldState, newParseMode;
	int i, defineType;

	static qboolean bScriptFileLoaded = qfalse;
	static char ScriptFileBuffer[100000];

	if ( !bScriptFileLoaded )
	{
		fileHandle_t f;
		int len = FS_FOpenFileByMode(globalFilename, &f, FS_READ);

		if ( len < 0 )
			Com_Error(ERR_DROP, "Couldn't load player animation script %s\n", globalFilename);

		if ( len >= sizeof(ScriptFileBuffer) - 1 )
			Com_Error(ERR_DROP, "Couldn't load player animation script %s\n", globalFilename);

		FS_Read(ScriptFileBuffer, len, f);
		ScriptFileBuffer[len] = 0;
		FS_FCloseFile(f);
		bScriptFileLoaded = qtrue;
	}

	// the scriptData passed into here must be the one this binary is using
	globalScriptData = scriptData;

	g_pLoadAnims = pLoadAnims;
	g_piNumLoadAnims = piNumAnims;

	// start at the defines
	parseMode = PARSEMODE_DEFINES;

	BG_InitWeaponStrings();

	// init the global defines
	memset( defineStr, 0, sizeof( defineStr ) );
	memset( defineStrings, 0, sizeof( defineStrings ) );
	memset( numDefines, 0, sizeof( numDefines ) );
	defineStringsOffset = 0;

	for ( i = 0; i < MAX_INDENT_LEVELS; i++ )
		indexes[i] = -1;
	indentLevel = 0;
	currentScript = NULL;

	text_p = ScriptFileBuffer;
	Com_BeginParseSession( "BG_AnimParseAnimScript" );

	// read in the weapon defines
	while ( 1 )
	{
		token = Com_Parse( &text_p );
		if ( !token || !token[0] )
		{
			break;
		}

		// check for a new section
		newParseMode = BG_IndexForString( token, animParseModesStr, qtrue );
		if ( newParseMode >= 0 )
		{
			if ( indentLevel )
			{
				BG_AnimParseError( "BG_AnimParseAnimScript: unexpected '%s'", token );   // RF mod
			}

			parseMode = newParseMode;
			parseMovetype = ANIM_MT_UNUSED;
			parseEvent = -1;
			continue;
		}

		switch ( parseMode )
		{
		case PARSEMODE_DEFINES:
			if ( !Q_stricmp( token, "set" ) )
			{
				// read in the define type
				token = Com_ParseOnLine( &text_p );
				if ( !token || !token[0] )
				{
					BG_AnimParseError( "BG_AnimParseAnimScript: expected condition type string" );   // RF mod
				}
				defineType = BG_IndexForString( token, animConditionsStr, qfalse );

				// read in the define
				token = Com_ParseOnLine( &text_p );
				if ( !token || !token[0] )
				{
					BG_AnimParseError( "BG_AnimParseAnimScript: expected condition define string" ); // RF mod
				}

				// copy the define to the strings list
				defineStr[defineType][numDefines[defineType]].string = BG_CopyStringIntoBuffer( token, defineStrings, sizeof( defineStrings ), &defineStringsOffset );
				defineStr[defineType][numDefines[defineType]].hash = BG_StringHashValue( defineStr[defineType][numDefines[defineType]].string );
				// expecting an =
				token = Com_ParseOnLine( &text_p );
				if ( !token )
				{
					BG_AnimParseError( "BG_AnimParseAnimScript: expected '=', found end of line" );  // RF mod
				}
				if ( Q_stricmp( token, "=" ) )
				{
					BG_AnimParseError( "BG_AnimParseAnimScript: expected '=', found '%s'", token );  // RF mod
				}

				// parse the bits
				BG_ParseConditionBits( &text_p, animConditionsTable[defineType].values, defineType, defineBits[defineType][numDefines[defineType]] );
				numDefines[defineType]++;
			}

			break;

		case PARSEMODE_ANIMATION:
		case PARSEMODE_CANNED_ANIMATIONS:
			if ( !Q_stricmp( token, "{" ) )
			{
				// about to increment indent level, check that we have enough information to do this
				if ( indentLevel >= MAX_INDENT_LEVELS )   // too many indentations
				{
					BG_AnimParseError( "BG_AnimParseAnimScript: unexpected '%s'", token );   // RF mod
				}
				if ( indexes[indentLevel] < 0 )       // we havent found out what this new group is yet
				{
					BG_AnimParseError( "BG_AnimParseAnimScript: unexpected '%s'", token );   // RF mod
				}
				//
				indentLevel++;
			}
			else if ( !Q_stricmp( token, "}" ) )
			{
				// reduce the indentLevel
				indentLevel--;
				if ( indentLevel < 0 )
				{
					BG_AnimParseError( "BG_AnimParseAnimScript: unexpected '%s'", token );   // RF mod
				}
				if ( indentLevel == 1 )
				{
					currentScript = NULL;
				}
				// make sure we read a new index before next indent
				indexes[indentLevel] = -1;

			}
			else if ( indentLevel == 0 && ( indexes[indentLevel] < 0 ) )
			{
				if ( Q_stricmp( token, "state" ) )
				{
					BG_AnimParseError( "BG_AnimParseAnimScript: expected 'state'" ); // RF mod
				}

				// read in the state type
				token = Com_ParseOnLine( &text_p );
				if ( !token )
				{
					BG_AnimParseError( "BG_AnimParseAnimScript: expected state type" );  // RF mod
				}
				indexes[indentLevel] = BG_IndexForString( token, animStateStr, qfalse );

//----(SA) // RF mod
				// check for the open bracket
				token = Com_Parse( &text_p );
				if ( !token || Q_stricmp( token, "{" ) )
				{
					BG_AnimParseError( "BG_AnimParseAnimScript: expected '{'" );
				}
				indentLevel++;
//----(SA) // RF mod
			}
			else if ( ( indentLevel == 1 ) && ( indexes[indentLevel] < 0 ) )
			{
				// we are expecting a movement type
				indexes[indentLevel] = BG_IndexForString( token, animMoveTypesStr, qfalse );
				if ( parseMode == PARSEMODE_ANIMATION )
				{
					currentScript = &scriptData->scriptAnims[indexes[0]][indexes[1]];
					parseMovetype = indexes[1];
				}
				else if ( parseMode == PARSEMODE_CANNED_ANIMATIONS )
				{
					currentScript = &scriptData->scriptCannedAnims[indexes[0]][indexes[1]];
				}
				memset( currentScript, 0, sizeof( *currentScript ) );
			}
			else if ( ( indentLevel == 2 ) && ( indexes[indentLevel] < 0 ) )
			{
				// we are expecting a condition specifier
				// move the text_p backwards so we can read in the last token again
				text_p -= strlen( token );
				// sanity check that
				if ( Q_strncmp( text_p, token, strlen( token ) ) )
				{
					// this should never happen, just here to check that this operation is correct before code goes live
					BG_AnimParseError( "BG_AnimParseAnimScript: internal error" );
				}
				//
				memset( &tempScriptItem, 0, sizeof( tempScriptItem ) );
				indexes[indentLevel] = BG_ParseConditions( &text_p, &tempScriptItem );
				// do we have enough room in this script for another item?
				if ( currentScript->numItems >= MAX_ANIMSCRIPT_ITEMS )
				{
					BG_AnimParseError( "BG_AnimParseAnimScript: exceeded maximum items per script (%i)", MAX_ANIMSCRIPT_ITEMS ); // RF mod
				}
				// are there enough items left in the global list?
				if ( scriptData->numScriptItems >= MAX_ANIMSCRIPT_ITEMS_PER_MODEL )
				{
					BG_AnimParseError( "BG_AnimParseAnimScript: exceeded maximum global items (%i)", MAX_ANIMSCRIPT_ITEMS_PER_MODEL );   // RF mod
				}
				// it was parsed ok, so grab an item from the global list to use
				currentScript->items[currentScript->numItems] = &scriptData->scriptItems[ scriptData->numScriptItems++ ];
				currentScriptItem = currentScript->items[currentScript->numItems];
				currentScript->numItems++;
				// copy the data across from the temp script item
				*currentScriptItem = tempScriptItem;
			}
			else if ( indentLevel == 3 )
			{
				// we are reading the commands, so parse this line as if it were a command
				// move the text_p backwards so we can read in the last token again
				text_p -= strlen( token );
				// sanity check that
				if ( Q_strncmp( text_p, token, strlen( token ) ) )
				{
					// this should never happen, just here to check that this operation is correct before code goes live
					BG_AnimParseError( "BG_AnimParseAnimScript: internal error" );
				}
				//
				BG_ParseCommands( &text_p, currentScriptItem, scriptData );
			}
			else
			{
				// huh ??
				BG_AnimParseError( "BG_AnimParseAnimScript: unexpected '%s'", token );   // RF mod
			}

			break;

		case PARSEMODE_STATECHANGES:
		case PARSEMODE_EVENTS:
			if ( !Q_stricmp( token, "{" ) )
			{
				// about to increment indent level, check that we have enough information to do this
				if ( indentLevel >= MAX_INDENT_LEVELS )   // too many indentations
				{
					BG_AnimParseError( "BG_AnimParseAnimScript: unexpected '%s'", token );   // RF mod
				}
				if ( indexes[indentLevel] < 0 )       // we havent found out what this new group is yet
				{
					BG_AnimParseError( "BG_AnimParseAnimScript: unexpected '%s'", token );   // RF mod
				}
				//
				indentLevel++;
			}
			else if ( !Q_stricmp( token, "}" ) )
			{
				// reduce the indentLevel
				indentLevel--;
				if ( indentLevel < 0 )
				{
					BG_AnimParseError( "BG_AnimParseAnimScript: unexpected '%s'", token );   // RF mod
				}
				if ( indentLevel == 0 )
				{
					currentScript = NULL;
				}
				// make sure we read a new index before next indent
				indexes[indentLevel] = -1;
			}
			else if ( indentLevel == 0 && ( indexes[indentLevel] < 0 ) )
			{
				if ( parseMode == PARSEMODE_STATECHANGES )
				{
					if ( Q_stricmp( token, "statechange" ) )
					{
						BG_AnimParseError( "BG_AnimParseAnimScript: expected 'statechange', got '%s'", token );  // RF mod
					}

					// read in the old state type
					token = Com_ParseOnLine( &text_p );
					if ( !token )
					{
						BG_AnimParseError( "BG_AnimParseAnimScript: expected <state type>" );    // RF mod
					}
					oldState = BG_IndexForString( token, animStateStr, qfalse );

					// read in the new state type
					token = Com_ParseOnLine( &text_p );
					if ( !token )
					{
						BG_AnimParseError( "BG_AnimParseAnimScript: expected <state type>" );    // RF mod
					}
					indexes[indentLevel] = BG_IndexForString( token, animStateStr, qfalse );

					currentScript = &scriptData->scriptStateChange[oldState][indexes[indentLevel]];

//----(SA)		// RF mod
					// check for the open bracket
					token = Com_Parse( &text_p );
					if ( !token || Q_stricmp( token, "{" ) )
					{
						BG_AnimParseError( "BG_AnimParseAnimScript: expected '{'" );
					}
					indentLevel++;
//----(SA)		// RF mod
				}
				else
				{
					// read in the event type
					indexes[indentLevel] = BG_IndexForString( token, animEventTypesStr, qfalse );
					currentScript = &scriptData->scriptEvents[indexes[0]];

					parseEvent = indexes[indentLevel];
				}

				memset( currentScript, 0, sizeof( *currentScript ) );
			}
			else if ( ( indentLevel == 1 ) && ( indexes[indentLevel] < 0 ) )
			{
				// we are expecting a condition specifier
				// move the text_p backwards so we can read in the last token again
				text_p -= strlen( token );
				// sanity check that
				if ( Q_strncmp( text_p, token, strlen( token ) ) )
				{
					// this should never happen, just here to check that this operation is correct before code goes live
					BG_AnimParseError( "BG_AnimParseAnimScript: internal error" );
				}
				//
				memset( &tempScriptItem, 0, sizeof( tempScriptItem ) );
				indexes[indentLevel] = BG_ParseConditions( &text_p, &tempScriptItem );
				// do we have enough room in this script for another item?
				if ( currentScript->numItems >= MAX_ANIMSCRIPT_ITEMS )
				{
					BG_AnimParseError( "BG_AnimParseAnimScript: exceeded maximum items per script (%i)", MAX_ANIMSCRIPT_ITEMS ); // RF mod
				}
				// are there enough items left in the global list?
				if ( scriptData->numScriptItems >= MAX_ANIMSCRIPT_ITEMS_PER_MODEL )
				{
					BG_AnimParseError( "BG_AnimParseAnimScript: exceeded maximum global items (%i)", MAX_ANIMSCRIPT_ITEMS_PER_MODEL );   // RF mod
				}
				// it was parsed ok, so grab an item from the global list to use
				currentScript->items[currentScript->numItems] = &scriptData->scriptItems[ scriptData->numScriptItems++ ];
				currentScriptItem = currentScript->items[currentScript->numItems];
				currentScript->numItems++;
				// copy the data across from the temp script item
				*currentScriptItem = tempScriptItem;
			}
			else if ( indentLevel == 2 )
			{
				// we are reading the commands, so parse this line as if it were a command
				// move the text_p backwards so we can read in the last token again
				text_p -= strlen( token );
				// sanity check that
				if ( Q_strncmp( text_p, token, strlen( token ) ) )
				{
					// this should never happen, just here to check that this operation is correct before code goes live
					BG_AnimParseError( "BG_AnimParseAnimScript: internal error" );
				}
				//
				BG_ParseCommands( &text_p, currentScriptItem, scriptData );
			}
			else
			{
				// huh ??
				BG_AnimParseError( "BG_AnimParseAnimScript: unexpected '%s'", token );   // RF mod
			}
		}
	}

	if ( indentLevel )
	{
		BG_AnimParseError( "BG_AnimParseAnimScript: unexpected end of file: %s", token );
	}

	globalFilename = NULL;
	Com_EndParseSession();
}

/*
=================
BG_FinalizePlayerAnims
=================
*/
void BG_FinalizePlayerAnims()
{
	float duration;
	loadAnim_t *pLoadAnim;
	vec3_t vDelta;
	vec2_t vRot;
	XAnim_s *pXAnims;
	animation_t *pCurrAnim;
	animation_t *pAnims;
	int iNumAnims;
	int i;

	assert(g_pLoadAnims);
	assert(g_piNumLoadAnims);

	pAnims = globalScriptData->animations;
	pXAnims = globalScriptData->animTree.anims;

	iNumAnims = XAnimGetAnimTreeSize(pXAnims);
	assert(iNumAnims < MAX_MODEL_ANIMATIONS);
	globalScriptData->numAnimations = iNumAnims;

	pCurrAnim = pAnims;
	pAnims->flags |= 0x101;

	I_strncpyz(pCurrAnim->name, "root", sizeof(pCurrAnim->name));
	pCurrAnim->nameHash = 0;
	pCurrAnim++;

	for ( i = 1; i < iNumAnims; i++, pCurrAnim++ )
	{
		pLoadAnim = BG_LoadAnimForAnimIndex(i);

		if ( !pLoadAnim )
		{
			pCurrAnim->flags |= 0x100;
			I_strncpyz(pCurrAnim->name, "unused", sizeof(pCurrAnim->name));
			pCurrAnim->nameHash = 0;
			continue;
		}

		if ( !XAnimIsPrimitive(pXAnims, i) )
		{
			pCurrAnim->flags |= 1;
			I_strncpyz(pCurrAnim->name, pLoadAnim->name, sizeof(pLoadAnim->name));
			pCurrAnim->nameHash = pLoadAnim->nameHash;

			if ( !pCurrAnim->initialLerp )
			{
				pCurrAnim->initialLerp = -1;
			}

			pCurrAnim->duration = 0;
			pCurrAnim->moveSpeed = 0.0;
			continue;
		}

		I_strncpyz(pCurrAnim->name, XAnimGetAnimName(pXAnims, i), sizeof(pLoadAnim->name));
		pCurrAnim->nameHash = BG_StringHashValue(pCurrAnim->name);

		if ( !pCurrAnim->initialLerp )
		{
			pCurrAnim->initialLerp = -1;
		}

		duration = XAnimGetLength(pXAnims, i);

		if ( duration == 0.0 )
		{
			pCurrAnim->duration = 500;
			pCurrAnim->moveSpeed = 0.0;
		}
		else
		{
			pCurrAnim->duration = (int)(duration * 1000.0);
			XAnimGetRelDelta(pXAnims, i, vRot, vDelta, 0.0, 1.0);
			pCurrAnim->moveSpeed = VectorLength(vDelta) / duration;

			if ( pCurrAnim->duration < 500 )
			{
				pCurrAnim->duration = 500;
			}
		}

		if ( XAnimIsLooped(pXAnims, i) )
		{
			pCurrAnim->flags |= 0x80;
		}
	}

	BG_AnimParseAnimScript(globalScriptData, NULL, NULL);
	BG_SetupAnimNoteTypes(globalScriptData);
}