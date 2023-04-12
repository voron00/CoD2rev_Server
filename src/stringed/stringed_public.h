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

int SEH_GetCurrentLanguage();
void SEH_InitLanguage();
unsigned int SEH_VerifyLanguageSelection(int iLanguageSelection);
void SEH_Init_StringEd();
void SEH_Shutdown_StringEd();
const char *SEH_StringEd_GetString(const char *pszReference);
const char *SEH_SafeTranslateString(const char *pszReference);
int SEH_GetLocalizedTokenReference(char *token, const char *reference, const char *messageType, msgLocErrType_t errType);
const char *SEH_LocalizeTextMessage(const char *pszInputBuffer, const char *pszMessageType, msgLocErrType_t errType);
int SEH_ReadCharFromString(const char **text);
int SEH_PrintStrlen(const char *string);
const char *SEH_GetLanguageName(const int iLanguage);
int SEH_StringEd_SetLanguageStrings(int iLanguage);
void SEH_UpdateLanguageInfo();
qboolean SEH_GetLanguageIndexForName(const char* language, int *langindex);
void SEH_UpdateCurrentLanguage(int langindex);

std::string *SE_GetString_LoadObj(const char *psPackageAndStringReference);
char *SE_LoadLanguage(bool forceEnglish);
void SE_NewLanguage();
void SE_Init();
void SE_ShutDown();