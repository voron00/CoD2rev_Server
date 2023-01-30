#include "../qcommon/qcommon.h"
#include "g_shared.h"
#include "../clientscript/clientscript_public.h"

#ifdef TESTING_LIBRARY
#define g_entities ((gentity_t*)( 0x08665480 ))
#else
extern gentity_t g_entities[];
#endif

gentity_t* Scr_GetEntity(unsigned int index)
{
	scr_entref_t entRef;

	Scr_GetEntityRef(&entRef, index);

	if ( entRef.classnum == 0 )
		return &g_entities[entRef.entnum];

	Scr_ParamError(index, "not an entity");
	return 0;
}

void Scr_AddEntity(gentity_t *ent)
{
	Scr_AddEntityNum(ent->s.number, 0);
}