#include "qcommon.h"
#include "cm_local.h"
#include "../server/server.h"

cm_world_t cm_world;

unsigned short CM_AllocWorldSector(float *mins, float *maxs)
{
	vec2_t bounds;
	int head;
	bool axis;

	if ( !cm_world.freeHead )
		return 0;

	Vector2Subtract(maxs, mins, bounds);
	axis = bounds[1] >= bounds[0];

	if ( bounds[bounds[1] >= bounds[0]] <= 512.0 )
		return 0;

	head = cm_world.freeHead;

	cm_world.freeHead = cm_world.sectors[head].tree.nextFree;
	cm_world.sectors[head].tree.axis = axis;
	cm_world.sectors[head].tree.dist = (maxs[axis] + mins[axis]) * 0.5;

	return head;
}

void CM_AddStaticModelToNode(cStaticModel_s *model, unsigned short childNodeIndex)
{
	unsigned short modelIndex;
	cStaticModel_s *nextModel;

	modelIndex = model - cm.staticModelList;

	for ( nextModel = (cStaticModel_s *)&cm_world.sectors[childNodeIndex].contents.staticModels;
	        modelIndex >= (unsigned short)(nextModel->writable.nextModelInWorldSector - 1);
	        nextModel = &cm.staticModelList[nextModel->writable.nextModelInWorldSector - 1] )
	{
		;
	}

	model->writable.nextModelInWorldSector = nextModel->writable.nextModelInWorldSector;
	nextModel->writable.nextModelInWorldSector = modelIndex + 1;
}

void CM_AddEntityToNode(svEntity_t *svEnt, unsigned short childNodeIndex)
{
	unsigned short index;
	svEntity_t *nextEntity;

	index = svEnt - sv.svEntities;

	for ( nextEntity = (svEntity_t *)&cm_world.sectors[childNodeIndex].contents.entities;
	        index >= (unsigned short)(nextEntity->worldSector - 1);
	        nextEntity = (svEntity_t *)&sv.svEntities[nextEntity->worldSector - 1].nextEntityInWorldSector )
	{
		;
	}

	svEnt->worldSector = childNodeIndex;
	svEnt->nextEntityInWorldSector = nextEntity->worldSector;
	nextEntity->worldSector = index + 1;
}

void CM_SortNode(unsigned short nodeIndex, float *mins, float *maxs)
{
	uint16_t nextModelIndex;
	uint16_t entityIndex;
	unsigned short child1Index;
	unsigned short child0Index;
	cStaticModel_s *staticModel;
	svEntity_t *writeEnt;
	float dist;
	int axis;
	cStaticModel_s *model;
	svEntity_t *svEnt;

	if ( !cm_world.sorted )
	{
		axis = cm_world.sectors[nodeIndex].tree.axis;
		dist = cm_world.sectors[nodeIndex].tree.dist;
		writeEnt = 0;
		entityIndex = cm_world.sectors[nodeIndex].contents.entities;

		while ( entityIndex )
		{
			svEnt = &sv.svEntities[entityIndex - 1];

			if ( svEnt->linkmin[axis] <= dist )
			{
				if ( dist > svEnt->linkmax[axis] )
				{
					child1Index = cm_world.sectors[nodeIndex].tree.child[1];

					if ( child1Index )
						goto LABEL_13;

					child1Index = CM_AllocWorldSector(mins, maxs);

					if ( child1Index )
					{
						cm_world.sectors[nodeIndex].tree.child[1] = child1Index;
						cm_world.sectors[child1Index].tree.nextFree = nodeIndex;
						goto LABEL_13;
					}
				}

				goto LABEL_12;
			}

			child1Index = cm_world.sectors[nodeIndex].tree.child[0];

			if ( child1Index )
			{
LABEL_13:
				entityIndex = sv.svEntities[entityIndex - 1].nextEntityInWorldSector;
				CM_AddEntityToNode(svEnt, child1Index);
				cm_world.sectors[child1Index].contents.contentsEntities |= SV_GEntityForSvEntity(svEnt)->r.contents;

				if ( writeEnt )
					writeEnt->nextEntityInWorldSector = entityIndex;
				else
					cm_world.sectors[nodeIndex].contents.entities = entityIndex;
			}
			else
			{
				child1Index = CM_AllocWorldSector(mins, maxs);

				if ( child1Index )
				{
					cm_world.sectors[nodeIndex].tree.child[0] = child1Index;
					cm_world.sectors[child1Index].tree.nextFree = nodeIndex;
					goto LABEL_13;
				}
LABEL_12:
				writeEnt = &sv.svEntities[entityIndex - 1];
				entityIndex = sv.svEntities[entityIndex - 1].nextEntityInWorldSector;
			}
		}

		staticModel = 0;
		nextModelIndex = cm_world.sectors[nodeIndex].contents.staticModels;

		while ( 1 )
		{
			if ( !nextModelIndex )
				return;

			model = &cm.staticModelList[nextModelIndex - 1];

			if ( model->absmin[axis] <= dist )
				break;

			child0Index = cm_world.sectors[nodeIndex].tree.child[0];

			if ( child0Index )
			{
LABEL_27:
				nextModelIndex = model->writable.nextModelInWorldSector;
				CM_AddStaticModelToNode(model, child0Index);
				cm_world.sectors[child0Index].contents.contentsStaticModels |= XModelGetContents(model->xmodel);

				if ( staticModel )
					staticModel->writable.nextModelInWorldSector = nextModelIndex;
				else
					cm_world.sectors[nodeIndex].contents.staticModels = nextModelIndex;
			}
			else
			{
				child0Index = CM_AllocWorldSector(mins, maxs);

				if ( child0Index )
				{
					cm_world.sectors[nodeIndex].tree.child[0] = child0Index;
					cm_world.sectors[child0Index].tree.nextFree = nodeIndex;
					goto LABEL_27;
				}
LABEL_26:
				staticModel = model;
				nextModelIndex = model->writable.nextModelInWorldSector;
			}
		}

		if ( dist > model->absmax[axis] )
		{
			child0Index = cm_world.sectors[nodeIndex].tree.child[1];

			if ( child0Index )
				goto LABEL_27;

			child0Index = CM_AllocWorldSector(mins, maxs);

			if ( child0Index )
			{
				cm_world.sectors[nodeIndex].tree.child[1] = child0Index;
				cm_world.sectors[child0Index].tree.nextFree = nodeIndex;
				goto LABEL_27;
			}
		}

		goto LABEL_26;
	}
}

void CM_UnlinkEntity(svEntity_t *ent)
{
	uint16_t sectorId;
	int contents;
	uint16_t nextFree;
	worldSector_s *newSector;
	svEntity_t *svEnt;

	sectorId = ent->worldSector;

	if ( ent->worldSector )
	{
		newSector = &cm_world.sectors[sectorId];
		ent->worldSector = 0;

		if ( &sv.svEntities[cm_world.sectors[sectorId].contents.entities - 1] == ent )
		{
			cm_world.sectors[sectorId].contents.entities = ent->nextEntityInWorldSector;
		}
		else
		{
			for ( svEnt = &sv.svEntities[cm_world.sectors[sectorId].contents.entities - 1];
			        &sv.svEntities[svEnt->nextEntityInWorldSector - 1] != ent;
			        svEnt = &sv.svEntities[svEnt->nextEntityInWorldSector - 1] )
			{
				;
			}

			svEnt->nextEntityInWorldSector = ent->nextEntityInWorldSector;
		}

		while ( !newSector->contents.entities )
		{
			if ( newSector->contents.staticModels )
				break;

			if ( newSector->tree.child[0] )
				break;

			if ( newSector->tree.child[1] )
				break;

			newSector->contents.contentsEntities = 0;

			if ( !newSector->tree.nextFree )
				break;

			nextFree = newSector->tree.nextFree;
			newSector->tree.nextFree = cm_world.freeHead;
			cm_world.freeHead = sectorId;
			newSector = &cm_world.sectors[nextFree];

			if ( cm_world.sectors[nextFree].tree.child[0] == sectorId )
				cm_world.sectors[nextFree].tree.child[0] = 0;
			else
				cm_world.sectors[nextFree].tree.child[1] = 0;

			sectorId = nextFree;
		}

		while ( 1 )
		{
			contents = cm_world.sectors[newSector->tree.child[0]].contents.contentsEntities | cm_world.sectors[newSector->tree.child[1]].contents.contentsEntities;

			if ( newSector->contents.entities )
			{
				for ( svEnt = &sv.svEntities[newSector->contents.entities - 1];
				        ;
				        svEnt = &sv.svEntities[svEnt->nextEntityInWorldSector - 1] )
				{
					contents |= SV_GEntityForSvEntity(svEnt)->r.contents;

					if ( !svEnt->nextEntityInWorldSector )
						break;
				}
			}

			newSector->contents.contentsEntities = contents;

			if ( !newSector->tree.nextFree )
				break;

			newSector = &cm_world.sectors[newSector->tree.nextFree];
		}
	}
}

void CM_LinkEntity(svEntity_t *ent, float *absmin, float *absmax, clipHandle_t clipHandle)
{
	cmodel_s *model;
	int contents;
	worldSector_s *sector;
	vec2_t maxs;
	vec3_t mins;
	float dist;
	int axis;
	unsigned short i;

	model = CM_ClipHandleToModel(clipHandle);
	contents = model->leaf.brushContents | model->leaf.terrainContents;

	if ( *(uint64_t *)&model->leaf.brushContents )
	{
		while ( 1 )
		{
			Vector2Copy(cm_world.mins, mins);
			Vector2Copy(cm_world.maxs, maxs);

			for ( i = 1; ; i = sector->tree.child[1] )
			{
				while ( 1 )
				{
					cm_world.sectors[i].contents.contentsEntities |= contents;
					sector = &cm_world.sectors[i];

					axis = cm_world.sectors[i].tree.axis;
					dist = cm_world.sectors[i].tree.dist;

					if ( absmin[axis] <= dist )
						break;

					mins[axis] = dist;

					if ( !sector->tree.child[0] )
						goto LABEL_13;

					i = sector->tree.child[0];
				}

				if ( dist <= absmax[axis] )
					break;

				maxs[axis] = dist;

				if ( !sector->tree.child[1] )
					goto LABEL_13;
			}

			if ( i == ent->worldSector && (ent->linkcontents & ~contents) == 0 )
			{
				ent->linkcontents = contents;
				Vector2Copy(absmin, ent->linkmin);
				Vector2Copy(absmax, ent->linkmax);
				return;
			}
LABEL_13:
			if ( !ent->worldSector )
				break;

			if ( i == ent->worldSector && (ent->linkcontents & ~contents) == 0 )
				goto LABEL_18;

			CM_UnlinkEntity(ent);
		}

		CM_AddEntityToNode(ent, i);
LABEL_18:
		ent->linkcontents = contents;
		Vector2Copy(absmin, ent->linkmin);
		Vector2Copy(absmax, ent->linkmax);
		CM_SortNode(i, mins, maxs);
	}
	else
	{
		CM_UnlinkEntity(ent);
	}
}

void CM_ClipMoveToEntities_r(moveclip_t *clip, unsigned short nodeIndex, const float *p1, const float *p2, trace_t *trace)
{
	float v5;
	float v6;
	float v7;
	float v8;
	float v9;
	int side;
	worldSector_s *node;
	float diff;
	float t1;
	float frac;
	float offset;
	float t2;
	float frac2;
	float absDiff;
	unsigned int entnum;
	float mid[4];
	svEntity_s *check;
	float p[4];

	p[0] = *p1;
	p[1] = p1[1];
	p[2] = p1[2];
	p[3] = p1[3];

	while ( 1 )
	{
		node = &cm_world.sectors[nodeIndex];

		if ( (clip->contentmask & node->contents.contentsEntities) == 0 )
			break;

		for ( entnum = node->contents.entities; entnum; entnum = check->nextEntityInWorldSector )
		{
			check = &sv.svEntities[entnum - 1];

			if ( (check->linkcontents & clip->contentmask) != 0 )
				SV_ClipMoveToEntity(clip, check, trace);
		}

		t1 = p[node->tree.axis] - node->tree.dist;
		t2 = p2[node->tree.axis] - node->tree.dist;
		offset = clip->outerSize[node->tree.axis];

		if ( (float)(t2 - t1) < 0.0 )
			v9 = p2[node->tree.axis] - node->tree.dist;
		else
			v9 = p[node->tree.axis] - node->tree.dist;

		if ( v9 < offset )
		{
			if ( (float)(t1 - t2) < 0.0 )
				v8 = p2[node->tree.axis] - node->tree.dist;
			else
				v8 = p[node->tree.axis] - node->tree.dist;

			if ( -offset < v8 )
			{
				if ( p[3] >= trace->fraction )
					return;

				diff = t2 - t1;

				if ( (float)(t2 - t1) == 0.0 )
				{
					side = 0;
					frac = 1.0;
					frac2 = 0.0;
				}
				else
				{
					absDiff = fabs(diff);

					if ( diff < 0.0 )
						v7 = p[node->tree.axis] - node->tree.dist;
					else
						v7 = -t1;

					frac2 = (float)(v7 - offset) * (float)(1.0 / absDiff);
					frac = (float)(v7 + offset) * (float)(1.0 / absDiff);
					side = diff >= 0.0;
				}

				if ( (float)(1.0 - frac) < 0.0 )
					v6 = 0.0;
				else
					v6 = frac;

				mid[0] = (float)((float)(*p2 - p[0]) * v6) + p[0];
				mid[1] = (float)((float)(p2[1] - p[1]) * v6) + p[1];
				mid[2] = (float)((float)(p2[2] - p[2]) * v6) + p[2];
				mid[3] = (float)((float)(p2[3] - p[3]) * v6) + p[3];

				CM_ClipMoveToEntities_r(clip, node->tree.child[side], p, mid, trace);

				if ( (float)(frac2 - 0.0) < 0.0 )
					v5 = 0.0;
				else
					v5 = frac2;

				p[0] = (float)((float)(*p2 - p[0]) * v5) + p[0];
				p[1] = (float)((float)(p2[1] - p[1]) * v5) + p[1];
				p[2] = (float)((float)(p2[2] - p[2]) * v5) + p[2];
				p[3] = (float)((float)(p2[3] - p[3]) * v5) + p[3];

				nodeIndex = node->tree.child[1 - side];
			}
			else
			{
				nodeIndex = node->tree.child[1];
			}
		}
		else
		{
			nodeIndex = node->tree.child[0];
		}
	}
}

void CM_ClipMoveToEntities(moveclip_t *clip, trace_t *trace)
{
	vec4_t end;
	vec4_t start;

	VectorCopy(clip->extents.start, start);
	VectorCopy(clip->extents.end, end);

	start[3] = 0.0;
	end[3] = trace->fraction;

	CM_ClipMoveToEntities_r(clip, 1, start, end, trace);
}

void CM_PointTraceToEntities_r(pointtrace_t *clip, unsigned short nodeIndex, const float *p1, const float *p2, trace_t *trace)
{
	float v6;
	worldSector_s *node;
	float t1;
	float frac;
	float t2;
	unsigned int entnum;
	float mid[4];
	svEntity_s *check;
	float p[4];

	p[0] = *p1;
	p[1] = p1[1];
	p[2] = p1[2];
	p[3] = p1[3];

	while ( 1 )
	{
		node = &cm_world.sectors[nodeIndex];

		if ( (clip->contentmask & node->contents.contentsEntities) == 0 )
			break;

		for ( entnum = node->contents.entities; entnum; entnum = check->nextEntityInWorldSector )
		{
			check = &sv.svEntities[entnum - 1];
			SV_PointTraceToEntity(clip, check, trace);
		}

		t1 = p[node->tree.axis] - node->tree.dist;
		t2 = p2[node->tree.axis] - node->tree.dist;

		if ( (float)(t1 * t2) < 0.0 )
		{
			if ( p[3] >= trace->fraction )
				return;

			frac = t1 / (float)(t1 - t2);

			mid[0] = (float)((float)(*p2 - p[0]) * frac) + p[0];
			mid[1] = (float)((float)(p2[1] - p[1]) * frac) + p[1];
			mid[2] = (float)((float)(p2[2] - p[2]) * frac) + p[2];
			mid[3] = (float)((float)(p2[3] - p[3]) * frac) + p[3];

			CM_PointTraceToEntities_r(clip, node->tree.child[t2 >= 0.0], p, mid, trace);
			nodeIndex = node->tree.child[t2 < 0.0];

			p[0] = mid[0];
			p[1] = mid[1];
			p[2] = mid[2];
			p[3] = mid[3];
		}
		else
		{
			if ( (float)(t2 - t1) < 0.0 )
				v6 = p2[node->tree.axis] - node->tree.dist;
			else
				v6 = p[node->tree.axis] - node->tree.dist;

			nodeIndex = node->tree.child[v6 < 0.0];
		}
	}
}

void CM_PointTraceToEntities(pointtrace_t *clip, trace_t *trace)
{
	vec4_t end;
	vec4_t start;

	VectorCopy(clip->extents.start, start);
	VectorCopy(clip->extents.end, end);

	start[3] = 0.0;
	end[3] = trace->fraction;

	CM_PointTraceToEntities_r(clip, 1, start, end, trace);
}

void CM_AreaEntities_r(unsigned short nodeIndex, areaParms_t *ap)
{
	struct worldSector_s *node;
	gentity_t *gcheck;
	int en;
	unsigned short nextNodeIndex;
	int gnum;

	for (node = &cm_world.sectors[nodeIndex] ; node->contents.contentsEntities & ap->contentmask; node = &cm_world.sectors[nodeIndex])
	{
		for(en = node->contents.entities; en > 0; en = sv.svEntities[gnum].nextEntityInWorldSector)
		{
			gnum = en -1;
			gcheck = SV_GentityNum(gnum);
			if ( gcheck->r.contents & ap->contentmask )
			{
				if ( gcheck->r.absmin[0] > ap->maxs[0]
				        || gcheck->r.absmin[1] > ap->maxs[1]
				        || gcheck->r.absmin[2] > ap->maxs[2]
				        || gcheck->r.absmax[0] < ap->mins[0]
				        || gcheck->r.absmax[1] < ap->mins[1]
				        || gcheck->r.absmax[2] < ap->mins[2] )
				{
					continue;
				}
				if ( ap->count == ap->maxcount )
				{
					Com_DPrintf("CM_AreaEntities: MAXCOUNT\n");
					return;
				}
				ap->list[ap->count] = gnum;
				++ap->count;
			}
		}

		if ( node->tree.dist >= ap->maxs[node->tree.axis] )
		{
			nodeIndex = node->tree.child[1];
			if ( node->tree.dist <= ap->mins[node->tree.axis] )
			{
				return;
			}
		}
		else if ( node->tree.dist <= ap->mins[node->tree.axis] )
		{
			nodeIndex = node->tree.child[0];
		}
		else
		{
			nextNodeIndex = node->tree.child[1];
			CM_AreaEntities_r(node->tree.child[0], ap);
			nodeIndex = nextNodeIndex;
		}
	}
}

int CM_AreaEntities(const float *mins, const float *maxs, int *entityList, int maxcount, int contentmask)
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

void CM_ClearWorld()
{
	vec2_t bounds;
	unsigned int i;

	memset(&cm_world, 0, sizeof(cm_world));
	CM_ModelBounds(0, cm_world.mins, cm_world.maxs);
	cm_world.freeHead = 2;

	for ( i = 2; i <= 1022; ++i )
		cm_world.sectors[i].tree.nextFree = i + 1;

	cm_world.sectors[1023].tree.nextFree = 0;
	Vector2Subtract(cm_world.maxs, cm_world.mins, bounds);
	cm_world.sectors[1].tree.axis = bounds[1] >= bounds[0];
	cm_world.sectors[1].tree.dist = (cm_world.maxs[bounds[1] >= bounds[0]] + cm_world.mins[bounds[1] >= bounds[0]]) * 0.5;
}

int CM_PointTraceStaticModelsComplete_r(staticmodeltrace_t *clip, unsigned short nodeIndex, const float *p1_, const float *p2)
{
	float v5;
	int side;
	worldSector_s *node;
	unsigned short modelnum;
	float t1;
	float frac;
	float p1[3];
	float t2;
	float mid[3];
	cStaticModel_s *check;

	p1[0] = *p1_;
	p1[1] = p1_[1];

	for ( p1[2] = p1_[2]; ; p1[2] = mid[2] )
	{
		while ( 1 )
		{
			node = &cm_world.sectors[nodeIndex];

			if ( (clip->contents & node->contents.contentsStaticModels) == 0 )
				return 1;

			for ( modelnum = node->contents.staticModels; modelnum; modelnum = check->writable.nextModelInWorldSector )
			{
				check = &cm.staticModelList[modelnum - 1];

				if ( (clip->contents & XModelGetContents(check->xmodel)) != 0
				        && !CM_TraceBox(&clip->extents, check->absmin, check->absmax, 1.0)
				        && !CM_TraceStaticModelComplete(check, clip->extents.start, clip->extents.end, clip->contents) )
				{
					return 0;
				}
			}

			t1 = p1[node->tree.axis] - node->tree.dist;
			t2 = p2[node->tree.axis] - node->tree.dist;

			if ( (float)(t1 * t2) < 0.0 )
				break;

			if ( (float)(t2 - t1) < 0.0 )
				v5 = t2;
			else
				v5 = t1;

			nodeIndex = node->tree.child[v5 < 0.0];
		}

		frac = t1 / (float)(t1 - t2);

		mid[0] = (float)((float)(*p2 - p1[0]) * frac) + p1[0];
		mid[1] = (float)((float)(p2[1] - p1[1]) * frac) + p1[1];
		mid[2] = (float)((float)(p2[2] - p1[2]) * frac) + p1[2];

		side = t2 >= 0.0;

		if ( !CM_PointTraceStaticModelsComplete_r(clip, node->tree.child[side], p1, mid) )
			break;

		nodeIndex = node->tree.child[1 - side];

		p1[0] = mid[0];
		p1[1] = mid[1];
	}

	return 0;
}

int CM_PointTraceStaticModelsComplete(const float *start, const float *end, int contentmask)
{
	staticmodeltrace_t clip;

	clip.contents = contentmask;
	VectorCopy(start, clip.extents.start);
	VectorCopy(end, clip.extents.end);
	CM_CalcTraceExtents(&clip.extents);

	return CM_PointTraceStaticModelsComplete_r(&clip, 1, clip.extents.start, clip.extents.end);
}

void CM_PointTraceStaticModels_r(locTraceWork_t *tw, unsigned short nodeIndex, const float *p1_, const float *p2, trace_t *trace)
{
	float v5;
	int side;
	worldSector_s *node;
	unsigned short modelnum;
	float t1;
	float frac;
	float p1[4];
	float t2;
	float mid[4];
	cStaticModel_s *check;

	p1[0] = *p1_;
	p1[1] = p1_[1];
	p1[2] = p1_[2];
	p1[3] = p1_[3];

	while ( 1 )
	{
		node = &cm_world.sectors[nodeIndex];

		if ( (tw->contents & node->contents.contentsStaticModels) == 0 )
			break;

		for ( modelnum = node->contents.staticModels; modelnum; modelnum = check->writable.nextModelInWorldSector )
		{
			check = &cm.staticModelList[modelnum - 1];

			if ( (tw->contents & XModelGetContents(check->xmodel)) != 0
			        && !CM_TraceBox(&tw->extents, check->absmin, check->absmax, trace->fraction) )
			{
				CM_TraceStaticModel(check, trace, tw->extents.start, tw->extents.end, tw->contents);
			}
		}

		t1 = p1[node->tree.axis] - node->tree.dist;
		t2 = p2[node->tree.axis] - node->tree.dist;

		if ( (float)(t1 * t2) < 0.0 )
		{
			if ( p1[3] >= trace->fraction )
				return;

			frac = t1 / (float)(t1 - t2);

			mid[0] = (float)((float)(*p2 - p1[0]) * frac) + p1[0];
			mid[1] = (float)((float)(p2[1] - p1[1]) * frac) + p1[1];
			mid[2] = (float)((float)(p2[2] - p1[2]) * frac) + p1[2];
			mid[3] = (float)((float)(p2[3] - p1[3]) * frac) + p1[3];
			side = t2 >= 0.0;

			CM_PointTraceStaticModels_r(tw, node->tree.child[side], p1, mid, trace);
			nodeIndex = node->tree.child[1 - side];

			p1[0] = mid[0];
			p1[1] = mid[1];
			p1[2] = mid[2];
			p1[3] = mid[3];
		}
		else
		{
			if ( (float)(t2 - t1) < 0.0 )
				v5 = t2;
			else
				v5 = t1;

			nodeIndex = node->tree.child[v5 < 0.0];
		}
	}
}

void CM_PointTraceStaticModels(trace_t *results, const float *start, const float *end, int contentmask)
{
	vec4_t end_;
	vec4_t start_;
	locTraceWork_t tw;

	tw.contents = contentmask;
	VectorCopy(start, tw.extents.start);
	VectorCopy(end, tw.extents.end);
	CM_CalcTraceExtents(&tw.extents);
	VectorCopy(tw.extents.start, start_);
	start_[3] = 0.0;
	VectorCopy(tw.extents.end, end_);
	end_[3] = results->fraction;

	CM_PointTraceStaticModels_r(&tw, 1, start_, end_, results);
}

int CM_PointSightTraceToEntities_r(sightpointtrace_t *clip, unsigned short nodeIndex, const float *p1, const float *p2)
{
	float v5;
	worldSector_s *node;
	float t1;
	float frac;
	float t2;
	unsigned int entnum;
	int hitNum;
	float mid[3];
	svEntity_s *check;

	node = &cm_world.sectors[nodeIndex];

	if ( (clip->contentmask & node->contents.contentsEntities) == 0 )
		return 0;

	t1 = p1[node->tree.axis] - node->tree.dist;
	t2 = p2[node->tree.axis] - node->tree.dist;

	if ( (float)(t1 * t2) < 0.0 )
	{
		frac = t1 / (float)(t1 - t2);

		mid[0] = (float)((float)(*p2 - *p1) * frac) + *p1;
		mid[1] = (float)((float)(p2[1] - p1[1]) * frac) + p1[1];
		mid[2] = (float)((float)(p2[2] - p1[2]) * frac) + p1[2];

		hitNum = CM_PointSightTraceToEntities_r(clip, node->tree.child[t2 >= 0.0], p1, mid);

		if ( hitNum )
			return hitNum;

		hitNum = CM_PointSightTraceToEntities_r(clip, node->tree.child[t2 < 0.0], mid, p2);

		if ( hitNum )
			return hitNum;
	}
	else
	{
		if ( (float)(t2 - t1) < 0.0 )
			v5 = p2[node->tree.axis] - node->tree.dist;
		else
			v5 = p1[node->tree.axis] - node->tree.dist;

		hitNum = CM_PointSightTraceToEntities_r(clip, node->tree.child[v5 < 0.0], p1, p2);

		if ( hitNum )
			return hitNum;
	}

	for ( entnum = node->contents.entities; entnum; entnum = check->nextEntityInWorldSector )
	{
		check = &sv.svEntities[entnum - 1];
		hitNum = SV_PointSightTraceToEntity(clip, check);

		if ( hitNum )
			return hitNum;
	}

	return 0;
}

int CM_PointSightTraceToEntities(sightpointtrace_t *clip)
{
	return CM_PointSightTraceToEntities_r(clip, 1, clip->start, clip->end);
}

int CM_ClipSightTraceToEntities_r(sightclip_t *clip, unsigned short nodeIndex, const float *p1, const float *p2)
{
	float v5;
	float v6;
	float v7;
	float v8;
	float v9;
	int side;
	worldSector_s *node;
	float diff;
	float t1;
	float frac;
	float offset;
	float t2;
	float frac2;
	float absDiff;
	unsigned int entnum;
	int hitNum;
	float mid[3];
	svEntity_s *check;
	float p[3];

	p[0] = *p1;
	p[1] = p1[1];
	p[2] = p1[2];

	while ( 1 )
	{
		while ( 1 )
		{
			while ( 1 )
			{
				node = &cm_world.sectors[nodeIndex];

				if ( (clip->contentmask & node->contents.contentsEntities) == 0 )
					return 0;

				for ( entnum = node->contents.entities; entnum; entnum = check->nextEntityInWorldSector )
				{
					check = &sv.svEntities[entnum - 1];
					hitNum = SV_ClipSightToEntity(clip, check);

					if ( hitNum )
						return hitNum;
				}

				t1 = p[node->tree.axis] - node->tree.dist;
				t2 = p2[node->tree.axis] - node->tree.dist;
				offset = clip->outerSize[node->tree.axis];
				v9 = (float)(t2 - t1) < 0.0 ? p2[node->tree.axis] - node->tree.dist : p[node->tree.axis] - node->tree.dist;

				if ( v9 < offset )
					break;

				nodeIndex = node->tree.child[0];
			}

			v8 = (float)(t1 - t2) < 0.0 ? p2[node->tree.axis] - node->tree.dist : p[node->tree.axis] - node->tree.dist;

			if ( -offset < v8 )
				break;

			nodeIndex = node->tree.child[1];
		}

		diff = t2 - t1;

		if ( (float)(t2 - t1) == 0.0 )
		{
			side = 0;
			frac = 1.0;
			frac2 = 0.0;
		}
		else
		{
			absDiff = fabs(diff);

			if ( diff < 0.0 )
				v7 = p[node->tree.axis] - node->tree.dist;
			else
				v7 = -t1;

			frac2 = (float)(v7 - offset) * (float)(1.0 / absDiff);
			frac = (float)(v7 + offset) * (float)(1.0 / absDiff);
			side = diff >= 0.0;
		}

		v6 = (float)(1.0 - frac) < 0.0 ? 1.0 : frac;
		mid[0] = (float)((float)(*p2 - p[0]) * v6) + p[0];
		mid[1] = (float)((float)(p2[1] - p[1]) * v6) + p[1];
		mid[2] = (float)((float)(p2[2] - p[2]) * v6) + p[2];
		hitNum = CM_ClipSightTraceToEntities_r(clip, node->tree.child[side], p, mid);

		if ( hitNum )
			break;

		if ( (float)(frac2 - 0.0) < 0.0 )
			v5 = 0.0;
		else
			v5 = frac2;

		p[0] = (float)((float)(*p2 - p[0]) * v5) + p[0];
		p[1] = (float)((float)(p2[1] - p[1]) * v5) + p[1];
		p[2] = (float)((float)(p2[2] - p[2]) * v5) + p[2];

		nodeIndex = node->tree.child[1 - side];
	}

	return hitNum;
}

int CM_ClipSightTraceToEntities(sightclip_t *clip)
{
	return CM_ClipSightTraceToEntities_r(clip, 1, clip->start, clip->end);
}

void CM_LinkStaticModel(cStaticModel_s *staticModel)
{
	worldSector_s *sector;
	vec2_t maxs;
	vec2_t mins;
	int contents;
	float dist;
	int axis;
	unsigned short i;

	contents = XModelGetContents(staticModel->xmodel);

	Vector2Copy(cm_world.mins, mins);
	Vector2Copy(cm_world.maxs, maxs);

	for ( i = 1; ; i = sector->tree.child[1] )
	{
		while ( 1 )
		{
			cm_world.sectors[i].contents.contentsStaticModels |= contents;
			sector = &cm_world.sectors[i];
			axis = cm_world.sectors[i].tree.axis;
			dist = cm_world.sectors[i].tree.dist;

			if ( staticModel->absmin[axis] <= dist )
				break;

			mins[axis] = dist;

			if ( !sector->tree.child[0] )
				goto LINK;

			i = sector->tree.child[0];
		}

		if ( dist <= staticModel->absmax[axis] )
			break;

		maxs[axis] = dist;

		if ( !sector->tree.child[1] )
			break;
	}
LINK:
	CM_AddStaticModelToNode(staticModel, i);
	CM_SortNode(i, mins, maxs);
}

void CM_LinkAllStaticModels()
{
	int i;
	cStaticModel_s *model;

	i = 0;
	model = cm.staticModelList;

	while ( i < cm.numStaticModels )
	{
		if ( XModelGetContents(model->xmodel) )
			CM_LinkStaticModel(model);

		++i;
		++model;
	}
}

void CM_LinkWorld()
{
	CM_ClearWorld();
	CM_LinkAllStaticModels();
}