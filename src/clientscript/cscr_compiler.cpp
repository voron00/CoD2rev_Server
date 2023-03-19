#include "../qcommon/qcommon.h"
#include "clientscript_public.h"

#ifdef TESTING_LIBRARY
#define scrCompilePub (*((scrCompilePub_t*)( 0x08202A40 )))
#else
scrCompilePub_t scrCompilePub;
#endif

#ifdef TESTING_LIBRARY
#define scrVarPub (*((scrVarPub_t*)( 0x08394000 )))
#else
extern scrVarPub_t scrVarPub;
#endif

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
	int latchedValue;
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
	VariableCompileValue value_start[32];
} scrCompileGlob_t;
static_assert((sizeof(scrCompileGlob_t) == 0x1D8), "ERROR: scrCompileGlob_t size is invalid!");

#ifdef TESTING_LIBRARY
#define scrCompileGlob (*((scrCompileGlob_t*)( 0x08202860 )))
#else
scrCompileGlob_t scrCompileGlob;
#endif

int GetExpressionCount(sval_u exprlist)
{
	int count;
	sval_u *node;

	count = 0;

	for ( node = exprlist.node->node; node; node = node[1].node )
		++count;

	return count;
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
	*(uint32_t *)scrCompileGlob.codePos = (uint32_t)pos;
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
	AddOpcodePos(sourcePos.sourcePosValue, 1);
}

void EmitGetString(unsigned int value, sval_u sourcePos)
{
	EmitOpcode(OP_GetString, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, 1);
	EmitShort(value);
	CompileTransferRefToString(value, 1);
}

void EmitGetIString(unsigned int value, sval_u sourcePos)
{
	EmitOpcode(OP_GetIString, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, 1);
	EmitShort(value);
	CompileTransferRefToString(value, 1);
}

void EmitGetVector(const float *value, sval_u sourcePos)
{
	int i;

	EmitOpcode(OP_GetVector, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, 1);

	for ( i = 0; i < 3; ++i )
		EmitFloat(value[i]);

	RemoveRefToVector(value);
}

void EmitGetFloat(float value, sval_u sourcePos)
{
	EmitOpcode(OP_GetFloat, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, 1);
	EmitFloat(value);
}

void EmitGetInteger(int value, sval_u sourcePos)
{
	if ( value < 0 )
	{
		if ( value > -256 )
		{
			EmitOpcode(OP_GetNegByte, 1, CALL_NONE);
			AddOpcodePos(sourcePos.sourcePosValue, 1);
			EmitByte(-(byte)value);
			return;
		}

		if ( value > -65536 )
		{
			EmitOpcode(OP_GetNegUnsignedShort, 1, CALL_NONE);
			AddOpcodePos(sourcePos.sourcePosValue, 1);
			EmitShort(-(short)value);
			return;
		}
	}
	else
	{
		if ( !value )
		{
			EmitOpcode(OP_GetZero, 1, CALL_NONE);
			AddOpcodePos(sourcePos.sourcePosValue, 1);
			return;
		}

		if ( value <= 255 )
		{
			EmitOpcode(OP_GetByte, 1, CALL_NONE);
			AddOpcodePos(sourcePos.sourcePosValue, 1);
			EmitByte(value);
			return;
		}

		if ( value <= 0xFFFF )
		{
			EmitOpcode(OP_GetUnsignedShort, 1, CALL_NONE);
			AddOpcodePos(sourcePos.sourcePosValue, 1);
			EmitShort(value);
			return;
		}
	}

	EmitOpcode(OP_GetInteger, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, 1);
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
		return;
	}
}

void EmitOpcode(unsigned int op, int offset, int callType)
{
	unsigned int firstpos;
	unsigned int lastpos;
	int count;
	int i;

	if ( scrCompilePub.value_count )
	{
		count = scrCompilePub.value_count;
		scrCompilePub.value_count = 0;

		for ( i = 0; i < count; ++i )
		{
			EmitValue((VariableCompileValue *)(const char *)(&scrCompileGlob.value_start[i].sourcePos.codePosValue - 1));
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
			if ( *scrCompilePub.opcodePos == 30 )
			{
				RemoveOpcodePos();
				*scrCompilePub.opcodePos = 31;
				return;
			}
			firstpos = (byte)*scrCompilePub.opcodePos - 24;
			if ( firstpos > 5 )
				goto setopcodepos;
			RemoveOpcodePos();
			*scrCompilePub.opcodePos = 31;
			EmitByte(firstpos);
			return;
		case OP_EvalArrayRef:
			if ( *scrCompilePub.opcodePos == 55 )
			{
				RemoveOpcodePos();
				*scrCompilePub.opcodePos = 34;
				return;
			}
			if ( *scrCompilePub.opcodePos != 54 )
				goto setopcodepos;
			RemoveOpcodePos();
			*scrCompilePub.opcodePos = 33;
			return;
		case OP_EvalFieldVariable:
			if ( *scrCompilePub.opcodePos == 38 )
			{
				*scrCompilePub.opcodePos = 41;
				return;
			}
			if ( *scrCompilePub.opcodePos == 13 )
			{
				*scrCompilePub.opcodePos = 39;
				return;
			}
			if ( *scrCompilePub.opcodePos != 14 )
				goto setopcodepos;
			*scrCompilePub.opcodePos = 40;
			return;
		case OP_EvalFieldVariableRef:
			if ( *scrCompilePub.opcodePos == 38 )
			{
				*scrCompilePub.opcodePos = 45;
				return;
			}
			if ( *scrCompilePub.opcodePos == 13 )
			{
				*scrCompilePub.opcodePos = 43;
				return;
			}
			if ( *scrCompilePub.opcodePos != 14 )
				goto setopcodepos;
			*scrCompilePub.opcodePos = 44;
			return;
		case OP_SafeSetVariableFieldCached0:
			if ( *scrCompilePub.opcodePos != 22 )
				goto setopcodepos;
			*scrCompilePub.opcodePos = 48;
			return;
		case OP_SetVariableField:
			switch ( *scrCompilePub.opcodePos )
			{
			case '7':
				RemoveOpcodePos();
				*scrCompilePub.opcodePos = 61;
				return;
			case '6':
				RemoveOpcodePos();
				*scrCompilePub.opcodePos = 60;
				return;
			case '-':
				RemoveOpcodePos();
				*scrCompilePub.opcodePos = 59;
				return;
			case '+':
				RemoveOpcodePos();
				*scrCompilePub.opcodePos = 56;
				return;
			}
			if ( *scrCompilePub.opcodePos != 44 )
				goto setopcodepos;
			RemoveOpcodePos();
			*scrCompilePub.opcodePos = 58;
			return;
		case OP_ScriptFunctionCall:
			if ( *scrCompilePub.opcodePos != 78 )
				goto setopcodepos;
			*scrCompilePub.opcodePos = 79;
			return;
		case OP_ScriptMethodCall:
			if ( *scrCompilePub.opcodePos != 15 )
				goto setopcodepos;
			RemoveOpcodePos();
			*scrCompilePub.opcodePos = 80;
			if ( *scrCompileGlob.prevOpcodePos == 78 )
			{
				TempMemorySetPos(scrCompilePub.opcodePos);
				--scrCompilePub.opcodePos;
				scrCompileGlob.prevOpcodePos = 0;
				scrCompileGlob.codePos = scrCompilePub.opcodePos;
				*scrCompilePub.opcodePos = 79;
			}
			return;
		case OP_ScriptMethodThreadCall:
			if ( *scrCompilePub.opcodePos != 15 )
				goto setopcodepos;
			RemoveOpcodePos();
			*scrCompilePub.opcodePos = 84;
			return;
		case OP_CastFieldObject:
			if ( *scrCompilePub.opcodePos == 30 )
			{
				*scrCompilePub.opcodePos = 90;
				return;
			}
			lastpos = (byte)*scrCompilePub.opcodePos - 24;
			if ( lastpos > 5 )
				goto setopcodepos;
			*scrCompilePub.opcodePos = 90;
			EmitByte(lastpos);
			break;
		case OP_JumpOnFalse:
			if ( *scrCompilePub.opcodePos != 92 )
				goto setopcodepos;
			RemoveOpcodePos();
			*scrCompilePub.opcodePos = 95;
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
	VariableValue val;
	int type;
	int i;
	int varIndex;
	VariableValueInternal_u *adr;
	unsigned int index;

	index = FindVariable(threadCountId, 0);

	if ( index )
	{
		Scr_EvalVariable(&val, index);

		for ( i = 0; i < val.u.intValue; ++i )
		{
			varIndex = FindVariable(threadCountId, i + 2);
			adr = GetVariableValueAddress(varIndex);
			type = GetVarType(varIndex);

			if ( pos->type == VAR_DEVELOPER_CODEPOS && type == VAR_CODEPOS )
				CompileError2(adr->u.codePosValue, "normal script cannot reference a function in a /# ... #/ comment");

			if ( pos->type == VAR_UNDEFINED )
				CompileError2(adr->u.codePosValue, "unknown function");

			if ( !allowFarCall && *(uint32_t *)adr->u.intValue == 1 )
				CompileError2(adr->u.codePosValue, "unknown function");

			*(uint32_t *)adr->u.intValue = pos->u.intValue;
		}
	}
}

void LinkFile(unsigned int filePosId)
{
	VariableValue setValue;
	VariableValue newValue;
	VariableValue linkValue;
	unsigned int index;
	unsigned int parentId;
	unsigned int i;

	newValue.type = VAR_UNDEFINED;
	newValue.u.intValue = 0;

	for ( i = FindNextSibling(filePosId); i; i = FindNextSibling(i) )
	{
		parentId = FindObject(i);
		index = FindVariable(parentId, 1u);

		if ( index )
		{
			Scr_EvalVariable(&setValue, index);
			linkValue = setValue;

			if ( setValue.type == VAR_INCLUDE_CODEPOS )
				SetVariableValue(i, &newValue);
			else
				LinkThread(parentId, &linkValue, 1);
		}
		else
		{
			LinkThread(parentId, &newValue, 1);
		}
	}
}

unsigned int AddFilePrecache(unsigned int filename, unsigned int sourcePos, bool include)
{
	unsigned int var;

	SL_AddRefToString(filename);
	Scr_CompileRemoveRefToString(filename);
	scrCompileGlob.precachescriptList->filename = filename;
	scrCompileGlob.precachescriptList->sourcePos = sourcePos;
	scrCompileGlob.precachescriptList->include = include;
	++scrCompileGlob.precachescriptList;
	var = GetVariable(scrCompilePub.scriptsPos, filename);

	return GetObjectA(var);
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
		EmitInclude(*node);
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
	Scr_RegisterLocalVar(expr.stringValue, sourcePos, block);
}

void Scr_CalcLocalVarsFormalParameterListInternal(sval_u *node, scr_block_s *block)
{
	while ( 1 )
	{
		node = node[1].node;

		if ( !node )
			break;

		Scr_CalcLocalVarsSafeSetVariableField(*node->node, node->node[1], block);
	}
}

void Scr_CalcLocalVarsSafeSetVariableField2(sval_u expr, sval_u sourcePos, scr_block_s *block)
{
	Scr_RegisterLocalVar(expr.stringValue, sourcePos, block);
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
		Scr_CalcLocalVarsSafeSetVariableField2(expr.node[1], expr.node[2], block);
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
			childBlocks[i]->abortLevel = 0;

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
				CompileError(constValue[i].sourcePos.stringValue, "type %s is not a float", var_typename[type]);
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
	bool result;

	switch ( expr.node->type )
	{
	case ENUM_integer:
		EvalInteger(expr.node[1].intValue, expr.node[2], constValue);
		result = 1;
		break;

	case ENUM_float:
		EvalFloat(expr.node[1].floatValue, expr.node[2], constValue);
		result = 1;
		break;

	case ENUM_minus_integer:
		EvalInteger(-expr.node[1].intValue, expr.node[2], constValue);
		result = 1;
		break;

	case ENUM_minus_float:
		EvalFloat(-expr.node[1].floatValue, expr.node[2], constValue);
		result = 1;
		break;

	case ENUM_string:
		EvalString(expr.node[1].stringValue, expr.node[2], constValue);
		result = 1;
		break;

	case ENUM_istring:
		EvalIString(expr.node[1].stringValue, expr.node[2], constValue);
		result = 1;
		break;

	case ENUM_undefined:
		EvalUndefined(expr.node[1], constValue);
		result = 1;
		break;

	case ENUM_expression_list:
		result = EvalPrimitiveExpressionList(expr.node[1], expr.node[2], constValue);
		break;

	case ENUM_false:
		EvalInteger(0, expr.node[1], constValue);
		result = 1;
		break;

	case ENUM_true:
		EvalInteger(1, expr.node[1], constValue);
		result = 1;
		break;

	default:
		result = 0;
		break;
	}

	return result;
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
		CompileError(sourcePos.stringValue, "%s", scrVarPub.error_message);
		return 0;
	}
	else
	{
		constValue->value.u.intValue = constValue1.value.u.intValue;
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
		return EvalExpression(*exprlist.node->node->node, constValue);

	if ( count != 3 )
		return 0;

	i = 0;

	for ( node = exprlist.node->node; node; node = node[1].node )
	{
		if ( !EvalExpression(*node->node, &constValue2[i]) )
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

	abortLevel = 3;
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
			Scr_CalcLocalVarsStatement(*node, currentBlock);

			if ( currentBlock->abortLevel )
			{
				if ( currentBlock->abortLevel == 2 )
				{
					currentBlock->abortLevel = 0;
					abortLevel = 0;
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
	abortLevel = 3;

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
		Scr_CalcLocalVarsStatement(*node, block);
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
	stmttblock->block->abortLevel = 0;
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

void EmitCastFieldObject(sval_u sourcePos)
{
	EmitOpcode(OP_CastFieldObject, -1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, 0);
}

void EmitSelfObject(sval_u sourcePos)
{
	EmitOpcode(OP_GetSelfObject, 0, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, 1);
}

void EmitLevelObject(sval_u sourcePos)
{
	EmitOpcode(OP_GetLevelObject, 0, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, 1);
}

void EmitAnimObject(sval_u sourcePos)
{
	EmitOpcode(OP_GetAnimObject, 0, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, 1);
}

/*
sval_u *GetSingleParameter(sval_u exprlist)
{
	if ( !exprlist.node->node )
		return 0;

	if ( exprlist.node->node[1].node )
		return 0;

	return exprlist.node->node;
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
		EmitExpressionFieldObject(*node->node, node->node[1], block);
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
*/

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