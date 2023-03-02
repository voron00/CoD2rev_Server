#include "qcommon.h"

char objBuf[204800];
short serverObjMap[2048];
bool objAlloced[2048];

int objFreeCount;
int com_lastDObjIndex;
int g_bDObjInited;

int Com_GetFreeDObjIndex()
{
	int i;

	for ( i = com_lastDObjIndex + 1; i < 2048; ++i )
	{
		if ( !objAlloced[i] )
		{
			com_lastDObjIndex = i;
			objAlloced[i] = 1;
			--objFreeCount;

			return i;
		}
	}

	for ( i = 1; i < com_lastDObjIndex; ++i )
	{
		if ( !objAlloced[i] )
		{
			com_lastDObjIndex = i;
			objAlloced[i] = 1;
			--objFreeCount;

			return i;
		}
	}

	return 0;
}

void Com_ServerDObjCreate(DObjModel_s *dobjModels, unsigned short numModels, XAnimTree_s *tree, int handle)
{
	int index;

	index = Com_GetFreeDObjIndex();
	serverObjMap[handle] = index;

	DObjCreate(dobjModels, numModels, tree, (void *)&objBuf[sizeof(DObj) * index], handle + 1);

	if ( !objFreeCount )
		Com_Error(ERR_DROP, "No free DObjs");
}

void Com_ServerDObjFree(int handle)
{
	int index;

	index = serverObjMap[handle];

	if ( serverObjMap[handle] )
	{
		serverObjMap[handle] = 0;
		objAlloced[index] = 0;
		++objFreeCount;
		DObjFree((DObj_s *)(&objBuf[sizeof(DObj) * index]));
	}
}

void Com_SafeServerDObjFree(gentity_t *ent)
{
	Com_ServerDObjFree(ent->s.number);
}

DObj* Com_GetServerDObj(int handle)
{
	if ( serverObjMap[handle] )
		return (DObj *)&objBuf[sizeof(DObj) * serverObjMap[handle]];
	else
		return 0;
}

void Com_InitDObj()
{
	Com_Memset(objAlloced, 0, sizeof(objAlloced));
	objFreeCount = 2047;
	Com_Memset(serverObjMap, 0, sizeof(serverObjMap));
	com_lastDObjIndex = 1;
	g_bDObjInited = 1;
}

void Com_ShutdownDObj()
{
	if ( g_bDObjInited )
		g_bDObjInited = 0;
}

void Com_AbortDObj()
{
	g_bDObjInited = 0;
}