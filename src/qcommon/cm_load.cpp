#include "qcommon.h"
#include "cm_local.h"
#include "sys_thread.h"

clipMap_t cm;

/*
===================
CM_TempBoxModel

To keep everything totally uniform, bounding boxes are turned into small
BSP trees instead of being compared directly.
Capsules are handled differently though.
===================
*/
clipHandle_t CM_TempBoxModel( const vec3_t mins, const vec3_t maxs, int contents )
{
	cmodel_t *box_model;
	cbrush_t *box_brush;

	CM_GetBox(&box_brush, &box_model);

	VectorCopy(mins, box_model->mins);
	VectorCopy(maxs, box_model->maxs);

	VectorCopy(mins, box_brush->mins);
	VectorCopy(maxs, box_brush->maxs);

// DHM - Nerve
	box_brush->contents = contents;
// dhm

	return BOX_MODEL_HANDLE;
}

/*
==================
CM_LeafCluster
==================
*/
int CM_LeafCluster( int leafnum )
{
	if ( leafnum < 0 || leafnum >= cm.numLeafs )
	{
		Com_Error( ERR_DROP, "CM_LeafCluster: bad number %i", leafnum );
	}

	return cm.leafs[leafnum].cluster;
}

/*
===================
CM_ModelBounds
===================
*/
void CM_ModelBounds( clipHandle_t model, vec3_t mins, vec3_t maxs )
{
	cmodel_t    *cmod;

	cmod = CM_ClipHandleToModel( model );
	VectorCopy( cmod->mins, mins );
	VectorCopy( cmod->maxs, maxs );
}

/*
===================
CM_Hunk_Alloc
===================
*/
void* CM_Hunk_Alloc( int size )
{
	return Hunk_Alloc(size);
}

/*
===================
CM_Hunk_CheckTempMemoryHighClear
===================
*/
void CM_Hunk_CheckTempMemoryHighClear()
{
	;
}

/*
===================
CM_Hunk_ClearTempMemoryHigh
===================
*/
void CM_Hunk_ClearTempMemoryHigh()
{
	Hunk_ClearTempMemoryHighInternal();
}

/*
===================
CM_Hunk_AllocateTempMemoryHigh
===================
*/
void* CM_Hunk_AllocateTempMemoryHigh( int size )
{
	return Hunk_AllocateTempMemoryHighInternal(size);
}

/*
===================
CM_EntityString
===================
*/
char *CM_EntityString()
{
	return cm.entityString;
}

/*
===================
CM_InitThreadData
===================
*/
void CM_InitThreadData( int threadContext )
{
	TraceThreadInfo *tti;

	tti = &g_traceThreadInfo[threadContext];

	tti->checkcount = 0;

	tti->partitions = (uint16_t *)Hunk_Alloc(sizeof(uint16_t) * cm.partitionCount);
	tti->edges = (int32_t *)Hunk_Alloc(sizeof(int32_t) * cm.edgeCount);
	tti->verts = (int32_t *)Hunk_Alloc(sizeof(int32_t) * cm.vertCount);

	tti->box_brush = (cbrush_t *)Hunk_Alloc(sizeof(cbrush_t));
	tti->box_brush = cm.box_brush;

	tti->box_model = (cmodel_t *)Hunk_Alloc(sizeof(cmodel_t));
	tti->box_model = &cm.box_model;
}

/*
===================
CM_InitAllThreadData
===================
*/
void CM_InitAllThreadData()
{
	CM_InitThreadData(THREAD_CONTEXT_MAIN);
}

/*
===================
CM_LoadMapData
===================
*/
void CM_LoadMapData( const char *name )
{
	CM_LoadMapFromBsp(name, true);
	CM_LoadStaticModels();
}

/*
===================
CM_LoadMap
===================
*/
void CM_LoadMap( const char *name, int *checksum )
{
	if ( !name || !*name )
	{
		Com_Error(ERR_DROP, "CM_LoadMap: NULL name");
	}

	if ( !cm.name || strcasecmp(cm.name, name) )
	{
		CM_LoadMapData(name);
		CM_InitAllThreadData();
	}

	*checksum = cm.checksum;
}

/*
===================
CM_Shutdown
===================
*/
void CM_Shutdown()
{
	Com_Memset(&cm, 0, sizeof(clipMap_t));
}

/*
===================
CM_Cleanup
===================
*/
void CM_Cleanup( void )
{
	cm.header = NULL;
}