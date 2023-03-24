#include "../qcommon/qcommon.h"

#ifdef TESTING_LIBRARY
#define svs (*((serverStatic_t*)( 0x0841FB00 )))
#define sv (*((server_t*)( 0x0842BC80 )))
#else
extern server_t sv;
extern serverStatic_t svs;
#endif

vec3_t actorLocationalMins = { -64.0, -64.0, -32.0 };
vec3_t actorLocationalMaxs = { 64.0, 64.0, 72.0 };

clipHandle_t SV_ClipHandleForEntity(gentity_s *touch)
{
	if ( touch->r.bmodel )
		return touch->s.index;
	else
		return CM_TempBoxModel(touch->r.mins, touch->r.maxs, touch->r.contents);
}

void SV_LinkEntity(gentity_s *ent)
{
	svEntity_t *writeEnt;
	int j;
	int i;
	float max;
	float max2d;
	clipHandle_t handle;
	vec3_t maxs;
	vec3_t mins;
	DObj_s *obj;
	svEntity_t *svEnt;
	float *angles;
	float *currentOrigin;
	int lastLeaf;
	int c;
	int b;
	int a;
	int num_leafs;
	int cluster;
	int leafs[128];

	svEnt = SV_SvEntityForGentity(ent);

	if ( ent->r.bmodel )
	{
		ent->s.solid = 0xFFFFFF;
	}
	else if ( (ent->r.contents & 0x2000001) != 0 )
	{
		a = (int)ent->r.maxs[0];

		if ( a <= 0 )
			a = 1;

		if ( a > 255 )
			a = 255;

		b = (int)(1.0 - ent->r.mins[2]);

		if ( b <= 0 )
			b = 1;

		if ( b > 255 )
			b = 255;

		c = (int)(ent->r.maxs[2] + 32.0);

		if ( c <= 0 )
			c = 1;

		if ( c > 255 )
			c = 255;

		ent->s.solid = a | (c << 16) | (b << 8);
	}
	else
	{
		ent->s.solid = 0;
	}

	angles = ent->r.currentAngles;
	currentOrigin = ent->r.currentOrigin;
	SnapAngles(ent->r.currentAngles);

	if ( !ent->r.bmodel || *angles == 0.0 && angles[1] == 0.0 && angles[2] == 0.0 )
	{
		VectorAdd(currentOrigin, ent->r.mins, ent->r.absmin);
		VectorAdd(currentOrigin, ent->r.maxs, ent->r.absmax);
	}
	else if ( *angles == 0.0 && angles[2] == 0.0 )
	{
		max2d = RadiusFromBounds2D(ent->r.mins, ent->r.maxs);

		for ( i = 0; i <= 1; ++i )
		{
			ent->r.absmin[i] = currentOrigin[i] - max2d;
			ent->r.absmax[i] = currentOrigin[i] + max2d;
		}

		ent->r.absmin[2] = currentOrigin[2] + ent->r.mins[2];
		ent->r.absmax[2] = currentOrigin[2] + ent->r.maxs[2];
	}
	else
	{
		max = RadiusFromBounds(ent->r.mins, ent->r.maxs);

		for ( j = 0; j <= 2; ++j )
		{
			ent->r.absmin[j] = currentOrigin[j] - max;
			ent->r.absmax[j] = currentOrigin[j] + max;
		}
	}

	ent->r.absmin[0] = ent->r.absmin[0] - 1.0;
	ent->r.absmin[1] = ent->r.absmin[1] - 1.0;
	ent->r.absmin[2] = ent->r.absmin[2] - 1.0;
	ent->r.absmax[0] = ent->r.absmax[0] + 1.0;
	ent->r.absmax[1] = ent->r.absmax[1] + 1.0;
	ent->r.absmax[2] = ent->r.absmax[2] + 1.0;
	svEnt->numClusters = 0;
	svEnt->lastCluster = 0;

	if ( (ent->r.svFlags & 0x19) == 0 )
	{
		num_leafs = CM_BoxLeafnums(ent->r.absmin, ent->r.absmax, leafs, 128, &lastLeaf);

		if ( !num_leafs )
		{
unlink:
			CM_UnlinkEntity(svEnt);
			return;
		}

		for ( a = 0; a < num_leafs; ++a )
		{
			cluster = CM_LeafCluster(leafs[a]);

			if ( cluster != -1 )
			{
				writeEnt = svEnt;
				svEnt->clusternums[svEnt->numClusters] = cluster;
				++writeEnt->numClusters;

				if ( svEnt->numClusters == 16 )
					break;
			}
		}

		if ( a != num_leafs )
			svEnt->lastCluster = CM_LeafCluster(lastLeaf);
	}

	ent->r.linked = 1;

	if ( !ent->r.contents )
		goto unlink;

	handle = SV_ClipHandleForEntity(ent);
	obj = Com_GetServerDObj(ent->s.number);

	if ( obj && (ent->r.svFlags & 6) != 0 )
	{
		if ( (ent->r.svFlags & 2) != 0 )
		{
			Vector2Add(currentOrigin, actorLocationalMins, mins);
			Vector2Add(currentOrigin, actorLocationalMaxs, maxs);
		}
		else
		{
			DObjGetBounds(obj, mins, maxs);
			Vector2Add(currentOrigin, mins, mins);
			Vector2Add(currentOrigin, maxs, maxs);
		}
		CM_LinkEntity(svEnt, mins, maxs, handle);
	}
	else
	{
		CM_LinkEntity(svEnt, ent->r.absmin, ent->r.absmax, handle);
	}
}

void SV_UnlinkEntity(gentity_s *ent)
{
	svEntity_t *svEnt;

	svEnt = SV_SvEntityForGentity(ent);
	ent->r.linked = 0;
	CM_UnlinkEntity(svEnt);
}

void SV_ClipMoveToEntity(moveclip_t *clip, svEntity_t *entity, trace_t *trace)
{
	vec3_t maxs;
	vec3_t mins;
	float fraction;
	int num;
	float *angles;
	unsigned int model;
	gentity_s *touch;

	num = entity - sv.svEntities;
	touch = SV_GentityNum(num);
	if ( (clip->contentmask & touch->r.contents) != 0
	        && (clip->passEntityNum == 1023
	            || num != clip->passEntityNum
	            && touch->r.ownerNum != clip->passEntityNum
	            && touch->r.ownerNum != clip->passOwnerNum) )
	{
		VectorAdd(touch->r.absmin, clip->mins, mins);
		VectorAdd(touch->r.absmax, clip->maxs, maxs);
		if ( !CM_TraceBox(&clip->extents, mins, maxs, trace->fraction) )
		{
			model = SV_ClipHandleForEntity(touch);
			angles = touch->r.currentAngles;
			if ( !touch->r.bmodel )
				angles = vec3_origin;
			fraction = trace->fraction;
			CM_TransformedBoxTrace(
			    trace,
			    clip->extents.start,
			    clip->extents.end,
			    clip->mins,
			    clip->maxs,
			    model,
			    clip->contentmask,
			    touch->r.currentOrigin,
			    angles);
			if ( trace->fraction < fraction )
				trace->hitId = touch->s.number;
		}
	}
}