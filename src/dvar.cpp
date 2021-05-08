#include "i_shared.h"
#include "dvar.h"
#include "cmd.h"

dvar_t *sortedDvars;
dvar_t dvarPool[MAX_DVARS];
int dvarCount;
int dvar_modifiedFlags;
static dvar_t* dvarHashTable[FILE_HASH_SIZE];
dvar_t *dvar_cheats;
static qboolean isDvarSystemActive;

static long generateHashValue(const char *fname)
{
	int	i;
	long hash;
	char letter;

	hash = 0;
	i = 0;

	while (fname[i] != '\0')
	{
		letter = tolower(fname[i]);
		hash += (long)(letter) * (i + 119);
		i++;
	}

	hash &= (FILE_HASH_SIZE - 1);

	return hash;
}

dvar_t* Dvar_FindVar(const char* dvarName)
{
	dvar_t *var;
	long hash;

	hash = generateHashValue(dvarName);

	for (var = dvarHashTable[hash]; var; var = var->hashNext)
	{
		if (!I_stricmp(dvarName, var->name))
			return var;
	}

	return NULL;
}

void Dvar_AddFlags(dvar_t* var, unsigned short flags)
{
	var->flags |= flags;
}

void Dvar_ClearFlags(dvar_t* var, unsigned short flags)
{
	var->flags &= ~flags;
}

void Dvar_ClearModified(dvar_t* var)
{
	var->modified = 0;
}

dvar_t* Dvar_Register(const char* dvarName, DvarType type, unsigned short flags, DvarValue value, DvarLimits domain)
{
	dvar_t *var;

	var = Dvar_FindVar(dvarName);

	if (var == NULL)
		return Dvar_RegisterVariant(dvarName, type, flags, value, domain);
	else
		return var;
}

dvar_t* Dvar_RegisterVariant(const char* dvarName, DvarType type, unsigned short flags, DvarValue value, DvarLimits domain)
{
	dvar_t *var;
	long hash;

	if (dvarCount > MAX_DVARS)
	{
		Com_Error(ERR_FATAL, "Can't create dvar '%s': %i dvars already exist", dvarName, dvarCount);
		return NULL;
	}

	var = &dvarPool[dvarCount];
	dvarCount++;

	if (flags & DVAR_USER_CREATED)
		var->name = CopyString(dvarName);
	else
		var->name = dvarName;

	// link the variable in
	var->next = sortedDvars;
	sortedDvars = var;
	hash = generateHashValue(dvarName);
	var->hashNext = dvarHashTable[hash];
	dvarHashTable[hash] = var;

	var->flags = flags;
	var->type = type;

	switch(type)
	{
	case DVAR_BOOL:
		var->current.boolean = value.boolean;
		var->latched.boolean = value.boolean;
		var->reset.boolean = value.boolean;
		break;

	case DVAR_FLOAT:
		var->current.decimal = value.decimal;
		var->latched.decimal = value.decimal;
		var->reset.decimal = value.decimal;
		var->domain = domain;
		break;

	case DVAR_VEC2:
		Vector2Copy(value.vec2, var->current.vec2);
		Vector2Copy(value.vec2, var->latched.vec2);
		Vector2Copy(value.vec2, var->reset.vec2);
		var->domain = domain;
		break;

	case DVAR_VEC3:
		VectorCopy(value.vec3, var->current.vec3);
		VectorCopy(value.vec3, var->latched.vec3);
		VectorCopy(value.vec3, var->reset.vec3);
		var->domain = domain;
		break;

	case DVAR_VEC4:
		Vector4Copy(value.vec4, var->current.vec4);
		Vector4Copy(value.vec4, var->latched.vec4);
		Vector4Copy(value.vec4, var->reset.vec4);
		var->domain = domain;
		break;

	case DVAR_COLOR:
		var->current.color = value.color;
		var->latched.color = value.color;
		var->reset.color = value.color;
		break;

	case DVAR_ENUM:
		var->current.integer = value.integer;
		var->domain = domain;
		break;

	case DVAR_INT:
		var->current.integer = value.integer;
		var->latched.integer = value.integer;
		var->reset.integer = value.integer;
		var->domain = domain;
		break;

	case DVAR_STRING:
		var->current.string = CopyString(value.string);
		var->latched.string = CopyString(value.string);
		var->reset.string = CopyString(value.string);
		break;
	}

	dvar_modifiedFlags |= var->flags;

	return var;
}

const char* Dvar_ValueToString(dvar_t* dvar, DvarValue value)
{
	switch (dvar->type)
	{
	case DVAR_BOOL:
		if (value.boolean)
			return "1";
		else
			return "0";

	case DVAR_FLOAT:
		return va("%g", value.decimal);

	case DVAR_VEC2:
		return va("%g %g", value.vec2[0], value.vec2[1]);

	case DVAR_VEC3:
		return va("%g %g %g", value.vec3[0], value.vec3[1], value.vec3[2]);

	case DVAR_VEC4:
		return va("%g %g %g %g", value.vec4[0], value.vec4[1], value.vec4[2], value.vec4[3]);

	case DVAR_COLOR:
		return va("%.3g %.3g %.3g %.3g", (float)value.color.rgba[0] / (float)0xff, (float)value.color.rgba[1] / (float)0xff, (float)value.color.rgba[2] / (float)0xff, (float)value.color.rgba[3] / (float)0xff);

	case DVAR_ENUM:
	case DVAR_INT:
		return va("%i", value.integer);

	case DVAR_STRING:
		return va("%s", value.string);
	}

	return "";
}

dvar_t* Dvar_RegisterBool(const char* dvarName, qboolean value, unsigned short flags)
{
	dvar_t *dvar;
	DvarLimits domain;
	DvarValue val;

	domain.integer.min = 0;
	domain.integer.max = 0;

	val.boolean = value;

	dvar = Dvar_Register(dvarName, DVAR_BOOL, flags, val, domain);
	return dvar;
}

dvar_t* Dvar_RegisterInt(const char* dvarName, int value, int min, int max, unsigned short flags)
{
	dvar_t *dvar;
	DvarLimits domain;
	DvarValue val;

	domain.integer.min = min;
	domain.integer.max = max;

	val.integer = value;

	dvar = Dvar_Register(dvarName, DVAR_INT, flags, val, domain);
	return dvar;
}

dvar_t* Dvar_RegisterFloat(const char* dvarName, float value, float min, float max, unsigned short flags)
{
	dvar_t *dvar;
	DvarLimits domain;
	DvarValue val;

	domain.decimal.min = min;
	domain.decimal.max = max;

	val.decimal = value;

	dvar = Dvar_Register(dvarName, DVAR_FLOAT, flags, val, domain);
	return dvar;
}

dvar_t* Dvar_RegisterString(const char* dvarName, const char* value, unsigned short flags)
{
	dvar_t *dvar;
	DvarLimits domain;
	DvarValue val;

	domain.integer.min = 0;
	domain.integer.max = 0;

	val.string = (char *)value;

	dvar = Dvar_Register(dvarName, DVAR_STRING, flags, val, domain);
	return dvar;
}

dvar_t* Dvar_RegisterEnum(const char* dvarName, const char** valueList, int defaultIndex, unsigned short flags)
{
	dvar_t *dvar;
	DvarLimits domain;
	DvarValue val;
	int i;

	domain.enumeration.strings = valueList;

	for (i = 0; valueList[i] != NULL; i++)
		domain.enumeration.stringCount = i;

	val.integer = defaultIndex;

	dvar = Dvar_Register(dvarName, DVAR_ENUM, flags, val, domain);
	return dvar;
}

const char *Dvar_DisplayableLatchedValue(dvar_t *var)
{
	return Dvar_ValueToString(var, var->latched);
}

const char *Dvar_DisplayableResetValue(dvar_t *var)
{
	return Dvar_ValueToString(var, var->reset);
}

const char *Dvar_DisplayableValue(dvar_t *var)
{
	return Dvar_ValueToString(var, var->current);
}

qboolean Dvar_IsAtDefaultValue(dvar_t *var)
{
	return Dvar_ValuesEqual(var->type, var->current, var->reset);
}

qboolean Dvar_HasLatchedValue(dvar_t *var)
{
	return Dvar_ValuesEqual(var->type, var->current, var->latched) == qfalse;
}

void Dvar_VectorDomainToString(int components, DvarLimits domain, char *outBuffer, int outBufferLen)
{
	if ( domain.decimal.min == FLT_MIN )
	{
		if ( domain.decimal.max == FLT_MAX )
			snprintf(outBuffer, outBufferLen, "Domain is any %iD vector", components);
		else
			snprintf(outBuffer, outBufferLen, "Domain is any %iD vector with components %g or smaller", components, domain.decimal.max);
	}
	else if ( domain.decimal.max == FLT_MAX )
		snprintf(outBuffer, outBufferLen, "Domain is any %iD vector with components %g or bigger", components, domain.decimal.min);
	else
		snprintf(outBuffer, outBufferLen, "Domain is any %iD vector with components from %g to %g", components, domain.decimal.min, domain.decimal.max);
}

void Dvar_DomainToString_Internal(DvarType type, DvarLimits domain, char *outBuffer, int outBufferLen, int *outLineCount)
{
	int numchars;

	if (outLineCount)
		outLineCount = NULL;

	switch (type)
	{
	case DVAR_BOOL:
		snprintf(outBuffer, outBufferLen, "Domain is 0 or 1");
		break;

	case DVAR_FLOAT:
		if ( domain.decimal.min == FLT_MIN )
		{
			if ( domain.decimal.max == FLT_MAX )
				snprintf(outBuffer, outBufferLen, "Domain is any number");
			else
				snprintf(outBuffer, outBufferLen, "Domain is any number %g or smaller", domain.decimal.max);
		}
		else if ( domain.decimal.max == FLT_MAX )
			snprintf(outBuffer, outBufferLen, "Domain is any number %g or bigger", domain.decimal.min);
		else
			snprintf(outBuffer, outBufferLen, "Domain is any number from %g to %g", domain.decimal.min, domain.decimal.max);
		break;

	case DVAR_VEC2:
		Dvar_VectorDomainToString(2, domain, outBuffer, outBufferLen);
		break;

	case DVAR_VEC3:
		Dvar_VectorDomainToString(3, domain, outBuffer, outBufferLen);
		break;

	case DVAR_VEC4:
		Dvar_VectorDomainToString(4, domain, outBuffer, outBufferLen);
		break;

	case DVAR_INT:
		if ( domain.integer.min == INT_MAX )
		{
			if ( domain.integer.max != domain.integer.min )
			{
				snprintf(outBuffer, outBufferLen, "Domain is any integer %i or smaller", domain.integer.max);
				break;
			}
			snprintf(outBuffer, outBufferLen, "Domain is any integer");
		}
		else
		{
			if ( domain.integer.max != domain.integer.min )
			{
				snprintf(outBuffer, outBufferLen, "Domain is any integer from %i to %i", domain.integer.min, domain.integer.max);
				break;
			}
			snprintf(outBuffer, outBufferLen, "Domain is any integer %i or bigger", domain.integer.min);
		}
		break;

	case DVAR_ENUM:
		numchars = snprintf(outBuffer, outBufferLen, "Domain is one of the following:");
		if (numchars >= 0)
		{
			outBuffer += numchars;
			for (int i = 0; i < domain.enumeration.stringCount + 1; i++)
			{
				numchars = snprintf(outBuffer, outBufferLen, "\n  %2i: %s", i, domain.enumeration.strings[i]);
				if (numchars < 0)
					break;
				if (outLineCount)
					outLineCount++;
				outBuffer += numchars;
			}
		}
		break;

	case DVAR_STRING:
		snprintf(outBuffer, outBufferLen, "Domain is any text");
		break;

	case DVAR_COLOR:
		snprintf(outBuffer, outBufferLen, "Domain is any 4-component color, in RGBA format");
		break;
	}

	outBuffer[outBufferLen - 1] = 0;
}

void Dvar_PrintDomain(DvarType type, DvarLimits domain)
{
	char buf[MAX_STRING_CHARS];
	Dvar_DomainToString_Internal(type, domain, buf, sizeof(buf), 0);
	Com_Printf("  %s\n", buf);
}

qboolean Dvar_Command()
{
	const char *cmd;
	dvar_t *var;
	char dvar_value[MAX_STRING_CHARS];

	cmd = Cmd_Argv(0);
	var = Dvar_FindVar(cmd);

	if ( !var )
		return qfalse;

	if ( Cmd_Argc() == 1 )
	{
		Com_Printf("\"%s\" is: \"%s^7\" default: \"%s^7\"\n", var->name, Dvar_DisplayableValue(var), Dvar_DisplayableResetValue(var));
		if ( Dvar_HasLatchedValue(var) )
		{
			Com_Printf("latched: \"%s\"\n", Dvar_DisplayableLatchedValue(var));
		}
		Dvar_PrintDomain(var->type, var->domain);
	}
	else
	{
		Dvar_GetCombinedString(dvar_value, 1);
		Dvar_SetCommand(cmd, dvar_value);
	}

	return qtrue;
}

qboolean Dvar_ValueInDomain(DvarType type, DvarValue value, DvarLimits domain)
{
	int i;

	switch (type)
	{
	case DVAR_BOOL:
		return qtrue;

	case DVAR_FLOAT:
		if (isnan(value.decimal))
			return qfalse;
		return value.decimal >= domain.decimal.min && value.decimal <= domain.decimal.max;

	case DVAR_VEC2:
		for (i = 0; i < 2; i++)
		{
			if (isnan(value.vec2[i]))
				return qfalse;

			if (value.vec2[i] < domain.decimal.min || value.vec2[i] > domain.decimal.max)
				return qfalse;
		}
		return qtrue;

	case DVAR_VEC3:
		for (i = 0; i < 3; i++)
		{
			if (isnan(value.vec3[i]))
				return qfalse;

			if (value.vec3[i] < domain.decimal.min || value.vec3[i] > domain.decimal.max)
				return qfalse;
		}
		return qtrue;

	case DVAR_VEC4:
		for (i = 0; i < 4; i++)
		{
			if (isnan(value.vec4[i]))
				return qfalse;

			if (value.vec4[i] < domain.decimal.min || value.vec4[i] > domain.decimal.max)
				return qfalse;
		}
		return qtrue;

	case DVAR_COLOR:
		return qtrue;

	case DVAR_ENUM:
		return value.integer >= 0 && value.integer <= domain.enumeration.stringCount;

	case DVAR_INT:
		return value.integer >= domain.integer.min && value.integer <= domain.integer.max;

	case DVAR_STRING:
		return qtrue;
	}

	return qfalse;
}

void Dvar_SetLatchedValue(dvar_t *dvar, DvarValue value)
{
	if (!dvar)
		return;

	if (!Dvar_ValueInDomain(dvar->type, value, dvar->domain))
	{
		Com_Printf("'%s' is not a valid value for dvar '%s'\n", Dvar_ValueToString(dvar, value), dvar->name);
		Dvar_PrintDomain(dvar->type, dvar->domain);
		return;
	}

	switch (dvar->type)
	{
	case DVAR_BOOL:
		if (value.boolean)
			dvar->latched.boolean = qtrue;
		else
			dvar->latched.boolean = qfalse;
		break;

	case DVAR_FLOAT:
		dvar->latched.decimal = value.decimal;
		break;

	case DVAR_VEC2:
		Vector2Copy(value.vec2, dvar->latched.vec2);
		break;

	case DVAR_VEC3:
		VectorCopy(value.vec3, dvar->latched.vec3);
		break;

	case DVAR_VEC4:
		Vector4Copy(value.vec4, dvar->latched.vec4);
		break;

	case DVAR_COLOR:
		dvar->latched.color = value.color;
		break;

	case DVAR_ENUM:
		dvar->latched.integer = value.integer;
		break;

	case DVAR_INT:
		dvar->latched.integer = value.integer;
		break;

	case DVAR_STRING:
		FreeString(dvar->latched.string);
		dvar->latched.string = CopyString(value.string);
		break;
	}
}

qboolean Dvar_ValuesEqual(DvarType type, DvarValue val0, DvarValue val1)
{
	switch (type)
	{
	case DVAR_BOOL:
		return val0.boolean == val1.boolean;

	case DVAR_FLOAT:
		return val0.decimal == val1.decimal;

	case DVAR_VEC2:
		return Vector2Compare(val0.vec2, val1.vec2);

	case DVAR_VEC3:
		return VectorCompare(val0.vec3, val1.vec3);

	case DVAR_VEC4:
		return Vector4Compare(val0.vec4, val1.vec4);

	case DVAR_COLOR:
		return val0.color.i == val1.color.i;

	case DVAR_ENUM:
	case DVAR_INT:
		return val0.integer == val1.integer;

	case DVAR_STRING:
		return I_strcmp(val0.string, val1.string) == 0;
	}

	return qfalse;
}

void Dvar_SetVariant(dvar_t* dvar, DvarValue value, DvarSetSource source)
{
	if (!dvar)
		return;

	if (source != DVAR_SOURCE_INTERNAL)
	{
		if (dvar->flags & DVAR_ROM)
		{
			Com_Printf("%s is read only.\n", dvar->name);
			return;
		}

		if (dvar->flags & DVAR_INIT)
		{
			Com_Printf("%s is write protected.\n", dvar->name);
			return;
		}

		if ((dvar->flags & DVAR_CHEAT) && !dvar_cheats->current.boolean)
		{
			Com_Printf("%s is cheat protected.\n", dvar->name);
			return;
		}

		if (dvar->flags & DVAR_LATCH)
		{
			Dvar_SetLatchedValue(dvar, value);

			if (!Dvar_ValuesEqual(dvar->type, dvar->current, dvar->latched))
				Com_Printf("%s will be changed upon restarting.\n", dvar->name);

			return;
		}
	}

	if (!Dvar_ValueInDomain(dvar->type, value, dvar->domain))
	{
		Com_Printf("'%s' is not a valid value for dvar '%s'\n", Dvar_ValueToString(dvar, value), dvar->name);
		Dvar_PrintDomain(dvar->type, dvar->domain);
		return;
	}

	if (Dvar_ValuesEqual(dvar->type, dvar->current, value))
		return;

	switch (dvar->type)
	{
	case DVAR_BOOL:
		if (value.boolean)
			dvar->current.boolean = qtrue;
		else
			dvar->current.boolean = qfalse;
		break;

	case DVAR_FLOAT:
		dvar->current.decimal = value.decimal;
		break;

	case DVAR_VEC2:
		Vector2Copy(value.vec2, dvar->current.vec2);
		break;

	case DVAR_VEC3:
		VectorCopy(value.vec3, dvar->current.vec3);
		break;

	case DVAR_VEC4:
		Vector4Copy(value.vec4, dvar->current.vec4);
		break;

	case DVAR_COLOR:
		dvar->current.color = value.color;
		break;

	case DVAR_ENUM:
		dvar->current.integer = value.integer;
		break;

	case DVAR_INT:
		dvar->current.integer = value.integer;
		break;

	case DVAR_STRING:
		FreeString(dvar->current.string);
		dvar->current.string = CopyString(value.string);
		break;
	}

	// Set latched value too for non latch dvars
	Dvar_SetLatchedValue(dvar, value);
}

void Dvar_SetBool(dvar_t* dvar, qboolean value)
{
	DvarValue val;
	val.boolean = value;

	Dvar_SetVariant(dvar, val, DVAR_SOURCE_INTERNAL);
}

void Dvar_SetInt(dvar_t* dvar, int value)
{
	DvarValue val;
	val.integer = value;

	Dvar_SetVariant(dvar, val, DVAR_SOURCE_INTERNAL);
}

void Dvar_SetFloat(dvar_t* dvar, float value)
{
	DvarValue val;
	val.decimal = value;

	Dvar_SetVariant(dvar, val, DVAR_SOURCE_INTERNAL);
}

void Dvar_SetString(dvar_t* dvar, const char* value)
{
	DvarValue val;

	val.string = (char *)value;
	Dvar_SetVariant(dvar, val, DVAR_SOURCE_INTERNAL);
}

void Dvar_List_f(void)
{
	dvar_t *var;
	const char *match;
	int	i = 0;

	if (Cmd_Argc() > 1)
	{
		match = Cmd_Argv(1);
		Com_Printf("Displaying all dvars starting with: %s\n", match);
	}
	else
		match = NULL;

	Com_Printf("====================================== Dvar List ======================================\n");

	for (var = sortedDvars; var; var = var->next, i++)
	{
		if (match && !Com_Filter(match, var->name, qfalse))
			continue;

		if (var->flags & DVAR_SERVERINFO)
			Com_Printf("S");
		else
			Com_Printf(" ");

		if (var->flags & DVAR_USERINFO)
			Com_Printf("U");
		else
			Com_Printf(" ");

		if (var->flags & DVAR_ROM)
			Com_Printf("R");
		else
			Com_Printf(" ");

		if (var->flags & DVAR_INIT)
			Com_Printf("I");
		else
			Com_Printf(" ");

		if (var->flags & DVAR_ARCHIVE)
			Com_Printf("A");
		else
			Com_Printf(" ");

		if (var->flags & DVAR_LATCH)
			Com_Printf("L");
		else
			Com_Printf(" ");

		if (var->flags & DVAR_CHEAT)
			Com_Printf("C");
		else
			Com_Printf(" ");

		Com_Printf(" %s \"%s\"\n", var->name, Dvar_DisplayableValue(var));
	}

	Com_Printf("\n%i total dvars\n", i);
	Com_Printf("%i dvar indexes\n", dvarCount);
	Com_Printf("==================================== End Dvar List ====================================\n");
}

void Dvar_Dump_f(void)
{

}

qboolean Dvar_IsValidName(const char *dvarName)
{
	size_t len = strlen(dvarName);

	if (!len)
		return qfalse;

	if (len > MAX_DVAR_NAME_LENGTH)
		return qfalse;

	for (size_t i = 0; i < len; i++)
	{
		if (!isalnum(dvarName[i]) && dvarName[i] != 95)
			return qfalse;
	}

	return qtrue;
}

int Dvar_GetCombinedString(char *dest, int arg)
{
	int length = 0;
	int maxarg = Cmd_Argc();

	dest[0] = '\0';

	for (int i = arg; i < maxarg; i++)
	{
		const char *string = Cmd_Argv(i);
		length += strlen(string);

		if (length > MAX_STRING_CHARS)
			break;

		I_strncat(dest, MAX_STRING_CHARS, string);
	}

	return length;
}

int Dvar_StringToValue(DvarValue *value, const char *string, DvarType type)
{
	int valueset = 0;
	vec4_t colorConv;

	switch(type)
	{
	case DVAR_BOOL:
		if (isInteger(string, 0))
		{
			value->boolean = atoi(string) != 0;
			valueset = 1;
		}
		break;

	case DVAR_FLOAT:
		if (isFloat(string, 0))
		{
			value->decimal = atof(string);
			valueset = 1;
		}
		break;

	case DVAR_VEC2:
		if (isVector(string, 0, sizeof(vec2_t)))
		{
			strToVect(string, value->vec2, sizeof(vec2_t));
			valueset = 1;
		}
		break;

	case DVAR_VEC3:
		if (isVector(string, 0, sizeof(vec3_t)))
		{
			strToVect(string, value->vec2, sizeof(vec3_t));
			valueset = 1;
		}
		break;

	case DVAR_VEC4:
		if (isVector(string, 0, sizeof(vec4_t)))
		{
			strToVect(string, value->vec2, sizeof(vec4_t));
			valueset = 1;
		}
		break;

	case DVAR_COLOR:
		if (isVector(string, 0, 4))
		{
			strToVect(string, colorConv, 4);
			value->color.rgba[3] = (byte)((float)0xff * colorConv[3]);
		}
		else if (isVector(string, 0, 3))
		{
			strToVect(string, colorConv, 3);
			value->color.rgba[3] = 0x0;
		}
		else
			break;

		value->color.rgba[0] = (byte)((float)0xff * colorConv[0]);
		value->color.rgba[1] = (byte)((float)0xff * colorConv[1]);
		value->color.rgba[2] = (byte)((float)0xff * colorConv[2]);
		valueset = 1;
		break;

	case DVAR_ENUM:
	case DVAR_INT:
		if (isInteger(string, 0))
		{
			value->integer = atoi(string);
			valueset = 1;
		}
		break;

	case DVAR_STRING:
		value->string = (char *)string;
		valueset = 1;
		break;
	}

	return valueset;
}

void Dvar_SetFromStringByNameFromSource(const char *dvarName, const char *string, DvarSetSource source)
{
	dvar_t *dvar;
	DvarLimits domain;
	DvarValue val;
	char buf[MAX_STRING_CHARS];

	I_strncpyz(buf, string, sizeof(buf));
	dvar = Dvar_FindVar(dvarName);

	if (dvar == NULL)
	{
		domain.integer.min = 0;
		domain.integer.max = 0;

		val.string = buf;

		Dvar_RegisterVariant(dvarName, DVAR_STRING, DVAR_USER_CREATED, val, domain);
	}
	else
	{
		if (Dvar_StringToValue(&val, string, dvar->type))
			Dvar_SetVariant(dvar, val, source);
		else
			Com_Printf("'%s' is not a valid value for dvar '%s'\n", buf, dvarName);
	}
}

void Dvar_SetBoolFromSource(dvar_t *dvar, qboolean value, DvarSetSource source)
{
	DvarValue val;

	if (value)
		val.boolean = qtrue;
	else
		val.boolean = qfalse;

	Dvar_SetVariant(dvar, val, source);
}

void Dvar_SetFloatFromSource(dvar_t *dvar, float value, DvarSetSource source)
{
	DvarValue val;

	val.decimal = value;

	Dvar_SetVariant(dvar, val, source);
}

void Dvar_SetIntFromSource(dvar_t *dvar, int value, DvarSetSource source)
{
	DvarValue val;

	val.integer = value;

	Dvar_SetVariant(dvar, val, source);
}

void Dvar_SetCommand(const char *dvarName, const char *string)
{
	Dvar_SetFromStringByNameFromSource(dvarName, string, DVAR_SOURCE_EXTERNAL);
}

void Dvar_Set_f(void)
{
	char dvar_value[MAX_STRING_CHARS];

	if (Cmd_Argc() > 2)
	{
		const char *name = Cmd_Argv(1);

		if (Dvar_IsValidName(name))
		{
			Com_Memset(dvar_value, 0, sizeof(dvar_value));
			Dvar_GetCombinedString(dvar_value, 2);
			Dvar_SetCommand(name, dvar_value);
		}
		else
			Com_Printf("invalid variable name: %s\n", name);
	}
	else
		Com_Printf("USAGE: set <variable> <value>\n");
}

const char* Dvar_IndexStringToEnumString(dvar_t *dvar, const char *indexString)
{
	if (!dvar->domain.enumeration.stringCount)
		return "";

	for (int i = 0; i < dvar->domain.enumeration.stringCount; i++)
	{
		if (I_stricmp(indexString, dvar->domain.enumeration.strings[i]) == 0)
			return dvar->domain.enumeration.strings[i];
	}

	return "";
}

qboolean Dvar_ToggleInternal()
{
	dvar_t *dvar;

	if (Cmd_Argc() > 1)
	{
		const char *name = Cmd_Argv(1);
		dvar = Dvar_FindVar(name);

		if (dvar != NULL)
		{
			if (Cmd_Argc() == 2)
			{
				switch(dvar->type)
				{
				case DVAR_BOOL:
					Dvar_SetBoolFromSource(dvar, dvar->current.boolean == 0, DVAR_SOURCE_EXTERNAL);
					return qtrue;

				case DVAR_FLOAT:
					if (dvar->domain.decimal.min > 0.0 || dvar->domain.decimal.max < 1.0)
					{
						if (dvar->current.decimal == dvar->domain.decimal.min)
							Dvar_SetFloatFromSource(dvar, dvar->domain.decimal.max, DVAR_SOURCE_EXTERNAL);
						else
							Dvar_SetFloatFromSource(dvar, dvar->domain.decimal.min, DVAR_SOURCE_EXTERNAL);
					}
					else if (dvar->current.decimal == 0.0)
						Dvar_SetFloatFromSource(dvar, 1, DVAR_SOURCE_EXTERNAL);
					else
						Dvar_SetFloatFromSource(dvar, 0, DVAR_SOURCE_EXTERNAL);
					return qtrue;

				case DVAR_VEC2:
				case DVAR_VEC3:
				case DVAR_VEC4:
				case DVAR_STRING:
				case DVAR_COLOR:
					Com_Printf("'toggle' with no arguments makes no sense for dvar '%s'\n", dvar->name);
					return qfalse;

				case DVAR_INT:
					if (dvar->domain.integer.min > 0 || dvar->domain.integer.max <= 0)
					{
						if (dvar->current.integer == dvar->domain.integer.min)
							Dvar_SetIntFromSource(dvar, dvar->domain.integer.max, DVAR_SOURCE_EXTERNAL);
						else
							Dvar_SetIntFromSource(dvar, dvar->domain.integer.min, DVAR_SOURCE_EXTERNAL);
					}
					else if (dvar->current.integer)
						Dvar_SetIntFromSource(dvar, 0, DVAR_SOURCE_EXTERNAL);
					else
						Dvar_SetIntFromSource(dvar, 1, DVAR_SOURCE_EXTERNAL);
					return qtrue;

				case DVAR_ENUM:
					if (dvar->current.integer >= dvar->domain.enumeration.stringCount - 1)
						Dvar_SetIntFromSource(dvar, 0, DVAR_SOURCE_EXTERNAL);
					else
						Dvar_SetIntFromSource(dvar, dvar->current.integer + 1, DVAR_SOURCE_EXTERNAL);
					return qtrue;
				}
			}
			else
			{
				const char *value = Dvar_DisplayableValue(dvar);

				for (int i = 2; i + 1 < Cmd_Argc(); i++)
				{
					const char *argstr = Cmd_Argv(i);

					if (dvar->type == DVAR_ENUM)
					{
						const char *enumstr = Dvar_IndexStringToEnumString(dvar, argstr);

						if (*enumstr)
							argstr = enumstr;

						if (!strcasecmp(value, argstr))
						{
							const char *dvar_value = Cmd_Argv(i);
							Dvar_SetCommand(name, dvar_value);
							return qtrue;
						}
					}
				}
			}
		}
		else
		{
			Com_Printf("toggle failed: dvar '%s' not found.\n", name);
			return qfalse;
		}
	}
	else
	{
		Com_Printf("USAGE: %s <variable> <optional value sequence>\n", Cmd_Argv(0));
		return qfalse;
	}

	return qtrue;
}

void Dvar_Reset(dvar_t *dvar, DvarSetSource setSource)
{
	Dvar_SetVariant(dvar, dvar->reset, setSource);
}

void Dvar_Toggle_f(void)
{
	Dvar_ToggleInternal();
}

void Dvar_SetA_f(void)
{
	dvar_t *dvar;

	if (Cmd_Argc() > 2)
	{
		Dvar_Set_f();
		dvar = Dvar_FindVar(Cmd_Argv(1));

		if (dvar)
			Dvar_AddFlags(dvar, DVAR_ARCHIVE);
	}
	else
		Com_Printf("USAGE: seta <variable> <value>\n");
}

void Dvar_SetS_f(void)
{
	dvar_t *dvar;

	if (Cmd_Argc() > 2)
	{
		Dvar_Set_f();
		dvar = Dvar_FindVar(Cmd_Argv(1));

		if (dvar)
			Dvar_AddFlags(dvar, DVAR_SERVERINFO);
	}
	else
		Com_Printf("USAGE: sets <variable> <value>\n");
}

void Dvar_SetU_f(void)
{
	dvar_t *dvar;

	if (Cmd_Argc() > 2)
	{
		Dvar_Set_f();
		dvar = Dvar_FindVar(Cmd_Argv(1));

		if (dvar)
			Dvar_AddFlags(dvar, DVAR_USERINFO);
	}
	else
		Com_Printf("USAGE: setu <variable> <value>\n");
}

void Dvar_SetFromDvar_f(void)
{
	dvar_t *dvar;

	if (Cmd_Argc() == 3)
	{
		dvar = Dvar_FindVar(Cmd_Argv(2));

		if (dvar)
			Dvar_SetCommand(Cmd_Argv(1), Dvar_DisplayableValue(dvar));
		else
			Com_Printf("dvar '%s' doesn't exist\n", Cmd_Argv(2));
	}
	else
		Com_Printf("USAGE: setfromdvar <dest_dvar> <source_dvar>\n");
}

void Dvar_Reset_f(void)
{
	dvar_t *dvar;

	if (Cmd_Argc() == 2)
	{
		dvar = Dvar_FindVar(Cmd_Argv(1));

		if (dvar)
			Dvar_Reset(dvar, DVAR_SOURCE_EXTERNAL);
	}
	else
		Com_Printf("USAGE: reset <variable>\n");
}

void Dvar_TogglePrint_f(void)
{
	const char *cmd;
	dvar_t *dvar;

	if (Dvar_ToggleInternal())
	{
		cmd = Cmd_Argv(1);
		dvar = Dvar_FindVar(cmd);
		Com_Printf("%s toggled to %s\n", cmd, Dvar_DisplayableValue(dvar));
	}
}

const char *Dvar_VariableString( const char *var_name )
{
	dvar_t *var;

	var = Dvar_FindVar (var_name);

	if (!var)
		return "";

	if (var->type != DVAR_STRING)
	{
		Com_Printf("%s is not a string-based dvar\n", var->name);
		return "";
	}

	return Dvar_DisplayableValue(var);
}

void Dvar_RegisterBool_f(void)
{
	dvar_t *dvar;
	const char *name, *value;
	qboolean var_value;

	if (Cmd_Argc() == 3)
	{
		name = Cmd_Argv(1);
		value = Cmd_Argv(2);
		var_value = atoi(value) != 0;

		dvar = Dvar_FindVar(name);

		if (dvar)
		{
			if (dvar->type != DVAR_BOOL)
			{
				Com_Printf("dvar '%s' is not a boolean dvar\n", dvar->name);
				return;
			}
		}
		else
			Dvar_RegisterBool(name, var_value, DVAR_USER_CREATED);
	}
	else
		Com_Printf("USAGE: %s <name> <default>\n", Cmd_Argv(0));
}

void Dvar_RegisterInt_f(void)
{
	dvar_t *dvar;
	const char *name, *value, *min, *max;
	int var_min, var_max, var_value;

	if (Cmd_Argc() == 5)
	{
		name = Cmd_Argv(1);
		value = Cmd_Argv(2);
		min = Cmd_Argv(3);
		max = Cmd_Argv(4);

		var_value = atoi(value);
		var_min = atoi(min);
		var_max = atoi(max);

		dvar = Dvar_FindVar(name);

		if (var_max >= var_min)
		{
			if (dvar)
			{
				if (dvar->type != DVAR_INT)
				{
					Com_Printf("dvar '%s' is not a integer dvar\n", dvar->name);
					return;
				}
			}
			else
				Dvar_RegisterFloat(name, var_value, var_min, var_max, DVAR_USER_CREATED);
		}
		else
			Com_Printf("dvar %s: min %i should not be greater than max %i\n", Cmd_Argv(0), var_min, var_max);
	}
	else
		Com_Printf("USAGE: %s <name> <default> <min> <max>\n", Cmd_Argv(0));
}

void Dvar_RegisterFloat_f(void)
{
	dvar_t *dvar;
	const char *name, *value, *min, *max;
	float var_min, var_max, var_value;

	if (Cmd_Argc() == 5)
	{
		name = Cmd_Argv(1);
		value = Cmd_Argv(2);
		min = Cmd_Argv(3);
		max = Cmd_Argv(4);

		var_value = atof(value);
		var_min = atof(min);
		var_max = atof(max);

		dvar = Dvar_FindVar(name);

		if (var_max >= var_min)
		{
			if (dvar)
			{
				if (dvar->type != DVAR_FLOAT)
				{
					Com_Printf("dvar '%s' is not a float dvar\n", dvar->name);
					return;
				}
			}
			else
				Dvar_RegisterFloat(name, var_value, var_min, var_max, DVAR_USER_CREATED);
		}
		else
			Com_Printf("dvar %s: min %g should not be greater than max %g\n", Cmd_Argv(0), var_min, var_max);
	}
	else
		Com_Printf("USAGE: %s <name> <default> <min> <max>\n", Cmd_Argv(0));
}

void Dvar_AddCommands()
{
	Cmd_AddCommand("toggle", Dvar_Toggle_f);
	Cmd_AddCommand("togglep", Dvar_TogglePrint_f);
	Cmd_AddCommand("set", Dvar_Set_f);
	Cmd_AddCommand("sets", Dvar_SetS_f);
	Cmd_AddCommand("seta", Dvar_SetA_f);
	Cmd_AddCommand("setfromdvar", Dvar_SetFromDvar_f);
	Cmd_AddCommand("reset", Dvar_Reset_f);
	Cmd_AddCommand("dvarlist", Dvar_List_f);
	Cmd_AddCommand("dvardump", Dvar_Dump_f);
	Cmd_AddCommand("dvar_bool", Dvar_RegisterBool_f);
	Cmd_AddCommand("dvar_int", Dvar_RegisterInt_f);
	Cmd_AddCommand("dvar_float", Dvar_RegisterFloat_f);
	Cmd_AddCommand("setu", Dvar_SetU_f);
}

void Dvar_Init()
{
	isDvarSystemActive = qtrue;
	dvar_cheats = Dvar_RegisterBool("sv_cheats", qfalse, DVAR_INIT);
	Dvar_AddCommands();
}