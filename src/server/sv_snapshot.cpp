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
	byte buf[MAX_LARGE_MSGLEN]; // VoroN: !!! WARNING !!! In CoD2 1.0 they allocate this to 128k but wouldn't that potentially cause a netchan MAX_MSGLEN error? Changed to MAX_MSGLEN for now. Update: Nope, it SHOULD be 128k, otherwise snapshot overflows.
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

/*
====================
SV_RateMsec
Return the number of msec a given size message is supposed
to take to clear, based on the current rate
TTimo - use sv_maxRate or sv_dl_maxRate depending on regular or downloading client
====================
*/
#define HEADER_RATE_BYTES   48      // include our header, IP header, and some overhead
extern dvar_t *sv_maxRate;
static int SV_RateMsec( client_t *client, int messageSize )
{
	int rate;
	int rateMsec;
	int maxRate;

	// individual messages will never be larger than fragment size
	if ( messageSize > 1500 )
	{
		messageSize = 1500;
	}

	// low watermark for sv_maxRate, never 0 < sv_maxRate < 1000 (0 is no limitation)
	if ( sv_maxRate->current.integer && sv_maxRate->current.integer < 1000 )
	{
		Dvar_SetInt( sv_maxRate, 1000 );
	}

	rate = client->rate;
	maxRate = sv_maxRate->current.integer;

	if ( maxRate )
	{
		if ( maxRate < rate )
		{
			rate = maxRate;
		}
	}

	rateMsec = ( messageSize + HEADER_RATE_BYTES ) * 1000 / rate;
	return rateMsec;
}

/*
=======================
SV_SendMessageToClient
Called by SV_SendClientSnapshot and SV_SendClientGameState
=======================
*/
void SV_SendMessageToClient( msg_t *msg, client_t *client )
{
	byte buf[MAX_MSGLEN];
	int len;
	int rateMsec;

	memcpy(buf, msg->data, 4);
	len = MSG_WriteBitsCompress(msg->data + 4, buf + 4, msg->cursize - 4) + 4;

	if ( client->delayDropMsg )
		SV_DropClient(client, client->delayDropMsg);

	// record information about the message
	client->frames[client->netchan.outgoingSequence & PACKET_MASK].messageSize = len;
	client->frames[client->netchan.outgoingSequence & PACKET_MASK].messageSent = svs.time;
	client->frames[client->netchan.outgoingSequence & PACKET_MASK].messageAcked = -1;

	// send the datagram
	SV_Netchan_Transmit(client, buf, len);

	// set nextSnapshotTime based on rate and requested number of updates
	// local clients get snapshots every frame
	// TTimo - show_bug.cgi?id=491
	// added sv_lanForceRate check
	if ( client->netchan.remoteAddress.type == NA_LOOPBACK || Sys_IsLANAddress( client->netchan.remoteAddress ) )
	{
		client->nextSnapshotTime = svs.time - 1;
		return;
	}

	// normal rate / snapshotMsec calculation
	rateMsec = SV_RateMsec( client, len );

	// TTimo - during a download, ignore the snapshotMsec
	// the update server on steroids, with this disabled and sv_fps 60, the download can reach 30 kb/s
	// on a regular server, we will still top at 20 kb/s because of sv_fps 20
	if ( !*client->downloadName && rateMsec < client->snapshotMsec )
	{
		// never send more packets than this, no matter what the rate is at
		rateMsec = client->snapshotMsec;
		client->rateDelayed = qfalse;
	}
	else
	{
		client->rateDelayed = qtrue;
	}

	client->nextSnapshotTime = svs.time + rateMsec;

	// don't pile up empty snapshots while connecting
	if ( client->state != CS_ACTIVE )
	{
		// a gigantic connection message may have already put the nextSnapshotTime
		// more than a second away, so don't shorten it
		// do shorten if client is downloading
		if ( !*client->downloadName && client->nextSnapshotTime < svs.time + 1000 )
		{
			client->nextSnapshotTime = svs.time + 1000;
		}

		sv.bpsTotalBytes += len;
	}
}

/*
==================
SV_UpdateServerCommandsToClient
(re)send all server commands the client hasn't acknowledged yet
==================
*/
extern dvar_t *sv_debugReliableCmds;
void SV_UpdateServerCommandsToClient( client_t *client, msg_t *msg )
{
	int i;

	if ( client->reliableAcknowledge + 1 < client->reliableSequence && sv_debugReliableCmds->current.boolean )
		Com_Printf("Client %s has the following un-ack'd reliable commands:\n", client->name);

	// write any unacknowledged serverCommands
	for ( i = client->reliableAcknowledge + 1 ; i <= client->reliableSequence ; i++ )
	{
		MSG_WriteByte( msg, svc_serverCommand );
		MSG_WriteLong( msg, i );
		MSG_WriteString( msg, client->reliableCommands[ i & ( MAX_RELIABLE_COMMANDS - 1 ) ].command );

		if ( sv_debugReliableCmds->current.boolean )
			Com_Printf("%i: %s\n", i - client->reliableAcknowledge - 1, client->reliableCommands[ i & ( MAX_RELIABLE_COMMANDS - 1 ) ].command);
	}

	client->reliableSent = client->reliableSequence;
}

void SV_EmitPacketEntities(int clientNum, int oldEntities, int oldFirstEnt, int entities, int firstEnt, msg_t *msg)
{
	int newnum;
	int oldnum;
	int newIndex;
	int oldIndex;
	entityState_t *newent;
	entityState_t *oldent;

	newent = 0;
	oldent = 0;
	newIndex = 0;
	oldIndex = 0;

	while ( newIndex < entities || oldIndex < oldEntities )
	{
		if ( newIndex < entities )
		{
			newent = &svs.snapshotEntities[(firstEnt + newIndex) % svs.numSnapshotEntities];
			newnum = newent->number;
		}
		else
		{
			newnum = 9999;
		}

		if ( oldIndex < oldEntities )
		{
			oldent = &svs.snapshotEntities[(oldFirstEnt + oldIndex) % svs.numSnapshotEntities];
			oldnum = oldent->number;
		}
		else
		{
			oldnum = 9999;
		}

		if ( newnum == oldnum )
		{
			MSG_WriteDeltaEntity(msg, oldent, newent, 0);
			++oldIndex;
			++newIndex;
		}
		else if ( newnum >= oldnum )
		{
			if ( newnum > oldnum )
			{
				MSG_WriteDeltaEntity(msg, oldent, 0, 1);
				++oldIndex;
			}
		}
		else
		{
			MSG_WriteDeltaEntity(msg, &sv.svEntities[newnum].baseline.s, newent, 1);
			++newIndex;
		}
	}

	MSG_WriteBits( msg, ( MAX_GENTITIES - 1 ), GENTITYNUM_BITS );   // end of packetentities
}

void SV_EmitPacketClients(int clientNum, int oldClients, int oldFirstClient, int clients, int firstClient, msg_t *msg)
{
	int newnum;
	int oldnum;
	int newIndex;
	int oldIndex;
	clientState_t *newcl;
	clientState_t *oldcl;

	newcl = 0;
	oldcl = 0;
	newIndex = 0;
	oldIndex = 0;

	while ( newIndex < clients || oldIndex < oldClients )
	{
		if ( newIndex < clients )
		{
			newcl = &svs.snapshotClients[(firstClient + newIndex) % svs.numSnapshotClients];
			newnum = newcl->clientIndex;
		}
		else
		{
			newnum = 9999;
		}

		if ( oldIndex < oldClients )
		{
			oldcl = &svs.snapshotClients[(oldFirstClient + oldIndex) % svs.numSnapshotClients];
			oldnum = oldcl->clientIndex;
		}
		else
		{
			oldnum = 9999;
		}

		if ( newnum == oldnum )
		{
			MSG_WriteDeltaClient(msg, oldcl, newcl, 0);
			++oldIndex;
			++newIndex;
		}
		else if ( newnum >= oldnum )
		{
			if ( newnum > oldnum )
			{
				MSG_WriteDeltaClient(msg, oldcl, 0, 1);
				++oldIndex;
			}
		}
		else
		{
			MSG_WriteDeltaClient(msg, 0, newcl, 1);
			++newIndex;
		}
	}

	MSG_WriteBit0(msg);
}

extern dvar_t *sv_padPackets;
void SV_WriteSnapshotToClient(client_s *client, msg_t *msg)
{
	int oldFirstClient;
	int oldClients;
	int snapFlags;
	int i;
	int lastframe;
	clientSnapshot_t *oldframe;
	clientSnapshot_t *frame;

	frame = &client->frames[client->netchan.outgoingSequence & 0x1F];

	if ( client->deltaMessage > 0 && client->state == CS_ACTIVE )
	{
		if ( client->netchan.outgoingSequence - client->deltaMessage <= 28 )
		{
			oldframe = &client->frames[client->deltaMessage & 0x1F];
			lastframe = client->netchan.outgoingSequence - client->deltaMessage;

			if ( client->frames[client->deltaMessage & 0x1F].first_entity < svs.nextSnapshotEntities - svs.numSnapshotEntities )
			{
				Com_DPrintf("%s: Delta request from out of date entities.\n", client->name);
				oldframe = 0;
				lastframe = 0;
			}
		}
		else
		{
			Com_DPrintf("%s: Delta request from out of date packet.\n", client->name);
			oldframe = 0;
			lastframe = 0;
		}
	}
	else
	{
		oldframe = 0;
		lastframe = 0;
	}

	MSG_WriteByte(msg, svc_snapshot);
	MSG_WriteLong(msg, svs.time);
	MSG_WriteByte(msg, lastframe);
	snapFlags = svs.snapFlagServerBit;

	if ( client->rateDelayed )
		snapFlags = svs.snapFlagServerBit | 1;

	if ( client->state == CS_ACTIVE )
	{
		client->delayed = 1;
	}
	else if ( client->state != CS_ZOMBIE )
	{
		client->delayed = 0;
	}

	if ( !client->delayed )
		snapFlags |= 2u;

	MSG_WriteByte(msg, snapFlags);

	if ( oldframe )
	{
		MSG_WriteDeltaPlayerstate(msg, &oldframe->ps, &frame->ps);
		oldClients = oldframe->num_clients;
		oldFirstClient = oldframe->first_client;
		SV_EmitPacketEntities(client - svs.clients, oldframe->num_entities, oldframe->first_entity, frame->num_entities, frame->first_entity, msg);
	}
	else
	{
		MSG_WriteDeltaPlayerstate(msg, 0, &frame->ps);
		oldClients = 0;
		oldFirstClient = 0;
		SV_EmitPacketEntities(client - svs.clients, 0, 0, frame->num_entities, frame->first_entity, msg);
	}

	SV_EmitPacketClients( client - svs.clients, oldClients, oldFirstClient, frame->num_clients, frame->first_client, msg);

	for ( i = 0; i < sv_padPackets->current.integer; ++i )
		MSG_WriteByte(msg, svc_nop);
}

void SV_UpdateServerCommandsToClientRecover( client_t *client, msg_t *msg, int msgLen )
{
	int i;
	int cmdlen;

	for(i = client->reliableAcknowledge + 1; i <= client->reliableSequence; i++)
	{

		cmdlen = strlen(client->reliableCommands[i & ( MAX_RELIABLE_COMMANDS - 1 )].command);

		if ( cmdlen + msg->cursize + 6 >= msgLen )
			break;

		MSG_WriteByte(msg, svc_serverCommand);
		MSG_WriteLong(msg, i);
		MSG_WriteString(msg, client->reliableCommands[i & ( MAX_RELIABLE_COMMANDS - 1 )].command);

	}

	if ( i - 1 > client->reliableSent )
		client->reliableSent = i - 1;
}

void SV_SendClientSnapshot(client_s *client)
{
	byte buf[MAX_LARGE_MSGLEN]; // VoroN: !!! WARNING !!! In CoD2 1.0 they allocate this to 128k but wouldn't that potentially cause a netchan MAX_MSGLEN error? Changed to MAX_MSGLEN for now. Update: Nope, it SHOULD be 128k, otherwise snapshot overflows.
	msg_t msg;

	if ( client->state == CS_ACTIVE || client->state == CS_ZOMBIE )
		SV_BuildClientSnapshot(client);

	MSG_Init(&msg, buf, sizeof(buf));
	MSG_WriteLong(&msg, client->lastClientCommand);

	if ( client->state == CS_ACTIVE || client->state == CS_ZOMBIE )
	{
		SV_UpdateServerCommandsToClient(client, &msg);
		SV_WriteSnapshotToClient(client, &msg);
	}

	if ( client->state != CS_ZOMBIE )
		SV_WriteDownloadToClient(client, &msg);

	MSG_WriteByte(&msg, svc_EOF);

	if ( msg.overflowed )
	{
		Com_Printf("WARNING: msg overflowed for %s, trying to recover\n", client->name);

		if ( client->state == CS_ACTIVE || client->state == CS_ZOMBIE )
		{
			SV_ShowClientUnAckCommands(client);
			MSG_Init(&msg, buf, sizeof(buf));
			MSG_WriteLong(&msg, client->lastClientCommand);
			SV_UpdateServerCommandsToClientRecover(client, &msg, sizeof(buf));
			MSG_WriteByte(&msg, svc_EOF);
		}

		if ( msg.overflowed )
		{
			Com_Printf("WARNING: client disconnected for msg overflow: %s\n", client->name);
			NET_OutOfBandPrint(NS_SERVER, client->netchan.remoteAddress, "disconnect");
			SV_DropClient(client, "EXE_SERVERMESSAGEOVERFLOW");
		}
	}

	SV_SendMessageToClient(&msg, client);
}

void SV_ArchiveSnapshot()
{
	clientState_t *cs1;
	clientState_t *cs2;
	cachedClient_s *client2;
	byte buf[MAX_LARGE_MSGLEN]; // VoroN: !!! WARNING !!! In CoD2 1.0 they allocate this to 128k but wouldn't that potentially cause a netchan MAX_MSGLEN error? Changed to MAX_MSGLEN for now. Update: Nope, it SHOULD be 128k, otherwise snapshot overflows.
	msg_t msg;
	int clientIndex;
	int count;
	int clientNum;
	int num_clients;
	int maxClients;
	archivedEntity_s *archEnt;
	cachedClient_s *client1;
	int next_frame;
	int current_frame;
	cachedSnapshot_s *cachedFrame;
	playerState_s ps;
	size_t len;
	int currentBuffer;
	archivedSnapshot_s *aSnap;
	client_s *client;
	int i;
	svEntity_t *svEnt;
	archivedEntity_s aEnt;
	gentity_s *ent;
	int num;

	if ( sv.state == SS_GAME && svs.archivedSnapshotEnabled )
	{
		MSG_Init(&msg, buf, sizeof(buf));
		current_frame = svs.nextCachedSnapshotFrames - 512;

		if ( svs.nextCachedSnapshotFrames - 512 < 0 )
			current_frame = 0;

		next_frame = svs.nextArchivedSnapshotFrames - sv_fps->current.integer;

		for ( i = svs.nextCachedSnapshotFrames - 1; i >= current_frame; --i )
		{
			cachedFrame = &svs.cachedSnapshotFrames[i % 512];

			if ( cachedFrame->archivedFrame >= next_frame && !cachedFrame->usesDelta )
			{
				if ( cachedFrame->first_entity >= svs.nextCachedSnapshotEntities - 0x4000
				        && cachedFrame->first_client >= svs.nextCachedSnapshotClients - 4096 )
				{
					MSG_WriteBit0(&msg);
					MSG_WriteLong(&msg, cachedFrame->archivedFrame);
					MSG_WriteLong(&msg, svs.time);

					maxClients = sv_maxclients->current.integer;
					num_clients = cachedFrame->num_clients;

					client1 = 0;
					clientNum = 0;
					count = 0;

					while ( clientNum < maxClients || count < num_clients )
					{
						if ( clientNum >= maxClients || svs.clients[clientNum].state > CS_ZOMBIE )
						{
							if ( count < num_clients )
							{
								client1 = &svs.cachedSnapshotClients[(count + cachedFrame->first_client) % 4096];
								clientIndex = client1->cs.clientIndex;
							}
							else
							{
								clientIndex = 9999;
							}
							if ( clientNum == clientIndex )
							{
								cs1 = G_GetClientState(clientNum);
								MSG_WriteDeltaClient(&msg, &client1->cs, cs1, 1);

								if ( GetFollowPlayerState(clientNum, &ps) )
								{
									MSG_WriteBit1(&msg);
									MSG_WriteDeltaPlayerstate(&msg, &client1->ps, &ps);
								}
								else
								{
									MSG_WriteBit0(&msg);
								}

								++count;
								++clientNum;
							}
							else if ( clientNum >= clientIndex )
							{
								if ( clientNum > clientIndex )
									++count;
							}
							else
							{
								cs2 = G_GetClientState(clientNum);
								MSG_WriteDeltaClient(&msg, 0, cs2, 1);

								if ( GetFollowPlayerState(clientNum, &ps) )
								{
									MSG_WriteBit1(&msg);
									MSG_WriteDeltaPlayerstate(&msg, 0, &ps);
								}
								else
								{
									MSG_WriteBit0(&msg);
								}

								++clientNum;
							}
						}
						else
						{
							++clientNum;
						}
					}

					MSG_WriteBit0(&msg);

					for ( num = 0; num < sv.num_entities; ++num )
					{
						ent = SV_GentityNum(num);

						if ( ent->r.linked )
						{
							if ( ent->r.broadcastTime
							        || (ent->r.svFlags & 1) == 0
							        && ((svEnt = SV_SvEntityForGentity(ent), (ent->r.svFlags & 0x18) != 0) || svEnt->numClusters) )
							{
								memcpy(&aEnt.s, &ent->s, sizeof(entityState_t));
								aEnt.r.svFlags = ent->r.svFlags;

								if ( ent->r.broadcastTime )
									aEnt.r.svFlags |= 8u;

								aEnt.r.clientMask[0] = ent->r.clientMask[0];
								aEnt.r.clientMask[0] = ent->r.clientMask[0];

								VectorCopy(ent->r.absmin, aEnt.r.absmin);
								VectorCopy(ent->r.absmax, aEnt.r.absmax);

								MSG_WriteDeltaArchivedEntity(&msg, &sv.svEntities[ent->s.number].baseline, &aEnt, 1);
							}
						}
					}
					goto out;
				}
				break;
			}
		}

		MSG_WriteBit1(&msg);
		MSG_WriteLong(&msg, svs.time);

		cachedFrame = &svs.cachedSnapshotFrames[svs.nextCachedSnapshotFrames % 512];
		cachedFrame->archivedFrame = svs.nextArchivedSnapshotFrames;
		cachedFrame->num_entities = 0;
		cachedFrame->first_entity = svs.nextCachedSnapshotEntities;
		cachedFrame->num_clients = 0;
		cachedFrame->first_client = svs.nextCachedSnapshotClients;
		cachedFrame->usesDelta = 0;
		cachedFrame->time = svs.time;

		i = 0;
		client = svs.clients;

		while ( i < sv_maxclients->current.integer )
		{
			if ( client->state > CS_ZOMBIE )
			{
				client1 = &svs.cachedSnapshotClients[svs.nextCachedSnapshotClients % 4096];
				memcpy(&client1->cs, G_GetClientState(i), sizeof(client1->cs));
				MSG_WriteDeltaClient(&msg, 0, &client1->cs, 1);

				client2 = client1;
				client2->playerStateExists = GetFollowPlayerState(i, &client1->ps);

				if ( client1->playerStateExists )
				{
					MSG_WriteBit1(&msg);
					MSG_WriteDeltaPlayerstate(&msg, 0, &client1->ps);
				}
				else
				{
					MSG_WriteBit0(&msg);
				}

				if ( ++svs.nextCachedSnapshotClients > 2147483645 )
					Com_Error(ERR_FATAL, "svs.nextCachedSnapshotClients wrapped");

				++cachedFrame->num_clients;
			}

			++i;
			++client;
		}

		MSG_WriteBit0(&msg);

		for ( num = 0; num < sv.num_entities; ++num )
		{
			ent = SV_GentityNum(num);

			if ( ent->r.linked )
			{
				if ( ent->r.broadcastTime
				        || (ent->r.svFlags & 1) == 0
				        && ((svEnt = SV_SvEntityForGentity(ent), (ent->r.svFlags & 0x18) != 0) || svEnt->numClusters) )
				{
					archEnt = &svs.cachedSnapshotEntities[svs.nextCachedSnapshotEntities % 0x4000];
					memcpy(&archEnt->s, &ent->s, sizeof(entityState_t));
					archEnt->r.svFlags = ent->r.svFlags;

					if ( ent->r.broadcastTime )
						archEnt->r.svFlags |= 8u;

					archEnt->r.clientMask[0] = ent->r.clientMask[0];
					archEnt->r.clientMask[1] = ent->r.clientMask[1];

					VectorCopy(ent->r.absmin, archEnt->r.absmin);
					VectorCopy(ent->r.absmax, archEnt->r.absmax);

					MSG_WriteDeltaArchivedEntity(&msg, &sv.svEntities[ent->s.number].baseline, archEnt, 1);

					if ( ++svs.nextCachedSnapshotEntities > 2147483645 )
						Com_Error(ERR_FATAL, "svs.nextCachedSnapshotEntities wrapped");

					++cachedFrame->num_entities;
				}
			}
		}

		if ( ++svs.nextCachedSnapshotFrames > 2147483645 )
			Com_Error(ERR_FATAL, "svs.nextCachedSnapshotFrames wrapped");
out:
		MSG_WriteBits(&msg, 1023, 10);

		if ( msg.overflowed )
		{
			Com_DPrintf("SV_ArchiveSnapshot: ignoring snapshot because it overflowed.\n");
			return;
		}

		aSnap = &svs.archivedSnapshotFrames[svs.nextArchivedSnapshotFrames % 1200];
		aSnap->start = svs.nextArchivedSnapshotBuffer;
		aSnap->size = msg.cursize;
		currentBuffer = svs.nextArchivedSnapshotBuffer % 0x2000000;
		svs.nextArchivedSnapshotBuffer += msg.cursize;

		if ( svs.nextArchivedSnapshotBuffer > 2147483645 )
			Com_Error(ERR_FATAL, "svs.nextArchivedSnapshotBuffer wrapped");

		len = 0x2000000 - currentBuffer;

		if ( msg.cursize > 0x2000000 - currentBuffer )
		{
			memcpy(&svs.archivedSnapshotBuffer[currentBuffer], msg.data, len);
			memcpy(svs.archivedSnapshotBuffer, &msg.data[len], msg.cursize - len);
		}
		else
		{
			memcpy(&svs.archivedSnapshotBuffer[currentBuffer], msg.data, msg.cursize);
		}

		if ( ++svs.nextArchivedSnapshotFrames > 2147483645 )
			Com_Error(ERR_FATAL, "svs.nextArchivedSnapshotFrames wrapped");
	}
}

void SV_EnableArchivedSnapshot(qboolean enabled)
{
	svs.archivedSnapshotEnabled = enabled;

	if ( enabled )
	{
		if ( !svs.archivedSnapshotFrames )
		{
			svs.cachedSnapshotEntities = (archivedEntity_s *)Z_MallocInternal(0x450000u);
			svs.cachedSnapshotClients = (cachedClient_s *)Z_MallocInternal(0x2708000u);
			svs.archivedSnapshotFrames = (archivedSnapshot_s *)Z_MallocInternal(0x2580u);
			svs.archivedSnapshotBuffer = (byte *)Z_MallocInternal(0x2000000u);
			svs.cachedSnapshotFrames = (cachedSnapshot_s *)Z_MallocInternal(0x3800u);
		}
	}
}

void SV_InitArchivedSnapshot()
{
	svs.archivedSnapshotEnabled = 0;
	svs.nextArchivedSnapshotFrames = 0;
	svs.nextArchivedSnapshotBuffer = 0;
	svs.nextCachedSnapshotEntities = 0;
	svs.nextCachedSnapshotClients = 0;
	svs.nextCachedSnapshotFrames = 0;
}

void SV_ShutdownArchivedSnapshot()
{
	if ( svs.cachedSnapshotEntities )
	{
		Z_FreeInternal(svs.cachedSnapshotEntities);
		svs.cachedSnapshotEntities = 0;
	}

	if ( svs.cachedSnapshotClients )
	{
		Z_FreeInternal(svs.cachedSnapshotClients);
		svs.cachedSnapshotClients = 0;
	}

	if ( svs.archivedSnapshotFrames )
	{
		Z_FreeInternal(svs.archivedSnapshotFrames);
		svs.archivedSnapshotFrames = 0;
	}

	if ( svs.archivedSnapshotBuffer )
	{
		Z_FreeInternal(svs.archivedSnapshotBuffer);
		svs.archivedSnapshotBuffer = 0;
	}

	if ( svs.cachedSnapshotFrames )
	{
		Z_FreeInternal(svs.cachedSnapshotFrames);
		svs.cachedSnapshotFrames = 0;
	}
}

/*
=======================
SV_SendClientMessages
=======================
*/
extern dvar_t *sv_showAverageBPS;
void SV_SendClientMessages( void )
{
	int i;
	client_t    *c;
	int numclients = 0;         // NERVE - SMF - net debugging

	sv.bpsTotalBytes = 0;       // NERVE - SMF - net debugging
	sv.ubpsTotalBytes = 0;      // NERVE - SMF - net debugging

	// send a message to each connected client
	for ( i = 0; i < sv_maxclients->current.integer; i++ )
	{
		c = &svs.clients[i];

		// rain - changed <= CS_ZOMBIE to < CS_ZOMBIE so that the
		// disconnect reason is properly sent in the network stream
		if ( c->state < CS_ZOMBIE )
		{
			continue;       // not connected
		}

		if ( svs.time < c->nextSnapshotTime )
		{
			continue;       // not time yet
		}

		numclients++;       // NERVE - SMF - net debugging

		// send additional message fragments if the last message
		// was too large to send at once
		if ( c->netchan.unsentFragments )
		{
			c->nextSnapshotTime = svs.time +
			                      SV_RateMsec( c, c->netchan.unsentLength - c->netchan.unsentFragmentStart );
			SV_Netchan_TransmitNextFragment(&c->netchan);
			continue;
		}

		// generate and send a new message
		SV_SendClientSnapshot( c );
		SV_SendClientVoiceData( c );
	}

	// NERVE - SMF - net debugging
	if ( sv_showAverageBPS->current.boolean && numclients > 0 )
	{
		float ave = 0, uave = 0;

		for ( i = 0; i < MAX_BPS_WINDOW - 1; i++ )
		{
			sv.bpsWindow[i] = sv.bpsWindow[i + 1];
			ave += sv.bpsWindow[i];

			sv.ubpsWindow[i] = sv.ubpsWindow[i + 1];
			uave += sv.ubpsWindow[i];
		}

		sv.bpsWindow[MAX_BPS_WINDOW - 1] = sv.bpsTotalBytes;
		ave += sv.bpsTotalBytes;

		sv.ubpsWindow[MAX_BPS_WINDOW - 1] = sv.ubpsTotalBytes;
		uave += sv.ubpsTotalBytes;

		if ( sv.bpsTotalBytes >= sv.bpsMaxBytes )
		{
			sv.bpsMaxBytes = sv.bpsTotalBytes;
		}

		if ( sv.ubpsTotalBytes >= sv.ubpsMaxBytes )
		{
			sv.ubpsMaxBytes = sv.ubpsTotalBytes;
		}

		sv.bpsWindowSteps++;

		if ( sv.bpsWindowSteps >= MAX_BPS_WINDOW )
		{
			float comp_ratio;

			sv.bpsWindowSteps = 0;

			ave = ( ave / (float)MAX_BPS_WINDOW );
			uave = ( uave / (float)MAX_BPS_WINDOW );

			comp_ratio = ( 1 - ave / uave ) * 100.f;
			sv.ucompAve += comp_ratio;
			sv.ucompNum++;

			Com_DPrintf( "bpspc(%2.0f) bps(%2.0f) pk(%i) ubps(%2.0f) upk(%i) cr(%2.2f) acr(%2.2f)\n",
			             ave / (float)numclients, ave, sv.bpsMaxBytes, uave, sv.ubpsMaxBytes, comp_ratio, sv.ucompAve / sv.ucompNum );
		}
	}
	// -NERVE - SMF
}