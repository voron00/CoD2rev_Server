#pragma once

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

enum
{
	MEMORY_NODE_BITS = 0x10,
	MEMORY_NODE_COUNT = 0x10000,
	MT_SIZE = 0x100000,
	REFSTRING_STRING_OFFSET = 0x4,
};

enum
{
	MT_NODE_SIZE = 0x10
};

struct __attribute__((aligned(8))) MemoryNode
{
	uint16_t prev;
	uint16_t next;
};

struct scrMemTreePub_t
{
	char *mt_buffer;
};

typedef struct __attribute__((aligned(128))) scrMemTreeGlob_s
{
	MemoryNode nodes[MEMORY_NODE_COUNT];
	char leftBits[256];
	char numBits[256];
	char logBits[256];
	uint16_t head[MEMORY_NODE_BITS + 1];
	int totalAlloc;
	int totalAllocBuckets;
} scrMemTreeGlob_t;

#pragma pack(push)
#pragma pack(1)
struct VariableStackBuffer
{
	const char *pos;
	uint16_t size;
	uint16_t bufLen;
	uint16_t localId;
	unsigned char time;
	char buf[1];
};
#pragma pack(pop)

union VariableUnion
{
	int intValue;
	long int int64Value;
	float floatValue;
	unsigned int stringValue;
	const float *vectorValue;
	const char *codePosValue;
	uintptr_t pointerValue;
	VariableStackBuffer *stackValue;
	unsigned int entityOffset;
};

#define STACKBUF_BUFFER_SIZE int( sizeof(VariableUnion) + sizeof(unsigned char) )

union ObjectInfo_u
{
	uint16_t size;
	uint16_t entnum;
	uint16_t nextEntId;
	uint16_t self;
};

struct ObjectInfo
{
	uint16_t refCount;
	union ObjectInfo_u u;
};

union VariableValueInternal_u
{
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
	bool developer;
	bool developer_script;
	bool evaluate;
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
	bool bInited;
	uint16_t savecount;
	unsigned int checksum;
	unsigned int entId;
	unsigned int entFieldName;
	const char *programBuffer;
	const char *endScriptBuffer;
} scrVarPub_t;

extern scrVarPub_t scrVarPub;

enum
{
	FUNC_SCOPE_LOCAL,
	FUNC_SCOPE_FAR
};

enum
{
	SCR_DEV_NO,
	SCR_DEV_YES,
	SCR_DEV_IGNORE,
	SCR_DEV_EVALUATE
};

enum
{
	MAX_VM_STACK_DEPTH = 0x20,
	MAX_VM_OPERAND_STACK = 0x800,
};

#define MAX_EMBEDDED_FUNCTION_CALLS MAX_VM_STACK_DEPTH - 2

enum
{
	SCR_SYS_GAME = 0x1,
};

enum var_type_t
{
	VAR_UNDEFINED,
	VAR_POINTER,
	VAR_STRING,
	VAR_ISTRING,
	VAR_VECTOR,
	VAR_FLOAT,
	VAR_INTEGER,
	VAR_INT64,
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
	VAR_OBJECT,
	VAR_DEAD_ENTITY,
	VAR_ENTITY,
	VAR_ARRAY,
	VAR_DEAD_THREAD,
	VAR_COUNT
};

#define VAR_BEGIN_REF VAR_POINTER
#define VAR_END_REF VAR_FLOAT

#define FIRST_OBJECT VAR_THREAD
#define FIRST_NONFIELD_OBJECT VAR_ARRAY
#define FIRST_DEAD_OBJECT VAR_DEAD_THREAD

#define VARIABLELIST_CHILD_SIZE 0xFFFE
#define VARIABLELIST_PARENT_SIZE 0x8000

#define MAX_ARRAYINDEX 0x800000

#define VAR_MASK 0x1F
#define VAR_TYPE(var) (var->w.type & VAR_MASK)

#define IsObject(var) ((var->w.type & VAR_MASK) >= VAR_THREAD)
#define IsObjectVal(var) ((var->type & VAR_MASK) >= VAR_THREAD)

#define OBJECT_STACK 0x1FFFF
#define OBJECT_NOTIFY_LIST 0x1FFFE

enum var_bits_t
{
	VAR_NAME_BITS = 0x8,
	VAR_NAME_LOW_MASK = 0xFFFFFF,
	VAR_NAME_HIGH_MASK = 0xFFFFFF00,
	VAR_MIN_INT = 0xFF800000,
	VAR_MAX_INT = 0x7FFFFF,
	ENTITY_NUM_BITS = 0xE,
};

enum var_stat_t
{
	VAR_STAT_FREE = 0x0,
	VAR_STAT_MOVABLE = 0x20,
	VAR_STAT_HEAD = 0x40,
	VAR_STAT_EXTERNAL = 0x60,
	VAR_STAT_MASK = 0x60,
};

enum hash_stat_t
{
	HASH_STAT_FREE = 0x0,
	HASH_STAT_MOVABLE = 0x4000,
	HASH_STAT_HEAD = 0x8000,
	HASH_STAT_MASK = 0xC000,
};

#define HASH_NEXT_MASK 0x3FFF

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

typedef struct scr_anim_s
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
} scr_anim_t;

typedef struct __attribute__((aligned(128))) scrVmPub_s
{
	unsigned int *localVars;
	VariableValue *maxstack;
	int function_count;
	function_frame_t *function_frame;
	VariableValue *top;
	bool debugCode;
	bool abort_on_error;
	bool terminal_error;
	unsigned int inparamcount;
	unsigned int outparamcount;
	function_frame_t function_frame_start[MAX_VM_STACK_DEPTH];
	VariableValue stack[MAX_VM_OPERAND_STACK];
} scrVmPub_t;

extern scrVmPub_t scrVmPub;

typedef struct scrVmGlob_s
{
	VariableValue eval_stack[2];
	const char *dialog_error_message;
	int loading;
	int starttime;
	unsigned int localVarsStack[MAX_VM_OPERAND_STACK];
} scrVmGlob_t;

extern scrVmGlob_t scrVmGlob;

#define MAX_XANIMTREE_NUM 128

enum
{
	SCR_XANIM_CLIENT,
	SCR_XANIM_SERVER,
	SCR_XANIM_COUNT
};

enum
{
	ANIMTREE_NAMES,
	ANIMTREE_XANIM
};

struct scr_animtree_t
{
	struct XAnim_s *anims;
};

typedef struct scrAnimPub_s
{
	unsigned int animtrees;
	unsigned int animtree_node;
	unsigned int animTreeNames;
	scr_animtree_t xanim_lookup[SCR_XANIM_COUNT][MAX_XANIMTREE_NUM];
	unsigned int xanim_num[SCR_XANIM_COUNT];
	unsigned int animTreeIndex;
	bool animtree_loading;
} scrAnimPub_t;

extern scrAnimPub_t scrAnimPub;

typedef struct scrAnimGlob_s
{
	const char *start;
	const char *pos;
	unsigned short using_xanim_lookup[SCR_XANIM_COUNT][MAX_XANIMTREE_NUM];
	int bAnimCheck;
} scrAnimGlob_t;

extern scrAnimGlob_t scrAnimGlob;

#define SCR_FUNC_TABLE_SIZE 1024

typedef struct scrCompilePub_s
{
	int value_count;
	int far_function_count;
	unsigned int loadedscripts;
	unsigned int scriptsPos;
	unsigned int builtinFunc;
	unsigned int builtinMeth;
	uint16_t *archivedCanonicalStringsBuf;
	const char *in_ptr;
	const char *parseBuf;
	bool script_loading;
	bool allowedBreakpoint;
	int developer_statement;
	byte *opcodePos;
	unsigned int programLen;
	int func_table_size;
	intptr_t func_table[SCR_FUNC_TABLE_SIZE];
} scrCompilePub_t;

extern scrCompilePub_t scrCompilePub;
inline char g_EndPos;

struct SourceLookup
{
	unsigned int sourcePos;
	int type;
};

struct OpcodeLookup
{
	const char *codePos;
	unsigned int sourcePosIndex;
	int sourcePosCount;
	int profileTime;
	int profileUsage;
};

struct SourceBufferInfo
{
	const char *codePos;
	char *buf;
	const char *sourceBuf;
	int len;
	int sortedIndex;
	bool archive;
};

struct SaveSourceBufferInfo
{
	char *sourceBuf;
	int len;
};

typedef struct scrParserPub_s
{
	struct SourceBufferInfo *sourceBufferLookup;
	unsigned int sourceBufferLookupLen;
	const char *scriptfilename;
	const char *sourceBuf;
} scrParserPub_t;

extern scrParserPub_t scrParserPub;

typedef struct scrParserGlob_s
{
	OpcodeLookup *opcodeLookup;
	unsigned int opcodeLookupMaxLen;
	unsigned int opcodeLookupLen;
	SourceLookup *sourcePosLookup;
	unsigned int sourcePosLookupMaxLen;
	unsigned int sourcePosLookupLen;
	unsigned int sourceBufferLookupMaxLen;
	const byte *currentCodePos;
	unsigned int currentSourcePosCount;
	SaveSourceBufferInfo *saveSourceBufferLookup;
	unsigned int saveSourceBufferLookupLen;
	int delayedSourceIndex;
	int threadStartSourceIndex;
} scrParserGlob_t;

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

enum scr_enum_t
{
	ENUM_NOP = 0x0,
	ENUM_program = 0x1,
	ENUM_assignment = 0x2,
	ENUM_unknown_variable = 0x3,
	ENUM_local_variable = 0x4,
	ENUM_local_variable_frozen = 0x5,
	ENUM_primitive_expression = 0x6,
	ENUM_integer = 0x7,
	ENUM_float = 0x8,
	ENUM_minus_integer = 0x9,
	ENUM_minus_float = 0xA,
	ENUM_string = 0xB,
	ENUM_istring = 0xC,
	ENUM_array_variable = 0xD,
	ENUM_unknown_field = 0xE,
	ENUM_field_variable = 0xF,
	ENUM_field_variable_frozen = 0x10,
	ENUM_variable = 0x11,
	ENUM_function = 0x12,
	ENUM_call_expression = 0x13,
	ENUM_local_function = 0x14,
	ENUM_far_function = 0x15,
	ENUM_function_pointer = 0x16,
	ENUM_call = 0x17,
	ENUM_method = 0x18,
	ENUM_call_expression_statement = 0x19,
	ENUM_script_call = 0x1A,
	ENUM_return = 0x1B,
	ENUM_return2 = 0x1C,
	ENUM_wait = 0x1D,
	ENUM_script_thread_call = 0x1E,
	ENUM_undefined = 0x1F,
	ENUM_self = 0x20,
	ENUM_self_frozen = 0x21,
	ENUM_level = 0x22,
	ENUM_game = 0x23,
	ENUM_anim = 0x24,
	ENUM_if = 0x25,
	ENUM_if_else = 0x26,
	ENUM_while = 0x27,
	ENUM_for = 0x28,
	ENUM_inc = 0x29,
	ENUM_dec = 0x2A,
	ENUM_binary_equals = 0x2B,
	ENUM_statement_list = 0x2C,
	ENUM_developer_statement_list = 0x2D,
	ENUM_expression_list = 0x2E,
	ENUM_bool_or = 0x2F,
	ENUM_bool_and = 0x30,
	ENUM_binary = 0x31,
	ENUM_bool_not = 0x32,
	ENUM_bool_complement = 0x33,
	ENUM_size_field = 0x34,
	ENUM_self_field = 0x35,
	ENUM_precachetree = 0x36,
	ENUM_waittill = 0x37,
	ENUM_waittillmatch = 0x38,
	ENUM_waittillFrameEnd = 0x39,
	ENUM_notify = 0x3A,
	ENUM_endon = 0x3B,
	ENUM_switch = 0x3C,
	ENUM_case = 0x3D,
	ENUM_default = 0x3E,
	ENUM_break = 0x3F,
	ENUM_continue = 0x40,
	ENUM_expression = 0x41,
	ENUM_empty_array = 0x42,
	ENUM_animation = 0x43,
	ENUM_thread = 0x44,
	ENUM_begin_developer_thread = 0x45,
	ENUM_end_developer_thread = 0x46,
	ENUM_usingtree = 0x47,
	ENUM_false = 0x48,
	ENUM_true = 0x49,
	ENUM_animtree = 0x4A,
	ENUM_breakon = 0x4B,
	ENUM_breakpoint = 0x4C,
	ENUM_prof_begin = 0x4D,
	ENUM_prof_end = 0x4E,
	ENUM_vector = 0x4F,
	ENUM_object = 0x50,
	ENUM_thread_object = 0x51,
	ENUM_local = 0x52,
	ENUM_statement = 0x53,
	ENUM_bad_expression = 0x54,
	ENUM_bad_statement = 0x55,
	ENUM_include = 0x56,
	ENUM_argument = 0x57
};

enum scr_call_type_t
{
	CALL_NONE = 0x0,
	CALL_BUILTIN = 0x1,
	CALL_THREAD = 0x2,
	CALL_FUNCTION = 0x3,
};

enum scr_abort_t
{
	SCR_ABORT_NONE = 0x0,
	SCR_ABORT_CONTINUE = 0x1,
	SCR_ABORT_BREAK = 0x2,
	SCR_ABORT_RETURN = 0x3,
	SCR_ABORT_MAX = 0x3,
};

enum Scr_SourceType_t
{
	SOURCE_TYPE_NONE = 0x0,
	SOURCE_TYPE_BREAKPOINT = 0x1,
	SOURCE_TYPE_CALL = 0x2,
	SOURCE_TYPE_THREAD_START = 0x4,
	SOURCE_TYPE_BUILTIN_CALL = 0x8,
	SOURCE_TYPE_NOTIFY = 0x10,
};

enum
{
	INITIAL_OPCODE_LOOKUP_LEN = 0x10000,
	INITIAL_SOURCEPOS_LOOKUP_LEN = 0x10000,
	INITIAL_SOURCEBUFFER_LOOKUP_LEN = 0x10,
};

enum scr_builtin_type_t
{
	BUILTIN_ANY = 0x0,
	BUILTIN_DEVELOPER_ONLY = 0x1,
};

struct scr_localVar_t
{
	unsigned int name;
	// unsigned int sourcePos;
};

struct Scr_SourcePos_t
{
	unsigned int bufferIndex;
	int lineNum;
	unsigned int sourcePos;
};

#define LOCAL_VAR_STACK_SIZE 64
#define MAX_SWITCH_CASES 1024

struct scr_block_s
{
	int abortLevel;
	int localVarsCreateCount;
	int localVarsPublicCount;
	int localVarsCount;
	byte localVarsInitBits[8];
	scr_localVar_t localVars[LOCAL_VAR_STACK_SIZE];
};

union sval_u
{
	int type;
	unsigned int stringValue;
	unsigned int idValue;
	float floatValue;
	int intValue;
	sval_u *node;
	unsigned int sourcePosValue;
	const char *codePosValue;
	const char *debugString;
	scr_block_s *block;
};

struct stype_t
{
	sval_u val;
	unsigned int pos;
};

struct VariableCompileValue
{
	VariableValue value;
	sval_u sourcePos;
};

struct ThreadDebugInfo
{
	const char *pos[32];
	int posSize;
	float varUsage;
	float endonUsage;
};

struct CaseStatementInfo
{
	unsigned int name;
	const char *codePos;
	unsigned int sourcePos;
	CaseStatementInfo *next;
};

struct BreakStatementInfo
{
	const char *codePos;
	const char *nextCodePos;
	BreakStatementInfo *next;
};

struct ContinueStatementInfo
{
	const char *codePos;
	const char *nextCodePos;
	ContinueStatementInfo *next;
};

struct PrecacheEntry
{
	unsigned short filename;
	bool include;
	unsigned int sourcePos;
	PrecacheEntry *next;
};

#define VALUE_STACK_SIZE 32

typedef struct scrCompileGlob_s
{
	byte *codePos;
	byte *prevOpcodePos;
	unsigned int fileId;
	unsigned int threadId;
	int cumulOffset;
	int maxOffset;
	int maxCallOffset;
	bool bConstRefCount;
	bool in_developer_thread;
	unsigned int developer_thread_sourcePos;
	bool firstThread[2];
	bool bCanIgnoreCase;
	CaseStatementInfo *currentCaseStatement;
	bool bCanBreak;
	bool bCanIgnoreBreak;
	BreakStatementInfo *currentBreakStatement;
	bool bCanContinue;
	bool bCanIgnoreContinue;
	ContinueStatementInfo *currentContinueStatement;
	scr_block_s **breakChildBlocks;
	int *breakChildCount;
	scr_block_s *breakBlock;
	scr_block_s **continueChildBlocks;
	int *continueChildCount;
	bool forceNotCreate;
	PrecacheEntry *precachescriptList;
	PrecacheEntry *precachescriptListHead;
	VariableCompileValue value_start[VALUE_STACK_SIZE];
} scrCompileGlob_t;

struct scr_classStruct_t
{
	uint16_t id;
	uint16_t entArrayId;
	char charId;
	const char *name;
};

inline struct scr_classStruct_t scrClassMap[] =
{
	{ 0, 0, 'e', "entity" },
	{ 0, 0, 'h', "hudelem" },
	{ 0, 0, 'p', "pathnode" },
	{ 0, 0, 'v', "vehiclenode" }
};

#define SL_MAX_STRING_INDEX 0x10000

typedef struct __attribute__((aligned(64))) scrVarGlob_s
{
	VariableValueInternal variableList[SL_MAX_STRING_INDEX];
} scrVarGlob_t;

inline const char *var_typename[] =
{
	"undefined",
	"object",
	"string",
	"localized string",
	"vector",
	"float",
	"int",
	"int64",
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

void Scr_Error(const char *error);
void Scr_ErrorInternal();
void Scr_ObjectError(const char *error);
void Scr_ParamError(unsigned int index, const char *error);
int Scr_GetType(unsigned int param);
unsigned int Scr_GetNumParam();
void Scr_AddUndefined();
void Scr_AddBool(bool value);
void Scr_AddInt(int value);
void Scr_AddInt64(long int value);
void Scr_AddFloat(float value);
void Scr_AddAnim(scr_anim_s value);
void Scr_AddObject(uintptr_t id);
void Scr_AddEntityNum(int entnum, int classnum);
void Scr_AddString(const char *value);
void Scr_AddIString(const char *value);
void Scr_AddConstString(unsigned int value);
void Scr_AddVector(const float *value);
void Scr_MakeArray();
void Scr_AddArray();
void Scr_AddArrayStringIndexed(unsigned int stringValue);
int Scr_GetPointerType(unsigned int index);
scr_entref_t Scr_GetEntityRef( unsigned int index );
unsigned short Scr_ExecEntThreadNum(int entnum, int classnum, int handle, unsigned int paramcount);
void Scr_FreeThread(unsigned short handle);
int Scr_GetInt(unsigned int index);
long int Scr_GetInt64(unsigned int index);
float Scr_GetFloat(unsigned int index);
unsigned int Scr_GetConstString(unsigned int index);
unsigned int Scr_GetConstStringIncludeNull(unsigned int index);
unsigned int Scr_AllocString(const char *string);
const char* Scr_GetString(unsigned int index);
unsigned int Scr_GetConstIString(unsigned int index);
const char* Scr_GetIString(unsigned int index);
VariableValue GetEntityFieldValue(unsigned int classnum, int entnum, int offset);
bool SetEntityFieldValue(unsigned int classnum, int entnum, int offset, VariableValue *value);
void Scr_GetVector(unsigned int index, float *vector);
void VM_CancelNotify(unsigned int notifyListOwnerId, unsigned int startLocalId);
void VM_Notify(unsigned int notifyListOwnerId, unsigned int stringValue, VariableValue *top);
void Scr_NotifyNum(int entnum, int classnum, unsigned int stringValue, unsigned int paramcount);
scr_anim_s Scr_GetAnim(unsigned int index, struct XAnimTree_s *tree);
const char* Scr_GetTypeName(unsigned int index);
unsigned int Scr_GetConstLowercaseString(unsigned int index);
unsigned int Scr_GetObject(unsigned int paramnum);
void Scr_SetStructField(unsigned int structId, unsigned int index);

unsigned int VM_Execute(unsigned int localId, const char *pos, unsigned int paramcount);

void Scr_IncTime();
void Scr_DecTime();

void VM_TerminateTime(unsigned int timeId);
void Scr_TerminateThread(unsigned int localId);
void RuntimeErrorInternal(conChannel_t channel, const char *codePos, unsigned int index, const char *errorMessage);
void RuntimeError(const char *codePos, unsigned int index, const char *errorMsg, const char *format);
void Scr_ResetTimeout();
void Scr_TerminateRunningThread(unsigned int localId);
void Scr_SetLoading(int bLoading);
VariableStackBuffer* VM_ArchiveStack(int size, const char *codePos, VariableValue *top, unsigned int localVarCount, unsigned int *localId);
void VM_TrimStack(unsigned int startLocalId, VariableStackBuffer *stackValue, bool fromEndon);
void Scr_CancelNotifyList(unsigned int notifyListOwnerId);
unsigned short Scr_ExecThread(int callbackHook, unsigned int numArgs);
void Scr_AddExecThread(int handle, unsigned int paramcount);
void Scr_Settings(int developer, int developer_script, int abort_on_error);
void Scr_TerminalError(const char *error);
void Scr_ClearErrorMessage();
void Scr_Abort();
void Scr_ShutdownSystem(qboolean bComplete);
void Scr_Shutdown();
void Scr_InitSystem();
void Scr_Init();

void MT_Init();
byte *MT_InitForceAlloc();
void MT_ForceAllocIndex(unsigned char *allocBits, unsigned int nodeNum, int numBytes);
void MT_FinishForceAlloc(unsigned char *allocBits);
unsigned short MT_AllocIndex(int numBytes);
void MT_FreeIndex(unsigned int nodeNum, int numBytes);
void* MT_Alloc(int numBytes);
void MT_Free(void* p, int numBytes);
int MT_Realloc(int oldNumBytes, int newNumbytes);
byte* MT_GetRefByIndex(int index);
int MT_GetIndexByRef(byte* p);
void MT_DumpTree();

struct __attribute__((aligned(4))) RefString
{
	union
	{
		struct
		{
			unsigned int byteLen : 8;
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
			unsigned int byteLen : 8;
		};
		volatile int head;
	};
	vec3_t vec;
};

#define HASH_TABLE_SIZE 16384

struct HashEntry
{
	uint16_t status_next;
	union
	{
		uint16_t prev;
		uint16_t str;
	};
};

typedef struct __attribute__((aligned(128))) scrStringGlob_s
{
	HashEntry hashTable[HASH_TABLE_SIZE];
	bool inited;
	HashEntry *nextFreeEntry;
} scrStringGlob_t;

const char* SL_ConvertToString(unsigned int index);
unsigned int SL_ConvertToLowercase(unsigned int stringValue, unsigned int user);
unsigned int SL_ConvertFromString(const char *str);
unsigned int SL_GetLowercaseStringOfLen(const char *upperstring, unsigned int user, unsigned int len);
unsigned int SL_GetLowercaseString_(const char *str, unsigned int user);
unsigned int GetHashCode(const char *str, unsigned int len);
void SL_FreeString(unsigned int stringValue, RefString *refStr, unsigned int len);
void SL_AddUserInternal(RefString *refStr, unsigned int user);
unsigned int SL_GetStringOfLen(const char *str, unsigned int user, unsigned int len);
unsigned int SL_FindStringOfLen(const char *str, unsigned int len);
unsigned int SL_FindLowercaseString(const char *upperstring);
unsigned int SL_FindString(const char *string);
unsigned int SL_GetString_(const char *str, unsigned int user);
unsigned int SL_GetString(const char *str, unsigned int user);
unsigned int SL_GetStringForInt(int i);
unsigned int SL_GetStringForFloat(float f);
unsigned int SL_GetStringForVector(const float *v);
int SL_GetStringLen(unsigned int stringValue);
void Scr_SetString(uint16_t *to, unsigned int from);
void SL_RemoveRefToStringOfLen(unsigned int stringValue, unsigned int len);
void SL_RemoveRefToString(unsigned int stringValue);
void SL_TransferRefToUser(unsigned int stringIndex, unsigned int user);
void SL_AddRefToString(unsigned int stringValue);
void SL_ShutdownSystem(unsigned int user);
void SL_Shutdown();
void SL_Clear();
void SL_Init();
void SL_Restart();

unsigned int Scr_CreateCanonicalFilename(const char *name);
void Scr_CopyEntityNum( int fromEntnum, int toEntnum, int classnum );
void CopyEntity(unsigned int parentId, unsigned int newParentId);

unsigned int FindNextSibling(unsigned int id);
unsigned int FindPrevSibling(unsigned int id);
unsigned int FindVariable(unsigned int parentId, unsigned int name);
unsigned int FindVariableIndexInternal(unsigned int parentId, unsigned int name);
unsigned int FindObjectVariable(unsigned int parentId, unsigned int id);
unsigned int FindObject(unsigned int id);
void AddRefToVector(const float *vectorValue);
void RemoveRefToVector(const float *vectorValue);
void MakeVariableExternal(VariableValueInternal *value, VariableValueInternal *parentValue);
unsigned int GetNewObjectVariableReverse(unsigned int parentId, unsigned int id);
unsigned int GetNewObjectVariable(unsigned int parentId, unsigned int id);
unsigned int GetObjectVariable(unsigned int parentId, unsigned int id);
unsigned int GetVariable(unsigned int parentId, unsigned int name);
VariableValue Scr_EvalVariable( unsigned int id );
unsigned int GetNewArrayVariableIndex(unsigned int parentId, unsigned int index);
unsigned int GetNewArrayVariable(unsigned int parentId, unsigned int index);
void FreeVariable(unsigned int id);
unsigned int GetVariableName(unsigned int id);
unsigned int GetArrayVariable(unsigned int parentId, unsigned int index);
void SetNewVariableValue(unsigned int id, VariableValue *value);
unsigned int GetNewVariable(unsigned int parentId, unsigned int name);
void RemoveRefToEmptyObject(unsigned int id);
void AddRefToObject(unsigned int id);
void AddRefToValue(VariableValue *val);
void AddRefToValue(int type, VariableUnion u);
void FreeValue(unsigned int id);
void ClearObject(unsigned int parentId);
void RemoveVariable(unsigned int parentId, unsigned int index);
void RemoveNextVariable(unsigned int index);
void RemoveArrayVariable(unsigned int parentId, unsigned int unsignedValue);
void RemoveObjectVariable(unsigned int parentId, unsigned int id);
void RemoveRefToObject(unsigned int id);
void RemoveRefToValue(int type, VariableUnion u);
void RemoveRefToValue(VariableValue *value);
void RemoveRefToValue(int type, VariableUnion u);
unsigned int FindArrayVariable(unsigned int parentId, unsigned int index);
unsigned int GetParentLocalId(unsigned int threadId);
unsigned int GetSafeParentLocalId(unsigned int threadId);
unsigned int GetStartLocalId(unsigned int threadId);
unsigned int GetObjectType(unsigned int varIndex);
unsigned short AllocVariable();
unsigned int AllocValue();
unsigned int AllocEntity(int classnum, unsigned short entnum);
float* Scr_AllocVector(const float* vec);
unsigned int Scr_GetEntityId(int entnum, int classnum);
unsigned short Scr_GetThreadNotifyName(unsigned int startLocalId);
void Scr_RemoveThreadNotifyName(unsigned int startLocalId);
void Scr_KillThread(unsigned int parentId);
unsigned int GetArraySize(unsigned int id);
unsigned int Scr_GetSelf(unsigned int id);
scr_entref_t Scr_GetEntityIdRef( unsigned int entId );
void Scr_FreeEntityNum(int entnum, int classnum);
unsigned int GetVariableKeyObject(unsigned int id);
unsigned int Scr_EvalFieldObject(unsigned int tempVariable, VariableValue *value);
unsigned int Scr_EvalVariableObject(unsigned int id);
union VariableValueInternal_u* GetVariableValueAddress_Bad(unsigned int id);
union VariableUnion* GetVariableValueAddress(unsigned int id);
unsigned int Scr_GetThreadWaitTime(unsigned int startLocalId);
void Scr_KillEndonThread(unsigned int threadId);
void Scr_ClearWaitTime(unsigned int startLocalId);
void Scr_CastBool(VariableValue *value);
void Scr_EvalBoolNot(VariableValue *value);
void Scr_EvalBoolComplement(VariableValue *value);
void Scr_EvalOr(VariableValue *value1, VariableValue *value2);
void Scr_EvalExOr(VariableValue *value1, VariableValue *value2);
void Scr_EvalAnd(VariableValue *value1, VariableValue *value2);
void Scr_EvalEquality(VariableValue *value1, VariableValue *value2);
void Scr_EvalInequality(VariableValue *value1, VariableValue *value2);
void Scr_EvalLess(VariableValue *value1, VariableValue *value2);
void Scr_EvalGreater(VariableValue *value1, VariableValue *value2);
void Scr_EvalLessEqual(VariableValue *value1, VariableValue *value2);
void Scr_EvalGreaterEqual(VariableValue *value1, VariableValue *value2);
void Scr_EvalShiftLeft(VariableValue *value1, VariableValue *value2);
void Scr_EvalShiftRight(VariableValue *value1, VariableValue *value2);
void Scr_EvalPlus(VariableValue *value1, VariableValue *value2);
void Scr_EvalMinus(VariableValue *value1, VariableValue *value2);
void Scr_EvalMultiply(VariableValue *value1, VariableValue *value2);
void Scr_EvalDivide(VariableValue *value1, VariableValue *value2);
void Scr_EvalMod(VariableValue *value1, VariableValue *value2);
void Scr_EvalSizeValue(VariableValue *value);
void Scr_EvalBinaryOperator(int op, VariableValue *value1, VariableValue *value2);
void Scr_CastVector(VariableValue *value);
unsigned int GetObjectA(unsigned int id);
unsigned int GetDummyObject();
unsigned int GetDummyFieldValue();
unsigned int FindEntityId(int entnum, int classnum);
unsigned int AllocObject();
unsigned int AllocThread(unsigned int self);
unsigned int AllocChildThread(unsigned int self, unsigned int parentLocalId);
unsigned int Scr_AllocArray();
unsigned int Scr_EvalArrayRef(unsigned int parentId);
void Scr_RemoveClassMap(int classnum);
void Scr_StopThread(unsigned int threadId);
void Scr_EvalArray(VariableValue *value, VariableValue *index);
VariableValue Scr_FindVariableField(unsigned int parentId, unsigned int name);
VariableValue Scr_EvalVariableField(unsigned int id);
void SafeRemoveVariable(unsigned int parentId, unsigned int name);
unsigned int Scr_EvalArrayIndex(unsigned int parentId, VariableValue *var);
unsigned int Scr_GetVariableField(unsigned int fieldId, unsigned int index);
void ClearVariableField(unsigned int parentId, unsigned int name, VariableValue *value);
float Scr_GetEntryUsage(unsigned int type, VariableUnion u);
float Scr_GetObjectUsage(unsigned int parentId);
void SetVariableValue(unsigned int id, VariableValue *value);
void ClearVariableValue(unsigned int id);
bool IsFieldObject(unsigned int id);
bool IsValidArrayIndex(unsigned int index);
bool IsObjectFree(unsigned int id);
int Scr_GetClassnumForCharId(char charId);
void Scr_FreeValue(unsigned int id);
void Scr_FreeEntityList();
void Scr_FreeGameVariable(int bComplete);
void Scr_FreeObjects();
unsigned int GetInternalVariableIndex(unsigned int index);
int Scr_GetOffset(unsigned int classnum, const char *name);
void CopyArray(unsigned int parentId, unsigned int newParentId);
void ClearArray(unsigned int parentId, VariableValue *value);
void SetVariableFieldValue(unsigned int id, VariableValue *value);
unsigned int GetArray(unsigned int id);
void Scr_SetDynamicEntityField(int entnum, int classnum, unsigned int index);
void Scr_SetThreadWaitTime(unsigned int startLocalId, unsigned int waitTime);
void Scr_SetThreadNotifyName(unsigned int startLocalId, unsigned int stringValue);
unsigned int Scr_FindField(const char *name, int *type);
void Scr_AddFields(const char *path, const char *extension);
void Scr_AddClassField( int classnum, const char *name, unsigned int offset );
bool Scr_CastString(VariableValue *value);
void Scr_SetClassMap(int classnum);
void Scr_AllocGameVariable();
void Scr_DumpScriptThreads();
void Scr_DumpScriptVariablesDefault();
void VM_Shutdown();
void Scr_ShutdownVariables();
void Var_Init();

void SetAnimCheck(int bAnimCheck);
void Scr_UsingTree(const char *filename, unsigned int sourcePos);
void Scr_EmitAnimation(char *pos, unsigned int animName, unsigned int sourcePos);
void Scr_FindAnim(const char *filename, const char *animName, scr_anim_s *anim, int user);
void Scr_LoadAnimTreeAtIndex(int index, void *(*Alloc)(int), int user);
void Scr_EndLoadAnimTrees();
void Scr_PrecacheAnimTrees(void *(*Alloc)(int), int user);
scr_animtree_t Scr_FindAnimTree( const char *filename );

qboolean Scr_IsInScriptMemory(const char *pos);
void Scr_GetGenericField( byte *b, int type, ptrdiff_t ofs );
void Scr_SetGenericField(byte *data, int fieldtype, ptrdiff_t offset);
unsigned int SL_TransferToCanonicalString(unsigned int index);
unsigned int SL_GetCanonicalString(const char *str);

void Scr_EndLoadEvaluate();
void Scr_BeginLoadAnimTrees(int num);
void Scr_InitEvaluate();

bool Scr_IsIdentifier(const char *token);
unsigned int Scr_GetSourceBuffer(const char *codePos);
char* Scr_AddSourceBuffer(const char *filename, const char *extFilename, const char *codePos, bool archive);
void Scr_ShutdownOpcodeLookup();

qboolean Scr_IsSystemActive();
int GScr_LoadScriptAndLabel(const char *filename, const char *name, qboolean errorIfMissing);
unsigned int Scr_LoadScript(const char *filename);
void Scr_BeginLoadScripts();
void Scr_PostCompileScripts();
void Scr_EndLoadScripts();
void Scr_FreeScripts();

void Scr_RunCurrentThreads();
void Scr_ClearOutParams();

const char* Scr_GetDebugString(unsigned int index);
void Scr_CastDebugString(VariableValue *value);

void Scr_PrintPrevCodePos(conChannel_t channel, const char *codePos, unsigned int index);
void CompileError(unsigned int sourcePos, const char *format, ...);
void CompileError2(const char *codePos, const char *format, ...);
int Scr_ScanFile(char *buf, int max_size);
void AddOpcodePos(unsigned int sourcePos, int type);
void RemoveOpcodePos();
void AddThreadStartOpcodePos(unsigned int sourcePos);
void Scr_InitOpcodeLookup();
void Scr_CompileShutdown();

int yyparse();
void ScriptParse(sval_u *parseData, unsigned char user);

void EmitOpcode(unsigned int op, int offset, int callType);
void Scr_CalcLocalVarsArrayPrimitiveExpressionRef(sval_u expr, scr_block_s *block);
bool EvalPrimitiveExpressionList(sval_u exprlist, sval_u sourcePos, VariableCompileValue *constValue);
bool EvalExpression(sval_u expr, VariableCompileValue *constValue);
void Scr_CalcLocalVarsStatement(sval_u val, scr_block_s *block);
void Scr_CalcLocalVarsThread(sval_u exprlist, sval_u stmtlist, sval_u *stmttblock);
int Scr_FindLocalVarIndex(unsigned int name, sval_u sourcePos, bool create, scr_block_s *block);

void EmitArrayPrimitiveExpressionRef(sval_u expr, sval_u sourcePos, scr_block_s *block);
void EmitStatement(sval_u val, bool lastStatement, unsigned int endSourcePos, scr_block_s *block);
void EmitStatementList(sval_u val, bool lastStatement, unsigned int endSourcePos, scr_block_s *block);

void EmitPrimitiveExpressionFieldObject(sval_u expr, sval_u sourcePos, scr_block_s *block);
void EmitExpression(sval_u expr, scr_block_s *block);
void EmitPrimitiveExpression(sval_u expr, scr_block_s *block);
bool EmitOrEvalExpression(sval_u expr, VariableCompileValue *constValue, scr_block_s *block);
void EmitVariableExpression(sval_u expr, scr_block_s *block);
void EmitCall(sval_u func_name, sval_u params, bool bStatement, scr_block_s *block);
void EmitFunction(sval_u func, sval_u sourcePos);
void EmitMethod(sval_u expr, sval_u func_name, sval_u params, sval_u methodSourcePos, bool bStatement, scr_block_s *block);
void EmitThread(sval_u val);

void ScriptCompile(sval_u val, unsigned int filePosId, unsigned int scriptId);

void Scr_AddExecEntThreadNum(int entnum, int classnum, int handle, unsigned int paramcount);

unsigned int Scr_UsingTreeInternal(const char *filename, unsigned int *index, int user);
void Scr_EmitAnimationInternal(char *pos, unsigned int animName, unsigned int names);
bool Scr_LoadAnimTreeInternal(const char *filename, unsigned int parentNode, unsigned int names);
int Scr_GetAnimTreeSize(unsigned int parentNode);
void ConnectScriptToAnim( unsigned int names, int index, unsigned int filename, unsigned int name, int treeIndex );
void Scr_PrecacheAnimationTree(unsigned int parentNode);
void Scr_CheckAnimsDefined(unsigned int names, unsigned int filename);

void EmitIncludeList(sval_u val);
void EmitThreadList(sval_u val);
void LinkThread(unsigned int threadCountId, VariableValue *pos, bool allowFarCall);
unsigned int SpecifyThreadPosition(unsigned int posId, unsigned int name, unsigned int sourcePos, int type);
void LinkFile(unsigned int filePosId);
void Scr_CalcLocalVarsVariableExpressionRef(sval_u expr, scr_block_s *block);
void Scr_CalcLocalVarsStatementList(sval_u val, scr_block_s *block);
bool EmitOrEvalPrimitiveExpression(sval_u expr, VariableCompileValue *constValue, scr_block_s *block);
void EmitVariableExpressionRef(sval_u expr, scr_block_s *block);
void EmitBreakOn(sval_u expr, sval_u param, sval_u sourcePos);

int MT_GetSubTreeSize(int nodeNum);
void MT_InitBits();
void MT_AddMemoryNode(int newNode, int size);
void MT_SafeFreeIndex( unsigned int nodeNum );
int MT_GetSize(int numBytes);
bool MT_RemoveMemoryNode(int oldNode, int size);
void MT_Error(const char *funcName, int numBytes);
void MT_RemoveHeadMemoryNode(int size);

unsigned int Scr_GetLineNumInternal(const char *buf, unsigned int sourcePos, const char **startLine, int *col);
OpcodeLookup* Scr_GetPrevSourcePosOpcodeLookup(const char *codePos);
unsigned int Scr_GetLineInfo(const char *buf, unsigned int sourcePos, int *col, char *line);
OpcodeLookup* Scr_GetSourcePosOpcodeLookup( const char *codePos );
void Scr_PrintSourcePos(conChannel_t channel, const char *filename, const char *buf, unsigned int sourcePos);
char* Scr_ReadFile(const char *filename, const char *extFilename, const char *codePos, bool archive);
void Scr_AddSourceBufferInternal(const char *extFilename, const char *codePos, char *sourceBuf, int len, bool doEolFixup, bool archive);

RefString *GetRefString(unsigned int stringValue);
RefString *GetRefString(const char* string);
int SL_ConvertFromRefString(RefString *refString);
int SL_GetRefStringLen(RefString *refString);
void CreateCanonicalFilename(char *newFilename, const char *filename, int count);
void SL_RemoveAllRefToString(unsigned int stringValue);

void InitVariables();
void Scr_InitClassMap();

unsigned int FindArrayVariableIndex(unsigned int parentId, unsigned int index);
float Scr_GetThreadUsage(VariableStackBuffer *stackBuf, float *endonUsage);
int ThreadInfoCompare(const void *ainfo1, const void *ainfo2);
unsigned int GetNewVariableIndexReverseInternal(unsigned int parentId, unsigned int name);
unsigned int GetNewVariableIndexInternal(unsigned int parentId, unsigned int name);
unsigned int GetVariableIndexInternal(unsigned int parentId, unsigned int name);
unsigned int GetArrayVariableIndex(unsigned int parentId, unsigned int index);
float* Scr_AllocVectorInternal();
void ClearObjectInternal(unsigned int parentId);
int Scr_MakeValuePrimitive(unsigned int parentId);
unsigned int Scr_FindArrayIndex(unsigned int parentId, VariableValue *index);
void Scr_CastWeakerPair(VariableValue *value1, VariableValue *value2);
void Scr_CastWeakerStringPair(VariableValue *value1, VariableValue *value2);
void Scr_ClearThread(unsigned int parentId);
void SafeRemoveArrayVariable(unsigned int parentId, unsigned int name);
VariableValue Scr_EvalVariableEntityField(unsigned int entId, unsigned int name);
void Scr_AddFieldsForFile(const char *filename);

int Scr_FindAllThreadsInternal(unsigned int selfId,unsigned int threadId,int count, bool isLocalId,unsigned int *threads);
unsigned int Scr_GetWaittillThreadStackId(unsigned int localId, unsigned int startLocalId);
void VM_CancelNotifyInternal(unsigned int notifyListOwnerId, unsigned int startLocalId, unsigned int notifyListId, unsigned int notifyNameListId, unsigned int stringValue);
void Scr_CancelWaittill(unsigned int startLocalId);
void IncInParam();
void Scr_VM_Init();
void VM_SetTime();
void VM_Resume(unsigned int timeId);
unsigned int VM_Execute(const char *pos, unsigned int localId, unsigned int localVarCount, VariableValue *top, VariableValue *startTop);
