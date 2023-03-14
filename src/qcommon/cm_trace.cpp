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

cmodel_t *CM_ClipHandleToModel( clipHandle_t handle )
{
	if ( handle < 0 )
	{
		Com_Error( ERR_DROP, "CM_ClipHandleToModel: bad handle %i", handle );
	}

	if ( handle < cm.numSubModels )
	{
		return &cm.cmodels[handle];
	}
	else
	{
		TraceThreadInfo *tti = (TraceThreadInfo *)Sys_GetValue(3);
		cmodel_t *box_model = tti->box_model;
		return box_model;
	}
}

bool CM_CullBox(traceWork_t *tw, const float *origin, const float *halfSize)
{
	vec3_t distorig;
	vec3_t mid;
	int i;

	VectorSubtract(tw->midpoint, origin, distorig);
	VectorAdd(tw->size, halfSize, mid);

	for(i = 0; i < 3; ++i)
	{
		if ( tw->halfDeltaAbs[i] + mid[i] < fabs(distorig[i]) )
		{
			return 1;
		}
	}

	if ( tw->axialCullOnly)
	{
		return 0;
	}

	if(tw->halfDeltaAbs[1] * mid[2] + mid[1] * tw->halfDeltaAbs[2] < fabs(tw->halfDelta[1] * distorig[2] - tw->halfDelta[2] * distorig[1]))
	{
		return 1;
	}

	if(tw->halfDeltaAbs[0] * mid[2] + mid[0] * tw->halfDeltaAbs[2] < fabs(tw->halfDelta[2] * distorig[0] - tw->halfDelta[0] * distorig[2]))
	{
		return 1;
	}

	if(tw->halfDeltaAbs[1] * mid[0] + tw->halfDeltaAbs[0] * mid[1] < fabs(tw->halfDelta[0] * distorig[1] - tw->halfDelta[1] * distorig[0]))
	{
		return 1;
	}

	return 0;
}

void CM_TestBoxInBrush(traceWork_t *tw, cbrush_s *brush, trace_t *trace)
{
	float diff;
	float absDiff;
	cbrushside_s *side;
	float dist;
	float normalDiff;
	float *normal;
	unsigned int i;

	if ( tw->bounds[0][0] <= brush->maxs[0]
	        && tw->bounds[0][1] <= brush->maxs[1]
	        && tw->bounds[0][2] <= brush->maxs[2]
	        && brush->mins[0] <= tw->bounds[1][0]
	        && brush->mins[1] <= tw->bounds[1][1]
	        && brush->mins[2] <= tw->bounds[1][2] )
	{
		side = brush->sides;
		for ( i = brush->numsides; i; --i )
		{
			normal = side->plane->normal;
			diff = side->plane->normal[2] * tw->offsetZ;
			absDiff = fabs(diff);
			normalDiff = normal[3] + tw->radius + absDiff;
			dist = DotProduct(tw->extents.start, normal) - normalDiff;
			if ( dist > 0.0 )
				return;
			++side;
		}
		trace->allsolid = 1;
		trace->startsolid = 1;
		trace->fraction = 0.0;
		trace->contents = brush->contents;
	}
}

void CM_TestInLeafBrushNode_r(traceWork_t *tw, cLeafBrushNode_s *node, trace_t *trace)
{
	cbrush_s *brush;
	int i;

	while ( (node->contents & tw->contents) != 0 )
	{
		if ( node->leafBrushCount )
		{
			if ( node->leafBrushCount > 0 )
			{
				for ( i = 0; i < node->leafBrushCount; ++i )
				{
					brush = &cm.brushes[node->data.leaf.brushes[i]];
					if ( (brush->contents & tw->contents) != 0 )
					{
						CM_TestBoxInBrush(tw, brush, trace);
						if ( trace->allsolid )
							break;
					}
				}
				return;
			}
			CM_TestInLeafBrushNode_r(tw, node + 1, trace);
			if ( trace->allsolid )
				return;
		}
		if ( tw->bounds[0][node->axis] <= node->data.children.dist )
		{
			if ( tw->bounds[1][node->axis] >= node->data.children.dist )
			{
				CM_TestInLeafBrushNode_r(tw, &node[node->data.children.childOffset[0]], trace);
				if ( trace->allsolid )
					return;
			}
			node += node->data.children.childOffset[1];
		}
		else
		{
			node += node->data.children.childOffset[0];
		}
	}
}

int CM_TestInLeafBrushNode(traceWork_t *tw, cLeaf_s *leaf, trace_t *trace)
{
	int i;

	for ( i = 0; i <= 2; ++i )
	{
		if ( leaf->mins[i] >= tw->bounds[1][i] )
			return 0;

		if ( tw->bounds[0][i] >= leaf->maxs[i] )
			return 0;
	}

	CM_TestInLeafBrushNode_r(tw, &cm.leafbrushNodes[leaf->leafBrushNode], trace);
	return trace->allsolid;
}

void CM_TestInLeaf(traceWork_t *tw, cLeaf_s *leaf, trace_t *trace)
{
	if ( ((leaf->brushContents & tw->contents) == 0 || !CM_TestInLeafBrushNode(tw, leaf, trace))
	        && (leaf->terrainContents & tw->contents) != 0 )
	{
		CM_MeshTestInLeaf(tw, leaf, trace);
	}
}

void CM_PositionTest(traceWork_t *tw, trace_t *trace)
{
	leafList_s ll;
	int i;
	uint16_t leafs[1024];

	if ( !trace->allsolid )
	{
		VectorSubtract(tw->extents.start, tw->size, ll.bounds[0]);
		VectorAdd(tw->extents.start, tw->size, ll.bounds[1]);
		for ( i = 0; i <= 2; ++i )
		{
			ll.bounds[0][i] = ll.bounds[0][i] - 1.0;
			ll.bounds[1][i] = ll.bounds[1][i] + 1.0;
		}
		ll.count = 0;
		ll.maxcount = 1024;
		ll.list = leafs;
		ll.lastLeaf = 0;
		ll.overflowed = 0;
		CM_BoxLeafnums_r(&ll, 0);
		if ( ll.count )
		{
			for ( i = 0; i < ll.count && !trace->allsolid; ++i )
				CM_TestInLeaf(tw, &cm.leafs[leafs[i]], trace);
		}
	}
}

void CM_TestCapsuleInCapsule(traceWork_t *tw, trace_t *trace)
{
	float halfwidth;
	float lengthZ;
	float offs;
	float radius;
	float halfheight;
	vec3_t symetricSize[2];
	vec3_t offset;
	float endpos[4];
	vec3_t botom;
	vec3_t top;
	vec3_t startbottom;
	vec3_t starttop;
	int i;

	VectorCopy(tw->extents.start, starttop);
	starttop[2] = starttop[2] + tw->offsetZ;
	VectorCopy(tw->extents.start, startbottom);
	startbottom[2] = startbottom[2] - tw->offsetZ;
	for ( i = 0; i <= 2; ++i )
	{
		offset[i] = (tw->threadInfo.box_model->mins[i] + tw->threadInfo.box_model->maxs[i]) * 0.5;
		symetricSize[0][i] = tw->threadInfo.box_model->mins[i] - offset[i];
		symetricSize[1][i] = tw->threadInfo.box_model->maxs[i] - offset[i];
	}
	if ( symetricSize[1][0] <= symetricSize[1][2] )
		halfwidth = symetricSize[1][0];
	else
		halfwidth = symetricSize[1][2];
	halfheight = symetricSize[1][2] - halfwidth;
	radius = (tw->radius + halfwidth) * (tw->radius + halfwidth);
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
	lengthZ = halfheight + tw->size[2] - tw->radius;
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

int CM_TraceSphereThroughSphere(traceWork_t *tw, const float *vStart, const float *vEnd, const float *vStationary, float radius, trace_t *trace)
{
	vec3_t vDelta;
	float dr;
	float fDiscriminant;
	float fDeltaLen;
	vec3_t vNormal;
	float fEntry;
	float sqd;
	float fA;
	float fB;
	float fC;
	float s;
	float fRadiusSqrd;

	VectorSubtract(vStart, vStationary, vDelta);
	sqd = VectorLengthSquared(vDelta);

	dr = tw->radius + radius;
	fRadiusSqrd = dr * dr;
	s = sqd - fRadiusSqrd;

	fC = VectorLengthSquared(vDelta) - fRadiusSqrd;

	if ( fC > 0.0 )
	{
		fB = DotProduct(vDelta, tw->delta);
		if ( fB >= 0.0)
		{
			return 1;
		}
		fA = tw->deltaLenSq;

		fDiscriminant = fB * fB - s * tw->deltaLenSq;
		if(fDiscriminant < 0.0)
		{
			return 1;
		}

		fDeltaLen = Vec3NormalizeTo(vDelta, vNormal);
		fEntry = fDeltaLen * 0.125 / fB + (-fB - sqrt(fDiscriminant)) / fA;

		if(trace->fraction <= fEntry)
		{
			return 1;
		}
		trace->fraction = I_fmax(fEntry, 0.0);

		VectorCopy(vNormal, trace->normal);
		trace->contents = tw->threadInfo.box_brush->contents;
		return 0;
	}

	trace->fraction = 0.0;
	trace->startsolid = 1;
	trace->contents = tw->threadInfo.box_brush->contents;

	Vec3NormalizeTo(vDelta, trace->normal);
	VectorSubtract(vEnd, vStationary, vDelta);

	if ( fRadiusSqrd >= VectorLengthSquared(vDelta) )
	{
		trace->allsolid = 1;
	}

	return 0;
}

int CM_TraceCylinderThroughCylinder(traceWork_t *tw, const float *vStationary, float fStationaryHalfHeight, float radius, trace_t *trace)
{
	float dr;
	float fA;
	vec3_t vDelta;
	vec3_t vNormal;
	float fHitHeight, fC;
	float fEpsilon, fDeltaLen;
	float sqd2;
	float fB;
	float fTotalHeight;
	float fEntry;
	float fDiscriminant;

	VectorSubtract(tw->extents.start, vStationary, vDelta);
	sqd2 = vDelta[1] * vDelta[1] + vDelta[0] * vDelta[0];

	dr = tw->radius + radius;
	fC = sqd2 - dr * dr;

	if ( fC > 0.0 )
	{
		fB = vDelta[0] * tw->delta[0] + vDelta[1] * tw->delta[1];
		if ( fB >= 0.0 )
		{
			return 1;
		}

		fA = tw->delta[1] * tw->delta[1] + tw->delta[0] * tw->delta[0];

		fDiscriminant = fB * fB - fC * fA;
		if(fDiscriminant < 0.0)
		{
			return 1;
		}

		vDelta[2] = 0.0;
		fDeltaLen = Vec3NormalizeTo(vDelta, vNormal);
		fEpsilon = (fDeltaLen * 0.125) / fB;
		fEntry = (-fB - sqrt(fDiscriminant)) / fA + fEpsilon;
		if ( trace->fraction <= fEntry )
		{
			return 1;
		}

		fTotalHeight = tw->size[2] - tw->radius + fStationaryHalfHeight;
		fHitHeight = (fEntry - fEpsilon) * tw->delta[2] + tw->extents.start[2] - vStationary[2];

		if ( fTotalHeight < fabs(fHitHeight) )
		{
			return 1;
		}
		trace->fraction = I_fmax(fEntry, 0.0);

		VectorCopy(vNormal, trace->normal);
		trace->contents = tw->threadInfo.box_brush->contents;
		return 0;
	}

	fTotalHeight = tw->size[2] - tw->radius + fStationaryHalfHeight;

	if ( fTotalHeight < fabs(vDelta[2]) )
	{
		return 1;
	}

	trace->fraction = 0.0;
	trace->startsolid = 1;
	vDelta[2] = 0.0;
	Vec3NormalizeTo(vDelta, trace->normal);
	trace->contents = tw->threadInfo.box_brush->contents;

	if ( fTotalHeight >= fabs(tw->extents.end[2] - vStationary[2]) )
	{
		trace->allsolid = 1;
	}

	return 0;
}

void CM_TraceCapsuleThroughCapsule(traceWork_t *tw, trace_t *trace)
{
	vec3_t top, bottom, starttop, startbottom, endtop, endbottom, offset;
	int i;
	float halfwidth;
	float halfheight;
	float radius;
	float offs;

	// test trace bounds vs. capsule bounds
	if ( tw->bounds[0][0] > tw->threadInfo.box_model->maxs[0] + RADIUS_EPSILON
	        || tw->bounds[0][1] > tw->threadInfo.box_model->maxs[1] + RADIUS_EPSILON
	        || tw->bounds[0][2] > tw->threadInfo.box_model->maxs[2] + RADIUS_EPSILON
	        || tw->bounds[1][0] < tw->threadInfo.box_model->mins[0] - RADIUS_EPSILON
	        || tw->bounds[1][1] < tw->threadInfo.box_model->mins[1] - RADIUS_EPSILON
	        || tw->bounds[1][2] < tw->threadInfo.box_model->mins[2] - RADIUS_EPSILON
	   )
	{
		return;
	}

	// top origin and bottom origin of each sphere at start and end of trace
	VectorCopy(tw->extents.start, starttop);
	starttop[2] += tw->offsetZ;

	VectorCopy(tw->extents.start, startbottom);
	startbottom[2] -= tw->offsetZ;

	VectorCopy(tw->extents.end, endtop);
	endtop[2] += tw->offsetZ;

	VectorCopy(tw->extents.end, endbottom);
	endbottom[2] -= tw->offsetZ;

	for ( i = 0; i < 3; ++i )
	{
		offset[i] = (tw->threadInfo.box_model->mins[i] + tw->threadInfo.box_model->maxs[i]) * 0.5;
	}
	halfwidth = tw->threadInfo.box_model->maxs[0] - offset[0];
	halfheight = tw->threadInfo.box_model->maxs[2] - offset[2];

	radius = ( halfwidth > halfheight ) ? halfheight : halfwidth;
	offs = halfheight - radius;

	VectorCopy( offset, top );
	top[2] += offs;
	VectorCopy( offset, bottom );
	bottom[2] -= offs;

	if ( top[2] >= startbottom[2] && bottom[2] > starttop[2])
	{
		if ( !CM_TraceSphereThroughSphere(tw, starttop, endtop, bottom, radius, trace) || tw->delta[2] <= 0.0 )
			return;
	}
	else if (top[2] < startbottom[2] && (!CM_TraceSphereThroughSphere(tw, startbottom, endbottom, top, radius, trace) || tw->delta[2] >= 0.0 ))
	{
		return;
	}

	if ( CM_TraceCylinderThroughCylinder(tw, offset, offs, radius, trace) )
	{
		if ( top[2] >= endbottom[2] )
		{
			if ( bottom[2] > endtop[2] && starttop[2] >= bottom[2] )
			{
				CM_TraceSphereThroughSphere(tw, starttop, endtop, bottom, radius, trace);
			}
		}
		else if ( startbottom[2] <= top[2] )
		{
			CM_TraceSphereThroughSphere(tw, startbottom, endbottom, top, radius, trace);
		}
	}
}

qboolean CM_TraceBoxInternal(TraceExtents *extents, const float *testvec, float s, float *fraction)
{
	int i;
	float d, o;

	for(i = 0; i < 3; ++i)
	{
		d = (extents->start[i] - testvec[i]) * s;
		o = (extents->end[i] - testvec[i]) * s;
		if ( d <= 0.0 )
		{
			if ( o > 0.0 )
			{
				d = (d * extents->invDelta[i]) * s;
				if ( d <= 0.0 )
				{
					return 1;
				}
				if ( d - *fraction < 0.0 )
				{
					*fraction = d;
				}
			}
		}
		else if ( o > 0.0 || ((float)(d * extents->invDelta[i]) * s) >= *fraction )
		{
			return 1;
		}
	}

	return 0;
}

qboolean QDECL CM_TraceBox(TraceExtents *extents, const float *mins, const float *maxs, float fraction)
{
	float lfraction = fraction;

	if(CM_TraceBoxInternal(extents, mins, -1.0, &lfraction) == qtrue)
	{
		return qtrue;
	}
	if(CM_TraceBoxInternal(extents, maxs, 1.0, &lfraction) == qtrue)
	{
		return qtrue;
	}

	return qfalse;
}

void CM_TraceCapsuleThroughTriangle(traceWork_t *tw, CollisionTriangle_s *tri, float offsetZ, trace_t *trace)
{
  long double v4; // fst7
  long double v5; // fst7
  float v6; // [esp+0h] [ebp-E8h]
  float v7; // [esp+0h] [ebp-E8h]
  float v8; // [esp+4h] [ebp-E4h]
  int v9; // [esp+14h] [ebp-D4h]
  int v10; // [esp+18h] [ebp-D0h]
  float v11; // [esp+1Ch] [ebp-CCh]
  float v12; // [esp+20h] [ebp-C8h]
  float v13; // [esp+24h] [ebp-C4h]
  float v14; // [esp+28h] [ebp-C0h]
  float v15; // [esp+2Ch] [ebp-BCh]
  vec3_t v16; // [esp+30h] [ebp-B8h] BYREF
  vec3_t v17; // [esp+40h] [ebp-A8h] BYREF
  float *verts; // [esp+50h] [ebp-98h]
  int vertId; // [esp+54h] [ebp-94h]
  CollisionEdge_s *collEdge; // [esp+58h] [ebp-90h]
  int edgeIndex; // [esp+5Ch] [ebp-8Ch]
  float v22; // [esp+60h] [ebp-88h]
  float v23; // [esp+64h] [ebp-84h]
  float len; // [esp+68h] [ebp-80h]
  float v25; // [esp+6Ch] [ebp-7Ch]
  float v26; // [esp+70h] [ebp-78h] BYREF
  float v27; // [esp+74h] [ebp-74h]
  float v28; // [esp+78h] [ebp-70h]
  float v29; // [esp+80h] [ebp-68h] BYREF
  float v30; // [esp+84h] [ebp-64h]
  vec3_t v31; // [esp+90h] [ebp-58h] BYREF
  float v32; // [esp+A0h] [ebp-48h]
  int v33; // [esp+A4h] [ebp-44h]
  float v34; // [esp+A8h] [ebp-40h]
  float v35; // [esp+ACh] [ebp-3Ch]
  float v36; // [esp+B0h] [ebp-38h] BYREF
  float v37; // [esp+B4h] [ebp-34h]
  float v38; // [esp+B8h] [ebp-30h]
  float frac; // [esp+C8h] [ebp-20h]
  float v40; // [esp+CCh] [ebp-1Ch]
  float v41; // [esp+D0h] [ebp-18h]
  float v42; // [esp+D4h] [ebp-14h]
  float v43; // [esp+D8h] [ebp-10h]
  int i; // [esp+DCh] [ebp-Ch]

  VectorCopy(tw->extents.end, v16);
  v16[2] = v16[2] - offsetZ;
  v42 = tw->radius + 0.125;
  v40 = DotProduct(v16, tri->plane) - tri->plane[3];
  if ( v40 < (long double)v42 )
  {
    VectorCopy(tw->extents.start, v17);
    v17[2] = v17[2] - offsetZ;
    v41 = DotProduct(v17, tri->plane) - tri->plane[3];
    v11 = v41 - v40;
    if ( v11 > 0.000099999997 )
    {
      v43 = -v42;
      if ( -v42 < (long double)v41 )
      {
        if ( v41 - v42 > 0.0 )
        {
          frac = (v41 - v42) / v11;
          if ( frac > (long double)trace->fraction )
            return;
          VectorMA(v17, frac, tw->delta, &v36);
        }
        else
        {
          frac = 0.0;
          VectorCopy(v17, &v36);
        }
        v35 = DotProduct(&v36, tri->svec) - tri->svec[3];
        v34 = DotProduct(&v36, tri->tvec) - tri->tvec[3];
        v33 = v35 + v34 > 1.0;
        v10 = v33;
        if ( v35 < 0.0 )
          v10 = v33 | 2;
        v33 = v10;
        v9 = v10;
        if ( v34 < 0.0 )
          v9 = v10 | 4;
        v33 = v9;
        if ( v9 )
        {
          for ( i = 0; i <= 2; ++i )
          {
            if ( ((v33 >> i) & 1) != 0 )
            {
              edgeIndex = tri->edges[i];
              if ( edgeIndex >= 0 && tw->threadInfo.edges[edgeIndex] != tw->threadInfo.checkcount )
              {
                tw->threadInfo.edges[edgeIndex] = tw->threadInfo.checkcount;
                collEdge = &cm.edges[edgeIndex];
                VectorSubtract(v17, collEdge->discEdgeAxis, v31);
                v26 = DotProduct(v31, collEdge->midpoint);
                v27 = DotProduct(v31, collEdge->start_v);
                v29 = DotProduct(tw->delta, collEdge->midpoint);
                v30 = DotProduct(tw->delta, collEdge->start_v);
                v23 = Dot2Product(&v29, &v26);
                if ( v23 < 0.0 )
                {
                  v32 = Dot2Product(&v26, &v26);
                  v22 = v32 - v42 * v42;
                  if ( v22 > 0.0 )
                  {
                    len = Vec2Multiply(&v29);
                    v25 = v23 * v23 - len * v22;
                    if ( v25 > 0.0 )
                    {
                      v4 = sqrt(v25);
                      frac = (-v4 - v23) / len;
                      if ( trace->fraction > (long double)frac )
                      {
                        VectorMA(v31, frac, tw->delta, v31);
                        v38 = DotProduct(v31, collEdge->discNormalAxis);
                        v7 = v38 - 0.5;
                        if ( fabs(v7) <= 0.5 )
                        {
                          v36 = (frac * v29 + v26) / v42;
                          v37 = (frac * v30 + v27) / v42;
                          VectorScale(collEdge->midpoint, v36, trace->normal);
                          VectorMA(trace->normal, v37, collEdge->start_v, trace->normal);
                          if ( tri->plane[2] >= 0.69999999
                            && trace->normal[2] >= 0.0
                            && trace->normal[2] < 0.69999999
                            && v17[2] > (long double)v16[2] )
                          {
                            VectorCopy(tri->plane, trace->normal);
                          }
                          trace->fraction = frac;
                        }
                      }
                    }
                  }
                  else
                  {
                    v28 = DotProduct(v31, collEdge->discNormalAxis);
                    v6 = v28 - 0.5;
                    if ( fabs(v6) <= 0.5 )
                    {
                      VectorScale(collEdge->midpoint, v26, trace->normal);
                      VectorMA(trace->normal, v27, collEdge->start_v, trace->normal);
                      Vec3Normalize(trace->normal);
                      if ( tri->plane[2] >= 0.69999999
                        && trace->normal[2] >= 0.0
                        && trace->normal[2] < 0.69999999
                        && v17[2] > (long double)v16[2] )
                      {
                        VectorCopy(tri->plane, trace->normal);
                      }
                      trace->fraction = 0.0;
                      if ( tw->radius * tw->radius > (long double)v32 )
                        trace->startsolid = 1;
                      return;
                    }
                  }
                }
              }
            }
            else
            {
              vertId = tri->verts[i];
              if ( vertId >= 0 && tw->threadInfo.verts[vertId] != tw->threadInfo.checkcount )
              {
                tw->threadInfo.verts[vertId] = tw->threadInfo.checkcount;
                verts = cm.verts[vertId];
                VectorSubtract(v17, verts, v31);
                v23 = DotProduct(tw->delta, v31);
                if ( v23 < 0.0 )
                {
                  v32 = DotProduct(v31, v31);
                  v22 = v32 - v42 * v42;
                  if ( v22 <= 0.0 )
                  {
                    frac = 1.0 / sqrt(v32);
                    VectorScale(v31, frac, trace->normal);
                    if ( tri->plane[2] >= 0.69999999
                      && trace->normal[2] >= 0.0
                      && trace->normal[2] < 0.69999999
                      && v17[2] > (long double)v16[2] )
                    {
                      VectorCopy(tri->plane, trace->normal);
                    }
                    trace->fraction = 0.0;
                    if ( tw->radius * tw->radius > (long double)v32 )
                      trace->startsolid = 1;
                    return;
                  }
                  len = tw->deltaLenSq;
                  v25 = v23 * v23 - len * v22;
                  if ( v25 >= 0.0 )
                  {
                    v5 = sqrt(v25);
                    frac = (-v5 - v23) / len;
                    if ( trace->fraction > (long double)frac )
                    {
                      VectorMA(v31, frac, tw->delta, trace->normal);
                      v8 = 1.0 / v42;
                      VectorScale(trace->normal, v8, trace->normal);
                      if ( tri->plane[2] >= 0.69999999
                        && trace->normal[2] >= 0.0
                        && trace->normal[2] < 0.69999999
                        && v17[2] > (long double)v16[2] )
                      {
                        VectorCopy(tri->plane, trace->normal);
                      }
                      trace->fraction = frac;
                    }
                  }
                }
              }
            }
          }
        }
        else
        {
          VectorCopy(tri->plane, trace->normal);
          trace->fraction = frac;
          if ( tw->radius > (long double)v41 )
            trace->startsolid = 1;
        }
      }
      else
      {
        v14 = offsetZ + offsetZ;
        v15 = v14 * tri->plane[2] + v41;
        if ( v43 < (long double)v15 )
        {
          frac = (v43 - v41) / tri->plane[2];
          v13 = DotProduct(v17, tri->svec) - tri->svec[3];
          v12 = DotProduct(v17, tri->tvec) - tri->tvec[3];
          v35 = frac * tri->svec[2] + v13;
          if ( v35 >= 0.0 )
          {
            v34 = frac * tri->tvec[2] + v12;
            if ( v34 >= 0.0 && v35 + v34 <= 1.0 )
              goto LABEL_8;
          }
          frac = v42 <= (long double)v15 ? (v42 - v41) / tri->plane[2] : offsetZ + offsetZ;
          v35 = frac * tri->svec[2] + v13;
          if ( v35 >= 0.0 )
          {
            v34 = frac * tri->tvec[2] + v12;
            if ( v34 >= 0.0 && v35 + v34 <= 1.0 )
            {
LABEL_8:
              VectorCopy(tri->plane, trace->normal);
              trace->fraction = 0.0;
              trace->startsolid = 1;
            }
          }
        }
      }
    }
  }
}