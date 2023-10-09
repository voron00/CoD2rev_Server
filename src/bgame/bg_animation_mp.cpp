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

extern animStringItem_t weaponStrings[];

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

loadAnim_t *g_pLoadAnims;
unsigned int *g_piNumLoadAnims;

static int parseMovetype;
static int parseEvent;

// these are used globally during script parsing
static int numDefines[NUM_ANIM_CONDITIONS];
static char defineStrings[10000];       // stores the actual strings
static int defineStringsOffset;
static animStringItem_t defineStr[NUM_ANIM_CONDITIONS][MAX_ANIM_DEFINES];
static int defineBits[NUM_ANIM_CONDITIONS][MAX_ANIM_DEFINES][2];

/*
================
return a hash value for the given string
================
*/
long BG_StringHashValue( const char *fname )
{
	int i;
	long hash;

	if ( !fname )
	{
		return -1;
	}

	hash = 0;
	i = 0;
	while ( fname[i] != '\0' )
	{
		if ( Q_isupper( fname[i] ) )
		{
			hash += (long)( fname[i] + ( 'a' - 'A' ) ) * ( i + 119 );
		}
		else
		{
			hash += (long)( fname[i] ) * ( i + 119 );
		}

		i++;
	}
	if ( hash == -1 )
	{
		hash = 0;   // never return -1
	}
	return hash;
}

/*
================
return a hash value for the given string (make sure the strings and lowered first)
================
*/
long BG_StringHashValue_Lwr( const char *fname )
{
	int i;
	long hash;

	hash = 0;
	i = 0;
	while ( fname[i] != '\0' )
	{
		hash += (long)( fname[i] ) * ( i + 119 );
		i++;
	}
	if ( hash == -1 )
	{
		hash = 0;   // never return -1
	}
	return hash;
}

/*
=================
BG_AnimParseError
=================
*/
void QDECL BG_AnimParseError( const char *msg, ... )
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
BG_IndexForString
  errors out if no match found
=================
*/
int BG_IndexForString( char *token, animStringItem_t *strings, qboolean allowFail )
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
	memset( result, 0, sizeof( int ) * 2 );
	memset( tempBits, 0, sizeof( int ) * 2 );

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
			COM_BitSet( result, 0 );
			continue;
		}

		if ( !Q_stricmp( token, "none," ) )      // first bit is always the "unused" bit
		{
			COM_BitSet( result, 0 );
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
				Q_strcat( currentString, sizeof( currentString ), " " );
			}
			Q_strcat( currentString, sizeof( currentString ), token );
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
					COM_BitSet( tempBits, indexFound );
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
BG_ParseConditions
  returns qtrue if everything went ok, error drops otherwise
=================
*/
qboolean BG_ParseConditions( const char **text_pp, animScriptItem_t *scriptItem )
{
	int conditionIndex, conditionValue[2];
	char    *token;

	conditionValue[0] = 0;
	conditionValue[1] = 0;

	while ( 1 )
	{
		token = Com_ParseExt( text_pp, qfalse );
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
				token = Com_ParseExt( text_pp, qfalse );
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

#if COMPILE_PLAYER == 1
int custom_animation[MAX_CLIENTS] = {0};
#endif

/*
===============
BG_PlayAnim
===============
*/
int BG_PlayAnim(playerState_s *ps, int animNum, int bodyPart, int forceDuration, qboolean setTimer, qboolean isContinue, qboolean force)
{
	int wasSet;
	int duration;

#if COMPILE_PLAYER == 1
	if (custom_animation[ps->clientNum])
		animNum = custom_animation[ps->clientNum];
#endif

	wasSet = 0;

	if ( forceDuration )
		duration = forceDuration;
	else
		duration = globalScriptData->animations[animNum].duration + 50;

	if ( bodyPart == ANIM_BP_TORSO )
	{
retry:
		if ( ps->torsoTimer <= 49 || force )
		{
			if ( isContinue && (ps->torsoAnim & 0xFFFFFDFF) == animNum )
			{
				if ( setTimer && (globalScriptData->animations[animNum].flags & 0x80) != 0 )
					ps->torsoTimer = duration;
			}
			else
			{
				ps->torsoAnim = animNum | ps->torsoAnim & 0x200 ^ 0x200;

				if ( setTimer )
					ps->torsoTimer = duration;

				ps->torsoAnimDuration = duration;
			}
		}
		goto out;
	}

	if ( bodyPart > ANIM_BP_TORSO )
	{
		if ( bodyPart != ANIM_BP_BOTH )
			goto out;
	}
	else if ( bodyPart != ANIM_BP_LEGS )
	{
		goto out;
	}

	if ( ps->legsTimer <= 49 || force )
	{
		if ( isContinue && (ps->legsAnim & 0xFFFFFDFF) == animNum )
		{
			if ( setTimer && (globalScriptData->animations[animNum].flags & 0x80) != 0 )
				ps->legsTimer = duration;
		}
		else
		{
			wasSet = 1;
			ps->legsAnimDuration = duration;
			ps->legsAnim = animNum | ps->legsAnim & 0x200 ^ 0x200;

			if ( setTimer )
				ps->legsTimer = duration;
		}
	}

	if ( bodyPart == ANIM_BP_BOTH )
	{
		animNum = 0;
		goto retry;
	}
out:
	if ( wasSet )
		return duration;
	else
		return -1;
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
		if ( scriptCommand->bodyPart[0] == ANIM_BP_BOTH || scriptCommand->bodyPart[0] == ANIM_BP_LEGS )
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
		if ( scriptCommand->bodyPart[1] == ANIM_BP_BOTH || scriptCommand->bodyPart[1] == ANIM_BP_LEGS )
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

/*
================
BG_AnimScriptEvent
  returns the duration in milliseconds that this model should be paused. -1 if no event found
================
*/
int QDECL BG_AnimScriptEvent( playerState_s *ps, scriptAnimEventTypes_t event, qboolean isContinue, qboolean force )
{
	animScriptCommand_t *script;
	animScriptItem_t *item;

	if ( event != ANIM_ET_DEATH && ps->pm_type > PM_INTERMISSION )
		return -1;

	if ( !globalScriptData->scriptEvents[event].numItems )
		return -1;

	item = BG_FirstValidItem(ps->clientNum, &globalScriptData->scriptEvents[event]);

	if ( !item )
		return -1;

	if ( !item->numCommands )
		return -1;

	script = &item->commands[rand() % item->numCommands];
	return BG_ExecuteCommand(ps, script, 1, isContinue, force);
}

/*
==============
BG_UpdateConditionValue
==============
*/
void BG_UpdateConditionValue(int client, int condition, int value, qboolean checkConversion)
{
	if ( !checkConversion || animConditionsTable[condition].type )
	{
		level_bgs.clientinfo[client].clientConditions[condition][0] = value;
	}
	else
	{
		level_bgs.clientinfo[client].clientConditions[condition][0] = 0;
		level_bgs.clientinfo[client].clientConditions[condition][1] = 0;
		COM_BitSet(level_bgs.clientinfo[client].clientConditions[condition], value);
	}
}

/*
==============
BG_GetConditionValue
==============
*/
unsigned int QDECL BG_GetConditionValue(clientInfo_t *ci, int condition, qboolean checkConversion)
{
	unsigned int i;

	if ( !checkConversion || animConditionsTable[condition].type )
		return ci->clientConditions[condition][0];

	for ( i = 0; i < MAX_CLIENTS; ++i )
	{
		if ( COM_BitTest(ci->clientConditions[condition], i) )
			return i;
	}

	return 0;
}

/*
==============
BG_UpdateConditionValue
==============
*/
void BG_AnimPlayerConditions(entityState_s *es, clientInfo_t *ci)
{
	WeaponDef *weaponDef;
	int legsAnim;

	weaponDef = BG_GetWeaponDef(es->weapon);

	BG_UpdateConditionValue(es->clientNum, ANIM_COND_PLAYERANIMTYPE, weaponDef->playerAnimType, 1);
	BG_UpdateConditionValue(es->clientNum, ANIM_COND_WEAPONCLASS, weaponDef->weaponClass, 1);

	if ( (es->eFlags & 0x40000) != 0 )
		BG_UpdateConditionValue(es->clientNum, ANIM_COND_WEAPON_POSITION, 1, 1);
	else
		BG_UpdateConditionValue(es->clientNum, ANIM_COND_WEAPON_POSITION, 0, 1);

	if ( (es->eFlags & 0x300) != 0 )
		BG_UpdateConditionValue(es->clientNum, ANIM_COND_MOUNTED, 1, 1);
	else
		BG_UpdateConditionValue(es->clientNum, ANIM_COND_MOUNTED, 0, 1);

	BG_UpdateConditionValue(es->clientNum, ANIM_COND_UNDERHAND, ci->playerAngles[0] > 0.0, 1);

	if ( (es->eFlags & 4) != 0 )
		BG_UpdateConditionValue(es->clientNum, ANIM_COND_CROUCHING, 1, 1);
	else
		BG_UpdateConditionValue(es->clientNum, ANIM_COND_CROUCHING, 0, 1);

	if ( (es->eFlags & 0x40) != 0 )
		BG_UpdateConditionValue(es->clientNum, ANIM_COND_FIRING, 1, 1);
	else
		BG_UpdateConditionValue(es->clientNum, ANIM_COND_FIRING, 0, 1);

	legsAnim = es->legsAnim & 0xFFFFFDFF;

	if ( level_bgs.animData.animScriptData.animations[legsAnim].movetype && BG_GetConditionValue(ci, 3, 0) != level_bgs.animData.animScriptData.animations[legsAnim].movetype )
	{
		BG_UpdateConditionValue(es->clientNum, ANIM_COND_MOVETYPE, level_bgs.animData.animScriptData.animations[legsAnim].movetype, 0);
	}

	if ( (level_bgs.animData.animScriptData.animations[legsAnim].flags & 0x10) != 0 )
	{
		BG_UpdateConditionValue(es->clientNum, ANIM_COND_STRAFING, 1, 1);
	}
	else if ( (level_bgs.animData.animScriptData.animations[legsAnim].flags & 0x20) != 0 )
	{
		BG_UpdateConditionValue(es->clientNum, ANIM_COND_STRAFING, 2, 1);
	}
	else
	{
		BG_UpdateConditionValue(es->clientNum, ANIM_COND_STRAFING, 0, 1);
	}
}

/*
================
BG_AnimScriptAnimation
  runs the normal locomotive animations
  returns 1 if an animation was set, -1 if no animation was found, 0 otherwise
================
*/
int BG_AnimScriptAnimation(playerState_s *ps, int state, int movetype, qboolean force)
{
	animScriptItem_t *item;

	item = 0;

	if ( ps->pm_type > PM_INTERMISSION )
		return -1;

	while ( !item && state >= AISTATE_RELAXED )
	{
		if ( globalScriptData->scriptAnims[state][movetype].numItems )
		{
			item = BG_FirstValidItem(ps->clientNum, &globalScriptData->scriptAnims[state][movetype]);

			if ( !item )
				--state;
		}
		else
		{
			--state;
		}
	}

	if ( !item )
		return -1;

	if ( !item->numCommands )
		return -1;

	BG_UpdateConditionValue(ps->clientNum, ANIM_COND_MOVETYPE, movetype, 1);
	return BG_ExecuteCommand(ps, &item->commands[ps->clientNum % item->numCommands], 0, force, 0) != -1;
}

/*
=================
BG_AnimationIndexForString
=================
*/
unsigned int BG_AnimationIndexForString(const char *string)
{
	animation_s *animation;
	int count;
	int value;
	int hash;
	int i;
	loadAnim_t *loadAnim;

	if ( g_pLoadAnims )
	{
		hash = BG_StringHashValue(string);
		count = 0;
		loadAnim = g_pLoadAnims;

		while ( count < *g_piNumLoadAnims )
		{
			if ( hash == loadAnim->nameHash && !I_stricmp(string, loadAnim->name) )
				return count;

			++count;
			++loadAnim;
		}

		loadAnim = &g_pLoadAnims[*g_piNumLoadAnims];
		Scr_FindAnim("multiplayer", string, &loadAnim->anim, level_bgs.anim_user);
		strcpy(loadAnim->name, string);
		loadAnim->nameHash = hash;

		return (*g_piNumLoadAnims)++;
	}
	else
	{
		value = BG_StringHashValue(string);
		i = 0;

		for ( animation = globalScriptData->animations; ; ++animation )
		{
			if ( i >= globalScriptData->numAnimations )
				BG_AnimParseError("BG_AnimationIndexForString: unknown player animation '%s'", string);

			if ( value == animation->nameHash && !I_stricmp(string, animation->name) )
				break;

			++i;
		}

		return i;
	}
}

/*
=================
BG_ParseCommands
=================
*/
void BG_ParseCommands(const char **input, animScriptItem_t *scriptItem, animScriptData_t *scriptData)
{
	int *pFlags;
	int64_t temp;
	int *lerp;
	int nextIndex;
	char *token;
	int blend;
	int i;
	int partIndex;
	animScriptCommand_t *command;

	command = 0;
	partIndex = 0;

	while ( 1 )
	{
		token = partIndex > 0 ? Com_ParseOnLine(input) : Com_Parse(input);

		if ( !token || !*token )
			break;

		if ( !I_stricmp(token, "}") )
		{
			*input -= strlen(token);
			return;
		}

		if ( !partIndex )
		{
			if ( scriptItem->numCommands > 7 )
				BG_AnimParseError("BG_ParseCommands: exceeded maximum number of animations (%i)", 8);

			command = &scriptItem->commands[scriptItem->numCommands++];
			memset(command, 0, 4u);
		}

		command->bodyPart[partIndex] = BG_IndexForString(token, animBodyPartsStr, 1);

		if ( command->bodyPart[partIndex] <= 0 )
		{
			*input -= strlen(token);
			goto parse_optional;
		}

		token = Com_ParseOnLine(input);

		if ( !token || !*token )
			BG_AnimParseError("BG_ParseCommands: expected animation");

		command->animIndex[partIndex] = BG_AnimationIndexForString(token);
		command->animDuration[partIndex] = scriptData->animations[command->animIndex[partIndex]].duration;

		if ( !g_pLoadAnims )
		{
			if ( parseMovetype && command->bodyPart[partIndex] != 2 )
			{
				pFlags = &scriptData->animations[command->animIndex[partIndex]].flags;
				temp = 1LL << parseMovetype;
				scriptData->animations[command->animIndex[partIndex]].movetype |= 1LL << parseMovetype;
				pFlags[2] |= HIDWORD(temp);

				if ( (parseMovetype == 18 || parseMovetype == 19)
				        && scriptData->animations[command->animIndex[partIndex]].moveSpeed != 0.0 )
				{
					scriptData->animations[command->animIndex[partIndex]].flags |= 2u;
				}

				for ( i = 0; ; ++i )
				{
					if ( i >= scriptItem->numConditions )
						goto parse_event;
					if ( scriptItem->conditions[i].index == 8 )
						break;
				}

				if ( scriptItem->conditions[i].value[0] == 1 )
				{
					scriptData->animations[command->animIndex[partIndex]].flags |= 0x10u;
				}

				else if ( scriptItem->conditions[i].value[0] == 2 )
				{
					scriptData->animations[command->animIndex[partIndex]].flags |= 0x20u;
				}
			}
parse_event:
			switch ( parseEvent )
			{
			case 2:
				scriptData->animations[command->animIndex[partIndex]].flags |= 8u;
				scriptData->animations[command->animIndex[partIndex]].initialLerp = 30;
				break;
			case 1:
				scriptData->animations[command->animIndex[partIndex]].moveSpeed = 0.0;
				scriptData->animations[command->animIndex[partIndex]].flags |= 0x40u;
				break;
			case 10:
				scriptData->animations[command->animIndex[partIndex]].moveSpeed = 0.0;
				break;
			default:
				if ( parseMovetype > 20 && parseMovetype <= 30 )
					scriptData->animations[command->animIndex[partIndex]].moveSpeed = 0.0;
				break;
			}
			goto skip;
		}
		do
		{
skip:
			blend = 0;
			token = Com_ParseOnLine(input);
			if ( !token || !*token )
			{
unget_token:
				Com_UngetToken();
				continue;
			}
			if ( I_stricmp(token, "duration") )
			{
				if ( I_stricmp(token, "turretanim") )
				{
					if ( I_stricmp(token, "blendtime") )
						goto unget_token;
					blend = 1;
					token = Com_ParseOnLine(input);
					if ( !token || !*token )
						BG_AnimParseError("BG_ParseCommands: expected blendtime value");
					if ( !g_pLoadAnims )
					{
						lerp = &scriptData->animations[command->animIndex[partIndex]].initialLerp;
						*lerp = atoi(token);
					}
				}
				else
				{
					blend = 1;
					if ( !g_pLoadAnims )
						scriptData->animations[command->animIndex[partIndex]].flags |= 4u;
					if ( command->bodyPart[partIndex] != 3 )
						BG_AnimParseError("BG_ParseCommands: Turret animations can only be played on the 'both' body part");
				}
			}
			else
			{
				blend = 1;
				token = Com_ParseOnLine(input);
				if ( !token || !*token )
					BG_AnimParseError("BG_ParseCommands: expected duration value");
				command->animDuration[partIndex] = atoi(token);
			}
		}
		while ( blend );
		if ( command->bodyPart[partIndex] != 3 )
		{
			nextIndex = partIndex++;
			if ( nextIndex <= 0 )
				continue;
		}
parse_optional:
		while ( 1 )
		{
			token = Com_ParseOnLine(input);
			if ( !token || !*token )
				break;
			if ( I_stricmp(token, "sound") )
				BG_AnimParseError("BG_ParseCommands: unknown parameter '%s'", token);
			token = Com_ParseOnLine(input);
			if ( !token || !*token )
				BG_AnimParseError("BG_ParseCommands: expected sound");
			if ( strstr(token, ".wav") )
				BG_AnimParseError("BG_ParseCommands: wav files not supported, only sound scripts");
			command->soundAlias = globalScriptData->soundAlias(token);
		}
		partIndex = 0;
	}
}

/*
=================
BG_AnimParseAnimScript
  Parse the animation script for this model, converting it into run-time structures
=================
*/
void BG_AnimParseAnimScript(animScriptData_t *scriptData, loadAnim_t *pLoadAnims, unsigned int *piNumAnims)
{
#define MAX_INDENT_LEVELS   3

	// FIXME: change this to use the botlib parser
	const char *text_p;
	char *token;
	int parseMode;
	animScript_t            *currentScript;
	animScriptItem_t tempScriptItem;
	// TTimo gcc: might be used unitialized
	animScriptItem_t        *currentScriptItem = NULL;
	int indexes[MAX_INDENT_LEVELS], indentLevel, oldState, newParseMode;
	int i, defineType;

	static int bScriptFileLoaded = 0;
	static char ScriptFileBuffer[100000];
	fileHandle_t f;
	int length;

	if ( !bScriptFileLoaded )
	{
		length = FS_FOpenFileByMode(globalFilename, &f, FS_READ);

		if ( length < 0 )
			Com_Error(ERR_DROP, "Couldn't load player animation script %s\n", globalFilename);

		if ( length > 99998 )
			Com_Error(ERR_DROP, "Couldn't load player animation script %s\n", globalFilename);

		FS_Read(ScriptFileBuffer, length, f);
		ScriptFileBuffer[length] = 0;
		FS_FCloseFile(f);
		bScriptFileLoaded = 1;
	}

	// the scriptData passed into here must be the one this binary is using
	globalScriptData = scriptData;

	g_pLoadAnims = pLoadAnims;
	g_piNumLoadAnims = piNumAnims;

	// start at the defines
	parseMode = PARSEMODE_DEFINES;

	//BG_LoadPlayerAnimTypes();
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
		if ( !token || !*token )
		{
			if ( indentLevel )
			{
				BG_AnimParseError( "BG_AnimParseAnimScript: unexpected end of file: %s" );
			}
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
				token = Com_ParseExt( &text_p, qfalse );
				if ( !token || !*token )
				{
					BG_AnimParseError( "BG_AnimParseAnimScript: expected condition type string" );   // RF mod
				}
				defineType = BG_IndexForString( token, animConditionsStr, qfalse );

				// read in the define
				token = Com_ParseExt( &text_p, qfalse );
				if ( !token || !*token )
				{
					BG_AnimParseError( "BG_AnimParseAnimScript: expected condition define string" ); // RF mod
				}

				// copy the define to the strings list
				defineStr[defineType][numDefines[defineType]].string = BG_CopyStringIntoBuffer( token, defineStrings, sizeof( defineStrings ), &defineStringsOffset );
				defineStr[defineType][numDefines[defineType]].hash = BG_StringHashValue( defineStr[defineType][numDefines[defineType]].string );
				// expecting an =
				token = Com_ParseExt( &text_p, qfalse );
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

				// copy the weapon defines over to the enemy_weapon defines
				/*
				memcpy( &defineStr[ANIM_COND_ENEMY_WEAPON][0], &defineStr[ANIM_COND_WEAPON][0], sizeof( animStringItem_t ) * MAX_ANIM_DEFINES );
				memcpy( &defineBits[ANIM_COND_ENEMY_WEAPON][0], &defineBits[ANIM_COND_WEAPON][0], sizeof( defineBits[ANIM_COND_ENEMY_WEAPON][0] ) * MAX_ANIM_DEFINES );
				numDefines[ANIM_COND_ENEMY_WEAPON] = numDefines[ANIM_COND_WEAPON];
				*/
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
				token = Com_ParseExt( &text_p, qfalse );
				if ( !token )
				{
					BG_AnimParseError( "BG_AnimParseAnimScript: expected state type" );  // RF mod
				}
				indexes[indentLevel] = BG_IndexForString( token, animStateStr, qfalse );

//----(SA) // RF mod
				// check for the open bracket
				token = Com_ParseExt( &text_p, qtrue );
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
						BG_AnimParseError( "BG_AnimParseAnimScript: expected 'statechange', got '%s'", token );	// RF mod
					}
					// read in the old state type
					token = Com_ParseExt( &text_p, qfalse );
					if ( !token )
					{
						BG_AnimParseError( "BG_AnimParseAnimScript: expected <state type>" );	// RF mod
					}
					oldState = BG_IndexForString( token, animStateStr, qfalse );
					// read in the new state type
					token = Com_ParseExt( &text_p, qfalse );
					if ( !token )
					{
						BG_AnimParseError( "BG_AnimParseAnimScript: expected <state type>" );	// RF mod
					}
					indexes[indentLevel] = BG_IndexForString( token, animStateStr, qfalse );
					currentScript = &scriptData->scriptStateChange[oldState][indexes[indentLevel]];
					//----(SA)		// RF mod
					// check for the open bracket
					token = Com_ParseExt( &text_p, qtrue );
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

		default:
			break;
		}
	}

	globalFilename = NULL;
	Com_EndParseSession();
}

void BG_UpdatePlayerDObj(DObj_s *pDObj, entityState_s *es, clientInfo_t *ci, int attachIgnoreCollision)
{
	int numModels;
	DObjModel_s models[8];
	XAnimTree_s *tree;
	int i;
	int weapon;

	weapon = es->weapon;

	if ( (es->eFlags & 0x300) != 0 )
		weapon = 0;

	tree = ci->pXAnimTree;

	if ( !ci->infoValid || !ci->model[0] )
	{
		XAnimClearTree(tree);
		level_bgs.SafeDObjFree(es->number);
		return;
	}

	if ( pDObj )
	{
		if ( ci->iDObjWeapon == weapon && !ci->dobjDirty )
			return;

		level_bgs.SafeDObjFree(es->number);
	}

	models->model = level_bgs.GetXModel(ci->model);
	models->boneName = 0;
	models->ignoreCollision = 0;

	numModels = 1;

	for ( i = 0; i < 6; ++i )
	{
		if ( ci->attachModelNames[i][0] )
		{
			models[numModels].model = level_bgs.GetXModel(ci->attachModelNames[i]);
			models[numModels].boneName = ci->attachTagNames[i];
			models[numModels++].ignoreCollision = (attachIgnoreCollision >> i) & 1;
		}
	}

	ci->iDObjWeapon = weapon;
	level_bgs.CreateDObj(models, numModels, tree, es->number);
	ci->dobjDirty = 0;
}

animation_s *BG_GetAnimationForIndex(int client, unsigned int index)
{
	if ( index >= globalScriptData->numAnimations )
		Com_Error(ERR_DROP, "BG_GetAnimationForIndex: index out of bounds");

	return &globalScriptData->animations[index];
}

qboolean BG_IsCrouchingAnim(const clientInfo_t *ci, int animNum)
{
	animation_s *index;

	index = BG_GetAnimationForIndex(ci->clientNum, animNum & 0xFFFFFDFF);
	return (index->stance & 0x180 | index->movetype & 0xC4) != 0;
}

qboolean BG_IsProneAnim(const clientInfo_t *ci, int animNum)
{
	return (BG_GetAnimationForIndex(ci->clientNum, animNum & 0xFFFFFDFF)->movetype & 0x308) != 0;
}

void BG_SetNewAnimation(clientInfo_t *ci, lerpFrame_t *lf, int newAnimation, qboolean isComplete)
{
	qboolean croucnAnim;
	qboolean proneAnim;
	float blendTime;
	float newGoalTime;
	float goalTime2;
	float nextGoalTime;
	float goalTime;
	float time1;
	float time2;
	int hasTime;
	qboolean proneMatch;
	qboolean crouchMatch;
	unsigned int index;
	int length;
	XAnim_s *anim;
	XAnimTree_s *tree;
	float fStartTime;
	float oldTime;
	int firstAnim;
	int animNum;
	int transitionMin;
	animation_s *animation;
	animation_s *anim2;
	unsigned int animIndex;

	transitionMin = -1;
	firstAnim = 0;
	fStartTime = 0.0;
	animation = lf->animation;
	animNum = lf->animationNumber;
	if ( !animation )
		firstAnim = 1;
	lf->animationNumber = newAnimation;
	animIndex = newAnimation & 0xFFFFFDFF;
	if ( animIndex >= level_bgs.animData.animScriptData.numAnimations )
		Com_Error(ERR_DROP, "Player animation index out of range (%i): %i", level_bgs.animData.animScriptData.numAnimations, animIndex);
	tree = ci->pXAnimTree;
	anim = level_bgs.animData.animScriptData.animTree.anims;
	if ( animIndex )
	{
		anim2 = &level_bgs.animData.animScriptData.animations[animIndex];
		lf->animation = anim2;
		lf->animationTime = anim2->initialLerp;
		croucnAnim = BG_IsCrouchingAnim(ci, animIndex);
		crouchMatch = croucnAnim == BG_IsCrouchingAnim(ci, animNum);
		proneAnim = BG_IsProneAnim(ci, animIndex);
		proneMatch = proneAnim == BG_IsProneAnim(ci, animNum);
		if ( lf == &ci->legs && (!crouchMatch || !proneMatch) )
			ci->stanceTransitionTime = level_bgs.time + 400;
	}
	else
	{
		anim2 = 0;
		lf->animation = 0;
		lf->animationTime = 200;
	}
	if ( firstAnim && lf == &ci->legs )
	{
		lf->animationTime = 0;
	}
	else
	{
		if ( !anim2 || lf->animationTime <= 0 )
		{
			if ( !anim2 || anim2->moveSpeed == 0.0 )
			{
				if ( !animation || animation->moveSpeed == 0.0 )
					transitionMin = 170;
				else
					transitionMin = 250;
			}
			else
			{
				transitionMin = 120;
			}
		}
		if ( ci->stanceTransitionTime - level_bgs.time > transitionMin )
			transitionMin = ci->stanceTransitionTime - level_bgs.time;
		if ( lf->animationTime < transitionMin )
			lf->animationTime = transitionMin;
	}
	if ( anim2 && anim2->moveSpeed != 0.0 && XAnimIsLooped(anim, animIndex) )
	{
		index = animNum & 0xFFFFFDFF;
		if ( animation && animation->moveSpeed != 0.0 && XAnimIsLooped(anim, index) )
		{
			fStartTime = XAnimGetTime(tree, index);
		}
		else
		{
			if ( XAnimIsPrimitive(anim, index) )
				length = XAnimGetLengthMsec(anim, index) + 200;
			else
				length = 1000;
			oldTime = (float)(level_bgs.time % length) / (float)length + (float)ci->clientNum * 0.36000001;
			fStartTime = oldTime - (float)(int)oldTime;
		}
	}
	if ( animation )
	{
		blendTime = (float)lf->animationTime * 0.001;
		XAnimClearGoalWeight(tree, animNum & 0xFFFFFDFF, blendTime);
	}
	if ( animIndex )
	{
		if ( lf != &ci->legs )
		{
			ci->leftHandGun = 0;
			ci->dobjDirty = 1;
		}
		if ( (anim2->flags & 0x40) != 0 )
		{
			if ( XAnimIsLooped(anim, animIndex) )
				Com_Error(ERR_DROP, "death animation '%s' is looping", anim2->name);
			if ( isComplete )
			{
				newGoalTime = (float)lf->animationTime * 0.001;
				XAnimSetCompleteGoalWeight(tree, animIndex, 1.0, newGoalTime, 1.0, 0, 0, 0);
			}
			else
			{
				XAnimSetCompleteGoalWeightKnobAll(tree, animIndex, level_bgs.animData.generic_human.root.index, 1.0, 0.0, 1.0, 0, 0);
				XAnimSetTime(tree, animIndex, 1.0);
			}
		}
		else
		{
			hasTime = 0;
			if ( anim2->moveSpeed != 0.0 && XAnimGetWeight(tree, animIndex) == 0.0 )
				hasTime = 1;
			goalTime2 = (float)lf->animationTime * 0.001;
			XAnimSetCompleteGoalWeight(tree, animIndex, 1.0, goalTime2, 1.0, 0, anim2->noteType, lf != &ci->legs);
			if ( hasTime )
				XAnimSetTime(tree, animIndex, fStartTime);
		}
		if ( lf != &ci->legs )
		{
			nextGoalTime = (float)lf->animationTime * 0.001;
			XAnimSetCompleteGoalWeight(
			    tree,
			    level_bgs.animData.animScriptData.torsoAnim,
			    1.0,
			    nextGoalTime,
			    1.0,
			    0,
			    anim2->noteType,
			    0);
			goalTime = (float)lf->animationTime * 0.001;
			XAnimSetCompleteGoalWeight(
			    tree,
			    level_bgs.animData.animScriptData.legsAnim,
			    0.0099999998,
			    goalTime,
			    1.0,
			    0,
			    anim2->noteType,
			    0);
		}
	}
	else if ( lf != &ci->legs )
	{
		time1 = (float)lf->animationTime * 0.001;
		XAnimSetCompleteGoalWeight(tree, level_bgs.animData.animScriptData.torsoAnim, 0.0, time1, 1.0, 0, 0, 0);
		time2 = (float)lf->animationTime * 0.001;
		XAnimSetCompleteGoalWeight(tree, level_bgs.animData.animScriptData.legsAnim, 1.0, time2, 1.0, 0, 0, 0);
	}
}

void BG_SetAnimRate(XAnimTree_s *tree, unsigned int animIndex, float rate)
{
	XAnimSetAnimRate(tree, animIndex, rate);
}

void BG_RunLerpFrameRate(clientInfo_t *ci, lerpFrame_t *lf, int newAnimation, const entityState_s *es)
{
	float oldPos;
	int bNewAnim;
	float newSpeedScale;
	float dist;
	float newPos;
	XAnimTree_s *tree;
	animation_s *animation;

	bNewAnim = 0;
	if ( lf->animation && (lf->animation->flags & 2) != 0 )
		bNewAnim = 1;
	tree = ci->pXAnimTree;
	if ( newAnimation != lf->animationNumber || !lf->animation && (newAnimation & 0xFFFFFDFF) != 0 )
		BG_SetNewAnimation(ci, lf, newAnimation, ((unsigned int)es->eFlags >> 19) & 1);
	if ( (newAnimation & 0xFFFFFDFF) != 0 )
	{
		animation = lf->animation;
		if ( animation->moveSpeed == 0.0 || !lf->oldFrameSnapshotTime )
		{
			lf->animSpeedScale = 1.0;
			lf->oldFrameSnapshotTime = level_bgs.latestSnapshotTime;
			VectorCopy(es->pos.trBase, lf->oldFramePos);
		}
		else if ( level_bgs.latestSnapshotTime != lf->oldFrameSnapshotTime )
		{
			if ( bNewAnim )
			{
				oldPos = lf->oldFramePos[2] - es->pos.trBase[2];
				dist = fabs(oldPos);
			}
			else
			{
				dist = Vec3Distance(lf->oldFramePos, es->pos.trBase);
			}
			newPos = dist / ((float)(level_bgs.latestSnapshotTime - lf->oldFrameSnapshotTime) * 0.001);
			lf->animSpeedScale = newPos / animation->moveSpeed;
			lf->oldFrameSnapshotTime = level_bgs.latestSnapshotTime;
			VectorCopy(es->pos.trBase, lf->oldFramePos);
			if ( lf->animSpeedScale >= 0.1 )
			{
				if ( lf->animSpeedScale > 2.0 )
				{
					if ( (animation->flags & 2) != 0 )
					{
						if ( lf->animSpeedScale > 4.0 )
							lf->animSpeedScale = 4.0;
					}
					else if ( animation->moveSpeed <= 150.0 )
					{
						if ( animation->moveSpeed >= 20.0 )
						{
							newSpeedScale = 3.0 - (animation->moveSpeed - 20.0) / 130.0;
							if ( lf->animSpeedScale > (float)newSpeedScale )
								lf->animSpeedScale = newSpeedScale;
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
			BG_SetAnimRate(tree, lf->animationNumber & 0xFFFFFDFF, lf->animSpeedScale);
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
	scale = fabs( swing );
	if ( scale < swingTolerance * 0.5 )
	{
		scale = 0.5;
	}
	else if ( scale < swingTolerance )
	{
		scale = 1.0;
	}
	else
	{
		scale = 2.0;
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
		*angle = AngleMod( *angle + move );
	}
	else if ( swing < 0 )
	{
		move = level_bgs.frametime * scale * -speed;
		if ( move <= swing )
		{
			move = swing;
			*swinging = qfalse;
		}
		*angle = AngleMod( *angle + move );
	}

	// clamp to no more than tolerance
	swing = AngleSubtract( destination, *angle );
	if ( swing > clampTolerance )
	{
		*angle = AngleMod( destination - (clampTolerance - 1) );
	}
	else if ( swing < -clampTolerance )
	{
		*angle = AngleMod( destination + (clampTolerance - 1) );
	}
}

void BG_PlayerAnimation_VerifyAnim(XAnimTree_s *pAnimTree, lerpFrame_t *lf)
{
	if ( lf->animationNumber )
	{
		if ( XAnimGetWeight(pAnimTree, lf->animationNumber & 0xFFFFFDFF) == 0.0 )
		{
			lf->animationNumber = 0;
			lf->animation = 0;
			lf->animationTime = 150;
		}
	}
}

extern dvar_t *bg_swingSpeed;
void BG_PlayerAngles(const entityState_s *es, clientInfo_t *ci)
{
	vec3_t headAngles;
	vec3_t torsoAngles;
	vec3_t legsAngles;
	float lerpMoveDir;
	float fraction;
	float clampTolerance;
	float destination;

	fraction = GetLeanFraction(ci->lerpLean);
	lerpMoveDir = ci->lerpMoveDir;
	VectorCopy(ci->playerAngles, headAngles);
	headAngles[1] = AngleMod(headAngles[1]);
	VectorClear(legsAngles);
	VectorClear(torsoAngles);
	if ( (es->eFlags & 0x300) != 0 )
	{
		ci->torso.yawing = 1;
		ci->torso.pitching = 1;
		ci->legs.yawing = 1;
	}
	else if ( (BG_GetConditionValue(ci, 3, 0) & 0xC0000) != 0 )
	{
		ci->torso.yawing = 1;
		ci->torso.pitching = 1;
		ci->legs.yawing = 1;
	}
	else if ( (es->eFlags & 0x4000) != 0 )
	{
		ci->torso.yawing = 1;
		ci->torso.pitching = 1;
		ci->legs.yawing = 1;
	}
	else if ( (BG_GetConditionValue(ci, 3, 0) & 6) != 0 )
	{
		if ( BG_GetConditionValue(ci, 6, 1) )
		{
			ci->torso.yawing = 1;
			ci->torso.pitching = 1;
		}
	}
	else
	{
		ci->torso.yawing = 1;
		ci->torso.pitching = 1;
		ci->legs.yawing = 1;
	}
	legsAngles[1] = headAngles[1] + lerpMoveDir;
	if ( (es->eFlags & 0x20000) != 0 )
	{
		legsAngles[1] = headAngles[1];
		torsoAngles[1] = headAngles[1];
		clampTolerance = 90.0;
	}
	else if ( (BG_GetConditionValue(ci, 3, 0) & 0xC0000) != 0 )
	{
		torsoAngles[1] = legsAngles[1];
		clampTolerance = 0.0;
	}
	else if ( (es->eFlags & 0x4000) != 0 )
	{
		torsoAngles[1] = headAngles[1];
		legsAngles[1] = headAngles[1];
		torsoAngles[0] = 90.0;
		clampTolerance = 90.0;
	}
	else if ( (es->eFlags & 8) != 0 )
	{
		torsoAngles[1] = headAngles[1];
		clampTolerance = 90.0;
	}
	else if ( (es->eFlags & 0x40) != 0 )
	{
		torsoAngles[1] = headAngles[1];
		clampTolerance = 45.0;
	}
	else
	{
		if ( (es->eFlags & 0x40000) != 0 )
			torsoAngles[1] = headAngles[1];
		else
			torsoAngles[1] = lerpMoveDir * 0.30000001 + headAngles[1];
		clampTolerance = 90.0;
	}
	BG_SwingAngles(
	    torsoAngles[1],
	    0.0,
	    clampTolerance,
	    bg_swingSpeed->current.decimal,
	    &ci->torso.yawAngle,
	    &ci->torso.yawing);
	clampTolerance = 150.0;
	if ( (es->eFlags & 0x20000) != 0 )
		goto LABEL_26;
	if ( (es->eFlags & 8) != 0 )
	{
		ci->legs.yawing = 0;
		ci->legs.yawAngle = headAngles[1] + lerpMoveDir;
	}
	else if ( (level_bgs.animData.animScriptData.animations[es->legsAnim & 0xFFFFFDFF].flags & 0x30) != 0 )
	{
		ci->legs.yawing = 0;
		legsAngles[1] = headAngles[1];
		BG_SwingAngles(
		    headAngles[1],
		    0.0,
		    clampTolerance,
		    bg_swingSpeed->current.decimal,
		    &ci->legs.yawAngle,
		    &ci->legs.yawing);
	}
	else
	{
		if ( ci->legs.yawing )
		{
LABEL_26:
			BG_SwingAngles(
			    legsAngles[1],
			    0.0,
			    clampTolerance,
			    bg_swingSpeed->current.decimal,
			    &ci->legs.yawAngle,
			    &ci->legs.yawing);
			goto LABEL_33;
		}
		BG_SwingAngles(
		    legsAngles[1],
		    40.0,
		    clampTolerance,
		    bg_swingSpeed->current.decimal,
		    &ci->legs.yawAngle,
		    &ci->legs.yawing);
	}
LABEL_33:
	if ( (es->eFlags & 0x300) != 0 )
	{
		ci->torso.yawAngle = headAngles[1];
		ci->legs.yawAngle = headAngles[1];
	}
	else if ( (BG_GetConditionValue(ci, 3, 0) & 0xC0000) != 0 )
	{
		ci->torso.yawAngle = headAngles[1] + lerpMoveDir;
		ci->legs.yawAngle = headAngles[1] + lerpMoveDir;
	}
	if ( (es->eFlags & 0x20000) != 0 )
	{
		destination = 0.0;
	}
	else if ( (es->eFlags & 0x300) != 0 )
	{
		destination = 0.0;
	}
	else if ( (BG_GetConditionValue(ci, 3, 0) & 0xC0000) != 0 )
	{
		destination = 0.0;
	}
	else if ( es->eFlags == 0x4000 )
	{
		destination = 0.0;
	}
	else if ( headAngles[0] <= 180.0 )
	{
		destination = headAngles[0] * 0.60000002;
	}
	else
	{
		destination = (headAngles[0] + -360.0) * 0.60000002;
	}
	BG_SwingAngles(destination, 0.0, 45.0, 0.15000001, &ci->torso.pitchAngle, &ci->torso.pitching);
}

void BG_PlayerAnimation(const DObj_s *pDObj, entityState_s *es, clientInfo_t *ci)
{
	BG_PlayerAngles(es, ci);
	BG_AnimPlayerConditions(es, ci);
	BG_PlayerAnimation_VerifyAnim(ci->pXAnimTree, &ci->legs);
	BG_PlayerAnimation_VerifyAnim(ci->pXAnimTree, &ci->torso);

	if ( ci->leftHandGun && (ci->torso.animationNumber & 0xFFFFFDFF) == 0 )
	{
		ci->leftHandGun = 0;
		ci->dobjDirty = 1;
	}

	BG_RunLerpFrameRate(ci, &ci->legs, es->legsAnim, es);
	BG_RunLerpFrameRate(ci, &ci->torso, es->torsoAnim, es);
}

void BG_AnimUpdatePlayerStateConditions(pmove_t *pmove)
{
	int index;
	WeaponDef *weaponDef;
	playerState_s *ps;

	ps = pmove->ps;
	index = BG_GetViewmodelWeaponIndex(pmove->ps);
	weaponDef = BG_GetWeaponDef(index);

	BG_UpdateConditionValue(ps->clientNum, ANIM_COND_PLAYERANIMTYPE, weaponDef->playerAnimType, 1);
	BG_UpdateConditionValue(ps->clientNum, ANIM_COND_WEAPONCLASS, weaponDef->weaponClass, 1);

	if ( (ps->eFlags & 0x40000) != 0 )
		BG_UpdateConditionValue(ps->clientNum, ANIM_COND_WEAPON_POSITION, 1, 1);
	else
		BG_UpdateConditionValue(ps->clientNum, ANIM_COND_WEAPON_POSITION, 0, 1);

	if ( (ps->eFlags & 0x300) != 0 )
		BG_UpdateConditionValue(ps->clientNum, ANIM_COND_MOUNTED, 1, 1);
	else
		BG_UpdateConditionValue(ps->clientNum, ANIM_COND_MOUNTED, 0, 1);

	BG_UpdateConditionValue(ps->clientNum, ANIM_COND_UNDERHAND, ps->viewangles[0] > 0.0, 1);

	if ( (pmove->cmd.buttons & 1) != 0 )
		BG_UpdateConditionValue(ps->clientNum, ANIM_COND_FIRING, 1, 1);
	else
		BG_UpdateConditionValue(ps->clientNum, ANIM_COND_FIRING, 0, 1);
}

loadAnim_t* BG_GetAnim(unsigned int index)
{
	loadAnim_t *animation;
	signed int i;

	if ( index >= globalScriptData->numAnimations )
		Com_Error(ERR_DROP, "Player animation index %i out of 0 to %i range", index, globalScriptData->numAnimations);

	i = 0;
	animation = g_pLoadAnims;

	while ( i < *g_piNumLoadAnims )
	{
		if ( index == animation->anim.index )
			return animation;

		++i;
		++animation;
	}

	return 0;
}

void BG_LerpAngles(float *angles_goal, float maxAngleChange, float *angles)
{
	int i;
	float diff;

	for ( i = 0; i < 3; ++i )
	{
		diff = angles_goal[i] - angles[i];

		if ( diff <= maxAngleChange )
		{
			if ( -maxAngleChange <= diff )
				angles[i] = angles_goal[i];
			else
				angles[i] = angles[i] - maxAngleChange;
		}
		else
		{
			angles[i] = angles[i] + maxAngleChange;
		}
	}
}

void BG_LerpOffset(float *offset_goal, float maxOffsetChange, float *offset)
{
	float scale;
	float error;
	vec3_t diff;

	VectorSubtract(offset_goal, offset, diff);

	scale = VectorLengthSquared(diff);

	if ( scale != 0.0 )
	{
		error = I_rsqrt(scale) * maxOffsetChange;

		if ( error >= 1.0 )
			VectorCopy(offset_goal, offset);
		else
			VectorMA(offset, error, diff, offset);
	}
}

void BG_Player_DoControllersInternal(DObj_s *obj, const entityState_s *es, int *partBits, clientInfo_t *ci, clientControllers_t *info)
{
	float ang;
	float ang2;
	float vTorsoScale;
	int i;
	float fCos;
	float fSin;
	vec3_t vHeadAngles;
	vec3_t vTorsoAngles;
	float fLeanFrac;
	vec3_t tag_origin_offset;
	vec3_t tag_origin_angles;
	float angles[6][3];

	if ( (es->eFlags & 0x300) != 0 )
	{
		memset(info, 0, sizeof(clientControllers_t));
	}
	else
	{
		VectorClear(tag_origin_angles);
		VectorClear(vTorsoAngles);
		VectorCopy(ci->playerAngles, vHeadAngles);

		tag_origin_angles[1] = ci->legs.yawAngle;
		vTorsoAngles[1] = ci->torso.yawAngle;

		if ( (BG_GetConditionValue(ci, 3, 0) & 0xC0000) == 0 )
		{
			vTorsoAngles[0] = ci->torso.pitchAngle;

			if ( (es->eFlags & 8) != 0 )
			{
				vTorsoAngles[0] = AngleNormalize180(vTorsoAngles[0]);

				if ( vTorsoAngles[0] <= 0.0 )
					vTorsoAngles[0] = vTorsoAngles[0] * 0.25;
				else
					vTorsoAngles[0] = vTorsoAngles[0] * 0.5;
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
			if ( (es->eFlags & 4) != 0 )
			{
				if ( fLeanFrac <= 0.0 )
					tag_origin_offset[1] = -fLeanFrac * 12.5 + tag_origin_offset[1];
				else
					tag_origin_offset[1] = -fLeanFrac * 2.5 + tag_origin_offset[1];
			}
			else if ( fLeanFrac <= 0.0 )
			{
				tag_origin_offset[1] = -fLeanFrac * 5.0 + tag_origin_offset[1];
			}
			else
			{
				tag_origin_offset[1] = -fLeanFrac * 2.5 + tag_origin_offset[1];
			}
		}

		if ( (es->eFlags & 0x20000) == 0 )
			tag_origin_angles[1] = AngleSubtract(tag_origin_angles[1], ci->playerAngles[1]);

		if ( (es->eFlags & 8) != 0 )
		{
			if ( fLeanFrac != 0.0 )
				vHeadAngles[2] = vHeadAngles[2] * 0.5;

			tag_origin_angles[0] = tag_origin_angles[0] + es->fTorsoPitch;
			vTorsoScale = vTorsoAngles[1] * 0.0174532925199433;
			I_sinCos(vTorsoScale, &fSin, &fCos);
			tag_origin_offset[0] = (1.0 - fCos) * -24.0 + tag_origin_offset[0];
			tag_origin_offset[1] = fSin * -12.0 + tag_origin_offset[1];

			if ( fLeanFrac * fSin > 0.0 )
				tag_origin_offset[1] = -fLeanFrac * (1.0 - fCos) * 16.0 + tag_origin_offset[1];

			angles[0][0] = 0.0;
			angles[0][1] = vTorsoAngles[2] * -1.2;
			angles[0][2] = vTorsoAngles[2] * 0.30000001;

			if ( es->fTorsoPitch != 0.0 || es->fWaistPitch != 0.0 )
			{
				ang = AngleSubtract(es->fTorsoPitch, es->fWaistPitch);
				angles[0][0] = ang + angles[0][0];
			}

			angles[1][0] = 0.0;
			angles[1][1] = vTorsoAngles[1] * 0.1 - vTorsoAngles[2] * 0.2;
			angles[1][2] = vTorsoAngles[2] * 0.2;
			angles[2][0] = vTorsoAngles[0];
			angles[2][1] = vTorsoAngles[1] * 0.80000001 + vTorsoAngles[2];
			angles[2][2] = vTorsoAngles[2] * -0.2;
		}
		else
		{
			if ( fLeanFrac != 0.0 )
			{
				if ( (es->eFlags & 4) != 0 )
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

			angles[0][0] = vTorsoAngles[0] * 0.2;
			angles[0][1] = vTorsoAngles[1] * 0.40000001;
			angles[0][2] = vTorsoAngles[2] * 0.5;

			if ( es->fTorsoPitch != 0.0 || es->fWaistPitch != 0.0 )
			{
				ang2 = AngleSubtract(es->fTorsoPitch, es->fWaistPitch);
				angles[0][0] = ang2 + angles[0][0];
			}

			angles[1][0] = vTorsoAngles[0] * 0.30000001;
			angles[1][1] = vTorsoAngles[1] * 0.40000001;
			angles[1][2] = vTorsoAngles[2] * 0.5;
			angles[2][0] = vTorsoAngles[0] * 0.5;
			angles[2][1] = vTorsoAngles[1] * 0.2;
			angles[2][2] = vTorsoAngles[2] * -0.60000002;
		}

		angles[3][0] = vHeadAngles[0] * 0.30000001;
		angles[3][1] = vHeadAngles[1] * 0.30000001;
		angles[3][2] = 0.0;
		angles[4][0] = vHeadAngles[0] * 0.69999999;
		angles[4][1] = vHeadAngles[1] * 0.69999999;
		angles[4][2] = vHeadAngles[2] * -0.30000001;
		VectorClear(angles[5]);

		if ( es->fWaistPitch != 0.0 || es->fTorsoPitch != 0.0 )
			angles[5][0] = AngleSubtract(es->fWaistPitch, es->fTorsoPitch);

		for ( i = 0; i < 6; ++i )
			VectorCopy(angles[i], info->angles[i]);

		VectorCopy(tag_origin_angles, info->tag_origin_angles);
		VectorCopy(tag_origin_offset, info->tag_origin_offset);
	}
}

void BG_Player_DoControllers(DObj_s *obj, const gentity_s *ent, int *partBits, clientInfo_t *ci, int frametime)
{
	float maxOffsetChange;
	clientControllers_t controllers;
	float maxAngleChange;
	int i;

	BG_Player_DoControllersInternal(obj, &ent->s, partBits, ci, &controllers);
	maxAngleChange = (float)frametime * 0.36000001;

	for ( i = 0; i < 6; ++i )
	{
		BG_LerpAngles(controllers.angles[i], maxAngleChange, ci->control.angles[i]);
		DObjSetControlTagAngles(obj, partBits, *controller_names[i], ci->control.angles[i]);
	}

	BG_LerpAngles(controllers.tag_origin_angles, maxAngleChange, ci->control.tag_origin_angles);
	maxOffsetChange = (float)frametime * 0.1;
	BG_LerpOffset(controllers.tag_origin_offset, maxOffsetChange, ci->control.tag_origin_offset);
	DObjSetLocalTag(obj, partBits, scr_const.tag_origin, ci->control.tag_origin_offset, ci->control.tag_origin_angles);
}

void BG_SetupAnimNoteTypes(animScriptData_t *scriptData)
{
	int k;
	int i;
	int j;
	animScriptItem_t *item;
	animScript_t *script;

	for ( i = 0; i < scriptData->numAnimations; ++i )
		scriptData->animations[i].noteType = 0;

	if ( !level_bgs.anim_user )
	{
		script = &scriptData->scriptEvents[10];

		for ( j = 0; j < script->numItems; ++j )
		{
			item = script->items[j];

			for ( k = 0; k < item->numCommands; ++k )
			{
				if ( item->commands[k].bodyPart[0] )
					scriptData->animations[item->commands[k].animIndex[0]].noteType = 1;
				if ( item->commands[k].bodyPart[1] )
					scriptData->animations[item->commands[k].animIndex[1]].noteType = 1;
			}
		}
	}
}

void BG_FinalizePlayerAnims()
{
	const char *name;
	float length;
	loadAnim_t *loadAnim;
	vec3_t vDelta;
	vec2_t vRot;
	XAnim_s *xanim;
	animation_s *animation;
	int size;
	int index;

	animation = globalScriptData->animations;
	xanim = globalScriptData->animTree.anims;
	size = XAnimGetAnimTreeSize(xanim);
	globalScriptData->numAnimations = size;
	animation->flags |= 0x101u;
	I_strncpyz(animation->name, "root", 64);
	animation->nameHash = 0;
	++animation;
	index = 1;

	while ( index < size )
	{
		loadAnim = BG_GetAnim(index);

		if ( loadAnim )
		{
			if ( XAnimIsPrimitive(xanim, index) )
			{
				name = XAnimGetAnimName(xanim, index);
				I_strncpyz(animation->name, name, 64);
				animation->nameHash = BG_StringHashValue_Lwr(animation->name);

				if ( !animation->initialLerp )
					animation->initialLerp = -1;

				length = XAnimGetLength(xanim, index);

				if ( length == 0.0 )
				{
					animation->duration = 500;
					animation->moveSpeed = 0.0;
				}
				else
				{
					animation->duration = (int)(length * 1000.0);
					XAnimGetRelDelta(xanim, index, vRot, vDelta, 0.0, 1.0);
					animation->moveSpeed = VectorLength(vDelta) / length;

					if ( animation->duration <= 499 )
						animation->duration = 500;
				}
				if ( XAnimIsLooped(xanim, index) )
					animation->flags |= 0x80u;
			}
			else
			{
				animation->flags |= 1u;
				I_strncpyz(animation->name, loadAnim->name, 64);
				animation->nameHash = loadAnim->nameHash;

				if ( !animation->initialLerp )
					animation->initialLerp = -1;

				animation->duration = 0;
				animation->moveSpeed = 0.0;
			}
		}
		else
		{
			animation->flags |= 0x100u;
			I_strncpyz(animation->name, "unused", 64);
			animation->nameHash = 0;
		}

		++index;
		++animation;
	}

	BG_AnimParseAnimScript(globalScriptData, 0, 0);
	BG_SetupAnimNoteTypes(globalScriptData);
}

void BG_PostLoadAnim()
{
	XAnim_s *anims;
	int i;
	int j;

	anims = level_bgs.animData.generic_human.tree.anims;

	for ( i = 0; i < 64; ++i )
		level_bgs.clientinfo[i].pXAnimTree = XAnimCreateTree(anims, Hunk_AllocXAnimServer);

	for ( j = 0; j < 8; ++j )
		g_scr_data.playerCorpseInfo[j].tree = XAnimCreateTree(anims, Hunk_AllocXAnimServer);
}

void BG_FindAnimTree(scr_animtree_t *pTree, const char *filename, int bEnforceExists)
{
	scr_animtree_t tree;

	Scr_FindAnimTreeInternal(&tree, filename);
	pTree->anims = tree.anims;

	if ( !pTree->anims )
	{
		if ( bEnforceExists )
			Com_Error(ERR_DROP, "Could not find animation tree '%s'", filename);
	}
}

void BG_FindAnimTrees()
{
	scr_animtree_t *animTree;
	scr_animtree_t loadAnimTree;

	animTree = &level_bgs.animData.generic_human.tree;
	BG_FindAnimTree(&loadAnimTree, "multiplayer", 1);
	animTree->anims = loadAnimTree.anims;
	level_bgs.animData.animScriptData.animTree = level_bgs.animData.generic_human.tree;
	level_bgs.animData.animScriptData.torsoAnim = level_bgs.animData.generic_human.torso.index;
	level_bgs.animData.animScriptData.legsAnim = level_bgs.animData.generic_human.legs.index;
	level_bgs.animData.animScriptData.turningAnim = level_bgs.animData.generic_human.turning.index;
}

void BG_FindAnims()
{
	Scr_FindAnim("multiplayer", "root", &level_bgs.animData.generic_human.root, level_bgs.anim_user);
	Scr_FindAnim("multiplayer", "torso", &level_bgs.animData.generic_human.torso, level_bgs.anim_user);
	Scr_FindAnim("multiplayer", "legs", &level_bgs.animData.generic_human.legs, level_bgs.anim_user);
	Scr_FindAnim("multiplayer", "turning", &level_bgs.animData.generic_human.turning, level_bgs.anim_user);
}

// VoroN: Made this thing static because it was crashing otherwise
loadAnim_t playerAnims[MAX_MODEL_ANIMATIONS];
unsigned int iNumPlayerAnims;
void BG_LoadAnim()
{
	memset( playerAnims, 0, sizeof( playerAnims ) );
	iNumPlayerAnims = 0;

	BG_FindAnims();
	BG_AnimParseAnimScript(&level_bgs.animData.animScriptData, playerAnims, &iNumPlayerAnims);
	Scr_PrecacheAnimTrees(level_bgs.AllocXAnim, level_bgs.anim_user);
	BG_FindAnimTrees();
	Scr_EndLoadAnimTrees();
	BG_FinalizePlayerAnims();
}