#include "../qcommon/qcommon.h"
#include "g_shared.h"

#ifdef TESTING_LIBRARY
#define level (*((level_locals_t*)( 0x0859B400 )))
#else
extern level_locals_t level;
#endif

clientState_t* G_GetClientState(int num)
{
	return &level.clients[num].sess.state;
}

int G_GetClientArchiveTime(int clientindex)
{
	return level.clients[clientindex].sess.archiveTime;
}

void G_SetClientArchiveTime(int clindex, int time)
{
	level.clients[clindex].sess.archiveTime = time;
}