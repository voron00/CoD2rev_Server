#pragma once
#include "../xanim/xanim_public.h"

#include <stdint.h>

#define MAX_SUBMODELS           1024
#define BOX_MODEL_HANDLE        MAX_SUBMODELS - 1
#define CAPSULE_MODEL_HANDLE    MAX_SUBMODELS - 1

// to allow boxes to be treated as brush models, we allocate
// some extra indexes along with those needed by the map
#define BOX_BRUSHES         1
#define BOX_LEAF_BRUSHES    2   // ydnar

#define RADIUS_EPSILON 1.0f
#define TRACE_EPSILON 0.001f

// keep 1/8 unit away to keep the position valid before network snapping
// and to avoid various numeric issues
#define SURFACE_CLIP_EPSILON    ( 0.125 )

// plane types are used to speed some tests
// 0-2 are axial planes
#define PLANE_X         0
#define PLANE_Y         1
#define PLANE_Z         2
#define PLANE_NON_AXIAL 3

#define PlaneTypeForNormal( x ) ( x[0] == 1.0 ? PLANE_X : ( x[1] == 1.0 ? PLANE_Y : ( x[2] == 1.0 ? PLANE_Z : PLANE_NON_AXIAL ) ) )

enum LumpType
{
	LUMP_MATERIALS,
	LUMP_LIGHTBYTES,
	LUMP_LIGHTGRIDENTRIES,
	LUMP_LIGHTGRIDCOLORS,
	LUMP_PLANES,
	LUMP_BRUSHSIDES,
	LUMP_BRUSHES,
	LUMP_TRIANGLES,
	LUMP_DRAWVERTS,
	LUMP_DRAWINDICES,
	LUMP_CULLGROUPS,
	LUMP_CULLGROUPINDICES,
	LUMP_OBSOLETE_1,
	LUMP_OBSOLETE_2,
	LUMP_OBSOLETE_3,
	LUMP_OBSOLETE_4,
	LUMP_OBSOLETE_5,
	LUMP_PORTALVERTS,
	LUMP_OCCLUDER,
	LUMP_OCCLUDERPLANES,
	LUMP_OCCLUDEREDGES,
	LUMP_OCCLUDERINDICES,
	LUMP_AABBTREES,
	LUMP_CELLS,
	LUMP_PORTALS,
	LUMP_NODES,
	LUMP_LEAFS,
	LUMP_LEAFBRUSHES,
	LUMP_LEAFSURFACES,
	LUMP_COLLISIONVERTS,
	LUMP_COLLISIONEDGES,
	LUMP_COLLISIONTRIS,
	LUMP_COLLISIONBORDERS,
	LUMP_COLLISIONPARTITIONS,
	LUMP_COLLISIONAABBS,
	LUMP_MODELS,
	LUMP_VISIBILITY,
	LUMP_ENTITIES,
	LUMP_PATHCONNECTIONS,
	LUMP_COUNT
};

struct cStaticModelWritable
{
	unsigned short nextModelInWorldSector;
};

typedef struct cStaticModel_s
{
	cStaticModelWritable writable;
	XModel *xmodel;
	vec3_t origin;
	vec3_t invScaledAxis[3];
	vec3_t absmin;
	vec3_t absmax;
} cStaticModel_t;
#if defined(__i386__)
static_assert((sizeof(cStaticModel_t) == 80), "ERROR: cStaticModel_t size is invalid!");
#endif

typedef struct cbrushside_s
{
	cplane_t *plane;
	int materialNum;
} cbrushside_t;
#if defined(__i386__)
static_assert((sizeof(cbrushside_t) == 8), "ERROR: cbrushside_t size is invalid!");
#endif

typedef struct cNode_s
{
	cplane_t *plane;
	int16_t children[2];
} cNode_t;
#if defined(__i386__)
static_assert((sizeof(cNode_t) == 8), "ERROR: cNode_t size is invalid!");
#endif

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
} cLeaf_t;
#if defined(__i386__)
static_assert((sizeof(cLeaf_t) == 44), "ERROR: cLeaf_t size is invalid!");
#endif

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
	int16_t leafBrushCount;
	int contents;
	cLeafBrushNodeData_t data;
} cLeafBrushNode_t;
#pragma pack(pop)
#if defined(__i386__)
static_assert((sizeof(cLeafBrushNode_t) == 20), "ERROR: cLeafBrushNode_t size is invalid!");
#endif

typedef struct CollisionBorder
{
	float distEq[3];
	float zBase;
	float zSlope;
	float start;
	float length;
} CollisionBorder_t;
#if defined(__i386__)
static_assert((sizeof(CollisionBorder_t) == 28), "ERROR: CollisionBorder_t size is invalid!");
#endif

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
#if defined(__i386__)
static_assert((sizeof(CollisionAabbTree_t) == 32), "ERROR: CollisionAabbTree_t size is invalid!");
#endif

typedef struct cmodel_s
{
	vec3_t mins;
	vec3_t maxs;
	float radius;
	cLeaf_t leaf;
} cmodel_t;
#if defined(__i386__)
static_assert((sizeof(cmodel_t) == 72), "ERROR: cmodel_t size is invalid!");
#endif

typedef struct __attribute__((aligned(16))) cbrush_s
{
	float mins[3];
	int contents;
	float maxs[3];
	int numsides;
	cbrushside_t *sides;
	int16_t axialMaterialNum[2][3];
} cbrush_t;
#if defined(__i386__)
static_assert((sizeof(cbrush_t) == 48), "ERROR: cbrush_t size is invalid!");
#endif

typedef struct CollisionEdge_s
{
	vec3_t origin;
	vec3_t axis[3];
} CollisionEdge_t;
#if defined(__i386__)
static_assert((sizeof(CollisionEdge_t) == 48), "ERROR: CollisionEdge_t size is invalid!");
#endif

typedef struct CollisionTriangle_s
{
	vec4_t plane;
	vec4_t svec;
	vec4_t tvec;
	int32_t verts[3];
	int32_t edges[3];
} CollisionTriangle_t;
#if defined(__i386__)
static_assert((sizeof(CollisionTriangle_t) == 72), "ERROR: CollisionTriangle_t size is invalid!");
#endif

typedef struct CollisionPartition
{
	byte triCount;
	byte borderCount;
	CollisionTriangle_t *tris;
	CollisionBorder_t *borders;
} CollisionPartition_t;
#if defined(__i386__)
static_assert((sizeof(CollisionPartition_t) == 12), "ERROR: CollisionPartition_t size is invalid!");
#endif

typedef struct clipMap_s
{
	char *name;
	int numStaticModels;
	cStaticModel_t *staticModelList;
	int numMaterials;
	dmaterial_t *materials;
	int numBrushSides;
	cbrushside_t *brushsides;
	int numNodes;
	cNode_t *nodes;
	int numLeafs;
	cLeaf_t *leafs;
	int leafbrushNodesCount;
	cLeafBrushNode_t *leafbrushNodes;
	int numLeafBrushes;
	uint16_t *leafbrushes;
	int numLeafSurfaces;
	int *leafsurfaces;
	int vertCount;
	float (*verts)[3];
	int edgeCount;
	CollisionEdge_t *edges;
	int triCount;
	CollisionTriangle_t *tris;
	int borderCount;
	CollisionBorder_t *borders;
	int partitionCount;
	CollisionPartition_t *partitions;
	int aabbTreeCount;
	CollisionAabbTree_t *aabbTrees;
	int numSubModels;
	cmodel_t *cmodels;
	uint16_t numBrushes;
	cbrush_t *brushes;
	int numClusters;
	int clusterBytes;
	byte *visibility;
	qboolean vised;             // if false, visibility is just a single cluster of ffs
	int numEntityChars;
	char *entityString;
	cbrush_t *box_brush;
	cmodel_t box_model;
	int pathNodeCount;
	void *pathNodes;
	int chainNodeCount;
	void *chainNodes;
	void *chainNodesRev;
	int visBytes;
	int pathVis;
	int nodeTreeCount;
	void *nodeTree;
	int planeCount;
	cplane_t *planes;
	struct dheader_t *header;
	unsigned int checksum;
} clipMap_t;
#if defined(__i386__)
static_assert((sizeof(clipMap_t) == 284), "ERROR: clipMap_t size is invalid!"); // original size: 0x110
#endif

typedef struct TraceThreadInfo
{
	int checkcount;
	int32_t *edges;
	int32_t *verts;
	uint16_t *partitions;
	cbrush_t *box_brush;
	cmodel_t *box_model;
} TraceThreadInfo;
#if defined(__i386__)
static_assert((sizeof(TraceThreadInfo) == 0x18), "ERROR: TraceThreadInfo size is invalid!");
#endif

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
	float radius;
	float offsetZ;
	vec3_t radiusOffset;
	TraceThreadInfo threadInfo;
} traceWork_t;
#if defined(__i386__)
static_assert((sizeof(traceWork_t) == 0xB8), "ERROR: traceWork_t size is invalid!");
#endif

typedef struct worldContents_s
{
	int contentsStaticModels;
	int contentsEntities;
	uint16_t entities;
	uint16_t staticModels;
} worldContents_t;

typedef struct worldTree_s
{
	float dist;
	uint16_t axis;
	union
	{
		uint16_t parent;
		uint16_t nextFree;
	};
	uint16_t child[2];
} worldTree_t;

typedef struct worldSector_s
{
	worldContents_t contents;
	worldTree_t tree;
} worldSector_t;

struct locTraceWork_t
{
	int contents;
	TraceExtents extents;
};

struct staticmodeltrace_t
{
	TraceExtents extents;
	int contents;
};

struct pointtrace_t
{
	TraceExtents extents;
	int passEntNum;
	int ignoreEntNum;
	int contentmask;
	int bLocational;
	unsigned char *priorityMap;
};

struct moveclip_t
{
	vec3_t mins;
	vec3_t maxs;
	vec3_t outerSize;
	TraceExtents extents;
	int passEntityNum;
	int passOwnerNum;
	int contentmask;
};

struct sightpointtrace_t
{
	vec3_t start;
	vec3_t end;
	int passEntityNum[2];
	int contentmask;
	int locational;
};

struct sightclip_t
{
	vec3_t mins;
	vec3_t maxs;
	vec3_t outerSize;
	vec3_t start;
	vec3_t end;
	int passEntityNum[2];
	int contentmask;
};

#define	AREA_NODES	1024
struct cm_world_t
{
	float mins[3];
	float maxs[3];
	bool lockTree;
	uint16_t freeHead;
	worldSector_t sectors[AREA_NODES];
};
#if defined(__i386__)
static_assert((sizeof(cm_world_t) == 0x601C), "ERROR: cm_world_t size is invalid!");
#endif

typedef struct leafList_s
{
	int count;
	int maxcount;
	qboolean overflowed;
	int *list;
	vec3_t bounds[2];
	int lastLeaf;
} leafList_t;

typedef struct
{
	int start;
	const float *mins;
	const float *maxs;
	int *list;
	int count;
	int maxcount;
	int contentmask;
} areaParms_t;
#if defined(__i386__)
static_assert((sizeof(areaParms_t) == 0x1C), "ERROR: areaParms_t size is invalid!");
#endif