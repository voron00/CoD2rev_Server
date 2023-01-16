#pragma once

#include <stdint.h>

typedef struct
{
	char material[64];
	int surfaceFlags;
	int contentFlags;
} dmaterial_t;

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

typedef struct cbrushside_s
{
	cplane_t *plane;
	unsigned int materialNum;
} cbrushside_t;

typedef struct
{
	cplane_t *plane;
	int16_t children[2];
} cNode_t;

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

typedef struct
{
	uint16_t *brushes;
} cLeafBrushNodeLeaf_t;

typedef struct
{
	float dist;
	float range;
	uint16_t childOffset[2];
} cLeafBrushNodeChildren_t;

typedef union
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

typedef struct CollisionBorder
{
	float distEq[3];
	float zBase;
	float zSlope;
	float start;
	float length;
} CollisionBorder_t;

typedef struct CollisionPartition
{
	char triCount;
	char borderCount;
	int firstTri;
	CollisionBorder_t *borders;
} CollisionPartition_t;

typedef union
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

typedef struct cmodel_s
{
	vec3_t mins;
	vec3_t maxs;
	float radius;
	cLeaf_t leaf;
} cmodel_t;

typedef struct __attribute__((aligned(16))) cbrush_t
{
	float mins[3];
	int contents;
	float maxs[3];
	unsigned int numsides;
	cbrushside_t *sides;
	int16_t axialMaterialNum[2][3];
} cbrush_t;

typedef struct
{
	float position[3];
	float normal[3][3];
} CollisionEdge_t;

typedef struct
{
	float normal[3];
	float distance;
	float unknown[8];
	unsigned int vertex_id[3];
	int edge_id[3];
} CollisionTriangle_t;

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