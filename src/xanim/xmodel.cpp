#include "../qcommon/qcommon.h"
#include "../script/script_public.h"

/*
===============
LocalMatrixTransformVector
===============
*/
void LocalMatrixTransformVector( const vec3_t in1, const vec3_t in2[3], vec3_t out )
{
	out[ 0 ] = ( in1[ 0 ] * in2[ 0 ][ 0 ] ) + ( in1[ 1 ] * in2[ 1 ][ 0 ] ) + ( in1[ 2 ] * in2[ 2 ][ 0 ] );
	out[ 1 ] = ( in1[ 0 ] * in2[ 0 ][ 1 ] ) + ( in1[ 1 ] * in2[ 1 ][ 1 ] ) + ( in1[ 2 ] * in2[ 2 ][ 1 ] );
	out[ 2 ] = ( in1[ 0 ] * in2[ 0 ][ 2 ] ) + ( in1[ 1 ] * in2[ 1 ][ 2 ] ) + ( in1[ 2 ] * in2[ 2 ][ 2 ] );
}

/*
===============
LocalMatrixTransposeTransformVector
===============
*/
void LocalMatrixTransposeTransformVector( const vec3_t in1, const vec3_t in2[3], vec3_t out )
{
	out[ 0 ] = in1[ 0 ] * in2[ 0 ][ 0 ] + in1[ 1 ] * in2[ 0 ][ 1 ] + in1[ 2 ] * in2[ 0 ][ 2 ];
	out[ 1 ] = in1[ 0 ] * in2[ 1 ][ 0 ] + in1[ 1 ] * in2[ 1 ][ 1 ] + in1[ 2 ] * in2[ 1 ][ 2 ];
	out[ 2 ] = in1[ 0 ] * in2[ 2 ][ 0 ] + in1[ 1 ] * in2[ 2 ][ 1 ] + in1[ 2 ] * in2[ 2 ][ 2 ];
}

/*
===============
Com_ValidXModelName
===============
*/
bool Com_ValidXModelName( const char *name )
{
	return !I_strnicmp(name, "xmodel", 6) && name[6] == '/';
}

/*
===============
XModelGetMemUsage
===============
*/
int XModelGetMemUsage( const XModel *model )
{
	return model->memUsage;
}

/*
===============
XModelGetSkins
===============
*/
const trXSkin_t* XModelGetSkins( const XModel *model )
{
	assert(model);
	return &model->xskins;
}

/*
===============
XModelGetCollisionLod
===============
*/
int XModelGetCollisionLod( const XModel *model )
{
	assert(model);
	return model->collLod;
}

/*
===============
XModelGetContents
===============
*/
int XModelGetContents( const XModel *model )
{
	assert(model);
	return model->contents;
}

/*
===============
XModelGetLodName
===============
*/
const char *XModelGetLodName( const XModel *model, int lod )
{
	return model->lodInfo[lod].filename;
}

/*
===============
XModelGetBoneIndex
===============
*/
int XModelGetBoneIndex( const XModel *model, unsigned int name )
{
	int numBones, localBoneIndex;
	unsigned short *boneNames;

	assert(model);

	boneNames = model->parts->hierarchy->names;
	numBones  = model->parts->numBones;

	assert(numBones < DOBJ_MAX_PARTS);

	for ( localBoneIndex = numBones - 1; ; localBoneIndex-- )
	{
		if ( localBoneIndex < 0 )
		{
			break;
		}

		if ( name == boneNames[localBoneIndex] )
		{
			break;
		}
	}

	assert((unsigned)localBoneIndex < DOBJ_MAX_PARTS);
	return localBoneIndex;
}

/*
===============
XModelBoneNames
===============
*/
unsigned short* XModelBoneNames( XModel *model )
{
	assert(model);
	return model->parts->hierarchy->names;
}

/*
===============
XModelSetOptimize
===============
*/
void XModelSetOptimize( int )
{
	UNIMPLEMENTED(__FUNCTION__);
}

/*
===============
XModelGetBounds
===============
*/
void XModelGetBounds( const XModel *model, vec3_t mins, vec3_t maxs )
{
	VectorCopy(model->mins, mins);
	VectorCopy(model->maxs, maxs);
}

/*
===============
XModelBad
===============
*/
bool XModelBad( const XModel *model )
{
	return model->bad;
}

/*
===============
XModelTraceLine
===============
*/
int XModelTraceLine( const XModel *model, trace_t *results, DObjAnimMat *pose, const vec3_t localStart, const vec3_t localEnd, int contentmask )
{
	float axis[3][3];
	int i, j, partIndex, numBones, boneIdx;
	float s, t, frac, hitFrac, startDist, endDist;
	vec3_t hit, boneVec, normal;
	XModelCollTri_s *ctri;
	XModelCollSurf_s *csurf;
	TraceExtents boneExtents;
	vec3_t startDelta, endDelta;
	DObjAnimMat *Mat;

	partIndex = -1;
	numBones = XModelNumBones(model);

	for ( i = 0; i < model->numCollSurfs; i++ )
	{
		csurf = &model->collSurfs[i];

		if ( !( csurf->contents & contentmask ) )
		{
			continue;
		}

		boneIdx = csurf->boneIdx;
		Mat = &pose[boneIdx];
		assert(Mat);

		VectorSubtract(localStart, Mat->trans, startDelta);
		VectorSubtract(localEnd, Mat->trans, endDelta);

		ConvertQuatToMat(Mat, axis);

		LocalMatrixTransposeTransformVector(startDelta, axis, boneExtents.start);
		LocalMatrixTransposeTransformVector(endDelta, axis, boneExtents.end);

		CM_CalcTraceEntents(&boneExtents);

		if ( CM_TraceBox(&boneExtents, csurf->mins, csurf->maxs, results->fraction) )
		{
			continue;
		}

		VectorSubtract(boneExtents.end, boneExtents.start, boneVec);

		for ( j = 0; j < csurf->numCollTris; j++ )
		{
			ctri = &csurf->collTris[j];

			endDist = DotProduct(boneExtents.end, ctri->plane) - ctri->plane[3];

			if ( endDist >= 0 )
			{
				continue;
			}

			startDist = DotProduct(boneExtents.start, ctri->plane) - ctri->plane[3];

			if ( startDist <= 0 )
			{
				continue;
			}

			frac = I_fmax( ( startDist - SURFACE_CLIP_EPSILON ) / ( startDist - endDist ), 0.0f );

			if ( frac >= results->fraction )
			{
				continue;
			}

			hitFrac = startDist / (startDist - endDist);
			VectorMA(boneExtents.start, hitFrac, boneVec, hit);

			s = DotProduct(hit, ctri->svec) - ctri->svec[3];

			if ( s < -0.001f || s > 1.001f )
			{
				continue;
			}

			t = DotProduct(hit, ctri->tvec) - ctri->tvec[3];

			if ( t < -0.001f || t > 1.001f )
			{
				continue;
			}

			partIndex = boneIdx;

			results->startsolid = qfalse;
			results->allsolid = qfalse;

			results->fraction = frac;
			assert(results->fraction >= 0 && results->fraction <= 1.0f);

			results->surfaceFlags = csurf->surfFlags;
			results->contents = csurf->contents;

			VectorCopy(ctri->plane, results->normal);
		}
	}

	if ( partIndex < 0 )
	{
		return -1;
	}

	Mat = &pose[partIndex];
	assert(Mat);

	ConvertQuatToMat(Mat, axis);

	LocalMatrixTransformVector(results->normal, axis, normal);
	VectorCopy(normal, results->normal);

	return partIndex;
}


/*
===============
XModelFree
===============
*/
void XModelFree( XModel *model )
{
	assert(model);

	if ( XModelBad(model) )
	{
		return;
	}

	for ( int i = 0; i < MAX_LODS; i++ )
	{
		if ( !model->lodInfo[i].surfNames )
		{
			continue;
		}

		for ( int j = 0; j < model->lodInfo[i].numsurfs; j++ )
		{
			SL_RemoveRefToString(model->lodInfo[i].surfNames[j]);
		}

		model->lodInfo[i].surfNames = 0;
	}
}

/*
===============
XModelPartsFree
===============
*/
void XModelPartsFree( XModelParts *modelParts )
{
	unsigned short *boneNames;
	int numBones;

	assert(modelParts);

	boneNames = modelParts->hierarchy->names;
	numBones = modelParts->numBones;

	for ( int i = 0; i < numBones; i++ )
	{
		SL_RemoveRefToString(boneNames[i]);
	}
}

/*
===============
XModelFindExisting
===============
*/
XModel* XModelFindExisting( const char *model )
{
	return (XModel *)Hunk_FindDataForFile( FILEDATA_XMODEL, model );
}

/*
===============
XModelSetData
===============
*/
const char* XModelSetData( const char *modelName, XModel *model, Alloc_t Alloc )
{
	return Hunk_SetDataForFile( FILEDATA_XMODEL, modelName, model, Alloc );
}

struct XModelDefault
{
	unsigned short boneNames[1];
	XBoneHierarchy hierarchy;
	XModelParts modelParts;
	XModelSurfs modelSurf;
	XBoneInfo boneInfo;
	unsigned char partClassification[1];
	unsigned short surfNames[1];
};

XModelDefault g_default;

/*
===============
XModelCreateDefaultParts
===============
*/
XModelParts_s *XModelCreateDefaultParts()
{
	g_default.hierarchy.names = g_default.boneNames;
	g_default.modelParts.hierarchy = &g_default.hierarchy;

	g_default.modelParts.quats = 0;
	g_default.modelParts.trans = 0;

	g_default.modelParts.numBones = 1;
	g_default.modelParts.numRootBones = 1;

	g_default.modelParts.partClassification = g_default.partClassification;
	g_default.partClassification[0] = 0;

	g_default.boneNames[0] = 0;

	return &g_default.modelParts;
}

/*
===============
XModelCreateDefaultSurface
===============
*/
XModelSurfs_s *XModelCreateDefaultSurface()
{
	g_default.modelSurf.surfs = 0;
	return &g_default.modelSurf;
}

/*
===============
XModelMakeDefault
===============
*/
void XModelMakeDefault( XModel *model )
{
	model->bad = true;
	model->parts = XModelCreateDefaultParts();

	for ( int i = 0; i < MAX_LODS; i++ )
	{
		model->lodInfo[i].surfs = 0;
		model->lodInfo[i].filename = "";
		model->lodInfo[i].dist = 0;
		model->lodInfo[i].numsurfs = 1;
		model->lodInfo[i].surfNames = g_default.surfNames;

		g_default.surfNames[0] = 0;
	}

	model->lodInfo->surfs = XModelCreateDefaultSurface();
	model->numLods = 1;
	model->collLod = 0;

	model->name = "DEFAULT";

	g_default.boneInfo.bounds[0][0] = -16;
	g_default.boneInfo.bounds[0][1] = -16;
	g_default.boneInfo.bounds[0][2] = -16;

	g_default.boneInfo.bounds[1][0] = 16;
	g_default.boneInfo.bounds[1][1] = 16;
	g_default.boneInfo.bounds[1][2] = 16;

	model->boneInfo = &g_default.boneInfo;
}


/*
===============
XModelCreateDefault
===============
*/
XModel* XModelCreateDefault( Alloc_t Alloc )
{
	XModel *model = (XModel *)Alloc( sizeof( XModel ) );
	XModelMakeDefault(model);

	return model;
}

/*
===============
XModelDefaultModel
===============
*/
XModel* XModelDefaultModel( const char *name, Alloc_t Alloc )
{
	XModel *model = XModelCreateDefault(Alloc);
	XModelSetData(name, model, Alloc);

	return model;
}

/*
===============
XModelSurfsSupported
===============
*/
bool XModelHasSurfs()
{
	return false; // not supported in server binary
}

/*
===============
R_XModelSurfsLoadFile
===============
*/
XModelSurfs_s* R_XModelSurfsLoadFile()
{
	return NULL; // not supported in server binary
}

/*
===============
XModelPartsFindData
===============
*/
XModelParts* XModelPartsFindData( const char *name )
{
	return (XModelParts *)Hunk_FindDataForFile( FILEDATA_XMODELPARTS, name );
}

/*
===============
XModelPartsSetData
===============
*/
void XModelPartsSetData( const char *partName, XModelParts *modelParts, Alloc_t Alloc )
{
	Hunk_SetDataForFile( FILEDATA_XMODELPARTS, partName, modelParts, Alloc );
}

/*
===============
XModelSurfsFindData
===============
*/
XModelSurfs* XModelSurfsFindData( const char *surfName )
{
	return (XModelSurfs *)Hunk_FindDataForFile( FILEDATA_XMODELSURFS, surfName );
}

/*
===============
XModelSurfsSetData
===============
*/
void XModelSurfsSetData( const char *surfName, XModelSurfs *modelSurfs, Alloc_t Alloc )
{
	Hunk_SetDataForFile( FILEDATA_XMODELSURFS, surfName, modelSurfs, Alloc );
}

/*
===============
XModelSurfsPrecache
===============
*/
XModelSurfs_s* XModelSurfsPrecache( XModel *model, const char *surfName, Alloc_t Alloc )
{
	XModelSurfs_s *modelSurfs = XModelSurfsFindData(surfName);

	if ( modelSurfs )
	{
		return modelSurfs;
	}

	modelSurfs = R_XModelSurfsLoadFile();

	if ( modelSurfs )
	{
		XModelSurfsSetData( surfName, modelSurfs, *Alloc );
		return modelSurfs;
	}

	Com_Printf("ERROR: Cannot find 'xmodelsurfs '%s'.\n", surfName);
	return NULL;
}

/*
===============
XModelSurfsLoad
===============
*/
bool XModelSurfsLoad( XModel *model, Alloc_t Alloc )
{
	for ( int i = 0; i < MAX_LODS && *model->lodInfo[i].filename; i++ )
	{
		model->lodInfo[i].surfs = XModelSurfsPrecache( model, model->lodInfo[i].filename, Alloc );

		if ( !model->lodInfo[i].surfs )
		{
			return false;
		}
	}

	return true;
}

/*
===============
XModelLoad
===============
*/
XModel* XModelLoad( const char *name, Alloc_t Alloc, Alloc_t AllocColl )
{
	XModel *model = XModelLoadFile(name, Alloc, AllocColl);

	if ( !model )
	{
		return NULL;
	}

	if ( !XModelHasSurfs() )
	{
		return model;
	}

	if ( XModelSurfsLoad(model, Alloc) )
	{
		return model;
	}

	XModelFree(model);
	return NULL;
}

/*
===============
XModelPartsPrecache
===============
*/
XModelParts* XModelPartsPrecache( XModel *model, const char *name, Alloc_t Alloc )
{
	XModelParts *parts = XModelPartsFindData(name);

	if ( parts )
	{
		return parts;
	}

	parts = XModelPartsLoadFile(model, name, Alloc);

	if ( parts )
	{
		XModelPartsSetData(name, parts, Alloc);
		return parts;
	}

	Com_Printf("ERROR: Cannot find xmodelparts '%s'.\n", name);
	return NULL;
}

/*
===============
XModelPrecache
===============
*/
XModel* XModelPrecache( const char *name, Alloc_t Alloc, Alloc_t AllocColl )
{
	XModel *model = XModelFindExisting(name);

	if ( model )
	{
		return model;
	}

	model = XModelLoad( name, Alloc, AllocColl );

	if ( model )
	{
		model->name = XModelSetData( name, model, Alloc );
		return model;
	}

	Com_Printf("ERROR: Cannot find xmodel '%s'.\n", name);
	return XModelDefaultModel(name, Alloc);
}
