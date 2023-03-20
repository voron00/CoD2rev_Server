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

#ifdef TESTING_LIBRARY
#define scrAnimPub (*((scrAnimPub_t*)( 0x08202440 )))
#else
extern scrAnimPub_t scrAnimPub;
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

void EmitBoolComplement(sval_u sourcePos)
{
	EmitOpcode(OP_BoolComplement, 0, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, 0);
}

void EmitBoolNot(sval_u sourcePos)
{
	EmitOpcode(OP_BoolNot, 0, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, 0);
}

void EmitDecTop()
{
	EmitOpcode(OP_DecTop, -1, CALL_NONE);
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
	AddOpcodePos(indexSourcePos.sourcePosValue, 0);
	AddOpcodePos(sourcePos.sourcePosValue, 1);
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
	const char *s;
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
		s = SL_ConvertToString(name);
		CompileError(sourcePos.sourcePosValue, "uninitialised variable '%s'", s);
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
			AddOpcodePos(node->node[1].sourcePosValue, 0);
	}
}

void Scr_BeginDevScript(int *type, char **savedPos)
{
	if ( scrCompilePub.developer_statement )
	{
		*type = 0;
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

		*type = 1;
	}
}

void Scr_EndDevScript(int type, char **savedPos)
{
	if ( type == 1 )
	{
		scrCompilePub.developer_statement = 0;

		if ( !scrVarPub.developer_script )
			TempMemorySetPos(*savedPos);
	}
}

int Scr_GetCacheType(int type)
{
	if ( type )
		return 12;
	else
		return 7;
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
		opcode = param_count + 62;

	EmitOpcode(opcode, 1 - param_count, CALL_BUILTIN);
	AddOpcodePos(sourcePos.sourcePosValue, 1);

	if ( opcode == OP_CallBuiltin )
		EmitByte(param_count);
}

void EmitCallBuiltinMethodOpcode(int param_count, sval_u sourcePos)
{
	int opcode;

	if ( param_count > 5 )
		opcode = OP_CallBuiltinMethod;
	else
		opcode = param_count + 69;

	EmitOpcode(opcode, -param_count, CALL_BUILTIN);
	AddOpcodePos(sourcePos.sourcePosValue, 1);

	if ( opcode == OP_CallBuiltinMethod )
		EmitByte(param_count);
}

void EmitSelf(sval_u sourcePos)
{
	EmitOpcode(OP_GetSelf, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, 1);
}

void EmitLevel(sval_u sourcePos)
{
	EmitOpcode(OP_GetLevel, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, 1);
}

void EmitGame(sval_u sourcePos)
{
	EmitOpcode(OP_GetGame, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, 1);
}

void EmitAnim(sval_u sourcePos)
{
	EmitOpcode(OP_GetAnim, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, 1);
}

void EmitEmptyArray(sval_u sourcePos)
{
	EmitOpcode(OP_EmptyArray, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, 1);
}

void EmitAnimation(sval_u anim, sval_u sourcePos)
{
	EmitOpcode(OP_GetAnimation, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, 1);
	EmitCodepos((const char *)0xFFFFFFFF);
	Scr_EmitAnimation(scrCompileGlob.codePos, anim.stringValue, sourcePos.sourcePosValue);
	Scr_CompileRemoveRefToString(anim.stringValue);
}

void EmitCastBool(sval_u sourcePos)
{
	EmitOpcode(OP_CastBool, 0, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, 0);
}

void EmitAnimTree(sval_u sourcePos)
{
	if ( scrAnimPub.animTreeIndex )
		EmitGetInteger(scrAnimPub.animTreeIndex, sourcePos);
	else
		CompileError(sourcePos.sourcePosValue, "#using_animtree was not specified");
}

void Scr_PushValue(VariableCompileValue *constValue)
{
	int index;

	if ( scrCompilePub.value_count <= 31 )
	{
		index = scrCompilePub.value_count;
		scrCompileGlob.value_start[index].value.type = constValue->value.u.intValue;
		scrCompileGlob.value_start[index].sourcePos.stringValue = constValue->value.type;
		scrCompileGlob.value_start[index + 1].value.u.intValue = constValue->sourcePos.intValue;
		++scrCompilePub.value_count;
	}
	else
	{
		CompileError(constValue->sourcePos.stringValue, "VALUE_STACK_SIZE exceeded");
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
		return EmitOrEvalExpression(*exprlist.node->node->node, constValue, block);

	if ( expr_count == 3 )
	{
		success = 1;

		for ( node = exprlist.node->node; node; node = node[1].node )
		{
			if ( success )
			{
				success = EmitOrEvalExpression(*node->node, &constValue2, block);

				if ( success )
					Scr_PushValue(&constValue2);
			}
			else
			{
				EmitExpression(*node->node, block);
			}
		}
		if ( success )
		{
			scrCompilePub.value_count -= 3;
			Scr_CreateVector((VariableCompileValue *)(const char *)(&scrCompileGlob.value_start[scrCompilePub.value_count].sourcePos.codePosValue - 1), &constValue->value);
			constValue->sourcePos = sourcePos;
			return 1;
		}
		else
		{
			EmitOpcode(OP_vector, -2, CALL_NONE);
			AddOpcodePos(sourcePos.sourcePosValue, 1);
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
	AddOpcodePos(expr1sourcePos.stringValue, 0);
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
	AddOpcodePos(expr1sourcePos.stringValue, 0);
	EmitShort(0);
	pos = scrCompileGlob.codePos;
	nextPos = (intptr_t)TempMalloc(0);
	EmitExpression(expr2, block);
	EmitCastBool(expr2sourcePos);
	*(uint16_t *)pos = (intptr_t)TempMalloc(0) - nextPos;
}

char EmitOrEvalBinaryOperatorExpression(sval_u expr1, sval_u expr2, sval_u opcode, sval_u sourcePos, VariableCompileValue *constValue, scr_block_s *block)
{
	VariableCompileValue constValue2;
	VariableCompileValue constValue1;

	if ( !EmitOrEvalExpression(expr1, &constValue1, block) )
	{
		EmitExpression(expr2, block);
emitOpcode:
		EmitOpcode(SLOBYTE(opcode.sourcePosValue), -1, CALL_NONE);
		AddOpcodePos(sourcePos.stringValue, 0);
		return 0;
	}

	Scr_PushValue(&constValue1);

	if ( !EmitOrEvalExpression(expr2, &constValue2, block) )
		goto emitOpcode;

	Scr_PopValue();
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

void EmitSize(sval_u expr, sval_u sourcePos, scr_block_s *block)
{
	EmitPrimitiveExpression(expr, block);
	EmitOpcode(OP_size, 0, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, 0);
}

void EmitGetFunction(sval_u func, sval_u sourcePos)
{
	EmitOpcode(OP_GetFunction, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, 3);
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
	bool result;

	switch ( expr.node->type )
	{
	case ENUM_variable:
		EmitVariableExpression(expr.node[1], block);
		result = 0;
		break;

	case ENUM_function:
		EmitGetFunction(expr.node[1], expr.node[2]);
		result = 0;
		break;

	case ENUM_call_expression:
		EmitCallExpression(expr.node[1], 0, block);
		result = 0;
		break;

	case ENUM_self:
		EmitSelf(expr.node[1]);
		result = 0;
		break;

	case ENUM_level:
		EmitLevel(expr.node[1]);
		result = 0;
		break;

	case ENUM_game:
		EmitGame(expr.node[1]);
		result = 0;
		break;

	case ENUM_anim:
		EmitAnim(expr.node[1]);
		result = 0;
		break;

	case ENUM_expression_list:
		result = EmitOrEvalPrimitiveExpressionList(expr.node[1], expr.node[2], constValue, block);
		break;

	case ENUM_size_field:
		EmitSize(expr.node[1], expr.node[2], block);
		result = 0;
		break;

	case ENUM_empty_array:
		EmitEmptyArray(expr.node[1]);
		result = 0;
		break;

	case ENUM_animation:
		EmitAnimation(expr.node[1], expr.node[2]);
		result = 0;
		break;

	case ENUM_animtree:
		EmitAnimTree(expr.node[1]);
		result = 0;
		break;

	default:
		result = EvalPrimitiveExpression(expr, constValue);
		break;
	}

	return result;
}

bool EmitOrEvalExpression(sval_u expr, VariableCompileValue *constValue, scr_block_s *block)
{
	bool result;

	switch ( expr.node->type )
	{
	case ENUM_primitive_expression:
		result = EmitOrEvalPrimitiveExpression(expr.node[1], constValue, block);
		break;

	case ENUM_bool_or:
		EmitBoolOrExpression(expr.node[1], expr.node[2], expr.node[3], expr.node[4], block);
		result = 0;
		break;

	case ENUM_bool_and:
		EmitBoolAndExpression(expr.node[1], expr.node[2], expr.node[3], expr.node[4], block);
		result = 0;
		break;

	case ENUM_binary:
		result = EmitOrEvalBinaryOperatorExpression(expr.node[1], expr.node[2], expr.node[3], expr.node[4], constValue, block);
		break;

	case ENUM_bool_not:
		EmitExpression(expr.node[1], block);
		EmitBoolNot(expr.node[2]);
		result = 0;
		break;

	case ENUM_bool_complement:
		EmitExpression(expr.node[1], block);
		EmitBoolComplement(expr.node[2]);
		result = 0;
		break;

	default:
		result = 0;
		break;
	}

	return result;
}

void EmitPostScriptFunction(sval_u func, int param_count, bool bMethod, sval_u nameSourcePos)
{
	if ( bMethod )
		EmitOpcode(OP_ScriptMethodCall, -param_count - 1, CALL_FUNCTION);
	else
		EmitOpcode(OP_ScriptFunctionCall, -param_count, CALL_FUNCTION);

	AddOpcodePos(nameSourcePos.sourcePosValue, 3);
	EmitFunction(func, nameSourcePos);
}

void EmitPostScriptFunctionPointer(sval_u expr, int param_count, bool bMethod, sval_u nameSourcePos, sval_u sourcePos, scr_block_s *block)
{
	EmitExpression(expr, block);

	if ( bMethod )
		EmitOpcode(OP_ScriptMethodCallPointer, -param_count - 2, CALL_FUNCTION);
	else
		EmitOpcode(OP_ScriptFunctionCallPointer, -param_count - 1, CALL_FUNCTION);

	AddOpcodePos(sourcePos.stringValue, 0);
	AddOpcodePos(nameSourcePos.sourcePosValue, 1);
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

	AddOpcodePos(sourcePos.sourcePosValue, 3);
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

	AddOpcodePos(sourcePos.sourcePosValue, 1);
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

	AddOpcodePos(sourcePos.sourcePosValue, 0);
}

void EmitFieldVariable(sval_u expr, sval_u field, sval_u sourcePos, scr_block_s *block)
{
	EmitPrimitiveExpressionFieldObject(expr, sourcePos, block);
	EmitOpcode(OP_EvalFieldVariable, 1, CALL_NONE);
	AddOpcodePos(sourcePos.sourcePosValue, 0);
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
		CompileError(sourcePos.stringValue, "$ can only be used in the script debugger");
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
				type = GetVarType((unsigned short)idValue);

				if ( type >= VAR_THREAD && (type <= VAR_CHILD_THREAD || type == VAR_REMOVED_THREAD) )
				{
					EmitOpcode(OP_thread_object, 1, CALL_NONE);
					EmitShort(idValue);
					return;
				}
			}
		}
		goto out;
	}

	classnum = (const char *)Scr_GetClassnumForCharId(*s);

	if ( (int)classnum < 0 || (entnum = (const char *)atoi(s + 1)) == 0 && s[1] != 48 )
	{
out:
		CompileError(sourcePos.stringValue, "bad expression");
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

	index = Scr_FindLocalVarIndex(expr.stringValue, sourcePos, 0, block);

	if ( index > 5 )
		opcode = OP_EvalLocalVariableCached;
	else
		opcode = index + 24;

	EmitOpcode(opcode, 1, CALL_NONE);

	if ( opcode == OP_EvalLocalVariableCached )
		EmitByte(index);

	AddOpcodePos(sourcePos.sourcePosValue, 1);
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
				CompileError(expr.node[2].stringValue, "self field can only be used in the script debugger");
			else
				CompileError(expr.node[2].stringValue, "self field in assignment expression not currently supported");
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
		EmitExpression(*node->node, block);
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
	VariableValue lValue;
	VariableValue value;
	unsigned int funcId;
	char *savedPos;
	void (*func)();
	int type;
	const char *pName;
	sval_u sourcePos;
	unsigned int index;
	int param_count;
	bool statement;

	statement = bStatement;
	index = Scr_GetBuiltin(func_name);

	if ( !index )
		goto script_function;

	pName = SL_ConvertToString(index);
	sourcePos.sourcePosValue = func_name.node[2].sourcePosValue;
	funcId = FindVariable(scrCompilePub.builtinFunc, index);

	if ( funcId )
	{
		Scr_EvalVariable(&lValue, funcId);
		value = lValue;
		type = Scr_IsVariable(lValue.type);
		func = (void (*)())value.u.intValue;
	}
	else
	{
		type = 0;
		func = Scr_GetFunction(&pName, &type);
		funcId = GetNewVariable(scrCompilePub.builtinFunc, index);
		value.type = Scr_GetCacheType(type);
		value.u.intValue = (int)func;
		SetVariableValue(funcId, &value);
	}
	if ( func )
	{
		if ( type == 1 && (Scr_BeginDevScript(&type, &savedPos), type == 1) && !statement )
		{
			CompileError(
			    sourcePos.sourcePosValue,
			    "return value of developer command can not be accessed if not in a /# ... #/ comment");
		}
		else
		{
			param_count = EmitExpressionList(params, block);
			if ( param_count <= 255 )
			{
				Scr_CompileRemoveRefToString(index);
				EmitCallBuiltinOpcode(param_count, sourcePos);
				newFuncIndex = AddFunction((int)func);
				EmitShort(newFuncIndex);
				AddExpressionListOpcodePos(params);
				if ( statement )
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
		if ( statement )
			EmitDecTop();
	}
}

void EmitFunction(sval_u func, sval_u sourcePos)
{
	const char *funcName;
	unsigned int Variable;
	VariableValue evalValue2;
	VariableValue evalValue;
	VariableValue val;
	int scope;
	VariableValue lValue;
	unsigned int index;
	bool defined;
	unsigned int name;
	unsigned int parentId;
	unsigned int id;
	unsigned int countId;
	int varIndex;
	unsigned int object;
	VariableValue value;
	VariableValue value2;

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

	object = 0;

	if ( func.node->type == ENUM_local_function )
	{
		scope = 0;
		varIndex = GetVariable(scrCompileGlob.filePosId, func.node[1].idValue);
		CompileTransferRefToString(func.node[1].stringValue, 2u);
		object = GetObjectA(varIndex);
	}
	else
	{
		scope = 1;
		funcName = SL_ConvertToString(func.node[1].stringValue);
		name = Scr_CreateCanonicalFilename(funcName);
		Scr_CompileRemoveRefToString(func.node[1].stringValue);
		Variable = FindVariable(scrCompilePub.loadedscripts, name);
		Scr_EvalVariable(&val, Variable);
		value = val;
		defined = val.type != VAR_UNDEFINED;
		parentId = AddFilePrecache(name, sourcePos.sourcePosValue, 0);
		if ( defined )
		{
			varIndex = FindVariable(parentId, func.node[2].idValue);
			if ( !varIndex || GetVarType(varIndex) != VAR_OBJECT )
			{
				CompileError(sourcePos.sourcePosValue, "unknown function");
				return;
			}
		}
		else
		{
			varIndex = GetVariable(parentId, func.node[2].idValue);
		}
		CompileTransferRefToString(func.node[2].stringValue, 2u);
		object = GetObjectA(varIndex);
		index = FindVariable(object, 1u);
		if ( index )
		{
			Scr_EvalVariable(&evalValue, index);
			lValue = evalValue;
			if ( evalValue.type == VAR_INCLUDE_CODEPOS )
			{
				CompileError(sourcePos.sourcePosValue, "unknown function");
				return;
			}
			if ( lValue.u.intValue )
			{
				if ( lValue.type == VAR_CODEPOS || scrCompilePub.developer_statement )
					EmitCodepos(lValue.u.codePosValue);
				else
					CompileError(sourcePos.sourcePosValue, "normal script cannot reference a function in a /# ... #/ comment");
				return;
			}
		}
	}

	EmitCodepos((const char *)scope);
	countId = GetVariable(object, 0);
	Scr_EvalVariable(&evalValue2, countId);
	value2 = evalValue2;
	if ( evalValue2.type == VAR_UNDEFINED )
	{
		value2.type = VAR_INTEGER;
		value2.u.intValue = 0;
	}
	id = GetNewVariable(object, value2.u.intValue + 2);
	value.u.codePosValue = scrCompileGlob.codePos;
	if ( scrCompilePub.developer_statement )
		value.type = VAR_DEVELOPER_CODEPOS;
	else
		value.type = VAR_CODEPOS;
	SetNewVariableValue(id, &value);
	++value2.u.intValue;
	SetVariableValue(countId, &value2);
	AddOpcodePos(sourcePos.sourcePosValue, 0);
}

void EmitMethod(sval_u expr, sval_u func_name, sval_u params, sval_u methodSourcePos, bool bStatement, scr_block_s *block)
{
	short newFuncIndex;
	VariableValue lValue;
	VariableValue value;
	unsigned int funcId;
	char *savedPos;
	void (*meth)(scr_entref_t);
	int type;
	const char *pName;
	sval_u sourcePos;
	unsigned int index;
	int param_count;
	bool statement;

	statement = bStatement;
	index = Scr_GetBuiltin(func_name);

	if ( !index )
		goto script_method;

	pName = SL_ConvertToString(index);
	sourcePos.sourcePosValue = func_name.node[2].sourcePosValue;
	funcId = FindVariable(scrCompilePub.builtinMeth, index);

	if ( funcId )
	{
		Scr_EvalVariable(&lValue, funcId);
		value = lValue;
		type = Scr_IsVariable(lValue.type);
		meth = (void (*)(scr_entref_t))value.u.intValue;
	}
	else
	{
		type = 0;
		meth = Scr_GetMethod(&pName, &type);
		funcId = GetNewVariable(scrCompilePub.builtinMeth, index);
		value.type = Scr_GetCacheType(type);
		value.u.intValue = (int)meth;
		SetVariableValue(funcId, &value);
	}
	if ( meth )
	{
		if ( type == 1 && (Scr_BeginDevScript(&type, &savedPos), type == 1) && !statement )
		{
			CompileError(
			    sourcePos.sourcePosValue,
			    "return value of developer command can not be accessed if not in a /# ... #/ comment");
		}
		else
		{
			param_count = EmitExpressionList(params, block);
			EmitPrimitiveExpression(expr, block);
			if ( param_count <= 255 )
			{
				Scr_CompileRemoveRefToString(index);
				EmitCallBuiltinMethodOpcode(param_count, sourcePos);
				newFuncIndex = AddFunction((int)meth);
				EmitShort(newFuncIndex);
				AddOpcodePos(methodSourcePos.sourcePosValue, 0);
				AddExpressionListOpcodePos(params);
				if ( statement )
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
		AddOpcodePos(methodSourcePos.sourcePosValue, 0);
		AddExpressionListOpcodePos(params);
		if ( statement )
			EmitDecTop();
	}
}
