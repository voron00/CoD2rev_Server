#include "../qcommon/qcommon.h"
#include "clientscript_public.h"

scrVmGlob_t scrVmGlob;
scrVmPub_t scrVmPub;

qboolean Scr_IsSystemActive()
{
	return scrVarPub.timeArrayId != 0;
}

void Scr_Error(const char *error)
{
	if ( !scrVarPub.error_message )
		scrVarPub.error_message = error;

	Scr_ErrorInternal();
}

void Scr_ObjectError(const char *error)
{
	scrVarPub.error_index = -1;
	Scr_Error(error);
}

void Scr_ParamError(int paramNum, const char *error)
{
	scrVarPub.error_index = paramNum + 1;
	Scr_Error(error);
}

int Scr_GetType(unsigned int param)
{
	if ( param < scrVmPub.outparamcount )
		return scrVmPub.top[-param].type;

	Scr_Error(va("parameter %d does not exist", param + 1));
	return 0;
}

unsigned int Scr_GetNumParam()
{
	return scrVmPub.outparamcount;
}

void Scr_ClearOutParams()
{
	while ( scrVmPub.outparamcount )
	{
		RemoveRefToValue(scrVmPub.top);
		--scrVmPub.top;
		--scrVmPub.outparamcount;
	}
}

void IncInParam()
{
	Scr_ClearOutParams();

	if ( scrVmPub.top == scrVmPub.maxstack )
		Com_Error(ERR_DROP, "Internal script stack overflow");

	++scrVmPub.top;
	++scrVmPub.inparamcount;
}

void Scr_AddUndefined()
{
	IncInParam();
	scrVmPub.top->type = VAR_UNDEFINED;
}

void Scr_AddBool(bool value)
{
	IncInParam();
	scrVmPub.top->type = VAR_INTEGER;
	scrVmPub.top->u.intValue = value;
}

void Scr_AddInt(int value)
{
	IncInParam();
	scrVmPub.top->type = VAR_INTEGER;
	scrVmPub.top->u.intValue = value;
}

void Scr_AddFloat(float value)
{
	IncInParam();
	scrVmPub.top->type = VAR_FLOAT;
	scrVmPub.top->u.floatValue = value;
}

void Scr_AddAnim(scr_anim_t value)
{
	IncInParam();
	scrVmPub.top->type = VAR_ANIMATION;
	scrVmPub.top->u.codePosValue = value.linkPointer;
}

void Scr_AddObject(unsigned int id)
{
	IncInParam();
	scrVmPub.top->type = VAR_OBJECT;
	scrVmPub.top->u.pointerValue = id;
	AddRefToObject(id);
}

void Scr_AddEntityNum(int entnum, unsigned int classnum)
{
	unsigned int entId;

	entId = Scr_GetEntityId(entnum, classnum);
	Scr_AddObject(entId);
}

void Scr_AddString(const char *value)
{
	IncInParam();
	scrVmPub.top->type = VAR_STRING;
	scrVmPub.top->u.stringValue = SL_GetString(value, 0);
}

void Scr_AddIString(const char *value)
{
	IncInParam();
	scrVmPub.top->type = VAR_ISTRING;
	scrVmPub.top->u.stringValue = SL_GetString(value, 0);
}

void Scr_AddConstString(unsigned int value)
{
	IncInParam();
	scrVmPub.top->type = VAR_STRING;
	scrVmPub.top->u.stringValue = value;
	SL_AddRefToString(value);
}

void Scr_AddVector(const float *value)
{
	IncInParam();
	scrVmPub.top->type = VAR_VECTOR;
	scrVmPub.top->u.vectorValue = Scr_AllocVector(value);
}

void Scr_MakeArray()
{
	IncInParam();
	scrVmPub.top->type = VAR_OBJECT;
	scrVmPub.top->u.intValue = Scr_AllocArray();
}

void Scr_AddArray()
{
	unsigned int ArraySize;
	unsigned int id;

	--scrVmPub.top;
	--scrVmPub.inparamcount;

	ArraySize = GetArraySize(scrVmPub.top->u.stringValue);
	id = GetNewArrayVariable(scrVmPub.top->u.stringValue, ArraySize);
	SetNewVariableValue(id, scrVmPub.top + 1);
}

void Scr_AddArrayStringIndexed(unsigned int stringValue)
{
	unsigned int id;

	--scrVmPub.top;
	--scrVmPub.inparamcount;

	id = GetNewVariable(scrVmPub.top->u.stringValue, stringValue);
	SetNewVariableValue(id, scrVmPub.top + 1);
}

int Scr_GetPointerType(unsigned int index)
{
	if ( index >= scrVmPub.outparamcount )
	{
		Scr_Error(va("parameter %d does not exist", index + 1));
		return 0;
	}

	if ( scrVmPub.top[-index].type != VAR_OBJECT )
	{
		Scr_Error(va("type %s is not a pointer", var_typename[scrVmPub.top[-index].type]));
	}

	return GetObjectType(scrVmPub.top[-index].u.intValue);
}

void Scr_GetEntityRef(scr_entref_t *entRef, unsigned int index)
{
	VariableValue *entryValue;
	int entId;

	if ( index < scrVmPub.outparamcount )
	{
		entryValue = &scrVmPub.top[-index];

		if ( entryValue->type == VAR_OBJECT )
		{
			entId = entryValue->u.pointerValue;

			if ( GetObjectType(entryValue->u.pointerValue) == VAR_ENTITY )
			{
				Scr_GetEntityIdRef(entRef, entId);
				return;
			}

			scrVarPub.error_index = index + 1;
			Scr_Error(va("type %s is not an entity", var_typename[GetObjectType(entId)]));
		}

		scrVarPub.error_index = index + 1;
		Scr_Error(va("type %s is not an entity", var_typename[entryValue->type]));
	}

	Scr_Error(va("parameter %d does not exist", index + 1));
	entRef = 0;
}

int Scr_GetInt(unsigned int index)
{
	VariableValue *entryValue;

	if ( index >= scrVmPub.outparamcount )
	{
		Scr_Error(va("parameter %d does not exist", index + 1));
		return 0;
	}

	entryValue = &scrVmPub.top[-index];

	if ( entryValue->type != VAR_INTEGER )
	{
		scrVarPub.error_index = index + 1;
		Scr_Error(va("type %s is not an int", var_typename[entryValue->type]));
	}

	return entryValue->u.intValue;
}

double Scr_GetFloat(unsigned int index)
{
	VariableValue *entryValue;

	if ( index >= scrVmPub.outparamcount )
	{
		Scr_Error(va("parameter %d does not exist", index + 1));
		return 0;
	}

	entryValue = &scrVmPub.top[-index];

	if ( entryValue->type != VAR_FLOAT )
	{
		if ( entryValue->type == VAR_INTEGER )
			return (float)entryValue->u.intValue;

		scrVarPub.error_index = index + 1;
		Scr_Error(va("type %s is not a float", var_typename[entryValue->type]));
	}

	return entryValue->u.floatValue;
}

unsigned int Scr_GetConstString(unsigned int index)
{
	VariableValue *entryValue;

	if ( index >= scrVmPub.outparamcount )
	{
		Scr_Error(va("parameter %d does not exist", index + 1));
		return 0;
	}

	entryValue = &scrVmPub.top[-index];

	if ( !Scr_CastString(entryValue) )
	{
		scrVarPub.error_index = index + 1;
		Scr_ErrorInternal();
	}

	return entryValue->u.intValue;
}

unsigned int Scr_GetConstStringIncludeNull(unsigned int index)
{
	if ( index >= scrVmPub.outparamcount || scrVmPub.top[-index].type )
		return Scr_GetConstString(index);
	else
		return 0;
}

const char* Scr_GetString(unsigned int index)
{
	unsigned int stringValue;

	stringValue = Scr_GetConstString(index);
	return SL_ConvertToString(stringValue);
}

unsigned int Scr_GetConstIString(unsigned int index)
{
	VariableValue *entryValue;

	if ( index >= scrVmPub.outparamcount )
	{
		Scr_Error(va("parameter %d does not exist", index + 1));
		return 0;
	}

	entryValue = &scrVmPub.top[-index];

	if ( entryValue->type != VAR_ISTRING )
	{
		scrVarPub.error_index = index + 1;
		Scr_Error(va("type %s is not a localized string", var_typename[entryValue->type]));
	}

	return entryValue->u.intValue;
}

const char* Scr_GetIString(unsigned int index)
{
	unsigned int stringValue;

	stringValue = Scr_GetConstIString(index);
	return SL_ConvertToString(stringValue);
}

void Scr_GetVector(unsigned int index, float *vector)
{
	VariableValue *entryValue;

	if ( index < scrVmPub.outparamcount )
	{
		entryValue = &scrVmPub.top[-index];

		if ( entryValue->type == VAR_VECTOR )
		{
			VectorCopy(entryValue->u.vectorValue, vector);
			return;
		}

		scrVarPub.error_index = index + 1;
		Scr_Error(va("type %s is not a vector", var_typename[entryValue->type]));
	}

	Scr_Error(va("parameter %d does not exist", index + 1));
}

const char* Scr_GetDebugString(unsigned int index)
{
	VariableValue *value;

	if ( index >= scrVmPub.outparamcount )
	{
		Scr_Error(va("parameter %d does not exist", index + 1));
		return 0;
	}
	else
	{
		value = &scrVmPub.top[-index];
		Scr_CastDebugString(value);
		return SL_ConvertToString(value->u.intValue);
	}
}

void Scr_GetAnim(scr_anim_s *pAnim, unsigned int index, XAnimTree_s *tree)
{
	XAnim_s *xanim;
	const char *animTreeName;
	const char *entityTreeName;
	const char *animName;
	VariableValue *pValue;

	if ( index >= scrVmPub.outparamcount )
		goto paramerror;

	pValue = &scrVmPub.top[-index];

	if ( pValue->type != VAR_ANIMATION )
	{
		scrVarPub.error_message = va("type %s is not an anim", var_typename[pValue->type]);
		goto error;
	}

	pAnim->linkPointer = pValue->u.codePosValue;

	if ( tree )
	{
		xanim = Scr_GetAnims(pAnim->tree);

		if ( xanim != XAnimGetAnims(tree) )
		{
			xanim = XAnimGetAnims(tree);
			animTreeName = XAnimGetAnimTreeDebugName(xanim);
			xanim = Scr_GetAnims(pAnim->tree);
			entityTreeName = XAnimGetAnimTreeDebugName(xanim);
			xanim = Scr_GetAnims(pAnim->tree);
			animName = XAnimGetAnimDebugName(xanim, pAnim->index);
			scrVarPub.error_message = va(
			                              "anim '%s' in animtree '%s' does not belong to the entity's animtree '%s'",
			                              animName,
			                              entityTreeName,
			                              animTreeName);
error:
			RemoveRefToValue(pValue);
			pValue->type = VAR_UNDEFINED;
			scrVarPub.error_index = index + 1;
			Scr_ErrorInternal();
paramerror:
			Scr_Error(va("parameter %d does not exist", index + 1));
			pAnim->index = 0;
			pAnim->tree = 0;
		}
	}
}

const char* Scr_GetTypeName(unsigned int index)
{
	if ( index < scrVmPub.outparamcount )
		return var_typename[scrVmPub.top[-index].type];

	Scr_Error(va("parameter %d does not exist", index + 1));
	return 0;
}

unsigned int Scr_GetConstLowercaseString(unsigned int index)
{
	unsigned int stringValue;
	VariableValue *value;
	const char *string;
	int i;
	char tempString[8192];

	if ( index >= scrVmPub.outparamcount )
		goto error;

	value = &scrVmPub.top[-index];

	if ( !Scr_CastString(value) )
	{
		scrVarPub.error_index = index + 1;
		Scr_ErrorInternal();
error:
		Scr_Error(va("parameter %d does not exist", index + 1));
		return 0;
	}

	stringValue = value->u.stringValue;
	string = SL_ConvertToString(value->u.stringValue);

	for ( i = 0; ; ++i )
	{
		tempString[i] = tolower(string[i]);

		if ( !string[i] )
			break;
	}

	value->u.stringValue = SL_GetString(tempString, 0);
	SL_RemoveRefToString(stringValue);

	return value->u.stringValue;
}

unsigned int Scr_GetObject(unsigned int paramnum)
{
	VariableValue *var;

	if (paramnum >= scrVmPub.outparamcount)
	{
		Scr_Error(va("parameter %d does not exist", paramnum + 1));
		return 0;
	}

	var = &scrVmPub.top[-paramnum];

	if (var->type == 1)
	{
		return var->u.pointerValue;
	}

	scrVarPub.error_index = paramnum + 1;
	Scr_Error(va("type %s is not an object", var_typename[var->type]));
	return 0;
}

void Scr_SetStructField(unsigned int structId, unsigned int index)
{
	unsigned int id;

	id = Scr_GetVariableField(structId, index);
	scrVmPub.inparamcount = 0;
	SetVariableFieldValue(id, scrVmPub.top);
	--scrVmPub.top;
}

void Scr_SetLoading(int bLoading)
{
	scrVmGlob.loading = bLoading;
}

void Scr_SetDynamicEntityField(int entnum, unsigned int classnum, unsigned int index)
{
	unsigned int entId;

	entId = Scr_GetEntityId(entnum, classnum);
	Scr_SetStructField(entId, index);
}

unsigned short Scr_ExecEntThreadNum(int entnum, unsigned int classnum, int handle, unsigned int paramcount)
{
	unsigned int threadId;
	const char *pos;
	unsigned int self;
	unsigned short id;

	pos = &scrVarPub.programBuffer[handle];

	if ( !scrVmPub.function_count )
		Scr_ResetTimeout();

	self = Scr_GetEntityId(entnum, classnum);
	AddRefToObject(self);
	threadId = AllocThread(self);
	id = VM_Execute(threadId, pos, paramcount);
	RemoveRefToValue(scrVmPub.top);
	scrVmPub.top->type = VAR_UNDEFINED;
	--scrVmPub.top;
	--scrVmPub.inparamcount;

	return id;
}

void Scr_FreeThread(unsigned short handle)
{
	RemoveRefToObject(handle);
}

void Scr_IncTime()
{
	Scr_RunCurrentThreads();
	Scr_FreeEntityList();
	++scrVarPub.time;
	scrVarPub.time &= 0xFFFFFFu;
}

void Scr_DecTime()
{
	--scrVarPub.time;
	scrVarPub.time &= 0xFFFFFFu;
}

bool SetEntityFieldValue(unsigned int classnum, int entnum, int offset, VariableValue *value)
{
	scrVmPub.outparamcount = 1;
	scrVmPub.top = value;

	if ( Scr_SetObjectField(classnum, entnum, offset) )
	{
		if ( scrVmPub.outparamcount )
		{
			RemoveRefToValue(scrVmPub.top);
			--scrVmPub.top;
			scrVmPub.outparamcount = 0;
		}

		return true;
	}
	else
	{
		scrVmPub.outparamcount = 0;
		return false;
	}
}

void GetEntityFieldValue(VariableValue *pValue, unsigned int classnum, int entnum, int offset)
{
	int type;

	scrVmPub.top = scrVmGlob.eval_stack - 1;
	scrVmGlob.eval_stack->type = VAR_UNDEFINED;
	Scr_GetObjectField(classnum, entnum, offset);
	scrVmPub.inparamcount = 0;
	type = scrVmGlob.eval_stack->type;
	pValue->u.intValue = scrVmGlob.eval_stack->u.intValue;
	pValue->type = type;
}

void VM_CancelNotifyInternal(unsigned int notifyListOwnerId, unsigned int startLocalId, unsigned int notifyListId, unsigned int notifyNameListId, unsigned int stringValue)
{
	Scr_RemoveThreadNotifyName(startLocalId);
	RemoveObjectVariable(notifyNameListId, startLocalId);

	if ( !GetArraySize(notifyNameListId) )
	{
		RemoveVariable(notifyListId, stringValue);

		if ( !GetArraySize(notifyListId) )
			RemoveVariable(notifyListOwnerId, 0x1FFFEu);
	}
}

void VM_CancelNotify(unsigned int notifyListOwnerId, unsigned int startLocalId)
{
	unsigned int varIndex;
	unsigned int index;
	unsigned int notifyNameListId;
	unsigned int notifyListId;
	unsigned int name;

	varIndex = FindVariable(notifyListOwnerId, 0x1FFFEu);
	notifyListId = FindObject(varIndex);
	name = Scr_GetThreadNotifyName(startLocalId);
	index = FindVariable(notifyListId, name);
	notifyNameListId = FindObject(index);
	VM_CancelNotifyInternal(notifyListOwnerId, startLocalId, notifyListId, notifyNameListId, name);
}

VariableStackBuffer* VM_ArchiveStack(int size, const char *codePos, VariableValue *top, unsigned int localVarCount, unsigned int *localId)
{
	unsigned int id;
	char *buf;
	char *pos;
	VariableStackBuffer *stackBuf;

	stackBuf = (VariableStackBuffer *)MT_Alloc(5 * size + 11);
	id = *localId;
	stackBuf->localId = *localId;
	stackBuf->size = size;
	stackBuf->bufLen = 5 * size + 11;
	stackBuf->pos = codePos;
	stackBuf->time = scrVarPub.time;
	scrVmPub.localVars -= localVarCount;
	buf = &stackBuf->buf[5 * size];

	while ( size )
	{
		pos = buf - 4;

		if ( top->type == VAR_CODEPOS )
		{
			--scrVmPub.function_count;
			--scrVmPub.function_frame;
			*(uint32_t *)pos = (uint32_t)scrVmPub.function_frame->fs.pos;
			scrVmPub.localVars -= scrVmPub.function_frame->fs.localVarCount;
			id = GetParentLocalId(id);
		}
		else
		{
			*(uint32_t *)pos = (uint32_t)top->u.codePosValue;
		}

		buf = pos - 1;
		*buf = top->type;
		--top;
		--size;
	}

	--scrVmPub.function_count;
	--scrVmPub.function_frame;
	AddRefToObject(id);
	*localId = id;

	return stackBuf;
}

void VM_TerminateStack(unsigned int endLocalId, unsigned int startLocalId, VariableStackBuffer *stackValue)
{
	unsigned int variable;
	unsigned int array;
	VariableValue entryValue;
	unsigned int id;
	unsigned char type;
	int size;
	char *buf;
	VariableUnion tempValue;
	unsigned int parentId;
	unsigned int localId;

	size = stackValue->size;
	localId = stackValue->localId;
	buf = &stackValue->buf[5 * size];

	while ( size )
	{
		buf -= 4;
		tempValue.intValue = *(int *)buf--;
		type = *buf;
		--size;

		if ( type == VAR_CODEPOS )
		{
			parentId = GetParentLocalId(localId);
			Scr_KillThread(localId);
			RemoveRefToObject(localId);

			if ( localId == endLocalId )
			{
				++size;
				*buf = 0;
				Scr_SetThreadWaitTime(startLocalId, scrVarPub.time);
				stackValue->pos = tempValue.codePosValue;
				stackValue->localId = parentId;
				stackValue->size = size;
				entryValue.type = VAR_STACK;
				entryValue.u.stackValue = stackValue;
				variable = GetVariable(scrVarPub.timeArrayId, scrVarPub.time);
				array = GetArray(variable);
				id = GetNewObjectVariable(array, startLocalId);
				SetNewVariableValue(id, &entryValue);
				return;
			}

			localId = parentId;
		}
		else
		{
			RemoveRefToValueInternal(type, tempValue);
		}
	}

	Scr_KillThread(localId);
	RemoveRefToObject(localId);
	MT_Free(stackValue, stackValue->bufLen);
}

void VM_TrimStack(unsigned int startLocalId, VariableStackBuffer *stackValue, bool fromEndon)
{
	unsigned int NewVariable;
	VariableValue localVariable;
	unsigned char type;
	int size;
	char *buf;
	VariableUnion tempValue;
	unsigned int parentLocalId;
	unsigned int localId;
	bool bFromEndon;

	bFromEndon = fromEndon;
	size = stackValue->size;
	localId = stackValue->localId;
	buf = &stackValue->buf[5 * size];

	while ( size )
	{
		buf -= 4;
		tempValue.intValue = *(int *)buf--;
		type = *buf;
		--size;

		if ( type == VAR_CODEPOS )
		{
			if ( FindObjectVariable(scrVarPub.pauseArrayId, localId) )
			{
				++size;
				stackValue->localId = localId;
				stackValue->size = size;
				Scr_StopThread(localId);

				if ( !bFromEndon )
				{
					Scr_SetThreadNotifyName(startLocalId, 0);
					stackValue->pos = 0;
					localVariable.type = VAR_STACK;
					localVariable.u.stackValue = stackValue;
					NewVariable = GetNewVariable(startLocalId, 0x1FFFFu);
					SetNewVariableValue(NewVariable, &localVariable);
				}

				return;
			}

			parentLocalId = GetParentLocalId(localId);
			Scr_KillThread(localId);
			RemoveRefToObject(localId);
			localId = parentLocalId;
		}
		else
		{
			RemoveRefToValueInternal(type, tempValue);
		}
	}

	if ( bFromEndon )
		RemoveVariable(startLocalId, 0x1FFFFu);

	Scr_KillThread(startLocalId);
	RemoveRefToObject(startLocalId);
	MT_Free(stackValue, stackValue->bufLen);
}

void Scr_CancelWaittill(unsigned int startLocalId)
{
	unsigned int localId;
	unsigned int parentId;
	VariableValueInternal_u *adr;
	unsigned int selfNameId;
	unsigned int self;

	self = Scr_GetSelf(startLocalId);
	localId = FindObjectVariable(scrVarPub.pauseArrayId, self);
	selfNameId = FindObject(localId);
	parentId = FindObjectVariable(selfNameId, startLocalId);
	adr = GetVariableValueAddress(parentId);
	VM_CancelNotify(adr->u.intValue, startLocalId);
	RemoveObjectVariable(selfNameId, startLocalId);

	if ( !GetArraySize(selfNameId) )
		RemoveObjectVariable(scrVarPub.pauseArrayId, self);
}

void Scr_CancelNotifyList(unsigned int notifyListOwnerId)
{
	VariableStackBuffer *stackBuf;
	unsigned int localId;
	unsigned int selfId;
	VariableStackBuffer *stackValue;
	unsigned int startLocalId;
	unsigned int id;
	unsigned int parentId;
	unsigned int notifyNameListId;
	unsigned int arrayId;
	int stackId;
	int parentLocalId;

	while ( 1 )
	{
		id = FindVariable(notifyListOwnerId, 0x1FFFEu);

		if ( !id )
			break;

		parentId = FindObject(id);
		notifyNameListId = FindNextSibling(parentId);

		if ( !notifyNameListId )
			break;

		arrayId = FindObject(notifyNameListId);
		stackId = FindNextSibling(arrayId);

		if ( !stackId )
			break;

		startLocalId = GetVariableKeyObject(stackId);

		if ( GetObjectType(stackId) == VAR_STACK )
		{
			stackValue = GetVariableValueAddress(stackId)->u.stackValue;
			Scr_CancelWaittill(startLocalId);
			VM_TrimStack(startLocalId, stackValue, 0);
		}
		else
		{
			AddRefToObject(startLocalId);
			Scr_CancelWaittill(startLocalId);
			selfId = Scr_GetSelf(startLocalId);
			localId = GetStartLocalId(selfId);
			parentLocalId = FindVariable(localId, 0x1FFFFu);

			if ( parentLocalId )
			{
				stackBuf = GetVariableValueAddress(parentLocalId)->u.stackValue;
				VM_TrimStack(localId, stackBuf, 1);
			}

			Scr_KillEndonThread(startLocalId);
			RemoveRefToEmptyObject(startLocalId);
		}
	}
}

void VM_TerminateTime(unsigned int timeId)
{
	VariableStackBuffer *stackValue;
	unsigned int startLocalId;
	unsigned int stackId;

	AddRefToObject(timeId);

	while ( 1 )
	{
		stackId = FindNextSibling(timeId);

		if ( !stackId )
			break;

		startLocalId = GetVariableKeyObject(stackId);
		stackValue = GetVariableValueAddress(stackId)->u.stackValue;
		RemoveObjectVariable(timeId, startLocalId);
		Scr_ClearWaitTime(startLocalId);
		VM_TerminateStack(startLocalId, startLocalId, stackValue);
	}

	RemoveRefToObject(timeId);
}

void Scr_TerminateWaittillThread(unsigned int localId, unsigned int startLocalId)
{
	unsigned short ThreadNotifyName;
	unsigned int object;
	unsigned int ObjectVariable;
	unsigned int variable;
	unsigned int var;
	VariableStackBuffer *stackValue;
	unsigned int stackId;
	unsigned int var2;
	unsigned int notifyListId;
	unsigned int notifyNameListId;
	unsigned int notifyListOwnerId;
	unsigned int stringValue;
	unsigned int selfNameId;
	unsigned int selfId;

	ThreadNotifyName = Scr_GetThreadNotifyName(startLocalId);
	stringValue = ThreadNotifyName;

	if ( ThreadNotifyName )
	{
		selfId = Scr_GetSelf(startLocalId);
		object = FindObjectVariable(scrVarPub.pauseArrayId, selfId);
		selfNameId = FindObject(object);
		ObjectVariable = FindObjectVariable(selfNameId, startLocalId);
		notifyListOwnerId = GetVariableValueAddress(ObjectVariable)->u.pointerValue;
		variable = FindVariable(notifyListOwnerId, 0x1FFFEu);
		notifyListId = FindObject(variable);
		var = FindVariable(notifyListId, stringValue);
		notifyNameListId = FindObject(var);
		stackId = FindObjectVariable(notifyNameListId, startLocalId);
		stackValue = GetVariableValueAddress(stackId)->u.stackValue;
		VM_CancelNotifyInternal(notifyListOwnerId, startLocalId, notifyListId, notifyNameListId, stringValue);
		RemoveObjectVariable(selfNameId, startLocalId);

		if ( !GetArraySize(selfNameId) )
			RemoveObjectVariable(scrVarPub.pauseArrayId, selfId);
	}
	else
	{
		var2 = FindVariable(startLocalId, 0x1FFFFu);
		stackValue = GetVariableValueAddress(var2)->u.stackValue;
		RemoveVariable(startLocalId, 0x1FFFFu);
	}

	VM_TerminateStack(localId, startLocalId, stackValue);
}

void Scr_TerminateWaitThread(unsigned int localId, unsigned int startLocalId)
{
	unsigned int variable;
	VariableStackBuffer *stackValue;
	unsigned int object;
	unsigned int time;
	unsigned int parentId;

	time = Scr_GetThreadWaitTime(startLocalId);
	Scr_ClearWaitTime(startLocalId);
	variable = FindVariable(scrVarPub.timeArrayId, time);
	parentId = FindObject(variable);
	object = FindObjectVariable(parentId, startLocalId);
	stackValue = GetVariableValueAddress(object)->u.stackValue;
	RemoveObjectVariable(parentId, startLocalId);

	if ( !GetArraySize(parentId) && time != scrVarPub.time )
		RemoveVariable(scrVarPub.timeArrayId, time);

	VM_TerminateStack(localId, startLocalId, stackValue);
}

void Scr_TerminateRunningThread(unsigned int localId)
{
	unsigned int id;
	int count;
	int current;

	current = scrVmPub.function_count;
	count = scrVmPub.function_count;

	while ( 1 )
	{
		id = scrVmPub.function_frame_start[current].fs.localId;

		if ( id == localId )
			break;

		--current;

		if ( !GetSafeParentLocalId(id) )
			count = current;
	}

	while ( count >= current )
		scrVmPub.function_frame_start[count--].fs.pos = &g_EndPos;
}

void Scr_TerminateThread(unsigned int localId)
{
	int type;
	int startLocalId;

	startLocalId = GetStartLocalId(localId);
	type = GetObjectType(startLocalId);

	if ( type == VAR_NOTIFY_THREAD )
	{
		Scr_TerminateWaittillThread(localId, startLocalId);
	}
	else if ( type > VAR_NOTIFY_THREAD )
	{
		if ( type == VAR_TIME_THREAD )
			Scr_TerminateWaitThread(localId, startLocalId);
	}
	else if ( type == VAR_THREAD )
	{
		Scr_TerminateRunningThread(localId);
	}
}

unsigned short Scr_ExecThread(int callbackHook, unsigned int numArgs)
{
	unsigned int self;
	const char *codePos;
	unsigned short callback;

	codePos = &scrVarPub.programBuffer[callbackHook];

	if ( !scrVmPub.function_count )
		Scr_ResetTimeout();

	Scr_IsInOpcodeMemory(codePos);
	AddRefToObject(scrVarPub.levelId);
	self = AllocThread(scrVarPub.levelId);
	callback = VM_Execute(self, codePos, numArgs);
	RemoveRefToValue(scrVmPub.top);
	scrVmPub.top->type = VAR_UNDEFINED;
	--scrVmPub.top;
	--scrVmPub.inparamcount;

	return callback;
}

void Scr_AddExecThread(int handle, unsigned int paramcount)
{
	unsigned int self;
	unsigned int callback;
	const char *codePos;

	codePos = &scrVarPub.programBuffer[handle];

	if ( !scrVmPub.function_count )
		Scr_ResetTimeout();

	AddRefToObject(scrVarPub.levelId);
	self = AllocThread(scrVarPub.levelId);
	callback = VM_Execute(self, codePos, paramcount);
	RemoveRefToObject(callback);
	++scrVmPub.outparamcount;
	--scrVmPub.inparamcount;
}

int Scr_AddLocalVars(unsigned int localId)
{
	int count;
	unsigned int fieldIndex;

	count = 0;

	for ( fieldIndex = FindLastSibling(localId); fieldIndex; fieldIndex = FindLastSibling(fieldIndex) )
	{
		*++scrVmPub.localVars = fieldIndex;
		++count;
	}

	return count;
}

void VM_UnarchiveStack(unsigned int startLocalId, function_stack_t *stack, VariableStackBuffer *stackValue)
{
	int function_count;
	unsigned int localId;
	VariableValue *startTop;
	int size;
	const char *buf;
	const char *pos;

	scrVmPub.function_frame->fs.pos = stackValue->pos;
	++scrVmPub.function_count;
	++scrVmPub.function_frame;
	size = stackValue->size;
	buf = stackValue->buf;
	startTop = stack->startTop;

	while ( size )
	{
		++startTop;
		--size;
		startTop->type = *(unsigned char *)buf;
		pos = buf + 1;

		if ( startTop->type == VAR_CODEPOS )
		{
			scrVmPub.function_frame->fs.pos = *(const char **)pos;
			++scrVmPub.function_count;
			++scrVmPub.function_frame;
		}
		else
		{
			startTop->u.intValue = *(int *)pos;
		}

		buf = pos + 4;
	}

	stack->pos = stackValue->pos;
	stack->top = startTop;
	localId = stackValue->localId;
	stack->localId = localId;
	Scr_ClearWaitTime(startLocalId);
	function_count = scrVmPub.function_count;

	while ( 1 )
	{
		scrVmPub.function_frame_start[function_count--].fs.localId = localId;

		if ( !function_count )
			break;

		localId = GetParentLocalId(localId);
	}

	while ( ++function_count != scrVmPub.function_count )
	{
		scrVmPub.function_frame_start[function_count].fs.localVarCount = Scr_AddLocalVars(scrVmPub.function_frame_start[function_count].fs.localId);
	}

	stack->localVarCount = Scr_AddLocalVars(stack->localId);

	if ( stackValue->time != LOBYTE(scrVarPub.time) )
		Scr_ResetTimeout();

	MT_Free(stackValue, stackValue->bufLen);
}

void VM_Resume(unsigned int timeId)
{
	unsigned int id;
	VariableStackBuffer *stackBuf;
	unsigned int parentId;
	unsigned int localId;
	function_stack_t stack;

	Scr_ResetTimeout();
	AddRefToObject(timeId);

	for ( stack.startTop = scrVmPub.stack; ; RemoveRefToValue(stack.startTop + 1) )
	{
		localId = FindNextSibling(timeId);

		if ( !localId )
			break;

		parentId = GetVariableKeyObject(localId);
		stackBuf = GetVariableValueAddress(localId)->u.stackValue;
		RemoveObjectVariable(timeId, parentId);
		VM_UnarchiveStack(parentId, &stack, stackBuf);
		id = VM_ExecuteInternal(stack.pos, stack.localId, stack.localVarCount, stack.top, stack.startTop);
		RemoveRefToObject(id);
	}

	RemoveRefToObject(timeId);
	ClearVariableValue(scrVarPub.tempVariable);
	scrVmPub.top = scrVmPub.stack;
}

void VM_Notify(unsigned int notifyListOwnerId, unsigned int stringValue, VariableValue *top)
{
	unsigned int localId;
	int type;
	unsigned int variable;
	unsigned int array;
	unsigned int newObject;
	VariableValue stackValue;
	VariableValue value;
	VariableValue value2;
	bool bNoStack;
	int notifyListEntry;
	unsigned int selfNameId;
	unsigned int selfId;
	int bufLen;
	size_t len;
	int newSize;
	int size;
	char *buf;
	VariableStackBuffer *stackBuf;
	VariableStackBuffer *newStackBuf;
	VariableValueInternal_u *stackId;
	VariableValue *vars;
	unsigned int startLocalId;
	unsigned int notifyListId;
	unsigned int notifyNameListId;

	notifyListId = FindVariable(notifyListOwnerId, 0x1FFFEu);

	if ( notifyListId )
	{
		notifyListId = FindObject(notifyListId);
		notifyNameListId = FindVariable(notifyListId, stringValue);
		if ( notifyNameListId )
		{
			notifyNameListId = FindObject(notifyNameListId);
			AddRefToObject(notifyNameListId);
			scrVarPub.evaluate = 1;
			notifyListEntry = notifyNameListId;
next:
			while ( 1 )
			{
				notifyListEntry = FindLastSibling(notifyListEntry);
				if ( !notifyListEntry )
					break;
				startLocalId = GetVariableKeyObject(notifyListEntry);
				selfId = Scr_GetSelf(startLocalId);
				localId = FindObjectVariable(scrVarPub.pauseArrayId, selfId);
				selfNameId = FindObject(localId);
				if ( GetObjectType(notifyListEntry) )
				{
					stackId = GetVariableValueAddress(notifyListEntry);
					newStackBuf = stackId->u.stackValue;
					if ( *((byte *)newStackBuf->pos - 1) == OP_waittillmatch )
					{
						size = *newStackBuf->pos;
						buf = &newStackBuf->buf[5 * (newStackBuf->size - size)];
						for ( vars = top; size; --vars )
						{
							if ( vars->type == VAR_PRECODEPOS )
								goto next;
							--size;
							value.type = (unsigned char)*buf;
							if ( value.type == VAR_PRECODEPOS )
								break;
							value.u.intValue = *(int *)++buf;
							buf += 4;
							AddRefToValue(&value);
							type = vars->type;
							value2.u.intValue = vars->u.intValue;
							value2.type = type;
							AddRefToValue(&value2);
							Scr_EvalEquality(&value, &value2);
							if ( scrVarPub.error_message )
							{
								scriptError(
								    newStackBuf->pos,
								    *newStackBuf->pos - size + 3,
								    scrVarPub.error_message,
								    scrVmGlob.dialog_error_message);
								Scr_ClearErrorMessage();
								goto next;
							}
							if ( !value.u.intValue )
								goto next;
						}
						++newStackBuf->pos;
						bNoStack = 1;
					}
					else
					{
						bNoStack = top->type == VAR_PRECODEPOS;
					}
					stackValue.type = VAR_STACK;
					stackValue.u.stackValue = newStackBuf;
					variable = GetVariable(scrVarPub.timeArrayId, scrVarPub.time);
					array = GetArray(variable);
					newObject = GetNewObjectVariable(array, startLocalId);
					SetNewVariableValue(newObject, &stackValue);
					stackId = GetVariableValueAddress(newObject);
					VM_CancelNotifyInternal(notifyListOwnerId, startLocalId, notifyListId, notifyNameListId, stringValue);
					RemoveObjectVariable(selfNameId, startLocalId);
					if ( !GetArraySize(selfNameId) )
						RemoveObjectVariable(scrVarPub.pauseArrayId, selfId);
					Scr_SetThreadWaitTime(startLocalId, scrVarPub.time);
					if ( bNoStack )
					{
						notifyListEntry = notifyNameListId;
					}
					else
					{
						size = newStackBuf->size;
						newSize = size;
						vars = top;
						do
						{
							++newSize;
							--vars;
						}
						while ( vars->type != VAR_PRECODEPOS );
						len = 5 * size;
						bufLen = 5 * newSize + 11;
						if ( !MT_Realloc(newStackBuf->bufLen, bufLen) )
						{
							stackBuf = (VariableStackBuffer *)MT_Alloc(bufLen);
							stackBuf->bufLen = bufLen;
							stackBuf->pos = newStackBuf->pos;
							stackBuf->localId = newStackBuf->localId;
							memcpy(stackBuf->buf, newStackBuf->buf, len);
							MT_Free(newStackBuf, newStackBuf->bufLen);
							newStackBuf = stackBuf;
							stackId->u.stackValue = stackBuf;
						}
						newStackBuf->size = newSize;
						buf = &newStackBuf->buf[len];
						newSize -= size;
						do
						{
							AddRefToValue(++vars);
							*buf++ = vars->type;
							*(int *)buf = vars->u.intValue;
							buf += 4;
							--newSize;
						}
						while ( newSize );
						notifyListEntry = notifyNameListId;
					}
				}
				else
				{
					VM_CancelNotifyInternal(notifyListOwnerId, startLocalId, notifyListId, notifyNameListId, stringValue);
					Scr_KillEndonThread(startLocalId);
					RemoveObjectVariable(selfNameId, startLocalId);
					if ( !GetArraySize(selfNameId) )
						RemoveObjectVariable(scrVarPub.pauseArrayId, selfId);
					Scr_TerminateThread(selfId);
					notifyListEntry = notifyNameListId;
				}
			}
			RemoveRefToObject(notifyNameListId);
			scrVarPub.evaluate = 0;
		}
	}
}

void Scr_NotifyNum(int entnum, unsigned int classnum, unsigned int stringValue, unsigned int paramcount)
{
	int type;
	VariableValue *entryValue;
	unsigned int id;
	unsigned int params;

	Scr_ClearOutParams();
	entryValue = &scrVmPub.top[-paramcount];
	params = scrVmPub.inparamcount - paramcount;
	id = FindEntityId(entnum, classnum);

	if ( id )
	{
		type = entryValue->type;
		entryValue->type = VAR_PRECODEPOS;
		scrVmPub.inparamcount = 0;
		VM_Notify(id, stringValue, scrVmPub.top);
		entryValue->type = type;
	}

	while ( scrVmPub.top != entryValue )
	{
		RemoveRefToValue(scrVmPub.top);
		--scrVmPub.top;
	}

	scrVmPub.inparamcount = params;
}

void Scr_ResetTimeout()
{
	scrVmGlob.starttime = Sys_MilliSeconds();
}

void runtimeError(conChannel_t channel, const char *codePos, unsigned int index, const char *errorMessage)
{
	int i;

	Com_PrintMessage(channel, va("\n******* script runtime error *******\n%s: ", errorMessage));
	Scr_PrintPrevCodePos(channel, codePos, index);

	if ( scrVmPub.function_count )
	{
		for ( i = scrVmPub.function_count - 1; i > 0; --i )
		{
			Com_PrintMessage(channel, "called from:\n");
			Scr_PrintPrevCodePos(
			    channel,
			    scrVmPub.function_frame_start[i].fs.pos,
			    scrVmPub.function_frame_start[i].fs.localId == 0);
		}

		Com_PrintMessage(channel, "started from:\n");
		Scr_PrintPrevCodePos(channel, scrVmPub.function_frame_start[0].fs.pos, 1u);
	}

	Com_PrintMessage(channel, "************************************\n");
}

#ifdef LIBCOD
extern dvar_t *com_developer;
#endif

void scriptError(const char *codePos, unsigned int index, const char *errorMsg, const char *format)
{
	const char *line;
	const char *fmt;
	bool drop;

	if ( scrVarPub.developer || scrVmPub.terminal_error )
	{
		if ( scrVmPub.debugCode )
		{
			Com_Printf("%s\n", errorMsg);

			if ( scrVmPub.terminal_error )
				goto error;
		}
		else
		{
			drop = 0;

			if ( scrVmPub.abort_on_error || scrVmPub.terminal_error )
				drop = 1;

			if ( drop )
				runtimeError(CON_CHANNEL_DONT_FILTER, codePos, index, errorMsg);
			else
				runtimeError(CON_CHANNEL_LOGFILEONLY, codePos, index, errorMsg);
#ifdef LIBCOD
			if (com_developer->current.integer == 2)
				drop = 0;
#endif
			if ( drop )
			{
error:
				fmt = format;

				if ( !format )
					fmt = "";
				if ( format )
					line = "\n";
				else
					line = "";

				if ( scrVmPub.terminal_error )
					Com_Error(ERR_SCRIPT_DROP, "script runtime error\n(see console for details)\n", errorMsg, line, fmt);

				Com_Error(ERR_SCRIPT, "script runtime error\n(see console for details)\n", errorMsg, line, fmt);
			}
		}
	}
}

unsigned int VM_Execute(unsigned int localId, const char *pos, unsigned int numArgs)
{
	int varType;
	unsigned int parentId;
	VariableValue *startTop;
	unsigned int numparams;

	Scr_ClearOutParams();
	startTop = &scrVmPub.top[-numArgs];
	numparams = scrVmPub.inparamcount - numArgs;

	if ( scrVmPub.function_count > 29 )
	{
		Scr_KillThread(localId);
		scrVmPub.inparamcount = numparams + 1;

		while ( numparams )
		{
			RemoveRefToValue(scrVmPub.top);
			--scrVmPub.top;
			--numparams;
		}

		++scrVmPub.top;
		scrVmPub.top->type = VAR_UNDEFINED;
		scriptError(pos, 0, "script stack overflow (too many embedded function calls)", 0);

		return localId;
	}
	else
	{
		if ( scrVmPub.function_count )
		{
			++scrVmPub.function_count;
			++scrVmPub.function_frame;
			scrVmPub.function_frame->fs.localId = 0;
		}

		scrVmPub.function_frame->fs.pos = pos;
		++scrVmPub.function_count;
		++scrVmPub.function_frame;
		scrVmPub.function_frame->fs.localId = localId;
		varType = startTop->type;
		startTop->type = VAR_PRECODEPOS;
		scrVmPub.inparamcount = 0;
		parentId = VM_ExecuteInternal(pos, localId, 0, scrVmPub.top, startTop);
		startTop->type = varType;
		scrVmPub.top = startTop + 1;
		scrVmPub.inparamcount = numparams + 1;
		ClearVariableValue(scrVarPub.tempVariable);

		if ( scrVmPub.function_count )
		{
			--scrVmPub.function_count;
			--scrVmPub.function_frame;
		}

		return parentId;
	}
}

void VM_SetTime()
{
	unsigned int localId;
	unsigned int id;

	if ( scrVarPub.timeArrayId )
	{
		id = FindVariable(scrVarPub.timeArrayId, scrVarPub.time);

		if ( id )
		{
			localId = FindObject(id);
			VM_Resume(localId);
			SafeRemoveVariable(scrVarPub.timeArrayId, scrVarPub.time);
		}
	}
}

void Scr_RunCurrentThreads()
{
	VM_SetTime();
}

void Scr_Settings(int developer, int developer_script, int abort_on_error)
{
	scrVarPub.developer = developer != 0;
	scrVarPub.developer_script = developer_script != 0;
	scrVmPub.abort_on_error = abort_on_error != 0;
}

void Scr_TerminalError(const char *error)
{
	Scr_DumpScriptThreads();
	Scr_DumpScriptVariables();
	scrVmPub.terminal_error = 1;
	Scr_Error(error);
}

void Scr_Abort()
{
	scrVarPub.timeArrayId = 0;
	scrVarPub.bInited = 0;
}

void Scr_ShutdownSystem(unsigned char sys, qboolean bComplete)
{
	unsigned int timeId;
	unsigned int parentId;
	VariableValueInternal_u notifyListOwnerId;
	unsigned int id;
	unsigned int nextId;
	unsigned int localId;

	Scr_CompileShutdown();
	Scr_FreeEntityList();

	if ( scrVarPub.timeArrayId )
	{
		Scr_FreeGameVariable(bComplete);

		for ( id = FindNextSibling(scrVarPub.timeArrayId); id; id = FindNextSibling(id) )
		{
			timeId = FindObject(id);
			VM_TerminateTime(timeId);
		}

		while ( 1 )
		{
			nextId = FindNextSibling(scrVarPub.pauseArrayId);

			if ( !nextId )
				break;

			parentId = FindObject(nextId);
			localId = FindNextSibling(parentId);
			notifyListOwnerId = *GetVariableValueAddress(localId);
			AddRefToObject(notifyListOwnerId.u.pointerValue);
			Scr_CancelNotifyList(notifyListOwnerId.u.pointerValue);
			RemoveRefToObject(notifyListOwnerId.u.pointerValue);
		}

		ClearObject(scrVarPub.levelId);
		RemoveRefToEmptyObject(scrVarPub.levelId);
		scrVarPub.levelId = 0;
		ClearObject(scrVarPub.animId);
		RemoveRefToEmptyObject(scrVarPub.animId);
		scrVarPub.animId = 0;
		ClearObject(scrVarPub.timeArrayId);
		RemoveRefToEmptyObject(scrVarPub.timeArrayId);
		scrVarPub.timeArrayId = 0;
		RemoveRefToEmptyObject(scrVarPub.pauseArrayId);
		scrVarPub.pauseArrayId = 0;
		Scr_FreeObjects();
	}
}

void Scr_Shutdown()
{
	if ( scrVarPub.bInited )
	{
		scrVarPub.bInited = 0;
		Var_FreeTempVariables();
		Var_Shutdown();
		SL_Shutdown();
	}
}

void Scr_ClearErrorMessage()
{
	scrVarPub.error_message = 0;
	scrVmGlob.dialog_error_message = 0;
	scrVarPub.error_index = 0;
}

void Scr_ClearStrings()
{
	scrCompilePub.canonicalStrings = 0;
	Hunk_ClearHigh(scrVarPub.canonicalStrMark);
}

void Scr_AllocAnims(int num)
{
	scrAnimPub.animtree_loading = 1;
	scrAnimPub.xanim_num[num] = 0;
	scrAnimPub.xanim_lookup[num][0].anims = 0;
	scrAnimPub.animtrees = Scr_AllocArray();
	scrAnimPub.animtree_node = 0;
	scrCompilePub.developer_statement = 0;
}

void Scr_AllocStrings()
{
	scrVarPub.canonicalStrMark = Hunk_SetMark();
	scrCompilePub.canonicalStrings = (uint16_t *)Hunk_AllocInternal(0x20000u);
	scrVarPub.canonicalStrCount = 0;
}

void Scr_VM_Init()
{
	scrVmPub.maxstack = &scrVmPub.stack[2047];
	scrVmPub.top = scrVmPub.stack;
	scrVmPub.function_count = 0;
	scrVmPub.function_frame = scrVmPub.function_frame_start;
	scrVmPub.localVars = scrVmGlob.localVarsStack - 1;
	scrVarPub.evaluate = 0;
	scrVmPub.debugCode = 0;
	Scr_ClearErrorMessage();
	scrVmPub.terminal_error = 0;
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
	scrVmGlob.loading = 0;
}

void Scr_Init()
{
	if ( !scrVarPub.bInited )
	{
		SL_CheckInit();
		Var_Init();
		Scr_VM_Init();
		scrCompilePub.script_loading = 0;
		scrAnimPub.animtree_loading = 0;
		scrCompilePub.scriptsPos = 0;
		scrCompilePub.loadedscripts = 0;
		scrAnimPub.animtrees = 0;
		scrCompilePub.builtinMeth = 0;
		scrCompilePub.builtinFunc = 0;
		scrVarPub.bInited = 1;
	}
}