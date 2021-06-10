#include "i_common.h"
#include "i_shared.h"
#include "sys_thread.h"
#include "cm_local.h"

#if TESTING_LIBRARY == 1
#define cm (*((clipMap_t*)( 0x08185BE0 )))
#else
static clipMap_t cm;
#endif

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