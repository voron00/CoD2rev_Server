#pragma once

#include "i_math.h"
#include "i_shared.h"

#include <float.h>
#include <limits.h>

#define	MAX_DVARS 1280
#define MAX_DVAR_NAME_LENGTH 32
#define FILE_HASH_SIZE 256

enum DvarSetSource
{
	DVAR_SOURCE_INTERNAL = 0,
	DVAR_SOURCE_EXTERNAL = 1,
	DVAR_SOURCE_SCRIPT = 2
};

enum DvarType : char
{
	DVAR_BOOL = 0x00,
	DVAR_FLOAT = 0x01,
	DVAR_VEC2 = 0x02,
	DVAR_VEC3 = 0x03,
	DVAR_VEC4 = 0x04,
	DVAR_INT = 0x05,
	DVAR_ENUM = 0x06,
	DVAR_STRING = 0x07,
	DVAR_COLOR = 0x08
};

typedef union
{
	int i;
	byte rgba[4];
} ucolor_t;

union DvarLimits
{
	struct
	{
		int stringCount;
		const char **strings;
	} enumeration;
	struct
	{
		int min;
		int max;
	} integer;
	struct
	{
		float min;
		float max;
	} decimal;
};

typedef struct
{
	union
	{
		byte boolean;
		int integer;
		float decimal;
		vec2_t vec2;
		vec3_t vec3;
		vec4_t vec4;
		char *string;
		ucolor_t color;
	};
} DvarValue;

typedef struct dvar_s
{
	const char *name;
	unsigned short flags;
	DvarType type;
	byte modified;
	DvarValue current;
	DvarValue latched;
	DvarValue reset;
	DvarLimits domain;
	struct dvar_s *next;
	struct dvar_s *hashNext;
} dvar_t;

#define	DVAR_ARCHIVE 1
#define	DVAR_USERINFO 2
#define	DVAR_SERVERINFO 4
#define	DVAR_SYSTEMINFO 8
#define	DVAR_INIT 16
#define	DVAR_LATCH 32
#define	DVAR_ROM 64
#define DVAR_CHEAT 128
#define	DVAR_TEMP 256
#define DVAR_NORESTART 1024
#define	DVAR_USER_CREATED 16384

dvar_t* Dvar_FindVar(const char* dvarName);
void Dvar_AddFlags(dvar_t* var, unsigned short flags);
void Dvar_ClearFlags(dvar_t* var, unsigned short flags);
void Dvar_ClearModified(dvar_t* var);
dvar_t* Dvar_Register(const char* dvarName, DvarType type, unsigned short flags, DvarValue value, DvarLimits domain);
dvar_t* Dvar_RegisterVariant(const char* dvarName, DvarType type, unsigned short flags, DvarValue value, DvarLimits domain);
const char* Dvar_ValueToString(dvar_t* dvar, DvarValue value);
dvar_t* Dvar_RegisterBool(const char* dvarName, qboolean value, unsigned short flags);
dvar_t* Dvar_RegisterInt(const char* dvarName, int value, int min, int max, unsigned short flags);
dvar_t* Dvar_RegisterFloat(const char* dvarName, float value, float min, float max, unsigned short flags);
dvar_t* Dvar_RegisterString(const char* dvarName, const char* value, unsigned short flags);
dvar_t* Dvar_RegisterEnum(const char* dvarName, const char** valueList, int defaultIndex, unsigned short flags);
int Dvar_GetCombinedString(char *dest, int arg);
void Dvar_SetLatchedValue(dvar_t *var, DvarValue value);
qboolean Dvar_ValuesEqual(DvarType type, DvarValue val0, DvarValue val1);
void Dvar_SetVariant(dvar_t* dvar, DvarValue value, DvarSetSource source);
void Dvar_SetBool(dvar_t* dvar, qboolean value);
void Dvar_SetInt(dvar_t* dvar, int value);
void Dvar_SetFloat(dvar_t* dvar, float value);
void Dvar_SetString(dvar_t* dvar, const char* value);
const char* Dvar_DisplayableValue(dvar_t* dvar);
void Dvar_SetCommand(const char *dvarName, const char *string);
void Dvar_List_f(void);
void Dvar_Set_f(void);
void Dvar_Toggle_f(void);
void Dvar_SetA_f(void);
void Dvar_SetS_f(void);
void Dvar_SetU_f(void);
void Dvar_SetFromDvar_f(void);
const char *Dvar_VariableString( const char *var_name );

void Dvar_Init();
qboolean Dvar_Command();
