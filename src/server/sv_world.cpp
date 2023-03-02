#include "../qcommon/qcommon.h"

vec2_t actorLocationalMins = { -64.0, -32.0 };
vec2_t actorLocationalMaxs = { 64.0, 72.0 };

clipHandle_t SV_ClipHandleForEntity(gentity_s *touch)
{
	if ( touch->r.bmodel )
		return touch->s.index;
	else
		return CM_TempBoxModel(touch->r.mins, touch->r.maxs, touch->r.contents);
}

#define MAX_TOTAL_ENT_LEAFS     128
void SV_LinkEntity( gentity_t *gEnt )
{
	uint16_t leafs[MAX_TOTAL_ENT_LEAFS];
	int cluster;
	int num_leafs;
	int i, j, k;
	int lastLeaf;
	float       *origin, *angles;
	struct svEntity_s  *ent;
	struct DObj_s* dobj;
	vec3_t min, max;
	clipHandle_t clip;

	assert(gEnt->r.inuse);

	ent = SV_SvEntityForGentity( gEnt );
	/*
		// Ridah, sanity check for possible currentOrigin being reset bug
		if ( !gEnt->r.bmodel && VectorCompare( gEnt->r.currentOrigin, vec3_origin ) ) {
			Com_DPrintf( "WARNING: BBOX entity is being linked at world origin, this is probably a bug\n" );
		}
		if ( ent->worldSector ) {
			SV_UnlinkEntity( gEnt );    // unlink from old position
		}
	*/
	// encode the size into the entityState_t for client prediction
	if ( gEnt->r.bmodel )
	{
		gEnt->s.solid = SOLID_BMODEL;       // a solid_box will never create this value
	}
	else if ( gEnt->r.contents & ( CONTENTS_SOLID | CONTENTS_BODY ) )
	{
		// assume that x/y are equal and symetric
		i = gEnt->r.maxs[0];
		if ( i < 1 )
		{
			i = 1;
		}
		if ( i > 255 )
		{
			i = 255;
		}

		// z is not symetric
		j = ( 1.0 - gEnt->r.mins[2] );
		if ( j < 1 )
		{
			j = 1;
		}
		if ( j > 255 )
		{
			j = 255;
		}

		// and z maxs can be negative...
		k = ( gEnt->r.maxs[2] + 32 );
		if ( k < 1 )
		{
			k = 1;
		}
		if ( k > 255 )
		{
			k = 255;
		}
		gEnt->s.solid = ( k << 16 ) | ( j << 8 ) | i;
		/*
		BLACKOPS
				if ( gEnt->s.solid == 0xFFFFFF )
				{
					gEnt->s.solid = 1;
				}
		*/
	}
	else
	{
		gEnt->s.solid = 0;
	}

	// get the position
	origin = gEnt->r.currentOrigin;
	angles = gEnt->r.currentAngles;

	SnapAngles(angles);

	// set the abs box
	if ( gEnt->r.bmodel && ( angles[0] || angles[1] || angles[2] ) )
	{
		// expand for rotation
		float max;
		int i;

		max = RadiusFromBounds( gEnt->r.mins, gEnt->r.maxs );
		for ( i = 0 ; i < 3 ; i++ )
		{
			gEnt->r.absmin[i] = origin[i] - max;
			gEnt->r.absmax[i] = origin[i] + max;
		}
	}
	else
	{
		// normal
		VectorAdd( origin, gEnt->r.mins, gEnt->r.absmin );
		VectorAdd( origin, gEnt->r.maxs, gEnt->r.absmax );
	}

	// because movement is clipped an epsilon away from an actual edge,
	// we must fully check even when bounding boxes don't quite touch
	gEnt->r.absmin[0] -= 1;
	gEnt->r.absmin[1] -= 1;
	gEnt->r.absmin[2] -= 1;
	gEnt->r.absmax[0] += 1;
	gEnt->r.absmax[1] += 1;
	gEnt->r.absmax[2] += 1;

	// link to PVS leafs
	ent->numClusters = 0;
	ent->lastCluster = 0;

	if ( !(gEnt->r.svFlags & 0x19) )
	{
		//get all leafs, including solids
		num_leafs = CM_BoxLeafnums( gEnt->r.absmin, gEnt->r.absmax, leafs, MAX_TOTAL_ENT_LEAFS, &lastLeaf );

		// if none of the leafs were inside the map, the
		// entity is outside the world and can be considered unlinked
		if ( !num_leafs )
		{
			CM_UnlinkEntity(ent);
			return;
		}

		// store as many explicit clusters as we can
		for ( i = 0 ; i < num_leafs ; i++ )
		{
			cluster = CM_LeafCluster( leafs[i] );
			if ( cluster != -1 )
			{
				ent->clusternums[ent->numClusters++] = cluster;
				if ( ent->numClusters == MAX_ENT_CLUSTERS )
				{
					break;
				}
			}
		}

		// store off a last cluster if we need to
		if ( i != num_leafs )
		{
			ent->lastCluster = CM_LeafCluster( lastLeaf );
		}
	}
	gEnt->r.linked = qtrue;

	if ( !gEnt->r.contents )
	{
		CM_UnlinkEntity(ent);
		return;
	}
	clip = SV_ClipHandleForEntity(gEnt);
	dobj = Com_GetServerDObj(gEnt->s.number);
	if ( dobj && gEnt->r.svFlags & 6 )
	{
		if ( gEnt->r.svFlags & 2 )
		{
			VectorAdd(origin, actorLocationalMins, min);
			VectorAdd(origin, actorLocationalMaxs, max);
		}
		else
		{
			DObjGetBounds(dobj, min, max);
			VectorAdd(origin, min, min);
			VectorAdd(origin, max, max);
		}
		CM_LinkEntity(ent, min, max, clip);
	}
	else
	{
		CM_LinkEntity(ent, gEnt->r.absmin, gEnt->r.absmax, clip);
	}
}

void SV_UnlinkEntity( gentity_t *gEnt )
{
	svEntity_t *ent;

	ent = SV_SvEntityForGentity(gEnt);
	gEnt->r.linked = 0;
	CM_UnlinkEntity(ent);
}