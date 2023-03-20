#include "../qcommon/qcommon.h"
#include "g_shared.h"

#ifdef TESTING_LIBRARY
#define player_methods (((scr_method_t*)( 0x081518E0 )))
#else
scr_method_t player_methods[] =
{

};
#endif

void (*Player_GetMethod(const char **pName))(scr_entref_t)
{
	const char *name;
	unsigned int i;

	name = *pName;

#ifdef TESTING_LIBRARY
	for ( i = 0; i < 59; ++i )
#else
	for ( i = 0; i < COUNT_OF(player_methods); ++i )
#endif
	{
		if ( !strcmp(name, player_methods[i].name) )
		{
			*pName = player_methods[i].name;
			return player_methods[i].call;
		}
	}

	return NULL;
}