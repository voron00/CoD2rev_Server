#include "qcommon.h"
#include "cm_local.h"
#include "../server/server.h"

#ifdef TESTING_LIBRARY
#define cm (*((clipMap_t*)( 0x08185BE0 )))
#define cme (*((clipMapExtra_t*)( 0x08185CF4 )))
#else
extern clipMap_t cm;
extern clipMapExtra_t cme;
#endif

#ifdef TESTING_LIBRARY
#define svs (*((serverStatic_t*)( 0x0841FB00 )))
#define sv (*((server_t*)( 0x0842BC80 )))
#else
extern server_t sv;
extern serverStatic_t svs;
#endif

#ifdef TESTING_LIBRARY
#define cm_world (*((cm_world_t*)( 0x08185D80 )))
#else
cm_world_t cm_world;
#endif

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

	if ( model->leaf.brushContents + model->leaf.terrainContents )
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