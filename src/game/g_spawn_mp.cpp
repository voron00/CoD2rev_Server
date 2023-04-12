#include "../qcommon/qcommon.h"
#include "g_shared.h"
#include "../clientscript/clientscript_public.h"

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

spawn_t spawns[] =
{
	{ "info_null", SP_info_null },
	{ "info_notnull", SP_info_notnull },
	{ "func_group", SP_info_null },
	{ "trigger_multiple", SP_trigger_multiple },
	{ "trigger_radius", SP_trigger_radius },
	{ "trigger_disk", SP_trigger_disk },
	{ "trigger_hurt", SP_trigger_hurt },
	{ "trigger_once", SP_trigger_once },
	{ "light", SP_light },
	{ "misc_model", SP_misc_model },
	{ "misc_mg42", SP_turret },
	{ "misc_turret", SP_turret },
	{ "corona", SP_corona },
	{ "trigger_use", SP_trigger_use },
	{ "trigger_use_touch", SP_trigger_use_touch },
	{ "trigger_damage", SP_trigger_damage },
	{ "trigger_lookat", SP_trigger_lookat },
	{ "script_brushmodel", SP_script_brushmodel },
	{ "script_model", SP_script_model },
	{ "script_origin", SP_script_origin },
	{ "script_struct", G_FreeEntity },
};

void GScr_AddFieldsForEntity()
{
	game_entity_field_t *i;

	for ( i = g_entity_fields; i->name; ++i )
		Scr_AddClassField(0, i->name, i - g_entity_fields);

	GScr_AddFieldsForClient();
}

void GScr_AddFieldsForRadiant()
{
	Scr_AddFields("radiant", "txt");
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

void Scr_GetEnt()
{
	int i;
	gentity_s *get;
	int offset;
	unsigned short name;
	unsigned short value;
	const char *key;
	gentity_s *ent;

	name = Scr_GetConstString(0);
	key = Scr_GetString(1u);
	offset = Scr_GetOffset(0, key);

	if ( offset >= 0 && g_entity_fields[offset].type == F_STRING )
	{
		get = 0;
		i = 0;
		ent = g_entities;

		while ( i < level.num_entities )
		{
			if ( ent->r.inuse )
			{
				value = *(uint16_t *)((char *)&ent->s.number + g_entity_fields[offset].ofs);

				if ( value )
				{
					if ( value == name )
					{
						if ( get )
							Scr_Error("getent used with more than one entity");

						get = ent;
					}
				}
			}

			++i;
			++ent;
		}

		if ( get )
			Scr_AddEntity(get);
	}
}

void Scr_GetEntArray()
{
	int i;
	int j;
	int offset;
	unsigned short name;
	unsigned short value;
	const char *key;
	gentity_s *ent;

	if ( Scr_GetNumParam() )
	{
		name = Scr_GetConstString(0);
		key = Scr_GetString(1u);
		offset = Scr_GetOffset(0, key);

		if ( offset >= 0 && g_entity_fields[offset].type == F_STRING )
		{
			Scr_MakeArray();
			j = 0;
			ent = g_entities;

			while ( j < level.num_entities )
			{
				if ( ent->r.inuse )
				{
					value = *(uint16_t *)((char *)&ent->s.number + g_entity_fields[offset].ofs);

					if ( value )
					{
						if ( value == name )
						{
							Scr_AddEntity(ent);
							Scr_AddArray();
						}
					}
				}

				++j;
				++ent;
			}
		}
	}
	else
	{
		Scr_MakeArray();
		i = 0;
		ent = g_entities;

		while ( i < level.num_entities )
		{
			if ( ent->r.inuse )
			{
				Scr_AddEntity(ent);
				Scr_AddArray();
			}

			++i;
			++ent;
		}
	}
}

void Scr_Notify(gentity_s *ent, unsigned short stringValue, unsigned int paramcount)
{
	Scr_NotifyNum(ent->s.number, 0, stringValue, paramcount);
}

unsigned short Scr_ExecEntThread(gentity_s *ent, int handle, unsigned int paramcount)
{
	return Scr_ExecEntThreadNum(ent->s.number, 0, handle, paramcount);
}

void Scr_FreeEntityConstStrings(gentity_s *ent)
{
	int i;
	game_entity_field_t *field;

	for ( field = g_entity_fields; field->name; ++field )
	{
		if ( field->type == F_STRING )
			Scr_SetString((uint16_t *)((char *)ent + field->ofs), 0);
	}

	for ( i = 0; i <= 6; ++i )
	{
		ent->attachModelNames[i] = 0;
		Scr_SetString(&ent->attachTagNames[i], 0);
	}
}

void Scr_FreeEntity(gentity_s *ent)
{
	Scr_FreeEntityConstStrings(ent);
	Scr_FreeEntityNum(ent->s.number, 0);
}

void GScr_SetDynamicEntityField(gentity_s *ent, unsigned int index)
{
	Scr_SetDynamicEntityField(ent->s.number, 0, index);
}

void G_SetEntityScriptVariable(const char *key, const char *value, gentity_s *ent)
{
	unsigned int index;

	index = G_SetEntityScriptVariableInternal(key, value);

	if ( index )
		GScr_SetDynamicEntityField(ent, index);
}

void G_ParseEntityField(const char *key, const char *value, gentity_s *ent)
{
	uint16_t *string;
	vec3_t vector;
	gentity_s *entity;
	game_entity_field_t *f;

	for ( f = g_entity_fields; ; ++f )
	{
		if ( !f->name )
		{
			G_SetEntityScriptVariable(key, value, ent);
			return;
		}

		if ( !I_stricmp(f->name, key) )
			break;
	}

	entity = ent;

	switch ( f->type )
	{
	case F_INT:
		*(int *)((char *)&entity->s.number + f->ofs) = atoi(value);
		break;

	case F_FLOAT:
		*(float *)((char *)&entity->s.number + f->ofs) = atof(value);
		break;

	case F_STRING:
		Scr_SetString((uint16_t *)((char *)entity + f->ofs), 0);
		string = (uint16_t *)((char *)entity + f->ofs);
		*string = G_NewString(value);
		break;

	case F_VECTOR:
		VectorClear(vector);
		sscanf(value, "%f %f %f", vector, &vector[1], &vector[2]);
		*(vec_t *)((char *)&entity->s.number + f->ofs) = vector[0];
		*(vec_t *)((char *)&entity->s.eType + f->ofs) = vector[1];
		*(vec_t *)((char *)&entity->s.eFlags + f->ofs) = vector[2];
		break;

	case F_MODEL:
		if ( *value == 42 )
			ent->s.index = (unsigned short)atoi(value + 1);
		else
			G_SetModel(ent, value);
		break;

	default:
		return;
	}
}

void G_ParseEntityFields(gentity_s *ent)
{
	int i;

	for ( i = 0; i < level.spawnVars.numSpawnVars; ++i )
		G_ParseEntityField(level.spawnVars.spawnVars[i].key, level.spawnVars.spawnVars[i].value, ent);

	G_SetOrigin(ent, ent->r.currentOrigin);
	G_SetAngle(ent, ent->r.currentAngles);
}

int G_SpawnInt(const char *key, const char *defaultString, int *out)
{
	int present;
	const char *s;

	present = G_SpawnString(key, defaultString, &s);
	*out = atoi(s);

	return present;
}

int G_SpawnFloat(const char *key, const char *defaultString, float *out)
{
	int present;
	const char *s;

	present = G_SpawnString(key, defaultString, &s);
	*out = atof(s);

	return present;
}

int G_SpawnVector(const char *key, const char *defaultString, float *out)
{
	int present;
	const char *s;

	present = G_SpawnString(key, defaultString, &s);
	VectorClear(out);
	sscanf(s, "%f %f %f", out, out + 1, out + 2);

	return present;
}

int G_CallSpawnEntity(gentity_s *ent)
{
	const char *classname;
	gitem_s *item;
	spawn_t *spawn;

	if ( ent->classname )
	{
		classname = SL_ConvertToString(ent->classname);
		item = G_GetItemForClassname(classname);

		if ( item )
		{
			G_SpawnItem(ent, item);
			return 1;
		}
		else
		{
			for ( spawn = spawns; spawn->name; ++spawn )
			{
				if ( !strcmp(spawn->name, classname) )
				{
					spawn->spawn(ent);
					return 1;
				}
			}

			Com_Printf("%s doesn't have a spawn function\n", SL_ConvertToString(ent->classname));
			return 0;
		}
	}
	else
	{
		Com_Printf("G_CallSpawnEntity: NULL classname\n");
		return 0;
	}
}

void G_CallSpawn(void)
{
	const char *classname;
	gentity_s *ent;
	gitem_s *item;
	spawn_t *spawn;

	G_SpawnString("classname", "", &classname);

	if ( classname )
	{
		item = G_GetItemForClassname(classname);

		if ( item )
		{
			ent = G_Spawn();
			G_ParseEntityFields(ent);
			G_SpawnItem(ent, item);
		}
		else
		{
			for ( spawn = spawns; ; ++spawn )
			{
				if ( !spawn->name )
				{
					ent = G_Spawn();
					G_ParseEntityFields(ent);
					return;
				}

				if ( !strcmp(spawn->name, classname) )
					break;
			}

			if ( spawn->spawn != G_FreeEntity )
			{
				ent = G_Spawn();
				G_ParseEntityFields(ent);
				spawn->spawn(ent);
			}
		}
	}
	else
	{
		Com_Printf("G_CallSpawn: NULL classname\n");
	}
}

void G_LoadScriptStructs()
{
	unsigned int structId;
	unsigned int index;
	int i;

	Scr_AddExecThread(g_scr_data.createstruct, 0);
	structId = Scr_GetObject(0);

	for ( i = 0; i < level.spawnVars.numSpawnVars; ++i )
	{
		index = G_SetEntityScriptVariableInternal(level.spawnVars.spawnVars[i].key, level.spawnVars.spawnVars[i].value);

		if ( index )
			Scr_SetStructField(structId, index);
	}
}

void G_LoadStructs()
{
	const char *classname;
	unsigned short callback;

	callback = Scr_ExecThread(g_scr_data.initstructs, 0);
	Scr_FreeThread(callback);

	while ( G_ParseSpawnVars(&level.spawnVars) )
	{
		G_SpawnString("classname", "", &classname);

		if ( !strcmp("script_struct", classname) )
			G_LoadScriptStructs();
	}

	SV_ResetEntityParsePoint();
}

extern dvar_t *g_motd;
extern dvar_t *g_gravity;
void SP_worldspawn()
{
	const char *s;

	G_SpawnString("classname", "", &s);

	if ( I_stricmp(s, "worldspawn") )
		Com_Error(ERR_DROP, "SP_worldspawn: The first entity isn't 'worldspawn'");

	SV_SetConfigstring(2u, "cod");
	SV_SetConfigstring(0xDu, va("%i", level.startTime));

	G_SpawnString("ambienttrack", "", &s);

	if ( *s )
	{
		SV_SetConfigstring(3u, va("n\\%s", s));
	}
	else
	{
		SV_SetConfigstring(3u, "");
	}

	G_SpawnString("message", "", &s);

	SV_SetConfigstring(4u, s);
	SV_SetConfigstring(0xEu, g_motd->current.string);

	G_SpawnString("gravity", "800", &s);
	Dvar_SetFloat(g_gravity, atof(s));

	G_SpawnString("northyaw", "", &s);

	if ( *s )
		SV_SetConfigstring(0xBu, s);
	else
		SV_SetConfigstring(0xBu, "0");

	G_SpawnString("spawnflags", "0", &s);

	g_entities[1022].spawnflags = atoi(s);
	g_entities[1022].s.number = 1022;

	Scr_SetString(&g_entities[1022].classname, scr_const.worldspawn);

	g_entities[1022].r.inuse = 1;
}

void G_SpawnEntitiesFromString()
{
	if ( !G_ParseSpawnVars(&level.spawnVars) )
		Com_Error(ERR_DROP, "G_SpawnEntitiesFromString: no entities");

	SP_worldspawn();

	while ( G_ParseSpawnVars(&level.spawnVars) )
		G_CallSpawn();
}