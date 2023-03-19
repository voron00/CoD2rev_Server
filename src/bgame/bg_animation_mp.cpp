#include "../qcommon/qcommon.h"
#include "bg_public.h"

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
static const char *globalTypesFilename = "mp/playeranimtypes.txt";    // to prevent redundant params

// VoroN: iDK if these should be in this file or not, but whatever.
unsigned int g_playerAnimTypeNamesCount;
char *g_playerAnimTypeNames;
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

	hash = BG_StringHashValue_Lwr( token );

	for ( i = 0, strav = strings; strav->string; strav++, i++ )
	{
		if ( strav->hash == -1 )
		{
			strav->hash = BG_StringHashValue_Lwr( strav->string );
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
	int /*indexBits,*/ tempBits[2];
	char currentString[MAX_QPATH];
	qboolean minus = qfalse;
	char *token;

	//indexBits = 0;
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

/*
============
BG_LoadPlayerAnimTypes
============
*/
void BG_LoadPlayerAnimTypes()
{
	unsigned int count;
	int len;
	fileHandle_t f;
	char buffer[4096];
	char *s;
	const char *data_p;

	g_playerAnimTypeNamesCount = 0;
	len = FS_FOpenFileByMode(globalTypesFilename, &f, FS_READ);

	if ( len < 0 )
		Com_Error(ERR_DROP, "Couldn't load file ", globalTypesFilename);

	if ( len > 4095 )
	{
		FS_FCloseFile(f);
		Com_Error(ERR_DROP, "%s max size exceeded", globalTypesFilename);
	}

	FS_Read(buffer, len, f);
	buffer[len] = 0;
	FS_FCloseFile(f);
	data_p = buffer;

	Com_BeginParseSession("BG_AnimParseAnimScript");

	while ( 1 )
	{
		s = Com_Parse(&data_p);

		if ( !s || !*s )
			break;

		if ( g_playerAnimTypeNamesCount > 63 )
			Com_Error(ERR_DROP, "Player anim type array size exceeded");

		count = g_playerAnimTypeNamesCount;
		(&g_playerAnimTypeNames)[count] = (char *)Hunk_AllocLowInternal(strlen(s) + 1);
		strcpy((&g_playerAnimTypeNames)[g_playerAnimTypeNamesCount], s);
		++g_playerAnimTypeNamesCount;
	}

	Com_EndParseSession();
}

/*
============
BG_InitWeaponStrings
  Builds the list of weapon names from the item list. This is done here rather
  than hardcoded to ease the process of modifying the weapons.
============
*/
void BG_InitWeaponString(int index, const char *name)
{
	weaponStrings[index].string = name;
	weaponStrings[index].hash = BG_StringHashValue_Lwr(name);
}

void BG_LoadWeaponStrings()
{
	int index;

	for ( index = 0; index < g_playerAnimTypeNamesCount; ++index )
		BG_InitWeaponString(index, (&g_playerAnimTypeNames)[index]);
}

void BG_InitWeaponStrings()
{
	memset(weaponStrings, 0, sizeof(weaponStrings));
	BG_LoadWeaponStrings();
}

