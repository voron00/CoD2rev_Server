#include "qcommon.h"
#include "cm_local.h"
#include "../server/server.h"

#define SECTOR_HEAD 1

cm_world_t cm_world;

/*
===============
CM_UnlinkEntity
===============
*/
void CM_UnlinkEntity( svEntity_t *ent )
{
	uint16_t nodeIndex;
	uint16_t parentNodeIndex;
	int contents;
	worldSector_t *node;
	svEntity_t *scan;

	nodeIndex = ent->worldSector;

	if ( !nodeIndex )
	{
		return;     // not linked in anywhere
	}

	node = &cm_world.sectors[nodeIndex];
	ent->worldSector = 0;

	assert(node->contents.entities);

	if ( &sv.svEntities[node->contents.entities - 1] == ent )
	{
		node->contents.entities = ent->nextEntityInWorldSector;
	}
	else
	{
		for ( scan = &sv.svEntities[node->contents.entities - 1] ; ; scan = &sv.svEntities[scan->nextEntityInWorldSector - 1] )
		{
			if ( &sv.svEntities[scan->nextEntityInWorldSector - 1] == ent )
			{
				assert(scan->nextEntityInWorldSector);
				scan->nextEntityInWorldSector = ent->nextEntityInWorldSector;
				break;
			}
		}
	}

	while ( 1 )
	{
		if ( node->contents.entities )
		{
			break;
		}

		if ( node->contents.staticModels )
		{
			break;
		}

		if ( node->tree.child[0] )
		{
			break;
		}

		if ( node->tree.child[1] )
		{
			break;
		}

		assert(!node->contents.contentsStaticModels);
		node->contents.contentsEntities = 0;

		if ( !node->tree.parent )
		{
			assert(nodeIndex == SECTOR_HEAD);
			break;
		}

		parentNodeIndex = node->tree.parent;
		node->tree.parent = cm_world.freeHead;

		cm_world.freeHead = nodeIndex;
		node = &cm_world.sectors[parentNodeIndex];

		if ( node->tree.child[0] == nodeIndex )
		{
			node->tree.child[0] = 0;
		}
		else
		{
			assert(node->tree.child[1] == nodeIndex);
			node->tree.child[1] = 0;
		}

		nodeIndex = parentNodeIndex;
	}

	while ( 1 )
	{
		contents = cm_world.sectors[node->tree.child[0]].contents.contentsEntities | cm_world.sectors[node->tree.child[1]].contents.contentsEntities;

		if ( node->contents.entities )
		{
			for ( scan = &sv.svEntities[node->contents.entities - 1] ; ; scan = &sv.svEntities[scan->nextEntityInWorldSector - 1] )
			{
				contents |= SV_GEntityForSvEntity(scan)->r.contents;

				if ( !scan->nextEntityInWorldSector )
				{
					break;
				}
			}
		}

		node->contents.contentsEntities = contents;
		parentNodeIndex = node->tree.parent;

		if ( !parentNodeIndex )
		{
			break;
		}

		node = &cm_world.sectors[parentNodeIndex];
	}
}

/*
================
CM_AreaEntities_r
================
*/
static void CM_AreaEntities_r( unsigned short nodeIndex, areaParms_t *ap )
{
	gentity_t *gcheck;
	unsigned short entnum;

	if ( !(cm_world.sectors[nodeIndex].contents.contentsEntities & ap->contentmask) )
	{
		return;
	}

	for ( entnum = cm_world.sectors[nodeIndex].contents.entities; entnum; entnum = sv.svEntities[entnum - 1].nextEntityInWorldSector )
	{
		gcheck = SV_GEntityForSvEntity(&sv.svEntities[entnum - 1]);

		if ( !(gcheck->r.contents & ap->contentmask) )
		{
			continue;
		}

		if (	          	 gcheck->r.absmin[ 0 ] > ap->maxs[ 0 ]
		                     || gcheck->r.absmax[ 0 ] < ap->mins[ 0 ]
		                     || gcheck->r.absmin[ 1 ] > ap->maxs[ 1 ]
		                     || gcheck->r.absmax[ 1 ] < ap->mins[ 1 ]
		                     || gcheck->r.absmin[ 2 ] > ap->maxs[ 2 ]
		                     || gcheck->r.absmax[ 2 ] < ap->mins[ 2 ] )
		{
			continue;
		}

		if ( ap->count == ap->maxcount )
		{
			Com_DPrintf("CM_AreaEntities: MAXCOUNT\n");
			return;
		}

		ap->list[ap->count] = &sv.svEntities[entnum - 1] - sv.svEntities;
		ap->count++;
	}

	// recurse down both sides
	if ( ap->maxs[cm_world.sectors[nodeIndex].tree.axis] > cm_world.sectors[nodeIndex].tree.dist )
	{
		CM_AreaEntities_r(cm_world.sectors[nodeIndex].tree.child[0], ap);
	}

	if ( cm_world.sectors[nodeIndex].tree.dist > ap->mins[cm_world.sectors[nodeIndex].tree.axis] )
	{
		CM_AreaEntities_r(cm_world.sectors[nodeIndex].tree.child[1], ap);
	}
}

/*
================
CM_AreaEntities
================
*/
int CM_AreaEntities( const vec3_t mins, const vec3_t maxs, int *entityList, int maxcount, int contentmask )
{
	areaParms_t ap;

	ap.mins = mins;
	ap.maxs = maxs;
	ap.list = entityList;
	ap.count = 0;
	ap.maxcount = maxcount;
	ap.contentmask = contentmask;

	CM_AreaEntities_r(1, &ap);

	return ap.count;
}

/*
================
CM_AllocWorldSector
================
*/
static unsigned short CM_AllocWorldSector( vec2_t mins, vec2_t maxs )
{
	worldSector_t *node;
	unsigned short nodeIndex;
	vec2_t size;
	unsigned short axis;

	nodeIndex = cm_world.freeHead;

	if ( !nodeIndex )
	{
		return 0;
	}

	Vector2Subtract(maxs, mins, size);

	axis = size[1] >= size[0];

	if ( size[size[1] >= size[0]] <= 512 )
	{
		return 0;
	}

	node = &cm_world.sectors[cm_world.freeHead];

	assert(!node->contents.contentsStaticModels);
	assert(!node->contents.contentsEntities);
	assert(!node->contents.entities);
	assert(!node->contents.staticModels);

	cm_world.freeHead = node->tree.parent;

	node->tree.axis = axis;
	node->tree.dist = (float)(maxs[axis] + mins[axis]) * 0.5;

	assert(!node->tree.child[0]);
	assert(!node->tree.child[1]);

	return nodeIndex;
}

/*
================
CM_AddStaticModelToNode
================
*/
static void CM_AddStaticModelToNode( cStaticModel_t *staticModel, unsigned short childNodeIndex )
{
	unsigned short modelnum;
	cStaticModel_t *prevStaticModel;

	modelnum = staticModel - cm.staticModelList;

	for ( prevStaticModel = (cStaticModel_t *)&cm_world.sectors[childNodeIndex].contents.staticModels ; ; prevStaticModel = &cm.staticModelList[prevStaticModel->writable.nextModelInWorldSector - 1] )
	{
		if ( (unsigned short)(prevStaticModel->writable.nextModelInWorldSector - 1) > modelnum )
		{
			staticModel->writable.nextModelInWorldSector = prevStaticModel->writable.nextModelInWorldSector;
			prevStaticModel->writable.nextModelInWorldSector = modelnum + 1;
			break;
		}
	}
}

/*
================
CM_AddEntityToNode
================
*/
static void CM_AddEntityToNode( svEntity_t *ent, unsigned short childNodeIndex )
{
	unsigned short entnum;
	uint16_t *prevEnt;

	entnum = ent - sv.svEntities;

	for ( prevEnt = &cm_world.sectors[childNodeIndex].contents.entities ; ; prevEnt = &sv.svEntities[*prevEnt - 1].nextEntityInWorldSector )
	{
		if ( (unsigned short)(*prevEnt - 1) > entnum )
		{
			ent->worldSector = childNodeIndex;
			ent->nextEntityInWorldSector = *prevEnt;
			*prevEnt = entnum + 1;
			break;
		}
	}
}

/*
================
CM_SortNode
================
*/
static void CM_SortNode( unsigned short nodeIndex, vec2_t mins, vec2_t maxs )
{
	worldSector_t *node;
	unsigned short modelnum;
	svEntity_t *prevEnt;
	float dist;
	svEntity_t *ent;
	cStaticModel_t *staticModel;
	unsigned short entnum;
	int axis;
	cStaticModel_t *prevStaticModel;
	unsigned short childNodeIndex;

	if ( cm_world.lockTree )
	{
		return;
	}

	node = &cm_world.sectors[nodeIndex];

	axis = cm_world.sectors[nodeIndex].tree.axis;
	dist = cm_world.sectors[nodeIndex].tree.dist;

	// Sort entities
	prevEnt = NULL;
	entnum = cm_world.sectors[nodeIndex].contents.entities;

	while ( entnum )
	{
		ent = &sv.svEntities[entnum - 1];

		if ( ent->linkmin[axis] <= dist )
		{
			if ( dist > ent->linkmax[axis] )
			{
				childNodeIndex = node->tree.child[1];

				if ( childNodeIndex )
				{
					goto addEntity;
				}

				childNodeIndex = CM_AllocWorldSector(mins, maxs);

				if ( childNodeIndex )
				{
					node->tree.child[1] = childNodeIndex;
					cm_world.sectors[childNodeIndex].tree.parent = nodeIndex;

					goto addEntity;
				}
			}

			goto skipEntity;
		}

		childNodeIndex = node->tree.child[0];

		if ( childNodeIndex )
		{
addEntity:
			entnum = ent->nextEntityInWorldSector;

			assert(prevEnt || (&sv.svEntities[node->contents.entities - 1] == ent));
			assert(!prevEnt || (&sv.svEntities[prevEnt->nextEntityInWorldSector - 1] == ent));

			CM_AddEntityToNode(ent, childNodeIndex);

			cm_world.sectors[childNodeIndex].contents.contentsEntities |= SV_GEntityForSvEntity(ent)->r.contents;
			cm_world.sectors[childNodeIndex].contents.contentsEntities |= ent->linkcontents;

			if ( prevEnt )
				prevEnt->nextEntityInWorldSector = entnum;
			else
				node->contents.entities = entnum;
		}
		else
		{
			childNodeIndex = CM_AllocWorldSector(mins, maxs);

			if ( childNodeIndex )
			{
				node->tree.child[0] = childNodeIndex;
				cm_world.sectors[childNodeIndex].tree.parent = nodeIndex;

				goto addEntity;
			}
skipEntity:
			prevEnt = &sv.svEntities[entnum - 1];
			entnum = ent->nextEntityInWorldSector;
		}
	}

	// Sort static models
	prevStaticModel = NULL;
	modelnum = node->contents.staticModels;

	while ( modelnum )
	{
		staticModel = &cm.staticModelList[modelnum - 1];

		if ( staticModel->absmin[axis] <= dist )
		{
			if ( dist > staticModel->absmax[axis] )
			{
				childNodeIndex = node->tree.child[1];

				if ( childNodeIndex )
				{
					goto addStaticModel;
				}

				childNodeIndex = CM_AllocWorldSector(mins, maxs);

				if ( childNodeIndex )
				{
					node->tree.child[1] = childNodeIndex;
					cm_world.sectors[childNodeIndex].tree.parent = nodeIndex;

					goto addStaticModel;
				}
			}

			goto skipStaticModel;
		}

		childNodeIndex = node->tree.child[0];

		if ( childNodeIndex )
		{
addStaticModel:
			modelnum = staticModel->writable.nextModelInWorldSector;

			assert(prevStaticModel  || (&cm.staticModelList[node->contents.staticModels - 1] == staticModel));
			assert(!prevStaticModel || (&cm.staticModelList[prevStaticModel->writable.nextModelInWorldSector - 1] == staticModel));

			CM_AddStaticModelToNode(staticModel, childNodeIndex);
			cm_world.sectors[childNodeIndex].contents.contentsStaticModels |= XModelGetContents(staticModel->xmodel);

			if ( prevStaticModel )
				prevStaticModel->writable.nextModelInWorldSector = modelnum;
			else
				node->contents.staticModels = modelnum;
		}
		else
		{
			childNodeIndex = CM_AllocWorldSector(mins, maxs);

			if ( childNodeIndex )
			{
				node->tree.child[0] = childNodeIndex;
				cm_world.sectors[childNodeIndex].tree.parent = nodeIndex;

				goto addStaticModel;
			}
skipStaticModel:
			prevStaticModel = staticModel;
			modelnum = staticModel->writable.nextModelInWorldSector;
		}
	}
}

/*
================
CM_LinkEntity
================
*/
void CM_LinkEntity( svEntity_t *ent, vec3_t absmin, vec3_t absmax, clipHandle_t clipHandle )
{
	cmodel_t *cmod;
	int linkcontents;
	worldSector_t *node;
	vec2_t maxs;
	vec3_t mins;
	float dist;
	int axis;
	unsigned short nodeIndex;

	cmod = CM_ClipHandleToModel(clipHandle);
	linkcontents = cmod->leaf.brushContents | cmod->leaf.terrainContents;

	if ( !*(int64_t *)&cmod->leaf.brushContents )
	{
		CM_UnlinkEntity(ent);
		return;
	}

	while ( 1 )
	{
		Vector2Copy(cm_world.mins, mins);
		Vector2Copy(cm_world.maxs, maxs);

		for ( nodeIndex = 1; ; nodeIndex = node->tree.child[1] )
		{
			while ( 1 )
			{
				cm_world.sectors[nodeIndex].contents.contentsEntities |= linkcontents;
				node = &cm_world.sectors[nodeIndex];

				axis = node->tree.axis;
				dist = node->tree.dist;

				if ( absmin[axis] <= dist )
				{
					break;
				}

				mins[axis] = dist;

				if ( !node->tree.child[0] )
				{
					goto LABEL_13;
				}

				nodeIndex = node->tree.child[0];
			}

			if ( dist <= absmax[axis] )
			{
				break;
			}

			maxs[axis] = dist;

			if ( !node->tree.child[1] )
			{
				goto LABEL_13;
			}
		}

		if ( nodeIndex == ent->worldSector && !(ent->linkcontents & ~linkcontents) )
		{
			ent->linkcontents = linkcontents;
			Vector2Copy(absmin, ent->linkmin);
			Vector2Copy(absmax, ent->linkmax);
			return;
		}
LABEL_13:
		if ( !ent->worldSector )
		{
			break;
		}

		if ( nodeIndex == ent->worldSector && !(ent->linkcontents & ~linkcontents) )
		{
			goto LABEL_18;
		}

		CM_UnlinkEntity(ent);
	}
	CM_AddEntityToNode(ent, nodeIndex);
LABEL_18:
	ent->linkcontents = linkcontents;
	Vector2Copy(absmin, ent->linkmin);
	Vector2Copy(absmax, ent->linkmax);
	CM_SortNode(nodeIndex, mins, maxs);
}

/*
================
CM_ClipMoveToEntities_r
================
*/
static void CM_ClipMoveToEntities_r( moveclip_t *clip, unsigned short nodeIndex, const vec4_t p1, const vec4_t p2, trace_t *trace )
{
	int side;
	worldSector_t *node;
	float diff;
	float t1;
	float frac;
	float offset;
	float t2;
	float frac2;
	float absDiff;
	unsigned short entnum;
	vec4_t mid;
	svEntity_t *check;
	vec4_t p;

	VectorCopy4(p1, p);

	while ( 1 )
	{
		node = &cm_world.sectors[nodeIndex];

		if ( !(clip->contentmask & node->contents.contentsEntities) )
		{
			break;
		}

		for ( entnum = node->contents.entities; entnum; entnum = check->nextEntityInWorldSector )
		{
			check = &sv.svEntities[entnum - 1];

			// if it doesn't have any brushes of a type we
			// are looking for, ignore it
			if ( !(check->linkcontents & clip->contentmask) )
			{
				continue;
			}

			SV_ClipMoveToEntity(clip, check, trace);
		}

		t1 = p[node->tree.axis] - node->tree.dist;
		t2 = p2[node->tree.axis] - node->tree.dist;

		offset = clip->outerSize[node->tree.axis];

		if ( I_fmin(t1, t2) >= offset )
		{
			nodeIndex = node->tree.child[0];
			continue;
		}

		if ( -offset >= I_fmax(t1, t2) )
		{
			nodeIndex = node->tree.child[1];
			continue;
		}

		if ( p[3] >= trace->fraction )
		{
			return;
		}

		diff = t2 - t1;

		if ( diff == 0.0 )
		{
			side = 0;
			frac = 1.0;
			frac2 = 0.0;
		}
		else
		{
			absDiff = I_fabs(diff);
			frac2 = (I_fsel(diff, -t1, t1) - offset) * (1.0 / absDiff);
			frac = (I_fsel(diff, -t1, t1) + offset) * (1.0 / absDiff);
			side = I_side(diff);
		}

		assert(frac >= 0);
		frac = I_fmin(frac, 1.0);

		mid[0] = (float)((float)(p2[0] - p[0]) * frac) + p[0];
		mid[1] = (float)((float)(p2[1] - p[1]) * frac) + p[1];
		mid[2] = (float)((float)(p2[2] - p[2]) * frac) + p[2];
		mid[3] = (float)((float)(p2[3] - p[3]) * frac) + p[3];

		CM_ClipMoveToEntities_r(clip, node->tree.child[side], p, mid, trace);

		assert(frac2 <= 1.f);
		frac2 = I_fmax(frac2, 0.0);

		p[0] = (float)((float)(p2[0] - p[0]) * frac2) + p[0];
		p[1] = (float)((float)(p2[1] - p[1]) * frac2) + p[1];
		p[2] = (float)((float)(p2[2] - p[2]) * frac2) + p[2];
		p[3] = (float)((float)(p2[3] - p[3]) * frac2) + p[3];

		nodeIndex = node->tree.child[1 - side];
	}
}

/*
================
CM_ClipMoveToEntities
================
*/
void CM_ClipMoveToEntities( moveclip_t *clip, trace_t *trace )
{
	vec4_t end;
	vec4_t start;

	assert(trace->fraction <= 1.f);

	VectorCopy(clip->extents.start, start);
	VectorCopy(clip->extents.end, end);

	start[3] = 0.0;
	end[3] = trace->fraction;

	CM_ClipMoveToEntities_r(clip, 1, start, end, trace);
}

/*
================
CM_PointTraceToEntities_r
================
*/
static void CM_PointTraceToEntities_r( pointtrace_t *clip, unsigned short nodeIndex, const vec4_t p1, const vec4_t p2, trace_t *trace )
{
	worldSector_t *node;
	float t1;
	float frac;
	float t2;
	int side;
	unsigned short entnum;
	vec4_t mid;
	svEntity_t *check;
	vec4_t p;

	VectorCopy4(p1, p);

	while ( 1 )
	{
		node = &cm_world.sectors[nodeIndex];

		if ( !(clip->contentmask & node->contents.contentsEntities) )
		{
			break;
		}

		for ( entnum = node->contents.entities; entnum; entnum = check->nextEntityInWorldSector )
		{
			check = &sv.svEntities[entnum - 1];
			SV_PointTraceToEntity(clip, check, trace);
		}

		t1 = p[node->tree.axis] - node->tree.dist;
		t2 = p2[node->tree.axis] - node->tree.dist;

		if ( (float)(t1 * t2) >= 0.0 )
		{
			nodeIndex = node->tree.child[1 - I_side(I_fmin(t1, t2))];
			continue;
		}

		if ( p[3] >= trace->fraction )
		{
			return;
		}

		frac = t1 / (float)(t1 - t2);

		assert(frac >= 0);
		assert(frac <= 1.f);

		mid[0] = (float)((float)(p2[0] - p[0]) * frac) + p[0];
		mid[1] = (float)((float)(p2[1] - p[1]) * frac) + p[1];
		mid[2] = (float)((float)(p2[2] - p[2]) * frac) + p[2];
		mid[3] = (float)((float)(p2[3] - p[3]) * frac) + p[3];

		side = I_side(t2);

		CM_PointTraceToEntities_r(clip, node->tree.child[side], p, mid, trace);

		nodeIndex = node->tree.child[1 - side];

		VectorCopy4(mid, p);
	}
}

/*
================
CM_PointTraceToEntities
================
*/
void CM_PointTraceToEntities( pointtrace_t *clip, trace_t *trace )
{
	vec4_t end;
	vec4_t start;

	assert(trace->fraction <= 1.f);

	VectorCopy(clip->extents.start, start);
	VectorCopy(clip->extents.end, end);

	start[3] = 0.0;
	end[3] = trace->fraction;

	CM_PointTraceToEntities_r(clip, 1, start, end, trace);
}

/*
================
CM_PointTraceStaticModelsComplete_r
================
*/
static qboolean CM_PointTraceStaticModelsComplete_r( staticmodeltrace_t *clip, unsigned short nodeIndex, const vec3_t p1_, const vec3_t p2 )
{
	int side;
	worldSector_t *node;
	unsigned short modelnum;
	float t1;
	float frac;
	vec3_t p1;
	float t2;
	vec3_t mid;
	cStaticModel_t *check;

	VectorCopy(p1_, p1);

	while ( 1 )
	{
		while ( 1 )
		{
			node = &cm_world.sectors[nodeIndex];

			if ( !(clip->contents & node->contents.contentsStaticModels) )
			{
				return qtrue;
			}

			for ( modelnum = node->contents.staticModels; modelnum; modelnum = check->writable.nextModelInWorldSector )
			{
				check = &cm.staticModelList[modelnum - 1];

				if ( !(clip->contents & XModelGetContents(check->xmodel)) )
				{
					continue;
				}

				if ( CM_TraceBox(&clip->extents, check->absmin, check->absmax, 1.0) )
				{
					continue;
				}

				if ( CM_TraceStaticModelComplete(check, clip->extents.start, clip->extents.end, clip->contents) )
				{
					continue;
				}

				return qfalse;
			}

			t1 = p1[node->tree.axis] - node->tree.dist;
			t2 = p2[node->tree.axis] - node->tree.dist;

			if ( (float)(t1 * t2) < 0.0 )
			{
				break;
			}

			nodeIndex = node->tree.child[1 - I_side(I_fmin(t1, t2))];
		}

		assert(t1 - t2);

		frac = t1 / (float)(t1 - t2);

		assert(frac >= 0);
		assert(frac <= 1.f);

		mid[0] = (float)((float)(p2[0] - p1[0]) * frac) + p1[0];
		mid[1] = (float)((float)(p2[1] - p1[1]) * frac) + p1[1];
		mid[2] = (float)((float)(p2[2] - p1[2]) * frac) + p1[2];

		side = I_side(t2);

		if ( !CM_PointTraceStaticModelsComplete_r(clip, node->tree.child[side], p1, mid) )
		{
			break;
		}

		nodeIndex = node->tree.child[1 - side];

		VectorCopy(mid, p1);
	}

	return qfalse;
}

/*
================
CM_PointTraceStaticModelsComplete
================
*/
qboolean CM_PointTraceStaticModelsComplete( const vec3_t start, const vec3_t end, int contentmask )
{
	staticmodeltrace_t clip;

	clip.contents = contentmask;

	VectorCopy(start, clip.extents.start);
	VectorCopy(end, clip.extents.end);

	CM_CalcTraceEntents(&clip.extents);

	return CM_PointTraceStaticModelsComplete_r(&clip, 1, clip.extents.start, clip.extents.end);
}

/*
================
CM_PointTraceStaticModels_r
================
*/
static void CM_PointTraceStaticModels_r( locTraceWork_t *tw, unsigned short nodeIndex, const vec4_t p1_, const vec4_t p2, trace_t *trace )
{
	int side;
	worldSector_t *node;
	unsigned short modelnum;
	float t1;
	float frac;
	vec4_t mid;
	float t2;
	vec4_t p1;
	cStaticModel_t *check;

	VectorCopy4(p1_, p1);

	while ( 1 )
	{
		node = &cm_world.sectors[nodeIndex];

		if ( !(tw->contents & node->contents.contentsStaticModels) )
		{
			break;
		}

		for ( modelnum = node->contents.staticModels; modelnum; modelnum = check->writable.nextModelInWorldSector )
		{
			check = &cm.staticModelList[modelnum - 1];

			if ( !(tw->contents & XModelGetContents(check->xmodel)) )
			{
				continue;
			}

			if ( CM_TraceBox(&tw->extents, check->absmin, check->absmax, trace->fraction) )
			{
				continue;
			}

			CM_TraceStaticModel(check, trace, tw->extents.start, tw->extents.end, tw->contents);
		}

		t1 = p1[node->tree.axis] - node->tree.dist;
		t2 = p2[node->tree.axis] - node->tree.dist;

		if ( (float)(t1 * t2) >= 0.0 )
		{
			nodeIndex = node->tree.child[1 - I_side(I_fmin(t1, t2))];
			continue;
		}

		if ( p1[3] >= trace->fraction )
		{
			return;
		}

		assert(t1 - t2);

		frac = t1 / (float)(t1 - t2);

		assert(frac >= 0);
		assert(frac <= 1.f);

		mid[0] = (float)((float)(p2[0] - p1[0]) * frac) + p1[0];
		mid[1] = (float)((float)(p2[1] - p1[1]) * frac) + p1[1];
		mid[2] = (float)((float)(p2[2] - p1[2]) * frac) + p1[2];
		mid[3] = (float)((float)(p2[3] - p1[3]) * frac) + p1[3];

		side = I_side(t2);

		CM_PointTraceStaticModels_r(tw, node->tree.child[side], p1, mid, trace);

		nodeIndex = node->tree.child[1 - side];

		VectorCopy4(mid, p1);
	}
}

/*
================
CM_PointTraceStaticModels
================
*/
void CM_PointTraceStaticModels( trace_t *results, const vec3_t start, const vec3_t end, int contentmask )
{
	vec4_t end_;
	vec4_t start_;
	locTraceWork_t tw;

	tw.contents = contentmask;

	VectorCopy(start, tw.extents.start);
	VectorCopy(end, tw.extents.end);

	CM_CalcTraceEntents(&tw.extents);

	VectorCopy(tw.extents.start, start_);
	start_[3] = 0.0;

	VectorCopy(tw.extents.end, end_);
	end_[3] = results->fraction;

	CM_PointTraceStaticModels_r(&tw, 1, start_, end_, results);
}

/*
================
CM_PointSightTraceToEntities_r
================
*/
static int CM_PointSightTraceToEntities_r( sightpointtrace_t *clip, unsigned short nodeIndex, const vec3_t p1, const vec3_t p2 )
{
	worldSector_t *node;
	float t1;
	float frac;
	float t2;
	unsigned short entnum;
	int hitNum;
	int side;
	vec3_t mid;
	svEntity_t *check;

	node = &cm_world.sectors[nodeIndex];

	if ( !(clip->contentmask & node->contents.contentsEntities) )
	{
		return 0;
	}

	t1 = p1[node->tree.axis] - node->tree.dist;
	t2 = p2[node->tree.axis] - node->tree.dist;

	if ( (float)(t1 * t2) >= 0.0 )
	{
		hitNum = CM_PointSightTraceToEntities_r(clip, node->tree.child[1 - I_side(I_fmin(t1, t2))], p1, p2);

		if ( hitNum )
		{
			return hitNum;
		}
	}
	else
	{
		frac = t1 / (float)(t1 - t2);

		assert(frac >= 0);
		assert(frac <= 1.f);

		mid[0] = (float)((float)(p2[0] - p1[0]) * frac) + p1[0];
		mid[1] = (float)((float)(p2[1] - p1[1]) * frac) + p1[1];
		mid[2] = (float)((float)(p2[2] - p1[2]) * frac) + p1[2];

		side = I_side(t2);

		hitNum = CM_PointSightTraceToEntities_r(clip, node->tree.child[side], p1, mid);

		if ( hitNum )
		{
			return hitNum;
		}

		hitNum = CM_PointSightTraceToEntities_r(clip, node->tree.child[1 - side], mid, p2);

		if ( hitNum )
		{
			return hitNum;
		}
	}

	for ( entnum = node->contents.entities; entnum; entnum = check->nextEntityInWorldSector )
	{
		check = &sv.svEntities[entnum - 1];
		hitNum = SV_PointSightTraceToEntity(clip, check);

		if ( hitNum )
		{
			return hitNum;
		}
	}

	return 0;
}

/*
================
CM_PointSightTraceToEntities
================
*/
int CM_PointSightTraceToEntities( sightpointtrace_t *clip )
{
	return CM_PointSightTraceToEntities_r(clip, 1, clip->start, clip->end);
}

/*
================
CM_ClipSightTraceToEntities_r
================
*/
static int CM_ClipSightTraceToEntities_r( sightclip_t *clip, unsigned short nodeIndex, const vec3_t p1, const vec3_t p2 )
{
	int side;
	worldSector_t *node;
	float diff;
	float t1;
	float frac;
	float offset;
	float t2;
	float frac2;
	float absDiff;
	unsigned short entnum;
	int hitNum;
	vec3_t mid;
	svEntity_t *check;
	vec3_t p;

	VectorCopy(p1, p);

	while ( 1 )
	{
		while ( 1 )
		{
			while ( 1 )
			{
				node = &cm_world.sectors[nodeIndex];

				if ( !(clip->contentmask & node->contents.contentsEntities) )
				{
					return 0;
				}

				for ( entnum = node->contents.entities; entnum; entnum = check->nextEntityInWorldSector )
				{
					check = &sv.svEntities[entnum - 1];
					hitNum = SV_ClipSightToEntity(clip, check);

					if ( hitNum )
					{
						return hitNum;
					}
				}

				t1 = p[node->tree.axis] - node->tree.dist;
				t2 = p2[node->tree.axis] - node->tree.dist;

				offset = clip->outerSize[node->tree.axis];

				if ( I_fmin(t1, t2) < offset )
				{
					break;
				}

				nodeIndex = node->tree.child[0];
			}

			if ( -offset < I_fmax(t1, t2) )
			{
				break;
			}

			nodeIndex = node->tree.child[1];
		}

		diff = t2 - t1;

		if ( diff == 0.0 )
		{
			side = 0;
			frac = 1.0;
			frac2 = 0.0;
		}
		else
		{
			absDiff = I_fabs(diff);
			frac2 = (I_fsel(diff, -t1, t1) - offset) * (1.0 / absDiff);
			frac = (I_fsel(diff, -t1, t1) + offset) * (1.0 / absDiff);
			side = I_side(diff);
		}

		assert(frac >= 0);
		frac = I_fmin(frac, 1.0);

		mid[0] = (float)((float)(p2[0] - p[0]) * frac) + p[0];
		mid[1] = (float)((float)(p2[1] - p[1]) * frac) + p[1];
		mid[2] = (float)((float)(p2[2] - p[2]) * frac) + p[2];

		hitNum = CM_ClipSightTraceToEntities_r(clip, node->tree.child[side], p, mid);

		if ( hitNum )
		{
			break;
		}

		assert(frac2 <= 1.f);
		frac2 = I_fmax(frac2, 0.0);

		p[0] = (float)((float)(p2[0] - p[0]) * frac2) + p[0];
		p[1] = (float)((float)(p2[1] - p[1]) * frac2) + p[1];
		p[2] = (float)((float)(p2[2] - p[2]) * frac2) + p[2];

		nodeIndex = node->tree.child[1 - side];
	}

	return hitNum;
}

/*
================
CM_ClipSightTraceToEntities
================
*/
int CM_ClipSightTraceToEntities( sightclip_t *clip )
{
	return CM_ClipSightTraceToEntities_r(clip, 1, clip->start, clip->end);
}

/*
================
CM_LinkStaticModel
================
*/
void CM_LinkStaticModel( cStaticModel_t *staticModel )
{
	int contents;
	worldSector_t *node;
	unsigned short nodeIndex;
	float dist;
	vec2_t mins;
	vec2_t maxs;
	int axis;

	contents = XModelGetContents(staticModel->xmodel);
	assert(contents);

	Vector2Copy(cm_world.mins, mins);
	Vector2Copy(cm_world.maxs, maxs);

	for ( nodeIndex = 1; ; nodeIndex = node->tree.child[1] )
	{
		while ( 1 )
		{
			cm_world.sectors[nodeIndex].contents.contentsStaticModels |= contents;

			node = &cm_world.sectors[nodeIndex];

			axis = node->tree.axis;
			dist = node->tree.dist;

			if ( staticModel->absmin[axis] <= dist )
			{
				break;
			}

			mins[axis] = dist;

			if ( !node->tree.child[0] )
			{
				CM_AddStaticModelToNode(staticModel, nodeIndex);
				CM_SortNode(nodeIndex, mins, maxs);
				return;
			}

			nodeIndex = node->tree.child[0];
		}

		if ( dist <= staticModel->absmax[axis] )
		{
			break;
		}

		maxs[axis] = dist;

		if ( !node->tree.child[1] )
		{
			break;
		}
	}

	CM_AddStaticModelToNode(staticModel, nodeIndex);
	CM_SortNode(nodeIndex, mins, maxs);
}

/*
================
CM_LinkAllStaticModels
================
*/
void CM_LinkAllStaticModels()
{
	cStaticModel_t *staticModel;
	int i;

	for ( i = 0, staticModel = cm.staticModelList; i < cm.numStaticModels ; i++, staticModel++ )
	{
		assert(staticModel->xmodel);

		if ( !XModelGetContents(staticModel->xmodel) )
		{
			continue;
		}

		CM_LinkStaticModel(staticModel);
	}
}

/*
================
CM_ClearWorld
================
*/
void CM_ClearWorld()
{
	vec2_t bounds;
	int i;

	memset(&cm_world, 0, sizeof(cm_world));

	CM_ModelBounds(0, cm_world.mins, cm_world.maxs);

	cm_world.freeHead = 2;

	for ( i = 2; i < AREA_NODES - 1; i++ )
	{
		cm_world.sectors[i].tree.parent = i + 1;
	}

	cm_world.sectors[AREA_NODES - 1].tree.parent = 0;

	Vector2Subtract(cm_world.maxs, cm_world.mins, bounds);

	cm_world.sectors[SECTOR_HEAD].tree.axis = bounds[1] >= bounds[0];
	cm_world.sectors[SECTOR_HEAD].tree.dist = (cm_world.maxs[bounds[1] >= bounds[0]] + cm_world.mins[bounds[1] >= bounds[0]]) * 0.5;

	assert(!cm_world.sectors[SECTOR_HEAD].tree.child[0]);
	assert(!cm_world.sectors[SECTOR_HEAD].tree.child[1]);
}

/*
================
CM_LinkWorld
================
*/
void CM_LinkWorld()
{
	CM_ClearWorld();
	CM_LinkAllStaticModels();
}