#include "qcommon.h"
#include "cm_local.h"

static void CM_InitStaticModel(cStaticModel_s *staticModel, const vec3_t origin, const vec3_t angles, const vec3_t scale)
{
	float axis[3][3];

	VectorCopy(origin, staticModel->origin);
	AnglesToAxis(angles, axis);

	VectorScale(axis[0], scale[0], axis[0]);
	VectorScale(axis[1], scale[1], axis[1]);
	VectorScale(axis[2], scale[2], axis[2]);

	MatrixInverse(axis, staticModel->invScaledAxis);

	if ( XModelGetStaticBounds(staticModel->xmodel, axis, staticModel->absmin, staticModel->absmax) )
	{
		VectorAdd(staticModel->absmin, origin, staticModel->absmin);
		VectorAdd(staticModel->absmax, origin, staticModel->absmax);
	}
}

static bool CM_CreateStaticModel(cStaticModel_s *staticModel, const char *name, const vec3_t origin, const vec3_t angles, const vec3_t scale)
{
	XModel *model;

	if ( !name || !name[0] )
		Com_Error(ERR_DROP, "Invalid static model name");

	if ( scale[0] == 0.0 )
		Com_Error(ERR_DROP, "Static model [%s] has x scale of 0.0", name);

	if ( scale[1] == 0.0 )
		Com_Error(ERR_DROP, "Static model [%s] has y scale of 0.0", name);

	if ( scale[2] == 0.0 )
		Com_Error(ERR_DROP, "Static model [%s] has z scale of 0.0", name);

	model = CM_XModelPrecache(name);

	if ( !model )
		return 0;

	staticModel->xmodel = model;
	CM_InitStaticModel(staticModel, origin, angles, scale);

	return 1;
}

void CM_LoadStaticModels()
{
	int count;
	qboolean bMiscModel;
	vec3_t scale;
	vec3_t angles;
	vec3_t origin;
	char value[MAX_QPATH];
	char key[MAX_QPATH];
	char modelName[MAX_QPATH];
	const char *ptr;
	const char *token;

	ptr = cm.entityString;

	cm.numStaticModels = 0;
	cm.staticModelList = 0;

	while ( 1 )
	{
		token = Com_Parse(&ptr);

		if ( !ptr || token[0] != '{' )
			break;

		modelName[0] = 0;
		bMiscModel = 0;

		while ( 1 )
		{
			token = Com_Parse(&ptr);

			if ( !ptr )
				break;

			if ( token[0] == '}' )
				break;

			strcpy(key, token);
			token = Com_Parse(&ptr);

			if ( !ptr )
				break;

			strcpy(value, token);

			if ( !strcasecmp(key, "classname") )
			{
				if ( !strcasecmp(value, "misc_model") )
					bMiscModel = 1;
			}
			else if ( !strcasecmp(key, "model") )
			{
				strcpy(modelName, value);
			}
		}

		if ( bMiscModel && Com_ValidXModelName(modelName) )
			++cm.numStaticModels;
	}

	if ( cm.numStaticModels )
	{
		cm.staticModelList = (cStaticModel_s *)CM_Hunk_Alloc(sizeof(cStaticModel_s) * cm.numStaticModels);
		ptr = cm.entityString;
		count = 0;

		while ( 1 )
		{
			token = Com_Parse(&ptr);

			if ( !ptr || token[0] != '{' )
				break;

			modelName[0] = 0;

			memset(origin, 0, sizeof(origin));
			memset(angles, 0, sizeof(angles));

			scale[2] = 1.0;
			scale[1] = 1.0;
			scale[0] = 1.0;

			bMiscModel = 0;

			while ( 1 )
			{
				token = Com_Parse(&ptr);

				if ( !ptr )
					break;

				if ( token[0] == '}' )
					break;

				strcpy(key, token);
				token = Com_Parse(&ptr);

				if ( !ptr )
					break;

				strcpy(value, token);

				if ( !strcasecmp(key, "classname") )
				{
					if ( !strcasecmp(value, "misc_model") )
						bMiscModel = 1;
				}
				else if ( !strcasecmp(key, "model") )
				{
					strcpy(modelName, value);
				}
				else if ( !strcasecmp(key, "origin") )
				{
					sscanf(value, "%f %f %f", &origin[0], &origin[1], &origin[2]);
				}
				else if ( !strcasecmp(key, "angles") )
				{
					sscanf(value, "%f %f %f", &angles[0], &angles[1], &angles[2]);
				}
				else if ( !strcasecmp(key, "modelscale_vec") )
				{
					sscanf(value, "%f %f %f", &scale[0], &scale[1], &scale[2]);
				}
				else if ( !strcasecmp(key, "modelscale") )
				{
					scale[2] = atof(value);
					scale[1] = scale[2];
					scale[0] = scale[2];
				}
			}

			if ( bMiscModel && Com_ValidXModelName(modelName) )
			{
				if ( CM_CreateStaticModel(&cm.staticModelList[count], &modelName[7], origin, angles, scale) )
					++count;
				else
					--cm.numStaticModels;
			}
		}
	}
}