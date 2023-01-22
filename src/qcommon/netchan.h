#pragma once

#define MAX_MSGLEN				0x4000
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
static_assert((sizeof(netProfileInfo_t) == 0x5E0), "ERROR: netProfileInfo_t size is invalid!");

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
} netchan_t;
static_assert((sizeof(netchan_t) == 0x8040), "ERROR: netchan_t size is invalid!");

qboolean NET_GetLoopPacket(int sock, netadr_t *net_from, msg_t *net_message);
void NET_SendLoopPacket(int sock, int length, const void *data, netadr_t to);
const char  *NET_AdrToString( netadr_t a );
void Netchan_Init(uint16_t port);