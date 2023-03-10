#include "qcommon.h"
#include "cm_local.h"

#ifdef TESTING_LIBRARY
#define cm (*((clipMap_t*)( 0x08185BE0 )))
#define cme (*((clipMapExtra_t*)( 0x08185CF4 )))
#else
extern clipMap_t cm;
extern clipMapExtra_t cme;
#endif

void CM_PositionTestCapsuleInTriangle(traceWork_t *tw, CollisionTriangle_s *collTtris, trace_t *trace)
{
	float scale2;
	int side4;
	int side2;
	int side;
	float offsetZ;
	float length;
	float scaledDist2;
	float scaledDist2_4;
	float dist;
	float offsetPos;
	vec3_t top;
	float distToEdge;
	float cutoffDist;
	float radius;
	float radiusNegU;
	float *vertex;
	int vertexId;
	CollisionEdge_s *edge;
	int edgeIndex;
	float scale;
	vec3_t delta;
	int sides;
	float scaledDist[2];
	vec3_t start;
	int i;

	offsetZ = tw->offsetZ;

	if ( collTtris->plane[2] < 0.0 )
		HIBYTE(offsetZ) ^= 0x80u;

	VectorCopy(tw->extents.start, top);
	top[2] = top[2] - offsetZ;
	radius = tw->radius;
	dist = DotProduct(top, collTtris->plane) - collTtris->plane[3];

	if ( dist < radius )
	{
		radiusNegU = -radius;

		if ( -radius < dist )
		{
			sides = 0;
			VectorMA(top, -dist, collTtris->plane, start);
			scaledDist[1] = DotProduct(start, collTtris->svec) - collTtris->svec[3];
			scaledDist[0] = DotProduct(start, collTtris->tvec) - collTtris->tvec[3];
			side = sides;

			if ( scaledDist[1] + scaledDist[0] > 1.0 )
				side = sides | 1;

			sides = side;
			side2 = side;

			if ( scaledDist[1] < 0.0 )
				side2 = side | 2;

			sides = side2;
			side4 = side2;

			if ( scaledDist[0] < 0.0 )
				side4 = side2 | 4;

			sides = side4;

			if ( side4 )
			{
				for ( i = 0; i <= 2; ++i )
				{
					if ( ((sides >> i) & 1) != 0 )
					{
						edgeIndex = collTtris->edges[i];

						if ( edgeIndex >= 0 )
						{
							edge = &cm.edges[edgeIndex];
							VectorSubtract(top, edge->discEdgeAxis, delta);
							scale = DotProduct(delta, edge->discNormalAxis);
							scale2 = scale - 0.5;

							if ( fabs(scale2) <= 0.5 )
							{
								length = VectorLengthSquared(edge->discNormalAxis);
								scale = scale / length;
								VectorMA(delta, -scale, edge->discNormalAxis, delta);

								if ( tw->radius * tw->radius > VectorLengthSquared(delta) )
								{
									trace->startsolid = 1;
									trace->allsolid = 1;
									trace->fraction = 0.0;
									return;
								}
							}
						}
					}
					else
					{
						vertexId = collTtris->verts[i];

						if ( vertexId >= 0 )
						{
							vertex = cm.verts[vertexId];
							VectorSubtract(top, vertex, delta);

							if ( tw->radius * tw->radius > VectorLengthSquared(delta) )
							{
								trace->startsolid = 1;
								trace->allsolid = 1;
								trace->fraction = 0.0;
								return;
							}
						}
					}
				}
			}
			else
			{
				trace->startsolid = 1;
				trace->allsolid = 1;
				trace->fraction = 0.0;
			}
		}
		else
		{
			offsetPos = offsetZ + offsetZ;
			distToEdge = offsetPos * collTtris->plane[2] + dist;

			if ( radiusNegU < distToEdge )
			{
				cutoffDist = (radiusNegU - dist) / collTtris->plane[2];
				scaledDist2_4 = DotProduct(top, collTtris->svec) - collTtris->svec[3];
				scaledDist2 = DotProduct(top, collTtris->tvec) - collTtris->tvec[3];
				scaledDist[1] = cutoffDist * collTtris->svec[2] + scaledDist2_4;

				if ( scaledDist[1] < 0.0
				        || (scaledDist[0] = cutoffDist * collTtris->tvec[2] + scaledDist2, scaledDist[0] < 0.0)
				        || scaledDist[1] + scaledDist[0] > 1.0 )
				{
					if ( radius <= distToEdge )
						cutoffDist = (radius - dist) / collTtris->plane[2];
					else
						cutoffDist = offsetZ + offsetZ;

					scaledDist[1] = cutoffDist * collTtris->svec[2] + scaledDist2_4;

					if ( scaledDist[1] >= 0.0 )
					{
						scaledDist[0] = cutoffDist * collTtris->tvec[2] + scaledDist2;

						if ( scaledDist[0] >= 0.0 && scaledDist[1] + scaledDist[0] <= 1.0 )
						{
							trace->startsolid = 1;
							trace->allsolid = 1;
							trace->fraction = 0.0;
						}
					}
				}
				else
				{
					trace->startsolid = 1;
					trace->allsolid = 1;
					trace->fraction = 0.0;
				}
			}
		}
	}
}

void CM_PositionTestInAabbTree_r(traceWork_t *tw, CollisionAabbTree_s *aabbTree, trace_t *trace)
{
	int i;
	signed short checkcount;
	CollisionPartition *partition;
	int index;
	CollisionAabbTree_s *tree;
	int count;

	if ( !CM_CullBox(tw, aabbTree->origin, aabbTree->halfSize) )
	{
		if ( aabbTree->childCount )
		{
			count = 0;
			tree = &cm.aabbTrees[aabbTree->u.firstChildIndex];
			while ( count < aabbTree->childCount )
			{
				CM_PositionTestInAabbTree_r(tw, tree, trace);
				++count;
				++tree;
			}
		}
		else
		{
			index = aabbTree->u.firstChildIndex;
			checkcount = tw->threadInfo.checkcount;
			if ( tw->threadInfo.partitions[index] != checkcount )
			{
				tw->threadInfo.partitions[index] = checkcount;
				partition = &cm.partitions[index];
				for ( i = 0; i < partition->triCount; ++i )
					CM_PositionTestCapsuleInTriangle(tw, &partition->triIndices[i], trace);
			}
		}
	}
}

void CM_MeshTestInLeaf(traceWork_t *tw, cLeaf_s *leaf, trace_t *trace)
{
	CollisionAabbTree_s *tree;
	dmaterial_t *material;
	int i;

	for ( i = 0; i < leaf->collAabbCount; ++i )
	{
		tree = &cm.aabbTrees[i + leaf->firstCollAabbIndex];
		material = &cm.materials[tree->materialIndex];
		if ( (tw->contents & material->contentFlags) != 0 )
		{
			CM_PositionTestInAabbTree_r(tw, tree, trace);
			if ( trace->allsolid )
			{
				trace->surfaceFlags = material->surfaceFlags;
				trace->contents = material->contentFlags;
				trace->material = material;
				return;
			}
		}
	}
}

void CM_TracePointThroughTriangle(traceWork_t *tw, CollisionTriangle_s *tri, trace_t *trace)
{
	float v3;
	float v4;
	vec3_t bounds;
	float scale;
	float frac;
	float v8;
	float v9;

	v8 = DotProduct(tw->extents.end, tri->plane) - tri->plane[3];
	if ( v8 < 0.0 )
	{
		v9 = DotProduct(tw->extents.start, tri->plane) - tri->plane[3];
		if ( v9 > 0.0 )
		{
			frac = (v9 - 0.125) / (v9 - v8);
			frac = I_fmax(frac, 0.0);
			if ( frac < trace->fraction )
			{
				scale = v9 / (v9 - v8);
				VectorMA(tw->extents.start, scale, tw->delta, bounds);
				v4 = DotProduct(bounds, tri->svec) - tri->svec[3];
				if ( v4 >= -0.001 && v4 <= 1.001 )
				{
					v3 = DotProduct(bounds, tri->tvec) - tri->tvec[3];
					if ( v3 >= -0.001 && v4 + v3 <= 1.001 )
					{
						trace->fraction = frac;
						VectorCopy(tri->plane, trace->normal);
					}
				}
			}
		}
	}
}
