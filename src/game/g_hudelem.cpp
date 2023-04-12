#include "../qcommon/qcommon.h"
#include "g_shared.h"
#include "../clientscript/clientscript_public.h"

game_hudelem_t g_hudelems[1024];

game_hudelem_field_t g_hudelem_fields[] =
{
	{ "x", 4, 1, 0, 0, NULL, NULL },
	{ "y", 8, 1, 0, 0, NULL, NULL },
	{ "z", 12, 1, 0, 0, NULL, NULL },
	{ "fontscale", 16, 1, -1, 0, HudElem_SetFontScale, NULL },
	{ "font", 20, 0, -1, 0, HudElem_SetFont, HudElem_GetFont },
	{ "alignx", 24, 0, 3, 2, HudElem_SetAlignX, HudElem_GetAlignX },
	{ "aligny", 24, 0, 3, 0, HudElem_SetAlignY, HudElem_GetAlignY },
	{ "horzalign", 28, 0, 7, 3, HudElem_SetHorzAlign, HudElem_GetHorzAlign },
	{ "vertalign", 28, 0, 7, 0, HudElem_SetVertAlign, HudElem_GetVertAlign },
	{ "color", 32, 0, -1, 0, HudElem_SetColor, HudElem_GetColor },
	{ "alpha", 32, 0, -1, 0, HudElem_SetAlpha, HudElem_GetAlpha },
	{ "label", 48, 0, -1, 0, HudElem_SetLocalizedString, NULL },
	{ "sort", 120, 1, 0, 0, NULL, NULL },
	{ "foreground", 124, 0, -1, 0, HudElem_SetBoolean, NULL },
	{ "archived", 136, 0, -1, 0, HudElem_SetBoolean, NULL }
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

const char *g_he_aligny[] = { "top", "middle", "bottom" };
const char *g_he_alignx[] = { "left", "center", "right" };
const char *g_he_font[] =   { "default", "bigfixed", "smallfixed", };

game_hudelem_t* HECmd_GetHudElem(scr_entref_t entRef)
{
	if ( entRef.classnum == CLASS_NUM_HUDELEM )
		return &g_hudelems[entRef.entnum];

	Scr_ObjectError("not a hud element");
	return 0;
}

void HudElem_SetEnumString(game_hudelem_t *hud, const game_hudelem_field_t *f, const char **names, int nameCount)
{
	unsigned int *position;
	char buf[2048];
	const char *stringValue;
	int i;

	position = (unsigned int *)((byte *)hud + f->ofs);
	stringValue = Scr_GetString(0);

	for ( i = 0; i < nameCount; ++i )
	{
		if ( !I_stricmp(stringValue, names[i]) )
		{
			*position &= ~(f->size << f->shift);
			*position |= i << f->shift;
			return;
		}
	}

	sprintf(buf, "\"%s\" is not a valid value for hudelem field \"%s\"\nShould be one of:", stringValue, f->name);

	for ( i = 0; i < nameCount; ++i )
	{
		strncat(buf, va(" %s", names[i]), 2047);
		buf[2047] = 0;
	}

	Scr_Error(buf);
}

void HudElem_SetFontScale(game_hudelem_t *hud, int offset)
{
	float value;

	value = Scr_GetFloat(0);

	if ( value <= 0.0 )
	{
		Scr_Error(va("font scale was %g; should be > 0", value));
	}

	hud->elem.fontScale = value;
}

void HudElem_SetFont(game_hudelem_t *hud, int offset)
{
	HudElem_SetEnumString(hud, &g_hudelem_fields[offset], g_he_font, COUNT_OF(g_he_font));
}

void HudElem_SetAlignX(game_hudelem_t *hud, int offset)
{
	HudElem_SetEnumString(hud, &g_hudelem_fields[offset], g_he_alignx, COUNT_OF(g_he_alignx));
}

void HudElem_SetAlignY(game_hudelem_t *hud, int offset)
{
	HudElem_SetEnumString(hud, &g_hudelem_fields[offset], g_he_aligny, COUNT_OF(g_he_aligny));
}

void HudElem_SetHorzAlign(game_hudelem_t *hud, int offset)
{
	HudElem_SetEnumString(hud, &g_hudelem_fields[offset], g_he_horzalign, COUNT_OF(g_he_horzalign));
}

void HudElem_SetVertAlign(game_hudelem_t *hud, int offset)
{
	HudElem_SetEnumString(hud, &g_hudelem_fields[offset], g_he_vertalign, COUNT_OF(g_he_vertalign));
}

void HudElem_SetColor(game_hudelem_t *hud, int offset)
{
	vec3_t color;

	Scr_GetVector(0, color);

	hud->elem.color.split.r = (int)((float)(255.0 * I_fclamp(color[0], 0.0f, 1.0f)) + 9.313225746154785e-10);
	hud->elem.color.split.g = (int)((float)(255.0 * I_fclamp(color[1], 0.0f, 1.0f)) + 9.313225746154785e-10);
	hud->elem.color.split.b = (int)((float)(255.0 * I_fclamp(color[2], 0.0f, 1.0f)) + 9.313225746154785e-10);
}

void HudElem_SetAlpha(game_hudelem_t *hud, int offset)
{
	float value;

	value = Scr_GetFloat(0);

	hud->elem.color.split.a = (int)((float)(255.0 * I_fclamp(value, 0.0f, 1.0f)) + 9.313225746154785e-10);
}

void HudElem_SetLocalizedString(game_hudelem_t *hud, int offset)
{
	const char *string;

	string = Scr_GetIString(0);
	*(int *)((byte *)&hud->elem.type + g_hudelem_fields[offset].ofs) = G_LocalizedStringIndex(string);
}

void HudElem_SetBoolean(game_hudelem_t *hud, int offset)
{
	*(int *)((byte *)&hud->elem.type + g_hudelem_fields[offset].ofs) = Scr_GetInt(0);
}

void HudElem_AddString(game_hudelem_t *hud, const game_hudelem_field_t *field, const char **names)
{
	Scr_AddString(names[field->size & (*(int *)((byte *)&hud->elem.type + field->ofs) >> field->shift)]);
}

void HudElem_GetFont(game_hudelem_t *hud, int offset)
{
	HudElem_AddString(hud, &g_hudelem_fields[offset], g_he_font);
}

void HudElem_GetAlignX(game_hudelem_t *hud, int offset)
{
	HudElem_AddString(hud, &g_hudelem_fields[offset], g_he_alignx);
}

void HudElem_GetAlignY(game_hudelem_t *hud, int offset)
{
	HudElem_AddString(hud, &g_hudelem_fields[offset], g_he_aligny);
}

void HudElem_GetHorzAlign(game_hudelem_t *hud, int offset)
{
	HudElem_AddString(hud, &g_hudelem_fields[offset], g_he_horzalign);
}

void HudElem_GetVertAlign(game_hudelem_t *hud, int offset)
{
	HudElem_AddString(hud, &g_hudelem_fields[offset], g_he_vertalign);
}

void HudElem_GetColor(game_hudelem_t *hud, int offset)
{
	vec3_t color;

	color[0] = hud->elem.color.split.r * 0.0039215689;
	color[1] = hud->elem.color.split.g * 0.0039215689;
	color[2] = hud->elem.color.split.b * 0.0039215689;

	Scr_AddVector(color);
}

void HudElem_GetAlpha(game_hudelem_t *hud, int offset)
{
	float alpha;

	alpha = hud->elem.color.split.a * 0.0039215689;

	Scr_AddFloat(alpha);
}

void HudElem_ClearTypeSettings(game_hudelem_t *hud)
{
	hud->elem.width = 0;
	hud->elem.height = 0;
	hud->elem.materialIndex = 0;
	hud->elem.fromX = 0.0;
	hud->elem.fromY = 0.0;
	hud->elem.fromAlignOrg = 0;
	hud->elem.fromAlignScreen = 0;
	hud->elem.fromWidth = 0;
	hud->elem.fromHeight = 0;
	hud->elem.scaleStartTime = 0;
	hud->elem.scaleTime = 0;
	hud->elem.time = 0;
	hud->elem.duration = 0;
	hud->elem.value = 0.0;
	hud->elem.text = 0;
}

void HudElem_SetDefaults(game_hudelem_t *hud)
{
	hud->elem.type = 1;
	hud->elem.x = 0.0;
	hud->elem.y = 0.0;
	hud->elem.z = 0.0;
	hud->elem.fontScale = 1.0;
	hud->elem.font = 0;
	hud->elem.alignOrg = 0;
	hud->elem.alignScreen = 0;
	hud->elem.color.rgba = -1;
	hud->elem.fromColor.rgba = 0;
	hud->elem.fadeStartTime = 0;
	hud->elem.fadeTime = 0;
	hud->elem.label = 0;
	hud->elem.sort = 0.0;
	hud->elem.foreground.rgba = 0;
	hud->archived = 1;
	HudElem_ClearTypeSettings(hud);
}

void HECmd_SetText(scr_entref_t entRef)
{
	game_hudelem_t *hud;
	const char *string;

	hud = HECmd_GetHudElem(entRef);
	string = Scr_GetIString(0);
	HudElem_ClearTypeSettings(hud);
	hud->elem.type = HE_TYPE_TEXT;
	hud->elem.text = G_LocalizedStringIndex(string);
}

void HECmd_SetPlayerNameString(scr_entref_t entRef)
{
	game_hudelem_t *hud;
	gentity_t *entity;

	hud = HECmd_GetHudElem(entRef);
	entity = Scr_GetEntity(0);

	if ( entity )
	{
		if ( entity->client )
		{
			HudElem_ClearTypeSettings(hud);
			hud->elem.type = HE_TYPE_PLAYERNAME;
			hud->elem.value = (float)entity->s.number;
		}
		else
		{
			Com_Printf("Invalid entity passed to hudelem setplayernamestring(), entity is not a client\n");
		}
	}
	else
	{
		Com_Printf("Invalid entity passed to hudelem setplayernamestring()\n");
	}
}

void HECmd_SetGameTypeString(scr_entref_t entRef)
{
	game_hudelem_t *hud;
	const char *string;

	hud = HECmd_GetHudElem(entRef);
	string = Scr_GetString(0);

	if ( string )
	{
		if ( Scr_GetGameTypeNameForScript(string) )
		{
			SV_SetConfigstring(0x14, string);
			HudElem_ClearTypeSettings(hud);
			hud->elem.type = HE_TYPE_GAMETYPE;
			hud->elem.value = 20.0;
		}
		else
		{
			Com_Printf("Invalid gametype '%s'\n", string);
		}
	}
	else
	{
		Com_Printf("Invalid entity passed to hudelem setgametypestring()\n");
	}
}

void HECmd_SetMapNameString(scr_entref_t entRef)
{
	game_hudelem_t *hud;
	const char *string;

	hud = HECmd_GetHudElem(entRef);
	string = Scr_GetString(0);

	if ( string )
	{
		if ( SV_MapExists(string) )
		{
			SV_SetConfigstring(0x13, string);
			HudElem_ClearTypeSettings(hud);
			hud->elem.type = HE_TYPE_MAPNAME;
		}
		else
		{
			Com_Printf("Invalid map name passed to hudelem setmapnamestring(), map not found\n");
		}
	}
	else
	{
		Com_Printf("Invalid mapname passed to hudelem setmapnamestring()\n");
	}
}

void HECmd_SetShader(scr_entref_t entRef)
{
	const char *shader;
	int index;
	int height;
	int width;
	unsigned int paramNum;
	game_hudelem_t *hud;

	hud = HECmd_GetHudElem(entRef);
	paramNum = Scr_GetNumParam();

	if ( paramNum != 1 && paramNum != 3 )
		Scr_Error("USAGE: <hudelem> setShader(\"shadername\"[, optional_width, optional_height]);");

	shader = Scr_GetString(0);
	index = G_ShaderIndex(shader);

	if ( paramNum == 1 )
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
	hud->elem.materialIndex = index;
	hud->elem.width = width;
	hud->elem.height = height;
}

void HECmd_SetTimer_Internal(scr_entref_t entRef, he_type_t type, const char *funcName)
{
	float time;
	int timeInt;
	game_hudelem_t *hud;

	hud = HECmd_GetHudElem(entRef);

	if ( Scr_GetNumParam() != 1 )
	{
		Scr_Error(va("USAGE: <hudelem> %s(time_in_seconds);\n", funcName));
	}

	time = Scr_GetFloat(0) * 1000.0;
	timeInt = ceil(time);

	if ( timeInt <= 0 && type != HE_TYPE_TIMER_UP )
	{
		Scr_ParamError(0, va("time %g should be > 0", time));
	}

	HudElem_ClearTypeSettings(hud);
	hud->elem.type = type;
	hud->elem.time = level.time + timeInt;
}

void HECmd_SetClock_Internal(scr_entref_t entref, he_type_t type, const char *funcName)
{
	float time;
	int height;
	int width;
	int index;
	int duration;
	int timeInt;
	unsigned int paramNum;
	game_hudelem_t *hud;

	hud = HECmd_GetHudElem(entref);
	paramNum = Scr_GetNumParam();

	if ( paramNum != 3 && paramNum != 5 )
	{
		Scr_Error(va("USAGE: <hudelem> %s(time_in_seconds, total_clock_time_in_seconds, shadername[, width, height]);\n", funcName));
	}

	time = Scr_GetFloat(0) * 1000.0;
	timeInt = ceil(time);

	if ( timeInt <= 0 && type != HE_TYPE_CLOCK_UP )
	{
		Scr_ParamError(0, va("time %g should be > 0", time));
	}

	time = Scr_GetFloat(1) * 1000.0;
	duration = ceil(time);

	if ( duration <= 0 )
	{
		Scr_ParamError(1, va("duration %g should be > 0", time));
	}

	index = G_ShaderIndex(Scr_GetString(2));

	if ( paramNum == 3 )
	{
		width = 0;
		height = 0;
	}
	else
	{
		width = Scr_GetInt(3);

		if ( width < 0 )
		{
			Scr_ParamError(3,  va("width %i < 0", width));
		}

		height = Scr_GetInt(4);

		if ( height < 0 )
		{
			Scr_ParamError(4, va("height %i < 0", height));
		}
	}

	HudElem_ClearTypeSettings(hud);
	hud->elem.type = type;
	hud->elem.time = level.time + timeInt;
	hud->elem.duration = duration;
	hud->elem.materialIndex = index;
	hud->elem.width = width;
	hud->elem.height = height;
}

void HECmd_SetTimer(scr_entref_t entRef)
{
	HECmd_SetTimer_Internal(entRef, HE_TYPE_TIMER_DOWN, "setTimer");
}

void HECmd_SetTimerUp(scr_entref_t entRef)
{
	HECmd_SetTimer_Internal(entRef, HE_TYPE_TIMER_UP, "setTimerUp");
}

void HECmd_SetTenthsTimer(scr_entref_t entRef)
{
	HECmd_SetTimer_Internal(entRef, HE_TYPE_TENTHS_TIMER_DOWN, "setTenthsTimer");
}

void HECmd_SetTenthsTimerUp(scr_entref_t entRef)
{
	HECmd_SetTimer_Internal(entRef, HE_TYPE_TENTHS_TIMER_UP, "setTenthsTimerUp");
}

void HECmd_SetClock(scr_entref_t entRef)
{
	HECmd_SetClock_Internal(entRef, HE_TYPE_CLOCK_DOWN, "setClock");
}

void HECmd_SetClockUp(scr_entref_t entRef)
{
	HECmd_SetClock_Internal(entRef, HE_TYPE_CLOCK_UP, "setClockUp");
}

void HECmd_SetValue(scr_entref_t entRef)
{
	game_hudelem_t *hud;
	float value;

	hud = HECmd_GetHudElem(entRef);
	value = Scr_GetFloat(0);
	HudElem_ClearTypeSettings(hud);
	hud->elem.type = HE_TYPE_VALUE;
	hud->elem.value = value;
}

void HECmd_SetWaypoint(scr_entref_t entRef)
{
	int waypoint;
	game_hudelem_t *hud;

	hud = HECmd_GetHudElem(entRef);
	waypoint = Scr_GetInt(0);
	hud->elem.type = HE_TYPE_WAYPOINT;
	hud->elem.value = (float)waypoint;
}

void HECmd_FadeOverTime(scr_entref_t entRef)
{
	float value;
	float fadetime;
	game_hudelem_t *hud;

	hud = HECmd_GetHudElem(entRef);
	value = Scr_GetFloat(0);

	if ( value > 0.0 )
	{
		if ( value > 60.0 )
		{
			Scr_ParamError(0, va("fade time %g > 60", value));
		}
	}
	else
	{
		Scr_ParamError(0, va("fade time %g <= 0", value));
	}

	hud->elem.fadeStartTime = level.time;
	fadetime = value * 1000.0;
	hud->elem.fadeTime = Q_rint(fadetime);
	hud->elem.fromColor.rgba = hud->elem.color.rgba;
}

void HECmd_ScaleOverTime(scr_entref_t entRef)
{
	float scaletime;
	int height;
	int width;
	float value;
	game_hudelem_t *hud;

	hud = HECmd_GetHudElem(entRef);

	if ( Scr_GetNumParam() != 3 )
		Scr_Error("hudelem scaleOverTime(time_in_seconds, new_width, new_height)");

	value = Scr_GetFloat(0);

	if ( value > 0.0 )
	{
		if ( value > 60.0 )
		{
			Scr_ParamError(0, va("scale time %g > 60", value));
		}
	}
	else
	{
		Scr_ParamError(0, va("scale time %g <= 0", value));
	}

	width = Scr_GetInt(1);
	height = Scr_GetInt(2);
	hud->elem.scaleStartTime = level.time;
	scaletime = value * 1000.0;
	hud->elem.scaleTime = Q_rint(scaletime);
	hud->elem.fromWidth = hud->elem.width;
	hud->elem.fromHeight = hud->elem.height;
	hud->elem.width = width;
	hud->elem.height = height;
}

void HECmd_MoveOverTime(scr_entref_t entRef)
{
	float movetime;
	float value;
	game_hudelem_t *hud;

	hud = HECmd_GetHudElem(entRef);
	value = Scr_GetFloat(0);

	if ( value > 0.0 )
	{
		if ( value > 60.0 )
		{
			Scr_ParamError(0, va("move time %g > 60", value));
		}
	}
	else
	{
		Scr_ParamError(0, va("move time %g <= 0", value));
	}

	hud->elem.moveStartTime = level.time;
	movetime = value * 1000.0;
	hud->elem.moveTime = Q_rint(movetime);
	hud->elem.fromX = hud->elem.x;
	hud->elem.fromY = hud->elem.y;
	hud->elem.fromAlignOrg = hud->elem.alignOrg;
	hud->elem.fromAlignScreen = hud->elem.alignScreen;
}

void HECmd_Reset(scr_entref_t entRef)
{
	game_hudelem_t *hud;

	hud = HECmd_GetHudElem(entRef);
	HudElem_SetDefaults(hud);
}

void HECmd_Destroy(scr_entref_t entRef)
{
	game_hudelem_t *hud;

	hud = HECmd_GetHudElem(entRef);
	HudElem_Free(hud);
}

void (*HudElem_GetMethod(const char **pName))(scr_entref_t)
{
	const char *name;
	unsigned int i;

	name = *pName;

	for ( i = 0; i < COUNT_OF(g_he_methods); ++i )
	{
		if ( !strcmp(name, g_he_methods[i].name) )
		{
			*pName = g_he_methods[i].name;
			return g_he_methods[i].call;
		}
	}

	return NULL;
}

void HudElem_UpdateClient(gclient_s *client, int clientNum, byte which)
{
	game_hudelem_t *hud;
	hudelem_s *clienthud;
	int current;
	int archived;
	unsigned int i;

	if ( (which & HUDELEM_UPDATE_ARCHIVAL) != 0 )
		memset(client->ps.hud.archival, 0, sizeof(client->ps.hud.archival));

	if ( (which & HUDELEM_UPDATE_CURRENT) != 0 )
		memset(&client->ps.hud, 0, sizeof(client->ps.hud.current));

	archived = 0;
	current = 0;

	hud = g_hudelems;

	for ( i = 0; i < 1024; ++i )
	{
		if ( hud->elem.type
		        && (!hud->team || hud->team == client->sess.state.team)
		        && (hud->clientNum == 1023 || hud->clientNum == clientNum) )
		{
			if ( hud->archived )
			{
				if ( ((which ^ HUDELEM_UPDATE_ARCHIVAL) & HUDELEM_UPDATE_ARCHIVAL) == 0 )
				{
					clienthud = &client->ps.hud.archival[archived++];

					if ( archived <= MAX_HUDELEMS_ARCHIVAL )
						memcpy(clienthud, hud, sizeof(hudelem_s));
				}
			}
			else if ( (which & HUDELEM_UPDATE_CURRENT) != 0 )
			{
				clienthud = &client->ps.hud.current[current++];

				if ( current <= MAX_HUDELEMS_CURRENT )
					memcpy(clienthud, hud, sizeof(hudelem_s));
			}
		}

		++hud;
	}
}

void Scr_GetHudElemField(int entnum, int offset)
{
	game_hudelem_t *hud;
	game_hudelem_field_t *field;

	field = &g_hudelem_fields[offset];
	hud = &g_hudelems[entnum];

	if ( field->getter )
		field->getter(hud, offset);
	else
		Scr_GetGenericField((byte *)hud, field->type, field->ofs);
}

void Scr_SetHudElemField(int entnum, int offset)
{
	game_hudelem_t *hud;
	game_hudelem_field_t *field;

	field = &g_hudelem_fields[offset];
	hud = &g_hudelems[entnum];

	if ( field->setter )
		field->setter(hud, offset);
	else
		Scr_SetGenericField((byte *)hud, field->type, field->ofs);
}

void GScr_AddFieldsForHudElems()
{
	game_hudelem_field_t *i;

	for ( i = g_hudelem_fields; i->name; ++i )
		Scr_AddClassField(1, i->name, i - g_hudelem_fields);
}

void Scr_FreeHudElemConstStrings(game_hudelem_t *hud)
{
	game_hudelem_field_t *i;

	for ( i = g_hudelem_fields; i->name; ++i )
	{
		if ( i->type == 3 )
			Scr_SetString((uint16_t *)(hud + i->ofs), 0);
	}
}

void HudElem_Free(game_hudelem_t *hud)
{
	Scr_FreeHudElem(hud);
	hud->elem.type = 0;
}

void HudElem_DestroyAll()
{
	unsigned int i;

	for ( i = 0; i < sizeof(g_hudelems) / sizeof(g_hudelems[0]); ++i )
	{
		if ( g_hudelems[i].elem.type != HE_TYPE_FREE )
			HudElem_Free(&g_hudelems[i]);
	}

	memset(g_hudelems, 0, sizeof(g_hudelems));
}

void HudElem_ClientDisconnect(gentity_t *ent)
{
	unsigned int i;

	for ( i = 0; i < sizeof(g_hudelems) / sizeof(g_hudelems[0]); ++i )
	{
		if ( g_hudelems[i].elem.type != HE_TYPE_FREE )
		{
			if ( g_hudelems[i].clientNum == ent->s.number )
				HudElem_Free(&g_hudelems[i]);
		}
	}
}

game_hudelem_t *HudElem_Alloc(int clientNum, int teamNum)
{
	unsigned int i;

	for ( i = 0; i < sizeof(g_hudelems) / sizeof(g_hudelems[0]); ++i )
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

void GScr_NewTeamHudElem()
{
	const char *team;
	unsigned short teamName;
	game_hudelem_s *hud;

	teamName = Scr_GetConstString(0);

	if ( teamName == scr_const.allies )
	{
		hud = HudElem_Alloc(1023, 2);
	}
	else if ( teamName == scr_const.axis )
	{
		hud = HudElem_Alloc(1023, 1);
	}
	else if ( teamName == scr_const.spectator )
	{
		hud = HudElem_Alloc(1023, 3);
	}
	else
	{
		team = Scr_GetString(0);
		Scr_ParamError(0, va("team \"%s\" should be \"allies\", \"axis\", or \"spectator\"", team));
		hud = HudElem_Alloc(1023, 0);
	}

	if ( !hud )
		Scr_Error("out of hudelems");

	Scr_AddHudElem(hud);
}

void GScr_NewClientHudElem()
{
	gentity_s *ent;
	game_hudelem_s *hud;

	ent = Scr_GetEntity(0);

	if ( !ent->client )
		Scr_ParamError(0, "not a client");

	hud = HudElem_Alloc(ent->s.number, 0);

	if ( !hud )
		Scr_Error("out of hudelems");

	Scr_AddHudElem(hud);
}

void GScr_NewHudElem()
{
	game_hudelem_t *hud;

	hud = HudElem_Alloc(1023, 0);

	if ( !hud )
		Scr_Error("out of hudelems");

	Scr_AddHudElem(hud);
}