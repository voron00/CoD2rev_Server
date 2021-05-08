#include "i_shared.h"
#include "sys_thread.h"
#include "cm_local.h"

static clipMap_t cm;

cbrush_t g_box_brush[NUMTHREADS];
cmodel_t g_box_model[NUMTHREADS];

void CM_InitThreadData(int threadContext)
{
	TraceThreadInfo *traceThreadInfo;

	traceThreadInfo = &g_traceThreadInfo[threadContext];
	traceThreadInfo->checkcount.global = 0;
	//traceThreadInfo->checkcount.partitions = (int *)Hunk_Alloc(4 * cm.partitionCount);
	traceThreadInfo->box_brush = &g_box_brush[threadContext];
	memcpy(traceThreadInfo->box_brush, cm.box_brush, sizeof(cbrush_t));
	traceThreadInfo->box_model = &g_box_model[threadContext];
	memcpy(traceThreadInfo->box_model, &cm.box_model, sizeof(cmodel_t));
}