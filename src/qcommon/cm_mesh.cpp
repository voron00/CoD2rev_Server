#include "qcommon.h"
#include "cm_local.h"

/*
==================
CM_RayTriangleIntersect
==================
*/
bool CM_RayTriangleIntersect( const vec3_t orig, const vec3_t dir, const vec3_t vert1, const vec3_t vert2, const vec3_t vert3, float *pt, float *pu, float *pv )
{
	float d0;
	vec3_t c2;
	vec3_t c;
	vec3_t v3;
	vec3_t v2;
	vec3_t v1;
	float d2;
	float d1;

	VectorSubtract(vert2, vert1, v1);
	VectorSubtract(vert3, vert1, v2);

	Vec3Cross(dir, v2, c);
	d0 = DotProduct(v1, c);

	if ( d0 < 0.001 )
	{
		return false;
	}

	VectorSubtract(orig, vert1, v3);
	d1 = DotProduct(v3, c);

	if ( d1 < 0.0 || d1 > d0 )
	{
		return false;
	}

	Vec3Cross(v3, v1, c2);
	d2 = DotProduct(dir, c2);

	if ( d2 < 0.0 || d1 + d2 > d0 )
	{
		return false;
	}

	*pt = DotProduct(v2, c2) / d0;

	if ( pu )
		*pu = d1 / d0;

	if ( pv )
		*pv = d2 / d0;

	return true;
}

/*
==================
CM_CullBox
==================
*/
bool CM_CullBox( traceWork_t *tw, const vec3_t origin, const vec3_t halfSize )
{
	vec3_t mid;
	vec3_t distorig;

	VectorSubtract(tw->midpoint, origin, distorig);
	VectorAdd(halfSize, tw->size, mid);

	if ( fabs(distorig[0]) > mid[0] + tw->halfDeltaAbs[0] )
	{
		return true;
	}

	if ( fabs(distorig[1]) > mid[1] + tw->halfDeltaAbs[1] )
	{
		return true;
	}

	if ( fabs(distorig[2]) > mid[2] + tw->halfDeltaAbs[2] )
	{
		return true;
	}

	if ( tw->axialCullOnly )
	{
		return false;
	}

	if ( fabs(tw->halfDelta[1] * distorig[2] - tw->halfDelta[2] * distorig[1]) > mid[1] * tw->halfDeltaAbs[2] + mid[2] * tw->halfDeltaAbs[1] )
	{
		return true;
	}

	if ( fabs(tw->halfDelta[2] * distorig[0] - tw->halfDelta[0] * distorig[2]) > mid[2] * tw->halfDeltaAbs[0] + mid[0] * tw->halfDeltaAbs[2] )
	{
		return true;
	}

	if ( fabs(tw->halfDelta[0] * distorig[1] - tw->halfDelta[1] * distorig[0]) > mid[0] * tw->halfDeltaAbs[1] + mid[1] * tw->halfDeltaAbs[0] )
	{
		return true;
	}

	return false;
}

/*
==================
CM_PositionTestInAabbTree_r
==================
*/
static void CM_PositionTestInAabbTree_r( traceWork_t *tw, CollisionAabbTree_t *aabbTree, trace_t *trace )
{
	int triCount;
	int16_t checkStamp;
	CollisionPartition *partition;
	int partitionIndex;
	CollisionAabbTree_t *child;
	int childIndex;

	if ( CM_CullBox(tw, aabbTree->origin, aabbTree->halfSize) )
	{
		return;
	}

	if ( aabbTree->childCount )
	{
		for ( childIndex = 0, child = &cm.aabbTrees[aabbTree->u.firstChildIndex]; childIndex < aabbTree->childCount; childIndex++, child++ )
		{
			CM_PositionTestInAabbTree_r(tw, child, trace);
		}

		return;
	}

	partitionIndex = aabbTree->u.firstChildIndex;
	checkStamp = tw->threadInfo.checkcount;

	if ( tw->threadInfo.partitions[partitionIndex] == checkStamp )
	{
		return;
	}

	tw->threadInfo.partitions[partitionIndex] = checkStamp;
	partition = &cm.partitions[partitionIndex];

	for ( triCount = 0; triCount < partition->triCount; triCount++ )
	{
		CM_PositionTestCapsuleInTriangle(tw, &partition->tris[triCount], trace);
	}
}

/*
==================
CM_TraceThroughAabbTree_r
==================
*/
static void CM_TraceThroughAabbTree_r( traceWork_t *tw, CollisionAabbTree_t *aabbTree, trace_t *trace )
{
	CollisionTriangle_s *tri;
	int i;
	int16_t checkcount;
	CollisionPartition *partition;
	int partitionIndex;
	CollisionAabbTree_t *child;
	int childIndex;

	if ( CM_CullBox(tw, aabbTree->origin, aabbTree->halfSize) )
	{
		return;
	}

	if ( aabbTree->childCount )
	{
		for ( childIndex = 0, child = &cm.aabbTrees[aabbTree->u.firstChildIndex]; childIndex < aabbTree->childCount; childIndex++, child++ )
		{
			CM_TraceThroughAabbTree_r(tw, child, trace);
		}

		return;
	}

	partitionIndex = aabbTree->u.firstChildIndex;
	checkcount = tw->threadInfo.checkcount;

	if ( tw->threadInfo.partitions[partitionIndex] == checkcount )
	{
		return;
	}

	tw->threadInfo.partitions[partitionIndex] = checkcount;
	partition = &cm.partitions[partitionIndex];

	if ( tw->isPoint )
	{
		for ( i = 0; i < partition->triCount; i++ )
		{
			CM_TracePointThroughTriangle(tw, &partition->tris[i], trace);
		}

		return;
	}

	for ( i = 0; i < partition->triCount; i++ )
	{
		tri = &partition->tris[i];
		CM_TraceCapsuleThroughTriangle(tw, tri, tw->offsetZ, trace);

		if ( tri->plane[2] >= 0.0 )
		{
			continue;
		}

		CM_TraceCapsuleThroughTriangle(tw, tri, -tw->offsetZ, trace);
	}

	if ( (tw->delta[0] || tw->delta[1] ) && tw->offsetZ != 0.0 )
	{
		for ( i = 0; i < partition->borderCount; i++ )
		{
			CM_TraceCapsuleThroughBorder(tw, &partition->borders[i], trace);
		}
	}
}

/*
==================
CM_MeshTestInLeaf
==================
*/
void CM_MeshTestInLeaf( traceWork_t *tw, cLeaf_t *leaf, trace_t *trace )
{
	CollisionAabbTree_t *aabbTree;
	dmaterial_t *materialInfo;
	int k;

	assert(!tw->isPoint);
	assert(!trace->allsolid);

	for ( k = 0; k < leaf->collAabbCount; k++ )
	{
		aabbTree = &cm.aabbTrees[k + leaf->firstCollAabbIndex];
		materialInfo = &cm.materials[aabbTree->materialIndex];

		if ( !(tw->contents & materialInfo->contentFlags) )
		{
			continue;
		}

		CM_PositionTestInAabbTree_r(tw, aabbTree, trace);

		if ( trace->allsolid )
		{
			trace->surfaceFlags = materialInfo->surfaceFlags;
			trace->contents = materialInfo->contentFlags;
			trace->material = materialInfo;
			return;
		}
	}
}

/*
==================
CM_TraceThroughAabbTree
==================
*/
void CM_TraceThroughAabbTree( traceWork_t *tw, CollisionAabbTree_t *aabbTree, trace_t *trace )
{
	dmaterial_t *materialInfo;
	float oldFraction;

	materialInfo = &cm.materials[aabbTree->materialIndex];

	if ( !(tw->contents & materialInfo->contentFlags) )
	{
		return;
	}

	oldFraction = trace->fraction;
	CM_TraceThroughAabbTree_r(tw, aabbTree, trace);

	if ( oldFraction > trace->fraction )
	{
		trace->surfaceFlags = materialInfo->surfaceFlags;
		trace->contents = materialInfo->contentFlags;
		trace->material = materialInfo;
	}
}

/*
==================
CM_SightTraceThroughAabbTree
==================
*/
void CM_SightTraceThroughAabbTree( traceWork_t *tw, CollisionAabbTree_t *aabbTree, trace_t *trace )
{
	if ( !(tw->contents & cm.materials[aabbTree->materialIndex].contentFlags) )
	{
		return;
	}

	CM_TraceThroughAabbTree(tw, aabbTree, trace);
}

/*
==================
CM_TracePointThroughTriangle
==================
*/
void CM_TracePointThroughTriangle( traceWork_t *tw, CollisionTriangle_t *tri, trace_t *trace )
{
	float negativeU;
	float v;
	vec3_t triNormalScaledByAreaX2;
	float frac;
	float projTriAreaScaledByTraceLenX2;
	float t;

	projTriAreaScaledByTraceLenX2 = DotProduct(tw->extents.end, tri->plane) - tri->plane[3];

	if ( projTriAreaScaledByTraceLenX2 >= 0.0 )
	{
		return;
	}

	t = DotProduct(tw->extents.start, tri->plane) - tri->plane[3];

	if ( t <= 0.0 )
	{
		return;
	}

	frac = (t - SURFACE_CLIP_EPSILON) / (t - projTriAreaScaledByTraceLenX2);
	frac = I_fmax(frac, 0.0);

	if ( frac >= trace->fraction )
	{
		return;
	}

	VectorMA(tw->extents.start, t / (t - projTriAreaScaledByTraceLenX2), tw->delta, triNormalScaledByAreaX2);
	v = DotProduct(triNormalScaledByAreaX2, tri->svec) - tri->svec[3];

	if ( v < -0.001 || v > 1.001 )
	{
		return;
	}

	negativeU = DotProduct(triNormalScaledByAreaX2, tri->tvec) - tri->tvec[3];

	if ( negativeU < -0.001 || v + negativeU > 1.001 )
	{
		return;
	}

	trace->fraction = frac;
	assert(trace->fraction >= 0 && trace->fraction <= 1.0f);
	VectorCopy(tri->plane, trace->normal);
}

/*
==================
CM_TraceCapsuleThroughBorder
==================
*/
void CM_TraceCapsuleThroughBorder( traceWork_t *tw, CollisionBorder *border, trace_t *trace )
{
	vec3_t edgePoint;
	float c;
	float discriminant;
	float offsetLenSq;
	float b;
	vec2_t offset;
	vec3_t endpos;
	float s;
	float t;
	float d;
	float traceDeltaDot;
	float radius;

	traceDeltaDot = Dot2Product(tw->delta, border->distEq);

	if ( traceDeltaDot >= 0.0 )
	{
		return;
	}

	radius = tw->radius + SURFACE_CLIP_EPSILON;

	d = Dot2Product(tw->extents.start, border->distEq) - border->distEq[2];
	t = (radius - d) / traceDeltaDot;

	if ( t >= trace->fraction || -radius > t * tw->deltaLen )
	{
		return;
	}

	VectorMA(tw->extents.start, t, tw->delta, endpos);
	s = border->distEq[1] * endpos[0] - border->distEq[0] * endpos[1] - border->start;

	if ( s < 0.0 )
	{
		edgePoint[0] = border->distEq[1] * border->start + border->distEq[0] * border->distEq[2];
		edgePoint[1] = border->distEq[1] * border->distEq[2] - border->distEq[0] * border->start;

		Vector2Subtract(tw->extents.start, edgePoint, offset);
		b = Dot2Product(tw->delta, offset);

		if ( b >= 0.0 )
		{
			return;
		}

		offsetLenSq = Dot2Product(offset, offset);
		c = offsetLenSq - Square(radius);

		if ( c < 0.0 )
		{
			edgePoint[2] = border->zBase;

			assert(tw->offsetZ == tw->size[2] - tw->radius);
			assert(tw->offsetZ >= 0);

			if ( fabs(edgePoint[2] - tw->extents.start[2]) <= tw->offsetZ )
			{
				VectorSet(trace->normal, border->distEq[0], border->distEq[1], 0.0);
				assert(Vec3IsNormalized( trace->normal ));
				trace->fraction = 0.0;

				if ( Square(tw->radius) > offsetLenSq )
				{
					trace->startsolid = qtrue;
				}
			}

			return;
		}

		discriminant = Square(b) - tw->deltaLenSq * c;

		if ( discriminant < 0.0 )
		{
			return;
		}

		assert(tw->deltaLenSq > 0.0f);
		t = (-b - sqrt(discriminant)) / tw->deltaLenSq;

		if ( t >= trace->fraction || t <= 0.0 )
		{
			return;
		}

		VectorMA(tw->extents.start, t, tw->delta, endpos);
		s = 0.0;

		assert(tw->offsetZ == tw->size[2] - tw->radius);
		assert(tw->offsetZ >= 0);

		if ( fabs(endpos[2] - s * border->zSlope + border->zBase) <= tw->offsetZ )
		{
			trace->fraction = t;
			assert(trace->fraction >= 0 && trace->fraction <= 1.0f);
			VectorSet(trace->normal, border->distEq[0], border->distEq[1], 0.0);
			assert(Vec3IsNormalized( trace->normal ));
		}

		return;
	}

	if ( s <= border->length )
	{
		if ( t < 0.0 )
		{
			t = 0.0;
		}

		if ( fabs(endpos[2] - s * border->zSlope + border->zBase) <= tw->offsetZ )
		{
			trace->fraction = t;
			assert(trace->fraction >= 0 && trace->fraction <= 1.0f);
			VectorSet(trace->normal, border->distEq[0], border->distEq[1], 0.0);
			assert(Vec3IsNormalized( trace->normal ));
		}

		return;
	}

	edgePoint[0] = (border->start + border->length) * border->distEq[1] + border->distEq[0] * border->distEq[2];
	edgePoint[1] = border->distEq[1] * border->distEq[2] - (border->start + border->length) * border->distEq[0];

	Vector2Subtract(tw->extents.start, edgePoint, offset);
	b = Dot2Product(tw->delta, offset);

	if ( b >= 0.0 )
	{
		return;
	}

	offsetLenSq = Dot2Product(offset, offset);
	c = offsetLenSq - Square(radius);

	if ( c >= 0.0 )
	{
		discriminant = Square(b) - tw->deltaLenSq * c;

		if ( discriminant < 0.0 )
		{
			return;
		}

		assert(tw->deltaLenSq > 0.0f);
		t = (-b - sqrt(discriminant)) / tw->deltaLenSq;

		if ( t >= trace->fraction || t <= 0.0 )
		{
			return;
		}

		VectorMA(tw->extents.start, t, tw->delta, endpos);
		s = border->length;

		assert(tw->offsetZ == tw->size[2] - tw->radius);
		assert(tw->offsetZ >= 0);

		if ( fabs(endpos[2] - s * border->zSlope + border->zBase) <= tw->offsetZ )
		{
			trace->fraction = t;
			assert(trace->fraction >= 0 && trace->fraction <= 1.0f);
			VectorSet(trace->normal, border->distEq[0], border->distEq[1], 0.0);
			assert(Vec3IsNormalized( trace->normal ));
		}

		return;
	}

	edgePoint[2] = border->zSlope * border->length + border->zBase;

	if ( fabs(tw->extents.start[2] - edgePoint[2]) <= tw->offsetZ )
	{
		VectorSet(trace->normal, border->distEq[0], border->distEq[1], 0.0);
		assert(Vec3IsNormalized( trace->normal ));
		trace->fraction = 0.0;

		if ( Square(tw->radius) > offsetLenSq )
		{
			trace->startsolid = qtrue;
		}
	}
}

/*
==================
CM_TraceCapsuleThroughTriangle
==================
*/
void CM_TraceCapsuleThroughTriangle( traceWork_t *tw, CollisionTriangle_t *tri, float offsetZ, trace_t *trace )
{
	float startDist;
	float tScale;
	float sScale;
	float planeDist;
	vec3_t endpos;
	vec3_t sphereStart;
	int vertId;
	CollisionEdge_t *edge;
	int edgeIndex;
	float c;
	float b;
	float a;
	float discriminant;
	vec2_t offset;
	vec2_t deltaOffset;
	vec3_t hitDelta;
	float offsetLenSq;
	int vertToCheck;
	float t;
	float s;
	float d;
	vec3_t start;
	float hitFrac;
	float scaledPlaneDist;
	float hitDist;
	float radius;
	float radiusNegU;
	int checkIter;

	VectorCopy(tw->extents.end, endpos);
	endpos[2] = endpos[2] - offsetZ;

	radius = tw->radius + SURFACE_CLIP_EPSILON;
	scaledPlaneDist = DotProduct(endpos, tri->plane) - tri->plane[3];

	if ( scaledPlaneDist >= radius )
	{
		return;
	}

	VectorCopy(tw->extents.start, sphereStart);
	sphereStart[2] = sphereStart[2] - offsetZ;

	hitDist = DotProduct(sphereStart, tri->plane) - tri->plane[3];
	startDist = hitDist - scaledPlaneDist;

	if ( startDist <= 0.000099999997 )
	{
		return;
	}

	radiusNegU = -radius;

	if ( radiusNegU >= hitDist )
	{
		planeDist = (offsetZ + offsetZ) * tri->plane[2] + hitDist;

		if ( radiusNegU >= planeDist )
		{
			return;
		}

		hitFrac = (radiusNegU - hitDist) / tri->plane[2];

		sScale = DotProduct(sphereStart, tri->svec) - tri->svec[3];
		tScale = DotProduct(sphereStart, tri->tvec) - tri->tvec[3];

		s = hitFrac * tri->svec[2] + sScale;

		if ( s < 0.0 )
		{
			return;
		}

		t = hitFrac * tri->tvec[2] + tScale;

		if ( t >= 0.0 && s + t <= 1.0 )
		{
			VectorCopy(tri->plane, trace->normal);
			trace->fraction = 0.0;
			trace->startsolid = qtrue;
			return;
		}

		hitFrac = radius <= planeDist ? (radius - hitDist) / tri->plane[2] : offsetZ + offsetZ;
		s = hitFrac * tri->svec[2] + sScale;

		if ( s < 0.0 )
		{
			return;
		}

		t = hitFrac * tri->tvec[2] + tScale;

		if ( t >= 0.0 && s + t <= 1.0 )
		{
			VectorCopy(tri->plane, trace->normal);
			trace->fraction = 0.0;
			trace->startsolid = qtrue;
			return;
		}

		return;
	}

	if ( hitDist - radius <= 0.0 )
	{
		hitFrac = 0.0;
		VectorCopy(sphereStart, start);
	}
	else
	{
		hitFrac = (hitDist - radius) / startDist;

		if ( hitFrac > trace->fraction )
		{
			return;
		}

		VectorMA(sphereStart, hitFrac, tw->delta, start);
	}

	s = DotProduct(start, tri->svec) - tri->svec[3];
	t = DotProduct(start, tri->tvec) - tri->tvec[3];

	vertToCheck = s + t > 1.0;

	if ( s < 0.0 )
	{
		vertToCheck |= 2;
	}

	if ( t < 0.0 )
	{
		vertToCheck |= 4;
	}

	if ( !vertToCheck )
	{
		VectorCopy(tri->plane, trace->normal);
		trace->fraction = hitFrac;

		if ( tw->radius > hitDist )
		{
			trace->startsolid = qtrue;
		}

		return;
	}

	for ( checkIter = 0; checkIter < 3; checkIter++ )
	{
		if ( ((vertToCheck >> checkIter) & 1) )
		{
			// checking edges
			edgeIndex = tri->edges[checkIter];

			if ( edgeIndex < 0 )
			{
				continue;
			}

			if ( tw->threadInfo.edges[edgeIndex] == tw->threadInfo.checkcount )
			{
				continue;
			}

			tw->threadInfo.edges[edgeIndex] = tw->threadInfo.checkcount;
			edge = &cm.edges[edgeIndex];

			VectorSubtract(sphereStart, edge->origin, hitDelta);

			offset[0] = DotProduct(hitDelta, edge->axis[0]);
			offset[1] = DotProduct(hitDelta, edge->axis[1]);

			deltaOffset[0] = DotProduct(tw->delta, edge->axis[0]);
			deltaOffset[1] = DotProduct(tw->delta, edge->axis[1]);

			b = Dot2Product(deltaOffset, offset);

			if ( b >= 0.0 )
			{
				continue;
			}

			offsetLenSq = Dot2Product(offset, offset);
			c = offsetLenSq - Square(radius);

			if ( c <= 0.0 )
			{
				d = DotProduct(hitDelta, edge->axis[2]);

				if ( fabs(d - 0.5) > 0.5 )
				{
					continue;
				}

				VectorScale(edge->axis[0], offset[0], trace->normal);
				VectorMA(trace->normal, offset[1], edge->axis[1], trace->normal);
				Vec3Normalize(trace->normal);

				if ( tri->plane[2] >= 0.69999999 && trace->normal[2] >= 0.0 && trace->normal[2] < 0.69999999 && sphereStart[2] > endpos[2] )
				{
					VectorCopy(tri->plane, trace->normal);
				}

				trace->fraction = 0.0;

				if ( Square(tw->radius) > offsetLenSq )
				{
					trace->startsolid = qtrue;
				}

				return;
			}

			a = Vec2Multiply(deltaOffset);
			discriminant = Square(b) - a * c;

			if ( discriminant <= 0.0 )
			{
				continue;
			}

			hitFrac = (-sqrt(discriminant) - b) / a;

			if ( trace->fraction <= hitFrac )
			{
				continue;
			}

			VectorMA(hitDelta, hitFrac, tw->delta, hitDelta);
			start[2] = DotProduct(hitDelta, edge->axis[2]);

			if ( fabs(start[2] - 0.5) > 0.5 )
			{
				continue;
			}

			start[0] = (hitFrac * deltaOffset[0] + offset[0]) / radius;
			start[1] = (hitFrac * deltaOffset[1] + offset[1]) / radius;

			VectorScale(edge->axis[0], start[0], trace->normal);
			VectorMA(trace->normal, start[1], edge->axis[1], trace->normal);

			if ( tri->plane[2] >= 0.69999999 && trace->normal[2] >= 0.0 && trace->normal[2] < 0.69999999 && sphereStart[2] > endpos[2] )
			{
				VectorCopy(tri->plane, trace->normal);
			}

			trace->fraction = hitFrac;
		}
		else
		{
			// checking verts
			vertId = tri->verts[checkIter];

			if ( vertId < 0 )
			{
				continue;
			}

			if ( tw->threadInfo.verts[vertId] == tw->threadInfo.checkcount )
			{
				continue;
			}

			tw->threadInfo.verts[vertId] = tw->threadInfo.checkcount;
			VectorSubtract(sphereStart, cm.verts[vertId], hitDelta);
			b = DotProduct(tw->delta, hitDelta);

			if ( b >= 0.0 )
			{
				continue;
			}

			offsetLenSq = DotProduct(hitDelta, hitDelta);
			c = offsetLenSq - Square(radius);

			if ( c <= 0.0 )
			{
				hitFrac = 1.0 / sqrt(offsetLenSq);
				VectorScale(hitDelta, hitFrac, trace->normal);

				if ( tri->plane[2] >= 0.69999999 && trace->normal[2] >= 0.0 && trace->normal[2] < 0.69999999 && sphereStart[2] > endpos[2] )
				{
					VectorCopy(tri->plane, trace->normal);
				}

				trace->fraction = 0.0;

				if ( Square(tw->radius) > offsetLenSq )
				{
					trace->startsolid = qtrue;
				}

				return;
			}

			a = tw->deltaLenSq;
			discriminant = Square(b) - a * c;

			if ( discriminant < 0.0 )
			{
				continue;
			}

			hitFrac = (-sqrt(discriminant) - b) / a;

			if ( trace->fraction <= hitFrac )
			{
				continue;
			}

			VectorMA(hitDelta, hitFrac, tw->delta, trace->normal);
			VectorScale(trace->normal, 1.0 / radius, trace->normal);

			if ( tri->plane[2] >= 0.69999999 && trace->normal[2] >= 0.0 && trace->normal[2] < 0.69999999 && sphereStart[2] > endpos[2] )
			{
				VectorCopy(tri->plane, trace->normal);
			}

			trace->fraction = hitFrac;
		}
	}
}

/*
==================
CM_PositionTestCapsuleInTriangle
==================
*/
void CM_PositionTestCapsuleInTriangle( traceWork_t *tw, CollisionTriangle_t *tri, trace_t *trace )
{
	float offsetZ;
	float tScale;
	float sScale;
	float hitDist;
	vec3_t sphereStart;
	float planeDist;
	float hitFrac;
	float radius;
	float radiusNegU;
	int vertId;
	CollisionEdge_t *edge;
	int edgeIndex;
	float d;
	vec3_t hitDelta;
	int vertToCheck;
	float t;
	float s;
	vec3_t start;
	int checkIter;

	offsetZ = tw->offsetZ;

	if ( tri->plane[2] < 0.0 )
	{
		offsetZ = offsetZ * -1.0;
	}

	VectorCopy(tw->extents.start, sphereStart);
	sphereStart[2] = sphereStart[2] - offsetZ;

	radius = tw->radius;
	hitDist = DotProduct(sphereStart, tri->plane) - tri->plane[3];

	if ( hitDist >= radius )
	{
		return;
	}

	radiusNegU = -radius;

	if ( radiusNegU >= hitDist )
	{
		planeDist = (offsetZ + offsetZ) * tri->plane[2] + hitDist;

		if ( radiusNegU >= planeDist )
		{
			return;
		}

		hitFrac = (radiusNegU - hitDist) / tri->plane[2];

		sScale = DotProduct(sphereStart, tri->svec) - tri->svec[3];
		tScale = DotProduct(sphereStart, tri->tvec) - tri->tvec[3];

		s = hitFrac * tri->svec[2] + sScale;

		if ( s < 0.0 || (t = hitFrac * tri->tvec[2] + tScale, t < 0.0) || s + t > 1.0 )
		{
			hitFrac = radius <= planeDist ? (radius - hitDist) / tri->plane[2] : offsetZ + offsetZ;
			s = hitFrac * tri->svec[2] + sScale;

			if ( s < 0.0 )
			{
				return;
			}

			t = hitFrac * tri->tvec[2] + tScale;

			if ( t >= 0.0 && s + t <= 1.0 )
			{
				trace->startsolid = qtrue;
				trace->allsolid = qtrue;
				trace->fraction = 0.0;
				return;
			}
		}
		else
		{
			trace->startsolid = qtrue;
			trace->allsolid = qtrue;
			trace->fraction = 0.0;
		}

		return;
	}

	VectorMA(sphereStart, -hitDist, tri->plane, start);

	s = DotProduct(start, tri->svec) - tri->svec[3];
	t = DotProduct(start, tri->tvec) - tri->tvec[3];

	vertToCheck = s + t > 1.0;

	if ( s < 0.0 )
	{
		vertToCheck |= 2;
	}

	if ( t < 0.0 )
	{
		vertToCheck |= 4;
	}

	if ( !vertToCheck )
	{
		trace->startsolid = qtrue;
		trace->allsolid = qtrue;
		trace->fraction = 0.0;
		return;
	}

	for ( checkIter = 0; checkIter < 3; checkIter++ )
	{
		if ( ((vertToCheck >> checkIter) & 1) )
		{
			edgeIndex = tri->edges[checkIter];

			if ( edgeIndex < 0 )
			{
				continue;
			}

			edge = &cm.edges[edgeIndex];

			VectorSubtract(sphereStart, edge->origin, hitDelta);
			d = DotProduct(hitDelta, edge->axis[2]);

			if ( fabs(d - 0.5) > 0.5 )
			{
				continue;
			}

			d = d / VectorLengthSquared(edge->axis[2]);
			VectorMA(hitDelta, -d, edge->axis[2], hitDelta);

			if ( Square(tw->radius) > VectorLengthSquared(hitDelta) )
			{
				trace->startsolid = qtrue;
				trace->allsolid = qtrue;
				trace->fraction = 0.0;
				return;
			}
		}
		else
		{
			vertId = tri->verts[checkIter];

			if ( vertId < 0 )
			{
				continue;
			}

			VectorSubtract(sphereStart, cm.verts[vertId], hitDelta);

			if ( Square(tw->radius) > VectorLengthSquared(hitDelta) )
			{
				trace->startsolid = qtrue;
				trace->allsolid = qtrue;
				trace->fraction = 0.0;
				return;
			}
		}
	}
}