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
#define VAR_NAME_BITS 8

#define VAR_STAT_FREE 0
#define VAR_STAT_MOVABLE 32
#define VAR_STAT_HEAD 64
#define VAR_STAT_MASK 96
#define VAR_STAT_EXTERNAL VAR_STAT_MASK

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

void Scr_GetEntityIdRef(scr_entref_t *entRef, unsigned int entId)
{
	entRef->entnum = scrVarGlob.variableList[entId].u.o.u.entnum;
	entRef->classnum = scrVarGlob.variableList[entId].w.status >> 8;
}

union VariableValueInternal_u* GetVariableValueAddress(unsigned int id)
{
	return &scrVarGlob.variableList[id].u;
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
	return scrVarGlob.variableList[threadId].w.status >> 8;
}

unsigned int GetSafeParentLocalId(unsigned int threadId)
{
	if ( (scrVarGlob.variableList[threadId].w.status & VAR_MASK) == 18 )
		return scrVarGlob.variableList[threadId].w.status >> 8;
	else
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

unsigned int FindArrayVariableIndex(unsigned int parentId, unsigned int index)
{
	return FindVariableIndex(parentId, (index + 0x800000) & 0xFFFFFF);
}

unsigned int FindArrayVariable(unsigned int parentId, unsigned int index)
{
	return scrVarGlob.variableList[FindArrayVariableIndex(parentId, index)].hash.id;
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

void Scr_DumpScriptThreads()
{
	UNIMPLEMENTED(__FUNCTION__);
}

void Scr_DumpScriptVariablesDefault()
{
	UNIMPLEMENTED(__FUNCTION__);
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