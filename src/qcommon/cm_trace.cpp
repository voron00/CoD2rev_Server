#include "qcommon.h"
#include "cm_local.h"
#include "sys_thread.h"

#ifdef TESTING_LIBRARY
#define cm (*((clipMap_t*)( 0x08185BE0 )))
#define cme (*((clipMapExtra_t*)( 0x08185CF4 )))
#else
extern clipMap_t cm;
extern clipMapExtra_t cme;
#endif

cmodel_t *CM_ClipHandleToModel( clipHandle_t handle )
{
	if ( handle < 0 )
	{
		Com_Error( ERR_DROP, "CM_ClipHandleToModel: bad handle %i", handle );
	}

	if ( handle < cm.numSubModels )
	{
		return &cm.cmodels[handle];
	}
	else
	{
		TraceThreadInfo *tti = (TraceThreadInfo *)Sys_GetValue(3);
		cmodel_t *box_model = tti->box_model;
		return box_model;
	}
}