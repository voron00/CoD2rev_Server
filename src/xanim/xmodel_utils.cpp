#include "../qcommon/qcommon.h"

TestLod g_testLods[MAX_LODS];

/*
==================
XModelSetTestLods
==================
*/
void XModelSetTestLods( int lodLevel, float dist )
{
	assert((unsigned)lodLevel < MAX_LODS);

	g_testLods[lodLevel].dist = dist;
	g_testLods[lodLevel].enabled = dist >= 0.0f;
}

/*
==================
XModelGetBasePoseBone
==================
*/
const DObjAnimMat* XModelGetBasePoseBone( const XModel *model, int skelMatBoneOffset )
{
	UNIMPLEMENTED(__FUNCTION__);
	return NULL;
}

/*
==================
XModelGetBasePose
==================
*/
DObjAnimMat* XModelGetBasePose( const XModel *model )
{
	assert(model);
	return &model->parts->skel.Mat;
}

/*
==================
XModelNumBones
==================
*/
int XModelNumBones( const XModel *model )
{
	assert(model);
	return model->parts->numBones;
}

/*
==================
XModelGetNumLods
==================
*/
int XModelGetNumLods( const XModel *model )
{
	assert(model);
	return model->numLods;
}

/*
==================
XModelGetSurfaces
==================
*/
int XModelGetSurfaces( XModel *model, XSurface_s **surfaces, int lod, int **partBits )
{
	assert(model);
	assert(surfaces);
	assert((unsigned)lod < MAX_LODS);

	*surfaces = model->lodInfo[lod].surfs->surfs;
	*partBits = model->lodInfo[lod].surfs->partBits;

	return model->lodInfo[lod].numsurfs;
}

/*
==================
XModelGetFlags
==================
*/
int XModelGetFlags( const XModel *model )
{
	assert(model);
	return model->flags;
}

/*
==================
XModelGetName
==================
*/
const char* XModelGetName( const XModel *model )
{
	assert(model);
	return model->name;
}

/*
==================
XModelGetLodForDist
==================
*/
int XModelGetLodForDist(const XModel *model, float dist )
{
	float lodDist;
	int lodIndex, lodCount;

	assert(!IS_NAN(dist));
	lodCount = XModelGetNumLods(model);

	for ( lodIndex = 0; lodIndex < lodCount; lodIndex++ )
	{
		 if ( g_testLods[lodIndex].enabled )
			lodDist = g_testLods[lodIndex].dist;
		else
			lodDist = model->lodInfo[lodIndex].dist;

		assert(!IS_NAN(lodDist));

		if ( lodDist == 0.0f || lodDist > dist )
		{
			return lodIndex;
		}
	}

	return -1;
}

/*
==================
XModelGetLodOutDist
==================
*/
float XModelGetLodOutDist( const XModel *model )
{
	int lodMax = XModelGetNumLods(model) - 1;

	if ( g_testLods[lodMax].enabled )
		return g_testLods[lodMax].dist;
	else
		return model->lodInfo[lodMax].dist;
}

/*
==================
XModelGetSurfaceName
==================
*/
const char* XModelGetSurfaceName( const XModel *model, int subMatIndex, int lod )
{
	unsigned short name = model->lodInfo[lod].surfNames[subMatIndex];

	if ( name )
	{
		return SL_ConvertToString(name);
	}

	return "DEFAULT";
}
