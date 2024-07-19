#include "qcommon.h"
#include "cm_local.h"

/*
==================
CM_Hunk_AllocXModel
==================
*/
void* CM_Hunk_AllocXModel(int size)
{
	return Hunk_Alloc(size);
}

/*
==================
CM_Hunk_AllocXModelColl
==================
*/
void* CM_Hunk_AllocXModelColl(int size)
{
	return Hunk_Alloc(size);
}

/*
==================
CM_XModelPrecache
==================
*/
XModel* CM_XModelPrecache(const char *name)
{
	return XModelPrecache(name, CM_Hunk_AllocXModel, CM_Hunk_AllocXModelColl);
}

/*
==================
CM_TraceStaticModel
==================
*/
void CM_TraceStaticModel( cStaticModel_s *sm, trace_t *results, const vec3_t start, const vec3_t end, int contentmask )
{
	vec3_t normal;
	vec3_t delta;
	vec3_t localStart;
	vec3_t localEnd;
	DObjAnimMat *pose;

	VectorSubtract(start, sm->origin, delta);
	MatrixTransformVector(delta, sm->invScaledAxis, localStart);

	VectorSubtract(end, sm->origin, delta);
	MatrixTransformVector(delta, sm->invScaledAxis, localEnd);

	pose = XModelGetBasePose(sm->xmodel);

	if ( XModelTraceLine(sm->xmodel, results, pose, localStart, localEnd, contentmask) < 0 )
	{
		return;
	}

	results->entityNum = ENTITYNUM_WORLD;
	MatrixTransposeTransformVector(results->normal, sm->invScaledAxis, normal);
	Vec3Normalize(normal);
	VectorCopy(normal, results->normal);
}

/*
==================
CM_TraceStaticModel
==================
*/
qboolean CM_TraceStaticModelComplete( cStaticModel_s *sm, const vec3_t start, const vec3_t end, int contentmask )
{
	trace_t trace;
	vec3_t delta;
	vec3_t localStart;
	vec3_t localEnd;
	DObjAnimMat *pose;

	VectorSubtract(start, sm->origin, delta);
	MatrixTransformVector(delta, sm->invScaledAxis, localStart);

	VectorSubtract(end, sm->origin, delta);
	MatrixTransformVector(delta, sm->invScaledAxis, localEnd);

	trace.fraction = 1.0;

	pose = XModelGetBasePose(sm->xmodel);

	return XModelTraceLine(sm->xmodel, &trace, pose, localStart, localEnd, contentmask) < 0;
}