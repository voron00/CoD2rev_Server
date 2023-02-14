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

void HudElem_SetEnumString(game_hudelem_t *hud, const game_hudelem_field_t *f, const char **names, int nameCount)
{
	unsigned int *offset;
	char buf[2048];
	const char *stringValue;
	int i;

	offset = (unsigned int *)(hud + f->constId);
	stringValue = Scr_GetString(0);

	for ( i = 0; i < nameCount; ++i )
	{
		if ( !I_stricmp(stringValue, names[i]) )
		{
			*offset &= ~(f->size << f->type);
			*offset |= i << f->type;
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
	*(int *)(&hud + g_hudelem_fields[offset].constId) = G_LocalizedStringIndex(string);
}

void HudElem_SetBoolean(game_hudelem_t *hud, int offset)
{
	*(int *)(&hud + g_hudelem_fields[offset].constId) = Scr_GetInt(0);
}

void HudElem_AddString(game_hudelem_t *hud, const game_hudelem_field_t *field, const char **names)
{
	Scr_AddString(names[field->size & (*(int *)(&hud + field->constId) >> field->type)]);
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

void Scr_GetHudElemField(int entnum, int offset)
{
	game_hudelem_s *hud;
	game_hudelem_field_t *field;

	field = &g_hudelem_fields[offset];
	hud = &g_hudelems[entnum];

	if ( field->getter )
		field->getter(hud, offset);
	else
		Scr_GetGenericField((byte *)hud, field->ofs, field->constId);
}

void Scr_SetHudElemField(int entnum, int offset)
{
	game_hudelem_s *hud;
	game_hudelem_field_t *field;

	field = &g_hudelem_fields[offset];
	hud = &g_hudelems[entnum];

	if ( field->setter )
		field->setter(hud, offset);
	else
		Scr_SetGenericField((byte *)hud, field->ofs, field->constId);
}

void Scr_AddHudElem(game_hudelem_t *hud)
{
	Scr_AddEntityNum(hud - g_hudelems, 1);
}

void Scr_FreeHudElemConstStrings(game_hudelem_s *hud)
{
	game_hudelem_field_t *i;

	for ( i = g_hudelem_fields; i->name; ++i )
	{
		if ( i->ofs == 3 )
			Scr_SetString((uint16_t *)(hud + i->constId), 0);
	}
}

void Scr_FreeHudElem(game_hudelem_s *hud)
{
	Scr_FreeHudElemConstStrings(hud);
	Scr_FreeEntityNum(hud - g_hudelems, 1);
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

void GScr_NewHudElem()
{
	game_hudelem_t *hud;

	hud = HudElem_Alloc(1023, 0);

	if ( !hud )
		Scr_Error("out of hudelems");

	Scr_AddHudElem(hud);
}