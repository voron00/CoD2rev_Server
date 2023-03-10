#include "qcommon.h"
#include "cm_local.h"

void QDECL CM_CalcTraceExtents(TraceExtents *extents)
{
	float delta;
	float diff;
	int i;

	for ( i = 0; i <= 2; ++i )
	{
		diff = extents->start[i] - extents->end[i];

		if ( diff == 0.0 )
			delta = 0.0;
		else
			delta = 1.0 / diff;

		extents->invDelta[i] = delta;
	}
}