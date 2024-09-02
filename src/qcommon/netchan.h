#pragma once

#define MAX_PACKETLEN           1400
#define FRAGMENT_SIZE           ( MAX_PACKETLEN - 100 )

struct netProfilePacket_t
{
	int iTime;
	int iSize;
	int bFragment;
};

#define MAX_PROFILE_PACKETS 60
struct netProfileStream_t
{
	netProfilePacket_t packets[MAX_PROFILE_PACKETS];
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
#if defined(__i386__)
static_assert((sizeof(netProfileInfo_t) == 0x5E0), "ERROR: netProfileInfo_t size is invalid!");
#endif

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
	netProfileInfo_t *pProf;
#ifdef LIBCOD
	int protocol;
#endif
} netchan_t;
/*
#if defined(__i386__)
static_assert((sizeof(netchan_t) == 0x8040), "ERROR: netchan_t size is invalid!");
#endif
*/

extern dvar_t* net_profile;
extern dvar_t* net_lanauthorize;

qboolean NET_GetLoopPacket(int sock, netadr_t *net_from, msg_t *net_message);
const char* NET_AdrToString( netadr_t a );
qboolean NET_StringToAdr( const char *s, netadr_t *a );
qboolean NET_SendPacket( netsrc_t sock, int length, const void *data, netadr_t to );
qboolean NET_GetPacket(netadr_t *net_from, msg_t *net_message);
void NetProf_PrepProfiling(netProfileInfo_t **prof);
void NetProf_AddPacket(netProfileStream_t *pProfStream, int iSize, int bFragment);
void NetProf_NewSendPacket(netchan_t *chan, int iSize, int bFragment);
void NetProf_NewRecievePacket(netchan_t *chan, int iSize, int bFragment);
void NetProf_UpdateStatistics(netProfileStream_t *pStream);
void SV_Netchan_PrintProfileStats(int bPrintToConsole);
qboolean NET_OutOfBandPrint(netsrc_t sock, netadr_t adr, const char *msg);
void NET_OutOfBandVoiceData( netsrc_t sock, netadr_t adr, byte *data, int len );
void Netchan_Setup( netsrc_t sock, netchan_t *chan, netadr_t adr, int qport );
qboolean Netchan_TransmitNextFragment(netchan_t *chan);
qboolean Netchan_Transmit(netchan_t *chan, int length, const byte *data);
qboolean Netchan_Process(netchan_t *chan, msg_t *msg);
qboolean NET_CompareBaseAdr(netadr_t a, netadr_t b);
qboolean NET_CompareAdr(netadr_t a, netadr_t b);
qboolean NET_IsLocalAddress( netadr_t adr );
void Netchan_Init();