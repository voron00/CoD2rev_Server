#include "../qcommon/qcommon.h"
#include "script_public.h"

scrVmGlob_t scrVmGlob;
scrVmPub_t scrVmPub;

int g_script_error_level;
jmp_buf g_script_error[MAX_VM_STACK_DEPTH + 1];

/*
==============
Scr_Allign2
==============
*/
const char* Scr_Allign2( char const *pos )
{
	UNIMPLEMENTED(__FUNCTION__);
	return NULL;
}

/*
==============
Scr_Allign4
==============
*/
const char* Scr_Allign4( char const *pos )
{
	UNIMPLEMENTED(__FUNCTION__);
	return NULL;
}

/*
==============
Scr_Allign4Strict
==============
*/
const char* Scr_Allign4Strict( char const *pos )
{
	UNIMPLEMENTED(__FUNCTION__);
	return NULL;
}

/*
==============
Scr_GetLocalVarAtIndex
==============
*/
unsigned int Scr_GetLocalVarAtIndex( int index )
{
	return scrVmPub.localVars[-index];
}

/*
==============
Scr_GetLocalVar
==============
*/
unsigned int Scr_GetLocalVar( const char *pos )
{
	return Scr_GetLocalVarAtIndex( *(unsigned char *)pos );
}

/*
==============
Scr_ReadShort
==============
*/
short Scr_ReadShort( const char **pos )
{
	short value = *(reinterpret_cast<const short *>(*pos));
	*pos += sizeof(short);

	return value;
}

/*
==============
Scr_ReadInt
==============
*/
int Scr_ReadInt( const char **pos )
{
	int value = *(reinterpret_cast<const int *>(*pos));
	*pos += sizeof(int);

	return value;
}

/*
==============
Scr_ReadVector
==============
*/
const float* Scr_ReadVector( const char **pos )
{
	const float *value = reinterpret_cast<const float *>(*pos);
	*pos += sizeof(vec3_t);

	return value;
}

/*
==============
Scr_ReadIntArray
==============
*/
const intptr_t* Scr_ReadIntArray( const char **pos, int count )
{
	const intptr_t *value = reinterpret_cast<const intptr_t *>(*pos);
	*pos += sizeof(const intptr_t) * count;

	return value;
}

/*
==============
Scr_ReadUnsignedShort
==============
*/
unsigned short Scr_ReadUnsignedShort( const char **pos )
{
	unsigned short value = *(reinterpret_cast<const unsigned short *>(*pos));
	*pos += sizeof(unsigned short);

	return value;
}

/*
==============
Scr_ReadUnsigned
==============
*/
uintptr_t Scr_ReadUnsigned( const char **pos )
{
	uintptr_t value = *(reinterpret_cast<const uintptr_t *>(*pos));
	*pos += sizeof(uintptr_t);

	return value;
}

/*
==============
Scr_ReadFloat
==============
*/
float Scr_ReadFloat( const char **pos )
{
	float value = *(reinterpret_cast<const float *>(*pos));
	*pos += sizeof(float);

	return value;
}

/*
==============
Scr_ReadCodePos
==============
*/
const char* Scr_ReadCodePos( const char **pos )
{
	const char *value = *(reinterpret_cast<const char**>(const_cast<char *>(*pos)));
	*pos += sizeof(const char *);

	return value;
}

/*
==============
Scr_ReadByte
==============
*/
unsigned char Scr_ReadByte( const char **pos )
{
	unsigned char value = *(reinterpret_cast<const unsigned char *>(*pos));
	*pos += sizeof(unsigned char);

	return value;
}

/*
==============
Scr_StackClear
==============
*/
void Scr_StackClear( void )
{
	scrVmPub.top = scrVmPub.stack;
}

/*
==============
Scr_DecTime
==============
*/
void Scr_DecTime()
{
	assert(!(scrVarPub.time & ~VAR_NAME_LOW_MASK));
	--scrVarPub.time;
	scrVarPub.time &= VAR_NAME_LOW_MASK;
}

/*
==============
Scr_GetNumParam
==============
*/
unsigned int Scr_GetNumParam()
{
	return scrVmPub.outparamcount;
}

/*
==============
Scr_IsSystemActive
==============
*/
int Scr_IsSystemActive()
{
	return scrVarPub.timeArrayId != 0;
}

/*
==============
Scr_GetReturnPos
==============
*/
char* Scr_GetReturnPos( unsigned int *localId )
{
	char *pos;

	if ( scrVmPub.function_count < 2 )
	{
		return 0;
	}

	pos = (char *)scrVmPub.function_frame[-1].fs.pos;

	if ( pos == &g_EndPos )
	{
		return 0;
	}

	*localId = scrVmPub.function_frame[-1].fs.localId;
	return pos;
}

/*
==============
Scr_GetNumScriptThreads
==============
*/
unsigned int Scr_GetNumScriptThreads()
{
	UNIMPLEMENTED(__FUNCTION__);
	return 0;
}

/*
==============
Scr_SetLoading
==============
*/
void Scr_SetLoading( int bLoading )
{
	scrVmGlob.loading = bLoading;
}

/*
==============
Scr_Abort
==============
*/
void Scr_Abort()
{
	scrVarPub.timeArrayId = 0;
	scrVarPub.bInited = false;
}

/*
==============
Scr_Settings
==============
*/
void Scr_Settings( int developer, int developer_script, int abort_on_error )
{
	assert(!abort_on_error || developer);

	scrVarPub.developer = developer != 0;
	scrVarPub.developer_script = developer_script != 0;

	scrVmPub.abort_on_error = abort_on_error != 0;
}

/*
==============
Scr_ClearErrorMessage
==============
*/
void Scr_ClearErrorMessage()
{
	scrVarPub.error_message = NULL;
	scrVmGlob.dialog_error_message = NULL;

	scrVarPub.error_index = 0;
}

/*
==============
Scr_ResetTimeout
==============
*/
void Scr_ResetTimeout()
{
	scrVmGlob.starttime = Sys_MilliSeconds();
}

/*
==============
Scr_ErrorWithDialogMessage
==============
*/
void Scr_ErrorWithDialogMessage( const char *error, const char *dialog_error )
{
	scrVarPub.error_message = error;
	scrVmGlob.dialog_error_message = dialog_error;

	Scr_ErrorInternal();
}

/*
==============
Scr_Error
==============
*/
void Scr_Error( const char *error )
{
	if ( scrVarPub.error_message == NULL )
	{
		scrVarPub.error_message = error;
	}

	Scr_ErrorInternal();
}

/*
==============
Scr_GetPointerType
==============
*/
int Scr_GetPointerType( scr_param_t index )
{
	VariableValue *value;

	if ( index >= scrVmPub.outparamcount )
	{
		Scr_Error(va("parameter %d does not exist", index + 1));
		return 0;
	}

	value = &scrVmPub.top[-index];

	if ( value->type != VAR_POINTER )
	{
		Scr_Error(va("type %s is not a pointer", var_typename[value->type]));
		return 0;
	}

	return GetObjectType(value->u.pointerValue);
}

/*
==============
Scr_GetTypeName
==============
*/
const char* Scr_GetTypeName( scr_param_t index )
{
	VariableValue *value;

	if ( index >= scrVmPub.outparamcount )
	{
		Scr_Error(va("parameter %d does not exist", index + 1));
		return NULL;
	}

	value = &scrVmPub.top[-index];
	return var_typename[value->type];
}

/*
==============
Scr_GetType
==============
*/
int Scr_GetType( scr_param_t index )
{
	VariableValue *value;

	if ( index >= scrVmPub.outparamcount )
	{
		Scr_Error(va("parameter %d does not exist", index + 1));
		return 0;
	}

	value = &scrVmPub.top[-index];
	return value->type;
}

/*
==============
Scr_GetObject
==============
*/
unsigned int Scr_GetObject( scr_param_t index )
{
	VariableValue *value;

	if ( index >= scrVmPub.outparamcount )
	{
		Scr_Error(va("parameter %d does not exist", index + 1));
		return 0;
	}

	value = &scrVmPub.top[-index];

	if ( value->type != VAR_POINTER )
	{
		scrVarPub.error_index = index + 1;
		Scr_Error(va("type %s is not an object", var_typename[value->type]));
		return 0;
	}

	return value->u.pointerValue;
}

/*
==============
Scr_GetEntityRef
==============
*/
scr_entref_t Scr_GetEntityRef( scr_param_t index )
{
	VariableValue *value;
	unsigned int id;
	scr_entref_t entref;

	// init empty variable
	entref.entnum = 0;
	entref.classnum = 0;

	if ( index >= scrVmPub.outparamcount )
	{
		Scr_Error(va("parameter %d does not exist", index + 1));
		return entref;
	}

	value = &scrVmPub.top[-index];

	if ( value->type != VAR_POINTER )
	{
		scrVarPub.error_index = index + 1;
		Scr_Error(va("type %s is not an entity", var_typename[value->type]));
		return entref;
	}

	id = value->u.pointerValue;

	if ( GetObjectType(value->u.pointerValue) != VAR_ENTITY )
	{
		scrVarPub.error_index = index + 1;
		Scr_Error(va("type %s is not an entity", var_typename[GetObjectType(id)]));
		return entref;
	}

	return Scr_GetEntityIdRef(id);
}

/*
==============
Scr_GetFunc
==============
*/
unsigned int Scr_GetFunc( scr_param_t index )
{
	VariableValue *value;

	if ( index >= scrVmPub.outparamcount )
	{
		Scr_Error(va("parameter %d does not exist", index + 1));
		return 0;
	}

	value = &scrVmPub.top[-index];

	if ( value->type != VAR_FUNCTION )
	{
		scrVarPub.error_index = index + 1;
		Scr_Error(va("type %s is not a function", var_typename[value->type]));
		return 0;
	}

	return value->u.codePosValue - scrVarPub.programBuffer;
}

/*
==============
Scr_GetVector
==============
*/
void Scr_GetVector( scr_param_t index, vec3_t vectorValue )
{
	VariableValue *value;

	if ( index >= scrVmPub.outparamcount )
	{
		Scr_Error(va("parameter %d does not exist", index + 1));
		return;
	}

	value = &scrVmPub.top[-index];

	if ( value->type != VAR_VECTOR )
	{
		scrVarPub.error_index = index + 1;
		Scr_Error(va("type %s is not a vector", var_typename[value->type]));
		return;
	}

	VectorCopy(value->u.vectorValue, vectorValue);
}

/*
==============
Scr_GetConstIString
==============
*/
unsigned int Scr_GetConstIString( scr_param_t index )
{
	VariableValue *value;

	if ( index >= scrVmPub.outparamcount )
	{
		Scr_Error(va("parameter %d does not exist", index + 1));
		return 0;
	}

	value = &scrVmPub.top[-index];

	if ( value->type != VAR_ISTRING )
	{
		scrVarPub.error_index = index + 1;
		Scr_Error(va("type %s is not a localized string", var_typename[value->type]));
		return 0;
	}

	return value->u.stringValue;
}

/*
==============
Scr_GetFloat
==============
*/
float Scr_GetFloat( scr_param_t index )
{
	VariableValue *value;

	if ( index >= scrVmPub.outparamcount )
	{
		Scr_Error(va("parameter %d does not exist", index + 1));
		return 0;
	}

	value = &scrVmPub.top[-index];

	// cast to float
	if ( value->type == VAR_INTEGER )
	{
		return (float)value->u.intValue;
	}

	if ( value->type != VAR_FLOAT )
	{
		scrVarPub.error_index = index + 1;
		Scr_Error(va("type %s is not a float", var_typename[value->type]));
		return 0;
	}

	return value->u.floatValue;
}

/*
==============
Scr_GetInt
==============
*/
int Scr_GetInt( scr_param_t index )
{
	VariableValue *value;

	if ( index >= scrVmPub.outparamcount )
	{
		Scr_Error(va("parameter %d does not exist", index + 1));
		return 0;
	}

	value = &scrVmPub.top[-index];

	if ( value->type != VAR_INTEGER )
	{
		scrVarPub.error_index = index + 1;
		Scr_Error(va("type %s is not an int", var_typename[value->type]));
	}

	return value->u.intValue;
}

/*
==============
Scr_GetInt64
==============
*/
scr_int64_t Scr_GetInt64( scr_param_t index )
{
	VariableValue *value;

	if ( index >= scrVmPub.outparamcount )
	{
		Scr_Error(va("parameter %d does not exist", index + 1));
		return 0;
	}

	value = &scrVmPub.top[-index];

	if ( value->type != VAR_INT64 )
	{
		scrVarPub.error_index = index + 1;
		Scr_Error(va("type %s is not an int64", var_typename[value->type]));
	}

	return value->u.int64Value;
}

/*
==============
Scr_ObjectError
==============
*/
void Scr_ObjectError( const char *error )
{
	scrVarPub.error_index = -1;
	Scr_Error(error);
}

/*
==============
Scr_ParamError
==============
*/
void Scr_ParamError( unsigned int index, const char *error )
{
	scrVarPub.error_index = index + 1;
	Scr_Error(error);
}

/*
==============
Scr_GetIString
==============
*/
const char* Scr_GetIString( unsigned int index )
{
	return SL_ConvertToString( Scr_GetConstIString( index ) );
}

/*
==============
Scr_FindAllThreads
==============
*/
int Scr_FindAllThreads( unsigned int selfId, unsigned int *threads, unsigned int localId )
{
	unsigned int timeId, notifyListId, stackId, threadId;
	int count = 0;

	if ( localId && selfId == Scr_GetSelf(localId) )
	{
		if ( threads )
		{
			*threads = localId;
		}

		count = 1;
	}

	for ( threadId = FindNextSibling(scrVarPub.timeArrayId); threadId; threadId = FindNextSibling(threadId) )
	{
		count = Scr_FindAllThreadsInternal(selfId, threadId, count, true, threads);
	}

	notifyListId = FindVariable(selfId, OBJECT_NOTIFY_LIST);

	if ( notifyListId )
	{
		timeId = FindObject(notifyListId);
		assert(timeId);

		for ( stackId = FindNextSibling(timeId); stackId; stackId = FindNextSibling(stackId) )
		{
			count = Scr_FindAllThreadsInternal(selfId, stackId, count, false, threads);
		}
	}

	return count;
}

/*
==============
Scr_TraverseScript
==============
*/
void Scr_TraverseScript( char const *pos )
{
	UNIMPLEMENTED(__FUNCTION__);
}

/*
==============
Scr_IsEndonThread
==============
*/
bool Scr_IsEndonThread( unsigned int localId )
{
	unsigned int stackId;
	int type;

	if ( GetObjectType(localId) != VAR_NOTIFY_THREAD )
	{
		return false;
	}

	if ( GetStartLocalId(localId) != localId )
	{
		return false;
	}

	stackId = Scr_GetWaittillThreadStackId(localId, localId);
	type = GetObjectType(stackId);

	assert((type == VAR_UNDEFINED) || (type == VAR_STACK));
	return type == VAR_UNDEFINED;
}

/*
==============
Scr_TerminalError
==============
*/
void Scr_TerminalError( const char *error )
{
	Scr_DumpScriptThreads();
	Scr_DumpScriptVariablesDefault();

	scrVmPub.terminal_error = true;
	Scr_Error(error);
}

/*
==============
Scr_InitSystem
==============
*/
void Scr_InitSystem()
{
	assert(!scrVarPub.timeArrayId);
	scrVarPub.timeArrayId = AllocObject();

	assert(!scrVarPub.pauseArrayId);
	scrVarPub.pauseArrayId = Scr_AllocArray();

	assert(!scrVarPub.levelId);
	scrVarPub.levelId = AllocObject();

	assert(!scrVarPub.animId);
	scrVarPub.animId = AllocObject();

	scrVarPub.time = 0;
	g_script_error_level = -1;
}

/*
==============
Scr_AddArrayStringIndexed
==============
*/
void Scr_AddArrayStringIndexed( unsigned int stringValue )
{
	unsigned int id;

	assert(scrVmPub.inparamcount);

	--scrVmPub.top;
	--scrVmPub.inparamcount;

	assert(scrVmPub.top->type == VAR_POINTER);

	id = GetNewVariable(scrVmPub.top->u.pointerValue, stringValue);
	SetNewVariableValue(id, scrVmPub.top + 1);
}

/*
==============
Scr_AddArray
==============
*/
void Scr_AddArray()
{
	unsigned int id;

	assert(scrVmPub.inparamcount);

	--scrVmPub.top;
	--scrVmPub.inparamcount;

	assert(scrVmPub.top->type == VAR_POINTER);

	id = GetNewArrayVariable(scrVmPub.top->u.pointerValue, GetArraySize(scrVmPub.top->u.pointerValue));
	SetNewVariableValue(id, scrVmPub.top + 1);
}

/*
==============
Scr_GetAnimTree
==============
*/
scr_animtree_t Scr_GetAnimTree( scr_param_t index ) // untested
{
	VariableValue *value;
	scr_animtree_t tree;

	if ( index >= scrVmPub.outparamcount )
	{
		Scr_Error(va("parameter %d does not exist", index + 1));

		tree.anims = NULL;
		return tree;
	}

	value = &scrVmPub.top[-index];

	if ( value->type != VAR_INTEGER )
	{
		scrVarPub.error_message = va("type %s is not an animtree", var_typename[value->type]);

		RemoveRefToValue(value);
		value->type = VAR_UNDEFINED;

		scrVarPub.error_index = index + 1;
		Scr_ErrorInternal();

		tree.anims = NULL;
		return tree;
	}

	if ( value->u.intValue > scrAnimPub.xanim_num[SCR_XANIM_SERVER] || !Scr_GetAnims(value->u.intValue) )
	{
		scrVarPub.error_message = "bad anim tree";

		RemoveRefToValue(value);
		value->type = VAR_UNDEFINED;

		scrVarPub.error_index = index + 1;
		Scr_ErrorInternal();

		tree.anims = NULL;
		return tree;
	}

	tree.anims = Scr_GetAnims(value->u.intValue);
	return tree;
}

/*
==============
Scr_GetAnim
==============
*/
scr_anim_s Scr_GetAnim( scr_param_t index, XAnimTree_s *tree )
{
	VariableValue *value;
	scr_anim_s anim;

	if ( index >= scrVmPub.outparamcount )
	{
		Scr_Error(va("parameter %d does not exist", index + 1));

		anim.index = 0;
		anim.tree = 0;

		return anim;
	}

	value = &scrVmPub.top[-index];

	if ( value->type != VAR_ANIMATION )
	{
		scrVarPub.error_message = va("type %s is not an anim", var_typename[value->type]);

		RemoveRefToValue(value);
		value->type = VAR_UNDEFINED;

		scrVarPub.error_index = index + 1;
		Scr_ErrorInternal();

		anim.index = 0;
		anim.tree = 0;

		return anim;
	}

	anim.linkPointer = value->u.codePosValue;

	if ( !tree )
	{
		return anim;
	}

	if ( Scr_GetAnims(anim.tree) != XAnimGetAnims(tree) )
	{
		scrVarPub.error_message = va("anim '%s' in animtree '%s' does not belong to the entity's animtree '%s'",
		                             XAnimGetAnimDebugName(Scr_GetAnims(anim.tree), anim.index),
		                             XAnimGetAnimTreeDebugName(Scr_GetAnims(anim.tree)),
		                             XAnimGetAnimTreeDebugName(XAnimGetAnims(tree)));

		RemoveRefToValue(value);
		value->type = VAR_UNDEFINED;

		scrVarPub.error_index = index + 1;
		Scr_ErrorInternal();
	}

	return anim;
}

/*
==============
Scr_ClearOutParams
==============
*/
void Scr_ClearOutParams()
{
	while ( scrVmPub.outparamcount )
	{
		RemoveRefToValue(scrVmPub.top);

		--scrVmPub.top;
		--scrVmPub.outparamcount;
	}
}

/*
==============
Scr_GetConstLowercaseString
==============
*/
unsigned int Scr_GetConstLowercaseString( scr_param_t index )
{
	unsigned int stringValue;
	VariableValue *value;
	const char *string;
	int i;
	char tempString[8192];

	if ( index >= scrVmPub.outparamcount )
	{
		Scr_Error(va("parameter %d does not exist", index + 1));
		return 0;
	}

	value = &scrVmPub.top[-index];

	if ( !Scr_CastString(value) )
	{
		scrVarPub.error_index = index + 1;
		Scr_ErrorInternal();
		return 0;
	}

	assert(value->type == VAR_STRING);
	stringValue = value->u.stringValue;
	string = SL_ConvertToString(stringValue);

	for ( i = 0; ; i++ )
	{
		tempString[i] = tolower(string[i]);

		if ( !string[i] )
		{
			break;
		}
	}

	assert(value->type == VAR_STRING);
	value->u.stringValue = SL_GetString(tempString, 0);
	SL_RemoveRefToString(stringValue);

	return value->u.stringValue;
}

/*
==============
Scr_GetConstString
==============
*/
unsigned int Scr_GetConstString( scr_param_t index )
{
	VariableValue *value;

	if ( index >= scrVmPub.outparamcount )
	{
		Scr_Error(va("parameter %d does not exist", index + 1));
		return 0;
	}

	value = &scrVmPub.top[-index];

	if ( !Scr_CastString(value) )
	{
		scrVarPub.error_index = index + 1;
		Scr_ErrorInternal();
		return 0;
	}

	assert(value->type == VAR_STRING);
	return value->u.stringValue;
}

/*
==============
Scr_GetNextCodepos
==============
*/
const char* Scr_GetNextCodepos( VariableValue *top, const char *pos, int opcode, int mode, unsigned int *localId )
{
	UNIMPLEMENTED(__FUNCTION__);
	return NULL;
}

/*
==============
Scr_Shutdown
==============
*/
void Scr_Shutdown()
{
	if ( !scrVarPub.bInited )
	{
		return;
	}

	scrVarPub.bInited = false;

	Scr_ShutdownVariables();
	VM_Shutdown();
	SL_Shutdown();
}

/*
==============
Scr_GetDebugString
==============
*/
const char* Scr_GetDebugString( scr_param_t index )
{
	VariableValue *value;

	if ( index >= scrVmPub.outparamcount )
	{
		Scr_Error(va("parameter %d does not exist", index + 1));
		return 0;
	}

	value = &scrVmPub.top[-index];
	Scr_CastDebugString(value);

	assert(value->type == VAR_STRING);
	return SL_ConvertToString(value->u.stringValue);
}

/*
==============
Scr_GetStringIncludeNull
==============
*/
const char* Scr_GetStringIncludeNull( scr_param_t index )
{
	return SL_ConvertToString( Scr_GetConstString( index ) );
}

/*
==============
Scr_GetConstStringIncludeNull
==============
*/
unsigned int Scr_GetConstStringIncludeNull( scr_param_t index )
{
	if ( index < scrVmPub.outparamcount && scrVmPub.top[-index].type == VAR_UNDEFINED )
	{
		return 0;
	}

	return Scr_GetConstString(index);
}

/*
==============
Scr_GetString
==============
*/
const char* Scr_GetString( unsigned int index )
{
	return SL_ConvertToString( Scr_GetConstString( index ) );
}

/*
==============
Scr_FreeThread
==============
*/
void Scr_FreeThread( unsigned short handle )
{
	assert(scrVarPub.timeArrayId);
	assert(handle);

	RemoveRefToObject(handle);
}

/*
==============
VM_CancelNotify
==============
*/
void VM_CancelNotify( unsigned int notifyListOwnerId, unsigned int startLocalId )
{
	unsigned int notifyNameListId, notifyListId, stringValue;

	notifyListId = FindObject( FindVariable( notifyListOwnerId, OBJECT_NOTIFY_LIST ) );
	stringValue = Scr_GetThreadNotifyName(startLocalId);
	assert(stringValue);
	notifyNameListId = FindObject( FindVariable( notifyListId, stringValue ) );

	VM_CancelNotifyInternal(notifyListOwnerId, startLocalId, notifyListId, notifyNameListId, stringValue);
}

/*
==============
SetEntityFieldValue
==============
*/
bool SetEntityFieldValue( unsigned int classnum, int entnum, int offset, VariableValue *value )
{
	assert(!scrVmPub.inparamcount);
	assert(!scrVmPub.outparamcount);

	scrVmPub.outparamcount = 1;
	scrVmPub.top = value;

	if ( !Scr_SetObjectField(classnum, entnum, offset) )
	{
		assert(!scrVmPub.inparamcount);
		assert(scrVmPub.outparamcount == 1);

		scrVmPub.outparamcount = 0;
		return false;
	}

	assert(!scrVmPub.inparamcount);

	if ( scrVmPub.outparamcount )
	{
		assert(scrVmPub.outparamcount == 1);

		RemoveRefToValue(scrVmPub.top);
		--scrVmPub.top;

		scrVmPub.outparamcount = 0;
	}

	return true;
}

/*
==============
Scr_CancelNotifyList
==============
*/
void Scr_CancelNotifyList( unsigned int notifyListOwnerId )
{
	unsigned int selfStartLocalId, selfLocalId, startLocalId, notifyListId, notifyNameListId, stackId;
	VariableStackBuffer *stackValue;

	while ( 1 )
	{
		notifyListId = FindVariable(notifyListOwnerId, OBJECT_NOTIFY_LIST);

		if ( !notifyListId )
		{
			break;
		}

		notifyNameListId = FindNextSibling(FindObject(notifyListId));

		if ( !notifyNameListId )
		{
			break;
		}

		stackId = FindNextSibling(FindObject(notifyNameListId));

		if ( !stackId )
		{
			break;
		}

		startLocalId = GetVariableKeyObject(stackId);
		assert(startLocalId);

		if ( GetObjectType(stackId) == VAR_STACK )
		{
			stackValue = GetVariableValueAddress(stackId)->stackValue;
			Scr_CancelWaittill(startLocalId);

			VM_TrimStack(startLocalId, stackValue, false);
			continue;
		}

		AddRefToObject(startLocalId);
		Scr_CancelWaittill(startLocalId);

		selfLocalId = Scr_GetSelf(startLocalId);
		selfStartLocalId = GetStartLocalId(selfLocalId);

		stackId = FindVariable(selfStartLocalId, OBJECT_STACK);

		if ( stackId )
		{
			stackValue = GetVariableValueAddress(stackId)->stackValue;

			assert(!Scr_GetThreadNotifyName( selfStartLocalId ));
			assert(GetObjectType( stackId ) == VAR_STACK);
			assert(!stackValue->pos);

			VM_TrimStack(selfStartLocalId, stackValue, true);
		}

		Scr_KillEndonThread(startLocalId);
		RemoveRefToEmptyObject(startLocalId);
	}
}

/*
==============
Scr_SetStructField
==============
*/
void Scr_SetStructField( unsigned int structId, unsigned int index )
{
	unsigned int fieldValueId;

	assert(!scrVmPub.outparamcount);
	assert(scrVmPub.inparamcount == 1);

	fieldValueId = Scr_GetVariableField(structId, index);

	assert(scrVmPub.inparamcount == 1);
	scrVmPub.inparamcount = 0;

	SetVariableFieldValue(fieldValueId, scrVmPub.top);

	assert(!scrVmPub.inparamcount);
	assert(!scrVmPub.outparamcount);

	scrVmPub.top--;
}

/*
==============
Scr_ShutdownSystem
==============
*/
void Scr_ShutdownSystem( int bComplete )
{
	unsigned int id, parentId;

	Scr_CompileShutdown();
	Scr_FreeEntityList();

	if ( !scrVarPub.timeArrayId )
	{
		return;
	}

	Scr_FreeGameVariable(bComplete);

	for ( id = FindNextSibling(scrVarPub.timeArrayId); id; id = FindNextSibling(id) )
	{
		VM_TerminateTime(FindObject(id));
	}

	while ( 1 )
	{
		id = FindNextSibling(scrVarPub.pauseArrayId);

		if ( !id )
		{
			break;
		}

		id = FindNextSibling(FindObject(id));
		assert(id);

		parentId = GetVariableValueAddress(id)->pointerValue;

		AddRefToObject(parentId);
		Scr_CancelNotifyList(parentId);
		RemoveRefToObject(parentId);
	}

	assert(scrVarPub.levelId);
	ClearObject(scrVarPub.levelId);
	RemoveRefToEmptyObject(scrVarPub.levelId);
	scrVarPub.levelId = 0;

	assert(scrVarPub.animId);
	ClearObject(scrVarPub.animId);
	RemoveRefToEmptyObject(scrVarPub.animId);
	scrVarPub.animId = 0;

	assert(scrVarPub.timeArrayId);
	ClearObject(scrVarPub.timeArrayId);
	RemoveRefToEmptyObject(scrVarPub.timeArrayId);
	scrVarPub.timeArrayId = 0;

	assert(scrVarPub.pauseArrayId);
	RemoveRefToEmptyObject(scrVarPub.pauseArrayId);
	scrVarPub.pauseArrayId = 0;

	assert(!scrVarPub.freeEntList);
	Scr_FreeObjects();
}

/*
==============
Scr_SetDynamicEntityField
==============
*/
void Scr_SetDynamicEntityField( int entnum, int classnum, unsigned int index )
{
	unsigned int entId;

	entId = Scr_GetEntityId(entnum, classnum);
	assert(GetObjectType( entId ) == VAR_ENTITY);

	Scr_SetStructField(entId, index);
}

/*
==============
Scr_MakeArray
==============
*/
void Scr_MakeArray()
{
	IncInParam();

	scrVmPub.top->type = VAR_POINTER;
	scrVmPub.top->u.pointerValue = Scr_AllocArray();
}

/*
==============
Scr_AddVector
==============
*/
void Scr_AddVector( const vec3_t value )
{
	IncInParam();

	scrVmPub.top->type = VAR_VECTOR;
	scrVmPub.top->u.vectorValue = Scr_AllocVector(value);
}

/*
==============
Scr_AddConstString
==============
*/
void Scr_AddConstString( unsigned int value )
{
	assert(value);
	IncInParam();

	scrVmPub.top->type = VAR_STRING;
	scrVmPub.top->u.stringValue = value;

	SL_AddRefToString(value);
}

/*
==============
Scr_AddIString
==============
*/
void Scr_AddIString( const char *value )
{
	assert(value);
	IncInParam();

	scrVmPub.top->type = VAR_ISTRING;
	scrVmPub.top->u.stringValue = SL_GetString(value, 0);
}

/*
==============
Scr_AddString
==============
*/
void Scr_AddString( const char *value )
{
	assert(value);
	IncInParam();

	scrVmPub.top->type = VAR_STRING;
	scrVmPub.top->u.stringValue = SL_GetString(value, 0);
}

/*
==============
Scr_AddObject
==============
*/
void Scr_AddObject( unsigned int id )
{
	assert(id);
	assert(GetObjectType( id ) != VAR_THREAD);
	assert(GetObjectType( id ) != VAR_NOTIFY_THREAD);
	assert(GetObjectType( id ) != VAR_TIME_THREAD);
	assert(GetObjectType( id ) != VAR_CHILD_THREAD);
	assert(GetObjectType( id ) != VAR_DEAD_THREAD);

	IncInParam();

	scrVmPub.top->type = VAR_POINTER;
	scrVmPub.top->u.pointerValue = id;

	AddRefToObject(id);
}

/*
==============
Scr_AddUndefined
==============
*/
void Scr_AddUndefined()
{
	IncInParam();
	scrVmPub.top->type = VAR_UNDEFINED;
}

/*
==============
Scr_AddAnim
==============
*/
void Scr_AddAnim( scr_anim_s value )
{
	IncInParam();

	scrVmPub.top->type = VAR_ANIMATION;
	scrVmPub.top->u.codePosValue = value.linkPointer;
}

/*
==============
Scr_AddFloat
==============
*/
void Scr_AddFloat( float value )
{
	IncInParam();

	scrVmPub.top->type = VAR_FLOAT;
	scrVmPub.top->u.floatValue = value;
}

/*
==============
Scr_AddInt
==============
*/
void Scr_AddInt( int value )
{
	IncInParam();

	scrVmPub.top->type = VAR_INTEGER;
	scrVmPub.top->u.intValue = value;
}

/*
==============
Scr_AddInt64
==============
*/
void Scr_AddInt64( scr_int64_t value )
{
	IncInParam();

	scrVmPub.top->type = VAR_INT64;
	scrVmPub.top->u.int64Value = value;
}

/*
==============
Scr_AddBool
==============
*/
void Scr_AddBool( bool value )
{
	assert(value == false || value == true);
	IncInParam();

	scrVmPub.top->type = VAR_INTEGER;
	scrVmPub.top->u.intValue = value;
}

/*
==============
Scr_AddStruct
==============
*/
void Scr_AddStruct()
{
	unsigned int id = AllocObject();

	Scr_AddObject(id);
	RemoveRefToObject(id);
}

/*
==============
Scr_AddEntityNum
==============
*/
void Scr_AddEntityNum( int entnum, int classnum )
{
	Scr_AddObject( Scr_GetEntityId( entnum, classnum ) );
}

/*
==============
GetEntityFieldValue
==============
*/
VariableValue GetEntityFieldValue( unsigned int classnum, int entnum, int offset )
{
	VariableValue value;

	assert(!scrVmPub.inparamcount);
	assert(!scrVmPub.outparamcount);

	scrVmPub.top = scrVmGlob.eval_stack - 1;
	scrVmGlob.eval_stack->type = VAR_UNDEFINED;

	Scr_GetObjectField(classnum, entnum, offset);

	assert(!scrVmPub.inparamcount || scrVmPub.inparamcount == 1);
	assert(!scrVmPub.outparamcount);
	assert(scrVmPub.top - scrVmPub.inparamcount == scrVmGlob.eval_stack - 1);

	scrVmPub.inparamcount = 0;

	value.u = scrVmGlob.eval_stack->u;
	value.type = scrVmGlob.eval_stack->type;

	return value;
}

/*
==============
Scr_NotifyNum
==============
*/
void Scr_NotifyNum( int entnum, int classnum, unsigned int stringValue, scr_param_t paramcount )
{
	int type;
	VariableValue *startTop;
	unsigned int id;

	assert(scrVarPub.timeArrayId);
	assert(paramcount <= scrVmPub.inparamcount);

	Scr_ClearOutParams();

	startTop = &scrVmPub.top[-paramcount];
	paramcount = scrVmPub.inparamcount - paramcount;

	id = FindEntityId(entnum, classnum);

	if ( id )
	{
		type = startTop->type;

		startTop->type = VAR_PRECODEPOS;
		scrVmPub.inparamcount = 0;

		VM_Notify(id, stringValue, scrVmPub.top);

		startTop->type = type;
	}

	while ( scrVmPub.top != startTop )
	{
		RemoveRefToValue(scrVmPub.top);
		scrVmPub.top--;
	}

	assert(!scrVmPub.outparamcount);
	scrVmPub.inparamcount = paramcount;
}

/*
==============
Scr_Init
==============
*/
void Scr_Init()
{
	if ( scrVarPub.bInited )
	{
		return;
	}

	SL_Restart();

	Var_Init();
	Scr_VM_Init();

	scrCompilePub.script_loading = false;
	scrAnimPub.animtree_loading = false;

	scrCompilePub.scriptsPos = 0;
	scrCompilePub.loadedscripts = 0;

	scrAnimPub.animtrees = 0;

	scrCompilePub.builtinMeth = 0;
	scrCompilePub.builtinFunc = 0;

	scrVarPub.bInited = true;
}

/*
==============
Scr_ExecCode
==============
*/
void Scr_ExecCode( const char *pos, unsigned int localId )
{
	Scr_ResetTimeout();

	assert(scrVarPub.timeArrayId);
	assert(!scrVmPub.inparamcount);
	assert(!scrVmPub.outparamcount);
	assert(!scrVarPub.evaluate);
	assert(!scrVmPub.debugCode);

	scrVmPub.debugCode = true;

	if ( localId )
	{
		VM_Execute(localId, pos, 0);
	}
	else
	{
		AddRefToObject(scrVarPub.levelId);
		localId = AllocThread(scrVarPub.levelId);

		VM_Execute(localId, pos, 0);

		Scr_KillThread(localId);
		RemoveRefToObject(localId);
	}

	assert(scrVmPub.debugCode);

	scrVmPub.debugCode = false;

	assert(scrVmPub.inparamcount == 1);
	assert(!scrVmPub.outparamcount);

	if ( scrVmPub.function_count )
	{
		scrVmPub.function_count--;
		scrVmPub.function_frame--;
	}

	scrVmPub.top--;
	scrVmPub.inparamcount = 0;
}

/*
==============
Scr_AddExecEntThreadNum
==============
*/
void Scr_AddExecEntThreadNum( int entnum, int classnum, int handle, unsigned int paramcount )
{
	const char *pos;
	unsigned int localId, id, selfId;

	pos = &scrVarPub.programBuffer[handle];

	if ( !scrVmPub.function_count )
	{
		assert(scrVmPub.localVars == scrVmGlob.localVarsStack - 1);
		Scr_ResetTimeout();
	}

	assert(scrVarPub.timeArrayId);
	assert(handle);
	assert(Scr_IsInScriptMemory( pos ));

	selfId = Scr_GetEntityId(entnum, classnum);
	AddRefToObject(selfId);

	localId = AllocThread(selfId);
	id = VM_Execute(localId, pos, paramcount);

	RemoveRefToObject(id);

	scrVmPub.outparamcount++;
	scrVmPub.inparamcount--;

	assert(scrVmPub.localVars == scrVmGlob.localVarsStack - 1);
}

/*
==============
Scr_AddExecThread
==============
*/
void Scr_AddExecThread( int handle, unsigned int paramcount )
{
	const char *pos;
	unsigned int id, localId;

	pos = &scrVarPub.programBuffer[handle];

	if ( !scrVmPub.function_count )
	{
		assert(scrVmPub.localVars == scrVmGlob.localVarsStack - 1);
		Scr_ResetTimeout();
	}

	assert(scrVarPub.timeArrayId);
	assert(handle);
	assert(Scr_IsInScriptMemory( pos ));

	AddRefToObject(scrVarPub.levelId);

	localId = AllocThread(scrVarPub.levelId);
	id = VM_Execute(localId, pos, paramcount);

	RemoveRefToObject(id);

	scrVmPub.outparamcount++;
	scrVmPub.inparamcount--;

	assert(scrVmPub.localVars == scrVmGlob.localVarsStack - 1);
}

/*
==============
Scr_ExecEntThreadNum
==============
*/
unsigned short Scr_ExecEntThreadNum( int entnum, int classnum, int handle, unsigned int paramcount )
{
	const char *pos;
	unsigned int localId, id, selfId;

	pos = &scrVarPub.programBuffer[handle];

	if ( !scrVmPub.function_count )
	{
		assert(scrVmPub.localVars == scrVmGlob.localVarsStack - 1);
		Scr_ResetTimeout();
	}

	assert(scrVarPub.timeArrayId);
	assert(handle);
	assert(Scr_IsInScriptMemory( pos ));

	selfId = Scr_GetEntityId(entnum, classnum);
	AddRefToObject(selfId);

	localId = AllocThread(selfId);
	id = VM_Execute(localId, pos, paramcount);

	RemoveRefToValue(scrVmPub.top);
	scrVmPub.top->type = VAR_UNDEFINED;

	scrVmPub.top--;
	scrVmPub.inparamcount--;

	//assert(scrVmPub.localVars == scrVmGlob.localVarsStack - 1);

	return id;
}

/*
==============
Scr_ExecThread
==============
*/
unsigned short Scr_ExecThread( int handle, unsigned int paramcount )
{
	const char *pos;
	unsigned int localId, id;

	pos = &scrVarPub.programBuffer[handle];

	if ( !scrVmPub.function_count )
	{
		assert(scrVmPub.localVars == scrVmGlob.localVarsStack - 1);
		Scr_ResetTimeout();
	}

	assert(scrVarPub.timeArrayId);
	assert(handle);
	assert(Scr_IsInScriptMemory( pos ));

	AddRefToObject(scrVarPub.levelId);

	localId = AllocThread(scrVarPub.levelId);
	id = VM_Execute(localId, pos, paramcount);

	RemoveRefToValue(scrVmPub.top);
	scrVmPub.top->type = VAR_UNDEFINED;

	scrVmPub.top--;
	scrVmPub.inparamcount--;

	assert(scrVmPub.localVars == scrVmGlob.localVarsStack - 1);

	return id;
}

/*
==============
Scr_RunCurrentThreads
==============
*/
void Scr_RunCurrentThreads()
{
	assert(!scrVmPub.function_count);
	assert(!scrVarPub.error_message);
	assert(!scrVarPub.error_index);
	assert(!scrVmPub.outparamcount);
	assert(!scrVmPub.inparamcount);
	assert(scrVmPub.top == scrVmPub.stack);

	VM_SetTime();
}

/*
==============
Scr_IncTime
==============
*/
void Scr_IncTime()
{
	Scr_RunCurrentThreads();
	Scr_FreeEntityList();

	assert(!(scrVarPub.time & ~VAR_NAME_LOW_MASK));

	scrVarPub.time++;
	scrVarPub.time &= VAR_NAME_LOW_MASK;
}

/*
==============
Scr_ErrorInternal
==============
*/
void Scr_ErrorInternal()
{
	assert(scrVarPub.error_message);

	if ( !scrVarPub.evaluate && !scrCompilePub.script_loading )
	{
		if ( scrVarPub.developer && scrVmGlob.loading )
		{
			scrVmPub.terminal_error = true;
		}

		if ( scrVmPub.function_count || scrVmPub.debugCode )
		{
			assert(g_script_error_level >= 0 && g_script_error_level < MAX_VM_STACK_DEPTH + 1);
			longjmp(g_script_error[g_script_error_level], -1);
		}

		Com_Error(ERR_DROP, "%s", scrVarPub.error_message);
	}

	if ( scrVmPub.terminal_error )
	{
		Com_Error(ERR_DROP, "%s", scrVarPub.error_message);
	}
}

/*
==============
Scr_FindAllThreadsInternal
==============
*/
int Scr_FindAllThreadsInternal( unsigned int selfId, unsigned int threadId, int count, bool a4, unsigned int *threads )
{
	unsigned int id, stackId;

	id = FindObject(threadId);

	for ( stackId = FindNextSibling(id); stackId; stackId = FindNextSibling(stackId) )
	{
		if ( GetObjectType(stackId) != VAR_STACK )
		{
			continue;
		}

		for ( threadId = GetVariableValueAddress(stackId)->stackValue->localId; threadId; threadId = GetSafeParentLocalId(threadId) )
		{
			if ( a4 && selfId != Scr_GetSelf(threadId) )
			{
				continue;
			}

			if ( threads )
			{
				threads[count] = threadId;
			}

			count++;
			break;
		}
	}

	return count;
}

/*
==============
Scr_TerminateRunningThread
==============
*/
void Scr_TerminateRunningThread( unsigned int localId )
{
	int threadId, topThread, function_count;

	assert(scrVmPub.function_count);
	function_count = scrVmPub.function_count;
	topThread = scrVmPub.function_count;

	while ( 1 )
	{
		assert(function_count);
		threadId = scrVmPub.function_frame_start[function_count].fs.localId;

		if ( threadId == localId )
		{
			break;
		}

		function_count--;

		if ( !GetSafeParentLocalId(threadId) )
		{
			topThread = function_count;
		}
	}

	while ( topThread >= function_count )
	{
		scrVmPub.function_frame_start[topThread].fs.pos = &g_EndPos;
		topThread--;
	}
}

/*
==============
Scr_AddLocalVars
==============
*/
int Scr_AddLocalVars( unsigned int localId )
{
	unsigned int fieldIndex;
	int localVarCount = 0;

	for ( fieldIndex = FindPrevSibling(localId); fieldIndex; fieldIndex = FindPrevSibling(fieldIndex) )
	{
		*scrVmPub.localVars++;
		*scrVmPub.localVars = fieldIndex;

		localVarCount++;
	}

	return localVarCount;
}

/*
==============
Scr_GetWaittillThreadStackId
==============
*/
unsigned int Scr_GetWaittillThreadStackId( unsigned int localId, unsigned int startLocalId )
{
	unsigned short stringValue;
	unsigned int id, index, parentId, selfId;

	assert(scrVarPub.developer);
	stringValue = Scr_GetThreadNotifyName(startLocalId);

	if ( !stringValue )
	{
		return FindVariable(startLocalId, OBJECT_STACK);
	}

	selfId = Scr_GetSelf(startLocalId);

	id = FindObjectVariable(scrVarPub.pauseArrayId, selfId);
	parentId = FindObject(id);

	id = FindObjectVariable(parentId, startLocalId);

	index = FindVariable(GetVariableValueAddress(id)->pointerValue, OBJECT_NOTIFY_LIST);
	parentId = FindObject(index);

	index = FindVariable(parentId, stringValue);
	parentId = FindObject(index);

	return FindObjectVariable(parentId, startLocalId);
}

/*
==============
Scr_VM_Init
==============
*/
void Scr_VM_Init()
{
	scrVmPub.maxstack = &scrVmPub.stack[MAX_VM_OPERAND_STACK - 1];
	scrVmPub.top = scrVmPub.stack;

	scrVmPub.function_count = 0;
	scrVmPub.function_frame = scrVmPub.function_frame_start;

	scrVmPub.localVars = scrVmGlob.localVarsStack - 1;

	scrVarPub.evaluate = false;
	scrVmPub.debugCode = false;

	Scr_ClearErrorMessage();

	scrVmPub.terminal_error = false;

	scrVmPub.outparamcount = 0;
	scrVmPub.inparamcount = 0;

	scrVarPub.tempVariable = AllocValue();

	scrVarPub.timeArrayId = 0;
	scrVarPub.pauseArrayId = 0;

	scrVarPub.levelId = 0;
	scrVarPub.gameId = 0;
	scrVarPub.animId = 0;

	scrVarPub.freeEntList = 0;

	scrVmPub.stack->type = VAR_CODEPOS;
	scrVmGlob.loading = false;
}

/*
==============
VM_UnarchiveStack
==============
*/
void VM_UnarchiveStack( unsigned int startLocalId, function_stack_t *fs, VariableStackBuffer *stackValue )
{
	int function_count, size;
	unsigned int localId;
	VariableValue *top;
	char *buf;

	assert(!scrVmPub.function_count);
	assert(stackValue->pos);
	assert(fs->startTop == &scrVmPub.stack[0]);

	scrVmPub.function_frame->fs.pos = stackValue->pos;

	scrVmPub.function_count++;
	scrVmPub.function_frame++;

	size = stackValue->size;
	buf = stackValue->buf;

	top = fs->startTop;

	while ( size )
	{
		top++;
		size--;

		top->type = *(unsigned char *)buf;
		buf += sizeof(unsigned char);

		if ( top->type == VAR_CODEPOS )
		{
			assert(scrVmPub.function_count < MAX_VM_STACK_DEPTH);
			scrVmPub.function_frame->fs.pos = *(const char **)buf;

			scrVmPub.function_count++;
			scrVmPub.function_frame++;
		}
		else
		{
			top->u.codePosValue = *(const char **)buf;
		}

		buf += sizeof(VariableUnion);
	}

	fs->pos = stackValue->pos;
	fs->top = top;

	localId = stackValue->localId;
	fs->localId = localId;
	Scr_ClearWaitTime(startLocalId);

	assert(scrVmPub.function_count < MAX_VM_STACK_DEPTH);
	function_count = scrVmPub.function_count;

	while ( 1 )
	{
		scrVmPub.function_frame_start[function_count].fs.localId = localId;
		function_count--;

		if ( !function_count )
		{
			break;
		}

		localId = GetParentLocalId(localId);
	}

	while ( ++function_count != scrVmPub.function_count )
	{
		scrVmPub.function_frame_start[function_count].fs.localVarCount = Scr_AddLocalVars(scrVmPub.function_frame_start[function_count].fs.localId);
	}

	fs->localVarCount = Scr_AddLocalVars(fs->localId);

	if ( stackValue->time != (unsigned char)scrVarPub.time )
	{
		Scr_ResetTimeout();
	}

	MT_Free(stackValue, stackValue->bufLen);
	assert(scrVmPub.stack[0].type == VAR_CODEPOS);
}

/*
==============
VM_ArchiveStack
==============
*/
VariableStackBuffer* VM_ArchiveStack( function_stack_t *stack )
{
	unsigned int localId;
	int bufLen, size;
	char *buf;
	VariableValue *top;
	VariableStackBuffer *stackValue;

	top = stack->top;

	size = top - stack->startTop;
	assert(size == (unsigned short)size);

	bufLen = STACKBUF_BUFFER_SIZE * size + sizeof(*stackValue) - 1;
	assert(bufLen == (unsigned short)bufLen);

	stackValue = (VariableStackBuffer *)MT_Alloc(bufLen);

	localId = stack->localId;

	stackValue->localId = localId;
	stackValue->size = size;
	stackValue->bufLen = bufLen;
	stackValue->pos = stack->pos;
	stackValue->time = scrVarPub.time;

	scrVmPub.localVars -= stack->localVarCount;
	buf = &stackValue->buf[STACKBUF_BUFFER_SIZE * size];

	while ( size )
	{
		buf -= sizeof(VariableUnion);

		if ( top->type == VAR_CODEPOS )
		{
			scrVmPub.function_count--;
			scrVmPub.function_frame--;

			*(const char **)buf = scrVmPub.function_frame->fs.pos;
			scrVmPub.localVars -= scrVmPub.function_frame->fs.localVarCount;

			localId = GetParentLocalId(localId);
		}
		else
		{
			*(const char **)buf = top->u.codePosValue;
		}

		buf -= sizeof(unsigned char);
		assert(top->type >= 0 && top->type < (1 << 8));
		*(unsigned char *)buf = top->type;

		top--;
		size--;
	}

	scrVmPub.function_count--;
	scrVmPub.function_frame--;

	AddRefToObject(localId);
	stack->localId = localId;

	return stackValue;
}

/*
==============
VM_Shutdown
==============
*/
void VM_Shutdown()
{
	if ( !scrVarPub.gameId )
	{
		return;
	}

	FreeValue(scrVarPub.gameId);
	scrVarPub.gameId = 0;
}

/*
==============
GetDummyFieldValue
==============
*/
unsigned int GetDummyFieldValue( void )
{
	ClearVariableValue(scrVarPub.tempVariable);
	return scrVarPub.tempVariable;
}

/*
==============
GetDummyObject
==============
*/
unsigned int GetDummyObject( void )
{
	ClearVariableValue(scrVarPub.tempVariable);
	return GetObjectA(scrVarPub.tempVariable);
}

/*
==============
VM_CancelNotifyInternal
==============
*/
void VM_CancelNotifyInternal( unsigned int notifyListOwnerId, unsigned int startLocalId, unsigned int notifyListId, unsigned int notifyNameListId, unsigned int stringValue )
{
	assert(stringValue == Scr_GetThreadNotifyName( startLocalId ));
	assert(notifyListId == FindObject( FindVariable( notifyListOwnerId, OBJECT_NOTIFY_LIST ) ));
	assert(notifyNameListId == FindObject( FindVariable( notifyListId, stringValue ) ));

	Scr_RemoveThreadNotifyName(startLocalId);
	RemoveObjectVariable(notifyNameListId, startLocalId);

	if ( GetArraySize(notifyNameListId) )
	{
		return;
	}

	RemoveVariable(notifyListId, stringValue);

	if ( GetArraySize(notifyListId) )
	{
		return;
	}

	RemoveVariable(notifyListOwnerId, OBJECT_NOTIFY_LIST);
}

/*
==============
Scr_CancelWaittill
==============
*/
void Scr_CancelWaittill( unsigned int startLocalId )
{
	unsigned int id, selfNameId, selfId;

	selfId = Scr_GetSelf(startLocalId);
	id = FindObjectVariable(scrVarPub.pauseArrayId, selfId);

	selfNameId = FindObject(id);
	id = FindObjectVariable(selfNameId, startLocalId);

	VM_CancelNotify(GetVariableValueAddress(id)->pointerValue, startLocalId);
	RemoveObjectVariable(selfNameId, startLocalId);

	if ( GetArraySize(selfNameId) )
	{
		return;
	}

	RemoveObjectVariable(scrVarPub.pauseArrayId, selfId);
}

/*
==============
VM_TrimStack
==============
*/
void VM_TrimStack( unsigned int startLocalId, VariableStackBuffer *stackValue, bool fromEndon )
{
	VariableValue tempValue, value;
	int size;
	char *buf;
	unsigned int parentLocalId, localId;

	assert(startLocalId);

	size = stackValue->size;
	localId = stackValue->localId;
	buf = &stackValue->buf[STACKBUF_BUFFER_SIZE * size];

	while ( size )
	{
		buf -= sizeof(VariableUnion);
		value.u.codePosValue = *(const char **)buf;

		buf -= sizeof(unsigned char);
		value.type = *(unsigned char *)buf;

		size--;

		if ( value.type != VAR_CODEPOS )
		{
			RemoveRefToValue(&value);
			continue;
		}

		if ( !FindObjectVariable(scrVarPub.pauseArrayId, localId) )
		{
			parentLocalId = GetParentLocalId(localId);
			Scr_KillThread(localId);

			RemoveRefToObject(localId);
			localId = parentLocalId;

			continue;
		}

		assert(startLocalId != localId);
		stackValue->localId = localId;
		stackValue->size = size + 1;

		Scr_StopThread(localId);

		if ( !fromEndon )
		{
			Scr_SetThreadNotifyName(startLocalId, 0);
			stackValue->pos = 0;

			tempValue.type = VAR_STACK;
			tempValue.u.stackValue = stackValue;

			SetNewVariableValue(GetNewVariable(startLocalId, OBJECT_STACK), &tempValue);
		}

		return;
	}

	assert(startLocalId == localId);

	if ( fromEndon )
	{
		RemoveVariable(startLocalId, OBJECT_STACK);
	}

	Scr_KillThread(startLocalId);
	RemoveRefToObject(startLocalId);

	MT_Free(stackValue, stackValue->bufLen);
}

/*
==============
VM_TerminateStack
==============
*/
void VM_TerminateStack( unsigned int endLocalId, unsigned int startLocalId, VariableStackBuffer *stackValue )
{
	VariableValue tempValue, value;
	int size;
	char *buf;
	unsigned int stackId, parentLocalId, localId;

	assert(startLocalId);

	size = stackValue->size;
	localId = stackValue->localId;
	buf = &stackValue->buf[STACKBUF_BUFFER_SIZE * size];

	while ( size )
	{
		buf -= sizeof(VariableUnion);
		value.u.codePosValue = *(const char **)buf;

		buf -= sizeof(unsigned char);
		value.type = *(unsigned char *)buf;

		size--;

		if ( value.type != VAR_CODEPOS )
		{
			RemoveRefToValue(&value);
			continue;
		}

		parentLocalId = GetParentLocalId(localId);

		Scr_KillThread(localId);
		RemoveRefToObject(localId);

		if ( localId != endLocalId )
		{
			localId = parentLocalId;
			continue;
		}

		assert(startLocalId != localId);
		*buf = 0;
		assert(stackValue->size >= size);

		Scr_SetThreadWaitTime(startLocalId, scrVarPub.time);

		assert(value.u.codePosValue);
		stackValue->pos = value.u.codePosValue;
		stackValue->localId = parentLocalId;
		stackValue->size = size + 1;

		tempValue.type = VAR_STACK;
		tempValue.u.stackValue = stackValue;

		stackId = GetNewObjectVariable(GetArray(GetVariable(scrVarPub.timeArrayId, scrVarPub.time)), startLocalId);

		SetNewVariableValue(stackId, &tempValue);
		return;
	}

	assert(localId == endLocalId);
	assert(startLocalId == localId);

	Scr_KillThread(localId);
	RemoveRefToObject(localId);

	MT_Free(stackValue, stackValue->bufLen);
}

/*
==============
VM_TerminateTime
==============
*/
void VM_TerminateTime( unsigned int timeId )
{
	VariableStackBuffer *stackValue;
	unsigned int stackId, startLocalId;

	assert(timeId);
	assert(!scrVmPub.function_count);

	AddRefToObject(timeId);

	while ( 1 )
	{
		stackId = FindNextSibling(timeId);

		if ( !stackId )
		{
			break;
		}

		startLocalId = GetVariableKeyObject(stackId);

		assert(startLocalId);
		assert(GetObjectType( stackId ) == VAR_STACK);

		stackValue = GetVariableValueAddress(stackId)->stackValue;

		RemoveObjectVariable(timeId, startLocalId);
		Scr_ClearWaitTime(startLocalId);

		VM_TerminateStack(startLocalId, startLocalId, stackValue);
	}

	RemoveRefToObject(timeId);
}

/*
==============
Scr_TerminateWaittillThread
==============
*/
void Scr_TerminateWaittillThread( unsigned int localId, unsigned int startLocalId )
{
	VariableStackBuffer *stackValue;
	unsigned int stackId, notifyListId, notifyNameListId, notifyListOwnerId, stringValue, selfNameId, selfId;

	stringValue = Scr_GetThreadNotifyName(startLocalId);

	if ( stringValue )
	{
		selfId = Scr_GetSelf(startLocalId);
		selfNameId = FindObject(FindObjectVariable(scrVarPub.pauseArrayId, selfId));

		notifyListOwnerId = GetVariableValueAddress(FindObjectVariable(selfNameId, startLocalId))->pointerValue;
		notifyListId = FindObject(FindVariable(notifyListOwnerId, OBJECT_NOTIFY_LIST));
		notifyNameListId = FindObject(FindVariable(notifyListId, stringValue));

		stackId = FindObjectVariable(notifyNameListId, startLocalId);

		assert(stackId);
		assert(GetObjectType( stackId ) == VAR_STACK);

		stackValue = GetVariableValueAddress(stackId)->stackValue;

		VM_CancelNotifyInternal(notifyListOwnerId, startLocalId, notifyListId, notifyNameListId, stringValue);
		RemoveObjectVariable(selfNameId, startLocalId);

		if ( !GetArraySize(selfNameId) )
		{
			RemoveObjectVariable(scrVarPub.pauseArrayId, selfId);
		}
	}
	else
	{
		stackId = FindVariable(startLocalId, OBJECT_STACK);

		assert(stackId);
		assert(GetObjectType( stackId ) == VAR_STACK);

		stackValue = GetVariableValueAddress(stackId)->stackValue;

		RemoveVariable(startLocalId, OBJECT_STACK);
	}

	VM_TerminateStack(localId, startLocalId, stackValue);
}

/*
==============
Scr_TerminateWaitThread
==============
*/
void Scr_TerminateWaitThread( unsigned int localId, unsigned int startLocalId )
{
	VariableStackBuffer *stackValue;
	unsigned int stackId, iTime, id;

	iTime = Scr_GetThreadWaitTime(startLocalId);
	Scr_ClearWaitTime(startLocalId);

	id = FindObject(FindVariable(scrVarPub.timeArrayId, iTime));
	stackId = FindObjectVariable(id, startLocalId);

	assert(stackId);
	assert(GetObjectType( stackId ) == VAR_STACK);

	stackValue = GetVariableValueAddress(stackId)->stackValue;

	RemoveObjectVariable(id, startLocalId);

	if ( !GetArraySize(id) && iTime != scrVarPub.time )
	{
		RemoveVariable(scrVarPub.timeArrayId, iTime);
	}

	VM_TerminateStack(localId, startLocalId, stackValue);
}

/*
==============
Scr_TerminateThread
==============
*/
void Scr_TerminateThread( unsigned int localId )
{
	unsigned int startLocalId = GetStartLocalId(localId);

	switch ( GetObjectType(startLocalId) )
	{
	case VAR_THREAD:
		Scr_TerminateRunningThread(localId);
		break;

	case VAR_NOTIFY_THREAD:
		Scr_TerminateWaittillThread(localId, startLocalId);
		break;

	case VAR_TIME_THREAD:
		Scr_TerminateWaitThread(localId, startLocalId);
		break;

	default:
		assert(0); // unreachable
		break;
	}
}

/*
==============
IncInParam
==============
*/
void IncInParam()
{
	assert(((scrVmPub.top >= scrVmGlob.eval_stack - 1) && (scrVmPub.top <= scrVmGlob.eval_stack)) || ((scrVmPub.top >= scrVmPub.stack) && (scrVmPub.top <= scrVmPub.maxstack)));

	Scr_ClearOutParams();

	if ( scrVmPub.top == scrVmPub.maxstack )
	{
		Com_Error(ERR_DROP, "Internal script stack overflow");
	}

	scrVmPub.top++;
	scrVmPub.inparamcount++;

	assert(((scrVmPub.top >= scrVmGlob.eval_stack) && (scrVmPub.top <= scrVmGlob.eval_stack + 1)) || ((scrVmPub.top >= scrVmPub.stack) && (scrVmPub.top <= scrVmPub.maxstack)));
}

/*
==============
VM_Notify
==============
*/
void VM_Notify( unsigned int notifyListOwnerId, unsigned int stringValue, VariableValue *top )
{
	unsigned int notifyListId, notifyNameListId, notifyListEntry, startLocalId, selfId, selfNameId, stackId;
	VariableUnion *tempValue;
	VariableStackBuffer *stackValue, *newStackValue;
	VariableValue *currentValue;
	VariableValue tempValue2, tempValue3, tempValue5;
	char *buf;
	int size, newSize, len, bufLen, type;
	bool bNoStack;

	notifyListId = FindVariable(notifyListOwnerId, OBJECT_NOTIFY_LIST);

	if ( !notifyListId )
	{
		return;
	}

	notifyListId = FindObject(notifyListId);
	assert(notifyListId);

	notifyNameListId = FindVariable(notifyListId, stringValue);

	if ( !notifyNameListId )
	{
		return;
	}

	notifyNameListId = FindObject(notifyNameListId);
	assert(notifyNameListId);

	AddRefToObject(notifyNameListId);

	assert(!scrVarPub.evaluate);
	scrVarPub.evaluate = true;

	notifyListEntry = notifyNameListId;

next:
	while ( 1 )
	{
		notifyListEntry = FindPrevSibling(notifyListEntry);

		if ( !notifyListEntry )
		{
			break;
		}

		startLocalId = GetVariableKeyObject(notifyListEntry);
		selfId = Scr_GetSelf(startLocalId);
		selfNameId = FindObject(FindObjectVariable(scrVarPub.pauseArrayId, selfId));

		if ( !GetObjectType(notifyListEntry) )
		{
			VM_CancelNotifyInternal(notifyListOwnerId, startLocalId, notifyListId, notifyNameListId, stringValue);
			Scr_KillEndonThread(startLocalId);

			RemoveObjectVariable(selfNameId, startLocalId);

			if ( !GetArraySize(selfNameId) )
			{
				RemoveObjectVariable(scrVarPub.pauseArrayId, selfId);
			}

			Scr_TerminateThread(selfId);

			notifyListEntry = notifyNameListId;
			continue;
		}

		assert(GetObjectType( notifyListEntry ) == VAR_STACK);

		tempValue = GetVariableValueAddress(notifyListEntry);
		stackValue = tempValue->stackValue;

		if ( *((unsigned char *)stackValue->pos - 1) == OP_waittillmatch )
		{
			size = *stackValue->pos;
			assert(size >= 0);
			assert(size <= stackValue->size);

			buf = &stackValue->buf[STACKBUF_BUFFER_SIZE * (stackValue->size - size)];

			for ( currentValue = top; size; currentValue-- )
			{
				assert(currentValue->type != VAR_CODEPOS);

				if ( currentValue->type == VAR_PRECODEPOS )
				{
					goto next;
				}

				size--;

				tempValue3.type = *(unsigned char *)buf;
				assert(tempValue3.type != VAR_CODEPOS);

				if ( tempValue3.type == VAR_PRECODEPOS )
				{
					break;
				}

				buf += sizeof(unsigned char);

				tempValue3.u.codePosValue = *(const char **)buf;
				buf += sizeof(VariableUnion);

				AddRefToValue(&tempValue3);

				type = currentValue->type;
				tempValue2.u = currentValue->u;
				tempValue2.type = type;

				AddRefToValue(&tempValue2);

				Scr_EvalEquality(&tempValue3, &tempValue2);

				if ( scrVarPub.error_message )
				{
					RuntimeError( stackValue->pos, *stackValue->pos - size + int( sizeof(VariableUnion) - sizeof(unsigned char) ), scrVarPub.error_message, scrVmGlob.dialog_error_message );
					Scr_ClearErrorMessage();

					goto next;
				}

				assert(tempValue3.type == VAR_INTEGER);

				if ( !tempValue3.u.intValue )
				{
					goto next;
				}
			}

			stackValue->pos++;
			bNoStack = true;
		}
		else
		{
			bNoStack = top->type == VAR_PRECODEPOS;
		}

		tempValue5.type = VAR_STACK;
		tempValue5.u.stackValue = stackValue;

		stackId = GetNewObjectVariable(GetArray(GetVariable(scrVarPub.timeArrayId, scrVarPub.time)), startLocalId);
		SetNewVariableValue(stackId, &tempValue5);

		tempValue = GetVariableValueAddress(stackId);

		VM_CancelNotifyInternal(notifyListOwnerId, startLocalId, notifyListId, notifyNameListId, stringValue);
		RemoveObjectVariable(selfNameId, startLocalId);

		if ( !GetArraySize(selfNameId) )
		{
			RemoveObjectVariable(scrVarPub.pauseArrayId, selfId);
		}

		Scr_SetThreadWaitTime(startLocalId, scrVarPub.time);

		if ( bNoStack )
		{
			notifyListEntry = notifyNameListId;
		}
		else
		{
			assert(top->type != VAR_PRECODEPOS);
			assert(top->type != VAR_CODEPOS);

			size = stackValue->size;
			newSize = size;
			currentValue = top;

			do
			{
				newSize++;
				currentValue--;
				assert(currentValue->type != VAR_CODEPOS);
			}
			while ( currentValue->type != VAR_PRECODEPOS );
			assert(newSize >= 0 && newSize < (1 << 16));

			len = STACKBUF_BUFFER_SIZE * size;
			bufLen = STACKBUF_BUFFER_SIZE * newSize + sizeof(*stackValue) - 1;

			if ( !MT_Realloc(stackValue->bufLen, bufLen) )
			{
				newStackValue = (VariableStackBuffer *)MT_Alloc(bufLen);

				newStackValue->bufLen = bufLen;
				newStackValue->pos = stackValue->pos;
				newStackValue->localId = stackValue->localId;

				memcpy(newStackValue->buf, stackValue->buf, len);
				MT_Free(stackValue, stackValue->bufLen);

				stackValue = newStackValue;
				tempValue->stackValue = newStackValue;
			}

			stackValue->size = newSize;
			buf = &stackValue->buf[len];

			newSize -= size;
			assert(newSize);

			do
			{
				currentValue++;
				AddRefToValue(currentValue);

				*buf = currentValue->type;
				buf += sizeof(unsigned char);

				*(const char **)buf = currentValue->u.codePosValue;
				buf += sizeof(VariableUnion);

				newSize--;
			}
			while ( newSize );

			assert(buf - (const char *)stackValue == bufLen);
			notifyListEntry = notifyNameListId;
		}
	}

	RemoveRefToObject(notifyNameListId);

	assert(scrVarPub.evaluate);
	scrVarPub.evaluate = false;
}

/*
==============
VM_SetTime
==============
*/
void VM_SetTime()
{
	unsigned int id;

	assert(!(scrVarPub.time & ~VAR_NAME_LOW_MASK));

	if ( !scrVarPub.timeArrayId )
	{
		return;
	}

	id = FindVariable(scrVarPub.timeArrayId, scrVarPub.time);

	if ( !id )
	{
		return;
	}

	VM_Resume(FindObject(id));
	SafeRemoveVariable(scrVarPub.timeArrayId, scrVarPub.time);
}

/*
==============
VM_Resume
==============
*/
void VM_Resume( unsigned int timeId )
{
	VariableStackBuffer *stackValue;
	unsigned int startLocalId, stackId;
	function_stack_t fs;

	assert(scrVmPub.top == scrVmPub.stack);
	Scr_ResetTimeout();

	assert(timeId);
	AddRefToObject(timeId);

	for ( fs.startTop = scrVmPub.stack; ; RemoveRefToValue(fs.startTop + 1) )
	{
		assert(!scrVarPub.error_index);
		assert(!scrVmPub.outparamcount);
		assert(!scrVmPub.inparamcount);
		assert(!scrVmPub.function_count);
		assert(scrVmPub.localVars == scrVmGlob.localVarsStack - 1);
		assert(fs.startTop == &scrVmPub.stack[0]);

		stackId = FindNextSibling(timeId);

		if ( !stackId )
		{
			break;
		}

		startLocalId = GetVariableKeyObject(stackId);

		assert(startLocalId);
		assert(GetObjectType( stackId ) == VAR_STACK);

		stackValue = GetVariableValueAddress(stackId)->stackValue;
		RemoveObjectVariable(timeId, startLocalId);

		VM_UnarchiveStack( startLocalId, &fs, stackValue );
		RemoveRefToObject( VM_Execute( fs ) );
	}

	RemoveRefToObject(timeId);
	ClearVariableValue(scrVarPub.tempVariable);

	scrVmPub.top = scrVmPub.stack;
}

/*
==============
VM_Execute
==============
*/
unsigned int VM_Execute( unsigned int localId, const char *pos, scr_param_t paramcount )
{
	function_stack_t fs;
	int type;

	assert(paramcount <= scrVmPub.inparamcount);
	Scr_ClearOutParams();

	fs.startTop = &scrVmPub.top[-paramcount];
	paramcount = scrVmPub.inparamcount - paramcount;

	// overflow
	if ( scrVmPub.function_count >= MAX_EMBEDDED_FUNCTION_CALLS )
	{
		Scr_KillThread(localId);
		scrVmPub.inparamcount = paramcount + 1;

		while ( paramcount )
		{
			RemoveRefToValue(scrVmPub.top);

			scrVmPub.top--;
			paramcount--;
		}

		scrVmPub.top++;
		scrVmPub.top->type = VAR_UNDEFINED;

		RuntimeError(pos, 0, "script stack overflow (too many embedded function calls)", NULL);

		return localId;
	}

	if ( scrVmPub.function_count )
	{
		scrVmPub.function_count++;
		scrVmPub.function_frame++;

		scrVmPub.function_frame->fs.localId = 0;
	}

	scrVmPub.function_frame->fs.pos = pos;
	fs.pos = scrVmPub.function_frame->fs.pos;

	scrVmPub.function_count++;
	scrVmPub.function_frame++;

	scrVmPub.function_frame->fs.localId = localId;
	fs.localId = scrVmPub.function_frame->fs.localId;

	type = fs.startTop->type;
	fs.startTop->type = VAR_PRECODEPOS;

	scrVmPub.inparamcount = 0;

	fs.top = scrVmPub.top;
	fs.localVarCount = 0;

	localId = VM_Execute( fs );

	fs.startTop->type = type;
	scrVmPub.top = fs.startTop + 1;

	scrVmPub.inparamcount = paramcount + 1;

	assert(!scrVmPub.outparamcount);
	ClearVariableValue(scrVarPub.tempVariable);

	if ( scrVmPub.function_count )
	{
		scrVmPub.function_count--;
		scrVmPub.function_frame--;
	}

	return localId;
}

/*
==============
VM_Execute
==============
*/
unsigned int VM_Execute( function_stack_t fs )
{
	int jumpOffset, entnum, gCaseCount, waitTime;
	unsigned int parentLocalId, builtinIndex, stringValue, id, threadId, classnum, removeCount;
	unsigned int outparamcount, selfId, objectId, fieldValueId, caseValue, currentCaseValue, stackId;
	VariableValue stackValue, tempValue;
	VariableValue *value;
	scr_entref_t entref;
	const char *currentCodePos, *tempCodePos;

	int gOpcode = 0;
	int gParamCount = 0;

	g_script_error_level++;
	assert(g_script_error_level >= 0 && g_script_error_level < MAX_VM_STACK_DEPTH + 1);

	if ( setjmp( g_script_error[ g_script_error_level ] ) )
	{
error:
		switch ( gOpcode )
		{
		case OP_EvalLocalArrayRefCached0:
		case OP_EvalLocalArrayRefCached:
		case OP_EvalArrayRef:
		case OP_ClearArray:
		case OP_EvalLocalVariableRef:
			assert(scrVarPub.error_index >= -1);
			if ( scrVarPub.error_index < 0 )
				scrVarPub.error_index = 1;
			break;

		case OP_EvalSelfFieldVariable:
		case OP_EvalFieldVariable:
		case OP_ClearFieldVariable:
		case OP_SetVariableField:
		case OP_SetSelfFieldVariableField:
		case OP_inc:
		case OP_dec:
			scrVarPub.error_index = 0;
			break;

		case OP_CallBuiltin0:
		case OP_CallBuiltin1:
		case OP_CallBuiltin2:
		case OP_CallBuiltin3:
		case OP_CallBuiltin4:
		case OP_CallBuiltin5:
		case OP_CallBuiltin:
			assert(scrVarPub.error_index >= 0);
			if ( scrVarPub.error_index > 0 )
				scrVarPub.error_index = scrVmPub.outparamcount - scrVarPub.error_index + 1;
			break;

		case OP_CallBuiltinMethod0:
		case OP_CallBuiltinMethod1:
		case OP_CallBuiltinMethod2:
		case OP_CallBuiltinMethod3:
		case OP_CallBuiltinMethod4:
		case OP_CallBuiltinMethod5:
		case OP_CallBuiltinMethod:
			assert(scrVarPub.error_index >= -1);
			if ( scrVarPub.error_index > 0 )
				scrVarPub.error_index = scrVmPub.outparamcount - scrVarPub.error_index + 2;
			else if ( scrVarPub.error_index < 0 )
				scrVarPub.error_index = 1;
			break;

		default:
			break;
		}

		RuntimeError(fs.pos, scrVarPub.error_index, scrVarPub.error_message, scrVmGlob.dialog_error_message);
		Scr_ClearErrorMessage();

		switch ( gOpcode )
		{
		case OP_EvalLocalArrayCached:
		case OP_EvalArray:
			RemoveRefToValue(fs.top);
			fs.top--;
			RemoveRefToValue(fs.top);
			fs.top->type = VAR_UNDEFINED;
			break;

		case OP_EvalLocalArrayRefCached0:
		case OP_EvalLocalArrayRefCached:
		case OP_EvalArrayRef:
		case OP_EvalLocalVariableRef:
			fieldValueId = GetDummyFieldValue();
			RemoveRefToValue(fs.top);
			fs.top--;
			break;

		case OP_ClearArray:
		case OP_wait:
			RemoveRefToValue(fs.top);
			fs.top--;
			break;

		case OP_GetSelfObject:
			objectId = GetDummyObject();
			break;

		case OP_EvalSelfFieldVariable:
		case OP_EvalFieldVariable:
			fs.top->type = VAR_UNDEFINED;
			break;

		case OP_EvalSelfFieldVariableRef:
		case OP_EvalFieldVariableRef:
			fieldValueId = GetDummyFieldValue();
			break;

		case OP_ClearFieldVariable:
			if ( scrVmPub.outparamcount )
			{
				assert(scrVmPub.outparamcount == 1);
				assert(scrVmPub.top->type == VAR_UNDEFINED);
				scrVmPub.outparamcount = 0;
			}
			break;

		case OP_checkclearparams:
			assert(fs.top->type != VAR_CODEPOS);
			while ( fs.top->type != VAR_PRECODEPOS )
			{
				RemoveRefToValue(fs.top);
				fs.top--;
				assert(fs.top->type != VAR_CODEPOS);
			}
			fs.top->type = VAR_CODEPOS;
			break;

		case OP_SetVariableField:
			if ( scrVmPub.outparamcount )
			{
				assert(scrVmPub.outparamcount == 1);
				assert(scrVmPub.top == fs.top);
				RemoveRefToValue(fs.top);
				scrVmPub.outparamcount = 0;
				fs.top--;
				break;
			}
			fs.top--;
			break;

		case OP_SetSelfFieldVariableField:
			RemoveRefToValue(fs.top);
			scrVmPub.outparamcount = 0;
			fs.top--;
			break;

		case OP_CallBuiltin0:
		case OP_CallBuiltin1:
		case OP_CallBuiltin2:
		case OP_CallBuiltin3:
		case OP_CallBuiltin4:
		case OP_CallBuiltin5:
		case OP_CallBuiltin:
		case OP_CallBuiltinMethod0:
		case OP_CallBuiltinMethod1:
		case OP_CallBuiltinMethod2:
		case OP_CallBuiltinMethod3:
		case OP_CallBuiltinMethod4:
		case OP_CallBuiltinMethod5:
		case OP_CallBuiltinMethod:
			Scr_ClearOutParams();
			fs.top = scrVmPub.top + 1;
			fs.top->type = VAR_UNDEFINED;
			break;

		case OP_ScriptFunctionCall2:
		case OP_ScriptFunctionCall:
		case OP_ScriptMethodCall:
			Scr_ReadCodePos(&fs.pos);
			while ( fs.top->type != VAR_PRECODEPOS )
			{
				RemoveRefToValue(fs.top);
				fs.top--;
			}
			fs.top->type = VAR_UNDEFINED;
			break;

		case OP_ScriptFunctionCallPointer:
		case OP_ScriptMethodCallPointer:
			assert(fs.top->type != VAR_CODEPOS);
			while ( fs.top->type != VAR_PRECODEPOS )
			{
				RemoveRefToValue(fs.top);
				fs.top--;
				assert(fs.top->type != VAR_CODEPOS);
			}
			fs.top->type = VAR_UNDEFINED;
			break;

		case OP_ScriptThreadCall:
		case OP_ScriptMethodThreadCall:
			Scr_ReadCodePos(&fs.pos);
			for ( outparamcount = Scr_ReadUnsigned(&fs.pos); outparamcount; outparamcount-- )
			{
				RemoveRefToValue(fs.top);
				fs.top--;
			}
			fs.top++;
			fs.top->type = VAR_UNDEFINED;
			break;

		case OP_ScriptThreadCallPointer:
		case OP_ScriptMethodThreadCallPointer:
			for ( outparamcount = Scr_ReadUnsigned(&fs.pos); outparamcount; outparamcount-- )
			{
				RemoveRefToValue(fs.top);
				fs.top--;
			}
			fs.top++;
			fs.top->type = VAR_UNDEFINED;
			break;

		case OP_CastFieldObject:
			objectId = GetDummyObject();
			fs.top--;
			break;

		case OP_EvalLocalVariableObjectCached:
			fs.pos++;
			objectId = GetDummyObject();
			break;

		case OP_JumpOnFalse:
		case OP_JumpOnTrue:
		case OP_JumpOnFalseExpr:
		case OP_JumpOnTrueExpr:
			Scr_ReadUnsignedShort(&fs.pos);
			fs.top--;
			break;

		case OP_jumpback:
			jumpOffset = Scr_ReadUnsignedShort(&fs.pos);
			fs.pos -= jumpOffset;
			break;

		case OP_bit_or:
		case OP_bit_ex_or:
		case OP_bit_and:
		case OP_equality:
		case OP_inequality:
		case OP_less:
		case OP_greater:
		case OP_less_equal:
		case OP_greater_equal:
		case OP_shift_left:
		case OP_shift_right:
		case OP_plus:
		case OP_minus:
		case OP_multiply:
		case OP_divide:
		case OP_mod:
			fs.top--;
			break;

		case OP_waittillmatch:
			fs.pos++;
			RemoveRefToValue(fs.top);
			fs.top--;
			RemoveRefToValue(fs.top);
			fs.top--;
			break;

		case OP_waittill:
		case OP_endon:
			RemoveRefToValue(fs.top);
			fs.top--;
			RemoveRefToValue(fs.top);
			fs.top--;
			break;

		case OP_notify:
			assert(fs.top->type != VAR_CODEPOS);
			while ( fs.top->type != VAR_PRECODEPOS )
			{
				RemoveRefToValue(fs.top);
				fs.top--;
				assert(fs.top->type != VAR_CODEPOS);
			}
			RemoveRefToValue(fs.top);
			fs.top--;
			break;

		case OP_switch:
			while ( gCaseCount )
			{
				currentCaseValue = Scr_ReadUnsigned(&fs.pos);
				currentCodePos = Scr_ReadCodePos(&fs.pos);
				gCaseCount--;
			}
			if ( !currentCaseValue )
			{
				fs.pos = currentCodePos;
				assert(fs.pos);
			}
			RemoveRefToValue(fs.top);
			fs.top--;
			break;
		}
	}

	assert(!scrVmPub.inparamcount);
	assert(!scrVmPub.outparamcount);

	while ( 1 )
	{
		assert(!scrVarPub.error_message);
		assert(!scrVarPub.error_index);
		assert(!scrVmPub.outparamcount);
		assert(!scrVmPub.inparamcount);

		gOpcode = Scr_ReadByte(&fs.pos);

		switch ( gOpcode )
		{
		case OP_End:
			parentLocalId = GetSafeParentLocalId(fs.localId);
			Scr_KillThread(fs.localId);

			scrVmPub.localVars -= fs.localVarCount;
			assert(fs.top->type != VAR_PRECODEPOS);

			while ( fs.top->type != VAR_CODEPOS )
			{
				RemoveRefToValue(fs.top);
				fs.top--;
				assert(fs.top->type != VAR_PRECODEPOS);
			}

			scrVmPub.function_count--;
			scrVmPub.function_frame--;

			if ( !parentLocalId )
			{
				assert(fs.top == fs.startTop);
thread_end:
				value = fs.startTop + 1;
				value->type = VAR_UNDEFINED;
				goto thread_return;
			}

			assert(fs.top->type == VAR_CODEPOS);
			fs.top->type = VAR_UNDEFINED;
			goto end;

		case OP_Return:
			parentLocalId = GetSafeParentLocalId(fs.localId);
			Scr_KillThread(fs.localId);

			scrVmPub.localVars -= fs.localVarCount;
			assert(fs.top->type != VAR_PRECODEPOS);

			tempValue.u = fs.top->u;
			tempValue.type = fs.top->type;

			fs.top--;
			assert(fs.top->type != VAR_PRECODEPOS);

			while ( fs.top->type != VAR_CODEPOS )
			{
				RemoveRefToValue(fs.top);
				fs.top--;
				assert(fs.top->type != VAR_PRECODEPOS);
			}

			scrVmPub.function_count--;
			scrVmPub.function_frame--;

			if ( !parentLocalId )
			{
				assert(fs.top == fs.startTop);
				value = fs.top + 1;

				value->u = tempValue.u;
				value->type = tempValue.type;
thread_return:
				if ( !gParamCount )
				{
					assert(g_script_error_level >= 0);
					g_script_error_level--;
					return fs.localId;
				}

				gParamCount--;
				RemoveRefToObject(fs.localId);

				fs.pos = scrVmPub.function_frame->fs.pos;
				assert(fs.pos);

				fs.localId = scrVmPub.function_frame->fs.localId;
				fs.localVarCount = scrVmPub.function_frame->fs.localVarCount;

				fs.top = scrVmPub.function_frame->fs.top;
				fs.startTop = scrVmPub.function_frame->fs.startTop;
				fs.top->type = scrVmPub.function_frame->topType;

				fs.top++;
				continue;
			}

			assert(fs.top->type == VAR_CODEPOS);

			fs.top->u = tempValue.u;
			fs.top->type = tempValue.type;
end:
			assert(fs.top != fs.startTop);
			RemoveRefToObject(fs.localId);

			fs.pos = scrVmPub.function_frame->fs.pos;
			assert(fs.pos);

			fs.localVarCount = scrVmPub.function_frame->fs.localVarCount;
			fs.localId = parentLocalId;
			continue;

		case OP_GetUndefined:
			assert(fs.top >= scrVmPub.stack);
			assert(fs.top+1 <= scrVmPub.maxstack);
			fs.top++;
			fs.top->type = VAR_UNDEFINED;
			continue;

		case OP_GetZero:
			assert(fs.top >= scrVmPub.stack);
			assert(fs.top+1 <= scrVmPub.maxstack);
			fs.top++;
			fs.top->type = VAR_INTEGER;
			fs.top->u.intValue = 0;
			continue;

		case OP_GetByte:
			assert(fs.top >= scrVmPub.stack);
			assert(fs.top+1 <= scrVmPub.maxstack);
			fs.top++;
			fs.top->type = VAR_INTEGER;
			fs.top->u.intValue = Scr_ReadByte(&fs.pos);
			continue;

		case OP_GetNegByte:
			assert(fs.top >= scrVmPub.stack);
			assert(fs.top+1 <= scrVmPub.maxstack);
			fs.top++;
			fs.top->type = VAR_INTEGER;
			fs.top->u.intValue = -Scr_ReadByte(&fs.pos);
			continue;

		case OP_GetUnsignedShort:
			assert(fs.top >= scrVmPub.stack);
			assert(fs.top+1 <= scrVmPub.maxstack);
			fs.top++;
			fs.top->type = VAR_INTEGER;
			fs.top->u.intValue = Scr_ReadUnsignedShort(&fs.pos);
			continue;

		case OP_GetNegUnsignedShort:
			assert(fs.top >= scrVmPub.stack);
			assert(fs.top+1 <= scrVmPub.maxstack);
			fs.top++;
			fs.top->type = VAR_INTEGER;
			fs.top->u.intValue = -Scr_ReadUnsignedShort(&fs.pos);
			continue;

		case OP_GetInteger:
			assert(fs.top >= scrVmPub.stack);
			assert(fs.top+1 <= scrVmPub.maxstack);
			fs.top++;
			fs.top->type = VAR_INTEGER;
			fs.top->u.intValue = Scr_ReadInt(&fs.pos);
			continue;

		case OP_GetFloat:
			assert(fs.top >= scrVmPub.stack);
			assert(fs.top+1 <= scrVmPub.maxstack);
			fs.top++;
			fs.top->type = VAR_FLOAT;
			fs.top->u.floatValue = Scr_ReadFloat(&fs.pos);
			continue;

		case OP_GetString:
			assert(fs.top >= scrVmPub.stack);
			assert(fs.top+1 <= scrVmPub.maxstack);
			fs.top++;
			fs.top->type = VAR_STRING;
			fs.top->u.stringValue = Scr_ReadUnsignedShort(&fs.pos);
			SL_AddRefToString(fs.top->u.stringValue);
			continue;

		case OP_GetIString:
			assert(fs.top >= scrVmPub.stack);
			assert(fs.top+1 <= scrVmPub.maxstack);
			fs.top++;
			fs.top->type = VAR_ISTRING;
			fs.top->u.stringValue = Scr_ReadUnsignedShort(&fs.pos);
			SL_AddRefToString(fs.top->u.stringValue);
			continue;

		case OP_GetVector:
			assert(fs.top >= scrVmPub.stack);
			assert(fs.top+1 <= scrVmPub.maxstack);
			fs.top++;
			fs.top->type = VAR_VECTOR;
			fs.top->u.vectorValue = Scr_ReadVector(&fs.pos);
			continue;

		case OP_GetLevelObject:
			objectId = scrVarPub.levelId;
			continue;

		case OP_GetAnimObject:
			objectId = scrVarPub.animId;
			continue;

		case OP_GetSelf:
			assert(fs.top >= scrVmPub.stack);
			assert(fs.top+1 <= scrVmPub.maxstack);
			fs.top++;
			fs.top->type = VAR_POINTER;
			fs.top->u.pointerValue = Scr_GetSelf(fs.localId);
			AddRefToObject(fs.top->u.pointerValue);
			continue;

		case OP_GetLevel:
			assert(fs.top >= scrVmPub.stack);
			assert(fs.top+1 <= scrVmPub.maxstack);
			fs.top++;
			fs.top->type = VAR_POINTER;
			fs.top->u.pointerValue = scrVarPub.levelId;
			AddRefToObject(scrVarPub.levelId);
			continue;

		case OP_GetGame:
			assert(fs.top >= scrVmPub.stack);
			assert(fs.top+1 <= scrVmPub.maxstack);
			fs.top++;
			*fs.top = Scr_EvalVariable(scrVarPub.gameId);
			continue;

		case OP_GetAnim:
			assert(fs.top >= scrVmPub.stack);
			assert(fs.top+1 <= scrVmPub.maxstack);
			fs.top++;
			fs.top->type = VAR_POINTER;
			fs.top->u.pointerValue = scrVarPub.animId;
			AddRefToObject(scrVarPub.animId);
			continue;

		case OP_GetAnimation:
			assert(fs.top >= scrVmPub.stack);
			assert(fs.top+1 <= scrVmPub.maxstack);
			fs.top++;
			fs.top->type = VAR_ANIMATION;
			fs.top->u.pointerValue = Scr_ReadUnsigned(&fs.pos);
			continue;

		case OP_GetGameRef:
			fieldValueId = scrVarPub.gameId;
			continue;

		case OP_GetFunction:
			assert(fs.top >= scrVmPub.stack);
			assert(fs.top+1 <= scrVmPub.maxstack);
			fs.top++;
			fs.top->type = VAR_FUNCTION;
			fs.top->u.codePosValue = Scr_ReadCodePos(&fs.pos);
			continue;

		case OP_CreateLocalVariable:
			scrVmPub.localVars++;
			fs.localVarCount++;
			*scrVmPub.localVars = GetNewVariable(fs.localId, Scr_ReadUnsignedShort(&fs.pos));
			continue;

		case OP_RemoveLocalVariables:
			removeCount = *fs.pos++;
			scrVmPub.localVars -= removeCount;
			fs.localVarCount -= removeCount;
			while ( removeCount )
			{
				RemoveNextVariable(fs.localId);
				removeCount--;
			}
			continue;

		case OP_EvalLocalVariableCached0:
			assert(fs.top >= scrVmPub.stack);
			assert(fs.top+1 <= scrVmPub.maxstack);
			fs.top++;
			*fs.top = Scr_EvalVariable(Scr_GetLocalVarAtIndex(0));
			continue;

		case OP_EvalLocalVariableCached1:
			assert(fs.top >= scrVmPub.stack);
			assert(fs.top+1 <= scrVmPub.maxstack);
			fs.top++;
			*fs.top = Scr_EvalVariable(Scr_GetLocalVarAtIndex(1));
			continue;

		case OP_EvalLocalVariableCached2:
			assert(fs.top >= scrVmPub.stack);
			assert(fs.top+1 <= scrVmPub.maxstack);
			fs.top++;
			*fs.top = Scr_EvalVariable(Scr_GetLocalVarAtIndex(2));
			continue;

		case OP_EvalLocalVariableCached3:
			assert(fs.top >= scrVmPub.stack);
			assert(fs.top+1 <= scrVmPub.maxstack);
			fs.top++;
			*fs.top = Scr_EvalVariable(Scr_GetLocalVarAtIndex(3));
			continue;

		case OP_EvalLocalVariableCached4:
			assert(fs.top >= scrVmPub.stack);
			assert(fs.top+1 <= scrVmPub.maxstack);
			fs.top++;
			*fs.top = Scr_EvalVariable(Scr_GetLocalVarAtIndex(4));
			continue;

		case OP_EvalLocalVariableCached5:
			assert(fs.top >= scrVmPub.stack);
			assert(fs.top+1 <= scrVmPub.maxstack);
			fs.top++;
			*fs.top = Scr_EvalVariable(Scr_GetLocalVarAtIndex(5));
			continue;

		case OP_EvalLocalVariableCached:
			assert(fs.top >= scrVmPub.stack);
			assert(fs.top+1 <= scrVmPub.maxstack);
			fs.top++;
			*fs.top = Scr_EvalVariable(Scr_GetLocalVar(fs.pos));
			fs.pos++;
			continue;

		case OP_EvalLocalArrayCached:
			assert(fs.top >= scrVmPub.stack);
			assert(fs.top+1 <= scrVmPub.maxstack);
			fs.top++;
			*fs.top = Scr_EvalVariable(Scr_GetLocalVar(fs.pos));
			fs.pos++;
			Scr_EvalArray(fs.top, fs.top - 1);
			fs.top--;
			continue;

		case OP_EvalArray:
			Scr_EvalArray(fs.top, fs.top - 1);
			fs.top--;
			continue;

		case OP_EvalLocalArrayRefCached0:
			fieldValueId = Scr_EvalArrayIndex(Scr_EvalArrayRef(Scr_GetLocalVarAtIndex(0)), fs.top);
			assert(fieldValueId);
			fs.top--;
			continue;

		case OP_EvalLocalArrayRefCached:
			fieldValueId = Scr_EvalArrayIndex(Scr_EvalArrayRef(Scr_GetLocalVar(fs.pos)), fs.top);
			assert(fieldValueId);
			fs.pos++;
			fs.top--;
			continue;

		case OP_EvalArrayRef:
			fieldValueId = Scr_EvalArrayIndex(Scr_EvalArrayRef(fieldValueId), fs.top);
			assert(fieldValueId);
			fs.top--;
			continue;

		case OP_ClearArray:
			assert(fieldValueId);
			ClearArray(fieldValueId, fs.top);
			fs.top--;
			continue;

		case OP_EmptyArray:
			fs.top++;
			fs.top->type = VAR_POINTER;
			fs.top->u.pointerValue = Scr_AllocArray();
			continue;

		case OP_GetSelfObject:
			objectId = Scr_GetSelf(fs.localId);
			if ( !IsFieldObject(objectId) )
			{
				Scr_Error(va("%s is not an object", var_typename[GetObjectType(objectId)]));
				goto error;
			}
			continue;

		case OP_EvalLevelFieldVariable:
			assert(fs.top >= scrVmPub.stack);
			assert(fs.top+1 <= scrVmPub.maxstack);
			fs.top++;
			*fs.top = Scr_EvalVariable(FindVariable(scrVarPub.levelId, Scr_ReadUnsignedShort(&fs.pos)));
			continue;

		case OP_EvalAnimFieldVariable:
			assert(fs.top >= scrVmPub.stack);
			assert(fs.top+1 <= scrVmPub.maxstack);
			fs.top++;
			*fs.top = Scr_EvalVariable(FindVariable(scrVarPub.animId, Scr_ReadUnsignedShort(&fs.pos)));
			continue;

		case OP_EvalSelfFieldVariable:
			objectId = Scr_GetSelf(fs.localId);
			if ( !IsFieldObject(objectId) )
			{
				fs.top++;
				Scr_ReadUnsignedShort(&fs.pos);
				Scr_Error(va("%s is not an object", var_typename[GetObjectType(objectId)]));
				goto error;
			}
			assert(fs.top >= scrVmPub.stack);
			assert(fs.top+1 <= scrVmPub.maxstack);
			fs.top++;
			*fs.top = Scr_FindVariableField(objectId, Scr_ReadUnsignedShort(&fs.pos));
			continue;

		case OP_EvalFieldVariable:
			assert(fs.top >= scrVmPub.stack);
			assert(fs.top+1 <= scrVmPub.maxstack);
			fs.top++;
			assert(objectId);
			*fs.top = Scr_FindVariableField(objectId, Scr_ReadUnsignedShort(&fs.pos));
			continue;

		case OP_EvalLevelFieldVariableRef:
			fieldValueId = Scr_GetVariableField(scrVarPub.levelId, Scr_ReadUnsignedShort(&fs.pos));
			continue;

		case OP_EvalAnimFieldVariableRef:
			fieldValueId = Scr_GetVariableField(scrVarPub.animId, Scr_ReadUnsignedShort(&fs.pos));
			continue;

		case OP_EvalSelfFieldVariableRef:
			fieldValueId = Scr_GetVariableField(Scr_GetSelf(fs.localId), Scr_ReadUnsignedShort(&fs.pos));
			continue;

		case OP_EvalFieldVariableRef:
			assert(objectId);
			fieldValueId = Scr_GetVariableField(objectId, Scr_ReadUnsignedShort(&fs.pos));
			continue;

		case OP_ClearFieldVariable:
			assert(objectId);
			ClearVariableField(objectId, Scr_ReadUnsignedShort(&fs.pos), fs.top);
			continue;

		case OP_SafeCreateVariableFieldCached:
			scrVmPub.localVars++;
			fs.localVarCount++;
			*scrVmPub.localVars = GetNewVariable(fs.localId, Scr_ReadUnsignedShort(&fs.pos));
			assert(fs.top->type != VAR_CODEPOS);
			if ( fs.top->type != VAR_PRECODEPOS )
			{
				SetVariableValue(Scr_GetLocalVarAtIndex(0), fs.top);
				fs.top--;
			}
			continue;

		case OP_SafeSetVariableFieldCached0:
			assert(fs.top->type != VAR_CODEPOS);
			if ( fs.top->type != VAR_PRECODEPOS )
			{
				SetVariableValue(Scr_GetLocalVarAtIndex(0), fs.top);
				fs.top--;
			}
			continue;

		case OP_SafeSetVariableFieldCached:
			assert(fs.top->type != VAR_CODEPOS);
			if ( fs.top->type != VAR_PRECODEPOS )
			{
				SetVariableValue(Scr_GetLocalVar(fs.pos), fs.top);
				fs.pos++;
				fs.top--;
				continue;
			}
			fs.pos++;
			continue;

		case OP_SafeSetWaittillVariableFieldCached:
			assert(fs.top->type != VAR_PRECODEPOS);
			if ( fs.top->type != VAR_CODEPOS )
			{
				SetVariableValue(Scr_GetLocalVar(fs.pos), fs.top);
				fs.pos++;
				fs.top--;
				continue;
			}
			ClearVariableValue(Scr_GetLocalVar(fs.pos));
			fs.pos++;
			continue;

		case OP_clearparams:
			assert(fs.top->type != VAR_PRECODEPOS);
			while ( fs.top->type != VAR_CODEPOS )
			{
				RemoveRefToValue(fs.top);
				fs.top--;
				assert(fs.top->type != VAR_PRECODEPOS);
			}
			continue;

		case OP_checkclearparams:
			assert(fs.top->type != VAR_CODEPOS);
			if ( fs.top->type != VAR_PRECODEPOS )
			{
				Scr_Error("function called with too many parameters");
				goto error;
			}
			fs.top->type = VAR_CODEPOS;
			continue;

		case OP_EvalLocalVariableRefCached0:
			fieldValueId = Scr_GetLocalVarAtIndex(0);
			continue;

		case OP_EvalLocalVariableRefCached:
			fieldValueId = Scr_GetLocalVar(fs.pos);
			fs.pos++;
			continue;

		case OP_SetLevelFieldVariableField:
			SetVariableValue(GetVariable(scrVarPub.levelId, Scr_ReadUnsignedShort(&fs.pos)), fs.top);
			fs.top--;
			continue;

		case OP_SetVariableField:
			assert(fieldValueId);
			SetVariableFieldValue(fieldValueId, fs.top);
			fs.top--;
			continue;

		case OP_SetAnimFieldVariableField:
			SetVariableValue(GetVariable(scrVarPub.animId, Scr_ReadUnsignedShort(&fs.pos)), fs.top);
			fs.top--;
			continue;

		case OP_SetSelfFieldVariableField:
			fieldValueId = Scr_GetVariableField(Scr_GetSelf(fs.localId), Scr_ReadUnsignedShort(&fs.pos));
			assert(fieldValueId);
			SetVariableFieldValue(fieldValueId, fs.top);
			fs.top--;
			continue;

		case OP_SetLocalVariableFieldCached0:
			SetVariableValue(Scr_GetLocalVarAtIndex(0), fs.top);
			fs.top--;
			continue;

		case OP_SetLocalVariableFieldCached:
			SetVariableValue(Scr_GetLocalVar(fs.pos), fs.top);
			fs.pos++;
			fs.top--;
			continue;

		case OP_CallBuiltin0:
		case OP_CallBuiltin1:
		case OP_CallBuiltin2:
		case OP_CallBuiltin3:
		case OP_CallBuiltin4:
		case OP_CallBuiltin5:
			assert(!scrVmPub.outparamcount);
			scrVmPub.outparamcount = gOpcode - OP_CallBuiltin0;
			goto CallBuiltin;

		case OP_CallBuiltin:
			assert(!scrVmPub.outparamcount);
			scrVmPub.outparamcount = Scr_ReadByte(&fs.pos);
CallBuiltin:
			assert(!scrVmPub.inparamcount);
			scrVmPub.top = fs.top;

			builtinIndex = Scr_ReadUnsignedShort(&fs.pos);
			scrVmPub.function_frame->fs.pos = fs.pos;

			scrCompilePub.func_table[builtinIndex].f();
			goto post_builtin;

		case OP_CallBuiltinMethod0:
		case OP_CallBuiltinMethod1:
		case OP_CallBuiltinMethod2:
		case OP_CallBuiltinMethod3:
		case OP_CallBuiltinMethod4:
		case OP_CallBuiltinMethod5:
			assert(!scrVmPub.outparamcount);
			scrVmPub.outparamcount = gOpcode - OP_CallBuiltinMethod0;
			goto CallBuiltinMethod;

		case OP_CallBuiltinMethod:
			assert(!scrVmPub.outparamcount);
			scrVmPub.outparamcount = Scr_ReadByte(&fs.pos);
CallBuiltinMethod:
			scrVmPub.top = fs.top - 1;
			builtinIndex = Scr_ReadUnsignedShort(&fs.pos);

			if ( fs.top->type != VAR_POINTER )
			{
				RemoveRefToValue(fs.top);
				scrVarPub.error_index = -1;
				Scr_Error(va("%s is not an entity", var_typename[fs.top->type]));
				goto error;
			}

			objectId = fs.top->u.pointerValue;

			if ( GetObjectType(objectId) != VAR_ENTITY )
			{
				RemoveRefToObject(objectId);
				scrVarPub.error_index = -1;
				Scr_Error(va("%s is not an entity", var_typename[GetObjectType(objectId)]));
				goto error;
			}

			entref = Scr_GetEntityIdRef(objectId);
			RemoveRefToObject(objectId);
			scrVmPub.function_frame->fs.pos = fs.pos;

			scrCompilePub.func_table[builtinIndex].m(entref);
post_builtin:
			fs.top = scrVmPub.top;
			fs.pos = scrVmPub.function_frame->fs.pos;

			if ( scrVmPub.outparamcount )
			{
				outparamcount = scrVmPub.outparamcount;

				scrVmPub.outparamcount = 0;
				scrVmPub.top -= outparamcount;

				do
				{
					RemoveRefToValue(fs.top);
					fs.top--;
					outparamcount--;
				}
				while ( outparamcount );
			}

			if ( scrVmPub.inparamcount )
			{
				assert(scrVmPub.inparamcount == 1);
				scrVmPub.inparamcount = 0;
				assert(fs.top == scrVmPub.top);
			}
			else
			{
				assert(fs.top == scrVmPub.top);
				fs.top++;
				fs.top->type = VAR_UNDEFINED;
			}
			continue;

		case OP_wait:
#define WAIT_FRAME_TIME 20 // ideally should use sv_fps value instead
			assert(Scr_IsInScriptMemory( fs.pos ));
			switch ( fs.top->type )
			{
			case VAR_FLOAT:
				if ( fs.top->u.floatValue < 0 )
				{
					Scr_Error("negative wait is not allowed");
					goto error;
				}

				waitTime = Q_rint(fs.top->u.floatValue * WAIT_FRAME_TIME);

				if ( !waitTime )
				{
					waitTime = fs.top->u.floatValue != 0;
				}
				break;

			case VAR_INTEGER:
				if ( fs.top->u.intValue < 0 )
				{
					Scr_Error("negative wait is not allowed");
					goto error;
				}

				waitTime = fs.top->u.intValue * WAIT_FRAME_TIME;
				break;

			default:
				scrVarPub.error_index = 2;
				Scr_Error(va("type %s is not a float", var_typename[fs.top->type]));
				goto error;
			}

			if ( waitTime > VAR_NAME_LOW_MASK )
			{
				scrVarPub.error_index = 2;
				Scr_Error("wait is too long");
				goto error;
			}

			assert(waitTime >= 0);

			if ( waitTime )
			{
				Scr_ResetTimeout();
			}

			waitTime = (scrVarPub.time + waitTime) & VAR_NAME_LOW_MASK;
			fs.top--;

			stackValue.type = VAR_STACK;
			stackValue.u.stackValue = VM_ArchiveStack( &fs );

			stackId = GetNewObjectVariable(GetArray(GetVariable(scrVarPub.timeArrayId, waitTime)), fs.localId);
			SetNewVariableValue(stackId, &stackValue);

			Scr_SetThreadWaitTime(fs.localId, waitTime);
			goto thread_end;

		case OP_waittillFrameEnd:
			assert(Scr_IsInScriptMemory( fs.pos ));
			assert(!(scrVarPub.time & ~VAR_NAME_LOW_MASK));

			stackValue.type = VAR_STACK;
			stackValue.u.stackValue = VM_ArchiveStack( &fs );

			stackId = GetNewObjectVariableReverse(GetArray(GetVariable(scrVarPub.timeArrayId, scrVarPub.time)), fs.localId);
			SetNewVariableValue(stackId, &stackValue);

			Scr_SetThreadWaitTime(fs.localId, scrVarPub.time);
			goto thread_end;

		case OP_PreScriptCall:
			assert(fs.top >= scrVmPub.stack);
			assert(fs.top+1 <= scrVmPub.maxstack);
			fs.top++;
			fs.top->type = VAR_PRECODEPOS;
			continue;

		case OP_ScriptFunctionCall2:
			assert(fs.top >= scrVmPub.stack);
			assert(fs.top+1 <= scrVmPub.maxstack);
			fs.top++;
			fs.top->type = VAR_PRECODEPOS;
			goto scriptFunctionCall;

		case OP_ScriptFunctionCall:
scriptFunctionCall:
			if ( scrVmPub.function_count > MAX_EMBEDDED_FUNCTION_CALLS )
			{
				Scr_Error("script stack overflow (too many embedded function calls)");
				goto error;
			}

			selfId = Scr_GetSelf(fs.localId);
			AddRefToObject(selfId);

			fs.localId = AllocChildThread(selfId, fs.localId);

			scrVmPub.function_frame->fs.pos = fs.pos;
			fs.pos = Scr_ReadCodePos(&scrVmPub.function_frame->fs.pos);
function_call:
			scrVmPub.function_frame->fs.localVarCount = fs.localVarCount;
			fs.localVarCount = 0;

			scrVmPub.function_count++;
			scrVmPub.function_frame++;

			scrVmPub.function_frame->fs.localId = fs.localId;
			assert(fs.pos);
			continue;

		case OP_ScriptFunctionCallPointer:
			if ( fs.top->type != VAR_FUNCTION )
			{
				Scr_Error(va("%s is not a function pointer", var_typename[fs.top->type]));
				goto error;
			}

			if ( scrVmPub.function_count > MAX_EMBEDDED_FUNCTION_CALLS )
			{
				scrVarPub.error_index = 1;
				Scr_Error("script stack overflow (too many embedded function calls)");
				goto error;
			}

			selfId = Scr_GetSelf(fs.localId);
			AddRefToObject(selfId);

			fs.localId = AllocChildThread(selfId, fs.localId);

			scrVmPub.function_frame->fs.pos = fs.pos;
			fs.pos = fs.top->u.codePosValue;

			fs.top--;
			goto function_call;

		case OP_ScriptMethodCall:
			if ( fs.top->type != VAR_POINTER )
			{
				scrVarPub.error_index = 1;
				Scr_Error(va("%s is not an object", var_typename[fs.top->type]));
				goto error;
			}

			if ( scrVmPub.function_count > MAX_EMBEDDED_FUNCTION_CALLS )
			{
				Scr_Error("script stack overflow (too many embedded function calls)");
				goto error;
			}

			fs.localId = AllocChildThread(fs.top->u.pointerValue, fs.localId);
			fs.top--;

			scrVmPub.function_frame->fs.pos = fs.pos;
			fs.pos = Scr_ReadCodePos(&scrVmPub.function_frame->fs.pos);
			goto function_call;

		case OP_ScriptMethodCallPointer:
			if ( fs.top->type != VAR_FUNCTION )
			{
				RemoveRefToValue(fs.top);
				fs.top--;
				value = fs.top + 1;
				Scr_Error(va("%s is not a function pointer", var_typename[value->type]));
				goto error;
			}

			tempCodePos = fs.top->u.codePosValue;
			fs.top--;

			if ( fs.top->type != VAR_POINTER )
			{
				scrVarPub.error_index = 2;
				Scr_Error(va("%s is not an object", var_typename[fs.top->type]));
				goto error;
			}

			if ( scrVmPub.function_count > MAX_EMBEDDED_FUNCTION_CALLS )
			{
				scrVarPub.error_index = 1;
				Scr_Error("script stack overflow (too many embedded function calls)");
				goto error;
			}

			fs.localId = AllocChildThread(fs.top->u.pointerValue, fs.localId);
			fs.top--;

			scrVmPub.function_frame->fs.pos = fs.pos;
			fs.pos = tempCodePos;
			goto function_call;

		case OP_ScriptThreadCall:
			if ( scrVmPub.function_count > MAX_EMBEDDED_FUNCTION_CALLS )
			{
				scrVarPub.error_index = 1;
				Scr_Error("script stack overflow (too many embedded function calls)");
				goto error;
			}

			selfId = Scr_GetSelf(fs.localId);
			AddRefToObject(selfId);

			fs.localId = AllocThread(selfId);

			scrVmPub.function_frame->fs.pos = fs.pos;
			scrVmPub.function_frame->fs.startTop = fs.startTop;

			fs.pos = Scr_ReadCodePos(&scrVmPub.function_frame->fs.pos);
			fs.startTop = &fs.top[-Scr_ReadUnsigned(&scrVmPub.function_frame->fs.pos)];
thread_call:
			scrVmPub.function_frame->fs.top = fs.startTop;
			scrVmPub.function_frame->topType = fs.startTop->type;

			fs.startTop->type = VAR_PRECODEPOS;
			gParamCount++;
			goto function_call;

		case OP_ScriptThreadCallPointer:
			if ( fs.top->type != VAR_FUNCTION )
			{
				Scr_Error(va("%s is not a function pointer", var_typename[fs.top->type]));
				goto error;
			}

			if ( scrVmPub.function_count > MAX_EMBEDDED_FUNCTION_CALLS )
			{
				scrVarPub.error_index = 1;
				Scr_Error("script stack overflow (too many embedded function calls)");
				goto error;
			}

			tempCodePos = fs.top->u.codePosValue;
			fs.top--;

			selfId = Scr_GetSelf(fs.localId);
			AddRefToObject(selfId);

			fs.localId = AllocThread(selfId);

			scrVmPub.function_frame->fs.pos = fs.pos;
			scrVmPub.function_frame->fs.startTop = fs.startTop;

			fs.pos = tempCodePos;
			fs.startTop = &fs.top[-Scr_ReadUnsigned(&scrVmPub.function_frame->fs.pos)];
			goto thread_call;

		case OP_ScriptMethodThreadCall:
			if ( fs.top->type != VAR_POINTER )
			{
				scrVarPub.error_index = 2;
				Scr_Error(va("%s is not an object", var_typename[fs.top->type]));
				goto error;
			}

			if ( scrVmPub.function_count > MAX_EMBEDDED_FUNCTION_CALLS )
			{
				scrVarPub.error_index = 1;
				Scr_Error("script stack overflow (too many embedded function calls)");
				goto error;
			}

			fs.localId = AllocThread(fs.top->u.pointerValue);
			fs.top--;

			scrVmPub.function_frame->fs.pos = fs.pos;
			scrVmPub.function_frame->fs.startTop = fs.startTop;

			fs.pos = Scr_ReadCodePos(&scrVmPub.function_frame->fs.pos);
			fs.startTop = &fs.top[-Scr_ReadUnsigned(&scrVmPub.function_frame->fs.pos)];
			goto thread_call;

		case OP_ScriptMethodThreadCallPointer:
			if ( fs.top->type != VAR_FUNCTION )
			{
				RemoveRefToValue(fs.top);
				fs.top--;
				value = fs.top + 1;
				Scr_Error(va("%s is not a function pointer", var_typename[value->type]));
				goto error;
			}

			tempCodePos = fs.top->u.codePosValue;
			fs.top--;

			if ( fs.top->type != VAR_POINTER )
			{
				scrVarPub.error_index = 2;
				Scr_Error(va("%s is not an object", var_typename[fs.top->type]));
				goto error;
			}

			if ( scrVmPub.function_count > MAX_EMBEDDED_FUNCTION_CALLS )
			{
				scrVarPub.error_index = 1;
				Scr_Error("script stack overflow (too many embedded function calls)");
				goto error;
			}

			fs.localId = AllocThread(fs.top->u.pointerValue);
			fs.top--;

			scrVmPub.function_frame->fs.pos = fs.pos;
			scrVmPub.function_frame->fs.startTop = fs.startTop;

			fs.pos = tempCodePos;
			fs.startTop = &fs.top[-Scr_ReadUnsigned(&scrVmPub.function_frame->fs.pos)];
			goto thread_call;

		case OP_DecTop:
			RemoveRefToValue(fs.top);
			fs.top--;
			continue;

		case OP_CastFieldObject:
			objectId = Scr_EvalFieldObject(scrVarPub.tempVariable, fs.top);
			fs.top--;
			continue;

		case OP_EvalLocalVariableObjectCached:
			objectId = Scr_EvalVariableObject(Scr_GetLocalVar(fs.pos));
			fs.pos++;
			continue;

		case OP_CastBool:
			Scr_CastBool(fs.top);
			continue;

		case OP_BoolNot:
			Scr_EvalBoolNot(fs.top);
			continue;

		case OP_BoolComplement:
			Scr_EvalBoolComplement(fs.top);
			continue;

		case OP_JumpOnFalse:
			Scr_CastBool(fs.top);
			assert(fs.top->type == VAR_INTEGER);
			jumpOffset = Scr_ReadUnsignedShort(&fs.pos);
			if ( !fs.top->u.intValue )
			{
				fs.pos += jumpOffset;
			}
			fs.top--;
			continue;

		case OP_JumpOnTrue:
			Scr_CastBool(fs.top);
			assert(fs.top->type == VAR_INTEGER);
			jumpOffset = Scr_ReadUnsignedShort(&fs.pos);
			if ( fs.top->u.intValue )
			{
				fs.pos += jumpOffset;
			}
			fs.top--;
			continue;

		case OP_JumpOnFalseExpr:
			Scr_CastBool(fs.top);
			assert(fs.top->type == VAR_INTEGER);
			jumpOffset = Scr_ReadUnsignedShort(&fs.pos);
			if ( fs.top->u.intValue )
			{
				fs.top--;
				continue;
			}
			fs.pos += jumpOffset;
			continue;

		case OP_JumpOnTrueExpr:
			Scr_CastBool(fs.top);
			assert(fs.top->type == VAR_INTEGER);
			jumpOffset = Scr_ReadUnsignedShort(&fs.pos);
			if ( !fs.top->u.intValue )
			{
				fs.top--;
				continue;
			}
			fs.pos += jumpOffset;
			continue;

		case OP_jump:
			jumpOffset = Scr_ReadUnsigned(&fs.pos);
			fs.pos += jumpOffset;
			continue;

		case OP_jumpback:
#define INFINITE_LOOP_TIMEOUT 5000
			if ( (unsigned int)(Sys_MilliSeconds() - scrVmGlob.starttime) < INFINITE_LOOP_TIMEOUT )
			{
				jumpOffset = Scr_ReadUnsignedShort(&fs.pos);
				fs.pos -= jumpOffset;
				continue;
			}

			if ( scrVmGlob.loading )
			{
				Com_Printf("script runtime warning: potential infinite loop in script.\n");
				Scr_PrintPrevCodePos(CON_CHANNEL_DONT_FILTER, fs.pos, 0);

				jumpOffset = Scr_ReadUnsignedShort(&fs.pos);
				fs.pos -= jumpOffset;

				Scr_ResetTimeout();
				continue;
			}

			if ( scrVmPub.abort_on_error )
			{
				Scr_TerminalError("potential infinite loop in script");
				goto error;
			}

			Com_Printf("script runtime error: potential infinite loop in script - killing thread.\n");
			Scr_PrintPrevCodePos(CON_CHANNEL_DONT_FILTER, fs.pos, 0);

			Scr_ResetTimeout();

			while ( 1 )
			{
				parentLocalId = GetSafeParentLocalId(fs.localId);
				Scr_KillThread(fs.localId);

				scrVmPub.localVars -= fs.localVarCount;
				assert(fs.top->type != VAR_PRECODEPOS);

				while ( fs.top->type != VAR_CODEPOS )
				{
					RemoveRefToValue(fs.top);
					fs.top--;
					assert(fs.top->type != VAR_PRECODEPOS);
				}

				scrVmPub.function_count--;
				scrVmPub.function_frame--;

				if ( !parentLocalId )
				{
					break;
				}

				assert(fs.top != fs.startTop);
				RemoveRefToObject(fs.localId);

				assert(fs.top->type == VAR_CODEPOS);
				fs.pos = scrVmPub.function_frame->fs.pos;
				assert(fs.pos);

				fs.localVarCount = scrVmPub.function_frame->fs.localVarCount;
				fs.localId = parentLocalId;
				fs.top--;
			}

			assert(fs.top == fs.startTop);
			goto thread_end;

		case OP_inc:
			assert(fs.top >= scrVmPub.stack);
			assert(fs.top+1 <= scrVmPub.maxstack);
			fs.top++;

			*fs.top = Scr_EvalVariableField(fieldValueId);

			if ( fs.top->type != VAR_INTEGER )
			{
				Scr_Error(va("++ must be applied to an int (applied to %s)", var_typename[fs.top->type]));
				goto error;
			}

			fs.top->u.intValue++;
			assert(*fs.pos == OP_SetVariableField);
			fs.pos++;

			SetVariableFieldValue(fieldValueId, fs.top);
			fs.top--;
			continue;

		case OP_dec:
			assert(fs.top >= scrVmPub.stack);
			assert(fs.top+1 <= scrVmPub.maxstack);
			fs.top++;

			*fs.top = Scr_EvalVariableField(fieldValueId);

			if ( fs.top->type != VAR_INTEGER )
			{
				Scr_Error(va("-- must be applied to an int (applied to %s)", var_typename[fs.top->type]));
				goto error;
			}

			fs.top->u.intValue--;
			assert(*fs.pos == OP_SetVariableField);
			fs.pos++;

			SetVariableFieldValue(fieldValueId, fs.top);
			fs.top--;
			continue;

		case OP_bit_or:
			Scr_EvalOr(fs.top - 1, fs.top);
			fs.top--;
			continue;

		case OP_bit_ex_or:
			Scr_EvalExOr(fs.top - 1, fs.top);
			fs.top--;
			continue;

		case OP_bit_and:
			Scr_EvalAnd(fs.top - 1, fs.top);
			fs.top--;
			continue;

		case OP_equality:
			Scr_EvalEquality(fs.top - 1, fs.top);
			fs.top--;
			continue;

		case OP_inequality:
			Scr_EvalInequality(fs.top - 1, fs.top);
			fs.top--;
			continue;

		case OP_less:
			Scr_EvalLess(fs.top - 1, fs.top);
			fs.top--;
			continue;

		case OP_greater:
			Scr_EvalGreater(fs.top - 1, fs.top);
			fs.top--;
			continue;

		case OP_less_equal:
			Scr_EvalLessEqual(fs.top - 1, fs.top);
			fs.top--;
			continue;

		case OP_greater_equal:
			Scr_EvalGreaterEqual(fs.top - 1, fs.top);
			fs.top--;
			continue;

		case OP_shift_left:
			Scr_EvalShiftLeft(fs.top - 1, fs.top);
			fs.top--;
			continue;

		case OP_shift_right:
			Scr_EvalShiftRight(fs.top - 1, fs.top);
			fs.top--;
			continue;

		case OP_plus:
			Scr_EvalPlus(fs.top - 1, fs.top);
			fs.top--;
			continue;

		case OP_minus:
			Scr_EvalMinus(fs.top - 1, fs.top);
			fs.top--;
			continue;

		case OP_multiply:
			Scr_EvalMultiply(fs.top - 1, fs.top);
			fs.top--;
			continue;

		case OP_divide:
			Scr_EvalDivide(fs.top - 1, fs.top);
			fs.top--;
			continue;

		case OP_mod:
			Scr_EvalMod(fs.top - 1, fs.top);
			fs.top--;
			continue;

		case OP_size:
			Scr_EvalSizeValue(fs.top);
			continue;

		case OP_waittillmatch:
		case OP_waittill:
			assert(Scr_IsInScriptMemory( fs.pos ));
			if ( fs.top->type != VAR_POINTER )
			{
				scrVarPub.error_index = 2;
				Scr_Error(va("%s is not an object", var_typename[fs.top->type]));
				goto error;
			}

			if ( !IsFieldObject(fs.top->u.pointerValue) )
			{
				scrVarPub.error_index = 2;
				Scr_Error(va("%s is not an object", var_typename[GetObjectType(fs.top->u.pointerValue)]));
				goto error;
			}

			tempValue.u = fs.top->u;
			fs.top--;

			if ( fs.top->type != VAR_STRING )
			{
				fs.top++;
				scrVarPub.error_index = 3;
				Scr_Error("first parameter of waittill must evaluate to a string");
				goto error;
			}

			stringValue = fs.top->u.stringValue;
			fs.top--;

			assert(GetObjectType( tempValue.u.pointerValue ) != VAR_THREAD);
			assert(GetObjectType( tempValue.u.pointerValue ) != VAR_NOTIFY_THREAD);
			assert(GetObjectType( tempValue.u.pointerValue ) != VAR_TIME_THREAD);
			assert(GetObjectType( tempValue.u.pointerValue ) != VAR_CHILD_THREAD);
			assert(GetObjectType( tempValue.u.pointerValue ) != VAR_DEAD_THREAD);

			stackValue.type = VAR_STACK;
			stackValue.u.stackValue = VM_ArchiveStack( &fs );

			stackId = GetNewObjectVariable(GetArray(GetVariable(GetArray(GetVariable(tempValue.u.pointerValue, OBJECT_NOTIFY_LIST)), stringValue)), fs.localId);
			SetNewVariableValue(stackId, &stackValue);

			tempValue.type = VAR_POINTER;
			SetNewVariableValue(GetNewObjectVariable(GetArray(GetObjectVariable(scrVarPub.pauseArrayId, Scr_GetSelf(fs.localId))), fs.localId), &tempValue);

			Scr_SetThreadNotifyName(fs.localId, stringValue);
			goto thread_end;

		case OP_notify:
			if ( fs.top->type != VAR_POINTER )
			{
				scrVarPub.error_index = 2;
				Scr_Error(va("%s is not an object", var_typename[fs.top->type]));
				goto error;
			}

			id = fs.top->u.pointerValue;

			if ( !IsFieldObject(id) )
			{
				scrVarPub.error_index = 2;
				Scr_Error(va("%s is not an object", var_typename[GetObjectType(id)]));
				goto error;
			}

			fs.top--;

			if ( fs.top->type != VAR_STRING )
			{
				fs.top++;
				scrVarPub.error_index = 1;
				Scr_Error("first parameter of notify must evaluate to a string");
				goto error;
			}

			stringValue = fs.top->u.stringValue;
			fs.top--;

			scrVmPub.function_frame->fs.pos = fs.pos;
			VM_Notify(id, stringValue, fs.top);

			fs.pos = scrVmPub.function_frame->fs.pos;

			RemoveRefToObject(id);
			SL_RemoveRefToString(stringValue);

			assert(fs.top->type != VAR_CODEPOS);
			while ( fs.top->type != VAR_PRECODEPOS )
			{
				RemoveRefToValue(fs.top);
				fs.top--;
				assert(fs.top->type != VAR_CODEPOS);
			}

			fs.top--;
			continue;

		case OP_endon:
			if ( fs.top->type != VAR_POINTER )
			{
				scrVarPub.error_index = 1;
				Scr_Error(va("%s is not an object", var_typename[fs.top->type]));
				goto error;
			}

			if ( !IsFieldObject(fs.top->u.pointerValue) )
			{
				scrVarPub.error_index = 1;
				Scr_Error(va("%s is not an object", var_typename[GetObjectType(fs.top->u.pointerValue)]));
				goto error;
			}

			value = fs.top - 1;

			if ( value->type != VAR_STRING )
			{
				Scr_Error("first parameter of endon must evaluate to a string");
				goto error;
			}

			stringValue = value->u.stringValue;

			AddRefToObject(fs.localId);
			threadId = AllocThread(fs.localId);

			assert(GetObjectType( fs.top->u.pointerValue ) != VAR_THREAD);
			assert(GetObjectType( fs.top->u.pointerValue ) != VAR_NOTIFY_THREAD);
			assert(GetObjectType( fs.top->u.pointerValue ) != VAR_TIME_THREAD);
			assert(GetObjectType( fs.top->u.pointerValue ) != VAR_CHILD_THREAD);
			assert(GetObjectType( fs.top->u.pointerValue ) != VAR_DEAD_THREAD);

			GetObjectVariable(GetArray(GetVariable(GetArray(GetVariable(fs.top->u.pointerValue, OBJECT_NOTIFY_LIST)), stringValue)), threadId);
			RemoveRefToObject(threadId);

			tempValue.type = VAR_POINTER;
			tempValue.u = fs.top->u;

			SetNewVariableValue(GetNewObjectVariable(GetArray(GetObjectVariable(scrVarPub.pauseArrayId, fs.localId)), threadId), &tempValue);
			Scr_SetThreadNotifyName(threadId, stringValue);

			fs.top -= 2;
			continue;

		case OP_voidCodepos:
			assert(fs.top >= scrVmPub.stack);
			assert(fs.top+1 <= scrVmPub.maxstack);
			fs.top++;
			fs.top->type = VAR_PRECODEPOS;
			continue;

		case OP_switch:
			jumpOffset = Scr_ReadUnsigned(&fs.pos);
			fs.pos += jumpOffset;

			gCaseCount = Scr_ReadUnsignedShort(&fs.pos);

			switch ( fs.top->type )
			{
			case VAR_STRING:
				caseValue = fs.top->u.stringValue;
				SL_RemoveRefToString(caseValue);
				break;

			case VAR_INTEGER:
				if ( !IsValidArrayIndex(fs.top->u.pointerValue) )
				{
					Scr_Error(va("switch index %d out of range", fs.top->u.intValue));
					goto error;
				}
				caseValue = GetInternalVariableIndex(fs.top->u.pointerValue);
				break;

			default:
				Scr_Error(va("cannot switch on %s", var_typename[fs.top->type]));
				goto error;
			}

			if ( !gCaseCount )
			{
				fs.top--;
				continue;
			}

			assert(caseValue);

			do
			{
				currentCaseValue = Scr_ReadUnsigned(&fs.pos);
				currentCodePos = Scr_ReadCodePos(&fs.pos);

				if ( currentCaseValue == caseValue )
				{
					fs.pos = currentCodePos;
					assert(fs.pos);
					goto loop_dec_top;
				}

				gCaseCount--;
			}
			while ( gCaseCount );

			if ( !currentCaseValue )
			{
				fs.pos = currentCodePos;
				assert(fs.pos);
			}
loop_dec_top:
			fs.top--;
			continue;

		case OP_endswitch:
			gCaseCount = Scr_ReadUnsignedShort(&fs.pos);
			Scr_ReadIntArray(&fs.pos, sizeof(unsigned short) * gCaseCount);
			continue;

		case OP_vector:
			fs.top -= 2;
			Scr_CastVector(fs.top);
			continue;

		case OP_NOP:
			continue;

		case OP_abort:
			assert(g_script_error_level >= 0);
			g_script_error_level--;
			return 0;

		case OP_object:
			assert(fs.top >= scrVmPub.stack);
			assert(fs.top+1 <= scrVmPub.maxstack);
			fs.top++;

			classnum = Scr_ReadUnsigned(&fs.pos);
			entnum = Scr_ReadUnsigned(&fs.pos);

			fs.top->u.pointerValue = FindEntityId(entnum, classnum);

			if ( !fs.top->u.pointerValue )
			{
				fs.top->type = VAR_UNDEFINED;
				Scr_Error("unknown object");
				goto error;
			}

			fs.top->type = VAR_POINTER;
			AddRefToObject(fs.top->u.pointerValue);
			continue;

		case OP_thread_object:
			++fs.top;
			fs.top->u.pointerValue = Scr_ReadUnsignedShort(&fs.pos);
			fs.top->type = VAR_POINTER;
			AddRefToObject(fs.top->u.pointerValue);
			continue;

		case OP_EvalLocalVariable:
			assert(fs.top >= scrVmPub.stack);
			assert(fs.top+1 <= scrVmPub.maxstack);
			fs.top++;
			*fs.top = Scr_EvalVariable(FindVariable(fs.localId, Scr_ReadUnsignedShort(&fs.pos)));
			continue;

		case OP_EvalLocalVariableRef:
			fieldValueId = FindVariable(fs.localId, Scr_ReadUnsignedShort(&fs.pos));
			if ( !fieldValueId )
			{
				Scr_Error("cannot create a new local variable in the debugger");
				goto error;
			}
			continue;

		case OP_prof_begin:
			fs.pos++;
			continue;

		case OP_prof_end:
			fs.pos++;
			continue;

		case OP_breakpoint:
			if (scrVarPub.developer)
			{
				Com_PrintMessage(CON_CHANNEL_DONT_FILTER, "\nCode hit debug breakpoint at:\n");
				Scr_PrintPrevCodePos(CON_CHANNEL_DONT_FILTER, fs.pos, 0);
			}
			continue;

		default:
			scrVmPub.terminal_error = 1;
			RuntimeErrorInternal(CON_CHANNEL_DONT_FILTER, fs.pos, 0, va("CODE ERROR: unknown opcode %d", gOpcode));
			continue;
		}
	}
}
