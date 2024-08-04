#include "../qcommon/qcommon.h"
#include "dvar.h"

dvar_t *sortedDvars;
dvar_t dvarPool[MAX_DVARS];
int dvarCount;
static dvar_t* dvarHashTable[FILE_HASH_SIZE];
dvar_t *dvar_cheats;
static bool isDvarSystemActive;
static bool isLoadingAutoExecGlobalFlag;
int dvar_modifiedFlags;

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

static dvar_t* Dvar_FindMalleableVar(const char* dvarName)
{
	dvar_t *dvar;
	long hash;

	hash = generateHashValue(dvarName);

	for (dvar = dvarHashTable[hash]; dvar; dvar = dvar->hashNext)
	{
		if (!I_stricmp(dvarName, dvar->name))
			return dvar;
	}

	return NULL;
}

dvar_t* Dvar_FindVar(const char* dvarName)
{
	return Dvar_FindMalleableVar(dvarName);
}

bool Dvar_IsSystemActive()
{
	return isDvarSystemActive;
}

bool Dvar_IsValidName(const char *dvarName)
{
	char nameChar;
	int index;

	if (!dvarName)
	{
		return false;
	}

	for (index = 0; dvarName[index]; ++index)
	{
		nameChar = dvarName[index];
		if (!isalnum(nameChar) && nameChar != '_')
		{
			return false;
		}
	}

	return true;
}

static void Dvar_FreeString(DvarValue *value)
{
	FreeString((char *)value->string);
	value->string = NULL;
}

static bool Dvar_ShouldFreeCurrentString(dvar_t *dvar)
{
	return dvar->current.string
	       && dvar->current.string != dvar->latched.string
	       && dvar->current.string != dvar->reset.string;
}

static bool Dvar_ShouldFreeResetString(dvar_t *dvar)
{
	return dvar->reset.string
	       && dvar->reset.string != dvar->current.string
	       && dvar->reset.string != dvar->latched.string;
}

static bool Dvar_ShouldFreeLatchedString(dvar_t *dvar)
{
	return dvar->latched.string
	       && dvar->latched.string != dvar->current.string
	       && dvar->latched.string != dvar->reset.string;
}

static void Dvar_WeakCopyString(const char *string, DvarValue *value)
{
	value->string = string;
}

static void Dvar_CopyString(const char *string, DvarValue *value)
{
	value->string = CopyString(string);
}

static void Dvar_ClampVectorToDomain(vec_t *vector, int components, float min, float max)
{
	for (int channel = 0; channel < components; channel++)
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

static DvarValue Dvar_ClampValueToDomain(DvarType type, DvarValue value, const DvarValue resetValue, const DvarLimits domain)
{
	switch (type)
	{
	case DVAR_TYPE_BOOL:
		value.boolean = value.boolean != false;
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
		Dvar_ClampVectorToDomain(value.vec2, DVAR_TYPE_VEC2, domain.decimal.min, domain.decimal.max);
		break;
	case DVAR_TYPE_VEC3:
		Dvar_ClampVectorToDomain(value.vec3, DVAR_TYPE_VEC3, domain.decimal.min, domain.decimal.max);
		break;
	case DVAR_TYPE_VEC4:
		Dvar_ClampVectorToDomain(value.vec4, DVAR_TYPE_VEC4, domain.decimal.min, domain.decimal.max);
		break;
	case DVAR_TYPE_INT:
		assert(domain.integer.min <= domain.integer.max);
		if (value.integer >= domain.integer.min)
		{
			if ( value.integer > domain.integer.max )
			{
				value.integer = domain.integer.max;
			}
		}
		else
		{
			value.integer = domain.integer.min;
		}
		break;
	case DVAR_TYPE_ENUM:
		if (value.integer < 0 || value.integer >= domain.enumeration.stringCount)
		{
			value.integer = resetValue.integer;
		}
		break;
	default:
		break;
	}

	return value;
}

static void Dvar_AssignCurrentStringValue(dvar_t *dvar, DvarValue *dest, const char *string)
{
	assert(string);
	if (dvar->latched.string && (string == dvar->latched.string || !strcmp(string, dvar->latched.string)))
	{
		Dvar_WeakCopyString(dvar->latched.string, dest);
	}
	else if (dvar->reset.string && (string == dvar->reset.string || !strcmp(string, dvar->reset.string)))
	{
		Dvar_WeakCopyString(dvar->reset.string, dest);
	}
	else
	{
		Dvar_CopyString(string, dest);
	}
}

static void Dvar_UpdateValue(dvar_t *dvar, DvarValue value)
{
	DvarValue oldString;
	bool shouldFree;
	DvarValue currentString;

	assert(dvar);
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
		if (value.string != dvar->current.string)
		{
			shouldFree = Dvar_ShouldFreeCurrentString(dvar);
			if (shouldFree)
			{
				oldString.string = dvar->current.string;
			}
			Dvar_AssignCurrentStringValue(dvar, &currentString, value.string);
			dvar->current.string = currentString.string;
			if (Dvar_ShouldFreeLatchedString(dvar))
			{
				Dvar_FreeString(&dvar->latched);
			}
			dvar->latched.string = 0;
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

static bool Dvar_StringToBool(const char *string)
{
	return atoi(string) != 0;
}

static int Dvar_StringToInt(const char *string)
{
	return atoi(string);
}

static float Dvar_StringToFloat(const char *string)
{
	return atof(string);
}

static void Dvar_StringToVec2(const char *string, vec2_t vector)
{
	sscanf(string, "%g %g", &vector[0], &vector[1]);
}

static void Dvar_StringToVec3(const char *string, vec3_t vector)
{
	if (string[0] == '(' )
		sscanf(string, "( %g %g %g )", &vector[0], &vector[1], &vector[2]);
	else
		sscanf(string, "%g %g %g", &vector[0], &vector[1], &vector[2]);
}

static void Dvar_StringToVec4(const char *string, vec4_t vector)
{
	sscanf(string, "%g %g %g %g", &vector[0], &vector[1], &vector[2], &vector[3]);
}

static void Dvar_StringToColor(const char *string, unsigned char *color)
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

static int Dvar_StringToEnum(const DvarLimits *domain, const char *string)
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

static const char *Dvar_EnumToString(const dvar_t *dvar)
{
	assert(dvar);
	assert(dvar->name);
	if (dvar->domain.enumeration.stringCount)
		return dvar->domain.enumeration.strings[dvar->current.integer];

	return "";
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

static DvarValue Dvar_StringToValue(const DvarType type, const DvarLimits domain, const char *string)
{
	DvarValue value;

	assert(string);
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

static void Dvar_AssignResetStringValue(dvar_t *dvar, DvarValue *dest, const char *string)
{
	assert(string);
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

static void Dvar_UpdateResetValue(dvar_t *dvar, DvarValue value)
{
	DvarValue oldString;
	bool shouldFree;
	DvarValue resetString;

	assert(dvar);
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

static void Dvar_MakeExplicitType(dvar_t *dvar, const char *dvarName, DvarType type, unsigned short flags, DvarValue resetValue, DvarLimits domain)
{
	bool wasString;
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

static void Dvar_PerformUnregistration(dvar_t *dvar)
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

static void Dvar_ReinterpretDvar(dvar_t *dvar, const char *dvarName, DvarType type, unsigned short flags, DvarValue value, DvarLimits domain)
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

static dvar_t *Dvar_RegisterNew(const char *dvarName, DvarType type, unsigned short flags, DvarValue value, DvarLimits domain)
{
	int hash;
	dvar_t* dvar;
	dvar_t** sorted;

	if (dvarCount >= MAX_DVARS)
		Com_Error(ERR_FATAL, "Can't create dvar '%s': %i dvars already exist", dvarName, MAX_DVARS);

	dvar = &dvarPool[dvarCount];
	dvarCount++;
	dvar->type = type;

	if (flags & DVAR_EXTERNAL)
	{
		dvar->name = CopyString(dvarName);
	}
	else
	{
		dvar->name = dvarName;
	}

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
	for ( sorted = &sortedDvars; *sorted && strcasecmp(dvar->name, (*sorted)->name) >= 0; sorted = &(*sorted)->next );
	dvar->next = *sorted;
	*sorted = dvar;
	dvar->flags = flags;
	hash = generateHashValue(dvarName);
	dvar->hashNext = dvarHashTable[hash];
	dvarHashTable[hash] = dvar;

	return dvar;
}

static const char *Dvar_ValueToString(const dvar_t *dvar, DvarValue value)
{
	switch (dvar->type)
	{
	case DVAR_TYPE_BOOL:
		return value.boolean ? "1" : "0";
	case DVAR_TYPE_FLOAT:
		return va("%g", value.decimal);
	case DVAR_TYPE_VEC2:
		return va("%g %g", value.vec2[0], value.vec2[1]);
	case DVAR_TYPE_VEC3:
		return va("%g %g %g", value.vec3[0], value.vec3[1], value.vec3[2]);
	case DVAR_TYPE_VEC4:
		return va("%g %g %g %g", value.vec4[0], value.vec4[1], value.vec4[2], value.vec4[3]);
	case DVAR_TYPE_INT:
		return va("%i", value.integer);
	case DVAR_TYPE_ENUM:
		if (dvar->domain.enumeration.stringCount)
			return dvar->domain.enumeration.strings[value.integer];
		else
			return "";
	case DVAR_TYPE_STRING:
		return va("%s", value.integer);
	case DVAR_TYPE_COLOR:
		return va("%g %g %g %g",
		          (float)((float)value.color[0] / 255.0f),
		          (float)((float)value.color[1] / 255.0f),
		          (float)((float)value.color[2] / 255.0f),
		          (float)((float)value.color[3] / 255.0f));
	default:
		return "";
	}
}

static bool Dvar_VectorInDomain(const vec_t* vector, int components, float min, float max)
{
	for (int channel = 0; channel < components; channel++)
	{
		if (min > vector[channel])
		{
			return false;
		}

		if (vector[channel] > max)
		{
			return false;
		}
	}

	return true;
}

static bool Dvar_ValueInDomain(DvarType type, DvarValue value, DvarLimits domain)
{
	switch (type)
	{
	case DVAR_TYPE_BOOL:
		assert((value.boolean == true || value.boolean == false));
		return true;
	case DVAR_TYPE_FLOAT:
		if (domain.decimal.min <= value.decimal)
		{
			return value.decimal <= domain.decimal.max;
		}
		return false;
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
		return false;
	case DVAR_TYPE_ENUM:
		return value.integer >= 0 && (value.integer < domain.enumeration.stringCount || !value.integer);
	case DVAR_TYPE_STRING:
		return true;
	case DVAR_TYPE_COLOR:
		return true;
	default:
		return false;
	}
}

static bool Dvar_ValuesEqual(DvarType type, DvarValue val0, DvarValue val1)
{
	switch (type)
	{
	case DVAR_TYPE_BOOL:
		return val0.boolean == val1.boolean;
	case DVAR_TYPE_FLOAT:
		return val0.decimal == val1.decimal;
	case DVAR_TYPE_VEC2:
		return val0.vec2[0] == val1.vec2[0] && val0.vec2[1] == val1.vec2[1];
	case DVAR_TYPE_VEC3:
		return val0.vec3[0] == val1.vec3[0] && val0.vec3[1] == val1.vec3[1] && val0.vec3[2] == val1.vec3[2];
	case DVAR_TYPE_VEC4:
		return val0.vec4[0] == val1.vec4[0] && val0.vec4[1] == val1.vec4[1] && val0.vec4[2] == val1.vec4[2] && val0.vec4[3] == val1.vec4[3];
	case DVAR_TYPE_INT:
		return val0.integer == val1.integer;
	case DVAR_TYPE_ENUM:
		return val0.integer == val1.integer;
	case DVAR_TYPE_STRING:
		return strcmp(val0.string, val1.string) == 0;
	case DVAR_TYPE_COLOR:
		return val0.integer == val1.integer;
	default:
		return false;
	}
}

static bool Dvar_CanChangeValue(const dvar_t *dvar, DvarValue value, DvarSetSource source)
{
	char* reason;

	assert(dvar);
	if (!dvar)
	{
		return false;
	}

	if (Dvar_ValuesEqual(dvar->type, value, dvar->reset))
	{
		return true;
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
		if (source == DVAR_SOURCE_EXTERNAL || source == DVAR_SOURCE_SCRIPT)
		{
			reason = va("%s is cheat protected.\n", dvar->name);
		}
	}

	if (!reason)
	{
		return true;
	}

	Com_Printf(reason);
	return false;
}

static void Dvar_AssignLatchedStringValue(dvar_t *dvar, DvarValue *dest, const char *string)
{
	assert(string);
	if (dvar->current.string && (string == dvar->current.string || !strcmp(string, dvar->current.string)))
	{
		Dvar_WeakCopyString(dvar->current.string, dest);
	}
	else if (dvar->reset.string && (string == dvar->reset.string || !strcmp(string, dvar->reset.string)))
	{
		Dvar_WeakCopyString(dvar->reset.string, dest);
	}
	else
	{
		Dvar_CopyString(string, dest);
	}
}

static void Dvar_SetLatchedValue(dvar_t *dvar, DvarValue value)
{
	DvarValue latchedString;
	DvarValue oldString;
	bool shouldFree;

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

static void Dvar_SetVariant(dvar_t *dvar, DvarValue value, DvarSetSource source)
{
	bool shouldFreeString;
	DvarValue oldString;
	DvarValue currentString;

	assert(dvar);
	assert(dvar->name);
	if (!dvar || !dvar->name || !dvar->name[0])
	{
		return;
	}

	Com_PrintMessage(CON_CHANNEL_LOGFILEONLY, va("      dvar set %s %s\n", dvar->name, Dvar_ValueToString(dvar, value)));
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

	if (source != DVAR_SOURCE_EXTERNAL && source != DVAR_SOURCE_SCRIPT)
	{
#if 0
		if (source == DVAR_SOURCE_DEVGUI && dvar->flags & 0x800)
		{
			Dvar_SetLatchedValue(dvar, value);
			return;
		}
#endif
	}
	else
	{
		if (!Dvar_CanChangeValue(dvar, value, source))
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

static void Dvar_MakeLatchedValueCurrent(dvar_t *dvar)
{
	Dvar_SetVariant(dvar, dvar->latched, DVAR_SOURCE_INTERNAL);
}

static void Dvar_Reregister(dvar_t *dvar, const char *dvarName, DvarType type, unsigned short flags, DvarValue resetValue, DvarLimits domain)
{
	assert(dvar);
	assert(dvarName);
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

static dvar_t *Dvar_RegisterVariant(const char *dvarName, DvarType type, unsigned short flags, DvarValue value, DvarLimits domain)
{
	dvar_t *dvar = Dvar_FindMalleableVar(dvarName);

	if (!dvar)
	{
		return Dvar_RegisterNew(dvarName, type, flags, value, domain);
	}

	Dvar_Reregister(dvar, dvarName, type, flags, value, domain);

	return dvar;
}

dvar_t *Dvar_RegisterBool(const char *dvarName, bool value, unsigned short flags)
{
	DvarLimits dvarDomain;
	DvarValue dvarValue;

	dvarValue.boolean = value;
	memset(&dvarDomain, 0, sizeof(dvarDomain));

	return Dvar_RegisterVariant(dvarName, DVAR_TYPE_BOOL, flags, dvarValue, dvarDomain);
}

dvar_t *Dvar_RegisterInt(const char *dvarName, int value, int min, int max, unsigned short flags)
{
	DvarLimits dvarDomain;
	DvarValue dvarValue;

	dvarValue.integer = value;

	dvarDomain.integer.min = min;
	dvarDomain.integer.max = max;

	return Dvar_RegisterVariant(dvarName, DVAR_TYPE_INT, flags, dvarValue, dvarDomain);
}

dvar_t *Dvar_RegisterFloat(const char *dvarName, float value, float min, float max, unsigned short flags)
{
	DvarLimits dvarDomain;
	DvarValue dvarValue;

	dvarValue.decimal = value;

	dvarDomain.decimal.min = min;
	dvarDomain.decimal.max = max;

	return Dvar_RegisterVariant(dvarName, DVAR_TYPE_FLOAT, flags, dvarValue, dvarDomain);
}

dvar_t *Dvar_RegisterString(const char *dvarName, const char *value, unsigned short flags)
{
	DvarLimits dvarDomain;
	DvarValue dvarValue;

	dvarValue.string = value;
	memset(&dvarDomain, 0, sizeof(dvarDomain));

	return Dvar_RegisterVariant(dvarName, DVAR_TYPE_STRING, flags, dvarValue, dvarDomain);
}

dvar_t *Dvar_RegisterEnum(const char *dvarName, const char **valueList, int defaultIndex, unsigned short flags)
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

dvar_t *Dvar_RegisterVec2(const char *dvarName, float x, float y, float min, float max, unsigned short flags)
{
	DvarLimits dvarDomain;
	DvarValue dvarValue;

	dvarDomain.decimal.min = min;
	dvarDomain.decimal.max = max;

	dvarValue.vec2[0] = x;
	dvarValue.vec2[1] = y;

	return Dvar_RegisterVariant(dvarName, DVAR_TYPE_VEC2, flags, dvarValue, dvarDomain);
}

dvar_t *Dvar_RegisterVec3(const char *dvarName, float x, float y, float z, float min, float max, unsigned short flags)
{
	DvarLimits dvarDomain;
	DvarValue dvarValue;

	dvarDomain.decimal.min = min;
	dvarDomain.decimal.max = max;

	dvarValue.vec3[0] = x;
	dvarValue.vec3[1] = y;
	dvarValue.vec3[2] = z;

	return Dvar_RegisterVariant(dvarName, DVAR_TYPE_VEC3, flags, dvarValue, dvarDomain);
}

dvar_t *Dvar_RegisterVec4(const char *dvarName, float x, float y, float z, float w, float min, float max, unsigned short flags)
{
	DvarLimits dvarDomain;
	DvarValue dvarValue;

	dvarDomain.decimal.min = min;
	dvarDomain.decimal.max = max;

	dvarValue.vec4[0] = x;
	dvarValue.vec4[1] = y;
	dvarValue.vec4[2] = z;
	dvarValue.vec4[3] = w;

	return Dvar_RegisterVariant(dvarName, DVAR_TYPE_VEC4, flags, dvarValue, dvarDomain);
}

dvar_t *Dvar_RegisterColor(const char *dvarName, float r, float g, float b, float a, unsigned short flags)
{
	DvarLimits dvarDomain;
	DvarValue dvarValue;

	dvarValue.color[0] = (int)((float)((float)(255.0 * I_fclamp(r, 0.0f, 1.0f)) + 0.001) + 9.313225746154785e-10);
	dvarValue.color[1] = (int)((float)((float)(255.0 * I_fclamp(g, 0.0f, 1.0f)) + 0.001) + 9.313225746154785e-10);
	dvarValue.color[2] = (int)((float)((float)(255.0 * I_fclamp(b, 0.0f, 1.0f)) + 0.001) + 9.313225746154785e-10);
	dvarValue.color[3] = (int)((float)((float)(255.0 * I_fclamp(a, 0.0f, 1.0f)) + 0.001) + 9.313225746154785e-10);

	memset(&dvarDomain, 0, sizeof(dvarDomain));

	return Dvar_RegisterVariant(dvarName, DVAR_TYPE_COLOR, flags, dvarValue, dvarDomain);
}

const char *Dvar_DisplayableValue(const dvar_t *dvar)
{
	assert(dvar);
	return Dvar_ValueToString(dvar, dvar->current);
}

const char *Dvar_DisplayableResetValue(const dvar_t *dvar)
{
	assert(dvar);
	return Dvar_ValueToString(dvar, dvar->reset);
}

const char *Dvar_DisplayableLatchedValue(const dvar_t *dvar)
{
	assert(dvar);
	return Dvar_ValueToString(dvar, dvar->latched);
}

static void Dvar_VectorDomainToString(int components, DvarLimits domain, char *outBuffer, int outBufferLen)
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

static const char *Dvar_DomainToString_Internal(DvarType type, DvarLimits domain, char *outBuffer, int outBufferLen, int *outLineCount)
{
	char* outBufferEnd;
	char* outBufferWalk;
	int charsWritten;
	int stringIndex;

	assert(outBufferLen > 0);
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
					++*outLineCount;
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

bool Dvar_IsAtDefaultValue(const dvar_t *dvar)
{
	return Dvar_ValuesEqual(dvar->type, dvar->current, dvar->reset);
}

bool Dvar_HasLatchedValue(const dvar_t *dvar)
{
	return Dvar_ValuesEqual(dvar->type, dvar->current, dvar->latched) == false;
}

void Dvar_SetBoolFromSource(dvar_t *dvar, bool value, DvarSetSource source)
{
	DvarValue newValue;

	assert(dvar);
	assert(dvar->name);
	if (dvar && dvar->name)
	{
		if (dvar->type == DVAR_TYPE_BOOL)
			newValue.boolean = value;
		else
			newValue.string = value ? "1" : "0";

		Dvar_SetVariant(dvar, newValue, source);
	}
}

void Dvar_SetBool(dvar_t *dvar, bool value)
{
	Dvar_SetBoolFromSource(dvar, value, DVAR_SOURCE_INTERNAL);
}

void Dvar_SetIntFromSource(dvar_t *dvar, int value, DvarSetSource source)
{
	char string[32];
	DvarValue newValue;

	assert(dvar);
	assert(dvar->name);
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

void Dvar_SetInt(dvar_t *dvar, int value)
{
	Dvar_SetIntFromSource(dvar, value, DVAR_SOURCE_INTERNAL);
}

void Dvar_SetFloatFromSource(dvar_t *dvar, float value, DvarSetSource source)
{
	char string[32];
	DvarValue newValue;

	assert(dvar);
	assert(dvar->name);
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

void Dvar_SetFloat(dvar_t *dvar, float value)
{
	Dvar_SetFloatFromSource(dvar, value, DVAR_SOURCE_INTERNAL);
}

void Dvar_SetVec2FromSource(dvar_t *dvar, float x, float y, DvarSetSource source)
{
	char string[64];
	DvarValue newValue;

	assert(dvar);
	assert(dvar->name);
	if (dvar && dvar->name)
	{
		if (dvar->type == DVAR_TYPE_VEC2)
		{
			newValue.vec2[0] = x;
			newValue.vec2[1] = y;
		}
		else
		{
			Com_sprintf(string, sizeof(string), "%g %g", x, y);
			newValue.string = string;
		}

		Dvar_SetVariant(dvar, newValue, source);
	}
}

void Dvar_SetVec2(dvar_t *dvar, float x, float y)
{
	Dvar_SetVec2FromSource(dvar, x, y, DVAR_SOURCE_INTERNAL);
}

void Dvar_SetVec3FromSource(dvar_t *dvar, float x, float y, float z, DvarSetSource source)
{
	char string[96];
	DvarValue newValue;

	assert(dvar);
	assert(dvar->name);
	if (dvar && dvar->name)
	{
		if (dvar->type == DVAR_TYPE_VEC3)
		{
			newValue.vec3[0] = x;
			newValue.vec3[1] = y;
			newValue.vec3[2] = z;
		}
		else
		{
			Com_sprintf(string, sizeof(string), "%g %g %g", x, y, z);
			newValue.string = string;
		}

		Dvar_SetVariant(dvar, newValue, source);
	}
}

void Dvar_SetVec3(dvar_t *dvar, float x, float y, float z)
{
	Dvar_SetVec3FromSource(dvar, x, y, z, DVAR_SOURCE_INTERNAL);
}

void Dvar_SetVec4FromSource(dvar_t *dvar, float x, float y, float z, float w, DvarSetSource source)
{
	char string[128];
	DvarValue newValue;

	assert(dvar);
	assert(dvar->name);
	if (dvar && dvar->name)
	{
		if (dvar->type == DVAR_TYPE_VEC4)
		{
			newValue.vec4[0] = x;
			newValue.vec4[1] = y;
			newValue.vec4[2] = z;
			newValue.vec4[3] = w;
		}
		else
		{
			Com_sprintf(string, sizeof(string), "%g %g %g %g", x, y, z, w);
			newValue.string = string;
		}

		Dvar_SetVariant(dvar, newValue, source);
	}
}

void Dvar_SetVec4(dvar_t *dvar, float x, float y, float z, float w)
{
	Dvar_SetVec4FromSource(dvar, x, y, z, w, DVAR_SOURCE_INTERNAL);
}

void Dvar_SetStringFromSource(dvar_t *dvar, const char *string, DvarSetSource source)
{
	char stringCopy[MAX_STRING_CHARS];
	DvarValue newValue;

	assert(dvar);
	assert(dvar->name);
	assert(string);
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

void Dvar_SetString(dvar_t *dvar, const char *value)
{
	Dvar_SetStringFromSource(dvar, value, DVAR_SOURCE_INTERNAL);
}

void Dvar_SetColorFromSource(dvar_t *dvar, float r, float g, float b, float a, DvarSetSource source)
{
	char string[128];
	DvarValue newValue;

	assert(dvar);
	assert(dvar->name);
	if (dvar && dvar->name)
	{
		if (dvar->type == DVAR_TYPE_COLOR)
		{
			newValue.color[0] = (int)((float)(255.0 * I_fclamp(r, 0.0f, 1.0f)) + 9.313225746154785e-10);
			newValue.color[1] = (int)((float)(255.0 * I_fclamp(g, 0.0f, 1.0f)) + 9.313225746154785e-10);
			newValue.color[2] = (int)((float)(255.0 * I_fclamp(b, 0.0f, 1.0f)) + 9.313225746154785e-10);
			newValue.color[3] = (int)((float)(255.0 * I_fclamp(a, 0.0f, 1.0f)) + 9.313225746154785e-10);
		}
		else
		{
			Com_sprintf(string, sizeof(string), "%g %g %g %g", r, g, b, a);
			newValue.string = string;
		}

		Dvar_SetVariant(dvar, newValue, source);
	}
}

void Dvar_SetColor(dvar_t *dvar, float r, float g, float b, float a)
{
	Dvar_SetColorFromSource(dvar, r, g, b, a, DVAR_SOURCE_INTERNAL);
}

void Dvar_SetBoolByName(const char *dvarName, bool value)
{
	dvar_t *dvar;

	dvar = Dvar_FindVar(dvarName);

	if ( dvar )
		Dvar_SetBool(dvar, value);
	else if ( value )
		Dvar_RegisterString(dvarName, "1", DVAR_EXTERNAL);
	else
		Dvar_RegisterString(dvarName, "0", DVAR_EXTERNAL);
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

void Dvar_SetFloatByName(const char *dvarName, float value)
{
	char string[32];
	dvar_t *dvar;

	dvar = Dvar_FindVar(dvarName);

	if ( dvar )
		Dvar_SetFloat(dvar, value);
	else
	{
		Com_sprintf(string, sizeof(string), "%g", value);
		Dvar_RegisterString(dvarName, string, DVAR_EXTERNAL);
	}
}

void Dvar_SetVec2ByName(const char *dvarName, float x, float y)
{
	char string[64];
	dvar_t *dvar;

	dvar = Dvar_FindVar(dvarName);

	if ( dvar )
		Dvar_SetVec2(dvar, x, y);
	else
	{
		Com_sprintf(string, sizeof(string), "%g %g", x, y);
		Dvar_RegisterString(dvarName, string, DVAR_EXTERNAL);
	}
}

void Dvar_SetVec3ByName(const char *dvarName, float x, float y, float z)
{
	char string[128];
	dvar_t *dvar;

	dvar = Dvar_FindVar(dvarName);

	if ( dvar )
		Dvar_SetVec3(dvar, x, y, z);
	else
	{
		Com_sprintf(string, sizeof(string), "%g %g %g", x, y, z);
		Dvar_RegisterString(dvarName, string, DVAR_EXTERNAL);
	}
}

void Dvar_SetVec4ByName(const char *dvarName, float x, float y, float z, float w)
{
	char string[128];
	dvar_t *dvar;

	dvar = Dvar_FindVar(dvarName);

	if ( dvar )
		Dvar_SetVec4(dvar, x, y, z, w);
	else
	{
		Com_sprintf(string, sizeof(string), "%g %g %g %g", x, y, z, w);
		Dvar_RegisterString(dvarName, string, DVAR_EXTERNAL);
	}
}

void Dvar_SetStringByName(const char *dvarName, const char *value)
{
	dvar_t *dvar;

	dvar = Dvar_FindVar(dvarName);

	if ( dvar )
		Dvar_SetString(dvar, value);
	else
		Dvar_RegisterString(dvarName, value, DVAR_EXTERNAL);
}

void Dvar_SetColorByName(const char *dvarName, float r, float g, float b, float a)
{
	char string[128];
	dvar_t *dvar;

	dvar = Dvar_FindVar(dvarName);

	if ( dvar )
		Dvar_SetColor(dvar, r, g, b, a);
	else
	{
		Com_sprintf(string, sizeof(string), "%g %g %g %g", r, g, b, a);
		Dvar_RegisterString(dvarName, string, DVAR_EXTERNAL);
	}
}

static void Dvar_SetFromStringFromSource(dvar_t *dvar, const char *string, DvarSetSource source)
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

static dvar_t *Dvar_SetFromStringByNameFromSource(const char *dvarName, const char *string, DvarSetSource source, unsigned short flags)
{
	dvar_t* dvar;
	dvar = Dvar_FindVar(dvarName);

	if (!dvar)
		return Dvar_RegisterString(dvarName, string, flags | DVAR_EXTERNAL);

	Dvar_SetFromStringFromSource(dvar, string, source);
	return dvar;
}

void Dvar_SetFromStringByName(const char *dvarName, const char *string)
{
	Dvar_SetFromStringByNameFromSource(dvarName, string, DVAR_SOURCE_INTERNAL, DVAR_NOFLAG);
}

bool Dvar_GetBool(const char *dvarName)
{
	dvar_t *dvar;

	dvar = Dvar_FindVar(dvarName);

	if (dvar)
	{
		if (dvar->type == DVAR_TYPE_BOOL)
			return dvar->current.boolean;

		return Dvar_StringToBool(dvar->current.string);
	}

	return false;
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
			return (float)dvar->current.integer;

		if (dvar->type == DVAR_TYPE_STRING)
			return Dvar_StringToFloat(dvar->current.string);
	}

	return 0.0;
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

	dvar = Dvar_SetFromStringByNameFromSource(dvarName, string, DVAR_SOURCE_EXTERNAL, DVAR_NOFLAG);

	if (dvar && isLoadingAutoExecGlobalFlag)
	{
		Dvar_AddFlags(dvar, DVAR_AUTOEXEC);
		Dvar_UpdateResetValue(dvar, dvar->current);
	}
}

void Dvar_SetInAutoExec(bool inAutoExec)
{
	isLoadingAutoExecGlobalFlag = inAutoExec;
}

void Dvar_Reset(dvar_t *dvar, DvarSetSource setSource)
{
	Dvar_SetVariant(dvar, dvar->reset, setSource);
}

void Dvar_AddFlags(dvar_t *dvar, int flags)
{
	dvar->flags |= flags;
}

void Dvar_ClearFlags(dvar_t *dvar, int flags)
{
	dvar->flags &= ~flags;
}

void Dvar_ClearModified(dvar_t *dvar)
{
	dvar->modified = false;
}

void Dvar_Shutdown()
{
	int dvarIter;
	dvar_t *dvar;

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
	sortedDvars = NULL;
	dvar_cheats = NULL;
	dvar_modifiedFlags = 0;
	isDvarSystemActive = false;
	Com_Memset(dvarHashTable, 0, sizeof(dvarHashTable));
}

void Dvar_ResetScriptInfo()
{
	int dvarIter;

	for ( dvarIter = 0; dvarIter < dvarCount; ++dvarIter )
		dvarPool[dvarIter].flags &= ~DVAR_SERVERINFO_NOUPDATE;
}

void Dvar_Init()
{
	isDvarSystemActive = true;
	dvar_cheats = Dvar_RegisterBool("sv_cheats", false, DVAR_INIT);
	Dvar_AddCommands();
}