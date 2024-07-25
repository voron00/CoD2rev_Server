#include "qcommon.h"
#include "cm_local.h"
#include "sys_thread.h"

clipMap_t cm;

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
CM_NumInlineModels
===================
*/
int CM_NumInlineModels( void )
{
	return cm.numSubModels;
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

	tti->partitions = (uint16_t *)Hunk_Alloc( cm.partitionCount * sizeof( *tti->partitions ) );
	tti->edges = (int32_t *)Hunk_Alloc( cm.edgeCount * sizeof( *tti->edges ) );
	tti->verts = (int32_t *)Hunk_Alloc( cm.vertCount * sizeof( *tti->verts ) );

	tti->box_brush = (cbrush_t *)Hunk_Alloc( sizeof( *tti->box_brush ) );
	tti->box_brush = cm.box_brush;

	tti->box_model = (cmodel_t *)Hunk_Alloc( sizeof( *tti->box_model ) );
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
CM_LoadMapInternal
===================
*/
void CM_LoadMapInternal( const char *name )
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
		CM_LoadMapInternal(name);
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
	Com_Memset(&cm, 0, sizeof(cm));
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