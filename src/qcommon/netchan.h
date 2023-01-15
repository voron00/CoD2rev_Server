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

typedef void netProfileInfo_t;

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
	netProfileInfo_t *netProfile;
} netchan_t;

const char *NET_AdrToString( netadr_t a );

qboolean NET_GetLoopPacket( netsrc_t sock, netadr_t *net_from, msg_t *net_message );
void NET_SendLoopPacket( netsrc_t sock, int length, const void *data, netadr_t to );
void Netchan_Init( unsigned short port );
void Netchan_Setup( netsrc_t sock, netchan_t *chan, netadr_t adr, unsigned int qport );
qboolean Netchan_Process( netchan_t *chan, msg_t *msg );
qboolean Netchan_TransmitNextFragment( netchan_t *chan );
qboolean Netchan_Transmit( netchan_t *chan, int length, const byte *data );
