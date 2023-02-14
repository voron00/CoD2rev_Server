#pragma once

struct scr_entref_t
{
	uint16_t entnum;
	uint16_t classnum;
};

typedef void (*xfunction_t)();
typedef void (*xmethod_t)(scr_entref_t);

typedef struct scr_function_s
{
	const char      *name;
	xfunction_t     call;
	qboolean        developer;
} scr_function_t;

typedef struct scr_method_s
{
	const char     *name;
	xmethod_t      call;
	qboolean       developer;
} scr_method_t;

struct VariableStackBuffer
{
	const char *pos;
	uint16_t size;
	uint16_t bufLen;
	uint16_t localId;
	char time;
	char buf[1];
};

union VariableUnion
{
	int intValue;
	float floatValue;
	unsigned int stringValue;
	const float *vectorValue;
	const char *codePosValue;
	unsigned int pointerValue;
	VariableStackBuffer *stackValue;
	unsigned int entityOffset;
};

union ObjectInfo_u
{
	uint16_t size;
	uint16_t entnum;
	uint16_t nextEntId;
	uint16_t self;
};

struct ObjectInfo
{
	uint16_t status;
	union ObjectInfo_u u;
};

union VariableValueInternal_u
{
	uint16_t refCount;
	uint16_t next;
	union VariableUnion u;
	struct ObjectInfo o;
};

union VariableValueInternal_w
{
	unsigned int status;
	unsigned int type;
	unsigned int name;
	unsigned int classnum;
	unsigned int notifyName;
	unsigned int waitTime;
	unsigned int parentLocalId;
};

union VariableValueInternal_v
{
	uint16_t next;
	uint16_t index;
};

struct VariableValue
{
	union VariableUnion u;
	int type;
};

union Variable_u
{
	uint16_t prev;
	uint16_t prevSibling;
};

struct Variable
{
	uint16_t id;
	union Variable_u u;
};

struct VariableValueInternal
{
	Variable hash;
	union VariableValueInternal_u u;
	union VariableValueInternal_w w;
	union VariableValueInternal_v v;
	uint16_t nextSibling;
};

typedef struct scrVarPub_s
{
	const char *fieldBuffer;
	int canonicalStrMark;
	uint16_t canonicalStrCount;
	byte developer;
	byte developer_script;
	byte evaluate;
	const char *error_message;
	int error_index;
	unsigned int time;
	unsigned int timeArrayId;
	unsigned int pauseArrayId;
	unsigned int levelId;
	unsigned int gameId;
	unsigned int animId;
	unsigned int freeEntList;
	unsigned int tempVariable;
	byte bInited;
	uint16_t savecount;
	unsigned int checksum;
	unsigned int entId;
	unsigned int entFieldName;
	const char *programBuffer;
	const char *endScriptBuffer;
} scrVarPub_t;
static_assert((sizeof(scrVarPub_t) == 0x50), "ERROR: scrVarPub_t size is invalid!");

enum var_type_t
{
	VAR_UNDEFINED,
	VAR_OBJECT,
	VAR_STRING,
	VAR_ISTRING,
	VAR_VECTOR,
	VAR_FLOAT,
	VAR_INTEGER,
	VAR_CODEPOS,
	VAR_PRECODEPOS,
	VAR_FUNCTION,
	VAR_STACK,
	VAR_ANIMATION,
	VAR_DEVELOPER_CODEPOS,
	VAR_INCLUDE_CODEPOS,
	VAR_THREAD_LIST,
	VAR_THREAD,
	VAR_NOTIFY_THREAD,
	VAR_TIME_THREAD,
	VAR_CHILD_THREAD,
	VAR_STRUCT,
	VAR_REMOVED_ENTITY,
	VAR_ENTITY,
	VAR_ARRAY,
	VAR_REMOVED_THREAD,
	VAR_COUNT
};

struct function_stack_t
{
	const char *pos;
	unsigned int localId;
	unsigned int localVarCount;
	VariableValue *top;
	VariableValue *startTop;
};

struct function_frame_t
{
	function_stack_t fs;
	int topType;
};

struct scr_anim_t
{
	union
	{
		struct
		{
			uint16_t index;
			uint16_t tree;
		};
		const char *linkPointer;
	};
};

typedef struct __attribute__((aligned(128))) scrVmPub_s
{
	unsigned int *localVars;
	VariableValue *maxstack;
	int function_count;
	function_frame_t *function_frame;
	VariableValue *top;
	byte debugCode;
	byte abort_on_error;
	byte terminal_error;
	unsigned int inparamcount;
	unsigned int outparamcount;
	function_frame_t function_frame_start[32];
	VariableValue stack[2048];
} scrVmPub_t;
static_assert((sizeof(scrVmPub_t) == 0x4380), "ERROR: scrVmPub_t size is invalid!");

struct scr_animtree_t
{
	XAnim_s *anims;
};

typedef struct scrAnimPub_s
{
	unsigned int animtrees;
	unsigned int animtree_node;
	unsigned int animTreeNames;
	scr_animtree_t xanim_lookup[2][128];
	unsigned int xanim_num[2];
	unsigned int animTreeIndex;
	bool animtree_loading;
} scrAnimPub_t;
static_assert((sizeof(scrAnimPub_t) == 0x41C), "ERROR: scrAnimPub_t size is invalid!");

typedef struct scrCompilePub_s
{
	int value_count;
	int far_function_count;
	unsigned int loadedscripts;
	unsigned int scriptsPos;
	unsigned int builtinFunc;
	unsigned int builtinMeth;
	uint16_t *canonicalStrings;
	const char *in_ptr;
	const char *parseBuf;
	byte script_loading;
	byte allowedBreakpoint;
	int developer_statement;
	char *opcodePos;
	unsigned int programLen;
	int func_table_size;
	int func_table[1024];
} scrCompilePub_t;
static_assert((sizeof(scrCompilePub_t) == 0x1038), "ERROR: scrCompilePub_t size is invalid!");

typedef struct scrParserPub_s
{
	struct SourceBufferInfo *sourceBufferLookup;
	unsigned int sourceBufferLookupLen;
	const char *scriptfilename;
	const char *sourceBuf;
} scrParserPub_t;
static_assert((sizeof(scrParserPub_t) == 0x10), "ERROR: scrParserPub_t size is invalid!");

void SetAnimCheck(int bAnimCheck);

enum scr_opcode_t
{
	OP_End = 0x0,
	OP_Return = 0x1,
	OP_GetUndefined = 0x2,
	OP_GetZero = 0x3,
	OP_GetByte = 0x4,
	OP_GetNegByte = 0x5,
	OP_GetUnsignedShort = 0x6,
	OP_GetNegUnsignedShort = 0x7,
	OP_GetInteger = 0x8,
	OP_GetFloat = 0x9,
	OP_GetString = 0xA,
	OP_GetIString = 0xB,
	OP_GetVector = 0xC,
	OP_GetLevelObject = 0xD,
	OP_GetAnimObject = 0xE,
	OP_GetSelf = 0xF,
	OP_GetLevel = 0x10,
	OP_GetGame = 0x11,
	OP_GetAnim = 0x12,
	OP_GetAnimation = 0x13,
	OP_GetGameRef = 0x14,
	OP_GetFunction = 0x15,
	OP_CreateLocalVariable = 0x16,
	OP_RemoveLocalVariables = 0x17,
	OP_EvalLocalVariableCached0 = 0x18,
	OP_EvalLocalVariableCached1 = 0x19,
	OP_EvalLocalVariableCached2 = 0x1A,
	OP_EvalLocalVariableCached3 = 0x1B,
	OP_EvalLocalVariableCached4 = 0x1C,
	OP_EvalLocalVariableCached5 = 0x1D,
	OP_EvalLocalVariableCached = 0x1E,
	OP_EvalLocalArrayCached = 0x1F,
	OP_EvalArray = 0x20,
	OP_EvalLocalArrayRefCached0 = 0x21,
	OP_EvalLocalArrayRefCached = 0x22,
	OP_EvalArrayRef = 0x23,
	OP_ClearArray = 0x24,
	OP_EmptyArray = 0x25,
	OP_GetSelfObject = 0x26,
	OP_EvalLevelFieldVariable = 0x27,
	OP_EvalAnimFieldVariable = 0x28,
	OP_EvalSelfFieldVariable = 0x29,
	OP_EvalFieldVariable = 0x2A,
	OP_EvalLevelFieldVariableRef = 0x2B,
	OP_EvalAnimFieldVariableRef = 0x2C,
	OP_EvalSelfFieldVariableRef = 0x2D,
	OP_EvalFieldVariableRef = 0x2E,
	OP_ClearFieldVariable = 0x2F,
	OP_SafeCreateVariableFieldCached = 0x30,
	OP_SafeSetVariableFieldCached0 = 0x31,
	OP_SafeSetVariableFieldCached = 0x32,
	OP_SafeSetWaittillVariableFieldCached = 0x33,
	OP_clearparams = 0x34,
	OP_checkclearparams = 0x35,
	OP_EvalLocalVariableRefCached0 = 0x36,
	OP_EvalLocalVariableRefCached = 0x37,
	OP_SetLevelFieldVariableField = 0x38,
	OP_SetVariableField = 0x39,
	OP_SetAnimFieldVariableField = 0x3A,
	OP_SetSelfFieldVariableField = 0x3B,
	OP_SetLocalVariableFieldCached0 = 0x3C,
	OP_SetLocalVariableFieldCached = 0x3D,
	OP_CallBuiltin0 = 0x3E,
	OP_CallBuiltin1 = 0x3F,
	OP_CallBuiltin2 = 0x40,
	OP_CallBuiltin3 = 0x41,
	OP_CallBuiltin4 = 0x42,
	OP_CallBuiltin5 = 0x43,
	OP_CallBuiltin = 0x44,
	OP_CallBuiltinMethod0 = 0x45,
	OP_CallBuiltinMethod1 = 0x46,
	OP_CallBuiltinMethod2 = 0x47,
	OP_CallBuiltinMethod3 = 0x48,
	OP_CallBuiltinMethod4 = 0x49,
	OP_CallBuiltinMethod5 = 0x4A,
	OP_CallBuiltinMethod = 0x4B,
	OP_wait = 0x4C,
	OP_waittillFrameEnd = 0x4D,
	OP_PreScriptCall = 0x4E,
	OP_ScriptFunctionCall2 = 0x4F,
	OP_ScriptFunctionCall = 0x50,
	OP_ScriptFunctionCallPointer = 0x51,
	OP_ScriptMethodCall = 0x52,
	OP_ScriptMethodCallPointer = 0x53,
	OP_ScriptThreadCall = 0x54,
	OP_ScriptThreadCallPointer = 0x55,
	OP_ScriptMethodThreadCall = 0x56,
	OP_ScriptMethodThreadCallPointer = 0x57,
	OP_DecTop = 0x58,
	OP_CastFieldObject = 0x59,
	OP_EvalLocalVariableObjectCached = 0x5A,
	OP_CastBool = 0x5B,
	OP_BoolNot = 0x5C,
	OP_BoolComplement = 0x5D,
	OP_JumpOnFalse = 0x5E,
	OP_JumpOnTrue = 0x5F,
	OP_JumpOnFalseExpr = 0x60,
	OP_JumpOnTrueExpr = 0x61,
	OP_jump = 0x62,
	OP_jumpback = 0x63,
	OP_inc = 0x64,
	OP_dec = 0x65,
	OP_bit_or = 0x66,
	OP_bit_ex_or = 0x67,
	OP_bit_and = 0x68,
	OP_equality = 0x69,
	OP_inequality = 0x6A,
	OP_less = 0x6B,
	OP_greater = 0x6C,
	OP_less_equal = 0x6D,
	OP_greater_equal = 0x6E,
	OP_shift_left = 0x6F,
	OP_shift_right = 0x70,
	OP_plus = 0x71,
	OP_minus = 0x72,
	OP_multiply = 0x73,
	OP_divide = 0x74,
	OP_mod = 0x75,
	OP_size = 0x76,
	OP_waittillmatch = 0x77,
	OP_waittill = 0x78,
	OP_notify = 0x79,
	OP_endon = 0x7A,
	OP_voidCodepos = 0x7B,
	OP_switch = 0x7C,
	OP_endswitch = 0x7D,
	OP_vector = 0x7E,
	OP_NOP = 0x7F,
	OP_abort = 0x80,
	OP_object = 0x81,
	OP_thread_object = 0x82,
	OP_EvalLocalVariable = 0x83,
	OP_EvalLocalVariableRef = 0x84,
	OP_prof_begin = 0x85,
	OP_prof_end = 0x86,
	OP_breakpoint = 0x87,
	OP_assignmentBreakpoint = 0x88,
	OP_manualAndAssignmentBreakpoint = 0x89,
	OP_count = 0x8A,
};

struct scr_localVar_t
{
	unsigned int name;
	unsigned int sourcePos;
};

struct scr_block_s
{
	int abortLevel;
	int localVarsCreateCount;
	int localVarsPublicCount;
	int localVarsCount;
	byte localVarsInitBits[8];
	scr_localVar_t localVars[64];
};

union sval_u
{
	byte type;
	unsigned int stringValue;
	unsigned int idValue;
	float floatValue;
	int intValue;
	sval_u *node;
	unsigned int sourcePosValue;
	const char *codePosValue;
	const char *debugString;
	struct scr_block_s *block;
};

void Scr_Error(const char *error);
void Scr_ParamError(int paramNum, const char *error);
int Scr_GetType(unsigned int param);
void Scr_AddUndefined();
void Scr_AddBool(bool value);
void Scr_AddInt(int value);
void Scr_AddFloat(float value);
void Scr_AddAnim(scr_anim_t value);
void Scr_AddObject(unsigned int id);
void Scr_AddEntityNum(int entnum, unsigned int classnum);
void Scr_AddStruct();
void Scr_AddString(const char *value);
void Scr_AddIString(const char *value);
void Scr_AddConstString(unsigned int value);
void Scr_AddVector(const float *value);
void Scr_MakeArray();
void Scr_AddArray();
void Scr_AddArrayStringIndexed(unsigned int stringValue);
int Scr_GetPointerType(unsigned int index);
void Scr_GetEntityRef(scr_entref_t *entRef, unsigned int index);
int Scr_GetInt(unsigned int index);
double Scr_GetFloat(unsigned int index);
unsigned int Scr_GetConstString(unsigned int index);
unsigned int Scr_GetConstStringIncludeNull(unsigned int index);
const char* Scr_GetString(unsigned int index);
unsigned int Scr_GetConstIString(unsigned int index);
const char* Scr_GetIString(unsigned int index);
void Scr_GetVector(unsigned int index, float *vector);
void VM_CancelNotify(unsigned int notifyListOwnerId, unsigned int startLocalId);
void Scr_Settings(int developer, int developer_script, int abort_on_error);
void Scr_TerminalError(const char *error);
void Scr_Abort();
void Scr_Shutdown();
void Scr_Init();

void MT_Init();
void MT_AddMemoryNode(int newNode, int size);
bool MT_RemoveMemoryNode(int oldNode, int size);
void MT_RemoveHeadMemoryNode(int size);
int MT_GetSize(int numBytes);
void MT_RelocateNode(int nodeNum);
unsigned short MT_AllocIndex(int numBytes);
void MT_FreeIndex(unsigned int nodeNum, int numBytes);
void* MT_Alloc(int numBytes);
void MT_Free(void* p, int numBytes);
byte* MT_GetRefByIndex(int index);
int MT_GetIndexByRef(byte* p);
void MT_FreeForLength(byte *p, unsigned int data, int length);
void *MT_BeginRelocate();
void MT_EndRelocate(byte *ptr);

struct __attribute__((aligned(4))) RefString
{
	union
	{
		struct
		{
			unsigned int length : 8;
			unsigned int user : 8;
			unsigned int refCount : 16;
		};
		volatile int data;
	};
	char str[1];
};

struct RefVector
{
	union
	{
		struct
		{
			unsigned int refCount : 16;
			unsigned int user : 8;
			unsigned int length : 8;
		};
		volatile int head;
	};
	vec3_t vec;
};

#ifdef __cplusplus
extern "C" {
#endif

const char* SL_ConvertToString(unsigned int index);
unsigned int SL_ConvertToLowercase(unsigned int stringValue, unsigned char user);
unsigned int SL_ConvertFromString(const char *str);
unsigned int GetHashCode(const char *str, unsigned int len);
void SL_FreeString(unsigned int stringValue, RefString *refStr, unsigned int len);
void SL_AddUserInternal(RefString *refStr, unsigned char user);
unsigned int SL_GetStringOfLen(const char *str, unsigned char user, unsigned int len);
unsigned int QDECL SL_GetStringOfSize(const char *str, unsigned char user, unsigned int len, int type);
unsigned int SL_FindStringOfLen(const char *str, unsigned int len);
unsigned int SL_FindString(const char *string);
unsigned int SL_GetString_(const char *str, unsigned char user);
unsigned int SL_GetString(const char *str, unsigned char user);
unsigned int SL_GetStringForInt(int i);
unsigned int SL_GetStringForFloat(float f);
unsigned int SL_GetStringForVector(const float *v);
void Scr_SetString(uint16_t *to, unsigned int from);
void SL_RemoveRefToString(unsigned int stringValue);
void SL_TransferRefToUser(unsigned int stringIndex, unsigned char user);
void SL_ChangeUser(unsigned char from, unsigned char to);
void SL_AddRefToString(unsigned int stringValue);
void SL_Shutdown();
void SL_CheckInit();

#ifdef __cplusplus
}
#endif

unsigned int FindNextSibling(unsigned int id);
unsigned int FindVariable(unsigned int parentId, unsigned int name);
unsigned int FindVariableIndex(unsigned int parentId, unsigned int name);
unsigned int FindObjectVariable(unsigned int parentId, unsigned int id);
unsigned int FindObject(unsigned int id);
void AddRefToVector(const float *vectorValue);
void RemoveRefToVector(const float *vectorValue);
void MakeVariableExternal(VariableValueInternal *value, VariableValueInternal *parentValue);
unsigned int GetNewObjectVariableReverse(unsigned int parentId, unsigned int id);
unsigned int GetVariable(unsigned int parentId, unsigned int name);
void Scr_EvalVariable(VariableValue *val, unsigned int index);
unsigned int GetNewArrayVariableIndex(unsigned int parentId, unsigned int index);
unsigned int GetNewArrayVariable(unsigned int parentId, unsigned int index);
void FreeVariable(unsigned int id);
void SetNewVariableValue(unsigned int id, VariableValue *value);
unsigned int GetNewVariable(unsigned int parentId, unsigned int name);
void RemoveRefToEmptyObject(unsigned int id);
void AddRefToObject(unsigned int id);
void AddRefToValue(VariableValue *val);
void FreeChildValue(unsigned int id);
void ClearObject(unsigned int parentId);
void RemoveVariable(unsigned int parentId, unsigned int index);
void RemoveNextVariable(unsigned int index);
void RemoveArrayVariable(unsigned int parentId, unsigned int unsignedValue);
void RemoveObjectVariable(unsigned int parentId, unsigned int id);
void RemoveRefToObject(unsigned int id);
void RemoveRefToValue(VariableValue *value);
unsigned int FindArrayVariable(unsigned int parentId, unsigned int index);
unsigned int GetParentLocalId(unsigned int threadId);
unsigned int GetSafeParentLocalId(unsigned int threadId);
unsigned int GetVarType(int varIndex);
unsigned int AllocVariable();
unsigned int AllocValue();
unsigned int AllocEntity(unsigned int classnum, unsigned short entnum);
float* Scr_AllocVector(const float* vec);
unsigned int Scr_GetEntityId(int entnum, unsigned int classnum);
unsigned short Scr_GetThreadNotifyName(unsigned int startLocalId);
void Scr_RemoveThreadNotifyName(unsigned int startLocalId);
void Scr_KillThread(unsigned int parentId);
unsigned int GetArraySize(unsigned int id);
unsigned int Scr_GetSelf(unsigned int id);
void Scr_GetEntityIdRef(scr_entref_t *entRef, unsigned int entId);
void Scr_FreeEntityNum(int entnum, unsigned int classnum);
unsigned int AllocObject();
unsigned int Scr_AllocArray();
void Scr_EvalArray(VariableValue *value, VariableValue *index);
void Scr_AddClassField(unsigned int classnum, const char *name, unsigned short offset);
bool Scr_CastString(VariableValue *value);
void Scr_DumpScriptThreads();
void Scr_DumpScriptVariablesDefault();
void Var_Shutdown();
void Var_FreeTempVariables();
void Var_Init();

enum fieldtype_t
{
	F_INT = 0x0,
	F_FLOAT = 0x1,
	F_LSTRING = 0x2,
	F_STRING = 0x3,
	F_VECTOR = 0x4,
	F_ENTITY = 0x5,
	F_VECTORHACK = 0x6,
	F_OBJECT = 0x7,
	F_MODEL = 0x8,
};

void Scr_GetGenericField(const byte *data, int fieldtype, int offset);
void Scr_SetGenericField(byte *data, int fieldtype, int offset);
unsigned int Scr_GetCanonicalStringIndex(unsigned int index);
unsigned int SL_GetCanonicalString(const char *str);

#ifdef __cplusplus
extern "C" {
#endif

void CompileError(unsigned int sourcePos, const char *format, ...);
int Scr_ScanFile(char *buf, int max_size);
void QDECL Scr_YYACError(const char* fmt, ...);
void QDECL ScriptParse(sval_u *source, char user);

#ifdef __cplusplus
}
#endif