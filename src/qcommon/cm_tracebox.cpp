#include "qcommon.h"
#include "cm_local.h"

/*
===================
CM_CalcTraceEntents
===================
*/
void CM_CalcTraceEntents( TraceExtents *extents )
{
	float delta;
	float diff;
	int i;

	for ( i = 0; i < 3; ++i )
	{
		diff = extents->start[i] - extents->end[i];

		if ( diff == 0.0 )
			delta = 0.0;
		else
			delta = 1.0 / diff;

		extents->invDelta[i] = delta;
	}
}

/*
===================
CM_TraceBox
===================
*/
qboolean CM_TraceBox( TraceExtents *extents, const vec3_t mins, const vec3_t maxs, float fraction )
{
	int i;
	float f;
	float t2;
	float t1;
	float s;
	float o;

	o = 0.0;
	s = -1.0;

	while ( 1 )
	{
		for ( i = 0; i < 3; ++i )
		{
			t1 = (extents->start[i] - mins[i]) * s;
			t2 = (extents->end[i] - mins[i]) * s;

			if ( t1 > 0.0 )
			{
				if ( t2 > 0.0 )
				{
					return qtrue;
				}

				f = t1 * extents->invDelta[i] * s;

				if ( f >= fraction )
				{
					return qtrue;
				}

				o = I_fmax(o, f);
			}
			else
			{
				if ( t2 > 0.0 )
				{
					f = t1 * extents->invDelta[i] * s;

					if ( o >= f )
					{
						return qtrue;
					}

					fraction = I_fmin(fraction, f);
				}
			}
		}

		if ( s == 1.0 )
		{
			break;
		}

		s = 1.0;
		mins = maxs;
	}

	return qfalse;
}