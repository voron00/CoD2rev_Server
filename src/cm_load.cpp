#include "i_common.h"
#include "i_shared.h"
#include "sys_thread.h"
#include "cm_local.h"

static clipMap_t cm;

void CM_InitThreadData(int threadContext)
{
	TraceThreadInfo *traceThreadInfo;
	cbrush_t *box_brush;
	cmodel_t *box_model;

	traceThreadInfo = &g_traceThreadInfo[threadContext];

	traceThreadInfo->global = 0;
	traceThreadInfo->partitions = (int *)Hunk_AllocInternal(2 * cm.partitionCount);
	traceThreadInfo->edges = (int *)Hunk_AllocInternal(4 * cm.edgeCount);
	traceThreadInfo->verts = (int *)Hunk_AllocInternal(4 * cm.vertCount);

	box_brush = (cbrush_t *)Hunk_AllocInternal(sizeof(cbrush_t));
	traceThreadInfo->box_brush = box_brush;
	memcpy(box_brush, &cm.box_brush, sizeof(cbrush_t));

	box_model = (cmodel_t *)Hunk_AllocInternal(sizeof(cmodel_t));
	traceThreadInfo->box_model = box_model;
	memcpy(box_model, &cm.box_model, sizeof(cmodel_t));
}