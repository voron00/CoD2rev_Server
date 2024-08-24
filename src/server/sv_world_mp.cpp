#include "../qcommon/qcommon.h"

vec3_t actorLocationalMins = { -64.0, -64.0, -32.0 };
vec3_t actorLocationalMaxs = { 64.0, 64.0, 72.0 };

/*
================
SV_ClipHandleForEntity

Returns a headnode that can be used for testing or clipping to a
given entity.  If the entity is a bsp model, the headnode will
be returned, otherwise a custom box tree will be constructed.
================
*/
clipHandle_t SV_ClipHandleForEntity( const gentity_t *ent )
{
	if ( ent->r.bmodel )
	{
		// explicit hulls in the BSP model
		return ent->s.index;
	}

	// create a temp tree from bounding box sizes
	return CM_TempBoxModel(ent->r.mins, ent->r.maxs, ent->r.contents);
}

/*
=============
SV_PointContents
=============
*/
int SV_PointContents( const vec3_t p, int passEntityNum, int contentmask )
{
	int touch[MAX_GENTITIES];
	gentity_t *hit;
	int contents;
	int i, num;
	clipHandle_t model;

	// get base contents from world
	contents = CM_PointContents(p, 0);

	// or in contents from all the other entities
	num = CM_AreaEntities(p, p, touch, MAX_GENTITIES, contentmask);

	for ( i = 0; i < num; ++i )
	{
		if ( touch[i] == passEntityNum )
		{
			continue;
		}
		hit = SV_GentityNum(touch[i]);
		// might intersect, so do an exact clip
		model = SV_ClipHandleForEntity(hit);

		contents |= CM_TransformedPointContents(p, model, hit->r.currentOrigin, hit->r.currentAngles);
	}

	return contents & contentmask;
}

/*
===============
SV_ClipMoveToEntity
===============
*/
void SV_ClipMoveToEntity( moveclip_t *clip, svEntity_t *entity, trace_t *trace )
{
	vec3_t maxs;
	vec3_t mins;
	float oldFraction;
	int num;
	float *angles;
	clipHandle_t model;
	gentity_t *touch;

	num = entity - sv.svEntities;
	touch = SV_GentityNum(num);

	// if it doesn't have any brushes of a type we
	// are looking for, ignore it
	if ( !(clip->contentmask & touch->r.contents) )
	{
		return;
	}

	// see if we should ignore this entity
	if ( clip->passEntityNum != ENTITYNUM_NONE )
	{
		if ( num == clip->passEntityNum )
		{
			return;   // don't clip against the pass entity
		}
		if ( touch->r.ownerNum == clip->passEntityNum )
		{
			return;   // don't clip against own missiles
		}
		if ( touch->r.ownerNum == clip->passOwnerNum )
		{
			return;   // don't clip against other missiles from our owner
		}
	}

	VectorAdd(touch->r.absmin, clip->mins, mins);
	VectorAdd(touch->r.absmax, clip->maxs, maxs);

	if ( CM_TraceBox(&clip->extents, mins, maxs, trace->fraction) )
	{
		return;
	}

	model = SV_ClipHandleForEntity(touch);
	angles = touch->r.currentAngles;

	if ( !touch->r.bmodel )
	{
		angles = vec3_origin; // boxes don't rotate
	}

	oldFraction = trace->fraction;

	CM_TransformedBoxTrace(trace, clip->extents.start, clip->extents.end, clip->mins, clip->maxs, model, clip->contentmask, touch->r.currentOrigin, angles);

	if ( trace->fraction < oldFraction )
	{
		trace->entityNum = touch->s.number;
	}
}

/*
===============
SV_SightTraceToEntity
===============
*/
int SV_SightTraceToEntity( const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int entityNum, int contentmask )
{
	gentity_t *touch;
	int i;
	clipHandle_t model;
	float *origin, *angles;
	vec3_t bmin;
	vec3_t bmax;

	touch = SV_GentityNum(entityNum);

	// if it doesn't have any brushes of a type we
	// are looking for, ignore it
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

	model = SV_ClipHandleForEntity(touch);

	origin = touch->r.currentOrigin;
	angles = touch->r.currentAngles;

	if ( !touch->r.bmodel )
	{
		angles = vec3_origin; // boxes don't rotate
	}

	if ( CM_TransformedBoxSightTrace(0, start, end, mins, maxs, model, contentmask, origin, angles) )
	{
		return -1;
	}

	return 0;
}

/*
===============
SV_ClipSightToEntity
===============
*/
int SV_ClipSightToEntity( sightclip_t *clip, svEntity_t *check )
{
	int num;
	float *angles;
	clipHandle_t model;
	gentity_t *touch;

	num = check - sv.svEntities;
	touch = SV_GentityNum(num);

	// if it doesn't have any brushes of a type we
	// are looking for, ignore it
	if ( !(clip->contentmask & touch->r.contents) )
	{
		return 0;
	}

	// see if we should ignore this entity
	if ( clip->passEntityNum[0] != ENTITYNUM_NONE )
	{
		if ( num == clip->passEntityNum[0] )
		{
			return 0; // don't clip against the pass entity
		}

		if ( touch->r.ownerNum == clip->passEntityNum[0] )
		{
			return 0;  // don't clip against own missiles
		}
	}

	// see if we should ignore this entity
	if ( clip->passEntityNum[1] != ENTITYNUM_NONE )
	{
		if ( num == clip->passEntityNum[1] )
		{
			return 0; // don't clip against the pass entity
		}

		if ( touch->r.ownerNum == clip->passEntityNum[1] )
		{
			return 0; // don't clip against own missiles
		}
	}

	model = SV_ClipHandleForEntity(touch);
	angles = touch->r.currentAngles;

	if ( !touch->r.bmodel )
	{
		angles = vec3_origin; // boxes don't rotate
	}

	if ( !CM_TransformedBoxSightTrace(0, clip->start, clip->end, clip->mins, clip->maxs, model, clip->contentmask, touch->r.currentOrigin, angles) )
	{
		return 0;
	}

	return -1;
}

/*
===============
SV_LinkEntity
===============
*/
void SV_LinkEntity( gentity_t *gEnt )
{
	int leafs[MAX_TOTAL_ENT_LEAFS];
	int cluster;
	int num_leafs;
	int i, j, k;
	int lastLeaf;
	float *origin, *angles;
	svEntity_s *ent;
	DObj *obj;
	vec3_t min, max;
	clipHandle_t clip;

	assert(gEnt->r.inuse);

	ent = SV_SvEntityForGentity( gEnt );

	// encode the size into the entityState_t for client prediction
	if ( gEnt->r.bmodel )
	{
		gEnt->s.solid = SOLID_BMODEL;       // a solid_box will never create this value
	}
	else if ( gEnt->r.contents & ( CONTENTS_SOLID | CONTENTS_BODY ) )
	{
		// assume that x/y are equal and symetric
		i = gEnt->r.maxs[0];
		if ( i < 1 )
		{
			i = 1;
		}
		if ( i > 255 )
		{
			i = 255;
		}

		// z is not symetric
		j = ( 1.0 - gEnt->r.mins[2] );
		if ( j < 1 )
		{
			j = 1;
		}
		if ( j > 255 )
		{
			j = 255;
		}

		// and z maxs can be negative...
		k = ( gEnt->r.maxs[2] + 32 );
		if ( k < 1 )
		{
			k = 1;
		}
		if ( k > 255 )
		{
			k = 255;
		}
		gEnt->s.solid = ( k << 16 ) | ( j << 8 ) | i;
	}
	else
	{
		gEnt->s.solid = 0;
	}

	// get the position
	origin = gEnt->r.currentOrigin;
	angles = gEnt->r.currentAngles;

	assert(!IS_NAN((angles)[0]) && !IS_NAN((angles)[1]) && !IS_NAN((angles)[2]));
	assert(!IS_NAN((origin)[0]) && !IS_NAN((origin)[1]) && !IS_NAN((origin)[2]));

	SnapAngles(angles);

	// set the abs box
	if ( !gEnt->r.bmodel || ( !angles[0] && !angles[1] && !angles[2] ) )
	{
		// normal
		VectorAdd( origin, gEnt->r.mins, gEnt->r.absmin );
		VectorAdd( origin, gEnt->r.maxs, gEnt->r.absmax );
	}
	else if ( ( !angles[0] && !angles[2] ) )
	{
		// expand for rotation
		float max;
		int i;

		max = RadiusFromBounds2D( gEnt->r.mins, gEnt->r.maxs );
		for ( i = 0 ; i < 2 ; i++ )
		{
			gEnt->r.absmin[i] = origin[i] - max;
			gEnt->r.absmax[i] = origin[i] + max;
		}
		gEnt->r.absmin[2] = origin[2] + gEnt->r.mins[2];
		gEnt->r.absmax[2] = origin[2] + gEnt->r.maxs[2];
	}
	else
	{
		// expand for rotation
		float max;
		int i;

		max = RadiusFromBounds( gEnt->r.mins, gEnt->r.maxs );
		for ( i = 0 ; i < 3 ; i++ )
		{
			gEnt->r.absmin[i] = origin[i] - max;
			gEnt->r.absmax[i] = origin[i] + max;
		}
	}

	// because movement is clipped an epsilon away from an actual edge,
	// we must fully check even when bounding boxes don't quite touch
	gEnt->r.absmin[0] -= 1;
	gEnt->r.absmin[1] -= 1;
	gEnt->r.absmin[2] -= 1;
	gEnt->r.absmax[0] += 1;
	gEnt->r.absmax[1] += 1;
	gEnt->r.absmax[2] += 1;

	// link to PVS leafs
	ent->numClusters = 0;
	ent->lastCluster = 0;

	if ( !(gEnt->r.svFlags & 0x19) )
	{
		//get all leafs, including solids
		num_leafs = CM_BoxLeafnums( gEnt->r.absmin, gEnt->r.absmax, leafs, MAX_TOTAL_ENT_LEAFS, &lastLeaf );

		// if none of the leafs were inside the map, the
		// entity is outside the world and can be considered unlinked
		if ( !num_leafs )
		{
			CM_UnlinkEntity(ent);
			return;
		}

		// store as many explicit clusters as we can
		for ( i = 0 ; i < num_leafs ; i++ )
		{
			cluster = CM_LeafCluster( leafs[i] );
			if ( cluster != -1 )
			{
				ent->clusternums[ent->numClusters++] = cluster;
				if ( ent->numClusters == MAX_ENT_CLUSTERS )
				{
					break;
				}
			}
		}

		// store off a last cluster if we need to
		if ( i != num_leafs )
		{
			ent->lastCluster = CM_LeafCluster( lastLeaf );
		}
	}
	gEnt->r.linked = qtrue;

	if ( !gEnt->r.contents )
	{
		CM_UnlinkEntity(ent);
		return;
	}

	clip = SV_ClipHandleForEntity(gEnt);
	obj = Com_GetServerDObj(gEnt->s.number);

	if ( obj && gEnt->r.svFlags & 6 )
	{
		if ( gEnt->r.svFlags & 2 )
		{
			VectorAdd(origin, actorLocationalMins, min);
			VectorAdd(origin, actorLocationalMaxs, max);
		}
		else
		{
			DObjGetBounds(obj, min, max);

			VectorAdd(origin, min, min);
			VectorAdd(origin, max, max);
		}

		CM_LinkEntity(ent, min, max, clip);
	}
	else
	{
		CM_LinkEntity(ent, gEnt->r.absmin, gEnt->r.absmax, clip);
	}
}

/*
===============
SV_UnlinkEntity
===============
*/
void SV_UnlinkEntity( gentity_t *gEnt )
{
	svEntity_t *ent;

	ent = SV_SvEntityForGentity(gEnt);
	gEnt->r.linked = qfalse;
	CM_UnlinkEntity(ent);
}

/*
===============
SV_LocationalSightTraceDObj
===============
*/
DObj* SV_LocationalSightTraceDObj( const sightpointtrace_t *clip, const gentity_t *touch )
{
	if ( !clip->locational )
	{
		return NULL;
	}

	if ( !(touch->r.svFlags & 4) )
	{
		return NULL;
	}

	return Com_GetServerDObj(touch->s.number);
}

/*
===============
SV_PointSightTraceToEntity
===============
*/
int SV_PointSightTraceToEntity( sightpointtrace_t *clip, svEntity_t *check )
{
	vec3_t entAxis[4];
	vec3_t localEnd;
	vec3_t localStart;
	DObjTrace_s objTrace;
	vec3_t absmax;
	vec3_t absmin;
	DObj *obj;
	TraceExtents extents;
	int num;
	float *angles;
	clipHandle_t model;
	gentity_t *touch;

	num = check - sv.svEntities;
	touch = SV_GentityNum(num);

	// if it doesn't have any brushes of a type we
	// are looking for, ignore it
	if ( !(clip->contentmask & touch->r.contents) )
	{
		return 0;
	}

	// see if we should ignore this entity
	if ( clip->passEntityNum[0] != ENTITYNUM_NONE )
	{
		if ( num == clip->passEntityNum[0] )
		{
			return 0; // don't clip against the pass entity
		}

		if ( touch->r.ownerNum == clip->passEntityNum[0] )
		{
			return 0; // don't clip against own missiles
		}
	}

	// see if we should ignore this entity
	if ( clip->passEntityNum[1] != ENTITYNUM_NONE )
	{
		if ( num == clip->passEntityNum[1] )
		{
			return 0; // don't clip against the pass entity
		}

		if ( touch->r.ownerNum == clip->passEntityNum[1] )
		{
			return 0; // don't clip against own missiles
		}
	}

	obj = SV_LocationalSightTraceDObj(clip, touch);

	if ( !obj )
	{
		model = SV_ClipHandleForEntity(touch);
		angles = touch->r.currentAngles;

		if ( !touch->r.bmodel )
		{
			angles = vec3_origin; // boxes don't rotate
		}

		if ( CM_TransformedBoxSightTrace(0, clip->start, clip->end, vec3_origin, vec3_origin, model, clip->contentmask, touch->r.currentOrigin, angles) )
		{
			return -1;
		}

		return 0;
	}

	if ( !DObjHasContents(obj, clip->contentmask) )
	{
		return 0;
	}

	VectorCopy(touch->r.currentOrigin, entAxis[3]);
	DObjGetBounds(obj, absmin, absmax);

	VectorAdd(entAxis[3], absmin, absmin);
	VectorAdd(entAxis[3], absmax, absmax);

	VectorCopy(clip->start, extents.start);
	VectorCopy(clip->end, extents.end);

	CM_CalcTraceEntents(&extents);

	if ( CM_TraceBox(&extents, absmin, absmax, 1.0) )
	{
		return 0;
	}

	G_DObjCalcPose(touch);
	AnglesToAxis(touch->r.currentAngles, entAxis);

	MatrixTransposeTransformVector43(extents.start, entAxis, localStart);
	MatrixTransposeTransformVector43(extents.end, entAxis, localEnd);

	objTrace.fraction = 1.0;
	DObjGeomTraceline(obj, localStart, localEnd, clip->contentmask, &objTrace);

	if ( objTrace.fraction < 1.0 )
	{
		return -1;
	}

	return 0;
}

/*
===============
SV_LocationalTraceDObj
===============
*/
DObj* SV_LocationalTraceDObj( const pointtrace_t *clip, const gentity_t *touch )
{
	if ( !clip->bLocational )
	{
		return NULL;
	}

	if ( !(touch->r.svFlags & 6) )
	{
		return NULL;
	}

	return Com_GetServerDObj(touch->s.number);
}

/*
===============
SV_PointTraceToEntity
===============
*/
void SV_PointTraceToEntity( pointtrace_t *clip, svEntity_t *check, trace_t *trace )
{
	vec3_t entAxis[4];
	float oldFraction;
	vec3_t localEnd;
	vec3_t localStart;
	DObjTrace_s objTrace;
	vec3_t absmax;
	vec3_t absmin;
	DObj *obj;
	int num;
	float *angles;
	clipHandle_t model;
	gentity_t *touch;

	num = check - sv.svEntities;
	touch = SV_GentityNum(num);

	// if it doesn't have any brushes of a type we
	// are looking for, ignore it
	if ( !(clip->contentmask & touch->r.contents) )
	{
		return;
	}

	// see if we should ignore this entity
	if ( clip->passEntNum != ENTITYNUM_NONE )
	{
		if ( num == clip->passEntNum )
		{
			return;   // don't clip against the pass entity
		}

		if ( touch->r.ownerNum == clip->passEntNum )
		{
			return;   // don't clip against own missiles
		}

		if ( touch->r.ownerNum == clip->ignoreEntNum )
		{
			return;   // don't clip against other missiles from our owner
		}
	}

	obj = SV_LocationalTraceDObj(clip, touch);

	if ( !obj )
	{
		// if it doesn't have any brushes of a type we
		// are looking for, ignore it
		if ( !(clip->contentmask & check->linkcontents) )
		{
			return;
		}

		if ( CM_TraceBox(&clip->extents, touch->r.absmin, touch->r.absmax, trace->fraction) )
		{
			return;
		}

		model = SV_ClipHandleForEntity(touch);
		angles = touch->r.currentAngles;

		if ( !touch->r.bmodel )
		{
			angles = vec3_origin; // boxes don't rotate
		}

		oldFraction = trace->fraction;

		CM_TransformedBoxTrace(trace, clip->extents.start, clip->extents.end, vec3_origin, vec3_origin, model, clip->contentmask, touch->r.currentOrigin, angles);

		if ( trace->fraction >= oldFraction )
		{
			return;
		}

		trace->surfaceFlags = 0;
		trace->partName = 0;
		trace->partGroup = 0;
		trace->entityNum = touch->s.number;
		trace->contents = touch->r.contents;
		trace->material = NULL;

		return;
	}

	if ( touch->r.svFlags & 4 )
	{
		if ( !DObjHasContents(obj, clip->contentmask) )
		{
			return;
		}

		VectorCopy(touch->r.currentOrigin, entAxis[3]);
		DObjGetBounds(obj, absmin, absmax);

		VectorAdd(entAxis[3], absmin, absmin);
		VectorAdd(entAxis[3], absmax, absmax);
	}
	else if ( clip->priorityMap )
	{
		VectorCopy(touch->r.currentOrigin, entAxis[3]);

		VectorAdd(entAxis[3], actorLocationalMins, absmin);
		VectorAdd(entAxis[3], actorLocationalMaxs, absmax);
	}
	else
	{
		return;
	}

	if ( CM_TraceBox(&clip->extents, absmin, absmax, trace->fraction) )
	{
		return;
	}

	G_DObjCalcPose(touch);
	AnglesToAxis(touch->r.currentAngles, entAxis);

	MatrixTransposeTransformVector43(clip->extents.start, entAxis, localStart);
	MatrixTransposeTransformVector43(clip->extents.end, entAxis, localEnd);

	objTrace.fraction = trace->fraction;

	if ( touch->r.svFlags & 4 )
	{
		DObjGeomTraceline(obj, localStart, localEnd, clip->contentmask, &objTrace);
	}
	else
	{
		DObjTraceline(obj, localStart, localEnd, clip->priorityMap, &objTrace);
	}

	if ( objTrace.fraction >= trace->fraction )
	{
		return;
	}

	trace->fraction = objTrace.fraction;
	trace->surfaceFlags = objTrace.surfaceflags;
	trace->partName = objTrace.partName;
	trace->partGroup = objTrace.partGroup;

	MatrixTransformVector(objTrace.normal, entAxis, trace->normal);

	trace->entityNum = touch->s.number;
	trace->contents = touch->r.contents;
	trace->material = NULL;
}

/*
===============
SV_LocationalTraceToEntity
===============
*/
void SV_LocationalTraceToEntity( trace_t *results, const vec3_t start, const vec3_t end, int entIndex, int contentMask, unsigned char *priorityMap )
{
	pointtrace_t ptrace;
	svEntity_t *check;
	gentity_t *ent;

	ent = SV_GentityNum(entIndex);
	check = SV_SvEntityForGentity(ent);

	memset(results, 0, sizeof(trace_t));

	results->fraction = 1.0;
	results->entityNum = ENTITYNUM_NONE;

	VectorCopy(start, ptrace.extents.start);
	VectorCopy(end, ptrace.extents.end);

	CM_CalcTraceEntents(&ptrace.extents);

	ptrace.passEntNum = -1;
	ptrace.ignoreEntNum = -1;
	ptrace.contentmask = contentMask;
	ptrace.bLocational = 1;
	ptrace.priorityMap = priorityMap;

	SV_PointTraceToEntity(&ptrace, check, results);
}

/*
===============
SV_SightTrace
===============
*/
void SV_SightTrace( int *hitNum, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum0, int passEntityNum1, int contentmask )
{
	int entNum;
	sightclip_t clip;
	sightpointtrace_t spt;
	vec3_t temp;

	*hitNum = CM_BoxSightTrace(*hitNum, start, end, mins, maxs, 0, contentmask);

	if ( *hitNum )
	{
		return;
	}

	if ( !(maxs[0] - mins[0] + maxs[1] - mins[1] + maxs[2] - mins[2]) )
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

		VectorAdd(maxs, mins, temp);
		VectorScale(temp, 0.5, temp);
		VectorAdd(start, temp, clip.start);
		VectorAdd(end, temp, clip.end);

		entNum = CM_ClipSightTraceToEntities(&clip);
	}

	*hitNum = entNum;
}

/*
===============
SV_TracePassed
===============
*/
int SV_TracePassed( const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum0, int passEntityNum1, int contentmask, int locational, int staticmodels )
{
	sightclip_t clip;
	sightpointtrace_t spt;
	vec3_t temp;

	if ( CM_BoxSightTrace(0, start, end, mins, maxs, 0, contentmask) )
	{
		return 0;
	}

	if ( staticmodels )
	{
		if ( !CM_PointTraceStaticModelsComplete(start, end, contentmask) )
		{
			return 0;
		}
	}

	if ( !(maxs[0] - mins[0] + maxs[1] - mins[1] + maxs[2] - mins[2]) )
	{
		spt.contentmask = contentmask;

		VectorCopy(start, spt.start);
		VectorCopy(end, spt.end);

		spt.passEntityNum[0] = passEntityNum0;
		spt.passEntityNum[1] = passEntityNum1;

		spt.locational = locational;

		if ( CM_PointSightTraceToEntities(&spt) )
		{
			return 0;
		}
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
		{
			return 0;
		}
	}

	return 1;
}

/*
==================
SV_Trace

Moves the given mins/maxs volume through the world from start to end.
passEntityNum and entities owned by passEntityNum are explicitly not checked.
==================
*/
void SV_Trace( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask, int locational, unsigned char *priorityMap, int staticmodels )
{
	moveclip_t clip;
	pointtrace_t pt;
	vec3_t temp;

	// clip to world
	CM_BoxTrace(results, start, end, mins, maxs, 0, contentmask);
	results->entityNum = results->fraction != 1.0 ? ENTITYNUM_WORLD : ENTITYNUM_NONE;

	if ( results->fraction == 0 )
	{
		return;     // blocked immediately by the world
	}

	if ( staticmodels )
	{
		CM_PointTraceStaticModels(results, start, end, contentmask);

		if ( results->fraction == 0 )
		{
			return; // blocked by static model
		}
	}

	if ( !(maxs[0] - mins[0] + maxs[1] - mins[1] + maxs[2] - mins[2]) )
	{
		pt.contentmask = contentmask;

		VectorCopy(start, pt.extents.start);
		VectorCopy(end, pt.extents.end);

		CM_CalcTraceEntents(&pt.extents);

		pt.passEntNum = passEntityNum;
		pt.bLocational = locational;
		pt.priorityMap = priorityMap;

		if ( passEntityNum == ENTITYNUM_NONE )
		{
			pt.ignoreEntNum = -1;
		}
		else
		{
			pt.ignoreEntNum = SV_GentityNum(passEntityNum)->r.ownerNum;

			if ( pt.ignoreEntNum == ENTITYNUM_NONE )
			{
				pt.ignoreEntNum = -1;
			}
		}

		CM_PointTraceToEntities(&pt, results);
	}
	else
	{
		clip.contentmask = contentmask;
		clip.passEntityNum = passEntityNum;

		if ( passEntityNum == ENTITYNUM_NONE )
		{
			clip.passOwnerNum = -1;
		}
		else
		{
			clip.passOwnerNum = SV_GentityNum(passEntityNum)->r.ownerNum;

			if ( clip.passOwnerNum == ENTITYNUM_NONE )
			{
				clip.passOwnerNum = -1;
			}
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

		CM_CalcTraceEntents(&clip.extents);

		// clip to other solid entities
		CM_ClipMoveToEntities(&clip, results);
	}
}