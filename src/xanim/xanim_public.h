#pragma once

#define XMODEL_VERSION 20

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

union XAnimIndices
{
	byte *b;
	uint16_t *s;
	void *data;
};

struct XAnimNotifyInfo
{
	unsigned short name;
	unsigned short pad;
	float time;
};

struct XAnimDynamicIndices
{
	char _1[1];
	uint16_t _2[1];
};

struct XAnimDeltaPartQuatDataFrames
{
	int16_t (*frames)[2];
	XAnimDynamicIndices indices;
};

struct XAnimDeltaPartQuatData
{
	XAnimDeltaPartQuatDataFrames frames;
	int16_t frame0[2];
};

struct XAnimDeltaPartQuat
{
	uint16_t size;
	uint16_t pad;
	XAnimDeltaPartQuatData u;
};

struct XAnimDynamicFrames
{
	char (*_1)[3];
	uint16_t (*_2)[3];
};

struct XAnimPartTransFrames
{
	float mins[3];
	float size[3];
	XAnimDynamicFrames frames;
	XAnimDynamicIndices indices;
};

struct XAnimPartTransData
{
	XAnimPartTransFrames frames;
	float frame0[3];
};

struct XAnimPartTrans
{
	uint16_t size;
	char smallTrans;
	char pad;
	XAnimPartTransData u;
};

struct XAnimDeltaPart
{
	XAnimPartTrans *trans;
	XAnimDeltaPartQuat *quat;
};

typedef struct
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
	XAnimIndices indices;
	XAnimNotifyInfo *notify;
	XAnimDeltaPart *deltaPart;
	const char *name;
	bool isDefault;
} XAnimParts; // !!! Size not confirmed

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
static_assert((sizeof(XAnim) == 0x14), "ERROR: XAnim size is invalid!");

typedef struct XAnimTree_s
{
	XAnim_s *anims;
	uint16_t children;
	uint16_t pad;
} XAnimTree;

struct XBoneInfo
{
	vec3_t bounds[2];
	vec3_t offset;
	float radiusSquared;
};

typedef struct XModelParts_s
{
	unsigned short numBones;
	unsigned short numRootBones;
	unsigned short **boneNames;
	short *quats;
	float *trans;
	unsigned char *partClassification;
	DObjAnimMat Mat;
	DObjAnimMat localMat;
} XModelParts;
static_assert((sizeof(XModelParts) == 0x54), "ERROR: XModelParts size is invalid!");

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
	unsigned short numsurfs;
	unsigned short *surfnames;
	XModelSurfs_s *surface;
} XModelLodInfo;

typedef struct XModelCollTri_s
{
	vec4_t plane;
	vec4_t svec;
	vec4_t tvec;
} XModelCollTri;

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
};

struct DSkel_t
{
	DSkelPartBits_s *partBits;
	int timeStamp;
	DObjAnimMat *mat;
};

typedef struct DObj_s
{
	XAnimTree_s *tree;
	DSkel_t skel;
	unsigned short duplicateParts;
	unsigned int ignoreCollision;
	byte numModels;
	byte numBones;
	byte duplicatePartsSize;
	XModel *models[1];
} DObj; // !!! Size not confirmed

void DObjInit();
void DObjShutdown();

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

#ifdef __cplusplus
}
#endif

qboolean XModelBad(XModel *model);
XModel* XModelPrecache(const char *name, void *(*Alloc)(int), void *(*AllocColl)(int));

qboolean XModelGetStaticBounds(const XModel *model, const float *axis, float *mins, float *maxs);

void XAnimInit();
void XAnimShutdown();