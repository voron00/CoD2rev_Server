#include "qcommon.h"
#include "cm_local.h"

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