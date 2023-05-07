#include "qcommon.h"
#include "cm_local.h"
#include "sys_thread.h"

void CM_GetTraceThreadInfo(TraceThreadInfo *threadInfo)
{
	TraceThreadInfo *value;

	value = (TraceThreadInfo *)Sys_GetValue(THREAD_VALUE_TRACE);
	++value->checkcount;
	*threadInfo = *value;
}

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
		TraceThreadInfo *tti = (TraceThreadInfo *)Sys_GetValue(THREAD_VALUE_TRACE);
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

void CM_SetAxialCullOnly(traceWork_t *tw)
{
	float length;
	float size;
	float cull;
	vec3_t vec;

	VectorSubtract(tw->bounds[1], tw->bounds[0], vec);
	cull = vec[0] * vec[1] * vec[2];
	size = tw->size[0] * tw->size[1] * tw->size[2];
	length = size * 16.0 * tw->deltaLen;
	tw->axialCullOnly = length > cull;
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
	cbrush_s *b;
	int i;

	while ( (node->contents & tw->contents) != 0 )
	{
		if ( node->leafBrushCount )
		{
			if ( node->leafBrushCount > 0 )
			{
				for ( i = 0; i < node->leafBrushCount; ++i )
				{
					b = &cm.brushes[node->data.leaf.brushes[i]];

					if ( (b->contents & tw->contents) != 0 )
					{
						CM_TestBoxInBrush(tw, b, trace);

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
	int leafs[1024];

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

void CM_SightTraceThroughAabbTree(traceWork_t *tw, CollisionAabbTree_s *aabbTree, trace_t *trace)
{
	if ( (tw->contents & cm.materials[aabbTree->materialIndex].contentFlags) != 0 )
		CM_TraceThroughAabbTree(tw, aabbTree, trace);
}

void CM_TraceThroughLeafBrushNode_r(traceWork_t *tw, cLeafBrushNode_s *node, const float *p1_, const float *p2, trace_t *trace)
{
	float v5;
	float v6;
	float v7;
	float v8;
	float v9;
	int side;
	float diff;
	float t1;
	float frac;
	int k;
	float p1[4];
	float offset;
	float tmax;
	float t2;
	float frac2;
	unsigned short *brushes;
	float absDiff;
	float invDist;
	float tmin;
	int brushnum;
	float mid[4];

	p1[0] = p1_[0];
	p1[1] = p1_[1];
	p1[2] = p1_[2];
	p1[3] = p1_[3];

	while ( (tw->contents & node->contents) != 0 )
	{
		if ( node->leafBrushCount )
		{
			if ( node->leafBrushCount > 0 )
			{
				brushes = node->data.leaf.brushes;

				for ( k = 0; k < node->leafBrushCount; ++k )
				{
					brushnum = brushes[k];

					if ( (tw->contents & cm.brushes[brushnum].contents) != 0 )
						CM_TraceThroughBrush(tw, &cm.brushes[brushnum], trace);
				}

				return;
			}

			CM_TraceThroughLeafBrushNode_r(tw, node + 1, p1, p2, trace);
		}

		t1 = p1[node->axis] - node->data.children.dist;
		t2 = p2[node->axis] - node->data.children.dist;

		offset = (float)(tw->size[node->axis] + 0.125) - node->data.children.range;

		if ( (float)(t1 - t2) < 0.0 )
			v9 = t2;
		else
			v9 = t1;

		tmax = v9;

		if ( (float)(t2 - t1) < 0.0 )
			v8 = t2;
		else
			v8 = t1;

		tmin = v8;

		if ( v8 < offset )
		{
			if ( -offset < tmax )
			{
				if ( p1[3] >= trace->fraction )
					return;

				diff = t2 - t1;
				absDiff = fabs(t2 - t1);

				if ( absDiff <= 0.00000047683716 )
				{
					side = 0;
					frac = 1.0;
					frac2 = 0.0;
				}
				else
				{
					if ( diff < 0.0 )
						v7 = t1;
					else
						v7 = -t1;

					invDist = 1.0 / absDiff;

					frac2 = (float)(v7 - offset) * (float)(1.0 / absDiff);
					frac = (float)(v7 + offset) * (float)(1.0 / absDiff);

					side = diff >= 0.0;
				}

				if ( (float)(1.0 - frac) < 0.0 )
					v6 = 1.0;
				else
					v6 = frac;

				mid[0] = (float)((float)(*p2 - p1[0]) * v6) + p1[0];
				mid[1] = (float)((float)(p2[1] - p1[1]) * v6) + p1[1];
				mid[2] = (float)((float)(p2[2] - p1[2]) * v6) + p1[2];
				mid[3] = (float)((float)(p2[3] - p1[3]) * v6) + p1[3];

				CM_TraceThroughLeafBrushNode_r(tw, &node[node->data.children.childOffset[side]], p1, mid, trace);

				if ( (float)(frac2 - 0.0) < 0.0 )
					v5 = 0.0;
				else
					v5 = frac2;

				frac2 = v5;

				p1[0] = (float)((float)(*p2 - p1[0]) * v5) + p1[0];
				p1[1] = (float)((float)(p2[1] - p1[1]) * v5) + p1[1];
				p1[2] = (float)((float)(p2[2] - p1[2]) * v5) + p1[2];
				p1[3] = (float)((float)(p2[3] - p1[3]) * v5) + p1[3];

				node += node->data.children.childOffset[1 - side];
			}
			else
			{
				node += node->data.children.childOffset[1];
			}
		}
		else
		{
			if ( -offset >= tmax )
				return;

			node += node->data.children.childOffset[0];
		}
	}
}

bool CM_TraceThroughLeafBrushNode(traceWork_t *tw, cLeaf_s *leaf, trace_t *trace)
{
	vec4_t start;
	vec4_t end;
	vec3_t absmin;
	vec3_t absmax;

	VectorSubtract(leaf->mins, tw->size, absmin);
	VectorAdd(leaf->maxs, tw->size, absmax);

	if ( CM_TraceBox(&tw->extents, absmin, absmax, trace->fraction) )
		return 0;

	VectorCopy(tw->extents.start, start);
	VectorCopy(tw->extents.end, end);

	start[3] = 0.0;
	end[3] = trace->fraction;

	CM_TraceThroughLeafBrushNode_r(tw, &cm.leafbrushNodes[leaf->leafBrushNode], start, end, trace);

	return 0.0 == trace->fraction;
}

void CM_TraceThroughLeaf(traceWork_t *tw, cLeaf_s *leaf, trace_t *trace)
{
	int i;

	if ( trace->fraction != 0.0
	        && ((leaf->brushContents & tw->contents) == 0 || !CM_TraceThroughLeafBrushNode(tw, leaf, trace))
	        && (leaf->terrainContents & tw->contents) != 0 )
	{
		for ( i = 0; i < leaf->collAabbCount && trace->fraction != 0.0; ++i )
			CM_TraceThroughAabbTree(tw, &cm.aabbTrees[i + leaf->firstCollAabbIndex], trace);
	}
}

int CM_SightTraceThroughBrush(traceWork_t *tw, cbrush_t *brush)
{
	float d1;
	cbrushside_t *side;
	int j;
	cplane_s *plane;
	float enterFrac;
	float delta;
	float frac;
	float dist;
	float sign;
	float d2;
	float *bounds;
	int index;
	float leaveFrac;
	int i;

	side = 0;

	enterFrac = 0.0;
	leaveFrac = 1.0;
	sign = -1.0;
	bounds = brush->mins;

	for ( index = 0; ; index = 1 )
	{
		for ( j = 0; j < 3; ++j )
		{
			d1 = (tw->extents.start[j] - bounds[j]) * sign - tw->radiusOffset[j];
			d2 = (tw->extents.end[j] - bounds[j]) * sign - tw->radiusOffset[j];

			if ( d1 <= 0.0 )
			{
				if ( d2 > 0.0 )
				{
					frac = (d1 * tw->extents.invDelta[j]) * sign;
					if ( enterFrac >= frac )
					{
						return 0;
					}
					if ( frac - leaveFrac < 0.0 )
					{
						leaveFrac = (d1 * tw->extents.invDelta[j]) * sign;
					}
				}
			}
			else
			{
				if ( d2 > 0.0 )
				{
					return 0;
				}
				frac = d1 * tw->extents.invDelta[j] * sign;
				if ( frac >= leaveFrac )
				{
					return 0;
				}
				if ( enterFrac - frac < 0.0 )
				{
					enterFrac = d1 * tw->extents.invDelta[j] * sign;
				}
			}
		}
		if ( index )
		{
			break;
		}
		sign = 1.0;
		bounds = brush->maxs;
	}
	i = brush->numsides;
	if ( i )
	{
		side = brush->sides;
	}

	while ( i )
	{
		plane = side->plane;
		dist = plane->dist + tw->radius + fabs(plane->normal[2] * tw->offsetZ);

		d1 = DotProduct(tw->extents.start, plane->normal) - dist;
		d2 = DotProduct(tw->extents.end, plane->normal) - dist;

		if ( d1 <= 0.0 )
		{
			if ( d2 > 0.0 )
			{
				delta = d1 - d2;

				if ( d1 > leaveFrac * delta )
				{
					leaveFrac = d1 / delta;
					if ( enterFrac >= leaveFrac )
					{
						return 0;
					}
				}
			}
		}
		else
		{
			delta = d1 - d2;

			if ( d2 > 0.0 )
			{
				return 0;
			}

			if ( d1 > enterFrac * delta )
			{
				enterFrac = d1 / delta;
				if ( enterFrac >= leaveFrac )
				{
					return 0;
				}
			}
		}
		--i;
		++side;
	}

	return brush - cm.brushes + 1;
}

int CM_SightTraceThroughLeafBrushNode_r(traceWork_t *tw, cLeafBrushNode_s *remoteNode, const float *p1_, const float *p2)
{
	float v6;
	float v7;
	float v8;
	float v9;
	float v10;
	unsigned short *brushes;
	int side;
	float diff;
	float t1;
	float frac;
	int k;
	float p1[3];
	float offset;
	float tmax;
	float t2;
	float frac2;
	float absDiff;
	float invDist;
	int hitNum;
	float tmin;
	int brushnum;
	float mid[3];

	p1[0] = p1_[0];
	p1[1] = p1_[1];
	p1[2] = p1_[2];

	while ( 1 )
	{
		if ( (tw->contents & remoteNode->contents) == 0 )
			return 0;

		if ( remoteNode->leafBrushCount )
			break;

retry:
		t1 = p1[remoteNode->axis] - remoteNode->data.children.dist;
		t2 = p2[remoteNode->axis] - remoteNode->data.children.dist;

		offset = (float)(tw->size[remoteNode->axis] + 0.125) - remoteNode->data.children.range;

		if ( (float)(t1 - t2) < 0.0 )
			v10 = t2;
		else
			v10 = t1;

		tmax = v10;

		if ( (float)(t2 - t1) < 0.0 )
			v9 = t2;
		else
			v9 = t1;

		tmin = v9;

		if ( v9 < offset )
		{
			if ( -offset < tmax )
			{
				diff = t2 - t1;
				absDiff = fabs(t2 - t1);

				if ( absDiff <= 0.00000047683716 )
				{
					side = 0;
					frac = 1.0;
					frac2 = 0.0;
				}
				else
				{
					if ( diff < 0.0 )
						v8 = t1;
					else
						v8 = -t1;

					invDist = 1.0 / absDiff;

					frac2 = (float)(v8 - offset) * (float)(1.0 / absDiff);
					frac = (float)(v8 + offset) * (float)(1.0 / absDiff);

					side = diff >= 0.0;
				}

				if ( (float)(1.0 - frac) < 0.0 )
					v7 = 1.0;
				else
					v7 = frac;

				mid[0] = (float)((float)(*p2 - p1[0]) * v7) + p1[0];
				mid[1] = (float)((float)(p2[1] - p1[1]) * v7) + p1[1];
				mid[2] = (float)((float)(p2[2] - p1[2]) * v7) + p1[2];

				hitNum = CM_SightTraceThroughLeafBrushNode_r(tw, &remoteNode[remoteNode->data.children.childOffset[side]], p1, mid);

				if ( hitNum )
					return hitNum;

				if ( (float)(frac2 - 0.0) < 0.0 )
					v6 = 0.0;
				else
					v6 = frac2;

				frac2 = v6;

				p1[0] = (float)((float)(*p2 - p1[0]) * v6) + p1[0];
				p1[1] = (float)((float)(p2[1] - p1[1]) * v6) + p1[1];
				p1[2] = (float)((float)(p2[2] - p1[2]) * v6) + p1[2];

				remoteNode += remoteNode->data.children.childOffset[1 - side];
			}
			else
			{
				remoteNode += remoteNode->data.children.childOffset[1];
			}
		}
		else
		{
			if ( -offset >= tmax )
				return 0;

			remoteNode += remoteNode->data.children.childOffset[0];
		}
	}

	if ( remoteNode->leafBrushCount <= 0 )
	{
		hitNum = CM_SightTraceThroughLeafBrushNode_r(tw, remoteNode + 1, p1, p2);

		if ( hitNum )
			return hitNum;

		goto retry;
	}

	brushes = remoteNode->data.leaf.brushes;

	for ( k = 0; k < remoteNode->leafBrushCount; ++k )
	{
		brushnum = brushes[k];

		if ( (tw->contents & cm.brushes[brushnum].contents) != 0 )
		{
			hitNum = CM_SightTraceThroughBrush(tw, &cm.brushes[brushnum]);

			if ( hitNum )
				return hitNum;
		}
	}

	return 0;
}

int CM_SightTraceThroughLeafBrushNode(traceWork_t *tw, cLeaf_s *leaf)
{
	vec3_t absmax;
	vec3_t absmin;

	VectorSubtract(leaf->mins, tw->size, absmin);
	VectorAdd(leaf->maxs, tw->size, absmax);

	if ( CM_TraceBox(&tw->extents, absmin, absmax, 1.0) )
		return 0;
	else
		return CM_SightTraceThroughLeafBrushNode_r(
		           tw,
		           &cm.leafbrushNodes[leaf->leafBrushNode],
		           tw->extents.start,
		           tw->extents.end);
}

int CM_SightTraceThroughLeaf(traceWork_t *tw, cLeaf_s *leaf, trace_t *trace)
{
	int hitNum;
	int i;

	if ( (leaf->brushContents & tw->contents) != 0 )
	{
		hitNum = CM_SightTraceThroughLeafBrushNode(tw, leaf);

		if ( hitNum )
			return hitNum;
	}

	if ( (leaf->terrainContents & tw->contents) != 0 )
	{
		for ( i = 0; i < leaf->collAabbCount; ++i )
		{
			CM_SightTraceThroughAabbTree(tw, &cm.aabbTrees[i + leaf->firstCollAabbIndex], trace);

			if ( trace->fraction != 1.0 )
				return i + leaf->firstCollAabbIndex + cm.numBrushes + 1;
		}
	}

	return 0;
}

int CM_SightTraceSphereThroughSphere(traceWork_t *tw, const float *vStart, const float *vEnd, const float *vStationary, float radius, trace_t *trace)
{
	float fDiscriminant;
	float fC;
	float fB;
	float fA;
	vec3_t vNormal;
	vec3_t vDelta;
	float fDeltaLen;

	VectorSubtract(vStart, vStationary, vDelta);

	fC = VectorLengthSquared(vDelta) - Square(radius + tw->radius);
	if ( fC > 0.0 )
	{
		fB = DotProduct(tw->delta, vDelta);
		if ( fB < 0.0 )
		{
			fA = tw->deltaLenSq;
			fDiscriminant = Square(fB) - (fA * fC);
			if ( fDiscriminant >= 0.0 )
			{
				fDeltaLen = Vec3NormalizeTo(vDelta, vNormal);
				if((-fB - sqrt(fDiscriminant)) / fA + (fB * 0.125) / fDeltaLen >= trace->fraction)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
		}
		return 1;
	}
	return 0;
}

int CM_SightTraceCylinderThroughCylinder(traceWork_t *tw, const float *vStationary, float fStationaryHalfHeight, float radius, trace_t *trace)
{
	float fDiscriminant;
	float fEntry;
	float fHitHeight;
	float fEpsilon;
	float fTotalHeight;
	float fC;
	float fB;
	float fA;
	vec3_t vNormal;
	vec3_t vDelta;
	float fDeltaLen;

	VectorSubtract(tw->extents.start, vStationary, vDelta);

	fC = Square(vDelta[0]) + Square(vDelta[1]) - Square(radius + tw->radius);
	if ( fC > 0.0 )
	{
		fB = tw->delta[0] * vDelta[0] + tw->delta[1] * vDelta[1];
		if ( fB < 0.0 )
		{
			fA = Square(tw->delta[0]) + Square(tw->delta[1]);
			fDiscriminant = Square(fB) - (fA * fC);
			if ( fDiscriminant >= 0.0 )
			{
				vDelta[2] = 0.0;
				fDeltaLen = Vec3NormalizeTo(vDelta, vNormal);
				fEpsilon = (fB * 0.125) / fDeltaLen;
				fEntry = ((-fB - sqrt(fDiscriminant)) / fA) + fEpsilon;
				if ( fEntry < trace->fraction )
				{
					fTotalHeight = tw->size[2] - tw->radius + fStationaryHalfHeight;
					fHitHeight = (fEntry - fEpsilon) * tw->delta[2] + tw->extents.start[2] - vStationary[2];
					if(fabs(fHitHeight) > fTotalHeight)
					{
						return 1;
					}
					else
					{
						return 0;
					}
				}
			}
		}
		return 1;
	}
	fTotalHeight = tw->size[2] - tw->radius + fStationaryHalfHeight;
	if(fabs(vDelta[2]) > fTotalHeight)
	{
		return 1;
	}
	return 0;
}

int CM_SightTraceCapsuleThroughCapsule(traceWork_t *tw, trace_t *trace)
{
	float radius;
	float halfheight;
	vec3_t symetricSize[2];
	vec3_t offset;
	vec3_t endbottom;
	vec3_t endtop;
	vec3_t startbottom;
	vec3_t starttop;
	vec3_t bottom;
	vec3_t top;
	int i;

	if ( tw->bounds[0][0] > tw->threadInfo.box_model->maxs[0] + 1.0
	        || tw->bounds[0][1] > tw->threadInfo.box_model->maxs[1] + 1.0
	        || tw->bounds[0][2] > tw->threadInfo.box_model->maxs[2] + 1.0
	        || tw->threadInfo.box_model->mins[0] - 1.0 > tw->bounds[1][0]
	        || tw->threadInfo.box_model->mins[1] - 1.0 > tw->bounds[1][1]
	        || tw->threadInfo.box_model->mins[2] - 1.0 > tw->bounds[1][2] )
	{
		return 0;
	}

	VectorCopy(tw->extents.start, starttop);
	starttop[2] = starttop[2] + tw->offsetZ;
	VectorCopy(tw->extents.start, startbottom);
	startbottom[2] = startbottom[2] - tw->offsetZ;
	VectorCopy(tw->extents.end, endtop);
	endtop[2] = endtop[2] + tw->offsetZ;
	VectorCopy(tw->extents.end, endbottom);
	endbottom[2] = endbottom[2] - tw->offsetZ;

	for ( i = 0; i <= 2; ++i )
	{
		offset[i] = (tw->threadInfo.box_model->mins[i] + tw->threadInfo.box_model->maxs[i]) * 0.5;
		symetricSize[0][i] = tw->threadInfo.box_model->mins[i] - offset[i];
		symetricSize[1][i] = tw->threadInfo.box_model->maxs[i] - offset[i];
	}

	if ( symetricSize[1][0] <= symetricSize[1][2] )
		radius = symetricSize[1][0];
	else
		radius = symetricSize[1][2];

	halfheight = symetricSize[1][2] - radius;
	VectorCopy(offset, top);
	top[2] = top[2] + halfheight;
	VectorCopy(offset, bottom);
	bottom[2] = bottom[2] - halfheight;

	if ( startbottom[2] <= top[2] )
	{
		if ( bottom[2] > starttop[2] )
		{
			if ( !CM_SightTraceSphereThroughSphere(tw, starttop, endtop, bottom, radius, trace) )
				return -1;
			if ( tw->delta[2] <= 0.0 )
				return 0;
		}
	}
	else
	{
		if ( !CM_SightTraceSphereThroughSphere(tw, startbottom, endbottom, top, radius, trace) )
			return -1;
		if ( tw->delta[2] >= 0.0 )
			return 0;
	}

	if ( CM_SightTraceCylinderThroughCylinder(tw, offset, halfheight, radius, trace) )
	{
		if ( endbottom[2] <= top[2] )
		{
			if ( bottom[2] > endtop[2]
			        && starttop[2] >= bottom[2]
			        && !CM_SightTraceSphereThroughSphere(tw, starttop, endtop, bottom, radius, trace) )
			{
				return -1;
			}
		}
		else if ( top[2] >= startbottom[2]
		          && !CM_SightTraceSphereThroughSphere(tw, startbottom, endbottom, top, radius, trace) )
		{
			return -1;
		}

		return 0;
	}

	return -1;
}

void CM_TraceThroughTree(traceWork_t *tw, int num, const float *p1_, const float *p2, trace_t *trace)
{
	cNode_t *node;
	int side;
	float diff;
	cplane_s *plane;
	float t1;
	float frac;
	vec4_t p1;
	float offset;
	float t2;
	float frac2;
	float absDiff;
	// float invDist;
	vec4_t mid;

	VectorCopy4(p1_, p1);

	while ( num >= 0 )
	{
		node = &cm.nodes[num];
		plane = cm.nodes[num].plane;

		if ( plane->type >= 3 )
		{
			t1 = DotProduct(plane->normal, p1) - plane->dist;
			t2 = DotProduct(plane->normal, p2) - plane->dist;

			if ( tw->isPoint )
				offset = 0.125;
			else
				offset = 2048.0;
		}
		else
		{
			t1 = p1[plane->type] - plane->dist;
			t2 = p2[plane->type] - plane->dist;
			offset = tw->size[plane->type] + 0.125;
		}

		if ( I_fmin(t2, t1) < offset )
		{
			if ( -offset < I_fmax(t1, t2) )
			{
				if ( p1[3] >= trace->fraction )
				{
					return;
				}
				diff = t2 - t1;
				absDiff = fabs(t2 - t1);
				if ( absDiff <= 0.00000047683716 )
				{
					side = 0;
					frac = 1.0;
					frac2 = 0.0;
				}
				else
				{
					if ( diff < 0.0 )
					{
						frac2 = (t1 - offset) * (1.0 / absDiff);
						frac = (t1 + offset) * (1.0 / absDiff);
					}
					else
					{
						frac2 = (-t1 - offset) * (1.0 / absDiff);
						frac = (-t1 + offset) * (1.0 / absDiff);
					}
					// invDist = 1.0 / absDiff;
					side = diff >= 0.0;
				}

				frac = I_fmin(frac, 1.0);

				mid[0] = ((p2[0] - p1[0]) * frac) + p1[0];
				mid[1] = ((p2[1] - p1[1]) * frac) + p1[1];
				mid[2] = ((p2[2] - p1[2]) * frac) + p1[2];
				mid[3] = ((p2[3] - p1[3]) * frac) + p1[3];

				CM_TraceThroughTree(tw, node->children[side], p1, mid, trace);

				frac2 = I_fmax(frac2, 0.0);

				p1[0] = ((p2[0] - p1[0]) * frac2) + p1[0];
				p1[1] = ((p2[1] - p1[1]) * frac2) + p1[1];
				p1[2] = ((p2[2] - p1[2]) * frac2) + p1[2];
				p1[3] = ((p2[3] - p1[3]) * frac2) + p1[3];
				num = node->children[side ^ 1];
			}
			else
			{
				num = node->children[1];
			}
		}
		else
		{
			num = node->children[0];
		}
	}

	CM_TraceThroughLeaf(tw, &cm.leafs[-1 - num], trace);
}

int CM_SightTraceThroughTree(traceWork_t *tw, int num, const float *p1_, const float *p2, trace_t *trace)
{
	float v7;
	float v8;
	cNode_t *node;
	int side;
	float diff;
	cplane_s *plane;
	float t1;
	float frac;
	vec3_t p1;
	float offset;
	float t2;
	float frac2;
	float absDiff;
	// float invDist;
	int hitNum;
	vec3_t mid;

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
				plane = cm.nodes[num].plane;
				if ( plane->type >= 3 )
				{
					t1 = DotProduct(plane->normal, p1) - plane->dist;
					t2 = DotProduct(plane->normal, p2) - plane->dist;
					offset = tw->isPoint ? 0.125 : 2048.0;
				}
				else
				{
					t1 = p1[plane->type] - plane->dist;
					t2 = p2[plane->type] - plane->dist;
					offset = tw->size[plane->type] + 0.125;
				}
				if ( I_fmin(t2, t1) < offset )
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

		//if( absDiff <= TRACE_EPSILON)
		if ( absDiff <= 0.00000047683716 )
		{
			side = 0;
			frac = 1.0;
			frac2 = 0.0;
		}
		else
		{
			if ( diff < 0.0 )
			{
				v8 = t1;
			}
			else
			{
				v8 = -t1;
			}
			// invDist = 1.0 / absDiff;
			frac2 = (v8 - offset) * (1.0 / absDiff);
			frac = (v8 + offset) * (1.0 / absDiff);
			side = diff >= 0.0;
		}

		v7 = I_fmax(1.0, frac);
		mid[0] = (p2[0] - p1[0]) * v7 + p1[0];
		mid[1] = (p2[1] - p1[1]) * v7 + p1[1];
		mid[2] = (p2[2] - p1[2]) * v7 + p1[2];
		hitNum = CM_SightTraceThroughTree(tw, node->children[side], p1, mid, trace);
		if ( hitNum )
		{
			break;
		}

		frac2 = I_fmax(frac2, 0.0);

		p1[0] = (p2[0] - p1[0]) * frac2 + p1[0];
		p1[1] = (p2[1] - p1[1]) * frac2 + p1[1];
		p1[2] = (p2[2] - p1[2]) * frac2 + p1[2];
		num = node->children[side ^ 1];
	}
	return hitNum;
}

void RotatePoint(float *point, const float (*mat)[3])
{
	vec3_t tvec;

	VectorCopy(point, tvec);

	point[0] = (*mat)[0] * tvec[0] + (*mat)[1] * tvec[1] + (*mat)[2] * tvec[2];
	point[1] = (*mat)[3] * tvec[0] + (*mat)[4] * tvec[1] + (*mat)[5] * tvec[2];
	point[2] = (*mat)[6] * tvec[0] + (*mat)[7] * tvec[1] + (*mat)[8] * tvec[2];
}

int CM_BoxSightTrace(int oldHitNum, const float *start, const float *end, const float *mins, const float *maxs, unsigned int model, int brushmask)
{
	float radius;
	trace_t trace;
	int hitNum;
	cmodel_s *cmodel;
	vec3_t offset;
	traceWork_t tw;
	int i;
	int prevHitNum;

	cmodel = CM_ClipHandleToModel(model);
	trace.fraction = 1.0;
	trace.startsolid = 0;
	trace.allsolid = 0;
	tw.contents = brushmask;

	for ( i = 0; i < 3; ++i )
	{
		offset[i] = (mins[i] + maxs[i]) * 0.5;
		tw.size[i] = maxs[i] - offset[i];
		tw.extents.start[i] = start[i] + offset[i];
		tw.delta[i - 6] = end[i] + offset[i];
		tw.midpoint[i] = (tw.extents.start[i] + tw.delta[i - 6]) * 0.5;
		tw.delta[i] = tw.delta[i - 6] - tw.extents.start[i];
		tw.halfDelta[i] = tw.delta[i] * 0.5;
		tw.halfDeltaAbs[i] = fabs(tw.halfDelta[i]);
	}

	CM_CalcTraceExtents(&tw.extents);
	tw.deltaLenSq = VectorLengthSquared(tw.delta);
	tw.deltaLen = sqrt(tw.deltaLenSq);

	if ( tw.size[0] <= tw.size[2] )
		radius = tw.size[0];
	else
		radius = tw.size[2];

	tw.radius = radius;
	tw.offsetZ = tw.size[2] - radius;

	for ( i = 0; i <= 1; ++i )
	{
		if ( tw.delta[i - 6] <= tw.extents.start[i] )
		{
			tw.bounds[0][i] = tw.delta[i - 6] - tw.radius;
			tw.bounds[1][i] = tw.extents.start[i] + tw.radius;
		}
		else
		{
			tw.bounds[0][i] = tw.extents.start[i] - tw.radius;
			tw.bounds[1][i] = tw.delta[i - 6] + tw.radius;
		}
	}
	if ( tw.extents.end[2] <= tw.extents.start[2] )
	{
		tw.bounds[0][2] = tw.extents.end[2] - tw.offsetZ - tw.radius;
		tw.bounds[1][2] = tw.extents.start[2] + tw.offsetZ + tw.radius;
	}
	else
	{
		tw.bounds[0][2] = tw.extents.start[2] - tw.offsetZ - tw.radius;
		tw.bounds[1][2] = tw.extents.end[2] + tw.offsetZ + tw.radius;
	}

	CM_SetAxialCullOnly(&tw);
	tw.isPoint = 0.0 == tw.size[0] + tw.size[1] + tw.size[2];
	tw.radiusOffset[0] = tw.radius;
	tw.radiusOffset[1] = tw.radius;
	tw.radiusOffset[2] = tw.radius + tw.offsetZ;
	CM_GetTraceThreadInfo(&tw.threadInfo);

	if ( model )
	{
		if ( model == 1023 )
		{
			if ( (tw.contents & tw.threadInfo.box_brush->contents) != 0 )
				return CM_SightTraceCapsuleThroughCapsule(&tw, &trace);
			else
				return 0;
		}
		else
		{
			return CM_SightTraceThroughLeaf(&tw, &cmodel->leaf, &trace);
		}
	}
	else
	{
		hitNum = 0;

		if ( oldHitNum > 0 )
		{
			prevHitNum = oldHitNum - 1;

			if ( prevHitNum < cm.numBrushes )
				hitNum = CM_SightTraceThroughBrush(&tw, &cm.brushes[prevHitNum]);
		}

		if ( !hitNum )
			return CM_SightTraceThroughTree(&tw, 0, tw.extents.start, tw.extents.end, &trace);
	}

	return hitNum;
}

int CM_TransformedBoxSightTrace(int hitNum, const float *start, const float *end, const float *mins, const float *maxs, unsigned int model, int brushmask, const float *origin, const float *angles)
{
	float matrix[3][3];
	//float halfwidth;
	//float halfheight;
	float symetricSize[2][3];
	float offset[3];
	float end_l[3];
	float start_l[3];
	int rotated;
	int i;

	for ( i = 0; i < 3; ++i )
	{
		offset[i] = (mins[i] + maxs[i]) * 0.5;
		symetricSize[0][i] = mins[i] - offset[i];
		symetricSize[1][i] = maxs[i] - offset[i];
		start_l[i] = start[i] + offset[i];
		end_l[i] = end[i] + offset[i];
	}

	start_l[0] = start_l[0] - origin[0];
	start_l[1] = start_l[1] - origin[1];
	start_l[2] = start_l[2] - origin[2];

	end_l[0] = end_l[0] - origin[0];
	end_l[1] = end_l[1] - origin[1];
	end_l[2] = end_l[2] - origin[2];

	rotated = angles[0] != 0.0 || angles[1] != 0.0 || angles[2] != 0.0;

	//halfwidth = symetricSize[1][0];
	//halfheight = symetricSize[1][2];

	if ( rotated )
	{
		AnglesToAxis(angles, matrix);
		RotatePoint(start_l, matrix);
		RotatePoint(end_l, matrix);
	}

	return CM_BoxSightTrace(hitNum, start_l, end_l, symetricSize[0], symetricSize[1], model, brushmask);
}

void CM_TraceCapsuleThroughCapsule(traceWork_t *tw, trace_t *trace)
{
	float radius;
	float offs;
	vec3_t symetricSize[2];
	vec3_t offset;
	vec3_t endbottom;
	vec3_t endtop;
	vec3_t startbottom;
	vec3_t starttop;
	vec3_t bottom;
	vec3_t top;
	int i;

	if ( tw->bounds[0][0] <= tw->threadInfo.box_model->maxs[0] + 1.0
	        && tw->bounds[0][1] <= tw->threadInfo.box_model->maxs[1] + 1.0
	        && tw->bounds[0][2] <= tw->threadInfo.box_model->maxs[2] + 1.0
	        && tw->threadInfo.box_model->mins[0] - 1.0 <= tw->bounds[1][0]
	        && tw->threadInfo.box_model->mins[1] - 1.0 <= tw->bounds[1][1]
	        && tw->threadInfo.box_model->mins[2] - 1.0 <= tw->bounds[1][2] )
	{
		VectorCopy(tw->extents.start, starttop);
		starttop[2] = starttop[2] + tw->offsetZ;
		VectorCopy(tw->extents.start, startbottom);
		startbottom[2] = startbottom[2] - tw->offsetZ;
		VectorCopy(tw->extents.end, endtop);
		endtop[2] = endtop[2] + tw->offsetZ;
		VectorCopy(tw->extents.end, endbottom);
		endbottom[2] = endbottom[2] - tw->offsetZ;

		for ( i = 0; i < 3; ++i )
		{
			offset[i] = (tw->threadInfo.box_model->mins[i] + tw->threadInfo.box_model->maxs[i]) * 0.5;

			symetricSize[0][i] = tw->threadInfo.box_model->mins[i] - offset[i];
			symetricSize[1][i] = tw->threadInfo.box_model->maxs[i] - offset[i];
		}

		if ( symetricSize[1][0] <= symetricSize[1][2] )
			radius = symetricSize[1][0];
		else
			radius = symetricSize[1][2];

		offs = symetricSize[1][2] - radius;
		VectorCopy(offset, top);
		top[2] = top[2] + offs;
		VectorCopy(offset, bottom);
		bottom[2] = bottom[2] - offs;

		if ( startbottom[2] <= top[2] )
		{
			if ( bottom[2] > starttop[2]
			        && (!CM_TraceSphereThroughSphere(tw, starttop, endtop, bottom, radius, trace) || tw->delta[2] <= 0.0) )
			{
				return;
			}
		}
		else if ( !CM_TraceSphereThroughSphere(tw, startbottom, endbottom, top, radius, trace) || tw->delta[2] >= 0.0 )
		{
			return;
		}

		if ( CM_TraceCylinderThroughCylinder(tw, offset, offs, radius, trace) )
		{
			if ( endbottom[2] <= top[2] )
			{
				if ( bottom[2] > endtop[2] && starttop[2] >= bottom[2] )
					CM_TraceSphereThroughSphere(tw, starttop, endtop, bottom, radius, trace);
			}
			else if ( top[2] >= startbottom[2] )
			{
				CM_TraceSphereThroughSphere(tw, startbottom, endbottom, top, radius, trace);
			}
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
	vec3_t endpos;
	vec3_t botom;
	vec3_t top;
	vec3_t startbottom;
	vec3_t starttop;
	int i;

	VectorCopy(tw->extents.start, starttop);
	starttop[2] = starttop[2] + tw->offsetZ;
	VectorCopy(tw->extents.start, startbottom);
	startbottom[2] = startbottom[2] - tw->offsetZ;

	for ( i = 0; i < 3; ++i )
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

void CM_Trace(trace_t *results, const float *start, const float *end, const float *mins, const float *maxs, clipHandle_t model, int brushmask)
{
	vec_t radius;
	vec4_t _end;
	vec4_t _start;
	cmodel_s *cmodel;
	vec3_t offset;
	traceWork_t tw;
	int i;

	cmodel = CM_ClipHandleToModel(model);
	tw.contents = brushmask;

	for ( i = 0; i < 3; ++i )
	{
		offset[i] = (mins[i] + maxs[i]) * 0.5;
		tw.size[i] = maxs[i] - offset[i];
		tw.extents.start[i] = start[i] + offset[i];
		tw.delta[i - 6] = end[i] + offset[i];
		tw.midpoint[i] = (tw.extents.start[i] + tw.delta[i - 6]) * 0.5;
		tw.delta[i] = tw.delta[i - 6] - tw.extents.start[i];
		tw.halfDelta[i] = tw.delta[i] * 0.5;
		tw.halfDeltaAbs[i] = fabs(tw.halfDelta[i]);
	}

	CM_CalcTraceExtents(&tw.extents);
	tw.deltaLenSq = VectorLengthSquared(tw.delta);
	tw.deltaLen = sqrt(tw.deltaLenSq);

	if ( tw.size[0] <= tw.size[2] )
		radius = tw.size[0];
	else
		radius = tw.size[2];

	tw.radius = radius;
	tw.offsetZ = tw.size[2] - radius;

	for ( i = 0; i <= 1; ++i )
	{
		if ( tw.delta[i - 6] <= tw.extents.start[i] )
		{
			tw.bounds[0][i] = tw.delta[i - 6] - tw.radius;
			tw.bounds[1][i] = tw.extents.start[i] + tw.radius;
		}
		else
		{
			tw.bounds[0][i] = tw.extents.start[i] - tw.radius;
			tw.bounds[1][i] = tw.delta[i - 6] + tw.radius;
		}
	}

	if ( tw.extents.end[2] <= tw.extents.start[2] )
	{
		tw.bounds[0][2] = tw.extents.end[2] - tw.offsetZ - tw.radius;
		tw.bounds[1][2] = tw.extents.start[2] + tw.offsetZ + tw.radius;
	}
	else
	{
		tw.bounds[0][2] = tw.extents.start[2] - tw.offsetZ - tw.radius;
		tw.bounds[1][2] = tw.extents.end[2] + tw.offsetZ + tw.radius;
	}

	CM_SetAxialCullOnly(&tw);
	CM_GetTraceThreadInfo(&tw.threadInfo);

	if ( VectorCompare(start, end) )
	{
		tw.isPoint = 0;

		if ( model )
		{
			if ( model == 1023 )
			{
				if ( (tw.contents & tw.threadInfo.box_brush->contents) != 0 )
					CM_TestCapsuleInCapsule(&tw, results);
			}
			else if ( !results->allsolid )
			{
				CM_TestInLeaf(&tw, &cmodel->leaf, results);
			}
		}
		else
		{
			CM_PositionTest(&tw, results);
		}
	}
	else
	{
		tw.isPoint = 0.0 == tw.size[0] + tw.size[1] + tw.size[2];

		tw.radiusOffset[0] = tw.radius;
		tw.radiusOffset[1] = tw.radius;
		tw.radiusOffset[2] = tw.radius + tw.offsetZ;

		if ( model )
		{
			if ( model == 1023 )
			{
				if ( (tw.contents & tw.threadInfo.box_brush->contents) != 0 )
					CM_TraceCapsuleThroughCapsule(&tw, results);
			}
			else
			{
				CM_TraceThroughLeaf(&tw, &cmodel->leaf, results);
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

void CM_BoxTrace(trace_t *results, const float *start, const float *end, const float *mins, const float *maxs, clipHandle_t model, int brushmask)
{
	memset(results, 0, sizeof(trace_t));
	results->fraction = 1.0;
	CM_Trace(results, start, end, mins, maxs, model, brushmask);
}

void TransposeMatrix(const float (*matrix)[3], float (*transpose)[3])
{
	int j;
	int i;

	for ( i = 0; i < 3; ++i )
	{
		for ( j = 0; j < 3; ++j )
		{
			*(&(*transpose)[3 * i] + j) = *(&(*matrix)[3 * j] + i);
		}
	}
}

void CM_TransformedBoxTraceRotated(trace_t *results, const float *start, const float *end, const float *mins, const float *maxs, clipHandle_t model, int brushmask, const float *origin, const float (*matrix)[3])
{
	vec3_t transpose[3];
	// float halfheight;
	vec3_t symetricSize[2];
	vec3_t offset;
	vec3_t end_l;
	vec3_t start_l;
	int i;
	float oldFraction;

	for ( i = 0; i < 3; ++i )
	{
		offset[i] = (mins[i] + maxs[i]) * 0.5;
		symetricSize[0][i] = mins[i] - offset[i];
		symetricSize[1][i] = maxs[i] - offset[i];
		start_l[i] = start[i] + offset[i];
		end_l[i] = end[i] + offset[i];
	}

	VectorSubtract(start_l, origin, start_l);
	VectorSubtract(end_l, origin, end_l);
	// halfheight = symetricSize[1][2];
	RotatePoint(start_l, matrix);
	RotatePoint(end_l, matrix);
	oldFraction = results->fraction;

	CM_Trace(results, start_l, end_l, symetricSize[0], symetricSize[1], model, brushmask);

	if ( oldFraction > results->fraction )
	{
		TransposeMatrix(matrix, transpose);
		RotatePoint(results->normal, transpose);
	}
}

void CM_TransformedBoxTrace(trace_t *results, const float *start, const float *end, const float *mins, const float *maxs, unsigned int model, int brushmask, const float *origin, const float *angles)
{
	vec3_t matrix[3];
	// float halfwidth;
	// float halfheight;
	vec3_t symetricSize[2];
	vec3_t offset;
	vec3_t end_l;
	vec3_t start_l;
	int i;
	// float oldFraction;

	if ( 0.0 != angles[0] || 0.0 != angles[1] || 0.0 != angles[2] )
	{
		AnglesToAxis(angles, matrix);
		CM_TransformedBoxTraceRotated(results, start, end, mins, maxs, model, brushmask, origin, matrix);
	}
	else
	{
		for ( i = 0; i < 3; ++i )
		{
			offset[i] = (mins[i] + maxs[i]) * 0.5;
			symetricSize[0][i] = mins[i] - offset[i];
			symetricSize[1][i] = maxs[i] - offset[i];
			start_l[i] = start[i] + offset[i];
			end_l[i] = end[i] + offset[i];
		}

		VectorSubtract(start_l, origin, start_l);
		VectorSubtract(end_l, origin, end_l);
		// halfwidth = symetricSize[1][0];
		// halfheight = symetricSize[1][2];
		// oldFraction = results->fraction;

		CM_Trace(results, start_l, end_l, symetricSize[0], symetricSize[1], model, brushmask);
	}
}

void CM_TransformedBoxTraceExternal(trace_t *results, const float *start, const float *end, const float *mins, const float *maxs, unsigned int model, int brushmask, const float *origin, const float *angles)
{
	memset(results, 0, sizeof(trace_t));
	results->fraction = 0.0;
	CM_TransformedBoxTrace(results, start, end, mins, maxs, model, brushmask, origin, angles);
}