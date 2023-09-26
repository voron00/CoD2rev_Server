#include "../qcommon/qcommon.h"
#include "clientscript_public.h"

scrCompilePub_t scrCompilePub;

struct CaseStatementInfo
{
	unsigned int name;
	char *codePos;
	unsigned int sourcePos;
	CaseStatementInfo *next;
};

struct BreakStatementInfo
{
	char *codePos;
	char *nextCodePos;
	BreakStatementInfo *next;
};

struct ContinueStatementInfo
{
	char *codePos;
	char *nextCodePos;
	ContinueStatementInfo *next;
};

struct PrecacheEntry
{
	unsigned short filename;
	bool include;
	unsigned int sourcePos;
	PrecacheEntry *next;
};

typedef struct scrCompileGlob_s
{
	char *codePos;
	char *prevOpcodePos;
	unsigned int filePosId;
	unsigned int fileCountId;
	int cumulOffset;
	int maxOffset;
	int maxCallOffset;
	bool bConstRefCount;
	bool in_developer_thread;
	unsigned int developer_thread_sourcePos;
	bool firstThread[2];
	CaseStatementInfo *currentCaseStatement;
	bool bCanBreak[1];
	BreakStatementInfo *currentBreakStatement;
	bool bCanContinue[1];
	ContinueStatementInfo *currentContinueStatement;
	scr_block_s **breakChildBlocks;
	int *breakChildCount;
	scr_block_s *breakBlock;
	scr_block_s **continueChildBlocks;
	int *continueChildCount;
	bool forceNotCreate;
	PrecacheEntry *precachescriptList;
	PrecacheEntry *precachescriptListHead;
	VariableCompileValue value_start[32];
} scrCompileGlob_t;
// static_assert((sizeof(scrCompileGlob_t) == 0x1DC), "ERROR: scrCompileGlob_t size is invalid!");

scrCompileGlob_t scrCompileGlob;

int GetExpressionCount(sval_u exprlist)
{
	int expr_count;
	sval_u *node;

	expr_count = 0;

	for ( node = exprlist.node->node; node; node = node[1].node )
		++expr_count;

	return expr_count;
}

void Scr_CompileRemoveRefToString(unsigned int stringValue)
{
	if ( !scrCompileGlob.bConstRefCount )
		SL_RemoveRefToString(stringValue);
}

void EmitCanonicalString(unsigned int stringValue)
{
	char *pos;

	scrCompileGlob.codePos = TempMallocAlign(2);

	if ( scrCompilePub.developer_statement == 2 )
	{
		Scr_CompileRemoveRefToString(stringValue);
	}
	else
	{
		if ( scrCompileGlob.bConstRefCount )
			SL_AddRefToString(stringValue);

		pos = scrCompileGlob.codePos;
		*(uint16_t *)pos = Scr_GetCanonicalStringIndex(stringValue);
	}
}

void CompileTransferRefToString(unsigned int stringValue, unsigned char user)
{
	if ( scrCompilePub.developer_statement == 2 )
	{
		Scr_CompileRemoveRefToString(stringValue);
	}
	else
	{
		if ( scrCompileGlob.bConstRefCount )
			SL_AddRefToString(stringValue);

		SL_TransferRefToUser(stringValue, user);
	}
}

void EmitCodepos(const char *pos)
{
	scrCompileGlob.codePos = TempMallocAlign(4);
	*(uint32_t *)scrCompileGlob.codePos = (intptr_t)pos;
}

void EmitShort(short value)
{
	scrCompileGlob.codePos = TempMallocAlign(2);
	*(uint16_t *)scrCompileGlob.codePos = value;
}

void EmitByte(byte value)
{
	scrCompileGlob.codePos = (char *)TempMalloc(1);
	*scrCompileGlob.codePos = value;
}

void EmitFloat(float value)
{
	scrCompileGlob.codePos = TempMallocAlignStrict(4);
	*(float *)scrCompileGlob.codePos = value;
}

void EvalUndefined(sval_u sourcePos, VariableCompileValue *constValue)
{
	constValue->value.type = VAR_UNDEFINED;
	constValue->sourcePos = sourcePos;
}

void EvalInteger(int value, sval_u sourcePos, VariableCompileValue *constValue)
{
	constValue->value.type = VAR_INTEGER;
	constValue->value.u.intValue = value;
	constValue->sourcePos = sourcePos;
}

void EvalFloat(float value, sval_u sourcePos, VariableCompileValue *constValue)
{
	constValue->value.type = VAR_FLOAT;
	constValue->value.u.floatValue = value;
	constValue->sourcePos = sourcePos;
}

void EvalString(unsigned int value, sval_u sourcePos, VariableCompileValue *constValue)
{
	constValue->value.type = VAR_STRING;
	constValue->value.u.stringValue = value;
	constValue->sourcePos = sourcePos;
}

void EvalIString(unsigned int value, sval_u sourcePos, VariableCompileValue *constValue)
{
	constValue->value.type = VAR_ISTRING;
	constValue->value.u.stringValue = value;
	constValue->sourcePos = sourcePos;
}

void EmitGetUndefined(sval_u sourcePos)
{
	EmitOpcode(OP_GetUndefined, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
}

void EmitGetString(unsigned int value, sval_u sourcePos)
{
	EmitOpcode(OP_GetString, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
	EmitShort(value);
	CompileTransferRefToString(value, 1);
}

void EmitGetIString(unsigned int value, sval_u sourcePos)
{
	EmitOpcode(OP_GetIString, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
	EmitShort(value);
	CompileTransferRefToString(value, 1);
}

void EmitGetVector(const float *value, sval_u sourcePos)
{
	int i;

	EmitOpcode(OP_GetVector, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);

	for ( i = 0; i < 3; ++i )
		EmitFloat(value[i]);

	RemoveRefToVector(value);
}

void EmitGetFloat(float value, sval_u sourcePos)
{
	EmitOpcode(OP_GetFloat, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
	EmitFloat(value);
}

void EmitGetInteger(int value, sval_u sourcePos)
{
	if ( value < 0 )
	{
		if ( value > -256 )
		{
			EmitOpcode(OP_GetNegByte, 1, CALL_NONE);
			AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
			EmitByte(-(byte)value);
			return;
		}

		if ( value > -65536 )
		{
			EmitOpcode(OP_GetNegUnsignedShort, 1, CALL_NONE);
			AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
			EmitShort(-(short)value);
			return;
		}
	}
	else
	{
		if ( !value )
		{
			EmitOpcode(OP_GetZero, 1, CALL_NONE);
			AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
			return;
		}

		if ( value < 256 )
		{
			EmitOpcode(OP_GetByte, 1, CALL_NONE);
			AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
			EmitByte(value);
			return;
		}

		if ( value < 65536 )
		{
			EmitOpcode(OP_GetUnsignedShort, 1, CALL_NONE);
			AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
			EmitShort(value);
			return;
		}
	}

	EmitOpcode(OP_GetInteger, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
	EmitCodepos((const char *)value);
}

void EmitValue(VariableCompileValue *constValue)
{
	switch ( constValue->value.type )
	{
	case VAR_UNDEFINED:
		EmitGetUndefined(constValue->sourcePos);
		break;

	case VAR_STRING:
		EmitGetString(constValue->value.u.stringValue, constValue->sourcePos);
		break;

	case VAR_ISTRING:
		EmitGetIString(constValue->value.u.stringValue, constValue->sourcePos);
		break;

	case VAR_VECTOR:
		EmitGetVector(constValue->value.u.vectorValue, constValue->sourcePos);
		break;

	case VAR_FLOAT:
		EmitGetFloat(constValue->value.u.floatValue, constValue->sourcePos);
		break;

	case VAR_INTEGER:
		EmitGetInteger(constValue->value.u.intValue, constValue->sourcePos);
		break;

	default:
		break;
	}
}

void EmitOpcode(unsigned int op, int offset, int callType)
{
	unsigned int index;
	int count;
	int i;

	if ( scrCompilePub.value_count )
	{
		count = scrCompilePub.value_count;
		scrCompilePub.value_count = 0;

		for ( i = 0; i < count; ++i )
		{
			EmitValue(&scrCompileGlob.value_start[i]);
		}
	}

	scrCompilePub.allowedBreakpoint = 0;

	if ( !scrCompileGlob.cumulOffset || callType == CALL_THREAD || callType == CALL_FUNCTION )
		scrCompilePub.allowedBreakpoint = 1;

	scrCompileGlob.cumulOffset += offset;

	if ( scrCompileGlob.maxOffset < scrCompileGlob.cumulOffset )
		scrCompileGlob.maxOffset = scrCompileGlob.cumulOffset;

	if ( callType && scrCompileGlob.maxCallOffset < scrCompileGlob.cumulOffset )
		scrCompileGlob.maxCallOffset = scrCompileGlob.cumulOffset;

	scrVarPub.checksum *= 31;
	scrVarPub.checksum += op;

	if ( scrCompilePub.opcodePos )
	{
		scrCompileGlob.codePos = scrCompilePub.opcodePos;

		switch ( op )
		{
		case OP_EvalArray:
			if ( *scrCompilePub.opcodePos == OP_EvalLocalVariableCached )
			{
				RemoveOpcodePos();
				*scrCompilePub.opcodePos = OP_EvalLocalArrayCached;
				return;
			}
			index = (byte)*scrCompilePub.opcodePos - OP_EvalLocalVariableCached0;
			if ( index > OP_GetNegByte )
				goto setopcodepos;
			RemoveOpcodePos();
			*scrCompilePub.opcodePos = OP_EvalLocalArrayCached;
			EmitByte(index);
			return;

		case OP_EvalArrayRef:
			if ( *scrCompilePub.opcodePos == OP_EvalLocalVariableRefCached )
			{
				RemoveOpcodePos();
				*scrCompilePub.opcodePos = OP_EvalLocalArrayRefCached;
				return;
			}
			if ( *scrCompilePub.opcodePos != OP_EvalLocalVariableRefCached0 )
				goto setopcodepos;
			RemoveOpcodePos();
			*scrCompilePub.opcodePos = OP_EvalLocalArrayRefCached0;
			return;

		case OP_EvalFieldVariable:
			if ( *scrCompilePub.opcodePos == OP_GetSelfObject )
			{
				*scrCompilePub.opcodePos = OP_EvalSelfFieldVariable;
				return;
			}
			if ( *scrCompilePub.opcodePos == OP_GetLevelObject )
			{
				*scrCompilePub.opcodePos = OP_EvalLevelFieldVariable;
				return;
			}
			if ( *scrCompilePub.opcodePos != OP_GetAnimObject )
				goto setopcodepos;
			*scrCompilePub.opcodePos = OP_EvalAnimFieldVariable;
			return;

		case OP_EvalFieldVariableRef:
			if ( *scrCompilePub.opcodePos == OP_GetSelfObject )
			{
				*scrCompilePub.opcodePos = OP_EvalSelfFieldVariableRef;
				return;
			}
			if ( *scrCompilePub.opcodePos == OP_GetLevelObject )
			{
				*scrCompilePub.opcodePos = OP_EvalLevelFieldVariableRef;
				return;
			}
			if ( *scrCompilePub.opcodePos != OP_GetAnimObject )
				goto setopcodepos;
			*scrCompilePub.opcodePos = OP_EvalAnimFieldVariableRef;
			return;

		case OP_SafeSetVariableFieldCached0:
			if ( *scrCompilePub.opcodePos != OP_CreateLocalVariable )
				goto setopcodepos;
			*scrCompilePub.opcodePos = OP_SafeCreateVariableFieldCached;
			return;

		case OP_SetVariableField:
			switch ( *scrCompilePub.opcodePos )
			{
			case OP_EvalLocalVariableRefCached:
				RemoveOpcodePos();
				*scrCompilePub.opcodePos = OP_SetLocalVariableFieldCached;
				return;
			case OP_EvalLocalVariableRefCached0:
				RemoveOpcodePos();
				*scrCompilePub.opcodePos = OP_SetLocalVariableFieldCached0;
				return;
			case OP_EvalSelfFieldVariableRef:
				RemoveOpcodePos();
				*scrCompilePub.opcodePos = OP_SetSelfFieldVariableField;
				return;
			case OP_EvalLevelFieldVariableRef:
				RemoveOpcodePos();
				*scrCompilePub.opcodePos = OP_SetLevelFieldVariableField;
				return;
			}
			if ( *scrCompilePub.opcodePos != OP_EvalAnimFieldVariableRef )
				goto setopcodepos;
			RemoveOpcodePos();
			*scrCompilePub.opcodePos = OP_SetAnimFieldVariableField;
			return;

		case OP_ScriptFunctionCall:
			if ( *scrCompilePub.opcodePos != OP_PreScriptCall )
				goto setopcodepos;
			*scrCompilePub.opcodePos = OP_ScriptFunctionCall2;
			return;

		case OP_ScriptMethodCall:
			if ( *scrCompilePub.opcodePos != OP_GetSelf )
				goto setopcodepos;
			RemoveOpcodePos();
			*scrCompilePub.opcodePos = OP_ScriptFunctionCall;
			if ( *scrCompileGlob.prevOpcodePos == OP_PreScriptCall )
			{
				TempMemorySetPos(scrCompilePub.opcodePos);
				--scrCompilePub.opcodePos;
				scrCompileGlob.prevOpcodePos = 0;
				scrCompileGlob.codePos = scrCompilePub.opcodePos;
				*scrCompilePub.opcodePos = OP_ScriptFunctionCall2;
			}
			return;

		case OP_ScriptMethodThreadCall:
			if ( *scrCompilePub.opcodePos != OP_GetSelf )
				goto setopcodepos;
			RemoveOpcodePos();
			*scrCompilePub.opcodePos = OP_ScriptThreadCall;
			return;

		case OP_CastFieldObject:
			if ( *scrCompilePub.opcodePos == OP_EvalLocalVariableCached )
			{
				*scrCompilePub.opcodePos = OP_EvalLocalVariableObjectCached;
				return;
			}
			index = (byte)*scrCompilePub.opcodePos - OP_EvalLocalVariableCached0;
			if ( index > OP_GetNegByte )
				goto setopcodepos;
			*scrCompilePub.opcodePos = OP_EvalLocalVariableObjectCached;
			EmitByte(index);
			return;

		case OP_JumpOnFalse:
			if ( *scrCompilePub.opcodePos != OP_BoolNot )
				goto setopcodepos;
			RemoveOpcodePos();
			*scrCompilePub.opcodePos = OP_JumpOnTrue;
			return;

		default:
			goto setopcodepos;
		}
	}
	else
	{
setopcodepos:
		scrCompileGlob.prevOpcodePos = scrCompilePub.opcodePos;
		scrCompilePub.opcodePos = (char *)TempMalloc(1);
		scrCompileGlob.codePos = scrCompilePub.opcodePos;
		*scrCompilePub.opcodePos = op;
	}
}

void LinkThread(unsigned int threadCountId, VariableValue *pos, bool allowFarCall)
{
	VariableValue tempValue;
	int type;
	int i;
	int valueId;
	VariableValueInternal_u *value;
	unsigned int countId;

	countId = FindVariable(threadCountId, 0);

	if ( countId )
	{
		Scr_EvalVariable(&tempValue, countId);

		for ( i = 0; i < tempValue.u.intValue; ++i )
		{
			valueId = FindVariable(threadCountId, i + 2);
			value = GetVariableValueAddress(valueId);
			type = Scr_GetObjectType(valueId);

			if ( pos->type == VAR_DEVELOPER_CODEPOS && type == VAR_CODEPOS )
				CompileError2(value->u.codePosValue, "normal script cannot reference a function in a /# ... #/ comment");

			if ( pos->type == VAR_UNDEFINED )
				CompileError2(value->u.codePosValue, "unknown function");

			if ( !allowFarCall && value->u.stackValue->pos == (const char *)1 )
				CompileError2(value->u.codePosValue, "unknown function");

			value->u.stackValue->pos = pos->u.codePosValue;
		}
	}
}

void LinkFile(unsigned int filePosId)
{
	VariableValue pos;
	VariableValue emptyValue;
	unsigned int nameId;
	unsigned int threadCountId;
	unsigned int threadCountPtr;

	emptyValue.type = VAR_UNDEFINED;
	emptyValue.u.intValue = 0;

	for ( threadCountPtr = FindNextSibling(filePosId); threadCountPtr; threadCountPtr = FindNextSibling(threadCountPtr) )
	{
		threadCountId = FindObject(threadCountPtr);
		nameId = FindVariable(threadCountId, 1);

		if ( nameId )
		{
			Scr_EvalVariable(&pos, nameId);

			if ( pos.type == VAR_INCLUDE_CODEPOS )
				SetVariableValue(threadCountPtr, &emptyValue);
			else
				LinkThread(threadCountId, &pos, 1);
		}
		else
		{
			LinkThread(threadCountId, &emptyValue, 1);
		}
	}
}

unsigned int AddFilePrecache(unsigned int filename, unsigned int sourcePos, bool include)
{
	SL_AddRefToString(filename);
	Scr_CompileRemoveRefToString(filename);
	scrCompileGlob.precachescriptList->filename = filename;
	scrCompileGlob.precachescriptList->sourcePos = sourcePos;
	scrCompileGlob.precachescriptList->include = include;
	++scrCompileGlob.precachescriptList;

	return GetObjectA(GetVariable(scrCompilePub.scriptsPos, filename));
}

void EmitInclude(sval_u val)
{
	unsigned int filename;

	filename = Scr_CreateCanonicalFilename(SL_ConvertToString(val.node[1].stringValue));
	Scr_CompileRemoveRefToString(val.node[1].stringValue);
	AddFilePrecache(filename, val.node[2].sourcePosValue, 1);
}

void EmitIncludeList(sval_u val)
{
	sval_u *node;

	for ( node = val.node->node[1].node; node; node = node[1].node )
		EmitInclude(node[0]);
}

unsigned int SpecifyThreadPosition(unsigned int posId, unsigned int name, unsigned int sourcePos, int type)
{
	unsigned int buffer;
	unsigned int id;
	VariableValue pos;

	id = GetVariable(posId, 1);
	Scr_EvalVariable(&pos, id);

	if ( pos.type )
	{
		if ( pos.u.intValue )
		{
			buffer = Scr_GetSourceBuffer(pos.u.codePosValue);
			CompileError(sourcePos, "function '%s' already defined in '%s'", SL_ConvertToString(name), scrParserPub.sourceBufferLookup[buffer].buf);
		}
		else
		{
			CompileError(sourcePos, "function '%s' already defined", SL_ConvertToString(name));
		}
		return 0;
	}
	else
	{
		pos.type = type;
		pos.u.intValue = 0;
		SetNewVariableValue(id, &pos);
		return id;
	}
}

void SpecifyThread(sval_u val)
{
	unsigned int var;
	sval_u node;
	int type;
	unsigned int posId;

	node.type = val.node->type;

	if ( val.node->type == ENUM_begin_developer_thread )
	{
		if ( scrCompileGlob.in_developer_thread )
		{
			CompileError(val.node[1].sourcePosValue, "cannot recurse /#");
		}
		else
		{
			scrCompileGlob.in_developer_thread = 1;
			scrCompileGlob.developer_thread_sourcePos = val.node[1].sourcePosValue;
		}
	}
	else if ( node.type > ENUM_begin_developer_thread )
	{
		if ( node.type == ENUM_end_developer_thread )
		{
			if ( scrCompileGlob.in_developer_thread )
				scrCompileGlob.in_developer_thread = 0;
			else
				CompileError(val.node[1].sourcePosValue, "#/ has no matching /#");
		}
	}
	else if ( node.type == ENUM_thread && (!scrCompileGlob.in_developer_thread || scrVarPub.developer_script) )
	{
		var = GetVariable(scrCompileGlob.filePosId, val.node[1].idValue);
		posId = GetObjectA(var);

		if ( scrCompileGlob.in_developer_thread )

			type = VAR_DEVELOPER_CODEPOS;
		else
			type = VAR_CODEPOS;

		SpecifyThreadPosition(posId, val.node[1].sourcePosValue, val.node[4].sourcePosValue, type);
	}
}

void Scr_CheckLocalVarsCount(int localVarsCount)
{
	if ( localVarsCount > 63 )
		Com_Error(ERR_DROP, "LOCAL_VAR_STACK_SIZE exceeded");
}

void Scr_RegisterLocalVar(unsigned int name, sval_u sourcePos, scr_block_s *block)
{
	int i;

	if ( !block->abortLevel )
	{
		for ( i = 0; i < block->localVarsCount; ++i )
		{
			if ( block->localVars[i] == name )
				return;
		}

		Scr_CheckLocalVarsCount(block->localVarsCount);
		block->localVars[block->localVarsCount++] = name;
	}
}

int Scr_FindLocalVar(scr_block_s *block, int startIndex, unsigned int name)
{
	while ( startIndex < block->localVarsCount )
	{
		if ( block->localVars[startIndex] == name )
			return startIndex;

		++startIndex;
	}

	return -1;
}

void Scr_CalcLocalVarsSafeSetVariableField(sval_u expr, sval_u sourcePos, scr_block_s *block)
{
	Scr_RegisterLocalVar(expr.sourcePosValue, sourcePos, block);
}

void Scr_CalcLocalVarsFormalParameterListInternal(sval_u *node, scr_block_s *block)
{
	while ( 1 )
	{
		node = node[1].node;

		if ( !node )
			break;

		Scr_CalcLocalVarsSafeSetVariableField(node->node[0], node->node[1], block);
	}
}

void Scr_CalcLocalVarsWaittillStatement(sval_u exprlist, scr_block_s *block)
{
	Scr_CalcLocalVarsFormalParameterListInternal(exprlist.node->node[1].node, block);
}

void Scr_CalcLocalVarsFormalParameterList(sval_u exprlist, scr_block_s *block)
{
	Scr_CalcLocalVarsFormalParameterListInternal(exprlist.node->node, block);
}

void Scr_CalcLocalVarsArrayVariableRef(sval_u expr, scr_block_s *block)
{
	Scr_CalcLocalVarsArrayPrimitiveExpressionRef(expr, block);
}

void Scr_CalcLocalVarsVariableExpressionRef(sval_u expr, scr_block_s *block)
{
	if ( expr.node->type == ENUM_local_variable )
	{
		Scr_CalcLocalVarsSafeSetVariableField(expr.node[1], expr.node[2], block);
	}
	else if ( expr.node->type == ENUM_array_variable )
	{
		Scr_CalcLocalVarsArrayVariableRef(expr.node[1], block);
	}
}

void Scr_CalcLocalVarsAssignmentStatement(sval_u lhs, sval_u rhs, scr_block_s *block)
{
	Scr_CalcLocalVarsVariableExpressionRef(lhs, block);
}

void Scr_CalcLocalVarsArrayPrimitiveExpressionRef(sval_u expr, scr_block_s *block)
{
	if ( expr.node->type == ENUM_variable )
		Scr_CalcLocalVarsVariableExpressionRef(expr.node[1], block);
}

void Scr_CopyBlock(scr_block_s *from, scr_block_s **to)
{
	if ( !*to )
		*to = (scr_block_s *)Hunk_AllocateTempMemoryHighInternal(sizeof(scr_block_s));

	memcpy(*to, from, sizeof(scr_block_s));
	(*to)->localVarsPublicCount = 0;
}

void Scr_MergeChildBlocks(scr_block_s **childBlocks, int childCount, scr_block_s *block)
{
	scr_block_s *childBlock;
	unsigned int name;
	int i;
	int localVar;
	int startIndex;

	if ( childCount && !block->abortLevel )
	{
		for ( i = 0; i < childCount; ++i )
		{
			childBlock = childBlocks[i];
			childBlock->localVarsPublicCount = block->localVarsCount;

			for ( startIndex = 0; startIndex < block->localVarsCount; ++startIndex )
			{
				name = block->localVars[startIndex];
				localVar = Scr_FindLocalVar(childBlock, startIndex, name);

				if ( localVar < 0 )
				{
					localVar = childBlock->localVarsCount;
					Scr_CheckLocalVarsCount(localVar);
					++childBlock->localVarsCount;
				}

				while ( localVar > startIndex )
				{
					childBlock->localVars[localVar] = childBlock->localVarsInitBits[localVar + 1];
					--localVar;
				}

				childBlock->localVars[startIndex] = name;
			}
		}
	}
}

void Scr_TransferBlock(scr_block_s *from, scr_block_s *to)
{
	unsigned int name;
	int localVar;
	int startIndex;

	for ( startIndex = 0; startIndex < to->localVarsPublicCount || startIndex < from->localVarsCreateCount; ++startIndex )
	{
		name = from->localVars[startIndex];
		localVar = Scr_FindLocalVar(to, startIndex, name);

		if ( localVar < 0 )
		{
			localVar = to->localVarsCount;
			Scr_CheckLocalVarsCount(localVar);
			++to->localVarsCount;
		}

		if ( localVar >= to->localVarsPublicCount )
			++to->localVarsPublicCount;

		while ( localVar > startIndex )
		{
			to->localVars[localVar] = to->localVarsInitBits[localVar + 1];
			--localVar;
		}

		to->localVars[startIndex] = name;

		if ( (((int)*((unsigned char *)from->localVarsInitBits + (startIndex >> 3)) >> (startIndex & 7)) & 1) != 0 )
			*((byte *)to->localVarsInitBits + (startIndex >> 3)) |= 1 << (startIndex & 7);
	}

	to->localVarsCreateCount = from->localVarsCreateCount;
	to->abortLevel = SCR_ABORT_NONE;
}

void Scr_CheckMaxSwitchCases(int count)
{
	if ( count > 1023 )
		Com_Error(ERR_DROP, "MAX_SWITCH_CASES exceeded");
}

void Scr_AddBreakBlock(scr_block_s *block)
{
	if ( !block->abortLevel )
	{
		if ( scrCompileGlob.breakChildBlocks )
		{
			Scr_CheckMaxSwitchCases(*scrCompileGlob.breakChildCount);
			scrCompileGlob.breakChildBlocks[(*scrCompileGlob.breakChildCount)++] = block;
		}
	}
}

void Scr_AppendChildBlocks(scr_block_s **childBlocks, int childCount, scr_block_s *block)
{
	unsigned int var;
	int i;
	int k;
	int j;

	if ( childCount && !block->abortLevel )
	{
		for ( i = 0; i < childCount; ++i )
			childBlocks[i]->abortLevel = SCR_ABORT_NONE;

		for ( j = 0; j < (*childBlocks)->localVarsCount; ++j )
		{
			var = (*childBlocks)->localVars[j];

			if ( Scr_FindLocalVar(block, 0, var) < 0 )
			{
				for ( k = 1; k < childCount; ++k )
				{
					if ( Scr_FindLocalVar(childBlocks[k], 0, var) < 0 )
						goto out;
				}

				block->localVars[block->localVarsCount++] = var;
			}
out:
			;
		}
	}
}

void Scr_InitFromChildBlocks(scr_block_s **childBlocks, int childCount, scr_block_s *block)
{
	int count;
	scr_block_s *childBlock;
	int i;
	int k;
	int j;

	if ( childCount )
	{
		count = (*childBlocks)->localVarsPublicCount;

		for ( i = 1; i < childCount; ++i )
		{
			childBlock = childBlocks[i];
			if ( childBlock->localVarsPublicCount < count )
				count = childBlock->localVarsPublicCount;
		}

		block->localVarsCreateCount = count;

		for ( j = 0; j < count; ++j )
		{
			if ( (((int)*((unsigned char *)block->localVarsInitBits + (j >> 3)) >> (j & 7)) & 1) == 0 )
			{
				for ( k = 0; k < childCount; ++k )
				{
					if ( (((unsigned char)((int)*((unsigned char *)childBlocks[k]->localVarsInitBits + (j >> 3)) >> (j & 7)) ^ 1) & 1) != 0 )
						goto out;
				}

				*((byte *)block->localVarsInitBits + (j >> 3)) |= 1 << (j & 7);
			}
out:
			;
		}
	}
}

void Scr_CreateVector(VariableCompileValue *constValue, VariableValue *value)
{
	int type;
	int i;
	float vec[3];

	for ( i = 0; i < 3; ++i )
	{
		type = constValue[i].value.type;

		if ( type == VAR_FLOAT )
		{
			vec[2 - i] = constValue[i].value.u.floatValue;
		}
		else
		{
			if ( type != VAR_INTEGER )
			{
				CompileError(constValue[i].sourcePos.sourcePosValue, "type %s is not a float", var_typename[type]);
				return;
			}

			vec[2 - i] = (float)constValue[i].value.u.intValue;
		}
	}

	value->type = VAR_VECTOR;
	value->u.vectorValue = Scr_AllocVector(vec);
}

void Scr_AddContinueBlock(scr_block_s *block)
{
	if ( !block->abortLevel )
	{
		if ( scrCompileGlob.continueChildBlocks )
		{
			Scr_CheckMaxSwitchCases(*scrCompileGlob.continueChildCount);
			scrCompileGlob.continueChildBlocks[(*scrCompileGlob.continueChildCount)++] = block;
		}
	}
}

bool EvalPrimitiveExpression(sval_u expr, VariableCompileValue *constValue)
{
	switch ( expr.node->type )
	{
	case ENUM_integer:
		EvalInteger(expr.node[1].intValue, expr.node[2], constValue);
		return 1;

	case ENUM_float:
		EvalFloat(expr.node[1].floatValue, expr.node[2], constValue);
		return 1;

	case ENUM_minus_integer:
		EvalInteger(-expr.node[1].intValue, expr.node[2], constValue);
		return 1;

	case ENUM_minus_float:
		EvalFloat(-expr.node[1].floatValue, expr.node[2], constValue);
		return 1;

	case ENUM_string:
		EvalString(expr.node[1].stringValue, expr.node[2], constValue);
		return 1;

	case ENUM_istring:
		EvalIString(expr.node[1].stringValue, expr.node[2], constValue);
		return 1;

	case ENUM_undefined:
		EvalUndefined(expr.node[1], constValue);
		return 1;

	case ENUM_expression_list:
		return EvalPrimitiveExpressionList(expr.node[1], expr.node[2], constValue);

	case ENUM_false:
		EvalInteger(0, expr.node[1], constValue);
		return 1;

	case ENUM_true:
		EvalInteger(1, expr.node[1], constValue);
		return 1;

	default:
		return 0;
	}
}

char EvalBinaryOperatorExpression(sval_u expr1, sval_u expr2, sval_u opcode, sval_u sourcePos, VariableCompileValue *constValue)
{
	VariableCompileValue constValue1;
	VariableCompileValue constValue2;

	if ( !EvalExpression(expr1, &constValue1) )
		return 0;

	if ( !EvalExpression(expr2, &constValue2) )
		return 0;

	AddRefToValue(&constValue1.value);
	AddRefToValue(&constValue2.value);

	Scr_EvalBinaryOperator(opcode.type, &constValue1.value, &constValue2.value);

	if ( scrVarPub.error_message )
	{
		CompileError(sourcePos.sourcePosValue, "%s", scrVarPub.error_message);
		return 0;
	}
	else
	{
		constValue->value.u = constValue1.value.u;
		constValue->value.type = constValue1.value.type;
		constValue->sourcePos = sourcePos;
		return 1;
	}
}

bool EvalExpression(sval_u expr, VariableCompileValue *constValue)
{
	if ( expr.node->type == ENUM_primitive_expression )
		return EvalPrimitiveExpression(expr.node[1], constValue);

	if ( expr.node->type == ENUM_binary )
		return EvalBinaryOperatorExpression(expr.node[1], expr.node[2], expr.node[3], expr.node[4], constValue);

	return 0;
}

bool EvalPrimitiveExpressionList(sval_u exprlist, sval_u sourcePos, VariableCompileValue *constValue)
{
	VariableCompileValue constValue2[3];
	int i;
	sval_u *node;
	int count;

	count = GetExpressionCount(exprlist);

	if ( count == 1 )
		return EvalExpression(exprlist.node->node->node[0], constValue);

	if ( count != 3 )
		return 0;

	i = 0;

	for ( node = exprlist.node->node; node; node = node[1].node )
	{
		if ( !EvalExpression(node->node[0], &constValue2[i]) )
			return 0;
		++i;
	}

	Scr_CreateVector(constValue2, &constValue->value);
	constValue->sourcePos = sourcePos;

	return 1;
}

void Scr_CalcLocalVarsIncStatement(sval_u expr, scr_block_s *block)
{
	Scr_CalcLocalVarsVariableExpressionRef(expr, block);
}

void Scr_CalcLocalVarsSwitchStatement(sval_u stmtlist, scr_block_s *block)
{
	int count;
	scr_block_s **childBlocks;
	int abortLevel;
	bool hasDefault;
	scr_block_s *currentBlock;
	int *breakChildCount;
	scr_block_s **breakChildBlocks;
	int newChildCount;
	scr_block_s **newBreakChildBlocks;
	sval_u *node;

	abortLevel = SCR_ABORT_RETURN;
	breakChildBlocks = scrCompileGlob.breakChildBlocks;
	breakChildCount = scrCompileGlob.breakChildCount;
	newChildCount = 0;
	newBreakChildBlocks = (scr_block_s **)Hunk_AllocateTempMemoryHighInternal(4096);
	scrCompileGlob.breakChildBlocks = newBreakChildBlocks;
	scrCompileGlob.breakChildCount = &newChildCount;
	count = 0;
	currentBlock = 0;
	hasDefault = 0;
	childBlocks = (scr_block_s **)Hunk_AllocateTempMemoryHighInternal(4096);

	for ( node = stmtlist.node->node[1].node; node; node = node[1].node )
	{
		if ( node->node->type == ENUM_case || node->node->type == ENUM_default )
		{
			currentBlock = 0;
			Scr_CopyBlock(block, &currentBlock);

			if ( node->node->type == ENUM_case )
			{
				node->node[3].block = currentBlock;
			}
			else
			{
				node->node[2].block = currentBlock;
				hasDefault = 1;
			}
		}
		else if ( currentBlock )
		{
			Scr_CalcLocalVarsStatement(node[0], currentBlock);

			if ( currentBlock->abortLevel )
			{
				if ( currentBlock->abortLevel == SCR_ABORT_BREAK )
				{
					currentBlock->abortLevel = SCR_ABORT_NONE;
					abortLevel = SCR_ABORT_NONE;
					Scr_CheckMaxSwitchCases(count);
					childBlocks[count++] = currentBlock;
				}
				else if ( currentBlock->abortLevel <= abortLevel )
				{
					abortLevel = currentBlock->abortLevel;
				}

				currentBlock = 0;
			}
		}
	}

	if ( hasDefault )
	{
		if ( currentBlock )
		{
			Scr_AddBreakBlock(currentBlock);
			Scr_CheckMaxSwitchCases(count);
			childBlocks[count++] = currentBlock;
		}

		if ( !block->abortLevel )
			block->abortLevel = abortLevel;

		Scr_AppendChildBlocks(newBreakChildBlocks, newChildCount, block);
		Scr_MergeChildBlocks(childBlocks, count, block);
	}

	scrCompileGlob.breakChildBlocks = breakChildBlocks;
	scrCompileGlob.breakChildCount = breakChildCount;
}

void Scr_CalcLocalVarsForStatement(sval_u stmt1, sval_u expr, sval_u stmt2, sval_u stmt, scr_block_s *block, sval_u *forStatBlock, sval_u *forStatPostBlock)
{
	int i;
	int *continueChildCount;
	scr_block_s **continueChildBlocks;
	int newContinueChildCount;
	scr_block_s **newContinueChildBlocks;
	int newBreakChildCount;
	scr_block_s **newBreakChildBlocks;
	int *breakChildCount;
	scr_block_s **breakChildBlocks;
	VariableCompileValue constValue;
	bool constConditional;

	Scr_CalcLocalVarsStatement(stmt1, block);

	if ( expr.node->type == ENUM_expression )
	{
		constConditional = 0;

		if ( EvalExpression(expr.node[1], &constValue) )
		{
			if ( constValue.value.type == VAR_INTEGER || constValue.value.type == VAR_FLOAT )
			{
				Scr_CastBool(&constValue.value);

				if ( constValue.value.u.intValue )
					constConditional = 1;
			}

			RemoveRefToValue(&constValue.value);
		}
	}
	else
	{
		constConditional = 1;
	}

	breakChildBlocks = scrCompileGlob.breakChildBlocks;
	breakChildCount = scrCompileGlob.breakChildCount;
	continueChildBlocks = scrCompileGlob.continueChildBlocks;
	continueChildCount = scrCompileGlob.continueChildCount;
	newBreakChildCount = 0;
	newContinueChildCount = 0;
	newContinueChildBlocks = (scr_block_s **)Hunk_AllocateTempMemoryHighInternal(4096);
	scrCompileGlob.continueChildBlocks = newContinueChildBlocks;
	scrCompileGlob.continueChildCount = &newContinueChildCount;

	if ( constConditional )
	{
		newBreakChildBlocks = (scr_block_s **)Hunk_AllocateTempMemoryHighInternal(4096);
		scrCompileGlob.breakChildCount = &newBreakChildCount;
	}
	else
	{
		newBreakChildBlocks = 0;
	}

	scrCompileGlob.breakChildBlocks = newBreakChildBlocks;
	Scr_CopyBlock(block, (scr_block_s **)forStatBlock);
	Scr_CopyBlock(block, (scr_block_s **)forStatPostBlock);
	Scr_CalcLocalVarsStatement(stmt, forStatBlock->block);
	Scr_AddContinueBlock(forStatBlock->block);

	for ( i = 0; i < newContinueChildCount; ++i )
		Scr_AppendChildBlocks(&newContinueChildBlocks[i], 1, block);

	Scr_CalcLocalVarsStatement(stmt2, forStatPostBlock->block);
	Scr_AppendChildBlocks((scr_block_s **)forStatPostBlock, 1, block);
	Scr_MergeChildBlocks((scr_block_s **)forStatPostBlock, 1, block);

	if ( constConditional )
		Scr_AppendChildBlocks(newBreakChildBlocks, newBreakChildCount, block);

	Scr_MergeChildBlocks((scr_block_s **)forStatBlock, 1, block);
	scrCompileGlob.breakChildBlocks = breakChildBlocks;
	scrCompileGlob.breakChildCount = breakChildCount;
	scrCompileGlob.continueChildBlocks = continueChildBlocks;
	scrCompileGlob.continueChildCount = continueChildCount;
}

void Scr_CalcLocalVarsWhileStatement(sval_u expr, sval_u stmt, scr_block_s *block, sval_u *whileStatBlock)
{
	int breakChildCount;
	int *continueChildCount;
	scr_block_s **continueChildBlocks;
	int initial_count;
	scr_block_s **newBlock;
	int childCount;
	scr_block_s **newChildBlock;
	int *oldBreakChildCount;
	scr_block_s **breakChildBlocks;
	VariableCompileValue constValue;
	bool hasChildren;

	hasChildren = 0;

	if ( EvalExpression(expr, &constValue) )
	{
		if ( constValue.value.type == VAR_INTEGER || constValue.value.type == VAR_FLOAT )
		{
			Scr_CastBool(&constValue.value);

			if ( constValue.value.u.intValue )
				hasChildren = 1;
		}

		RemoveRefToValue(&constValue.value);
	}

	breakChildBlocks = scrCompileGlob.breakChildBlocks;
	oldBreakChildCount = scrCompileGlob.breakChildCount;
	continueChildBlocks = scrCompileGlob.continueChildBlocks;
	continueChildCount = scrCompileGlob.continueChildCount;
	childCount = 0;
	initial_count = 0;
	newBlock = (scr_block_s **)Hunk_AllocateTempMemoryHighInternal(4096);
	scrCompileGlob.continueChildBlocks = newBlock;
	scrCompileGlob.continueChildCount = &initial_count;

	if ( hasChildren )
	{
		newChildBlock = (scr_block_s **)Hunk_AllocateTempMemoryHighInternal(4096);
		scrCompileGlob.breakChildCount = &childCount;
	}
	else
	{
		newChildBlock = 0;
	}

	scrCompileGlob.breakChildBlocks = newChildBlock;
	Scr_CopyBlock(block, (scr_block_s **)whileStatBlock);
	Scr_CalcLocalVarsStatement(stmt, whileStatBlock->block);
	Scr_AddContinueBlock(whileStatBlock->block);

	for ( breakChildCount = 0; breakChildCount < initial_count; ++breakChildCount )
		Scr_AppendChildBlocks(&newBlock[breakChildCount], 1, block);
	if ( hasChildren )
		Scr_AppendChildBlocks(newChildBlock, childCount, block);

	Scr_MergeChildBlocks((scr_block_s **)whileStatBlock, 1, block);
	scrCompileGlob.breakChildBlocks = breakChildBlocks;
	scrCompileGlob.breakChildCount = oldBreakChildCount;
	scrCompileGlob.continueChildBlocks = continueChildBlocks;
	scrCompileGlob.continueChildCount = continueChildCount;
}


void Scr_CalcLocalVarsIfElseStatement(sval_u stmt1, sval_u stmt2, scr_block_s *block, sval_u *ifStatBlock, sval_u *elseStatBlock)
{
	int abortLevel;
	int childCount;
	scr_block_s *childBlocks[2];

	childCount = 0;
	abortLevel = SCR_ABORT_RETURN;

	Scr_CopyBlock(block, (scr_block_s **)ifStatBlock);
	Scr_CalcLocalVarsStatement(stmt1, ifStatBlock->block);

	if ( ifStatBlock->node->intValue < 4 )
	{
		abortLevel = ifStatBlock->node->intValue;

		if ( !abortLevel )
		{
			childBlocks[0] = ifStatBlock->block;
			childCount = 1;
		}
	}

	Scr_CopyBlock(block, (scr_block_s **)elseStatBlock);
	Scr_CalcLocalVarsStatement(stmt2, elseStatBlock->block);

	if ( elseStatBlock->node->intValue <= abortLevel )
	{
		abortLevel = elseStatBlock->node->intValue;

		if ( !abortLevel )
			childBlocks[childCount++] = elseStatBlock->block;
	}

	if ( !block->abortLevel )
		block->abortLevel = abortLevel;

	Scr_AppendChildBlocks(childBlocks, childCount, block);
	Scr_MergeChildBlocks(childBlocks, childCount, block);
}

void Scr_CalcLocalVarsIfStatement(sval_u stmt, scr_block_s *block, sval_u *ifStatBlock)
{
	Scr_CopyBlock(block, (scr_block_s **)ifStatBlock);
	Scr_CalcLocalVarsStatement(stmt, ifStatBlock->block);
	Scr_MergeChildBlocks((scr_block_s **)ifStatBlock, 1, block);
}

void Scr_CalcLocalVarsStatementList(sval_u val, scr_block_s *block)
{
	sval_u *node;

	for ( node = val.node->node[1].node; node; node = node[1].node )
		Scr_CalcLocalVarsStatement(node[0], block);
}

void Scr_CalcLocalVarsDeveloperStatementList(sval_u val, scr_block_s *block, sval_u *devStatBlock)
{
	Scr_CopyBlock(block, (scr_block_s **)devStatBlock);
	Scr_CalcLocalVarsStatementList(val, devStatBlock->block);
	Scr_MergeChildBlocks((scr_block_s **)devStatBlock, 1, block);
}

void Scr_CalcLocalVarsThread(sval_u exprlist, sval_u stmtlist, sval_u *stmttblock)
{
	scrCompileGlob.forceNotCreate = 0;
	stmttblock->block = (scr_block_s *)Hunk_AllocateTempMemoryHighInternal(sizeof(scr_block_s));
	stmttblock->block->abortLevel = SCR_ABORT_NONE;
	stmttblock->block->localVarsCreateCount = 0;
	stmttblock->block->localVarsCount = 0;
	stmttblock->block->localVarsPublicCount = 0;
	memset(stmttblock->block->localVarsInitBits, 0, sizeof(stmttblock->block->localVarsInitBits));
	Scr_CalcLocalVarsFormalParameterList(exprlist, stmttblock->block);
	Scr_CalcLocalVarsStatementList(stmtlist, stmttblock->block);
}

void Scr_CalcLocalVarsStatement(sval_u val, scr_block_s *block)
{
	switch ( val.node->type )
	{
	case ENUM_assignment:
		Scr_CalcLocalVarsAssignmentStatement(val.node[1], val.node[2], block);
		break;

	case ENUM_return:
	case ENUM_return2:
		if ( block->abortLevel == SCR_ABORT_NONE )
			block->abortLevel = SCR_ABORT_RETURN;
		break;

	case ENUM_if:
		Scr_CalcLocalVarsIfStatement(val.node[2], block, &val.node[4]);
		break;

	case ENUM_if_else:
		Scr_CalcLocalVarsIfElseStatement(val.node[2], val.node[3], block, &val.node[6], &val.node[7]);
		break;

	case ENUM_while:
		Scr_CalcLocalVarsWhileStatement(val.node[1], val.node[2], block, &val.node[5]);
		break;

	case ENUM_for:
		Scr_CalcLocalVarsForStatement(val.node[1], val.node[2], val.node[3], val.node[4], block, &val.node[7], &val.node[8]);
		break;

	case ENUM_inc:
	case ENUM_dec:
	case ENUM_binary_equals:
		Scr_CalcLocalVarsIncStatement(val.node[1], block);
		break;

	case ENUM_statement_list:
		Scr_CalcLocalVarsStatementList(val.node[1], block);
		break;

	case ENUM_developer_statement_list:
		Scr_CalcLocalVarsDeveloperStatementList(val.node[1], block, &val.node[3]);
		break;

	case ENUM_waittill:
		Scr_CalcLocalVarsWaittillStatement(val.node[2], block);
		break;

	case ENUM_switch:
		Scr_CalcLocalVarsSwitchStatement(val.node[2], block);
		break;

	case ENUM_break:
		Scr_AddBreakBlock(block);
		if ( block->abortLevel == SCR_ABORT_NONE )
			block->abortLevel = SCR_ABORT_BREAK;
		break;

	case ENUM_continue:
		Scr_AddContinueBlock(block);
		if ( block->abortLevel == SCR_ABORT_NONE )
			block->abortLevel = SCR_ABORT_CONTINUE;
		break;

	default:
		return;
	}
}

sval_u *GetSingleParameter(sval_u exprlist)
{
	if ( !exprlist.node->node )
		return 0;

	if ( exprlist.node->node[1].node )
		return 0;

	return exprlist.node->node;
}

void EmitCastFieldObject(sval_u sourcePos)
{
	EmitOpcode(OP_CastFieldObject, -1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
}

void EmitSelfObject(sval_u sourcePos)
{
	EmitOpcode(OP_GetSelfObject, 0, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
}

void EmitLevelObject(sval_u sourcePos)
{
	EmitOpcode(OP_GetLevelObject, 0, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
}

void EmitAnimObject(sval_u sourcePos)
{
	EmitOpcode(OP_GetAnimObject, 0, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
}

void EmitBoolComplement(sval_u sourcePos)
{
	EmitOpcode(OP_BoolComplement, 0, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
}

void EmitBoolNot(sval_u sourcePos)
{
	EmitOpcode(OP_BoolNot, 0, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
}

void EmitDecTop()
{
	EmitOpcode(OP_DecTop, -1, CALL_NONE);
}

void EmitEnd()
{
	EmitOpcode(OP_End, 0, CALL_NONE);
}

void EmitSafeSetVariableField(sval_u expr, sval_u sourcePos, scr_block_s *block)
{
	int opcode;
	int index;

	index = Scr_FindLocalVarIndex(expr.sourcePosValue, sourcePos, 1, block);

	if ( index )
		opcode = OP_SafeSetVariableFieldCached;
	else
		opcode = OP_SafeSetVariableFieldCached0;

	EmitOpcode(opcode, 0, CALL_NONE);

	if ( index )
		EmitByte(index);

	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
}

void EmitCanonicalStringConst(unsigned int stringValue)
{
	bool count;

	count = scrCompileGlob.bConstRefCount;
	scrCompileGlob.bConstRefCount = 1;
	EmitCanonicalString(stringValue);
	scrCompileGlob.bConstRefCount = count;
}

void EmitEvalArray(sval_u sourcePos, sval_u indexSourcePos)
{
	EmitOpcode(OP_EvalArray, -1, CALL_NONE);
	AddOpcodePos(indexSourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
}

unsigned int Scr_GetBuiltin(sval_u func_name)
{
	sval_u *func_namea;
	sval_u *func_nameb;

	if ( func_name.node->type != ENUM_script_call )
		return 0;

	func_namea = func_name.node[1].node;

	if ( func_namea->type != ENUM_function )
		return 0;

	func_nameb = func_namea[1].node;

	if ( func_nameb->type != ENUM_local_function )
		return 0;

	if ( FindVariable(scrCompileGlob.filePosId, func_nameb[1].idValue) )
		return 0;

	return func_nameb[1].idValue;
}

int Scr_FindLocalVarIndex(unsigned int name, sval_u sourcePos, bool create, scr_block_s *block)
{
	int i;

	if ( !block )
		goto unreachable;

	for ( i = 0; ; ++i )
	{
		if ( i >= block->localVarsCount )
			goto out;

		if ( i == block->localVarsCreateCount )
		{
			++block->localVarsCreateCount;
			EmitOpcode(OP_CreateLocalVariable, 0, CALL_NONE);
			EmitCanonicalStringConst(block->localVars[i]);
		}

		if ( block->localVars[i] == name )
			break;
	}

	Scr_CompileRemoveRefToString(name);

	if ( (*((byte *)block->localVarsInitBits + (i >> 3)) & (unsigned char)(1 << (i & 7))) != 0 )
		return block->localVarsCreateCount - i - 1;

	if ( create && !scrCompileGlob.forceNotCreate )
	{
		*((byte *)block->localVarsInitBits + (i >> 3)) |= 1 << (i & 7);
		return block->localVarsCreateCount - i - 1;
	}
out:
	if ( !create || scrCompileGlob.forceNotCreate )
	{
		CompileError(sourcePos.sourcePosValue, "uninitialised variable '%s'", SL_ConvertToString(name));
		return 0;
	}
	else
	{
unreachable:
		CompileError(sourcePos.sourcePosValue, "unreachable code");
		return 0;
	}
}

void AddExpressionListOpcodePos(sval_u exprlist)
{
	sval_u *node;

	if ( scrVarPub.developer )
	{
		for ( node = exprlist.node->node; node; node = node[1].node )
			AddOpcodePos(node->node[1].sourcePosValue, SOURCE_TYPE_NONE);
	}
}

void Scr_BeginDevScript(int *type, char **savedPos)
{
	if ( scrCompilePub.developer_statement )
	{
		*type = BUILTIN_ANY;
	}
	else
	{
		if ( scrVarPub.developer_script )
		{
			scrCompilePub.developer_statement = 1;
		}
		else
		{
			*savedPos = (char *)TempMalloc(0);
			scrCompilePub.developer_statement = 2;
		}

		*type = BUILTIN_DEVELOPER_ONLY;
	}
}

void Scr_EndDevScript(int type, char **savedPos)
{
	if ( type == BUILTIN_DEVELOPER_ONLY )
	{
		scrCompilePub.developer_statement = 0;

		if ( !scrVarPub.developer_script )
			TempMemorySetPos(*savedPos);
	}
}

int Scr_GetCacheType(int type)
{
	if ( type )
		return VAR_DEVELOPER_CODEPOS;
	else
		return VAR_CODEPOS;
}

bool Scr_IsVariable(int type)
{
	return type != VAR_CODEPOS;
}

void EmitCallBuiltinOpcode(int param_count, sval_u sourcePos)
{
	int opcode;

	if ( param_count > 5 )
		opcode = OP_CallBuiltin;
	else
		opcode = param_count + OP_CallBuiltin0;

	EmitOpcode(opcode, 1 - param_count, CALL_BUILTIN);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);

	if ( opcode == OP_CallBuiltin )
		EmitByte(param_count);
}

void EmitCallBuiltinMethodOpcode(int param_count, sval_u sourcePos)
{
	int opcode;

	if ( param_count > 5 )
		opcode = OP_CallBuiltinMethod;
	else
		opcode = param_count + OP_CallBuiltinMethod0;

	EmitOpcode(opcode, -param_count, CALL_BUILTIN);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);

	if ( opcode == OP_CallBuiltinMethod )
		EmitByte(param_count);
}

void EmitSelf(sval_u sourcePos)
{
	EmitOpcode(OP_GetSelf, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
}

void EmitLevel(sval_u sourcePos)
{
	EmitOpcode(OP_GetLevel, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
}

void EmitGame(sval_u sourcePos)
{
	EmitOpcode(OP_GetGame, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
}

void EmitAnim(sval_u sourcePos)
{
	EmitOpcode(OP_GetAnim, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
}

void EmitEmptyArray(sval_u sourcePos)
{
	EmitOpcode(OP_EmptyArray, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
}

void EmitAnimation(sval_u anim, sval_u sourcePos)
{
	EmitOpcode(OP_GetAnimation, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
	EmitCodepos((const char *)0xFFFFFFFF);
	Scr_EmitAnimation(scrCompileGlob.codePos, anim.stringValue, sourcePos.sourcePosValue);
	Scr_CompileRemoveRefToString(anim.stringValue);
}

void EmitCastBool(sval_u sourcePos)
{
	EmitOpcode(OP_CastBool, 0, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
}

void EmitAnimTree(sval_u sourcePos)
{
	if ( scrAnimPub.animTreeIndex )
		EmitGetInteger(scrAnimPub.animTreeIndex, sourcePos);
	else
		CompileError(sourcePos.sourcePosValue, "#using_animtree was not specified");
}

void EmitBreakOn(sval_u expr, sval_u param, sval_u sourcePos)
{
	CompileError(sourcePos.sourcePosValue, "illegal function name");
}

void Scr_PushValue(VariableCompileValue *constValue)
{
	int index;

	if ( scrCompilePub.value_count < 32 )
	{
		index = scrCompilePub.value_count;
		scrCompileGlob.value_start[index].value.u = constValue->value.u;
		scrCompileGlob.value_start[index].value.type = constValue->value.type;
		scrCompileGlob.value_start[index].sourcePos.sourcePosValue = constValue->sourcePos.sourcePosValue;
		++scrCompilePub.value_count;
	}
	else
	{
		CompileError(constValue->sourcePos.sourcePosValue, "VALUE_STACK_SIZE exceeded");
	}
}

void Scr_PopValue()
{
	--scrCompilePub.value_count;
}

bool EmitOrEvalPrimitiveExpressionList(sval_u exprlist, sval_u sourcePos, VariableCompileValue *constValue, scr_block_s *block)
{
	VariableCompileValue constValue2;
	bool success;
	sval_u *node;
	int expr_count;

	expr_count = GetExpressionCount(exprlist);

	if ( expr_count == 1 )
		return EmitOrEvalExpression(exprlist.node->node->node[0], constValue, block);

	if ( expr_count == 3 )
	{
		success = 1;

		for ( node = exprlist.node->node; node; node = node[1].node )
		{
			if ( success )
			{
				success = EmitOrEvalExpression(node->node[0], &constValue2, block);

				if ( success )
					Scr_PushValue(&constValue2);
			}
			else
			{
				EmitExpression(node->node[0], block);
			}
		}

		if ( success )
		{
			scrCompilePub.value_count -= 3;
			Scr_CreateVector(&scrCompileGlob.value_start[scrCompilePub.value_count], &constValue->value);
			constValue->sourcePos = sourcePos;
			return 1;
		}
		else
		{
			EmitOpcode(OP_vector, -2, CALL_NONE);
			AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
			AddExpressionListOpcodePos(exprlist);
			return 0;
		}
	}
	else
	{
		CompileError(sourcePos.sourcePosValue, "expression list must have 1 or 3 parameters");
		return 0;
	}
}

void EmitBoolOrExpression(sval_u expr1, sval_u expr2, sval_u expr1sourcePos, sval_u expr2sourcePos, scr_block_s *block)
{
	unsigned short nextPos;
	char *pos;

	EmitExpression(expr1, block);
	EmitOpcode(OP_JumpOnTrueExpr, -1, CALL_NONE);
	AddOpcodePos(expr1sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	EmitShort(0);
	pos = scrCompileGlob.codePos;
	nextPos = (intptr_t)TempMalloc(0);
	EmitExpression(expr2, block);
	EmitCastBool(expr2sourcePos);
	*(uint16_t *)pos = (intptr_t)TempMalloc(0) - nextPos;
}

void EmitBoolAndExpression(sval_u expr1, sval_u expr2, sval_u expr1sourcePos, sval_u expr2sourcePos, scr_block_s *block)
{
	unsigned short nextPos;
	char *pos;

	EmitExpression(expr1, block);
	EmitOpcode(OP_JumpOnFalseExpr, -1, CALL_NONE);
	AddOpcodePos(expr1sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	EmitShort(0);
	pos = scrCompileGlob.codePos;
	nextPos = (intptr_t)TempMalloc(0);
	EmitExpression(expr2, block);
	EmitCastBool(expr2sourcePos);
	*(uint16_t *)pos = (intptr_t)TempMalloc(0) - nextPos;
}

bool EmitOrEvalBinaryOperatorExpression(sval_u expr1, sval_u expr2, sval_u opcode, sval_u sourcePos, VariableCompileValue *constValue, scr_block_s *block)
{
	VariableCompileValue constValue2;
	VariableCompileValue constValue1;

	if ( !EmitOrEvalExpression(expr1, &constValue1, block) )
	{
		EmitExpression(expr2, block);
		EmitOpcode(opcode.type, -1, CALL_NONE);
		AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
		return 0;
	}

	Scr_PushValue(&constValue1);

	if ( !EmitOrEvalExpression(expr2, &constValue2, block) )
	{
		EmitOpcode(opcode.type, -1, CALL_NONE);
		AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
		return 0;
	}

	Scr_PopValue();
	Scr_EvalBinaryOperator(opcode.intValue, &constValue1.value, &constValue2.value);

	if ( scrVarPub.error_message )
	{
		CompileError(sourcePos.sourcePosValue, "%s", scrVarPub.error_message);
		return 0;
	}
	else
	{
		constValue->value.u = constValue1.value.u;
		constValue->value.type = constValue1.value.type;
		constValue->sourcePos = sourcePos;
		return 1;
	}
}

void EmitSize(sval_u expr, sval_u sourcePos, scr_block_s *block)
{
	EmitPrimitiveExpression(expr, block);
	EmitOpcode(OP_size, 0, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
}

void EmitGetFunction(sval_u func, sval_u sourcePos)
{
	EmitOpcode(OP_GetFunction, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT|SOURCE_TYPE_CALL);
	EmitFunction(func, sourcePos);
}

void EmitCallExpression(sval_u expr, bool bStatement, scr_block_s *block)
{
	if ( expr.node->type == ENUM_call )
	{
		EmitCall(expr.node[1], expr.node[2], bStatement, block);
	}
	else if ( expr.node->type == ENUM_method )
	{
		EmitMethod(expr.node[1], expr.node[2], expr.node[3], expr.node[4], bStatement, block);
	}
}

bool EmitOrEvalPrimitiveExpression(sval_u expr, VariableCompileValue *constValue, scr_block_s *block)
{
	switch ( expr.node->type )
	{
	case ENUM_variable:
		EmitVariableExpression(expr.node[1], block);
		return 0;

	case ENUM_function:
		EmitGetFunction(expr.node[1], expr.node[2]);
		return 0;

	case ENUM_call_expression:
		EmitCallExpression(expr.node[1], 0, block);
		return 0;

	case ENUM_self:
		EmitSelf(expr.node[1]);
		return 0;

	case ENUM_level:
		EmitLevel(expr.node[1]);
		return 0;

	case ENUM_game:
		EmitGame(expr.node[1]);
		return 0;

	case ENUM_anim:
		EmitAnim(expr.node[1]);
		return 0;

	case ENUM_expression_list:
		return EmitOrEvalPrimitiveExpressionList(expr.node[1], expr.node[2], constValue, block);

	case ENUM_size_field:
		EmitSize(expr.node[1], expr.node[2], block);
		return 0;

	case ENUM_empty_array:
		EmitEmptyArray(expr.node[1]);
		return 0;

	case ENUM_animation:
		EmitAnimation(expr.node[1], expr.node[2]);
		return 0;

	case ENUM_animtree:
		EmitAnimTree(expr.node[1]);
		return 0;

	case ENUM_breakon:
		EmitBreakOn(expr.node[1], expr.node[2], expr.node[3]);
		return 0;

	default:
		return EvalPrimitiveExpression(expr, constValue);
	}
}

bool EmitOrEvalExpression(sval_u expr, VariableCompileValue *constValue, scr_block_s *block)
{
	switch ( expr.node->type )
	{
	case ENUM_primitive_expression:
		return EmitOrEvalPrimitiveExpression(expr.node[1], constValue, block);

	case ENUM_bool_or:
		EmitBoolOrExpression(expr.node[1], expr.node[2], expr.node[3], expr.node[4], block);
		return 0;

	case ENUM_bool_and:
		EmitBoolAndExpression(expr.node[1], expr.node[2], expr.node[3], expr.node[4], block);
		return 0;

	case ENUM_binary:
		return EmitOrEvalBinaryOperatorExpression(expr.node[1], expr.node[2], expr.node[3], expr.node[4], constValue, block);

	case ENUM_bool_not:
		EmitExpression(expr.node[1], block);
		EmitBoolNot(expr.node[2]);
		return 0;

	case ENUM_bool_complement:
		EmitExpression(expr.node[1], block);
		EmitBoolComplement(expr.node[2]);
		return 0;

	default:
		return 0;
	}
}

void EmitPostScriptFunction(sval_u func, int param_count, bool bMethod, sval_u nameSourcePos)
{
	if ( bMethod )
		EmitOpcode(OP_ScriptMethodCall, -param_count - 1, CALL_FUNCTION);
	else
		EmitOpcode(OP_ScriptFunctionCall, -param_count, CALL_FUNCTION);

	AddOpcodePos(nameSourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT|SOURCE_TYPE_CALL);
	EmitFunction(func, nameSourcePos);
}

void EmitPostScriptFunctionPointer(sval_u expr, int param_count, bool bMethod, sval_u nameSourcePos, sval_u sourcePos, scr_block_s *block)
{
	EmitExpression(expr, block);

	if ( bMethod )
		EmitOpcode(OP_ScriptMethodCallPointer, -param_count - 2, CALL_FUNCTION);
	else
		EmitOpcode(OP_ScriptFunctionCallPointer, -param_count - 1, CALL_FUNCTION);

	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	AddOpcodePos(nameSourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
}

void EmitPostScriptFunctionCall(sval_u func_name, int param_count, bool bMethod, sval_u nameSourcePos, scr_block_s *block)
{
	if ( func_name.node->type == ENUM_function )
	{
		EmitPostScriptFunction(func_name.node[1], param_count, bMethod, nameSourcePos);
	}
	else if ( func_name.node->type == ENUM_function_pointer )
	{
		EmitPostScriptFunctionPointer(func_name.node[1], param_count, bMethod, nameSourcePos, func_name.node[2], block);
	}
}

void EmitPostScriptThread(sval_u func, int param_count, bool bMethod, sval_u sourcePos)
{
	if ( bMethod )
		EmitOpcode(OP_ScriptMethodThreadCall, -param_count, CALL_THREAD);
	else
		EmitOpcode(OP_ScriptThreadCall, 1 - param_count, CALL_THREAD);

	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT|SOURCE_TYPE_CALL);
	EmitFunction(func, sourcePos);
	EmitCodepos((const char *)param_count);
}

void EmitPostScriptThreadPointer(sval_u expr, int param_count, bool bMethod, sval_u sourcePos, scr_block_s *block)
{
	EmitExpression(expr, block);

	if ( bMethod )
		EmitOpcode(OP_ScriptMethodThreadCallPointer, -param_count - 1, CALL_THREAD);
	else
		EmitOpcode(OP_ScriptThreadCallPointer, -param_count, CALL_THREAD);

	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
	EmitCodepos((const char *)param_count);
}


void EmitPostScriptThreadCall(sval_u func_name, int param_count, bool bMethod, sval_u sourcePos, sval_u nameSourcePos, scr_block_s *block)
{
	if ( func_name.node->type == ENUM_function )
	{
		EmitPostScriptThread(func_name.node[1], param_count, bMethod, nameSourcePos);
	}
	else if ( func_name.node->type == ENUM_function_pointer )
	{
		EmitPostScriptThreadPointer(func_name.node[1], param_count, bMethod, func_name.node[2], block);
	}

	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
}

void EmitFieldVariable(sval_u expr, sval_u field, sval_u sourcePos, scr_block_s *block)
{
	EmitPrimitiveExpressionFieldObject(expr, sourcePos, block);
	EmitOpcode(OP_EvalFieldVariable, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	EmitCanonicalString(field.stringValue);
}

void EmitCallExpressionFieldObject(sval_u expr, scr_block_s *block)
{
	if ( expr.node->type == ENUM_call )
	{
		EmitCall(expr.node[1], expr.node[2], 0, block);
		EmitCastFieldObject(expr.node[3]);
	}
	else if ( expr.node->type == ENUM_method )
	{
		EmitMethod(expr.node[1], expr.node[2], expr.node[3], expr.node[4], 0, block);
		EmitCastFieldObject(expr.node[5]);
	}
}

void EmitObject(sval_u expr, sval_u sourcePos)
{
	int type;
	const char *s;
	unsigned int idValue;
	const char *entnum;
	const char *classnum;

	if ( scrCompilePub.script_loading )
	{
		CompileError(sourcePos.sourcePosValue, "$ can only be used in the script debugger");
		return;
	}

	s = SL_ConvertToString(expr.stringValue);

	if ( *s == 116 )
	{
		idValue = atoi(s + 1);

		if ( idValue )
		{
			if ( idValue <= 0xFFFD && !IsObjectFree((unsigned short)idValue) )
			{
				type = Scr_GetObjectType((unsigned short)idValue);

				if ( type >= VAR_THREAD && (type <= VAR_CHILD_THREAD || type == VAR_REMOVED_THREAD) )
				{
					EmitOpcode(OP_thread_object, 1, CALL_NONE);
					EmitShort(idValue);
					return;
				}
			}
		}

		goto error;
	}

	classnum = (const char *)Scr_GetClassnumForCharId(*s);

	if ( (intptr_t)classnum < 0 || (entnum = (const char *)atoi(s + 1)) == 0 && s[1] != 48 )
	{
error:
		CompileError(sourcePos.sourcePosValue, "bad expression");
		return;
	}

	EmitOpcode(OP_object, 1, CALL_NONE);
	EmitCodepos(classnum);
	EmitCodepos(entnum);
}

void EmitLocalVariable(sval_u expr, sval_u sourcePos, scr_block_s *block)
{
	int opcode;
	int index;

	index = Scr_FindLocalVarIndex(expr.sourcePosValue, sourcePos, 0, block);

	if ( index > OP_GetNegByte )
		opcode = OP_EvalLocalVariableCached;
	else
		opcode = index + OP_EvalLocalVariableCached0;

	EmitOpcode(opcode, 1, CALL_NONE);

	if ( opcode == OP_EvalLocalVariableCached )
		EmitByte(index);

	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
}

void EmitFormalParameterListInternal(sval_u *node, scr_block_s *block)
{
	while ( 1 )
	{
		node = node[1].node;

		if ( !node )
			break;

		EmitSafeSetVariableField(node->node[0], node->node[1], block);
	}
}

void EmitFormalParameterList(sval_u exprlist, sval_u sourcePos, scr_block_s *block)
{
	EmitFormalParameterListInternal(exprlist.node->node, block);
	EmitOpcode(OP_checkclearparams, 0, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
}

bool Scr_IsLastStatement(sval_u *node)
{
	if ( !node )
		return 1;

	if ( scrVarPub.developer_script )
		return 0;

	while ( node )
	{
		if ( node->node->type != ENUM_developer_statement_list )
			return 0;

		node = node[1].node;
	}

	return 1;
}

bool IsUndefinedPrimitiveExpression(sval_u expr)
{
	return expr.node->type == ENUM_undefined;
}

bool IsUndefinedExpression(sval_u expr)
{
	if ( expr.node->type == ENUM_primitive_expression && IsUndefinedPrimitiveExpression(expr.node[1]) )
		return true;

	return false;
}

void EmitEvalArrayRef(sval_u sourcePos, sval_u indexSourcePos)
{
	EmitOpcode(OP_EvalArrayRef, -1, CALL_NONE);
	AddOpcodePos(indexSourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
}

void EmitArrayVariableRef(sval_u expr, sval_u index, sval_u sourcePos, sval_u indexSourcePos, scr_block_s *block)
{
	EmitExpression(index, block);
	EmitArrayPrimitiveExpressionRef(expr, sourcePos, block);
	EmitEvalArrayRef(sourcePos, indexSourcePos);
}

void EmitLocalVariableRef(sval_u expr, sval_u sourcePos, scr_block_s *block)
{
	int opcode;
	int index;

	index = Scr_FindLocalVarIndex(expr.idValue, sourcePos, 1, block);

	if ( index )
		opcode = OP_EvalLocalVariableRefCached;
	else
		opcode = OP_EvalLocalVariableRefCached0;

	EmitOpcode(opcode, 0, CALL_NONE);

	if ( index )
		EmitByte(index);

	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
}

void EmitFieldVariableRef(sval_u expr, sval_u field, sval_u sourcePos, scr_block_s *block)
{
	EmitPrimitiveExpressionFieldObject(expr, sourcePos, block);
	EmitOpcode(OP_EvalFieldVariableRef, 0, CALL_NONE);
	EmitCanonicalString(field.stringValue);
}

void EmitVariableExpressionRef(sval_u expr, scr_block_s *block)
{
	if ( expr.node->type == ENUM_field_variable )
	{
		EmitFieldVariableRef(expr.node[1], expr.node[2], expr.node[3], block);
	}
	else if ( expr.node->type > ENUM_field_variable )
	{
		if ( expr.node->type == ENUM_self_field || expr.node->type == ENUM_object )
		{
			if ( scrCompilePub.script_loading )
				CompileError(expr.node[2].sourcePosValue, "$ and self field can only be used in the script debugger");
			else
				CompileError(expr.node[2].sourcePosValue, "not an lvalue");
		}
	}
	else if ( expr.node->type == ENUM_local_variable )
	{
		EmitLocalVariableRef(expr.node[1], expr.node[2], block);
	}
	else if ( expr.node->type == ENUM_array_variable )
	{
		EmitArrayVariableRef(expr.node[1], expr.node[2], expr.node[3], expr.node[4], block);
	}
}


void EmitGameRef(sval_u sourcePos)
{
	EmitOpcode(OP_GetGameRef, 0, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
}

void EmitArrayPrimitiveExpressionRef(sval_u expr, sval_u sourcePos, scr_block_s *block)
{
	if ( expr.node->type == ENUM_variable )
	{
		EmitVariableExpressionRef(expr.node[1], block);
	}
	else if ( expr.node->type == ENUM_game )
	{
		EmitGameRef(expr.node[1]);
	}
	else
	{
		CompileError(sourcePos.sourcePosValue, "not an lvalue");
	}
}

void EmitClearFieldVariable(sval_u expr, sval_u field, sval_u sourcePos, sval_u rhsSourcePos, scr_block_s *block)
{
	EmitPrimitiveExpressionFieldObject(expr, sourcePos, block);
	EmitOpcode(OP_ClearFieldVariable, 0, CALL_NONE);
	AddOpcodePos(rhsSourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	EmitCanonicalString(field.stringValue);
}

void EmitClearArray(sval_u sourcePos, sval_u indexSourcePos)
{
	EmitOpcode(OP_ClearArray, -1, CALL_NONE);
	AddOpcodePos(indexSourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
}

void EmitSetVariableField(sval_u sourcePos)
{
	EmitOpcode(OP_SetVariableField, -1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
}

void EmitClearArrayVariable(sval_u expr, sval_u index, sval_u sourcePos, sval_u indexSourcePos, scr_block_s *block)
{
	EmitExpression(index, block);
	EmitArrayPrimitiveExpressionRef(expr, sourcePos, block);
	EmitClearArray(sourcePos, indexSourcePos);
}

bool EmitClearVariableExpression(sval_u expr, sval_u rhsSourcePos, scr_block_s *block)
{
	if ( expr.node->type == ENUM_field_variable )
	{
		EmitClearFieldVariable(expr.node[1], expr.node[2], expr.node[3], rhsSourcePos, block);
		return 1;
	}

	if ( expr.node->type > ENUM_field_variable )
	{
		if ( expr.node->type == ENUM_self_field || expr.node->type == ENUM_object )
		{
			if ( scrCompilePub.script_loading )
				CompileError(expr.node[2].sourcePosValue, "$ and self field can only be used in the script debugger");
			else
				CompileError(expr.node[2].sourcePosValue, "not an lvalue");
		}
		return 1;
	}

	if ( expr.node->type != ENUM_local_variable )
	{
		if ( expr.node->type == ENUM_array_variable )
			EmitClearArrayVariable(expr.node[1], expr.node[2], expr.node[3], expr.node[4], block);

		return 1;
	}

	return 0;
}

void EmitCallExpressionStatement(sval_u expr, scr_block_s *block)
{
	EmitCallExpression(expr, 1, block);
}

void EmitReturn()
{
	EmitOpcode(OP_Return, -1, CALL_NONE);
}

void EmitEndStatement(sval_u sourcePos, scr_block_s *block)
{
	if ( block->abortLevel == SCR_ABORT_NONE )
		block->abortLevel = SCR_ABORT_RETURN;

	EmitEnd();
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
}

void EmitRemoveLocalVars(scr_block_s *block, scr_block_s *outerBlock)
{
	int removeCount;

	if ( block->abortLevel == SCR_ABORT_NONE )
	{
		removeCount = block->localVarsCreateCount - outerBlock->localVarsPublicCount;

		if ( removeCount )
		{
			EmitOpcode(OP_RemoveLocalVariables, 0, CALL_NONE);
			EmitByte(removeCount);
			block->localVarsCreateCount = block->localVarsPublicCount;
		}
	}
}

void EmitNOP2(bool lastStatement, unsigned int endSourcePos, scr_block_s *block)
{
	unsigned int checksum;

	checksum = scrVarPub.checksum;

	if ( lastStatement )
	{
		EmitEnd();
		AddOpcodePos(endSourcePos, SOURCE_TYPE_BREAKPOINT);
	}
	else
	{
		EmitRemoveLocalVars(block, block);
	}

	scrVarPub.checksum = checksum + 1;
}

void EmitIfStatement(sval_u expr, sval_u stmt, sval_u sourcePos, bool lastStatement, unsigned int endSourcePos, scr_block_s *block, sval_u *ifStatBlock)
{
	unsigned short nextPos;
	char *pos;

	EmitExpression(expr, block);
	EmitOpcode(OP_JumpOnFalse, -1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	EmitShort(0);
	pos = scrCompileGlob.codePos;
	nextPos = (intptr_t)TempMalloc(0);
	Scr_TransferBlock(block, ifStatBlock->block);
	EmitStatement(stmt, lastStatement, endSourcePos, ifStatBlock->block);
	EmitNOP2(lastStatement, endSourcePos, ifStatBlock->block);
	*(uint16_t *)pos = (intptr_t)TempMalloc(0) - nextPos;
}

void EmitWaitStatement(sval_u expr, sval_u sourcePos, sval_u waitSourcePos, scr_block_s *block)
{
	EmitExpression(expr, block);
	EmitOpcode(OP_wait, -1, CALL_NONE);
	AddOpcodePos(waitSourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	AddOpcodePos(waitSourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
}

void EmitReturnStatement(sval_u expr, sval_u sourcePos, scr_block_s *block)
{
	if ( block->abortLevel == SCR_ABORT_NONE )
		block->abortLevel = SCR_ABORT_RETURN;

	EmitExpression(expr, block);
	EmitReturn();
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
}

void EmitAssignmentStatement(sval_u lhs, sval_u rhs, sval_u sourcePos, sval_u rhsSourcePos, scr_block_s *block)
{
	if ( !IsUndefinedExpression(rhs) || !EmitClearVariableExpression(lhs, rhsSourcePos, block) )
	{
		EmitExpression(rhs, block);
		EmitVariableExpressionRef(lhs, block);
		EmitSetVariableField(sourcePos);
	}
}

void EmitCaseStatementInfo(unsigned int name, sval_u sourcePos)
{
	sval_u *statement;

	if ( scrCompilePub.developer_statement != 2 )
	{
		statement = (sval_u *)Hunk_AllocateTempMemoryHighInternal(16);
		statement->idValue = name;
		statement[1].idValue = (intptr_t)TempMalloc(0);
		statement[2].sourcePosValue = sourcePos.sourcePosValue;
		statement[3].codePosValue = (const char *)scrCompileGlob.currentCaseStatement;
		scrCompileGlob.currentCaseStatement = (CaseStatementInfo *)statement;
	}
}

void EmitDefaultStatement(sval_u sourcePos)
{
	EmitCaseStatementInfo(0, sourcePos);
}

void EmitCaseStatement(sval_u expr, sval_u sourcePos)
{
	unsigned int index;

	if ( expr.node->type == ENUM_integer )
	{
		if ( !IsValidArrayIndex(expr.node[1].idValue) )
		{
			CompileError(sourcePos.sourcePosValue, va("case index %d out of range", expr.node[1].idValue));
			return;
		}

		index = GetInternalVariableIndex(expr.node[1].sourcePosValue);
	}
	else
	{
		if ( expr.node->type != ENUM_string )
		{
			CompileError(sourcePos.sourcePosValue, "case expression must be an int or string");
			return;
		}

		index = expr.node[1].idValue;
		CompileTransferRefToString(index, 1u);
	}

	EmitCaseStatementInfo(index, sourcePos);
}

void EmitCreateLocalVars(scr_block_s *block)
{
	int i;

	if ( block->localVarsCreateCount != block->localVarsPublicCount )
	{
		for ( i = block->localVarsCreateCount; i < block->localVarsPublicCount; ++i )
		{
			EmitOpcode(OP_CreateLocalVariable, 0, CALL_NONE);
			EmitCanonicalStringConst(block->localVars[i]);
		}

		block->localVarsCreateCount = block->localVarsPublicCount;
	}
}

void EmitBinaryEqualsOperatorExpression(sval_u lhs, sval_u rhs, sval_u opcode, sval_u sourcePos, scr_block_s *block)
{
	scrCompileGlob.bConstRefCount = 1;
	EmitVariableExpression(lhs, block);
	scrCompileGlob.bConstRefCount = 0;
	EmitExpression(rhs, block);
	EmitOpcode(opcode.type, -1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	EmitVariableExpressionRef(lhs, block);
	EmitSetVariableField(sourcePos);
}

void EmitIncStatement(sval_u expr, sval_u sourcePos, scr_block_s *block)
{
	scrCompileGlob.forceNotCreate = 1;
	EmitVariableExpressionRef(expr, block);
	scrCompileGlob.forceNotCreate = 0;
	EmitOpcode(OP_inc, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	EmitSetVariableField(sourcePos);
}

void EmitDecStatement(sval_u expr, sval_u sourcePos, scr_block_s *block)
{
	scrCompileGlob.forceNotCreate = 1;
	EmitVariableExpressionRef(expr, block);
	scrCompileGlob.forceNotCreate = 0;
	EmitOpcode(OP_dec, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	EmitSetVariableField(sourcePos);
}

void ConnectContinueStatements()
{
	ContinueStatementInfo *statement;
	char *codePos;

	codePos = (char *)TempMalloc(0);

	for ( statement = scrCompileGlob.currentContinueStatement; statement; statement = statement->next )
		*(uint32_t *)statement->codePos = codePos - statement->nextCodePos;
}

void ConnectBreakStatements()
{
	BreakStatementInfo *statement;
	char *codePos;

	codePos = (char *)TempMalloc(0);

	for ( statement = scrCompileGlob.currentBreakStatement; statement; statement = statement->next )
		*(uint32_t *)statement->codePos = codePos - statement->nextCodePos;
}

void EmitForStatement(sval_u stmt1, sval_u expr, sval_u stmt2, sval_u stmt, sval_u sourcePos, sval_u forSourcePos, scr_block_s *block, sval_u *forStatBlock, sval_u *forStatPostBlock)
{
	scr_block_s *breakBlock;
	int *continueChildCount;
	scr_block_s **continueChildBlocks;
	int newContinueChildCount;
	scr_block_s **childBlocks;
	int newBreakChildCount;
	scr_block_s **oldContinueChildBlocks;
	int *oldBreakChildCount;
	scr_block_s **oldBreakChildBlocks;
	VariableCompileValue value;
	bool constConditional;
	ContinueStatementInfo *currentContinueStatement;
	bool cont2;
	bool cont1;
	BreakStatementInfo *currentBreakStatement;
	bool break2;
	bool break1;
	char *nextPos2;
	char *codePos;
	char *nextPos;

	break1 = scrCompileGlob.bCanBreak[0];
	break2 = scrCompileGlob.bCanBreak[1];
	currentBreakStatement = scrCompileGlob.currentBreakStatement;
	scrCompileGlob.bCanBreak[0] = 0;
	scrCompileGlob.bCanBreak[1] = 0;
	cont1 = scrCompileGlob.bCanContinue[0];
	cont2 = scrCompileGlob.bCanContinue[1];
	currentContinueStatement = scrCompileGlob.currentContinueStatement;
	scrCompileGlob.bCanContinue[0] = 0;
	scrCompileGlob.bCanContinue[1] = 0;
	EmitStatement(stmt1, 0, 0, block);
	Scr_TransferBlock(block, forStatBlock->block);
	EmitCreateLocalVars(forStatBlock->block);
	block->localVarsCreateCount = forStatBlock->block->localVarsCreateCount;
	Scr_TransferBlock(block, forStatPostBlock->block);
	nextPos = (char *)TempMalloc(0);

	if ( expr.node->type == ENUM_expression )
	{
		constConditional = 0;

		if ( EmitOrEvalExpression(expr.node[1], &value, block) )
		{
			if ( value.value.type == VAR_INTEGER || value.value.type == VAR_FLOAT )
			{
				Scr_CastBool(&value.value);

				if ( !value.value.u.intValue )
					CompileError(sourcePos.sourcePosValue, "conditional expression cannot be always false");

				constConditional = 1;
			}
			else
			{
				EmitValue(&value);
			}
		}
	}
	else
	{
		constConditional = 1;
	}

	oldBreakChildBlocks = scrCompileGlob.breakChildBlocks;
	oldBreakChildCount = scrCompileGlob.breakChildCount;
	breakBlock = scrCompileGlob.breakBlock;
	continueChildBlocks = scrCompileGlob.continueChildBlocks;
	continueChildCount = scrCompileGlob.continueChildCount;
	newBreakChildCount = 0;
	newContinueChildCount = 0;
	childBlocks = (scr_block_s **)Hunk_AllocateTempMemoryHighInternal(4096);
	scrCompileGlob.continueChildBlocks = childBlocks;
	scrCompileGlob.continueChildCount = &newContinueChildCount;
	scrCompileGlob.breakBlock = forStatBlock->block;

	if ( constConditional )
	{
		codePos = 0;
		nextPos2 = 0;
		oldContinueChildBlocks = (scr_block_s **)Hunk_AllocateTempMemoryHighInternal(4096);
		scrCompileGlob.breakChildCount = &newBreakChildCount;
	}
	else
	{
		EmitOpcode(OP_JumpOnFalse, -1, CALL_NONE);
		AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
		EmitShort(0);
		codePos = scrCompileGlob.codePos;
		nextPos2 = (char *)TempMalloc(0);
		oldContinueChildBlocks = 0;
	}

	scrCompileGlob.breakChildBlocks = oldContinueChildBlocks;
	scrCompileGlob.bCanBreak[0] = 1;
	scrCompileGlob.bCanBreak[1] = scrCompilePub.developer_statement != 0;
	scrCompileGlob.currentBreakStatement = 0;
	scrCompileGlob.bCanContinue[0] = 1;
	scrCompileGlob.bCanContinue[1] = scrCompilePub.developer_statement != 0;
	scrCompileGlob.currentContinueStatement = 0;
	EmitStatement(stmt, 0, 0, forStatBlock->block);
	Scr_AddContinueBlock(forStatBlock->block);
	scrCompileGlob.bCanBreak[0] = 0;
	scrCompileGlob.bCanBreak[1] = 0;
	scrCompileGlob.bCanContinue[0] = 0;
	scrCompileGlob.bCanContinue[1] = 0;
	ConnectContinueStatements();
	Scr_InitFromChildBlocks(childBlocks, newContinueChildCount, forStatPostBlock->block);
	EmitStatement(stmt2, 0, 0, forStatPostBlock->block);
	EmitOpcode(OP_jumpback, 0, CALL_NONE);
	AddOpcodePos(forSourcePos.sourcePosValue, SOURCE_TYPE_NONE);

	if ( stmt.node->type == ENUM_statement_list )
		AddOpcodePos(stmt.node[3].sourcePosValue, SOURCE_TYPE_BREAKPOINT);

	EmitShort(0);

	*(uint16_t *)scrCompileGlob.codePos = (intptr_t)TempMalloc(0) - (intptr_t)nextPos;

	if ( codePos )
		*(uint16_t *)codePos = (intptr_t)TempMalloc(0) - (intptr_t)nextPos2;

	ConnectBreakStatements();
	scrCompileGlob.bCanBreak[0] = break1;
	scrCompileGlob.bCanBreak[1] = break2;
	scrCompileGlob.currentBreakStatement = currentBreakStatement;
	scrCompileGlob.bCanContinue[0] = cont1;
	scrCompileGlob.bCanContinue[1] = cont2;
	scrCompileGlob.currentContinueStatement = currentContinueStatement;

	if ( constConditional )
		Scr_InitFromChildBlocks(oldContinueChildBlocks, newBreakChildCount, block);

	scrCompileGlob.breakChildBlocks = oldBreakChildBlocks;
	scrCompileGlob.breakChildCount = oldBreakChildCount;
	scrCompileGlob.breakBlock = breakBlock;
	scrCompileGlob.continueChildBlocks = continueChildBlocks;
	scrCompileGlob.continueChildCount = continueChildCount;
}

void EmitWhileStatement(sval_u expr, sval_u stmt, sval_u sourcePos, sval_u whileSourcePos, scr_block_s *block, sval_u *whileStatBlock)
{
	scr_block_s *breakBlock;
	int *continueChildCount;
	scr_block_s **continueChildBlocks;
	int childCount;
	scr_block_s **childBlocks;
	int *breakChildCount;
	scr_block_s **breakChildBlocks;
	VariableCompileValue value;
	bool constConditional;
	ContinueStatementInfo *currentContinueStatement;
	bool cont2;
	bool cont1;
	BreakStatementInfo *currentBreakStatement;
	bool break2;
	bool break1;
	char *nextPos2;
	char *codePos;
	char *nextPos;

	break1 = scrCompileGlob.bCanBreak[0];
	break2 = scrCompileGlob.bCanBreak[1];
	currentBreakStatement = scrCompileGlob.currentBreakStatement;
	scrCompileGlob.bCanBreak[0] = 0;
	scrCompileGlob.bCanBreak[1] = 0;
	cont1 = scrCompileGlob.bCanContinue[0];
	cont2 = scrCompileGlob.bCanContinue[1];
	currentContinueStatement = scrCompileGlob.currentContinueStatement;
	scrCompileGlob.bCanContinue[0] = 0;
	scrCompileGlob.bCanContinue[1] = 0;
	Scr_TransferBlock(block, whileStatBlock->block);
	EmitCreateLocalVars(whileStatBlock->block);
	block->localVarsCreateCount = whileStatBlock->block->localVarsCreateCount;
	nextPos = (char *)TempMalloc(0);
	constConditional = 0;

	if ( EmitOrEvalExpression(expr, &value, block) )
	{
		if ( value.value.type == VAR_INTEGER || value.value.type == VAR_FLOAT )
		{
			Scr_CastBool(&value.value);

			if ( !value.value.u.intValue )
				CompileError(sourcePos.sourcePosValue, "conditional expression cannot be always false");

			constConditional = 1;
		}
		else
		{
			EmitValue(&value);
		}
	}

	breakChildBlocks = scrCompileGlob.breakChildBlocks;
	breakChildCount = scrCompileGlob.breakChildCount;
	breakBlock = scrCompileGlob.breakBlock;
	continueChildBlocks = scrCompileGlob.continueChildBlocks;
	continueChildCount = scrCompileGlob.continueChildCount;
	childCount = 0;
	scrCompileGlob.continueChildBlocks = 0;
	scrCompileGlob.breakBlock = whileStatBlock->block;

	if ( constConditional )
	{
		codePos = 0;
		nextPos2 = 0;
		childBlocks = (scr_block_s **)Hunk_AllocateTempMemoryHighInternal(4096);
		scrCompileGlob.breakChildCount = &childCount;
	}
	else
	{
		EmitOpcode(OP_JumpOnFalse, -1, CALL_NONE);
		AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
		EmitShort(0);
		codePos = scrCompileGlob.codePos;
		nextPos2 = (char *)TempMalloc(0);
		childBlocks = 0;
	}

	scrCompileGlob.breakChildBlocks = childBlocks;
	scrCompileGlob.bCanBreak[0] = 1;
	scrCompileGlob.bCanBreak[1] = scrCompilePub.developer_statement != 0;
	scrCompileGlob.currentBreakStatement = 0;
	scrCompileGlob.bCanContinue[0] = 1;
	scrCompileGlob.bCanContinue[1] = scrCompilePub.developer_statement != 0;
	scrCompileGlob.currentContinueStatement = 0;
	EmitStatement(stmt, 0, 0, whileStatBlock->block);

	if ( whileStatBlock->block->abortLevel != SCR_ABORT_RETURN )
		whileStatBlock->block->abortLevel = SCR_ABORT_NONE;

	scrCompileGlob.bCanBreak[0] = 0;
	scrCompileGlob.bCanBreak[1] = 0;
	scrCompileGlob.bCanContinue[0] = 0;
	scrCompileGlob.bCanContinue[1] = 0;
	ConnectContinueStatements();
	EmitOpcode(OP_jumpback, 0, CALL_NONE);
	AddOpcodePos(whileSourcePos.sourcePosValue, SOURCE_TYPE_NONE);

	if ( stmt.node->type == ENUM_statement_list )
		AddOpcodePos(stmt.node[3].sourcePosValue, SOURCE_TYPE_BREAKPOINT);

	EmitShort(0);
	*(uint16_t *)scrCompileGlob.codePos = (intptr_t)TempMalloc(0) - (intptr_t)nextPos;

	if ( codePos )
		*(uint16_t *)codePos = (intptr_t)TempMalloc(0) - (intptr_t)nextPos2;

	ConnectBreakStatements();
	scrCompileGlob.bCanBreak[0] = break1;
	scrCompileGlob.bCanBreak[1] = break2;
	scrCompileGlob.currentBreakStatement = currentBreakStatement;
	scrCompileGlob.bCanContinue[0] = cont1;
	scrCompileGlob.bCanContinue[1] = cont2;
	scrCompileGlob.currentContinueStatement = currentContinueStatement;

	if ( constConditional )
		Scr_InitFromChildBlocks(childBlocks, childCount, block);

	scrCompileGlob.breakChildBlocks = breakChildBlocks;
	scrCompileGlob.breakChildCount = breakChildCount;
	scrCompileGlob.breakBlock = breakBlock;
	scrCompileGlob.continueChildBlocks = continueChildBlocks;
	scrCompileGlob.continueChildCount = continueChildCount;
}

void EmitIfElseStatement(sval_u expr, sval_u stmt1, sval_u stmt2, sval_u sourcePos, sval_u elseSourcePos, bool lastStatement, unsigned int endSourcePos, scr_block_s *block, sval_u *ifStatBlock, sval_u *elseStatBlock)
{
	char *pos;
	int childCount;
	scr_block_s *childBlocks[2];
	unsigned int checksum;
	char *nextPos;
	char *pos1;
	char *pos2;
	char *codePos;
	bool last;

	last = lastStatement;
	childCount = 0;
	EmitExpression(expr, block);
	EmitOpcode(OP_JumpOnFalse, -1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	EmitShort(0);
	codePos = scrCompileGlob.codePos;
	pos1 = (char *)TempMalloc(0);
	Scr_TransferBlock(block, ifStatBlock->block);
	EmitStatement(stmt1, lastStatement, endSourcePos, ifStatBlock->block);
	EmitRemoveLocalVars(ifStatBlock->block, ifStatBlock->block);

	if ( ifStatBlock->block->abortLevel == SCR_ABORT_NONE )
	{
		childBlocks[0] = ifStatBlock->block;
		childCount = 1;
	}

	checksum = scrVarPub.checksum;

	if ( last )
	{
		EmitEnd();
		EmitCodepos(0);
		AddOpcodePos(endSourcePos, SOURCE_TYPE_BREAKPOINT);
		pos2 = 0;
		nextPos = 0;
	}
	else
	{
		EmitOpcode(OP_jump, 0, CALL_NONE);
		AddOpcodePos(elseSourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
		EmitCodepos(0);
		pos2 = scrCompileGlob.codePos;
		nextPos = (char *)TempMalloc(0);
	}

	scrVarPub.checksum = checksum + 1;
	*(uint16_t *)codePos = (intptr_t)TempMalloc(0) - (intptr_t)pos1;
	Scr_TransferBlock(block, elseStatBlock->block);
	EmitStatement(stmt2, last, endSourcePos, elseStatBlock->block);
	EmitNOP2(last, endSourcePos, elseStatBlock->block);

	if ( elseStatBlock->block->abortLevel == SCR_ABORT_NONE )
		childBlocks[childCount++] = elseStatBlock->block;

	if ( !last )
	{
		pos = pos2;
		*(uint32_t *)pos = (char *)TempMalloc(0) - nextPos;
	}

	Scr_InitFromChildBlocks(childBlocks, childCount, block);
}

void EmitSwitchStatementList(sval_u val, bool lastStatement, unsigned int endSourcePos, scr_block_s *block)
{
	bool last;
	scr_block_s *breakBlock;
	int *breakChildCount;
	scr_block_s **breakChildBlocks;
	int childCount;
	scr_block_s **childBlocks;
	bool hasDefault;
	sval_u *node;
	sval_u *nextNode;
	bool isDefault;

	isDefault = lastStatement;
	breakChildBlocks = scrCompileGlob.breakChildBlocks;
	breakChildCount = scrCompileGlob.breakChildCount;
	breakBlock = scrCompileGlob.breakBlock;
	childCount = 0;
	childBlocks = (scr_block_s **)Hunk_AllocateTempMemoryHighInternal(4096);
	scrCompileGlob.breakChildBlocks = childBlocks;
	scrCompileGlob.breakChildCount = &childCount;
	scrCompileGlob.breakBlock = 0;
	hasDefault = 0;

	for ( nextNode = val.node->node[1].node; nextNode; nextNode = node )
	{
		node = nextNode[1].node;

		if ( nextNode->node->type == ENUM_case || nextNode->node->type == ENUM_default )
		{
			if ( scrCompileGlob.breakBlock )
			{
				scrCompileGlob.bCanBreak[0] = 0;
				EmitRemoveLocalVars(scrCompileGlob.breakBlock, scrCompileGlob.breakBlock);
			}

			if ( nextNode->node->type == ENUM_case )
			{
				scrCompileGlob.breakBlock = nextNode->node[3].block;
				EmitCaseStatement(nextNode->node[1], nextNode->node[2]);
			}
			else
			{
				scrCompileGlob.breakBlock = nextNode->node[2].block;
				hasDefault = 1;
				EmitDefaultStatement(nextNode->node[1]);
			}

			Scr_TransferBlock(block, scrCompileGlob.breakBlock);
			scrCompileGlob.bCanBreak[0] = 1;
		}
		else
		{
			if ( !scrCompileGlob.breakBlock )
			{
				CompileError(endSourcePos, "missing case statement");
				return;
			}

			last = 0;

			if ( isDefault && Scr_IsLastStatement(node) )
				last = 1;

			EmitStatement(nextNode[0], last, endSourcePos, scrCompileGlob.breakBlock);

			if ( scrCompileGlob.breakBlock && scrCompileGlob.breakBlock->abortLevel )
			{
				scrCompileGlob.breakBlock = 0;
				scrCompileGlob.bCanBreak[0] = 0;
			}
		}
	}

	if ( scrCompileGlob.breakBlock )
	{
		scrCompileGlob.bCanBreak[0] = 0;
		EmitRemoveLocalVars(scrCompileGlob.breakBlock, scrCompileGlob.breakBlock);
	}

	if ( hasDefault )
	{
		if ( scrCompileGlob.breakBlock )
			Scr_AddBreakBlock(scrCompileGlob.breakBlock);

		Scr_InitFromChildBlocks(childBlocks, childCount, block);
	}

	scrCompileGlob.breakChildBlocks = breakChildBlocks;
	scrCompileGlob.breakChildCount = breakChildCount;
	scrCompileGlob.breakBlock = breakBlock;
}

int CompareCaseInfo(const void *elem1, const void *elem2)
{
	if ( *(uint32_t *)elem1 <= *(uint32_t *)elem2 )
		return *(uint32_t *)elem1 < *(uint32_t *)elem2;
	else
		return -1;
}

void EmitSwitchStatement(sval_u expr, sval_u stmtlist, sval_u sourcePos, bool lastStatement, unsigned int endSourcePos, scr_block_s *block)
{
	int num;
	char *nextPos;
	char *pos3;
	char *pos2;
	char *pos1;
	BreakStatementInfo *currentBreakStatement;
	bool break2;
	bool break1;
	CaseStatementInfo *currentStatement;
	CaseStatementInfo *nextStatement;
	CaseStatementInfo *currentCaseStatement;
	bool oldThread;

	oldThread = scrCompileGlob.firstThread[2];
	currentCaseStatement = scrCompileGlob.currentCaseStatement;
	scrCompileGlob.firstThread[2] = 0;
	break1 = scrCompileGlob.bCanBreak[0];
	break2 = scrCompileGlob.bCanBreak[1];
	currentBreakStatement = scrCompileGlob.currentBreakStatement;
	scrCompileGlob.bCanBreak[0] = 0;
	scrCompileGlob.bCanBreak[1] = 0;
	EmitExpression(expr, block);
	EmitOpcode(OP_switch, -1, CALL_NONE);
	EmitCodepos(0);
	pos1 = scrCompileGlob.codePos;
	nextPos = (char *)TempMalloc(0);
	scrCompileGlob.firstThread[2] = scrCompilePub.developer_statement != 0;
	scrCompileGlob.currentCaseStatement = 0;
	scrCompileGlob.bCanBreak[1] = scrCompilePub.developer_statement != 0;
	scrCompileGlob.currentBreakStatement = 0;
	EmitSwitchStatementList(stmtlist, lastStatement, endSourcePos, block);
	scrCompileGlob.firstThread[2] = 0;
	scrCompileGlob.bCanBreak[1] = 0;
	EmitOpcode(OP_endswitch, 0, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	EmitShort(0);
	pos2 = scrCompileGlob.codePos;
	*(uint32_t *)pos1 = scrCompileGlob.codePos - nextPos;
	pos3 = TempMallocAlignStrict(0);
	num = 0;
	currentStatement = scrCompileGlob.currentCaseStatement;

	while ( currentStatement )
	{
		EmitCodepos((const char *)currentStatement->name);
		EmitCodepos(currentStatement->codePos);
		currentStatement = currentStatement->next;
		++num;
	}

	*(uint16_t *)pos2 = num;
	qsort(pos3, num, 8u, CompareCaseInfo);

	while ( num > 1 )
	{
		if ( *(uint32_t *)pos3 == *((uint32_t *)pos3 + 2) )
		{
			for ( nextStatement = scrCompileGlob.currentCaseStatement; nextStatement; nextStatement = nextStatement->next )
			{
				if ( nextStatement->name == *(uint32_t *)pos3 )
				{
					CompileError(nextStatement->sourcePos, "duplicate case expression");
					return;
				}
			}
		}

		--num;
		pos3 += 8;
	}

	ConnectBreakStatements();
	scrCompileGlob.firstThread[2] = oldThread;
	scrCompileGlob.currentCaseStatement = currentCaseStatement;
	scrCompileGlob.bCanBreak[0] = break1;
	scrCompileGlob.bCanBreak[1] = break2;
	scrCompileGlob.currentBreakStatement = currentBreakStatement;
}

void EmitBreakStatement(sval_u sourcePos, scr_block_s *block)
{
	BreakStatementInfo *newBreakStatement;

	if ( scrCompileGlob.bCanBreak[0] && block->abortLevel == SCR_ABORT_NONE )
	{
		Scr_AddBreakBlock(block);
		EmitRemoveLocalVars(block, scrCompileGlob.breakBlock);
		block->abortLevel = SCR_ABORT_BREAK;
		EmitOpcode(OP_jump, 0, CALL_NONE);
		AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
		EmitCodepos(0);
		newBreakStatement = (BreakStatementInfo *)Hunk_AllocateTempMemoryHighInternal(sizeof(BreakStatementInfo));
		newBreakStatement->codePos = scrCompileGlob.codePos;
		newBreakStatement->nextCodePos = (char *)TempMalloc(0);
		newBreakStatement->next = scrCompileGlob.currentBreakStatement;
		scrCompileGlob.currentBreakStatement = newBreakStatement;
	}
	else
	{
		CompileError(sourcePos.sourcePosValue, "illegal break statement");
	}
}

void EmitContinueStatement(sval_u sourcePos, scr_block_s *block)
{
	ContinueStatementInfo *newContinueStatement;

	if ( scrCompileGlob.bCanContinue[0] && block->abortLevel == SCR_ABORT_NONE )
	{
		Scr_AddContinueBlock(block);
		EmitRemoveLocalVars(block, block);
		block->abortLevel = SCR_ABORT_CONTINUE;
		EmitOpcode(OP_jump, 0, CALL_NONE);
		AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
		EmitCodepos(0);
		newContinueStatement = (ContinueStatementInfo *)Hunk_AllocateTempMemoryHighInternal(sizeof(ContinueStatementInfo));
		newContinueStatement->codePos = scrCompileGlob.codePos;
		newContinueStatement->nextCodePos = (char *)TempMalloc(0);
		newContinueStatement->next = scrCompileGlob.currentContinueStatement;
		scrCompileGlob.currentContinueStatement = newContinueStatement;
	}
	else
	{
		CompileError(sourcePos.sourcePosValue, "illegal continue statement");
	}
}

void EmitBreakpointStatement(sval_u sourcePos)
{
	if ( scrVarPub.developer_script )
	{
		EmitOpcode(OP_breakpoint, 0, CALL_NONE);
		AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
	}
}

void EmitProfStatement(sval_u profileName, sval_u sourcePos, int op)
{
	if ( scrVarPub.developer_script )
	{
		Scr_CompileRemoveRefToString(profileName.stringValue);
		EmitOpcode(op, 0, CALL_NONE);
		EmitByte(0);
	}
	else
	{
		Scr_CompileRemoveRefToString(profileName.stringValue);
	}
}

void EmitProfBeginStatement(sval_u profileName, sval_u sourcePos)
{
	EmitProfStatement(profileName, sourcePos, OP_prof_begin);
}

void EmitProfEndStatement(sval_u profileName, sval_u sourcePos)
{
	EmitProfStatement(profileName, sourcePos, OP_prof_end);
}

void EmitStatementList(sval_u val, bool lastStatement, unsigned int endSourcePos, scr_block_s *block)
{
	bool isLastStatement;
	sval_u *node;
	sval_u *nextNode;

	for ( nextNode = val.node->node[1].node; nextNode; nextNode = node )
	{
		node = nextNode[1].node;
		isLastStatement = 0;

		if ( lastStatement && Scr_IsLastStatement(node) )
			isLastStatement = 1;

		EmitStatement(nextNode[0], isLastStatement, endSourcePos, block);
	}
}

void EmitWaittillFrameEnd(sval_u sourcePos)
{
	EmitOpcode(OP_waittillFrameEnd, 0, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
}

void EmitNotifyStatement(sval_u obj, sval_u exprlist, sval_u sourcePos, sval_u notifySourcePos, scr_block_s *block)
{
	int expr_count;
	sval_u *node;
	sval_u *startNode;

	EmitOpcode(OP_voidCodepos, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
	expr_count = 0;
	startNode = 0;

	for ( node = exprlist.node->node; node; node = node[1].node )
	{
		startNode = node;
		EmitExpression(node->node[0], block);
		++expr_count;
	}

	EmitPrimitiveExpression(obj, block);
	EmitOpcode(OP_notify, -expr_count - 2, CALL_NONE);
	AddOpcodePos(notifySourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	AddOpcodePos(startNode->node[1].sourcePosValue, SOURCE_TYPE_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
}

void EmitEndOnStatement(sval_u obj, sval_u expr, sval_u sourcePos, sval_u exprSourcePos, scr_block_s *block)
{
	EmitExpression(expr, block);
	EmitPrimitiveExpression(obj, block);
	EmitOpcode(OP_endon, -2, CALL_NONE);
	AddOpcodePos(exprSourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
}

void EmitWaittillmatchStatement(sval_u obj, sval_u exprlist, sval_u sourcePos, sval_u waitSourcePos, scr_block_s *block)
{
	int i;
	sval_u *nextNode;
	sval_u *node;

	nextNode = exprlist.node->node[1].node;

	for ( i = 0; ; ++i )
	{
		nextNode = nextNode[1].node;

		if ( !nextNode )
			break;

		EmitExpression(nextNode->node[0], block);
	}

	node = exprlist.node->node[1].node;
	EmitExpression(node->node[0], block);
	EmitPrimitiveExpression(obj, block);
	EmitOpcode(OP_waittillmatch, -2 - i, CALL_NONE);
	AddOpcodePos(waitSourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	AddOpcodePos(waitSourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	AddOpcodePos(node->node[1].sourcePosValue, SOURCE_TYPE_NONE);

	while ( 1 )
	{
		node = node[1].node;

		if ( !node )
			break;

		AddOpcodePos(node->node[1].sourcePosValue, SOURCE_TYPE_NONE);
	}

	EmitByte(i);
	EmitOpcode(OP_clearparams, 0, CALL_NONE);
}

void EmitDeveloperStatementList(sval_u val, sval_u sourcePos, scr_block_s *block, sval_u *devStatBlock)
{
	char *pos;
	unsigned int checksum;

	if ( scrCompilePub.developer_statement )
	{
		CompileError(sourcePos.sourcePosValue, "cannot recurse /#");
	}
	else
	{
		checksum = scrVarPub.checksum;
		Scr_TransferBlock(block, devStatBlock->block);

		if ( scrVarPub.developer_script )
		{
			scrCompilePub.developer_statement = 1;
			EmitStatementList(val, 0, 0, devStatBlock->block);
			EmitRemoveLocalVars(devStatBlock->block, devStatBlock->block);
		}
		else
		{
			pos = (char *)TempMalloc(0);
			scrCompilePub.developer_statement = 2;
			EmitStatementList(val, 0, 0, devStatBlock->block);
			TempMemorySetPos(pos);
		}

		scrCompilePub.developer_statement = 0;
		scrVarPub.checksum = checksum;
	}
}

void EmitSafeSetWaittillVariableField(sval_u expr, sval_u sourcePos, scr_block_s *block)
{
	unsigned char index;

	index = Scr_FindLocalVarIndex(expr.sourcePosValue, sourcePos, 1, block);
	EmitOpcode(OP_SafeSetWaittillVariableFieldCached, 0, CALL_NONE);
	EmitByte(index);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
}

void EmitFormalWaittillParameterListRefInternal(sval_u *node, scr_block_s *block)
{
	while ( 1 )
	{
		node = node[1].node;

		if ( !node )
			break;

		EmitSafeSetWaittillVariableField(node->node[0], node->node[1], block);
	}
}

void EmitWaittillStatement(sval_u obj, sval_u exprlist, sval_u sourcePos, sval_u waitSourcePos, scr_block_s *block)
{
	sval_u *node;

	node = exprlist.node->node[1].node;
	EmitExpression(node->node[0], block);
	EmitPrimitiveExpression(obj, block);
	EmitOpcode(OP_waittill, -2, CALL_NONE);
	AddOpcodePos(waitSourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	AddOpcodePos(waitSourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
	AddOpcodePos(node->node[1].sourcePosValue, SOURCE_TYPE_NONE);
	EmitFormalWaittillParameterListRefInternal(node, block);
	EmitOpcode(OP_clearparams, 0, CALL_NONE);
}

void EmitStatement(sval_u val, bool lastStatement, unsigned int endSourcePos, scr_block_s *block)
{
	switch ( val.node->type )
	{
	case ENUM_assignment:
		EmitAssignmentStatement(val.node[1], val.node[2], val.node[3], val.node[4], block);
		break;

	case ENUM_call_expression_statement:
		EmitCallExpressionStatement(val.node[1], block);
		break;

	case ENUM_return:
		EmitReturnStatement(val.node[1], val.node[2], block);
		break;

	case ENUM_return2:
		EmitEndStatement(val.node[1], block);
		break;

	case ENUM_wait:
		EmitWaitStatement(val.node[1], val.node[2], val.node[3], block);
		break;

	case ENUM_if:
		EmitIfStatement(val.node[1], val.node[2], val.node[3], lastStatement, endSourcePos, block, &val.node[4]);
		break;

	case ENUM_if_else:
		EmitIfElseStatement(val.node[1], val.node[2], val.node[3], val.node[4], val.node[5], lastStatement, endSourcePos, block, &val.node[6], &val.node[7]);
		break;

	case ENUM_while:
		EmitWhileStatement(val.node[1], val.node[2], val.node[3], val.node[4], block, &val.node[5]);
		break;

	case ENUM_for:
		EmitForStatement(val.node[1], val.node[2], val.node[3], val.node[4], val.node[5], val.node[6], block, &val.node[7], &val.node[8]);
		break;

	case ENUM_inc:
		EmitIncStatement(val.node[1], val.node[2], block);
		break;

	case ENUM_dec:
		EmitDecStatement(val.node[1], val.node[2], block);
		break;

	case ENUM_binary_equals:
		EmitBinaryEqualsOperatorExpression(val.node[1], val.node[2], val.node[3], val.node[4], block);
		break;

	case ENUM_statement_list:
		EmitStatementList(val.node[1], lastStatement, endSourcePos, block);
		break;

	case ENUM_developer_statement_list:
		EmitDeveloperStatementList(val.node[1], val.node[2], block, &val.node[3]);
		break;

	case ENUM_waittill:
		EmitWaittillStatement(val.node[1], val.node[2], val.node[3], val.node[4], block);
		break;

	case ENUM_waittillmatch:
		EmitWaittillmatchStatement(val.node[1], val.node[2], val.node[3], val.node[4], block);
		break;

	case ENUM_waittillFrameEnd:
		EmitWaittillFrameEnd(val.node[1]);
		break;

	case ENUM_notify:
		EmitNotifyStatement(val.node[1], val.node[2], val.node[3], val.node[4], block);
		break;

	case ENUM_endon:
		EmitEndOnStatement(val.node[1], val.node[2], val.node[3], val.node[4], block);
		break;

	case ENUM_switch:
		EmitSwitchStatement(val.node[1], val.node[2], val.node[3], lastStatement, endSourcePos, block);
		break;

	case ENUM_case:
		CompileError(val.node[2].sourcePosValue, "illegal case statement");
		break;

	case ENUM_default:
		CompileError(val.node[1].sourcePosValue, "illegal default statement");
		break;

	case ENUM_break:
		EmitBreakStatement(val.node[1], block);
		break;

	case ENUM_continue:
		EmitContinueStatement(val.node[1], block);
		break;

	case ENUM_breakpoint:
		EmitBreakpointStatement(val.node[1]);
		break;

	case ENUM_prof_begin:
		EmitProfBeginStatement(val.node[1], val.node[2]);
		break;

	case ENUM_prof_end:
		EmitProfEndStatement(val.node[1], val.node[2]);
		break;

	default:
		return;
	}
}

void EmitArrayVariable(sval_u expr, sval_u index, sval_u sourcePos, sval_u indexSourcePos, scr_block_s *block)
{
	EmitExpression(index, block);
	EmitPrimitiveExpression(expr, block);
	EmitEvalArray(sourcePos, indexSourcePos);
}

void EmitVariableExpression(sval_u expr, scr_block_s *block)
{
	if ( expr.node->type == ENUM_field_variable )
	{
		EmitFieldVariable(expr.node[1], expr.node[2], expr.node[3], block);
	}
	else if ( expr.node->type > ENUM_field_variable )
	{
		if ( expr.node->type == ENUM_self_field )
		{
			if ( scrCompilePub.script_loading )
				CompileError(expr.node[2].sourcePosValue, "self field can only be used in the script debugger");
			else
				CompileError(expr.node[2].sourcePosValue, "self field in assignment expression not currently supported");
		}
		else if ( expr.node->type == ENUM_object )
		{
			EmitObject(expr.node[1], expr.node[2]);
		}
	}
	else if ( expr.node->type == ENUM_local_variable )
	{
		EmitLocalVariable(expr.node[1], expr.node[2], block);
	}
	else if ( expr.node->type == ENUM_array_variable )
	{
		EmitArrayVariable(expr.node[1], expr.node[2], expr.node[3], expr.node[4], block);
	}
}

void EmitExpressionFieldObject(sval_u expr, sval_u sourcePos, scr_block_s *block)
{
	if ( expr.node->type == ENUM_primitive_expression )
		EmitPrimitiveExpressionFieldObject(expr.node[1], expr.node[2], block);
	else
		CompileError(sourcePos.sourcePosValue, "not an object");
}

void EmitExpressionListFieldObject(sval_u exprlist, sval_u sourcePos, scr_block_s *block)
{
	sval_u *node;

	node = GetSingleParameter(exprlist);

	if ( node )
		EmitExpressionFieldObject(node->node[0], node->node[1], block);
	else
		CompileError(sourcePos.sourcePosValue, "not an object");
}

void EmitPrimitiveExpressionFieldObject(sval_u expr, sval_u sourcePos, scr_block_s *block)
{
	switch ( expr.node->type )
	{
	case ENUM_variable:
		EmitVariableExpression(expr.node[1], block);
		EmitCastFieldObject(expr.node[2]);
		break;

	case ENUM_call_expression:
		EmitCallExpressionFieldObject(expr.node[1], block);
		break;

	case ENUM_self:
		EmitSelfObject(expr.node[1]);
		break;

	case ENUM_level:
		EmitLevelObject(expr.node[1]);
		break;

	case ENUM_anim:
		EmitAnimObject(expr.node[1]);
		break;

	case ENUM_expression_list:
		EmitExpressionListFieldObject(expr.node[1], sourcePos, block);
		break;

	default:
		CompileError(sourcePos.sourcePosValue, "not an object");
		break;
	}
}

void EmitPreFunctionCall(sval_u func_name)
{
	if ( func_name.node->type == ENUM_script_call )
		EmitOpcode(OP_PreScriptCall, 1, CALL_NONE);
}

void EmitExpression(sval_u expr, scr_block_s *block)
{
	VariableCompileValue constValue;

	if ( EmitOrEvalExpression(expr, &constValue, block) )
		EmitValue(&constValue);
}

void EmitPrimitiveExpression(sval_u expr, scr_block_s *block)
{
	VariableCompileValue constValue;

	if ( EmitOrEvalPrimitiveExpression(expr, &constValue, block) )
		EmitValue(&constValue);
}

int EmitExpressionList(sval_u exprlist, scr_block_s *block)
{
	sval_u *node;
	int expr_count;

	expr_count = 0;

	for ( node = exprlist.node->node; node; node = node[1].node )
	{
		EmitExpression(node->node[0], block);
		++expr_count;
	}

	return expr_count;
}

void EmitPostFunctionCall(sval_u func_name, int param_count, bool bMethod, scr_block_s *block)
{
	if ( func_name.node->type == ENUM_script_call )
	{
		EmitPostScriptFunctionCall(func_name.node[1], param_count, bMethod, func_name.node[2], block);
	}
	else if ( func_name.node->type == ENUM_script_thread_call )
	{
		EmitPostScriptThreadCall(func_name.node[1], param_count, bMethod, func_name.node[2], func_name.node[3], block);
	}
}

int AddFunction(int func)
{
	int i;

	for ( i = 0; i < scrCompilePub.func_table_size; ++i )
	{
		if ( scrCompilePub.func_table[i] == func )
			return i;
	}

	if ( scrCompilePub.func_table_size == 1024 )
		Com_Error(ERR_DROP, "SCR_FUNC_TABLE_SIZE exceeded");

	scrCompilePub.func_table[scrCompilePub.func_table_size++] = func;
	return i;
}

void EmitCall(sval_u func_name, sval_u params, bool bStatement, scr_block_s *block)
{
	short newFuncIndex;
	VariableValue value;
	unsigned int funcId;
	char *savedPos;
	void (*func)();
	int type;
	const char *pName;
	sval_u sourcePos;
	unsigned int index;
	int param_count;

	index = Scr_GetBuiltin(func_name);

	if ( !index )
		goto script_function;

	pName = SL_ConvertToString(index);
	sourcePos.sourcePosValue = func_name.node[2].sourcePosValue;
	funcId = FindVariable(scrCompilePub.builtinFunc, index);

	if ( funcId )
	{
		Scr_EvalVariable(&value, funcId);
		type = Scr_IsVariable(value.type);
		func = (void (*)())value.u.intValue;
	}
	else
	{
		type = BUILTIN_ANY;
		func = Scr_GetFunction(&pName, &type);
		funcId = GetNewVariable(scrCompilePub.builtinFunc, index);
		value.type = Scr_GetCacheType(type);
		value.u.intValue = (intptr_t)func;
		SetVariableValue(funcId, &value);
	}

	if ( func )
	{
		if ( type == BUILTIN_DEVELOPER_ONLY && (Scr_BeginDevScript(&type, &savedPos), type == BUILTIN_DEVELOPER_ONLY) && !bStatement )
		{
			CompileError(
			    sourcePos.sourcePosValue,
			    "return value of developer command can not be accessed if not in a /# ... #/ comment");
		}
		else
		{
			param_count = EmitExpressionList(params, block);

			if ( param_count < 256 )
			{
				Scr_CompileRemoveRefToString(index);
				EmitCallBuiltinOpcode(param_count, sourcePos);
				newFuncIndex = AddFunction((intptr_t)func);
				EmitShort(newFuncIndex);
				AddExpressionListOpcodePos(params);

				if ( bStatement )
					EmitDecTop();

				Scr_EndDevScript(type, &savedPos);
			}
			else
			{
				CompileError(sourcePos.sourcePosValue, "parameter count exceeds 256");
			}
		}
	}
	else
	{
script_function:
		EmitPreFunctionCall(func_name);
		param_count = EmitExpressionList(params, block);
		EmitPostFunctionCall(func_name, param_count, 0, block);
		AddExpressionListOpcodePos(params);

		if ( bStatement )
			EmitDecTop();
	}
}

void EmitFunction(sval_u func, sval_u sourcePos)
{
	const char *funcName;
	int scope;
	bool bExists;
	unsigned int name;
	unsigned int fileId;
	unsigned int id;
	unsigned int countId;
	unsigned int threadName;
	unsigned int threadId;
	unsigned int posId;
	VariableValue value;
	VariableValue count;

	if ( scrCompilePub.developer_statement == 2 )
	{
		Scr_CompileRemoveRefToString(func.node[1].stringValue);

		if ( func.node->type == ENUM_far_function )
		{
			Scr_CompileRemoveRefToString(func.node[2].stringValue);
			--scrCompilePub.far_function_count;
		}

		return;
	}

	threadId = 0;

	if ( func.node->type == ENUM_local_function )
	{
		scope = 0;
		threadName = GetVariable(scrCompileGlob.filePosId, func.node[1].idValue);
		CompileTransferRefToString(func.node[1].stringValue, 2u);
		threadId = GetObjectA(threadName);
	}
	else
	{
		scope = 1;
		funcName = SL_ConvertToString(func.node[1].stringValue);
		name = Scr_CreateCanonicalFilename(funcName);
		Scr_CompileRemoveRefToString(func.node[1].stringValue);
		Scr_EvalVariable(&value, FindVariable(scrCompilePub.loadedscripts, name));
		bExists = value.type != VAR_UNDEFINED;
		fileId = AddFilePrecache(name, sourcePos.sourcePosValue, 0);

		if ( bExists )
		{
			threadName = FindVariable(fileId, func.node[2].idValue);

			if ( !threadName || Scr_GetObjectType(threadName) != VAR_OBJECT )
			{
				CompileError(sourcePos.sourcePosValue, "unknown function");
				return;
			}
		}
		else
		{
			threadName = GetVariable(fileId, func.node[2].idValue);
		}

		CompileTransferRefToString(func.node[2].stringValue, 2u);
		threadId = GetObjectA(threadName);
		posId = FindVariable(threadId, 1u);

		if ( posId )
		{
			Scr_EvalVariable(&value, posId);

			if ( value.type == VAR_INCLUDE_CODEPOS )
			{
				CompileError(sourcePos.sourcePosValue, "unknown function");
				return;
			}

			if ( value.u.intValue )
			{
				if ( value.type == VAR_CODEPOS || scrCompilePub.developer_statement )
					EmitCodepos(value.u.codePosValue);
				else
					CompileError(sourcePos.sourcePosValue, "normal script cannot reference a function in a /# ... #/ comment");

				return;
			}
		}
	}

	EmitCodepos((const char *)scope);
	countId = GetVariable(threadId, 0);
	Scr_EvalVariable(&count, countId);

	if ( count.type == VAR_UNDEFINED )
	{
		count.type = VAR_INTEGER;
		count.u.intValue = 0;
	}

	id = GetNewVariable(threadId, count.u.intValue + 2);
	value.u.codePosValue = scrCompileGlob.codePos;

	if ( scrCompilePub.developer_statement )
		value.type = VAR_DEVELOPER_CODEPOS;
	else
		value.type = VAR_CODEPOS;

	SetNewVariableValue(id, &value);
	++count.u.intValue;
	SetVariableValue(countId, &count);
	AddOpcodePos(sourcePos.sourcePosValue, SOURCE_TYPE_NONE);
}

void EmitMethod(sval_u expr, sval_u func_name, sval_u params, sval_u methodSourcePos, bool bStatement, scr_block_s *block)
{
	short newMethIndex;
	VariableValue value;
	unsigned int methId;
	char *savedPos;
	void (*meth)(scr_entref_t);
	int type;
	const char *pName;
	sval_u sourcePos;
	unsigned int index;
	int param_count;

	index = Scr_GetBuiltin(func_name);

	if ( !index )
		goto script_method;

	pName = SL_ConvertToString(index);
	sourcePos.sourcePosValue = func_name.node[2].sourcePosValue;
	methId = FindVariable(scrCompilePub.builtinMeth, index);

	if ( methId )
	{
		Scr_EvalVariable(&value, methId);
		type = Scr_IsVariable(value.type);
		meth = (void (*)(scr_entref_t))value.u.intValue;
	}
	else
	{
		type = BUILTIN_ANY;
		meth = Scr_GetMethod(&pName, &type);
		methId = GetNewVariable(scrCompilePub.builtinMeth, index);
		value.type = Scr_GetCacheType(type);
		value.u.intValue = (intptr_t)meth;
		SetVariableValue(methId, &value);
	}

	if ( meth )
	{
		if ( type == BUILTIN_DEVELOPER_ONLY && (Scr_BeginDevScript(&type, &savedPos), type == BUILTIN_DEVELOPER_ONLY) && !bStatement )
		{
			CompileError(
			    sourcePos.sourcePosValue,
			    "return value of developer command can not be accessed if not in a /# ... #/ comment");
		}
		else
		{
			param_count = EmitExpressionList(params, block);
			EmitPrimitiveExpression(expr, block);

			if ( param_count < 256 )
			{
				Scr_CompileRemoveRefToString(index);
				EmitCallBuiltinMethodOpcode(param_count, sourcePos);
				newMethIndex = AddFunction((intptr_t)meth);
				EmitShort(newMethIndex);
				AddOpcodePos(methodSourcePos.sourcePosValue, SOURCE_TYPE_NONE);
				AddExpressionListOpcodePos(params);

				if ( bStatement )
					EmitDecTop();

				Scr_EndDevScript(type, &savedPos);
			}
			else
			{
				CompileError(sourcePos.sourcePosValue, "parameter count exceeds 256");
			}
		}
	}
	else
	{
script_method:
		EmitPreFunctionCall(func_name);
		param_count = EmitExpressionList(params, block);
		EmitPrimitiveExpression(expr, block);
		EmitPostFunctionCall(func_name, param_count, 1, block);
		AddOpcodePos(methodSourcePos.sourcePosValue, SOURCE_TYPE_NONE);
		AddExpressionListOpcodePos(params);

		if ( bStatement )
			EmitDecTop();
	}
}

void InitThread(int type)
{
	scrCompileGlob.firstThread[2] = 0;
	scrCompileGlob.currentCaseStatement = 0;
	scrCompileGlob.bCanBreak[0] = 0;
	scrCompileGlob.bCanBreak[1] = 0;
	scrCompileGlob.currentBreakStatement = 0;
	scrCompileGlob.bCanContinue[0] = 0;
	scrCompileGlob.bCanContinue[1] = 0;
	scrCompileGlob.currentContinueStatement = 0;
	scrCompileGlob.breakChildBlocks = 0;
	scrCompileGlob.continueChildBlocks = 0;

	if ( scrCompileGlob.firstThread[type] )
	{
		scrCompileGlob.firstThread[type] = 0;
		EmitEnd();
		AddOpcodePos(0, SOURCE_TYPE_NONE);
		AddOpcodePos(0xFFFFFFFE, SOURCE_TYPE_NONE);
	}
}

void SetThreadPosition(unsigned int posId)
{
	unsigned int nextPosId;
	VariableValueInternal_u *value;

	nextPosId = FindVariable(posId, 1u);
	value = GetVariableValueAddress(nextPosId);
	value->u.codePosValue = (const char *)TempMalloc(0);
}

void EmitThreadInternal(unsigned int id, sval_u val, sval_u sourcePos, sval_u endSourcePos, scr_block_s *block)
{
	scrCompileGlob.fileCountId = id;
	AddThreadStartOpcodePos(sourcePos.sourcePosValue);
	scrCompileGlob.cumulOffset = 0;
	scrCompileGlob.maxOffset = 0;
	scrCompileGlob.maxCallOffset = 0;
	CompileTransferRefToString(val.node[1].stringValue, 2u);
	EmitFormalParameterList(val.node[2], sourcePos, block);
	EmitStatementList(val.node[3], 1, endSourcePos.sourcePosValue, block);
	EmitEnd();
	AddOpcodePos(endSourcePos.sourcePosValue, SOURCE_TYPE_BREAKPOINT);
	AddOpcodePos(0xFFFFFFFE, SOURCE_TYPE_NONE);

	if ( scrCompileGlob.maxOffset + 32 * scrCompileGlob.maxCallOffset > 2047 )
		CompileError(sourcePos.sourcePosValue, "function exceeds operand stack size");
}

void EmitNormalThread(sval_u val, sval_u *stmttblock)
{
	unsigned int posId;
	unsigned int threadId;

	InitThread(0);
	posId = FindVariable(scrCompileGlob.filePosId, val.node[1].sourcePosValue);
	threadId = FindObject(posId);
	SetThreadPosition(threadId);
	EmitThreadInternal(threadId, val, val.node[4], val.node[5], stmttblock->block);
}

void EmitDeveloperThread(sval_u val, sval_u *stmttblock)
{
	unsigned int posId;
	char *pos;
	unsigned int threadId;
	unsigned int checksum;

	if ( scrVarPub.developer_script )
	{
		scrCompilePub.developer_statement = 1;
		InitThread(1);
		posId = FindVariable(scrCompileGlob.filePosId, val.node[1].sourcePosValue);
		threadId = FindObject(posId);
		SetThreadPosition(threadId);
		EmitThreadInternal(threadId, val, val.node[4], val.node[5], stmttblock->block);
	}
	else
	{
		pos = (char *)TempMalloc(0);
		checksum = scrVarPub.checksum;
		scrCompilePub.developer_statement = 2;
		InitThread(1);
		EmitThreadInternal(0, val, val.node[4], val.node[5], stmttblock->block);
		TempMemorySetPos(pos);
		scrVarPub.checksum = checksum;
	}

	scrCompilePub.developer_statement = 0;
}

void EmitThread(sval_u val)
{
	if ( val.node->type == ENUM_begin_developer_thread )
	{
		scrCompileGlob.in_developer_thread = 1;
	}
	else if ( val.node->type > ENUM_begin_developer_thread )
	{
		if ( val.node->type == ENUM_end_developer_thread )
		{
			scrCompileGlob.in_developer_thread = 0;
		}
		else if ( val.node->type == ENUM_usingtree )
		{
			if ( scrCompileGlob.in_developer_thread )
			{
				CompileError(val.node[2].sourcePosValue, "cannot put #using_animtree inside /# ... #/ comment");
			}
			else
			{
				Scr_UsingTree(SL_ConvertToString(val.node[1].stringValue), val.node[3].sourcePosValue);
				Scr_CompileRemoveRefToString(val.node[1].stringValue);
			}
		}
	}
	else if ( val.node->type == ENUM_thread )
	{
		Scr_CalcLocalVarsThread(val.node[2], val.node[3], &val.node[6]);

		if ( scrCompileGlob.in_developer_thread )
			EmitDeveloperThread(val, &val.node[6]);
		else
			EmitNormalThread(val, &val.node[6]);
	}
}

void EmitThreadList(sval_u val)
{
	sval_u *node;
	sval_u *node2;

	scrCompileGlob.in_developer_thread = 0;

	for ( node = val.node->node[1].node; node; node = node[1].node )
		SpecifyThread(node[0]);

	if ( scrCompileGlob.in_developer_thread )
		CompileError(scrCompileGlob.developer_thread_sourcePos, "/# has no matching #/");

	scrCompileGlob.firstThread[0] = 1;
	scrCompileGlob.firstThread[1] = 1;

	for ( node2 = val.node->node[1].node; node2; node2 = node2[1].node )
		EmitThread(node2[0]);
}

void ScriptCompile(sval_u val, unsigned int filePosId, unsigned int scriptId)
{
	VariableValueInternal_u *pos;
	unsigned int includePosId;
	unsigned int threadCountId;
	VariableValue includePos;
	unsigned int index;
	unsigned short name;
	VariableValue value;
	unsigned int posId;
	unsigned int id;
	unsigned int includeFilePosId;
	PrecacheEntry *precachescript;
	PrecacheEntry *precachescript2;
	PrecacheEntry *precachescriptList;
	unsigned short filename;
	int func_count;
	int j;
	int i;

	scrCompileGlob.filePosId = filePosId;
	scrCompileGlob.bConstRefCount = 0;
	scrAnimPub.animTreeIndex = 0;
	scrCompilePub.developer_statement = 0;

	if ( scrCompilePub.far_function_count )
		precachescriptList = (PrecacheEntry *)Z_MallocInternal(sizeof(PrecacheEntry) * scrCompilePub.far_function_count);
	else
		precachescriptList = 0;

	scrCompileGlob.precachescriptList = precachescriptList;

	if ( precachescriptList )
	{
		precachescriptList->next = scrCompileGlob.precachescriptListHead;
		scrCompileGlob.precachescriptListHead = precachescriptList;
	}

	EmitIncludeList(val.node[0]);
	EmitThreadList(val.node[1]);
	scrCompilePub.programLen = (char *)TempMalloc(0) - scrVarPub.programBuffer;
	Hunk_ClearTempMemoryHighInternal();
	func_count = scrCompilePub.far_function_count;

	for ( i = 0; i < func_count; ++i )
	{
		precachescript = &precachescriptList[i];
		filename = precachescript->filename;
		includeFilePosId = Scr_LoadScript(SL_ConvertToString(filename));

		if ( !includeFilePosId )
		{
			CompileError(precachescript->sourcePos, "Could not find script '%s'", SL_ConvertToString(filename));
			return;
		}

		SL_RemoveRefToString(filename);

		if ( precachescript->include )
		{
			for ( j = i + 1; j < func_count; ++j )
			{
				precachescript2 = &precachescriptList[j];

				if ( !precachescript2->include )
					break;

				if ( precachescript2->filename == filename )
				{
					CompileError(precachescript2->sourcePos, "Duplicate #include");
					return;
				}
			}

			precachescript->include = 0;

			for ( id = FindNextSibling(includeFilePosId); id; id = FindNextSibling(id) )
			{
				if ( Scr_GetObjectType(id) == VAR_OBJECT )
				{
					posId = FindObject(id);
					index = FindVariable(posId, 1);

					if ( index )
					{
						Scr_EvalVariable(&includePos, index);

						if ( includePos.type != VAR_INCLUDE_CODEPOS )
						{
							name = GetVariableName(id);
							threadCountId = GetObjectA(GetVariable(filePosId, name));
							includePosId = SpecifyThreadPosition(threadCountId, name, precachescript->sourcePos, VAR_INCLUDE_CODEPOS);
							pos = GetVariableValueAddress(includePosId);
							*pos = *GetVariableValueAddress(index);
							LinkThread(threadCountId, &includePos, 0);
						}
					}
				}
			}
		}
	}

	if ( precachescriptList )
	{
		scrCompileGlob.precachescriptListHead = precachescriptList->next;
		Z_FreeInternal(precachescriptList);
	}

	LinkFile(filePosId);
	value.type = VAR_INTEGER;
	SetVariableValue(scriptId, &value);
}

void Scr_CompileShutdown()
{
	PrecacheEntry *precachescriptList;

	while ( scrCompileGlob.precachescriptListHead )
	{
		precachescriptList = scrCompileGlob.precachescriptListHead;
		scrCompileGlob.precachescriptListHead = scrCompileGlob.precachescriptListHead->next;
		Z_FreeInternal(precachescriptList);
	}
}