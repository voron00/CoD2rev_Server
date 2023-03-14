#include "../qcommon/qcommon.h"
#include "g_shared.h"
#include "../clientscript/clientscript_public.h"

#ifdef TESTING_LIBRARY
#define g_entities ((gentity_t*)( 0x08665480 ))
#else
extern gentity_t g_entities[];
#endif

extern game_hudelem_t g_hudelems[];

game_entity_field_t g_entity_fields[] =
{
	{ "classname", 360, 3, Scr_ReadOnlyField },
	{ "origin", 312, 4, Scr_SetOrigin },
	{ "model", 356, 8, Scr_ReadOnlyField },
	{ "spawnflags", 368, 0, Scr_ReadOnlyField },
	{ "target", 362, 3, NULL },
	{ "targetname", 364, 3, NULL },
	{ "count", 416, 0, NULL },
	{ "health", 404, 0, Scr_SetHealth },
	{ "dmg", 412, 0, NULL },
	{ "angles", 324, 4, Scr_SetAngles }
};

void GScr_AddFieldsForEntity()
{
	game_entity_field_t *i;

	for ( i = g_entity_fields; i->name; ++i )
		Scr_AddClassField(0, i->name, i - g_entity_fields);

	GScr_AddFieldsForClient();
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

void Scr_ReadOnlyField(gentity_s *ent, int offset)
{
	Scr_Error("Tried to set a read only entity field");
}

unsigned int G_SetEntityScriptVariableInternal(const char *key, const char *value)
{
	int int_value;
	float float_value;
	vec3_t vector_value;
	unsigned int field;
	int type;

	field = Scr_FindField(key, &type);

	if ( !field )
		return 0;

	if ( type == VAR_VECTOR )
	{
		VectorClear(vector_value);
		sscanf(value, "%f %f %f", vector_value, &vector_value[1], &vector_value[2]);
		Scr_AddVector(vector_value);
	}
	else if ( type > VAR_VECTOR )
	{
		if ( type == VAR_FLOAT )
		{
			float_value = atof(value);
			Scr_AddFloat(float_value);
		}
		else if ( type == VAR_INTEGER )
		{
			int_value = atoi(value);
			Scr_AddInt(int_value);
		}
	}
	else if ( type == VAR_STRING )
	{
		Scr_AddString(value);
	}

	return field;
}

int Scr_SetEntityField(int entnum, int offset)
{
	gentity_s *entity;
	game_entity_field_t *field;

	entity = &g_entities[entnum];

	if ( (offset & 0xC000) == 49152 )
	{
		if ( entity->client )
		{
			Scr_SetClientField(entity->client, offset & 0xFFFF3FFF);
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		field = &g_entity_fields[offset];

		if ( field->setter )
			field->setter(entity, offset);
		else
			Scr_SetGenericField((byte *)entity, field->type, field->ofs);

		return 1;
	}
}

void Scr_GetEntityField(int entnum, int offset)
{
	gentity_s *entity;
	game_entity_field_t *field;

	entity = &g_entities[entnum];

	if ( (offset & 0xC000) == 49152 )
	{
		if ( entity->client )
			Scr_GetClientField(entity->client, offset & 0xFFFF3FFF);
	}
	else
	{
		field = &g_entity_fields[offset];

		Scr_GetGenericField((byte *)entity, field->type, field->ofs);
	}
}

int Scr_SetObjectField(unsigned int classnum, int entnum, int offset)
{
	if ( classnum == CLASS_NUM_ENTITY )
		return Scr_SetEntityField(entnum, offset);

	if ( classnum == CLASS_NUM_HUDELEM )
		Scr_SetHudElemField(entnum, offset);

	return 1;
}

void Scr_GetObjectField(unsigned int classnum, int entnum, int offset)
{
	if ( classnum )
	{
		if ( classnum == CLASS_NUM_HUDELEM )
			Scr_GetHudElemField(entnum, offset);
	}
	else
	{
		Scr_GetEntityField(entnum, offset);
	}
}

void Scr_Notify(gentity_s *ent, unsigned short stringValue, unsigned int paramcount)
{
	Scr_NotifyNum(ent->s.number, 0, stringValue, paramcount);
}