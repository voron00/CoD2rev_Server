#include "../qcommon/qcommon.h"
#include "../qcommon/netchan.h"

/*
==================
SV_Netchan_UpdateProfileStats
==================
*/
void SV_Netchan_UpdateProfileStats()
{
	client_t *cl;
	int i;

	if ( !svs.clients )
		return;

	if ( svs.pOOBProf )
	{
		NetProf_UpdateStatistics(&svs.pOOBProf->send);
		NetProf_UpdateStatistics(&svs.pOOBProf->recieve);
	}

	for (i = 0, cl = svs.clients; i < sv_maxclients->current.integer; ++i, ++cl )
	{
		if ( !cl->state )
			continue;

		if ( !cl->netchan.pProf )
			continue;

		NetProf_UpdateStatistics(&cl->netchan.pProf->send);
		NetProf_UpdateStatistics(&cl->netchan.pProf->recieve);
	}
}

/*
==================
SV_Netchan_AddOOBProfilePacket
==================
*/
void SV_Netchan_AddOOBProfilePacket(int iLength)
{
	if ( net_profile->current.integer )
	{
		NetProf_PrepProfiling(&svs.pOOBProf);
		NetProf_AddPacket(&svs.pOOBProf->send, iLength, 0);
	}
}

/*
==================
SV_Netchan_TransmitNextFragment
==================
*/
bool SV_Netchan_TransmitNextFragment(netchan_t *chan)
{
	return Netchan_TransmitNextFragment(chan);
}

/*
==================
SV_Netchan_Transmit
==================
*/
bool SV_Netchan_Transmit(client_t *client, byte *data, int length)
{
	SV_Netchan_Encode(client, data + 4, length - 4);
	return Netchan_Transmit(&client->netchan, length, data);
}

/*
==================
SV_Netchan_PrintProfileStats
==================
*/
void SV_Netchan_PrintProfileStats(int bPrintToConsole)
{
	UNIMPLEMENTED(__FUNCTION__);
}

/*
==================
SV_Netchan_SendOOBPacket
==================
*/
void SV_Netchan_SendOOBPacket(int iLength, const void *pData, netadr_t to)
{
	if ( *(int *)pData != -1 )
		Com_Printf("SV_Netchan_SendOOBPacket used to send non-OOB packet.\n");

	NetProf_PrepProfiling(&svs.pOOBProf);
	NET_SendPacket(NS_SERVER, iLength, pData, to);
	SV_Netchan_AddOOBProfilePacket(iLength);
}

/*
==================
SV_Netchan_Encode
==================
*/
void SV_Netchan_Encode( client_t *client, byte *data, int cursize )
{
	int i, index;
	byte key, *string;

	string = (byte *)client->lastClientCommandString;
	key = client->challenge ^ client->netchan.outgoingSequence;

	for ( i = 0, index = 0; i < cursize; i++ )
	{
		if ( !string[index] )
		{
			index = 0;
		}

		// modify the key with the last sent and acknowledged server command
		key ^= string[index] << ( i & 1 );
		data[i] ^= key;

		index++;
	}
}

/*
==================
SV_Netchan_Decode
==================
*/
void SV_Netchan_Decode( client_t *client, byte *data, int remaining )
{
	int i, index;
	byte key, *string;

	string = (byte *)client->reliableCommandInfo[ client->reliableAcknowledge & ( MAX_RELIABLE_COMMANDS - 1 ) ].cmd;
	key = client->challenge ^ (byte)client->serverId ^ client->messageAcknowledge;

	for ( i = 0, index = 0; i < remaining; i++ )
	{
		if ( !string[index] )
		{
			index = 0;
		}

		// modify the key with the last sent and acknowledged server command
		key ^= string[index] << ( i & 1 );
		data[i] ^= key;

		index++;
	}
}