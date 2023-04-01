#include "../qcommon/qcommon.h"

#ifdef TESTING_LIBRARY
#define svs (*((serverStatic_t*)( 0x0841FB00 )))
#define sv (*((server_t*)( 0x0842BC80 )))
#else
extern server_t sv;
extern serverStatic_t svs;
#endif

/*
===============
SV_AddEntToSnapshot
===============
*/
void SV_AddEntToSnapshot(  int number, snapshotEntityNumbers_t *eNums )
{
	// if we are full, silently discard entities
	if ( eNums->numSnapshotEntities == MAX_SNAPSHOT_ENTITIES )
	{
		return;
	}

	eNums->snapshotEntities[ eNums->numSnapshotEntities ] = number;
	eNums->numSnapshotEntities++;
}

/*
===============
SV_AddEntitiesVisibleFromPoint
===============
*/
void SV_AddEntitiesVisibleFromPoint( float *origin, int clientNum, snapshotEntityNumbers_t *eNums )
{
	int e, i;
	gentity_t *ent;
	svEntity_t  *svEnt;
	int l;
	int clientcluster;
	int leafnum;
	byte    *clientpvs;
	byte    *bitvector;
	float fogOpaqueDistSqrd;

	leafnum = CM_PointLeafnum( origin );
	clientcluster = CM_LeafCluster( leafnum );

	if(clientcluster < 0)
	{
		return;
	}

	clientpvs = CM_ClusterPVS( clientcluster );

	fogOpaqueDistSqrd = G_GetFogOpaqueDistSqrd();

	if ( fogOpaqueDistSqrd == 3.4028235e38 )
	{
		fogOpaqueDistSqrd = 0.0;
	}

	for ( e = 0 ; e < sv.num_entities ; e++ )
	{
		ent = SV_GentityNum( e );

		// never send entities that aren't linked in.
		// never send client's own entity, because it can
		// be regenerated from the playerstate
		if ( !ent->r.linked || e == clientNum)
		{
			continue;
		}


		if ( !ent->r.broadcastTime )
		{
			if ( ent->r.svFlags & 1 || (1 << (clientNum & 31)) & ent->r.clientMask[clientNum >> 5] )
			{
				continue;
			}

		}
		else
		{
			if(ent->r.broadcastTime < 0 || ent->r.broadcastTime >= svs.time)
			{
				SV_AddEntToSnapshot( e, eNums );
				continue;
			}
			ent->r.broadcastTime = 0;
		}


		if ( ent->r.svFlags & 0x18 )
		{
			SV_AddEntToSnapshot( e, eNums );
			continue;
		}

		svEnt = SV_SvEntityForGentity( ent );

		bitvector = clientpvs;

		// check individual leafs
		if ( !svEnt->numClusters )
		{
			//goto notVisible; Different in CoD
			SV_AddEntToSnapshot( e, eNums );
			continue;
		}

		l = 0;
		for ( i = 0 ; i < svEnt->numClusters ; i++ )
		{
			l = svEnt->clusternums[i];
			if ( bitvector[l >> 3] & ( 1 << ( l & 7 ) ) )
			{
				break;
			}
		}

		// if we haven't found it to be visible,
		// check overflow clusters that coudln't be stored
		if ( i == svEnt->numClusters )
		{
			if ( svEnt->lastCluster )
			{
				for ( ; l <= svEnt->lastCluster ; l++ )
				{
					if ( bitvector[l >> 3] & ( 1 << ( l & 7 ) ) )
					{
						break;
					}
				}
				if ( l == svEnt->lastCluster )
				{
					continue;    // not visible
				}
			}
			else
			{
				continue;
			}
		}

		if ( fogOpaqueDistSqrd != 0.0)
		{
			if(BoxDistSqrdExceeds(ent->r.absmin, ent->r.absmax, origin, fogOpaqueDistSqrd))
			{
				continue;
			}
		}
		// add it
		SV_AddEntToSnapshot( e, eNums );
	}
}

/*
===============
SV_AddCachedEntitiesVisibleFromPoint
===============
*/
void SV_AddCachedEntitiesVisibleFromPoint(int from_num_entities, int from_first_entity, float *origin, signed int clientNum, snapshotEntityNumbers_t *eNums)
{
	int e, i, l;
	int clientcluster;
	int leafnum, boxleafnums;
	byte    *clientpvs;
	byte    *bitvector;
	float fogOpaqueDistSqrd;
	int clusternums[128];
	int lastLeaf;
	archivedEntity_t *aent;

	leafnum = CM_PointLeafnum( origin );
	clientcluster = CM_LeafCluster( leafnum );

	if(clientcluster < 0)
	{
		return;
	}

	clientpvs = CM_ClusterPVS( clientcluster );

	fogOpaqueDistSqrd = G_GetFogOpaqueDistSqrd();

	if ( fogOpaqueDistSqrd == 3.4028235e38 )
	{
		fogOpaqueDistSqrd = 0.0;
	}

	int maxCachedSnapshotEntities = 0x4000; // sizeof(svs.cachedSnapshotEntities) / sizeof(svs.cachedSnapshotEntities[0]); NOT for CoD2, CoD2 uses malloc for that

	for ( e = 0 ; e < from_num_entities ; e++ )
	{

		aent = &svs.cachedSnapshotEntities[(e + from_first_entity) % maxCachedSnapshotEntities];
		if ( (1 << (clientNum & 31)) & aent->r.clientMask[clientNum >> 5] || aent->s.number == clientNum )
		{
			continue;
		}

		if ( aent->r.svFlags & 0x18 )
		{
			SV_AddEntToSnapshot( e, eNums );
			continue;
		}

		boxleafnums = CM_BoxLeafnums(aent->r.absmin, aent->r.absmax, clusternums, sizeof(clusternums) / sizeof(clusternums[0]), &lastLeaf);
		if ( !boxleafnums )
		{
			continue;
		}

		bitvector = clientpvs;

		for ( i = 0 ; i < boxleafnums ; i++ )
		{
			l = CM_LeafCluster(clusternums[i]);
			if ( bitvector[l >> 3] & ( 1 << ( l & 7 ) ) && l != -1)
			{
				break;
			}
		}

		if ( i == boxleafnums)
		{
			continue;
		}

		if(!(fogOpaqueDistSqrd == 0.0 || BoxDistSqrdExceeds(aent->r.absmin, aent->r.absmax, origin, fogOpaqueDistSqrd) == qfalse) )
		{
			continue;
		}

		// add it
		SV_AddEntToSnapshot( e, eNums );
	}
}

cachedSnapshot_t* SV_GetCachedSnapshotInternal(int archivedFrame)
{
	int startIndex;
	archivedSnapshot_t *frame;
	signed int oldArchivedFrame;
	int newnum;
	int oldindex;
	signed int oldnum;
	msg_t msg;
	byte buf[MAX_MSGLEN];
	int partSize;
	int firstCachedSnapshotFrame;
	int i;
	cachedSnapshot_t *cachedFrame;
	cachedSnapshot_t *oldCachedFrame;
	cachedClient_t *cachedClient;
	cachedClient_t *oldCachedClient;
	archivedEntity_t *cachedEntity;

	frame = &svs.archivedSnapshotFrames[archivedFrame % 1200];

	if ( frame->start < svs.nextArchivedSnapshotBuffer - 0x2000000 )
	{
		return NULL;
	}

	firstCachedSnapshotFrame = svs.nextCachedSnapshotFrames - 512;

	if ( svs.nextCachedSnapshotFrames - 512 < 0 )
	{
		firstCachedSnapshotFrame = 0;
	}

	for ( i = svs.nextCachedSnapshotFrames - 1; i >= firstCachedSnapshotFrame; --i )
	{
		cachedFrame = &svs.cachedSnapshotFrames[i % 512];

		if ( cachedFrame->archivedFrame == archivedFrame )
		{
			if (
			    cachedFrame->first_entity >= svs.nextCachedSnapshotEntities - 0x4000
			    && cachedFrame->first_client >= svs.nextCachedSnapshotClients - 4096 )
			{
				return cachedFrame;
			}

			break;
		}
	}

	MSG_Init(&msg, buf, sizeof(buf));
	msg.cursize = frame->size;

	startIndex = frame->start % 0x2000000;
	partSize = 0x2000000 - startIndex;

	if ( msg.cursize > 0x2000000 - startIndex )
	{
		memcpy(msg.data, &svs.archivedSnapshotBuffer[startIndex], partSize);
		memcpy(&msg.data[partSize], svs.archivedSnapshotBuffer, msg.cursize - partSize);
	}
	else
	{
		memcpy(msg.data, &svs.archivedSnapshotBuffer[startIndex], msg.cursize);
	}

	if ( MSG_ReadBit(&msg) )
	{
		cachedFrame = &svs.cachedSnapshotFrames[svs.nextCachedSnapshotFrames % 512];
		cachedFrame->archivedFrame = archivedFrame;
		cachedFrame->num_entities = 0;
		cachedFrame->first_entity = svs.nextCachedSnapshotEntities;
		cachedFrame->num_clients = 0;
		cachedFrame->first_client = svs.nextCachedSnapshotClients;
		cachedFrame->usesDelta = 0;
		cachedFrame->time = MSG_ReadLong(&msg);

		while ( MSG_ReadBit(&msg) )
		{
			newnum = MSG_ReadBits(&msg, 6);

			if ( msg.readcount > msg.cursize )
			{
				Com_Error(ERR_DROP, "SV_GetCachedSnapshot: end of message");
			}

			cachedClient = &svs.cachedSnapshotClients[svs.nextCachedSnapshotClients % 4096];
			MSG_ReadDeltaClient(&msg, 0, &cachedClient->cs, newnum);
			cachedClient->playerStateExists = MSG_ReadBit(&msg);

			if ( cachedClient->playerStateExists )
			{
				MSG_ReadDeltaPlayerstate(&msg, 0, &cachedClient->ps);
			}

			if ( ++svs.nextCachedSnapshotClients >= 0x7FFFFFFE )
			{
				Com_Error(ERR_FATAL, "svs.nextCachedSnapshotClients wrapped");
			}

			++cachedFrame->num_clients;
		}

		while ( 1 )
		{
			newnum = MSG_ReadBits(&msg, 10);

			if ( newnum == 1023 )
			{
				break;
			}

			if ( msg.readcount > msg.cursize )
			{
				Com_Error(ERR_DROP, "SV_GetCachedSnapshot: end of message");
			}

			cachedEntity = &svs.cachedSnapshotEntities[svs.nextCachedSnapshotEntities % 0x4000];
			MSG_ReadDeltaArchivedEntity( &msg, &sv.svEntities[newnum].baseline, cachedEntity, newnum);

			if ( ++svs.nextCachedSnapshotEntities >= 0x7FFFFFFE )
			{
				Com_Error(ERR_FATAL, "svs.nextCachedSnapshotEntities wrapped");
			}

			++cachedFrame->num_entities;
		}

		if ( ++svs.nextCachedSnapshotFrames >= 0x7FFFFFFE )
		{
			Com_Error(ERR_FATAL, "svs.nextCachedSnapshotFrames wrapped");
		}
	}
	else
	{
		oldArchivedFrame = MSG_ReadLong(&msg);

		if ( oldArchivedFrame < svs.nextArchivedSnapshotFrames - 1200 )
		{
			return NULL;
		}

		frame = &svs.archivedSnapshotFrames[oldArchivedFrame % 1200];

		if ( frame->start < svs.nextArchivedSnapshotBuffer - 0x2000000 )
		{
			return NULL;
		}

		oldCachedFrame = SV_GetCachedSnapshotInternal(oldArchivedFrame);

		if ( !oldCachedFrame )
		{
			return NULL;
		}

		cachedFrame = &svs.cachedSnapshotFrames[svs.nextCachedSnapshotFrames % 512];
		cachedFrame->archivedFrame = archivedFrame;
		cachedFrame->num_entities = 0;
		cachedFrame->first_entity = svs.nextCachedSnapshotEntities;
		cachedFrame->num_clients = 0;
		cachedFrame->first_client = svs.nextCachedSnapshotClients;
		cachedFrame->usesDelta = 1;
		cachedFrame->time = MSG_ReadLong(&msg);

		oldindex = 0;
		oldCachedClient = 0;

		if ( oldCachedFrame->num_clients > 0 )
		{
			oldCachedClient = &svs.cachedSnapshotClients[(oldindex + oldCachedFrame->first_client) % 4096];
			oldnum = oldCachedClient->cs.clientIndex;
		}
		else
		{
			oldnum = 99999;
		}

		while ( MSG_ReadBit(&msg) )
		{
			newnum = MSG_ReadBits(&msg, 6);

			if ( msg.readcount > msg.cursize )
			{
				Com_Error(ERR_DROP, "SV_GetCachedSnapshot: end of message");
			}

			while ( oldnum < newnum )
			{
				if ( ++oldindex < oldCachedFrame->num_clients )
				{
					oldCachedClient = &svs.cachedSnapshotClients[(oldindex + oldCachedFrame->first_client) % 4096];
					oldnum = oldCachedClient->cs.clientIndex;
				}
				else
				{
					oldnum = 99999;
				}
			}

			if ( oldnum == newnum )
			{
				cachedClient = &svs.cachedSnapshotClients[svs.nextCachedSnapshotClients % 4096];
				MSG_ReadDeltaClient(&msg, &oldCachedClient->cs, &cachedClient->cs, newnum);
				cachedClient->playerStateExists = MSG_ReadBit(&msg);

				if ( cachedClient->playerStateExists )
				{
					MSG_ReadDeltaPlayerstate(&msg, &oldCachedClient->ps, &cachedClient->ps);
				}

				if ( ++svs.nextCachedSnapshotClients >= 0x7FFFFFFE )
				{
					Com_Error(ERR_FATAL, "svs.nextCachedSnapshotClients wrapped");
				}

				++cachedFrame->num_clients;

				if ( ++oldindex < oldCachedFrame->num_clients )
				{
					oldCachedClient = &svs.cachedSnapshotClients[(oldindex + oldCachedFrame->first_client) % 4096];
					oldnum = oldCachedClient->cs.clientIndex;
				}
				else
				{
					oldnum = 99999;
				}
			}
			else
			{
				cachedClient = &svs.cachedSnapshotClients[svs.nextCachedSnapshotClients % 4096];
				MSG_ReadDeltaClient(&msg, 0, &cachedClient->cs, newnum);
				cachedClient->playerStateExists = MSG_ReadBit(&msg);

				if ( cachedClient->playerStateExists )
				{
					MSG_ReadDeltaPlayerstate(&msg, 0, &cachedClient->ps);
				}

				if ( ++svs.nextCachedSnapshotClients >= 0x7FFFFFFE )
				{
					Com_Error(ERR_FATAL, "svs.nextCachedSnapshotClients wrapped");
				}

				++cachedFrame->num_clients;
			}
		}

		while ( 1 )
		{
			newnum = MSG_ReadBits(&msg, 10);

			if ( newnum == 1023 )
			{
				break;
			}

			if ( msg.readcount > msg.cursize )
			{
				Com_Error(ERR_DROP, "SV_GetCachedSnapshot: end of message");
			}

			cachedEntity = &svs.cachedSnapshotEntities[svs.nextCachedSnapshotEntities % 0x4000];
			MSG_ReadDeltaArchivedEntity(&msg, &sv.svEntities[newnum].baseline, cachedEntity, newnum);

			if ( ++svs.nextCachedSnapshotEntities >= 0x7FFFFFFE )
			{
				Com_Error(ERR_FATAL, "svs.nextCachedSnapshotEntities wrapped");
			}

			++cachedFrame->num_entities;
		}

		if ( ++svs.nextCachedSnapshotFrames >= 0x7FFFFFFE )
		{
			Com_Error(ERR_FATAL, "svs.nextCachedSnapshotFrames wrapped");
		}
	}

	return cachedFrame;
}

extern dvar_t *sv_fps;
cachedSnapshot_t* SV_GetCachedSnapshot(int *pArchiveTime)
{
	cachedSnapshot_s *cachedSnap;
	int archivedFrame;

	if ( !svs.archivedSnapshotEnabled )
		return 0;

	if ( *pArchiveTime <= 0 )
		return 0;

	archivedFrame = svs.nextArchivedSnapshotFrames - sv_fps->current.integer * *pArchiveTime / 1000;

	if ( archivedFrame < svs.nextArchivedSnapshotFrames - 1200 )
	{
		archivedFrame = svs.nextArchivedSnapshotFrames - 1200;
		*pArchiveTime = 1000
		                * (svs.nextArchivedSnapshotFrames - (svs.nextArchivedSnapshotFrames - 1200))
		                / sv_fps->current.integer;
	}

	if ( archivedFrame < 0 )
	{
		archivedFrame = 0;
		*pArchiveTime = 1000 * svs.nextArchivedSnapshotFrames / sv_fps->current.integer;
	}

	while ( archivedFrame < svs.nextArchivedSnapshotFrames )
	{
		cachedSnap = SV_GetCachedSnapshotInternal(archivedFrame);

		if ( cachedSnap )
			return cachedSnap;

		++archivedFrame;
	}

	*pArchiveTime = 0;
	return 0;
}

void SV_BuildClientSnapshot( client_t *client )
{
	vec3_t org;
	clientSnapshot_t            *frame;
	snapshotEntityNumbers_t entityNumbers;
	int i;
	archivedEntity_t			*aent;
	gentity_t              		*ent;
	entityState_t               *entState;
	cachedClient_t				*cachedClient;
	clientState_t               *clientState;
	gentity_t					*clent;
	int clientNum;
	playerState_t               *ps;
	int							archiveTime;
	cachedSnapshot_t			*cachedSnap;
	client_t					*snapClient;

	// this is the frame we are creating
	frame = &client->frames[ client->netchan.outgoingSequence & PACKET_MASK ];

	// clear everything in this snapshot
	entityNumbers.numSnapshotEntities = 0;

	// show_bug.cgi?id=62
	frame->num_entities = 0;
	frame->num_clients = 0;

	clent = client->gentity;

	if ( !clent || client->state == CS_ZOMBIE || sv.state < SS_GAME )
	{
		return;
	}

	frame->first_entity = svs.nextSnapshotEntities;
	frame->first_client = svs.nextSnapshotClients;

	clientNum = client - svs.clients;
	archiveTime = G_GetClientArchiveTime(clientNum);
	cachedSnap = SV_GetCachedSnapshot(&archiveTime);
	G_SetClientArchiveTime(clientNum, archiveTime);

	// grab the current playerState_t
	ps = SV_GameClientNum( clientNum );

	frame->ps = *ps;

	//Update client num from other source here
	clientNum = frame->ps.clientNum;

	if ( clientNum < 0 || clientNum >= MAX_GENTITIES )
	{
		Com_Error( ERR_DROP, "SV_BuildClientSnapshot(): bad gEnt" );
	}

	// find the client's viewpoint
	VectorCopy( ps->origin, org );
	org[2] += ps->viewHeightCurrent;

//----(SA)	added for 'lean'
	// need to account for lean, so areaportal doors draw properly
	AddLeanToPosition(org, ps->viewangles[1], ps->leanf, 16.0, 20.0);
//----(SA)	end

	if(cachedSnap)
	{
		int snapTime = svs.time - cachedSnap->time;

		SV_AddCachedEntitiesVisibleFromPoint(cachedSnap->num_entities, cachedSnap->first_entity, org, clientNum, &entityNumbers);

		for ( i = 0 ; i < entityNumbers.numSnapshotEntities; ++i )
		{
			entState = &svs.snapshotEntities[svs.nextSnapshotEntities % svs.numSnapshotEntities];
			aent = &svs.cachedSnapshotEntities[(cachedSnap->first_entity + entityNumbers.snapshotEntities[i]) % 0x4000];

			*entState = aent->s;

			if ( entState->pos.trTime )
			{
				entState->pos.trTime += snapTime;
			}
			if ( entState->apos.trTime )
			{
				entState->apos.trTime += snapTime;
			}
			if ( entState->time2 )
			{
				entState->time2 += snapTime;
			}

			svs.nextSnapshotEntities++;
			// this should never hit, map should always be restarted first in SV_Frame
			if ( svs.nextSnapshotEntities >= 0x7FFFFFFE )
			{
				Com_Error( ERR_FATAL, "svs.nextSnapshotEntities wrapped" );
			}

			frame->num_entities++;
		}

		for ( i = 0; i < cachedSnap->num_clients; ++i )
		{
			cachedClient = &svs.cachedSnapshotClients[(i + cachedSnap->first_client) % 4096];
			clientState = &svs.snapshotClients[svs.nextSnapshotClients % svs.numSnapshotClients];
			*clientState = cachedClient->cs;
			svs.nextSnapshotClients++;

			// this should never hit, map should always be restarted first in SV_Frame
			if ( svs.nextSnapshotClients >= 0x7FFFFFFE )
			{
				Com_Error( ERR_FATAL, "svs.nextSnapshotClients wrapped" );
			}
			frame->num_clients++;
		}
		return;
	}

	// add all the entities directly visible to the eye, which
	// may include portal entities that merge other viewpoints
	SV_AddEntitiesVisibleFromPoint( org, clientNum, &entityNumbers );

	// copy the entity states out
	for ( i = 0 ; i < entityNumbers.numSnapshotEntities ; i++ )
	{
		ent = SV_GentityNum( entityNumbers.snapshotEntities[i] );
		entState = &svs.snapshotEntities[svs.nextSnapshotEntities % svs.numSnapshotEntities];
		*entState = ent->s;
		svs.nextSnapshotEntities++;
		// this should never hit, map should always be restarted first in SV_Frame
		if ( svs.nextSnapshotEntities >= 0x7FFFFFFE )
		{
			Com_Error( ERR_FATAL, "svs.nextSnapshotEntities wrapped" );
		}
		frame->num_entities++;
	}

	// copy the client states out
	for ( snapClient = svs.clients, i = 0 ; i < sv_maxclients->current.integer ; i++, snapClient++ )
	{
		if(snapClient->state < CS_PRIMED)
		{
			continue;
		}

		clientState = &svs.snapshotClients[svs.nextSnapshotClients % svs.numSnapshotClients];
		memcpy(clientState, G_GetClientState(i), sizeof(clientState_t));

		if ( clientState->clientIndex != i )
		{
			continue;
		}

		svs.nextSnapshotClients++;
		// this should never hit, map should always be restarted first in SV_Frame
		if ( svs.nextSnapshotClients >= 0x7FFFFFFE )
		{
			Com_Error( ERR_FATAL, "svs.nextSnapshotClients wrapped" );
		}
		frame->num_clients++;
	}
}

int SV_GetCurrentClientInfo(int clientNum, playerState_s *ps, clientState_t *cs)
{
	if ( svs.clients[clientNum].state != CS_ACTIVE )
		return 0;

	if ( !GetFollowPlayerState(clientNum, ps) )
		return 0;

	memcpy(cs, G_GetClientState(clientNum), sizeof(clientState_t));
	return 1;
}

int SV_GetArchivedClientInfo(int clientNum, int *pArchiveTime, playerState_t *ps, clientState_t *cs)
{
	cachedSnapshot_t *cachedSnapshot;
	unsigned int i;
	int offsettime;
	cachedClient_t *cachedClient;

	cachedSnapshot = SV_GetCachedSnapshot(pArchiveTime);

	if ( cachedSnapshot )
	{
		offsettime = svs.time - cachedSnapshot->time;
		cachedClient = 0;

		for ( i = 0; ; ++i )
		{
			if ( i >= cachedSnapshot->num_clients )
			{
				return 0;
			}

			cachedClient = &svs.cachedSnapshotClients[(i + cachedSnapshot->first_client) % 4096];

			if ( cachedClient->cs.clientIndex == clientNum )
			{
				break;
			}
		}

		if ( !cachedClient->playerStateExists )
		{
			return 0;
		}

		if ( cs )
		{
			Com_Memcpy(cs, &cachedClient->cs, sizeof(clientState_t));
		}

		if ( ps )
		{
			Com_Memcpy(ps, &cachedClient->ps, sizeof(playerState_t));

			if ( ps->commandTime )
			{
				ps->commandTime += offsettime;
			}

			if ( ps->pm_time )
			{
				ps->pm_time += offsettime;
			}

			if ( ps->foliageSoundTime )
			{
				ps->foliageSoundTime += offsettime;
			}

			if ( ps->jumpTime )
			{
				ps->jumpTime += offsettime;
			}

			if ( ps->viewHeightLerpTime )
			{
				ps->viewHeightLerpTime += offsettime;
			}

			if ( ps->shellshockTime )
			{
				ps->shellshockTime += offsettime;
			}

			for ( i = 0; i < MAX_HUDELEMS_ARCHIVAL && ps->hud.archival[i].type; ++i )
			{
				if ( ps->hud.archival[i].time )
				{
					ps->hud.archival[i].time += offsettime;
				}

				if ( ps->hud.archival[i].fadeStartTime )
				{
					ps->hud.archival[i].fadeStartTime += offsettime;
				}

				if ( ps->hud.archival[i].scaleStartTime )
				{
					ps->hud.archival[i].scaleStartTime += offsettime;
				}

				if ( ps->hud.archival[i].moveStartTime )
				{
					ps->hud.archival[i].moveStartTime += offsettime;
				}
			}

			ps->deltaTime += offsettime;
		}

		return 1;
	}
	else if ( *pArchiveTime > 0 )
	{
		return 0;
	}
	else
	{
		return SV_GetCurrentClientInfo(clientNum, ps, cs);
	}
}

bool SV_GetClientPositionsAtTime(int clientNum, int gametime, float *origin)
{
	long double delta;
	int pArchiveTime;
	int rewindDeltaTime;
	int frameTime;
	int i;
	clientState_t cstate;
	playerState_t pstate;
	bool foundEnd;
	bool foundStart;
	vec3_t endPos;
	vec3_t currentPos;
	int endOffset;
	int startOffset;
	float progress;
	int j;

	frameTime = 1000 / sv_fps->current.integer;
	rewindDeltaTime = frameTime * (svs.time / frameTime);
	startOffset = frameTime * (2 + (rewindDeltaTime - gametime) / frameTime);
	endOffset = frameTime * (1 + (rewindDeltaTime - gametime) / frameTime);
	foundStart = 0;
	foundEnd = 0;
	pArchiveTime = startOffset;

	for ( i = 0; i < 10; ++i )
	{
		if ( SV_GetArchivedClientInfo(clientNum, &pArchiveTime, &pstate, &cstate) )
		{
			foundStart = 1;
			startOffset = pArchiveTime;
			memcpy(currentPos, pstate.origin, sizeof(currentPos));
			break;
		}

		pArchiveTime += frameTime;
	}

	pArchiveTime = endOffset;

	for ( i = 0; i < 10; ++i )
	{
		if ( SV_GetArchivedClientInfo(clientNum, &pArchiveTime, &pstate, &cstate) )
		{
			foundEnd = 1;
			endOffset = pArchiveTime;
			memcpy(endPos, pstate.origin, sizeof(endPos));
			break;
		}

		pArchiveTime -= frameTime;
	}

	if ( foundStart && foundEnd )
	{
		progress = (long double)(gametime % frameTime) / (long double)(startOffset - endOffset);
	}
	else if ( foundStart )
	{
		progress = 0.0;
		VectorClear(endPos);
	}
	else
	{
		if ( !foundEnd )
			return 0;

		progress = 1.0;
		VectorClear(currentPos);
	}

	for ( j = 0; j < 3; ++j )
	{
		delta = lerp(currentPos[j], endPos[j], progress);
		origin[j] = delta;
	}

	return 1;
}