#include "qcommon.h"
#include "cm_local.h"
#include "sys_thread.h"

#ifdef TESTING_LIBRARY
#define cm (*((clipMap_t*)( 0x08185BE0 )))
#define cme (*((clipMapExtra_t*)( 0x08185CF4 )))
#else
extern clipMap_t cm;
extern clipMapExtra_t cme;
#endif

cmodel_t *CM_ClipHandleToModel( clipHandle_t handle )
{
	if ( handle < 0 )
	{
		Com_Error( ERR_DROP, "CM_ClipHandleToModel: bad handle %i", handle );
	}

	if ( handle < cm.numSubModels )
	{
		return &cm.cmodels[handle];
	}
	else
	{
		TraceThreadInfo *tti = (TraceThreadInfo *)Sys_GetValue(3);
		cmodel_t *box_model = tti->box_model;
		return box_model;
	}
}

bool CM_CullBox(traceWork_t *tw, const float *origin, const float *halfSize)
{
	vec3_t distorig;
	vec3_t mid;
	int i;

	VectorSubtract(tw->midpoint, origin, distorig);
	VectorAdd(tw->size, halfSize, mid);

	for(i = 0; i < 3; ++i)
	{
		if ( tw->halfDeltaAbs[i] + mid[i] < fabs(distorig[i]) )
		{
			return 1;
		}
	}

	if ( tw->axialCullOnly)
	{
		return 0;
	}

	if(tw->halfDeltaAbs[1] * mid[2] + mid[1] * tw->halfDeltaAbs[2] < fabs(tw->halfDelta[1] * distorig[2] - tw->halfDelta[2] * distorig[1]))
	{
		return 1;
	}

	if(tw->halfDeltaAbs[0] * mid[2] + mid[0] * tw->halfDeltaAbs[2] < fabs(tw->halfDelta[2] * distorig[0] - tw->halfDelta[0] * distorig[2]))
	{
		return 1;
	}

	if(tw->halfDeltaAbs[1] * mid[0] + tw->halfDeltaAbs[0] * mid[1] < fabs(tw->halfDelta[0] * distorig[1] - tw->halfDelta[1] * distorig[0]))
	{
		return 1;
	}

	return 0;
}

void CM_TestBoxInBrush(traceWork_t *tw, cbrush_s *brush, trace_t *trace)
{
	float diff;
	float absDiff;
	cbrushside_s *side;
	float dist;
	float normalDiff;
	float *normal;
	unsigned int i;

	if ( tw->bounds[0][0] <= brush->maxs[0]
	        && tw->bounds[0][1] <= brush->maxs[1]
	        && tw->bounds[0][2] <= brush->maxs[2]
	        && brush->mins[0] <= tw->bounds[1][0]
	        && brush->mins[1] <= tw->bounds[1][1]
	        && brush->mins[2] <= tw->bounds[1][2] )
	{
		side = brush->sides;
		for ( i = brush->numsides; i; --i )
		{
			normal = side->plane->normal;
			diff = side->plane->normal[2] * tw->offsetZ;
			absDiff = fabs(diff);
			normalDiff = normal[3] + tw->radius + absDiff;
			dist = DotProduct(tw->extents.start, normal) - normalDiff;
			if ( dist > 0.0 )
				return;
			++side;
		}
		trace->allsolid = 1;
		trace->startsolid = 1;
		trace->fraction = 0.0;
		trace->contents = brush->contents;
	}
}

void CM_TestInLeafBrushNode_r(traceWork_t *tw, cLeafBrushNode_s *node, trace_t *trace)
{
	cbrush_s *brush;
	int i;

	while ( (node->contents & tw->contents) != 0 )
	{
		if ( node->leafBrushCount )
		{
			if ( node->leafBrushCount > 0 )
			{
				for ( i = 0; i < node->leafBrushCount; ++i )
				{
					brush = &cm.brushes[node->data.leaf.brushes[i]];
					if ( (brush->contents & tw->contents) != 0 )
					{
						CM_TestBoxInBrush(tw, brush, trace);
						if ( trace->allsolid )
							break;
					}
				}
				return;
			}
			CM_TestInLeafBrushNode_r(tw, node + 1, trace);
			if ( trace->allsolid )
				return;
		}
		if ( tw->bounds[0][node->axis] <= node->data.children.dist )
		{
			if ( tw->bounds[1][node->axis] >= node->data.children.dist )
			{
				CM_TestInLeafBrushNode_r(tw, &node[node->data.children.childOffset[0]], trace);
				if ( trace->allsolid )
					return;
			}
			node += node->data.children.childOffset[1];
		}
		else
		{
			node += node->data.children.childOffset[0];
		}
	}
}

int CM_TestInLeafBrushNode(traceWork_t *tw, cLeaf_s *leaf, trace_t *trace)
{
	int i;

	for ( i = 0; i <= 2; ++i )
	{
		if ( leaf->mins[i] >= tw->bounds[1][i] )
			return 0;

		if ( tw->bounds[0][i] >= leaf->maxs[i] )
			return 0;
	}

	CM_TestInLeafBrushNode_r(tw, &cm.leafbrushNodes[leaf->leafBrushNode], trace);
	return trace->allsolid;
}

void CM_TestInLeaf(traceWork_t *tw, cLeaf_s *leaf, trace_t *trace)
{
	if ( ((leaf->brushContents & tw->contents) == 0 || !CM_TestInLeafBrushNode(tw, leaf, trace))
	        && (leaf->terrainContents & tw->contents) != 0 )
	{
		CM_MeshTestInLeaf(tw, leaf, trace);
	}
}

void CM_PositionTest(traceWork_t *tw, trace_t *trace)
{
	leafList_s ll;
	int i;
	uint16_t leafs[1024];

	if ( !trace->allsolid )
	{
		VectorSubtract(tw->extents.start, tw->size, ll.bounds[0]);
		VectorAdd(tw->extents.start, tw->size, ll.bounds[1]);
		for ( i = 0; i <= 2; ++i )
		{
			ll.bounds[0][i] = ll.bounds[0][i] - 1.0;
			ll.bounds[1][i] = ll.bounds[1][i] + 1.0;
		}
		ll.count = 0;
		ll.maxcount = 1024;
		ll.list = leafs;
		ll.lastLeaf = 0;
		ll.overflowed = 0;
		CM_BoxLeafnums_r(&ll, 0);
		if ( ll.count )
		{
			for ( i = 0; i < ll.count && !trace->allsolid; ++i )
				CM_TestInLeaf(tw, &cm.leafs[leafs[i]], trace);
		}
	}
}

void CM_TestCapsuleInCapsule(traceWork_t *tw, trace_t *trace)
{
	float halfwidth;
	float lengthZ;
	float offs;
	float radius;
	float halfheight;
	vec3_t symetricSize[2];
	vec3_t offset;
	float endpos[4];
	vec3_t botom;
	vec3_t top;
	vec3_t startbottom;
	vec3_t starttop;
	int i;

	VectorCopy(tw->extents.start, starttop);
	starttop[2] = starttop[2] + tw->offsetZ;
	VectorCopy(tw->extents.start, startbottom);
	startbottom[2] = startbottom[2] - tw->offsetZ;
	for ( i = 0; i <= 2; ++i )
	{
		offset[i] = (tw->threadInfo.box_model->mins[i] + tw->threadInfo.box_model->maxs[i]) * 0.5;
		symetricSize[0][i] = tw->threadInfo.box_model->mins[i] - offset[i];
		symetricSize[1][i] = tw->threadInfo.box_model->maxs[i] - offset[i];
	}
	if ( symetricSize[1][0] <= symetricSize[1][2] )
		halfwidth = symetricSize[1][0];
	else
		halfwidth = symetricSize[1][2];
	halfheight = symetricSize[1][2] - halfwidth;
	radius = (tw->radius + halfwidth) * (tw->radius + halfwidth);
	VectorCopy(offset, top);
	top[2] = top[2] + halfheight;
	VectorSubtract(top, starttop, endpos);
	if ( radius > VectorLengthSquared(endpos) )
	{
		trace->allsolid = 1;
		trace->startsolid = 1;
		trace->fraction = 0.0;
	}
	VectorSubtract(top, startbottom, endpos);
	if ( radius > VectorLengthSquared(endpos) )
	{
		trace->allsolid = 1;
		trace->startsolid = 1;
		trace->fraction = 0.0;
	}
	VectorCopy(offset, botom);
	botom[2] = botom[2] - halfheight;
	VectorSubtract(botom, starttop, endpos);
	if ( radius > VectorLengthSquared(endpos) )
	{
		trace->allsolid = 1;
		trace->startsolid = 1;
		trace->fraction = 0.0;
	}
	VectorSubtract(botom, startbottom, endpos);
	if ( radius > VectorLengthSquared(endpos) )
	{
		trace->allsolid = 1;
		trace->startsolid = 1;
		trace->fraction = 0.0;
	}
	offs = tw->extents.start[2] - offset[2];
	lengthZ = halfheight + tw->size[2] - tw->radius;
	if ( lengthZ >= fabs(offs) )
	{
		top[2] = 0.0;
		starttop[2] = 0.0;
		VectorSubtract(starttop, top, endpos);
		if ( radius > VectorLengthSquared(endpos) )
		{
			trace->allsolid = 1;
			trace->startsolid = 1;
			trace->fraction = 0.0;
		}
	}
}

int CM_TraceSphereThroughSphere(traceWork_t *tw, const float *vStart, const float *vEnd, const float *vStationary, float radius, trace_t *trace)
{
	vec3_t vDelta;
	float dr;
	float fDiscriminant;
	float fDeltaLen;
	vec3_t vNormal;
	float fEntry;
	float sqd;
	float fA;
	float fB;
	float fC;
	float s;
	float fRadiusSqrd;

	VectorSubtract(vStart, vStationary, vDelta);
	sqd = VectorLengthSquared(vDelta);

	dr = tw->radius + radius;
	fRadiusSqrd = dr * dr;
	s = sqd - fRadiusSqrd;

	fC = VectorLengthSquared(vDelta) - fRadiusSqrd;

	if ( fC > 0.0 )
	{
		fB = DotProduct(vDelta, tw->delta);
		if ( fB >= 0.0)
		{
			return 1;
		}
		fA = tw->deltaLenSq;

		fDiscriminant = fB * fB - s * tw->deltaLenSq;
		if(fDiscriminant < 0.0)
		{
			return 1;
		}

		fDeltaLen = Vec3NormalizeTo(vDelta, vNormal);
		fEntry = fDeltaLen * 0.125 / fB + (-fB - sqrt(fDiscriminant)) / fA;

		if(trace->fraction <= fEntry)
		{
			return 1;
		}
		trace->fraction = I_fmax(fEntry, 0.0);

		VectorCopy(vNormal, trace->normal);
		trace->contents = tw->threadInfo.box_brush->contents;
		return 0;
	}

	trace->fraction = 0.0;
	trace->startsolid = 1;
	trace->contents = tw->threadInfo.box_brush->contents;

	Vec3NormalizeTo(vDelta, trace->normal);
	VectorSubtract(vEnd, vStationary, vDelta);

	if ( fRadiusSqrd >= VectorLengthSquared(vDelta) )
	{
		trace->allsolid = 1;
	}

	return 0;
}

int CM_TraceCylinderThroughCylinder(traceWork_t *tw, const float *vStationary, float fStationaryHalfHeight, float radius, trace_t *trace)
{
	float dr;
	float fA;
	vec3_t vDelta;
	vec3_t vNormal;
	float fHitHeight, fC;
	float fEpsilon, fDeltaLen;
	float sqd2;
	float fB;
	float fTotalHeight;
	float fEntry;
	float fDiscriminant;

	VectorSubtract(tw->extents.start, vStationary, vDelta);
	sqd2 = vDelta[1] * vDelta[1] + vDelta[0] * vDelta[0];

	dr = tw->radius + radius;
	fC = sqd2 - dr * dr;

	if ( fC > 0.0 )
	{
		fB = vDelta[0] * tw->delta[0] + vDelta[1] * tw->delta[1];
		if ( fB >= 0.0 )
		{
			return 1;
		}

		fA = tw->delta[1] * tw->delta[1] + tw->delta[0] * tw->delta[0];

		fDiscriminant = fB * fB - fC * fA;
		if(fDiscriminant < 0.0)
		{
			return 1;
		}

		vDelta[2] = 0.0;
		fDeltaLen = Vec3NormalizeTo(vDelta, vNormal);
		fEpsilon = (fDeltaLen * 0.125) / fB;
		fEntry = (-fB - sqrt(fDiscriminant)) / fA + fEpsilon;
		if ( trace->fraction <= fEntry )
		{
			return 1;
		}

		fTotalHeight = tw->size[2] - tw->radius + fStationaryHalfHeight;
		fHitHeight = (fEntry - fEpsilon) * tw->delta[2] + tw->extents.start[2] - vStationary[2];

		if ( fTotalHeight < fabs(fHitHeight) )
		{
			return 1;
		}
		trace->fraction = I_fmax(fEntry, 0.0);

		VectorCopy(vNormal, trace->normal);
		trace->contents = tw->threadInfo.box_brush->contents;
		return 0;
	}

	fTotalHeight = tw->size[2] - tw->radius + fStationaryHalfHeight;

	if ( fTotalHeight < fabs(vDelta[2]) )
	{
		return 1;
	}

	trace->fraction = 0.0;
	trace->startsolid = 1;
	vDelta[2] = 0.0;
	Vec3NormalizeTo(vDelta, trace->normal);
	trace->contents = tw->threadInfo.box_brush->contents;

	if ( fTotalHeight >= fabs(tw->extents.end[2] - vStationary[2]) )
	{
		trace->allsolid = 1;
	}

	return 0;
}

void CM_TraceCapsuleThroughCapsule(traceWork_t *tw, trace_t *trace)
{
	vec3_t top, bottom, starttop, startbottom, endtop, endbottom, offset;
	int i;
	float halfwidth;
	float halfheight;
	float radius;
	float offs;

	// test trace bounds vs. capsule bounds
	if ( tw->bounds[0][0] > tw->threadInfo.box_model->maxs[0] + RADIUS_EPSILON
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
	VectorCopy(tw->extents.start, starttop);
	starttop[2] += tw->offsetZ;

	VectorCopy(tw->extents.start, startbottom);
	startbottom[2] -= tw->offsetZ;

	VectorCopy(tw->extents.end, endtop);
	endtop[2] += tw->offsetZ;

	VectorCopy(tw->extents.end, endbottom);
	endbottom[2] -= tw->offsetZ;

	for ( i = 0; i < 3; ++i )
	{
		offset[i] = (tw->threadInfo.box_model->mins[i] + tw->threadInfo.box_model->maxs[i]) * 0.5;
	}
	halfwidth = tw->threadInfo.box_model->maxs[0] - offset[0];
	halfheight = tw->threadInfo.box_model->maxs[2] - offset[2];

	radius = ( halfwidth > halfheight ) ? halfheight : halfwidth;
	offs = halfheight - radius;

	VectorCopy( offset, top );
	top[2] += offs;
	VectorCopy( offset, bottom );
	bottom[2] -= offs;

	if ( top[2] >= startbottom[2] && bottom[2] > starttop[2])
	{
		if ( !CM_TraceSphereThroughSphere(tw, starttop, endtop, bottom, radius, trace) || tw->delta[2] <= 0.0 )
			return;
	}
	else if (top[2] < startbottom[2] && (!CM_TraceSphereThroughSphere(tw, startbottom, endbottom, top, radius, trace) || tw->delta[2] >= 0.0 ))
	{
		return;
	}

	if ( CM_TraceCylinderThroughCylinder(tw, offset, offs, radius, trace) )
	{
		if ( top[2] >= endbottom[2] )
		{
			if ( bottom[2] > endtop[2] && starttop[2] >= bottom[2] )
			{
				CM_TraceSphereThroughSphere(tw, starttop, endtop, bottom, radius, trace);
			}
		}
		else if ( startbottom[2] <= top[2] )
		{
			CM_TraceSphereThroughSphere(tw, startbottom, endbottom, top, radius, trace);
		}
	}
}

qboolean CM_TraceBoxInternal(TraceExtents *extents, const float *testvec, float s, float *fraction)
{
	int i;
	float d, o;

	for(i = 0; i < 3; ++i)
	{
		d = (extents->start[i] - testvec[i]) * s;
		o = (extents->end[i] - testvec[i]) * s;
		if ( d <= 0.0 )
		{
			if ( o > 0.0 )
			{
				d = (d * extents->invDelta[i]) * s;
				if ( d <= 0.0 )
				{
					return 1;
				}
				if ( d - *fraction < 0.0 )
				{
					*fraction = d;
				}
			}
		}
		else if ( o > 0.0 || ((float)(d * extents->invDelta[i]) * s) >= *fraction )
		{
			return 1;
		}
	}

	return 0;
}

qboolean QDECL CM_TraceBox(TraceExtents *extents, const float *mins, const float *maxs, float fraction)
{
	float lfraction = fraction;

	if(CM_TraceBoxInternal(extents, mins, -1.0, &lfraction) == qtrue)
	{
		return qtrue;
	}
	if(CM_TraceBoxInternal(extents, maxs, 1.0, &lfraction) == qtrue)
	{
		return qtrue;
	}

	return qfalse;
}