#pragma once

#include <string>

enum msgLocErrType_t
{
	LOCMSG_SAFE,
	LOCMSG_NOERR,
};

enum
{
	LANGUAGE_ENGLISH,
	LANGUAGE_FRENCH,
	LANGUAGE_GERMAN,
	LANGUAGE_ITALIAN,
	LANGUAGE_SPANISH,
	LANGUAGE_BRITISH,
	LANGUAGE_RUSSIAN,
	LANGUAGE_POLISH,
	LANGUAGE_KOREAN,
	LANGUAGE_TAIWANESE,
	LANGUAGE_JAPANESE,
	LANGUAGE_CHINESE,
	LANGUAGE_THAI,
	LANGUAGE_LEET,
	MAX_LANGUAGES,
};

struct languageInfo_t
{
	const char *pszName;
	int bPresent;
};

qboolean SEH_GetLanguageIndexForName(const char* language, int *langindex);
const char *SEH_GetLanguageName(const int iLanguage);
int SEH_GetCurrentLanguage();
void SEH_UpdateCurrentLanguage(int langindex);
void SEH_InitLanguage();