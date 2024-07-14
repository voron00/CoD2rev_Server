#include "../qcommon/qcommon.h"
#include "../script/script_public.h"

qboolean XModelGetStaticBounds(const XModel *model, float (*axis)[3], float *mins, float *maxs)
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

int XModelTraceLine(const XModel *model, trace_t *results, DObjAnimMat *pose, const float *localStart, const float *localEnd, int contentmask)
{
	float dot;
	float axis[3][3];
	vec3_t normal;
	int j;
	int i;
	float t;
	float s;
	float dist;
	float fraction;
	float endDist;
	float startDist;
	vec3_t hit;
	vec3_t boneVec;
	XModelCollTri_s *tri;
	XModelCollSurf_s *surf;
	int boneIndex;
	TraceExtents boneExtents;
	vec3_t endDelta;
	vec3_t startDelta;
	DObjAnimMat *Mat;
	int numBones;
	int boneIdx;

	boneIndex = -1;
	numBones = XModelNumBones(model);

	for ( i = 0; i < model->numCollSurfs; ++i )
	{
		surf = &model->collSurfs[i];

		if ( (surf->contents & contentmask) != 0 )
		{
			boneIdx = surf->boneIdx;
			Mat = &pose[boneIdx];
			VectorSubtract(localStart, Mat->trans, startDelta);
			VectorSubtract(localEnd, Mat->trans, endDelta);
			ConvertQuatToMat(Mat, axis);
			MatrixTransposeTransformVector(startDelta, axis, boneExtents.start);
			MatrixTransposeTransformVector(endDelta, axis, boneExtents.end);
			CM_CalcTraceExtents(&boneExtents);

			if ( !CM_TraceBox(&boneExtents, surf->mins, surf->maxs, results->fraction) )
			{
				VectorSubtract(boneExtents.end, boneExtents.start, boneVec);

				for ( j = 0; j < surf->numCollTris; ++j )
				{
					tri = &surf->collTris[j];
					dot = DotProduct(boneExtents.end, tri->plane);
					startDist = dot - tri->plane[3];

					if ( startDist < 0.0 )
					{
						dot = DotProduct(boneExtents.start, tri->plane);
						endDist = dot - tri->plane[3];

						if ( endDist > 0.0 )
						{
							fraction = (endDist - 0.125) / (endDist - startDist);
							fraction = I_fmax(fraction, 0.0);

							if ( fraction < results->fraction )
							{
								dist = endDist / (endDist - startDist);
								VectorMA(boneExtents.start, dist, boneVec, hit);
								dot = DotProduct(hit, tri->svec);
								s = dot - tri->svec[3];

								if ( s >= -0.001 && s <= 1.001 )
								{
									dot = DotProduct(hit, tri->tvec);
									t = dot - tri->tvec[3];

									if ( t >= -0.001 && s + t <= 1.001 )
									{
										boneIndex = boneIdx;
										results->startsolid = 0;
										results->allsolid = 0;
										results->fraction = fraction;
										results->surfaceFlags = surf->surfFlags;
										results->contents = surf->contents;
										VectorCopy(tri->plane, results->normal);
									}
								}
							}
						}
					}
				}
			}
		}
	}

	if ( boneIndex < 0 )
		return -1;

	Mat = &pose[boneIndex];
	ConvertQuatToMat(Mat, axis);
	MatrixTransformVector(results->normal, axis, normal);
	VectorCopy(normal, results->normal);

	return boneIndex;
}

qboolean XModelBad(XModel *model)
{
	return model->bad;
}

void XModelGetBounds(const XModel *model, float *mins, float *maxs)
{
	VectorCopy(model->mins, mins);
	VectorCopy(model->maxs, maxs);
}

void XModelPartsFree(XModelParts *modelParts)
{
	int numBones;
	int i;
	unsigned short *name;

	name = modelParts->hierarchy->names;
	numBones = modelParts->numBones;

	for ( i = 0; i < numBones; ++i )
		SL_RemoveRefToString(name[i]);
}

void XModelFree(XModel *model)
{
	int j;
	int i;

	if ( !XModelBad(model) )
	{
		for ( i = 0; i < 4; ++i )
		{
			if ( model->lodInfo[i].surfNames )
			{
				for ( j = 0; j < model->lodInfo[i].numsurfs; ++j )
					SL_RemoveRefToString(model->lodInfo[i].surfNames[j]);

				model->lodInfo[i].surfNames = 0;
			}
		}
	}
}

XModelParts* XModelPartsFindData(const char *partName)
{
	return (XModelParts *)Hunk_FindDataForFile(FILEDATA_XMODELPARTS, partName);
}

void XModelPartsSetData(const char *partName, XModelParts *parts, void *(*Alloc)(int))
{
	Hunk_SetDataForFile(FILEDATA_XMODELPARTS, partName, parts, Alloc);
}

XModelSurfs* XModelSurfsFindData(const char *surfName)
{
	return (XModelSurfs *)Hunk_FindDataForFile(FILEDATA_XMODELSURFS, surfName);
}

void XModelSurfsSetData(const char *surfName, XModelSurfs *surface, void *(*Alloc)(int))
{
	Hunk_SetDataForFile(FILEDATA_XMODELSURFS, surfName, surface, Alloc);
}

XModel* XModelFindData(const char *model)
{
	return (XModel *)Hunk_FindDataForFile(FILEDATA_XMODEL, model);
}

const char* XModelSetData(const char *modelName, XModel *model, void *(*Alloc)(int))
{
	return Hunk_SetDataForFile(FILEDATA_XMODEL, modelName, model, Alloc);
}

XModelParts* XModelPartsPrecache(XModel *model, const char *name, void *(*Alloc)(int))
{
	XModelParts *parts;

	parts = XModelPartsFindData(name);

	if ( parts )
		return parts;

	parts = XModelPartsLoadFile(model, name, Alloc);

	if ( parts )
	{
		XModelPartsSetData(name, parts, Alloc);
		return parts;
	}
	else
	{
		Com_Printf("^1ERROR: Cannot find xmodelparts '%s'.\n", name);
		return NULL;
	}
}

XModelSurfs* XModelReadSurfaces()
{
	UNIMPLEMENTED(__FUNCTION__);
	return NULL;
}

XModelSurfs* XModelSurfsPrecache(XModel *model, const char *name, void *(*Alloc)(int))
{
	XModelSurfs *surface;

	surface = XModelSurfsFindData(name);

	if ( surface )
		return surface;

	surface = XModelReadSurfaces();

	if ( surface )
	{
		XModelSurfsSetData(name, surface, Alloc);
		return surface;
	}
	else
	{
		Com_Printf("^1ERROR: Cannot find 'xmodelsurfs '%s'.\n", name);
		return NULL;
	}
}

bool XModelSurfsLoad(XModel *model, void *(*Alloc)(int))
{
	int i;

	for ( i = 0; i < 4 && *model->lodInfo[i].filename; ++i )
	{
		model->lodInfo[i].surfs = XModelSurfsPrecache(model, model->lodInfo[i].filename, Alloc);

		if ( !model->lodInfo[i].surfs )
			return false;
	}

	return true;
}

XModel defaultModel;
XModelParts defaultModelPart;
XBoneInfo defaultBoneInfo;
XModelSurfs defaultSurface;
XBoneHierarchy defaultHierarchy;
unsigned short defaultBoneNames[2];
unsigned short defaultSurfnames;
byte defaultpartClassification;

XModelParts* XModelCreateDefaultParts()
{
	defaultHierarchy.names = defaultBoneNames;
	defaultModelPart.hierarchy = &defaultHierarchy;
	defaultModelPart.quats = 0;
	defaultModelPart.trans = 0;
	defaultModelPart.numBones = 1;
	defaultModelPart.numRootBones = 1;
	defaultModelPart.partClassification = &defaultpartClassification;
	defaultpartClassification = 0;
	defaultBoneNames[0] = 0;

	return &defaultModelPart;
}

XModelSurfs* setDefaultSurface()
{
	defaultSurface.surf = NULL;
	return &defaultSurface;
}

XModel* XModelMakeDefault(const char *name, void *(*Alloc)(int))
{
	XModel *model;
	int i;

	model = (XModel *)Alloc(sizeof(XModel));

	model->bad = 1;
	model->parts = XModelCreateDefaultParts();

	for ( i = 0; i < 4; ++i )
	{
		model->lodInfo[i].surfs = 0;
		model->lodInfo[i].filename = "";
		model->lodInfo[i].dist = 0.0;
		model->lodInfo[i].numsurfs = 1;
		model->lodInfo[i].surfNames = &defaultSurfnames;
		defaultSurfnames = 0;
	}

	model->lodInfo[0].surfs = setDefaultSurface();
	model->numLods = 1;
	model->collLod = 0;
	model->name = "DEFAULT";

	defaultBoneInfo.bounds[0][0] = -16.0;
	defaultBoneInfo.bounds[0][1] = -16.0;
	defaultBoneInfo.bounds[0][2] = -16.0;
	defaultBoneInfo.bounds[1][0] = 16.0;
	defaultBoneInfo.bounds[1][1] = 16.0;
	defaultBoneInfo.bounds[1][2] = 16.0;

	model->boneInfo = &defaultBoneInfo;

	XModelSetData(name, model, Alloc);

	return model;
}

bool XModelSurfsSupported()
{
#ifdef DEDICATED
	return false;
#endif
	return true;
}

XModel* XModelLoad(const char *name, void *(*Alloc)(int), void *(*AllocColl)(int))
{
	XModel *model;

	model = XModelLoadFile(name, Alloc, AllocColl);

	if ( !model )
		return NULL;

	if ( !XModelSurfsSupported() || XModelSurfsLoad(model, Alloc) )
		return model;

	XModelFree(model);
	return NULL;
}

XModel* XModelPrecache(const char *name, void *(*Alloc)(int), void *(*AllocColl)(int))
{
	XModel *model;

	model = XModelFindData(name);

	if ( model )
		return model;

	model = XModelLoad(name, Alloc, AllocColl);

	if ( model )
	{
		model->name = XModelSetData(name, model, Alloc);
		return model;
	}
	else
	{
		Com_Printf("^1ERROR: Cannot find xmodel '%s'.\n", name);
		return XModelMakeDefault(name, Alloc);
	}
}