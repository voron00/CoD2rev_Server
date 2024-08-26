#include "qcommon.h"

#define DOBJ_HANDLE_MAX 2 << 10

DObj objBuf[DOBJ_HANDLE_MAX];

short clientObjMap[DOBJ_HANDLE_MAX];
short serverObjMap[DOBJ_HANDLE_MAX];

bool objAlloced[DOBJ_HANDLE_MAX];

int objFreeCount;
int com_lastDObjIndex;

qboolean g_bDObjInited;

/*
==================
Com_AbortDObj
==================
*/
void Com_AbortDObj()
{
	g_bDObjInited = qfalse;
}

/*
==================
Com_ShutdownDObj
==================
*/
void Com_ShutdownDObj()
{
	if ( !g_bDObjInited )
	{
		return;
	}

	g_bDObjInited = qfalse;
}

/*
==================
Com_ShutdownDObj
==================
*/
void Com_InitDObj()
{
	Com_Memset(objAlloced, 0, sizeof(objAlloced));
	objFreeCount = DOBJ_HANDLE_MAX - 1;

	Com_Memset(clientObjMap, 0, sizeof(clientObjMap));
	Com_Memset(serverObjMap, 0, sizeof(serverObjMap));

	com_lastDObjIndex = 1;
	g_bDObjInited = qtrue;
}

/*
==================
Com_GetServerDObj
==================
*/
DObj* Com_GetServerDObj( int handle )
{
	assert(((unsigned)handle < (sizeof( serverObjMap ) / (sizeof( serverObjMap[0] ) * (sizeof( serverObjMap ) != 4 || sizeof( serverObjMap[0] ) <= 4)))));
	assert((unsigned)serverObjMap[handle] < DOBJ_HANDLE_MAX);

	if ( !serverObjMap[handle] )
	{
		return NULL;
	}

	return &objBuf[serverObjMap[handle]];
}

/*
==================
Com_GetClientDObj
==================
*/
DObj* Com_GetClientDObj( int handle )
{
	assert((handle >= 0 && handle < ((((1<<10) + 512)) + 1)));
	assert(((unsigned)handle < (sizeof( clientObjMap ) / (sizeof( clientObjMap[0] ) * (sizeof( clientObjMap ) != 4 || sizeof( clientObjMap[0] ) <= 4)))));
	assert(((unsigned)clientObjMap[handle] < (2048+1024)));

	if ( !clientObjMap[handle] )
	{
		return NULL;
	}

	return &objBuf[clientObjMap[handle]];
}

/*
==================
Com_ClientDObjClearAllSkel
==================
*/
void Com_ClientDObjClearAllSkel()
{
	for ( int handleOffset = 0; handleOffset < DOBJ_HANDLE_MAX; handleOffset++ )
	{
		if ( !clientObjMap[handleOffset] )
		{
			continue;
		}

		DObjSkelClear(&objBuf[clientObjMap[handleOffset]]);
	}
}

/*
==================
Com_SafeServerDObjFree
==================
*/
void Com_SafeServerDObjFree( int handle )
{
	int index;

	assert(((unsigned)handle < ((1<<10))));
	index = serverObjMap[handle];

	if ( !index )
	{
		return;
	}

	serverObjMap[handle] = 0;
	assert((unsigned)index < ARRAY_COUNT( objAlloced ));

	objAlloced[index] = false;
	objFreeCount++;

	DObjFree(&objBuf[index]);
}

/*
==================
Com_SafeClientDObjFree
==================
*/
void Com_SafeClientDObjFree( int handle )
{
	int index;

	assert(((unsigned)handle < ((((1<<10) + 512)) + 1)));
	assert(((unsigned)handle < (sizeof( clientObjMap ) / (sizeof( clientObjMap[0] ) * (sizeof( clientObjMap ) != 4 || sizeof( clientObjMap[0] ) <= 4)))));

	index = clientObjMap[handle];

	if ( !index )
	{
		return;
	}

	clientObjMap[handle] = 0;
	assert((unsigned)index < ARRAY_COUNT( objAlloced ));

	objAlloced[index] = false;
	objFreeCount++;

	DObjFree(&objBuf[index]);
}

/*
==================
DB_LoadDObjs
==================
*/
void* DB_LoadDObjs( void * )
{
	UNIMPLEMENTED(__FUNCTION__);
	return NULL;
}

/*
==================
DB_SaveDObjs
==================
*/
void* DB_SaveDObjs( void * )
{
	UNIMPLEMENTED(__FUNCTION__);
	return NULL;
}

/*
==================
Com_ServerDObjCreate
==================
*/
void Com_ServerDObjCreate( DObjModel_s *dobjModels, unsigned short numModels, XAnimTree_s *tree, int handle )
{
	int index;

	assert(dobjModels);
	assert(((unsigned)handle < ((1<<10))));
	assert(!Com_GetServerDObj( handle ));

	index = Com_GetFreeDObjIndex();
	assert((unsigned)handle < ARRAY_COUNT( serverObjMap ));

	serverObjMap[handle] = index;
	assert((unsigned)index < DOBJ_HANDLE_MAX);

	DObjCreate(dobjModels, numModels, tree, &objBuf[index], handle + 1);

	if ( !objFreeCount )
	{
		Com_Error(ERR_DROP, "No free DObjs");
	}
}

/*
==================
Com_ClientDObjCreate
==================
*/
void Com_ClientDObjCreate( DObjModel_s *dobjModels, unsigned short numModels, XAnimTree_s *tree, int handle )
{
	int index;

	assert(dobjModels);
	assert(((unsigned)handle < ((((1<<10) + 512)) + 1)));
	assert(!Com_GetClientDObj( handle ));

	index = Com_GetFreeDObjIndex();
	assert((unsigned)handle < ARRAY_COUNT( clientObjMap ));

	clientObjMap[handle] = index;
	assert((unsigned)index < DOBJ_HANDLE_MAX);

	DObjCreate(dobjModels, numModels, tree, &objBuf[index], handle + 1);

	if ( !objFreeCount )
	{
		Com_Error(ERR_DROP, "No free DObjs");
	}
}

/*
==================
Com_GetFreeDObjIndex
==================
*/
int Com_GetFreeDObjIndex()
{
	int i;

	for ( i = com_lastDObjIndex + 1; i < DOBJ_HANDLE_MAX; i++ )
	{
		if ( objAlloced[i] )
		{
			continue;
		}

		com_lastDObjIndex = i;
		assert(i);
		assert((unsigned)i < ARRAY_COUNT( objAlloced ));

		objAlloced[i] = true;
		assert(objFreeCount);
		--objFreeCount;

		return i;
	}

	for ( i = 1; i < com_lastDObjIndex; i++ )
	{
		if ( objAlloced[i] )
		{
			continue;
		}

		com_lastDObjIndex = i;
		assert(i);
		assert((unsigned)i < ARRAY_COUNT( objAlloced ));

		objAlloced[i] = true;
		assert(objFreeCount);
		--objFreeCount;

		return i;
	}

	return 0;
}
