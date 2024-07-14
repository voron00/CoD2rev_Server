#include "../qcommon/qcommon.h"
#include "../qcommon/netchan.h"

/*
==================
SV_QueueVoicePacket
==================
*/
void SV_QueueVoicePacket(int talkerNum, int clientNum, VoicePacket_t *voicePacket)
{
	client_t *client = &svs.clients[clientNum];

	if ( client->voicePacketCount >= MAX_VOICE_PACKETS )
	{
		return;
	}

	client->voicePackets[client->voicePacketCount].dataSize = voicePacket->dataSize;
	memcpy(client->voicePackets[client->voicePacketCount].data, voicePacket->data, voicePacket->dataSize);
	client->voicePackets[client->voicePacketCount].talker = talkerNum;
	client->voicePacketCount++;
}

/*
==================
SV_ClientHasClientMuted
==================
*/
bool SV_ClientHasClientMuted(int listener, int talker)
{
	return svs.clients[listener].muteList[talker];
}

/*
==================
SV_ClientWantsVoiceData
==================
*/
bool SV_ClientWantsVoiceData(int clientNum)
{
	return svs.clients[clientNum].sendVoice;
}

/*
==================
SV_PreGameUserVoice
==================
*/
void SV_PreGameUserVoice(client_t *cl, msg_t *msg)
{
	int talker;
	int listener;
	int packet;
	int packetCount;
	VoicePacket_t voicePacket;

	if ( !sv_voice->current.boolean )
	{
		return;
	}

	talker = cl - svs.clients;
	packetCount = MSG_ReadByte(msg);

	for ( packet = 0; packet < packetCount; packet++ )
	{
		voicePacket.dataSize = MSG_ReadShort(msg);

		if ( voicePacket.dataSize <= 0 || voicePacket.dataSize > MAX_VOICE_PACKET_DATA )
		{
			Com_Printf("Received invalid voice packet of size %i from %s\n", voicePacket.dataSize, cl->name);
			return;
		}

		MSG_ReadData(msg, voicePacket.data, voicePacket.dataSize);

		for ( listener = 0; listener < MAX_CLIENTS; listener++ )
		{
			if ( listener == talker )
				continue;

			if ( svs.clients[listener].state < CS_CONNECTED )
				continue;

			if ( SV_ClientHasClientMuted(listener, talker) )
				continue;

			if ( !SV_ClientWantsVoiceData(listener) )
				continue;

			SV_QueueVoicePacket(talker, listener, &voicePacket);
		}
	}
}

/*
==================
SV_UserVoice
==================
*/
void SV_UserVoice(client_t *cl, msg_t *msg)
{
	int packet;
	int packetCount;
	VoicePacket_t voicePacket;

	if ( !sv_voice->current.boolean )
	{
		return;
	}

	packetCount = MSG_ReadByte(msg);

	for ( packet = 0; packet < packetCount; packet++ )
	{
		voicePacket.dataSize = MSG_ReadByte(msg);

		if ( voicePacket.dataSize <= 0 || voicePacket.dataSize > MAX_VOICE_PACKET_DATA )
		{
			Com_Printf("Received invalid voice packet of size %i from %s\n", voicePacket.dataSize, cl->name);
			return;
		}

		MSG_ReadData(msg, voicePacket.data, voicePacket.dataSize);
		G_BroadcastVoice(cl->gentity, &voicePacket);
	}
}

/*
==================
SV_WriteClientVoiceData
==================
*/
void SV_WriteClientVoiceData(client_t *client, msg_t *msg)
{
	int i;

	MSG_WriteByte(msg, client->voicePacketCount);

	for ( i = 0; i < client->voicePacketCount; ++i )
	{
		MSG_WriteByte(msg, client->voicePackets[i].talker);
		MSG_WriteByte(msg, client->voicePackets[i].dataSize);
		MSG_WriteData(msg, client->voicePackets[i].data, client->voicePackets[i].dataSize);
	}
}

/*
==================
SV_SendClientVoiceData
==================
*/
void SV_SendClientVoiceData(client_t *client)
{
	byte msg_buf[MAX_VOICE_MSG_LEN];
	msg_t msg;

	if ( client->state != CS_ACTIVE )
		return;

	if ( !client->voicePacketCount )
		return;

	MSG_Init(&msg, msg_buf, sizeof(msg_buf));
	MSG_WriteString(&msg, "v");
	SV_WriteClientVoiceData(client, &msg);

	if ( msg.overflowed )
	{
		Com_Printf("WARNING: voice msg overflowed for %s\n", client->name);
		return;
	}

	NET_OutOfBandVoiceData(NS_SERVER, client->netchan.remoteAddress, msg.data, msg.cursize);
	client->voicePacketCount = 0;
}