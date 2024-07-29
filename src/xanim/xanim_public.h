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
#if defined(__i386__)
static_assert((sizeof(XAnimParts) == 44), "ERROR: XAnimParts size is invalid!");
#endif

struct DObjAnimMat
{
	vec4_t quat;
	vec3_t trans;
	float transWeight;
};
#if defined(__i386__)
static_assert((sizeof(DObjAnimMat) == 32), "ERROR: DObjAnimMat size is invalid!");
#endif

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
#if defined(__i386__)
static_assert((sizeof(XAnim) == 0x14), "ERROR: XAnim size is invalid!");
#endif

typedef struct XAnimTree_s
{
	XAnim_s *anims;
	uint16_t entnum;
	bool bAbs;
	bool bUseGoalWeight;
	uint16_t infoArray[1];
} XAnimTree;
#if defined(__i386__)
static_assert((sizeof(XAnimTree) == 0xC), "ERROR: XAnimTree size is invalid!");
#endif

struct XBoneInfo
{
	vec3_t bounds[2];
	vec3_t offset;
	float radiusSquared;
};

struct XAnimToXModel
{
	int partBits[4];
	byte boneIndex[128];
};

struct XModelCollTri_s
{
	vec4_t plane;
	vec4_t svec;
	vec4_t tvec;
};

struct DSkelPartBits_s
{
	int anim[4];
	int control[4];
	int skel[4];
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
#if defined(__i386__)
static_assert((sizeof(XModelParts) == 100), "ERROR: XModelParts size is invalid!");
#endif

typedef struct XModelSurfs_s
{
	struct XSurface *surf; // !!! Not loaded in server binary
	int partBits[4];
} XModelSurfs;
#if defined(__i386__)
static_assert((sizeof(XModelSurfs) == 20), "ERROR: XModelSurfs size is invalid!");
#endif

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

typedef struct
{
	XModelParts_s *parts;
	XModelLodInfo_s lodInfo[4];
	XModelCollSurf_s *collSurfs;
	int numCollSurfs;
	int contents;
	XBoneInfo *boneInfo;
	vec3_t mins;
	vec3_t maxs;
	short numLods;
	short collLod;
	struct Material *xskins; // !!! Not loaded in server binary
	int memUsage;
	const char *name;
	char flags;
	bool bad;
} XModel;
#if defined(__i386__)
static_assert((sizeof(XModel) == 0x90), "ERROR: XModel size is invalid!");
#endif

struct DObjModel_s
{
	XModel *model;
	const char *boneName;
	qboolean ignoreCollision;
};

#define DOBJ_MAX_SUBMODELS 8

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
	byte modelParents[8];
	byte matOffset[8];
	vec3_t mins;
	vec3_t maxs;
} DObj;
#if defined(__i386__)
static_assert((sizeof(DObj) == 100), "ERROR: DObj size is invalid!");
#endif

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
#if defined(__i386__)
static_assert((sizeof(XAnimInfo) == 40), "ERROR: XAnimInfo size is invalid!");
#endif

extern XAnimInfo g_xAnimInfo[];

struct XAnimClientNotify
{
	const char *name;
	unsigned int notifyName;
	float timeFrac;
};

struct XAnimSimpleRotPos
{
	float rot[2];
	float posWeight;
	float pos[3];
};

struct XAnimCalcAnimInfo
{
	DObjAnimMat rotTransArray[512];
	int animPartBits[4];
	int ignorePartBits[4];
};

#pragma pack(push, 1)
struct XAnimDeltaInfo
{
	bool hasTime;
	bool hasWeight;
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

void DObjCreate(DObjModel_s *dobjModels, unsigned int numModels, XAnimTree_s *tree, void *buf, unsigned int entnum);
void DObjGetBounds(const DObj_s *obj, float *mins, float *maxs);
void DObjInit();
void DObjShutdown();
void DObjFree(DObj_s *obj);
void DObjAbort();

void DObjSetControlTagAngles(const DObj_s *obj, int *partBits, unsigned int boneIndex, float *angles);
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
void DObjTraceline(DObj_s *obj, float *start, float *end, unsigned char *priorityMap, DObjTrace_s *trace);

void XAnimGetRelDelta(const XAnim_s *anim, unsigned int animIndex, float *rot, float *trans, float startTime, float endTime);

void XAnimCalc(const DObj_s *obj, unsigned int entry, float weightScale, DObjAnimMat *rotTransArray, bool bClear, bool bNormQuat, XAnimCalcAnimInfo *animInfo, int rotTransArrayIndex);
void XAnim_CalcDeltaForTime(const XAnimParts_s *anim, const float time, float *rotDelta, float *posDelta);
void XAnimCalcDeltaTree(const XAnimTree_s *tree, unsigned int animIndex, float weightScale, bool bClear, bool bNormQuat, XAnimSimpleRotPos *rotPos);
void XAnimCalcDelta(XAnimTree_s *tree, unsigned int animIndex, float *rot, float *trans, bool bUseGoalWeight);
void XAnimGetAbsDelta(const XAnim_s *anims, unsigned int animIndex, float *rot, float *trans, float factor);
void XAnimCalcAbsDelta(XAnimTree_s *tree, unsigned int animIndex, float *rot, float *trans);

unsigned int XAnimSetModel(const XAnimEntry *animEntry, XModel *const *model, int numModels);

void DObjDisplayAnim(DObj_s *obj);
int DObjUpdateServerInfo(DObj_s *obj, float dtime, int bNotify);
void DObjInitServerTime(DObj_s *obj, float dtime);

qboolean XModelBad(XModel *model);
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
DObjAnimMat* XModelGetBasePose(const XModel *model);
XAnim* Scr_GetAnims(unsigned int index);

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
void XAnimUpdateInfoSyncInternal(const XAnimTree_s *tree, unsigned int index, bool update, XAnimState *state, float dtime);
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
void *Hunk_AllocXAnimServer(int size);
float XAnimGetTime(const XAnimTree_s *tree, unsigned int animIndex);
XAnim_s* XAnimGetAnims(const XAnimTree_s *tree);
const char* XAnimGetAnimTreeDebugName(const XAnim_s *anims);
bool XAnimNotetrackExists(const XAnim_s *anims, unsigned int animIndex, unsigned int name);

int DObjHasContents(DObj_s *obj, int contentmask);
void DObjGeomTraceline(DObj_s *obj, float *localStart, float *localEnd, int contentmask, DObjTrace_s *results);
void DObjDumpInfo(const DObj_s *obj);

int XAnim_ReadShort(const unsigned char **pos);
int XAnim_ReadUnsignedShort(const unsigned char **pos);
int XAnim_ReadInt(const unsigned char **pos);
float XAnim_ReadFloat(const unsigned char **pos);
void ConsumeQuat(const unsigned char **pos, short *out);
void QuatMultiplyEquals(const float *in, float *inout);
void QuatMultiplyReverseEquals(float *in, float *inout);
void MatrixTransformVectorQuatTransEquals(float *inout, DObjAnimMat *in);
void InvMatrixTransformVectorQuatTrans(const float *in, const DObjAnimMat *mat, float *out);
void DObjCalcTransWeight(DObjAnimMat *Mat);