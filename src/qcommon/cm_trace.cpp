#include "qcommon.h"
#include "cm_local.h"
#include "sys_thread.h"

/*
==================
CM_GetBox
==================
*/
void CM_GetBox( cbrush_t **box_brush, cmodel_t **box_model )
{
	TraceThreadInfo *value;

	value = (TraceThreadInfo *)Sys_GetValue(THREAD_VALUE_TRACE);

	*box_brush = value->box_brush;
	*box_model = value->box_model;
}

/*
==================
CM_GetTrackThreadInfo
==================
*/
void CM_GetTrackThreadInfo( TraceThreadInfo *threadInfo )
{
	TraceThreadInfo *value;

	value = (TraceThreadInfo *)Sys_GetValue(THREAD_VALUE_TRACE);
	value->checkcount++;

	*threadInfo = *value;
}

/*
==================
CM_ClipHandleToModel
==================
*/
cmodel_t *CM_ClipHandleToModel( clipHandle_t handle )
{
	cmodel_t *box_model;
	cbrush_t *box_brush;

	if ( handle < 0 )
	{
		Com_Error( ERR_DROP, "CM_ClipHandleToModel: bad handle %i", handle );
	}

	if ( handle < cm.numSubModels )
	{
		return &cm.cmodels[handle];
	}

	CM_GetBox(&box_brush, &box_model);

	return box_model;
}

/*
===================
CM_TempBoxModel

To keep everything totally uniform, bounding boxes are turned into small
BSP trees instead of being compared directly.
Capsules are handled differently though.
===================
*/
clipHandle_t CM_TempBoxModel( const vec3_t mins, const vec3_t maxs, int contents )
{
	cmodel_t *box_model;
	cbrush_t *box_brush;

	CM_GetBox(&box_brush, &box_model);

	VectorCopy(mins, box_model->mins);
	VectorCopy(maxs, box_model->maxs);

	VectorCopy(mins, box_brush->mins);
	VectorCopy(maxs, box_brush->maxs);

// DHM - Nerve
	box_brush->contents = contents;
// dhm

	return BOX_MODEL_HANDLE;
}

/*
==================
CM_RadiusOfModel
==================
*/
float CM_RadiusOfModel( clipHandle_t handle )
{
	return CM_ClipHandleToModel(handle)->radius;
}

/*
==================
CM_ContentsOfModel
==================
*/
int CM_ContentsOfModel( clipHandle_t handle )
{
	cmodel_t *cmod = CM_ClipHandleToModel(handle);

	return cmod->leaf.brushContents | cmod->leaf.terrainContents;
}

/*
==================
CM_BoxSightTrace
==================
*/
int CM_BoxSightTrace( int oldHitNum, const vec3_t start, const vec3_t end, const vec3_t mins, const vec3_t maxs, clipHandle_t model, int brushmask )
{
	trace_t trace;
	cmodel_t *cmod;
	vec3_t offset;
	traceWork_t tw;
	int i;

	assert(cm.numNodes);
	assert(mins);
	assert(maxs);

	cmod = CM_ClipHandleToModel(model);

	trace.fraction = 1.0;
	trace.startsolid = 0;
	trace.allsolid = 0;

	// set basic parms
	tw.contents = brushmask;

	// adjust so that mins and maxs are always symetric, which
	// avoids some complications with plane expanding of rotated
	// bmodels
	for ( i = 0; i < 3; i++ )
	{
		assert(maxs[i] >= mins[i]);

		offset[i] = (mins[i] + maxs[i]) * 0.5;
		tw.size[i] = maxs[i] - offset[i];
		tw.extents.start[i] = start[i] + offset[i];
		tw.extents.end[i] = end[i] + offset[i];
		tw.midpoint[i] = (tw.extents.start[i] + tw.extents.end[i]) * 0.5;
		tw.delta[i] = tw.extents.end[i] - tw.extents.start[i];
		tw.halfDelta[i] = tw.delta[i] * 0.5;
		tw.halfDeltaAbs[i] = fabs(tw.halfDelta[i]);
	}

	CM_CalcTraceEntents(&tw.extents);

	tw.deltaLenSq = VectorLengthSquared(tw.delta);
	tw.deltaLen = sqrt(tw.deltaLenSq);

	if ( tw.size[0] > tw.size[2] )
		tw.radius = tw.size[2];
	else
		tw.radius = tw.size[0];

	tw.offsetZ = tw.size[2] - tw.radius;

	// calculate bounds
	for ( i = 0; i < 2; i++ )
	{
		if ( tw.extents.end[i] > tw.extents.start[i] )
		{
			tw.bounds[0][i] = tw.extents.start[i] - tw.radius;
			tw.bounds[1][i] = tw.extents.end[i] + tw.radius;
		}
		else
		{
			tw.bounds[0][i] = tw.extents.end[i] - tw.radius;
			tw.bounds[1][i] = tw.extents.start[i] + tw.radius;
		}
	}

	assert(tw.offsetZ >= 0);

	if ( tw.extents.end[2] > tw.extents.start[2] )
	{
		tw.bounds[0][2] = tw.extents.start[2] - tw.offsetZ - tw.radius;
		tw.bounds[1][2] = tw.extents.end[2] + tw.offsetZ + tw.radius;
	}
	else
	{
		tw.bounds[0][2] = tw.extents.end[2] - tw.offsetZ - tw.radius;
		tw.bounds[1][2] = tw.extents.start[2] + tw.offsetZ + tw.radius;
	}

	CM_SetAxialCullOnly(&tw);

	assert(tw.size[0] >= 0);
	assert(tw.size[1] >= 0);
	assert(tw.size[2] >= 0);

	// check for point special case
	tw.isPoint = 0.0 == tw.size[0] + tw.size[1] + tw.size[2];

	assert(tw.offsetZ >= 0);

	tw.radiusOffset[0] = tw.radius;
	tw.radiusOffset[1] = tw.radius;
	tw.radiusOffset[2] = tw.radius + tw.offsetZ;

	CM_GetTrackThreadInfo(&tw.threadInfo);

	//
	// general sweeping through world
	//
	if ( model )
	{
		if ( model == CAPSULE_MODEL_HANDLE )
		{
			if ( (tw.contents & tw.threadInfo.box_brush->contents) )
				return CM_SightTraceCapsuleThroughCapsule(&tw, &trace);
			else
				return 0;
		}
		else
		{
			return CM_SightTraceThroughLeaf(&tw, &cmod->leaf, &trace);
		}
	}
	else
	{
		int hitNum = 0;

		if ( oldHitNum > 0 )
		{
			if ( oldHitNum - 1 < cm.numBrushes )
				hitNum = CM_SightTraceThroughBrush(&tw, &cm.brushes[oldHitNum - 1]);
		}

		if ( hitNum )
			return hitNum;
		else
			return CM_SightTraceThroughTree(&tw, 0, tw.extents.start, tw.extents.end, &trace);
	}
}


/*
==================
CM_TransformedBoxTrace

Handles offseting and rotation of the end points for moving and
rotating entities
==================
*/
void CM_TransformedBoxTrace( trace_t *results, const vec3_t start, const vec3_t end,
                             const vec3_t mins, const vec3_t maxs,
                             clipHandle_t model, int brushmask,
                             const vec3_t origin, const vec3_t angles )
{
	vec3_t start_l, end_l;
	qboolean rotated;
	vec3_t offset;
	vec3_t symetricSize[2];
	vec3_t matrix[3], transpose[3];
	int i;

	assert(mins);
	assert(maxs);

	// adjust so that mins and maxs are always symetric, which
	// avoids some complications with plane expanding of rotated
	// bmodels
	for ( i = 0 ; i < 3 ; i++ )
	{
		offset[i] = ( mins[i] + maxs[i] ) * 0.5;
		symetricSize[0][i] = mins[i] - offset[i];
		symetricSize[1][i] = maxs[i] - offset[i];
		start_l[i] = start[i] + offset[i];
		end_l[i] = end[i] + offset[i];
	}

	// subtract origin offset
	VectorSubtract( start_l, origin, start_l );
	VectorSubtract( end_l, origin, end_l );

	// rotate start and end into the models frame of reference
	rotated = qfalse;
	if ( ( angles[0] || angles[1] || angles[2] ) )
	{
		rotated = qtrue;
	}

	if ( rotated )
	{
		// rotation on trace line (start-end) instead of rotating the bmodel
		// NOTE: This is still incorrect for bounding boxes because the actual bounding
		//		 box that is swept through the model is not rotated. We cannot rotate
		//		 the bounding box or the bmodel because that would make all the brush
		//		 bevels invalid.
		//		 However this is correct for capsules since a capsule itself is rotated too.
		CreateRotationMatrix( angles, matrix );
		RotatePoint( start_l, matrix );
		RotatePoint( end_l, matrix );
	}

	float oldFraction = results->fraction;

	// sweep the box through the model
	CM_Trace( results, start_l, end_l, symetricSize[0], symetricSize[1], model, brushmask );

	// if the bmodel was rotated and there was a collision
	if ( rotated )
	{
		if ( oldFraction > results->fraction )
		{
			// rotation of bmodel collision plane
			TransposeMatrix( matrix, transpose );
			RotatePoint( results->normal, transpose );
		}
	}
}

/*
==================
CM_BoxTrace
==================
*/
void CM_BoxTrace( trace_t *results, const vec3_t start, const vec3_t end,
                  const vec3_t mins, const vec3_t maxs,
                  clipHandle_t model, int brushmask )
{
	memset(results, 0, sizeof(trace_t));
	results->fraction = 1.0;
	CM_Trace(results, start, end, mins, maxs, model, brushmask);
}

/*
==================
CM_TransformedBoxSightTrace
==================
*/
int CM_TransformedBoxSightTrace( int hitNum, const vec3_t start, const vec3_t end,
                                 const vec3_t mins, const vec3_t maxs,
                                 clipHandle_t model, int brushmask,
                                 const vec3_t origin, const vec3_t angles )
{
	vec3_t start_l, end_l;
	qboolean rotated;
	vec3_t offset;
	vec3_t symetricSize[2];
	vec3_t matrix[3], transpose[3];
	int i;

	assert(mins);
	assert(maxs);

	// adjust so that mins and maxs are always symetric, which
	// avoids some complications with plane expanding of rotated
	// bmodels
	for ( i = 0 ; i < 3 ; i++ )
	{
		offset[i] = ( mins[i] + maxs[i] ) * 0.5;
		symetricSize[0][i] = mins[i] - offset[i];
		symetricSize[1][i] = maxs[i] - offset[i];
		start_l[i] = start[i] + offset[i];
		end_l[i] = end[i] + offset[i];
	}

	// subtract origin offset
	VectorSubtract( start_l, origin, start_l );
	VectorSubtract( end_l, origin, end_l );

	// rotate start and end into the models frame of reference
	rotated = qfalse;
	if ( ( angles[0] || angles[1] || angles[2] ) )
	{
		rotated = qtrue;
	}

	if ( rotated )
	{
		// rotation on trace line (start-end) instead of rotating the bmodel
		// NOTE: This is still incorrect for bounding boxes because the actual bounding
		//		 box that is swept through the model is not rotated. We cannot rotate
		//		 the bounding box or the bmodel because that would make all the brush
		//		 bevels invalid.
		//		 However this is correct for capsules since a capsule itself is rotated too.
		CreateRotationMatrix( angles, matrix );
		RotatePoint( start_l, matrix );
		RotatePoint( end_l, matrix );
	}

	return CM_BoxSightTrace( hitNum, start_l, end_l, symetricSize[0], symetricSize[1], model, brushmask );
}

/*
==================
CM_TransformedBoxTraceExternal
==================
*/
void CM_TransformedBoxTraceExternal( trace_t *results, const vec3_t start, const vec3_t end,
                                     const vec3_t mins, const vec3_t maxs,
                                     clipHandle_t model, int brushmask,
                                     const vec3_t origin, const vec3_t angles )
{
	memset(results, 0, sizeof(trace_t));
	results->fraction = 1.0;
	CM_TransformedBoxTrace(results, start, end, mins, maxs, model, brushmask, origin, angles);
}

/*
==================
CM_SightTraceSphereThroughSphere
==================
*/
qboolean CM_SightTraceSphereThroughSphere( traceWork_t *tw, const vec3_t vStart, const vec3_t vEnd,
        const vec3_t vStationary, float radius, trace_t *trace )
{
	vec3_t vNormal;
	vec_t fDeltaLen;
	float fDiscriminant;
	float fC;
	float fB;
	float fA;
	vec3_t vDelta;

	VectorSubtract(vStart, vStationary, vDelta);

	fC = DotProduct(vDelta, vDelta) - Square(radius + tw->radius);

	if ( fC <= 0 )
	{
		return qfalse;
	}

	fB = DotProduct(tw->delta, vDelta);

	if ( fB >= 0 )
	{
		return qtrue;
	}

	fA = tw->deltaLenSq;

	fDiscriminant = Square(fB) - fA * fC;

	if ( fDiscriminant < 0 )
	{
		return qtrue;
	}

	fDeltaLen = Vec3NormalizeTo(vDelta, vNormal);

	return (-fB - sqrt(fDiscriminant)) / fA + fB * SURFACE_CLIP_EPSILON / fDeltaLen >= trace->fraction;
}

/*
==================
CM_SetAxialCullOnly
==================
*/
void CM_SetAxialCullOnly( traceWork_t *tw )
{
	vec3_t d;
	float c;
	float s;

	VectorSubtract(tw->bounds[1], tw->bounds[0], d);

	s = tw->size[1] * tw->size[0] * tw->size[2];
	c = s * 16.0 * tw->deltaLen;
	tw->axialCullOnly = d[0] * d[1] * d[2] < c;
}

/*
==================
CM_TraceSphereThroughSphere
==================
*/
qboolean CM_TraceSphereThroughSphere( traceWork_t *tw, const vec3_t vStart, const vec3_t vEnd,
                                      const vec3_t vStationary, float radius, trace_t *trace )
{
	vec3_t vNormal;
	vec_t fDeltaLen;
	float fEntry;
	float fRadiusSqrd;
	float fDiscriminant;
	float fC;
	float fB;
	float fA;
	vec3_t vDelta;

	VectorSubtract(vStart, vStationary, vDelta);
	fRadiusSqrd = Square(radius + tw->radius);
	fC = DotProduct(vDelta, vDelta) - fRadiusSqrd;

	if ( fC <= 0 )
	{
		trace->fraction = 0;
		trace->startsolid = 1;

		Vec3NormalizeTo(vDelta, trace->normal);
		trace->contents = tw->threadInfo.box_brush->contents;
		VectorSubtract(vEnd, vStationary, vDelta);

		if ( fRadiusSqrd >= VectorLengthSquared(vDelta) )
		{
			trace->allsolid = 1;
		}

		return qfalse;
	}

	fB = DotProduct(tw->delta, vDelta);

	if ( fB >= 0 )
	{
		return qtrue;
	}

	fA = tw->deltaLenSq;
	assert(fA > 0.0f);
	fDiscriminant = Square(fB) - fA * fC;

	if ( fDiscriminant < 0 )
	{
		return qtrue;
	}

	fDeltaLen = Vec3NormalizeTo(vDelta, vNormal);
	fEntry = (-fB - sqrt(fDiscriminant)) / fA + fDeltaLen * SURFACE_CLIP_EPSILON / fB;

	if ( trace->fraction > fEntry )
	{
		trace->fraction = I_fmax(fEntry, 0);
		assert(trace->fraction >= 0 && trace->fraction <= 1.0f);
		VectorCopy(vNormal, trace->normal);
		trace->contents = tw->threadInfo.box_brush->contents;

		return qfalse;
	}

	return qtrue;
}

/*
==================
CM_SightTraceCylinderThroughCylinder
==================
*/
qboolean CM_SightTraceCylinderThroughCylinder( traceWork_t *tw, const vec3_t vStationary, float fStationaryHalfHeight, float radius, trace_t *trace )
{
	float fHitHeight;
	float fTotalHeight;
	vec3_t vNormal;
	vec_t fDeltaLen;
	float fEpsilon;
	float fEntry;
	float fDiscriminant;
	float fC;
	float fB;
	float fA;
	vec3_t vDelta;

	VectorSubtract(tw->extents.start, vStationary, vDelta);
	fC = Dot2Product(vDelta, vDelta) - Square(radius + tw->radius);

	if ( fC <= 0 )
	{
		fTotalHeight = tw->size[2] - tw->radius + fStationaryHalfHeight;
		assert(fTotalHeight >= 0);

		return fabs(vDelta[2]) > fTotalHeight;
	}

	fB = Dot2Product(tw->delta, vDelta);

	if ( fB >= 0 )
	{
		return qtrue;
	}

	fA = tw->deltaLenSq;
	fDiscriminant = Square(fB) - fA * fC;

	if ( fDiscriminant < 0 )
	{
		return qtrue;
	}

	vDelta[2] = 0.0;
	fDeltaLen = Vec3NormalizeTo(vDelta, vNormal);
	fEpsilon = fB * SURFACE_CLIP_EPSILON / fDeltaLen;
	fEntry = (-fB - sqrt(fDiscriminant)) / fA + fEpsilon;

	if ( fEntry >= trace->fraction )
	{
		return qtrue;
	}

	fTotalHeight = tw->size[2] - tw->radius + fStationaryHalfHeight;
	fHitHeight = (fEntry - fEpsilon) * tw->delta[2] + tw->extents.start[2] - vStationary[2];
	assert(fTotalHeight >= 0);

	return fabs(fHitHeight) > fTotalHeight;
}

/*
==================
CM_SightTraceThroughBrush
==================
*/
int CM_SightTraceThroughBrush( traceWork_t *tw, cbrush_t *brush )
{
	int index;
	float *bounds;
	float sign;
	int i;
	float frac;
	cbrushside_t *side;
	float d2;
	float d1;
	float delta;
	float leaveFrac;
	float enterFrac;
	float dist;
	int j;

	assert(!IS_NAN((tw->extents.start)[0]) && !IS_NAN((tw->extents.start)[1]) && !IS_NAN((tw->extents.start)[2]));
	assert(!IS_NAN((tw->extents.end)[0]) && !IS_NAN((tw->extents.end)[1]) && !IS_NAN((tw->extents.end)[2]));

	enterFrac = 0.0;
	leaveFrac = 1.0;

	sign = -1.0;
	bounds = brush->mins;

	for ( index = 0; ; index = 1 )
	{
		assert(!IS_NAN((bounds)[0]) && !IS_NAN((bounds)[1]) && !IS_NAN((bounds)[2]));
		assert(!IS_NAN((tw->radiusOffset)[0]) && !IS_NAN((tw->radiusOffset)[1]) && !IS_NAN((tw->radiusOffset)[2]));

		for ( i = 0; i < 3; i++ )
		{
			d1 = (tw->extents.start[i] - bounds[i]) * sign - tw->radiusOffset[i];
			d2 = (tw->extents.end[i] - bounds[i]) * sign - tw->radiusOffset[i];

			assert(!IS_NAN(d1));
			assert(!IS_NAN(d2));

			if ( d1 > 0 )
			{
				if ( d2 > 0 )
				{
					return 0;
				}

				frac = d1 * tw->extents.invDelta[i] * sign;

				if ( frac >= leaveFrac )
				{
					return 0;
				}

				enterFrac = I_fmax(enterFrac, frac);
				continue;
			}

			if ( d2 > 0 )
			{
				frac = d1 * tw->extents.invDelta[i] * sign;

				if ( enterFrac >= frac )
				{
					return 0;
				}

				leaveFrac = I_fmin(leaveFrac, frac);
				continue;
			}
		}

		if ( index )
		{
			break;
		}

		sign = 1.0;
		bounds = brush->maxs;
	}

	side = brush->sides;
	assert(brush->numsides >= 0);

	for ( j = brush->numsides; j; j--, side++ )
	{
		assert(!IS_NAN(side->plane->dist));
		assert(!IS_NAN(tw->radius));
		assert(!IS_NAN((side->plane->normal)[0]) && !IS_NAN((side->plane->normal)[1]) && !IS_NAN((side->plane->normal)[2]));
		assert(!IS_NAN(tw->offsetZ));

		dist = side->plane->normal[3] + tw->radius + fabs(side->plane->normal[2] * tw->offsetZ);

		assert(!IS_NAN(dist));

		d1 = DotProduct(tw->extents.start, side->plane->normal) - dist;
		d2 = DotProduct(tw->extents.end, side->plane->normal) - dist;

		assert(!IS_NAN(d1));
		assert(!IS_NAN(d2));

		if ( d1 > 0 )
		{
			delta = d1 - d2;
			assert(!IS_NAN(delta));

			if ( d2 > 0 )
			{
				return 0;
			}

			assert(delta > 0);

			if ( d1 > enterFrac * delta )
			{
				enterFrac = d1 / delta;

				if ( enterFrac >= leaveFrac )
				{
					return 0;
				}
			}

			continue;
		}

		if ( d2 > 0 )
		{
			delta = d1 - d2;
			assert(delta < 0);

			if ( d1 > leaveFrac * delta )
			{
				leaveFrac = d1 / delta;

				if ( enterFrac >= leaveFrac )
				{
					return 0;
				}
			}

			continue;
		}
	}

	return brush - cm.brushes + 1;
}

/*
==================
CM_TraceCylinderThroughCylinder
==================
*/
qboolean CM_TraceCylinderThroughCylinder( traceWork_t *tw, const vec3_t vStationary, float fStationaryHalfHeight, float radius, trace_t *trace )
{
	float fHitHeight;
	float fTotalHeight;
	vec3_t vNormal;
	vec_t fDeltaLen;
	float fEpsilon;
	float fEntry;
	float fDiscriminant;
	float fC;
	float fB;
	float fA;
	vec3_t vDelta;

	VectorSubtract(tw->extents.start, vStationary, vDelta);
	fC = Dot2Product(vDelta, vDelta) - Square(radius + tw->radius);

	if ( fC <= 0 )
	{
		fTotalHeight = tw->size[2] - tw->radius + fStationaryHalfHeight;
		assert(fTotalHeight >= 0);

		if ( fabs(vDelta[2]) > fTotalHeight )
		{
			return qtrue;
		}

		trace->fraction = 0.0;
		trace->startsolid = 1;

		vDelta[2] = 0.0;
		Vec3NormalizeTo(vDelta, trace->normal);
		trace->contents = tw->threadInfo.box_brush->contents;
		VectorSubtract(tw->extents.end, vStationary, vDelta);
		assert(fTotalHeight >= 0);

		if ( fTotalHeight >= fabs(vDelta[2]) )
		{
			trace->allsolid = 1;
		}

		return qfalse;
	}

	fB = Dot2Product(tw->delta, vDelta);

	if ( fB >= 0 )
	{
		return qtrue;
	}

	fA = Dot2Product(tw->delta, tw->delta);
	assert(fA > 0.0f);
	fDiscriminant = Square(fB) - fA * fC;

	if ( fDiscriminant < 0 )
	{
		return qtrue;
	}

	vDelta[2] = 0.0;
	fDeltaLen = Vec3NormalizeTo(vDelta, vNormal);
	fEpsilon = fDeltaLen * SURFACE_CLIP_EPSILON / fB;
	fEntry = (-fB - sqrt(fDiscriminant)) / fA + fEpsilon;

	if ( trace->fraction > fEntry )
	{
		fTotalHeight = tw->size[2] - tw->radius + fStationaryHalfHeight;
		assert(fTotalHeight >= 0);
		fHitHeight = (fEntry - fEpsilon) * tw->delta[2] + tw->extents.start[2] - vStationary[2];

		if ( fabs(fHitHeight) > fTotalHeight )
		{
			return qtrue;
		}

		trace->fraction = I_fmax(fEntry, 0.0);
		assert(trace->fraction >= 0 && trace->fraction <= 1.0f);
		VectorCopy(vNormal, trace->normal);
		trace->contents = tw->threadInfo.box_brush->contents;

		return qfalse;
	}

	return qtrue;
}

/*
==================
CM_TraceThroughBrush
==================
*/
void CM_TraceThroughBrush( traceWork_t *tw, cbrush_t *brush, trace_t *trace )
{
	float frac;
	int index;
	cplane_t axialPlane;
	cbrushside_t axialSide;
	int j;
	float *bounds;
	float sign;
	float delta;
	cbrushside_t *leadside;
	cbrushside_t *side;
	float f;
	qboolean allsolid;
	float d2;
	float d1;
	float leaveFrac;
	float enterFrac;
	float dist;
	int numsides;

	assert(!IS_NAN((tw->extents.start)[0]) && !IS_NAN((tw->extents.start)[1]) && !IS_NAN((tw->extents.start)[2]));
	assert(!IS_NAN((tw->extents.end)[0]) && !IS_NAN((tw->extents.end)[1]) && !IS_NAN((tw->extents.end)[2]));

	enterFrac = 0.0;
	leaveFrac = trace->fraction;

	allsolid = qtrue;
	leadside = NULL;

	sign = -1.0;
	bounds = brush->mins;
	index = 0;

	while ( 2 )
	{
		//
		// compare the trace against all planes of the brush
		// find the latest time the trace crosses a plane towards the interior
		// and the earliest time the trace crosses a plane towards the exterior
		//

		assert(!IS_NAN((bounds)[0]) && !IS_NAN((bounds)[1]) && !IS_NAN((bounds)[2]));
		assert(!IS_NAN((tw->radiusOffset)[0]) && !IS_NAN((tw->radiusOffset)[1]) && !IS_NAN((tw->radiusOffset)[2]));

		for ( j = 0; j < 3; j++ )
		{
			d1 = (tw->extents.start[j] - bounds[j]) * sign - tw->radiusOffset[j];
			d2 = (tw->extents.end[j] - bounds[j]) * sign - tw->radiusOffset[j];

			assert(!IS_NAN(d1));
			assert(!IS_NAN(d2));

			if ( d1 > 0 )
			{
				if ( d2 >= I_fmin(d1, SURFACE_CLIP_EPSILON) )
				{
					return;
				}

				frac = (d1 - SURFACE_CLIP_EPSILON) * tw->extents.invDelta[j] * sign;

				if ( frac >= leaveFrac )
				{
					return;
				}

				if ( d2 > 0 )
				{
					allsolid = qfalse;
				}

				if ( frac > enterFrac )
				{
					enterFrac = frac;
				}
				else
				{
					if ( leadside )
					{
						continue;
					}
				}

				axialSide.materialNum = brush->axialMaterialNum[index][j];
				VectorClear(axialPlane.normal);
				axialPlane.normal[j] = sign;
				axialSide.plane = &axialPlane;
				leadside = &axialSide;
			}
			else
			{
				if ( d2 > 0 )
				{
					frac = d1 * tw->extents.invDelta[j] * sign;

					if ( enterFrac >= frac )
					{
						return;
					}

					allsolid = qfalse;
					leaveFrac = I_fmin(leaveFrac, frac);
				}
			}
		}

		if ( !index )
		{
			sign = 1.0;
			bounds = brush->maxs;
			index = 1;
			continue;
		}

		break;
	}

	side = brush->sides;
	numsides = brush->numsides;

	while ( 2 )
	{
		//
		// compare the trace against all planes of the brush
		// find the latest time the trace crosses a plane towards the interior
		// and the earliest time the trace crosses a plane towards the exterior
		//

		if ( !numsides )
		{
			break;
		}

		assert(!IS_NAN(side->plane->dist));
		assert(!IS_NAN(tw->radius));
		assert(!IS_NAN((side->plane->normal)[0]) && !IS_NAN((side->plane->normal)[1]) && !IS_NAN((side->plane->normal)[2]));
		assert(!IS_NAN(tw->offsetZ));

		// adjust the plane distance apropriately for radius
		dist = side->plane->dist + tw->radius + fabs(side->plane->normal[2] * tw->offsetZ);
		assert(!IS_NAN(dist));

		d1 = DotProduct(tw->extents.start, side->plane->normal) - dist;
		d2 = DotProduct(tw->extents.end, side->plane->normal) - dist;

		assert(!IS_NAN(d1));
		assert(!IS_NAN(d2));

		if ( d1 > 0 )
		{
			// if completely in front of face, no intersection with the entire brush
			if ( d2 >= I_fmin(d1, SURFACE_CLIP_EPSILON) )
			{
				return;
			}

			if ( d2 > 0 )
			{
				allsolid = qfalse;
			}

			delta = d1 - d2;

			assert(!IS_NAN(delta));
			assert(delta > 0);

			f = d1 - SURFACE_CLIP_EPSILON;

			if ( f > enterFrac * delta )
			{
				enterFrac = f / delta;

				if ( enterFrac >= leaveFrac )
				{
					return;
				}

				leadside = side;
			}
			else
			{
				if ( !leadside )
				{
					leadside = side;
				}
			}
		}
		else
		{
			if ( d2 > 0 )
			{
				delta = d1 - d2;

				assert(delta < 0);

				if ( d1 > leaveFrac * delta )
				{
					leaveFrac = d1 / delta;

					if ( enterFrac >= leaveFrac )
					{
						return;
					}
				}

				allsolid = qfalse;
			}
		}

		--numsides;
		++side;
	}

	trace->contents = brush->contents;

	if ( leadside )
	{
		trace->fraction = enterFrac;
		assert(trace->fraction >= 0 && trace->fraction <= 1.0f);
		VectorCopy(leadside->plane->normal, trace->normal);
		trace->surfaceFlags = cm.materials[leadside->materialNum].surfaceFlags;
		trace->material = &cm.materials[leadside->materialNum];
		return;
	}

	trace->startsolid = qtrue;

	if ( allsolid )
	{
		trace->allsolid = qtrue;
		trace->fraction = 0.0;
	}
}

/*
==================
CM_TestCapsuleInCapsule

capsule inside capsule check
==================
*/
void CM_TestCapsuleInCapsule( traceWork_t *tw, trace_t *trace )
{
	int i;
	vec3_t mins, maxs;
	vec3_t top, bottom;
	vec3_t p1, p2, tmp;
	vec3_t offset, symetricSize[2];
	float radius, halfwidth, halfheight, offs, r;
	float fHeightDiff, fTotalHalfHeight;

	VectorCopy( tw->extents.start, top );
	top[2] = top[2] + tw->offsetZ;
	VectorCopy( tw->extents.start, bottom );
	for ( i = 0 ; i < 3 ; i++ )
	{
		offset[i] = ( tw->threadInfo.box_model->mins[i] + tw->threadInfo.box_model->maxs[i] ) * 0.5;
		symetricSize[0][i] = tw->threadInfo.box_model->mins[i] - offset[i];
		symetricSize[1][i] = tw->threadInfo.box_model->maxs[i] - offset[i];
	}
	halfwidth = symetricSize[ 1 ][ 0 ];
	halfheight = symetricSize[ 1 ][ 2 ];
	radius = ( halfwidth > halfheight ) ? halfheight : halfwidth;
	offs = halfheight - radius;

	r = Square( tw->radius + radius );
	// check if any of the spheres overlap
	VectorCopy( offset, p1 );
	p1[2] += offs;
	VectorSubtract( p1, top, tmp );
	if ( VectorLengthSquared( tmp ) < r )
	{
		trace->startsolid = trace->allsolid = qtrue;
		trace->fraction = 0;
	}
	VectorSubtract( p1, bottom, tmp );
	if ( VectorLengthSquared( tmp ) < r )
	{
		trace->startsolid = trace->allsolid = qtrue;
		trace->fraction = 0;
	}
	VectorCopy( offset, p2 );
	p2[2] -= offs;
	VectorSubtract( p2, top, tmp );
	if ( VectorLengthSquared( tmp ) < r )
	{
		trace->startsolid = trace->allsolid = qtrue;
		trace->fraction = 0;
	}
	VectorSubtract( p2, bottom, tmp );
	if ( VectorLengthSquared( tmp ) < r )
	{
		trace->startsolid = trace->allsolid = qtrue;
		trace->fraction = 0;
	}
	// if between cylinder up and lower bounds
	fHeightDiff = tw->extents.start[2] - offset[2];
	fTotalHalfHeight = offs + tw->size[2] - tw->radius;
	assert(fTotalHalfHeight >= 0);
	if ( fTotalHalfHeight >= fabs(fHeightDiff) )
	{
		// 2d coordinates
		top[2] = p1[2] = 0;
		// if the cylinders overlap
		VectorSubtract( top, p1, tmp );
		if ( VectorLengthSquared( tmp ) < r )
		{
			trace->startsolid = trace->allsolid = qtrue;
			trace->fraction = 0;
		}
	}
}

/*
================
CM_TestBoxInBrush
================
*/
void CM_TestBoxInBrush( traceWork_t *tw, cbrush_t *brush, trace_t *trace )
{
	int i;
	float dist;
	float d1;
	cbrushside_t    *side;

	assert(!IS_NAN((tw->extents.start)[0]) && !IS_NAN((tw->extents.start)[1]) && !IS_NAN((tw->extents.start)[2]));
	assert(!IS_NAN((tw->extents.end)[0]) && !IS_NAN((tw->extents.end)[1]) && !IS_NAN((tw->extents.end)[2]));

	// special test for axial
	// the first 6 brush planes are always axial
	if ( 	   tw->bounds[0][0] > brush->maxs[0]
	           || tw->bounds[0][1] > brush->maxs[1]
	           || tw->bounds[0][2] > brush->maxs[2]
	           || tw->bounds[1][0] < brush->mins[0]
	           || tw->bounds[1][1] < brush->mins[1]
	           || tw->bounds[1][2] < brush->mins[2]
	   )
	{
		return;
	}

	// the first six planes are the axial planes, so we only
	// need to test the remainder
	side = brush->sides;
	assert(brush->numsides >= 0);
	for ( i = brush->numsides; i; i--, side++ )
	{
		assert(!IS_NAN(side->plane->dist));
		assert(!IS_NAN(tw->radius));
		assert(!IS_NAN((side->plane->normal)[0]) && !IS_NAN((side->plane->normal)[1]) && !IS_NAN((side->plane->normal)[2]));
		assert(!IS_NAN(tw->offsetZ));

		// adjust the plane distance apropriately for mins/maxs
		dist = side->plane->normal[3] + tw->radius + fabs(side->plane->normal[2] * tw->offsetZ);
		assert(!IS_NAN(dist));

		d1 = DotProduct(tw->extents.start, side->plane->normal) - dist;
		assert(!IS_NAN(d1));

		// if completely in front of face, no intersection
		if ( d1 > 0 )
		{
			return;
		}
	}

	// inside this brush
	trace->startsolid = trace->allsolid = qtrue;
	trace->fraction = 0;
	trace->contents = brush->contents;
}

/*
================
CM_SightTraceCapsuleThroughCapsule

capsule vs. capsule collision (not rotated)
================
*/
int CM_SightTraceCapsuleThroughCapsule( traceWork_t *tw, trace_t *trace )
{
	int i;
	vec3_t top, bottom, starttop, startbottom, endtop, endbottom;
	vec3_t offset, symetricSize[2];
	float radius, halfwidth, halfheight, offs;

	// test trace bounds vs. capsule bounds
	if ( 	   tw->bounds[0][0] > tw->threadInfo.box_model->maxs[0] + RADIUS_EPSILON
	           || tw->bounds[0][1] > tw->threadInfo.box_model->maxs[1] + RADIUS_EPSILON
	           || tw->bounds[0][2] > tw->threadInfo.box_model->maxs[2] + RADIUS_EPSILON
	           || tw->bounds[1][0] < tw->threadInfo.box_model->mins[0] - RADIUS_EPSILON
	           || tw->bounds[1][1] < tw->threadInfo.box_model->mins[1] - RADIUS_EPSILON
	           || tw->bounds[1][2] < tw->threadInfo.box_model->mins[2] - RADIUS_EPSILON
	   )
	{
		return 0;
	}
	// top origin and bottom origin of each sphere at start and end of trace
	VectorCopy( tw->extents.start, starttop );
	starttop[2] = starttop[2] + tw->offsetZ;
	VectorCopy( tw->extents.start, startbottom );
	startbottom[2] = startbottom[2] - tw->offsetZ;
	VectorCopy( tw->extents.end, endtop );
	endtop[2] = endtop[2] + tw->offsetZ;
	VectorCopy( tw->extents.end, endbottom );
	endbottom[2] = endbottom[2] - tw->offsetZ;

	// calculate top and bottom of the capsule spheres to collide with
	for ( i = 0 ; i < 3 ; i++ )
	{
		offset[i] = ( tw->threadInfo.box_model->mins[i] + tw->threadInfo.box_model->maxs[i] ) * 0.5;
		symetricSize[0][i] = tw->threadInfo.box_model->mins[i] - offset[i];
		symetricSize[1][i] = tw->threadInfo.box_model->maxs[i] - offset[i];
	}
	halfwidth = symetricSize[ 1 ][ 0 ];
	halfheight = symetricSize[ 1 ][ 2 ];
	radius = ( halfwidth > halfheight ) ? halfheight : halfwidth;
	offs = halfheight - radius;
	VectorCopy( offset, top );
	top[2] += offs;
	VectorCopy( offset, bottom );
	bottom[2] -= offs;

	// test for collision between the spheres
	if ( startbottom[2] > top[2] )
	{
		if ( !CM_SightTraceSphereThroughSphere(tw, startbottom, endbottom, top, radius, trace) )
		{
			return -1;
		}

		if ( tw->delta[2] >= 0 )
		{
			return 0;
		}
	}
	else
	{
		if ( bottom[2] > starttop[2] )
		{
			if ( !CM_SightTraceSphereThroughSphere(tw, starttop, endtop, bottom, radius, trace) )
			{
				return -1;
			}

			if ( tw->delta[2] <= 0 )
			{
				return 0;
			}
		}
	}

	// height of the expanded cylinder is the height of both cylinders minus the radius of both spheres
	if ( !CM_SightTraceCylinderThroughCylinder(tw, offset, offs, radius, trace) )
	{
		return -1;
	}

	if ( endbottom[2] > top[2] )
	{
		if ( top[2] >= startbottom[2] )
		{
			if ( !CM_SightTraceSphereThroughSphere(tw, startbottom, endbottom, top, radius, trace) )
			{
				return -1;
			}
		}
	}
	else
	{
		if ( bottom[2] > endtop[2] )
		{
			if ( starttop[2] >= bottom[2] )
			{
				if ( !CM_SightTraceSphereThroughSphere(tw, starttop, endtop, bottom, radius, trace) )
				{
					return -1;
				}
			}
		}
	}

	return 0;
}

/*
================
CM_SightTraceThroughLeafBrushNode_r
================
*/
static int CM_SightTraceThroughLeafBrushNode_r( traceWork_t *tw, cLeafBrushNode_t *remoteNode, const vec3_t p1_, const vec3_t p2 )
{
	float absDiff;
	float diff;
	vec3_t mid;
	float frac2;
	float frac;
	int side;
	float invDist;
	vec3_t p1;
	float tmax;
	float tmin;
	float t2;
	float t1;
	float offset;
	int hitNum;
	cbrush_t *brush;
	int brushnum;
	int i;

	assert(remoteNode);
	VectorCopy(p1_, p1);

	while ( 1 )
	{
		if ( !(remoteNode->contents & tw->contents) )
		{
			return 0;
		}

		if ( remoteNode->leafBrushCount )
		{
			if ( remoteNode->leafBrushCount > 0 )
			{
				// trace line against all brushes in the leaf
				for ( i = 0; i < remoteNode->leafBrushCount; i++ )
				{
					brushnum = remoteNode->data.leaf.brushes[i];
					brush = &cm.brushes[brushnum];

					if ( !(brush->contents & tw->contents) )
					{
						continue;
					}

					hitNum = CM_SightTraceThroughBrush(tw, brush);

					if ( hitNum )
					{
						return hitNum;
					}
				}

				return 0;
			}

			hitNum = CM_SightTraceThroughLeafBrushNode_r(tw, remoteNode + 1, p1, p2);

			if ( hitNum )
			{
				return hitNum;
			}
		}

		t1 = p1[remoteNode->axis] - remoteNode->data.children.dist;
		t2 = p2[remoteNode->axis] - remoteNode->data.children.dist;

		offset = tw->size[remoteNode->axis] + SURFACE_CLIP_EPSILON - remoteNode->data.children.range;

		tmax = I_fmax(t1, t2);
		tmin = I_fmin(t1, t2);

		if ( tmin >= offset )
		{
			if ( -offset >= tmax )
			{
				return 0;
			}

			remoteNode += remoteNode->data.children.childOffset[0];
			continue;
		}

		if ( -offset >= tmax )
		{
			remoteNode += remoteNode->data.children.childOffset[1];
			continue;
		}

		diff = t2 - t1;
		absDiff = fabs(diff);

		if ( absDiff > 0.00000047683716 )
		{
			float temp;

			if ( diff < 0.0 )
				temp = t1;
			else
				temp = -t1;

			invDist = 1.0 / absDiff;

			frac2 = (temp - offset) * invDist;
			frac = (temp + offset) * invDist;

			side = I_side(diff);
		}
		else
		{
			side = 0;
			frac = 1.0;
			frac2 = 0.0;
		}

		assert(frac >= 0);
		frac = I_fmin(frac, 1.0);

		mid[0] = (p2[0] - p1[0]) * frac + p1[0];
		mid[1] = (p2[1] - p1[1]) * frac + p1[1];
		mid[2] = (p2[2] - p1[2]) * frac + p1[2];

		hitNum = CM_SightTraceThroughLeafBrushNode_r(tw, &remoteNode[remoteNode->data.children.childOffset[side]], p1, mid);

		if ( hitNum )
		{
			return hitNum;
		}

		assert(frac2 <= 1.0001f);
		frac2 = I_fmax(frac2, 0.0);

		p1[0] = (p2[0] - p1[0]) * frac2 + p1[0];
		p1[1] = (p2[1] - p1[1]) * frac2 + p1[1];
		p1[2] = (p2[2] - p1[2]) * frac2 + p1[2];

		remoteNode += remoteNode->data.children.childOffset[1 - side];
	}
}

/*
================
CM_TraceCapsuleThroughCapsule

capsule vs. capsule collision (not rotated)
================
*/
void CM_TraceCapsuleThroughCapsule( traceWork_t *tw, trace_t *trace )
{
	int i;
	vec3_t top, bottom, starttop, startbottom, endtop, endbottom;
	vec3_t offset, symetricSize[2];
	float radius, halfwidth, halfheight, offs;

	// test trace bounds vs. capsule bounds
	if ( 	   tw->bounds[0][0] > tw->threadInfo.box_model->maxs[0] + RADIUS_EPSILON
	           || tw->bounds[0][1] > tw->threadInfo.box_model->maxs[1] + RADIUS_EPSILON
	           || tw->bounds[0][2] > tw->threadInfo.box_model->maxs[2] + RADIUS_EPSILON
	           || tw->bounds[1][0] < tw->threadInfo.box_model->mins[0] - RADIUS_EPSILON
	           || tw->bounds[1][1] < tw->threadInfo.box_model->mins[1] - RADIUS_EPSILON
	           || tw->bounds[1][2] < tw->threadInfo.box_model->mins[2] - RADIUS_EPSILON
	   )
	{
		return;
	}
	// top origin and bottom origin of each sphere at start and end of trace
	VectorCopy( tw->extents.start, starttop );
	starttop[2] = starttop[2] + tw->offsetZ;
	VectorCopy( tw->extents.start, startbottom );
	startbottom[2] = startbottom[2] - tw->offsetZ;
	VectorCopy( tw->extents.end, endtop );
	endtop[2] = endtop[2] + tw->offsetZ;
	VectorCopy( tw->extents.end, endbottom );
	endbottom[2] = endbottom[2] - tw->offsetZ;

	// calculate top and bottom of the capsule spheres to collide with
	for ( i = 0 ; i < 3 ; i++ )
	{
		offset[i] = ( tw->threadInfo.box_model->mins[i] + tw->threadInfo.box_model->maxs[i] ) * 0.5;
		symetricSize[0][i] = tw->threadInfo.box_model->mins[i] - offset[i];
		symetricSize[1][i] = tw->threadInfo.box_model->maxs[i] - offset[i];
	}
	halfwidth = symetricSize[ 1 ][ 0 ];
	halfheight = symetricSize[ 1 ][ 2 ];
	radius = ( halfwidth > halfheight ) ? halfheight : halfwidth;
	offs = halfheight - radius;
	VectorCopy( offset, top );
	top[2] += offs;
	VectorCopy( offset, bottom );
	bottom[2] -= offs;

	// test for collision between the spheres
	if ( startbottom[2] > top[2] )
	{
		if ( !CM_TraceSphereThroughSphere(tw, startbottom, endbottom, top, radius, trace) )
		{
			return;
		}

		if ( tw->delta[2] >= 0 )
		{
			return;
		}
	}
	else
	{
		if ( bottom[2] > starttop[2] )
		{
			if ( !CM_TraceSphereThroughSphere(tw, starttop, endtop, bottom, radius, trace) )
			{
				return;
			}

			if ( tw->delta[2] <= 0 )
			{
				return;
			}
		}
	}

	// height of the expanded cylinder is the height of both cylinders minus the radius of both spheres
	if ( !CM_TraceCylinderThroughCylinder(tw, offset, offs, radius, trace) )
	{
		return;
	}

	if ( endbottom[2] > top[2] )
	{
		if ( top[2] >= startbottom[2] )
		{
			if ( !CM_TraceSphereThroughSphere(tw, startbottom, endbottom, top, radius, trace) )
			{
				return;
			}
		}
	}
	else
	{
		if ( bottom[2] > endtop[2] )
		{
			if ( starttop[2] >= bottom[2] )
			{
				if ( !CM_TraceSphereThroughSphere(tw, starttop, endtop, bottom, radius, trace) )
				{
					return;
				}
			}
		}
	}
}

/*
================
CM_TraceThroughLeafBrushNode_r
================
*/
static void CM_TraceThroughLeafBrushNode_r( traceWork_t *tw, cLeafBrushNode_t *node, const vec4_t p1_, const vec4_t p2, trace_t *trace )
{
	float absDiff;
	float diff;
	vec4_t mid;
	float frac2;
	float frac;
	int side;
	float invDist;
	vec4_t p1;
	float tmax;
	float tmin;
	float t2;
	float t1;
	float offset;
	cbrush_t *brush;
	int brushnum;
	int i;

	assert(node);
	Vector4Copy(p1_, p1);

	while ( 1 )
	{
		if ( !(node->contents & tw->contents) )
		{
			return;
		}

		if ( node->leafBrushCount )
		{
			if ( node->leafBrushCount > 0 )
			{
				// trace line against all brushes in the leaf
				for ( i = 0; i < node->leafBrushCount; i++ )
				{
					brushnum = node->data.leaf.brushes[i];
					brush = &cm.brushes[brushnum];

					if ( !(brush->contents & tw->contents) )
					{
						continue;
					}

					CM_TraceThroughBrush(tw, brush, trace);
				}

				return;
			}

			CM_TraceThroughLeafBrushNode_r(tw, node + 1, p1, p2, trace);
		}

		t1 = p1[node->axis] - node->data.children.dist;
		t2 = p2[node->axis] - node->data.children.dist;

		offset = tw->size[node->axis] + SURFACE_CLIP_EPSILON - node->data.children.range;

		tmax = I_fmax(t1, t2);
		tmin = I_fmin(t1, t2);

		if ( tmin >= offset )
		{
			if ( -offset >= tmax )
			{
				return;
			}

			node += node->data.children.childOffset[0];
			continue;
		}

		if ( -offset >= tmax )
		{
			node += node->data.children.childOffset[1];
			continue;
		}

		if ( p1[3] >= trace->fraction )
		{
			return;
		}

		diff = t2 - t1;
		absDiff = fabs(diff);

		if ( absDiff > 0.00000047683716 )
		{
			float temp;

			if ( diff < 0.0 )
				temp = t1;
			else
				temp = -t1;

			invDist = 1.0 / absDiff;

			frac2 = (temp - offset) * invDist;
			frac = (temp + offset) * invDist;

			side = I_side(diff);
		}
		else
		{
			side = 0;
			frac = 1.0;
			frac2 = 0.0;
		}

		assert(frac >= 0.0f);
		frac = I_fmin(frac, 1.0);

		mid[0] = (p2[0] - p1[0]) * frac + p1[0];
		mid[1] = (p2[1] - p1[1]) * frac + p1[1];
		mid[2] = (p2[2] - p1[2]) * frac + p1[2];
		mid[3] = (p2[3] - p1[3]) * frac + p1[3];

		CM_TraceThroughLeafBrushNode_r(tw, &node[node->data.children.childOffset[side]], p1, mid, trace);

		assert(frac2 <= 1.0f + TRACE_EPSILON);
		frac2 = I_fmax(frac2, 0.0);

		p1[0] = (p2[0] - p1[0]) * frac2 + p1[0];
		p1[1] = (p2[1] - p1[1]) * frac2 + p1[1];
		p1[2] = (p2[2] - p1[2]) * frac2 + p1[2];
		p1[3] = (p2[3] - p1[3]) * frac2 + p1[3];

		node += node->data.children.childOffset[1 - side];
	}
}

/*
================
CM_TestInLeafBrushNode_r
================
*/
static void CM_TestInLeafBrushNode_r( traceWork_t *tw, cLeafBrushNode_t *node, trace_t *trace )
{
	cbrush_t *b;
	int k;

	assert(node);

	while ( 1 )
	{
		if ( !(node->contents & tw->contents) )
		{
			break;
		}

		if ( node->leafBrushCount )
		{
			if ( node->leafBrushCount > 0 )
			{
				// test box position against all brushes in the leaf
				for ( k = 0; k < node->leafBrushCount; k++ )
				{
					b = &cm.brushes[node->data.leaf.brushes[k]];

					if ( !(b->contents & tw->contents) )
					{
						continue;
					}

					CM_TestBoxInBrush(tw, b, trace);

					if ( trace->allsolid )
					{
						break;
					}
				}

				return;
			}

			CM_TestInLeafBrushNode_r(tw, node + 1, trace);

			if ( trace->allsolid )
			{
				return;
			}
		}

		if ( tw->bounds[0][node->axis] > node->data.children.dist )
		{
			node += node->data.children.childOffset[0];
			continue;
		}

		if ( tw->bounds[1][node->axis] >= node->data.children.dist )
		{
			CM_TestInLeafBrushNode_r(tw, &node[node->data.children.childOffset[0]], trace);

			if ( trace->allsolid )
			{
				return;
			}
		}

		node += node->data.children.childOffset[1];
	}
}

/*
================
CM_SightTraceThroughLeafBrushNode
================
*/
int CM_SightTraceThroughLeafBrushNode( traceWork_t *tw, cLeaf_t *leaf )
{
	vec3_t absmax;
	vec3_t absmin;

	assert(leaf->leafBrushNode);

	VectorSubtract(leaf->mins, tw->size, absmin);
	VectorAdd(leaf->maxs, tw->size, absmax);

	if ( CM_TraceBox(&tw->extents, absmin, absmax, 1.0) )
	{
		return 0;
	}

	return CM_SightTraceThroughLeafBrushNode_r(tw, &cm.leafbrushNodes[leaf->leafBrushNode], tw->extents.start, tw->extents.end);
}

/*
================
CM_TraceThroughLeafBrushNode
================
*/
bool CM_TraceThroughLeafBrushNode( traceWork_t *tw, cLeaf_t *leaf, trace_t *trace )
{
	vec4_t start;
	vec4_t end;
	vec3_t absmin;
	vec3_t absmax;

	assert(leaf->leafBrushNode);

	VectorSubtract(leaf->mins, tw->size, absmin);
	VectorAdd(leaf->maxs, tw->size, absmax);

	if ( CM_TraceBox(&tw->extents, absmin, absmax, trace->fraction) )
	{
		return 0;
	}

	VectorCopy(tw->extents.start, start);
	VectorCopy(tw->extents.end, end);

	start[3] = 0.0;
	end[3] = trace->fraction;

	CM_TraceThroughLeafBrushNode_r(tw, &cm.leafbrushNodes[leaf->leafBrushNode], start, end, trace);

	return trace->fraction == 0;
}

/*
================
CM_TestInLeafBrushNode
================
*/
qboolean CM_TestInLeafBrushNode( traceWork_t *tw, cLeaf_t *leaf, trace_t *trace )
{
	int i;

	assert(leaf->leafBrushNode);

	for ( i = 0; i < 3; i++ )
	{
		if ( leaf->mins[i] >= tw->bounds[1][i] )
		{
			return qfalse;
		}

		if ( tw->bounds[0][i] >= leaf->maxs[i] )
		{
			return qfalse;
		}
	}

	CM_TestInLeafBrushNode_r(tw, &cm.leafbrushNodes[leaf->leafBrushNode], trace);

	return trace->allsolid;
}

/*
================
CM_TraceThroughLeaf
================
*/
void CM_TraceThroughLeaf( traceWork_t *tw, cLeaf_t *leaf, trace_t *trace )
{
	int k;

	if ( trace->fraction == 0 )
	{
		return;
	}

	if ( leaf->brushContents & tw->contents )
	{
		if ( CM_TraceThroughLeafBrushNode(tw, leaf, trace) )
		{
			return;
		}
	}

	if ( !(leaf->terrainContents & tw->contents) )
	{
		return;
	}

	// trace line against all brushes in the leaf
	for ( k = 0; k < leaf->collAabbCount; k++ )
	{
		if ( !trace->fraction )
		{
			return;
		}

		CM_TraceThroughAabbTree(tw, &cm.aabbTrees[k + leaf->firstCollAabbIndex], trace);
	}
}

/*
================
CM_TestInLeaf
================
*/
void CM_TestInLeaf( traceWork_t *tw, cLeaf_t *leaf, trace_t *trace )
{
	if ( (leaf->brushContents & tw->contents) )
	{
		if ( CM_TestInLeafBrushNode(tw, leaf, trace) )
		{
			return;
		}
	}

	if ( !(leaf->terrainContents & tw->contents) )
	{
		return;
	}

	CM_MeshTestInLeaf(tw, leaf, trace);
}

/*
================
CM_SightTraceThroughLeaf
================
*/
int CM_SightTraceThroughLeaf( traceWork_t *tw, cLeaf_t *leaf, trace_t *trace )
{
	int hitNum;
	int k;

	if ( (leaf->brushContents & tw->contents) )
	{
		hitNum = CM_SightTraceThroughLeafBrushNode(tw, leaf);

		if ( hitNum )
		{
			return hitNum;
		}
	}

	assert(trace->fraction == 1.0f);

	if ( (leaf->terrainContents & tw->contents) )
	{
		// trace line against all brushes in the leaf
		for ( k = 0; k < leaf->collAabbCount; k++ )
		{
			CM_SightTraceThroughAabbTree(tw, &cm.aabbTrees[k + leaf->firstCollAabbIndex], trace);

			if ( trace->fraction == 1 )
			{
				continue;
			}

			return k + leaf->firstCollAabbIndex + cm.numBrushes + 1;
		}
	}

	return 0;
}

/*
==================
CM_TraceThroughTree

Traverse all the contacted leafs from the start to the end position.
If the trace is a point, they will be exactly in order, but for larger
trace volumes it is possible to hit something in a later leaf with
a smaller intercept fraction.
==================
*/
void CM_TraceThroughTree( traceWork_t *tw, int num, const vec4_t p1_, const vec4_t p2, trace_t *trace )
{
	float invDist;
	float absDiff;
	float diff;
	vec4_t p1;
	int side;
	vec4_t mid;
	float frac2;
	float frac;
	float offset;
	float t2;
	float t1;
	cplane_t *plane;
	cNode_t *node;

	Vector4Copy(p1_, p1);

	while ( num >= 0 )
	{
		//
		// find the point distances to the seperating plane
		// and the offset for the size of the box
		//

		node = &cm.nodes[num];
		plane = node->plane;

		// adjust the plane distance apropriately for mins/maxs
		if ( plane->type < 3 )
		{
			t1 = p1[plane->type] - plane->dist;
			t2 = p2[plane->type] - plane->dist;

			offset = tw->size[plane->type] + SURFACE_CLIP_EPSILON;
		}
		else
		{
			t1 = DotProduct(plane->normal, p1) - plane->dist;
			t2 = DotProduct(plane->normal, p2) - plane->dist;

			if ( tw->isPoint )
			{
				offset = SURFACE_CLIP_EPSILON;
			}
			else
			{
				/*
				// an axial brush right behind a slanted bsp plane
				// will poke through when expanded, so adjust
				// by sqrt(3)
				offset = Q_fabs(tw->extents[0]*plane->normal[0]) +
				Q_fabs(tw->extents[1]*plane->normal[1]) +
				Q_fabs(tw->extents[2]*plane->normal[2]);

				offset *= 2;
				offset = tw->maxOffset;
				*/

				// this is silly
				offset = 2048;
			}
		}

		if ( I_fmin(t1, t2) >= offset )
		{
			num = node->children[0];
			continue;
		}

		if ( -offset >= I_fmax(t1, t2) )
		{
			num = node->children[1];
			continue;
		}

		if ( p1[3] >= trace->fraction )
		{
			return;
		}

		diff = t2 - t1;
		absDiff = fabs(diff);

		if ( absDiff > 0.00000047683716 )
		{
			float temp;

			if ( diff < 0.0 )
				temp = t1;
			else
				temp = -t1;

			invDist = 1.0 / absDiff;

			frac2 = (temp - offset) * invDist;
			frac = (temp + offset) * invDist;

			side = I_side(diff);
		}
		else
		{
			side = 0;
			frac = 1.0;
			frac2 = 0.0;
		}

		assert(frac >= 0);
		frac = I_fmin(frac, 1.0);

		mid[0] = (p2[0] - p1[0]) * frac + p1[0];
		mid[1] = (p2[1] - p1[1]) * frac + p1[1];
		mid[2] = (p2[2] - p1[2]) * frac + p1[2];
		mid[3] = (p2[3] - p1[3]) * frac + p1[3];

		CM_TraceThroughTree(tw, node->children[side], p1, mid, trace);

		assert(frac2 <= 1.0f);
		frac2 = I_fmax(frac2, 0.0);

		p1[0] = (p2[0] - p1[0]) * frac2 + p1[0];
		p1[1] = (p2[1] - p1[1]) * frac2 + p1[1];
		p1[2] = (p2[2] - p1[2]) * frac2 + p1[2];
		p1[3] = (p2[3] - p1[3]) * frac2 + p1[3];

		num = node->children[side ^ 1];
	}

	CM_TraceThroughLeaf(tw, &cm.leafs[-1 - num], trace);
}

/*
==================
CM_PositionTest
==================
*/
#define MAX_POSITION_LEAFS  1024
void CM_PositionTest( traceWork_t *tw, trace_t *trace )
{
	int leafs[MAX_POSITION_LEAFS];
	int i;
	leafList_t ll;

	if ( trace->allsolid )
	{
		return;
	}

	// identify the leafs we are touching
	VectorSubtract( tw->extents.start, tw->size, ll.bounds[0] );
	VectorAdd( tw->extents.start, tw->size, ll.bounds[1] );

	for ( i = 0 ; i < 3 ; i++ )
	{
		ll.bounds[0][i] -= 1;
		ll.bounds[1][i] += 1;
	}

	ll.count = 0;
	ll.maxcount = MAX_POSITION_LEAFS;
	ll.list = leafs;
	ll.lastLeaf = 0;
	ll.overflowed = qfalse;

	CM_BoxLeafnums_r( &ll, 0 );

	if ( !ll.count )
	{
		return;
	}

	// test the contents of the leafs
	for ( i = 0 ; i < ll.count ; i++ )
	{
		if ( trace->allsolid )
		{
			break;
		}
		CM_TestInLeaf( tw, &cm.leafs[leafs[i]], trace );
	}
}

/*
==================
CM_SightTraceThroughTree
==================
*/
int CM_SightTraceThroughTree( traceWork_t *tw, int num, const vec3_t p1_, const vec3_t p2, trace_t *trace )
{
	float invDist;
	float absDiff;
	float diff;
	vec3_t p1;
	int hitNum;
	int side;
	vec3_t mid;
	float frac2;
	float frac;
	float offset;
	float t2;
	float t1;
	cplane_t *plane;
	cNode_t *node;

	VectorCopy(p1_, p1);

	while ( 1 )
	{
		while ( 1 )
		{
			while ( 1 )
			{
				if ( num < 0 )
				{
					return CM_SightTraceThroughLeaf(tw, &cm.leafs[-1 - num], trace);
				}

				node = &cm.nodes[num];
				plane = node->plane;

				if ( plane->type < 3 )
				{
					t1 = p1[plane->type] - plane->dist;
					t2 = p2[plane->type] - plane->dist;

					offset = tw->size[plane->type] + SURFACE_CLIP_EPSILON;
				}
				else
				{
					t1 = DotProduct(plane->normal, p1) - plane->dist;
					t2 = DotProduct(plane->normal, p2) - plane->dist;

					offset = tw->isPoint ? SURFACE_CLIP_EPSILON : 2048;
				}

				if ( I_fmin(t1, t2) < offset )
				{
					break;
				}

				num = node->children[0];
			}

			if ( -offset < I_fmax(t1, t2) )
			{
				break;
			}

			num = node->children[1];
		}

		diff = t2 - t1;
		absDiff = fabs(diff);

		if ( absDiff > 0.00000047683716 )
		{
			float temp;

			if ( diff < 0.0 )
				temp = t1;
			else
				temp = -t1;

			invDist = 1.0 / absDiff;

			frac2 = (temp - offset) * invDist;
			frac = (temp + offset) * invDist;

			side = I_side(diff);
		}
		else
		{
			side = 0;
			frac = 1.0;
			frac2 = 0.0;
		}

		assert(frac >= 0);
		frac = I_fmin(frac, 1.0);

		mid[0] = (p2[0] - p1[0]) * frac + p1[0];
		mid[1] = (p2[1] - p1[1]) * frac + p1[1];
		mid[2] = (p2[2] - p1[2]) * frac + p1[2];

		hitNum = CM_SightTraceThroughTree(tw, node->children[side], p1, mid, trace);

		if ( hitNum )
		{
			break;
		}

		assert(frac2 <= 1.0f);
		frac2 = I_fmax(frac2, 0.0);

		p1[0] = (p2[0] - p1[0]) * frac2 + p1[0];
		p1[1] = (p2[1] - p1[1]) * frac2 + p1[1];
		p1[2] = (p2[2] - p1[2]) * frac2 + p1[2];

		num = node->children[side ^ 1];
	}

	return hitNum;
}

/*
==================
CM_Trace
==================
*/
void CM_Trace( trace_t *results, const vec3_t start, const vec3_t end,
               const vec3_t mins, const vec3_t maxs,
               clipHandle_t model, int brushmask )
{
	int i;
	traceWork_t tw;
	vec3_t offset;
	cmodel_t    *cmod;
	vec4_t _start, _end;

	assert(cm.numNodes);
	assert(mins);
	assert(maxs);
	assert(!IS_NAN((end)[0]) && !IS_NAN((end)[1]) && !IS_NAN((end)[2]));

	cmod = CM_ClipHandleToModel( model );

	// set basic parms
	tw.contents = brushmask;

	// adjust so that mins and maxs are always symetric, which
	// avoids some complications with plane expanding of rotated
	// bmodels
	for ( i = 0 ; i < 3 ; i++ )
	{
		assert(maxs[i] >= mins[i]);
		offset[i] = (mins[i] + maxs[i]) * 0.5;
		tw.size[i] = maxs[i] - offset[i];
		tw.extents.start[i] = start[i] + offset[i];
		tw.extents.end[i] = end[i] + offset[i];
		tw.midpoint[i] = (tw.extents.start[i] + tw.extents.end[i]) * 0.5;
		tw.delta[i] = tw.extents.end[i] - tw.extents.start[i];
		tw.halfDelta[i] = tw.delta[i] * 0.5;
		tw.halfDeltaAbs[i] = fabs(tw.halfDelta[i]);
	}

	CM_CalcTraceEntents(&tw.extents);

	tw.deltaLenSq = VectorLengthSquared(tw.delta);
	tw.deltaLen = sqrt(tw.deltaLenSq);

	if ( tw.size[2] >= tw.size[0] )
		tw.radius = tw.size[0];
	else
		tw.radius = tw.size[2];

	tw.offsetZ = tw.size[2] - tw.radius;

	// calculate bounds
	for ( i = 0; i < 2; i++ )
	{
		if ( tw.extents.end[i] > tw.extents.start[i] )
		{
			tw.bounds[0][i] = tw.extents.start[i] - tw.radius;
			tw.bounds[1][i] = tw.extents.end[i] + tw.radius;
		}
		else
		{
			tw.bounds[0][i] = tw.extents.end[i] - tw.radius;
			tw.bounds[1][i] = tw.extents.start[i] + tw.radius;
		}
	}

	assert(tw.offsetZ >= 0);

	if ( tw.extents.end[2] > tw.extents.start[2] )
	{
		tw.bounds[0][2] = tw.extents.start[2] - tw.offsetZ - tw.radius;
		tw.bounds[1][2] = tw.extents.end[2] + tw.offsetZ + tw.radius;
	}
	else
	{
		tw.bounds[0][2] = tw.extents.end[2] - tw.offsetZ - tw.radius;
		tw.bounds[1][2] = tw.extents.start[2] + tw.offsetZ + tw.radius;
	}

	CM_SetAxialCullOnly(&tw);
	CM_GetTrackThreadInfo(&tw.threadInfo);

	//
	// check for position test special case
	//
	if ( VectorCompare(start, end) )
	{
		tw.isPoint = 0;

		if ( model )
		{
			if ( model == CAPSULE_MODEL_HANDLE )
			{
				if ( (tw.contents & tw.threadInfo.box_brush->contents) )
				{
					CM_TestCapsuleInCapsule(&tw, results);
					assert(!IS_NAN(results->fraction));
				}
			}
			else
			{
				if ( !results->allsolid )
				{
					CM_TestInLeaf(&tw, &cmod->leaf, results);
					assert(!IS_NAN(results->fraction));
				}
			}
		}
		else
		{
			CM_PositionTest(&tw, results);
		}
	}
	else
	{
		assert(tw.size[0] >= 0);
		assert(tw.size[1] >= 0);
		assert(tw.size[2] >= 0);

		tw.isPoint = tw.size[0] + tw.size[1] + tw.size[2] == 0;

		assert(tw.offsetZ >= 0);

		tw.radiusOffset[0] = tw.radius;
		tw.radiusOffset[1] = tw.radius;
		tw.radiusOffset[2] = tw.radius + tw.offsetZ;

		if ( model )
		{
			if ( model == CAPSULE_MODEL_HANDLE )
			{
				if ( (tw.contents & tw.threadInfo.box_brush->contents) )
				{
					CM_TraceCapsuleThroughCapsule(&tw, results);
				}
			}
			else
			{
				CM_TraceThroughLeaf(&tw, &cmod->leaf, results);
			}
		}
		else
		{
			VectorCopy(tw.extents.start, _start);
			_start[3] = 0.0;
			VectorCopy(tw.extents.end, _end);
			_end[3] = results->fraction;
			CM_TraceThroughTree(&tw, 0, _start, _end, results);
		}
	}
}
