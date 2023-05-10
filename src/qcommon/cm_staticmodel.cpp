#include "qcommon.h"
#include "cm_local.h"

void* CM_Hunk_AllocXModel(int size)
{
	return Hunk_Alloc(size);
}

void* CM_Hunk_AllocXModelColl(int size)
{
	return Hunk_Alloc(size);
}

XModel* CM_XModelPrecache(const char *name)
{
	return XModelPrecache(name, CM_Hunk_AllocXModel, CM_Hunk_AllocXModelColl);
}

void CM_TraceStaticModel(cStaticModel_s *sm, trace_t *results, const float *start, const float *end, int contentmask)
{
	vec3_t normal;
	vec3_t position;
	vec3_t localStart;
	vec3_t localEnd;
	DObjAnimMat *pose;

	VectorSubtract(start, sm->origin, position);
	MatrixTransformVector(position, sm->invScaledAxis, localStart);
	VectorSubtract(end, sm->origin, position);
	MatrixTransformVector(position, sm->invScaledAxis, localEnd);

	pose = XModelGetBasePose(sm->xmodel);

	if ( XModelTraceLine(sm->xmodel, results, pose, localStart, localEnd, contentmask) >= 0 )
	{
		results->entityNum = 1022;
		MatrixTransposeTransformVector(results->normal, sm->invScaledAxis, normal);
		Vec3Normalize(normal);
		VectorCopy(normal, results->normal);
	}
}

unsigned int CM_TraceStaticModelComplete(cStaticModel_s *sm, const float *start, const float *end, int contentmask)
{
	trace_t trace;
	vec3_t trans;
	vec3_t localStart;
	vec3_t localEnd;
	DObjAnimMat *pose;

	VectorSubtract(start, sm->origin, trans);
	MatrixTransformVector(trans, sm->invScaledAxis, localStart);
	VectorSubtract(end, sm->origin, trans);
	MatrixTransformVector(trans, sm->invScaledAxis, localEnd);

	trace.fraction = 1.0;
	pose = XModelGetBasePose(sm->xmodel);

	return (unsigned int)XModelTraceLine(sm->xmodel, &trace, pose, localStart, localEnd, contentmask) >> 31;
}