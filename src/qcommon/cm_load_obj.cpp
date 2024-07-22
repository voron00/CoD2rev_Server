#include "qcommon.h"
#include "cm_local.h"

struct dbrush_t
{
	int16_t numSides;
	int16_t materialNum;
};

struct dbrushside_t
{
	union
	{
		int planeNum;
		float bound;
	};
	int materialNum;
};

/*
=================
CMod_LoadBrushes
=================
*/
#define MIN_NUM_SIDES 6
static void CMod_LoadBrushes()
{
	dbrushside_t *inSides;
	cbrush_t *outBrush;
	int axisIter;
	int index;
	int brushcount;
	dbrush_t *inBrush;
	cbrushside_t *sides;
	int numinsides;
	int sideIter;
	int brushIter;
	int materialNum;

	inBrush = (dbrush_t *)Com_GetBspLump(LUMP_BRUSHES, sizeof(dbrush_t), &brushcount);
	inSides = (dbrushside_t *)Com_GetBspLump(LUMP_BRUSHSIDES, sizeof(dbrushside_t), &numinsides);

	int outnumsides = numinsides - MIN_NUM_SIDES * brushcount;

	if ( outnumsides < 0 )
	{
		Com_Error(ERR_DROP, "CMod_LoadBrushes: bad side count");
	}

	if ( outnumsides )
		sides = (cbrushside_t *)CM_Hunk_Alloc( outnumsides * sizeof( *cm.brushsides ) );
	else
		sides = NULL;

	cm.brushsides = sides;
	cm.numBrushSides = outnumsides;

	cm.brushes = (cbrush_t *)CM_Hunk_Alloc( ( BOX_BRUSHES + brushcount ) * sizeof( *cm.brushes ) );
	cm.numBrushes = brushcount;

	if ( cm.numBrushes != brushcount )
	{
		Com_Error(ERR_DROP, "CMod_LoadBrushes: cm.numBrushes exceeded");
	}

	outBrush = cm.brushes;

	for ( brushIter = 0; brushIter < brushcount; brushIter++, outBrush++, inBrush++ )
	{
		outBrush->numsides = LittleShort(inBrush->numSides) - MIN_NUM_SIDES;

		if ( outBrush->numsides < 0 )
		{
			Com_Error(ERR_DROP, "CMod_LoadBrushes: brush has less than 6 sides");
		}

		outBrush->sides = (outBrush->numsides != 0 ? sides : NULL);

		for ( axisIter = 0; axisIter < 3; axisIter++ )
		{
			for( index = 0; index < 2; index++, inSides++ )
			{
				if ( index )
				{
					outBrush->maxs[axisIter] = LittleFloat(inSides->bound);
				}
				else
				{
					outBrush->mins[axisIter] = LittleFloat(inSides->bound);
				}

				materialNum = LittleLong(inSides->materialNum);

				if ( materialNum < 0 || materialNum >= cm.numMaterials )
				{
					Com_Error(ERR_DROP, "CMod_LoadBrushes: bad materialNum: %i", materialNum);
				}

				outBrush->axialMaterialNum[index][axisIter] = materialNum;

				if ( outBrush->axialMaterialNum[index][axisIter] != materialNum )
				{
					Com_Error(ERR_DROP, "CMod_LoadBrushes: axialMaterialNum exceeded");
				}
			}
		}

		for ( sideIter = 0; sideIter < outBrush->numsides; sideIter++, inSides++, sides++ )
		{
			sides->plane = &cm.planes[LittleLong(inSides->planeNum)];
			sides->materialNum = LittleLong(inSides->materialNum);

			if ( sides->materialNum < 0 || sides->materialNum >= cm.numMaterials )
			{
				Com_Error(ERR_DROP, "CMod_LoadBrushes: bad materialNum: %i", sides->materialNum);
			}
		}

		materialNum = LittleShort(inBrush->materialNum);

		if ( materialNum < 0 || materialNum >= cm.numMaterials )
		{
			Com_Error(ERR_DROP, "CMod_LoadBrushes: bad materialNum: %i", materialNum);
		}

		outBrush->contents = cm.materials[materialNum].contentFlags & ~(CONTENTS_TRANSLUCENT | CONTENTS_NONCOLLIDING);
	}
}

/*
=================
CMod_LoadLeafBrushes
=================
*/
static void CMod_LoadLeafBrushes()
{
	int brushIndex;
	uint16_t *out;
	uint32_t *in;
	int iter;
	int count;

	in = (uint32_t *)Com_GetBspLump(LUMP_LEAFBRUSHES, sizeof(uint32_t), &count);

	// ydnar: more than <count> brushes are stored in leafbrushes...
	cm.leafbrushes = (uint16_t *)CM_Hunk_Alloc( ( BOX_LEAF_BRUSHES + count ) * sizeof( *cm.leafbrushes ) );
	cm.numLeafBrushes = count;

	out = cm.leafbrushes;

	for ( iter = 0; iter < count; iter++, in++, out++ )
	{
		brushIndex = LittleLong(*in);
		*out = brushIndex;

		if ( *out != brushIndex )
		{
			Com_Error(ERR_DROP, "CMod_LoadLeafBrushes: brushIndex exceeded");
		}
	}
}

struct DiskCollAabbTree
{
	vec3_t origin;
	vec3_t halfSize;
	uint16_t materialIndex;
	uint16_t childCount;
	union
	{
		int firstChildIndex;
		int partitionIndex;
	};
};

/*
=================
CMod_LoadCollisionAabbTrees
=================
*/
static void CMod_LoadCollisionAabbTrees()
{
	CollisionAabbTree_s *out;
	DiskCollAabbTree *in;
	int index;
	int count;

	in = (DiskCollAabbTree *)Com_GetBspLump(LUMP_COLLISIONAABBS, sizeof(DiskCollAabbTree), &count);

	cm.aabbTrees = (CollisionAabbTree_s *)CM_Hunk_Alloc( count * sizeof( *cm.aabbTrees ) );
	cm.aabbTreeCount = count;

	out = cm.aabbTrees;

	for ( index = 0; index < count; index++, in++, out++ )
	{
		out->origin[0] = LittleFloat(in->origin[0]);
		out->origin[1] = LittleFloat(in->origin[1]);
		out->origin[2] = LittleFloat(in->origin[2]);

		out->halfSize[0] = LittleFloat(in->halfSize[0]);
		out->halfSize[1] = LittleFloat(in->halfSize[1]);
		out->halfSize[2] = LittleFloat(in->halfSize[2]);

		out->materialIndex = LittleShort(in->materialIndex);
		out->childCount = LittleShort(in->childCount);
		out->u.firstChildIndex = LittleLong(in->firstChildIndex);
	}
}

struct DiskLeafCollision
{
	int cluster;
	int area;
	int firstCollAabbIndex;
	int collAabbCount;
	int firstLeafBrush;
	int numLeafBrushes;
	int cellNum;
	int firstLightIndex;
	int numLights;
};

/*
=================
CMod_LoadLeafs
=================
*/
static void CMod_LoadLeafs(bool usePvs)
{
	int firstCollAabbIndex;
	cLeaf_s *out;
	int cluster;
	int leafIter;
	DiskLeafCollision *in;
	int count;
	int collAabbCount;

	in = (DiskLeafCollision *)Com_GetBspLump(LUMP_LEAFS, sizeof(DiskLeafCollision), &count);

	if ( !count )
	{
		Com_Error(ERR_DROP, "Map with no leafs");
	}

	cm.leafs = (cLeaf_s *)CM_Hunk_Alloc( count * sizeof( *cm.leafs ) );
	cm.numLeafs = count;

	cluster = 0;
	out = cm.leafs;

	for ( leafIter = 0; leafIter < count; leafIter++, in++, out++ )
	{
		if ( usePvs )
		{
			cluster = LittleLong(in->cluster);
			out->cluster = cluster;

			if ( out->cluster != cluster )
			{
				Com_Error(ERR_DROP, "CMod_LoadLeafs: cluster exceeded");
			}
		}

		firstCollAabbIndex = LittleLong(in->firstCollAabbIndex);
		out->firstCollAabbIndex = firstCollAabbIndex;

		if ( out->firstCollAabbIndex != firstCollAabbIndex )
		{
			Com_Error(ERR_DROP, "CMod_LoadLeafs: firstCollAabbIndex exceeded");
		}

		collAabbCount = LittleLong(in->collAabbCount);
		out->collAabbCount = collAabbCount;

		if ( out->collAabbCount != collAabbCount )
		{
			Com_Error(ERR_DROP, "CMod_LoadLeafs: collAabbCount exceeded");
		}

		if ( usePvs && cluster >= cm.numClusters )
		{
			cm.numClusters = cluster + 1;
		}
	}
}

struct DiskBrushModel
{
	vec3_t mins;
	vec3_t maxs;
	int firstTriangle;
	int numTriangles;
	int firstSurface;
	int numSurfaces;
	int firstBrush;
	int numBrushes;
};

/*
=================
CMod_LoadSubmodels
=================
*/
static void CMod_LoadSubmodels()
{
	int firstCollAabbIndex;
	int j;
	cmodel_t *out;
	DiskBrushModel *in;
	int bmodelIndex;
	vec3_t extent;
	int count;
	int collAabbCount;

	in = (DiskBrushModel *)Com_GetBspLump(LUMP_MODELS, sizeof(DiskBrushModel), &count);

	if ( !count )
	{
		Com_Error(ERR_DROP, "Map with no brush models (should at least have the world model)");
	}

	cm.cmodels = (cmodel_t *)CM_Hunk_Alloc( count * sizeof( *cm.cmodels ) );
	cm.numSubModels = count;

	if ( count > MAX_SUBMODELS - 1 )
	{
		Com_Error(ERR_DROP, "MAX_SUBMODELS exceeded");
	}

	for ( bmodelIndex = 0; bmodelIndex < count; bmodelIndex++, in++ )
	{
		out = &cm.cmodels[bmodelIndex];

		for ( j = 0; j < 3; j++ )
		{
			// spread the mins / maxs by a pixel
			out->mins[j] = LittleFloat(in->mins[j]) - 1;
			out->maxs[j] = LittleFloat(in->maxs[j]) + 1;

			extent[j] = I_fmax(fabs(out->mins[j]), fabs(out->maxs[j]));
		}

		out->radius = VectorLength(extent);

		if ( bmodelIndex == 0 )
		{
			continue;   // world model doesn't need other info
		}

		collAabbCount = LittleLong(in->numSurfaces);
		out->leaf.collAabbCount = collAabbCount;

		if ( out->leaf.collAabbCount != collAabbCount )
		{
			Com_Error(ERR_DROP, "CMod_LoadSubmodels: collAabbCount exceeded");
		}

		firstCollAabbIndex = LittleLong(in->firstSurface);
		out->leaf.firstCollAabbIndex = firstCollAabbIndex;

		if ( out->leaf.firstCollAabbIndex != firstCollAabbIndex )
		{
			Com_Error(ERR_DROP, "CMod_LoadSubmodels: firstCollAabbIndex exceeded");
		}
	}
}

/*
=================
CMod_GetLeafTerrainContents
=================
*/
static int CMod_GetLeafTerrainContents(cLeaf_s *leaf)
{
	int contents = 0;

	for ( int k = 0; k < leaf->collAabbCount; k++ )
	{
		contents |= cm.materials[cm.aabbTrees[k + leaf->firstCollAabbIndex].materialIndex].contentFlags;
	}

	return contents;
}

/*
=================
CMod_AllocLeafBrushNode
=================
*/
static cLeafBrushNode_s *CMod_AllocLeafBrushNode()
{
	cLeafBrushNode_s *node;

	node = (cLeafBrushNode_s *)TempMalloc(sizeof(cLeafBrushNode_s));
	Com_Memset(node, 0, sizeof(cLeafBrushNode_s));
	node->data.children.dist = -FLT_MAX;

	return node;
}

/*
=================
CMod_AllocLeafBrushNode
=================
*/
static float CMod_GetPartitionScore(uint16_t *leafBrushes, int numLeafBrushes, int axis, const vec3_t mins, const vec3_t maxs, float *dist)
{
	float max;
	float min;
	cbrush_t *b;
	int k;
	int leftBrushCount;
	int rightBrushCount;

	rightBrushCount = -1;
	leftBrushCount = -1;

	min = -FLT_MAX;
	max = FLT_MAX;

	for ( k = 0; k < numLeafBrushes; k++ )
	{
		b = &cm.brushes[leafBrushes[k]];

		if ( b->mins[axis] < *dist )
		{
			if ( *dist >= (b->maxs[axis] ))
			{
				leftBrushCount++;

				if ( b->maxs[axis] > min )
				{
					min = b->maxs[axis];
				}
			}
		}
		else
		{
			rightBrushCount++;

			if ( max > b->mins[axis] )
			{
				max = b->mins[axis];
			}
		}
	}

	assert(min <= *dist);
	assert(*dist <= max);

	*dist = (min + max) * 0.5;

	if ( I_min(leftBrushCount, rightBrushCount) <= 0 )
	{
		return 0;
	}

	return I_min(leftBrushCount, rightBrushCount) * I_fmin(max - mins[axis], maxs[axis] - min);
}

/*
=================
CMod_PartionLeafBrushes_r
=================
*/
cLeafBrushNode_s* CMod_PartionLeafBrushes_r(uint16_t *leafBrushes, int numLeafBrushes, const vec3_t mins, const vec3_t maxs)
{
	int nodeOffset;
	cLeafBrushNode_s *node;
	int side;
	float testDist;
	int numLeafBrushesChild;
	int testAxis;
	int k;
	float dist;
	float range;
	vec3_t childMaxs;
	cbrush_t *b;
	float bestScore;
	int len;
	vec3_t childMins;
	cLeafBrushNode_s *childNode;
	uint16_t *leafBrushesCopy;
	cLeafBrushNode_s *returnNode;
	int axis;
	int brushnum;
	float score;

	assert(numLeafBrushes);

	node = CMod_AllocLeafBrushNode();

	bestScore = 0.0;
	axis = -1;
	dist = 0.0;

	for ( testAxis = 0; testAxis < 3; testAxis++ )
	{
		for ( k = 0; k < numLeafBrushes; k++ )
		{
			brushnum = leafBrushes[k];
			b = &cm.brushes[brushnum];
			testDist = b->mins[testAxis];

			score = CMod_GetPartitionScore(leafBrushes, numLeafBrushes, testAxis, mins, maxs, &testDist);

			if ( score > bestScore )
			{
				bestScore = score;

				axis = testAxis;
				dist = testDist;
			}

			testDist = b->maxs[testAxis];

			score = CMod_GetPartitionScore(leafBrushes, numLeafBrushes, testAxis, mins, maxs, &testDist);

			if ( score > bestScore )
			{
				bestScore = score;

				axis = testAxis;
				dist = testDist;
			}
		}
	}

	if ( axis >= 0 )
	{
		len = sizeof(*leafBrushes) * numLeafBrushes;
		leafBrushesCopy = (uint16_t *)CM_Hunk_AllocateTempMemoryHigh(sizeof(*leafBrushes) * numLeafBrushes);
		Com_Memcpy(leafBrushesCopy, leafBrushes, len);
		numLeafBrushesChild = 0;

		for ( k = 0; k < numLeafBrushes; k++ )
		{
			brushnum = leafBrushesCopy[k];
			b = &cm.brushes[brushnum];

			if ( cm.brushes[brushnum].mins[axis] < dist && dist < b->maxs[axis] )
			{
				leafBrushes[numLeafBrushesChild] = brushnum;
				numLeafBrushesChild++;
			}
		}

		if ( numLeafBrushesChild )
		{
			returnNode = CMod_PartionLeafBrushes_r(leafBrushes, numLeafBrushesChild, mins, maxs);
			assert(returnNode == node + 1);
			node->leafBrushCount = -1;
			node->contents = returnNode->contents;
			leafBrushes += numLeafBrushesChild;
		}

		range = FLT_MAX;

		node->axis = axis;
		node->data.children.dist = dist;

		side = 0;
nextside:
		if ( side > 1 )
		{
			node->data.children.range = range;
			return node;
		}

		numLeafBrushesChild = 0;

		for ( k = 0; ; k++ )
		{
			if ( k >= numLeafBrushes )
			{
				assert(numLeafBrushesChild);

				VectorCopy(mins, childMins);
				VectorCopy(maxs, childMaxs);

				if ( side )
				{
					childMaxs[axis] = dist - range;
				}
				else
				{
					childMins[axis] = dist + range;
				}

				childNode = CMod_PartionLeafBrushes_r(leafBrushes, numLeafBrushesChild, childMins, childMaxs);
				nodeOffset = childNode - node;
				node->data.children.childOffset[side] = nodeOffset;

				if ( node->data.children.childOffset[side] != nodeOffset )
				{
					Com_Error(ERR_DROP, "CMod_PartionLeafBrushes_r: child exceeded");
				}

				node->contents |= childNode->contents;
				leafBrushes += numLeafBrushesChild;
				side++;

				goto nextside;
			}

			brushnum = leafBrushesCopy[k];
			b = &cm.brushes[brushnum];

			if ( side )
			{
				if ( b->maxs[axis] > dist )
				{
					continue;
				}

				range = I_fmin(range, dist - b->maxs[axis]);
			}
			else
			{
				if ( dist > b->mins[axis] )
				{
					continue;
				}

				range = I_fmin(range, b->mins[axis] - dist);
			}

			leafBrushes[numLeafBrushesChild] = brushnum;
			numLeafBrushesChild++;
		}
	}

	node->leafBrushCount = numLeafBrushes;

	if ( node->leafBrushCount != numLeafBrushes )
	{
		Com_Error(ERR_DROP, "CMod_PartionLeafBrushes_r: leafBrushCount exceeded");
	}

	for ( k = 0; k < numLeafBrushes; k++ )
	{
		brushnum = leafBrushes[k];
		b = &cm.brushes[brushnum];
		node->contents |= cm.brushes[brushnum].contents;
	}

	assert(node->contents);
	node->data.leaf.brushes = leafBrushes;

	return node;
}

/*
=================
CMod_PartionLeafBrushes
=================
*/
static void CMod_PartionLeafBrushes(uint16_t *leafBrushes, int numLeafBrushes, cLeaf_s *leaf)
{
	int j;
	vec3_t mins;
	int k;
	cbrush_t *b;
	vec3_t maxs;
	int brushnum;

	if ( !numLeafBrushes )
	{
		return;
	}

	VectorSet(mins, FLT_MAX, FLT_MAX, FLT_MAX);
	VectorSet(maxs, -FLT_MAX, -FLT_MAX, -FLT_MAX);

	for ( k = 0; k < numLeafBrushes; k++ )
	{
		brushnum = leafBrushes[k];
		b = &cm.brushes[brushnum];

		for ( j = 0; j < 3; j++ )
		{
			if ( mins[j] > b->mins[j] )
			{
				mins[j] = b->mins[j];
			}

			if ( b->maxs[j] > maxs[j] )
			{
				maxs[j] = b->maxs[j];
			}
		}
	}

	VectorCopy(mins, leaf->mins);
	VectorCopy(maxs, leaf->maxs);

	for ( j = 0; j < 3; j++ )
	{
		leaf->mins[j] = leaf->mins[j] - 0.125;
		leaf->maxs[j] = leaf->maxs[j] + 0.125;
	}

	CM_Hunk_CheckTempMemoryHighClear();
	leaf->leafBrushNode = CMod_PartionLeafBrushes_r(leafBrushes, numLeafBrushes, mins, maxs) - cm.leafbrushNodes;
	CM_Hunk_ClearTempMemoryHigh();
}

/*
=================
CMod_LoadLeafBrushNodes
=================
*/
static void CMod_LoadLeafBrushNodes()
{
	int contents;
	cLeaf_s *out;
	int numLeafBrushes;
	int leafIter;
	DiskLeafCollision *in;
	int indexFirstLeafBrush;
	int count;
	int brushIter;
	int brushnum;

	in = (DiskLeafCollision*)Com_GetBspLump(LUMP_LEAFS, sizeof(DiskLeafCollision), &count);
	assert(count == cm.numLeafs);
	out = cm.leafs;

	for ( leafIter = 0; leafIter < cm.numLeafs; leafIter++, in++, out++ )
	{
		numLeafBrushes = LittleLong(in->numLeafBrushes);
		indexFirstLeafBrush = LittleLong(in->firstLeafBrush);
		contents = 0;

		for ( brushIter = 0; brushIter < numLeafBrushes; brushIter++ )
		{
			contents |= cm.brushes[cm.leafbrushes[indexFirstLeafBrush + brushIter]].contents;
		}

		out->brushContents = contents;
		out->terrainContents = CMod_GetLeafTerrainContents(out);

		CMod_PartionLeafBrushes(&cm.leafbrushes[indexFirstLeafBrush], numLeafBrushes, out);
	}
}

/*
=================
CMod_LoadSubmodelBrushNodes
=================
*/
static void CMod_LoadSubmodelBrushNodes()
{
	int contents;
	cmodel_t *out;
	int numLeafBrushes;
	int leafBrushIndex;
	DiskBrushModel *in;
	int bmodelIndex;
	uint16_t *indexes;
	int count;
	int firstBrush;
	int firstBrushIndex;

	in = (DiskBrushModel *)Com_GetBspLump(LUMP_MODELS, sizeof(DiskBrushModel), &count);
	assert(count == cm.numSubModels);

	for ( bmodelIndex = 0; bmodelIndex < cm.numSubModels; bmodelIndex++, in++ )
	{
		if ( !bmodelIndex )
		{
			continue;
		}

		out = &cm.cmodels[bmodelIndex];
		numLeafBrushes = LittleLong(in->numBrushes);

		indexes = (uint16_t *)CM_Hunk_Alloc( numLeafBrushes * sizeof(uint16_t) );
		contents = 0;

		for ( leafBrushIndex = 0; leafBrushIndex < numLeafBrushes; leafBrushIndex++ )
		{
			firstBrushIndex = LittleLong(in->firstBrush);
			indexes[leafBrushIndex] = leafBrushIndex + firstBrushIndex;

			if ( indexes[leafBrushIndex] != leafBrushIndex + firstBrushIndex )
			{
				Com_Error(ERR_DROP, "CMod_LoadSubmodelBrushNodes: leafBrushes exceeded");
			}

			contents |= cm.brushes[leafBrushIndex + firstBrushIndex].contents;
		}

		out->leaf.brushContents = contents;
		out->leaf.terrainContents = CMod_GetLeafTerrainContents(&out->leaf);

		CMod_PartionLeafBrushes(indexes, numLeafBrushes, &out->leaf);
	}
}

/*
===================
CM_InitBoxHull

Set up the planes and nodes so that the six floats of a bounding box
can just be stored out and get a proper clipping hull structure.
===================
*/
static void CM_InitBoxHull( void )
{
	cLeafBrushNode_s *node;

	cm.box_brush = &cm.brushes[cm.numBrushes];

	cm.box_brush->numsides = 0;
	cm.box_brush->sides = 0;
	cm.box_brush->contents = -1;
	cm.box_model.leaf.brushContents = -1;
	cm.box_model.leaf.terrainContents = 0;

	VectorSet(cm.box_model.leaf.mins, FLT_MAX, FLT_MAX, FLT_MAX);
	VectorSet(cm.box_model.leaf.maxs, -FLT_MAX, -FLT_MAX, -FLT_MAX);

	cm.box_brush->axialMaterialNum[0][0] = -1;
	cm.box_brush->axialMaterialNum[0][1] = -1;
	cm.box_brush->axialMaterialNum[0][2] = -1;
	cm.box_brush->axialMaterialNum[1][0] = -1;
	cm.box_brush->axialMaterialNum[1][1] = -1;
	cm.box_brush->axialMaterialNum[1][2] = -1;

	node = CMod_AllocLeafBrushNode();
	cm.box_model.leaf.leafBrushNode = node - cm.leafbrushNodes;

	node->leafBrushCount = 1;
	node->data.leaf.brushes = &cm.leafbrushes[cm.numLeafBrushes];

	cm.leafbrushes[cm.numLeafBrushes] = cm.numBrushes;
}

/*
===================
CMod_LoadBrushRelated
===================
*/
static void CMod_LoadBrushRelated(bool usePvs)
{
	int leafbrushNodesCount;
	cLeafBrushNode_s *leafbrushNodes;

	CMod_LoadBrushes();
	CMod_LoadLeafBrushes();
	CMod_LoadCollisionAabbTrees();
	CMod_LoadLeafs(usePvs);
	CMod_LoadSubmodels();

	TempMemoryReset();

	cm.leafbrushNodes = ((cLeafBrushNode_s*)TempMalloc(0) - 1);

	CMod_LoadLeafBrushNodes();
	CMod_LoadSubmodelBrushNodes();

	CM_InitBoxHull();

	cm.leafbrushNodes++;
	leafbrushNodesCount = ((cLeafBrushNode_s*)TempMalloc(0)) - cm.leafbrushNodes;
	cm.leafbrushNodesCount = leafbrushNodesCount + 1;

	leafbrushNodes = (cLeafBrushNode_s *)CM_Hunk_Alloc(sizeof(cLeafBrushNode_s) * (leafbrushNodesCount + 1));
	Com_Memcpy(&leafbrushNodes[1], cm.leafbrushNodes, sizeof(cLeafBrushNode_s) * leafbrushNodesCount);

	cm.leafbrushNodes = leafbrushNodes;

	Hunk_ClearTempMemoryInternal();
}

struct dplane_t
{
	vec3_t normal;
	float dist;
};

/*
===================
CMod_LoadPlanes
===================
*/
static void CMod_LoadPlanes()
{
	cplane_t *out;
	int planeIter;
	char bits;
	dplane_t *in;
	int axisIter;
	int count;

	in = (dplane_t *)Com_GetBspLump(LUMP_PLANES, sizeof(dplane_t), &count);

	if ( !count )
	{
		Com_Error(ERR_DROP, "Map with no planes");
	}

	cm.planes = (cplane_t *)CM_Hunk_Alloc( count * sizeof( *cm.planes ) );
	cm.planeCount = count;

	for ( planeIter = 0, out = cm.planes; planeIter < count; in++, out++, planeIter++ )
	{
		bits = 0;

		for ( axisIter = 0; axisIter < 3; axisIter++ )
		{
			out->normal[axisIter] = LittleFloat(in->normal[axisIter]);

			if ( out->normal[axisIter] < 0 )
			{
				bits |= 1 << axisIter;
			}
		}

		out->dist = LittleFloat(in->dist);
		out->type = PlaneTypeForNormal(out->normal);
		out->signbits = bits;
	}
}

/*
===================
CMod_LoadMaterials
===================
*/
static void CMod_LoadMaterials()
{
	dmaterial_t *in;
	dmaterial_t *out;
	int count;
	int matIndex;

	in = (dmaterial_t *)Com_GetBspLump(LUMP_MATERIALS, sizeof(dmaterial_t), &count);

	if ( !count )
	{
		Com_Error(ERR_DROP, "Map with no materials");
	}

	cm.materials = (((dmaterial_t *)CM_Hunk_Alloc( sizeof(*cm.materials) * (count + 1))) + 1 );
	cm.numMaterials = count;

	Com_Memcpy( cm.materials, in, count * sizeof(*cm.materials) );

	if ( LittleLong(1) == 1 )
	{
		return;
	}

	out = cm.materials;

	for ( matIndex = 0; matIndex < count; matIndex++, in++, out++ )
	{
		out->contentFlags = LittleLong(out->contentFlags);
		out->surfaceFlags = LittleLong(out->surfaceFlags);
	}
}

struct dnode_t
{
	int planeNum;
	int children[2];
	int mins[3];
	int maxs[3];
};

/*
===================
CMod_LoadNodes
===================
*/
static void CMod_LoadNodes()
{
	int child;
	int j;
	cNode_t *out;
	dnode_t *in;
	int nodeIter;
	int count;

	in = (dnode_t *)Com_GetBspLump(LUMP_NODES, sizeof(dnode_t), &count);

	if ( !count )
	{
		Com_Error(ERR_DROP, "Map has no nodes");
	}

	cm.nodes = (cNode_t *)CM_Hunk_Alloc( count * sizeof( *cm.nodes ) );
	cm.numNodes = count;

	out = cm.nodes;

	for ( nodeIter = 0; nodeIter < count; nodeIter++, in++, out++ )
	{
		out->plane = &cm.planes[LittleLong(in->planeNum)];

		for ( j = 0; j < 2; j++ )
		{
			child = LittleLong(in->children[j]);
			out->children[j] = child;

			if ( out->children[j] != child )
			{
				Com_Error(ERR_DROP, "CMod_LoadNodes: children exceeded");
			}
		}
	}
}

/*
===================
CMod_LoadLeafSurfaces
===================
*/
static void CMod_LoadLeafSurfaces()
{
	int *in;
	int *out;
	int count;
	int i;

	in = (int *)Com_GetBspLump(LUMP_LEAFSURFACES, sizeof(int), &count);

	cm.leafsurfaces = (int *)CM_Hunk_Alloc( count * sizeof( *cm.leafsurfaces ) );
	cm.numLeafSurfaces = count;

	out = cm.leafsurfaces;

	for ( i = 0 ; i < count ; i++, in++, out++ )
	{
		*out = LittleLong( *in );
	}
}

/*
===================
CMod_LoadCollisionVerts
===================
*/
static void CMod_LoadCollisionVerts()
{
	vec4_t *in;
	vec3_t *out;
	int vertIter;
	int count;

	in = (vec4_t *)Com_GetBspLump(LUMP_COLLISIONVERTS, sizeof(vec4_t), &count);

	cm.verts = (vec3_t *)CM_Hunk_Alloc( count * sizeof( *cm.verts ) );
	cm.vertCount = count;

	out = cm.verts;

	for ( vertIter = 0; vertIter < count; vertIter++, in++, out++ )
	{
		out[0][0] = LittleFloat(in[0][1]);
		out[0][1] = LittleFloat(in[0][2]);
		out[0][2] = LittleFloat(in[0][3]);
	}
}

struct EdgeInfo
{
	float discriminant;
	vec3_t origin;
	vec3_t axis[3];
	float discNormalDist;
};

/*
===================
CMod_LoadCollisionEdges
===================
*/
static void CMod_LoadCollisionEdges()
{
	CollisionEdge_s *out;
	int edgeIter;
	EdgeInfo *in;
	int count;
	float dist;

	in = (EdgeInfo *)Com_GetBspLump(LUMP_COLLISIONEDGES, sizeof(EdgeInfo), &count);

	cm.edges = (CollisionEdge_s *)CM_Hunk_Alloc( count * sizeof( *cm.edges ) );
	cm.edgeCount = count;

	out = cm.edges;

	for ( edgeIter = 0; edgeIter < count; edgeIter++, in++, out++ )
	{
		out->origin[0] = LittleFloat(in->origin[0]);
		out->origin[1] = LittleFloat(in->origin[1]);
		out->origin[2] = LittleFloat(in->origin[2]);

		out->axis[0][0] = LittleFloat(in->axis[0][0]);
		out->axis[0][1] = LittleFloat(in->axis[0][1]);
		out->axis[0][2] = LittleFloat(in->axis[0][2]);

		out->axis[1][0] = LittleFloat(in->axis[1][0]);
		out->axis[1][1] = LittleFloat(in->axis[1][1]);
		out->axis[1][2] = LittleFloat(in->axis[1][2]);

		out->axis[2][0] = LittleFloat(in->axis[2][0]);
		out->axis[2][1] = LittleFloat(in->axis[2][1]);
		out->axis[2][2] = LittleFloat(in->axis[2][2]);

		dist = 1.0 / LittleFloat(in->discNormalDist);
		VectorScale(out->axis[2], dist, out->axis[2]);
	}
}

/*
===================
CMod_LoadCollisionTriangles
===================
*/
static void CMod_LoadCollisionTriangles()
{
	CollisionTriangle_s *out;
	int triIter;
	CollisionTriangle_s *in;
	int count;
	int j;

	in = (CollisionTriangle_s *)Com_GetBspLump(LUMP_COLLISIONTRIS, sizeof(CollisionTriangle_s), &count);

	cm.tris = (CollisionTriangle_s *)CM_Hunk_Alloc( count * sizeof( *cm.tris ) );
	cm.triCount = count;

	out = cm.tris;

	for ( triIter = 0; triIter < count; triIter++, in++, out++ )
	{
		out->plane[0] = LittleFloat(in->plane[0]);
		out->plane[1] = LittleFloat(in->plane[1]);
		out->plane[2] = LittleFloat(in->plane[2]);
		out->plane[3] = LittleFloat(in->plane[3]);

		out->svec[0] = LittleFloat(in->svec[0]);
		out->svec[1] = LittleFloat(in->svec[1]);
		out->svec[2] = LittleFloat(in->svec[2]);
		out->svec[3] = LittleFloat(in->svec[3]);

		out->tvec[0] = LittleFloat(in->tvec[0]);
		out->tvec[1] = LittleFloat(in->tvec[1]);
		out->tvec[2] = LittleFloat(in->tvec[2]);
		out->tvec[3] = LittleFloat(in->tvec[3]);

		for ( j = 0; j < 3; ++j )
		{
			out->edges[j] = LittleLong(in->edges[j]);
			out->verts[j] = LittleLong(in->verts[j]);
		}
	}
}

struct DiskCollBorder
{
	float distEq[3];
	float zBase;
	float zSlope;
	float start;
	float length;
};

/*
===================
CMod_LoadCollisionBorders
===================
*/
static void CMod_LoadCollisionBorders()
{
	CollisionBorder *out;
	DiskCollBorder *in;
	int index;
	int count;

	in = (DiskCollBorder *)Com_GetBspLump(LUMP_COLLISIONBORDERS, sizeof(DiskCollBorder), &count);

	cm.borders = (CollisionBorder *)CM_Hunk_Alloc( count * sizeof( *cm.borders ) );
	cm.borderCount = count;

	out = cm.borders;

	for ( index = 0; index < count; index++, in++, out++ )
	{
		out->distEq[0] = LittleFloat(in->distEq[0]);
		out->distEq[1] = LittleFloat(in->distEq[1]);
		out->distEq[2] = LittleFloat(in->distEq[2]);

		out->zBase = LittleFloat(in->zBase);
		out->zSlope = LittleFloat(in->zSlope);
		out->start = LittleFloat(in->start);
		out->length = LittleFloat(in->length);
	}
}

struct DiskCollPartition
{
	uint16_t checkStamp;
	byte triCount;
	byte borderCount;
	int firstTriIndex;
	int firstBorderIndex;
};

/*
===================
CMod_LoadCollisionPartitions
===================
*/
static void CMod_LoadCollisionPartitions()
{
	CollisionPartition *out;
	DiskCollPartition *in;
	int index;
	int count;

	in = (DiskCollPartition *)Com_GetBspLump(LUMP_COLLISIONPARTITIONS, sizeof(DiskCollPartition), &count);

	cm.partitions = (CollisionPartition *)CM_Hunk_Alloc( count * sizeof( *cm.partitions ) );
	cm.partitionCount = count;

	out = cm.partitions;

	for ( index = 0; index < count; index++, in++, out++ )
	{
		out->triCount = in->triCount;
		out->borderCount = in->borderCount;

		out->tris = &cm.tris[LittleLong(in->firstTriIndex)];
		out->borders = &cm.borders[LittleLong(in->firstBorderIndex)];
	}
}

/*
===================
CMod_LoadVisibility
===================
*/
#define VIS_HEADER  8
static void CMod_LoadVisibility()
{
	int len;
	byte    *buf;

	buf = Com_GetBspLump(LUMP_VISIBILITY, sizeof(byte), &len);

	if ( !len )
	{
		cm.clusterBytes = ( cm.numClusters + 31 ) & ~31;
		cm.visibility = (byte *)Hunk_Alloc( cm.clusterBytes );
		Com_Memset( cm.visibility, 255, cm.clusterBytes );
		return;
	}

	cm.vised = qtrue;
	cm.visibility = (byte *)Hunk_Alloc( len );
	cm.numClusters = LittleLong( ( (int *)buf )[0] );
	cm.clusterBytes = LittleLong( ( (int *)buf )[1] );
	Com_Memcpy( cm.visibility, buf + VIS_HEADER, len - VIS_HEADER );
}

/*
===================
CMod_LoadEntityString
===================
*/
static void CMod_LoadEntityString()
{
	char *entityString;
	int count;

	entityString = (char *)Com_GetBspLump(LUMP_ENTITIES, sizeof(char), &count);

	cm.numEntityChars = count;
	cm.entityString = (char *)CM_Hunk_Alloc(count);
	Com_Memcpy(cm.entityString, entityString, count);
}

/*
===================
CM_LoadMapFromBsp
===================
*/
void CM_LoadMapFromBsp(const char *name, bool usePvs)
{
	dheader_t *header;

	// free old stuff
	Com_Memset( &cm, 0, sizeof( cm ) );

	cm.name = (char *)CM_Hunk_Alloc(strlen(name) + 1);
	strcpy(cm.name, name);

	header = Com_GetBsp(0, &cm.checksum);
	cm.header = header;

	// load into heap
	CMod_LoadMaterials();
	CMod_LoadPlanes();
	CMod_LoadBrushRelated(usePvs);
	CMod_LoadNodes();
	CMod_LoadLeafSurfaces();
	CMod_LoadCollisionVerts();
	CMod_LoadCollisionEdges();
	CMod_LoadCollisionTriangles();
	CMod_LoadCollisionBorders();
	CMod_LoadCollisionPartitions();

	if ( usePvs )
	{
		CMod_LoadVisibility();
	}
	else if ( Com_BspHasLump(LUMP_VISIBILITY) )
	{
		Com_Error(ERR_DROP, "In single player, do not compile the bsp with visibility");
	}

	CMod_LoadEntityString();

	cm.header = NULL;
}