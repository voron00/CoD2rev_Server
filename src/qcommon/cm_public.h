#pragma once
#include "cm_local.h"

extern clipMap_t cm;
extern clipMapExtra_t cme;

void CMod_LoadBrushes();
void CMod_LoadLeafBrushes();
void CMod_LoadCollisionAabbTrees();
void CMod_LoadLeafs(bool usePvs);
void CMod_LoadSubmodels();
int CMod_GetLeafTerrainContents(cLeaf_s *leaf);
double CMod_GetPartitionScore(uint16_t *leafBrushes, int numLeafBrushes, int axis, const float *mins, const float *maxs, float *dist);
void CMod_PartionLeafBrushes(uint16_t *leafBrushes, int numLeafBrushes, cLeaf_s *leaf);
void CMod_LoadLeafBrushNodes();
void CMod_LoadSubmodelBrushNodes();
void CM_InitBoxHull();
void CMod_LoadBrushRelated(bool usePvs);
void CMod_LoadPlanes();
void CMod_LoadMaterials();
void CMod_LoadNodes();
void CMod_LoadLeafSurfaces();
void CMod_LoadCollisionVerts();
void CMod_LoadCollisionEdges();
void CMod_LoadCollisionTriangles();
void CMod_LoadCollisionBorders();
void CMod_LoadCollisionPartitions();
void CMod_LoadVisibility();
void CMod_LoadEntityString();
char *CM_EntityString();
void CM_LoadMapFromBsp(const char *name, bool usePvs);

BspHeader* Com_GetBspHeader(unsigned int *size, unsigned int *checksum);
cmodel_t *CM_ClipHandleToModel( clipHandle_t handle );
void CM_SetAxialCullOnly(traceWork_t *tw);
bool CM_CullBox(traceWork_t *tw, const float *origin, const float *halfSize);
void CM_MeshTestInLeaf(traceWork_t *tw, cLeaf_s *leaf, trace_t *trace);
void CM_BoxLeafnums_r( leafList_t *ll, int nodenum );
void CM_TraceThroughAabbTree_r(traceWork_t *tw, CollisionAabbTree_s *aabbTree, trace_t *trace);
int CM_TestInLeafBrushNode(traceWork_t *tw, cLeaf_s *leaf, trace_t *trace);
void CM_TraceStaticModel(cStaticModel_s *sm, trace_t *results, const float *start, const float *end, int contentmask);
unsigned int CM_TraceStaticModelComplete(cStaticModel_s *sm, const float *start, const float *end, int contentmask);
int CM_PointTraceStaticModelsComplete(const float *start, const float *end, int contentmask);
void CM_PointTraceStaticModels(trace_t *results, const float *start, const float *end, int contentmask);
int CM_PointSightTraceToEntities(sightpointtrace_t *clip);
int CM_ClipSightTraceToEntities(sightclip_t *clip);
int CM_AreaEntities(const float *mins, const float *maxs, int *entityList, int maxcount, int contentmask);
void CM_PointTraceToEntities(pointtrace_t *clip, trace_t *trace);
void CM_TraceThroughAabbTree(traceWork_t *tw, CollisionAabbTree_s *aabbTree, trace_t *trace);
void CM_TestInLeaf(traceWork_t *tw, cLeaf_s *leaf, trace_t *trace);
void CM_ClipMoveToEntities(moveclip_t *clip, trace_t *trace);
int CM_TransformedPointContents( const vec3_t p, clipHandle_t model, const vec3_t origin, const vec3_t angles );
void CM_SightTraceThroughAabbTree(traceWork_t *tw, CollisionAabbTree_s *aabbTree, trace_t *trace);
void CM_PositionTestInAabbTree_r(traceWork_t *tw, CollisionAabbTree_s *aabbTree, trace_t *trace);
int CM_TransformedBoxSightTrace(int hitNum, const float *start, const float *end, const float *mins, const float *maxs, unsigned int model, int brushmask, const float *origin, const float *angles);
int CM_BoxSightTrace(int oldHitNum, const float *start, const float *end, const float *mins, const float *maxs, unsigned int model, int brushmask);
void CM_Trace(trace_t *results, const float *start, const float *end, const float *mins, const float *maxs, clipHandle_t model, int brushmask);
void CM_BoxTrace(trace_t *results, const float *start, const float *end, const float *mins, const float *maxs, clipHandle_t model, int brushmask);
void CM_TransformedBoxTrace(trace_t *results, const float *start, const float *end, const float *mins, const float *maxs, unsigned int model, int brushmask, const float *origin, const float *angles);
void CM_TransformedBoxTraceExternal(trace_t *results, const float *start, const float *end, const float *mins, const float *maxs, unsigned int model, int brushmask, const float *origin, const float *angles);
int CM_SightTraceThroughLeafBrushNode(traceWork_t *tw, cLeaf_s *leaf);
void CM_PointTraceStaticModels_r(locTraceWork_t *tw, unsigned short nodeIndex, const float *p1_, const float *p2, trace_t *trace);
int CM_PointTraceStaticModelsComplete_r(staticmodeltrace_t *clip, unsigned short nodeIndex, const float *p1_, const float *p2);
bool CM_TraceThroughLeafBrushNode(traceWork_t *tw, cLeaf_s *leaf, trace_t *trace);
cLeafBrushNode_s * CMod_PartionLeafBrushes_r(uint16_t *leafBrushes, int numLeafBrushes, const float *mins, const float *maxs);
void CM_TraceThroughLeafBrushNode_r(traceWork_t *tw, cLeafBrushNode_s *node, const float *p1_, const float *p2, trace_t *trace);
void CM_TestInLeafBrushNode_r(traceWork_t *tw, cLeafBrushNode_s *node, trace_t *trace);
int CM_PointContentsLeafBrushNode_r(const float *p, cLeafBrushNode_s *node);
int CM_SightTraceThroughLeafBrushNode_r(traceWork_t *tw, cLeafBrushNode_s *remoteNode, const float *p1_, const float *p2);
int CM_SightTraceThroughLeaf(traceWork_t *tw, cLeaf_s *leaf, trace_t *trace);
void CM_TestCapsuleInCapsule(traceWork_t *tw, trace_t *trace);
int CM_SightTraceThroughBrush(traceWork_t *tw, cbrush_t *brush);
int CM_SightTraceThroughTree(traceWork_t *tw, int num, const float *p1_, const float *p2, trace_t *trace);
void CM_TraceCapsuleThroughCapsule(traceWork_t *tw, trace_t *trace);
void CM_TraceThroughLeaf(traceWork_t *tw, cLeaf_s *leaf, trace_t *trace);
void CM_TraceThroughTree(traceWork_t *tw, int num, const float *p1_, const float *p2, trace_t *trace);
void CM_PositionTest(traceWork_t *tw, trace_t *trace);
int CM_PointContents(const float *p, unsigned int model);
void CM_StoreLeafs( leafList_t *ll, int nodenum );
int CM_PointLeafnum_r( const vec3_t p, int num );
void CM_SortNode(unsigned short nodeIndex, float *mins, float *maxs);
void CM_AddStaticModelToNode(cStaticModel_s *model, unsigned short childNodeIndex);
void CM_LinkAllStaticModels();
void CM_LinkWorld();

#ifdef __cplusplus
extern "C" {
#endif

qboolean QDECL CM_TraceBox(TraceExtents *extents, const float *mins, const float *maxs, float fraction);
void QDECL CM_PositionTestCapsuleInTriangle(traceWork_t *tw, CollisionTriangle_s *collTtris, trace_t *trace);
void QDECL CM_TraceCapsuleThroughTriangle(traceWork_t *tw, CollisionTriangle_s *tri, float offsetZ, trace_t *trace);
void QDECL CM_TraceCapsuleThroughBorder(traceWork_t *tw, CollisionBorder *border, trace_t *trace);
void QDECL CM_TraceThroughBrush(traceWork_t *tw, cbrush_s *brush, trace_t *trace);
void QDECL CM_TestBoxInBrush(traceWork_t *tw, cbrush_s *brush, trace_t *trace);
void QDECL CM_TracePointThroughTriangle(traceWork_t *tw, CollisionTriangle_s *tri, trace_t *trace);
int QDECL CM_TraceSphereThroughSphere(traceWork_t *tw, const float *vStart, const float *vEnd, const float *vStationary, float radius, trace_t *trace);
int QDECL CM_TraceCylinderThroughCylinder(traceWork_t *tw, const float *vStationary, float fStationaryHalfHeight, float radius, trace_t *trace);

#ifdef __cplusplus
}
#endif