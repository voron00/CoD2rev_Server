#include "qcommon.h"
#include "cm_local.h"

#ifdef TESTING_LIBRARY
#define cm (*((clipMap_t*)( 0x08185BE0 )))
#define cme (*((clipMapExtra_t*)( 0x08185CF4 )))
#else
extern clipMap_t cm;
extern clipMapExtra_t cme;
#endif

struct dbrush_t
{
	uint16_t numSides;
	uint16_t materialNum;
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

void CMod_LoadBrushes()
{
	dbrushside_t *inSides;
	cbrush_t *outBrush;
	unsigned int axisIter;
	signed int index;
	unsigned int materialnum;
	unsigned int brushcount;
	dbrush_t *inBrush;
	cbrushside_t *sides;
	unsigned int numinsides;
	unsigned int sideIter;
	unsigned int brushIter;

	inBrush = (dbrush_t *)Com_GetBspLump(LUMP_BRUSHES, sizeof(dbrush_t), &brushcount);
	inSides = (dbrushside_t *)Com_GetBspLump(LUMP_BRUSHSIDES, sizeof(dbrushside_t), &numinsides);

	unsigned int outnumsides = numinsides - 6 * brushcount;

	if ( outnumsides < 0 )
		Com_Error(ERR_DROP, "CMod_LoadBrushes: bad side count");

	if ( outnumsides )
		sides = (cbrushside_t *)CM_Hunk_Alloc(sizeof(cbrushside_t) * outnumsides);
	else
		sides = NULL;

	cm.brushsides = sides;
	cm.numBrushSides = outnumsides;
	cm.brushes = (cbrush_t *)CM_Hunk_Alloc(sizeof(cbrush_t) * (brushcount + 1));
	cm.numBrushes = brushcount;

	if ( (uint16_t)brushcount != brushcount )
		Com_Error(ERR_DROP, "CMod_LoadBrushes: cm.numBrushes exceeded");

	outBrush = cm.brushes;

	for (brushIter = 0; brushIter < brushcount; ++brushIter)
	{
		outBrush->numsides = inBrush->numSides - 6;

		if ( (outBrush->numsides & 0x80000000) != 0 )
			Com_Error(ERR_DROP, "CMod_LoadBrushes: brush has less than 6 sides");

		outBrush->sides = (outBrush->numsides != 0 ? sides : NULL);

		for ( axisIter = 0; axisIter < 3; ++axisIter )
		{
			for( index = 0; index < 2; ++index )
			{
				if ( index )
				{
					outBrush->maxs[axisIter] = inSides->bound;
				}
				else
				{
					outBrush->mins[axisIter] = inSides->bound;
				}

				if ( (unsigned)inSides->materialNum < 0 || (unsigned)inSides->materialNum >= cm.numMaterials )
				{
					Com_Error(ERR_DROP, "CMod_LoadBrushes: bad materialNum: %i", inSides->materialNum);
				}

				outBrush->axialMaterialNum[index][axisIter] = inSides->materialNum;

				if ( (int16_t)(inSides->materialNum) != inSides->materialNum )
				{
					Com_Error(ERR_DROP, "CMod_LoadBrushes: axialMaterialNum exceeded");
				}

				++inSides;
			}
		}

		for (sideIter = 0; sideIter < outBrush->numsides; ++sideIter)
		{
			sides->plane = &cme.planes[inSides->planeNum];
			sides->materialNum = inSides->materialNum;

			if ( (sides->materialNum & 0x80000000) != 0 || sides->materialNum >= cm.numMaterials )
				Com_Error(ERR_DROP, "CMod_LoadBrushes: bad materialNum: %d brushIter", sides->materialNum);

			++inSides;
			++sides;
		}

		materialnum = inBrush->materialNum;

		if ( materialnum < 0 || materialnum >= cm.numMaterials )
			Com_Error(ERR_DROP, "CMod_LoadBrushes: bad materialNum: %d brushIter", inBrush->materialNum);

		++inBrush;
		outBrush->contents = cm.materials[materialnum].contentFlags & 0xDFFFFFFB;
		++outBrush;
	}
}

void CMod_LoadLeafBrushes()
{
	unsigned int brushIndex;
	uint16_t *out;
	uint32_t *in;
	unsigned int iter;
	unsigned int count;

	in = (uint32_t *)Com_GetBspLump(LUMP_LEAFBRUSHES, sizeof(uint32_t), &count);

	cm.leafbrushes = (uint16_t *)CM_Hunk_Alloc(sizeof(uint16_t) * count + 2);
	cm.numLeafBrushes = count;
	out = cm.leafbrushes;
	iter = 0;

	while ( iter < count )
	{
		brushIndex = *in;
		*out = *in;

		if ( *out != brushIndex )
		{
			Com_Error(ERR_DROP, "CMod_LoadLeafBrushes: brushIndex exceeded");
		}

		++iter;
		++in;
		++out;
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

void CMod_LoadCollisionAabbTrees()
{
	CollisionAabbTree_s *out;
	DiskCollAabbTree *in;
	unsigned int index;
	unsigned int count;

	in = (DiskCollAabbTree *)Com_GetBspLump(LUMP_COLLISIONAABBS, sizeof(DiskCollAabbTree), &count);

	cm.aabbTrees = (CollisionAabbTree_s *)CM_Hunk_Alloc(sizeof(CollisionAabbTree_s) * count);
	cm.aabbTreeCount = count;
	out = cm.aabbTrees;
	index = 0;

	while ( index < count )
	{
		out->origin[0] = in->origin[0];
		out->origin[1] = in->origin[1];
		out->origin[2] = in->origin[2];
		out->halfSize[0] = in->halfSize[0];
		out->halfSize[1] = in->halfSize[1];
		out->halfSize[2] = in->halfSize[2];
		out->materialIndex = in->materialIndex;
		out->childCount = in->childCount;
		out->u.firstChildIndex = in->firstChildIndex;
		++index;
		++in;
		++out;
	}
}

struct DiskLeaf
{
	int cluster;
	int unused0;
	int firstCollAabbIndex;
	int collAabbCount;
	int firstLeafBrush;
	int numLeafBrushes;
	int cellNum;
	int unused1;
	int unused2;
};

void CMod_LoadLeafs(bool usePvs)
{
	int firstCollAabbIndex;
	cLeaf_s *out;
	int cluster;
	unsigned int leafIter;
	DiskLeaf *in;
	unsigned int count;
	int collAabbCount;

	in = (DiskLeaf *)Com_GetBspLump(LUMP_LEAFS, sizeof(DiskLeaf), &count);

	if ( !count )
	{
		Com_Error(ERR_DROP, "Map with no leafs");
	}

	cm.leafs = (cLeaf_s *)CM_Hunk_Alloc(sizeof(cLeaf_s) * count);
	cm.numLeafs = count;
	cluster = 0;
	out = cm.leafs;
	leafIter = 0;

	while ( leafIter < count )
	{
		if ( usePvs )
		{
			cluster = in->cluster;
			out->cluster = cluster;

			if ( out->cluster != cluster )
			{
				Com_Error(ERR_DROP, "CMod_LoadLeafs: cluster exceeded");
			}
		}

		firstCollAabbIndex = in->firstCollAabbIndex;
		out->firstCollAabbIndex = firstCollAabbIndex;

		if ( out->firstCollAabbIndex != firstCollAabbIndex )
		{
			Com_Error(ERR_DROP, "CMod_LoadLeafs: firstCollAabbIndex exceeded");
		}

		collAabbCount = in->collAabbCount;
		out->collAabbCount = collAabbCount;

		if ( out->collAabbCount != collAabbCount )
		{
			Com_Error(ERR_DROP, "CMod_LoadLeafs: collAabbCount exceeded");
		}

		if ( usePvs && cluster >= cm.numClusters )
		{
			cm.numClusters = cluster + 1;
		}

		++leafIter;
		++in;
		++out;
	}
}

struct DiskBrushModel
{
	float mins[3];
	float maxs[3];
	unsigned int firstTriSoup;
	unsigned int triSoupCount;
	int firstSurface;
	int numSurfaces;
	int firstBrush;
	int numBrushes;
};

void CMod_LoadSubmodels()
{
	int firstCollAabbIndex;
	int j;
	cmodel_t *out;
	DiskBrushModel *in;
	unsigned int bmodelIndex;
	vec3_t extent;
	unsigned int count;
	int collAabbCount;

	in = (DiskBrushModel *)Com_GetBspLump(LUMP_MODELS, sizeof(DiskBrushModel), &count);

	if ( !count )
	{
		Com_Error(ERR_DROP, "Map with no brush models (should at least have the world model)");
	}

	cm.cmodels = (cmodel_t *)CM_Hunk_Alloc(sizeof(cmodel_t) * count);
	cm.numSubModels = count;

	if ( count > MAX_SUBMODELS - 1 )
	{
		Com_Error(ERR_DROP, "MAX_SUBMODELS exceeded");
	}

	bmodelIndex = 0;

	while ( bmodelIndex < count )
	{
		out = &cm.cmodels[bmodelIndex];

		for ( j = 0; j < 3; ++j )
		{
			out->mins[j] = in->mins[j] - 1.0;
			out->maxs[j] = in->maxs[j] + 1.0;

			if ( abs(out->mins[j]) - abs(out->maxs[j]) < 0.0 )
			{
				extent[j] = abs(out->maxs[j]);
			}
			else
			{
				extent[j] = abs(out->mins[j]);
			}
		}

		out->radius = VectorLength(extent);

		if ( bmodelIndex )
		{
			collAabbCount = in->numSurfaces;
			out->leaf.collAabbCount = collAabbCount;

			if ( out->leaf.collAabbCount != collAabbCount )
			{
				Com_Error(ERR_DROP, "CMod_LoadSubmodels: collAabbCount exceeded");
			}

			firstCollAabbIndex = in->firstSurface;
			out->leaf.firstCollAabbIndex = firstCollAabbIndex;

			if ( out->leaf.firstCollAabbIndex != firstCollAabbIndex )
			{
				Com_Error(ERR_DROP, "CMod_LoadSubmodels: firstCollAabbIndex exceeded");
			}
		}

		++bmodelIndex;
		++in;
	}
}

int CMod_GetLeafTerrainContents(cLeaf_s *leaf)
{
	int contents;
	int k;

	contents = 0;

	for ( k = 0; k < leaf->collAabbCount; ++k )
	{
		contents |= cm.materials[cm.aabbTrees[k + leaf->firstCollAabbIndex].materialIndex].contentFlags;
	}

	return contents;
}

cLeafBrushNode_s *CMod_AllocLeafBrushNode()
{
	cLeafBrushNode_s *node;

	node = (cLeafBrushNode_s *)TempMalloc(sizeof(cLeafBrushNode_s));
	memset(node, 0, sizeof(cLeafBrushNode_s));
	node->data.children.dist = -3.4028235e38;
	return node;
}

double CMod_GetPartitionScore(uint16_t *leafBrushes, int numLeafBrushes, int axis, const float *mins, const float *maxs, float *dist)
{
	signed int bc;
	float max;
	int rightBrushCount;
	int k;
	cbrush_t *b;
	float min;
	int leftBrushCount;

	rightBrushCount = -1;
	leftBrushCount = -1;

	min = -3.4028235e38;
	max = 3.4028235e38;

	for ( k = 0; k < numLeafBrushes; ++k )
	{
		b = &cm.brushes[leafBrushes[k]];

		if ( b->mins[axis] < *dist )
		{
			if ( *dist >= b->maxs[axis] )
			{
				++leftBrushCount;

				if ( b->maxs[axis] > min )
				{
					min = b->maxs[axis];
				}
			}
		}
		else
		{
			++rightBrushCount;

			if ( max > b->mins[axis] )
			{
				max = b->mins[axis];
			}
		}
	}

	if ( rightBrushCount < leftBrushCount )
	{
		bc = rightBrushCount;
	}
	else
	{
		bc = leftBrushCount;
	}

	*dist = (min + max) * 0.5;

	if ( bc <= 0 )
	{
		return 0.0;
	}

	if ( (float)((float)(maxs[axis] - min) - (float)(max - mins[axis])) < 0.0 )
	{
		return bc * (maxs[axis] - min);
	}

	return bc * (max - mins[axis]);
}

cLeafBrushNode_s * CMod_PartionLeafBrushes_r(uint16_t *leafBrushes, int numLeafBrushes, const float *mins, const float *maxs)
{
	int nodeOffset;
	cLeafBrushNode_s *node;
	int side;
	float testDist;
	int numLeafBrushesChild;
	unsigned int testAxis;
	int k;
	float dist;
	float range;
	float childMaxs[3];
	cbrush_t *b;
	float bestScore;
	int len;
	float childMins[3];
	cLeafBrushNode_s *childNode;
	uint16_t *leafBrushesCopy;
	cLeafBrushNode_s *returnNode;
	int axis;
	int brushnum;
	float score;

	node = CMod_AllocLeafBrushNode();
	bestScore = 0.0;
	axis = -1;
	dist = 0.0;

	for ( testAxis = 0; testAxis < 3; ++testAxis )
	{
		for ( k = 0; k < numLeafBrushes; ++k )
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
		len = sizeof(uint16_t) * numLeafBrushes;
		leafBrushesCopy = (uint16_t *)CM_Hunk_AllocateTempMemoryHigh(sizeof(uint16_t) * numLeafBrushes);
		Com_Memcpy(leafBrushesCopy, leafBrushes, len);
		numLeafBrushesChild = 0;

		for ( k = 0; k < numLeafBrushes; ++k )
		{
			brushnum = leafBrushesCopy[k];
			b = &cm.brushes[brushnum];

			if ( cm.brushes[brushnum].mins[axis] < dist && dist < b->maxs[axis] )
			{
				leafBrushes[numLeafBrushesChild++] = brushnum;
			}
		}

		if ( numLeafBrushesChild )
		{
			returnNode = CMod_PartionLeafBrushes_r(leafBrushes, numLeafBrushesChild, mins, maxs);
			node->leafBrushCount = -1;
			node->contents = returnNode->contents;
			leafBrushes += numLeafBrushesChild;
		}

		range = 3.4028235e38;
		node->axis = axis;
		node->data.children.dist = dist;
		side = 0;
LABEL_27:
		if ( side > 1 )
		{
			node->data.children.range = range;
			return node;
		}

		numLeafBrushesChild = 0;

		for ( k = 0; ; ++k )
		{
			if ( k >= numLeafBrushes )
			{
				childMins[0] = mins[0];
				childMins[1] = mins[1];
				childMins[2] = mins[2];
				childMaxs[0] = maxs[0];
				childMaxs[1] = maxs[1];
				childMaxs[2] = maxs[2];

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
				++side;
				goto LABEL_27;
			}

			brushnum = leafBrushesCopy[k];
			b = &cm.brushes[brushnum];

			if ( side )
			{
				if ( b->maxs[axis] > dist )
				{
					continue;
				}

				if ( ((dist - b->maxs[axis]) - range) < 0.0 )
				{
					range = dist - b->maxs[axis];
				}
			}
			else
			{
				if ( dist > b->mins[axis] )
				{
					continue;
				}

				if ( ((b->mins[axis] - dist) - range) < 0.0 )
				{
					range = b->mins[axis] - dist;
				}
			}

			leafBrushes[numLeafBrushesChild++] = brushnum;
		}
	}

	node->leafBrushCount = numLeafBrushes;

	if ( node->leafBrushCount != numLeafBrushes )
	{
		Com_Error(ERR_DROP, "CMod_PartionLeafBrushes_r: leafBrushCount exceeded");
	}

	for ( k = 0; k < numLeafBrushes; ++k )
	{
		brushnum = leafBrushes[k];
		b = &cm.brushes[brushnum];
		node->contents |= cm.brushes[brushnum].contents;
	}

	node->data.leaf.brushes = leafBrushes;
	return node;
}

void CMod_PartionLeafBrushes(uint16_t *leafBrushes, int numLeafBrushes, cLeaf_s *leaf)
{
	int j;
	vec3_t mins;
	int k;
	cbrush_t *b;
	vec3_t maxs;
	int brushnum;

	if ( numLeafBrushes )
	{
		VectorSet(mins, 3.4028235e38, 3.4028235e38, 3.4028235e38);
		VectorSet(maxs, -3.4028235e38, -3.4028235e38, -3.4028235e38);

		for ( k = 0; k < numLeafBrushes; ++k )
		{
			brushnum = leafBrushes[k];
			b = &cm.brushes[brushnum];

			for ( j = 0; j < 3; ++j )
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

		for ( j = 0; j < 3; ++j )
		{
			leaf->mins[j] = leaf->mins[j] - 0.125;
			leaf->maxs[j] = leaf->maxs[j] + 0.125;
		}

		CM_Hunk_CheckTempMemoryHighClear();
		leaf->leafBrushNode = CMod_PartionLeafBrushes_r(leafBrushes, numLeafBrushes, mins, maxs) - cm.leafbrushNodes;
		CM_Hunk_ClearTempMemoryHigh();
	}
}

void CMod_LoadLeafBrushNodes()
{
	int contents;
	cLeaf_s *out;
	unsigned int numLeafBrushes;
	unsigned int leafIter;
	DiskLeaf *in;
	int indexFirstLeafBrush;
	unsigned int count;
	unsigned int brushIter;
	int brushnum;

	in = (DiskLeaf*)Com_GetBspLump(LUMP_LEAFS, sizeof(DiskLeaf), &count);
	out = cm.leafs;

	for (leafIter = 0; leafIter < cm.numLeafs; ++leafIter )
	{
		numLeafBrushes = in->numLeafBrushes;
		indexFirstLeafBrush = in->firstLeafBrush;
		contents = 0;

		for ( brushIter = 0; brushIter < numLeafBrushes; ++brushIter )
		{
			brushnum = cm.leafbrushes[brushIter + indexFirstLeafBrush];
			contents |= cm.brushes[brushnum].contents;
		}

		out->brushContents = contents;
		out->terrainContents = CMod_GetLeafTerrainContents(out);
		CMod_PartionLeafBrushes(&cm.leafbrushes[indexFirstLeafBrush], numLeafBrushes, out);
		++in;
		++out;
	}
}

void CMod_LoadSubmodelBrushNodes()
{
	int contents;
	cmodel_t *out;
	int numLeafBrushes;
	int leafBrushIndex;
	DiskBrushModel *in;
	unsigned int bmodelIndex;
	uint16_t *indexes;
	unsigned int count;
	int firstBrush;

	in = (DiskBrushModel *)Com_GetBspLump(LUMP_MODELS, sizeof(DiskBrushModel), &count);

	++in;
	bmodelIndex = 1;

	while ( bmodelIndex < cm.numSubModels )
	{
		out = &cm.cmodels[bmodelIndex];
		numLeafBrushes = in->numBrushes;
		indexes = (uint16_t *)CM_Hunk_Alloc(sizeof(uint16_t) * numLeafBrushes);
		contents = 0;

		for ( leafBrushIndex = 0; leafBrushIndex < numLeafBrushes; ++leafBrushIndex )
		{
			firstBrush = leafBrushIndex + in->firstBrush;
			indexes[leafBrushIndex] = firstBrush;

			if ( indexes[leafBrushIndex] != firstBrush )
			{
				Com_Error(ERR_DROP, "CMod_LoadSubmodelBrushNodes: leafBrushes exceeded");
			}

			contents |= cm.brushes[firstBrush].contents;
		}

		out->leaf.brushContents = contents;
		out->leaf.terrainContents = CMod_GetLeafTerrainContents(&out->leaf);
		CMod_PartionLeafBrushes(indexes, numLeafBrushes, &out->leaf);
		++bmodelIndex;
		++in;
	}
}

void CM_InitBoxHull()
{
	cLeafBrushNode_s *node;

	cm.box_brush = &cm.brushes[cm.numBrushes];
	cm.box_brush->numsides = 0;
	cm.box_brush->sides = 0;
	cm.box_brush->contents = -1;
	cm.box_model.leaf.brushContents = -1;
	cm.box_model.leaf.terrainContents = 0;
	cm.box_model.leaf.mins[0] = 3.4028235e38;
	cm.box_model.leaf.mins[1] = 3.4028235e38;
	cm.box_model.leaf.mins[2] = 3.4028235e38;
	cm.box_model.leaf.maxs[0] = -3.4028235e38;
	cm.box_model.leaf.maxs[1] = -3.4028235e38;
	cm.box_model.leaf.maxs[2] = -3.4028235e38;
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

void CMod_LoadBrushRelated(bool usePvs)
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
	++cm.leafbrushNodes;
	leafbrushNodesCount = ((cLeafBrushNode_s*)TempMalloc(0)) - cm.leafbrushNodes;
	cm.leafbrushNodesCount = leafbrushNodesCount + 1;
	leafbrushNodes = (cLeafBrushNode_s *)CM_Hunk_Alloc(sizeof(cLeafBrushNode_s) * (leafbrushNodesCount + 1));
	Com_Memcpy(&leafbrushNodes[1], cm.leafbrushNodes, sizeof(cLeafBrushNode_s) * leafbrushNodesCount);
	cm.leafbrushNodes = leafbrushNodes;
	Hunk_ClearTempMemoryInternal();
}

struct dplane_t
{
	float normal[3];
	float dist;
};

void CMod_LoadPlanes()
{
	cplane_s *out;
	unsigned int planeIter;
	char bits;
	dplane_t *in;
	unsigned int axisIter;
	unsigned int count;

	in = (dplane_t *)Com_GetBspLump(LUMP_PLANES, sizeof(dplane_t), &count);

	if ( !count )
	{
		Com_Error(ERR_DROP, "Map with no planes");
	}

	cme.planes = (cplane_s *)CM_Hunk_Alloc(sizeof(cplane_s) * count);
	cme.planeCount = count;

	for (planeIter = 0, out = cme.planes ; planeIter < count; ++in, ++out, ++planeIter )
	{
		bits = 0;

		for ( axisIter = 0; axisIter < 3; ++axisIter )
		{
			out->normal[axisIter] = in->normal[axisIter];

			if ( out->normal[axisIter] < 0.0 )
			{
				bits |= 1 << axisIter;
			}
		}

		out->dist = in->dist;
		out->type = PlaneTypeForNormal(out->normal);
		out->signbits = bits;
	}
}

void CMod_LoadMaterials()
{
	dmaterial_t *in;
	unsigned int count;

	in = (dmaterial_t *)Com_GetBspLump(LUMP_MATERIALS, sizeof(dmaterial_t), &count);

	if ( !count )
	{
		Com_Error(ERR_DROP, "Map with no materials");
	}

	cm.materials = (dmaterial_t *)CM_Hunk_Alloc(sizeof(dmaterial_t) * (count + 1));
	cm.numMaterials = count;
	Com_Memcpy(cm.materials, in, sizeof(dmaterial_t) * count);
}

struct dnode_t
{
	int planeNum;
	int children[2];
	int mins[3];
	int maxs[3];
};

void CMod_LoadNodes()
{
	int child;
	int j;
	cNode_t *out;
	dnode_t *in;
	unsigned int nodeIter;
	unsigned int count;

	in = (dnode_t *)Com_GetBspLump(LUMP_NODES, sizeof(dnode_t), &count);

	if ( !count )
	{
		Com_Error(ERR_DROP, "Map has no nodes");
	}

	cm.nodes = (cNode_t *)CM_Hunk_Alloc(sizeof(cNode_t) * count);
	cm.numNodes = count;
	out = cm.nodes;
	nodeIter = 0;

	while ( nodeIter < count )
	{
		out->plane = &cme.planes[in->planeNum];

		for ( j = 0; j < 2; ++j )
		{
			child = in->children[j];
			out->children[j] = child;

			if ( out->children[j] != child )
			{
				Com_Error(ERR_DROP, "CMod_LoadNodes: children exceeded");
			}
		}

		++nodeIter;
		++out;
		++in;
	}
}

void CMod_LoadLeafSurfaces()
{
	unsigned int *in;
	unsigned int count;

	in = (unsigned int *)Com_GetBspLump(LUMP_LEAFSURFACES, sizeof(uint32_t), &count);
	cm.leafsurfaces = (unsigned int *)CM_Hunk_Alloc(sizeof(uint32_t) * count);
	cm.numLeafSurfaces = count;
	Com_Memcpy(cm.leafsurfaces, in, sizeof(uint32_t) * count);
}

void CMod_LoadCollisionVerts()
{
	vec3_t *out;
	unsigned int vertIter;
	vec4_t *in;
	unsigned int count;

	in = (vec4_t *)Com_GetBspLump(LUMP_COLLISIONVERTS, sizeof(vec4_t), &count);

	cm.verts = (vec3_t *)CM_Hunk_Alloc(sizeof(vec3_t) * count);
	cm.vertCount = count;

	for (vertIter = 0, out = cm.verts ; vertIter < count; ++in, ++out, ++vertIter )
	{
		(*out)[0] = (*in)[1];
		(*out)[1] = (*in)[2];
		(*out)[2] = (*in)[3];
	}
}

struct dedge_t
{
	float discriminant;
	vec3_t discEdgeAxis;
	vec3_t midpoint;
	vec3_t start_v;
	vec3_t discNormalAxis;
	float discNormalDist;
};

void CMod_LoadCollisionEdges()
{
	CollisionEdge_s *out;
	unsigned int edgeIter;
	dedge_t *in;
	unsigned int count;
	float normal;

	in = (dedge_t *)Com_GetBspLump(LUMP_COLLISIONEDGES, sizeof(dedge_t), &count);

	cm.edges = (CollisionEdge_s *)CM_Hunk_Alloc(sizeof(CollisionEdge_s) * count);
	cm.edgeCount = count;

	for (edgeIter = 0, out = cm.edges ; edgeIter < count; ++in, ++out, ++edgeIter )
	{
		VectorCopy(in->discEdgeAxis, out->discEdgeAxis);
		VectorCopy(in->midpoint, out->midpoint);
		VectorCopy(in->start_v, out->start_v);
		VectorCopy(in->discNormalAxis, out->discNormalAxis);

		normal = 1.0 / in->discNormalDist;
		VectorScale(out->discNormalAxis, normal, out->discNormalAxis);
	}
}

void CMod_LoadCollisionTriangles()
{
	CollisionTriangle_s *out;
	unsigned int triIter;
	CollisionTriangle_s *in;
	unsigned int count;
	int j;

	in = (CollisionTriangle_s *)Com_GetBspLump(LUMP_COLLISIONTRIS, sizeof(CollisionTriangle_s), &count);

	cm.triIndices = (CollisionTriangle_s *)CM_Hunk_Alloc(sizeof(CollisionTriangle_s) * count);
	cm.triCount = count;

	for (triIter = 0, out = cm.triIndices ; triIter < count; ++in, ++out, ++triIter )
	{
		Vector4Copy(in->plane, out->plane);
		Vector4Copy(in->svec, out->svec);
		Vector4Copy(in->tvec, out->tvec);

		for (j = 0; j < 3; ++j)
		{
			out->edges[j] = in->edges[j];
			out->verts[j] = in->verts[j];
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

void CMod_LoadCollisionBorders()
{
	CollisionBorder *out;
	DiskCollBorder *in;
	unsigned int index;
	unsigned int count;

	in = (DiskCollBorder *)Com_GetBspLump(LUMP_COLLISIONBORDERS, sizeof(DiskCollBorder), &count);
	cm.borders = (CollisionBorder *)CM_Hunk_Alloc(sizeof(CollisionBorder) * count);
	cm.borderCount = count;
	out = cm.borders;
	index = 0;

	while ( index < count )
	{
		out->distEq[0] = in->distEq[0];
		out->distEq[1] = in->distEq[1];
		out->distEq[2] = in->distEq[2];
		out->zBase = in->zBase;
		out->zSlope = in->zSlope;
		out->start = in->start;
		out->length = in->length;
		++index;
		++in;
		++out;
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

void CMod_LoadCollisionPartitions()
{
	CollisionPartition *out;
	DiskCollPartition *in;
	unsigned int index;
	unsigned int count;

	in = (DiskCollPartition *)Com_GetBspLump(LUMP_COLLISIONPARTITIONS, sizeof(DiskCollPartition), &count);
	cm.partitions = (CollisionPartition *)CM_Hunk_Alloc(sizeof(CollisionPartition) * count);
	cm.partitionCount = count;
	out = cm.partitions;
	index = 0;

	while ( index < count )
	{
		out->triCount = in->triCount;
		out->borderCount = in->borderCount;
		out->triIndices = &cm.triIndices[in->firstTriIndex];
		out->borders = &cm.borders[in->firstBorderIndex];

		++index;
		++in;
		++out;
	}
}

#define VIS_HEADER  8
void CMod_LoadVisibility()
{
	unsigned int len;
	byte    *buf;

	buf = Com_GetBspLump(LUMP_VISIBILITY, 1u, &len);

	if ( !len )
	{
		cm.clusterBytes = (cm.numClusters + 31) & ~31;
		cm.numClusters = 1;
		cm.visibility = (byte*)CM_Hunk_Alloc(cm.clusterBytes);
		Com_Memset( cm.visibility, 0xFF, cm.clusterBytes );
		return;
	}

	cm.vised = qtrue;
	cm.visibility = (byte*)CM_Hunk_Alloc( len - VIS_HEADER );
	cm.numClusters = ( (int *)buf )[0];
	cm.clusterBytes = ( (int *)buf )[1];

	Com_Memcpy( cm.visibility, buf + VIS_HEADER, len - VIS_HEADER );
}

void CMod_LoadEntityString()
{
	char *entityString;
	unsigned int count;

	entityString = (char*)Com_GetBspLump(LUMP_ENTITIES, sizeof(char), &count);
	cm.numEntityChars = count;
	cm.entityString = (char *)CM_Hunk_Alloc(count);
	Com_Memcpy(cm.entityString, entityString, count);
}

void CM_LoadMapFromBsp(const char *name, bool usePvs)
{
	BspHeader *header;

	Com_Memset(&cm, 0, sizeof(cm)); // 0x110
	Com_Memset(&cm, 0, sizeof(cme)); // 0xC VoroN: In CoD2 there is another struct right after cm that contatins planes and a bsp header.
	cm.name = (char *)CM_Hunk_Alloc(strlen(name) + 1);
	strcpy(cm.name, name);
	header = Com_GetBspHeader(0, &cm.checksum);
	cme.header = header;
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

	cme.header = 0;
}