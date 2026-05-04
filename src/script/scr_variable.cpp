#include "../qcommon/qcommon.h"
#include "script_public.h"

scrVarGlob_t scrVarGlob;
scrVarPub_t scrVarPub;

/*
==============
GetObjectType
==============
*/
unsigned int GetObjectType( unsigned int id )
{
	assert((scrVarGlob.variableList[id].w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	return scrVarGlob.variableList[id].w.type & VAR_MASK;
}

/*
==============
IsObjectFree
==============
*/
bool IsObjectFree( unsigned int id )
{
	return ( scrVarGlob.variableList[id].w.status & VAR_STAT_MASK ) == VAR_STAT_FREE;
}

/*
==============
Scr_GetClassnumForCharId
==============
*/
int Scr_GetClassnumForCharId( char charId )
{
	for ( int i = 0; i < CLASS_NUM_COUNT; i++ )
	{
		if ( scrClassMap[i].charId == charId )
		{
			return i;
		}
	}

	return -1;
}

/*
==============
Scr_GetChecksum
==============
*/
void Scr_GetChecksum( int *checksum )
{
	UNIMPLEMENTED(__FUNCTION__);
}

/*
==============
Scr_FindField
==============
*/
unsigned int Scr_FindField( const char *name, int *type )
{
	const char *pos;
	int len;
	unsigned int index;

	assert(scrVarPub.fieldBuffer);

	for ( pos = scrVarPub.fieldBuffer; *pos; pos += len + 3 )
	{
		len = strlen(pos) + 1;

		if ( !I_stricmp(name, pos) )
		{
			pos = &pos[len];
			index = *(unsigned short *)&pos[0];
			*type = pos[2];

			return index;
		}
	}

	return 0;
}

/*
==============
Scr_GetEntityIdRef
==============
*/
scr_entref_t Scr_GetEntityIdRef( unsigned int entId )
{
	scr_entref_t entref;
	VariableValueInternal *entValue;

	entValue = &scrVarGlob.variableList[entId];

	assert((entValue->w.type & VAR_MASK) == VAR_ENTITY);
	assert((entValue->w.name >> VAR_NAME_BITS) < CLASS_NUM_COUNT);

	entref.entnum = entValue->u.o.u.entnum;
	entref.classnum = entValue->w.classnum >> VAR_NAME_BITS;

	return entref;
}

/*
==============
GetVarType
==============
*/
int GetVarType( unsigned int id )
{
	UNIMPLEMENTED(__FUNCTION__);
	return 0;
}

/*
==============
IsVarFree
==============
*/
bool IsVarFree( unsigned int id )
{
	UNIMPLEMENTED(__FUNCTION__);
	return true;
}



/*
==============
Scr_GetEntNum
==============
*/
int Scr_GetEntNum( unsigned int id )
{
	assert(GetObjectType( id ) == VAR_ENTITY);
	return scrVarGlob.variableList[id].u.o.u.entnum;
}

/*
==============
Scr_GetEntClassId
==============
*/
char Scr_GetEntClassId( unsigned int id )
{
	assert(GetObjectType( id ) == VAR_ENTITY);
	return scrClassMap[scrVarGlob.variableList[id].w.name >> VAR_NAME_BITS].charId;
}

/*
==============
Scr_IsThreadAlive
==============
*/
int Scr_IsThreadAlive( unsigned int thread )
{
	VariableValueInternal *entryValue;

	assert(scrVarPub.timeArrayId);
	entryValue = &scrVarGlob.variableList[thread];

	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(((entryValue->w.type & VAR_MASK) >= VAR_THREAD && (entryValue->w.type & VAR_MASK) <= VAR_CHILD_THREAD) || (entryValue->w.type & VAR_MASK) == VAR_DEAD_THREAD);

	return (entryValue->w.type & VAR_MASK) != VAR_DEAD_THREAD;
}

/*
==============
IsFieldObject
==============
*/
bool IsFieldObject( unsigned int id )
{
	VariableValueInternal *entryValue;

	assert(id);
	entryValue = &scrVarGlob.variableList[id];

	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(IsObject( entryValue ));

	return (entryValue->w.type & VAR_MASK) < VAR_ARRAY;
}

/*
==============
FindObject
==============
*/
unsigned int FindObject( unsigned int id )
{
	VariableValueInternal *entryValue;

	assert(id);
	entryValue = &scrVarGlob.variableList[id];

	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert((entryValue->w.type & VAR_MASK) == VAR_POINTER);

	return entryValue->u.u.pointerValue;
}

/*
==============
GetVariableName
==============
*/
unsigned int GetVariableName( unsigned int id )
{
	VariableValueInternal *entryValue;

	entryValue = &scrVarGlob.variableList[id];

	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(!IsObject( entryValue ));

	return entryValue->w.name >> VAR_NAME_BITS;
}

/*
==============
FindPrevSibling
==============
*/
unsigned int FindPrevSibling( unsigned int id )
{
	VariableValueInternal *list, *entryValue;
	unsigned int nextSibling, childId, prevSibling;

	assert(id);
	list = scrVarGlob.variableList;

	entryValue = &list[id];
	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);

	nextSibling = entryValue->nextSibling;

	childId = list[nextSibling].hash.u.prev;
	prevSibling = list[childId].hash.u.prevSibling;

	entryValue = &list[prevSibling];
	childId = entryValue->hash.id;

	entryValue = &list[childId];
	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);

	if ( IsObject( entryValue ) )
	{
		return 0;
	}

	return childId;
}

/*
==============
FindNextSibling
==============
*/
unsigned int FindNextSibling( unsigned int id )
{
	VariableValueInternal *list, *entryValue;
	unsigned int nextSibling, childId;

	assert(id);
	list = scrVarGlob.variableList;

	entryValue = &list[id];
	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);

	nextSibling = entryValue->nextSibling;

	entryValue = &list[nextSibling];
	childId = entryValue->hash.id;

	entryValue = &list[childId];
	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);

	if ( IsObject( entryValue ) )
	{
		return 0;
	}

	return childId;
}

/*
==============
GetArraySize
==============
*/
unsigned int GetArraySize( unsigned int id )
{
	VariableValueInternal *entryValue;

	assert(id);
	entryValue = &scrVarGlob.variableList[id];
	assert((entryValue->w.type & VAR_MASK) == VAR_ARRAY);

	return entryValue->u.o.u.size;
}

/*
==============
GetVariableValueAddress
==============
*/
union VariableUnion* GetVariableValueAddress( unsigned int id )
{
	VariableValueInternal *entryValue;

	assert(id);
	entryValue = &scrVarGlob.variableList[id];

	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert((entryValue->w.type & VAR_MASK) != VAR_UNDEFINED);
	assert(!IsObject( entryValue ));

	return &entryValue->u.u;
}

/*
==============
SetNewVariableValue
==============
*/
void SetNewVariableValue( unsigned int id, VariableValue *value )
{
	VariableValueInternal *entryValue;

	assert((value->type & VAR_MASK) < VAR_THREAD);
	entryValue = &scrVarGlob.variableList[id];

	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(!IsObject( entryValue ));
	assert(value->type >= 0 && value->type < VAR_COUNT);
	assert((entryValue->w.type & VAR_MASK) == VAR_UNDEFINED);
	assert((value->type != VAR_POINTER) || ((entryValue->w.type & VAR_MASK) < FIRST_DEAD_OBJECT));
	assert(!(entryValue->w.type & VAR_MASK));

	entryValue->w.type |= value->type;
	entryValue->u.u = value->u;
}

/*
==============
SetNewVariableValue
==============
*/
VariableValue Scr_GetArrayIndexValue( unsigned int name )
{
	VariableValue value;

	assert(name);

	if ( name < SL_MAX_STRING_INDEX )
	{
		value.type = VAR_STRING;
		value.u.stringValue = (unsigned short)name;
	}
	else if ( name < OBJECT_NOTIFY_LIST )
	{
		value.type = VAR_POINTER;
		value.u.pointerValue = name - SL_MAX_STRING_INDEX;
	}
	else
	{
		value.type = VAR_INTEGER;
		value.u.intValue = name - MAX_ARRAYINDEX;
	}

	return value;
}

/*
==============
GetInternalVariableIndex
==============
*/
unsigned int GetInternalVariableIndex( unsigned int index )
{
	assert(IsValidArrayIndex( index ));
	return ( index + MAX_ARRAYINDEX ) & VAR_NAME_LOW_MASK;
}

/*
==============
IsValidArrayIndex
==============
*/
bool IsValidArrayIndex( unsigned int index )
{
	return index < MAX_ARRAYINDEX;
}

/*
==============
AddRefToVector
==============
*/
void AddRefToVector( const vec3_t vectorValue )
{
	RefVector *refVec = (RefVector *)(( byte *)vectorValue - REFSTRING_STRING_OFFSET );

	if ( refVec->byteLen )
	{
		return;
	}

	refVec->refCount++;
	assert(refVec->refCount);
}

/*
==============
Scr_GetRefCountToObject
==============
*/
int Scr_GetRefCountToObject( unsigned int id )
{
	VariableValueInternal *entryValue = &scrVarGlob.variableList[id];

	assert(((entryValue->w.status & VAR_STAT_MASK) == VAR_STAT_EXTERNAL));
	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(IsObject( entryValue ));

	return entryValue->u.o.refCount;
}

/*
==============
AddRefToObject
==============
*/
void AddRefToObject( unsigned int id )
{
	assert(id);
	VariableValueInternal *entryValue = &scrVarGlob.variableList[id];
	entryValue->u.o.refCount++;

	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(IsObject( entryValue ));
	assert(entryValue->u.o.refCount);
}

/*
==============
Scr_GetSelf
==============
*/
unsigned int Scr_GetSelf( unsigned int threadId )
{
	VariableValueInternal *entryValue = &scrVarGlob.variableList[threadId];

	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(((entryValue->w.type & VAR_MASK) >= VAR_THREAD) && ((entryValue->w.type & VAR_MASK) <= VAR_CHILD_THREAD));

	return entryValue->u.o.u.self;
}

/*
==============
GetStartLocalId
==============
*/
unsigned int GetStartLocalId( unsigned int threadId )
{
	assert((scrVarGlob.variableList[threadId].w.status & VAR_STAT_MASK) == VAR_STAT_EXTERNAL);
	assert((scrVarGlob.variableList[threadId].w.type & VAR_MASK) >= VAR_THREAD && (scrVarGlob.variableList[threadId].w.type & VAR_MASK) <= VAR_CHILD_THREAD);

	while ( (scrVarGlob.variableList[threadId].w.type & VAR_MASK) == VAR_CHILD_THREAD )
	{
		threadId = scrVarGlob.variableList[threadId].w.parentLocalId >> VAR_NAME_BITS;
	}

	assert((scrVarGlob.variableList[threadId].w.status & VAR_STAT_MASK) == VAR_STAT_EXTERNAL);
	assert((scrVarGlob.variableList[threadId].w.type & VAR_MASK) >= VAR_THREAD && (scrVarGlob.variableList[threadId].w.type & VAR_MASK) <= VAR_TIME_THREAD);

	return threadId;
}

/*
==============
GetSafeParentLocalId
==============
*/
unsigned int GetSafeParentLocalId( unsigned int threadId )
{
	VariableValueInternal *entryValue = &scrVarGlob.variableList[threadId];

	if ( (entryValue->w.type & VAR_MASK) != VAR_CHILD_THREAD )
	{
		return 0;
	}

	assert((entryValue->w.status & VAR_STAT_MASK) == VAR_STAT_EXTERNAL);
	assert((entryValue->w.type & VAR_MASK) >= VAR_THREAD && (entryValue->w.type & VAR_MASK) <= VAR_CHILD_THREAD);

	return entryValue->w.parentLocalId >> VAR_NAME_BITS;
}

/*
==============
GetParentLocalId
==============
*/
unsigned int GetParentLocalId( unsigned int threadId )
{
	VariableValueInternal *entryValue = &scrVarGlob.variableList[threadId];

	assert((entryValue->w.status & VAR_STAT_MASK) == VAR_STAT_EXTERNAL);
	assert((entryValue->w.type & VAR_MASK) == VAR_CHILD_THREAD);

	return entryValue->w.parentLocalId >> VAR_NAME_BITS;
}

/*
==============
Scr_GetThreadWaitTime
==============
*/
unsigned int Scr_GetThreadWaitTime( unsigned int startLocalId )
{
	VariableValueInternal *entryValue = &scrVarGlob.variableList[startLocalId];

	assert((entryValue->w.status & VAR_STAT_MASK) == VAR_STAT_EXTERNAL);
	assert((entryValue->w.type & VAR_MASK) == VAR_TIME_THREAD);

	return entryValue->w.waitTime >> VAR_NAME_BITS;
}

/*
==============
Scr_ClearWaitTime
==============
*/
void Scr_ClearWaitTime( unsigned int startLocalId )
{
	VariableValueInternal *entryValue = &scrVarGlob.variableList[startLocalId];

	assert(((entryValue->w.status & VAR_STAT_MASK) == VAR_STAT_EXTERNAL));
	assert((entryValue->w.type & VAR_MASK) == VAR_TIME_THREAD);

	entryValue->w.type &= ~VAR_MASK;
	entryValue->w.type |= VAR_THREAD;
}

/*
==============
Scr_SetThreadWaitTime
==============
*/
void Scr_SetThreadWaitTime( unsigned int startLocalId, unsigned int waitTime )
{
	VariableValueInternal *entryValue = &scrVarGlob.variableList[startLocalId];

	assert(((entryValue->w.status & VAR_STAT_MASK) == VAR_STAT_EXTERNAL));
	assert(((entryValue->w.type & VAR_MASK) == VAR_THREAD) || !Scr_GetThreadNotifyName(startLocalId));

	entryValue->w.type &= ~VAR_MASK;
	entryValue->w.type = (unsigned char)entryValue->w.type;
	entryValue->w.type |= VAR_TIME_THREAD;

	entryValue->w.waitTime |= waitTime << VAR_NAME_BITS;
}

/*
==============
Scr_GetThreadNotifyName
==============
*/
unsigned short Scr_GetThreadNotifyName( unsigned int startLocalId )
{
	VariableValueInternal *entryValue = &scrVarGlob.variableList[startLocalId];

	assert((entryValue->w.status & VAR_STAT_MASK) == VAR_STAT_EXTERNAL);
	assert((entryValue->w.type & VAR_MASK) == VAR_NOTIFY_THREAD);
	assert((entryValue->w.notifyName >> VAR_NAME_BITS) < ( 1 << 16 ));

	return entryValue->w.notifyName >> VAR_NAME_BITS;
}

/*
==============
Scr_RemoveThreadEmptyNotifyName
==============
*/
void Scr_RemoveThreadEmptyNotifyName( unsigned int startLocalId )
{
	UNIMPLEMENTED(__FUNCTION__);
}

/*
==============
Scr_SetThreadNotifyName
==============
*/
void Scr_SetThreadNotifyName( unsigned int startLocalId, unsigned int notifyName )
{
	VariableValueInternal *entryValue = &scrVarGlob.variableList[startLocalId];

	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(((entryValue->w.type & VAR_MASK) == VAR_THREAD));

	entryValue->w.type &= ~VAR_MASK;
	entryValue->w.type = (unsigned char)entryValue->w.type;
	entryValue->w.type |= VAR_NOTIFY_THREAD;

	entryValue->w.notifyName |= notifyName << VAR_NAME_BITS;
}

/*
==============
GetVariableKeyObject
==============
*/
unsigned int GetVariableKeyObject( unsigned int id )
{
	VariableValueInternal *entryValue = &scrVarGlob.variableList[id];

	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(!IsObject(entryValue));

	return (entryValue->w.name >> VAR_NAME_BITS) - SL_MAX_STRING_INDEX;
}

/*
==============
Scr_GetNumScriptVars
==============
*/
unsigned int Scr_GetNumScriptVars()
{
	UNIMPLEMENTED(__FUNCTION__);
	return 0;
}

/*
==============
Scr_DumpScriptVariables
==============
*/
void Scr_DumpScriptVariables( bool spreadsheet, bool summary, bool total, bool functionSummary, bool lineSort, const char *fileName, const char *functionName, int minCount )
{
	UNIMPLEMENTED(__FUNCTION__);
}

/*
==============
Scr_DumpScriptVariablesDefault
==============
*/
void Scr_DumpScriptVariablesDefault()
{
	Scr_DumpScriptVariables(0, 0, 0, 0, 0, 0, 0, 0);
}

/*
==============
IsObjectId
==============
*/
bool IsObjectId( unsigned int id )
{
	return GetObjectType( id ) >= VAR_THREAD;
}

/*
==============
Scr_EvalVariableObject
==============
*/
unsigned int Scr_EvalVariableObject( unsigned int id )
{
	VariableValueInternal *entryValue;
	int type;

	entryValue = &scrVarGlob.variableList[id];
	assert(((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE) || !id);

	type = entryValue->w.type & VAR_MASK;

	if ( type == VAR_POINTER )
	{
		type = scrVarGlob.variableList[entryValue->u.u.pointerValue].w.type & VAR_MASK;

		if ( type < VAR_ARRAY )
		{
			assert(type >= FIRST_OBJECT);
			return entryValue->u.u.pointerValue;
		}
	}

	Scr_Error(va("%s is not a field object", var_typename[type]));
	return 0;
}

/*
==============
RemoveRefToEmptyObject
==============
*/
void RemoveRefToEmptyObject( unsigned int id )
{
	VariableValueInternal *entryValue = &scrVarGlob.variableList[id];

	assert(((entryValue->w.status & VAR_STAT_MASK) == VAR_STAT_EXTERNAL));
	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(IsObject(entryValue));
	//assert(!entryValue->nextSibling);

	if ( !entryValue->u.o.refCount )
	{
		FreeVariable(id);
		return;
	}

	assert(id);
	entryValue->u.o.refCount--;
}

/*
==============
Var_Init
==============
*/
void Var_Init()
{
	InitVariables();
	Scr_InitClassMap();
}

/*
==============
FindObjectVariable
==============
*/
unsigned int FindObjectVariable( unsigned int parentId, unsigned int id )
{
	return scrVarGlob.variableList[FindVariableIndexInternal(parentId, id + SL_MAX_STRING_INDEX)].hash.id;
}

/*
==============
FindVariable
==============
*/
unsigned int FindVariable( unsigned int parentId, unsigned int index )
{
	return scrVarGlob.variableList[FindVariableIndexInternal(parentId, index)].hash.id;
}

/*
==============
AddRefToValue
==============
*/
void AddRefToValue( int type, VariableUnion u )
{
	switch ( type )
	{
	case VAR_POINTER:
		AddRefToObject(u.pointerValue);
		break;

	case VAR_STRING:
	case VAR_ISTRING:
		SL_AddRefToString(u.stringValue);
		break;

	case VAR_VECTOR:
		assert(type - 1 == VAR_VECTOR - VAR_BEGIN_REF);
		AddRefToVector(u.vectorValue);
		break;
	}
}

/*
==============
Scr_GetOffset
==============
*/
int Scr_GetOffset( unsigned int classnum, const char *name )
{
	unsigned int fieldId = FindVariable( scrClassMap[classnum].id, SL_ConvertFromString( name ) );

	if ( !fieldId )
	{
		return -1;
	}

	return scrVarGlob.variableList[fieldId].u.u.entityOffset;
}

/*
==============
Scr_EvalVariable
==============
*/
VariableValue Scr_EvalVariable( unsigned int id )
{
	VariableValueInternal *entryValue;
	VariableValue value;

	entryValue = &scrVarGlob.variableList[id];
	assert(((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE) || !id);

	value.type = entryValue->w.type & VAR_MASK;
	value.u = entryValue->u.u;

	assert(!IsObject(entryValue));
	AddRefToValue(value.type, value.u);

	return value;
}

/*
==============
Scr_FindAllVariableField
==============
*/
unsigned int Scr_FindAllVariableField( unsigned int parentId, unsigned int *names ) // untested
{
	VariableValueInternal *parentValue;
	unsigned int classnum, name, id, count;

	parentValue = &scrVarGlob.variableList[parentId];
	assert((parentValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert((parentValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(IsObject(parentValue));
	count = 0;

	switch ( parentValue->w.type & VAR_MASK )
	{
	case VAR_THREAD:
	case VAR_NOTIFY_THREAD:
	case VAR_TIME_THREAD:
	case VAR_CHILD_THREAD:
	case VAR_OBJECT:
	case VAR_DEAD_ENTITY:
		for ( id = scrVarGlob.variableList[parentValue->nextSibling].hash.id; id != parentId; id = scrVarGlob.variableList[scrVarGlob.variableList[id].nextSibling].hash.id )
		{
			name = scrVarGlob.variableList[id].w.name >> VAR_NAME_BITS;
			assert(name);

			if ( name == OBJECT_NOTIFY_LIST || name == OBJECT_STACK )
			{
				continue;
			}

			assert(name <= scrVarPub.canonicalStrCount);

			if ( names )
			{
				names[count] = name;
			}

			count++;
		}
		break;

	case VAR_ENTITY:
		classnum = scrClassMap[parentValue->w.name >> VAR_NAME_BITS].id;
		assert(classnum < CLASS_NUM_COUNT);
		for ( id = scrVarGlob.variableList[scrVarGlob.variableList[classnum].nextSibling].hash.id; id != classnum; id = scrVarGlob.variableList[scrVarGlob.variableList[id].nextSibling].hash.id )
		{
			name = (scrVarGlob.variableList[id].w.name >> VAR_NAME_BITS) - MAX_ARRAYINDEX;

			if ( name > scrVarPub.canonicalStrCount )
			{
				continue;
			}

			if ( FindVariable( parentId, name ) )
			{
				continue;
			}

			if ( names )
			{
				names[count] = name;
			}

			count++;
		}
		for ( id = scrVarGlob.variableList[parentValue->nextSibling].hash.id; id != parentId; id = scrVarGlob.variableList[scrVarGlob.variableList[id].nextSibling].hash.id )
		{
			name = scrVarGlob.variableList[id].w.name >> VAR_NAME_BITS;
			assert(name);

			if ( name == OBJECT_NOTIFY_LIST || name == OBJECT_STACK )
			{
				continue;
			}

			assert(name <= scrVarPub.canonicalStrCount);

			if ( names )
			{
				names[count] = name;
			}

			count++;
		}
		break;

	case VAR_ARRAY:
		for ( id = scrVarGlob.variableList[parentValue->nextSibling].hash.id; id != parentId; id = scrVarGlob.variableList[scrVarGlob.variableList[id].nextSibling].hash.id )
		{
			name = scrVarGlob.variableList[id].w.name >> VAR_NAME_BITS;
			assert(name);

			if ( names )
			{
				names[count] = name;
			}

			count++;
		}
		break;
	}

	return count;
}

/*
==============
FindArrayVariable
==============
*/
unsigned int FindArrayVariable( unsigned int parentId, unsigned int index )
{
	return scrVarGlob.variableList[ FindArrayVariableIndex( parentId, index ) ].hash.id;
}

/*
==============
FindEntityId
==============
*/
unsigned int FindEntityId( int entnum, int classnum )
{
	unsigned int entArrayId, id;
	VariableValueInternal *entryValue;

	assert((unsigned)entnum < (1 << 16));
	entArrayId = scrClassMap[classnum].entArrayId;

	assert(entArrayId);
	id = FindArrayVariable(entArrayId, entnum);

	if ( !id )
	{
		return 0;
	}

	entryValue = &scrVarGlob.variableList[id];

	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert((entryValue->w.type & VAR_MASK) == VAR_POINTER);
	assert(entryValue->u.u.pointerValue);

	return entryValue->u.u.pointerValue;
}

/*
==============
Scr_DumpScriptThreads
==============
*/
void Scr_DumpScriptThreads()
{
	ThreadDebugInfo *infoArray, *pInfo;
	ThreadDebugInfo info;
	int num, size, i, j, count, classnum, id;
	VariableValueInternal *entryValue;
	VariableStackBuffer *stackBuf;
	const char *pos, *buf;
	VariableValue value;

	infoArray = (ThreadDebugInfo *)Z_Malloc( sizeof( *infoArray ) * VARIABLELIST_CHILD_SIZE );

	if ( !infoArray )
	{
		Com_Printf("Cannot dump script threads: out of memory\n");
		return;
	}

	num = 0;

	for ( id = 1; id < VARIABLELIST_CHILD_SIZE; id++ )
	{
		entryValue = &scrVarGlob.variableList[id];

		if ( (entryValue->w.status & VAR_STAT_MASK) == VAR_STAT_FREE )
		{
			continue;
		}

		if ( (entryValue->w.type & VAR_MASK) != VAR_STACK )
		{
			continue;
		}

		pInfo = &infoArray[num];

		num++;
		info.posSize = 0;

		stackBuf = entryValue->u.u.stackValue;

		size = stackBuf->size;
		pos = stackBuf->pos;
		buf = stackBuf->buf;

		while ( size )
		{
			size--;

			value.type = *(unsigned char *)buf;
			buf += sizeof(unsigned char);

			value.u.codePosValue = *(const char **)buf;
			buf += sizeof(VariableUnion);

			if ( value.type == VAR_CODEPOS )
			{
				info.pos[info.posSize] = value.u.codePosValue;
				info.posSize++;
			}
		}

		info.pos[info.posSize] = pos;
		info.posSize++;

		pInfo->varUsage = Scr_GetThreadUsage(stackBuf, &pInfo->endonUsage);
		pInfo->posSize = info.posSize;

		info.posSize--;

		for ( j = 0; j < pInfo->posSize; j++ )
		{
			pInfo->pos[j] = info.pos[info.posSize - j];
		}
	}

	qsort(infoArray, num, sizeof(*infoArray), ThreadInfoCompare);
	Com_Printf("********************************\n");

	i = 0;

	while ( i < num )
	{
		pInfo = &infoArray[i];

		count = 0;
		info.varUsage = 0;
		info.endonUsage = 0;

		do
		{
			count++;

			info.varUsage = info.varUsage + infoArray[i].varUsage;
			info.endonUsage = info.endonUsage + infoArray[i].endonUsage;

			i++;
		}
		while ( i < num && !ThreadInfoCompare(pInfo, &infoArray[i]) );

		Com_Printf("count: %d, var usage: %d, endon usage: %d\n", count, (int)info.varUsage, (int)info.endonUsage);
		Scr_PrintPrevCodePos(CON_CHANNEL_DONT_FILTER, pInfo->pos[0], 0);

		for ( j = 1; j < pInfo->posSize; j++ )
		{
			Com_Printf("called from:\n");
			Scr_PrintPrevCodePos(CON_CHANNEL_DONT_FILTER, pInfo->pos[j], 0);
		}
	}

	Z_Free(infoArray);
	Com_Printf("********************************\n");

	for ( classnum = 0; classnum < CLASS_NUM_COUNT; classnum++ )
	{
		if ( !scrClassMap[classnum].entArrayId )
		{
			continue;
		}

		info.varUsage = 0;
		count = 0;

		for ( id = FindNextSibling(scrClassMap[classnum].entArrayId); id; id = FindNextSibling(id) )
		{
			count++;

			if ( (scrVarGlob.variableList[id].w.type & VAR_MASK) == VAR_POINTER )
			{
				info.varUsage = Scr_GetObjectUsage(scrVarGlob.variableList[id].u.u.pointerValue) + info.varUsage;
			}
		}

		Com_Printf("ent type '%s'... count: %d, var usage: %d\n", scrClassMap[classnum].name, count, (int)info.varUsage);
	}

	Com_Printf("********************************\n");
}

/*
==============
AllocChildThread
==============
*/
unsigned int AllocChildThread( unsigned int self, unsigned int parentLocalId )
{
	unsigned short id;
	VariableValueInternal *entryValue;

	id = AllocVariable();

	entryValue = &scrVarGlob.variableList[id];
	entryValue->w.status = VAR_STAT_EXTERNAL;

	assert(!(entryValue->w.type & VAR_MASK));
	entryValue->w.type |= VAR_CHILD_THREAD;

	assert(!(entryValue->w.parentLocalId & VAR_NAME_HIGH_MASK));
	entryValue->w.parentLocalId |= parentLocalId << VAR_NAME_BITS;

	entryValue->u.o.refCount = 0;
	entryValue->u.o.u.self = self;

	return id;
}

/*
==============
AllocThread
==============
*/
unsigned int AllocThread( unsigned int self )
{
	unsigned short id;
	VariableValueInternal *entryValue;

	id = AllocVariable();

	entryValue = &scrVarGlob.variableList[id];
	entryValue->w.status = VAR_STAT_EXTERNAL;

	assert(!(entryValue->w.type & VAR_MASK));
	entryValue->w.type |= VAR_THREAD;

	entryValue->u.o.refCount = 0;
	entryValue->u.o.u.self = self;

	return id;
}

/*
==============
Scr_AllocArray
==============
*/
unsigned int Scr_AllocArray()
{
	unsigned short id;
	VariableValueInternal *entryValue;

	id = AllocVariable();

	entryValue = &scrVarGlob.variableList[id];
	entryValue->w.status = VAR_STAT_EXTERNAL;

	assert(!(entryValue->w.type & VAR_MASK));
	entryValue->w.type |= VAR_ARRAY;

	entryValue->u.o.refCount = 0;
	entryValue->u.o.u.size = 0;

	return id;
}

/*
==============
AllocObject
==============
*/
unsigned int AllocObject()
{
	unsigned short id;
	VariableValueInternal *entryValue;

	id = AllocVariable();

	entryValue = &scrVarGlob.variableList[id];
	entryValue->w.status = VAR_STAT_EXTERNAL;

	assert(!(entryValue->w.type & VAR_MASK));
	entryValue->w.type |= VAR_OBJECT;

	entryValue->u.o.refCount = 0;

	return id;
}

/*
==============
AllocValue
==============
*/
unsigned int AllocValue()
{
	unsigned short id;
	VariableValueInternal *entryValue;

	id = AllocVariable();

	entryValue = &scrVarGlob.variableList[id];
	entryValue->w.status = VAR_STAT_EXTERNAL;

	return id;
}

/*
==============
Scr_InitStringSet
==============
*/
unsigned int Scr_InitStringSet( void )
{
	UNIMPLEMENTED(__FUNCTION__);
	return 0;
}

/*
==============
Scr_SetClassMap
==============
*/
void Scr_SetClassMap( int classnum )
{
	assert(!scrClassMap[classnum].entArrayId);
	assert(!scrClassMap[classnum].id);

	scrClassMap[classnum].entArrayId = Scr_AllocArray();
	scrClassMap[classnum].id = Scr_AllocArray();
}

/*
==============
GetArray
==============
*/
unsigned int GetArray( unsigned int id )
{
	assert(id);

	VariableValueInternal *entryValue = &scrVarGlob.variableList[id];

	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert((entryValue->w.type & VAR_MASK) == VAR_UNDEFINED || (entryValue->w.type & VAR_MASK) == VAR_POINTER);

	if ( (entryValue->w.type & VAR_MASK) == VAR_UNDEFINED )
	{
		entryValue->w.type |= VAR_POINTER;
		entryValue->u.u.pointerValue = Scr_AllocArray();
	}

	assert((entryValue->w.type & VAR_MASK) == VAR_POINTER);

	return entryValue->u.u.pointerValue;
}

/*
==============
GetObject
==============
*/
unsigned int GetObject_( unsigned int id )
{
	assert(id);

	VariableValueInternal *entryValue = &scrVarGlob.variableList[id];

	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert((entryValue->w.type & VAR_MASK) == VAR_UNDEFINED || (entryValue->w.type & VAR_MASK) == VAR_POINTER);

	if ( (entryValue->w.type & VAR_MASK) == VAR_UNDEFINED )
	{
		entryValue->w.type |= VAR_POINTER;
		entryValue->u.u.pointerValue = AllocObject();
	}

	assert((entryValue->w.type & VAR_MASK) == VAR_POINTER);

	return entryValue->u.u.pointerValue;
}

/*
==============
GetNewObjectVariableReverse
==============
*/
unsigned int GetNewObjectVariableReverse( unsigned int parentId, unsigned int id )
{
	assert((scrVarGlob.variableList[parentId].w.type & VAR_MASK) == VAR_ARRAY);
	return scrVarGlob.variableList[ GetNewVariableIndexReverseInternal( parentId, id + SL_MAX_STRING_INDEX ) ].hash.id;
}

/*
==============
GetNewObjectVariable
==============
*/
unsigned int GetNewObjectVariable( unsigned int parentId, unsigned int id )
{
	assert((scrVarGlob.variableList[parentId].w.type & VAR_MASK) == VAR_ARRAY);
	return scrVarGlob.variableList[ GetNewVariableIndexInternal( parentId, id + SL_MAX_STRING_INDEX ) ].hash.id;
}

/*
==============
GetObjectVariable
==============
*/
unsigned int GetObjectVariable( unsigned int parentId, unsigned int id )
{
	assert((scrVarGlob.variableList[parentId].w.type & VAR_MASK) == VAR_ARRAY);
	return scrVarGlob.variableList[ GetVariableIndexInternal( parentId, id + SL_MAX_STRING_INDEX ) ].hash.id;
}

/*
==============
GetNewVariable
==============
*/
unsigned int GetNewVariable( unsigned int parentId, unsigned int name )
{
	return scrVarGlob.variableList[ GetNewVariableIndexInternal( parentId, name ) ].hash.id;
}

/*
==============
GetVariable
==============
*/
unsigned int GetVariable( unsigned int parentId, unsigned int name )
{
	return scrVarGlob.variableList[ GetVariableIndexInternal( parentId, name ) ].hash.id;
}

/*
==============
GetVariable
==============
*/
unsigned int Scr_GetVariableField( unsigned int parentId, unsigned int name )
{
	VariableValueInternal *entryValue;
	unsigned int index, type;

	assert(parentId);
	entryValue = &scrVarGlob.variableList[parentId];

	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(IsObject(entryValue));

	type = entryValue->w.type & VAR_MASK;

	if ( type <= VAR_OBJECT )
	{
		return GetVariable(parentId, name);
	}

	if ( type != VAR_ENTITY )
	{
		Scr_Error(va("cannot set field of %s", var_typename[type]));
		return 0;
	}

	index = FindVariable(parentId, name);

	if ( !index )
	{
		scrVarPub.entId = parentId;
		scrVarPub.entFieldName = name;
		return VARIABLELIST_CHILD_SIZE;
	}

	return index;
}

/*
==============
GetNewArrayVariable
==============
*/
unsigned int GetNewArrayVariable( unsigned int parentId, unsigned int name )
{
	return scrVarGlob.variableList[ GetNewArrayVariableIndex( parentId, name ) ].hash.id;
}

/*
==============
GetArrayVariable
==============
*/
unsigned int GetArrayVariable( unsigned int parentId, unsigned int name )
{
	return scrVarGlob.variableList[ GetArrayVariableIndex( parentId, name ) ].hash.id;
}

/*
==============
RemoveRefToVector
==============
*/
void RemoveRefToVector( const vec3_t vectorValue )
{
	RefVector *refVec = (RefVector *)( (byte *)vectorValue - REFSTRING_STRING_OFFSET );

	if ( refVec->byteLen )
	{
		return;
	}

	if ( refVec->refCount )
	{
		refVec->refCount--;
		return;
	}

	MT_Free( refVec, sizeof( *refVec ) );
}

/*
==============
Scr_EvalArrayIndex
==============
*/
unsigned int Scr_EvalArrayIndex( unsigned int parentId, VariableValue *index )
{
	unsigned int stringValue;

	switch ( index->type )
	{
	case VAR_INTEGER:
		if ( IsValidArrayIndex(index->u.pointerValue) )
			return GetArrayVariable(parentId, index->u.pointerValue);
		Scr_Error(va("array index %d out of range", static_cast<int>(index->u.pointerValue)));
		return 0;

	case VAR_STRING:
		stringValue = GetVariable(parentId, index->u.stringValue);
		SL_RemoveRefToString(index->u.stringValue);
		return stringValue;

	default:
		Scr_Error(va("%s is not an array index", var_typename[index->type]));
		return 0;
	}
}

/*
==============
Scr_GetEntityId
==============
*/
unsigned int Scr_GetEntityId( int entnum, int classnum )
{
	VariableValueInternal *entryValue;
	unsigned int entId, id, entArrayId;

	assert((unsigned)entnum < (1 << 16));

	entArrayId = scrClassMap[classnum].entArrayId;
	assert(entArrayId);

	id = GetArrayVariable(entArrayId, entnum);
	assert(id);

	entryValue = &scrVarGlob.variableList[id];
	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);

	if ( ( entryValue->w.type & VAR_MASK ) != VAR_UNDEFINED )
	{
		assert((entryValue->w.type & VAR_MASK) == VAR_POINTER);
		return entryValue->u.u.pointerValue;
	}

	entId = AllocEntity(classnum, entnum);
	assert(!(entryValue->w.type & VAR_MASK));

	entryValue->w.type |= VAR_POINTER;
	entryValue->u.u.pointerValue = entId;

	return entId;
}

/*
==============
Scr_AddClassField
==============
*/
void Scr_AddClassField( int classnum, const char *name, unsigned int offset )
{
	unsigned int str, classId, fieldId;
	VariableValueInternal *entryValue;
	const char *namePos;

	assert(offset < (1 << 16));
	for ( namePos = name; *namePos; namePos++ )
	{
		assert((*namePos < 'A' || *namePos > 'Z'));
	}

	classId = scrClassMap[classnum].id;
	str = SL_GetCanonicalString(name);
	assert(!FindArrayVariable(classId, (unsigned)str));

	entryValue = &scrVarGlob.variableList[GetNewArrayVariable(classId, str)];

	entryValue->w.type &= ~VAR_MASK;
	entryValue->w.type |= VAR_INTEGER;
	entryValue->u.u.intValue = (unsigned short)offset;

	str = SL_GetString_(name, 0);
	assert(!FindVariable(classId, str));

	fieldId = GetNewVariable(classId, str);
	SL_RemoveRefToString(str);

	entryValue = &scrVarGlob.variableList[fieldId];

	entryValue->w.type &= ~VAR_MASK;
	entryValue->w.type |= VAR_INTEGER;
	entryValue->u.u.intValue = (unsigned short)offset;
}

/*
==============
RemoveRefToValue
==============
*/
void RemoveRefToValue( int type, VariableUnion u )
{
	switch ( type )
	{
	case VAR_POINTER:
		RemoveRefToObject(u.pointerValue);
		break;

	case VAR_STRING:
	case VAR_ISTRING:
		SL_RemoveRefToString(u.stringValue);
		break;

	case VAR_VECTOR:
		assert(type - 1 == VAR_VECTOR - VAR_BEGIN_REF);
		RemoveRefToVector(u.vectorValue);
		break;
	}
}

/*
==============
Scr_AllocVector
==============
*/
float* Scr_AllocVector( const vec3_t v )
{
	float* av = Scr_AllocVectorInternal();
	VectorCopy(v, av);

	return av;
}

/*
==============
FreeValue
==============
*/
void FreeValue( unsigned int id )
{
	VariableValueInternal *entryValue = &scrVarGlob.variableList[id];

	assert(((entryValue->w.status & VAR_STAT_MASK) == VAR_STAT_EXTERNAL));
	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(!IsObject( entryValue ));
	assert(scrVarGlob.variableList[entryValue->v.index].hash.id == id);

	RemoveRefToValue(entryValue->w.type & VAR_MASK, entryValue->u.u);
	FreeVariable(id);
}

/*
==============
Scr_RemoveThreadNotifyName
==============
*/
void Scr_RemoveThreadNotifyName( unsigned int startLocalId )
{
	unsigned short stringValue;
	VariableValueInternal *entryValue;

	entryValue = &scrVarGlob.variableList[startLocalId];

	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert((entryValue->w.type & VAR_MASK) == VAR_NOTIFY_THREAD);

	stringValue = Scr_GetThreadNotifyName(startLocalId);
	assert(stringValue);

	SL_RemoveRefToString(stringValue);

	entryValue->w.type &= ~VAR_MASK;
	entryValue->w.type |= VAR_THREAD;
}

/*
==============
Scr_CopyEntityNum
==============
*/
void Scr_CopyEntityNum( int fromEntnum, int toEntnum, int classnum )
{
	unsigned int fromEntId = FindEntityId( fromEntnum, classnum );

	if ( !fromEntId )
	{
		return;
	}

	if ( !FindNextSibling(fromEntId) )
	{
		return;
	}

	assert( !FindEntityId( toEntnum, classnum ) );
	CopyEntity( fromEntId, Scr_GetEntityId( toEntnum, classnum ) );
}

/*
==============
Scr_ClearVector
==============
*/
void Scr_ClearVector( VariableValue *value )
{
	for ( int i = 2; i >= 0; i-- )
	{
		RemoveRefToValue( &value[i] );
	}

	value->type = VAR_UNDEFINED;
}

/*
==============
Scr_CastString
==============
*/
bool Scr_CastString( VariableValue *value )
{
	const float *constTempVector;

	switch ( value->type )
	{
	case VAR_STRING:
		return true;

	case VAR_INTEGER:
		value->type = VAR_STRING;
		value->u.stringValue = SL_GetStringForInt(value->u.intValue);
		return true;

	case VAR_FLOAT:
		value->type = VAR_STRING;
		value->u.stringValue = SL_GetStringForFloat(value->u.floatValue);
		return true;

	case VAR_VECTOR:
		value->type = VAR_STRING;
		constTempVector = value->u.vectorValue;
		value->u.stringValue = SL_GetStringForVector(value->u.vectorValue);
		RemoveRefToVector(constTempVector);
		return true;

	default:
		scrVarPub.error_message = va("cannot cast %s to string", var_typename[value->type]);
		RemoveRefToValue(value);
		value->type = VAR_UNDEFINED;
		return false;
	}
}

/*
==============
Scr_CastBool
==============
*/
void Scr_CastBool( VariableValue *value )
{
	int type;

	switch ( value->type )
	{
	case VAR_INTEGER:
		value->u.intValue = value->u.intValue != 0;
		break;

	case VAR_FLOAT:
		value->type = VAR_INTEGER;
		value->u.intValue = value->u.floatValue != 0;
		break;

	default:
		type = value->type;
		RemoveRefToValue(value);
		value->type = VAR_UNDEFINED;
		Scr_Error(va("cannot cast %s to bool", var_typename[type]));
		break;
	}
}

/*
==============
Scr_EvalBoolComplement
==============
*/
void Scr_EvalBoolComplement( VariableValue *value )
{
	int type;

	if ( value->type == VAR_INTEGER )
	{
		value->u.intValue = ~value->u.intValue;
		return;
	}

	type = value->type;
	RemoveRefToValue(value);
	value->type = VAR_UNDEFINED;
	Scr_Error(va("~ cannot be applied to \"%s\"", var_typename[type]));
}

/*
==============
Scr_EvalBoolNot
==============
*/
void Scr_EvalBoolNot( VariableValue *value )
{
	Scr_CastBool(value);

	if ( value->type == VAR_INTEGER )
	{
		value->u.intValue = value->u.intValue == 0;
	}
}

/*
==============
ClearVariableValue
==============
*/
void ClearVariableValue( unsigned int id )
{
	VariableValueInternal *entryValue;

	assert(id);
	entryValue = &scrVarGlob.variableList[id];

	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(!IsObject( entryValue ));
	assert((entryValue->w.type & VAR_MASK) != VAR_STACK);

	RemoveRefToValue(entryValue->w.type & VAR_MASK, entryValue->u.u);

	entryValue->w.type &= ~VAR_MASK;
	assert((entryValue->w.type & VAR_MASK) == VAR_UNDEFINED);
}

/*
==============
SetVariableValue
==============
*/
void SetVariableValue( unsigned int id, VariableValue *value )
{
	VariableValueInternal *entryValue;

	assert(id);
	assert(!IsObjectVal( value ));
	assert(value->type >= 0 && value->type < VAR_COUNT);
	assert(value->type != VAR_STACK);

	entryValue = &scrVarGlob.variableList[id];

	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(!IsObject( entryValue ));
	assert((entryValue->w.type & VAR_MASK) != VAR_STACK);

	RemoveRefToValue(entryValue->w.type & VAR_MASK, entryValue->u.u);

	entryValue->w.type &= ~VAR_MASK;
	entryValue->w.type |= value->type;
	entryValue->u.u = value->u;
}

/*
==============
SetEmptyArray
==============
*/
void SetEmptyArray( unsigned int parentId )
{
	VariableValue tempValue;

	tempValue.type = VAR_POINTER;
	tempValue.u.pointerValue = Scr_AllocArray();

	SetVariableValue(parentId, &tempValue);
}

/*
==============
Scr_EvalFieldObject
==============
*/
unsigned int Scr_EvalFieldObject( unsigned int tempVariable, VariableValue *value )
{
	int type;
	VariableValue tempValue;

	type = value->type;

	if ( type == VAR_POINTER )
	{
		type = scrVarGlob.variableList[value->u.pointerValue].w.type & VAR_MASK;

		if ( type < VAR_ARRAY )
		{
			assert(type >= FIRST_OBJECT);

			tempValue.type = VAR_POINTER;
			tempValue.u = value->u;

			SetVariableValue(tempVariable, &tempValue);
			return tempValue.u.pointerValue;
		}
	}

	RemoveRefToValue(value->type, value->u);
	Scr_Error(va("%s is not a field object", var_typename[type]));
	return 0;
}

/*
==============
Scr_CastVector
==============
*/
void Scr_CastVector( VariableValue *value )
{
	int type, i;
	vec3_t vec;

	for ( i = 2; i >= 0; i-- )
	{
		type = value[i].type;

		switch ( type )
		{
		case VAR_FLOAT:
			vec[2 - i] = value[i].u.floatValue;
			break;

		case VAR_INTEGER:
			vec[2 - i] = (float)value[i].u.intValue;
			break;

		default:
			scrVarPub.error_index = i + 1;
			Scr_ClearVector(value);
			Scr_Error(va("type %s is not a float", var_typename[type]));
			return;
		}
	}

	value->type = VAR_VECTOR;
	value->u.vectorValue = Scr_AllocVector(vec);
}

/*
==============
Scr_CastDebugString
==============
*/
void Scr_CastDebugString( VariableValue *value )
{
	unsigned int stringValue;

	switch ( value->type )
	{
	case VAR_POINTER:
		stringValue = SL_GetString_( var_typename[ GetObjectType( value->u.pointerValue) ], 0 );
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
		stringValue = SL_GetString_( XAnimGetAnimDebugName( Scr_GetAnims( value->u.pointerValue >> 16 ), (unsigned short)value->u.pointerValue ), 0 );
		break;

	default:
		stringValue = SL_GetString_( var_typename[ value->type ], 0 );
		break;
	}

	RemoveRefToValue(value);

	value->type = VAR_STRING;
	value->u.stringValue = stringValue;
}

/*
==============
SafeRemoveVariable
==============
*/
void SafeRemoveVariable( unsigned int parentId, unsigned int name )
{
	VariableValueInternal *entryValue;
	unsigned int index, id;

	index = FindVariableIndexInternal(parentId, name);

	if ( !index )
	{
		return;
	}

	id = scrVarGlob.variableList[index].hash.id;
	entryValue = &scrVarGlob.variableList[id];

	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(!IsObject( entryValue ));

	MakeVariableExternal(&scrVarGlob.variableList[index], &scrVarGlob.variableList[parentId]);
	FreeValue(id);
}

/*
==============
RemoveNextVariable
==============
*/
void RemoveNextVariable( unsigned int parentId )
{
	VariableValueInternal *entryValue;
	unsigned short id;

	assert((scrVarGlob.variableList[parentId].w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	entryValue = &scrVarGlob.variableList[scrVarGlob.variableList[parentId].nextSibling];

	id = entryValue->hash.id;
	assert(id);

	MakeVariableExternal(entryValue, &scrVarGlob.variableList[parentId]);
	FreeValue(id);
}

/*
==============
RemoveVariable
==============
*/
void RemoveVariable( unsigned int parentId, unsigned int name )
{
	VariableValueInternal *entryValue;
	unsigned int id;

	assert((scrVarGlob.variableList[parentId].w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	entryValue = &scrVarGlob.variableList[FindVariableIndexInternal(parentId, name)];

	id = entryValue->hash.id;
	assert(id);

	MakeVariableExternal(entryValue, &scrVarGlob.variableList[parentId]);
	FreeValue(id);
}

/*
==============
Scr_AllocGameVariable
==============
*/
void Scr_AllocGameVariable()
{
	if ( scrVarPub.gameId )
	{
		return;
	}

	scrVarPub.gameId = AllocValue();
	SetEmptyArray(scrVarPub.gameId);
}

/*
==============
Scr_UnmatchingTypesError
==============
*/
void Scr_UnmatchingTypesError( VariableValue *value1, VariableValue *value2 )
{
	int type1, type2;
	const char *error_message = NULL;

	if ( !scrVarPub.error_message )
	{
		type1 = value1->type;
		type2 = value2->type;

		Scr_CastDebugString(value1);
		Scr_CastDebugString(value2);

		assert(value1->type == VAR_STRING);
		assert(value2->type == VAR_STRING);

		error_message = va("pair '%s' and '%s' has unmatching types '%s' and '%s'",
		                   SL_ConvertToString(value1->u.stringValue),
		                   SL_ConvertToString(value2->u.stringValue),
		                   var_typename[type1],
		                   var_typename[type2]);
	}

	RemoveRefToValue(value1);
	value1->type = VAR_UNDEFINED;

	RemoveRefToValue(value2);
	value2->type = VAR_UNDEFINED;

	Scr_Error(error_message);
}

/*
==============
RemoveObjectVariable
==============
*/
void RemoveObjectVariable( unsigned int parentId, unsigned int id )
{
	assert((scrVarGlob.variableList[parentId].w.type & VAR_MASK) == VAR_ARRAY);
	RemoveVariable(parentId, id + SL_MAX_STRING_INDEX);
}

/*
==============
ClearObject
==============
*/
void ClearObject( unsigned int parentId )
{
	assert((scrVarGlob.variableList[parentId].w.status & VAR_STAT_MASK) != VAR_STAT_FREE);

	AddRefToObject(parentId);
	ClearObjectInternal(parentId);
	RemoveRefToEmptyObject(parentId);
}

/*
==============
Scr_FreeGameVariable
==============
*/
void Scr_FreeGameVariable( int bComplete )
{
	VariableValueInternal *entryValue;

	assert(scrVarPub.gameId);

	if ( bComplete )
	{
		FreeValue(scrVarPub.gameId);
		scrVarPub.gameId = 0;
		return;
	}

	entryValue = &scrVarGlob.variableList[scrVarPub.gameId];
	assert((entryValue->w.type & VAR_MASK) == VAR_POINTER);

	Scr_MakeValuePrimitive(entryValue->u.u.pointerValue);
}

/*
==============
Scr_FreeEntityNum
==============
*/
void Scr_FreeEntityNum( int entnum, int classnum )
{
	VariableValueInternal *entryValue;
	unsigned int entId, entnumId, entArrayId;

	if ( !scrVarPub.bInited )
	{
		return;
	}

	entArrayId = scrClassMap[classnum].entArrayId;
	assert(entArrayId);

	entnumId = FindArrayVariable(entArrayId, entnum);

	if ( !entnumId )
	{
		return;
	}

	entId = FindObject(entnumId);
	assert(entId);

	entryValue = &scrVarGlob.variableList[entId];
	assert((entryValue->w.type & VAR_MASK) == VAR_ENTITY);
	assert((entryValue->w.classnum >> VAR_NAME_BITS) == classnum);

	entryValue->w.type &= ~VAR_MASK;
	entryValue->w.type |= VAR_DEAD_ENTITY;

	AddRefToObject(entId);

	entryValue->u.o.u.nextEntId = scrVarPub.freeEntList;
	scrVarPub.freeEntList = entId;

	RemoveArrayVariable(entArrayId, entnum);
}

/*
==============
Scr_EvalMod
==============
*/
void Scr_EvalMod( VariableValue *value1, VariableValue *value2 )
{
	if ( value1->type != VAR_INTEGER || value2->type != VAR_INTEGER )
	{
		Scr_UnmatchingTypesError(value1, value2);
		return;
	}

	if ( !value2->u.intValue )
	{
		value1->u.intValue = 0;
		Scr_Error("divide by 0");
		return;
	}

	value1->u.intValue %= value2->u.intValue;
}

/*
==============
Scr_EvalShiftRight
==============
*/
void Scr_EvalShiftRight( VariableValue *value1, VariableValue *value2 )
{
	if ( value1->type != VAR_INTEGER || value2->type != VAR_INTEGER )
	{
		Scr_UnmatchingTypesError(value1, value2);
		return;
	}

	value1->u.intValue >>= value2->u.intValue;
}

/*
==============
Scr_EvalShiftLeft
==============
*/
void Scr_EvalShiftLeft( VariableValue *value1, VariableValue *value2 )
{
	if ( value1->type != VAR_INTEGER || value2->type != VAR_INTEGER )
	{
		Scr_UnmatchingTypesError(value1, value2);
		return;
	}

	value1->u.intValue <<= value2->u.intValue;
}

/*
==============
Scr_EvalAnd
==============
*/
void Scr_EvalAnd( VariableValue *value1, VariableValue *value2 )
{
	if ( value1->type != VAR_INTEGER || value2->type != VAR_INTEGER )
	{
		Scr_UnmatchingTypesError(value1, value2);
		return;
	}

	value1->u.intValue &= value2->u.intValue;
}

/*
==============
Scr_EvalExOr
==============
*/
void Scr_EvalExOr( VariableValue *value1, VariableValue *value2 )
{
	if ( value1->type != VAR_INTEGER || value2->type != VAR_INTEGER )
	{
		Scr_UnmatchingTypesError(value1, value2);
		return;
	}

	value1->u.intValue ^= value2->u.intValue;
}

/*
==============
Scr_EvalOr
==============
*/
void Scr_EvalOr( VariableValue *value1, VariableValue *value2 )
{
	if ( value1->type != VAR_INTEGER || value2->type != VAR_INTEGER )
	{
		Scr_UnmatchingTypesError(value1, value2);
		return;
	}

	value1->u.intValue |= value2->u.intValue;
}

/*
==============
RemoveRefToObject
==============
*/
void RemoveRefToObject( unsigned int id )
{
	VariableValueInternal *entryValue;
	unsigned int classnum, entArrayId;

	assert(id);
	entryValue = &scrVarGlob.variableList[id];

	assert((entryValue->w.status & VAR_STAT_MASK) == VAR_STAT_EXTERNAL);
	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(IsObject( entryValue ));

	if ( !entryValue->u.o.refCount )
	{
		if ( scrVarGlob.variableList[entryValue->nextSibling].hash.id != id )
		{
			ClearObject(id);
		}

		FreeVariable(id);
		return;
	}

	entryValue->u.o.refCount--;

	if ( entryValue->u.o.refCount )
	{
		return;
	}

	if ( (entryValue->w.type & VAR_MASK) != VAR_ENTITY )
	{
		return;
	}

	if ( scrVarGlob.variableList[entryValue->nextSibling].hash.id != id )
	{
		return;
	}

	entryValue->w.type &= ~VAR_MASK;
	entryValue->w.type |= VAR_DEAD_ENTITY;

	classnum = entryValue->w.classnum >> VAR_NAME_BITS;
	assert(classnum < CLASS_NUM_COUNT);

	entArrayId = scrClassMap[classnum].entArrayId;
	assert(entArrayId);

	RemoveArrayVariable(entArrayId, entryValue->u.o.u.entnum);
}

/*
==============
Scr_KillEndonThread
==============
*/
void Scr_KillEndonThread( unsigned int threadId )
{
	VariableValueInternal *parentValue;

	parentValue = &scrVarGlob.variableList[threadId];
	assert((parentValue->w.status & VAR_STAT_MASK) == VAR_STAT_EXTERNAL);
	assert((parentValue->w.type & VAR_MASK) == VAR_THREAD);
	//assert(!parentValue->nextSibling);

	RemoveRefToObject(parentValue->u.o.u.self);
	assert(!FindObjectVariable( scrVarPub.pauseArrayId, threadId ));

	parentValue->w.type &= ~VAR_MASK;
	parentValue->w.type |= VAR_DEAD_THREAD;
}

/*
==============
Scr_ShutdownStringSet
==============
*/
void Scr_ShutdownStringSet( unsigned int setId )
{
	UNIMPLEMENTED(__FUNCTION__);
}

/*
==============
Scr_FreeValue
==============
*/
void Scr_FreeValue( unsigned int id )
{
	assert(id);
	RemoveRefToObject(id);
}

/*
==============
Scr_EvalArray
==============
*/
void Scr_EvalArray( VariableValue *value, VariableValue *index )
{
	char c[2];
	const char *s;
	VariableValueInternal *entryValue;

	switch( value->type )
	{
	case VAR_POINTER:
		entryValue = &scrVarGlob.variableList[value->u.pointerValue];
		assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
		assert(IsObject(entryValue));

		if ( (entryValue->w.type & VAR_MASK) != VAR_ARRAY )
		{
			scrVarPub.error_index = 1;
			Scr_Error(va("%s is not an array", var_typename[entryValue->w.type & VAR_MASK]));
			return;
		}

		*index = Scr_EvalVariable(Scr_FindArrayIndex(value->u.pointerValue, index));

		RemoveRefToObject(value->u.pointerValue);
		break;

	case VAR_STRING:
		if ( index->type != VAR_INTEGER )
		{
			Scr_Error(va("%s is not a string index", var_typename[index->type]));
			return;
		}

		if ( index->u.intValue < 0 )
		{
			Scr_Error(va("string index %d out of range", index->u.intValue));
			return;
		}

		s = SL_ConvertToString(value->u.stringValue);

		if ( index->u.intValue >= strlen(s) )
		{
			Scr_Error(va("string index %d out of range", index->u.intValue));
			return;
		}

		index->type = VAR_STRING;

		c[0] = s[index->u.intValue];
		c[1] = 0;

		index->u.stringValue = SL_GetStringOfLen(c, 0, sizeof(c));

		SL_RemoveRefToString(value->u.stringValue);
		break;

	case VAR_VECTOR:
		if ( index->type != VAR_INTEGER )
		{
			Scr_Error(va("%s is not a vector index", var_typename[index->type]));
			return;
		}

		if ( index->u.intValue > 2 )
		{
			Scr_Error(va("vector index %d out of range", index->u.intValue));
			return;
		}

		index->type = VAR_FLOAT;
		index->u.floatValue = value->u.vectorValue[index->u.intValue];

		RemoveRefToVector(value->u.vectorValue);
		break;

	default:
		assert(value->type != VAR_STACK);
		scrVarPub.error_index = 1;
		Scr_Error(va("%s is not an array, string, or vector", var_typename[value->type]));
		break;
	}
}

/*
==============
Scr_RemoveClassMap
==============
*/
void Scr_RemoveClassMap( int classnum )
{
	if ( !scrVarPub.bInited )
	{
		return;
	}

	RemoveRefToObject(scrClassMap[classnum].entArrayId);
	scrClassMap[classnum].entArrayId = 0;

	RemoveRefToObject(scrClassMap[classnum].id);
	scrClassMap[classnum].id = 0;
}

/*
==============
Scr_EvalDivide
==============
*/
void Scr_EvalDivide( VariableValue *value1, VariableValue *value2 )
{
	float *tempVector;

	Scr_CastWeakerPair(value1, value2);
	assert(value1->type == value2->type);

	switch ( value1->type )
	{
	case VAR_VECTOR:
		tempVector = Scr_AllocVectorInternal();

		if ( value2->u.vectorValue[0] == 0 || value2->u.vectorValue[1] == 0 || value2->u.vectorValue[2] == 0 )
		{
			VectorClear(tempVector);

			RemoveRefToVector(value1->u.vectorValue);
			RemoveRefToVector(value2->u.vectorValue);

			value1->u.vectorValue = tempVector;

			Scr_Error("divide by 0");
			return;
		}

		tempVector[0] = value1->u.vectorValue[0] / value2->u.vectorValue[0];
		tempVector[1] = value1->u.vectorValue[1] / value2->u.vectorValue[1];
		tempVector[2] = value1->u.vectorValue[2] / value2->u.vectorValue[2];

		RemoveRefToVector(value1->u.vectorValue);
		RemoveRefToVector(value2->u.vectorValue);

		value1->u.vectorValue = tempVector;
		break;

	case VAR_FLOAT:
		if ( value2->u.floatValue == 0 )
		{
			value1->u.floatValue = 0;
			Scr_Error("divide by 0");
			return;
		}

		value1->u.floatValue /= value2->u.floatValue;
		break;

	case VAR_INTEGER:
		value1->type = VAR_FLOAT;

		if ( value2->u.intValue == 0 )
		{
			value1->u.intValue = 0;
			Scr_Error("divide by 0");
			return;
		}

		value1->u.floatValue = (float)value1->u.intValue / (float)value2->u.intValue;
		break;

	default:
		Scr_UnmatchingTypesError(value1, value2);
		break;
	}
}

/*
==============
Scr_EvalMultiply
==============
*/
void Scr_EvalMultiply( VariableValue *value1, VariableValue *value2 )
{
	float *tempVector;

	Scr_CastWeakerPair(value1, value2);
	assert(value1->type == value2->type);

	switch ( value1->type )
	{
	case VAR_VECTOR:
		tempVector = Scr_AllocVectorInternal();

		tempVector[0] = value1->u.vectorValue[0] * value2->u.vectorValue[0];
		tempVector[1] = value1->u.vectorValue[1] * value2->u.vectorValue[1];
		tempVector[2] = value1->u.vectorValue[2] * value2->u.vectorValue[2];

		RemoveRefToVector(value1->u.vectorValue);
		RemoveRefToVector(value2->u.vectorValue);

		value1->u.vectorValue = tempVector;
		break;

	case VAR_FLOAT:
		value1->u.floatValue *= value2->u.floatValue;
		break;

	case VAR_INTEGER:
		value1->u.intValue *= value2->u.intValue;
		break;

	default:
		Scr_UnmatchingTypesError(value1, value2);
		break;
	}
}

/*
==============
Scr_EvalMinus
==============
*/
void Scr_EvalMinus( VariableValue *value1, VariableValue *value2 )
{
	float *tempVector;

	Scr_CastWeakerPair(value1, value2);
	assert(value1->type == value2->type);

	switch ( value1->type )
	{
	case VAR_VECTOR:
		tempVector = Scr_AllocVectorInternal();

		tempVector[0] = value1->u.vectorValue[0] - value2->u.vectorValue[0];
		tempVector[1] = value1->u.vectorValue[1] - value2->u.vectorValue[1];
		tempVector[2] = value1->u.vectorValue[2] - value2->u.vectorValue[2];

		RemoveRefToVector(value1->u.vectorValue);
		RemoveRefToVector(value2->u.vectorValue);

		value1->u.vectorValue = tempVector;
		break;

	case VAR_FLOAT:
		value1->u.floatValue -= value2->u.floatValue;
		break;

	case VAR_INTEGER:
		value1->u.intValue -= value2->u.intValue;
		break;

	default:
		Scr_UnmatchingTypesError(value1, value2);
		break;
	}
}

/*
==============
Scr_EvalPlus
==============
*/
void Scr_EvalPlus( VariableValue *value1, VariableValue *value2 )
{
	unsigned int s, len, s1len;
	float *tempVector;
	const char *s1, *s2;
	char str[8192];

	Scr_CastWeakerStringPair(value1, value2);
	assert(value1->type == value2->type);

	switch ( value1->type )
	{
	case VAR_STRING:
		s1 = SL_ConvertToString(value1->u.stringValue);
		s2 = SL_ConvertToString(value2->u.stringValue);

		s1len = SL_GetStringLen(value1->u.stringValue);
		len = s1len + SL_GetStringLen(value2->u.stringValue) + 1;

		if ( len >= sizeof(str) )
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
		tempVector = Scr_AllocVectorInternal();

		tempVector[0] = value1->u.vectorValue[0] + value2->u.vectorValue[0];
		tempVector[1] = value1->u.vectorValue[1] + value2->u.vectorValue[1];
		tempVector[2] = value1->u.vectorValue[2] + value2->u.vectorValue[2];

		RemoveRefToVector(value1->u.vectorValue);
		RemoveRefToVector(value2->u.vectorValue);

		value1->u.vectorValue = tempVector;
		break;

	case VAR_FLOAT:
		value1->u.floatValue += value2->u.floatValue;
		break;

	case VAR_INTEGER:
		value1->u.intValue += value2->u.intValue;
		break;

	default:
		Scr_UnmatchingTypesError(value1, value2);
		break;
	}
}

/*
==============
Scr_EvalGreater
==============
*/
void Scr_EvalGreater( VariableValue *value1, VariableValue *value2 )
{
	Scr_CastWeakerPair(value1, value2);
	assert(value1->type == value2->type);

	switch ( value1->type )
	{
	case VAR_FLOAT:
		value1->type = VAR_INTEGER;
		value1->u.intValue = value1->u.floatValue > value2->u.floatValue;
		break;

	case VAR_INTEGER:
		value1->u.intValue = value1->u.intValue > value2->u.intValue;
		break;

	default:
		Scr_UnmatchingTypesError(value1, value2);
		break;
	}
}

/*
==============
Scr_EvalLess
==============
*/
void Scr_EvalLess(VariableValue *value1, VariableValue *value2)
{
	Scr_CastWeakerPair(value1, value2);
	assert(value1->type == value2->type);

	switch ( value1->type )
	{
	case VAR_FLOAT:
		value1->type = VAR_INTEGER;
		value1->u.intValue = value1->u.floatValue < value2->u.floatValue;
		break;

	case VAR_INTEGER:
		value1->u.intValue = value1->u.intValue < value2->u.intValue;
		break;

	default:
		Scr_UnmatchingTypesError(value1, value2);
		break;
	}
}

/*
==============
Scr_EvalEquality
==============
*/
void Scr_EvalEquality( VariableValue *value1, VariableValue *value2 )
{
	int equal;

	Scr_CastWeakerPair(value1, value2);
	assert(value1->type == value2->type);

	switch ( value1->type )
	{
	case VAR_UNDEFINED:
		value1->type = VAR_INTEGER;
		value1->u.intValue = 1;
		break;

	case VAR_POINTER:
		if ( (scrVarGlob.variableList[value1->u.intValue].w.type & VAR_MASK) == VAR_ARRAY || (scrVarGlob.variableList[value2->u.intValue].w.type & VAR_MASK) == VAR_ARRAY )
		{
			if ( !scrVarPub.evaluate )
			{
				Scr_UnmatchingTypesError(value1, value2);
				return;
			}
		}

		value1->type = VAR_INTEGER;
		equal = value1->u.intValue == value2->u.intValue;

		RemoveRefToObject(value1->u.intValue);
		RemoveRefToObject(value2->u.intValue);

		value1->u.intValue = equal;
		break;

	case VAR_STRING:
	case VAR_ISTRING:
		value1->type = VAR_INTEGER;
		equal = value1->u.intValue == value2->u.intValue;

		SL_RemoveRefToString(value1->u.intValue);
		SL_RemoveRefToString(value2->u.intValue);

		value1->u.intValue = equal;
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
		value1->u.intValue = I_fabs(value1->u.floatValue - value2->u.floatValue) < 0.000001;
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

	default:
		Scr_UnmatchingTypesError(value1, value2);
		break;
	}
}

/*
==============
Scr_EvalSizeValue
==============
*/
void Scr_EvalSizeValue( VariableValue *value )
{
	unsigned int stringValue, id;
	const char *error_message;
	VariableValueInternal *entryValue;

	switch ( value->type )
	{
	case VAR_POINTER:
		id = value->u.pointerValue;
		entryValue = &scrVarGlob.variableList[id];
		value->type = VAR_INTEGER;
		if ( (entryValue->w.type & VAR_MASK) == VAR_ARRAY )
			value->u.intValue = entryValue->u.o.u.size;
		else
			value->u.intValue = 1;
		RemoveRefToObject(id);
		break;

	case VAR_STRING:
		value->type = VAR_INTEGER;
		stringValue = value->u.stringValue;
		value->u.intValue = strlen(SL_ConvertToString(stringValue));
		SL_RemoveRefToString(stringValue);
		break;

	default:
		assert(value->type != VAR_STACK);
		error_message = va("size cannot be applied to %s", var_typename[value->type]);
		RemoveRefToValue(value);
		value->type = VAR_UNDEFINED;
		Scr_Error(error_message);
		break;
	}
}

/*
==============
Scr_KillThread
==============
*/
void Scr_KillThread( unsigned int parentId )
{
	unsigned int name, notifyListEntry, selfNameId, id;
	VariableValueInternal *parentValue;

	assert(parentId);
	parentValue = &scrVarGlob.variableList[parentId];

	assert((parentValue->w.status & VAR_STAT_MASK) == VAR_STAT_EXTERNAL);
	assert(((parentValue->w.type & VAR_MASK) >= VAR_THREAD) && ((parentValue->w.type & VAR_MASK) <= VAR_CHILD_THREAD));
	Scr_ClearThread(parentId);

	id = FindObjectVariable(scrVarPub.pauseArrayId, parentId);

	if ( id )
	{
		for ( selfNameId = FindObject(id); ; RemoveObjectVariable(selfNameId, name) )
		{
			notifyListEntry = FindNextSibling(selfNameId);

			if ( !notifyListEntry )
			{
				break;
			}

			name = (unsigned short)(scrVarGlob.variableList[notifyListEntry].w.name >> VAR_NAME_BITS);
			//assert((name - SL_MAX_STRING_INDEX) < (1 << 16));

			VM_CancelNotify(GetVariableValueAddress(FindObjectVariable(selfNameId, name))->pointerValue, name);
			Scr_KillEndonThread(name);
		}

		assert(!GetArraySize(selfNameId));
		RemoveObjectVariable(scrVarPub.pauseArrayId, parentId);
	}

	parentValue->w.type &= ~VAR_MASK;
	parentValue->w.type |= VAR_DEAD_THREAD;
}

/*
==============
Scr_StopThread
==============
*/
void Scr_StopThread( unsigned int threadId )
{
	assert(threadId);
	Scr_ClearThread(threadId);

	scrVarGlob.variableList[threadId].u.o.u.self = scrVarPub.levelId;
	AddRefToObject(scrVarPub.levelId);
}

/*
==============
Scr_EvalLessEqual
==============
*/
void Scr_EvalLessEqual( VariableValue *value1, VariableValue *value2 )
{
	Scr_EvalGreater(value1, value2);
	assert((value1->type == VAR_INTEGER) || (value1->type == VAR_UNDEFINED));

	value1->u.intValue = value1->u.intValue == 0;
}

/*
==============
Scr_EvalGreaterEqual
==============
*/
void Scr_EvalGreaterEqual( VariableValue *value1, VariableValue *value2 )
{
	Scr_EvalLess(value1, value2);
	assert((value1->type == VAR_INTEGER) || (value1->type == VAR_UNDEFINED));

	value1->u.intValue = value1->u.intValue == 0;
}

/*
==============
Scr_EvalInequality
==============
*/
void Scr_EvalInequality( VariableValue *value1, VariableValue *value2 )
{
	Scr_EvalEquality(value1, value2);
	assert((value1->type == VAR_INTEGER) || (value1->type == VAR_UNDEFINED));

	value1->u.intValue = value1->u.intValue == 0;
}

/*
==============
SetVariableEntityFieldValue
==============
*/
void SetVariableEntityFieldValue( unsigned int entId, unsigned int fieldName, VariableValue *value )
{
	VariableValueInternal *entValue, *entryValue;
	unsigned int fieldId;

	assert(!IsObjectVal( value ));
	assert(value->type != VAR_STACK);

	entValue = &scrVarGlob.variableList[entId];
	assert((entValue->w.type & VAR_MASK) == VAR_ENTITY);
	assert((entValue->w.classnum >> VAR_NAME_BITS) < CLASS_NUM_COUNT);

	fieldId = FindArrayVariable(scrClassMap[scrVarGlob.variableList[entId].w.name >> VAR_NAME_BITS].id, fieldName);

	if ( fieldId )
	{
		if ( SetEntityFieldValue( entValue->w.classnum >> VAR_NAME_BITS, entValue->u.o.u.entnum, scrVarGlob.variableList[fieldId].u.u.entityOffset, value ) )
		{
			return;
		}
	}

	entryValue = &scrVarGlob.variableList[GetNewVariable(entId, fieldName)];
	assert(!(entryValue->w.type & VAR_MASK));

	entryValue->w.type |= value->type;
	entryValue->u.u = value->u;
}

/*
==============
ClearVariableField
==============
*/
void ClearVariableField( unsigned int parentId, unsigned int name, VariableValue *value )
{
	unsigned int classnum, fieldId;
	VariableValueInternal *parentValue, *entryValue;

	entryValue = &scrVarGlob.variableList[parentId];
	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(IsObject(entryValue));
	assert(((scrVarGlob.variableList[parentId].w.type & VAR_MASK) >= FIRST_OBJECT && (scrVarGlob.variableList[parentId].w.type & VAR_MASK) < FIRST_NONFIELD_OBJECT) || ((scrVarGlob.variableList[parentId].w.type & VAR_MASK) >= FIRST_DEAD_OBJECT));

	if ( FindVariableIndexInternal(parentId, name) )
	{
		RemoveVariable(parentId, name);
		return;
	}

	parentValue = &scrVarGlob.variableList[parentId];
	assert((parentValue->w.classnum >> VAR_NAME_BITS) < CLASS_NUM_COUNT);

	if ( (parentValue->w.type & VAR_MASK) != VAR_ENTITY )
	{
		return;
	}

	classnum = parentValue->w.classnum >> VAR_NAME_BITS;
	fieldId = FindArrayVariable(scrClassMap[classnum].id, name);

	if ( !fieldId )
	{
		return;
	}

	value += 1;
	value->type = VAR_UNDEFINED;

	SetEntityFieldValue( classnum, parentValue->u.o.u.entnum, scrVarGlob.variableList[fieldId].u.u.entityOffset, value );
}

/*
==============
Scr_FreeObjects
==============
*/
void Scr_FreeObjects()
{
	VariableValueInternal *entryValue;
	unsigned int id;

	for ( id = 1; id < VARIABLELIST_CHILD_SIZE; id++ )
	{
		entryValue = &scrVarGlob.variableList[id];

		if ( !( entryValue->w.status & VAR_STAT_MASK ) )
		{
			continue;
		}

		if ( (entryValue->w.type & VAR_MASK) == VAR_OBJECT || (entryValue->w.type & VAR_MASK) == VAR_DEAD_ENTITY )
		{
			Scr_CancelNotifyList(id);
			ClearObject(id);
		}
	}
}

/*
==============
Scr_FreeEntityList
==============
*/
void Scr_FreeEntityList()
{
	unsigned int entId;
	VariableValueInternal *entryValue;

	while ( scrVarPub.freeEntList )
	{
		entId = scrVarPub.freeEntList;
		entryValue = &scrVarGlob.variableList[entId];

		scrVarPub.freeEntList = entryValue->u.o.u.nextEntId;
		entryValue->u.o.u.entnum = 0;

		Scr_CancelNotifyList(entId);

		if ( scrVarGlob.variableList[entryValue->nextSibling].hash.id != entId )
		{
			ClearObjectInternal(entId);
		}

		RemoveRefToObject(entId);
	}
}

/*
==============
Scr_EvalBinaryOperator
==============
*/
void Scr_EvalBinaryOperator( int op, VariableValue *value1, VariableValue *value2 )
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
	}
}

/*
==============
SetVariableFieldValue
==============
*/
void SetVariableFieldValue( unsigned int id, VariableValue *value )
{
	if ( id == VARIABLELIST_CHILD_SIZE )
	{
		SetVariableEntityFieldValue(scrVarPub.entId, scrVarPub.entFieldName, value);
		return;
	}

	SetVariableValue(id, value);
}

/*
==============
Scr_AddStringSet
==============
*/
bool Scr_AddStringSet( unsigned int setId, const char *string )
{
	UNIMPLEMENTED(__FUNCTION__);
	return false;
}

/*
==============
ClearArray
==============
*/
void ClearArray( unsigned int parentId, VariableValue *value )
{
	unsigned int fieldId, id;
	VariableValue varValue;
	VariableValueInternal *parentValue, *entryValue, *entValue;

	if ( parentId == VARIABLELIST_CHILD_SIZE )
	{
		entValue = &scrVarGlob.variableList[scrVarPub.entId];
		assert((entValue->w.type & VAR_MASK) == VAR_ENTITY);
		assert((entValue->w.classnum >> VAR_NAME_BITS) < CLASS_NUM_COUNT);

		fieldId = FindArrayVariable(scrClassMap[(entValue->w.classnum >> VAR_NAME_BITS)].id, scrVarPub.entFieldName);

		if ( fieldId )
		{
			varValue = GetEntityFieldValue(entValue->w.classnum >> VAR_NAME_BITS, entValue->u.o.u.entnum, scrVarGlob.variableList[fieldId].u.u.entityOffset);
		}

		if ( !fieldId || varValue.type == VAR_UNDEFINED )
		{
			varValue.type = VAR_UNDEFINED;
			scrVarPub.error_index = 1;
			Scr_Error(va("%s is not an array", var_typename[varValue.type]));
			return;
		}

		if ( varValue.type == VAR_POINTER && !scrVarGlob.variableList[varValue.u.pointerValue].u.o.refCount )
		{
			RemoveRefToValue(&varValue);
			scrVarPub.error_index = 1;
			Scr_Error("read-only array cannot be changed");
			return;
		}

		RemoveRefToValue(&varValue);
		assert((varValue.type != VAR_POINTER) || !scrVarGlob.variableList[varValue.u.pointerValue].u.o.refCount);
		parentValue = NULL;
	}
	else
	{
		parentValue = &scrVarGlob.variableList[parentId];
		assert((parentValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);

		varValue.type = parentValue->w.type & VAR_MASK;
		varValue.u = parentValue->u.u;
	}

	if ( varValue.type != VAR_POINTER )
	{
		assert(varValue.type != VAR_STACK);
		scrVarPub.error_index = 1;
		Scr_Error(va("%s is not an array", var_typename[varValue.type]));
		return;
	}

	entryValue = &scrVarGlob.variableList[varValue.u.pointerValue];
	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(IsObject(entryValue));

	if ( (entryValue->w.type & VAR_MASK) != VAR_ARRAY )
	{
		scrVarPub.error_index = 1;
		Scr_Error(va("%s is not an array", var_typename[(entryValue->w.type & VAR_MASK)]));
		return;
	}

	if ( entryValue->u.o.refCount )
	{
		id = varValue.u.pointerValue;
		RemoveRefToObject(varValue.u.pointerValue);
		varValue.u.pointerValue = Scr_AllocArray();
		CopyArray(id, varValue.u.pointerValue);
		assert(parentValue);
		parentValue->u.u = varValue.u;
	}

	switch( value->type )
	{
	case VAR_INTEGER:
		if ( IsValidArrayIndex(value->u.pointerValue) )
			SafeRemoveArrayVariable(varValue.u.pointerValue, value->u.stringValue);
		else
			Scr_Error(va("array index %d out of range", static_cast<int>(value->u.pointerValue)));
		break;

	case VAR_STRING:
		SL_RemoveRefToString(value->u.stringValue);
		SafeRemoveVariable(varValue.u.pointerValue, value->u.stringValue);
		break;

	default:
		Scr_Error(va("%s is not an array index", var_typename[value->type]));
		break;
	}
}

/*
==============
Scr_EvalArrayRef
==============
*/
unsigned int Scr_EvalArrayRef( unsigned int parentId )
{
	unsigned int fieldId, id;
	VariableValue varValue;
	VariableValueInternal *parentValue, *entryValue, *entValue;

	if ( parentId == VARIABLELIST_CHILD_SIZE )
	{
		entValue = &scrVarGlob.variableList[scrVarPub.entId];
		assert((entValue->w.type & VAR_MASK) == VAR_ENTITY);
		assert((entValue->w.classnum >> VAR_NAME_BITS) < CLASS_NUM_COUNT);

		fieldId = FindArrayVariable(scrClassMap[(entValue->w.classnum >> VAR_NAME_BITS)].id, scrVarPub.entFieldName);

		if ( fieldId )
		{
			varValue = GetEntityFieldValue(entValue->w.classnum >> VAR_NAME_BITS, entValue->u.o.u.entnum, scrVarGlob.variableList[fieldId].u.u.entityOffset);
		}

		if ( !fieldId || varValue.type == VAR_UNDEFINED )
		{
			parentValue = &scrVarGlob.variableList[GetNewVariable(scrVarPub.entId, scrVarPub.entFieldName)];
			assert(!(parentValue->w.type & VAR_MASK));

			parentValue->w.type |= VAR_POINTER;
			parentValue->u.u.pointerValue = Scr_AllocArray();

			return parentValue->u.u.pointerValue;
		}

		if ( varValue.type == VAR_POINTER && !scrVarGlob.variableList[varValue.u.pointerValue].u.o.refCount )
		{
			RemoveRefToValue(&varValue);
			scrVarPub.error_index = 1;
			Scr_Error("read-only array cannot be changed");
			return 0;
		}

		RemoveRefToValue(&varValue);
		assert((varValue.type != VAR_POINTER) || !scrVarGlob.variableList[varValue.u.pointerValue].u.o.refCount);
		parentValue = NULL;
	}
	else
	{
		parentValue = &scrVarGlob.variableList[parentId];
		assert((parentValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);

		varValue.type = parentValue->w.type & VAR_MASK;

		if ( varValue.type == VAR_UNDEFINED )
		{
			parentValue->w.type |= VAR_POINTER;
			parentValue->u.u.pointerValue = Scr_AllocArray();

			return parentValue->u.u.pointerValue;
		}

		varValue.u = parentValue->u.u;
	}

	if ( varValue.type != VAR_POINTER )
	{
		assert(varValue.type != VAR_STACK);
		scrVarPub.error_index = 1;

		switch ( varValue.type )
		{
		case VAR_STRING:
			Scr_Error("string characters cannot be individually changed");
			return 0;

		case VAR_VECTOR:
			Scr_Error("vector components cannot be individually changed");
			return 0;

		default:
			Scr_Error(va("%s is not an array", var_typename[varValue.type]));
			return 0;
		}
	}

	entryValue = &scrVarGlob.variableList[varValue.u.pointerValue];
	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(IsObject(entryValue));

	if ( (entryValue->w.type & VAR_MASK) != VAR_ARRAY )
	{
		scrVarPub.error_index = 1;
		Scr_Error(va("%s is not an array", var_typename[(entryValue->w.type & VAR_MASK)]));
		return 0;
	}

	if ( entryValue->u.o.refCount )
	{
		id = varValue.u.pointerValue;
		RemoveRefToObject(varValue.u.pointerValue);
		varValue.u.pointerValue = Scr_AllocArray();
		CopyArray(id, varValue.u.pointerValue);
		assert(parentValue);
		parentValue->u.u = varValue.u;
	}

	return varValue.u.pointerValue;
}

/*
==============
Scr_FindVariableField
==============
*/
VariableValue Scr_FindVariableField( unsigned int parentId, unsigned int name )
{
	unsigned int id;
	VariableValueInternal *entryValue;
	VariableValue value;

	assert(parentId);
	entryValue = &scrVarGlob.variableList[parentId];
	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(IsObject( entryValue ));
	assert(((entryValue->w.type & VAR_MASK) >= FIRST_OBJECT && (entryValue->w.type & VAR_MASK) < FIRST_NONFIELD_OBJECT) || ((entryValue->w.type & VAR_MASK) >= FIRST_DEAD_OBJECT));

	id = FindVariable(parentId, name);

	if ( id )
	{
		return Scr_EvalVariable(id);
	}

	if ( (scrVarGlob.variableList[parentId].w.type & VAR_MASK) == VAR_ENTITY )
	{
		return Scr_EvalVariableEntityField(parentId, name);
	}

	value.type = VAR_UNDEFINED;
	return value;
}

/*
==============
Scr_EvalVariableField
==============
*/
VariableValue Scr_EvalVariableField( unsigned int id )
{
	if ( id == VARIABLELIST_CHILD_SIZE )
	{
		return Scr_EvalVariableEntityField(scrVarPub.entId, scrVarPub.entFieldName);
	}

	return Scr_EvalVariable(id);
}

/*
==============
Scr_AddFields
==============
*/
void Scr_AddFields( const char *path, const char *extension )
{
	char filename[MAX_QPATH];
	char **files;
	int numFiles, i;

	files = FS_ListFiles(path, extension, FS_LIST_PURE_ONLY, &numFiles);

	TempMemoryReset();

	scrVarPub.fieldBuffer = (const char *)Hunk_AllocLowInternal(0);
	*(char *)scrVarPub.fieldBuffer = 0;

	for ( i = 0; i < numFiles; i++ )
	{
		sprintf(filename, "%s/%s", path, files[i]);
		Scr_AddFieldsForFile(filename);
	}

	if ( files )
	{
		FS_FreeFileList(files);
	}

	*(char *)TempMalloc(1) = 0;

	Hunk_ConvertTempToPermLowInternal();
}

/*
==============
Scr_GetEntryUsage
==============
*/
float Scr_GetEntryUsage( unsigned int type, VariableUnion u )
{
	VariableValueInternal *parentValue;

	if ( type != VAR_POINTER )
	{
		return 0;
	}

	parentValue = &scrVarGlob.variableList[u.pointerValue];
	assert((parentValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(IsObject(parentValue));

	if ( ( parentValue->w.type & VAR_MASK ) == VAR_ARRAY )
	{
		return Scr_GetObjectUsage( u.pointerValue ) / ( parentValue->u.o.refCount + 1.0 );
	}

	return 0;
}

/*
==============
FreeVariable
==============
*/
void FreeVariable( unsigned int id )
{
	VariableValueInternal *entryValue, *entry, *childValue, *firstValue;
	unsigned short index, childId;

	assert(id);

	firstValue = &scrVarGlob.variableList[0];
	entryValue = &scrVarGlob.variableList[id];
	assert(((entryValue->w.status & VAR_STAT_MASK) == VAR_STAT_EXTERNAL));

	index = entryValue->v.index;
	entry = &scrVarGlob.variableList[index];
	//assert(entry->id == id);
	//assert(!entry->u.prevSibling);
	//assert(!entryValue->nextSibling);

	childValue = &scrVarGlob.variableList[entryValue->nextSibling];
	childValue->hash.u.prev = entry->hash.u.prev;

	childId = scrVarGlob.variableList[childValue->hash.u.prev].hash.id;

	childValue = &scrVarGlob.variableList[childId];
	childValue->nextSibling = entryValue->nextSibling;

	entryValue->w.type = VAR_UNDEFINED;
	entryValue->u.next = firstValue->u.next;

	entry->hash.u.prev = 0;

	childValue = &scrVarGlob.variableList[firstValue->u.next];
	childValue->hash.u.prev = index;

	firstValue->u.next = index;
}

/*
==============
FindVariableIndexInternal2
==============
*/
unsigned int FindVariableIndexInternal2( unsigned int name, unsigned int index )
{
	Variable *entry, *newEntry;
	VariableValueInternal *list, *entryValue, *newEntryValue;
	unsigned int newIndex;

	list = scrVarGlob.variableList;
	assert(!(name & ~VAR_NAME_LOW_MASK));
	assert((unsigned)index < VARIABLELIST_CHILD_SIZE);

	entry = &list[index].hash;
	assert((unsigned)entry->id < VARIABLELIST_CHILD_SIZE);

	entryValue = &list[entry->id];

	if ( (entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_HEAD )
	{
		return 0;
	}

	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(!IsObject( entryValue ));

	if ( entryValue->w.name >> VAR_NAME_BITS == name )
	{
		return index;
	}

	newIndex = entryValue->v.index;

	for ( newEntry = &list[newIndex].hash; newEntry != entry; newEntry = &list[newIndex].hash )
	{
		newEntryValue = &list[newEntry->id];
		assert((newEntryValue->w.status & VAR_STAT_MASK) == VAR_STAT_MOVABLE);
		assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
		assert(!IsObject( newEntryValue ));

		if ( newEntryValue->w.name >> VAR_NAME_BITS == name )
		{
			return newIndex;
		}

		newIndex = newEntryValue->v.index;
	}

	return 0;
}

/*
==============
InitVariables
==============
*/
void InitVariables()
{
	unsigned short parentId;
	VariableValueInternal *value, *firstValue;
	unsigned int index;

	for ( parentId = 0, index = 1; index < VARIABLELIST_CHILD_SIZE; index++ )
	{
		value = &scrVarGlob.variableList[index];

		value->w.type = VAR_UNDEFINED;
		assert(!(value->w.type & VAR_MASK));

		value->hash.id = index;
		value->v.index = index;

		scrVarGlob.variableList[parentId].u.next = index;
		value->hash.u.prev = parentId;

		parentId = index;
	}

	value = &scrVarGlob.variableList[0];

	value->w.type = VAR_UNDEFINED;
	assert(!(value->w.type & VAR_MASK));

	//value->w = value->w;

	value->hash.id = 0;
	value->v.index = 0;

	scrVarGlob.variableList[parentId].u.next = 0;
	value->hash.u.prev = parentId;
}

/*
==============
ThreadInfoCompare
==============
*/
int ThreadInfoCompare( const void *info1, const void *info2 )
{
	const char *pos1, *pos2;
	ThreadDebugInfo *pInfo1 = (ThreadDebugInfo *)info1, *pInfo2 = (ThreadDebugInfo *)info2;

	for ( int i = 0; ; i++ )
	{
		if ( i >= pInfo1->posSize || i >= pInfo2->posSize )
		{
			return pInfo1->posSize - pInfo2->posSize;
		}

		pos1 = pInfo1->pos[i];
		pos2 = pInfo2->pos[i];

		if ( pos1 != pos2 )
		{
			break;
		}
	}

	return pos1 - pos2;
}

/*
==============
Scr_GetEntryUsage
==============
*/
float Scr_GetEntryUsage( VariableValueInternal *entryValue )
{
	return Scr_GetEntryUsage( entryValue->w.type & VAR_MASK, entryValue->u.u ) + 1.0;
}

/*
==============
FindVariableIndexInternal
==============
*/
unsigned int FindVariableIndexInternal( unsigned int parentId, unsigned int name )
{
	VariableValueInternal *parentValue;

	assert(parentId);
	parentValue = &scrVarGlob.variableList[parentId];
	assert((parentValue->w.status & VAR_STAT_MASK) == VAR_STAT_EXTERNAL);
	assert((parentValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(IsObject( parentValue ));

	return FindVariableIndexInternal2( name, ( parentId + name ) % ( VARIABLELIST_CHILD_SIZE - 1 ) + 1 );
}

/*
==============
Scr_GetObjectUsage
==============
*/
float Scr_GetObjectUsage( unsigned int parentId )
{
	VariableValueInternal *parentValue;
	float usage = 1.0;
	unsigned int id;

	parentValue = &scrVarGlob.variableList[parentId];
	assert((parentValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(IsObject( parentValue ));

	for ( id = FindNextSibling(parentId); id; id = FindNextSibling(id) )
	{
		usage += Scr_GetEntryUsage(&scrVarGlob.variableList[id]);
	}

	return usage;
}

/*
==============
FindArrayVariableIndex
==============
*/
unsigned int FindArrayVariableIndex( unsigned int parentId, unsigned int index )
{
	assert(IsValidArrayIndex( index ));
	return FindVariableIndexInternal( parentId, ( index + MAX_ARRAYINDEX ) & VAR_NAME_LOW_MASK );
}

/*
==============
Scr_GetEndonUsage
==============
*/
float Scr_GetEndonUsage( unsigned int parentId )
{
	VariableValueInternal *parentValue;
	unsigned int id;

	parentValue = &scrVarGlob.variableList[parentId];
	assert((parentValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(IsObject( parentValue ));

	id = FindObjectVariable( scrVarPub.pauseArrayId, parentId );

	if ( !id )
	{
		return 0;
	}

	return Scr_GetObjectUsage( FindObject( id ) );
}

/*
==============
Scr_GetThreadUsage
==============
*/
float Scr_GetThreadUsage( VariableStackBuffer *stackBuf, float *endonUsage )
{
	unsigned int localId;
	float usage;
	int size;
	char *buf;
	VariableValue value;

	size = stackBuf->size;
	buf = &stackBuf->buf[STACKBUF_BUFFER_SIZE * size];

	usage = Scr_GetObjectUsage(stackBuf->localId);
	*endonUsage = Scr_GetEndonUsage(stackBuf->localId);

	localId = stackBuf->localId;

	while ( size )
	{
		buf -= sizeof(VariableUnion);
		value.u.codePosValue = *(const char **)buf;

		buf -= sizeof(unsigned char);
		value.type = *(unsigned char *)buf;

		size--;

		if ( value.type != VAR_CODEPOS )
		{
			usage += Scr_GetEntryUsage( value.type, value.u );
			continue;
		}

		localId = GetParentLocalId(localId);

		usage += Scr_GetObjectUsage(localId);
		*endonUsage += Scr_GetEndonUsage(localId);
	}

	return usage;
}

/*
==============
AllocVariable
==============
*/
unsigned short AllocVariable()
{
	VariableValueInternal *entryValue, *entry, *firstValue;
	unsigned short index, next, newIndex;

	firstValue = &scrVarGlob.variableList[0];
	index = firstValue->u.next;

	if ( !index )
	{
		Scr_TerminalError("exceeded maximum number of script variables");
	}

	entry = &scrVarGlob.variableList[index];
	entryValue = &scrVarGlob.variableList[entry->hash.id];
	assert((entryValue->w.status & VAR_STAT_MASK) == VAR_STAT_FREE);

	next = entryValue->u.next;

	if ( entry != entryValue && !(entry->w.status & VAR_STAT_MASK) )
	{
		newIndex = entry->v.next;
		assert(newIndex != index);

		scrVarGlob.variableList[newIndex].hash.id = entry->hash.id;
		entry->hash.id = index;

		entryValue->v.index = newIndex;
		entryValue->u.next = entry->u.next;

		entryValue = &scrVarGlob.variableList[index];
	}

	firstValue->u.next = next;
	scrVarGlob.variableList[next].hash.u.prev = 0;

	entryValue->v.index = index;

	entryValue->nextSibling = index;
	entry->hash.u.prevSibling = index;

	assert(entry->hash.id);
	return entry->hash.id;
}

/*
==============
GetNewVariableIndexInternal3
==============
*/
unsigned int GetNewVariableIndexInternal3( unsigned int parentId, unsigned int name, unsigned int index )
{
	VariableValueInternal *entry, *entryValue, *newEntry, *newEntryValue, *parentValue;
	unsigned short newIndex, next, prev, nextSiblingIndex, prevId, id;
	VariableValue value;
	int type;

	assert(!(name & ~VAR_NAME_LOW_MASK));

	entry = &scrVarGlob.variableList[index];
	entryValue = &scrVarGlob.variableList[entry->hash.id];
	type = entryValue->w.status & VAR_STAT_MASK;

	switch ( type )
	{
	case VAR_STAT_FREE:
		newIndex = entry->v.index;
		next = entryValue->u.next;

		if ( newIndex == entry->hash.id || entry->w.status & VAR_STAT_MASK )
		{
			newEntryValue = entryValue;
		}
		else
		{
			scrVarGlob.variableList[newIndex].hash.id = entry->hash.id;
			entry->hash.id = index;

			entryValue->v.index = newIndex;
			entryValue->u.next = entry->u.next;

			newEntryValue = entry;
		}

		prev = entry->hash.u.prev;

		assert(!scrVarGlob.variableList[prev].hash.id || (scrVarGlob.variableList[scrVarGlob.variableList[prev].hash.id].w.status & VAR_STAT_MASK) == VAR_STAT_FREE);
		assert(!scrVarGlob.variableList[next].hash.id || (scrVarGlob.variableList[scrVarGlob.variableList[next].hash.id].w.status & VAR_STAT_MASK) == VAR_STAT_FREE);

		scrVarGlob.variableList[scrVarGlob.variableList[prev].hash.id].u.next = next;
		scrVarGlob.variableList[next].hash.u.prev = prev;

		newEntryValue->w.status = VAR_STAT_HEAD;
		newEntryValue->v.index = index;
		break;

	case VAR_STAT_HEAD:
		if ( entry->w.status & VAR_STAT_MASK )
		{
			index = scrVarGlob.variableList[0].u.next;

			if ( !index )
			{
				Scr_TerminalError("exceeded maximum number of script variables");
			}

			entry = &scrVarGlob.variableList[index];
			newEntryValue = &scrVarGlob.variableList[entry->hash.id];
			assert((newEntryValue->w.status & VAR_STAT_MASK) == VAR_STAT_FREE);
			next = newEntryValue->u.next;

			scrVarGlob.variableList[0].u.next = next;
			scrVarGlob.variableList[next].hash.u.prev = 0;

			newEntryValue->w.status = VAR_STAT_MOVABLE;
			newEntryValue->v.next = entryValue->v.next;

			entryValue->v.index = index;
		}
		else
		{
			newIndex = entry->v.index;
			newEntry = &scrVarGlob.variableList[newIndex];
			newEntryValue = entry;

			prev = newEntry->hash.u.prev;
			next = entry->u.next;

			scrVarGlob.variableList[scrVarGlob.variableList[prev].hash.id].u.next = next;
			scrVarGlob.variableList[next].hash.u.prevSibling = prev;

			newEntry->hash.id = entry->hash.id;
			entry->hash.id = index;
			newEntry->hash.u.prev = entry->hash.u.prev;

			scrVarGlob.variableList[scrVarGlob.variableList[newEntry->hash.u.prev].hash.id].nextSibling = newIndex;
			scrVarGlob.variableList[entryValue->nextSibling].hash.u.prevSibling = newIndex;

			entryValue->w.type &= ~VAR_STAT_MASK;
			entryValue->w.type |= VAR_STAT_MOVABLE;

			newEntryValue->w.status = VAR_STAT_HEAD;
		}
		break;

	default:
		assert(type == VAR_STAT_MOVABLE || type == VAR_STAT_EXTERNAL);
		if ( entry->w.status & VAR_STAT_MASK )
		{
			newIndex = scrVarGlob.variableList[0].u.next;

			if ( !newIndex )
			{
				Scr_TerminalError("exceeded maximum number of script variables");
			}

			newEntry = &scrVarGlob.variableList[newIndex];
			newEntryValue = &scrVarGlob.variableList[newEntry->hash.id];
			assert((newEntryValue->w.status & VAR_STAT_MASK) == VAR_STAT_FREE);

			next = newEntryValue->u.next;

			scrVarGlob.variableList[0].u.next = next;
			scrVarGlob.variableList[next].hash.u.prev = 0;
		}
		else
		{
			assert(entry != entryValue);
			newIndex = entry->v.index;
			newEntry = &scrVarGlob.variableList[newIndex];
			newEntryValue = entry;

			prev = newEntry->hash.u.prev;
			next = entry->u.next;

			scrVarGlob.variableList[scrVarGlob.variableList[prev].hash.id].u.next = next;
			scrVarGlob.variableList[next].hash.u.prev = prev;
		}

		nextSiblingIndex = entryValue->nextSibling;

		scrVarGlob.variableList[scrVarGlob.variableList[entry->hash.u.prev].hash.id].nextSibling = newIndex;
		scrVarGlob.variableList[nextSiblingIndex].hash.u.prev = newIndex;

		if ( type == VAR_STAT_MOVABLE )
		{
			nextSiblingIndex = entryValue->v.index;
			prevId = scrVarGlob.variableList[nextSiblingIndex].hash.id;
			assert((scrVarGlob.variableList[prevId].w.status & VAR_STAT_MASK) == VAR_STAT_MOVABLE || (scrVarGlob.variableList[prevId].w.status & VAR_STAT_MASK) == VAR_STAT_HEAD);

			while ( 1 )
			{
				if ( scrVarGlob.variableList[prevId].v.index == index )
				{
					break;
				}

				prevId = scrVarGlob.variableList[scrVarGlob.variableList[prevId].v.next].hash.id;
				assert((scrVarGlob.variableList[prevId].w.status & VAR_STAT_MASK) == VAR_STAT_MOVABLE || (scrVarGlob.variableList[prevId].w.status & VAR_STAT_MASK) == VAR_STAT_HEAD);
			}

			scrVarGlob.variableList[prevId].v.index = newIndex;
		}
		else
		{
			assert(type == VAR_STAT_EXTERNAL);
			entryValue->v.index = newIndex;
		}

		newEntry->hash.u.prev = entry->hash.u.prev;
		id = newEntry->hash.id;
		newEntry->hash.id = entry->hash.id;
		entry->hash.id = id;
		newEntryValue->w.status = VAR_STAT_HEAD;
		newEntryValue->v.index = index;
		break;
	}

	assert(entry == &scrVarGlob.variableList[index]);
	assert(newEntryValue == &scrVarGlob.variableList[entry->hash.id]);
	assert((newEntryValue->w.type & VAR_MASK) == VAR_UNDEFINED);

	newEntryValue->w.type = (unsigned char)newEntryValue->w.type;
	newEntryValue->w.name |= name << VAR_NAME_BITS;

	parentValue = &scrVarGlob.variableList[parentId];

	assert((parentValue->w.status & VAR_STAT_MASK) == VAR_STAT_EXTERNAL);
	assert((parentValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(IsObject( parentValue ));

	if ( ( parentValue->w.type & VAR_MASK ) == VAR_ARRAY )
	{
		parentValue->u.o.u.size++;
		value = Scr_GetArrayIndexValue( name );
		AddRefToValue( &value );
	}

	return index;
}

/*
==============
AllocEntity
==============
*/
unsigned int AllocEntity( int classnum, unsigned short entnum )
{
	unsigned short id;
	VariableValueInternal *entryValue;

	id = AllocVariable();

	entryValue = &scrVarGlob.variableList[id];
	entryValue->w.status = VAR_STAT_EXTERNAL;

	assert(!(entryValue->w.type & VAR_MASK));
	entryValue->w.type |= VAR_ENTITY;

	assert(!(entryValue->w.classnum & VAR_NAME_HIGH_MASK));
	entryValue->w.classnum |= classnum << VAR_NAME_BITS;

	entryValue->u.o.refCount = 0;
	entryValue->u.o.u.entnum = entnum;

	return id;
}

/*
==============
GetNewVariableIndexReverseInternal2
==============
*/
unsigned int GetNewVariableIndexReverseInternal2( unsigned int parentId, unsigned int name, unsigned int index )
{
	VariableValueInternal *parentValue, *siblingValue, *parent, *entry;
	unsigned short siblingId;

	index = GetNewVariableIndexInternal3(parentId, name, index);

	parentValue = &scrVarGlob.variableList[parentId];
	assert(((parentValue->w.status & VAR_STAT_MASK) == VAR_STAT_EXTERNAL));

	parent = &scrVarGlob.variableList[scrVarGlob.variableList[parentValue->nextSibling].hash.u.prev];
	entry = &scrVarGlob.variableList[index];

	siblingId = parent->hash.u.prevSibling;
	siblingValue = &scrVarGlob.variableList[scrVarGlob.variableList[siblingId].hash.id];

	//assert((siblingValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	//assert(!IsObject( siblingValue ));

	scrVarGlob.variableList[entry->hash.id].nextSibling = parentValue->v.next;
	parent->hash.u.prev = index;

	entry->hash.u.prevSibling = siblingId;
	siblingValue->nextSibling = index;

	return index;
}

/*
==============
GetNewVariableIndexInternal2
==============
*/
unsigned int GetNewVariableIndexInternal2( unsigned int parentId, unsigned int name, unsigned int index )
{
	VariableValueInternal *parentValue, *siblingValue, *entry;
	unsigned short siblingId;

	index = GetNewVariableIndexInternal3(parentId, name, index);

	parentValue = &scrVarGlob.variableList[parentId];
	assert(((parentValue->w.status & VAR_STAT_MASK) == VAR_STAT_EXTERNAL));

	entry = &scrVarGlob.variableList[index];

	siblingId = parentValue->nextSibling;
	siblingValue = &scrVarGlob.variableList[siblingId];

	//assert((siblingValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	//assert(!IsObject( siblingValue ));

	scrVarGlob.variableList[entry->hash.id].nextSibling = siblingId;
	siblingValue->hash.u.prevSibling = index;

	entry->hash.u.prev = parentValue->v.next;
	parentValue->nextSibling = index;

	return index;
}

/*
==============
GetVariableIndexInternal
==============
*/
unsigned int GetVariableIndexInternal( unsigned int parentId, unsigned int name )
{
	VariableValueInternal *parentValue;
	unsigned int newIndex;

	assert(parentId);
	parentValue = &scrVarGlob.variableList[parentId];
	assert((parentValue->w.status & VAR_STAT_MASK) == VAR_STAT_EXTERNAL);
	assert((parentValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(IsObject( parentValue ));

	newIndex = FindVariableIndexInternal2( name, ( parentId + name ) % ( VARIABLELIST_CHILD_SIZE - 1 ) + 1 );

	if ( !newIndex )
	{
		newIndex = GetNewVariableIndexInternal2( parentId, name, ( parentId + name ) % ( VARIABLELIST_CHILD_SIZE - 1 ) + 1 );
	}

	return newIndex;
}

/*
==============
GetNewVariableIndexReverseInternal
==============
*/
unsigned int GetNewVariableIndexReverseInternal( unsigned int parentId, unsigned int name )
{
	assert(!FindVariableIndexInternal( parentId, name ));
	return GetNewVariableIndexReverseInternal2( parentId, name, ( parentId + name ) % ( VARIABLELIST_CHILD_SIZE - 1 ) + 1 );
}

/*
==============
GetNewVariableIndexInternal
==============
*/
unsigned int GetNewVariableIndexInternal( unsigned int parentId, unsigned int name )
{
	assert(!FindVariableIndexInternal( parentId, name ));
	return GetNewVariableIndexInternal2( parentId, name, ( parentId + name ) % ( VARIABLELIST_CHILD_SIZE - 1 ) + 1 );
}

/*
==============
CopyArray
==============
*/
void CopyArray( unsigned int parentId, unsigned int newParentId )
{
	unsigned int id;
	VariableValueInternal *parentValue, *entryValue, *newEntryValue;
	int type;

	parentValue = &scrVarGlob.variableList[parentId];

	assert((parentValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(IsObject( parentValue ));
	assert((parentValue->w.type & VAR_MASK) == VAR_ARRAY);

	id = scrVarGlob.variableList[parentValue->nextSibling].hash.id;
	assert(id);

	while ( 1 )
	{
		if ( id == parentId )
		{
			break;
		}

		entryValue = &scrVarGlob.variableList[id];
		type = entryValue->w.type & VAR_MASK;

		assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
		assert(!IsObject( entryValue ));

		newEntryValue = &scrVarGlob.variableList[scrVarGlob.variableList[ GetVariableIndexInternal( newParentId, entryValue->w.name >> VAR_NAME_BITS ) ].hash.id ];

		assert((newEntryValue->w.type & VAR_MASK) == VAR_UNDEFINED);
		assert(!(newEntryValue->w.type & VAR_MASK));

		newEntryValue->w.type |= type;

		if ( type == VAR_POINTER )
		{
			if ( ( scrVarGlob.variableList[entryValue->u.u.pointerValue].w.type & VAR_MASK ) == VAR_ARRAY )
			{
				newEntryValue->u.u.pointerValue = Scr_AllocArray();
				CopyArray(entryValue->u.u.pointerValue, newEntryValue->u.u.pointerValue);
			}
			else
			{
				newEntryValue->u.u.pointerValue = entryValue->u.u.pointerValue;
				AddRefToObject(entryValue->u.u.pointerValue);
			}
		}
		else
		{
			assert(type != VAR_STACK);
			newEntryValue->u.u.pointerValue = entryValue->u.u.pointerValue;
			AddRefToValue(type, entryValue->u.u);
		}

		id = scrVarGlob.variableList[entryValue->nextSibling].hash.id;
		assert(id);
	}
}

/*
==============
GetNewArrayVariableIndex
==============
*/
unsigned int GetNewArrayVariableIndex( unsigned int parentId, unsigned int index )
{
	assert(IsValidArrayIndex( index ));
	return GetNewVariableIndexInternal( parentId, ( index + MAX_ARRAYINDEX ) & VAR_NAME_LOW_MASK );
}

/*
==============
GetArrayVariableIndex
==============
*/
unsigned int GetArrayVariableIndex( unsigned int parentId, unsigned int index )
{
	assert(IsValidArrayIndex( index ));
	return GetVariableIndexInternal( parentId, ( index + MAX_ARRAYINDEX ) & VAR_NAME_LOW_MASK );
}

/*
==============
CopyEntity
==============
*/
void CopyEntity( unsigned int parentId, unsigned int newParentId )
{
	VariableValueInternal *entryValue, *newEntryValue, *parentValue, *newParentValue;
	unsigned int name, id;
	int type;

	assert(parentId);
	assert(newParentId);

	parentValue = &scrVarGlob.variableList[parentId];

	assert((parentValue->w.status & VAR_STAT_MASK) == VAR_STAT_EXTERNAL);
	assert((parentValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(IsObject( parentValue ));
	assert((parentValue->w.type & VAR_MASK) == VAR_ENTITY);

	newParentValue = &scrVarGlob.variableList[newParentId];

	assert((newParentValue->w.status & VAR_STAT_MASK) == VAR_STAT_EXTERNAL);
	assert((newParentValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(IsObject( newParentValue ));
	assert((newParentValue->w.type & VAR_MASK) == VAR_ENTITY);

	for ( id = FindNextSibling(parentId); id; id = FindNextSibling(id) )
	{
		entryValue = &scrVarGlob.variableList[id];

		assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE && (entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_EXTERNAL);
		assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
		assert(!IsObject( entryValue ));

		name = entryValue->w.name >> VAR_NAME_BITS;
		assert(name != OBJECT_STACK);

		if ( name == OBJECT_NOTIFY_LIST )
		{
			continue;
		}

		assert(!FindVariableIndexInternal( newParentId, name ));
		newEntryValue = &scrVarGlob.variableList[GetVariable(newParentId, name)];

		assert((newEntryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE && (newEntryValue->w.status & VAR_STAT_MASK) != VAR_STAT_EXTERNAL);
		assert((newEntryValue->w.type & VAR_MASK) == VAR_UNDEFINED);

		type = entryValue->w.type & VAR_MASK;
		assert(!(newEntryValue->w.type & VAR_MASK));

		newEntryValue->w.type |= type;
		assert((newEntryValue->w.name >> VAR_NAME_BITS) == name);
		newEntryValue->u.u = entryValue->u.u;

		AddRefToValue(type, newEntryValue->u.u);
	}
}

/*
==============
Scr_AllocVectorInternal
==============
*/
float* Scr_AllocVectorInternal()
{
	RefVector *refVec = (RefVector *)MT_Alloc( sizeof( *refVec ) );
	refVec->head = 0;

	return refVec->vec;
}

/*
==============
Scr_FindArrayIndex
==============
*/
unsigned int Scr_FindArrayIndex( unsigned int parentId, VariableValue *index )
{
	unsigned int id;

	switch ( index->type )
	{
	case VAR_INTEGER:
		if ( IsValidArrayIndex(index->u.intValue) )
			return FindArrayVariable(parentId, index->u.intValue);
		Scr_Error(va("array index %d out of range", index->u.intValue));
		AddRefToObject(parentId);
		return 0;

	case VAR_STRING:
		id = FindVariable(parentId, index->u.stringValue);
		SL_RemoveRefToString(index->u.stringValue);
		return id;

	default:
		Scr_Error(va("%s is not an array index", var_typename[index->type]));
		AddRefToObject(parentId);
		return 0;
	}
}

/*
==============
MakeVariableExternal
==============
*/
void MakeVariableExternal( VariableValueInternal *entry, VariableValueInternal *parentValue )
{
	unsigned int oldNextSiblingIndex, oldPrevSiblingIndex, nextSiblingIndex, prevSiblingIndex, index, oldIndex;
	VariableValueInternal *entryValue, *oldEntryValue, *oldEntry, *prev;
	VariableValue value;
	Variable tempEntry;

	index = entry - scrVarGlob.variableList;
	entryValue = &scrVarGlob.variableList[entry->hash.id];

	assert((entryValue->w.status & VAR_STAT_MASK) == VAR_STAT_MOVABLE || (entryValue->w.status & VAR_STAT_MASK) == VAR_STAT_HEAD);
	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(!IsObject( entryValue ));

	if ( (parentValue->w.type & VAR_MASK) == VAR_ARRAY )
	{
		parentValue->u.o.u.size--;

		assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
		assert(!IsObject( entryValue ));

		value = Scr_GetArrayIndexValue(entryValue->w.name >> VAR_NAME_BITS);
		RemoveRefToValue(&value);
	}

	if ( (entryValue->w.status & VAR_STAT_MASK) == VAR_STAT_HEAD )
	{
		oldIndex = entryValue->v.index;
		oldEntry = &scrVarGlob.variableList[oldIndex];

		oldEntryValue = &scrVarGlob.variableList[oldEntry->hash.id];

		if ( oldEntry != entry )
		{
			assert((oldEntryValue->w.status & VAR_STAT_MASK) == VAR_STAT_MOVABLE);

			oldEntryValue->w.type &= ~VAR_STAT_MASK;
			oldEntryValue->w.type |= VAR_STAT_HEAD;

			prevSiblingIndex = entry->hash.u.prevSibling;
			nextSiblingIndex = entryValue->nextSibling;

			oldPrevSiblingIndex = oldEntry->hash.u.prevSibling;
			oldNextSiblingIndex = oldEntryValue->nextSibling;

			scrVarGlob.variableList[oldNextSiblingIndex].hash.u.prev = index;
			scrVarGlob.variableList[scrVarGlob.variableList[oldPrevSiblingIndex].hash.id].nextSibling = index;

			scrVarGlob.variableList[nextSiblingIndex].hash.u.prev = oldIndex;
			scrVarGlob.variableList[scrVarGlob.variableList[prevSiblingIndex].hash.id].nextSibling = oldIndex;

			tempEntry = entry->hash;
			entry->hash = oldEntry->hash;

			oldEntry->hash = tempEntry;
			index = oldIndex;
		}
	}
	else
	{
		oldEntry = entry;
		oldEntryValue = entryValue;

		do
		{
			assert((oldEntryValue->w.status & VAR_STAT_MASK) == VAR_STAT_MOVABLE || (oldEntryValue->w.status & VAR_STAT_MASK) == VAR_STAT_HEAD);
			prev = oldEntry;

			oldIndex = oldEntryValue->v.index;
			oldEntry = &scrVarGlob.variableList[oldIndex];

			oldEntryValue = &scrVarGlob.variableList[oldEntry->hash.id];
		}
		while ( oldEntry != entry );

		scrVarGlob.variableList[prev->hash.id].v.index = entryValue->v.index;
	}

	assert(entryValue == &scrVarGlob.variableList[oldEntry->hash.id]);

	entryValue->w.type &= ~VAR_STAT_MASK;
	entryValue->w.type |= VAR_STAT_EXTERNAL;

	entryValue->v.index = index;
}

/*
==============
SafeRemoveArrayVariable
==============
*/
void SafeRemoveArrayVariable( unsigned int parentId, unsigned int index )
{
	assert(IsValidArrayIndex( index ));
	SafeRemoveVariable( parentId, ( index + MAX_ARRAYINDEX ) & VAR_NAME_LOW_MASK );
}

/*
==============
ClearObjectInternal
==============
*/
void ClearObjectInternal( unsigned int parentId )
{
	unsigned int id, nextId;
	VariableValueInternal *parentValue, *entryValue;

	parentValue = &scrVarGlob.variableList[parentId];
	assert((parentValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(IsObject( parentValue ));

	entryValue = &scrVarGlob.variableList[parentValue->nextSibling];
	//assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	//assert(!IsObject( entryValue ));

	for ( id = entryValue->hash.id; id != parentId; id = entryValue->hash.id )
	{
		MakeVariableExternal(entryValue, parentValue);
		entryValue = &scrVarGlob.variableList[scrVarGlob.variableList[id].nextSibling];
	}

	nextId = scrVarGlob.variableList[parentValue->nextSibling].hash.id;

	while ( nextId != parentId )
	{
		id = nextId;
		nextId = scrVarGlob.variableList[scrVarGlob.variableList[nextId].nextSibling].hash.id;
		FreeValue(id);
	}
}

/*
==============
Scr_MakeValuePrimitive
==============
*/
int Scr_MakeValuePrimitive( unsigned int parentId )
{
	VariableValueInternal *parentValue, *entryValue;
	unsigned int name, id;

	parentValue = &scrVarGlob.variableList[parentId];

	assert((parentValue->w.status & VAR_STAT_MASK) == VAR_STAT_EXTERNAL);
	assert((parentValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(IsObject( parentValue ));
	assert((parentValue->w.type & VAR_MASK) != VAR_THREAD);
	assert((parentValue->w.type & VAR_MASK) != VAR_NOTIFY_THREAD);
	assert((parentValue->w.type & VAR_MASK) != VAR_TIME_THREAD);
	assert((parentValue->w.type & VAR_MASK) != VAR_CHILD_THREAD);

	if ( ( parentValue->w.type & VAR_MASK ) != VAR_ARRAY )
	{
		return 0;
	}

	id = FindNextSibling(parentId);

	while ( id )
	{
		entryValue = &scrVarGlob.variableList[id];

		assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
		assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_EXTERNAL);
		assert(!IsObject( entryValue ));

		name = entryValue->w.name >> VAR_NAME_BITS;

		switch ( entryValue->w.type & VAR_MASK )
		{
		case VAR_CODEPOS:
		case VAR_PRECODEPOS:
		case VAR_FUNCTION:
		case VAR_STACK:
		case VAR_ANIMATION:
			RemoveVariable(parentId, name);
			id = FindNextSibling(parentId);
			continue;

		case VAR_POINTER:
			if ( !Scr_MakeValuePrimitive(entryValue->u.u.pointerValue) )
			{
				RemoveVariable(parentId, name);
				id = FindNextSibling(parentId);
				continue;
			}
			break;

		case VAR_UNDEFINED:
		case VAR_STRING:
		case VAR_ISTRING:
		case VAR_VECTOR:
		case VAR_FLOAT:
		case VAR_INTEGER:
			break;

		default:
			assert(0);
			break;
		}

		id = FindNextSibling(id);
	}

	return 1;
}

/*
==============
RemoveArrayVariable
==============
*/
void RemoveArrayVariable( unsigned int parentId, unsigned int index )
{
	assert(IsValidArrayIndex( index ));
	RemoveVariable( parentId, ( index + MAX_ARRAYINDEX ) & VAR_NAME_LOW_MASK );
}

/*
==============
Scr_CastWeakerStringPair
==============
*/
void Scr_CastWeakerStringPair( VariableValue *value1, VariableValue *value2 )
{
	int type1, type2;
	const float *constTempVector;

	type1 = value1->type;
	type2 = value2->type;

	if ( type1 == type2 )
	{
		return;
	}

	if ( type1 < type2 )
	{
		switch ( type1 )
		{
		case VAR_STRING:
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
			return;

		case VAR_FLOAT:
		case VAR_INTEGER: // just treat int value as float??
			value2->type = VAR_FLOAT;
			value2->u.floatValue = (float)value2->u.intValue;
			return;

		default:
			Scr_UnmatchingTypesError(value1, value2);
			return;
		}
	}
	else
	{
		switch ( type2 )
		{
		case VAR_STRING:
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
			return;

		case VAR_FLOAT:
		case VAR_INTEGER: // just treat int value as float??
			value1->type = VAR_FLOAT;
			value1->u.floatValue = (float)value1->u.intValue;
			return;

		default:
			Scr_UnmatchingTypesError(value1, value2);
			return;
		}
	}
}

/*
==============
Scr_CastWeakerPair
==============
*/
void Scr_CastWeakerPair( VariableValue *value1, VariableValue *value2 )
{
	float *tempVector;
	int type1, type2;

	type1 = value1->type;
	type2 = value2->type;

	if ( type1 == type2 )
	{
		return;
	}

	if ( type1 == VAR_FLOAT && type2 == VAR_INTEGER )
	{
		value2->type = VAR_FLOAT;
		value2->u.floatValue = (float)value2->u.intValue;
		return;
	}

	if ( type1 == VAR_INTEGER && type2 == VAR_FLOAT )
	{
		value1->type = VAR_FLOAT;
		value1->u.floatValue = (float)value1->u.intValue;
		return;
	}

	if ( type1 == VAR_VECTOR )
	{
		switch ( type2 )
		{
		case VAR_FLOAT:
			tempVector = Scr_AllocVectorInternal();
			tempVector[0] = value2->u.floatValue;
			tempVector[1] = value2->u.floatValue;
			tempVector[2] = value2->u.floatValue;
			value2->u.vectorValue = tempVector;
			value2->type = VAR_VECTOR;
			return;

		case VAR_INTEGER:
			tempVector = Scr_AllocVectorInternal();
			tempVector[0] = (float)value2->u.intValue;
			tempVector[1] = (float)value2->u.intValue;
			tempVector[2] = (float)value2->u.intValue;
			value2->u.vectorValue = tempVector;
			value2->type = VAR_VECTOR;
			return;
		}
	}

	if ( type2 == VAR_VECTOR )
	{
		switch ( type1 )
		{
		case VAR_FLOAT:
			tempVector = Scr_AllocVectorInternal();
			tempVector[0] = value1->u.floatValue;
			tempVector[1] = value1->u.floatValue;
			tempVector[2] = value1->u.floatValue;
			value1->u.vectorValue = tempVector;
			value1->type = VAR_VECTOR;
			return;

		case VAR_INTEGER:
			tempVector = Scr_AllocVectorInternal();
			tempVector[0] = (float)value1->u.intValue;
			tempVector[1] = (float)value1->u.intValue;
			tempVector[2] = (float)value1->u.intValue;
			value1->u.vectorValue = tempVector;
			value1->type = VAR_VECTOR;
			return;
		}
	}

	Scr_UnmatchingTypesError(value1, value2);
	return;
}

/*
==============
Scr_ClearThread
==============
*/
void Scr_ClearThread( unsigned int parentId )
{
	VariableValueInternal *parentValue;

	assert(parentId);
	parentValue = &scrVarGlob.variableList[parentId];
	assert((parentValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert(((parentValue->w.type & VAR_MASK) >= VAR_THREAD) && ((parentValue->w.type & VAR_MASK) <= VAR_CHILD_THREAD));
	assert(!FindVariable( parentId, OBJECT_STACK ));

	if ( scrVarGlob.variableList[parentValue->nextSibling].hash.id != parentId )
	{
		ClearObjectInternal(parentId);
	}

	RemoveRefToObject(parentValue->u.o.u.self);
}

/*
==============
Scr_EvalVariableEntityField
==============
*/
VariableValue Scr_EvalVariableEntityField( unsigned int entId, unsigned int name )
{
	VariableValueInternal *entValue, *entryValue;
	unsigned int fieldId, id;
	VariableValue value;

	entValue = &scrVarGlob.variableList[entId];
	assert((entValue->w.type & VAR_MASK) == VAR_ENTITY);
	assert((entValue->w.classnum >> VAR_NAME_BITS) < CLASS_NUM_COUNT);

	fieldId = FindArrayVariable(scrClassMap[entValue->w.parentLocalId >> VAR_NAME_BITS].id, name);

	if ( !fieldId )
	{
		value.type = VAR_UNDEFINED;
		return value;
	}

	value = GetEntityFieldValue(entValue->w.classnum >> VAR_NAME_BITS, entValue->u.o.u.entnum, scrVarGlob.variableList[fieldId].u.u.entityOffset);

	if ( value.type != VAR_POINTER )
	{
		return value;
	}

	entryValue = &scrVarGlob.variableList[value.u.pointerValue];

	if ( (entryValue->w.type & VAR_MASK) != VAR_ARRAY )
	{
		return value;
	}

	if ( !entryValue->u.o.refCount )
	{
		return value;
	}

	id = value.u.pointerValue;
	RemoveRefToObject(id);
	value.u.pointerValue = Scr_AllocArray();
	CopyArray(id, value.u.pointerValue);

	return value;
}

/*
==============
Scr_AddFieldsForFile
==============
*/
void Scr_AddFieldsForFile( const char *filename )
{
	fileHandle_t f;
	int type, tempType, i, len;
	unsigned int index;
	char *targetPos, *token, *sourceBuffer;
	const char *sourcePos;

	len = FS_FOpenFileByMode(filename, &f, FS_READ);

	if ( len < 0 )
	{
		Com_Error(ERR_DROP, va("cannot find '%s'", filename));
	}

	sourceBuffer = (char *)Hunk_AllocateTempMemoryHighInternal( len + 1 );
	FS_Read(sourceBuffer, len, f);

	sourceBuffer[len] = 0;
	FS_FCloseFile(f);

	sourcePos = sourceBuffer;
	Com_BeginParseSession("Scr_AddFields");

	while ( 1 )
	{
		token = Com_Parse(&sourcePos);

		if ( !sourcePos )
		{
			break;
		}

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
		else if ( !strcmp(token, "vector") )
		{
			type = VAR_VECTOR;
		}
		else
		{
			Com_Error(ERR_DROP, va("unknown type '%s' in '%s'", token, filename));
		}

		token = Com_Parse(&sourcePos);

		if ( !sourcePos )
		{
			Com_Error(ERR_DROP, va("missing field name in '%s'", filename));
		}

		len = strlen(token) + 1;

		for ( i = len - 1; i >= 0; i-- )
		{
			token[i] = tolower(token[i]);
		}

		index = SL_GetCanonicalString(token);

		if ( Scr_FindField(token, &tempType) )
		{
			Com_Error(ERR_DROP, "duplicate key '%s' in '%s'", token, filename);
		}

		//assert(targetPos == TempMalloc( 0 ) - 1);
		targetPos = TempMalloc(len + 3);
		strcpy(targetPos, token);
		targetPos += len;

		*(unsigned short *)targetPos = index;
		targetPos += 2;

		*targetPos = type;
		*targetPos++;

		*targetPos = 0;
	}

	Com_EndParseSession();
	Hunk_ClearTempMemoryHighInternal();
}

/*
==============
GetObjectA
==============
*/
unsigned int GetObjectA( unsigned int id )
{
	VariableValueInternal *entryValue;

	assert(id);
	entryValue = &scrVarGlob.variableList[id];
	assert((entryValue->w.status & VAR_STAT_MASK) != VAR_STAT_FREE);
	assert((entryValue->w.type & VAR_MASK) == VAR_UNDEFINED || (entryValue->w.type & VAR_MASK) == VAR_POINTER);

	if ( (entryValue->w.type & VAR_MASK) == VAR_UNDEFINED )
	{
		entryValue->w.type |= VAR_POINTER;
		entryValue->u.u.pointerValue = AllocObject();
	}

	assert((entryValue->w.type & VAR_MASK) == VAR_POINTER);
	return entryValue->u.u.pointerValue;
}

/*
==============
Scr_ShutdownVariables
==============
*/
void Scr_ShutdownVariables()
{
	if ( !scrVarPub.tempVariable )
	{
		return;
	}

	FreeValue(scrVarPub.tempVariable);
	scrVarPub.tempVariable = 0;
}

/*
==============
Scr_InitClassMap
==============
*/
void Scr_InitClassMap()
{
	for ( int classnum = 0; classnum < CLASS_NUM_COUNT; classnum++ )
	{
		scrClassMap[classnum].entArrayId = 0;
		scrClassMap[classnum].id = 0;
	}
}

// FIXME: Will be removed later
union VariableValueInternal_u* GetVariableValueAddress_Bad(unsigned int id)
{
	return &scrVarGlob.variableList[id].u;
}
