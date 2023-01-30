#include "../qcommon/qcommon.h"
#include "g_shared.h"
#include "../clientscript/clientscript_public.h"

#ifdef TESTING_LIBRARY
#define level (*((level_locals_t*)( 0x0859B400 )))
#else
extern level_locals_t level;
#endif

const char* G_ModelName(int modelIndex)
{
	return SV_GetConfigstringConst(modelIndex + 334);
}

int G_FindConfigstringIndex(const char *name, int start, int max, int create, const char *errormsg)
{
	const char *s1;
	int i;

	if ( !name || !*name )
		return 0;

	for ( i = 1; i < max; ++i )
	{
		s1 = SV_GetConfigstringConst(start + i);

		if ( !*s1 )
			break;

		if ( !strcasecmp(s1, name) )
			return i;
	}

	if ( create )
	{
		if ( i == max )
		{
			Com_Error(ERR_DROP, va("G_FindConfigstringIndex: overflow (%d): %s", start, name));
		}

		SV_SetConfigstring(start + i, name);
		return i;
	}
	else
	{
		if ( errormsg )
		{
			Scr_Error(va("%s \"%s\" not precached", errormsg, name));
		}

		return 0;
	}
}

int G_LocalizedStringIndex(const char *string)
{
	if ( *string )
		return G_FindConfigstringIndex(string, 1310, 256, level.initializing, "localized string");
	else
		return 0;
}