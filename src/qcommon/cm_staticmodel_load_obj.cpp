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

void CM_LoadStaticModels()
{
	UNIMPLEMENTED(__FUNCTION__);
}