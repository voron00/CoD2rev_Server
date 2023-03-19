#include "../qcommon/qcommon.h"

#ifdef TESTING_LIBRARY
#define svs (*((serverStatic_t*)( 0x0841FB00 )))
#define sv (*((server_t*)( 0x0842BC80 )))
#else
extern server_t sv;
extern serverStatic_t svs;
#endif

/*
===============
SV_AddEntToSnapshot
===============
*/
void SV_AddEntToSnapshot(  int number, snapshotEntityNumbers_t *eNums )
{
	// if we are full, silently discard entities
	if ( eNums->numSnapshotEntities == MAX_SNAPSHOT_ENTITIES )
	{
		return;
	}

	eNums->snapshotEntities[ eNums->numSnapshotEntities ] = number;
	eNums->numSnapshotEntities++;
}

/*
===============
SV_AddEntitiesVisibleFromPoint
===============
*/
void SV_AddCachedEntitiesVisibleFromPoint(int from_num_entities, int from_first_entity, float *origin, signed int clientNum, snapshotEntityNumbers_t *eNums)
{
	int e, i, l;
	int clientcluster;
	int leafnum, boxleafnums;
	byte    *clientpvs;
	byte    *bitvector;
	float fogOpaqueDistSqrd;
	uint16_t clusternums[128];
	int lastLeaf;
	archivedEntity_t *aent;

	leafnum = CM_PointLeafnum( origin );
	clientcluster = CM_LeafCluster( leafnum );

	if(clientcluster < 0)
	{
		return;
	}

	clientpvs = CM_ClusterPVS( clientcluster );

	fogOpaqueDistSqrd = G_GetFogOpaqueDistSqrd();

	if ( fogOpaqueDistSqrd == 3.4028235e38 )
	{
		fogOpaqueDistSqrd = 0.0;
	}

	int maxCachedSnapshotEntities = 0x4000; // sizeof(svs.cachedSnapshotEntities) / sizeof(svs.cachedSnapshotEntities[0]); NOT for CoD2, CoD2 uses malloc for that

	for ( e = 0 ; e < from_num_entities ; e++ )
	{

		aent = &svs.cachedSnapshotEntities[(e + from_first_entity) % maxCachedSnapshotEntities];
		if ( (1 << (clientNum & 31)) & aent->r.clientMask[clientNum >> 5] || aent->s.number == clientNum )
		{
			continue;
		}

		if ( aent->r.svFlags & 0x18 )
		{
			SV_AddEntToSnapshot( e, eNums );
			continue;
		}

		boxleafnums = CM_BoxLeafnums(aent->r.absmin, aent->r.absmax, clusternums, sizeof(clusternums) / sizeof(clusternums[0]), &lastLeaf);
		if ( !boxleafnums )
		{
			continue;
		}

		bitvector = clientpvs;

		for ( i = 0 ; i < boxleafnums ; i++ )
		{
			l = CM_LeafCluster(clusternums[i]);
			if ( bitvector[l >> 3] & ( 1 << ( l & 7 ) ) && l != -1)
			{
				break;
			}
		}

		if ( i == boxleafnums)
		{
			continue;
		}

		if(!(fogOpaqueDistSqrd == 0.0 || BoxDistSqrdExceeds(aent->r.absmin, aent->r.absmax, origin, fogOpaqueDistSqrd) == qfalse) )
		{
			continue;
		}

		// add it
		SV_AddEntToSnapshot( e, eNums );
	}
}