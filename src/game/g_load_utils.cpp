#include "../qcommon/qcommon.h"
#include "g_shared.h"

#ifdef TESTING_LIBRARY
#define level (*((level_locals_t*)( 0x0859B400 )))
#else
extern level_locals_t level;
#endif

unsigned int G_NewString(const char *string)
{
	size_t len;
	signed int i;
	char *chr;
	char str[16384];

	len = strlen(string) + 1;

	if ( len > 16384 )
		Com_Error(ERR_DROP, "G_NewString: len = %i > %i", len, 16384);

	chr = str;

	for ( i = 0; i < (int)len; ++i )
	{
		if ( string[i] == 92 && i < (int)(len - 1) )
		{
			if ( string[++i] == 110 )
				*chr = 10;
			else
				*chr = 92;

			++chr;
		}
		else
		{
			*chr++ = string[i];
		}
	}

	return SL_GetString(str, 0);
}

int G_SpawnStringInternal(const SpawnVar *spawnVar, const char *key, const char *defaultString, const char **out)
{
	int i;

	for ( i = 0; i < spawnVar->numSpawnVars; ++i )
	{
		if ( !strcmp(key, spawnVar->spawnVars[i].key) )
		{
			*out = spawnVar->spawnVars[i].value;
			return 1;
		}
	}

	*out = defaultString;
	return 0;
}

int G_SpawnString(const char *key, const char *defaultString, const char **out)
{
	return G_SpawnStringInternal(&level.spawnVars, key, defaultString, out);
}