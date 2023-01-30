#include "../qcommon/qcommon.h"
#include "clientscript_public.h"

#ifdef TESTING_LIBRARY
#define scrVarPub (*((scrVarPub_t*)( 0x08394000 )))
#else
extern scrVarPub_t scrVarPub;
#endif

#ifdef TESTING_LIBRARY
#define scrAnimPub (*((scrAnimPub_t*)( 0x08202440 )))
#else
extern scrAnimPub_t scrAnimPub;
#endif

#ifdef TESTING_LIBRARY
#define scrCompilePub (*((scrCompilePub_t*)( 0x08202A40 )))
#else
extern scrCompilePub_t scrCompilePub;
#endif

#ifdef TESTING_LIBRARY
#define scrVmPub (*((scrVmPub_t*)( 0x083D7600 )))
#else
scrVmPub_t scrVmPub;
#endif

extern const char *var_typename[];

typedef struct scrVmGlob_s
{
	VariableValue eval_stack[2];
	const char *dialog_error_message;
	int loading;
	int starttime;
	unsigned int localVarsStack[2048];
} scrVmGlob_t;
static_assert((sizeof(scrVmGlob_t) == 0x201C), "ERROR: scrVmGlob_t size is invalid!");

#ifdef TESTING_LIBRARY
#define scrVmGlob (*((scrVmGlob_t*)( 0x083D5580 )))
#else
scrVmGlob_t scrVmGlob;
#endif

int g_script_error_level;
jmp_buf g_script_error[33];

void Scr_ErrorInternal()
{
	if ( !scrVarPub.evaluate && !scrCompilePub.script_loading )
	{
		if ( scrVarPub.developer && scrVmGlob.loading )
			scrVmPub.terminal_error = 1;

// !!! REMOVE ME WHEN COMPLETE
#ifndef TESTING_LIBRARY
		if ( scrVmPub.function_count || scrVmPub.debugCode )
			longjmp(g_script_error[g_script_error_level], -1);
#endif

		Com_Error(ERR_DROP, "%s", scrVarPub.error_message);
	}

	if ( scrVmPub.terminal_error )
		Com_Error(ERR_DROP, "%s", scrVarPub.error_message);
}

void Scr_Error(const char *error)
{
	if ( !scrVarPub.error_message )
		scrVarPub.error_message = error;

	Scr_ErrorInternal();
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

void Scr_AddStruct()
{
	unsigned int id;

	id = AllocObject();
	Scr_AddObject(id);
	RemoveRefToObject(id);
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

	return GetVarType(scrVmPub.top[-index].u.intValue);
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

			if ( GetVarType(entryValue->u.pointerValue) == VAR_ENTITY )
			{
				Scr_GetEntityIdRef(entRef, entId);
				return;
			}

			scrVarPub.error_index = index + 1;
			Scr_Error(va("type %s is not an entity", var_typename[GetVarType(entId)]));
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
	unsigned int objectIndex;
	unsigned int objectId;
	unsigned int parentId;
	unsigned int name;

	varIndex = FindVariable(notifyListOwnerId, 0x1FFFEu);
	parentId = FindObject(varIndex);
	name = Scr_GetThreadNotifyName(startLocalId);
	objectIndex = FindVariable(parentId, name);
	objectId = FindObject(objectIndex);
	VM_CancelNotifyInternal(notifyListOwnerId, startLocalId, parentId, objectId, name);
}

/*
unsigned int VM_ExecuteInternal(const char *pos, unsigned int localId, unsigned int varCount, VariableValue *top, VariableValue *startTop)
{
	unsigned int opcode;
	unsigned int parentIdEnd;
	unsigned int parentIdReturn;
	VariableValue *nextVariable;
	unsigned int levelObject;
	unsigned int animObject;
	unsigned int gameRef;
	unsigned int arrayRef;
	VariableValue localVariable;
	byte localPos;

	if ( setjmp(g_script_error[++g_script_error_level]) )
		goto LABEL_271;

	while (1)
	{
		opcode = *(byte *)pos++;

		switch ( opcode )
		{
		case OP_End:
			parentIdEnd = GetSafeParentLocalId(localId);
			Scr_KillThread(localId);
			scrVmPub.localVars -= varCount;
			while ( top->type != VAR_CODEPOS )
				RemoveRefToValue(top--);
			--scrVmPub.function_count;
			--scrVmPub.function_frame;
			if ( !parentIdEnd )
				goto LABEL_254;
			top->type = VAR_UNDEFINED;
			RemoveRefToObject(localId);
			pos = scrVmPub.function_frame->fs.pos;
			varCount = scrVmPub.function_frame->fs.localVarCount;
			localId = parentIdEnd;
			continue;

		case OP_Return:
			parentIdReturn = GetSafeParentLocalId(localId);
			Scr_KillThread(localId);
			scrVmPub.localVars -= varCount;
			localVariable.u.codePosValue = top->u.codePosValue;
			localVariable.type = top->type;
			for ( --top; top->type != VAR_CODEPOS; --top )
				RemoveRefToValue(top);
			--scrVmPub.function_count;
			--scrVmPub.function_frame;
			if ( !parentIdReturn )
			{
				nextVariable = top + 1;
				nextVariable->u.codePosValue = localVariable.u.codePosValue;
				nextVariable->type = localVariable.type;
				goto LABEL_255;
			}
			top->u.codePosValue = localVariable.u.codePosValue;
			top->type = localVariable.type;
			RemoveRefToObject(localId);
			pos = scrVmPub.function_frame->fs.pos;
			varCount = scrVmPub.function_frame->fs.localVarCount;
			localId = parentIdReturn;
			continue;

		case OP_GetUndefined:
			++top;
			top->type = VAR_UNDEFINED;
			continue;

		case OP_GetZero:
			++top;
			top->type = VAR_INTEGER;
			top->u.intValue = 0;
			continue;

		case OP_GetByte:
			++top;
			top->type = VAR_INTEGER;
			top->u.intValue = *(byte *)pos++;
			continue;

		case OP_GetNegByte:
			++top;
			top->type = VAR_INTEGER;
			top->u.intValue = -*(byte *)pos++;
			continue;

		case OP_GetUnsignedShort:
			++top;
			top->type = VAR_INTEGER;
			top->u.intValue = *(unsigned short *)pos++;
			continue;

		case OP_GetNegUnsignedShort:
			++top;
			top->type = VAR_INTEGER;
			top->u.intValue = -*(unsigned short *)pos++;
			continue;

		case OP_GetInteger:
			++top;
			top->type = VAR_INTEGER;
			top->u.intValue = *(int *)pos++;
			continue;

		case OP_GetFloat:
			++top;
			top->type = VAR_FLOAT;
			top->u.floatValue = *(float *)pos++;
			continue;

		case OP_GetString:
			++top;
			top->type = VAR_STRING;
			top->u.stringValue = *(unsigned short *)pos++;
			SL_AddRefToString(top->u.stringValue);
			continue;

		case OP_GetIString:
			++top;
			top->type = VAR_ISTRING;
			top->u.stringValue = *(unsigned short *)pos++;
			SL_AddRefToString(top->u.stringValue);
			continue;

		case OP_GetVector:
			++top;
			top->type = VAR_VECTOR;
			*(float *)pos += sizeof(vec3_t);
			top->u.vectorValue = (float *)pos;
			continue;

		case OP_GetLevelObject:
			levelObject = scrVarPub.levelId;
			continue;

		case OP_GetAnimObject:
			animObject = scrVarPub.animId;
			continue;

		case OP_GetSelf:
			++top;
			top->type = VAR_OBJECT;
			top->u.pointerValue = Scr_GetSelf(localId);
			AddRefToObject(top->u.pointerValue);
			continue;

		case OP_GetLevel:
			++top;
			top->type = VAR_OBJECT;
			top->u.pointerValue = scrVarPub.levelId;
			AddRefToObject(scrVarPub.levelId);
			continue;

		case OP_GetGame:
			++top;
			Scr_EvalVariable(&localVariable, scrVarPub.gameId);
			top->u = localVariable.u;
			top->type = localVariable.type;
			continue;

		case OP_GetAnim:
			++top;
			top->type = VAR_OBJECT;
			top->u.pointerValue = scrVarPub.animId;
			AddRefToObject(scrVarPub.animId);
			continue;

		case OP_GetAnimation:
			++top;
			top->type = VAR_ANIMATION;
			top->u.intValue = *(int *)pos++;
			continue;

		case OP_GetGameRef:
			gameRef = scrVarPub.gameId;
			continue;

		case OP_GetFunction:
			++top;
			top->type = VAR_FUNCTION;
			top->u.intValue = *(int *)pos++;
			continue;

		case OP_CreateLocalVariable:
			++scrVmPub.localVars;
			++varCount;
			*scrVmPub.localVars = GetNewVariable(localId, *(unsigned short *)pos++);
			continue;

		case OP_RemoveLocalVariables:
			localPos = *pos++;
			scrVmPub.localVars -= localPos;
			varCount -= localPos;
			while ( localPos )
			{
				RemoveNextVariable(localId);
				--localPos;
			}
			continue;

		case OP_EvalLocalVariableCached0:
			++top;
			Scr_EvalVariable(&localVariable, *scrVmPub.localVars);
			top->u = localVariable.u;
			top->type = localVariable.type;
			continue;

		case OP_EvalLocalVariableCached1:
			++top;
			Scr_EvalVariable(&localVariable, *(scrVmPub.localVars - 1));
			top->u = localVariable.u;
			top->type = localVariable.type;
			continue;

		case OP_EvalLocalVariableCached2:
			++top;
			Scr_EvalVariable(&localVariable, *(scrVmPub.localVars - 2));
			top->u = localVariable.u;
			top->type = localVariable.type;
			continue;

		case OP_EvalLocalVariableCached3:
			++top;
			Scr_EvalVariable(&localVariable, *(scrVmPub.localVars - 3));
			top->u = localVariable.u;
			top->type = localVariable.type;
			continue;

		case OP_EvalLocalVariableCached4:
			++top;
			Scr_EvalVariable(&localVariable, *(scrVmPub.localVars - 4));
			top->u = localVariable.u;
			top->type = localVariable.type;
			continue;

		case OP_EvalLocalVariableCached5:
			++top;
			Scr_EvalVariable(&localVariable, *(scrVmPub.localVars - 5));
			top->u = localVariable.u;
			top->type = localVariable.type;
			continue;

		case OP_EvalLocalVariableCached:
			++top;
			Scr_EvalVariable(&localVariable, scrVmPub.localVars[-(byte)*pos]);
			top->u = localVariable.u;
			top->type = localVariable.type;
			++pos;
			continue;

		case OP_EvalLocalArrayCached:
			++top;
			Scr_EvalVariable(&localVariable, scrVmPub.localVars[-(byte)*pos]);
			top->u = localVariable.u;
			top->type = localVariable.type;
			++pos;
			Scr_EvalArray(top, top - 1);
			continue;

		case OP_EvalArray:
			Scr_EvalArray(top, top - 1);
			--top;
			continue;

		case OP_EvalLocalArrayRefCached0:
			arrayRef = *scrVmPub.localVars;
			arrayRef = Scr_EvalArrayIndex(Scr_EvalArrayRef(arrayRef), top);
			--top;
			continue;
		}
	}
}
*/

void Scr_Settings(int developer, int developer_script, int abort_on_error)
{
	scrVarPub.developer = developer != 0;
	scrVarPub.developer_script = developer_script != 0;
	scrVmPub.abort_on_error = abort_on_error != 0;
}

void Scr_TerminalError(const char *error)
{
	UNIMPLEMENTED(__FUNCTION__);
}

void Scr_Abort()
{
	scrVarPub.timeArrayId = 0;
	scrVarPub.bInited = 0;
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