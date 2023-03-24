#include "qcommon.h"
#include "cm_local.h"
#include "sys_thread.h"

clipMap_t cm;
clipMapExtra_t cme;

void CM_InitThreadData(int threadContext)
{
	TraceThreadInfo *tti;

	tti = &g_traceThreadInfo[threadContext];
	tti->checkcount = 0;
	tti->partitions = (uint16_t *)Hunk_Alloc(sizeof(uint16_t) * cm.partitionCount);
	tti->edges = (int *)Hunk_Alloc(sizeof(int32_t) * cm.edgeCount);
	tti->verts = (int *)Hunk_Alloc(sizeof(int32_t) * cm.vertCount);
	tti->box_brush = (cbrush_t *)Hunk_Alloc(sizeof(cbrush_t));
	Com_Memcpy(tti->box_brush, cm.box_brush, sizeof(cbrush_t));
	tti->box_model = (cmodel_t *)Hunk_Alloc(sizeof(cmodel_t));
	Com_Memcpy(tti->box_model, &cm.box_model, sizeof(cmodel_t));
}

clipHandle_t CM_TempBoxModel(const vec3_t mins, const vec3_t maxs, int capsule)
{
	TraceThreadInfo* tti = (TraceThreadInfo *)Sys_GetValue(THREAD_VALUE_TRACE);

	cmodel_t* box_model = tti->box_model;
	cbrush_t* box_brush = tti->box_brush;

	VectorCopy( mins, box_model->mins );
	VectorCopy( maxs, box_model->maxs );

	VectorCopy( mins, box_brush->mins );
	VectorCopy( maxs, box_brush->maxs );

	return CAPSULE_MODEL_HANDLE;
}

int CM_LeafCluster( int leafnum )
{
	if ( leafnum < 0 || leafnum >= cm.numLeafs )
	{
		Com_Error( ERR_DROP, "CM_LeafCluster: bad number %i", leafnum );
	}

	return cm.leafs[leafnum].cluster;
}

void CM_ModelBounds( clipHandle_t model, vec3_t mins, vec3_t maxs )
{
	cmodel_t    *cmod;

	cmod = CM_ClipHandleToModel( model );
	VectorCopy( cmod->mins, mins );
	VectorCopy( cmod->maxs, maxs );
}

void* CM_Hunk_Alloc(int size)
{
	return Hunk_Alloc(size);
}

void CM_Hunk_CheckTempMemoryHighClear()
{
	;
}

void CM_Hunk_ClearTempMemoryHigh()
{
	Hunk_ClearTempMemoryHighInternal();
}

void* CM_Hunk_AllocateTempMemoryHigh(int size)
{
	return Hunk_AllocateTempMemoryHighInternal(size);
}

void CM_InitAllThreadData()
{
	CM_InitThreadData(THREAD_CONTEXT_MAIN);
}

void CM_LoadMapData(const char *name)
{
	CM_LoadMapFromBsp(name, 1);
	CM_LoadStaticModels();
}

void CM_LoadMap(const char *name, int *checksum)
{
	if ( !name || !*name )
	{
		Com_Error(ERR_DROP, "CM_LoadMap: NULL name");
	}

	CM_LoadMapData(name);
	CM_InitAllThreadData();
	*checksum = cm.checksum;

#ifdef TESTING_LIBRARY
	Com_Memcpy((clipMap_t *)0x08185BE0, &cm, sizeof(clipMap_t));
	Com_Memcpy((clipMapExtra_t *)0x08185CF4, &cme, sizeof(clipMapExtra_t));
#endif
}

void CM_Shutdown()
{
	Com_Memset(&cm, 0, sizeof(clipMap_t));
}

void CM_Cleanup(void)
{
	cme.header = 0;
}