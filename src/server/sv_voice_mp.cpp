#include "../qcommon/qcommon.h"
#include "../qcommon/netchan.h"

extern dvar_t *sv_voice;

bool SV_ClientHasClientMuted(int listener, int talker)
{
	return svs.clients[listener].mutedClients[talker];
}

bool SV_ClientWantsVoiceData(unsigned int clientNum)
{
	return svs.clients[clientNum].hasVoip;
}

void SV_WriteClientVoiceData(client_s *client, msg_t *msg)
{
	int i;

	MSG_WriteByte(msg, client->unsentVoiceData);

	for ( i = 0; i < client->unsentVoiceData; ++i )
	{
		MSG_WriteByte(msg, client->voicedata[i].num);
		MSG_WriteByte(msg, client->voicedata[i].dataLen);
		MSG_WriteData(msg, client->voicedata[i].data, client->voicedata[i].dataLen);
	}
}

void SV_SendClientVoiceData(client_s *client)
{
	byte buf[MAX_LARGE_MSGLEN];
	msg_t msg;

	if ( client->state == CS_ACTIVE && client->unsentVoiceData )
	{
		MSG_Init(&msg, buf, sizeof(buf));
		MSG_WriteString(&msg, "v");
		SV_WriteClientVoiceData(client, &msg);

		if ( msg.overflowed )
		{
			Com_Printf("WARNING: voice msg overflowed for %s\n", client->name);
		}
		else
		{
			NET_OutOfBandVoiceData(NS_SERVER, client->netchan.remoteAddress, msg.data, msg.cursize);
			client->unsentVoiceData = 0;
		}
	}
}

void SV_QueueVoicePacket(int talkerNum, int clientNum, VoicePacket_t *voicePacket)
{
	client_s *client;

	client = &svs.clients[clientNum];

	if ( client->unsentVoiceData < 40 )
	{
		client->voicedata[client->unsentVoiceData].dataLen = voicePacket->dataLen;
		memcpy(client->voicedata[client->unsentVoiceData].data, voicePacket->data, voicePacket->dataLen);
		client->voicedata[client->unsentVoiceData++].num = talkerNum;
	}
}

void SV_UserVoice(client_s *cl, msg_t *msg)
{
	int packet;
	int packetCount;
	VoicePacket_t voicePacket;

	if ( sv_voice->current.boolean )
	{
		packetCount = MSG_ReadByte(msg);

		for ( packet = 0; packet < packetCount; ++packet )
		{
			voicePacket.dataLen = MSG_ReadByte(msg);

			if ( voicePacket.dataLen <= 0 || voicePacket.dataLen > 256 )
			{
				Com_Printf("Received invalid voice packet of size %i from %s\n", voicePacket.dataLen, cl->name);
				return;
			}

			MSG_ReadData(msg, voicePacket.data, voicePacket.dataLen);
			G_BroadcastVoice(cl->gentity, &voicePacket);
		}
	}
}

void SV_PreGameUserVoice(client_s *cl, msg_t *msg)
{
	int talker;
	int listener;
	int packet;
	int packetCount;
	VoicePacket_t voicePacket;

	if ( sv_voice->current.boolean )
	{
		talker = cl - svs.clients;
		packetCount = MSG_ReadByte(msg);

		for ( packet = 0; packet < packetCount; ++packet )
		{
			voicePacket.dataLen = MSG_ReadShort(msg);

			if ( voicePacket.dataLen <= 0 || voicePacket.dataLen > 256 )
			{
				Com_Printf("Received invalid voice packet of size %i from %s\n", voicePacket.dataLen, cl->name);
				return;
			}

			MSG_ReadData(msg, voicePacket.data, voicePacket.dataLen);

			for ( listener = 0; listener <= 63; ++listener )
			{
				if ( listener != talker
				        && svs.clients[listener].state > CS_ZOMBIE
				        && !SV_ClientHasClientMuted(listener, talker)
				        && SV_ClientWantsVoiceData(listener) )
				{
					SV_QueueVoicePacket(talker, listener, &voicePacket);
				}
			}
		}
	}
}