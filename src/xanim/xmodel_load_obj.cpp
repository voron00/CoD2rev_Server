#include "../qcommon/qcommon.h"

qboolean XModelGetStaticBounds(const XModel *model, const float *axis, float *mins, float *maxs)
{
	vec3_t bounds;
	vec3_t out;
	vec3_t in;
	int j;
	int k;
	int i;
	XModelCollSurf *surf;

	if ( !model->numCollSurfs )
		return 0;

	VectorSet(mins, 3.4028235e38, 3.4028235e38, 3.4028235e38);
	VectorSet(maxs, -3.4028235e38, -3.4028235e38, -3.4028235e38);

	for ( i = 0; i < model->numCollSurfs; ++i )
	{
		surf = &model->collSurfs[i];

		for ( j = 0; j < 8; ++j )
		{
			if ( (j & 1) != 0 )
				bounds[0] = surf->mins[0];
			else
				bounds[0] = surf->maxs[0];

			in[0] = bounds[0];

			if ( (j & 2) != 0 )
				bounds[1] = surf->mins[1];
			else
				bounds[1] = surf->maxs[1];

			in[1] = bounds[1];

			if ( (j & 4) != 0 )
				bounds[2] = surf->mins[2];
			else
				bounds[2] = surf->maxs[2];

			in[2] = bounds[2];

			MatrixTransformVector(in, axis, out);

			for ( k = 0; k < 3; ++k )
			{
				if ( mins[k] > out[k] )
					mins[k] = out[k];

				if ( out[k] > maxs[k] )
					maxs[k] = out[k];
			}
		}
	}

	return 1;
}