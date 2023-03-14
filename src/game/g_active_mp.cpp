#include "../qcommon/qcommon.h"
#include "g_shared.h"

#ifdef TESTING_LIBRARY
#define level_bgs (*((bgs_t*)( 0x0859EA40 )))
#else
extern bgs_t level_bgs;
#endif

qboolean G_UpdateClientInfo(gentity_s *ent)
{
	gclient_s *client;
	qboolean updated = 0;
	int i;
	const char *tagName;
	const char *modelName;
	clientInfo_t *info;

	client = ent->client;
	info = &level_bgs.clientinfo[ent->s.clientNum];
	modelName = G_ModelName(ent->model);
	client->sess.state.modelindex = ent->model;

	if ( strcmp(info->model, modelName) )
	{
		updated = 1;
		I_strncpyz(info->model, modelName, 64);
	}

	for ( i = 0; i < 6; ++i )
	{
		if ( ent->attachModelNames[i] )
		{
			modelName = G_ModelName(ent->attachModelNames[i]);
			client->sess.state.attachModelIndex[i] = ent->attachModelNames[i];

			if ( strcmp(info->attachModelNames[i], modelName) )
			{
				updated = 1;
				I_strncpyz(info->attachModelNames[i], modelName, 64);
			}

			tagName = SL_ConvertToString(ent->attachTagNames[i]);
			client->sess.state.attachTagIndex[i] = G_TagIndex(tagName);

			if ( strcmp(info->attachTagNames[i], tagName) )
			{
				updated = 1;
				I_strncpyz(info->attachTagNames[i], tagName, 64);
			}
		}
		else
		{
			info->attachModelNames[i][0] = 0;
			info->attachTagNames[i][0] = 0;
			client->sess.state.attachModelIndex[i] = 0;
			client->sess.state.attachTagIndex[i] = 0;
		}
	}

	return updated;
}

void G_UpdatePlayerContents(gentity_s *ent)
{
	if ( ent->client->noclip )
	{
		ent->r.contents = 0;
	}
	else if ( ent->client->ufo )
	{
		ent->r.contents = 0;
	}
	else if ( ent->client->sess.sessionState == STATE_DEAD )
	{
		ent->r.contents = 0;
	}
	else
	{
		ent->r.contents = 0x2000000;
	}
}