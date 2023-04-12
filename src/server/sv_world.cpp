#include "../qcommon/qcommon.h"

vec3_t actorLocationalMins = { -64.0, -64.0, -32.0 };
vec3_t actorLocationalMaxs = { 64.0, 64.0, 72.0 };

clipHandle_t SV_ClipHandleForEntity(gentity_s *touch)
{
	if ( touch->r.bmodel )
		return touch->s.index;
	else
		return CM_TempBoxModel(touch->r.mins, touch->r.maxs, touch->r.contents);
}

void SV_LinkEntity(gentity_s *ent)
{
	svEntity_t *writeEnt;
	int j;
	int i;
	float max;
	float max2d;
	clipHandle_t handle;
	vec3_t maxs;
	vec3_t mins;
	DObj_s *obj;
	svEntity_t *svEnt;
	float *angles;
	float *currentOrigin;
	int lastLeaf;
	int c;
	int b;
	int a;
	int num_leafs;
	int cluster;
	int leafs[128];

	svEnt = SV_SvEntityForGentity(ent);

	if ( ent->r.bmodel )
	{
		ent->s.solid = 0xFFFFFF;
	}
	else if ( (ent->r.contents & 0x2000001) != 0 )
	{
		a = (int)ent->r.maxs[0];

		if ( a <= 0 )
			a = 1;

		if ( a > 255 )
			a = 255;

		b = (int)(1.0 - ent->r.mins[2]);

		if ( b <= 0 )
			b = 1;

		if ( b > 255 )
			b = 255;

		c = (int)(ent->r.maxs[2] + 32.0);

		if ( c <= 0 )
			c = 1;

		if ( c > 255 )
			c = 255;

		ent->s.solid = a | (c << 16) | (b << 8);
	}
	else
	{
		ent->s.solid = 0;
	}

	angles = ent->r.currentAngles;
	currentOrigin = ent->r.currentOrigin;
	SnapAngles(ent->r.currentAngles);

	if ( !ent->r.bmodel || *angles == 0.0 && angles[1] == 0.0 && angles[2] == 0.0 )
	{
		VectorAdd(currentOrigin, ent->r.mins, ent->r.absmin);
		VectorAdd(currentOrigin, ent->r.maxs, ent->r.absmax);
	}
	else if ( *angles == 0.0 && angles[2] == 0.0 )
	{
		max2d = RadiusFromBounds2D(ent->r.mins, ent->r.maxs);

		for ( i = 0; i <= 1; ++i )
		{
			ent->r.absmin[i] = currentOrigin[i] - max2d;
			ent->r.absmax[i] = currentOrigin[i] + max2d;
		}

		ent->r.absmin[2] = currentOrigin[2] + ent->r.mins[2];
		ent->r.absmax[2] = currentOrigin[2] + ent->r.maxs[2];
	}
	else
	{
		max = RadiusFromBounds(ent->r.mins, ent->r.maxs);

		for ( j = 0; j <= 2; ++j )
		{
			ent->r.absmin[j] = currentOrigin[j] - max;
			ent->r.absmax[j] = currentOrigin[j] + max;
		}
	}

	ent->r.absmin[0] = ent->r.absmin[0] - 1.0;
	ent->r.absmin[1] = ent->r.absmin[1] - 1.0;
	ent->r.absmin[2] = ent->r.absmin[2] - 1.0;
	ent->r.absmax[0] = ent->r.absmax[0] + 1.0;
	ent->r.absmax[1] = ent->r.absmax[1] + 1.0;
	ent->r.absmax[2] = ent->r.absmax[2] + 1.0;
	svEnt->numClusters = 0;
	svEnt->lastCluster = 0;

	if ( (ent->r.svFlags & 0x19) == 0 )
	{
		num_leafs = CM_BoxLeafnums(ent->r.absmin, ent->r.absmax, leafs, 128, &lastLeaf);

		if ( !num_leafs )
		{
unlink:
			CM_UnlinkEntity(svEnt);
			return;
		}

		for ( a = 0; a < num_leafs; ++a )
		{
			cluster = CM_LeafCluster(leafs[a]);

			if ( cluster != -1 )
			{
				writeEnt = svEnt;
				svEnt->clusternums[svEnt->numClusters] = cluster;
				++writeEnt->numClusters;

				if ( svEnt->numClusters == 16 )
					break;
			}
		}

		if ( a != num_leafs )
			svEnt->lastCluster = CM_LeafCluster(lastLeaf);
	}

	ent->r.linked = 1;

	if ( !ent->r.contents )
		goto unlink;

	handle = SV_ClipHandleForEntity(ent);
	obj = Com_GetServerDObj(ent->s.number);

	if ( obj && (ent->r.svFlags & 6) != 0 )
	{
		if ( (ent->r.svFlags & 2) != 0 )
		{
			Vector2Add(currentOrigin, actorLocationalMins, mins);
			Vector2Add(currentOrigin, actorLocationalMaxs, maxs);
		}
		else
		{
			DObjGetBounds(obj, mins, maxs);
			Vector2Add(currentOrigin, mins, mins);
			Vector2Add(currentOrigin, maxs, maxs);
		}
		CM_LinkEntity(svEnt, mins, maxs, handle);
	}
	else
	{
		CM_LinkEntity(svEnt, ent->r.absmin, ent->r.absmax, handle);
	}
}

void SV_UnlinkEntity(gentity_s *ent)
{
	svEntity_t *svEnt;

	svEnt = SV_SvEntityForGentity(ent);
	ent->r.linked = 0;
	CM_UnlinkEntity(svEnt);
}

void SV_ClipMoveToEntity(moveclip_t *clip, svEntity_t *entity, trace_t *trace)
{
	vec3_t maxs;
	vec3_t mins;
	float fraction;
	int num;
	float *angles;
	unsigned int model;
	gentity_s *touch;

	num = entity - sv.svEntities;
	touch = SV_GentityNum(num);
	if ( (clip->contentmask & touch->r.contents) != 0
	        && (clip->passEntityNum == 1023
	            || num != clip->passEntityNum
	            && touch->r.ownerNum != clip->passEntityNum
	            && touch->r.ownerNum != clip->passOwnerNum) )
	{
		VectorAdd(touch->r.absmin, clip->mins, mins);
		VectorAdd(touch->r.absmax, clip->maxs, maxs);
		if ( !CM_TraceBox(&clip->extents, mins, maxs, trace->fraction) )
		{
			model = SV_ClipHandleForEntity(touch);
			angles = touch->r.currentAngles;
			if ( !touch->r.bmodel )
				angles = vec3_origin;
			fraction = trace->fraction;
			CM_TransformedBoxTrace(
			    trace,
			    clip->extents.start,
			    clip->extents.end,
			    clip->mins,
			    clip->maxs,
			    model,
			    clip->contentmask,
			    touch->r.currentOrigin,
			    angles);
			if ( trace->fraction < fraction )
				trace->hitId = touch->s.number;
		}
	}
}

void SV_PointTraceToEntity(pointtrace_t *clip, svEntity_t *check, trace_t *trace)
{
	vec3_t entAxis[4];
	float fraction;
	vec3_t absmax;
	vec3_t absmin;
	DObjTrace_s objTrace;
	vec3_t maxs;
	vec3_t mins;
	DObj_s *obj;
	int num;
	float *angles;
	unsigned int model;
	gentity_s *ent;

	num = check - sv.svEntities;
	ent = SV_GentityNum(num);

	if ( (clip->contentmask & ent->r.contents) == 0
	        || clip->passEntNum != 1023
	        && (num == clip->passEntNum || ent->r.ownerNum == clip->passEntNum || ent->r.ownerNum == clip->ignoreEntNum) )
	{
		return;
	}

	if ( clip->bLocational )
	{
		obj = Com_GetServerDObj(ent->s.number);

		if ( obj )
		{
			if ( (ent->r.svFlags & 6) != 0 )
			{
				if ( (ent->r.svFlags & 4) != 0 )
				{
					if ( !DObjHasContents(obj, clip->contentmask) )
						return;

					VectorCopy(ent->r.currentOrigin, entAxis[3]);
					DObjGetBounds(obj, mins, maxs);
					VectorAdd(entAxis[3], mins, mins);
					VectorAdd(entAxis[3], maxs, maxs);
LABEL_14:
					if ( !CM_TraceBox(&clip->extents, mins, maxs, trace->fraction) )
					{
						G_DObjCalcPose(ent);
						AnglesToAxis(ent->r.currentAngles, entAxis);
						MatrixTransposeTransformVector43(clip->extents.start, entAxis, absmin);
						MatrixTransposeTransformVector43(clip->extents.end, entAxis, absmax);
						objTrace.fraction = trace->fraction;

						if ( (ent->r.svFlags & 4) != 0 )
							DObjGeomTraceline(obj, absmin, absmax, clip->contentmask, &objTrace);
						else
							DObjTraceline(obj, absmin, absmax, clip->priorityMap, &objTrace);

						if ( objTrace.fraction < trace->fraction )
						{
							trace->fraction = objTrace.fraction;
							trace->surfaceFlags = objTrace.sflags;
							trace->modelIndex = objTrace.modelIndex;
							trace->partName = objTrace.partName;
							MatrixTransformVector(objTrace.normal, entAxis, trace->normal);
LABEL_26:
							trace->hitId = ent->s.number;
							trace->contents = ent->r.contents;
							trace->material = 0;
							return;
						}
					}
					return;
				}

				if ( clip->priorityMap )
				{
					VectorCopy(ent->r.currentOrigin, entAxis[3]);
					VectorAdd(entAxis[3], actorLocationalMins, mins);
					VectorAdd(entAxis[3], actorLocationalMaxs, maxs);
					goto LABEL_14;
				}
			}
		}
	}

	if ( (clip->contentmask & check->linkcontents) != 0
	        && !CM_TraceBox(&clip->extents, ent->r.absmin, ent->r.absmax, trace->fraction) )
	{
		model = SV_ClipHandleForEntity(ent);
		angles = ent->r.currentAngles;

		if ( !ent->r.bmodel )
			angles = vec3_origin;

		fraction = trace->fraction;

		CM_TransformedBoxTrace(
		    trace,
		    clip->extents.start,
		    clip->extents.end,
		    vec3_origin,
		    vec3_origin,
		    model,
		    clip->contentmask,
		    ent->r.currentOrigin,
		    angles);

		if ( trace->fraction < fraction )
		{
			trace->surfaceFlags = 0;
			trace->modelIndex = 0;
			trace->partName = 0;
			goto LABEL_26;
		}
	}
}

int SV_PointSightTraceToEntity(sightpointtrace_t *clip, svEntity_t *check)
{
	vec3_t entAxis[4];
	float absmax[4];
	float absmin[4];
	DObjTrace_s objTrace;
	float maxs[4];
	float mins[7];
	DObj_s *obj;
	TraceExtents extents;
	int num;
	float *angles;
	unsigned int model;
	gentity_s *ent;

	num = check - sv.svEntities;
	ent = SV_GentityNum(num);

	if ( (clip->contentmask & ent->r.contents) != 0 )
	{
		if ( clip->passEntityNum[0] != 1023 )
		{
			if ( num == clip->passEntityNum[0] )
				return 0;

			if ( ent->r.ownerNum == clip->passEntityNum[0] )
				return 0;
		}

		if ( clip->passEntityNum[1] != 1023 )
		{
			if ( num == clip->passEntityNum[1] )
				return 0;

			if ( ent->r.ownerNum == clip->passEntityNum[1] )
				return 0;
		}

		if ( clip->locational && (obj = Com_GetServerDObj(ent->s.number)) != 0 && (ent->r.svFlags & 4) != 0 )
		{
			if ( !DObjHasContents(obj, clip->contentmask) )
				return 0;

			VectorCopy(ent->r.currentOrigin, entAxis[3]);
			DObjGetBounds(obj, mins, maxs);
			VectorAdd(entAxis[3], mins, mins);
			VectorAdd(entAxis[3], maxs, maxs);
			VectorCopy(clip->start, extents.start);
			VectorCopy(clip->end, extents.end);
			CM_CalcTraceExtents(&extents);

			if ( CM_TraceBox(&extents, mins, maxs, 1.0) )
				return 0;

			G_DObjCalcPose(ent);
			AnglesToAxis(ent->r.currentAngles, entAxis);
			MatrixTransposeTransformVector43(extents.start, entAxis, absmin);
			MatrixTransposeTransformVector43(extents.end, entAxis, absmax);
			objTrace.fraction = 1.0;
			DObjGeomTraceline(obj, absmin, absmax, clip->contentmask, &objTrace);

			if ( objTrace.fraction < 1.0 )
				return -1;
		}
		else
		{
			model = SV_ClipHandleForEntity(ent);
			angles = ent->r.currentAngles;

			if ( !ent->r.bmodel )
				angles = vec3_origin;

			if ( CM_TransformedBoxSightTrace(
			            0,
			            clip->start,
			            clip->end,
			            vec3_origin,
			            vec3_origin,
			            model,
			            clip->contentmask,
			            ent->r.currentOrigin,
			            angles) )
			{
				return -1;
			}
		}

		return 0;
	}

	return 0;
}

int SV_ClipSightToEntity(sightclip_t *clip, svEntity_t *check)
{
	int num;
	float *angles;
	clipHandle_t model;
	gentity_s *touch;

	num = check - sv.svEntities;
	touch = SV_GentityNum(num);

	if ( (clip->contentmask & touch->r.contents) == 0 )
		return 0;

	if ( clip->passEntityNum[0] != 1023 )
	{
		if ( num == clip->passEntityNum[0] )
			return 0;

		if ( touch->r.ownerNum == clip->passEntityNum[0] )
			return 0;
	}

	if ( clip->passEntityNum[1] != 1023 )
	{
		if ( num == clip->passEntityNum[1] )
			return 0;

		if ( touch->r.ownerNum == clip->passEntityNum[1] )
			return 0;
	}

	model = SV_ClipHandleForEntity(touch);
	angles = touch->r.currentAngles;

	if ( !touch->r.bmodel )
		angles = vec3_origin;

	if ( CM_TransformedBoxSightTrace(
	            0,
	            clip->start,
	            clip->end,
	            clip->mins,
	            clip->maxs,
	            model,
	            clip->contentmask,
	            touch->r.currentOrigin,
	            angles) )
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

void SV_Trace(trace_t *results, const float *start, const float *mins, const float *maxs, const float *end, int passEntityNum, int contentmask, int locational, char *priorityMap, int staticmodels)
{
	unsigned short hitId;
	moveclip_t clip;
	pointtrace_t pt;
	vec3_t temp;

	CM_BoxTrace(results, start, end, mins, maxs, 0, contentmask);

	if ( results->fraction == 1.0 )
		hitId = 1023;
	else
		hitId = 1022;

	results->hitId = hitId;

	if ( results->fraction != 0.0 )
	{
		if ( !staticmodels || (CM_PointTraceStaticModels(results, start, end, contentmask), results->fraction != 0.0) )
		{
			if ( maxs[0] - mins[0] + maxs[1] - mins[1] + maxs[2] - mins[2] == 0.0 )
			{
				pt.contentmask = contentmask;
				VectorCopy(start, pt.extents.start);
				VectorCopy(end, pt.extents.end);
				CM_CalcTraceExtents(&pt.extents);
				pt.passEntNum = passEntityNum;
				pt.bLocational = locational;
				pt.priorityMap = priorityMap;

				if ( passEntityNum == 1023 )
				{
					pt.ignoreEntNum = -1;
				}
				else
				{
					pt.ignoreEntNum = SV_GentityNum(passEntityNum)->r.ownerNum;

					if ( pt.ignoreEntNum == 1023 )
						pt.ignoreEntNum = -1;
				}

				CM_PointTraceToEntities(&pt, results);
			}
			else
			{
				clip.contentmask = contentmask;
				clip.passEntityNum = passEntityNum;

				if ( passEntityNum == 1023 )
				{
					clip.passOwnerNum = -1;
				}
				else
				{
					clip.passOwnerNum = SV_GentityNum(passEntityNum)->r.ownerNum;

					if ( clip.passOwnerNum == 1023 )
						clip.passOwnerNum = -1;
				}

				VectorSubtract(maxs, mins, clip.outerSize);
				VectorScale(clip.outerSize, 0.5, clip.outerSize);
				VectorCopy(clip.outerSize, clip.maxs);
				VectorScale(clip.outerSize, -1.0, clip.mins);

				clip.outerSize[0] = clip.outerSize[0] + 1.0;
				clip.outerSize[1] = clip.outerSize[1] + 1.0;
				clip.outerSize[2] = clip.outerSize[2] + 1.0;

				VectorAdd(maxs, mins, temp);
				VectorScale(temp, 0.5, temp);
				VectorAdd(start, temp, clip.extents.start);
				VectorAdd(end, temp, clip.extents.end);

				CM_CalcTraceExtents(&clip.extents);
				CM_ClipMoveToEntities(&clip, results);
			}
		}
	}
}

int SV_TracePassed(const float *start, const float *mins, const float *maxs, const float *end, int passEntityNum0, int passEntityNum1, int contentmask, int locational, int staticmodels)
{
	sightclip_t clip;
	sightpointtrace_t spt;
	vec3_t temp;

	if ( CM_BoxSightTrace(0, start, end, mins, maxs, 0, contentmask) )
		return 0;

	if ( !staticmodels || CM_PointTraceStaticModelsComplete(start, end, contentmask) )
	{
		if ( maxs[0] - mins[0] + maxs[1] - mins[1] + maxs[2] - mins[2] == 0.0 )
		{
			spt.contentmask = contentmask;

			VectorCopy(start, spt.start);
			VectorCopy(end, spt.end);

			spt.passEntityNum[0] = passEntityNum0;
			spt.passEntityNum[1] = passEntityNum1;
			spt.locational = locational;

			if ( CM_PointSightTraceToEntities(&spt) )
				return 0;
		}
		else
		{
			clip.contentmask = contentmask;
			clip.passEntityNum[0] = passEntityNum0;
			clip.passEntityNum[1] = passEntityNum1;

			VectorSubtract(maxs, mins, clip.outerSize);
			VectorScale(clip.outerSize, 0.5, clip.outerSize);
			VectorCopy(clip.outerSize, clip.maxs);
			VectorScale(clip.outerSize, -1.0, clip.mins);

			clip.outerSize[0] = clip.outerSize[0] + 1.0;
			clip.outerSize[1] = clip.outerSize[1] + 1.0;
			clip.outerSize[2] = clip.outerSize[2] + 1.0;

			VectorAdd(maxs, mins, temp);
			VectorScale(temp, 0.5, temp);
			VectorAdd(start, temp, clip.start);
			VectorAdd(end, temp, clip.end);

			if ( CM_ClipSightTraceToEntities(&clip) )
				return 0;
		}

		return 1;
	}

	return 0;
}

void SV_SightTrace(int *hitNum,const float *start, const float *mins, const float *maxs, const float *end, int passEntityNum0, int passEntityNum1, int contentmask)
{
	int entNum;
	sightclip_t clip;
	sightpointtrace_t spt;
	vec3_t delta;

	*hitNum = CM_BoxSightTrace(*hitNum, start, end, mins, maxs, 0, contentmask);

	if ( !*hitNum )
	{
		if ( maxs[0] - mins[0] + maxs[1] - mins[1] + maxs[2] - mins[2] == 0.0 )
		{
			spt.contentmask = contentmask;

			VectorCopy(start, spt.start);
			VectorCopy(end, spt.end);

			spt.passEntityNum[0] = passEntityNum0;
			spt.passEntityNum[1] = passEntityNum1;
			spt.locational = 0;

			entNum = CM_PointSightTraceToEntities(&spt);
		}
		else
		{
			clip.contentmask = contentmask;
			clip.passEntityNum[0] = passEntityNum0;
			clip.passEntityNum[1] = passEntityNum1;

			VectorSubtract(maxs, mins, clip.outerSize);
			VectorScale(clip.outerSize, 0.5, clip.outerSize);
			VectorCopy(clip.outerSize, clip.maxs);
			VectorScale(clip.outerSize, -1.0, clip.mins);

			clip.outerSize[0] = clip.outerSize[0] + 1.0;
			clip.outerSize[1] = clip.outerSize[1] + 1.0;
			clip.outerSize[2] = clip.outerSize[2] + 1.0;

			VectorAdd(maxs, mins, delta);
			VectorScale(delta, 0.5, delta);
			VectorAdd(start, delta, clip.start);
			VectorAdd(end, delta, clip.end);

			entNum = CM_ClipSightTraceToEntities(&clip);
		}

		*hitNum = entNum;
	}
}

int SV_PointContents(const vec3_t p, int passEntityNum, int contentmask)
{
	clipHandle_t handle;
	int contents;
	int entities;
	int i;
	gentity_s *ent;
	int entityList[1024];

	contents = CM_PointContents(p, 0);
	entities = CM_AreaEntities(p, p, entityList, 1024, contentmask);

	for ( i = 0; i < entities; ++i )
	{
		if ( entityList[i] != passEntityNum )
		{
			ent = SV_GentityNum(entityList[i]);
			handle = SV_ClipHandleForEntity(ent);
			contents |= CM_TransformedPointContents(p, handle, ent->r.currentOrigin, ent->r.currentAngles);
		}
	}

	return contents & contentmask;
}

signed int SV_SightTraceToEntity(const float *start, const float *mins, const float *maxs, const float *end, int entityNum, int contentmask)
{
	struct gentity_s *touch;
	signed int i;
	unsigned int clipHandle;
	const float *angles;
	vec3_t bmin;
	vec3_t bmax;

	touch = SV_GentityNum(entityNum);

	if ( !(contentmask & touch->r.contents) )
	{
		return 0;
	}

	for(i = 0; i < 3; ++i)
	{
		if ( end[i] <= start[i] )
		{
			bmax[i] = (end[i] + mins[i]) - 1.0;
			bmin[i] = (start[i] + maxs[i]) + 1.0;
		}
		else
		{
			bmax[i] = (start[i] + mins[i]) - 1.0;
			bmin[i] = (end[i] + maxs[i]) + 1.0;
		}
	}

	if ( touch->r.absmin[0] > bmin[0] || touch->r.absmin[1] > bmin[1] || touch->r.absmin[2] > bmin[2] || bmax[0] > touch->r.absmax[0] || bmax[1] > touch->r.absmax[1] || bmax[2] > touch->r.absmax[2] )
	{
		return 0;
	}

	clipHandle = SV_ClipHandleForEntity(touch);
	angles = touch->r.currentAngles;

	if ( !touch->r.bmodel )
	{
		angles = vec3_origin;
	}

	if ( CM_TransformedBoxSightTrace(0, start, end, mins, maxs, clipHandle, contentmask, touch->r.currentOrigin, angles) )
	{
		return -1;
	}

	return 0;
}