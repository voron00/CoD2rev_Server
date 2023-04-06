#include "../qcommon/qcommon.h"
#include "clientscript_public.h"

#define MAX_SCRIPT_VARIABLES 65536

typedef struct __attribute__((aligned(64))) scrVarGlob_s
{
	VariableValueInternal variableList[MAX_SCRIPT_VARIABLES];
} scrVarGlob_t;
static_assert((sizeof(scrVarGlob_t) == 0x100000), "ERROR: scrVarGlob_t size is invalid!");

#ifdef TESTING_LIBRARY
#define scrVarGlob (*((scrVarGlob_t*)( 0x08294000 )))
#else
scrVarGlob_t scrVarGlob;
#endif

#ifdef TESTING_LIBRARY
#define scrVarPub (*((scrVarPub_t*)( 0x08394000 )))
#else
scrVarPub_t scrVarPub;
#endif

#define VAR_MASK 0x1F
#define VAR_NAME_HIGH_MASK 0xFFFFFF00
#define VAR_NAME_BITS 8

#define VAR_STAT_FREE 0
#define VAR_STAT_MOVABLE 32
#define VAR_STAT_HEAD 64
#define VAR_STAT_MASK 96
#define VAR_STAT_EXTERNAL VAR_STAT_MASK

#define VAR_TYPE(var) (var->w.type & VAR_MASK)

const char *var_typename[] =
{
	"undefined",
	"object",
	"string",
	"localized string",
	"vector",
	"float",
	"int",
	"codepos",
	"precodepos",
	"function",
	"stack",
	"animation",
	"developer codepos",
	"include codepos",
	"thread list",
	"thread",
	"thread",
	"thread",
	"thread",
	"struct",
	"removed entity",
	"entity",
	"array",
	"removed thread"
};

struct scr_classStruct_t
{
	uint16_t id;
	uint16_t entArrayId;
	char charId;
	const char *name;
};

#ifdef TESTING_LIBRARY
#define scrClassMap (((scr_classStruct_t*)( 0x0815AB80 )))
#else
struct scr_classStruct_t scrClassMap[] =
{
	{ 0u, 0u, 'e', "entity" },
	{ 0u, 0u, 'h', "hudelem" },
	{ 0u, 0u, 'p', "pathnode" },
	{ 0u, 0u, 'v', "vehiclenode" }
};
#endif

#define CLASS_NUM_COUNT 4

unsigned int GetVarType(int varIndex)
{
	return scrVarGlob.variableList[varIndex].w.status & VAR_MASK;
}

qboolean IsObject(VariableValueInternal *var)
{
	return (var->w.status & VAR_MASK) > VAR_THREAD_LIST;
}

qboolean IsObjectVal(int varIndex)
{
	return GetVarType(varIndex) > VAR_THREAD_LIST;
}

unsigned int FindNextSibling(unsigned int id)
{
	if ( (scrVarGlob.variableList[scrVarGlob.variableList[scrVarGlob.variableList[id].nextSibling].hash.id].w.status & VAR_MASK) > VAR_THREAD_LIST )
		return 0;
	else
		return scrVarGlob.variableList[scrVarGlob.variableList[id].nextSibling].hash.id;
}

unsigned int FindLastSibling(unsigned int id)
{
	if ( (scrVarGlob.variableList[scrVarGlob.variableList[scrVarGlob.variableList[scrVarGlob.variableList[scrVarGlob.variableList[id].nextSibling].hash.u.prev].hash.u.prev].hash.id].w.status & VAR_MASK) > VAR_THREAD_LIST )
		return 0;
	else
		return scrVarGlob.variableList[scrVarGlob.variableList[scrVarGlob.variableList[scrVarGlob.variableList[id].nextSibling].hash.u.prev].hash.u.prev].hash.id;
}

unsigned int FindVariableIndexInternal(unsigned int name, unsigned short index)
{
	unsigned int newIndex;
	VariableValueInternal *newEntry;
	VariableValueInternal *entryValue;
	VariableValueInternal *entry;

	entry = &scrVarGlob.variableList[scrVarGlob.variableList[index].hash.id];

	if ( (entry->w.status & VAR_STAT_MASK) != 64 )
		return 0;

	if ( entry->w.status >> 8 != name )
	{
		newIndex = entry->v.next;

		for ( entryValue = &scrVarGlob.variableList[newIndex];
		        entryValue != &scrVarGlob.variableList[index];
		        entryValue = &scrVarGlob.variableList[newIndex] )
		{
			newEntry = &scrVarGlob.variableList[entryValue->hash.id];

			if ( newEntry->w.status >> 8 == name )
				return newIndex;

			newIndex = newEntry->v.next;
		}

		return 0;
	}

	return index;
}

unsigned int FindVariable(unsigned int parentId, unsigned int name)
{
	return scrVarGlob.variableList[FindVariableIndex(parentId, name)].hash.id;
}

unsigned int FindVariableIndex(unsigned int parentId, unsigned int name)
{
	return FindVariableIndexInternal(name, (parentId + name) % 0xFFFD + 1);
}

unsigned int FindObjectVariable(unsigned int parentId, unsigned int id)
{
	return scrVarGlob.variableList[FindVariableIndex(parentId, id + 0x10000)].hash.id;
}

unsigned int FindObject(unsigned int id)
{
	return scrVarGlob.variableList[id].u.u.pointerValue;
}

void AddRefToVector(const float *vectorValue)
{
	RefVector *refVec = (RefVector*)((byte*)vectorValue - 4);

	if ( !refVec->length )
		++refVec->refCount;
}

void RemoveRefToVector(const float *vectorValue)
{
	RefVector *refVec = (RefVector*)((byte*)vectorValue - 4);

	if ( !refVec->length )
	{
		if ( refVec->refCount )
			--refVec->refCount;
		else
			MT_Free(refVec, sizeof(RefVector));
	}
}

void Scr_GetArrayIndexValue(VariableValue *value, unsigned int index)
{
	if ( index > 0xFFFF )
	{
		if ( index > 0x1FFFD )
		{
			value->type = 6;
			value->u.intValue = index - 0x800000;
		}
		else
		{
			value->type = 1;
			value->u.intValue = index - 0x10000;
		}
	}
	else
	{
		value->type = 2;
		value->u.intValue = (unsigned short)index;
	}
}

void MakeVariableExternal(VariableValueInternal *value, VariableValueInternal *parentValue)
{
	VariableValue var;
	unsigned int varIndex;
	int nextSibling;
	int prev;
	int nextSiblingIndex;
	int prevSiblingIndex;
	Variable hash;
	VariableValueInternal *oldEntryValue;
	VariableValueInternal *entryValue;
	unsigned int index;
	VariableValueInternal *nextEntry;
	VariableValueInternal *oldEntry;
	unsigned int oldIndex;

	index = value - scrVarGlob.variableList;
	entryValue = &scrVarGlob.variableList[value->hash.id];

	if ( (parentValue->w.status & VAR_MASK) == 22 )
	{
		--parentValue->u.o.u.size;
		varIndex = entryValue->w.status >> 8;
		Scr_GetArrayIndexValue(&var, varIndex);
		RemoveRefToValue(&var);
	}

	if ( (entryValue->w.status & VAR_STAT_MASK) == 64 )
	{
		oldIndex = entryValue->v.next;
		oldEntry = &scrVarGlob.variableList[oldIndex];
		oldEntryValue = &scrVarGlob.variableList[oldEntry->hash.id];

		if ( oldEntry != value )
		{
			oldEntryValue->w.status &= 0xFFFFFF9F;
			oldEntryValue->w.status |= 0x40u;
			prevSiblingIndex = value->hash.u.prev;
			nextSiblingIndex = entryValue->nextSibling;
			prev = oldEntry->hash.u.prev;
			nextSibling = oldEntryValue->nextSibling;
			scrVarGlob.variableList[nextSibling].hash.u.prev = index;
			scrVarGlob.variableList[scrVarGlob.variableList[prev].hash.id].nextSibling = index;
			scrVarGlob.variableList[nextSiblingIndex].hash.u.prev = oldIndex;
			scrVarGlob.variableList[scrVarGlob.variableList[prevSiblingIndex].hash.id].nextSibling = oldIndex;
			hash = value->hash;
			value->hash = oldEntry->hash;
			oldEntry->hash = hash;
			index = oldIndex;
		}
	}
	else
	{
		oldEntry = value;
		oldEntryValue = entryValue;
		do
		{
			nextEntry = oldEntry;
			oldIndex = oldEntryValue->v.next;
			oldEntry = &scrVarGlob.variableList[oldIndex];
			oldEntryValue = &scrVarGlob.variableList[oldEntry->hash.id];
		}
		while ( oldEntry != value );
		scrVarGlob.variableList[nextEntry->hash.id].v.next = entryValue->v.next;
	}

	entryValue->w.status &= 0xFFFFFF9F;
	entryValue->w.status |= 0x60u;
	entryValue->v.next = index;
}

unsigned int GetNewVariableIndexInternal3(unsigned int parentId, unsigned int name, unsigned int index)
{
	VariableValue value;
	unsigned int status;
	VariableValueInternal *parentValue;
	VariableValueInternal *newEntry;
	uint16_t next;
	uint16_t id;
	VariableValueInternal *newEntryValue;
	uint16_t i;
	uint16_t nextId;
	VariableValueInternal_u nextValue;
	VariableValueInternal *entry;
	VariableValueInternal *entryValue;

	entry = &scrVarGlob.variableList[index];
	entryValue = &scrVarGlob.variableList[entry->hash.id];
	status = entryValue->w.status & VAR_STAT_MASK;

	if ( status )
	{
		if ( status == 64 )
		{
			if ( (entry->w.status & VAR_STAT_MASK) != 0 )
			{
				index = scrVarGlob.variableList->u.next;
				if ( !scrVarGlob.variableList->u.next )
					Scr_TerminalError("exceeded maximum number of script variables");
				entry = &scrVarGlob.variableList[index];
				newEntry = &scrVarGlob.variableList[entry->hash.id];
				nextId = newEntry->u.next;
				scrVarGlob.variableList->u.next = nextId;
				scrVarGlob.variableList[nextId].hash.u.prev = 0;
				newEntry->w.status = 32;
				newEntry->v.next = entryValue->v.next;
				entryValue->v.next = index;
			}
			else
			{
				next = entry->v.next;
				newEntryValue = &scrVarGlob.variableList[next];
				newEntry = entry;
				nextValue.next = newEntryValue->hash.u.prev;
				nextId = entry->u.next;
				scrVarGlob.variableList[scrVarGlob.variableList[nextValue.next].hash.id].u.next = nextId;
				scrVarGlob.variableList[nextId].hash.u.prev = nextValue.next;
				newEntryValue->hash.id = entry->hash.id;
				entry->hash.id = index;
				newEntryValue->hash.u.prev = entry->hash.u.prev;
				scrVarGlob.variableList[scrVarGlob.variableList[newEntryValue->hash.u.prev].hash.id].nextSibling = next;
				scrVarGlob.variableList[entryValue->nextSibling].hash.u.prev = next;
				entryValue->w.status &= 0xFFFFFF9F;
				entryValue->w.status |= 0x20u;
				newEntry->w.status = 64;
			}
		}
		else
		{
			if ( (entry->w.status & VAR_STAT_MASK) != 0 )
			{
				next = scrVarGlob.variableList->u.next;
				if ( !scrVarGlob.variableList->u.next )
					Scr_TerminalError("exceeded maximum number of script variables");
				newEntryValue = &scrVarGlob.variableList[next];
				newEntry = &scrVarGlob.variableList[newEntryValue->hash.id];
				nextId = newEntry->u.next;
				scrVarGlob.variableList->u.next = nextId;
				scrVarGlob.variableList[nextId].hash.u.prev = 0;
			}
			else
			{
				next = entry->v.next;
				newEntryValue = &scrVarGlob.variableList[next];
				newEntry = entry;
				nextValue.next = newEntryValue->hash.u.prev;
				nextId = entry->u.next;
				scrVarGlob.variableList[scrVarGlob.variableList[nextValue.next].hash.id].u.next = nextId;
				scrVarGlob.variableList[nextId].hash.u.prev = nextValue.next;
			}

			nextValue.o.u.size = entryValue->nextSibling;
			scrVarGlob.variableList[scrVarGlob.variableList[entry->hash.u.prev].hash.id].nextSibling = next;
			scrVarGlob.variableList[nextValue.o.u.size].hash.u.prev = next;

			if ( status == 32 )
			{
				nextValue.o.u.size = entryValue->v.next;
				for ( i = scrVarGlob.variableList[nextValue.o.u.size].hash.id;
				        scrVarGlob.variableList[i].v.next != index;
				        i = scrVarGlob.variableList[scrVarGlob.variableList[i].v.next].hash.id )
				{
					;
				}
				scrVarGlob.variableList[i].v.next = next;
			}
			else
			{
				entryValue->v.next = next;
			}

			newEntryValue->hash.u.prev = entry->hash.u.prev;
			id = newEntryValue->hash.id;
			newEntryValue->hash.id = entry->hash.id;
			entry->hash.id = id;
			newEntry->w.status = 64;
			newEntry->v.next = index;
		}
	}
	else
	{
		next = entry->v.next;
		nextId = entryValue->u.next;

		if ( next == entry->hash.id || (entry->w.status & VAR_STAT_MASK) != 0 )
		{
			newEntry = entryValue;
		}
		else
		{
			scrVarGlob.variableList[next].hash.id = entry->hash.id;
			entry->hash.id = index;
			entryValue->v.next = next;
			entryValue->u.next = entry->u.next;
			newEntry = entry;
		}

		nextValue.next = entry->hash.u.prev;
		scrVarGlob.variableList[scrVarGlob.variableList[nextValue.next].hash.id].u.next = nextId;
		scrVarGlob.variableList[nextId].hash.u.prev = nextValue.next;
		newEntry->w.status = 64;
		newEntry->v.next = index;
	}

	newEntry->w.status = LOBYTE(newEntry->w.status);
	newEntry->w.status |= name << 8;
	parentValue = &scrVarGlob.variableList[parentId];

	if ( (parentValue->w.status & VAR_MASK) == 22 )
	{
		++parentValue->u.o.u.size;
		Scr_GetArrayIndexValue(&value, name);
		AddRefToValue(&value);
	}

	return index;
}

unsigned int GetNewVariableIndexInternal2(unsigned int parentId, unsigned int name, unsigned int index)
{
	unsigned int siblingId;
	VariableValueInternal *entry;
	uint16_t siblingIndex;

	siblingId = GetNewVariableIndexInternal3(parentId, name, index);
	entry = &scrVarGlob.variableList[parentId];
	siblingIndex = entry->nextSibling;
	scrVarGlob.variableList[scrVarGlob.variableList[siblingId].hash.id].nextSibling = siblingIndex;
	scrVarGlob.variableList[siblingIndex].hash.u.prev = siblingId;
	scrVarGlob.variableList[siblingId].hash.u.prev = entry->v.next;
	entry->nextSibling = siblingId;

	return siblingId;
}

unsigned int GetVariableName(unsigned int id)
{
	return scrVarGlob.variableList[id].w.name >> 8;
}

unsigned int GetVariableIndexInternal(unsigned int parentId, unsigned int name)
{
	unsigned int index;

	index = FindVariableIndexInternal(name, (parentId + name) % 0xFFFD + 1);

	if ( !index )
		return GetNewVariableIndexInternal2(parentId, name, (parentId + name) % 0xFFFD + 1);

	return index;
}

unsigned int GetArrayVariableIndex(unsigned int parentId, unsigned int index)
{
	return GetVariableIndexInternal(parentId, (index + 0x800000) & 0xFFFFFF);
}

unsigned int GetArrayVariable(unsigned int parentId, unsigned int index)
{
	return scrVarGlob.variableList[GetArrayVariableIndex(parentId, index)].hash.id;
}

unsigned int GetNewVariableIndexInternal(unsigned int parentId, unsigned int name)
{
	return GetNewVariableIndexInternal2(parentId, name, (parentId + name) % 0xFFFD + 1);
}

unsigned int GetNewVariableIndexReverseInternal2(unsigned int parentId, unsigned int name, unsigned int index)
{
	unsigned int siblingId;
	VariableValueInternal *parent;
	VariableValueInternal *sibling;
	VariableValueInternal_u siblingValue;

	siblingId = GetNewVariableIndexInternal3(parentId, name, index);
	parent = &scrVarGlob.variableList[scrVarGlob.variableList[scrVarGlob.variableList[parentId].nextSibling].hash.u.prev];
	siblingValue.next = parent->hash.u.prev;
	sibling = &scrVarGlob.variableList[scrVarGlob.variableList[siblingValue.next].hash.id];
	scrVarGlob.variableList[scrVarGlob.variableList[siblingId].hash.id].nextSibling = scrVarGlob.variableList[parentId].v.next;
	parent->hash.u.prev = siblingId;
	scrVarGlob.variableList[siblingId].hash.u.prev = siblingValue.next;
	sibling->nextSibling = siblingId;

	return siblingId;
}

unsigned int GetNewVariableIndexReverseInternal(unsigned int parentId, unsigned int name)
{
	return GetNewVariableIndexReverseInternal2(parentId, name, (parentId + name) % 0xFFFD + 1);
}

unsigned int GetNewObjectVariableReverse(unsigned int parentId, unsigned int id)
{
	return scrVarGlob.variableList[GetNewVariableIndexReverseInternal(parentId, id + 0x10000)].hash.id;
}

unsigned int GetNewObjectVariable(unsigned int parentId, unsigned int id)
{
	return scrVarGlob.variableList[GetNewVariableIndexInternal(parentId, id + 0x10000)].hash.id;
}

unsigned int GetObjectVariable(unsigned int parentId, unsigned int id)
{
	return scrVarGlob.variableList[GetVariableIndexInternal(parentId, id + 0x10000)].hash.id;
}

unsigned int GetVariable(unsigned int parentId, unsigned int name)
{
	return scrVarGlob.variableList[GetVariableIndexInternal(parentId, name)].hash.id;
}

void Scr_EvalVariable(VariableValue *val, unsigned int index)
{
	val->type = scrVarGlob.variableList[index].w.status & VAR_MASK;
	val->u = scrVarGlob.variableList[index].u.u;
	AddRefToValue(val);
}

unsigned int GetNewVariable(unsigned int parentId, unsigned int name)
{
	return scrVarGlob.variableList[GetNewVariableIndexInternal(parentId, name)].hash.id;
}

unsigned int GetNewArrayVariableIndex(unsigned int parentId, unsigned int index)
{
	return GetNewVariableIndexInternal(parentId, (index + 0x800000) & 0xFFFFFF);
}

unsigned int GetNewArrayVariable(unsigned int parentId, unsigned int index)
{
	return scrVarGlob.variableList[GetNewArrayVariableIndex(parentId, index)].hash.id;
}

unsigned int GetArraySize(unsigned int id)
{
	return scrVarGlob.variableList[id].u.o.u.size;
}

unsigned int Scr_GetSelf(unsigned int id)
{
	return scrVarGlob.variableList[id].u.o.u.self;
}

void SafeRemoveArrayVariable(unsigned int parentId, unsigned int name)
{
	SafeRemoveVariable(parentId, (name + 0x800000) & 0xFFFFFF);
}

void Scr_GetEntityIdRef(scr_entref_t *entRef, unsigned int entId)
{
	entRef->entnum = scrVarGlob.variableList[entId].u.o.u.entnum;
	entRef->classnum = scrVarGlob.variableList[entId].w.status >> 8;
}

union VariableValueInternal_u* GetVariableValueAddress(unsigned int id)
{
	return &scrVarGlob.variableList[id].u;
}

unsigned int GetVariableKeyObject(unsigned int id)
{
	return (scrVarGlob.variableList[id].w.name >> 8) - 0x10000;
}

unsigned int Scr_GetThreadWaitTime(unsigned int startLocalId)
{
	return scrVarGlob.variableList[startLocalId].w.waitTime >> 8;
}

void Scr_ClearWaitTime(unsigned int startLocalId)
{
	scrVarGlob.variableList[startLocalId].w.status &= 0xFFFFFFE0;
	scrVarGlob.variableList[startLocalId].w.type |= 0xFu;
}

void FreeVariable(unsigned int id)
{
	VariableValueInternal_v value;
	VariableValueInternal *entryValue;
	unsigned short next;
	unsigned short prev;

	entryValue = &scrVarGlob.variableList[id];
	value.next = entryValue->v.next;
	prev = scrVarGlob.variableList[value.next].hash.u.prev;
	next = entryValue->nextSibling;
	scrVarGlob.variableList[next].hash.u.prev = prev;
	scrVarGlob.variableList[scrVarGlob.variableList[prev].hash.id].nextSibling = next;
	entryValue->w.status = 0;
	entryValue->u.next = scrVarGlob.variableList->u.next;
	scrVarGlob.variableList[value.next].hash.u.prev = 0;
	scrVarGlob.variableList[scrVarGlob.variableList->u.next].hash.u.prev = value.next;
	scrVarGlob.variableList->u.next = value.next;
}

void SetNewVariableValue(unsigned int id, VariableValue *value)
{
	scrVarGlob.variableList[id].w.type |= value->type;
	scrVarGlob.variableList[id].u.u = value->u;
}

void RemoveRefToEmptyObject(unsigned int id)
{
	if ( scrVarGlob.variableList[id].u.refCount )
		--scrVarGlob.variableList[id].u.refCount;
	else
		FreeVariable(id);
}

void RemoveRefToValueInternal(int type, VariableUnion u)
{
	if ( type > VAR_ISTRING )
	{
		if ( type == VAR_VECTOR )
			RemoveRefToVector(u.vectorValue);
	}
	else if ( type >= VAR_STRING )
	{
		SL_RemoveRefToString(u.stringValue);
	}
	else if ( type == VAR_OBJECT )
	{
		RemoveRefToObject(u.stringValue);
	}
}

void AddRefToObject(unsigned int id)
{
	++scrVarGlob.variableList[id].u.refCount;
}

void AddRefToValueInternal(int type, VariableUnion u)
{
	if ( type > VAR_ISTRING )
	{
		if ( type == VAR_VECTOR )
			AddRefToVector(u.vectorValue);
	}
	else if ( type >= VAR_STRING )
	{
		SL_AddRefToString(u.stringValue);
	}
	else if ( type == VAR_OBJECT )
	{
		AddRefToObject(u.stringValue);
	}
}

void AddRefToValue(VariableValue *val)
{
	AddRefToValueInternal(val->type, val->u);
}

void FreeChildValue(unsigned int id)
{
	RemoveRefToValueInternal(scrVarGlob.variableList[id].w.status & VAR_MASK, scrVarGlob.variableList[id].u.u);
	FreeVariable(id);
}

void ClearObjectInternal(unsigned int parentId)
{
	unsigned int prevId;
	unsigned int i;
	unsigned int id;
	VariableValueInternal *parentValue;
	VariableValueInternal *value;

	parentValue = &scrVarGlob.variableList[parentId];
	value = &scrVarGlob.variableList[parentValue->nextSibling];

	for ( i = value->hash.id; i != parentId; i = value->hash.id )
	{
		MakeVariableExternal(value, parentValue);
		value = &scrVarGlob.variableList[scrVarGlob.variableList[i].nextSibling];
	}

	id = scrVarGlob.variableList[parentValue->nextSibling].hash.id;

	while ( id != parentId )
	{
		prevId = id;
		id = scrVarGlob.variableList[scrVarGlob.variableList[id].nextSibling].hash.id;
		FreeChildValue(prevId);
	}
}

void ClearObject(unsigned int parentId)
{
	AddRefToObject(parentId);
	ClearObjectInternal(parentId);
	RemoveRefToEmptyObject(parentId);
}

void RemoveVariable(unsigned int parentId, unsigned int index)
{
	unsigned int id;
	VariableValueInternal *value;

	value = &scrVarGlob.variableList[FindVariableIndex(parentId, index)];
	id = value->hash.id;
	MakeVariableExternal(value, &scrVarGlob.variableList[parentId]);
	FreeChildValue(id);
}

void RemoveNextVariable(unsigned int index)
{
	uint16_t id;
	VariableValueInternal *value;

	value = &scrVarGlob.variableList[scrVarGlob.variableList[index].nextSibling];
	id = value->hash.id;
	MakeVariableExternal(value, &scrVarGlob.variableList[index]);
	FreeChildValue(id);
}

void RemoveObjectVariable(unsigned int parentId, unsigned int id)
{
	RemoveVariable(parentId, id + 0x10000);
}

void RemoveArrayVariable(unsigned int parentId, unsigned int unsignedValue)
{
	RemoveVariable(parentId, (unsignedValue + 0x800000) & 0xFFFFFF);
}

void RemoveRefToObject(unsigned int id)
{
	VariableValueInternal *entryValue;

	entryValue = &scrVarGlob.variableList[id];

	if ( entryValue->u.refCount )
	{
		if ( !--entryValue->u.refCount
		        && (entryValue->w.status & VAR_MASK) == 21
		        && scrVarGlob.variableList[entryValue->nextSibling].hash.id == id )
		{
			entryValue->w.status &= 0xFFFFFFE0;
			entryValue->w.status |= 0x14u;
			RemoveArrayVariable(scrClassMap[entryValue->w.classnum >> 8].entArrayId, entryValue->u.o.u.size);
		}
	}
	else
	{
		if ( scrVarGlob.variableList[entryValue->nextSibling].hash.id != id )
			ClearObject(id);
		FreeVariable(id);
	}
}

void RemoveRefToValue(VariableValue *value)
{
	RemoveRefToValueInternal(value->type, value->u);
}

unsigned int GetParentLocalId(unsigned int threadId)
{
	return scrVarGlob.variableList[threadId].w.name >> 8;
}

unsigned int GetSafeParentLocalId(unsigned int threadId)
{
	if ( (scrVarGlob.variableList[threadId].w.status & VAR_MASK) == 18 )
		return scrVarGlob.variableList[threadId].w.name >> 8;
	else
		return 0;
}

unsigned int GetStartLocalId(unsigned int threadId)
{
	while ( (scrVarGlob.variableList[threadId].w.status & VAR_MASK) == 18 )
		threadId = scrVarGlob.variableList[threadId].w.name >> 8;

	return threadId;
}

unsigned int Scr_EvalFieldObject(unsigned int tempVariable, VariableValue *value)
{
	VariableValue tempValue;
	int type;

	type = value->type;

	if ( type == VAR_OBJECT && (type = scrVarGlob.variableList[value->u.intValue].w.status & 0x1F, type <= VAR_ENTITY) )
	{
		tempValue.type = VAR_OBJECT;
		tempValue.u.intValue = value->u.intValue;
		SetVariableValue(tempVariable, &tempValue);
		return tempValue.u.intValue;
	}
	else
	{
		RemoveRefToValue(value);
		Scr_Error(va("%s is not a field object", var_typename[type]));
		return 0;
	}
}

void Scr_CastBool(VariableValue *value)
{
	int type;

	if ( value->type == VAR_INTEGER )
	{
		value->u.intValue = value->u.intValue != 0;
	}
	else if ( value->type == VAR_FLOAT )
	{
		value->type = VAR_INTEGER;
		value->u.intValue = 0.0 != value->u.floatValue;
	}
	else
	{
		type = value->type;
		RemoveRefToValue(value);
		value->type = VAR_UNDEFINED;
		Scr_Error(va("cannot cast %s to bool", var_typename[type]));
	}
}

void Scr_EvalBoolNot(VariableValue *value)
{
	Scr_CastBool(value);

	if ( value->type == VAR_INTEGER )
		value->u.intValue = value->u.intValue == 0;
}

void Scr_EvalBoolComplement(VariableValue *value)
{
	int type;

	if ( value->type == VAR_INTEGER )
	{
		value->u.intValue = ~value->u.intValue;
	}
	else
	{
		type = value->type;
		RemoveRefToValue(value);
		value->type = VAR_UNDEFINED;
		Scr_Error(va("~ cannot be applied to \"%s\"", var_typename[type]));
	}
}

unsigned int Scr_EvalVariableObject(unsigned int id)
{
	int type;
	VariableValueInternal *entryValue;

	entryValue = &scrVarGlob.variableList[id];
	type = entryValue->w.status & 0x1F;

	if ( type == VAR_OBJECT )
	{
		type = scrVarGlob.variableList[entryValue->u.u.intValue].w.status & 0x1F;

		if ( type <= VAR_ENTITY )
			return entryValue->u.u.stringValue;
	}

	Scr_Error(va("%s is not a field object", var_typename[type]));
	return 0;
}

unsigned int AllocVariable()
{
	uint16_t newIndex;
	uint16_t next;
	VariableValueInternal *entryValue;
	VariableValueInternal *entry;
	uint16_t index;

	index = scrVarGlob.variableList->u.next;

	if ( !scrVarGlob.variableList->u.next )
		Scr_TerminalError("exceeded maximum number of script variables");

	entry = &scrVarGlob.variableList[index];
	entryValue = &scrVarGlob.variableList[entry->hash.id];
	next = entryValue->u.next;

	if ( entry != entryValue && (entry->w.status & VAR_STAT_MASK) == 0 )
	{
		newIndex = entry->v.next;
		scrVarGlob.variableList[newIndex].hash.id = entry->hash.id;
		entry->hash.id = index;
		entryValue->v.next = newIndex;
		entryValue->u.next = entry->u.next;
		entryValue = &scrVarGlob.variableList[index];
	}

	scrVarGlob.variableList->u.next = next;
	scrVarGlob.variableList[next].hash.u.prev = 0;
	entryValue->v.next = index;
	entryValue->nextSibling = index;
	entry->hash.u.prev = index;

	return entry->hash.id;
}

unsigned int AllocEntity(unsigned int classnum, unsigned short entnum)
{
	unsigned int id;
	VariableValueInternal *entryValue;

	id = AllocVariable();

	entryValue = &scrVarGlob.variableList[id];
	entryValue->w.status = VAR_STAT_EXTERNAL;
	entryValue->w.type |= 0x15u;
	entryValue->w.classnum  |= classnum << 8;
	entryValue->u.refCount = 0;
	entryValue->u.o.u.entnum = entnum;

	return id;
}

unsigned int AllocThread(unsigned int self)
{
	unsigned int id;
	VariableValueInternal *entryValue;

	id = AllocVariable();

	entryValue = &scrVarGlob.variableList[id];
	entryValue->w.status = 96;
	entryValue->w.type |= 0xFu;
	entryValue->u.refCount = 0;
	entryValue->u.o.u.self = self;

	return id;
}

unsigned int AllocChildThread(unsigned int self, unsigned int parentLocalId)
{
	unsigned int id;
	VariableValueInternal *entryValue;

	id = AllocVariable();

	entryValue = &scrVarGlob.variableList[id];
	entryValue->w.status = 96;
	entryValue->w.type |= 0x12u;
	entryValue->w.name |= parentLocalId << 8;
	entryValue->u.refCount = 0;
	entryValue->u.o.u.self = self;

	return id;
}

float* Scr_AllocVectorInternal()
{
	RefVector *refVec;

	refVec = (RefVector *)MT_Alloc(sizeof(RefVector));
	refVec->head = 0;

	return refVec->vec;
}

float* Scr_AllocVector(const float* vec)
{
	float* avec = Scr_AllocVectorInternal();

	VectorCopy(vec, avec);

	return avec;
}

unsigned int Scr_AllocArray()
{
	VariableValueInternal *entryValue;
	unsigned int id;

	id = AllocVariable();

	entryValue = &scrVarGlob.variableList[id];
	entryValue->w.status = VAR_STAT_EXTERNAL;
	entryValue->w.type |= 0x16u;
	entryValue->u.refCount = 0;
	entryValue->u.o.u.entnum = 0;

	return id;
}

unsigned int Scr_GetEntityId(int entnum, unsigned int classnum)
{
	VariableValueInternal *entryValue;
	unsigned int entId;

	entryValue = &scrVarGlob.variableList[GetArrayVariable(scrClassMap[classnum].entArrayId, entnum)];

	if ( (entryValue->w.status & VAR_MASK) != 0 )
		return entryValue->u.u.pointerValue;

	entId = AllocEntity(classnum, entnum);
	entryValue->w.type |= 1u;
	entryValue->u.u.intValue = entId;

	return entId;
}

void Scr_FreeEntityNum(int entnum, unsigned int classnum)
{
	VariableValueInternal *entryValue;
	unsigned int object;
	unsigned int id;
	unsigned int entArrayId;

	if ( scrVarPub.bInited )
	{
		entArrayId = scrClassMap[classnum].entArrayId;
		id = FindArrayVariable(entArrayId, entnum);

		if ( id )
		{
			object = FindObject(id);
			entryValue = &scrVarGlob.variableList[object];
			entryValue->w.status &= 0xFFFFFFE0;
			entryValue->w.type |= 0x14u;
			AddRefToObject(object);
			entryValue->u.o.u.size = scrVarPub.freeEntList;
			scrVarPub.freeEntList = object;
			RemoveArrayVariable(entArrayId, entnum);
		}
	}
}

unsigned short Scr_GetThreadNotifyName(unsigned int startLocalId)
{
	return scrVarGlob.variableList[startLocalId].w.notifyName >> 8;
}

void Scr_RemoveThreadNotifyName(unsigned int startLocalId)
{
	VariableValueInternal *entryValue;
	unsigned short stringValue;

	entryValue = &scrVarGlob.variableList[startLocalId];
	stringValue = Scr_GetThreadNotifyName(startLocalId);
	SL_RemoveRefToString(stringValue);
	entryValue->w.status &= 0xFFFFFFE0;
	entryValue->w.type |= 0xFu;
}

void Scr_ClearThread(unsigned int parentId)
{
	if ( scrVarGlob.variableList[scrVarGlob.variableList[parentId].nextSibling].hash.id != parentId )
		ClearObjectInternal(parentId);

	RemoveRefToObject(scrVarGlob.variableList[parentId].u.o.u.self);
}

void Scr_KillEndonThread(unsigned int threadId)
{
	VariableValueInternal *entryValue;

	entryValue = &scrVarGlob.variableList[threadId];
	RemoveRefToObject(entryValue->u.o.u.self);
	entryValue->w.status &= 0xFFFFFFE0;
	entryValue->w.type |= 0x17u;
}

void Scr_KillThread(unsigned int parentId)
{
	unsigned int object;
	union VariableValueInternal_u *VariableValueAddress;
	unsigned int startLocalId;
	unsigned int NextSibling;
	unsigned int id;
	unsigned int ObjectVariable;
	VariableValueInternal *entryValue;

	entryValue = &scrVarGlob.variableList[parentId];
	Scr_ClearThread(parentId);
	ObjectVariable = FindObjectVariable(scrVarPub.pauseArrayId, parentId);

	if ( ObjectVariable )
	{
		for ( id = FindObject(ObjectVariable); ; RemoveObjectVariable(id, startLocalId) )
		{
			NextSibling = FindNextSibling(id);

			if ( !NextSibling )
				break;

			startLocalId = (unsigned short)(scrVarGlob.variableList[NextSibling].w.status >> 8);
			object = FindObjectVariable(id, startLocalId);
			VariableValueAddress = GetVariableValueAddress(object);
			VM_CancelNotify(VariableValueAddress->u.intValue, startLocalId);
			Scr_KillEndonThread(startLocalId);
		}

		RemoveObjectVariable(scrVarPub.pauseArrayId, parentId);
	}

	entryValue->w.status &= 0xFFFFFFE0;
	entryValue->w.type |= 0x17u;
}

unsigned int AllocObject()
{
	VariableValueInternal *entryValue;
	unsigned int id;

	id = AllocVariable();

	scrVarGlob.variableList[id].w.status = VAR_STAT_EXTERNAL;
	entryValue = &scrVarGlob.variableList[id];
	entryValue->w.type |= 0x13u;
	entryValue->u.refCount = 0;

	return id;
}

unsigned int AllocValue()
{
	unsigned int index;

	index = AllocVariable();
	scrVarGlob.variableList[index].w.status = VAR_STAT_EXTERNAL;

	return index;
}

bool IsValidArrayIndex(unsigned int index)
{
	return index < 0x800000;
}

unsigned int GetInternalVariableIndex(unsigned int index)
{
	return (index + 0x800000) & 0xFFFFFF;
}

unsigned int FindArrayVariableIndex(unsigned int parentId, unsigned int index)
{
	return FindVariableIndex(parentId, (index + 0x800000) & 0xFFFFFF);
}

unsigned int FindArrayVariable(unsigned int parentId, unsigned int index)
{
	return scrVarGlob.variableList[FindArrayVariableIndex(parentId, index)].hash.id;
}

bool IsObjectFree(unsigned int id)
{
	return (scrVarGlob.variableList[id].w.status & 0x60) == 0;
}

int Scr_GetClassnumForCharId(char charId)
{
	int i;

	for ( i = 0; i < CLASS_NUM_COUNT; ++i )
	{
		if ( scrClassMap[i].charId == charId )
			return i;
	}

	return -1;
}

unsigned int Scr_FindArrayIndex(unsigned int parentId, VariableValue *index)
{
	unsigned int varIndex;

	if ( index->type == VAR_INTEGER )
	{
		if ( IsValidArrayIndex(index->u.intValue) )
		{
			return FindArrayVariable(parentId, index->u.intValue);
		}
		else
		{
			Scr_Error(va("array index %d out of range", index->u.intValue));
			AddRefToObject(parentId);
			return 0;
		}
	}
	else if ( index->type == VAR_STRING )
	{
		varIndex = FindVariable(parentId, index->u.stringValue);
		SL_RemoveRefToString(index->u.stringValue);
		return varIndex;
	}
	else
	{
		Scr_Error(va("%s is not an array index", var_typename[index->type]));
		AddRefToObject(parentId);
		return 0;
	}
}

void Scr_EvalArray(VariableValue *value, VariableValue *index)
{
	unsigned int ArrayIndex;
	int type;
	VariableValue val;
	char str[2];
	const char *string;
	VariableValueInternal *entryValue;

	type = value->type;

	if ( type == VAR_STRING )
	{
		if ( index->type == VAR_INTEGER )
		{
			if ( index->u.intValue < 0
			        || (string = SL_ConvertToString(value->u.intValue), index->u.intValue >= strlen(string)) )
			{
				Scr_Error(va("string index %d out of range", index->u.intValue));
			}
			else
			{
				index->type = VAR_STRING;
				str[0] = string[index->u.intValue];
				str[1] = 0;
				index->u.intValue = SL_GetStringOfLen(str, 0, sizeof(str));
				SL_RemoveRefToString(value->u.intValue);
			}
		}
		else
		{
			Scr_Error(va("%s is not a string index", var_typename[index->type]));
		}
	}
	else if ( type > VAR_STRING )
	{
		if ( type != VAR_VECTOR )
		{
			scrVarPub.error_index = 1;
			Scr_Error(va("%s is not an array, string, or vector", var_typename[value->type]));
			return;
		}

		if ( index->type == VAR_INTEGER )
		{
			if ( index->u.intValue > 2 )
			{
				Scr_Error(va("vector index %d out of range", index->u.intValue));
			}
			else
			{
				index->type = VAR_FLOAT;
				index->u.floatValue = value->u.vectorValue[index->u.intValue];
				RemoveRefToVector(value->u.vectorValue);
			}
		}
		else
		{
			Scr_Error(va("%s is not a vector index", var_typename[index->type]));
		}
	}
	else
	{
		if ( type != VAR_OBJECT )
		{
			scrVarPub.error_index = 1;
			Scr_Error(va("%s is not an array, string, or vector", var_typename[value->type]));
			return;
		}

		entryValue = &scrVarGlob.variableList[value->u.intValue];

		if ( (entryValue->w.status & VAR_MASK) == VAR_ARRAY )
		{
			ArrayIndex = Scr_FindArrayIndex(value->u.intValue, index);
			Scr_EvalVariable(&val, ArrayIndex);
			index->u.intValue = val.u.intValue;
			index->type = val.type;
			RemoveRefToObject(value->u.intValue);
		}
		else
		{
			scrVarPub.error_index = 1;
			Scr_Error(va("%s is not an array", var_typename[entryValue->w.status & VAR_MASK]));
		}
	}
}

unsigned int Scr_EvalArrayIndex(unsigned int parentId, VariableValue *var)
{
	unsigned int index;

	if ( var->type == VAR_INTEGER )
	{
		if ( IsValidArrayIndex(var->u.intValue) )
		{
			return GetArrayVariable(parentId, var->u.intValue);
		}
		else
		{
			Scr_Error(va("array index %d out of range", var->u.intValue));
			return 0;
		}
	}
	else if ( var->type == VAR_STRING )
	{
		index = GetVariable(parentId, var->u.stringValue);
		SL_RemoveRefToString(var->u.stringValue);
		return index;
	}
	else
	{
		Scr_Error(va("%s is not an array index", var_typename[var->type]));
		return 0;
	}
}

unsigned int Scr_GetVariableField(unsigned int fieldId, unsigned int index)
{
	unsigned int type;
	unsigned int var;

	type = scrVarGlob.variableList[fieldId].w.status & 0x1F;

	if ( type <= 0x13 )
		return GetVariable(fieldId, index);

	if ( type == 21 )
	{
		var = FindVariable(fieldId, index);

		if ( var )
		{
			return var;
		}
		else
		{
			scrVarPub.entId = fieldId;
			scrVarPub.entFieldName = index;
			return 65534;
		}
	}
	else
	{
		Scr_Error(va("cannot set field of %s", var_typename[type]));
		return 0;
	}
}

void CopyArray(unsigned int parentId, unsigned int newParentId)
{
	int i;
	VariableValueInternal *newEntryValue;
	VariableValueInternal *entryValue;
	int type;

	for ( i = scrVarGlob.variableList[scrVarGlob.variableList[parentId].nextSibling].hash.id;
	        i != parentId;
	        i = scrVarGlob.variableList[scrVarGlob.variableList[i].nextSibling].hash.id )
	{
		entryValue = &scrVarGlob.variableList[i];
		type = entryValue->w.status & 0x1F;

		newEntryValue = &scrVarGlob.variableList[scrVarGlob.variableList[GetVariableIndexInternal(
		                    newParentId,
		                    entryValue->w.classnum >> 8)].hash.id];

		newEntryValue->w.type |= type;

		if ( type == VAR_OBJECT )
		{
			if ( (scrVarGlob.variableList[entryValue->u.u.pointerValue].w.status & 0x1F) == 22 )
			{
				newEntryValue->u.u.pointerValue = Scr_AllocArray();
				CopyArray(entryValue->u.u.pointerValue, newEntryValue->u.u.pointerValue);
			}
			else
			{
				newEntryValue->u.u = entryValue->u.u;
				AddRefToObject(entryValue->u.u.pointerValue);
			}
		}
		else
		{
			newEntryValue->u.u = entryValue->u.u;
			AddRefToValueInternal(type, entryValue->u.u);
		}
	}
}

void Scr_EvalVariableEntityField(VariableValue *pValue, unsigned int entId, unsigned int name)
{
	int type;
	VariableValue val;
	unsigned int id;
	unsigned int fieldId;
	VariableValueInternal *entryValue;
	VariableValueInternal *objectValue;

	entryValue = &scrVarGlob.variableList[entId];
	fieldId = FindArrayVariable(scrClassMap[(entryValue->w.status >> 8)].id, name);

	if ( fieldId )
	{
		GetEntityFieldValue(
		    &val,
		    entryValue->w.status >> 8,
		    entryValue->u.o.u.size,
		    scrVarGlob.variableList[fieldId].u.u.intValue);

		type = val.type;
		pValue->u.intValue = val.u.intValue;
		pValue->type = type;

		if ( pValue->type == VAR_OBJECT )
		{
			objectValue = &scrVarGlob.variableList[pValue->u.pointerValue];

			if ( (objectValue->w.status & 0x1F) == 22 )
			{
				if ( objectValue->u.refCount )
				{
					id = pValue->u.pointerValue;
					RemoveRefToObject(id);
					pValue->u.pointerValue = Scr_AllocArray();
					CopyArray(id, pValue->u.pointerValue);
				}
			}
		}
	}
	else
	{
		pValue->u.intValue = 0;
		pValue->type = VAR_UNDEFINED;
	}
}

void Scr_FindVariableFieldInternal(VariableValue *pValue, unsigned int parentId, unsigned int name)
{
	unsigned int index;

	index = FindVariable(parentId, name);

	if ( index )
	{
		Scr_EvalVariable(pValue, index);
	}
	else if ( (scrVarGlob.variableList[parentId].w.status & 0x1F) == 21 )
	{
		Scr_EvalVariableEntityField(pValue, parentId, name);
	}
	else
	{
		pValue->u.intValue = 0;
		pValue->type = VAR_UNDEFINED;
	}
}

VariableValue Scr_FindVariableField(unsigned int parentId, unsigned int name)
{
	VariableValue value;

	Scr_FindVariableFieldInternal(&value, parentId, name);
	return value;
}

void Scr_EvalVariableFieldInternal(VariableValue *pValue, unsigned int id)
{
	VariableValue value;

	if ( id == 65534 )
	{
		Scr_EvalVariableEntityField(&value, scrVarPub.entId, scrVarPub.entFieldName);
	}
	else
	{
		Scr_EvalVariable(&value, id);
	}

	*pValue = value;
}

void Scr_CastDebugString(VariableValue *value)
{
	const char *s;
	unsigned int stringValue;

	switch ( value->type )
	{
	case VAR_OBJECT:
		stringValue = SL_GetString_(var_typename[value->u.pointerValue], 0);
		break;
	case VAR_STRING:
	case VAR_VECTOR:
	case VAR_FLOAT:
	case VAR_INTEGER:
		Scr_CastString(value);
		return;
	case VAR_ISTRING:
		value->type = VAR_STRING;
		return;
	case VAR_ANIMATION:
		s = XAnimGetAnimDebugName(Scr_GetAnims(value->u.pointerValue >> 16), (uint16_t)value->u.pointerValue);
		stringValue = SL_GetString_(s, 0);
		break;
	default:
		stringValue = SL_GetString_(var_typename[value->type], 0);
		break;
	}

	RemoveRefToValue(value);
	value->type = VAR_STRING;
	value->u.stringValue = stringValue;
}

void Scr_UnmatchingTypesError(VariableValue *value1, VariableValue *value2)
{
	const char *error;

	if ( scrVarPub.error_message )
	{
		error = 0;
	}
	else
	{
		Scr_CastDebugString(value1);
		Scr_CastDebugString(value2);
		error = va(
		            "pair '%s' and '%s' has unmatching types '%s' and '%s'",
		            SL_ConvertToString(value1->u.stringValue),
		            SL_ConvertToString(value2->u.stringValue),
		            var_typename[value1->type],
		            var_typename[value2->type]);
	}
	RemoveRefToValue(value1);
	value1->type = VAR_UNDEFINED;
	RemoveRefToValue(value2);
	value2->type = VAR_UNDEFINED;
	Scr_Error(error);
}

VariableValue Scr_EvalVariableField(unsigned int id)
{
	VariableValue value;

	Scr_EvalVariableFieldInternal(&value, id);
	return value;
}

void Scr_EvalOr(VariableValue *value1, VariableValue *value2)
{
	if ( value1->type == VAR_INTEGER && value2->type == VAR_INTEGER )
		value1->u.intValue |= value2->u.intValue;
	else
		Scr_UnmatchingTypesError(value1, value2);
}

void Scr_EvalExOr(VariableValue *value1, VariableValue *value2)
{
	if ( value1->type == VAR_INTEGER && value2->type == VAR_INTEGER )
		value1->u.intValue ^= value2->u.intValue;
	else
		Scr_UnmatchingTypesError(value1, value2);
}

void Scr_EvalAnd(VariableValue *value1, VariableValue *value2)
{
	if ( value1->type == VAR_INTEGER && value2->type == VAR_INTEGER )
		value1->u.intValue &= value2->u.intValue;
	else
		Scr_UnmatchingTypesError(value1, value2);
}

void Scr_CastWeakerPair(VariableValue *value1, VariableValue *value2)
{
	int type1;
	int type2;

	type1 = value1->type;
	type2 = value2->type;

	if ( type1 != type2 )
	{
		if ( type1 == VAR_FLOAT && type2 == VAR_INTEGER )
		{
			value2->type = VAR_FLOAT;
			value2->u.floatValue = (float)value2->u.intValue;
		}
		else if ( type1 == VAR_INTEGER && type2 == VAR_FLOAT )
		{
			value1->type = VAR_FLOAT;
			value1->u.floatValue = (float)value1->u.intValue;
		}
		else
		{
			Scr_UnmatchingTypesError(value1, value2);
		}
	}
}

void Scr_EvalEquality(VariableValue *value1, VariableValue *value2)
{
	int tempInt;
	int equal;

	Scr_CastWeakerPair(value1, value2);

	switch ( value1->type )
	{
	case VAR_UNDEFINED:
		value1->type = VAR_INTEGER;
		value1->u.intValue = 1;
		break;

	case VAR_STRING:
	case VAR_ISTRING:
		value1->type = VAR_INTEGER;
		tempInt = value1->u.intValue == value2->u.intValue;
		SL_RemoveRefToString(value1->u.intValue);
		SL_RemoveRefToString(value2->u.intValue);
		value1->u.intValue = tempInt;
		break;

	case VAR_VECTOR:
		value1->type = VAR_INTEGER;
		equal = VectorCompare(value1->u.vectorValue, value2->u.vectorValue);
		RemoveRefToVector(value1->u.vectorValue);
		RemoveRefToVector(value2->u.vectorValue);
		value1->u.intValue = equal;
		break;

	case VAR_FLOAT:
		value1->type = VAR_INTEGER;
		value1->u.intValue = fabs(value1->u.floatValue - value2->u.floatValue) < 0.000001;
		break;

	case VAR_INTEGER:
		value1->u.intValue = value1->u.intValue == value2->u.intValue;
		break;

	case VAR_FUNCTION:
		value1->type = VAR_INTEGER;
		value1->u.intValue = value1->u.intValue == value2->u.intValue;
		break;

	case VAR_ANIMATION:
		value1->type = VAR_INTEGER;
		value1->u.intValue = value1->u.intValue == value2->u.intValue;
		break;

	case VAR_OBJECT:
		if ( !(((scrVarGlob.variableList[value1->u.intValue].w.type & VAR_MASK) == VAR_ARRAY ||
		        (scrVarGlob.variableList[value2->u.intValue].w.type & VAR_MASK) == VAR_ARRAY)
		        && !scrVarPub.evaluate ))
		{
			value1->type = VAR_INTEGER;
			equal = value1->u.intValue == value2->u.intValue;
			RemoveRefToObject(value1->u.intValue);
			RemoveRefToObject(value2->u.intValue);
			value1->u.intValue = equal;
			break;
		}

	default:
		Scr_UnmatchingTypesError(value1, value2);
		break;
	}
}

void Scr_EvalInequality(VariableValue *value1, VariableValue *value2)
{
	Scr_EvalEquality(value1, value2);
	value1->u.intValue = value1->u.intValue == 0;
}

void Scr_EvalLess(VariableValue *value1, VariableValue *value2)
{
	int type;

	Scr_CastWeakerPair(value1, value2);
	type = value1->type;

	if ( type == VAR_FLOAT )
	{
		value1->type = VAR_INTEGER;
		value1->u.intValue = value2->u.floatValue > (float)value1->u.floatValue;
	}
	else if ( type == VAR_INTEGER )
	{
		value1->u.intValue = value1->u.intValue < value2->u.intValue;
	}
	else
	{
		Scr_UnmatchingTypesError(value1, value2);
	}
}

void Scr_EvalGreater(VariableValue *value1, VariableValue *value2)
{
	int type;

	Scr_CastWeakerPair(value1, value2);
	type = value1->type;

	if ( type == VAR_FLOAT )
	{
		value1->type = VAR_INTEGER;
		value1->u.intValue = value1->u.floatValue > (float)value2->u.floatValue;
	}
	else if ( type == VAR_INTEGER )
	{
		value1->u.intValue = value1->u.intValue > value2->u.intValue;
	}
	else
	{
		Scr_UnmatchingTypesError(value1, value2);
	}
}

void Scr_EvalLessEqual(VariableValue *value1, VariableValue *value2)
{
	Scr_EvalGreater(value1, value2);
	value1->u.intValue = value1->u.intValue == 0;
}

void Scr_EvalGreaterEqual(VariableValue *value1, VariableValue *value2)
{
	Scr_EvalLess(value1, value2);
	value1->u.intValue = value1->u.intValue == 0;
}

void Scr_EvalShiftLeft(VariableValue *value1, VariableValue *value2)
{
	if ( value1->type == VAR_INTEGER && value2->type == VAR_INTEGER )
		value1->u.intValue <<= value2->u.intValue;
	else
		Scr_UnmatchingTypesError(value1, value2);
}

void Scr_EvalShiftRight(VariableValue *value1, VariableValue *value2)
{
	if ( value1->type == VAR_INTEGER && value2->type == VAR_INTEGER )
		value1->u.intValue >>= value2->u.intValue;
	else
		Scr_UnmatchingTypesError(value1, value2);
}

void Scr_CastWeakerStringPair(VariableValue *value1, VariableValue *value2)
{
	const float *constTempVector;
	int type1;
	int type2;

	type1 = value1->type;
	type2 = value2->type;

	if ( type1 != type2 )
	{
		if ( type1 < type2 )
		{
			if ( type1 == VAR_STRING )
			{
				switch ( type2 )
				{
				case VAR_VECTOR:
					value2->type = VAR_STRING;
					constTempVector = value2->u.vectorValue;
					value2->u.intValue = SL_GetStringForVector(value2->u.vectorValue);
					RemoveRefToVector(constTempVector);
					return;

				case VAR_FLOAT:
					value2->type = VAR_STRING;
					value2->u.intValue = SL_GetStringForFloat(value2->u.floatValue);
					return;

				case VAR_INTEGER:
					value2->type = VAR_STRING;
					value2->u.intValue = SL_GetStringForInt(value2->u.intValue);
					return;
				}
			}
			else if ( type1 != VAR_FLOAT )
			{
				Scr_UnmatchingTypesError(value1, value2);
				return;
			}

			if ( type2 == VAR_INTEGER )
			{
				value2->type = VAR_FLOAT;
				value2->u.floatValue = (float)value2->u.intValue;
				return;
			}

			Scr_UnmatchingTypesError(value1, value2);
			return;
		}
		if ( type2 == VAR_STRING )
		{
			switch ( type1 )
			{
			case VAR_VECTOR:
				value1->type = VAR_STRING;
				constTempVector = value1->u.vectorValue;
				value1->u.intValue = SL_GetStringForVector(value1->u.vectorValue);
				RemoveRefToVector(constTempVector);
				return;

			case VAR_FLOAT:
				value1->type = VAR_STRING;
				value1->u.intValue = SL_GetStringForFloat(value1->u.floatValue);
				return;

			case VAR_INTEGER:
				value1->type = VAR_STRING;
				value1->u.intValue = SL_GetStringForInt(value1->u.intValue);
				return;
			}
		}
		else if ( type2 != VAR_FLOAT )
		{
			Scr_UnmatchingTypesError(value1, value2);
			return;
		}

		if ( type1 == VAR_INTEGER )
		{
			value1->type = VAR_FLOAT;
			value1->u.floatValue = (float)value1->u.intValue;
			return;
		}

		Scr_UnmatchingTypesError(value1, value2);
		return;
	}
}

void Scr_EvalPlus(VariableValue *value1, VariableValue *value2)
{
	unsigned int s;
	float *vec;
	int s1len;
	const char *s1;
	const char *s2;
	char str[8192];
	unsigned int len;

	Scr_CastWeakerStringPair(value1, value2);

	switch ( value1->type )
	{
	case VAR_STRING:
		s1 = SL_ConvertToString(value1->u.stringValue);
		s2 = SL_ConvertToString(value2->u.stringValue);
		s1len = SL_GetStringLen(value1->u.stringValue);
		len = s1len + SL_GetStringLen(value2->u.stringValue) + 1;

		if (len >= sizeof(str))
		{
			SL_RemoveRefToString(value1->u.stringValue);
			SL_RemoveRefToString(value2->u.stringValue);
			value1->type = VAR_UNDEFINED;
			value2->type = VAR_UNDEFINED;
			Scr_Error(va("cannot concat \"%s\" and \"%s\" - max string length exceeded", s1, s2));
			return;
		}

		strcpy(str, s1);
		strcpy(str + s1len, s2);

		s = SL_GetStringOfLen(str, 0, len);

		SL_RemoveRefToString(value1->u.stringValue);
		SL_RemoveRefToString(value2->u.stringValue);
		value1->u.stringValue = s;
		break;

	case VAR_VECTOR:
		vec = Scr_AllocVectorInternal();
		VectorAdd(value1->u.vectorValue, value2->u.vectorValue, vec);
		RemoveRefToVector(value1->u.vectorValue);
		RemoveRefToVector(value2->u.vectorValue);
		value1->u.vectorValue = vec;
		break;

	case VAR_FLOAT:
		value1->u.floatValue = value1->u.floatValue + value2->u.floatValue;
		break;

	case VAR_INTEGER:
		value1->u.intValue += value2->u.intValue;
		break;

	default:
		Scr_UnmatchingTypesError(value1, value2);
		break;
	}
}

void Scr_EvalMinus(VariableValue *value1, VariableValue *value2)
{
	float *tempVector;

	Scr_CastWeakerPair(value1, value2);

	switch ( value1->type )
	{
	case VAR_VECTOR:
		tempVector = Scr_AllocVectorInternal();
		VectorSubtract(value1->u.vectorValue, value2->u.vectorValue, tempVector);
		RemoveRefToVector(value1->u.vectorValue);
		RemoveRefToVector(value2->u.vectorValue);
		value1->u.vectorValue = tempVector;
		break;

	case VAR_FLOAT:
		value1->u.floatValue = value1->u.floatValue - value2->u.floatValue;
		break;

	case VAR_INTEGER:
		value1->u.intValue -= value2->u.intValue;
		break;

	default:
		Scr_UnmatchingTypesError(value1, value2);
		break;
	}
}

void Scr_EvalMultiply(VariableValue *value1, VariableValue *value2)
{
	int type;

	Scr_CastWeakerPair(value1, value2);
	type = value1->type;

	if ( type == VAR_FLOAT )
	{
		value1->u.floatValue = value1->u.floatValue * value2->u.floatValue;
	}
	else if ( type == VAR_INTEGER )
	{
		value1->u.intValue *= value2->u.intValue;
	}
	else
	{
		Scr_UnmatchingTypesError(value1, value2);
	}
}

void Scr_EvalDivide(VariableValue *value1, VariableValue *value2)
{
	int type;

	Scr_CastWeakerPair(value1, value2);
	type = value1->type;

	if ( type == VAR_FLOAT )
	{
		if ( value2->u.floatValue == 0.0 )
		{
			value1->u.intValue = 0;
			Scr_Error("divide by 0");
			return;
		}

		value1->u.floatValue = value1->u.floatValue / value2->u.floatValue;
	}
	else
	{
		if ( type == VAR_INTEGER )
		{
			value1->type = VAR_FLOAT;

			if ( value2->u.intValue )
			{
				value1->u.floatValue = (float)value1->u.intValue / (float)value2->u.intValue;
				return;
			}

			value1->u.intValue = 0;
			Scr_Error("divide by 0");
			return;
		}

		Scr_UnmatchingTypesError(value1, value2);
	}
}

void Scr_EvalMod(VariableValue *value1, VariableValue *value2)
{
	if ( value1->type == VAR_INTEGER && value2->type == VAR_INTEGER )
	{
		if ( value2->u.intValue )
		{
			value1->u.intValue %= value2->u.intValue;
		}
		else
		{
			value1->u.intValue = 0;
			Scr_Error("divide by 0");
		}
	}
	else
	{
		Scr_UnmatchingTypesError(value1, value2);
	}
}

void Scr_EvalSizeValue(VariableValue *value)
{
	unsigned int stringValue;
	char *error_message;
	VariableValueInternal *entryValue;
	unsigned int id;

	if ( value->type == VAR_OBJECT )
	{
		id = value->u.intValue;
		entryValue = &scrVarGlob.variableList[value->u.intValue];
		value->type = VAR_INTEGER;

		if ( VAR_TYPE(entryValue) == VAR_ARRAY )
		{
			value->u.intValue = entryValue->u.o.u.size;
		}
		else
		{
			value->u.intValue = 1;
		}

		RemoveRefToObject(id);
	}
	else if ( value->type == VAR_STRING )
	{
		value->type = VAR_INTEGER;
		stringValue = value->u.intValue;
		value->u.intValue = strlen(SL_ConvertToString(value->u.intValue));
		SL_RemoveRefToString(stringValue);
	}
	else
	{
		error_message = va("size cannot be applied to %s", var_typename[value->type]);
		RemoveRefToValue(value);
		value->type = VAR_UNDEFINED;
		Scr_Error(error_message);
	}
}

static void Scr_ClearVector(VariableValue *value)
{
	int i;

	for ( i = 2; i >= 0; --i )
	{
		RemoveRefToValue(&value[i]);
	}

	value->type = VAR_UNDEFINED;
}

void Scr_CastVector(VariableValue *value)
{
	int type;
	int i;
	vec3_t vec;

	for ( i = 2; i >= 0; --i )
	{
		type = value[i].type;

		if ( type == VAR_FLOAT )
		{
			vec[2 - i] = value[i].u.floatValue;
		}
		else
		{
			if ( type != VAR_INTEGER )
			{
				scrVarPub.error_index = i + 1;
				Scr_ClearVector(value);
				Scr_Error(va("type %s is not a float", var_typename[type]));
				return;
			}

			vec[2 - i] = (float)value[i].u.intValue;
		}
	}

	value->type = VAR_VECTOR;
	value->u.vectorValue = Scr_AllocVector(vec);
}

void Scr_EvalBinaryOperator(int op, VariableValue *value1, VariableValue *value2)
{
	switch ( op )
	{
	case OP_bit_or:
		Scr_EvalOr(value1, value2);
		break;
	case OP_bit_ex_or:
		Scr_EvalExOr(value1, value2);
		break;
	case OP_bit_and:
		Scr_EvalAnd(value1, value2);
		break;
	case OP_equality:
		Scr_EvalEquality(value1, value2);
		break;
	case OP_inequality:
		Scr_EvalInequality(value1, value2);
		break;
	case OP_less:
		Scr_EvalLess(value1, value2);
		break;
	case OP_greater:
		Scr_EvalGreater(value1, value2);
		break;
	case OP_less_equal:
		Scr_EvalLessEqual(value1, value2);
		break;
	case OP_greater_equal:
		Scr_EvalGreaterEqual(value1, value2);
		break;
	case OP_shift_left:
		Scr_EvalShiftLeft(value1, value2);
		break;
	case OP_shift_right:
		Scr_EvalShiftRight(value1, value2);
		break;
	case OP_plus:
		Scr_EvalPlus(value1, value2);
		break;
	case OP_minus:
		Scr_EvalMinus(value1, value2);
		break;
	case OP_multiply:
		Scr_EvalMultiply(value1, value2);
		break;
	case OP_divide:
		Scr_EvalDivide(value1, value2);
		break;
	case OP_mod:
		Scr_EvalMod(value1, value2);
		break;
	default:
		return;
	}
}

unsigned int GetObjectA(unsigned int id)
{
	VariableValueInternal *entryValue;

	entryValue = &scrVarGlob.variableList[id];

	if ( (entryValue->w.status & 0x1F) == 0 )
	{
		entryValue->w.type |= 1u;
		entryValue->u.u.pointerValue = AllocObject();
	}

	return entryValue->u.u.pointerValue;
}

unsigned int FreeTempVariableObject()
{
	ClearVariableValue(scrVarPub.tempVariable);
	return GetObjectA(scrVarPub.tempVariable);
}

unsigned int FreeTempVariable()
{
	ClearVariableValue(scrVarPub.tempVariable);
	return scrVarPub.tempVariable;
}

unsigned int FindEntityId(int entnum, unsigned int classnum)
{
	unsigned int id;

	id = FindArrayVariable(scrClassMap[classnum].entArrayId, entnum);

	if ( id )
		return scrVarGlob.variableList[id].u.u.pointerValue;
	else
		return 0;
}

void Scr_FreeValue(unsigned int id)
{
	RemoveRefToObject(id);
}

void SafeRemoveVariable(unsigned int parentId, unsigned int name)
{
	unsigned int index;
	unsigned int id;

	index = FindVariableIndex(parentId, name);

	if ( index )
	{
		id = scrVarGlob.variableList[index].hash.id;
		MakeVariableExternal(&scrVarGlob.variableList[index], &scrVarGlob.variableList[parentId]);
		FreeChildValue(id);
	}
}

void ClearVariableField(unsigned int parentId, unsigned int name, VariableValue *value)
{
	unsigned int classnum;
	VariableValueInternal *entryValue;
	unsigned int fieldId;
	VariableValue *nextValue;

	if ( FindVariableIndex(parentId, name) )
	{
		RemoveVariable(parentId, name);
	}
	else
	{
		entryValue = &scrVarGlob.variableList[parentId];

		if ( (entryValue->w.status & 0x1F) == 21 )
		{
			classnum = entryValue->w.status >> 8;
			fieldId = FindArrayVariable(scrClassMap[classnum].id, name);

			if ( fieldId )
			{
				nextValue = value + 1;
				nextValue->type = VAR_UNDEFINED;
				SetEntityFieldValue(classnum, entryValue->u.o.u.entnum, scrVarGlob.variableList[fieldId].u.u.entityOffset, nextValue);
			}
		}
	}
}

bool IsFieldObject(unsigned int id)
{
	return (scrVarGlob.variableList[id].w.status & 0x1F) <= 0x15;
}

void ClearArray(unsigned int parentId, VariableValue *value)
{
	VariableValue entityVal;
	unsigned int fieldId;
	VariableValue varValue;
	unsigned int id;
	VariableValueInternal *newEntryValue;
	VariableValueInternal *parentValue;
	VariableValueInternal *entryValue;

	if ( parentId == 65534 )
	{
		newEntryValue = &scrVarGlob.variableList[scrVarPub.entId];
		fieldId = FindArrayVariable(
		              scrClassMap[(newEntryValue->w.status >> 8)].id,
		              scrVarPub.entFieldName);
		if ( !fieldId
		        || (GetEntityFieldValue(
		                &entityVal,
		                newEntryValue->w.status >> 8,
		                newEntryValue->u.o.u.size,
		                scrVarGlob.variableList[fieldId].u.u.entityOffset),
		            varValue = entityVal,
		            entityVal.type == VAR_UNDEFINED) )
		{
			varValue.type = VAR_UNDEFINED;
LABEL_11:
			scrVarPub.error_index = 1;
			Scr_Error(va("%s is not an array", var_typename[varValue.type]));
			return;
		}

		if ( varValue.type == VAR_OBJECT && !scrVarGlob.variableList[varValue.u.pointerValue].u.refCount )
		{
			RemoveRefToValue(&varValue);
			scrVarPub.error_index = 1;
			Scr_Error("read-only array cannot be changed");
			return;
		}

		RemoveRefToValue(&varValue);
		entryValue = 0;
	}
	else
	{
		entryValue = &scrVarGlob.variableList[parentId];
		varValue.type = entryValue->w.status & 0x1F;
		varValue.u.intValue = entryValue->u.u.intValue;
	}

	if ( varValue.type != VAR_OBJECT )
		goto LABEL_11;

	parentValue = &scrVarGlob.variableList[varValue.u.pointerValue];

	if ( (parentValue->w.status & 0x1F) == 22 )
	{
		if ( parentValue->u.refCount )
		{
			id = varValue.u.pointerValue;
			RemoveRefToObject(varValue.u.stringValue);
			varValue.u.pointerValue = Scr_AllocArray();
			CopyArray(id, varValue.u.stringValue);
			entryValue->u.u.intValue = varValue.u.intValue;
		}
		if ( value->type == VAR_INTEGER )
		{
			if ( IsValidArrayIndex(value->u.pointerValue) )
			{
				SafeRemoveArrayVariable(varValue.u.stringValue, value->u.pointerValue);
			}
			else
			{
				Scr_Error(va("array index %d out of range", value->u.pointerValue));
			}
		}
		else if ( value->type == VAR_STRING )
		{
			SL_RemoveRefToString(value->u.stringValue);
			SafeRemoveVariable(varValue.u.stringValue, value->u.pointerValue);
		}
		else
		{
			Scr_Error(va("%s is not an array index", var_typename[value->type]));
		}
	}
	else
	{
		scrVarPub.error_index = 1;
		Scr_Error(va("%s is not an array", var_typename[(parentValue->w.status & 0x1F)]));
	}
}

unsigned int Scr_EvalArrayRef(unsigned int parentId)
{
	VariableValue varValue;
	unsigned int fieldId;
	VariableValue value;
	unsigned int id;
	VariableValueInternal *entValue;
	VariableValueInternal *entryValue;
	VariableValueInternal *parentValue;

	if ( parentId != 65534 )
	{
		parentValue = &scrVarGlob.variableList[parentId];
		value.type = parentValue->w.status & 0x1F;

		if ( value.type )
		{
			value.u = parentValue->u.u;
			goto LABEL_12;
		}
LABEL_10:
		parentValue->w.status |= 1u;
		parentValue->u.u.pointerValue = Scr_AllocArray();
		return parentValue->u.u.stringValue;
	}

	entValue = &scrVarGlob.variableList[scrVarPub.entId];
	fieldId = FindArrayVariable(scrClassMap[(entValue->w.status >> 8)].id, scrVarPub.entFieldName);

	if ( !fieldId
	        || (GetEntityFieldValue(
	                &varValue,
	                entValue->w.status >> 8,
	                entValue->u.o.u.size,
	                scrVarGlob.variableList[fieldId].u.u.entityOffset),
	            value = varValue,
	            varValue.type == VAR_UNDEFINED) )
	{
		parentValue = &scrVarGlob.variableList[GetNewVariable(scrVarPub.entId, scrVarPub.entFieldName)];
		goto LABEL_10;
	}

	if ( value.type == VAR_OBJECT && !scrVarGlob.variableList[value.u.pointerValue].u.refCount )
	{
		RemoveRefToValue(&value);
		scrVarPub.error_index = 1;
		Scr_Error("read-only array cannot be changed");
		return 0;
	}

	RemoveRefToValue(&value);
	parentValue = 0;
LABEL_12:
	if ( value.type == VAR_OBJECT )
	{
		entryValue = &scrVarGlob.variableList[value.u.pointerValue];

		if ( (entryValue->w.status & 0x1F) == 22 )
		{
			if ( entryValue->u.refCount )
			{
				id = value.u.pointerValue;
				RemoveRefToObject(value.u.stringValue);
				value.u.pointerValue = Scr_AllocArray();
				CopyArray(id, value.u.stringValue);
				parentValue->u.u = value.u;
			}

			return value.u.pointerValue;
		}
		else
		{
			scrVarPub.error_index = 1;
			Scr_Error(va("%s is not an array", var_typename[(entryValue->w.status & 0x1F)]));
			return 0;
		}
	}
	else
	{
		scrVarPub.error_index = 1;

		if ( value.type == VAR_STRING )
		{
			Scr_Error("string characters cannot be individually changed");
			return 0;
		}
		else
		{
			if ( value.type == VAR_VECTOR )
			{
				Scr_Error("vector components cannot be individually changed");
			}
			else
			{
				Scr_Error(va("%s is not an array", var_typename[value.type]));
			}

			return 0;
		}
	}
}

void SetVariableEntityFieldValue(unsigned int entId, unsigned int fieldName, VariableValue *value)
{
	unsigned int entryValue;
	unsigned int fieldId;

	fieldId = FindArrayVariable(scrClassMap[scrVarGlob.variableList[entId].w.classnum >> 8].id, fieldName);

	if ( !fieldId
	        || !SetEntityFieldValue(
	            scrVarGlob.variableList[entId].w.classnum >> 8,
	            scrVarGlob.variableList[entId].u.o.u.entnum,
	            scrVarGlob.variableList[fieldId].u.u.entityOffset,
	            value) )
	{
		entryValue = GetNewVariable(entId, fieldName);
		scrVarGlob.variableList[entryValue].w.type |= value->type;
		scrVarGlob.variableList[entryValue].u.u = value->u;
	}
}

void SetVariableValue(unsigned int id, VariableValue *value)
{
	VariableValueInternal *newVariable;

	newVariable = &scrVarGlob.variableList[id];
	RemoveRefToValueInternal(newVariable->w.status & 0x1F, newVariable->u.u);
	newVariable->w.status &= 0xFFFFFFE0;
	newVariable->w.type |= value->type;
	newVariable->u.u = value->u;
}

void SetVariableFieldValue(unsigned int id, VariableValue *value)
{
	if ( id == 65534 )
		SetVariableEntityFieldValue(scrVarPub.entId, scrVarPub.entFieldName, value);
	else
		SetVariableValue(id, value);
}

void ClearVariableValue(unsigned int id)
{
	RemoveRefToValueInternal(scrVarGlob.variableList[id].w.status & 0x1F, scrVarGlob.variableList[id].u.u);
	scrVarGlob.variableList[id].w.status &= 0xFFFFFFE0;
}

unsigned int GetArray(unsigned int id)
{
	VariableValueInternal *entryValue;

	entryValue = &scrVarGlob.variableList[id];

	if ( (entryValue->w.status & 0x1F) == 0 )
	{
		entryValue->w.type |= 1u;
		entryValue->u.u.pointerValue = Scr_AllocArray();
	}

	return entryValue->u.u.pointerValue;
}

void Scr_SetThreadWaitTime(unsigned int startLocalId, unsigned int waitTime)
{
	VariableValueInternal *entryValue;

	entryValue = &scrVarGlob.variableList[startLocalId];

	entryValue->w.status &= ~VAR_MASK;
	entryValue->w.status &= ~VAR_NAME_HIGH_MASK;
	entryValue->w.type |= VAR_TIME_THREAD;
	scrVarGlob.variableList[startLocalId].w.waitTime |= (waitTime << VAR_NAME_BITS);
}

void Scr_SetThreadNotifyName(unsigned int startLocalId, unsigned int stringValue)
{
	VariableValueInternal *entryValue;

	entryValue = &scrVarGlob.variableList[startLocalId];

	entryValue->w.status &= ~VAR_MASK;
	entryValue->w.status &= ~VAR_NAME_HIGH_MASK;
	entryValue->w.type |= VAR_NOTIFY_THREAD;
	entryValue->w.name |= stringValue << VAR_NAME_BITS;
}

int Scr_GetOffset(unsigned int classnum, const char *name)
{
	unsigned int str;
	unsigned int classId;
	unsigned int fieldId;

	classId = scrClassMap[classnum].id;
	str = SL_ConvertFromString(name);
	fieldId = FindVariable(classId, str);

	if ( fieldId )
		return scrVarGlob.variableList[fieldId].u.u.entityOffset;
	else
		return -1;
}

unsigned int Scr_FindField(const char *name, int *type)
{
	unsigned int index;
	const char *pos;
	int len;

	for ( pos = scrVarPub.fieldBuffer; *pos; pos += len + 3 )
	{
		len = strlen(pos) + 1;

		if ( !Q_stricmp(name, pos) )
		{
			index = *(uint16_t *)&pos[len];
			*type = pos[len +2];
			return index;
		}
	}

	return 0;
}

void Scr_AddClassField(unsigned int classnum, const char *name, unsigned short offset)
{
	unsigned int str;
	unsigned int classId;
	unsigned int index;
	VariableValueInternal *entryValue;
	unsigned int varIndex;

	classId = scrClassMap[classnum].id;
	str = SL_GetCanonicalString(name);
	entryValue = &scrVarGlob.variableList[GetNewArrayVariable(classId, str)];
	entryValue->w.status &= 0xFFFFFFE0;
	entryValue->w.type |= 6u;
	entryValue->u.u.intValue = offset;
	index = SL_GetString_(name, 0);
	varIndex = GetNewVariable(classId, index);
	SL_RemoveRefToString(index);
	entryValue = &scrVarGlob.variableList[varIndex];
	entryValue->w.status &= 0xFFFFFFE0;
	entryValue->w.type |= 6u;
	entryValue->u.u.intValue = offset;
}

bool Scr_CastString(VariableValue *value)
{
	const float *constTempVector;

	switch ( value->type )
	{
	case VAR_STRING:
		return true;

	case VAR_INTEGER:
		value->type = VAR_STRING;
		value->u.intValue = SL_GetStringForInt(value->u.intValue);
		return true;

	case VAR_FLOAT:
		value->type = VAR_STRING;
		value->u.intValue = SL_GetStringForFloat(value->u.floatValue);
		return true;

	case VAR_VECTOR:
		value->type = VAR_STRING;
		constTempVector = value->u.vectorValue;
		value->u.intValue = SL_GetStringForVector(value->u.vectorValue);
		RemoveRefToVector(constTempVector);
		return true;

	default:
		scrVarPub.error_message = va("cannot cast %s to string", var_typename[value->type]);
		RemoveRefToValue(value);
		value->type = VAR_UNDEFINED;
		return false;
	}
}

void Scr_SetClassMap(unsigned int classnum)
{
	scrClassMap[classnum].entArrayId = Scr_AllocArray();
	scrClassMap[classnum].id = Scr_AllocArray();
}

float Scr_GetObjectEntryUsage(VariableValueInternal *entryValue)
{
	return Scr_GetEntryUsage(entryValue->w.status & 0x1F, entryValue->u.u) + 1.0;
}

float Scr_GetEntryUsage(unsigned int type, VariableUnion u)
{
	if ( type == 1 && (scrVarGlob.variableList[u.intValue].w.status & 0x1F) == 22 )
		return Scr_GetObjectUsage(u.pointerValue) / ((float)scrVarGlob.variableList[u.intValue].u.refCount + 1.0);
	else
		return 0.0;
}

float Scr_GetObjectUsage(unsigned int parentId)
{
	float usage;
	unsigned int i;

	usage = 1.0;

	for ( i = FindNextSibling(parentId); i; i = FindNextSibling(i) )
		usage = Scr_GetObjectEntryUsage(&scrVarGlob.variableList[i]) + usage;

	return usage;
}

float Scr_GetEndonUsage(unsigned int parentId)
{
	unsigned int id;
	unsigned int localId;

	localId = FindObjectVariable(scrVarPub.pauseArrayId, parentId);

	if ( localId )
	{
		id = FindObject(localId);
		return Scr_GetObjectUsage(id);
	}
	else
	{
		return 0.0;
	}
}

float Scr_GetThreadUsage(VariableStackBuffer *stackBuf, float *endonUsage)
{
	unsigned int localId;
	float usage;
	int size;
	char *buf;
	char *pos;
	VariableUnion u;

	size = stackBuf->size;
	buf = &stackBuf->buf[5 * size];
	usage = Scr_GetObjectUsage(stackBuf->localId);
	*endonUsage = Scr_GetEndonUsage(stackBuf->localId);
	localId = stackBuf->localId;

	while ( size )
	{
		pos = buf - 4;
		u.intValue = *(int *)pos;
		buf = pos - 1;
		--size;
		if ( *buf == 7 )
		{
			localId = GetParentLocalId(localId);
			usage = Scr_GetObjectUsage(localId) + usage;
			*endonUsage = Scr_GetEndonUsage(localId) + *endonUsage;
		}
		else
		{
			usage = Scr_GetEntryUsage((uint8_t)*buf, u) + usage;
		}
	}

	return usage;
}

static int ThreadInfoCompare(const void *ainfo1, const void *ainfo2)
{
	const char *pos1;
	int i;
	const char *pos2;
	ThreadDebugInfo *info1, *info2;
	info1 = (ThreadDebugInfo *)ainfo1;
	info2 = (ThreadDebugInfo *)ainfo2;

	for ( i = 0; ; ++i )
	{
		if ( i >= info1->posSize || i >= info2->posSize )
		{
			return info1->posSize - info2->posSize;
		}
		pos1 = info1->pos[i];
		pos2 = info2->pos[i];
		if ( pos1 != pos2 )
		{
			break;
		}
	}
	return pos1 - pos2;
}

void Scr_DumpScriptThreads()
{
	ThreadDebugInfo *info;
	float usage;
	const char *currentPos;
	char type;
	char *buf;
	char *nextPos;
	const char *pos;
	int size;
	VariableStackBuffer *stackBuf;
	ThreadDebugInfo threadInfo;
	unsigned int id;
	unsigned int k;
	int count;
	int j;
	int arrayIndex;
	ThreadDebugInfo *pInfo;
	int num;
	ThreadDebugInfo *infoArray;
	VariableValueInternal *entryValue;
	unsigned int i;

	infoArray = (ThreadDebugInfo *)Z_TryMallocInternal(0x8BFEE8u);

	if ( infoArray )
	{
		num = 0;
		for ( i = 1; i <= 0xFFFD; ++i )
		{
			entryValue = &scrVarGlob.variableList[i];
			if ( (entryValue->w.status & 0x60) != 0 && (entryValue->w.status & 0x1F) == 10 )
			{
				pInfo = &infoArray[num++];
				threadInfo.posSize = 0;
				stackBuf = entryValue->u.u.stackValue;
				size = stackBuf->size;
				pos = stackBuf->pos;
				buf = stackBuf->buf;
				while ( size )
				{
					--size;
					type = *buf;
					nextPos = buf + 1;
					currentPos = *(const char **)nextPos;
					buf = nextPos + 4;
					if ( type == 7 )
						threadInfo.pos[threadInfo.posSize++] = currentPos;
				}
				threadInfo.pos[threadInfo.posSize++] = pos;
				info = pInfo;
				info->varUsage = Scr_GetThreadUsage(stackBuf, &pInfo->endonUsage);
				pInfo->posSize = threadInfo.posSize--;
				for ( j = 0; j < pInfo->posSize; ++j )
					pInfo->pos[j] = threadInfo.pos[threadInfo.posSize - j];
			}
		}
		qsort(infoArray, num, sizeof(ThreadDebugInfo), ThreadInfoCompare);
		Com_Printf("********************************\n");
		arrayIndex = 0;
		while ( arrayIndex < num )
		{
			pInfo = &infoArray[arrayIndex];
			count = 0;
			threadInfo.varUsage = 0.0;
			threadInfo.endonUsage = 0.0;
			do
			{
				++count;
				threadInfo.varUsage = threadInfo.varUsage + infoArray[arrayIndex].varUsage;
				threadInfo.endonUsage = threadInfo.endonUsage + infoArray[arrayIndex++].endonUsage;
			}
			while ( arrayIndex < num && !ThreadInfoCompare(pInfo, &infoArray[arrayIndex]) );
			Com_Printf(
			    "count: %d, var usage: %d, endon usage: %d\n",
			    count,
			    (int)threadInfo.varUsage,
			    (int)threadInfo.endonUsage);
			Scr_PrintPrevCodePos(CON_CHANNEL_DONT_FILTER, pInfo->pos[0], 0);
			for ( j = 1; j < pInfo->posSize; ++j )
			{
				Com_Printf("called from:\n");
				Scr_PrintPrevCodePos(CON_CHANNEL_DONT_FILTER, pInfo->pos[j], 0);
			}
		}
		Z_FreeInternal(infoArray);
		Com_Printf("********************************\n");
		for ( k = 0; k <= 3; ++k )
		{
			if ( scrClassMap[k].entArrayId )
			{
				threadInfo.varUsage = 0.0;
				count = 0;
				for ( id = FindNextSibling(scrClassMap[k].entArrayId); id; id = FindNextSibling(id) )
				{
					++count;
					if ( (scrVarGlob.variableList[id].w.status & 0x1F) == 1 )
					{
						usage = Scr_GetObjectUsage(scrVarGlob.variableList[id].u.u.intValue);
						threadInfo.varUsage = usage + threadInfo.varUsage;
					}
				}
				Com_Printf("ent type '%s'... count: %d, var usage: %d\n", scrClassMap[k].name, count, (int)threadInfo.varUsage);
			}
		}
		Com_Printf("********************************\n");
	}
	else
	{
		Com_Printf("Cannot dump script threads: out of memory\n");
	}
}

void Scr_DumpScriptVariables()
{
	;
}

void Scr_AddFieldsForFile(const char *filename)
{
	size_t strsize;
	char *lwr;
	fileHandle_t f;
	int dataType;
	int i;
	int size;
	unsigned int index;
	int type;
	char *targetPos;
	char *token;
	const char *sourcePos;
	char *buffer;
	int len;

	len = FS_FOpenFileByMode(filename, &f, FS_READ);

	if ( len < 0 )
	{
		Com_Error(ERR_DROP, va("cannot find '%s'", filename));
	}

	buffer = (char *)Hunk_AllocateTempMemoryHighInternal(len + 1);
	FS_Read(buffer, len, f);
	buffer[len] = 0;
	FS_FCloseFile(f);
	sourcePos = buffer;

	Com_BeginParseSession("Scr_AddFields");

	while ( 1 )
	{
		token = Com_Parse(&sourcePos);

		if ( !sourcePos )
			break;

		if ( !strcmp(token, "float") )
		{
			type = VAR_FLOAT;
		}
		else if ( !strcmp(token, "int") )
		{
			type = VAR_INTEGER;
		}
		else if ( !strcmp(token, "string") )
		{
			type = VAR_STRING;
		}
		else
		{
			if ( strcmp(token, "vector") )
			{
				Com_Error(ERR_DROP, va("unknown type '%s' in '%s'", token, filename));
			}
			type = VAR_VECTOR;
		}

		token = Com_Parse(&sourcePos);

		if ( !sourcePos )
		{
			Com_Error(ERR_DROP, va("missing field name in '%s'", filename));
		}

		strsize = strlen(token);
		len = strsize + 1;

		for ( i = strsize; i >= 0; --i )
		{
			lwr = &token[i];
			*lwr = tolower(token[i]);
		}

		index = SL_GetCanonicalString(token);

		if ( Scr_FindField(token, &dataType) )
			Com_Error(ERR_DROP, "duplicate key '%s' in '%s'", token, filename);

		size = len + 3;
		targetPos = (char *)TempMalloc(len + 3);
		strcpy(targetPos, token);
		targetPos += len;
		*(uint16_t *)targetPos = index;
		targetPos += 2;
		*targetPos++ = type;
		*targetPos = 0;
	}

	Com_EndParseSession();
	Hunk_ClearTempMemoryHighInternal();
}

void Scr_AddFields(const char *path, const char *extension)
{
	char s[MAX_QPATH];
	int i;
	char **fields;
	int numfields;

	fields = FS_ListFiles(path, extension, FS_LIST_PURE_ONLY, &numfields);
	TempMemoryReset();
	scrVarPub.fieldBuffer = (const char *)Hunk_AllocLowInternal(0);
	*(char *)scrVarPub.fieldBuffer = 0;

	for ( i = 0; i < numfields; ++i )
	{
		sprintf(s, "%s/%s", path, fields[i]);
		Scr_AddFieldsForFile(s);
	}

	if ( fields )
		FS_FreeFileList(fields);

	*(char *)TempMalloc(1) = 0;
	Hunk_ConvertTempToPermLowInternal();
}

void Var_ResetAll()
{
	union Variable_u hash;
	VariableValueInternal *entryValue;
	unsigned int i;

	hash.prev = 0;

	for ( i = 1; i <= 0xFFFD; ++i )
	{
		entryValue = &scrVarGlob.variableList[i];
		entryValue->w.status = 0;
		entryValue->v.next = i;
		entryValue->hash.id = i;
		entryValue->hash.u = hash;
		scrVarGlob.variableList[hash.prev].u.next = i;
		hash.prev = i;
	}

	scrVarGlob.variableList->w.status = 0;
	scrVarGlob.variableList->w.type = 0;
	scrVarGlob.variableList->v.next = 0;
	scrVarGlob.variableList->hash.id = 0;
	scrVarGlob.variableList->hash.u = hash;
	scrVarGlob.variableList[hash.prev].u.next = 0;
}

void Var_FreeTempVariables()
{
	if ( scrVarPub.tempVariable )
	{
		FreeChildValue(scrVarPub.tempVariable);
		scrVarPub.tempVariable = 0;
	}
}

void Var_Shutdown()
{
	if ( scrVarPub.gameId )
	{
		FreeChildValue(scrVarPub.gameId);
		scrVarPub.gameId = 0;
	}
}

void Var_InitClassMap()
{
	unsigned int classnum;

	for ( classnum = 0; classnum < CLASS_NUM_COUNT; ++classnum )
	{
		scrClassMap[classnum].entArrayId = 0;
		scrClassMap[classnum].id = 0;
	}
}

void Var_Init()
{
	Var_ResetAll();
	Var_InitClassMap();
}