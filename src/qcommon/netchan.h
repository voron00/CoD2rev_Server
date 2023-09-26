#pragma once

#if PROTOCOL_VERSION < 118
#define MAX_MSGLEN				0x4000
#else
#define MAX_MSGLEN				0x20000
#endif

#define MAX_LARGE_MSGLEN		0x20000 // For voice chat and snapshot

#define MAX_PACKETLEN           1400
#define FRAGMENT_SIZE           ( MAX_PACKETLEN - 100 )

typedef struct
{
	char command[MAX_STRING_CHARS];
	int cmdTime;
	int cmdType;
} reliableCommands_t;

struct netProfilePacket_t
{
	int iTime;
	int iSize;
	int bFragment;
};

struct netProfileStream_t
{
	netProfilePacket_t packets[60];
	int iCurrPacket;
	int iBytesPerSecond;
	int iLastBPSCalcTime;
	int iCountedPackets;
	int iCountedFragments;
	int iFragmentPercentage;
	int iLargestPacket;
	int iSmallestPacket;
};

typedef struct netProfileInfo_s
{
	netProfileStream_t send;
	netProfileStream_t recieve;
} netProfileInfo_t;
// static_assert((sizeof(netProfileInfo_t) == 0x5E0), "ERROR: netProfileInfo_t size is invalid!");

typedef struct
{
	int			outgoingSequence;
	netsrc_t	sock;
	int			dropped;
	int			incomingSequence;
	netadr_t	remoteAddress;
	int 		qport;
	int			fragmentSequence;
	int			fragmentLength;
	byte		fragmentBuffer[MAX_MSGLEN];
	qboolean	unsentFragments;
	int			unsentFragmentStart;
	int			unsentLength;
	byte		unsentBuffer[MAX_MSGLEN];
	netProfileInfo_t *prof;
	int protocol;
} netchan_t;
//static_assert((sizeof(netchan_t) == 0x8040), "ERROR: netchan_t size is invalid!");

extern dvar_t* net_profile;

qboolean NET_GetLoopPacket(int sock, netadr_t *net_from, msg_t *net_message);
void NET_SendLoopPacket(int sock, int length, const void *data, netadr_t to);
const char* NET_AdrToString( netadr_t a );
qboolean NET_StringToAdr( const char *s, netadr_t *a );
qboolean NET_SendPacket( netsrc_t sock, int length, const void *data, netadr_t to );
qboolean NET_GetPacket(netadr_t *net_from, msg_t *net_message);
void NetProf_PrepProfiling(netProfileInfo_t **prof);
void NetProf_AddPacket(netProfileStream_t *pProfStream, int iSize, int bFragment);
void NetProf_SendProfile(netchan_t *chan, int iSize, int bFragment);
void NetProf_ReceiveProfile(netchan_t *chan, int iSize, int bFragment);
void NetProf_UpdateStatistics(netProfileStream_t *pStream);
qboolean NET_OutOfBandPrint(netsrc_t sock, netadr_t adr, const char *msg);
qboolean NET_OutOfBandVoiceData(netsrc_t sock, netadr_t adr, byte *data, int len);
void Netchan_Prepare(netsrc_t src, netchan_t *chan, netadr_t adr, unsigned int qport);
qboolean Netchan_TransmitNextFragment(netchan_t *chan);
qboolean Netchan_Transmit(netchan_t *chan, int length, const byte *data);
qboolean Netchan_Process(netchan_t *chan, msg_t *msg);
qboolean NET_CompareBaseAdr(netadr_t a, netadr_t b);
qboolean NET_CompareAdr(netadr_t a, netadr_t b);
qboolean NET_IsLocalAddress( netadr_t adr );
void Netchan_Init(uint16_t port);