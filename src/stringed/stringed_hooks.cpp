#include "../qcommon/qcommon.h"
#include "stringed_public.h"

dvar_t* loc_language;
dvar_t* loc_forceEnglish;
dvar_t* loc_translate;
dvar_t* loc_warnings;
dvar_t* loc_warningsAsErrors;

languageInfo_t g_languages[MAX_LANGUAGES] =
{
	{ "english", 0 },
	{ "french", 0 },
	{ "german", 0 },
	{ "italian", 0 },
	{ "spanish", 0 },
	{ "british", 0 },
	{ "russian", 0 },
	{ "polish", 0 },
	{ "korean", 0 },
	{ "taiwanese", 0 },
	{ "japanese", 0 },
	{ "chinese", 0 },
	{ "thai", 0 },
	{ "leet", 0 }
};

qboolean SEH_GetLanguageIndexForName(const char* language, int *langindex)
{
	int i;

	for(i = 0; i < MAX_LANGUAGES; i++)
	{
		if(!I_stricmp(language, g_languages[i].pszName))
		{
			*langindex = i;
			return qtrue;
		}
	}
	return qfalse;
}

const char *SEH_GetLanguageName(const int iLanguage)
{
	if (iLanguage >= 0 && iLanguage < MAX_LANGUAGES)
	{
		return g_languages[iLanguage].pszName;
	}

	return g_languages[0].pszName;
}

int SEH_GetCurrentLanguage()
{
	return loc_language->current.integer;
}

void SEH_UpdateCurrentLanguage(int langindex)
{
	Dvar_SetInt(loc_language, langindex);
}

void SEH_InitLanguage()
{
	loc_language = Dvar_RegisterInt("loc_language", 0, 0, MAX_LANGUAGES - 1, 0x1021u);
	loc_forceEnglish = Dvar_RegisterBool("loc_forceEnglish", 0, 0x1021u);
	loc_translate = Dvar_RegisterBool("loc_translate", 1, 0x1020u);
	loc_warnings = Dvar_RegisterBool("loc_warnings", 0, 0x1000u);
	loc_warningsAsErrors = Dvar_RegisterBool("loc_warningsAsErrors", 0, 0x1000u);
}