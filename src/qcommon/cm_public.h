#pragma once
#include "cm_local.h"

BspHeader* Com_GetBspHeader(unsigned int *size, unsigned int *checksum);
cmodel_t *CM_ClipHandleToModel( clipHandle_t handle );
bool CM_CullBox(traceWork_t *tw, const float *origin, const float *halfSize);
void CM_MeshTestInLeaf(traceWork_t *tw, cLeaf_s *leaf, trace_t *trace);
void CM_BoxLeafnums_r( leafList_t *ll, int nodenum );
void CM_TestCapsuleInCapsule(traceWork_t *tw, trace_t *trace);
int CM_TraceSphereThroughSphere(traceWork_t *tw, const float *vStart, const float *vEnd, const float *vStationary, float radius, trace_t *trace);
int CM_TraceCylinderThroughCylinder(traceWork_t *tw, const float *vStationary, float fStationaryHalfHeight, float radius, trace_t *trace);
void CM_TraceCapsuleThroughCapsule(traceWork_t *tw, trace_t *trace);
void CM_TraceCapsuleThroughBorder(traceWork_t *tw, CollisionBorder *border, trace_t *trace);
void CM_TraceStaticModel(cStaticModel_s *sm, trace_t *results, const float *start, const float *end, int contentmask);
unsigned int CM_TraceStaticModelComplete(cStaticModel_s *sm, const float *start, const float *end, int contentmask);
int CM_AreaEntities(const vec3_t *mins, const vec3_t *maxs, int *entityList, int maxcount, int contentmask);
void CM_LinkWorld();

#ifdef __cplusplus
extern "C" {
#endif

qboolean QDECL CM_TraceBox(TraceExtents *extents, const float *mins, const float *maxs, float fraction);

#ifdef __cplusplus
}
#endif