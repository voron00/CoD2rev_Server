#include "../qcommon/qcommon.h"
#include "../clientscript/clientscript_public.h"

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

qboolean XModelBad(XModel *model)
{
	return model->bad;
}

void QDECL XModelGetBounds(const XModel *model, float *mins, float *maxs)
{
	VectorCopy(model->mins, mins);
	VectorCopy(model->maxs, maxs);
}

void QDECL XModelPartsFree(XModelParts *modelPart)
{
	int numBones;
	int i;
	unsigned short *name;

	name = *modelPart->boneNames;
	numBones = modelPart->numBones;

	for ( i = 0; i < numBones; ++i )
		SL_RemoveRefToString(name[i]);
}

void QDECL XModelFree(XModel *model)
{
	int j;
	int i;

	if ( !XModelBad(model) )
	{
		for ( i = 0; i < 4; ++i )
		{
			if ( model->lodInfo[i].surfnames )
			{
				for ( j = 0; j < model->lodInfo[i].numsurfs; ++j )
					SL_RemoveRefToString(model->lodInfo[i].surfnames[j]);

				model->lodInfo[i].surfnames = 0;
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

XModelParts* QDECL XModelPartsLoad(XModel *model, const char *partName, void *(*Alloc)(int))
{
	XModelParts *modelParts;

	modelParts = XModelPartsFindData(partName);

	if ( modelParts )
		return modelParts;

	modelParts = XModelPartsLoadFile(model, partName, Alloc);

	if ( modelParts )
	{
		XModelPartsSetData(partName, modelParts, Alloc);
		return modelParts;
	}
	else
	{
		Com_Printf("^1ERROR: Cannot find xmodelparts '%s'.\n", partName);
		return NULL;
	}
}

XModelSurfs* XModelReadSurface()
{
	UNIMPLEMENTED(__FUNCTION__);
	return NULL;
}

XModelSurfs* XModelLoadSurface(XModel *model, const char *surfName, void *(*Alloc)(int))
{
	XModelSurfs *surface;

	surface = XModelSurfsFindData(surfName);

	if ( surface )
		return surface;

	surface = XModelReadSurface();

	if ( surface )
	{
		XModelSurfsSetData(surfName, surface, Alloc);
		return surface;
	}
	else
	{
		Com_Printf("^1ERROR: Cannot find 'xmodelsurfs '%s'.\n", surfName);
		return NULL;
	}
}

bool XModelSurfsLoad(XModel *model, void *(*Alloc)(int))
{
	int i;

	for ( i = 0; i < 4 && *model->lodInfo[i].name; ++i )
	{
		model->lodInfo[i].surface = XModelLoadSurface(model, model->lodInfo[i].name, Alloc);

		if ( !model->lodInfo[i].surface )
			return false;
	}

	return true;
}

XModel defaultModel;
XModelParts defaultModelPart;
XBoneInfo defaultBoneInfo;
XModelSurfs defaultSurface;

unsigned short *pDefaultBoneNames;
unsigned short defaultBoneNames[2];
unsigned short defaultSurfnames;

byte defaultpartClassification;

XModelParts* setDefaultModelPart()
{
	pDefaultBoneNames = defaultBoneNames;
	defaultModelPart.boneNames = &pDefaultBoneNames;
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

XModel* XModelLoadDefaultModel(const char *name, void *(*Alloc)(int))
{
	XModel *model;
	int i;

	model = (XModel *)Alloc(sizeof(XModel));

	model->bad = 1;
	model->modelParts = setDefaultModelPart();

	for ( i = 0; i < 4; ++i )
	{
		model->lodInfo[i].surface = 0;
		model->lodInfo[i].name = "";
		model->lodInfo[i].dist = 0.0;
		model->lodInfo[i].numsurfs = 1;
		model->lodInfo[i].surfnames = &defaultSurfnames;
		defaultSurfnames = 0;
	}

	model->lodInfo[0].surface = setDefaultSurface();
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

bool XModelSurfacesSupported()
{
#ifdef DEDICATED
	return false;
#endif
	return true;
}

XModel* QDECL XModelLoad(const char *name, void *(*Alloc)(int), void *(*AllocColl)(int))
{
	XModel *model;

	model = XModelLoadFile(name, Alloc, AllocColl);

	if ( !model )
		return NULL;

	if ( !XModelSurfacesSupported() || XModelSurfsLoad(model, Alloc) )
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
		return XModelLoadDefaultModel(name, Alloc);
	}
}