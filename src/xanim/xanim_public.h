#pragma once

#define XANIM_VERSION 20

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

struct XAnimNotifyInfo
{
	unsigned short name;
	unsigned short pad;
	float time;
};

union XAnimDynamicIndices
{
	byte _1[1];
	uint16_t _2[1];
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

struct XAnimPartTransFrames
{
	float (*frames)[3];
	XAnimDynamicIndices indices;
};

struct XAnimPartTransData
{
	XAnimPartTransFrames frames;
	float frame0[3];
};

struct XAnimDeltaPartQuat
{
	uint16_t size;
	XAnimDeltaPartQuatData u;
};

struct XAnimPartTrans
{
	uint16_t size;
	XAnimPartTransData u;
};

struct XAnimDeltaPart
{
	XAnimPartTrans *trans;
	XAnimDeltaPartQuat *quat;
};

typedef struct XAnimParts_s
{
	unsigned short numframes;
	byte looping;
	byte indexCount;
	float framerate;
	float frequency;
	byte notifyCount;
	char assetType;
	short boneCount;
	uint16_t *names;
	char *dataByte;
	void *indices;	 // !!! Not known !!!
	XAnimNotifyInfo *notify;
	XAnimDeltaPart *deltaPart;
	const char *name;
	bool isDefault;
} XAnimParts;
static_assert((sizeof(XAnimParts) == 44), "ERROR: XAnimParts size is invalid!");

struct DObjAnimMat
{
	vec4_t quat;
	vec3_t trans;
	float transWeight;
};
static_assert((sizeof(DObjAnimMat) == 32), "ERROR: DObjAnimMat size is invalid!");

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
static_assert((sizeof(XAnim) == 0x14), "ERROR: XAnim size is invalid!");

typedef struct XAnimTree_s
{
	XAnim_s *anims;
	uint16_t parent;
	bool bUseDelta;
	bool bUseGoalWeight;
	uint16_t children[1];
} XAnimTree;
static_assert((sizeof(XAnimTree) == 0xC), "ERROR: XAnimTree size is invalid!");

struct XBoneInfo
{
	vec3_t bounds[2];
	vec3_t offset;
	float radiusSquared;
};

struct XModelCollTri_s
{
	vec4_t plane;
	vec4_t svec;
	vec4_t tvec;
};

typedef struct XModelParts_s
{
	short numBones;
	short numRootBones;
	unsigned short **boneNames;
	short *quats;
	float *trans;
	byte *partClassification;
	XModelCollTri_s partCollTris;
	DObjAnimMat baseMat;
} XModelParts;
static_assert((sizeof(XModelParts) == 100), "ERROR: XModelParts size is invalid!");

typedef struct XModelSurfs_s
{
	struct XSurface *surf; // !!! Not loaded in server binary
	int partBits[4];
} XModelSurfs;
static_assert((sizeof(XModelSurfs) == 20), "ERROR: XModelSurfs size is invalid!");

typedef struct XModelLodInfo_s
{
	float dist;
	const char *name;
	short numsurfs;
	unsigned short *surfnames;
	XModelSurfs_s *surface;
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
	XModelParts_s *modelParts;
	XModelLodInfo_s lodInfo[4];
	XModelCollSurf_s *collSurfs;
	int numCollSurfs;
	int contents;
	XBoneInfo *boneInfo;
	vec3_t mins;
	vec3_t maxs;
	short numLods;
	short collLod;
	struct Material *skins; // !!! Not loaded in server binary
	int memUsage;
	const char *name;
	char flags;
	bool bad;
} XModel;
static_assert((sizeof(XModel) == 0x90), "ERROR: XModel size is invalid!");

struct DObjModel_s
{
	XModel *model;
	const char *parentModelName;
	qboolean ignoreCollision;
};

struct DSkelPartBits_s
{
	int anim[4];
	int control[4];
	int skel[4];
	DObjAnimMat Mat;
};

struct DSkel_t
{
	DSkelPartBits_s *partBits;
	int timeStamp;
};

typedef struct DObj_s
{
	XAnimTree_s *tree;
	DSkel_t skel;
	unsigned short *duplicateParts;
	unsigned short duplicatePartsSize;
	unsigned int ignoreCollision;
	byte numModels;
	byte numBones;
	byte flags;
	XModel *models[8];
	byte modelIndex[8];
	byte boneIndex[8];
	vec3_t mins;
	vec3_t maxs;
} DObj;
static_assert((sizeof(DObj) == 100), "ERROR: DObj size is invalid!");

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
static_assert((sizeof(XAnimInfo) == 40), "ERROR: XAnimInfo size is invalid!");

struct XAnimClientNotify
{
	const char *name;
	unsigned int notifyName;
	float timeFrac;
};

struct XAnimSimpleRotPos
{
	vec2_t rot;
	float transWeight;
	vec3_t trans;
	vec3_t pos;
	float posWeight;
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

void DObjCreate(DObjModel_s *dobjModels, unsigned int numModels, XAnimTree_s *tree, void *buf, unsigned int entnum);
void DObjCreateDuplicateParts(DObj_s *obj);
void DObjGetBounds(const DObj_s *obj, float *mins, float *maxs);
void DObjInit();
void DObjShutdown();
void DObjFree(DObj_s *obj);
void DObjAbort();

int DObjGetBoneIndex(const DObj_s *obj, unsigned int name);

void XAnimFree(XAnimParts *parts);
void XAnimFreeList(XAnim *anims);

#ifdef __cplusplus
extern "C" {
#endif

void QDECL XModelPartsFree(XModelParts *modelParts);
void QDECL XModelFree(XModel *model);
XModel* QDECL XModelLoadFile(const char *name, void *(*Alloc)(int), void *(*AllocColl)(int));
XModelParts* QDECL XModelPartsLoad(XModel *model, const char *partName, void *(*Alloc)(int));
XModelParts* QDECL XModelPartsLoadFile(XModel *model, const char *partName, void *(*Alloc)(int));
XAnimParts* QDECL XAnimLoadFile(const char *name, void *(*Alloc)(int));
void QDECL XModelGetBounds(const XModel *model, float *mins, float *maxs);
int QDECL XModelGetBoneIndex(const XModel *model, unsigned int name);
int QDECL XModelNumBones(const XModel *model);
int QDECL XModelTraceLine(const XModel *model, trace_t *results, DObjAnimMat *pose, const float *localStart, const float *localEnd, int contentmask);

void QDECL XAnimUpdateInfoInternal(XAnimTree_s *tree, unsigned int infoIndex, float dtime, bool update);
float QDECL XAnimFindServerNoteTrack(const XAnimTree_s *anim, unsigned int infoIndex, float dtime);

// This function has some weird bs and breaks the decompiler. Moved to assembly.
void QDECL DObjUpdateServerInfo(DObj_s *obj, float dtime, int bNotify);

void QDECL XAnim_CalcDeltaForTime(const XAnimParts_s *parts, const float weightScale, float time, XAnimSimpleRotPos *rotPos);

float QDECL XAnimCalc_GetLowestElement(float value);
unsigned int QDECL XAnimSetModel(const XAnimEntry *animEntry, XModel *const *model, int numModels);

void QDECL XAnimCalcParts(const XAnimParts_s *parts, const unsigned char *animToModel, float time, float weightScale, DObjAnimMat *rotTransArray, int *ignorePartBits);
void QDECL XAnimCalcParts2(const XAnimParts_s *parts, const unsigned char *animToModel, float time, float weightScale, DObjAnimMat *rotTransArray, int *ignorePartBits);
void QDECL XAnimCalcNonLoopEnd(const XAnimParts_s *parts, const unsigned char *animToModel, float weightScale, DObjAnimMat *rotTransArray, int *ignorePartBits);

#ifdef __cplusplus
}
#endif

qboolean XModelBad(XModel *model);
XModel* XModelPrecache(const char *name, void *(*Alloc)(int), void *(*AllocColl)(int));

qboolean XModelGetStaticBounds(const XModel *model, const float *axis, float *mins, float *maxs);

const char* XAnimGetAnimDebugName(const XAnim_s *anims, unsigned int animIndex);
XAnimParts* XAnimPrecache(const char *name, void *(*Alloc)(int));
void XAnimInit();
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
void XAnimClearGoalWeight(XAnimTree_s *tree, unsigned int animIndex, float blendTime);
void XAnimSetTime(XAnimTree_s *tree, unsigned int animIndex, float time);
void XAnimRestart(XAnimTree_s *tree, unsigned int infoIndex);
void XAnimSetCompleteGoalWeight(XAnimTree_s *tree, unsigned int animIndex, float goalWeight, float goalTime, float rate, unsigned int notifyName, unsigned int notifyType, int bRestart);
void XAnimSetCompleteGoalWeightKnobAll(XAnimTree_s *tree, unsigned int animIndex, unsigned int rootIndex, float goalWeight, float goalTime, float rate, unsigned int notifyName, int bRestart);
void XAnimClearTreeGoalWeights(XAnimTree_s *tree, unsigned int animIndex, float blendTime);
void XAnimClearTreeGoalWeightsStrict(XAnimTree_s *tree, unsigned int animIndex, float blendTime);
int XAnimGetNumChildren(const XAnim_s *anim, unsigned int animIndex);
unsigned int XAnimGetChildAt(const XAnim_s *anim, unsigned int animIndex, unsigned int childIndex);
void XAnimSetGoalWeight(XAnimTree_s *tree, unsigned int animIndex, float goalWeight, float goalTime, float rate, unsigned int notifyName, unsigned int notifyType, int bRestart);
void XAnimUpdateOldTime(XAnimTree_s *tree, unsigned int infoIndex, XAnimState *syncState, float dtime, bool parentHasWeight, bool *childHasTimeForParent1, bool *childHasTimeForParent2);

void DObjCalcAnim(const DObj_s *obj, int *partBits);