#pragma once

#include <stdint.h>

#define MAX_SUBMODELS 1024
#define CAPSULE_MODEL_HANDLE 1023

// plane types are used to speed some tests
// 0-2 are axial planes
#define PLANE_X         0
#define PLANE_Y         1
#define PLANE_Z         2
#define PLANE_NON_AXIAL 3

#define PlaneTypeForNormal( x ) ( x[0] == 1.0 ? PLANE_X : ( x[1] == 1.0 ? PLANE_Y : ( x[2] == 1.0 ? PLANE_Z : PLANE_NON_AXIAL ) ) )

enum LumpType
{
	LUMP_MATERIALS = 0,
	LUMP_LIGHTBYTES = 1,
	LUMP_LIGHTGRIDENTRIES = 2,
	LUMP_LIGHTGRIDCOLORS = 3,
	LUMP_PLANES = 4,
	LUMP_BRUSHSIDES = 5,
	LUMP_BRUSHES = 6,
	LUMP_TRIANGLES = 7,
	LUMP_DRAWVERTS = 8,
	LUMP_DRAWINDICES = 9,
	LUMP_CULLGROUPS = 10,
	LUMP_CULLGROUPINDICES = 11,
	LUMP_OBSOLETE_1 = 12,
	LUMP_OBSOLETE_2 = 13,
	LUMP_OBSOLETE_3 = 14,
	LUMP_OBSOLETE_4 = 15,
	LUMP_OBSOLETE_5 = 16,
	LUMP_PORTALVERTS = 17,
	LUMP_OCCLUDER = 18,
	LUMP_OCCLUDERPLANES = 19,
	LUMP_OCCLUDEREDGES = 20,
	LUMP_OCCLUDERINDICES = 21,
	LUMP_AABBTREES = 22,
	LUMP_CELLS = 23,
	LUMP_PORTALS = 24,
	LUMP_NODES = 25,
	LUMP_LEAFS = 26,
	LUMP_LEAFBRUSHES = 27,
	LUMP_LEAFSURFACES = 28,
	LUMP_COLLISIONVERTS = 29,
	LUMP_COLLISIONEDGES = 30,
	LUMP_COLLISIONTRIS = 31,
	LUMP_COLLISIONBORDERS = 32,
	LUMP_COLLISIONPARTITIONS = 33,
	LUMP_COLLISIONAABBS = 34,
	LUMP_MODELS = 35,
	LUMP_VISIBILITY = 36,
	LUMP_ENTITIES = 37,
	LUMP_PATHCONNECTIONS = 38,
	LUMP_PRIMARY_LIGHTS = 39,
};

typedef struct dmaterial_s
{
	char material[64];
	int surfaceFlags;
	int contentFlags;
} dmaterial_t;
static_assert((sizeof(dmaterial_t) == 72), "ERROR: dmaterial_t size is invalid!");

typedef struct cStaticModel_s
{
	uint16_t writable;
	struct XModel *xmodel;
	vec3_t origin;
	vec3_t invScaledAxis[3];
	vec3_t absmin;
	vec3_t absmax;
} cStaticModel_t;

typedef struct cplane_s
{
	vec3_t normal;
	float dist;
	byte type;
	byte signbits;
	byte pad[2];
} cplane_t;
static_assert((sizeof(cplane_t) == 20), "ERROR: cplane_t size is invalid!");

typedef struct cbrushside_s
{
	cplane_t *plane;
	unsigned int materialNum;
} cbrushside_t;
static_assert((sizeof(cbrushside_t) == 8), "ERROR: cbrushside_t size is invalid!");

typedef struct cNode_s
{
	cplane_t *plane;
	int16_t children[2];
} cNode_t;
static_assert((sizeof(cNode_t) == 8), "ERROR: cNode_t size is invalid!");

typedef struct cLeaf_s
{
	uint16_t firstCollAabbIndex;
	uint16_t collAabbCount;
	int brushContents;
	int terrainContents;
	vec3_t mins;
	vec3_t maxs;
	int leafBrushNode;
	int16_t cluster;
	int16_t pad;
} cLeaf_t;
static_assert((sizeof(cLeaf_t) == 44), "ERROR: cLeaf_t size is invalid!");

typedef struct cLeafBrushNodeLeaf_s
{
	uint16_t *brushes;
} cLeafBrushNodeLeaf_t;

typedef struct cLeafBrushNodeChildren_s
{
	float dist;
	float range;
	uint16_t childOffset[2];
} cLeafBrushNodeChildren_t;

typedef union cLeafBrushNodeData_s
{
	cLeafBrushNodeLeaf_t leaf;
	cLeafBrushNodeChildren_t children;
} cLeafBrushNodeData_t;

#pragma pack(push, 2)
typedef struct cLeafBrushNode_s
{
	byte axis;
	uint16_t leafBrushCount;
	int contents;
	cLeafBrushNodeData_t data;
} cLeafBrushNode_t;
#pragma pack(pop)
static_assert((sizeof(cLeafBrushNode_t) == 20), "ERROR: cLeafBrushNode_t size is invalid!");

typedef struct CollisionBorder
{
	float distEq[3];
	float zBase;
	float zSlope;
	float start;
	float length;
} CollisionBorder_t;
static_assert((sizeof(CollisionBorder_t) == 28), "ERROR: CollisionBorder_t size is invalid!");

typedef union CollisionAabbTreeIndex_s
{
	int firstChildIndex;
	int partitionIndex;
} CollisionAabbTreeIndex_t;

typedef struct CollisionAabbTree_s
{
	float origin[3];
	float halfSize[3];
	uint16_t materialIndex;
	uint16_t childCount;
	CollisionAabbTreeIndex_t u;
} CollisionAabbTree_t;
static_assert((sizeof(CollisionAabbTree_t) == 32), "ERROR: CollisionAabbTree_t size is invalid!");

typedef struct cmodel_s
{
	vec3_t mins;
	vec3_t maxs;
	float radius;
	cLeaf_t leaf;
} cmodel_t;
static_assert((sizeof(cmodel_t) == 72), "ERROR: cmodel_t size is invalid!");

typedef struct __attribute__((aligned(16))) cbrush_s
{
	float mins[3];
	int contents;
	float maxs[3];
	unsigned int numsides;
	cbrushside_t *sides;
	int16_t axialMaterialNum[2][3];
} cbrush_t;
static_assert((sizeof(cbrush_t) == 48), "ERROR: cbrush_t size is invalid!");

typedef struct CollisionEdge_s
{
	float position[3];
	float normal[3][3];
} CollisionEdge_t;
static_assert((sizeof(CollisionEdge_t) == 48), "ERROR: CollisionEdge_t size is invalid!");

typedef struct CollisionTriangle_s
{
	float position[3];
	float normal[3][3];
	unsigned int vertex_id[3];
	int edge_id[3];
} CollisionTriangle_t;
static_assert((sizeof(CollisionTriangle_t) == 72), "ERROR: CollisionTriangle_t size is invalid!");

typedef struct CollisionPartition
{
	char triCount;
	char borderCount;
	CollisionTriangle_t *triIndices;
	CollisionBorder_t *borders;
} CollisionPartition_t;
static_assert((sizeof(CollisionPartition_t) == 12), "ERROR: CollisionPartition_t size is invalid!");

typedef void DynEntityDef;
typedef void DynEntityPose;
typedef void DynEntityClient;
typedef void DynEntityColl;

typedef struct clipMap_s
{
	const char *name;
	unsigned int numStaticModels;
	cStaticModel_t *staticModelList;
	unsigned int numMaterials;
	dmaterial_t *materials;
	unsigned int numBrushSides;
	cbrushside_t *brushsides;
	unsigned int numNodes;
	cNode_t *nodes;
	unsigned int numLeafs;
	cLeaf_t *leafs;
	unsigned int leafbrushNodesCount;
	cLeafBrushNode_t *leafbrushNodes;
	unsigned int numLeafBrushes;
	uint16_t *leafbrushes;
	unsigned int numLeafSurfaces;
	unsigned int *leafsurfaces;
	unsigned int vertCount;
	float (*verts)[3];
	unsigned int edgeCount;
	CollisionEdge_t *edges;
	int triCount;
	CollisionTriangle_t *triIndices;
	int borderCount;
	CollisionBorder_t *borders;
	int partitionCount;
	CollisionPartition_t *partitions;
	int aabbTreeCount;
	CollisionAabbTree_t *aabbTrees;
	unsigned int numSubModels;
	cmodel_t *cmodels;
	uint16_t numBrushes;
	cbrush_t *brushes;
	int numClusters;
	int clusterBytes;
	byte *visibility;
	int vised;
	int numEntityChars;
	char *entityString;
	cbrush_t *box_brush;
	cmodel_t box_model;
	uint16_t dynEntCount[2];
	DynEntityDef *dynEntDefList[2];
	DynEntityPose *dynEntPoseList[2];
	DynEntityClient *dynEntClientList[2];
	DynEntityColl *dynEntCollList[2];
	unsigned int checksum;
} clipMap_t;
static_assert((sizeof(clipMap_t) == 0x110), "ERROR: clipMap_t size is invalid!");

typedef struct clipMapExtra_s
{
	int planeCount;
	cplane_t *planes;
	struct BspHeader *header;
} clipMapExtra_t;
static_assert((sizeof(clipMapExtra_t) == 0xC), "ERROR: clipMapExtra_t size is invalid!");

struct TraceExtents
{
	vec3_t start;
	vec3_t end;
	vec3_t invDelta;
};

typedef struct TraceThreadInfo
{
	int checkcount;
	int *edges;
	int *verts;
	int *partitions;
	cbrush_t *box_brush;
	cmodel_t *box_model;
} TraceThreadInfo;
static_assert((sizeof(TraceThreadInfo) == 0x18), "ERROR: TraceThreadInfo size is invalid!");

typedef struct traceWork_s
{
	TraceExtents extents;
	vec3_t delta;
	float deltaLen;
	float deltaLenSq;
	vec3_t midpoint;
	vec3_t halfDelta;
	vec3_t halfDeltaAbs;
	vec3_t size;
	vec3_t bounds[2];
	int contents;
	qboolean isPoint;
	qboolean axialCullOnly;
	float offsetZ;
	vec3_t radiusOffset;
	float boundingRadius;
	TraceThreadInfo threadInfo;
} traceWork_t;
static_assert((sizeof(traceWork_t) == 0xB8), "ERROR: traceWork_t size is invalid!");

typedef struct
{
	float fraction;
	vec3_t normal;
	int surfaceFlags;
	int contents;
	const char *material;
	unsigned short entityNum;
	unsigned short entityClassNum;
	byte walkable;
	byte padding;
	byte allsolid;
	byte startsolid;
} trace_t;
static_assert((sizeof(trace_t) == 0x24), "ERROR: trace_t size is invalid!");