#include "qcommon.h"
#include "netchan.h"

#define PACKET_HEADER           10          // two ints and a short
#define FRAGMENT_BIT    ( 1 << 31 )

static const char *netsrcString[2] =
{
	"client",
	"server"
};

int g_qport;
int net_iProfilingOn;

dvar_t      *showpackets;
dvar_t      *showdrop;
dvar_t      *packetDebug;
dvar_t      *net_profile;
dvar_t      *net_showprofile;
dvar_t      *net_lanauthorize;

// there needs to be enough loopback messages to hold a complete
// gamestate of maximum size
#define MAX_LOOPBACK    16

typedef struct
{
	byte data[MAX_PACKETLEN];
	int datalen;
	int port;
} loopmsg_t;

typedef struct
{
	loopmsg_t msgs[MAX_LOOPBACK];
	int get, send;
} loopback_t;
static_assert((sizeof(loopback_t) == 0x5808), "ERROR: loopback_t size is invalid!");

loopback_t loopbacks[2];

qboolean NET_GetLoopPacketInternal(int sock, netadr_t *net_from, msg_t *net_message)
{
	loopback_t *loop;
	int msg;

	loop = &loopbacks[sock];

	if ( loop->send - loop->get > MAX_LOOPBACK )
		loop->get = loop->send - MAX_LOOPBACK;

	if ( loop->get >= loop->send )
		return qfalse;

	msg = loop->get & ( MAX_LOOPBACK - 1 );

	Com_Memcpy(net_message->data, &loop->msgs[msg], loop->msgs[msg].datalen);
	net_message->cursize = loop->msgs[msg].datalen;
	Com_Memset(net_from, 0, sizeof(netadr_t));
	net_from->type = NA_LOOPBACK;
	net_from->port = loop->msgs[msg].port;

	return qtrue;
}

qboolean NET_GetLoopPacket(int sock, netadr_t *net_from, msg_t *net_message)
{
	return NET_GetLoopPacketInternal(sock, net_from, net_message);
}

void NET_SendLoopPacket(int sock, int length, const void *data, netadr_t to)
{
	int port;
	loopback_t *loop;
	int msg;

	port = 0;

	if ( sock > NS_CLIENT )
	{
		if ( sock == NS_SERVER )
			sock = to.port;
	}
	else
	{
		port = sock;
		sock = NS_SERVER;
	}

	loop = &loopbacks[sock];
	msg = loop->send++ & ( MAX_LOOPBACK - 1 );
	Com_Memcpy(&loop->msgs[msg], data, length);
	loop->msgs[msg].datalen = length;
	loop->msgs[msg].port = port;
}

const char* NET_AdrToString( netadr_t a )
{
	static char s[64];

	if ( a.type == NA_LOOPBACK )
	{
		Com_sprintf( s, sizeof( s ), "loopback" );
	}
	else if ( a.type == NA_BOT )
	{
		Com_sprintf( s, sizeof( s ), "bot" );
	}
	else if ( a.type == NA_IP )
	{
		Com_sprintf( s, sizeof( s ), "%i.%i.%i.%i:%hu",
		             a.ip[0], a.ip[1], a.ip[2], a.ip[3], BigShort( a.port ) );
	}
	else
	{
		Com_sprintf( s, sizeof( s ), "%02x%02x%02x%02x.%02x%02x%02x%02x%02x%02x:%hu",
		             a.ipx[0], a.ipx[1], a.ipx[2], a.ipx[3], a.ipx[4], a.ipx[5], a.ipx[6], a.ipx[7], a.ipx[8], a.ipx[9],
		             BigShort( a.port ) );
	}

	return s;
}

qboolean NET_StringToAdr( const char *s, netadr_t *a )
{
	qboolean r;
	char base[MAX_STRING_CHARS];
	char    *port;

	if ( !strcmp( s, "localhost" ) )
	{
		Com_Memset( a, 0, sizeof( *a ) );
		a->type = NA_LOOPBACK;
		return qtrue;
	}

	// look for a port number
	Q_strncpyz( base, s, sizeof( base ) );
	port = strstr( base, ":" );
	if ( port )
	{
		*port = 0;
		port++;
	}

	r = Sys_StringToAdr( base, a );

	if ( !r )
	{
		a->type = NA_BAD;
		return qfalse;
	}

	// inet_addr returns this if out of range
	if ( a->ip[0] == 255 && a->ip[1] == 255 && a->ip[2] == 255 && a->ip[3] == 255 )
	{
		a->type = NA_BAD;
		return qfalse;
	}

	if ( port )
	{
		a->port = BigShort( (short)atoi( port ) );
	}
	else
	{
		a->port = BigShort( PORT_SERVER );
	}

	return qtrue;
}

qboolean NET_SendPacket( netsrc_t sock, int length, const void *data, netadr_t to )
{
	if ( showpackets->current.boolean && *(int *)data == -1 )
	{
		Com_Printf("[client %i] send packet %4i\n", 0, length);
	}

	if ( to.type == NA_LOOPBACK )
	{
		NET_SendLoopPacket(sock, length, data, to);
		return qtrue;
	}

	if ( to.type == NA_BOT )
	{
		return qfalse;
	}

	if ( to.type == NA_BAD )
	{
		return qfalse;
	}

	return Sys_SendPacket( length, data, to );
}

qboolean NET_GetPacket(netadr_t *net_from, msg_t *net_message)
{
	return Sys_GetPacket(net_from, net_message);
}

void NetProf_PrepProfiling(netProfileInfo_t **prof)
{
	if ( net_profile->current.integer )
	{
		if ( !net_iProfilingOn )
		{
			if ( !com_sv_running->current.boolean && net_profile->current.integer == 2 )
				net_iProfilingOn = 1;
			else
				net_iProfilingOn = 2;
			Com_Printf("Net Profiling turned on: %s\n", netsrcString[ net_iProfilingOn - 1 ]);
		}

		if ( !*prof )
		{
			*prof = (netProfileInfo_t *)Z_Malloc(sizeof(netProfileInfo_t));
			Com_Memset(*prof, 0, sizeof(netProfileInfo_t));
		}
	}
	else
	{
		if ( net_iProfilingOn )
		{
			net_iProfilingOn = 0;
			Com_Printf("Net Profiling turned off\n");
		}

		if ( *prof )
		{
			Z_Free(*prof);
			*prof = 0;
		}
	}
}

void NetProf_AddPacket(netProfileStream_t *pProfStream, int iSize, int bFragment)
{
	int stream;
	netProfilePacket_t *packet;

	stream = pProfStream->iCurrPacket + 1;
	pProfStream->iCurrPacket = stream / 60;
	pProfStream->iCurrPacket = stream - 60 * pProfStream->iCurrPacket;
	packet = &pProfStream->packets[pProfStream->iCurrPacket];
	packet->iTime = Sys_MilliSeconds();
	packet->iSize = iSize;
	packet->bFragment = bFragment;
}

void NetProf_SendProfile(netchan_t *chan, int iSize, int bFragment)
{
	if ( net_iProfilingOn )
	{
		NetProf_AddPacket(&chan->prof->send, iSize, bFragment);

		if ( (net_showprofile->current.integer & 2) != 0 )
		{
			if ( bFragment )
				Com_Printf("%s send%s: %i\n", netsrcString[ chan->sock ], " fragment", iSize);
			else
				Com_Printf("%s send%s: %i\n", netsrcString[ chan->sock ], " ", iSize);
		}
	}
}

void NetProf_ReceiveProfile(netchan_t *chan, int iSize, int bFragment)
{
	if ( net_iProfilingOn )
	{
		NetProf_AddPacket(&chan->prof->recieve, iSize, bFragment);

		if ( (net_showprofile->current.integer & 2) != 0 )
		{
			if ( bFragment )
				Com_Printf("%s recieve%s: %i\n", netsrcString[ chan->sock ], " fragment", iSize);
			else
				Com_Printf("%s recieve%s: %i\n", netsrcString[ chan->sock ], " ", iSize);
		}
	}
}

void NetProf_UpdateStatistics(netProfileStream_t *pStream)
{
	int lastBPSTime;
	int largestPacket;
	int iSize;
	int packetSize;
	int packetTime;
	int iTime;
	int packet;
	int fragmentCount;
	int packetCount;
	int i;

	packetCount = 0;
	fragmentCount = 0;
	packet = -1;
	iTime = Sys_MilliSeconds();
	packetSize = 0;
	iSize = 9999;
	largestPacket = 0;

	for ( i = 0; i < 60; ++i )
	{
		if ( pStream->packets[i].iTime && Sys_MilliSeconds() <= pStream->packets[i].iTime + 1000 )
		{
			++packetCount;

			if ( pStream->packets[i].bFragment )
				++fragmentCount;

			if ( pStream->packets[i].iTime < iTime )
			{
				packet = i;
				iTime = pStream->packets[i].iTime;
			}

			packetSize += pStream->packets[i].iSize;

			if ( pStream->packets[i].iSize < iSize )
				iSize = pStream->packets[i].iSize;
			if ( pStream->packets[i].iSize > largestPacket )
				largestPacket = pStream->packets[i].iSize;
		}
	}

	if ( packetCount )
	{
		if ( fragmentCount )
			pStream->iFragmentPercentage = 100 * fragmentCount / packetCount;
		else
			pStream->iFragmentPercentage = 0;

		pStream->iLargestPacket = largestPacket;
		pStream->iSmallestPacket = iSize;
		lastBPSTime = pStream->iLastBPSCalcTime + 100;

		if ( lastBPSTime < Sys_MilliSeconds() )
		{
			packetTime = Sys_MilliSeconds() - iTime;

			if ( packet != -1 )
			{
				packetSize -= pStream->packets[packet].iSize;
				--packetCount;

				if ( pStream->packets[packet].bFragment )
					--fragmentCount;
			}

			if ( packetTime > 0 && packetCount )
			{
				if ( packetSize )
					pStream->iBytesPerSecond = (int)((long double)packetSize / ((long double)packetTime * 0.001));
				else
					pStream->iBytesPerSecond = 0;

				pStream->iLastBPSCalcTime = Sys_MilliSeconds();
			}
			else
			{
				pStream->iBytesPerSecond = 0;
			}
		}

		pStream->iCountedPackets = packetCount;
		pStream->iCountedFragments = fragmentCount;
	}
	else
	{
		pStream->iBytesPerSecond = 0;
		pStream->iLastBPSCalcTime = 0;
		pStream->iCountedPackets = 0;
		pStream->iCountedFragments = 0;
		pStream->iFragmentPercentage = 0;
		pStream->iLargestPacket = 0;
		pStream->iSmallestPacket = 0;
	}
}

qboolean NET_OutOfBandPrint(netsrc_t sock, netadr_t adr, const char *msg)
{
	char buf[MAX_MSGLEN];
	size_t msglen;
	size_t buflen;

	// set the header
	buf[0] = -1;
	buf[1] = -1;
	buf[2] = -1;
	buf[3] = -1;

	if ( showpackets->current.boolean )
		Com_DPrintf("OOB Print: %s\n", msg);

	msglen = strlen(msg);

	if ( msglen > MAX_MSGLEN )
	{
		Com_DPrintf("OOB Packet is %i bytes - too large to send\n", msglen);
		return qfalse;
	}

	Com_Memcpy(buf + 4, msg, msglen + 1);
	buflen = strlen(buf);

	if ( sock == NS_SERVER )
		SV_Netchan_AddOOBProfilePacket(buflen);

	return NET_SendPacket(sock, buflen, buf, adr) > 0;
}

qboolean NET_OutOfBandVoiceData(netsrc_t sock, netadr_t adr, byte *data, int len)
{
	char buf[MAX_LARGE_MSGLEN];
	size_t buflen;

	// set the header
	buf[0] = -1;
	buf[1] = -1;
	buf[2] = -1;
	buf[3] = -1;

	Com_Memcpy(buf + 4, data, len);
	buflen = len + 4;

	if ( sock == NS_SERVER )
		SV_Netchan_AddOOBProfilePacket(buflen);

	return NET_SendPacket(sock, buflen, buf, adr) > 0;
}

void Netchan_Prepare(netsrc_t src, netchan_t *chan, netadr_t adr, unsigned int qport)
{
	Com_Memset(chan, 0, sizeof(netchan_t));
	chan->sock = src;
	chan->remoteAddress = adr;
	chan->qport = qport;
	chan->incomingSequence = 0;
	chan->outgoingSequence = 1;
	NetProf_PrepProfiling(&chan->prof);
}

qboolean Netchan_TransmitNextFragment(netchan_t *chan)
{
	msg_t send;
	byte send_buf[MAX_PACKETLEN];
	int fragmentLength;
	int status;

	NetProf_PrepProfiling(&chan->prof);

	// write the packet header
	MSG_Init( &send, send_buf, sizeof( send_buf ) );                // <-- only do the oob here
	MSG_WriteLong( &send, chan->outgoingSequence | FRAGMENT_BIT );

	// send the qport if we are a client
	if ( chan->sock == NS_CLIENT )
	{
		MSG_WriteShort( &send, chan->qport );
	}

	// copy the reliable message to the packet first
	fragmentLength = FRAGMENT_SIZE;

	if ( chan->unsentFragmentStart  + fragmentLength > chan->unsentLength )
	{
		fragmentLength = chan->unsentLength - chan->unsentFragmentStart;
	}

	if (chan->protocol < 118)
		MSG_WriteShort( &send, chan->unsentFragmentStart );
	else
		MSG_WriteLong( &send, chan->unsentFragmentStart );

	MSG_WriteShort( &send, fragmentLength );
	MSG_WriteData( &send, chan->unsentBuffer + chan->unsentFragmentStart, fragmentLength );

	// send the datagram
	status = NET_SendPacket( chan->sock, send.cursize, send.data, chan->remoteAddress );
	NetProf_SendProfile(chan, send.cursize, 1);

	if ( showpackets->current.boolean )
	{
		Com_Printf( "%s send %4i : s=%i fragment=%i,%i\n"
		            , netsrcString[ chan->sock ]
		            , send.cursize
		            , chan->outgoingSequence
		            , chan->unsentFragmentStart, fragmentLength );
	}

	chan->unsentFragmentStart += fragmentLength;

	// this exit condition is a little tricky, because a packet
	// that is exactly the fragment length still needs to send
	// a second packet of zero length so that the other side
	// can tell there aren't more to follow
	if ( chan->unsentFragmentStart == chan->unsentLength && fragmentLength != FRAGMENT_SIZE )
	{
		chan->outgoingSequence++;
		chan->unsentFragments = qfalse;
	}

	return status > 0;
}

qboolean Netchan_Transmit(netchan_t *chan, int length, const byte *data)
{
	msg_t send;
	byte send_buf[MAX_PACKETLEN];
	int status;

	if ( length > MAX_MSGLEN )
	{
		Com_Error( ERR_DROP, "Netchan_Transmit: length = %i", length );
	}

	chan->unsentFragmentStart = 0;

	// fragment large reliable messages
	if ( length >= FRAGMENT_SIZE )
	{
		chan->unsentFragments = qtrue;
		chan->unsentLength = length;
		Com_Memcpy( chan->unsentBuffer, data, length );

		// only send the first fragment now
		Netchan_TransmitNextFragment( chan );

		return qtrue;
	}

	NetProf_PrepProfiling(&chan->prof);

	// write the packet header
	MSG_Init( &send, send_buf, sizeof( send_buf ) );

	MSG_WriteLong( &send, chan->outgoingSequence );
	chan->outgoingSequence++;

	// send the qport if we are a client
	if ( chan->sock == NS_CLIENT )
	{
		MSG_WriteShort( &send, chan->qport );
	}

	if ( packetDebug->current.boolean )
		Com_Printf("Adding %i byte payload to packet\n", length);

	MSG_WriteData( &send, data, length );

	if ( packetDebug->current.boolean )
		Com_Printf("Sending %i byte packet\n", send.cursize);

	// send the datagram
	status = NET_SendPacket( chan->sock, send.cursize, send.data, chan->remoteAddress );
	NetProf_SendProfile(chan, send.cursize, 0);

	if ( showpackets->current.boolean )
	{
		Com_Printf( "%s send %4i : s=%i ack=%i\n"
		            , netsrcString[ chan->sock ]
		            , send.cursize
		            , chan->outgoingSequence - 1
		            , chan->incomingSequence );
	}

	return status > 0;
}

qboolean Netchan_Process(netchan_t *chan, msg_t *msg)
{
	int sequence;
	int fragmentStart, fragmentLength;
	qboolean fragmented;

	NetProf_PrepProfiling(&chan->prof);

	// get sequence numbers
	MSG_BeginReading( msg );
	sequence = MSG_ReadLong( msg );

	// check for fragment information
	if ( sequence & FRAGMENT_BIT )
	{
		sequence &= ~FRAGMENT_BIT;
		fragmented = qtrue;
	}
	else
	{
		fragmented = qfalse;
	}

	// read the qport if we are a server
	if ( chan->sock == NS_SERVER )
	{
		MSG_ReadShort( msg );
	}

	// read the fragment information
	if ( fragmented )
	{
		fragmentStart = MSG_ReadShort( msg );
		fragmentLength = MSG_ReadShort( msg );
	}
	else
	{
		fragmentStart = 0;      // stop warning message
		fragmentLength = 0;
	}

	NetProf_ReceiveProfile(chan, msg->cursize, fragmented);

	if ( showpackets->current.boolean )
	{
		if ( fragmented )
		{
			Com_Printf( "%s recv %4i : s=%i fragment=%i,%i\n"
			            , netsrcString[ chan->sock ]
			            , msg->cursize
			            , sequence
			            , fragmentStart, fragmentLength );
		}
		else
		{
			Com_Printf( "%s recv %4i : s=%i\n"
			            , netsrcString[ chan->sock ]
			            , msg->cursize
			            , sequence );
		}
	}

	//
	// discard out of order or duplicated packets
	//
	if ( sequence <= chan->incomingSequence )
	{
		if ( showdrop->current.boolean || showpackets->current.boolean )
		{
			Com_Printf( "%s:Out of order packet %i at %i\n"
			            , NET_AdrToString( chan->remoteAddress )
			            ,  sequence
			            , chan->incomingSequence );
		}
		return qfalse;
	}

	//
	// dropped packets don't keep the message from being used
	//
	chan->dropped = sequence - ( chan->incomingSequence + 1 );

	if ( chan->dropped > 0 )
	{
		if ( showdrop->current.boolean || showpackets->current.boolean )
		{
			Com_Printf( "%s:Dropped %i packets at %i\n"
			            , NET_AdrToString( chan->remoteAddress )
			            , chan->dropped
			            , sequence );
		}
	}


	//
	// if this is the final framgent of a reliable message,
	// bump incoming_reliable_sequence
	//
	if ( fragmented )
	{
		// TTimo
		// make sure we add the fragments in correct order
		// either a packet was dropped, or we received this one too soon
		// we don't reconstruct the fragments. we will wait till this fragment gets to us again
		// (NOTE: we could probably try to rebuild by out of order chunks if needed)
		if ( sequence != chan->fragmentSequence )
		{
			chan->fragmentSequence = sequence;
			chan->fragmentLength = 0;
		}

		// if we missed a fragment, dump the message
		if ( fragmentStart != chan->fragmentLength )
		{
			if ( showdrop->current.boolean || showpackets->current.boolean )
			{
				Com_Printf( "%s:Dropped a message fragment, sequence %d\n"
				            , NET_AdrToString( chan->remoteAddress )
				            , sequence );
			}
			// we can still keep the part that we have so far,
			// so we don't need to clear chan->fragmentLength
			return qfalse;
		}

		// copy the fragment to the fragment buffer
		if ( fragmentLength < 0 || msg->readcount + fragmentLength > msg->cursize ||
		        chan->fragmentLength + fragmentLength > sizeof( chan->fragmentBuffer ) )
		{
			if ( showdrop->current.boolean || showpackets->current.boolean )
			{
				Com_Printf( "%s:illegal fragment length\n"
				            , NET_AdrToString( chan->remoteAddress ) );
			}
			return qfalse;
		}

		Com_Memcpy( chan->fragmentBuffer + chan->fragmentLength,
		            msg->data + msg->readcount, fragmentLength );

		chan->fragmentLength += fragmentLength;

		// if this wasn't the last fragment, don't process anything
		if ( fragmentLength == FRAGMENT_SIZE )
		{
			return qfalse;
		}

		if ( chan->fragmentLength > msg->maxsize )
		{
			Com_Printf( "%s:fragmentLength %i > msg->maxsize\n"
			            , NET_AdrToString( chan->remoteAddress ),
			            chan->fragmentLength );
			return qfalse;
		}

		// copy the full message over the partial fragment

		// make sure the sequence number is still there
		*(int *)msg->data = LittleLong( sequence );

		Com_Memcpy( msg->data + 4, chan->fragmentBuffer, chan->fragmentLength );
		msg->cursize = chan->fragmentLength + 4;
		chan->fragmentLength = 0;
		msg->readcount = 4; // past the sequence number
		msg->bit = 32;  // past the sequence number

		// TTimo
		// clients were not acking fragmented messages
		chan->incomingSequence = sequence;

		return qtrue;
	}

	//
	// the message can now be read from the current message pointer
	//
	chan->incomingSequence = sequence;

	return qtrue;
}

int NET_CompareBaseAdrSigned(netadr_t *a, netadr_t *b)
{
	if ( a->type != b->type )
		return a->type - b->type;

	switch ( a->type )
	{
	case NA_BOT:
	case NA_LOOPBACK:
		return a->port - b->port;
	case NA_IP:
		return memcmp(a->ip, b->ip, 4);
	case NA_IPX:
		return memcmp(a->ipx, b->ipx, 10);
	}

	Com_Printf("NET_CompareBaseAdrSigned: bad address type\n");
	return 0;
}

qboolean NET_CompareBaseAdr(netadr_t a, netadr_t b)
{
	return NET_CompareBaseAdrSigned(&a, &b) == 0;
}

int NET_CompareAdrSigned(netadr_t *a, netadr_t *b)
{
	if ( a->type != b->type )
		return a->type - b->type;

	switch ( a->type )
	{
	case NA_LOOPBACK:
		return 0;
	case NA_IP:
		if ( a->port == b->port )
			return memcmp(a->ip, b->ip, 4);
		else
			return a->port - b->port;
	case NA_IPX:
		if ( a->port == b->port )
			return memcmp(a->ipx, b->ipx, 10);
		else
			return a->port - b->port;
	default:
		Com_Printf("NET_CompareAdrSigned: bad address type\n");
		return 0;
	}
}

qboolean NET_CompareAdr(netadr_t a, netadr_t b)
{
	return NET_CompareAdrSigned(&a, &b) == 0;
}

qboolean NET_IsLocalAddress( netadr_t adr )
{
	return adr.type == NA_LOOPBACK || adr.type == NA_BOT;
}

void Netchan_Init(uint16_t port)
{
	showpackets = Dvar_RegisterBool("showpackets", 0, 4096);
	showdrop = Dvar_RegisterBool("showdrop", 0, 4096);
	packetDebug = Dvar_RegisterBool("packetDebug", 0, 4096);
	g_qport = port;
	net_profile = Dvar_RegisterInt("net_profile", 0, 0, 2, 4096);
	net_showprofile = Dvar_RegisterInt("net_showprofile", 0, 0, 3, 4096);
	net_lanauthorize = Dvar_RegisterBool("net_lanauthorize", 0, 4096);
}