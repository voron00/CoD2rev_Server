#include "../qcommon/qcommon.h"
#include "g_shared.h"

#ifdef TESTING_LIBRARY
#define scriptent_methods (((scr_method_t*)( 0x08157300 )))
#else
scr_method_t scriptent_methods[] =
{

};
#endif

void (*ScriptEnt_GetMethod(const char **pName))(scr_entref_t)
{
	const char *name;
	unsigned int i;

	name = *pName;

#ifdef TESTING_LIBRARY
	for ( i = 0; i < 12; ++i )
#else
	for ( i = 0; i < COUNT_OF(scriptent_methods); ++i )
#endif
	{
		if ( !strcmp(name, scriptent_methods[i].name) )
		{
			*pName = scriptent_methods[i].name;
			return scriptent_methods[i].call;
		}
	}

	return NULL;
}