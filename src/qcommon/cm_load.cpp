#include "qcommon.h"
#include "cm_local.h"
#include "sys_thread.h"

clipMap_t cm;
clipMapExtra_t cme;

void CM_InitThreadData(int threadContext)
{
	TraceThreadInfo *traceThreadInfo;
	cbrush_t *box_brush;
	cmodel_t *box_model;

	traceThreadInfo = &g_traceThreadInfo[threadContext];

	traceThreadInfo->global = 0;
	traceThreadInfo->partitions = (int *)Hunk_Alloc(2 * cm.partitionCount);
	traceThreadInfo->edges = (int *)Hunk_Alloc(4 * cm.edgeCount);
	traceThreadInfo->verts = (int *)Hunk_Alloc(4 * cm.vertCount);

	box_brush = (cbrush_t *)Hunk_Alloc(sizeof(cbrush_t));
	traceThreadInfo->box_brush = box_brush;
	memcpy(box_brush, &cm.box_brush, sizeof(cbrush_t));

	box_model = (cmodel_t *)Hunk_Alloc(sizeof(cmodel_t));
	traceThreadInfo->box_model = box_model;
	memcpy(box_model, &cm.box_model, sizeof(cmodel_t));
}

void* CM_Hunk_Alloc(size_t size)
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
}

void CM_Shutdown()
{
	Com_Memset(&cm, 0, sizeof(clipMap_t));
}

void CM_Cleanup(void)
{
	cme.header = 0;
}