#include "../qcommon/qcommon.h"
#include "stringed_public.h"

dvar_t* loc_language;
dvar_t* loc_forceEnglish;
dvar_t* loc_translate;
dvar_t* loc_warnings;
dvar_t* loc_warningsAsErrors;

int g_currentAsian;

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

/*
==============
Language_IsAsian
==============
*/
inline int Language_IsAsian()
{
	return g_currentAsian;
}

/*
==============
Language_UpdateCurrentAsian
==============
*/
void Language_UpdateCurrentAsian()
{
	g_currentAsian = loc_language->current.integer <= 4;
}

/*
==============
SEH_GetCurrentLanguage
==============
*/
int SEH_GetCurrentLanguage()
{
	return loc_language->current.integer;
}

/*
==============
SEH_InitLanguage
==============
*/
void SEH_InitLanguage()
{
	loc_language = Dvar_RegisterInt("loc_language", 0, 0, MAX_LANGUAGES, 0x1021u);
	loc_forceEnglish = Dvar_RegisterBool("loc_forceEnglish", 0, 4129);
	loc_translate = Dvar_RegisterBool("loc_translate", 1, 4128);
	loc_warnings = Dvar_RegisterBool("loc_warnings", 0, 4096);
	loc_warningsAsErrors = Dvar_RegisterBool("loc_warningsAsErrors", 0, 4096);
	Language_UpdateCurrentAsian();
}

/*
==============
SEH_VerifyLanguageSelection
==============
*/
unsigned int SEH_VerifyLanguageSelection(int iLanguageSelection)
{
	int i;

	if (g_languages[iLanguageSelection].bPresent)
	{
		return iLanguageSelection;
	}
	for (i = 0; i < MAX_LANGUAGES; ++i)
	{
		if (g_languages[(i + iLanguageSelection) % MAX_LANGUAGES].bPresent)
		{
			return (i + iLanguageSelection) % MAX_LANGUAGES;
		}
	}
	return 0;
}

/*
==============
SEH_Init_StringEd
==============
*/
void SEH_Init_StringEd()
{
	SE_Init();
}

/*
==============
SEH_Shutdown_StringEd
==============
*/
void SEH_Shutdown_StringEd()
{
	SE_ShutDown();
}

/*
==============
SE_GetString
==============
*/
inline const char* SE_GetString(const char* psPackageAndStringReference)
{
	return SE_GetString_LoadObj(psPackageAndStringReference)->c_str();
}

/*
==============
SEH_StringEd_GetString
==============
*/
const char *SEH_StringEd_GetString(const char *pszReference)
{
	if (!loc_translate || !loc_translate->current.boolean)
	{
		return pszReference;
	}
	if (*pszReference && (!*pszReference || pszReference[1]))
	{
		return SE_GetString(pszReference);
	}
	return pszReference;
}

/*
==============
SEH_SafeTranslateString
==============
*/
const char *SEH_SafeTranslateString(const char *pszReference)
{
	const char* pszTranslation;
	static char szErrorString[MAX_STRING_CHARS];

	pszTranslation = SEH_StringEd_GetString(pszReference);
	if (!pszTranslation)
	{
		if (loc_warnings->current.boolean)
		{
			if (loc_warningsAsErrors->current.boolean)
			{
				Com_Error(ERR_LOCALIZATION, "Could not translate exe string \"%s\"", pszReference);
			}
			else
			{
				Com_Printf("WARNING: Could not translate exe string \"%s\"\n", pszReference);
			}

			strcpy(szErrorString, "^1UNLOCALIZED(^7");
			I_strncat(szErrorString, sizeof(szErrorString), pszReference);
			I_strncat(szErrorString, sizeof(szErrorString), "^1)^7");
		}
		else
		{
			I_strncpyz(szErrorString, pszReference, sizeof(szErrorString));
		}
		pszTranslation = szErrorString;
	}

	return pszTranslation;
}

/*
==============
SEH_GetLocalizedTokenReference
==============
*/
int SEH_GetLocalizedTokenReference(char *token, const char *reference, const char *messageType, msgLocErrType_t errType)
{
	const char* translation;

	translation = SEH_StringEd_GetString(reference);
	if (!translation)
	{
		if (loc_warnings && loc_warnings->current.boolean)
		{
			if (loc_warningsAsErrors && loc_warningsAsErrors->current.boolean && errType != 1)
			{
				Com_Error(ERR_LOCALIZATION, "Could not translate part of %s: \"%s\"", messageType, reference);
			}
			else
			{
				Com_Printf("WARNING: Could not translate part of %s: \"%s\"\n", messageType, reference);
			}
			translation = va("^1UNLOCALIZED(^7%s^1)^7", reference);
		}
		else
		{
			translation = va("%s", reference);
		}

		if (errType == 1)
		{
			return 0;
		}
	}

	I_strncpyz(token, translation, MAX_STRING_CHARS);

	return 1;
}

/*
==============
SEH_LocalizeTextMessage
==============
*/
const char *SEH_LocalizeTextMessage(const char *pszInputBuffer, const char *pszMessageType, msgLocErrType_t errType)
{
	char szInsertBuf[MAX_STRING_CHARS];
	char szTokenBuf[MAX_STRING_CHARS];
	int bLocOn;
	int iTokenLen;
	int iInsertLevel;
	int iLen;
	int bInsertEnabled;
	int insertIndex;
	const char* pszIn;
	int bLocSkipped;
	const char* pszTokenStart;
	int i;
	char* pszString;
	int digit;
	static int iCurrString;
	static char szStrings[10][MAX_STRING_CHARS];

	iCurrString = (iCurrString + 1) % 10;
	memset(szStrings[iCurrString], 0, sizeof(szStrings[iCurrString]));
	pszString = szStrings[iCurrString];
	iLen = 0;
	bLocOn = 1;
	bInsertEnabled = 1;
	iInsertLevel = 0;
	insertIndex = 1;
	bLocSkipped = 0;
	pszTokenStart = pszInputBuffer;
	pszIn = pszInputBuffer;
	while (*pszTokenStart)
	{
		if (!*pszIn || *pszIn == 20 || *pszIn == 21 || *pszIn == 22)
		{
			if (pszIn > pszTokenStart)
			{
				iTokenLen = pszIn - pszTokenStart;
				I_strncpyz(szTokenBuf, pszTokenStart, pszIn - pszTokenStart + 1);
				if (bLocOn)
				{
					if (!SEH_GetLocalizedTokenReference(szTokenBuf, szTokenBuf, pszMessageType, errType))
					{
						return 0;
					}
					iTokenLen = strlen(szTokenBuf);
				}

				if (iTokenLen + iLen >= MAX_STRING_CHARS)
				{
					if (loc_warnings
					        && loc_warnings->current.boolean
					        && loc_warningsAsErrors
					        && loc_warningsAsErrors->current.boolean
					        && errType != 1)
					{
						Com_Error(ERR_DROP, "%s too long when translated: \"%s\"", pszMessageType, pszInputBuffer);
					}
					Com_Printf("%s too long when translated: \"%s\"\n", pszMessageType,
					           pszInputBuffer);
				}

				for (i = 0; i < iTokenLen - 2; ++i)
				{
					if (!strncmp(&szTokenBuf[i], "&&", 2) && isdigit(szTokenBuf[i + 2]))
					{
						if (bInsertEnabled)
						{
							++iInsertLevel;
						}
						else
						{
							szTokenBuf[i] = 22;
							bLocSkipped = 1;
						}
					}
				}

				if (iInsertLevel <= 0 || iLen <= 0)
				{
					strcpy(&pszString[iLen], szTokenBuf);
				}
				else
				{
					for (i = 0; i < iLen - 2; ++i)
					{
						if (!strncmp(&pszString[i], "&&", 2) && isdigit(pszString[i + 2]))
						{
							digit = pszString[i + 2] - 48;
							if (!digit)
							{
								Com_Printf("%s cannot have &&0 as conversion format: \"%s\"\n",
								           pszMessageType, pszInputBuffer);
							}
							if (digit == insertIndex)
							{
								strcpy(szInsertBuf, &pszString[i + 3]);
								pszString[i] = 0;
								++insertIndex;
								break;
							}
						}
					}

					strcpy(&pszString[i], szTokenBuf);
					strcpy(&pszString[iTokenLen + i], szInsertBuf);
					iLen -= 3;
					--iInsertLevel;
				}
				iLen += iTokenLen;
			}

			bInsertEnabled = 1;
			if (*pszIn == 20)
			{
				bLocOn = 1;
				++pszIn;
			}
			else if (*pszIn == 21)
			{
				bLocOn = 0;
				++pszIn;
			}

			if (*pszIn == 22)
			{
				bInsertEnabled = 0;
				++pszIn;
			}
			pszTokenStart = pszIn;
		}
		else
		{
			++pszIn;
		}
	}

	if (bLocSkipped)
	{
		for (i = 0; i < iLen; ++i)
		{
			if (pszString[i] == 22)
			{
				pszString[i] = 37;
			}
		}
	}

	return pszString;
}

/*
==============
SEH_DecodeLetter
==============
*/
int SEH_DecodeLetter(unsigned char firstChar, unsigned char secondChar, unsigned char thirdChar, int *usedCount)
{
	if ((firstChar & 0x80u) != 0)
	{
		if ((firstChar & 0xE0) == 0xC0)
		{
			*usedCount = 2;
			return ((firstChar & 0x1F) << 6) | secondChar & 0x3F;
		}
		else if ((firstChar & 0xF0) == 0xE0)
		{
			*usedCount = 3;
			return thirdChar & 0x3F | ((((firstChar & 0xF) << 6) | secondChar & 0x3F) << 6);
		}
		else
		{
			*usedCount = 1;
			return firstChar;
		}
	}
	else
	{
		*usedCount = 1;
		return firstChar;
	}
}

/*
==============
SEH_ReadCharFromString
==============
*/
int SEH_ReadCharFromString(const char **text)
{
	unsigned int letter;

	letter = SEH_DecodeLetter(**text, (*text)[1], (*text)[2], (int*)&text);
	*text += (int)&text;
	return letter;
}

/*
==============
SEH_PrintStrlen
==============
*/
int SEH_PrintStrlen(const char *string)
{
	unsigned int c;
	int len;
	const char* p;

	if (!string)
	{
		return 0;
	}
	len = 0;
	p = string;
	while (*p)
	{
		c = SEH_ReadCharFromString(&p);
		if (c == 94 && p && *p != 94 && *p >= 48 && *p <= 64)
		{
			++p;
		}
		else if (c != 10 && c != 13)
		{
			++len;
		}
	}
	return len;
}

/*
==============
SEH_GetLanguageName
==============
*/
const char *SEH_GetLanguageName(const int iLanguage)
{
	if (iLanguage >= 0 && iLanguage < MAX_LANGUAGES)
	{
		return g_languages[iLanguage].pszName;
	}

	return g_languages[0].pszName;
}

/*
==============
SEH_StringEd_SetLanguageStrings
==============
*/
int SEH_StringEd_SetLanguageStrings(int iLanguage)
{
	const char* pszError;

	if (!g_languages[iLanguage].bPresent)
	{
		return 0;
	}

	pszError = SE_LoadLanguage(loc_forceEnglish->current.boolean);

	if (!pszError)
	{
		return 1;
	}

	if (!fs_ignoreLocalized->current.boolean && loc_warnings->current.boolean)
	{
		if (loc_warningsAsErrors->current.boolean)
		{
			Com_Error(ERR_LOCALIZATION, "Could not load localization strings for %s: %s", SEH_GetLanguageName(iLanguage), pszError);
		}
		else
		{
			Com_Printf("WARNING: Could not load localization strings for %s: %s\n", SEH_GetLanguageName(iLanguage), pszError);
		}
	}

	return 1;
}

/*
==============
SEH_UpdateLanguageInfo
==============
*/
void SEH_UpdateLanguageInfo()
{
	int lang;
	int i, iNumLanguages;

	Language_UpdateCurrentAsian();
	lang = loc_language->current.integer;
	if (g_languages[lang].bPresent = 1, !SEH_StringEd_SetLanguageStrings(lang))
	{
		iNumLanguages = 0;
		for (i = 0; i < MAX_LANGUAGES; ++i)
		{
			if (FS_LanguageHasAssets(i))
			{
				g_languages[i].bPresent = 1;
				++iNumLanguages;
			}
			else
			{
				g_languages[i].bPresent = 0;
			}
		}
		if (!iNumLanguages)
			Com_Printf("ERROR: No languages available because no localized assets were found\n");
		if (!SEH_StringEd_SetLanguageStrings(loc_language->current.integer))
		{
			for (i = 0; i < MAX_LANGUAGES; ++i)
			{
				Dvar_SetInt(loc_language, i);
				Language_UpdateCurrentAsian();
				if (SEH_StringEd_SetLanguageStrings(i))
				{
					return;
				}
			}
			Dvar_SetInt(loc_language, 0);
			Language_UpdateCurrentAsian();
		}
	}
}

/*
==============
SEH_GetLanguageIndexForName
==============
*/
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