#include "qcommon.h"
#include "cm_local.h"

void CM_TraceCapsuleThroughCapsule(traceWork_s *tw, trace_t *trace)
{
	float halfwidth;
	float lengthZ;
	float offs;
	float radius;
	float halfheight;
	vec3_t symetricSize[2];
	vec3_t offset;
	vec3_t endpos;
	vec3_t botom;
	vec3_t top;
	vec3_t startbottom;
	vec3_t starttop;
	int i;

	VectorCopy(tw->extents.start, starttop);
	starttop[2] = starttop[2] + tw->radiusOffset[0];
	VectorCopy(tw->extents.start, startbottom);
	startbottom[2] = startbottom[2] - tw->radiusOffset[0];

	for ( i = 0; i < 3; ++i )
	{
		offset[i] = (tw->threadInfo.box_model->mins[i] + tw->threadInfo.box_model->maxs[i]) * 0.5;
		symetricSize[0][i] = tw->threadInfo.box_model->mins[i] - offset[i];
		symetricSize[1][i] = tw->threadInfo.box_model->maxs[i] - offset[i];
	}

	if ( symetricSize[1][0] <= (long double)symetricSize[1][2] )
		halfwidth = symetricSize[1][0];
	else
		halfwidth = symetricSize[1][2];

	halfheight = symetricSize[1][2] - halfwidth;
	radius = (tw->offsetZ + halfwidth) * (tw->offsetZ + halfwidth);
	VectorCopy(offset, top);
	top[2] = top[2] + halfheight;
	VectorSubtract(top, starttop, endpos);

	if ( radius > VectorLengthSquared(endpos) )
	{
		trace->allsolid = 1;
		trace->startsolid = 1;
		trace->fraction = 0.0;
	}

	VectorSubtract(top, startbottom, endpos);

	if ( radius > VectorLengthSquared(endpos) )
	{
		trace->allsolid = 1;
		trace->startsolid = 1;
		trace->fraction = 0.0;
	}

	VectorCopy(offset, botom);
	botom[2] = botom[2] - halfheight;
	VectorSubtract(botom, starttop, endpos);

	if ( radius > VectorLengthSquared(endpos) )
	{
		trace->allsolid = 1;
		trace->startsolid = 1;
		trace->fraction = 0.0;
	}

	VectorSubtract(botom, startbottom, endpos);

	if ( radius > VectorLengthSquared(endpos) )
	{
		trace->allsolid = 1;
		trace->startsolid = 1;
		trace->fraction = 0.0;
	}

	offs = tw->extents.start[2] - offset[2];
	lengthZ = halfheight + tw->size[2] - tw->offsetZ;

	if ( lengthZ >= fabs(offs) )
	{
		top[2] = 0.0;
		starttop[2] = 0.0;
		VectorSubtract(starttop, top, endpos);

		if ( radius > VectorLengthSquared(endpos) )
		{
			trace->allsolid = 1;
			trace->startsolid = 1;
			trace->fraction = 0.0;
		}
	}
}