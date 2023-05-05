#include "../qcommon/qcommon.h"
#include "clientscript_public.h"

int g_script_error_level;
jmp_buf g_script_error[33];

void Scr_InitSystem()
{
	scrVarPub.timeArrayId = AllocObject();
	scrVarPub.pauseArrayId = Scr_AllocArray();
	scrVarPub.levelId = AllocObject();
	scrVarPub.animId = AllocObject();
	scrVarPub.time = 0;
	g_script_error_level = -1;
}

void Scr_ErrorInternal()
{
	if ( !scrVarPub.evaluate && !scrCompilePub.script_loading )
	{
		if ( scrVarPub.developer && scrVmGlob.loading )
			scrVmPub.terminal_error = 1;

		if ( scrVmPub.function_count || scrVmPub.debugCode )
			longjmp(g_script_error[g_script_error_level], -1);

		Com_Error(ERR_DROP, "%s", scrVarPub.error_message);
	}

	if ( scrVmPub.terminal_error )
		Com_Error(ERR_DROP, "%s", scrVarPub.error_message);
}

unsigned int QDECL VM_GetLocalVar(int a1)
{
	return scrVmPub.localVars[-a1];
}

int QDECL VM_IncShort(_WORD **a1)
{
	return (__int16)*(*a1)++;
}

int QDECL VM_GetShort(_WORD **a1)
{
	return (unsigned __int16)VM_IncShort(a1);
}

int QDECL VM_GetInt(_DWORD **a1)
{
	return *(*a1)++;
}

long double QDECL VM_GetFloat(_DWORD **a1)
{
	return *(float *)(*a1)++;
}

int QDECL VM_GetVector(int *a1)
{
	int v2;

	v2 = *a1;
	*a1 += 12;
	return v2;
}

int QDECL VM_AddInt(_DWORD **a1)
{
	return VM_GetInt(a1);
}

int QDECL VM_Floor(float a1)
{
	return (int)floor(a1 + 0.5);
}

void QDECL VM_AddIntSize(int *a1, int a2)
{
	*a1 += 4 * a2;
}

int QDECL VM_GetDword(_DWORD **a1)
{
	return VM_GetInt(a1);
}

unsigned int QDECL VM_AddLocalVar(unsigned __int8 *a1)
{
	return VM_GetLocalVar(*a1);
}

unsigned int QDECL VM_ExecuteInternal(const char *pos, unsigned int localId, unsigned int varCount, VariableValue *top, VariableValue *startTop)
{
	int type;
	VariableValue *v6;
	int v7;
	VariableValue *v8;
	int v9;
	VariableValue *v10;
	VariableValue *v11;
	VariableValue *v12;
	VariableValue *v13;
	VariableValue *v14;
	VariableValue *v15;
	VariableValue *v16;
	VariableValue *v17;
	VariableValue *v18;
	int v19;
	VariableValue *v20;
	VariableValue *v21;
	unsigned __int16 v22;
	VariableValue *v23;
	int v24;
	VariableValue *v25;
	unsigned int v26;
	int v27;
	VariableValue *v28;
	unsigned int v29;
	int v30;
	VariableValue *v31;
	unsigned int v32;
	int v33;
	VariableValue *v34;
	unsigned int v35;
	int v36;
	VariableValue *v37;
	unsigned int v38;
	int v39;
	VariableValue *v40;
	unsigned int v41;
	int v42;
	VariableValue *v43;
	unsigned int v44;
	int v45;
	unsigned int v46;
	VariableValue *v47;
	VariableValue *v48;
	unsigned __int16 v49;
	unsigned int Variable;
	int v51;
	VariableValue *v52;
	unsigned __int16 v53;
	int v54;
	unsigned __int16 v55;
	unsigned __int16 v56;
	unsigned __int16 v57;
	unsigned int v58;
	unsigned int v59;
	unsigned __int16 v60;
	unsigned int v61;
	unsigned __int16 v62;
	unsigned int v63;
	char *v64;
	char *v65;
	char *v66;
	unsigned int v67;
	unsigned int v68;
	char *v69;
	char *v70;
	int v71;
	int v72;
	char *v73;
	int v74;
	int v75;
	char *v76;
	unsigned int v77;
	VariableValue *v78;
	int v79;
	char *v80;
	VariableValue *v81;
	int v82;
	char *v83;
	unsigned int v84;
	unsigned int Array;
	unsigned int v86;
	int v87;
	unsigned int ObjectVariable;
	unsigned int v89;
	unsigned int NewObjectVariable;
	unsigned int v91;
	unsigned int v92;
	unsigned int v93;
	unsigned int v94;
	unsigned int v95;
	unsigned int v96;
	unsigned int v97;
	char *v98;
	char *v99;
	VariableValue *v100;
	VariableValue *v101;
	VariableValue *v102;
	unsigned __int16 v103;
	unsigned int v104;
	int v105;
	unsigned __int16 v106;
	char *v107;
	float v109;
	int v110;
	VariableValue v112;
	VariableValue v113;
	VariableValue v114;
	scr_entref_t entRef;
	VariableValue pValue;
	VariableValue v117;
	VariableValue v118;
	VariableValue v119;
	VariableValue v120;
	VariableValue v121;
	VariableValue v122;
	VariableValue v123;
	VariableValue v124;
	VariableValue v125;
	VariableValue val;
	int v127;
	unsigned int id;
	VariableValue v129;
	unsigned int v130;
	const char *v131;
	int v132;
	int entnum;
	unsigned int classnum;
	int v135;
	unsigned __int8 localPos;
	unsigned int InternalVariableIndex;
	int v138;
	int type2;
	unsigned int notifyListOwnerId;
	unsigned int parentId;
	int refCount;
	int opcode;
	unsigned int startLocalId;
	const char *prevCodePos;
	unsigned int name;
	unsigned int waitTime;
	scr_entref_t entRef2;
	VariableValue currentVariable;
	unsigned int paramcount;
	unsigned int self;
	unsigned int objectRef;
	unsigned int evalRef;

	refCount = 0;
	if ( setjmp(g_script_error[++g_script_error_level]) )
		goto LABEL_271;
	while ( 2 )
	{
		opcode = *(unsigned __int8 *)pos++;
		switch ( opcode )
		{
		case OP_End:
			parentId = GetSafeParentLocalId(localId);
			Scr_KillThread(localId);
			scrVmPub.localVars -= varCount;
			while ( top->type != VAR_CODEPOS )
				RemoveRefToValue(top--);
			--scrVmPub.function_count;
			--scrVmPub.function_frame;
			if ( !parentId )
				goto LABEL_254;
			top->type = VAR_UNDEFINED;
			goto LABEL_10;
		case OP_Return:
			parentId = GetSafeParentLocalId(localId);
			Scr_KillThread(localId);
			scrVmPub.localVars -= varCount;
			type = top->type;
			currentVariable.u.intValue = top->u.intValue;
			currentVariable.type = type;
			for ( --top; top->type != VAR_CODEPOS; --top )
				RemoveRefToValue(top);
			--scrVmPub.function_count;
			--scrVmPub.function_frame;
			if ( !parentId )
			{
				v6 = top + 1;
				v7 = currentVariable.type;
				top[1].u.intValue = currentVariable.u.intValue;
				v6->type = v7;
				goto LABEL_255;
			}
			v8 = top;
			v9 = currentVariable.type;
			top->u.intValue = currentVariable.u.intValue;
			v8->type = v9;
LABEL_10:
			RemoveRefToObject(localId);
			pos = scrVmPub.function_frame->fs.pos;
			varCount = scrVmPub.function_frame->fs.localVarCount;
			localId = parentId;
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
			top->u.intValue = *(unsigned __int8 *)pos++;
			continue;
		case OP_GetNegByte:
			++top;
			top->type = VAR_INTEGER;
			top->u.intValue = -*(unsigned __int8 *)pos++;
			continue;
		case OP_GetUnsignedShort:
			++top;
			top->type = VAR_INTEGER;
			v10 = top;
			v10->u.intValue = (unsigned __int16)VM_GetShort((_WORD **)&pos);
			continue;
		case OP_GetNegUnsignedShort:
			++top;
			top->type = VAR_INTEGER;
			v11 = top;
			v11->u.intValue = -(unsigned __int16)VM_GetShort((_WORD **)&pos);
			continue;
		case OP_GetInteger:
			++top;
			top->type = VAR_INTEGER;
			v12 = top;
			v12->u.intValue = VM_GetInt((_DWORD **)&pos);
			continue;
		case OP_GetFloat:
			++top;
			top->type = VAR_FLOAT;
			v13 = top;
			v13->u.floatValue = VM_GetFloat((_DWORD **)&pos);
			continue;
		case OP_GetString:
			++top;
			top->type = VAR_STRING;
			v14 = top;
			v14->u.intValue = (unsigned __int16)VM_GetShort((_WORD **)&pos);
			SL_AddRefToString(top->u.intValue);
			continue;
		case OP_GetIString:
			++top;
			top->type = VAR_ISTRING;
			v15 = top;
			v15->u.intValue = (unsigned __int16)VM_GetShort((_WORD **)&pos);
			SL_AddRefToString(top->u.intValue);
			continue;
		case OP_GetVector:
			++top;
			top->type = VAR_VECTOR;
			v16 = top;
			v16->u.intValue = VM_GetVector((int *)&pos);
			continue;
		case OP_GetLevelObject:
			objectRef = scrVarPub.levelId;
			continue;
		case OP_GetAnimObject:
			objectRef = scrVarPub.animId;
			continue;
		case OP_GetSelf:
			++top;
			top->type = VAR_OBJECT;
			v17 = top;
			v17->u.intValue = Scr_GetSelf(localId);
			AddRefToObject(top->u.intValue);
			continue;
		case OP_GetLevel:
			++top;
			top->type = VAR_OBJECT;
			top->u.intValue = scrVarPub.levelId;
			AddRefToObject(scrVarPub.levelId);
			continue;
		case OP_GetGame:
			v18 = ++top;
			Scr_EvalVariable(&val, scrVarPub.gameId);
			v19 = val.type;
			v18->u.intValue = val.u.intValue;
			v18->type = v19;
			continue;
		case OP_GetAnim:
			++top;
			top->type = VAR_OBJECT;
			top->u.intValue = scrVarPub.animId;
			AddRefToObject(scrVarPub.animId);
			continue;
		case OP_GetAnimation:
			++top;
			top->type = VAR_ANIMATION;
			v20 = top;
			v20->u.intValue = VM_GetInt((_DWORD **)&pos);
			continue;
		case OP_GetGameRef:
			evalRef = scrVarPub.gameId;
			continue;
		case OP_GetFunction:
			++top;
			top->type = VAR_FUNCTION;
			v21 = top;
			v21->u.intValue = VM_AddInt((_DWORD **)&pos);
			continue;
		case OP_CreateLocalVariable:
			++scrVmPub.localVars;
			++varCount;
			v22 = VM_GetShort((_WORD **)&pos);
			*scrVmPub.localVars = GetNewVariable(localId, v22);
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
			v23 = ++top;
			Scr_EvalVariable(&v125, *scrVmPub.localVars);
			v24 = v125.type;
			v23->u.intValue = v125.u.intValue;
			v23->type = v24;
			continue;
		case OP_EvalLocalVariableCached1:
			v25 = ++top;
			v26 = VM_GetLocalVar(1);
			Scr_EvalVariable(&v124, v26);
			v27 = v124.type;
			v25->u.intValue = v124.u.intValue;
			v25->type = v27;
			continue;
		case OP_EvalLocalVariableCached2:
			v28 = ++top;
			v29 = VM_GetLocalVar(2);
			Scr_EvalVariable(&v123, v29);
			v30 = v123.type;
			v28->u.intValue = v123.u.intValue;
			v28->type = v30;
			continue;
		case OP_EvalLocalVariableCached3:
			v31 = ++top;
			v32 = VM_GetLocalVar(3);
			Scr_EvalVariable(&v122, v32);
			v33 = v122.type;
			v31->u.intValue = v122.u.intValue;
			v31->type = v33;
			continue;
		case OP_EvalLocalVariableCached4:
			v34 = ++top;
			v35 = VM_GetLocalVar(4);
			Scr_EvalVariable(&v121, v35);
			v36 = v121.type;
			v34->u.intValue = v121.u.intValue;
			v34->type = v36;
			continue;
		case OP_EvalLocalVariableCached5:
			v37 = ++top;
			v38 = VM_GetLocalVar(5);
			Scr_EvalVariable(&v120, v38);
			v39 = v120.type;
			v37->u.intValue = v120.u.intValue;
			v37->type = v39;
			continue;
		case OP_EvalLocalVariableCached:
			v40 = ++top;
			v41 = VM_AddLocalVar((unsigned __int8 *)pos);
			Scr_EvalVariable(&v119, v41);
			v42 = v119.type;
			v40->u.intValue = v119.u.intValue;
			v40->type = v42;
			++pos;
			continue;
		case OP_EvalLocalArrayCached:
			v43 = ++top;
			v44 = VM_AddLocalVar((unsigned __int8 *)pos);
			Scr_EvalVariable(&v118, v44);
			v45 = v118.type;
			v43->u.intValue = v118.u.intValue;
			v43->type = v45;
			++pos;
			goto LABEL_49;
		case OP_EvalArray:
LABEL_49:
			Scr_EvalArray(top, top - 1);
			goto LABEL_3;
		case OP_EvalLocalArrayRefCached0:
			evalRef = *scrVmPub.localVars;
			goto LABEL_52;
		case OP_EvalLocalArrayRefCached:
			evalRef = VM_AddLocalVar((unsigned __int8 *)pos++);
			goto LABEL_52;
		case OP_EvalArrayRef:
LABEL_52:
			v46 = Scr_EvalArrayRef(evalRef);
			evalRef = Scr_EvalArrayIndex(v46, top);
			goto LABEL_3;
		case OP_ClearArray:
			ClearArray(evalRef, top);
			goto LABEL_3;
		case OP_EmptyArray:
			++top;
			top->type = VAR_OBJECT;
			v47 = top;
			v47->u.intValue = Scr_AllocArray();
			continue;
		case OP_GetSelfObject:
			objectRef = Scr_GetSelf(localId);
			if ( IsFieldObject(objectRef) )
				continue;
			goto LABEL_269;
		case OP_EvalLevelFieldVariable:
			objectRef = scrVarPub.levelId;
			goto LABEL_58;
		case OP_EvalAnimFieldVariable:
			objectRef = scrVarPub.animId;
LABEL_58:
			v48 = ++top;
			v49 = VM_GetShort((_WORD **)&pos);
			Variable = FindVariable(objectRef, v49);
			Scr_EvalVariable(&v117, Variable);
			v51 = v117.type;
			v48->u.intValue = v117.u.intValue;
			v48->type = v51;
			continue;
		case OP_EvalSelfFieldVariable:
			objectRef = Scr_GetSelf(localId);
			if ( IsFieldObject(objectRef) )
				goto LABEL_62;
			++top;
			VM_GetShort((_WORD **)&pos);
LABEL_269:
			type2 = GetObjectType(objectRef);
			goto LABEL_270;
		case OP_EvalFieldVariable:
LABEL_62:
			v52 = ++top;
			v53 = VM_GetShort((_WORD **)&pos);
			Scr_FindVariableFieldInternal(&pValue, objectRef, v53);
			v54 = pValue.type;
			v52->u.intValue = pValue.u.intValue;
			v52->type = v54;
			continue;
		case OP_EvalLevelFieldVariableRef:
			objectRef = scrVarPub.levelId;
			goto LABEL_66;
		case OP_EvalAnimFieldVariableRef:
			objectRef = scrVarPub.animId;
			goto LABEL_66;
		case OP_EvalSelfFieldVariableRef:
			objectRef = Scr_GetSelf(localId);
			goto LABEL_66;
		case OP_EvalFieldVariableRef:
LABEL_66:
			v55 = VM_GetShort((_WORD **)&pos);
			evalRef = Scr_GetVariableField(objectRef, v55);
			continue;
		case OP_ClearFieldVariable:
			v56 = VM_GetShort((_WORD **)&pos);
			ClearVariableField(objectRef, v56, top);
			continue;
		case OP_SafeCreateVariableFieldCached:
			++scrVmPub.localVars;
			++varCount;
			v57 = VM_GetShort((_WORD **)&pos);
			*scrVmPub.localVars = GetNewVariable(localId, v57);
			goto LABEL_69;
		case OP_SafeSetVariableFieldCached0:
LABEL_69:
			if ( top->type != VAR_PRECODEPOS )
				goto LABEL_70;
			continue;
		case OP_SafeSetVariableFieldCached:
			if ( top->type != VAR_PRECODEPOS )
				goto LABEL_72;
			++pos;
			continue;
		case OP_SafeSetWaittillVariableFieldCached:
			if ( top->type != VAR_CODEPOS )
				goto LABEL_72;
			v59 = VM_AddLocalVar((unsigned __int8 *)pos);
			ClearVariableValue(v59);
			++pos;
			continue;
		case OP_clearparams:
			while ( top->type != VAR_CODEPOS )
				RemoveRefToValue(top--);
			continue;
		case OP_checkclearparams:
			if ( top->type == VAR_PRECODEPOS )
			{
				top->type = VAR_CODEPOS;
			}
			else
			{
				Scr_Error("function called with too many parameters");
LABEL_81:
				evalRef = *scrVmPub.localVars;
			}
			continue;
		case OP_EvalLocalVariableRefCached0:
			goto LABEL_81;
		case OP_EvalLocalVariableRefCached:
			evalRef = VM_AddLocalVar((unsigned __int8 *)pos++);
			continue;
		case OP_SetLevelFieldVariableField:
			v60 = VM_GetShort((_WORD **)&pos);
			v61 = GetVariable(scrVarPub.levelId, v60);
			SetVariableValue(v61, top);
			goto LABEL_3;
		case OP_SetVariableField:
			goto LABEL_85;
		case OP_SetAnimFieldVariableField:
			v62 = VM_GetShort((_WORD **)&pos);
			v63 = GetVariable(scrVarPub.animId, v62);
			SetVariableValue(v63, top);
			goto LABEL_3;
		case OP_SetSelfFieldVariableField:
			v130 = (unsigned __int16)VM_GetShort((_WORD **)&pos);
			objectRef = Scr_GetSelf(localId);
			evalRef = Scr_GetVariableField(objectRef, v130);
			goto LABEL_85;
		case OP_SetLocalVariableFieldCached0:
LABEL_70:
			SetVariableValue(*scrVmPub.localVars, top);
			goto LABEL_3;
		case OP_SetLocalVariableFieldCached:
LABEL_72:
			v58 = VM_AddLocalVar((unsigned __int8 *)pos);
			SetVariableValue(v58, top);
			++pos;
			goto LABEL_3;
		case OP_CallBuiltin0:
			goto LABEL_93;
		case OP_CallBuiltin1:
			scrVmPub.outparamcount = 1;
			goto LABEL_93;
		case OP_CallBuiltin2:
			scrVmPub.outparamcount = 2;
			goto LABEL_93;
		case OP_CallBuiltin3:
			scrVmPub.outparamcount = 3;
			goto LABEL_93;
		case OP_CallBuiltin4:
			scrVmPub.outparamcount = 4;
			goto LABEL_93;
		case OP_CallBuiltin5:
			scrVmPub.outparamcount = 5;
			goto LABEL_93;
		case OP_CallBuiltin:
			scrVmPub.outparamcount = *(unsigned __int8 *)pos++;
LABEL_93:
			scrVmPub.top = top;
			v127 = (unsigned __int16)VM_GetShort((_WORD **)&pos);
			scrVmPub.function_frame->fs.pos = pos;
			((void (*)(void))scrCompilePub.func_table[v127])();
			goto LABEL_94;
		case OP_CallBuiltinMethod0:
			goto LABEL_106;
		case OP_CallBuiltinMethod1:
			scrVmPub.outparamcount = 1;
			goto LABEL_106;
		case OP_CallBuiltinMethod2:
			scrVmPub.outparamcount = 2;
			goto LABEL_106;
		case OP_CallBuiltinMethod3:
			scrVmPub.outparamcount = 3;
			goto LABEL_106;
		case OP_CallBuiltinMethod4:
			scrVmPub.outparamcount = 4;
			goto LABEL_106;
		case OP_CallBuiltinMethod5:
			scrVmPub.outparamcount = 5;
			goto LABEL_106;
		case OP_CallBuiltinMethod:
			scrVmPub.outparamcount = *(unsigned __int8 *)pos++;
LABEL_106:
			scrVmPub.top = top - 1;
			v127 = (unsigned __int16)VM_GetShort((_WORD **)&pos);
			if ( top->type != VAR_OBJECT )
				goto LABEL_110;
			objectRef = top->u.intValue;
			if ( GetObjectType(objectRef) == VAR_ENTITY )
			{
				Scr_GetEntityIdRef(&entRef, objectRef);
				entRef2 = entRef;
				RemoveRefToObject(objectRef);
				scrVmPub.function_frame->fs.pos = pos;
				((void (QDECL *)(scr_entref_t))scrCompilePub.func_table[v127])(entRef2);
LABEL_94:
				top = scrVmPub.top;
				pos = scrVmPub.function_frame->fs.pos;
				if ( scrVmPub.outparamcount )
				{
					paramcount = scrVmPub.outparamcount;
					scrVmPub.outparamcount = 0;
					scrVmPub.top -= paramcount;
					do
					{
						RemoveRefToValue(top--);
						--paramcount;
					}
					while ( paramcount );
				}
				if ( scrVmPub.inparamcount )
				{
					scrVmPub.inparamcount = 0;
				}
				else
				{
					++top;
					top->type = VAR_UNDEFINED;
				}
				continue;
			}
			type2 = GetObjectType(objectRef);
			RemoveRefToObject(objectRef);
			scrVarPub.error_index = -1;
			v64 = va("%s is not an entity", var_typename[type2]);
			Scr_Error(v64);
LABEL_110:
			type2 = top->type;
			RemoveRefToValue(top);
			scrVarPub.error_index = -1;
			v65 = va("%s is not an entity", var_typename[type2]);
			Scr_Error(v65);
LABEL_111:
			if ( top->type == VAR_FLOAT )
			{
				if ( top->u.floatValue < 0.0 )
					goto LABEL_124;
				v109 = top->u.floatValue * 20.0;
				waitTime = VM_Floor(v109);
				if ( !waitTime )
					waitTime = top->u.floatValue != 0.0;
			}
			else if ( top->type == VAR_INTEGER )
			{
				waitTime = 20 * top->u.intValue;
			}
			else
			{
				scrVarPub.error_index = 2;
				v66 = va("type %s is not a float", var_typename[top->type]);
				Scr_Error(v66);
			}
			if ( waitTime > 0xFFFFFE )
			{
				scrVarPub.error_index = 2;
				if ( (waitTime & 0x80000000) == 0 )
					Scr_Error("wait is too long");
LABEL_124:
				Scr_Error("negative wait is not allowed");
LABEL_125:
				v129.type = VAR_STACK;
				v129.u.stackValue = VM_ArchiveStack(top - startTop, pos, top, varCount, &localId);
				v68 = GetVariable(scrVarPub.timeArrayId, scrVarPub.time);
				notifyListOwnerId = GetArray(v68);
				id = GetNewObjectVariableReverse(notifyListOwnerId, localId);
				SetNewVariableValue(id, &v129);
				Scr_SetThreadWaitTime(localId, scrVarPub.time);
				goto LABEL_254;
			}
			if ( waitTime )
				Scr_ResetTimeout();
			waitTime = (scrVarPub.time + waitTime) & 0xFFFFFF;
			--top;
			v129.type = VAR_STACK;
			v129.u.stackValue = VM_ArchiveStack(top - startTop, pos, top, varCount, &localId);
			v67 = GetVariable(scrVarPub.timeArrayId, waitTime);
			notifyListOwnerId = GetArray(v67);
			id = GetNewObjectVariable(notifyListOwnerId, localId);
			SetNewVariableValue(id, &v129);
			Scr_SetThreadWaitTime(localId, waitTime);
LABEL_254:
			startTop[1].type = VAR_UNDEFINED;
LABEL_255:
			if ( refCount )
			{
				--refCount;
				RemoveRefToObject(localId);
				pos = scrVmPub.function_frame->fs.pos;
				localId = scrVmPub.function_frame->fs.localId;
				varCount = scrVmPub.function_frame->fs.localVarCount;
				top = scrVmPub.function_frame->fs.top;
				startTop = scrVmPub.function_frame->fs.startTop;
				top->type = scrVmPub.function_frame->topType;
				++top;
				continue;
			}
			--g_script_error_level;
			return localId;
		case OP_wait:
			goto LABEL_111;
		case OP_waittillFrameEnd:
			goto LABEL_125;
		case OP_PreScriptCall:
			++top;
			top->type = VAR_PRECODEPOS;
			continue;
		case OP_ScriptFunctionCall2:
			++top;
			top->type = VAR_PRECODEPOS;
			goto LABEL_128;
		case OP_ScriptFunctionCall:
LABEL_128:
			if ( scrVmPub.function_count <= 30 )
			{
				self = Scr_GetSelf(localId);
				AddRefToObject(self);
				localId = AllocChildThread(self, localId);
				scrVmPub.function_frame->fs.pos = pos;
				pos = (const char *)VM_AddInt((_DWORD **)scrVmPub.function_frame);
				goto LABEL_253;
			}
			Scr_Error("script stack overflow (too many embedded function calls)");
LABEL_131:
			if ( top->type != VAR_FUNCTION )
				goto LABEL_135;
			if ( scrVmPub.function_count <= 30 )
			{
				self = Scr_GetSelf(localId);
				AddRefToObject(self);
				localId = AllocChildThread(self, localId);
				scrVmPub.function_frame->fs.pos = pos;
				pos = (const char *)top->u.intValue;
				--top;
				goto LABEL_253;
			}
			scrVarPub.error_index = 1;
			Scr_Error("script stack overflow (too many embedded function calls)");
LABEL_135:
			v69 = va("%s is not a function pointer", var_typename[top->type]);
			Scr_Error(v69);
LABEL_136:
			if ( top->type != VAR_OBJECT )
				goto LABEL_263;
			if ( scrVmPub.function_count <= 30 )
			{
				localId = AllocChildThread(top->u.intValue, localId);
				--top;
				scrVmPub.function_frame->fs.pos = pos;
				pos = (const char *)VM_AddInt((_DWORD **)scrVmPub.function_frame);
				goto LABEL_253;
			}
			Scr_Error("script stack overflow (too many embedded function calls)");
LABEL_263:
			type2 = top->type;
			goto LABEL_267;
		case OP_ScriptFunctionCallPointer:
			goto LABEL_131;
		case OP_ScriptMethodCall:
			goto LABEL_136;
		case OP_ScriptMethodCallPointer:
			if ( top->type == VAR_FUNCTION )
			{
				prevCodePos = (const char *)top->u.intValue;
				--top;
				if ( top->type != VAR_OBJECT )
					goto LABEL_265;
				if ( scrVmPub.function_count <= 30 )
				{
					localId = AllocChildThread(top->u.intValue, localId);
					--top;
					scrVmPub.function_frame->fs.pos = pos;
					pos = prevCodePos;
					goto LABEL_253;
				}
				goto LABEL_162;
			}
			RemoveRefToValue(top--);
			v70 = va("%s is not a function pointer", var_typename[top[1].type]);
			Scr_Error(v70);
LABEL_146:
			if ( scrVmPub.function_count <= 30 )
			{
				self = Scr_GetSelf(localId);
				AddRefToObject(self);
				localId = AllocThread(self);
				scrVmPub.function_frame->fs.pos = pos;
				scrVmPub.function_frame->fs.startTop = startTop;
				pos = (const char *)VM_AddInt((_DWORD **)scrVmPub.function_frame);
				v71 = VM_GetInt((_DWORD**)scrVmPub.function_frame);
				startTop = &top[-v71];
				goto LABEL_252;
			}
			scrVarPub.error_index = 1;
			Scr_Error("script stack overflow (too many embedded function calls)");
LABEL_149:
			if ( top->type == VAR_FUNCTION )
			{
				if ( scrVmPub.function_count <= 30 )
				{
					prevCodePos = (const char *)top->u.intValue;
					--top;
					self = Scr_GetSelf(localId);
					AddRefToObject(self);
					localId = AllocThread(self);
					scrVmPub.function_frame->fs.pos = pos;
					scrVmPub.function_frame->fs.startTop = startTop;
					pos = prevCodePos;
					v72 = VM_GetInt((_DWORD**)scrVmPub.function_frame);
					startTop = &top[-v72];
					goto LABEL_252;
				}
				scrVarPub.error_index = 1;
				Scr_Error("script stack overflow (too many embedded function calls)");
			}
			v73 = va("%s is not a function pointer", var_typename[top->type]);
			Scr_Error(v73);
LABEL_154:
			if ( top->type != VAR_OBJECT )
				goto LABEL_265;
			if ( scrVmPub.function_count > 30 )
				goto LABEL_162;
			localId = AllocThread(top->u.intValue);
			--top;
			scrVmPub.function_frame->fs.pos = pos;
			scrVmPub.function_frame->fs.startTop = startTop;
			pos = (const char *)VM_AddInt((_DWORD **)scrVmPub.function_frame);
			v74 = VM_GetInt((_DWORD**)scrVmPub.function_frame);
			startTop = &top[-v74];
			goto LABEL_252;
		case OP_ScriptThreadCall:
			goto LABEL_146;
		case OP_ScriptThreadCallPointer:
			goto LABEL_149;
		case OP_ScriptMethodThreadCall:
			goto LABEL_154;
		case OP_ScriptMethodThreadCallPointer:
			if ( top->type != VAR_FUNCTION )
			{
				RemoveRefToValue(top--);
				v76 = va("%s is not a function pointer", var_typename[top[1].type]);
				Scr_Error(v76);
LABEL_164:
				RemoveRefToValue(top);
				goto LABEL_3;
			}
			prevCodePos = (const char *)top->u.intValue;
			--top;
			if ( top->type != VAR_OBJECT )
				goto LABEL_265;
			if ( scrVmPub.function_count > 30 )
			{
LABEL_162:
				scrVarPub.error_index = 1;
				Scr_Error("script stack overflow (too many embedded function calls)");
LABEL_265:
				type2 = top->type;
				goto LABEL_268;
			}
			localId = AllocThread(top->u.intValue);
			--top;
			scrVmPub.function_frame->fs.pos = pos;
			scrVmPub.function_frame->fs.startTop = startTop;
			pos = prevCodePos;
			v75 = VM_GetInt((_DWORD**)scrVmPub.function_frame);
			startTop = &top[-v75];
LABEL_252:
			scrVmPub.function_frame->fs.top = startTop;
			scrVmPub.function_frame->topType = startTop->type;
			startTop->type = VAR_PRECODEPOS;
			++refCount;
LABEL_253:
			scrVmPub.function_frame->fs.localVarCount = varCount;
			varCount = 0;
			++scrVmPub.function_count;
			++scrVmPub.function_frame;
			scrVmPub.function_frame->fs.localId = localId;
			continue;
		case OP_DecTop:
			goto LABEL_164;
		case OP_CastFieldObject:
			objectRef = Scr_EvalFieldObject(scrVarPub.tempVariable, top);
			goto LABEL_3;
		case OP_EvalLocalVariableObjectCached:
			v77 = VM_AddLocalVar((unsigned __int8 *)pos);
			objectRef = Scr_EvalVariableObject(v77);
			++pos;
			continue;
		case OP_CastBool:
			Scr_CastBool(top);
			continue;
		case OP_BoolNot:
			Scr_EvalBoolNot(top);
			continue;
		case OP_BoolComplement:
			Scr_EvalBoolComplement(top);
			continue;
		case OP_JumpOnFalse:
			Scr_CastBool(top);
			v135 = (unsigned __int16)VM_GetShort((_WORD **)&pos);
			if ( !top->u.intValue )
				pos += v135;
			goto LABEL_3;
		case OP_JumpOnTrue:
			Scr_CastBool(top);
			v135 = (unsigned __int16)VM_GetShort((_WORD **)&pos);
			if ( top->u.intValue )
				pos += v135;
			goto LABEL_3;
		case OP_JumpOnFalseExpr:
			Scr_CastBool(top);
			v135 = (unsigned __int16)VM_GetShort((_WORD **)&pos);
			if ( top->u.intValue )
				goto LABEL_3;
			pos += v135;
			continue;
		case OP_JumpOnTrueExpr:
			Scr_CastBool(top);
			v135 = (unsigned __int16)VM_GetShort((_WORD **)&pos);
			if ( !top->u.intValue )
				goto LABEL_3;
			pos += v135;
			continue;
		case OP_jump:
			v135 = VM_GetInt((_DWORD **)&pos);
			pos += v135;
			continue;
		case OP_jumpback:
			if ( (unsigned int)(Sys_MilliSeconds() - scrVmGlob.starttime) > 0x1387 )
			{
				if ( !scrVmGlob.loading )
				{
					if ( !scrVmPub.abort_on_error )
					{
						Com_Printf("script runtime error: potential infinite loop in script - killing thread.\n");
						Scr_PrintPrevCodePos(CON_CHANNEL_DONT_FILTER, pos, 0);
						Scr_ResetTimeout();
						while ( 1 )
						{
							parentId = GetSafeParentLocalId(localId);
							Scr_KillThread(localId);
							scrVmPub.localVars -= varCount;
							while ( top->type != VAR_CODEPOS )
								RemoveRefToValue(top--);
							--scrVmPub.function_count;
							--scrVmPub.function_frame;
							if ( !parentId )
								break;
							RemoveRefToObject(localId);
							pos = scrVmPub.function_frame->fs.pos;
							varCount = scrVmPub.function_frame->fs.localVarCount;
							localId = parentId;
							--top;
						}
						goto LABEL_254;
					}
					Scr_TerminalError("potential infinite loop in script");
LABEL_186:
					v78 = ++top;
					Scr_EvalVariableFieldInternal(&v114, evalRef);
					v79 = v114.type;
					v78->u.intValue = v114.u.intValue;
					v78->type = v79;
					if ( top->type == VAR_INTEGER )
					{
						++top->u.intValue;
						++pos;
					}
					else
					{
						v80 = va("++ must be applied to an int (applied to %s)", var_typename[top->type]);
						Scr_Error(v80);
LABEL_189:
						v81 = ++top;
						Scr_EvalVariableFieldInternal(&v113, evalRef);
						v82 = v113.type;
						v81->u.intValue = v113.u.intValue;
						v81->type = v82;
						if ( top->type != VAR_INTEGER )
						{
							v83 = va("-- must be applied to an int (applied to %s)", var_typename[top->type]);
							Scr_Error(v83);
LABEL_192:
							Scr_EvalOr(top - 1, top);
							goto LABEL_3;
						}
						--top->u.intValue;
						++pos;
					}
LABEL_85:
					SetVariableFieldValue(evalRef, top);
LABEL_3:
					--top;
					continue;
				}
				Com_Printf("script runtime warning: potential infinite loop in script.\n");
				Scr_PrintPrevCodePos(CON_CHANNEL_DONT_FILTER, pos, 0);
				v135 = (unsigned __int16)VM_GetShort((_WORD **)&pos);
				pos -= v135;
				Scr_ResetTimeout();
			}
			else
			{
LABEL_180:
				v135 = (unsigned __int16)VM_GetShort((_WORD **)&pos);
				pos -= v135;
			}
			continue;
		case OP_inc:
			goto LABEL_186;
		case OP_dec:
			goto LABEL_189;
		case OP_bit_or:
			goto LABEL_192;
		case OP_bit_ex_or:
			Scr_EvalExOr(top - 1, top);
			goto LABEL_3;
		case OP_bit_and:
			Scr_EvalAnd(top - 1, top);
			goto LABEL_3;
		case OP_equality:
			Scr_EvalEquality(top - 1, top);
			goto LABEL_3;
		case OP_inequality:
			Scr_EvalInequality(top - 1, top);
			goto LABEL_3;
		case OP_less:
			Scr_EvalLess(top - 1, top);
			goto LABEL_3;
		case OP_greater:
			Scr_EvalGreater(top - 1, top);
			goto LABEL_3;
		case OP_less_equal:
			Scr_EvalLessEqual(top - 1, top);
			goto LABEL_3;
		case OP_greater_equal:
			Scr_EvalGreaterEqual(top - 1, top);
			goto LABEL_3;
		case OP_shift_left:
			Scr_EvalShiftLeft(top - 1, top);
			goto LABEL_3;
		case OP_shift_right:
			Scr_EvalShiftRight(top - 1, top);
			goto LABEL_3;
		case OP_plus:
			Scr_EvalPlus(top - 1, top);
			goto LABEL_3;
		case OP_minus:
			Scr_EvalMinus(top - 1, top);
			goto LABEL_3;
		case OP_multiply:
			Scr_EvalMultiply(top - 1, top);
			goto LABEL_3;
		case OP_divide:
			Scr_EvalDivide(top - 1, top);
			goto LABEL_3;
		case OP_mod:
			Scr_EvalMod(top - 1, top);
			goto LABEL_3;
		case OP_size:
			Scr_EvalSizeValue(top);
			continue;
		case OP_waittillmatch:
		case OP_waittill:
			if ( top->type != VAR_OBJECT )
				goto LABEL_265;
			if ( !IsFieldObject(top->u.intValue) )
				goto LABEL_266;
			currentVariable.u.intValue = top->u.intValue;
			--top;
			if ( top->type == VAR_STRING )
			{
				name = top->u.intValue;
				--top;
				v129.type = VAR_STACK;
				v129.u.stackValue = VM_ArchiveStack(top - startTop, pos, top, varCount, &localId);
				v84 = GetVariable(currentVariable.u.stringValue, 0x1FFFEu);
				Array = GetArray(v84);
				v86 = GetVariable(Array, name);
				notifyListOwnerId = GetArray(v86);
				id = GetNewObjectVariable(notifyListOwnerId, localId);
				SetNewVariableValue(id, &v129);
				currentVariable.type = VAR_OBJECT;
				v87 = Scr_GetSelf(localId);
				ObjectVariable = GetObjectVariable(scrVarPub.pauseArrayId, v87);
				v89 = GetArray(ObjectVariable);
				NewObjectVariable = GetNewObjectVariable(v89, localId);
				SetNewVariableValue(NewObjectVariable, &currentVariable);
				Scr_SetThreadNotifyName(localId, name);
				goto LABEL_254;
			}
			++top;
			scrVarPub.error_index = 3;
			Scr_Error("first parameter of waittill must evaluate to a string");
LABEL_266:
			type2 = GetObjectType(top->u.intValue);
LABEL_268:
			scrVarPub.error_index = 2;
LABEL_270:
			v107 = va("%s is not an object", var_typename[type2]);
			Scr_Error(v107);
LABEL_271:
			switch ( opcode )
			{
			case OP_EvalLocalArrayRefCached0:
			case OP_EvalLocalArrayRefCached:
			case OP_EvalArrayRef:
			case OP_ClearArray:
			case OP_EvalLocalVariableRef:
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
				if ( scrVarPub.error_index <= 0 )
				{
					if ( scrVarPub.error_index < 0 )
						scrVarPub.error_index = 1;
				}
				else
				{
					scrVarPub.error_index = scrVmPub.outparamcount - scrVarPub.error_index + 2;
				}
				break;
			default:
				break;
			}
			scriptError(pos, scrVarPub.error_index, scrVarPub.error_message, scrVmGlob.dialog_error_message);
			Scr_ClearErrorMessage();
			switch ( opcode )
			{
			case OP_EvalLocalArrayCached:
			case OP_EvalArray:
				RemoveRefToValue(top--);
				RemoveRefToValue(top);
				top->type = VAR_UNDEFINED;
				continue;
			case OP_EvalLocalArrayRefCached0:
			case OP_EvalLocalArrayRefCached:
			case OP_EvalArrayRef:
			case OP_EvalLocalVariableRef:
				evalRef = FreeTempVariable();
				goto LABEL_295;
			case OP_ClearArray:
			case OP_wait:
				goto LABEL_295;
			case OP_GetSelfObject:
				goto LABEL_286;
			case OP_EvalSelfFieldVariable:
			case OP_EvalFieldVariable:
				top->type = VAR_UNDEFINED;
				continue;
			case OP_EvalSelfFieldVariableRef:
			case OP_EvalFieldVariableRef:
				evalRef = FreeTempVariable();
				continue;
			case OP_ClearFieldVariable:
				if ( scrVmPub.outparamcount )
					scrVmPub.outparamcount = 0;
				continue;
			case OP_checkclearparams:
				while ( top->type != VAR_PRECODEPOS )
					RemoveRefToValue(top--);
				top->type = VAR_CODEPOS;
				continue;
			case OP_SetVariableField:
				if ( scrVmPub.outparamcount )
					goto LABEL_291;
				goto LABEL_296;
			case OP_SetSelfFieldVariableField:
LABEL_291:
				RemoveRefToValue(top);
				scrVmPub.outparamcount = 0;
				goto LABEL_296;
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
				top = scrVmPub.top + 1;
				scrVmPub.top[1].type = VAR_UNDEFINED;
				continue;
			case OP_ScriptFunctionCall2:
			case OP_ScriptFunctionCall:
			case OP_ScriptMethodCall:
				VM_AddInt((_DWORD **)&pos);
				goto LABEL_302;
			case OP_ScriptFunctionCallPointer:
			case OP_ScriptMethodCallPointer:
LABEL_302:
				while ( top->type != VAR_PRECODEPOS )
					RemoveRefToValue(top--);
				top->type = VAR_UNDEFINED;
				continue;
			case OP_ScriptThreadCall:
			case OP_ScriptMethodThreadCall:
				VM_AddInt((_DWORD **)&pos);
				goto LABEL_306;
			case OP_ScriptThreadCallPointer:
			case OP_ScriptMethodThreadCallPointer:
LABEL_306:
				for ( paramcount = VM_GetInt((_DWORD **)&pos); paramcount; --paramcount )
					RemoveRefToValue(top--);
				++top;
				top->type = VAR_UNDEFINED;
				continue;
			case OP_CastFieldObject:
				objectRef = FreeTempVariableObject();
				goto LABEL_296;
			case OP_EvalLocalVariableObjectCached:
				++pos;
LABEL_286:
				objectRef = FreeTempVariableObject();
				continue;
			case OP_JumpOnFalse:
			case OP_JumpOnTrue:
			case OP_JumpOnFalseExpr:
			case OP_JumpOnTrueExpr:
				VM_GetShort((_WORD **)&pos);
				goto LABEL_296;
			case OP_jumpback:
				goto LABEL_180;
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
				goto LABEL_296;
			case OP_waittillmatch:
				++pos;
				goto LABEL_294;
			case OP_waittill:
			case OP_endon:
LABEL_294:
				RemoveRefToValue(top--);
				goto LABEL_295;
			case OP_notify:
				while ( top->type != VAR_PRECODEPOS )
					RemoveRefToValue(top--);
				goto LABEL_295;
			case OP_switch:
				while ( v138 )
				{
					v132 = VM_GetDword((_DWORD **)&pos);
					v131 = (const char *)VM_AddInt((_DWORD **)&pos);
					--v138;
				}
				if ( !v132 )
					pos = v131;
LABEL_295:
				RemoveRefToValue(top);
LABEL_296:
				--top;
				break;
			default:
				continue;
			}
			continue;
		case OP_notify:
			if ( top->type != VAR_OBJECT )
				goto LABEL_265;
			notifyListOwnerId = top->u.intValue;
			if ( !IsFieldObject(notifyListOwnerId) )
				goto LABEL_266;
			--top;
			if ( top->type != VAR_STRING )
			{
				++top;
				scrVarPub.error_index = 1;
				Scr_Error("first parameter of notify must evaluate to a string");
				goto LABEL_266;
			}
			name = top->u.intValue;
			--top;
			scrVmPub.function_frame->fs.pos = pos;
			VM_Notify(notifyListOwnerId, name, top);
			pos = scrVmPub.function_frame->fs.pos;
			RemoveRefToObject(notifyListOwnerId);
			SL_RemoveRefToString(name);
			while ( top->type != VAR_PRECODEPOS )
				RemoveRefToValue(top--);
			goto LABEL_3;
		case OP_endon:
			if ( top->type != VAR_OBJECT )
				goto LABEL_263;
			if ( !IsFieldObject(top->u.intValue) )
				goto LABEL_264;
			if ( top[-1].type == VAR_STRING )
			{
				name = top[-1].u.stringValue;
				AddRefToObject(localId);
				startLocalId = AllocThread(localId);
				v91 = GetVariable(top->u.intValue, 0x1FFFEu);
				v92 = GetArray(v91);
				v93 = GetVariable(v92, name);
				v94 = GetArray(v93);
				GetObjectVariable(v94, startLocalId);
				RemoveRefToObject(startLocalId);
				currentVariable.type = VAR_OBJECT;
				currentVariable.u.intValue = top->u.intValue;
				v95 = GetObjectVariable(scrVarPub.pauseArrayId, localId);
				v96 = GetArray(v95);
				v97 = GetNewObjectVariable(v96, startLocalId);
				SetNewVariableValue(v97, &currentVariable);
				Scr_SetThreadNotifyName(startLocalId, name);
				top -= 2;
				continue;
			}
			Scr_Error("first parameter of endon must evaluate to a string");
LABEL_264:
			type2 = GetObjectType(top->u.intValue);
LABEL_267:
			scrVarPub.error_index = 1;
			goto LABEL_270;
		case OP_voidCodepos:
			++top;
			top->type = VAR_PRECODEPOS;
			continue;
		case OP_switch:
			v135 = VM_GetInt((_DWORD **)&pos);
			pos += v135;
			v138 = (unsigned __int16)VM_GetShort((_WORD **)&pos);
			v110 = top->type;
			if ( v110 == VAR_STRING )
				goto LABEL_232;
			if ( v110 == VAR_INTEGER )
			{
				if ( IsValidArrayIndex(top->u.intValue) )
				{
					InternalVariableIndex = GetInternalVariableIndex(top->u.intValue);
				}
				else
				{
					v98 = va("switch index %d out of range", top->u.intValue);
					Scr_Error(v98);
LABEL_232:
					InternalVariableIndex = top->u.intValue;
					SL_RemoveRefToString(top->u.intValue);
				}
			}
			else
			{
				v99 = va("cannot switch on %s", var_typename[top->type]);
				Scr_Error(v99);
			}
			if ( !v138 )
				goto LABEL_3;
			do
			{
				v132 = VM_GetDword((_DWORD **)&pos);
				v131 = (const char *)VM_AddInt((_DWORD **)&pos);
				if ( v132 == InternalVariableIndex )
				{
					pos = v131;
					goto LABEL_3;
				}
				--v138;
			}
			while ( v138 );
			if ( !v132 )
				pos = v131;
			goto LABEL_3;
		case OP_endswitch:
			v138 = (unsigned __int16)VM_GetShort((_WORD **)&pos);
			VM_AddIntSize((int *)&pos, 2 * v138);
			continue;
		case OP_vector:
			top -= 2;
			Scr_CastVector(top);
			continue;
		case OP_NOP:
			continue;
		case OP_abort:
			--g_script_error_level;
			return 0;
		case OP_object:
			++top;
			classnum = VM_GetInt((_DWORD **)&pos);
			entnum = VM_GetInt((_DWORD **)&pos);
			v100 = top;
			v100->u.intValue = FindEntityId(entnum, classnum);
			if ( !top->u.intValue )
			{
				top->type = VAR_UNDEFINED;
				Scr_Error("unknown object");
			}
			goto LABEL_245;
		case OP_thread_object:
			v101 = ++top;
			v101->u.intValue = (unsigned __int16)VM_GetShort((_WORD **)&pos);
LABEL_245:
			top->type = VAR_OBJECT;
			AddRefToObject(top->u.intValue);
			continue;
		case OP_EvalLocalVariable:
			v102 = ++top;
			v103 = VM_GetShort((_WORD **)&pos);
			v104 = FindVariable(localId, v103);
			Scr_EvalVariable(&v112, v104);
			v105 = v112.type;
			v102->u.intValue = v112.u.intValue;
			v102->type = v105;
			continue;
		case OP_EvalLocalVariableRef:
			v106 = VM_GetShort((_WORD **)&pos);
			evalRef = FindVariable(localId, v106);
			if ( evalRef )
				continue;
			Scr_Error("cannot create a new local variable in the debugger");
			goto LABEL_250;
		case OP_prof_begin:
LABEL_250:
			++pos;
			continue;
		case OP_prof_end:
			++pos;
			continue;
		default:
			goto LABEL_252;
		}
	}
}