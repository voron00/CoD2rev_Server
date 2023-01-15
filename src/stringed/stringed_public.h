#pragma once

#include <string>

enum msgLocErrType_t
{
	LOCMSG_SAFE = 0x0,
	LOCMSG_NOERR = 0x1,
};

enum
{
	LANGUAGE_ENGLISH = 0x0,
	LANGUAGE_FRENCH = 0x1,
	LANGUAGE_GERMAN = 0x2,
	LANGUAGE_ITALIAN = 0x3,
	LANGUAGE_SPANISH = 0x4,
	LANGUAGE_BRITISH = 0x5,
	LANGUAGE_RUSSIAN = 0x6,
	LANGUAGE_POLISH = 0x7,
	LANGUAGE_KOREAN = 0x8,
	LANGUAGE_TAIWANESE = 0x9,
	LANGUAGE_JAPANESE = 0xA,
	LANGUAGE_CHINESE = 0xB,
	LANGUAGE_THAI = 0xC,
	LANGUAGE_LEET = 0xD,
	MAX_LANGUAGES = 0xE,
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

std::string *SE_GetString_LoadObj(const char *psPackageAndStringReference);
char *SE_LoadLanguage(bool forceEnglish);
void SE_NewLanguage();
void SE_Init();
void SE_ShutDown();