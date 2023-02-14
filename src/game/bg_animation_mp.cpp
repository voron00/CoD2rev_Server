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