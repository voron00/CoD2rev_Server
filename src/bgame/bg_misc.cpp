#include "../qcommon/qcommon.h"
#include "bg_public.h"

#ifdef TESTING_LIBRARY
#define level_bgs (*((bgs_t*)( 0x0859EA40 )))
#else
bgs_t level_bgs;
#endif

void BG_AddPredictableEventToPlayerstate( int newEvent, int eventParm, playerState_t *ps )
{
	ps->events[ps->eventSequence & ( MAX_EVENTS - 1 )] = newEvent;
	ps->eventParms[ps->eventSequence & ( MAX_EVENTS - 1 )] = eventParm;
	ps->eventSequence++;
}