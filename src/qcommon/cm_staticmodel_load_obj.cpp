#include "qcommon.h"
#include "cm_local.h"

void CM_InitStaticModel(cStaticModel_s *model, const float *origin, const float *angles, const float *modelscale_vec)
{
	vec3_t axis[3];

	VectorCopy(origin, model->origin);
	AnglesToAxis(angles, axis);

	VectorScale(axis[0], modelscale_vec[0], axis[0]);
	VectorScale(axis[1], modelscale_vec[1], axis[1]);
	VectorScale(axis[2], modelscale_vec[2], axis[2]);

	MatrixInverse(axis, model->invScaledAxis);

	if ( XModelGetStaticBounds(model->xmodel, axis, model->absmin, model->absmax) )
	{
		VectorAdd(model->absmin, origin, model->absmin);
		VectorAdd(model->absmax, origin, model->absmax);
	}
}

bool CM_CreateStaticModel(cStaticModel_s *model, const char *modelName, const float *origin, const float *angles, const float *modelscale_vec)
{
	XModel *xmodel;

	if ( !modelName || !*modelName )
		Com_Error(ERR_DROP, "Invalid static model name");

	if ( modelscale_vec[0] == 0.0 )
		Com_Error(ERR_DROP, "Static model [%s] has x scale of 0.0", modelName);

	if ( modelscale_vec[1] == 0.0 )
		Com_Error(ERR_DROP, "Static model [%s] has y scale of 0.0", modelName);

	if ( modelscale_vec[2] == 0.0 )
		Com_Error(ERR_DROP, "Static model [%s] has z scale of 0.0", modelName);

	xmodel = CM_XModelPrecache(modelName);

	if ( !xmodel )
		return false;

	model->xmodel = xmodel;
	CM_InitStaticModel(model, origin, angles, modelscale_vec);

	return true;
}

void CM_LoadStaticModels()
{
	int i;
	qboolean isMiscModel;
	vec3_t modelscale_vec;
	vec3_t angles;
	vec3_t origin;
	char string[MAX_QPATH];
	char buffer[MAX_QPATH];
	char xmodel[MAX_QPATH];
	const char *entitystring;
	char *parseData;

	entitystring = cm.entityString;

	cm.numStaticModels = 0;
	cm.staticModelList = 0;

	while ( 1 )
	{
		parseData = Com_Parse(&entitystring);

		if ( !entitystring || *parseData != 123 )
			break;

		xmodel[0] = 0;
		isMiscModel = 0;

		while ( 1 )
		{
			parseData = Com_Parse(&entitystring);

			if ( !entitystring )
				break;

			if ( *parseData == 125 )
				break;

			strcpy(buffer, parseData);
			parseData = Com_Parse(&entitystring);

			if ( !entitystring )
				break;

			strcpy(string, parseData);

			if ( !strcasecmp(buffer, "classname") )
			{
				if ( !strcasecmp(string, "misc_model") )
					isMiscModel = 1;
			}
			else if ( !strcasecmp(buffer, "model") )
			{
				strcpy(xmodel, string);
			}
		}

		if ( isMiscModel && Com_ValidXModelName(xmodel) )
			++cm.numStaticModels;
	}

	if ( cm.numStaticModels )
	{
		cm.staticModelList = (cStaticModel_s *)CM_Hunk_Alloc(sizeof(cStaticModel_s) * cm.numStaticModels);
		entitystring = cm.entityString;
		i = 0;

		while ( 1 )
		{
			parseData = Com_Parse(&entitystring);

			if ( !entitystring || *parseData != 123 )
				break;

			xmodel[0] = 0;
			memset(origin, 0, sizeof(origin));
			memset(angles, 0, sizeof(angles));
			modelscale_vec[2] = 1.0;
			modelscale_vec[1] = 1.0;
			modelscale_vec[0] = 1.0;
			isMiscModel = 0;

			while ( 1 )
			{
				parseData = Com_Parse(&entitystring);

				if ( !entitystring )
					break;

				if ( *parseData == 125 )
					break;

				strcpy(buffer, parseData);
				parseData = Com_Parse(&entitystring);

				if ( !entitystring )
					break;

				strcpy(string, parseData);

				if ( !strcasecmp(buffer, "classname") )
				{
					if ( !strcasecmp(string, "misc_model") )
						isMiscModel = 1;
				}
				else if ( !strcasecmp(buffer, "model") )
				{
					strcpy(xmodel, string);
				}
				else if ( !strcasecmp(buffer, "origin") )
				{
					sscanf(string, "%f %f %f", origin, &origin[1], &origin[2]);
				}
				else if ( !strcasecmp(buffer, "angles") )
				{
					sscanf(string, "%f %f %f", angles, &angles[1], &angles[2]);
				}
				else if ( !strcasecmp(buffer, "modelscale_vec") )
				{
					sscanf(string, "%f %f %f", modelscale_vec, &modelscale_vec[1], &modelscale_vec[2]);
				}
				else if ( !strcasecmp(buffer, "modelscale") )
				{
					modelscale_vec[2] = atof(string);
					modelscale_vec[1] = modelscale_vec[2];
					modelscale_vec[0] = modelscale_vec[2];
				}
			}

			if ( isMiscModel && Com_ValidXModelName(xmodel) )
			{
				if ( CM_CreateStaticModel(&cm.staticModelList[i], &xmodel[7], origin, angles, modelscale_vec) )
					++i;
				else
					--cm.numStaticModels;
			}
		}
	}
}