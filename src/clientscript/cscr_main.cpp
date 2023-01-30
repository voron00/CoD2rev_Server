#include "../qcommon/qcommon.h"
#include "clientscript_public.h"

void Scr_GetGenericField(const byte *data, int fieldtype, int offset)
{
	const char *model;
	unsigned short object;
	vec3_t vector;
	unsigned short stringValue;

	switch ( fieldtype )
	{
	case F_INT:
		Scr_AddInt(*(int *)&data[offset]);
		break;

	case F_FLOAT:
		Scr_AddFloat(*(float *)&data[offset]);
		break;

	case F_LSTRING:
		Scr_AddString((const char *)&data[offset]);
		break;

	case F_STRING:
		stringValue = *(unsigned short *)&data[offset];
		if ( stringValue )
			Scr_AddConstString(stringValue);
		break;

	case F_VECTOR:
		Scr_AddVector((const float *)&data[offset]);
		break;

	case F_ENTITY:
		if ( *(gentity_t **)&data[offset] )
			Scr_AddEntity(*(gentity_t **)&data[offset]);
		break;

	case F_VECTORHACK:
		vector[0] = 0.0;
		vector[1] = *(float *)&data[offset];
		vector[2] = 0.0;
		Scr_AddVector(vector);
		break;

	case F_OBJECT:
		object = *(unsigned short *)&data[offset];
		if ( object )
			Scr_AddObject(object);
		break;

	case F_MODEL:
		model = G_ModelName(data[offset]);
		Scr_AddString(model);
		break;

	default:
		break;
	}
}

void Scr_SetGenericField(byte *data, int fieldtype, int offset)
{
	unsigned int stringValue;
	vec3_t vector;

	switch ( fieldtype )
	{
	case F_INT:
		*(int *)&data[offset] = Scr_GetInt(0);
		break;

	case F_FLOAT:
		*(float *)&data[offset] = Scr_GetFloat(0);
		break;

	case F_STRING:
		stringValue = Scr_GetConstStringIncludeNull(0);
		Scr_SetString((unsigned short *)&data[offset], stringValue);
		break;

	case F_VECTOR:
		Scr_GetVector(0, vector);
		*(vec_t *)&data[offset] = vector[0];
		*(vec_t *)&data[offset + 4] = vector[1];
		*(vec_t *)&data[offset + 8] = vector[2];
		break;

	case F_ENTITY:
		*(gentity_t **)&data[offset] = Scr_GetEntity(0);
		break;

	case F_VECTORHACK:
		Scr_GetVector(0, vector);
		*(vec_t *)&data[offset] = vector[1];
		break;

	default:
		break;
	}
}