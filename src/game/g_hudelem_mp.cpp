#include "../qcommon/qcommon.h"
#include "g_shared.h"
#include "../script/script_public.h"

game_hudelem_t g_hudelems[MAX_GENTITIES];

const char *g_he_horzalign[] =
{
	"subleft",
	"left",
	"center",
	"right",
	"fullscreen",
	"noscale",
	"alignto640",
	"center_safearea"
};

const char *g_he_vertalign[] =
{
	"subtop",
	"top",
	"middle",
	"bottom",
	"fullscreen",
	"noscale",
	"alignto480",
	"center_safearea"
};

const char *g_he_alignx[] = { "left", "center", "right" };
const char *g_he_aligny[] = { "top", "middle", "bottom" };
const char *g_he_font[] =   { "default", "bigfixed", "smallfixed", };

#define HEOFS( x ) (intptr_t)&( (game_hudelem_t*)0 )->x

game_hudelem_field_t g_hudelem_fields[] =
{
	{ "x", HEOFS( elem.x ), F_FLOAT, 0, 0, NULL, NULL },
	{ "y", HEOFS( elem.y ), F_FLOAT, 0, 0, NULL, NULL },
	{ "z", HEOFS( elem.z ), F_FLOAT, 0, 0, NULL, NULL },
	{ "fontscale", HEOFS( elem.fontScale ), F_FLOAT, -1, 0, HudElem_SetFontScale, NULL },
	{ "font", HEOFS( elem.font ), F_INT, -1, 0, HudElem_SetFont, HudElem_GetFont },
	{ "alignx", HEOFS( elem.alignOrg ), F_INT, ARRAY_COUNT( g_he_alignx )-1, 2, HudElem_SetAlignX, HudElem_GetAlignX },
	{ "aligny", HEOFS( elem.alignOrg ), F_INT, ARRAY_COUNT( g_he_aligny )-1, 0, HudElem_SetAlignY, HudElem_GetAlignY },
	{ "horzalign", HEOFS( elem.alignScreen ), F_INT, ARRAY_COUNT( g_he_horzalign )-1, 3, HudElem_SetHorzAlign, HudElem_GetHorzAlign },
	{ "vertalign", HEOFS( elem.alignScreen ), F_INT, ARRAY_COUNT( g_he_vertalign )-1, 0, HudElem_SetVertAlign, HudElem_GetVertAlign },
	{ "color", HEOFS( elem.color ), F_INT, -1, 0, HudElem_SetColor, HudElem_GetColor },
	{ "alpha", HEOFS( elem.color ), F_INT, -1, 0, HudElem_SetAlpha, HudElem_GetAlpha },
	{ "label", HEOFS( elem.label ), F_INT, -1, 0, HudElem_SetLocalizedString, NULL },
	{ "sort", HEOFS( elem.sort ), F_FLOAT, 0, 0, NULL, NULL },
	{ "foreground", HEOFS( elem.foreground ), F_INT, -1, 0, HudElem_SetBoolean, NULL },
	{ "archived", HEOFS( archived ), F_INT, -1, 0, HudElem_SetBoolean, NULL },
	{ NULL, 0, F_INT, -1, 0, NULL, NULL } // field terminator
};

scr_method_t g_he_methods[] =
{
	{"settext", HECmd_SetText, 0 },
	{"setplayernamestring", HECmd_SetPlayerNameString, 0 },
	{"setmapnamestring", HECmd_SetMapNameString, 0 },
	{"setgametypestring", HECmd_SetGameTypeString, 0 },
	{"setshader", HECmd_SetShader, 0 },
	{"settimer", HECmd_SetTimer, 0 },
	{"settimerup", HECmd_SetTimerUp, 0 },
	{"settenthstimer", HECmd_SetTenthsTimer, 0 },
	{"settenthstimerup", HECmd_SetTenthsTimerUp, 0 },
	{"setclock", HECmd_SetClock, 0 },
	{"setclockup", HECmd_SetClockUp, 0 },
	{"setvalue", HECmd_SetValue, 0 },
	{"setwaypoint", HECmd_SetWaypoint, 0 },
	{"fadeovertime", HECmd_FadeOverTime, 0 },
	{"scaleovertime", HECmd_ScaleOverTime, 0 },
	{"moveovertime", HECmd_MoveOverTime, 0 },
	{"reset", HECmd_Reset, 0 },
	{"destroy", HECmd_Destroy, 0 }
};

/*
============
HudElem_UpdateClient
============
*/
void HudElem_UpdateClient( gclient_t *client, int clientNum, int which )
{
	game_hudelem_t *hud;
	hudelem_t *elem;
	int currentCount;
	int archivalCount;
	int i;

	assert(clientNum >= 0 && clientNum < level.maxclients);
	assert(level.gentities[clientNum].r.inuse);
	assert(client);

	if ( which & HUDELEM_UPDATE_ARCHIVAL )
	{
		memset(client->ps.hud.archival, 0, sizeof(client->ps.hud.archival));
	}

	if ( which & HUDELEM_UPDATE_CURRENT )
	{
		memset(client->ps.hud.current, 0, sizeof(client->ps.hud.current));
	}

	archivalCount = 0;
	currentCount = 0;

	for ( i = 0, hud = g_hudelems; i < MAX_GENTITIES; i++, hud++ )
	{
		if ( hud->elem.type == HE_TYPE_FREE )
		{
			continue;
		}

		if ( hud->team != TEAM_FREE && hud->team != client->sess.state.team )
		{
			continue;
		}

		if ( hud->clientNum != ENTITYNUM_NONE && hud->clientNum != clientNum )
		{
			continue;
		}

		if ( hud->archived )
		{
			if ( which & HUDELEM_UPDATE_ARCHIVAL )
			{
				elem = &client->ps.hud.archival[archivalCount];
				archivalCount++;

				if ( archivalCount <= MAX_HUDELEMS_ARCHIVAL )
				{
					*elem = hud->elem;
				}
			}
		}
		else
		{
			if ( which & HUDELEM_UPDATE_CURRENT )
			{
				elem = &client->ps.hud.current[currentCount];
				currentCount++;

				if ( currentCount <= MAX_HUDELEMS_CURRENT )
				{
					*elem = hud->elem;
				}
			}
		}
	}
}

/*
============
HudElem_GetMethod
============
*/
void (*HudElem_GetMethod( const char **pName ))( scr_entref_t )
{
	for ( int i = 0; i < ARRAY_COUNT(g_he_methods); i++ )
	{
		if ( !strcmp(*pName, g_he_methods[i].name) )
		{
			*pName = g_he_methods[i].name;
			return g_he_methods[i].call;
		}
	}

	return NULL;
}

/*
============
HudElem_Alloc
============
*/
game_hudelem_t *HudElem_Alloc( int clientNum, int teamNum )
{
	for ( int i = 0; i < ARRAY_COUNT(g_hudelems); i++ )
	{
		if ( g_hudelems[i].elem.type == HE_TYPE_FREE )
		{
			HudElem_SetDefaults(&g_hudelems[i]);

			g_hudelems[i].clientNum = clientNum;
			g_hudelems[i].team = teamNum;

			return &g_hudelems[i];
		}
	}

	return NULL;
}

/*
============
GScr_AddFieldsForHudElems
============
*/
void GScr_AddFieldsForHudElems()
{
	game_hudelem_field_t *f;

	for ( f = g_hudelem_fields; f->name; f++ )
	{
		Scr_AddClassField(CLASS_NUM_HUDELEM, f->name, f - g_hudelem_fields);
	}
}

/*
============
Scr_FreeHudElemConstStrings
============
*/
void Scr_FreeHudElemConstStrings( game_hudelem_t *hud )
{
	game_hudelem_field_t *f;

	for ( f = g_hudelem_fields; f->name; f++ )
	{
		if ( f->type == F_STRING )
		{
			Scr_SetString((unsigned short *)((byte *)hud + f->ofs), 0);
		}
	}
}

/*
============
Scr_SetHudElemField
============
*/
void Scr_SetHudElemField( int entnum, int offset )
{
	game_hudelem_t *hud;
	game_hudelem_field_t *f;

	assert((unsigned)offset < ARRAY_COUNT( g_hudelem_fields ) - 1);
	assert((unsigned)entnum < ARRAY_COUNT( g_hudelems ));

	f = &g_hudelem_fields[offset];
	hud = &g_hudelems[entnum];

	if ( f->setter )
	{
		f->setter(hud, offset);
	}
	else
	{
		Scr_SetGenericField((byte *)hud, f->type, f->ofs);
	}
}

/*
============
HudElem_Free
============
*/
void HudElem_Free( game_hudelem_t *hud )
{
	assert((unsigned)(hud - g_hudelems) < ARRAY_COUNT( g_hudelems ));
	assert(hud->elem.type > HE_TYPE_FREE && hud->elem.type < HE_TYPE_COUNT);
	Scr_FreeHudElem(hud);
	hud->elem.type = HE_TYPE_FREE;
}

/*
============
HudElem_DestroyAll
============
*/
void HudElem_DestroyAll()
{
	for ( int i = 0; i < ARRAY_COUNT(g_hudelems); i++ )
	{
		if ( g_hudelems[i].elem.type != HE_TYPE_FREE )
		{
			HudElem_Free(&g_hudelems[i]);
		}
	}

	memset(g_hudelems, 0, sizeof(g_hudelems));
}

/*
============
HudElem_ClientDisconnect
============
*/
void HudElem_ClientDisconnect( gentity_t *ent )
{
	for ( int i = 0; i < ARRAY_COUNT(g_hudelems); i++ )
	{
		if ( g_hudelems[i].elem.type != HE_TYPE_FREE )
		{
			if ( g_hudelems[i].clientNum == ent->s.number )
			{
				HudElem_Free(&g_hudelems[i]);
			}
		}
	}
}

/*
============
GScr_NewTeamHudElem
============
*/
void GScr_NewTeamHudElem()
{
	unsigned short teamName;
	game_hudelem_t *hud;

	teamName = Scr_GetConstString(0);

	if ( teamName == scr_const.allies )
	{
		hud = HudElem_Alloc(ENTITYNUM_NONE, TEAM_ALLIES);
	}
	else if ( teamName == scr_const.axis )
	{
		hud = HudElem_Alloc(ENTITYNUM_NONE, TEAM_AXIS);
	}
	else if ( teamName == scr_const.spectator )
	{
		hud = HudElem_Alloc(ENTITYNUM_NONE, TEAM_SPECTATOR);
	}
	else
	{
		Scr_ParamError(0, va("team \"%s\" should be \"allies\", \"axis\", or \"spectator\"", Scr_GetString(0)));
		hud = HudElem_Alloc(ENTITYNUM_NONE, TEAM_FREE);
	}

	if ( !hud )
		Scr_Error("out of hudelems");

	Scr_AddHudElem(hud);
}

/*
============
GScr_NewClientHudElem
============
*/
void GScr_NewClientHudElem()
{
	gentity_t *ent;
	game_hudelem_t *hud;

	ent = Scr_GetEntity(0);
	assert(ent);
	assert(ent->r.inuse);

	if ( !ent->client )
		Scr_ParamError(0, "not a client");

	hud = HudElem_Alloc(ent->s.number, TEAM_NONE);

	if ( !hud )
		Scr_Error("out of hudelems");

	Scr_AddHudElem(hud);
}

/*
============
GScr_NewHudElem
============
*/
void GScr_NewHudElem()
{
	game_hudelem_t *hud;

	hud = HudElem_Alloc(ENTITYNUM_NONE, TEAM_NONE);

	if ( !hud )
		Scr_Error("out of hudelems");

	Scr_AddHudElem(hud);
}

/*
============
Scr_GetHudElemField
============
*/
void Scr_GetHudElemField( int entnum, int offset )
{
	game_hudelem_t *hud;
	game_hudelem_field_t *f;

	assert((unsigned)offset < ARRAY_COUNT( g_hudelem_fields ) - 1);
	assert((unsigned)entnum < ARRAY_COUNT( g_hudelems ));

	f = &g_hudelem_fields[offset];
	hud = &g_hudelems[entnum];

	if ( f->getter )
	{
		f->getter(hud, offset);
	}
	else
	{
		Scr_GetGenericField((byte *)hud, f->type, f->ofs);
	}
}

/*
============
HudElem_ClearTypeSettings
============
*/
void HudElem_ClearTypeSettings( game_hudelem_t *hud )
{
	hud->elem.width = 0;
	hud->elem.height = 0;
	hud->elem.materialIndex = 0;
	hud->elem.fromX = 0;
	hud->elem.fromY = 0;
	hud->elem.fromAlignOrg = 0;
	hud->elem.fromAlignScreen = 0;
	hud->elem.fromWidth = 0;
	hud->elem.fromHeight = 0;
	hud->elem.scaleStartTime = 0;
	hud->elem.scaleTime = 0;
	hud->elem.time = 0;
	hud->elem.duration = 0;
	hud->elem.value = 0;
	hud->elem.text = 0;
}

/*
============
HudElem_SetFontScale
============
*/
void HudElem_SetFontScale( game_hudelem_t *hud, int offset )
{
	float scale;

	assert(g_hudelem_fields[offset].ofs == HEOFS( elem.fontScale ));
	scale = Scr_GetFloat(0);

	if ( scale <= 0 )
	{
		Scr_Error(va("font scale was %g; should be > 0", scale));
	}

	hud->elem.fontScale = scale;
}

/*
============
HudElem_SetBoolean
============
*/
void HudElem_SetBoolean( game_hudelem_t *hud, int offset )
{
	game_hudelem_field_t *f;

	f = &g_hudelem_fields[offset];
	*(int *)((byte *)hud + f->ofs) = Scr_GetInt(0) ? qtrue : qfalse; // fix: boolean value
}

/*
============
HudElem_SetDefaults
============
*/
void HudElem_SetDefaults( game_hudelem_t *hud )
{
	assert((unsigned)(hud - g_hudelems) < ARRAY_COUNT( g_hudelems ));

	hud->elem.type = HE_TYPE_TEXT;
	hud->elem.x = 0;
	hud->elem.y = 0;
	hud->elem.z = 0;
	hud->elem.fontScale = 1.0f;
	hud->elem.font = 0;
	hud->elem.alignOrg = 0;
	hud->elem.alignScreen = 0;
	hud->elem.color.rgba = -1;
	hud->elem.fromColor.rgba = 0;
	hud->elem.fadeStartTime = 0;
	hud->elem.fadeTime = 0;
	hud->elem.label = 0;
	hud->elem.sort = 0;
	hud->elem.foreground.rgba = 0;
	hud->archived = qtrue;

	HudElem_ClearTypeSettings(hud);
}

/*
============
HECmd_GetHudElem
============
*/
game_hudelem_t* HECmd_GetHudElem( scr_entref_t entref )
{
	if ( entref.classnum == CLASS_NUM_HUDELEM )
	{
		assert(entref.entnum < ARRAY_COUNT( g_hudelems ));
		return &g_hudelems[entref.entnum];
	}

	Scr_ObjectError("not a hud element");
	return NULL;
}

/*
============
HECmd_Reset
============
*/
void HECmd_Reset( scr_entref_t entref )
{
	game_hudelem_t *hud;

	hud = HECmd_GetHudElem(entref);
	HudElem_SetDefaults(hud);
}

/*
============
HECmd_MoveOverTime
============
*/
void HECmd_MoveOverTime( scr_entref_t entref )
{
	float movetime;
	game_hudelem_t *hud;

	hud = HECmd_GetHudElem(entref);
	movetime = Scr_GetFloat(0);

	if ( movetime <= 0 )
	{
		Scr_ParamError(0, va("move time %g <= 0", movetime));
	}

	if ( movetime > 60 )
	{
		Scr_ParamError(0, va("move time %g > 60", movetime));
	}

	hud->elem.moveStartTime = level.time;
	hud->elem.moveTime = Q_rint(movetime * 1000);
	hud->elem.fromX = hud->elem.x;
	hud->elem.fromY = hud->elem.y;
	hud->elem.fromAlignOrg = hud->elem.alignOrg;
	hud->elem.fromAlignScreen = hud->elem.alignScreen;
}

/*
============
HECmd_ScaleOverTime
============
*/
void HECmd_ScaleOverTime( scr_entref_t entref )
{
	int height;
	int width;
	float scaleTime;
	game_hudelem_t *hud;

	hud = HECmd_GetHudElem(entref);

	if ( Scr_GetNumParam() != 3 )
	{
		Scr_Error("hudelem scaleOverTime(time_in_seconds, new_width, new_height)");
	}

	scaleTime = Scr_GetFloat(0);

	if ( scaleTime <= 0 )
	{
		Scr_ParamError(0, va("scale time %g <= 0", scaleTime));
	}

	if ( scaleTime > 60 )
	{
		Scr_ParamError(0, va("scale time %g > 60", scaleTime));
	}

	width = Scr_GetInt(1);
	height = Scr_GetInt(2);

	hud->elem.scaleStartTime = level.time;
	hud->elem.scaleTime = Q_rint(scaleTime * 1000);
	hud->elem.fromWidth = hud->elem.width;
	hud->elem.fromHeight = hud->elem.height;
	hud->elem.width = width;
	hud->elem.height = height;
}

/*
============
HECmd_FadeOverTime
============
*/
void HECmd_FadeOverTime( scr_entref_t entref )
{
	float fadeTime;
	game_hudelem_t *hud;

	hud = HECmd_GetHudElem(entref);
	fadeTime = Scr_GetFloat(0);

	if ( fadeTime <= 0 )
	{
		Scr_ParamError(0, va("fade time %g <= 0", fadeTime));
	}

	if ( fadeTime > 60 )
	{
		Scr_ParamError(0, va("fade time %g > 60", fadeTime));
	}

	hud->elem.fadeStartTime = level.time;
	hud->elem.fadeTime = Q_rint(fadeTime * 1000);
	hud->elem.fromColor.rgba = hud->elem.color.rgba;
}

/*
============
HECmd_SetWaypoint
============
*/
void HECmd_SetWaypoint( scr_entref_t entref )
{
	int constantSize;
	game_hudelem_t *hud;

	hud = HECmd_GetHudElem(entref);
	constantSize = Scr_GetInt(0);

	hud->elem.type = HE_TYPE_WAYPOINT;
	hud->elem.value = (float)constantSize;
}

/*
============
HECmd_SetValue
============
*/
void HECmd_SetValue( scr_entref_t entref )
{
	game_hudelem_t *hud;
	float value;

	hud = HECmd_GetHudElem(entref);
	value = Scr_GetFloat(0);

	HudElem_ClearTypeSettings(hud);

	hud->elem.type = HE_TYPE_VALUE;
	hud->elem.value = value;
}

/*
============
HECmd_SetTimer_Internal
============
*/
static void HECmd_SetTimer_Internal( scr_entref_t entref, int type, const char *cmdName )
{
	int iTime;
	game_hudelem_t *hud;

	hud = HECmd_GetHudElem(entref);

	if ( Scr_GetNumParam() != 1 )
	{
		Scr_Error(va("USAGE: <hudelem> %s(time_in_seconds);\n", cmdName));
	}

	iTime = FastCeil(Scr_GetFloat(0) * 1000);

	if ( iTime <= 0 && type != HE_TYPE_TIMER_UP )
	{
		Scr_ParamError(0, va("time %g should be > 0", iTime * 0.001));
	}

	HudElem_ClearTypeSettings(hud);

	hud->elem.type = type;
	hud->elem.time = level.time + iTime;
}

/*
============
HudElem_SetAlpha
============
*/
void HudElem_SetAlpha( game_hudelem_t *hud, int offset )
{
	float alpha;
	assert(g_hudelem_fields[offset].ofs == HEOFS( elem.color ));

	alpha = Scr_GetFloat(0);
	hud->elem.color.split.a = Q_rint(I_fmax(0.0f, I_fmin(1.0f, alpha)) * 255);
}

/*
============
HudElem_SetColor
============
*/
void HudElem_SetColor(game_hudelem_t *hud, int offset)
{
	vec3_t color;
	assert(g_hudelem_fields[offset].ofs == HEOFS( elem.color ));

	Scr_GetVector(0, color);

	hud->elem.color.split.r = Q_rint(I_fmax(0.0f, I_fmin(1.0f, color[0])) * 255);
	hud->elem.color.split.g = Q_rint(I_fmax(0.0f, I_fmin(1.0f, color[1])) * 255);
	hud->elem.color.split.b = Q_rint(I_fmax(0.0f, I_fmin(1.0f, color[2])) * 255);
}

/*
============
HECmd_SetTenthsTimerUp
============
*/
void HECmd_SetTenthsTimerUp( scr_entref_t entref )
{
	HECmd_SetTimer_Internal(entref, HE_TYPE_TENTHS_TIMER_UP, "setTenthsTimerUp");
}

/*
============
HECmd_SetTenthsTimer
============
*/
void HECmd_SetTenthsTimer( scr_entref_t entref )
{
	HECmd_SetTimer_Internal(entref, HE_TYPE_TENTHS_TIMER_DOWN, "setTenthsTimer");
}

/*
============
HECmd_SetTimerUp
============
*/
void HECmd_SetTimerUp( scr_entref_t entref )
{
	HECmd_SetTimer_Internal(entref, HE_TYPE_TIMER_UP, "setTimerUp");
}

/*
============
HECmd_SetTimer
============
*/
void HECmd_SetTimer( scr_entref_t entref )
{
	HECmd_SetTimer_Internal(entref, HE_TYPE_TIMER_DOWN, "setTimer");
}

/*
============
HudElem_SetEnumString
============
*/
static void HudElem_SetEnumString( game_hudelem_t *hud, const game_hudelem_field_t *f, const char **names, int nameCount )
{
	int *value;
	char errormsg[2048];
	const char *selectedName;
	int nameIndex;

	assert(hud);
	assert(f);
	assert(names);
	assert(nameCount > 0);

	value = (int *)((byte *)hud + f->ofs);
	selectedName = Scr_GetString(0);

	for ( nameIndex = 0; nameIndex < nameCount; nameIndex++ )
	{
		if ( !I_stricmp(selectedName, names[nameIndex]) )
		{
			*value &= ~(f->size << f->shift);
			*value |= nameIndex << f->shift;
			return;
		}
	}

	sprintf(errormsg, "\"%s\" is not a valid value for hudelem field \"%s\"\nShould be one of:", selectedName, f->name);

	for ( nameIndex = 0; nameIndex < nameCount; nameIndex++ )
	{
		strncat(errormsg, va(" %s", names[nameIndex]), sizeof(errormsg) - 1);
		errormsg[sizeof(errormsg) - 1] = 0;
	}

	Scr_Error(errormsg);
}

/*
============
HudElem_SetVertAlign
============
*/
void HudElem_SetVertAlign( game_hudelem_t *hud, int offset )
{
	HudElem_SetEnumString(hud, &g_hudelem_fields[offset], g_he_vertalign, ARRAY_COUNT(g_he_vertalign));
}

/*
============
HudElem_SetHorzAlign
============
*/
void HudElem_SetHorzAlign( game_hudelem_t *hud, int offset )
{
	HudElem_SetEnumString(hud, &g_hudelem_fields[offset], g_he_horzalign, ARRAY_COUNT(g_he_horzalign));
}

/*
============
HudElem_SetAlignY
============
*/
void HudElem_SetAlignY( game_hudelem_t *hud, int offset )
{
	HudElem_SetEnumString(hud, &g_hudelem_fields[offset], g_he_aligny, ARRAY_COUNT(g_he_aligny));
}

/*
============
HudElem_SetAlignX
============
*/
void HudElem_SetAlignX( game_hudelem_t *hud, int offset )
{
	HudElem_SetEnumString(hud, &g_hudelem_fields[offset], g_he_alignx, ARRAY_COUNT(g_he_alignx));
}

/*
============
HudElem_SetFont
============
*/
void HudElem_SetFont( game_hudelem_t *hud, int offset )
{
	HudElem_SetEnumString(hud, &g_hudelem_fields[offset], g_he_font, ARRAY_COUNT(g_he_font));
}

/*
============
HECmd_Destroy
============
*/
void HECmd_Destroy( scr_entref_t entref )
{
	game_hudelem_t *hud;

	hud = HECmd_GetHudElem(entref);
	HudElem_Free(hud);
}

/*
============
HECmd_SetClock_Internal
============
*/
static void HECmd_SetClock_Internal( scr_entref_t entref, int type, const char *cmdName )
{
	const char *materialName;
	int height;
	int width;
	int materialIndex;
	int duration;
	int iTime;
	int numParam;
	game_hudelem_t *hud;

	hud = HECmd_GetHudElem(entref);
	numParam = Scr_GetNumParam();

	if ( numParam != 3 && numParam != 5 )
	{
		Scr_Error(va("USAGE: <hudelem> %s(time_in_seconds, total_clock_time_in_seconds, shadername[, width, height]);\n", cmdName));
	}

	iTime = FastCeil(Scr_GetFloat(0) * 1000);

	if ( iTime <= 0 && type != HE_TYPE_CLOCK_UP )
	{
		Scr_ParamError(0, va("time %g should be > 0", iTime * 0.001));
	}

	duration = FastCeil(Scr_GetFloat(1) * 1000);

	if ( duration <= 0 )
	{
		Scr_ParamError(1, va("duration %g should be > 0", duration * 0.001));
	}

	materialName = Scr_GetString(2);
	materialIndex = G_ShaderIndex(materialName);

	if ( numParam == 3 )
	{
		width = 0;
		height = 0;
	}
	else
	{
		width = Scr_GetInt(3);

		if ( width < 0 )
		{
			Scr_ParamError(3, va("width %i < 0", width));
		}

		height = Scr_GetInt(4);

		if ( height < 0 )
		{
			Scr_ParamError(4, va("height %i < 0", height));
		}
	}

	HudElem_ClearTypeSettings(hud);

	hud->elem.type = type;
	hud->elem.time = level.time + iTime;
	hud->elem.duration = duration;
	hud->elem.materialIndex = materialIndex;
	hud->elem.width = width;
	hud->elem.height = height;
}

/*
============
HECmd_SetShader
============
*/
void HECmd_SetShader( scr_entref_t entref )
{
	const char *materialName;
	int materialIndex;
	int height;
	int width;
	int numParam;
	game_hudelem_t *hud;

	hud = HECmd_GetHudElem(entref);
	numParam = Scr_GetNumParam();

	if ( numParam != 1 && numParam != 3 )
	{
		Scr_Error("USAGE: <hudelem> setShader(\"shadername\"[, optional_width, optional_height]);");
	}

	materialName = Scr_GetString(0);
	materialIndex = G_ShaderIndex(materialName);

	if ( numParam == 1 )
	{
		width = 0;
		height = 0;
	}
	else
	{
		width = Scr_GetInt(1);

		if ( width < 0 )
		{
			Scr_ParamError(1, va("width %i < 0", width));
		}

		height = Scr_GetInt(2);

		if ( height < 0 )
		{
			Scr_ParamError(2, va("height %i < 0", height));
		}
	}

	HudElem_ClearTypeSettings(hud);

	hud->elem.type = HE_TYPE_MATERIAL;
	hud->elem.materialIndex = materialIndex;
	hud->elem.width = width;
	hud->elem.height = height;
}

/*
============
HECmd_SetText
============
*/
void HECmd_SetText( scr_entref_t entref )
{
	game_hudelem_t *hud;
	const char *string;

	hud = HECmd_GetHudElem(entref);
	string = Scr_GetIString(0);

	HudElem_ClearTypeSettings(hud);

	hud->elem.type = HE_TYPE_TEXT;
	hud->elem.text = G_LocalizedStringIndex(string);
}

/*
============
HudElem_GetAlpha
============
*/
void HudElem_GetAlpha( game_hudelem_t *hud, int offset )
{
	assert(g_hudelem_fields[offset].ofs == HEOFS( elem.color ));
	Scr_AddFloat(hud->elem.color.split.a * (1.0f/255));
}

/*
============
HudElem_GetColor
============
*/
void HudElem_GetColor( game_hudelem_t *hud, int offset )
{
	vec3_t color;

	assert(g_hudelem_fields[offset].ofs == HEOFS( elem.color ));

	color[0] = hud->elem.color.split.r * (1.0f/255);
	color[1] = hud->elem.color.split.g * (1.0f/255);
	color[2] = hud->elem.color.split.b * (1.0f/255);

	Scr_AddVector(color);
}

/*
============
HudElem_SetLocalizedString
============
*/
void HudElem_SetLocalizedString( game_hudelem_t *hud, int offset )
{
	game_hudelem_field_t *f;
	const char *string;

	string = Scr_GetIString(0);
	f = &g_hudelem_fields[offset];

	*(int *)((byte *)hud + f->ofs) = G_LocalizedStringIndex(string);
}

/*
============
HudElem_GetEnumString
============
*/
static void HudElem_GetEnumString( game_hudelem_s *hud, const game_hudelem_field_t *f, const char **names, int nameCount )
{
	int index;

	assert(hud);
	assert(f);
	assert(names);
	assert(nameCount > 0);

	index = *(int *)((byte *)hud + f->ofs) >> f->shift;
	assert(index >= 0 && index < nameCount);

	Scr_AddString(names[index]);
}

/*
============
HECmd_SetClockUp
============
*/
void HECmd_SetClockUp( scr_entref_t entref )
{
	HECmd_SetClock_Internal(entref, HE_TYPE_CLOCK_UP, "setClockUp");
}

/*
============
HECmd_SetClock
============
*/
void HECmd_SetClock( scr_entref_t entref )
{
	HECmd_SetClock_Internal(entref, HE_TYPE_CLOCK_DOWN, "setClock");
}

/*
============
HudElem_GetVertAlign
============
*/
void HudElem_GetVertAlign( game_hudelem_t *hud, int offset )
{
	HudElem_GetEnumString(hud, &g_hudelem_fields[offset], g_he_vertalign, ARRAY_COUNT(g_he_vertalign));
}

/*
============
HudElem_GetHorzAlign
============
*/
void HudElem_GetHorzAlign( game_hudelem_t *hud, int offset )
{
	HudElem_GetEnumString(hud, &g_hudelem_fields[offset], g_he_horzalign, ARRAY_COUNT(g_he_horzalign));
}

/*
============
HudElem_GetAlignY
============
*/
void HudElem_GetAlignY( game_hudelem_t *hud, int offset )
{
	HudElem_GetEnumString(hud, &g_hudelem_fields[offset], g_he_aligny, ARRAY_COUNT(g_he_aligny));
}

/*
============
HudElem_GetAlignX
============
*/
void HudElem_GetAlignX( game_hudelem_t *hud, int offset )
{
	HudElem_GetEnumString(hud, &g_hudelem_fields[offset], g_he_alignx, ARRAY_COUNT(g_he_alignx));
}

/*
============
HudElem_GetFont
============
*/
void HudElem_GetFont( game_hudelem_t *hud, int offset )
{
	HudElem_GetEnumString(hud, &g_hudelem_fields[offset], g_he_font, ARRAY_COUNT(g_he_font));
}

/*
============
HECmd_SetMapNameString
============
*/
void HECmd_SetMapNameString( scr_entref_t entref )
{
	game_hudelem_t *hud;
	const char *string;

	hud = HECmd_GetHudElem(entref);
	string = Scr_GetString(0);

	if ( !string || !string[0] || !SV_MapExists(string) )
	{
		Com_Printf("Invalid mapname passed to hudelem setmapnamestring()\n");
		return;
	}

	SV_SetConfigstring(CS_VOTE_MAPNAME, string);
	HudElem_ClearTypeSettings(hud);
	hud->elem.type = HE_TYPE_MAPNAME;
}

/*
============
HECmd_SetGameTypeString
============
*/
void HECmd_SetGameTypeString( scr_entref_t entref )
{
	game_hudelem_t *hud;
	const char *string;

	hud = HECmd_GetHudElem(entref);
	string = Scr_GetString(0);

	if ( !string || !string[0] || !Scr_GetGameTypeNameForScript(string) )
	{
		Com_Printf("Invalid gametype passed to hudelem setgametypestring()\n");
		return;
	}

	SV_SetConfigstring(CS_VOTE_GAMETYPE, string);
	HudElem_ClearTypeSettings(hud);
	hud->elem.type = HE_TYPE_GAMETYPE;
	hud->elem.value = CS_VOTE_GAMETYPE;
}

/*
============
HECmd_SetPlayerNameString
============
*/
void HECmd_SetPlayerNameString( scr_entref_t entref )
{
	game_hudelem_t *hud;
	gentity_t *entity;

	hud = HECmd_GetHudElem(entref);
	entity = Scr_GetEntity(0);

	if ( !entity || !entity->client )
	{
		Com_Printf("Invalid entity passed to hudelem setplayernamestring()\n");
		return;
	}

	HudElem_ClearTypeSettings(hud);
	hud->elem.type = HE_TYPE_PLAYERNAME;
	hud->elem.value = entity->s.number;
}
