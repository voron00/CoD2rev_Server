#include "qcommon.h"
#include "cm_local.h"

void CM_PositionTestCapsuleInTriangle(traceWork_t *tw, CollisionTriangle_s *collTtris, trace_t *trace)
{
	float absScale;
	int side4;
	int side2;
	int side;
	float offsetZ;
	float length;
	vec2_t scaledDist2;
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
	vec2_t scaledDist;
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
				for ( i = 0; i < 3; ++i )
				{
					if ( ((sides >> i) & 1) != 0 )
					{
						edgeIndex = collTtris->edges[i];

						if ( edgeIndex >= 0 )
						{
							edge = &cm.edges[edgeIndex];
							VectorSubtract(top, edge->origin, delta);
							scale = DotProduct(delta, edge->axis[2]);
							absScale = scale - 0.5;

							if ( fabs(absScale) <= 0.5 )
							{
								length = VectorLengthSquared(edge->axis[2]);
								scale = scale / length;
								VectorMA(delta, -scale, edge->axis[2], delta);

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
				scaledDist2[1] = DotProduct(top, collTtris->svec) - collTtris->svec[3];
				scaledDist2[0] = DotProduct(top, collTtris->tvec) - collTtris->tvec[3];
				scaledDist[1] = cutoffDist * collTtris->svec[2] + scaledDist2[1];

				if ( scaledDist[1] < 0.0
				        || (scaledDist[0] = cutoffDist * collTtris->tvec[2] + scaledDist2[0], scaledDist[0] < 0.0)
				        || scaledDist[1] + scaledDist[0] > 1.0 )
				{
					if ( radius <= distToEdge )
						cutoffDist = (radius - dist) / collTtris->plane[2];
					else
						cutoffDist = offsetZ + offsetZ;

					scaledDist[1] = cutoffDist * collTtris->svec[2] + scaledDist2[1];

					if ( scaledDist[1] >= 0.0 )
					{
						scaledDist[0] = cutoffDist * collTtris->tvec[2] + scaledDist2[0];

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

void CM_TraceCapsuleThroughBorder(traceWork_t *tw, CollisionBorder *border, trace_t *trace)
{
	float ePos;
	float zStart;
	float zPos;
	float negDelta;
	float negDot;
	vec3_t edgePoint;
	float c;
	float discriminant;
	float offsetLenSq;
	float deltaDotOffset;
	vec3_t offset;
	float zLen;
	vec3_t endpos;
	float length;
	float t;
	float traceDeltaDot;
	float tDot;
	float radius;

	traceDeltaDot = Dot2Product(tw->delta, border->distEq);

	if ( traceDeltaDot >= 0.0 )
		return;

	radius = tw->radius + 0.125;
	tDot = Dot2Product(tw->extents.start, border->distEq) - border->distEq[2];
	t = (radius - tDot) / traceDeltaDot;

	if ( t >= trace->fraction || -radius > t * tw->deltaLen )
		return;

	VectorMA(tw->extents.start, t, tw->delta, endpos);
	length = border->distEq[1] * endpos[0] - border->distEq[0] * endpos[1] - border->start;

	if ( length < 0.0 )
	{
		edgePoint[0] = border->distEq[1] * border->start + border->distEq[0] * border->distEq[2];
		edgePoint[1] = border->distEq[1] * border->distEq[2] - border->distEq[0] * border->start;

		Vector2Subtract(tw->extents.start, edgePoint, offset);
		deltaDotOffset = Dot2Product(tw->delta, offset);

		if ( deltaDotOffset >= 0.0 )
			return;

		offsetLenSq = Dot2Product(offset, offset);
		c = offsetLenSq - radius * radius;

		if ( c < 0.0 )
		{
			edgePoint[2] = border->zBase;
			ePos = edgePoint[2] - tw->extents.start[2];

			if ( fabs(ePos) <= tw->offsetZ )
			{
				VectorSet(trace->normal, border->distEq[0], border->distEq[1], 0.0);
				trace->fraction = 0.0;

				if ( tw->radius * tw->radius > offsetLenSq )
					trace->startsolid = 1;
			}
			return;
		}

		discriminant = deltaDotOffset * deltaDotOffset - tw->deltaLenSq * c;

		if ( discriminant < 0.0 )
			return;

		negDot = -deltaDotOffset;
		t = (negDot - sqrt(discriminant)) / tw->deltaLenSq;

		if ( t >= trace->fraction || t <= 0.0 )
			return;

		VectorMA(tw->extents.start, t, tw->delta, endpos);
		length = 0.0;
		goto out;
	}

	if ( length <= border->length )
	{
		if ( t < 0.0 )
			t = 0.0;
out:
		zLen = length * border->zSlope + border->zBase;
		zPos = endpos[2] - zLen;

		if ( fabs(zPos) <= tw->offsetZ )
		{
			trace->fraction = t;
			VectorSet(trace->normal, border->distEq[0], border->distEq[1], 0.0);
		}

		return;
	}

	edgePoint[0] = (border->start + border->length) * border->distEq[1] + border->distEq[0] * border->distEq[2];
	edgePoint[1] = border->distEq[1] * border->distEq[2] - (border->start + border->length) * border->distEq[0];

	Vector2Subtract(tw->extents.start, edgePoint, offset);
	deltaDotOffset = Dot2Product(tw->delta, offset);

	if ( deltaDotOffset >= 0.0 )
		return;

	offsetLenSq = Dot2Product(offset, offset);
	c = offsetLenSq - radius * radius;

	if ( c >= 0.0 )
	{
		discriminant = deltaDotOffset * deltaDotOffset - tw->deltaLenSq * c;

		if ( discriminant < 0.0 )
			return;

		negDelta = -deltaDotOffset;
		t = (negDelta - sqrt(discriminant)) / tw->deltaLenSq;

		if ( t >= trace->fraction || t <= 0.0 )
			return;

		VectorMA(tw->extents.start, t, tw->delta, endpos);
		length = border->length;
		goto out;
	}

	edgePoint[2] = border->zSlope * border->length + border->zBase;
	zStart = tw->extents.start[2] - edgePoint[2];

	if ( fabs(zStart) <= tw->offsetZ )
	{
		VectorSet(trace->normal, border->distEq[0], border->distEq[1], 0.0);
		trace->fraction = 0.0;

		if ( tw->radius * tw->radius > offsetLenSq )
			trace->startsolid = 1;
	}
}

void CM_TracePointThroughTriangle(traceWork_t *tw, CollisionTriangle_s *tri, trace_t *trace)
{
	float dot2;
	float dot2d2;
	vec3_t v_start;
	float scale;
	float frac;
	float dot;
	float dot2d;

	dot = DotProduct(tw->extents.end, tri->plane) - tri->plane[3];

	if ( dot < 0.0 )
	{
		dot2d = DotProduct(tw->extents.start, tri->plane) - tri->plane[3];

		if ( dot2d > 0.0 )
		{
			frac = (dot2d - 0.125) / (dot2d - dot);
			frac = I_fmax(frac, 0.0);

			if ( frac < trace->fraction )
			{
				scale = dot2d / (dot2d - dot);
				VectorMA(tw->extents.start, scale, tw->delta, v_start);
				dot2d2 = DotProduct(v_start, tri->svec) - tri->svec[3];

				if ( dot2d2 >= -0.001 && dot2d2 <= 1.001 )
				{
					dot2 = DotProduct(v_start, tri->tvec) - tri->tvec[3];

					if ( dot2 >= -0.001 && dot2d2 + dot2 <= 1.001 )
					{
						trace->fraction = frac;
						VectorCopy(tri->plane, trace->normal);
					}
				}
			}
		}
	}
}

void CM_TraceCapsuleThroughTriangle(traceWork_t *tw, CollisionTriangle_s *tri, float offsetZ, trace_t *trace)
{
	float dotLenSq;
	float squaredDot;
	float absDot;
	float sphereScale;
	float distScale;
	int side4;
	int side2;
	float dist;
	float tVecDot;
	float sVecDot;
	float areaX2;
	float triNormalScaledByAreaX2;
	vec3_t tracePlaneScaledNormal;
	vec3_t start_v;
	float *verts;
	int vertId;
	CollisionEdge_s *edge;
	int edgeIndex;
	float dot2Offset;
	float dot3;
	float len;
	float dotLen;
	vec2_t scaledDist;
	float dot;
	vec2_t scaledDist2;
	vec3_t shiftedStart;
	float dot2;
	int side;
	float tVecScale;
	float fracScale;
	vec3_t sphereStart;
	float frac;
	float dot4;
	float planeDot;
	float offset;
	float offsetNegU;
	int i;

	VectorCopy(tw->extents.end, tracePlaneScaledNormal);
	tracePlaneScaledNormal[2] = tracePlaneScaledNormal[2] - offsetZ;
	offset = tw->radius + 0.125;
	dot4 = DotProduct(tracePlaneScaledNormal, tri->plane) - tri->plane[3];

	if ( dot4 < offset )
	{
		VectorCopy(tw->extents.start, start_v);
		start_v[2] = start_v[2] - offsetZ;
		planeDot = DotProduct(start_v, tri->plane) - tri->plane[3];
		dist = planeDot - dot4;

		if ( dist > 0.000099999997 )
		{
			offsetNegU = -offset;

			if ( -offset < planeDot )
			{
				if ( planeDot - offset > 0.0 )
				{
					frac = (planeDot - offset) / dist;

					if ( frac > trace->fraction )
						return;

					VectorMA(start_v, frac, tw->delta, sphereStart);
				}
				else
				{
					frac = 0.0;
					VectorCopy(start_v, sphereStart);
				}

				fracScale = DotProduct(sphereStart, tri->svec) - tri->svec[3];
				tVecScale = DotProduct(sphereStart, tri->tvec) - tri->tvec[3];
				side = fracScale + tVecScale > 1.0;
				side2 = side;

				if ( fracScale < 0.0 )
					side2 = side | 2;

				side = side2;
				side4 = side2;

				if ( tVecScale < 0.0 )
					side4 = side2 | 4;

				side = side4;

				if ( side4 )
				{
					for ( i = 0; i < 3; ++i )
					{
						if ( ((side >> i) & 1) != 0 )
						{
							edgeIndex = tri->edges[i];

							if ( edgeIndex >= 0 && tw->threadInfo.edges[edgeIndex] != tw->threadInfo.checkcount )
							{
								tw->threadInfo.edges[edgeIndex] = tw->threadInfo.checkcount;
								edge = &cm.edges[edgeIndex];
								VectorSubtract(start_v, edge->origin, shiftedStart);
								scaledDist[0] = DotProduct(shiftedStart, edge->axis[0]);
								scaledDist[1] = DotProduct(shiftedStart, edge->axis[1]);
								scaledDist2[0] = DotProduct(tw->delta, edge->axis[0]);
								scaledDist2[1] = DotProduct(tw->delta, edge->axis[1]);
								dot3 = Dot2Product(scaledDist2, scaledDist);

								if ( dot3 < 0.0 )
								{
									dot2 = Dot2Product(scaledDist, scaledDist);
									dot2Offset = dot2 - offset * offset;

									if ( dot2Offset > 0.0 )
									{
										len = Vec2Multiply(scaledDist2);
										dotLen = dot3 * dot3 - len * dot2Offset;

										if ( dotLen > 0.0 )
										{
											dotLenSq = sqrt(dotLen);
											frac = (-dotLenSq - dot3) / len;

											if ( trace->fraction > frac )
											{
												VectorMA(shiftedStart, frac, tw->delta, shiftedStart);
												sphereStart[2] = DotProduct(shiftedStart, edge->axis[2]);
												sphereScale = sphereStart[2] - 0.5;

												if ( fabs(sphereScale) <= 0.5 )
												{
													sphereStart[0] = (frac * scaledDist2[0] + scaledDist[0]) / offset;
													sphereStart[1] = (frac * scaledDist2[1] + scaledDist[1]) / offset;
													VectorScale(edge->axis[0], sphereStart[0], trace->normal);
													VectorMA(trace->normal, sphereStart[1], edge->axis[1], trace->normal);

													if ( tri->plane[2] >= 0.69999999
													        && trace->normal[2] >= 0.0
													        && trace->normal[2] < 0.69999999
													        && start_v[2] > tracePlaneScaledNormal[2] )
													{
														VectorCopy(tri->plane, trace->normal);
													}

													trace->fraction = frac;
												}
											}
										}
									}
									else
									{
										dot = DotProduct(shiftedStart, edge->axis[2]);
										absDot = dot - 0.5;

										if ( fabs(absDot) <= 0.5 )
										{
											VectorScale(edge->axis[0], scaledDist[0], trace->normal);
											VectorMA(trace->normal, scaledDist[1], edge->axis[1], trace->normal);
											Vec3Normalize(trace->normal);

											if ( tri->plane[2] >= 0.69999999
											        && trace->normal[2] >= 0.0
											        && trace->normal[2] < 0.69999999
											        && start_v[2] > tracePlaneScaledNormal[2] )
											{
												VectorCopy(tri->plane, trace->normal);
											}

											trace->fraction = 0.0;

											if ( tw->radius * tw->radius > dot2 )
												trace->startsolid = 1;

											return;
										}
									}
								}
							}
						}
						else
						{
							vertId = tri->verts[i];

							if ( vertId >= 0 && tw->threadInfo.verts[vertId] != tw->threadInfo.checkcount )
							{
								tw->threadInfo.verts[vertId] = tw->threadInfo.checkcount;
								verts = cm.verts[vertId];
								VectorSubtract(start_v, verts, shiftedStart);
								dot3 = DotProduct(tw->delta, shiftedStart);

								if ( dot3 < 0.0 )
								{
									dot2 = DotProduct(shiftedStart, shiftedStart);
									dot2Offset = dot2 - offset * offset;

									if ( dot2Offset <= 0.0 )
									{
										frac = 1.0 / sqrt(dot2);
										VectorScale(shiftedStart, frac, trace->normal);

										if ( tri->plane[2] >= 0.69999999
										        && trace->normal[2] >= 0.0
										        && trace->normal[2] < 0.69999999
										        && start_v[2] > tracePlaneScaledNormal[2] )
										{
											VectorCopy(tri->plane, trace->normal);
										}

										trace->fraction = 0.0;

										if ( tw->radius * tw->radius > dot2 )
											trace->startsolid = 1;

										return;
									}

									len = tw->deltaLenSq;
									dotLen = dot3 * dot3 - len * dot2Offset;

									if ( dotLen >= 0.0 )
									{
										squaredDot = sqrt(dotLen);
										frac = (-squaredDot - dot3) / len;

										if ( trace->fraction > frac )
										{
											VectorMA(shiftedStart, frac, tw->delta, trace->normal);
											distScale = 1.0 / offset;
											VectorScale(trace->normal, distScale, trace->normal);

											if ( tri->plane[2] >= 0.69999999
											        && trace->normal[2] >= 0.0
											        && trace->normal[2] < 0.69999999
											        && start_v[2] > tracePlaneScaledNormal[2] )
											{
												VectorCopy(tri->plane, trace->normal);
											}

											trace->fraction = frac;
										}
									}
								}
							}
						}
					}
				}
				else
				{
					VectorCopy(tri->plane, trace->normal);
					trace->fraction = frac;

					if ( tw->radius > planeDot )
						trace->startsolid = 1;
				}
			}
			else
			{
				areaX2 = offsetZ + offsetZ;
				triNormalScaledByAreaX2 = areaX2 * tri->plane[2] + planeDot;

				if ( offsetNegU < triNormalScaledByAreaX2 )
				{
					frac = (offsetNegU - planeDot) / tri->plane[2];
					sVecDot = DotProduct(start_v, tri->svec) - tri->svec[3];
					tVecDot = DotProduct(start_v, tri->tvec) - tri->tvec[3];
					fracScale = frac * tri->svec[2] + sVecDot;

					if ( fracScale >= 0.0 )
					{
						tVecScale = frac * tri->tvec[2] + tVecDot;

						if ( tVecScale >= 0.0 && fracScale + tVecScale <= 1.0 )
							goto out;
					}

					frac = offset <= triNormalScaledByAreaX2
					       ? (offset - planeDot) / tri->plane[2]
					       : offsetZ + offsetZ;

					fracScale = frac * tri->svec[2] + sVecDot;

					if ( fracScale >= 0.0 )
					{
						tVecScale = frac * tri->tvec[2] + tVecDot;

						if ( tVecScale >= 0.0 && fracScale + tVecScale <= 1.0 )
						{
out:
							VectorCopy(tri->plane, trace->normal);
							trace->fraction = 0.0;
							trace->startsolid = 1;
						}
					}
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

void CM_TraceThroughAabbTree_r(traceWork_t *tw, CollisionAabbTree_s *aabbTree, trace_t *trace)
{
	CollisionTriangle_s *collTri;
	int i;
	int j;
	int k;
	signed short checkcount;
	CollisionPartition *partition;
	int treeIndex;
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
				CM_TraceThroughAabbTree_r(tw, tree, trace);
				++count;
				++tree;
			}
		}
		else
		{
			treeIndex = aabbTree->u.firstChildIndex;
			checkcount = tw->threadInfo.checkcount;

			if ( tw->threadInfo.partitions[treeIndex] != checkcount )
			{
				tw->threadInfo.partitions[treeIndex] = checkcount;
				partition = &cm.partitions[treeIndex];

				if ( tw->isPoint )
				{
					for ( i = 0; i < partition->triCount; ++i )
						CM_TracePointThroughTriangle(tw, &partition->triIndices[i], trace);
				}
				else
				{
					for ( j = 0; j < partition->triCount; ++j )
					{
						collTri = &partition->triIndices[j];
						CM_TraceCapsuleThroughTriangle(tw, collTri, tw->offsetZ, trace);

						if ( collTri->plane[2] < 0.0 )
							CM_TraceCapsuleThroughTriangle(tw, collTri, -tw->offsetZ, trace);
					}

					if ( (tw->delta[0] != 0.0 || tw->delta[1] != 0.0) && tw->offsetZ != 0.0 )
					{
						for ( k = 0; k < partition->borderCount; ++k )
							CM_TraceCapsuleThroughBorder(tw, &partition->borders[k], trace);
					}
				}
			}
		}
	}
}

void CM_TraceThroughAabbTree(traceWork_t *tw, CollisionAabbTree_s *aabbTree, trace_t *trace)
{
	dmaterial_t *material;
	float fraction;

	material = &cm.materials[aabbTree->materialIndex];

	if ( (tw->contents & material->contentFlags) != 0 )
	{
		fraction = trace->fraction;
		CM_TraceThroughAabbTree_r(tw, aabbTree, trace);

		if ( fraction > trace->fraction )
		{
			trace->surfaceFlags = material->surfaceFlags;
			trace->contents = material->contentFlags;
			trace->material = material;
		}
	}
}