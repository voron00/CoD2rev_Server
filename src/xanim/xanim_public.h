#pragma once

#define XMODEL_VERSION 20
#define XANIM_VERSION 14

enum XAssetType
{
	ASSET_TYPE_XMODELPIECES = 0x0,
	ASSET_TYPE_XMODEL = 0x1,
	ASSET_TYPE_XANIMPARTS = 0x2,
	ASSET_TYPE_MATERIAL = 0x3,
	ASSET_TYPE_TECHNIQUE_SET = 0x4,
	ASSET_TYPE_IMAGE = 0x5,
	ASSET_TYPE_SOUND = 0x6,
	ASSET_TYPE_SOUND_CURVE = 0x7,
	ASSET_TYPE_LOADED_SOUND = 0x8,
	ASSET_TYPE_CLIPMAP = 0x9,
	ASSET_TYPE_CLIPMAP_PVS = 0xA,
	ASSET_TYPE_COMWORLD = 0xB,
	ASSET_TYPE_GAMEWORLD_SP = 0xC,
	ASSET_TYPE_GAMEWORLD_MP = 0xD,
	ASSET_TYPE_MAP_ENTS = 0xE,
	ASSET_TYPE_GFXWORLD = 0xF,
	ASSET_TYPE_LIGHT_DEF = 0x10,
	ASSET_TYPE_UI_MAP = 0x11,
	ASSET_TYPE_FONT = 0x12,
	ASSET_TYPE_MENULIST = 0x13,
	ASSET_TYPE_MENU = 0x14,
	ASSET_TYPE_LOCALIZE_ENTRY = 0x15,
	ASSET_TYPE_WEAPON = 0x16,
	ASSET_TYPE_SNDDRIVER_GLOBALS = 0x17,
	ASSET_TYPE_FX = 0x18,
	ASSET_TYPE_IMPACT_FX = 0x19,
	ASSET_TYPE_AITYPE = 0x1A,
	ASSET_TYPE_MPTYPE = 0x1B,
	ASSET_TYPE_CHARACTER = 0x1C,
	ASSET_TYPE_XMODELALIAS = 0x1D,
	ASSET_TYPE_RAWFILE = 0x1E,
	ASSET_TYPE_STRINGTABLE = 0x1F,
	ASSET_TYPE_COUNT = 0x20,
};

#define DOBJ_MAX_SUBMODELS 8

enum
{
	DOBJ_MAX_PARTS = 0x80,
	DOBJ_MAX_PART_BITS = 0x4,
	MAX_LODS = 0x4,
};

#define LOW_BIT 1
#define HIGH_BIT 1 << 31

union XAnimDynamicIndices
{
	byte _1[1];
	uint16_t _2[1];
};

struct XAnimPartTransFrames
{
	float (*frames)[3];
	XAnimDynamicIndices indices;
};

union XAnimPartTransData
{
	XAnimPartTransFrames frames;
	float frame0[3];
};

struct XAnimPartTrans
{
	uint16_t size;
	XAnimPartTransData u;
};

union XAnimPartQuatFrames
{
	int16_t (*frames)[4];
	int16_t (*frames2)[2];
};

struct XAnimPartQuatDataFrames
{
	XAnimPartQuatFrames u;
	XAnimDynamicIndices indices;
};

union XAnimPartQuatData
{
	XAnimPartQuatDataFrames frames;
	int16_t frame0[4];
	int16_t frame02[2];
};

struct XAnimPartQuat
{
	uint16_t size;
	XAnimPartQuatData u;
};

struct XAnimPart
{
	XAnimPartTrans *trans;
	XAnimPartQuat *quat;
};

struct XAnimDeltaPartTransFrames
{
	float (*frames)[3];
	XAnimDynamicIndices indices;
};

union XAnimDeltaPartTransData
{
	XAnimDeltaPartTransFrames frames;
	float frame0[3];
};

struct XAnimDeltaPartTrans
{
	uint16_t size;
	XAnimDeltaPartTransData u;
};

struct XAnimDeltaPartQuatDataFrames
{
	int16_t (*frames)[2];
	XAnimDynamicIndices indices;
};

union XAnimDeltaPartQuatData
{
	XAnimDeltaPartQuatDataFrames frames;
	int16_t frame0[2];
};

struct XAnimDeltaPartQuat
{
	uint16_t size;
	XAnimDeltaPartQuatData u;
};

struct XAnimDeltaPart
{
	XAnimDeltaPartTrans *trans;
	XAnimDeltaPartQuat *quat;
};

struct XAnimNotifyInfo
{
	unsigned short name;
	float time;
};

struct XAnimTime
{
	float time;
	int frameCount;
	float frameFrac;
	int frameIndex;
};

typedef struct XAnimParts_s
{
	unsigned short numframes;
	byte bLoop;
	byte bDelta;
	float framerate;
	float frequency;
	byte notifyCount;
	char assetType;
	short boneCount;
	uint16_t *names;
	char* simpleQuatBits;
	XAnimPart *parts;
	XAnimNotifyInfo *notify;
	XAnimDeltaPart *deltaPart;
	const char *name;
	bool isDefault;
} XAnimParts;

struct DObjAnimMat
{
	vec4_t quat;
	vec3_t trans;
	float transWeight;
};

struct XAnimParent
{
	uint16_t flags;
	uint16_t children;
};

union XAnimEntryUnion
{
	XAnimParts *parts;
	XAnimParent animParent;
};

struct XAnimEntry
{
	uint16_t numAnims;
	uint16_t parent;
	XAnimEntryUnion u;
};

typedef struct XAnim_s
{
	const char *debugName;
	unsigned int size;
	const char **debugAnimNames;
	XAnimEntry entries[1];
} XAnim;

#pragma pack(push, 1)
typedef struct XAnimTree_s
{
	XAnim_s *anims;
	uint16_t entnum;
	bool bAbs;
	bool bUseGoalWeight;
	uint16_t infoArray[1];
} XAnimTree;
#pragma pack(pop)

struct XBoneInfo
{
	vec3_t bounds[2];
	vec3_t offset;
	float radiusSquared;
};

struct XAnimToXModel
{
	int partBits[DOBJ_MAX_PART_BITS];
	byte boneIndex[DOBJ_MAX_PARTS];
};

struct XModelCollTri_s
{
	vec4_t plane;
	vec4_t svec;
	vec4_t tvec;
};

struct DSkelPartBits_s
{
	int anim[DOBJ_MAX_PART_BITS];
	int control[DOBJ_MAX_PART_BITS];
	int skel[DOBJ_MAX_PART_BITS];
};

struct DSkel_t
{
	DSkelPartBits_s partBits;
	DObjAnimMat Mat;
};

struct XModelConfigEntry
{
	char filename[1024];
	float dist;
};

struct XModelConfig
{
	XModelConfigEntry entries[4];
	float mins[3];
	float maxs[3];
	int collLod;
	byte flags;
};

struct XBoneHierarchy
{
	unsigned short *names;
	byte parentList[1];
};

typedef struct XModelParts_s
{
	short numBones;
	short numRootBones;
	XBoneHierarchy *hierarchy;
	short *quats;
	float *trans;
	byte *partClassification;
	DSkel_t skel;
} XModelParts;

struct XBlendLoadInfo
{
	unsigned short boneOffset;
	unsigned short boneWeight;
};

struct XVertexInfo_s
{
	float normal[3];
	unsigned char color[4];
	float binormal[3];
	float texCoordX;
	float tangent[3];
	float texCoordY;
	float offset[3];
	unsigned char numWeights;
	unsigned char pad;
	short boneOffset;
};

struct XVertexBuffer
{
	XVertexInfo_s v;
	XBlendLoadInfo w[1];
};

struct XRigidSurface
{
	XVertexBuffer *vb;
};

struct XSurface_s
{
	unsigned char tileMode;
	unsigned char vertListCount;
	unsigned short vertCount;
	unsigned short triCount;
	unsigned short boneOffset;
	unsigned short *triIndices;
	struct GfxPackedVertex *verts;
	XRigidSurface surfRigid;
	struct IDirect3DIndexBuffer9 *indexBuffer;
};

typedef struct XModelSurfs_s
{
	XSurface_s *surfs;
	int partBits[DOBJ_MAX_PART_BITS];
} XModelSurfs;

typedef struct XModelLodInfo_s
{
	float dist;
	const char *filename;
	short numsurfs;
	unsigned short *surfNames;
	XModelSurfs_s *surfs;
} XModelLodInfo;

typedef struct XModelCollSurf_s
{
	XModelCollTri_s *collTris;
	int numCollTris;
	vec3_t mins;
	vec3_t maxs;
	int boneIdx;
	int contents;
	int surfFlags;
} XModelCollSurf;

struct trXSkin_t
{
	struct Material **materialHandles;
};

typedef struct
{
	XModelParts_s *parts;
	XModelLodInfo_s lodInfo[MAX_LODS];
	XModelCollSurf_s *collSurfs;
	int numCollSurfs;
	int contents;
	XBoneInfo *boneInfo;
	vec3_t mins;
	vec3_t maxs;
	short numLods;
	short collLod;
	trXSkin_t xskins; // !!! Not loaded in server binary
	int memUsage;
	const char *name;
	char flags;
	bool bad;
} XModel;

struct DObjModel_s
{
	XModel *model;
	const char *boneName;
	qboolean ignoreCollision;
};

enum XAnimFlags
{
	XANIM_LOOP_SYNC_TIME = 0x1,
	XANIM_NONLOOP_SYNC_TIME = 0x2,
	XANIM_SYNC_ROOT = 0x4,
	XANIM_COMPLETE = 0x8,
	XANIM_ADDITIVE = 0x10,
	XANIM_CLIENT = 0x20,
	XANIM_SEPARATE = 0x40,
	XANIM_FORCELOAD = 0x80,
};

#define WEIGHT_EPSILON 0.001f

#define NO_BONEINDEX 0xFF
#define UNDEFINED_BONEINDEX DOBJ_MAX_PARTS - 1

#define DOBJ_PARTBITS_STRING_LEN DOBJ_MAX_PART_BITS * sizeof( int32_t )

#define testQuatBit(array, bitNum) ((((array)[(bitNum) >> 3]) >> ((bitNum) & 7)) & 1)
#define WEIGHTSCALE_EPSILON ( 1.0f / ( 1ULL << 30 ) )

struct DSurface_s
{
	unsigned short modelIndex;
	unsigned short subMatIndex;
};

typedef struct DObj_s
{
	XAnimTree_s *tree;
	DSkel_t *skel;
	int timeStamp;
	unsigned short *animToModel;
	unsigned short duplicateParts;
	int locked;
	byte numModels;
	byte numBones;
	byte ignoreCollision;
	XModel *models[DOBJ_MAX_SUBMODELS];
	byte modelParents[DOBJ_MAX_SUBMODELS];
	byte matOffset[DOBJ_MAX_SUBMODELS];
	vec3_t mins;
	vec3_t maxs;
} DObj;

struct XAnimState
{
	float time;
	float oldTime;
	int16_t cycleCount;
	int16_t oldCycleCount;
	float goalTime;
	float goalWeight;
	float weight;
	float rate;
};

union XAnimInfoUnion
{
	XAnimParts *parts;
	XAnimParent animParent;
};

typedef struct
{
	uint16_t notifyChild;
	int16_t notifyIndex;
	uint16_t notifyName;
	uint16_t notifyType;
	uint16_t prev;
	uint16_t next;
	XAnimState state;
} XAnimInfo;

extern XAnimInfo g_xAnimInfo[];

struct XAnimNotify_s
{
	const char *name;
	unsigned int type;
	float timeFrac;
};

struct XAnimSimpleRotPos
{
	float rot[2];
	float posWeight;
	float pos[3];
};

#define MAX_CALC_ANIM_BUFFER 512

struct XAnimCalcAnimInfo
{
	DObjAnimMat rotTransArray[MAX_CALC_ANIM_BUFFER];
	int animPartBits[DOBJ_MAX_PART_BITS];
	int ignorePartBits[DOBJ_MAX_PART_BITS];
};

#pragma pack(push, 1)
struct XAnimTimeInfo
{
	bool childHasTimeForParent;
	bool parentHasWeightForChild;
	XAnimEntry *entry;
};
#pragma pack(pop)

struct DObjTrace_s
{
	float fraction;
	int surfaceflags;
	float normal[3];
	unsigned short partName;
	unsigned short partGroup;
};

struct TestLod
{
	bool enabled;
	float dist;
};

extern TestLod g_testLods[];

void DObjCreate(DObjModel_s *dobjModels, unsigned int numModels, XAnimTree_s *tree, DObj *obj, unsigned int entnum);
void DObjGetBounds(const DObj_s *obj, float *mins, float *maxs);
void DObjInit();
void DObjShutdown();
void DObjFree(DObj_s *obj);
void DObjAbort();

void DObjSetControlTagAngles( const DObj_s *obj, int *partBits, unsigned int tagName, const vec3_t angles );
void DObjSetLocalTag(const DObj_s *obj, int *partBits, unsigned int boneIndex, const float *trans, const float *angles);
void ConvertQuatToMat(const DObjAnimMat *mat, float axis[3][3]);
void MatrixTransformVectorQuatTrans(const float *in, const DObjAnimMat *mat, float *out);
void QuatMultiply(const float *in1, const float *in2, float *out);

int DObjSkelExists(DObj_s *obj, int timeStamp);
int DObjSkelIsBoneUpToDate(DObj_s *obj, int boneIndex);
int DObjGetAllocSkelSize(const DObj_s *obj);
void DObjCreateSkel(DObj_s *obj, DSkel_t *skel, int time);
int DObjSkelAreBonesUpToDate(const DObj_s *obj, int *partBits);
XAnimTree_s* DObjGetTree(const DObj_s *obj);

int DObjSetControlRotTransIndex(const DObj_s *obj, const int *partBits, int boneIndex);
int DObjSetRotTransIndex(const DObj_s *obj, const int *partBits, int boneIndex);
int DObjNumBones(const DObj *obj);

void XAnimFree(XAnimParts *parts);
void XAnimFreeList(XAnim *anims);

int DObjGetBoneIndex(const DObj_s *obj, unsigned int name);
void DObjSetTree(DObj_s *obj, XAnimTree_s *tree);
void DObjGetHierarchyBits(DObj_s *obj, int boneIndex, int *partBits);

void XModelPartsFree(XModelParts *modelParts);
void XModelFree(XModel *model);
XModel* XModelLoadFile(const char *name, void *(*Alloc)(int), void *(*AllocColl)(int));
XModelParts* XModelPartsPrecache(XModel *model, const char *name, void *(*Alloc)(int));
XModelParts* XModelPartsLoadFile(XModel *model, const char *name, void *(*Alloc)(int));
XAnimParts* XAnimLoadFile(const char *name, void *(*Alloc)(int));
void XModelGetBounds(const XModel *model, float *mins, float *maxs);
int XModelGetBoneIndex(const XModel *model, unsigned int name);
int XModelNumBones(const XModel *model);
int XModelTraceLine(const XModel *model, trace_t *results, DObjAnimMat *pose, const float *localStart, const float *localEnd, int contentmask);

void XAnimUpdateInfoInternal(XAnimTree_s *tree, unsigned int infoIndex, float dtime, bool update);
float XAnimFindServerNoteTrack(const XAnimTree_s *anim, unsigned int infoIndex, float dtime);

void DObjCalcAnim(const DObj_s *obj, int *partBits);

DObjAnimMat* DObjGetRotTransArray(const DObj_s *obj);
void DObjCreateDuplicateParts(DObj_s *obj);
void DObjCalcSkel(DObj_s *obj, int *partBits);
void DObjTraceline( DObj *obj, const vec3_t start, const vec3_t end, unsigned char *priorityMap, DObjTrace_s *trace );

void XAnimGetRelDelta(const XAnim_s *anim, unsigned int animIndex, float *rot, float *trans, float startTime, float endTime);

void XAnimCalc(const DObj_s *obj, unsigned int entry, float weightScale, DObjAnimMat *rotTransArray, bool bClear, bool bNormQuat, XAnimCalcAnimInfo *animInfo, int rotTransArrayIndex);
void XAnim_CalcDeltaForTime(const XAnimParts_s *anim, const float time, float *rotDelta, float *posDelta);
void XAnimCalcDeltaTree(const XAnimTree_s *tree, unsigned int animIndex, float weightScale, bool bClear, bool bNormQuat, XAnimSimpleRotPos *rotPos);
void XAnimCalcDelta(XAnimTree_s *tree, unsigned int animIndex, float *rot, float *trans, bool bUseGoalWeight);
void XAnimGetAbsDelta(const XAnim_s *anims, unsigned int animIndex, float *rot, float *trans, float factor);
void XAnimCalcAbsDelta(XAnimTree_s *tree, unsigned int animIndex, float *rot, float *trans);
void XAnimCalcAbsDeltaParts(const XAnimParts_s *parts, const float weightScale, float time, XAnimSimpleRotPos *rotPos);
void XAnimCalcRelDeltaParts(const XAnimParts_s *parts, float weightScale, float time1, float time2, XAnimSimpleRotPos *rotPos, int quatIndex);
void XAnimClearServerNotify(XAnimInfo *info);
void XAnimClearTreeWeights(XAnimTree_s *tree, unsigned int index);
void XAnimSetupSyncNodes_r(XAnim_s *anims, unsigned int animIndex);
void XAnimClearGoalWeightKnobInternal(XAnimTree_s *tree, unsigned int infoIndex, float goalWeight, float goalTime);
void XAnimEnsureGoalWeightParent(XAnimTree_s *tree, unsigned int infoIndex, float goalTime);
void XAnim_GetTimeIndexCompressed( const XAnimTime *animTime, const unsigned short *indices, int tableSize, int *keyFrameIndex, float *keyFrameLerpFrac );
void XAnim_GetTimeIndexCompressed( const XAnimTime *animTime, const unsigned char *indices, int tableSize, int *keyFrameIndex, float *keyFrameLerpFrac );
void TransformToQuatRefFrame( const vec2_t rot, vec3_t trans );
void XAnim_SetTime( float fTime, int frameCount, XAnimTime *animTime );
void XAnim_CalcPosDeltaDuring_unsigned_short_( XAnimDeltaPart *animDelta, const float fTime, int frameCount, vec3_t posDelta );
void XAnim_CalcPosDeltaDuring_unsigned_char_( XAnimDeltaPart *animDelta, const float fTime, int frameCount, vec3_t posDelta );
void XAnimClearData( const DObj *obj, DObjAnimMat *rotTransArray, XAnimCalcAnimInfo *info );
void XAnim_CalcPosEnd( const XAnimPartTrans *trans, float scale, vec3_t posDelta );
void XAnim_CalcRotEnd( const XAnimPartQuat *quat, float scale, vec4_t rotDelta );
void XAnim_CalcSimpleRotEnd( const XAnimPartQuat *quat, float scale, vec2_t rotDelta );
void XAnim_CalcPosDuring_unsigned_short_( const XAnimTime *animTime, const XAnimPartTrans *trans, float weightScale, vec3_t posDelta );
void XAnim_CalcRotDuring_unsigned_short_( const XAnimTime *animTime, const XAnimPartQuat *quat, float weightScale, vec4_t rotDelta );
void XAnim_CalcSimpleRotDuring_unsigned_short_( const XAnimTime *animTime, const XAnimPartQuat *quat, float weightScale, vec2_t rotDelta );
void XAnim_CalcPosDuring_unsigned_char_( const XAnimTime *animTime, const XAnimPartTrans *trans, float weightScale, vec3_t posDelta );
void XAnim_CalcRotDuring_unsigned_char_( const XAnimTime *animTime, const XAnimPartQuat *quat, float weightScale, vec4_t rotDelta );
void XAnim_CalcSimpleRotDuring_unsigned_char_(const XAnimTime *animTime, const XAnimPartQuat *quat, float weightScale, vec2_t rotDelta );
void XAnim_CalcRotDeltaDuringInternal_unsigned_short_( const XAnimDeltaPartQuat *quat, const float fTime, int frameCount, vec2_t rotDelta );
void XAnim_CalcRotDeltaDuringInternal_unsigned_char_( const XAnimDeltaPartQuat *quat, const float fTime, int frameCount, vec2_t rotDelta );
void XAnimCalcParts_unsigned_short_( const XAnimParts *parts, const unsigned char *animToModel, float fTime, float weightScale, DObjAnimMat *rotTransArray, int *ignorePartBits );
void XAnimCalcParts_unsigned_char_( const XAnimParts *parts, const unsigned char *animToModel, float fTime, float weightScale, DObjAnimMat *rotTransArray, int *ignorePartBits );
void XAnim_CalcRotDeltaDuring_unsigned_short_( XAnimDeltaPart *animDelta, const float fTime, int frameCount, vec2_t rotDelta );
void XAnim_CalcRotDeltaDuring_unsigned_char_( XAnimDeltaPart *animDelta, const float fTime, int frameCount, vec2_t rotDelta );
void XAnimCloneAnimInfo( const XAnimInfo *from, XAnimInfo *to );
void XAnim_CalcPosDeltaEntire( const XAnimDeltaPart *animDelta, vec3_t posDelta );
void XAnim_CalcRotDeltaEntire( const XAnimDeltaPart *animDelta, vec2_t rotDelta );
void XAnimCalcNonLoopEnd( const XAnimParts_s *parts, const unsigned char *animToModel, float weightScale, DObjAnimMat *rotTransArray, int *ignorePartBits );

unsigned int XAnimSetModel(const XAnimEntry *animEntry, XModel *const *model, int numModels);

void DObjDisplayAnim(DObj_s *obj);
int DObjUpdateServerInfo(DObj_s *obj, float dtime, int bNotify);
void DObjInitServerTime(DObj_s *obj, float dtime);

bool XModelBad( const XModel *model );
XModel* XModelPrecache(const char *name, void *(*Alloc)(int), void *(*AllocColl)(int));
int XModelGetNumLods(const XModel *model);
int XModelGetFlags(const XModel *model);
const char* XModelGetName(const XModel *model);
qboolean XModelGetStaticBounds(const XModel *model, float (*axis)[3], float *mins, float *maxs);

const char* XAnimGetAnimDebugName(const XAnim_s *anims, unsigned int animIndex);
XAnimParts* XAnimPrecache(const char *name, void *(*Alloc)(int));
void XAnimInit();
void XAnimAbort();
void XAnimShutdown();

unsigned short* XModelBoneNames(XModel *model);
int XModelGetContents(const XModel *model);
DObjAnimMat* XModelGetBasePose( const XModel *model );

int XModelGetLodForDist(const XModel *model, float dist );
float XModelGetLodOutDist( const XModel *model );

XAnim* Scr_GetAnims(int index);
int Scr_CreateAnimationTree(unsigned int parentNode, unsigned int names, XAnim_s *anims, unsigned int childIndex, const char *parentName, unsigned int parentIndex, unsigned int filename, int treeIndex);

void XAnimCloneAnimTree(const XAnimTree_s *from, XAnimTree_s *to);
float XAnimGetAverageRateFrequency(const XAnimTree_s *tree, unsigned int infoIndex);
void XAnimProcessServerNotify(const XAnimTree_s *tree, XAnimInfo *info, const XAnimEntry *entry, float dtime);
void XAnimProcessClientNotify(XAnimInfo *info, const XAnimEntry *entry, float dtime);
void XAnimClearTree(XAnimTree_s *tree);
float XAnimGetWeight(const XAnimTree_s *tree, unsigned int animIndex);
void XAnimSetAnimRate(XAnimTree_s *tree, unsigned int animIndex, float rate);
bool XAnimIsLooped(const XAnim_s *anim, unsigned int animIndex);
bool XAnimIsPrimitive(XAnim_s *anim, unsigned int animIndex);
float XAnimGetLength(const XAnim_s *anims, unsigned int animIndex);
int XAnimGetLengthMsec(const XAnim_s *anim, unsigned int animIndex);
void XAnimDisplay(const XAnimTree_s *tree, unsigned int infoIndex, int depth);
void XAnimClearGoalWeight(XAnimTree_s *tree, unsigned int animIndex, float blendTime);
void XAnimSetTime(XAnimTree_s *tree, unsigned int animIndex, float time);
void XAnimRestart(XAnimTree_s *tree, unsigned int infoIndex);
float XAnimGetServerNotifyFracSyncTotal(const XAnimTree_s *tree, XAnimInfo *info, const XAnimEntry *entry, const XAnimState *state, const XAnimState *nextState, float dtime);
void XAnimSetCompleteGoalWeight(XAnimTree_s *tree, unsigned int animIndex, float goalWeight, float goalTime, float rate, unsigned int notifyName, unsigned int notifyType, int bRestart);
void XAnimSetCompleteGoalWeightKnobAll(XAnimTree_s *tree, unsigned int animIndex, unsigned int rootIndex, float goalWeight, float goalTime, float rate, unsigned int notifyName, int bRestart);
void XAnimClearTreeGoalWeights(XAnimTree_s *tree, unsigned int animIndex, float blendTime);
void XAnimClearTreeGoalWeightsStrict(XAnimTree_s *tree, unsigned int animIndex, float blendTime);
int XAnimGetNumChildren(const XAnim_s *anim, unsigned int animIndex);
unsigned int XAnimGetChildAt(const XAnim_s *anim, unsigned int animIndex, unsigned int childIndex);
void XAnimSetGoalWeight(XAnimTree_s *tree, unsigned int animIndex, float goalWeight, float goalTime, float rate, unsigned int notifyName, unsigned int notifyType, int bRestart);
void XAnimUpdateOldTime(XAnimTree_s *tree, unsigned int infoIndex, XAnimState *syncState, float dtime, bool parentHasWeight, bool *childHasTimeForParent1, bool *childHasTimeForParent2);
int XAnimSetGoalWeightInternal(XAnimTree_s *tree, unsigned int animIndex, float goalWeight, float goalTime, float rate, bool useGoalWeight, unsigned int notifyName, unsigned int notifyType);
void XAnimSetupSyncNodes(XAnim_s *anims);
void XAnimFreeTree(XAnimTree_s *tree, void (*Free)(void *, int));
XAnimTree_s* XAnimCreateTree(XAnim_s *anims, void *(*Alloc)(int));
void XAnimCreate(XAnim_s *anims, unsigned int animIndex, const char *name);
void XAnimBlend(XAnim_s *anims, unsigned int animIndex, const char *name, unsigned int children, unsigned int num, unsigned int flags);
XAnim_s* XAnimCreateAnims(const char *debugName, int size, void *(*Alloc)(int));
void *Hunk_AllocXAnimTreePrecache(int size);
XAnimParts_s *XAnimFindData(const char *name);
XAnimInfo* XAnimGetInfo(XAnimTree_s *tree, unsigned int infoIndex);
void XAnimFreeInfo(XAnimTree_s *tree, unsigned int infoIndex);
unsigned int XAnimGetAnimTreeSize(const XAnim_s *anims);
const char* XAnimGetAnimName(const XAnim_s *anims, unsigned int animIndex);
float XAnimGetTime(const XAnimTree_s *tree, unsigned int animIndex);
XAnim_s* XAnimGetAnims(const XAnimTree_s *tree);
const char* XAnimGetAnimTreeDebugName(const XAnim_s *anims);
bool XAnimNotetrackExists(const XAnim_s *anims, unsigned int animIndex, unsigned int name);
bool XAnimIsDefault( XAnimParts *parts );
XAnimParts* XAnimLoadDefaultAnim(XAnimParts *animParts, void *(*Alloc)(int));
const char* XAnimSetData(const char *modelName, XAnimParts *anim, void *(*Alloc)(int));

float ShortLerpAsVec( short from, short to, float frac );
void Short2LerpAsVec2( const short from[2], const short to[2], float frac, vec2_t out );
void Vec2MadShort2Lerp( const vec2_t from, float scale, const short to1[2], const short to2[2], float frac, vec2_t out );
void Vec4MadShort4Lerp( const vec4_t from, float scale, const short to1[4], const short to2[4], float frac, vec4_t out );
void Vec2MadShort2( const vec2_t from, float frac, const short to[2], vec2_t out );
void Vec4MadShort4( const vec4_t from, float frac, const short to[4], vec4_t out );
void Short4ScaleAsVec4( short from[4], float scale, vec4_t to );
float FloatLerp( float a, float to, float t );
void Vec3MadVec3Lerp( const vec3_t from, float scale, const vec3_t to1, const vec3_t to2, float frac, vec3_t out );
void Short2CopyAsVec2( const short from[2], vec2_t to );
XAnimInfo* GetAnimInfo( int infoIndex );

int DObjHasContents(DObj_s *obj, int contentmask);
void DObjGeomTraceline( DObj *obj, const vec3_t localStart, const vec3_t localEnd, int contentmask, DObjTrace_s *results );
void DObjDumpInfo(const DObj_s *obj);

unsigned char ConsumeByte (const unsigned char **pos);
short ConsumeShort(const unsigned char **pos);
unsigned short ConsumeUShort(const unsigned char **pos);
int ConsumeInt(const unsigned char **pos);
float ConsumeFloat(const unsigned char **pos);
void ConsumeQuat(const unsigned char **pos, short *out);
void QuatMultiplyEquals(const float *in, float *inout);
void QuatMultiplyReverseEquals(float *in, float *inout);
void MatrixTransformVectorQuatTransEquals(float *inout, DObjAnimMat *in);
void LocalInvMatrixTransformVectorQuatTrans(const float *in, const DObjAnimMat *mat, float *out);
void DObjCalcTransWeight(DObjAnimMat *Mat);
void DObjSkelClear( DObj_s *obj );
void DObjComputeBounds( DObj *obj );
void NormalizeQuatTrans( DObjAnimMat *mat );

void *Hunk_AllocXAnimPrecache(int size);
void *Hunk_AllocXAnimTreePrecache(int size);
