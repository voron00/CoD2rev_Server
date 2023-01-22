#include "../qcommon/qcommon.h"
#include "../qcommon/cmd.h"
#include "dvar.h"

dvar_t *sortedDvars;
dvar_t dvarPool[MAX_DVARS];
int dvarCount;
int dvar_modifiedFlags;
static dvar_t* dvarHashTable[FILE_HASH_SIZE];
dvar_t *dvar_cheats;
static qboolean isDvarSystemActive;
qboolean isLoadingAutoExecGlobalFlag;

static long generateHashValue( const char *fname )
{
	int i;
	long hash;
	char letter;

	if ( !fname )
	{
		Com_Error( ERR_DROP, "null name in generateHashValue" );
	}

	hash = 0;
	i = 0;

	while ( fname[i] != '\0' )
	{
		letter = tolower( fname[i] );
		hash += (long)( letter ) * ( i + 119 );
		i++;
	}

	hash &= ( FILE_HASH_SIZE - 1 );
	return hash;
}

void Dvar_CommandCompletion( void(*callback)(const char *s) )
{
	dvar_t *var;

	for ( var = sortedDvars ; var ; var = var->next )
	{
		callback( var->name );
	}
}

void Dvar_WriteVariables(fileHandle_t f)
{
	dvar_t	*var;

	for (var = sortedDvars; var; var = var->next)
	{
		if (I_stricmp(var->name, "cl_cdkey"))
		{
			if (var->flags & DVAR_ARCHIVE)
				FS_Printf(f, "seta %s \"%s\"\n", var->name, Dvar_DisplayableLatchedValue(var));
		}
	}
}

void Dvar_WriteDefaults(fileHandle_t f)
{
	dvar_t	*var;

	for (var = sortedDvars; var; var = var->next)
	{
		if (I_stricmp(var->name, "cl_cdkey"))
		{
			if ( ((var->flags & 0x40C0) == 0 ))
				FS_Printf(f, "set %s \"%s\"\n", var->name, Dvar_DisplayableResetValue(var));
		}
	}
}

dvar_t* Dvar_FindMalleableVar(const char* dvarName)
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

dvar_t* Dvar_FindVar(const char* dvarName)
{
	return Dvar_FindMalleableVar(dvarName);
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
	var->modified = false;
}

void Dvar_MakeLatchedValueCurrent(dvar_t *dvar)
{
	Dvar_SetVariant(dvar, dvar->latched, DVAR_SOURCE_INTERNAL);
}

void Dvar_ClampVectorToDomain(float* vector, int components, float min, float max)
{
	int channel;

	for (channel = 0; channel < components; ++channel)
	{
		if (min <= vector[channel])
		{
			if (vector[channel] > max)
			{
				vector[channel] = max;
			}
		}
		else
		{
			vector[channel] = min;
		}
	}
}

DvarValue Dvar_ClampValueToDomain(DvarType type, DvarValue value, const DvarValue resetValue, const DvarLimits domain)
{
	switch (type)
	{
	case DVAR_TYPE_BOOL:
		value.boolean = value.boolean != 0;
		break;
	case DVAR_TYPE_FLOAT:
		if (domain.decimal.min <= value.decimal)
		{
			if (value.decimal > domain.decimal.max)
			{
				value.decimal = domain.decimal.max;
			}
		}
		else
		{
			value.decimal = domain.decimal.min;
		}
		break;
	case DVAR_TYPE_VEC2:
		Dvar_ClampVectorToDomain(value.vec2, 2, domain.decimal.min, domain.decimal.max);
		break;
	case DVAR_TYPE_VEC3:
		Dvar_ClampVectorToDomain(value.vec3, 3, domain.decimal.min, domain.decimal.max);
		break;
	case DVAR_TYPE_VEC4:
		Dvar_ClampVectorToDomain(value.vec4, 4, domain.decimal.min, domain.decimal.max);
		break;
	case DVAR_TYPE_INT:
		if (value.integer >= domain.enumeration.stringCount)
		{
			if (value.integer > domain.integer.max)
			{
				value.integer = domain.integer.max;
			}
		}
		else
		{
			value.integer = domain.enumeration.stringCount;
		}
		break;
	case DVAR_TYPE_ENUM:
		if (value.integer < 0 || value.integer >= domain.enumeration.stringCount)
		{
			value.integer = resetValue.integer;
		}
		break;
	case DVAR_TYPE_STRING:
	case DVAR_TYPE_COLOR:
		break;
	default:
		break;
	}

	return value;
}

void Dvar_UpdateValue(dvar_t *dvar, DvarValue value)
{
	DvarValue oldString;
	qboolean shouldFree;
	DvarValue currentString;

	switch (dvar->type)
	{
	case DVAR_TYPE_VEC2:
		dvar->current.vec2[0] = value.vec2[0];
		dvar->current.vec2[1] = value.vec2[1];

		dvar->latched.vec2[0] = value.vec2[0];
		dvar->latched.vec2[1] = value.vec2[1];
		break;
	case DVAR_TYPE_VEC3:
		dvar->current.vec3[0] = value.vec3[0];
		dvar->current.vec3[1] = value.vec3[1];
		dvar->current.vec3[2] = value.vec3[2];

		dvar->latched.vec3[0] = value.vec3[0];
		dvar->latched.vec3[1] = value.vec3[1];
		dvar->latched.vec3[2] = value.vec3[2];
		break;
	case DVAR_TYPE_VEC4:
		dvar->current.vec4[0] = value.vec4[0];
		dvar->current.vec4[1] = value.vec4[1];
		dvar->current.vec4[2] = value.vec4[2];
		dvar->current.vec4[3] = value.vec4[3];

		dvar->latched.vec4[0] = value.vec4[0];
		dvar->latched.vec4[1] = value.vec4[1];
		dvar->latched.vec4[2] = value.vec4[2];
		dvar->latched.vec4[3] = value.vec4[3];
		break;
	case DVAR_TYPE_STRING:
		if (value.integer != dvar->current.integer)
		{
			shouldFree = Dvar_ShouldFreeCurrentString(dvar);
			if (shouldFree)
			{
				oldString.integer = dvar->current.integer;
			}
			Dvar_AssignCurrentStringValue(dvar, &currentString, value.string);
			dvar->current.integer = currentString.integer;
			if (Dvar_ShouldFreeLatchedString(dvar))
			{
				Dvar_FreeString(&dvar->latched);
			}
			dvar->latched.integer = 0;
			Dvar_WeakCopyString(dvar->current.string, &dvar->latched);
			if (shouldFree)
			{
				Dvar_FreeString(&oldString);
			}
		}
		break;
	default:
		dvar->current = value;
		dvar->latched = value;
		break;
	}
}

void Dvar_MakeExplicitType(dvar_t *dvar, const char *dvarName, DvarType type, unsigned int flags, DvarValue resetValue, DvarLimits domain)
{
	qboolean wasString;
	DvarValue castValue;

	dvar->type = type;
	dvar->domain = domain;
	if (flags & DVAR_ROM || (flags & DVAR_CHEAT && dvar_cheats && !dvar_cheats->current.boolean))
	{
		castValue = resetValue;
	}
	else
	{
		castValue = Dvar_StringToValue(dvar->type, dvar->domain, dvar->current.string);
		castValue = Dvar_ClampValueToDomain(type, castValue, resetValue, domain);
	}

	wasString = dvar->type == DVAR_TYPE_STRING && castValue.integer;
	if (wasString)
	{
		castValue.string = CopyString(castValue.string);
	}

	if (dvar->type != DVAR_TYPE_STRING && Dvar_ShouldFreeCurrentString(dvar))
	{
		Dvar_FreeString(&dvar->current);
	}

	dvar->current.string = 0;
	if (Dvar_ShouldFreeLatchedString(dvar))
	{
		Dvar_FreeString(&dvar->latched);
	}

	dvar->latched.string = 0;
	if (Dvar_ShouldFreeResetString(dvar))
	{
		Dvar_FreeString(&dvar->reset);
	}

	dvar->reset.string = 0;
	Dvar_UpdateResetValue(dvar, resetValue);
	Dvar_UpdateValue(dvar, castValue);

	dvar_modifiedFlags |= flags;

	if (wasString)
	{
		FreeString((char *)castValue.string);
	}
}

void Dvar_ReinterpretDvar(dvar_t *dvar, const char *dvarName, DvarType type, unsigned int flags, DvarValue value, DvarLimits domain)
{
	DvarValue resetValue;

	if (dvar->flags & DVAR_EXTERNAL && !(flags & DVAR_EXTERNAL))
	{
		resetValue = value;
		Dvar_PerformUnregistration(dvar);
		FreeString((char *)dvar->name);
		dvar->name = dvarName;
		dvar->flags &= ~DVAR_EXTERNAL;

		Dvar_MakeExplicitType(dvar, dvarName, type, flags, resetValue, domain);
	}
}

dvar_t *Dvar_RegisterNew(const char *dvarName, DvarType type, unsigned int flags, DvarValue value, DvarLimits domain)
{
	int hash;
	dvar_t* dvar;

	if (dvarCount >= MAX_DVARS)
		Com_Error(ERR_FATAL, "Can't create dvar '%s': %i dvars already exist", dvarName, MAX_DVARS);

	dvar = &dvarPool[dvarCount];
	dvarCount++;

	if (flags & DVAR_EXTERNAL)
	{
		dvar->name = CopyString(dvarName);
	}
	else
	{
		dvar->name = dvarName;
	}

	dvar->flags = flags;
	dvar->type = type;

	switch (type)
	{
	case DVAR_TYPE_VEC2:
		dvar->current.vec2[0] = value.vec2[0];
		dvar->current.vec2[1] = value.vec2[1];

		dvar->latched.vec2[0] = value.vec2[0];
		dvar->latched.vec2[1] = value.vec2[1];

		dvar->reset.vec2[0] = value.vec2[0];
		dvar->reset.vec2[1] = value.vec2[1];
		break;
	case DVAR_TYPE_VEC3:
		dvar->current.vec3[0] = value.vec3[0];
		dvar->current.vec3[1] = value.vec3[1];
		dvar->current.vec3[2] = value.vec3[2];

		dvar->latched.vec3[0] = value.vec3[0];
		dvar->latched.vec3[1] = value.vec3[1];
		dvar->latched.vec3[2] = value.vec3[2];

		dvar->reset.vec3[0] = value.vec3[0];
		dvar->reset.vec3[1] = value.vec3[1];
		dvar->reset.vec3[2] = value.vec3[2];
		break;
	case DVAR_TYPE_VEC4:
		dvar->current.vec4[0] = value.vec4[0];
		dvar->current.vec4[1] = value.vec4[1];
		dvar->current.vec4[2] = value.vec4[2];
		dvar->current.vec4[3] = value.vec4[3];

		dvar->latched.vec4[0] = value.vec4[0];
		dvar->latched.vec4[1] = value.vec4[1];
		dvar->latched.vec4[2] = value.vec4[2];
		dvar->latched.vec4[3] = value.vec4[3];

		dvar->reset.vec4[0] = value.vec4[0];
		dvar->reset.vec4[1] = value.vec4[1];
		dvar->reset.vec4[2] = value.vec4[2];
		dvar->reset.vec4[3] = value.vec4[3];
		break;
	case DVAR_TYPE_STRING:
		Dvar_CopyString(value.string, &dvar->current);
		Dvar_WeakCopyString(dvar->current.string, &dvar->latched);
		Dvar_WeakCopyString(dvar->current.string, &dvar->reset);
		break;
	default:
		dvar->current = value;
		dvar->latched = value;
		dvar->reset = value;
		break;
	}

	dvar->domain = domain;
	dvar->modified = false;

	dvar->next = sortedDvars;
	sortedDvars = dvar;
	hash = generateHashValue(dvarName);
	dvar->hashNext = dvarHashTable[hash];
	dvarHashTable[hash] = dvar;

	return dvar;
}

void Dvar_Reregister(dvar_t *dvar, const char *dvarName, DvarType type, unsigned int flags, DvarValue resetValue, DvarLimits domain)
{
	if ((dvar->flags ^ flags) & DVAR_EXTERNAL)
	{
		Dvar_ReinterpretDvar(dvar, dvarName, type, flags, resetValue, domain);
	}

	if (dvar->flags & DVAR_EXTERNAL && dvar->type != type)
	{
		Dvar_MakeExplicitType(dvar, dvarName, type, flags, resetValue, domain);
	}

	dvar->flags |= flags;

	if (dvar->flags & DVAR_CHEAT && dvar_cheats && !dvar_cheats->current.boolean)
	{
		Dvar_SetVariant(dvar, dvar->reset, DVAR_SOURCE_INTERNAL);
		Dvar_SetLatchedValue(dvar, dvar->reset);
	}

	if (dvar->flags & DVAR_LATCH)
	{
		Dvar_MakeLatchedValueCurrent(dvar);
	}
}

dvar_t *Dvar_RegisterVariant(const char *dvarName, DvarType type, unsigned int flags, DvarValue value, DvarLimits domain)
{
	dvar_t* dvar;

	dvar = Dvar_FindMalleableVar(dvarName);

	if (!dvar)
	{
		return Dvar_RegisterNew(dvarName, type, flags, value, domain);
	}

	Dvar_Reregister(dvar, dvarName, type, flags, value, domain);

	return dvar;
}

dvar_t *Dvar_RegisterBool(const char *dvarName, bool value, unsigned int flags)
{
	DvarLimits dvarDomain;
	DvarValue dvarValue;

	dvarDomain.integer.min = 0;
	dvarDomain.integer.max = 0;

	dvarValue.boolean = value;

	return Dvar_RegisterVariant(dvarName, DVAR_TYPE_BOOL, flags, dvarValue, dvarDomain);
}

dvar_t *Dvar_RegisterInt(const char *dvarName, int value, int min, int max, unsigned int flags)
{
	DvarLimits dvarDomain;
	DvarValue dvarValue;

	dvarDomain.integer.max = max;
	dvarDomain.integer.min = min;

	dvarValue.integer = value;

	return Dvar_RegisterVariant(dvarName, DVAR_TYPE_INT, flags, dvarValue, dvarDomain);
}

dvar_t *Dvar_RegisterFloat(const char *dvarName, float value, float min, float max, unsigned int flags)
{
	DvarLimits dvarDomain;
	DvarValue dvarValue;

	dvarDomain.decimal.max = max;
	dvarDomain.decimal.min = min;

	dvarValue.decimal = value;

	return Dvar_RegisterVariant(dvarName, DVAR_TYPE_FLOAT, flags, dvarValue, dvarDomain);
}

dvar_t *Dvar_RegisterString(const char *dvarName, const char *value, unsigned int flags)
{
	DvarLimits dvarDomain;
	DvarValue dvarValue;

	dvarDomain.enumeration.stringCount = 0;
	dvarDomain.enumeration.strings = NULL;
	dvarValue.string = value;

	return Dvar_RegisterVariant(dvarName, DVAR_TYPE_STRING, flags, dvarValue, dvarDomain);
}

dvar_t *Dvar_RegisterEnum(const char *dvarName, const char **valueList, int defaultIndex, unsigned int flags)
{
	DvarLimits dvarDomain;
	DvarValue dvarValue;

	dvarDomain.enumeration.strings = valueList;
	dvarDomain.enumeration.stringCount = 0;

	while (valueList[dvarDomain.enumeration.stringCount] != NULL)
	{
		dvarDomain.enumeration.stringCount++;
	}

	dvarValue.integer = defaultIndex;

	return Dvar_RegisterVariant(dvarName, DVAR_TYPE_ENUM, flags, dvarValue, dvarDomain);
}

dvar_t *Dvar_RegisterVec2(const char *dvarName, float x, float y, float min, float max, unsigned int flags)
{
	DvarLimits dvarDomain;
	DvarValue dvarValue;

	dvarDomain.decimal.max = max;
	dvarDomain.decimal.min = min;

	dvarValue.vec2[0] = x;
	dvarValue.vec2[1] = y;

	return Dvar_RegisterVariant(dvarName, DVAR_TYPE_VEC2, flags, dvarValue, dvarDomain);
}

dvar_t *Dvar_RegisterVec3(const char *dvarName, float x, float y, float z, float min, float max, unsigned int flags)
{
	DvarLimits dvarDomain;
	DvarValue dvarValue;

	dvarDomain.decimal.max = max;
	dvarDomain.decimal.min = min;

	dvarValue.vec3[0] = x;
	dvarValue.vec3[1] = y;
	dvarValue.vec3[2] = z;

	return Dvar_RegisterVariant(dvarName, DVAR_TYPE_VEC3, flags, dvarValue, dvarDomain);
}

dvar_t *Dvar_RegisterVec4(const char *dvarName, float x, float y, float z, float w, float min, float max, unsigned int flags)
{
	DvarLimits dvarDomain;
	DvarValue dvarValue;

	dvarDomain.decimal.max = max;
	dvarDomain.decimal.min = min;

	dvarValue.vec4[0] = x;
	dvarValue.vec4[1] = y;
	dvarValue.vec4[2] = z;
	dvarValue.vec4[3] = w;

	return Dvar_RegisterVariant(dvarName, DVAR_TYPE_VEC4, flags, dvarValue, dvarDomain);
}

dvar_t *Dvar_RegisterColor(const char *dvarName, float r, float g, float b, float a, unsigned int flags)
{
	DvarLimits dvarDomain;
	DvarValue dvarValue;

	dvarDomain.integer.max = 0;
	dvarDomain.integer.min = 0;

	dvarValue.color[0] = (int)((float)((float)(255.0 * I_fclamp(r, 0.0f, 1.0f)) + 0.001) + 9.313225746154785e-10);
	dvarValue.color[1] = (int)((float)((float)(255.0 * I_fclamp(g, 0.0f, 1.0f)) + 0.001) + 9.313225746154785e-10);
	dvarValue.color[2] = (int)((float)((float)(255.0 * I_fclamp(b, 0.0f, 1.0f)) + 0.001) + 9.313225746154785e-10);
	dvarValue.color[3] = (int)((float)((float)(255.0 * I_fclamp(a, 0.0f, 1.0f)) + 0.001) + 9.313225746154785e-10);

	return Dvar_RegisterVariant(dvarName, DVAR_TYPE_COLOR, flags, dvarValue, dvarDomain);
}

const char *Dvar_ValueToString(const dvar_t *dvar, DvarValue value)
{
	const char* result;

	switch (dvar->type)
	{
	case DVAR_TYPE_BOOL:
		result = value.boolean ? "1" : "0";
		break;
	case DVAR_TYPE_FLOAT:
		result = va("%g", value.decimal);
		break;
	case DVAR_TYPE_VEC2:
		result = va("%g %g", value.vec2[0], value.vec2[1]);
		break;
	case DVAR_TYPE_VEC3:
		result = va("%g %g %g", value.vec3[0], value.vec3[1], value.vec3[2]);
		break;
	case DVAR_TYPE_VEC4:
		result = va("%g %g %g %g", value.vec4[0], value.vec4[1], value.vec4[2], value.vec4[3]);
		break;
	case DVAR_TYPE_INT:
		result = va("%i", value.integer);
		break;
	case DVAR_TYPE_ENUM:
		if (dvar->domain.enumeration.stringCount)
		{
			result = dvar->domain.enumeration.strings[value.integer];
		}
		else
		{
			result = "";
		}
		break;
	case DVAR_TYPE_STRING:
		result = va("%s", value.integer);
		break;
	case DVAR_TYPE_COLOR:
		result = va("%g %g %g %g",
		            (float)((float)value.color[0] / 255.0f),
		            (float)((float)value.color[1] / 255.0f),
		            (float)((float)value.color[2] / 255.0f),
		            (float)((float)value.color[3] / 255.0f));
		break;
	default:
		result = "";
		break;
	}

	return result;
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
	if (domain.decimal.min == -FLT_MAX)
	{
		if (domain.decimal.max == FLT_MAX)
		{
			snprintf(outBuffer, outBufferLen, "Domain is any %iD vector", components);
		}
		else
		{
			snprintf(outBuffer, outBufferLen, "Domain is any %iD vector with components %g or smaller",
			         components, domain.decimal.max);
		}
	}
	else if (domain.decimal.max == FLT_MAX)
	{
		snprintf(outBuffer, outBufferLen, "Domain is any %iD vector with components %g or bigger",
		         components, domain.decimal.min);
	}
	else
	{
		snprintf(outBuffer, outBufferLen, "Domain is any %iD vector with components from %g to %g",
		         components, domain.decimal.min, domain.decimal.max);
	}
}

const char *Dvar_DomainToString_Internal(DvarType type, DvarLimits domain, char *outBuffer, int outBufferLen, int *outLineCount)
{
	char* outBufferEnd;
	char* outBufferWalk;
	int charsWritten;
	int stringIndex;

	outBufferEnd = &outBuffer[outBufferLen];
	if (outLineCount)
	{
		*outLineCount = 0;
	}

	switch (type)
	{
	case DVAR_TYPE_BOOL:
		snprintf(outBuffer, outBufferLen, "Domain is 0 or 1");
		break;
	case DVAR_TYPE_FLOAT:
		if (domain.decimal.min == -FLT_MAX)
		{
			if (domain.decimal.max == FLT_MAX)
			{
				snprintf(outBuffer, outBufferLen, "Domain is any number");
			}
			else
			{
				snprintf(outBuffer, outBufferLen, "Domain is any number %g or smaller", domain.decimal.max);
			}
		}
		else if (domain.decimal.max == FLT_MAX)
		{
			snprintf(outBuffer, outBufferLen, "Domain is any number %g or bigger", domain.decimal.min);
		}
		else
		{
			snprintf(outBuffer, outBufferLen, "Domain is any number from %g to %g", domain.decimal.min, domain.decimal.max);
		}
		break;
	case DVAR_TYPE_VEC2:
		Dvar_VectorDomainToString(2, domain, outBuffer, outBufferLen);
		break;
	case DVAR_TYPE_VEC3:
		Dvar_VectorDomainToString(3, domain, outBuffer, outBufferLen);
		break;
	case DVAR_TYPE_VEC4:
		Dvar_VectorDomainToString(4, domain, outBuffer, outBufferLen);
		break;
	case DVAR_TYPE_INT:
		if (domain.integer.min == INT_MAX)
		{
			if (domain.integer.max == INT_MAX)
			{
				snprintf(outBuffer, outBufferLen, "Domain is any integer");
			}
			else
			{
				snprintf(outBuffer, outBufferLen, "Domain is any integer %i or smaller", domain.integer.max);
			}
		}
		else if (domain.integer.max == INT_MAX)
		{
			snprintf(outBuffer, outBufferLen, "Domain is any integer %i or bigger", domain.integer.min);
		}
		else
		{
			snprintf(outBuffer, outBufferLen, "Domain is any integer from %i to %i", domain.integer.min, domain.integer.max);
		}
		break;
	case DVAR_TYPE_ENUM:
		charsWritten = snprintf(outBuffer, outBufferEnd - outBuffer, "Domain is one of the following:");
		if (charsWritten >= 0)
		{
			outBufferWalk = &outBuffer[charsWritten];
			for (stringIndex = 0; stringIndex < domain.enumeration.stringCount; ++stringIndex)
			{
				charsWritten = snprintf(outBufferWalk, outBufferEnd - outBufferWalk, "\n  %2i: %s",
				                        stringIndex, domain.enumeration.strings[stringIndex]);

				if (charsWritten < 0)
				{
					break;
				}

				if (outLineCount)
				{
					++* outLineCount;
				}
				outBufferWalk += charsWritten;
			}
		}
		break;
	case DVAR_TYPE_STRING:
		snprintf(outBuffer, outBufferLen, "Domain is any text");
		break;
	case DVAR_TYPE_COLOR:
		snprintf(outBuffer, outBufferLen, "Domain is any 4-component color, in RGBA format");
		break;
	default:
		*outBuffer = 0;
		break;
	}

	*(outBufferEnd - 1) = 0;
	return outBuffer;
}

void Dvar_PrintDomain(DvarType type, DvarLimits domain)
{
	char domainBuffer[MAX_STRING_CHARS];
	Com_Printf("  %s\n", Dvar_DomainToString_Internal(type, domain, domainBuffer, sizeof(domainBuffer), 0));
}

qboolean Dvar_VectorInDomain(const float* vector, int components, float min, float max)
{
	int channel;

	for (channel = 0; channel < components; ++channel)
	{
		if (min > vector[channel])
		{
			return 0;
		}
		if (vector[channel] > max)
		{
			return 0;
		}
	}
	return 1;
}

qboolean Dvar_ValueInDomain(DvarType type, DvarValue value, DvarLimits domain)
{
	switch (type)
	{
	case DVAR_TYPE_BOOL:
		return 1;
	case DVAR_TYPE_FLOAT:
		if (domain.decimal.min <= value.decimal)
		{
			return value.decimal <= domain.decimal.max;
		}
		return 0;
	case DVAR_TYPE_VEC2:
		return Dvar_VectorInDomain(value.vec2, 2, domain.decimal.min, domain.decimal.max);
	case DVAR_TYPE_VEC3:
		return Dvar_VectorInDomain(value.vec3, 3, domain.decimal.min, domain.decimal.max);
	case DVAR_TYPE_VEC4:
		return Dvar_VectorInDomain(value.vec4, 4, domain.decimal.min, domain.decimal.max);
	case DVAR_TYPE_INT:
		if (value.integer >= domain.integer.min)
		{
			return value.integer <= domain.integer.max;
		}
		return 0;
	case DVAR_TYPE_ENUM:
		return value.integer >= 0 && (value.integer < domain.enumeration.stringCount || !value.integer);
	case DVAR_TYPE_STRING:
		return 1;
	case DVAR_TYPE_COLOR:
		return 1;
	default:
		return 0;
	}
}

qboolean Dvar_ShouldFreeLatchedString(dvar_t *dvar)
{
	return dvar->latched.string
	       && dvar->latched.string != dvar->current.string
	       && dvar->latched.string != dvar->reset.string;
}

void Dvar_AssignLatchedStringValue(dvar_t *dvar, DvarValue *dest, const char *string)
{
	if (dvar->current.string && (string == dvar->current.string || !strcmp(string, dvar->current.string)))
		Dvar_WeakCopyString(dvar->current.string, dest);
	else if (dvar->reset.string && (string == dvar->reset.string || !strcmp(string, dvar->reset.string)))
		Dvar_WeakCopyString(dvar->reset.string, dest);
	else
		Dvar_CopyString(string, dest);
}

void Dvar_SetLatchedValue(dvar_t *dvar, DvarValue value)
{
	DvarValue latchedString;
	DvarValue oldString;
	qboolean shouldFree;

	switch (dvar->type)
	{
	case DVAR_TYPE_VEC2:
		dvar->latched.vec2[0] = value.vec2[0];
		dvar->latched.vec2[1] = value.vec2[1];
		break;
	case DVAR_TYPE_VEC3:
		dvar->latched.vec3[0] = value.vec3[0];
		dvar->latched.vec3[1] = value.vec3[1];
		dvar->latched.vec3[2] = value.vec3[2];
		break;
	case DVAR_TYPE_VEC4:
		dvar->latched.vec4[0] = value.vec4[0];
		dvar->latched.vec4[1] = value.vec4[1];
		dvar->latched.vec4[2] = value.vec4[2];
		dvar->latched.vec4[3] = value.vec4[3];
		break;
	case DVAR_TYPE_STRING:
		if (dvar->latched.string != value.string)
		{
			shouldFree = Dvar_ShouldFreeLatchedString(dvar);
			if (shouldFree)
			{
				oldString.string = dvar->latched.string;
			}

			Dvar_AssignLatchedStringValue(dvar, &latchedString, value.string);
			dvar->latched.string = latchedString.string;
			if (shouldFree)
			{
				Dvar_FreeString(&oldString);
			}
		}
		break;
	default:
		dvar->latched = value;
		break;
	}
}

qboolean Dvar_ValuesEqual(DvarType type, DvarValue val0, DvarValue val1)
{
	qboolean result;

	switch (type)
	{
	case DVAR_TYPE_BOOL:
		result = val0.boolean == val1.boolean;
		break;
	case DVAR_TYPE_FLOAT:
		result = val0.decimal == val1.decimal;
		break;
	case DVAR_TYPE_VEC2:
		result = val0.vec2[0] == val1.vec2[0] && val0.vec2[1] == val1.vec2[1];
		break;
	case DVAR_TYPE_VEC3:
		result = val0.vec3[0] == val1.vec3[0] && val0.vec3[1] == val1.vec3[1] && val0.vec3[2] == val1.vec3[2];
		break;
	case DVAR_TYPE_VEC4:
		result = val0.vec4[0] == val1.vec4[0] && val0.vec4[1] == val1.vec4[1]
		         && val0.vec4[2] == val1.vec4[2] && val0.vec4[3] == val1.vec4[3];
		break;
	case DVAR_TYPE_INT:
		result = val0.integer == val1.integer;
		break;
	case DVAR_TYPE_ENUM:
		result = val0.integer == val1.integer;
		break;
	case DVAR_TYPE_STRING:
		result = strcmp(val0.string, val1.string) == 0;
		break;
	case DVAR_TYPE_COLOR:
		result = val0.integer == val1.integer;
		break;
	default:
		result = 0;
		break;
	}

	return result;
}

void Dvar_AssignCurrentStringValue(dvar_t *dvar, DvarValue *dest, const char *string)
{
	if (dvar->latched.string && (string == dvar->latched.string || !strcmp(string, dvar->latched.string)))
		Dvar_WeakCopyString(dvar->latched.string, dest);
	else if (dvar->reset.string && (string == dvar->reset.string || !strcmp(string, dvar->reset.string)))
		Dvar_WeakCopyString(dvar->reset.string, dest);
	else
		Dvar_CopyString(string, dest);
}

qboolean Dvar_CanChangeValue(const dvar_t *dvar, DvarValue value, DvarSetSource source)
{
	char* reason;

	if (!dvar)
	{
		return 0;
	}

	if (Dvar_ValuesEqual(dvar->type, value, dvar->reset))
	{
		return 1;
	}

	reason = NULL;
	if (dvar->flags & DVAR_ROM)
	{
		reason = va("%s is read only.\n", dvar->name);
	}
	else if (dvar->flags & DVAR_INIT)
	{
		reason = va("%s is write protected.\n", dvar->name);
	}
	else if (dvar->flags & DVAR_CHEAT && !dvar_cheats->current.boolean)
	{
		if (source == DVAR_SOURCE_EXTERNAL)
		{
			reason = va("%s is cheat protected.\n", dvar->name);
		}
	}

	if (!reason)
	{
		return 1;
	}

	Com_Printf(reason);
	return 0;
}

void Dvar_SetVariant(dvar_t *dvar, DvarValue value, DvarSetSource source)
{
	qboolean shouldFreeString;
	DvarValue oldString;
	DvarValue currentString;

	if (!dvar || !dvar->name || !dvar->name[0])
	{
		return;
	}

	if (!Dvar_ValueInDomain(dvar->type, value, dvar->domain))
	{
		Com_Printf("'%s' is not a valid value for dvar '%s'\n", Dvar_ValueToString(dvar, value), dvar->name);
		Dvar_PrintDomain(dvar->type, dvar->domain);
		if (dvar->type == DVAR_TYPE_ENUM)
		{
			Dvar_SetVariant(dvar, dvar->reset, source);
		}
		return;
	}

	if (source == DVAR_SOURCE_EXTERNAL && !Dvar_CanChangeValue(dvar, value, source))
	{
		return;
	}

	if (dvar->flags & DVAR_LATCH)
	{
		Dvar_SetLatchedValue(dvar, value);
		if (!Dvar_ValuesEqual(dvar->type, dvar->latched, dvar->current))
		{
			Com_Printf("%s will be changed upon restarting.\n", dvar->name);
		}
		return;
	}

	if (Dvar_ValuesEqual(dvar->type, dvar->current, value))
	{
		Dvar_SetLatchedValue(dvar, dvar->current);
	}
	else
	{
		dvar_modifiedFlags |= dvar->flags;

		switch (dvar->type)
		{
		case DVAR_TYPE_VEC2:
			dvar->current.vec2[0] = value.vec2[0];
			dvar->current.vec2[1] = value.vec2[1];

			dvar->latched.vec2[0] = value.vec2[0];
			dvar->latched.vec2[1] = value.vec2[1];
			break;
		case DVAR_TYPE_VEC3:
			dvar->current.vec3[0] = value.vec3[0];
			dvar->current.vec3[1] = value.vec3[1];
			dvar->current.vec3[2] = value.vec3[2];

			dvar->latched.vec3[0] = value.vec3[0];
			dvar->latched.vec3[1] = value.vec3[1];
			dvar->latched.vec3[2] = value.vec3[2];
			break;
		case DVAR_TYPE_VEC4:
			dvar->current.vec4[0] = value.vec4[0];
			dvar->current.vec4[1] = value.vec4[1];
			dvar->current.vec4[2] = value.vec4[2];
			dvar->current.vec4[3] = value.vec4[3];

			dvar->latched.vec4[0] = value.vec4[0];
			dvar->latched.vec4[1] = value.vec4[1];
			dvar->latched.vec4[2] = value.vec4[2];
			dvar->latched.vec4[3] = value.vec4[3];
			break;
		case DVAR_TYPE_STRING:
			shouldFreeString = Dvar_ShouldFreeCurrentString(dvar);
			if (shouldFreeString)
			{
				oldString.string = dvar->current.string;
			}

			Dvar_AssignCurrentStringValue(dvar, &currentString, value.string);
			dvar->current.string = currentString.string;

			if (Dvar_ShouldFreeLatchedString(dvar))
			{
				Dvar_FreeString(&dvar->latched);
			}

			dvar->latched.string = NULL;
			Dvar_WeakCopyString(dvar->current.string, &dvar->latched);
			if (shouldFreeString)
			{
				Dvar_FreeString(&oldString);
			}
			break;
		default:
			dvar->current = value;
			dvar->latched = value;
			break;
		}

		dvar->modified = true;
	}
}

void Dvar_SetStringFromSource(dvar_t *dvar, const char *string, DvarSetSource source)
{
	char stringCopy[MAX_STRING_CHARS];
	DvarValue newValue;

	if (dvar && dvar->name)
	{
		if (dvar->type == DVAR_TYPE_STRING)
		{
			I_strncpyz(stringCopy, string, sizeof(stringCopy));
			newValue.string = stringCopy;
		}
		else
		{
			newValue.integer = Dvar_StringToEnum(&dvar->domain, string);
		}

		Dvar_SetVariant(dvar, newValue, source);
	}
}

void Dvar_SetBool(dvar_t *dvar, bool value)
{
	Dvar_SetBoolFromSource(dvar, value, DVAR_SOURCE_INTERNAL);
}

void Dvar_SetInt(dvar_t *dvar, int value)
{
	Dvar_SetIntFromSource(dvar, value, DVAR_SOURCE_INTERNAL);
}

void Dvar_SetFloat(dvar_t *dvar, float value)
{
	Dvar_SetFloatFromSource(dvar, value, DVAR_SOURCE_INTERNAL);
}

void Dvar_SetString(dvar_t *dvar, const char *value)
{
	Dvar_SetStringFromSource(dvar, value, DVAR_SOURCE_INTERNAL);
}

qboolean Dvar_IsValidName(const char *dvarName)
{
	char nameChar;
	int index;

	if (!dvarName)
	{
		return 0;
	}

	for (index = 0; dvarName[index]; ++index)
	{
		nameChar = dvarName[index];
		if (!isalnum(nameChar) && nameChar != '_')
		{
			return 0;
		}
	}

	return 1;
}

int Dvar_GetCombinedString(char *dest, int arg)
{
	int length = 0;
	int maxarg = Cmd_Argc();

	dest[0] = '\0';

	for (int i = arg; i < maxarg; i++)
	{
		const char *string = Cmd_Argv(i);
		length += strlen(string + 1);

		if (length > MAXPRINTMSG)
			break;

		I_strncat(dest, MAXPRINTMSG, string);

		if (i < maxarg - 1)
			I_strncat(dest, MAXPRINTMSG, " ");
	}

	return length;
}

const char *Dvar_EnumToString(const dvar_t *dvar)
{
	if (dvar->domain.enumeration.stringCount)
		return dvar->domain.enumeration.strings[dvar->current.integer];

	return "";
}

bool Dvar_StringToBool(const char *string)
{
	return atoi(string) != 0;
}

int Dvar_StringToInt(const char *string)
{
	return atoi(string);
}

float Dvar_StringToFloat(const char *string)
{
	return atof(string);
}

void Dvar_StringToVec2(const char *string, vec2_t vector)
{
	sscanf(string, "%g %g", &vector[0], &vector[1]);
}

void Dvar_StringToVec3(const char *string, vec3_t vector)
{
	if (string[0] == '(' )
		sscanf(string, "( %g %g %g )", &vector[0], &vector[1], &vector[2]);
	else
		sscanf(string, "%g %g %g", &vector[0], &vector[1], &vector[2]);
}

void Dvar_StringToVec4(const char *string, vec4_t vector)
{
	sscanf(string, "%g %g %g %g", &vector[0], &vector[1], &vector[2], &vector[3]);
}

void Dvar_StringToColor(const char *string, unsigned char *color)
{
	vec4_t colorVec;

	colorVec[0] = 0.0f;
	colorVec[1] = 0.0f;
	colorVec[2] = 0.0f;
	colorVec[3] = 0.0f;

	sscanf(string, "%g %g %g %g", &colorVec[0], &colorVec[1], &colorVec[2], &colorVec[3]);

	color[0] = (int)((float)(255.0 * I_fclamp(colorVec[0], 0.0f, 1.0f)) + 9.313225746154785e-10);
	color[1] = (int)((float)(255.0 * I_fclamp(colorVec[1], 0.0f, 1.0f)) + 9.313225746154785e-10);
	color[2] = (int)((float)(255.0 * I_fclamp(colorVec[2], 0.0f, 1.0f)) + 9.313225746154785e-10);
	color[3] = (int)((float)(255.0 * I_fclamp(colorVec[3], 0.0f, 1.0f)) + 9.313225746154785e-10);
}

int Dvar_StringToEnum(const DvarLimits *domain, const char *string)
{
	int stringIndex;
	const char* digit;

	for (stringIndex = 0; stringIndex < domain->enumeration.stringCount; ++stringIndex)
	{
		if (!I_stricmp(string, domain->enumeration.strings[stringIndex]))
		{
			return stringIndex;
		}
	}

	stringIndex = 0;
	for (digit = string; *digit; ++digit)
	{
		if (*digit < '0' || *digit > '9')
		{
			return DVAR_INVALID_ENUM_INDEX;
		}
		stringIndex = 10 * stringIndex + *digit - '0';
	}

	if (stringIndex >= 0 && stringIndex < domain->enumeration.stringCount)
	{
		return stringIndex;
	}

	for (stringIndex = 0; stringIndex < domain->enumeration.stringCount; ++stringIndex)
	{
		if (!I_strnicmp(string, domain->enumeration.strings[stringIndex], strlen(string)))
		{
			return stringIndex;
		}
	}

	return DVAR_INVALID_ENUM_INDEX;
}

DvarValue Dvar_StringToValue(const DvarType type, const DvarLimits domain, const char *string)
{
	DvarValue value;

	switch (type)
	{
	case DVAR_TYPE_BOOL:
		value.boolean = Dvar_StringToBool(string);
		break;
	case DVAR_TYPE_FLOAT:
		value.decimal = Dvar_StringToFloat(string);
		break;
	case DVAR_TYPE_VEC2:
		Dvar_StringToVec2(string, value.vec2);
		break;
	case DVAR_TYPE_VEC3:
		Dvar_StringToVec3(string, value.vec3);
		break;
	case DVAR_TYPE_VEC4:
		Dvar_StringToVec4(string, value.vec4);
		break;
	case DVAR_TYPE_INT:
		value.integer = Dvar_StringToInt(string);
		break;
	case DVAR_TYPE_ENUM:
		value.integer = Dvar_StringToEnum(&domain, string);
		break;
	case DVAR_TYPE_STRING:
		value.string = string;
		break;
	case DVAR_TYPE_COLOR:
		Dvar_StringToColor(string, value.color);
		break;
	default:
		break;
	}

	return value;
}

void Dvar_FreeString(DvarValue *value)
{
	FreeString((char *)value->string);
	value->string = NULL;
}

qboolean Dvar_ShouldFreeCurrentString(dvar_t *dvar)
{
	return dvar->current.string
	       && dvar->current.string != dvar->latched.string
	       && dvar->current.string != dvar->reset.string;
}

qboolean Dvar_ShouldFreeResetString(dvar_t *dvar)
{
	return dvar->reset.string
	       && dvar->reset.string != dvar->current.string
	       && dvar->reset.string != dvar->latched.string;
}

void Dvar_WeakCopyString(const char *string, DvarValue *value)
{
	value->string = string;
}

void Dvar_CopyString(const char *string, DvarValue *value)
{
	value->string = CopyString(string);
}

void Dvar_AssignResetStringValue(dvar_t *dvar, DvarValue *dest, const char *string)
{
	if (dvar->current.integer && (string == dvar->current.string || !strcmp(string, dvar->current.string)))
	{
		Dvar_WeakCopyString(dvar->current.string, dest);
	}
	else if (dvar->latched.integer && (string == dvar->latched.string || !strcmp(string, dvar->latched.string)))
	{
		Dvar_WeakCopyString(dvar->latched.string, dest);
	}
	else
	{
		Dvar_CopyString(string, dest);
	}
}

void Dvar_SetBoolFromSource(dvar_t *dvar, bool value, DvarSetSource source)
{
	DvarValue newValue;

	if (dvar && dvar->name)
	{
		if (dvar->type)
			newValue.string = value ? "1" : "0";
		else
			newValue.boolean = value;

		Dvar_SetVariant(dvar, newValue, source);
	}
}

void Dvar_SetFloatFromSource(dvar_t *dvar, float value, DvarSetSource source)
{
	char string[32];
	DvarValue newValue;

	if (dvar && dvar->name)
	{
		if (dvar->type == DVAR_TYPE_FLOAT)
			newValue.decimal = value;
		else
		{
			newValue.string = string;
			Com_sprintf(string, sizeof(string), "%g", value);
		}

		Dvar_SetVariant(dvar, newValue, source);
	}
}

void Dvar_SetIntFromSource(dvar_t *dvar, int value, DvarSetSource source)
{
	char string[32];
	DvarValue newValue;

	if (dvar && dvar->name)
	{
		if (dvar->type == DVAR_TYPE_INT || dvar->type == DVAR_TYPE_ENUM)
			newValue.integer = value;
		else
		{
			Com_sprintf(string, sizeof(string), "%i", value);
			newValue.string = string;
		}

		Dvar_SetVariant(dvar, newValue, source);
	}
}

void Dvar_SetFromStringFromSource(dvar_t *dvar, const char *string, DvarSetSource source)
{
	char buf[MAX_STRING_CHARS];
	DvarValue newValue;

	if (dvar && dvar->name)
	{
		I_strncpyz(buf, string, sizeof(buf));
		newValue = Dvar_StringToValue(dvar->type, dvar->domain, buf);
		if (dvar->type == DVAR_TYPE_ENUM && newValue.integer == DVAR_INVALID_ENUM_INDEX)
		{
			Com_Printf("'%s' is not a valid value for dvar '%s'\n", buf, dvar->name);
			Dvar_PrintDomain(dvar->type, dvar->domain);
			newValue = dvar->reset;
		}

		Dvar_SetVariant(dvar, newValue, source);
	}
}

dvar_t *Dvar_SetFromStringByNameFromSource(const char *dvarName, const char *string, DvarSetSource source, unsigned int flags)
{
	dvar_t* dvar;
	dvar = Dvar_FindVar(dvarName);

	if (!dvar)
		return Dvar_RegisterString(dvarName, string, flags | DVAR_EXTERNAL);

	Dvar_SetFromStringFromSource(dvar, string, source);
	return dvar;
}

void Dvar_SetIntByName(const char *dvarName, int value)
{
	dvar_t *dvar;
	char buf[32];

	dvar = Dvar_FindVar(dvarName);

	if (dvar)
		Dvar_SetInt(dvar, value);
	else
	{
		Com_sprintf(buf, sizeof(buf), "%i", value);
		Dvar_RegisterString(dvarName, buf, DVAR_EXTERNAL);
	}
}

void Dvar_UpdateResetValue(dvar_t *dvar, DvarValue value)
{
	DvarValue oldString;
	qboolean shouldFree;
	DvarValue resetString;

	switch (dvar->type)
	{
	case DVAR_TYPE_VEC2:
		dvar->reset.vec2[0] = value.vec2[0];
		dvar->reset.vec2[1] = value.vec2[1];
		break;
	case DVAR_TYPE_VEC3:
		dvar->reset.vec3[0] = value.vec3[0];
		dvar->reset.vec3[1] = value.vec3[1];
		dvar->reset.vec3[2] = value.vec3[2];
		break;
	case DVAR_TYPE_VEC4:
		dvar->reset.vec4[0] = value.vec4[0];
		dvar->reset.vec4[1] = value.vec4[1];
		dvar->reset.vec4[2] = value.vec4[2];
		dvar->reset.vec4[3] = value.vec4[3];
		break;
	case DVAR_TYPE_STRING:
		if (dvar->reset.string != value.string)
		{
			shouldFree = Dvar_ShouldFreeResetString(dvar);
			if (shouldFree)
			{
				oldString.string = dvar->reset.string;
			}
			Dvar_AssignResetStringValue(dvar, &resetString, value.string);
			dvar->reset = resetString;
			if (shouldFree)
			{
				Dvar_FreeString(&oldString);
			}
		}
		break;
	default:
		dvar->reset = value;
		break;
	}
}

qboolean Dvar_GetBool(const char *dvarName)
{
	dvar_t *dvar;

	dvar = Dvar_FindVar(dvarName);

	if (dvar)
	{
		if (dvar->type == DVAR_TYPE_BOOL)
			return dvar->current.boolean;

		return Dvar_StringToBool(dvar->current.string);
	}

	return qfalse;
}

int Dvar_GetInt(const char *dvarName)
{
	dvar_t *dvar;

	dvar = Dvar_FindVar(dvarName);

	if (dvar)
	{
		if (dvar->type == DVAR_TYPE_BOOL)
			return dvar->current.boolean != 0;

		if (dvar->type == DVAR_TYPE_INT || dvar->type == DVAR_TYPE_ENUM)
			return dvar->current.integer;

		if (dvar->type == DVAR_TYPE_FLOAT)
			return (int)dvar->current.decimal;

		return Dvar_StringToInt(dvar->current.string);
	}

	return 0;
}

float Dvar_GetFloat(const char *dvarName)
{
	dvar_t *dvar;

	dvar = Dvar_FindVar(dvarName);

	if (dvar)
	{
		if (dvar->type == DVAR_TYPE_FLOAT)
			return dvar->current.decimal;

		if (dvar->type == DVAR_TYPE_INT)
			return (double)dvar->current.integer;

		if (dvar->type == DVAR_TYPE_STRING)
			return Dvar_StringToFloat(dvar->current.string);
	}

	return 0;
}

const char* Dvar_GetString(const char *dvarName)
{
	dvar_t *dvar;

	dvar = Dvar_FindVar(dvarName);

	if (dvar)
	{
		if (dvar->type == DVAR_TYPE_STRING)
			return dvar->current.string;

		if (dvar->type == DVAR_TYPE_ENUM)
			return Dvar_EnumToString(dvar);
	}

	return "";
}

const char* Dvar_GetVariantString(const char *dvarName)
{
	dvar_t *dvar;

	dvar = Dvar_FindVar(dvarName);

	if (dvar)
		return Dvar_ValueToString(dvar, dvar->current);

	return "";
}

void Dvar_SetCommand(const char *dvarName, const char *string)
{
	dvar_t* dvar;

	dvar = Dvar_SetFromStringByNameFromSource(dvarName, string, DVAR_SOURCE_EXTERNAL, 0);

	if (dvar && isLoadingAutoExecGlobalFlag)
	{
		Dvar_AddFlags(dvar, DVAR_AUTOEXEC);
		Dvar_UpdateResetValue(dvar, dvar->current);
	}
}

const char *Dvar_IndexStringToEnumString(const dvar_t *dvar, const char *indexString)
{
	int enumIndex;
	size_t indexStringIndex;

	if (dvar->domain.enumeration.stringCount == 0)
	{
		return "";
	}

	for (indexStringIndex = 0; indexStringIndex < strlen(indexString); ++indexStringIndex)
	{
		if (!isdigit(indexString[indexStringIndex]))
		{
			return "";
		}
	}

	enumIndex = atoi(indexString);

	if (enumIndex >= 0 && enumIndex < dvar->domain.enumeration.stringCount)
	{
		return dvar->domain.enumeration.strings[enumIndex];
	}

	return "";
}

void Dvar_SetInAutoExec(qboolean inAutoExec)
{
	isLoadingAutoExecGlobalFlag = inAutoExec;
}

void Dvar_Reset(dvar_t *dvar, DvarSetSource setSource)
{
	Dvar_SetVariant(dvar, dvar->reset, setSource);
}

void Dvar_PerformUnregistration(dvar_t *dvar)
{
	DvarValue resetString;

	if (!(dvar->flags & DVAR_EXTERNAL))
	{
		dvar->flags |= DVAR_EXTERNAL;
		dvar->name = CopyString(dvar->name);
	}

	if (dvar->type != DVAR_TYPE_STRING)
	{
		Dvar_CopyString(Dvar_DisplayableLatchedValue(dvar), &dvar->current);
		if (Dvar_ShouldFreeLatchedString(dvar))
		{
			Dvar_FreeString(&dvar->latched);
		}

		dvar->latched.string = 0;
		Dvar_WeakCopyString(dvar->current.string, &dvar->latched);
		if (Dvar_ShouldFreeResetString(dvar))
		{
			Dvar_FreeString(&dvar->reset);
		}

		dvar->reset.string = 0;
		Dvar_AssignResetStringValue(dvar, &resetString, Dvar_DisplayableResetValue(dvar));
		dvar->reset.string = resetString.string;
		dvar->type = DVAR_TYPE_STRING;
	}
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
			if (dvar->type != DVAR_TYPE_BOOL)
			{
				Com_Printf("dvar '%s' is not a boolean dvar\n", dvar->name);
				return;
			}
		}
		else
			Dvar_RegisterBool(name, var_value, DVAR_EXTERNAL);
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
				if (dvar->type != DVAR_TYPE_INT)
				{
					Com_Printf("dvar '%s' is not a integer dvar\n", dvar->name);
					return;
				}
			}
			else
				Dvar_RegisterFloat(name, var_value, var_min, var_max, DVAR_EXTERNAL);
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
				if (dvar->type != DVAR_TYPE_FLOAT)
				{
					Com_Printf("dvar '%s' is not a float dvar\n", dvar->name);
					return;
				}
			}
			else
				Dvar_RegisterFloat(name, var_value, var_min, var_max, DVAR_EXTERNAL);
		}
		else
			Com_Printf("dvar %s: min %g should not be greater than max %g\n", Cmd_Argv(0), var_min, var_max);
	}
	else
		Com_Printf("USAGE: %s <name> <default> <min> <max>\n", Cmd_Argv(0));
}

qboolean Dvar_ToggleSimple(dvar_t *dvar)
{
	switch (dvar->type)
	{
	case DVAR_TYPE_BOOL:
		Dvar_SetBoolFromSource(dvar, !dvar->current.boolean, DVAR_SOURCE_EXTERNAL);
		return qtrue;

	case DVAR_TYPE_FLOAT:
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

	case DVAR_TYPE_VEC2:
	case DVAR_TYPE_VEC3:
	case DVAR_TYPE_VEC4:
	case DVAR_TYPE_STRING:
	case DVAR_TYPE_COLOR:
		Com_Printf("'toggle' with no arguments makes no sense for dvar '%s'\n", dvar->name);
		return qfalse;

	case DVAR_TYPE_INT:
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

	case DVAR_TYPE_ENUM:
		if (dvar->current.integer >= dvar->domain.enumeration.stringCount - 1)
			Dvar_SetIntFromSource(dvar, 0, DVAR_SOURCE_EXTERNAL);
		else
			Dvar_SetIntFromSource(dvar, dvar->current.integer + 1, DVAR_SOURCE_EXTERNAL);
		return qtrue;

	default:
		return qfalse;
	}
}

qboolean Dvar_ToggleInternal()
{
	const char* string;
	int argIndex;
	const char* argString;
	const char* dvarName;
	dvar_t* dvar;
	const char* enumString;

	if (Cmd_Argc() < 2)
	{
		Com_Printf("USAGE: %s <variable> <optional value sequence>\n", Cmd_Argv(0));
		return qfalse;
	}

	dvarName = Cmd_Argv(1);
	dvar = Dvar_FindVar(dvarName);
	if (!dvar)
	{
		Com_Printf("toggle failed: dvar '%s' not found.\n", dvarName);
		return qfalse;
	}

	if (Cmd_Argc() != 2)
	{
		string = Dvar_DisplayableValue(dvar);
		for (argIndex = 2; argIndex + 1 < Cmd_Argc(); ++argIndex)
		{
			argString = Cmd_Argv(argIndex);
			if (dvar->type == DVAR_TYPE_ENUM)
			{
				enumString = Dvar_IndexStringToEnumString(dvar, argString);
				if (strlen(enumString))
				{
					argString = (char*)enumString;
				}
			}
			if (!I_stricmp(string, argString))
			{
				Dvar_SetCommand(dvarName, Cmd_Argv(argIndex + 1));
				return qtrue;
			}
		}

		argString = Cmd_Argv(2);
		if (dvar->type == DVAR_TYPE_ENUM)
		{
			enumString = Dvar_IndexStringToEnumString(dvar, argString);
			if (strlen(enumString))
			{
				argString = (char*)enumString;
			}
		}
		Dvar_SetCommand(dvarName, argString);
		return qtrue;
	}

	return Dvar_ToggleSimple(dvar);
}

void Dvar_Toggle_f(void)
{
	Dvar_ToggleInternal();
}

void Dvar_Set_f(void)
{
	char dvar_value[MAXPRINTMSG];

	if (Cmd_Argc() > 2)
	{
		const char *name = Cmd_Argv(1);

		if (Dvar_IsValidName(name))
		{
			Dvar_GetCombinedString(dvar_value, 2);
			Dvar_SetCommand(name, dvar_value);
		}
		else
			Com_Printf("invalid variable name: %s\n", name);
	}
	else
		Com_Printf("USAGE: set <variable> <value>\n");
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

void Com_DvarDump(conChannel_t channel)
{
	dvar_t *dvar;
	const char *match;
	int	i = 0;
	int count = 0;
	char message[8196];
	char summary[128];

	if ( Cmd_Argc() <= 1 )
		match = NULL;
	else
		match = Cmd_Argv(1);

	if (channel != CON_CHANNEL_LOGFILEONLY || (com_logfile && com_logfile->current.integer))
	{
		Com_PrintMessage(channel, "=============================== DVAR DUMP ========================================\n");

		for (dvar = sortedDvars; dvar; dvar = dvar->next, i++)
		{
			count++;

			if (!match || Com_Filter(match, dvar->name, 0))
			{
				if (Dvar_HasLatchedValue(dvar))
					Com_sprintf(message, sizeof(message), "      %s \"%s\" -- latched \"%s\"\n", dvar->name,
					            Dvar_DisplayableValue(dvar), Dvar_DisplayableLatchedValue(dvar));
				else
					Com_sprintf(message, sizeof(message), "      %s \"%s\"\n", dvar->name, Dvar_DisplayableValue(dvar));

				Com_PrintMessage(channel, message);
			}
		}

		Com_sprintf(summary, sizeof(summary), "\n%i total dvars\n%i dvar indexes\n", count, dvarCount);
		Com_PrintMessage(channel, summary);
		Com_PrintMessage(channel, "=============================== END DVAR DUMP =====================================\n");
	}
}

void Dvar_Dump_f(void)
{
	Com_DvarDump(CON_CHANNEL_DONT_FILTER);
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

qboolean Dvar_Command()
{
	const char *cmd;
	dvar_t *var;
	char dvar_value[MAXPRINTMSG];

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

void Dvar_Shutdown()
{
	int dvarIter;
	dvar_t* dvar;

	isDvarSystemActive = 0;

	for (dvarIter = 0; dvarIter < dvarCount; ++dvarIter)
	{
		dvar = &dvarPool[dvarIter];

		if (dvar->type == DVAR_TYPE_STRING)
		{
			if (Dvar_ShouldFreeCurrentString(dvar))
			{
				Dvar_FreeString(&dvar->current);
			}
			dvar->current.integer = 0;

			if (Dvar_ShouldFreeResetString(dvar))
			{
				Dvar_FreeString(&dvar->reset);
			}
			dvar->reset.integer = 0;

			if (Dvar_ShouldFreeLatchedString(dvar))
			{
				Dvar_FreeString(&dvar->latched);
			}
			dvar->latched.integer = 0;
		}

		if (dvar->flags & DVAR_EXTERNAL)
		{
			FreeString((char *)dvar->name);
		}
	}

	dvarCount = 0;
	dvar_cheats = NULL;
	dvar_modifiedFlags = 0;
	Com_Memset(dvarHashTable, 0, sizeof(dvarHashTable));
}

void Dvar_Init()
{
	isDvarSystemActive = qtrue;
	dvar_cheats = Dvar_RegisterBool("sv_cheats", qfalse, DVAR_INIT);
	Dvar_AddCommands();
}