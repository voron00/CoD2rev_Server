#include "../qcommon/qcommon.h"
#include "bg_public.h"

int PM_GetViewHeightLerpTime(const playerState_s *ps, int iTarget, int bDown)
{
	if ( iTarget == 11 )
		return 400;

	if ( iTarget != 40 )
		return 200;

	if ( bDown )
		return 200;

	return 400;
}

int PM_GetEffectiveStance(const playerState_s *ps)
{
	if ( ps->viewHeightTarget == 40 )
		return 2;
	else
		return ps->viewHeightTarget == 11;
}

void PM_AddEvent(playerState_s *ps, int newEvent)
{
	BG_AddPredictableEventToPlayerstate(newEvent, 0, ps);
}

int CL_LocalClient_GetActiveCount()
{
	return 1;
}