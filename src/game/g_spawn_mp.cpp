#include "../qcommon/qcommon.h"
#include "g_shared.h"
#include "../clientscript/clientscript_public.h"

#ifdef TESTING_LIBRARY
#define g_entities ((gentity_t*)( 0x08665480 ))
#else
extern gentity_t g_entities[];
#endif

extern game_hudelem_t g_hudelems[];

gentity_t* Scr_EntityForRef(scr_entref_t entRef)
{
	if ( entRef.classnum == CLASS_NUM_ENTITY )
		return &g_entities[entRef.entnum];

	Scr_ObjectError("not an entity");
	return 0;
}

game_hudelem_t* Scr_HudElemForRef(scr_entref_t entRef)
{
	if ( entRef.classnum == CLASS_NUM_HUDELEM )
		return &g_hudelems[entRef.entnum];

	Scr_ObjectError("not a hud element");
	return 0;
}

gentity_t* Scr_GetEntity(unsigned int index)
{
	scr_entref_t entRef;

	Scr_GetEntityRef(&entRef, index);

	if ( entRef.classnum == CLASS_NUM_ENTITY )
		return &g_entities[entRef.entnum];

	Scr_ParamError(index, "not an entity");
	return 0;
}

void Scr_AddEntity(gentity_t *ent)
{
	Scr_AddEntityNum(ent->s.number, CLASS_NUM_ENTITY);
}

void Scr_AddHudElem(game_hudelem_t *hud)
{
	Scr_AddEntityNum(hud - g_hudelems, CLASS_NUM_HUDELEM);
}

void Scr_FreeHudElem(game_hudelem_t *hud)
{
	Scr_FreeHudElemConstStrings(hud);
	Scr_FreeEntityNum(hud - g_hudelems, CLASS_NUM_HUDELEM);
}