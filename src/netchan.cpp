#include "i_common.h"
#include "i_shared.h"
#include "netchan.h"
#include "sys_net.h"
#include "dvar.h"

#define PACKET_HEADER           10          // two ints and a short
#define FRAGMENT_BIT    ( 1 << 31 )

static const char *netsrcString[2] =
{
	"client",
	"server"
};

dvar_t      *showpackets;
dvar_t      *showdrop;
dvar_t      *packetDebug;
dvar_t      *g_qport;
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
} loopmsg_t;

typedef struct
{
	loopmsg_t msgs[MAX_LOOPBACK];
	int get, send;
} loopback_t;

loopback_t loopbacks[2];

/*
===============
Netchan_Init
===============
*/
void Netchan_Init( int port )
{
	showpackets = Dvar_RegisterBool("showpackets", 0, 4096);
	showdrop = Dvar_RegisterBool("showdrop", 0, 4096);
	packetDebug = Dvar_RegisterBool("packetDebug", 0, 4096);
	g_qport = Dvar_RegisterInt("net_qport", port, 1, 65535, 4096);
	net_profile = Dvar_RegisterInt("net_profile", 0, 0, 2, 4096);
	net_showprofile = Dvar_RegisterInt("net_showprofile", 0, 0, 3, 4096);
	net_lanauthorize = Dvar_RegisterBool("net_lanauthorize", 0, 4096);
}

/*
==============
Netchan_Setup
called to open a channel to a remote system
==============
*/
void Netchan_Setup( netsrc_t sock, netchan_t *chan, netadr_t adr, unsigned int qport )
{
	memset( chan, 0, sizeof( netchan_t ) );

	chan->sock = sock;
	chan->remoteAddress = adr;
	chan->qport = qport;
	chan->incomingSequence = 0;
	chan->outgoingSequence = 1;
}

qboolean    NET_CompareBaseAdr( netadr_t a, netadr_t b )
{
	if ( a.type != b.type )
	{
		return qfalse;
	}

	if ( a.type == NA_LOOPBACK )
	{
		return qtrue;
	}

	if ( a.type == NA_IP )
	{
		if ( a.ip[0] == b.ip[0] && a.ip[1] == b.ip[1] && a.ip[2] == b.ip[2] && a.ip[3] == b.ip[3] )
		{
			return qtrue;
		}
		return qfalse;
	}

	if ( a.type == NA_IPX )
	{
		if ( ( memcmp( a.ipx, b.ipx, 10 ) == 0 ) )
		{
			return qtrue;
		}
		return qfalse;
	}

	Com_Printf( "NET_CompareBaseAdr: bad address type\n" );
	return qfalse;
}

qboolean    NET_CompareAdr( netadr_t a, netadr_t b )
{
	if ( a.type != b.type )
	{
		return qfalse;
	}

	if ( a.type == NA_LOOPBACK )
	{
		return qtrue;
	}

	if ( a.type == NA_IP )
	{
		if ( a.ip[0] == b.ip[0] && a.ip[1] == b.ip[1] && a.ip[2] == b.ip[2] && a.ip[3] == b.ip[3] && a.port == b.port )
		{
			return qtrue;
		}
		return qfalse;
	}

	if ( a.type == NA_IPX )
	{
		if ( ( memcmp( a.ipx, b.ipx, 10 ) == 0 ) && a.port == b.port )
		{
			return qtrue;
		}
		return qfalse;
	}

	Com_Printf( "NET_CompareAdr: bad address type\n" );
	return qfalse;
}

const char  *NET_AdrToString( netadr_t a )
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

void NET_SendLoopPacket( netsrc_t sock, int length, const void *data, netadr_t to )
{
	int i;
	loopback_t  *loop;

	loop = &loopbacks[sock ^ 1];

	i = loop->send & ( MAX_LOOPBACK - 1 );
	loop->send++;

	Com_Memcpy( loop->msgs[i].data, data, length );
	loop->msgs[i].datalen = length;
}

qboolean NET_SendPacket( netsrc_t sock, int length, const void *data, netadr_t to )
{
	// sequenced packets are shown in netchan, so just show oob
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

/*
=================
Netchan_Process
Returns qfalse if the message should not be processed due to being
out of order or a fragment.
Msg must be large enough to hold MAX_MSGLEN, because if this is the
final fragment of a multi-part message, the entire thing will be
copied out.
=================
*/
qboolean Netchan_Process( netchan_t *chan, msg_t *msg )
{
	int sequence;
	int fragmentStart, fragmentLength;
	qboolean fragmented;
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

	// Pop off the qport if we are a server
	if ( chan->sock == NS_SERVER )
	{
		MSG_ReadShort( msg );
	}

	// read the fragment information
	if ( fragmented )
	{
		fragmentStart = MSG_ReadLong( msg );
		fragmentLength = MSG_ReadShort( msg );

	}
	else
	{
		fragmentStart = 0;      // stop warning message
		fragmentLength = 0;
	}

	if ( showpackets->current.boolean )
	{
		if ( fragmented )
		{
			Com_Printf("%s recv %4i : s=%i fragment=%i,%i\n"
			           , netsrcString[ chan->sock ]
			           , msg->cursize
			           , sequence
			           , fragmentStart, fragmentLength );
		}
		else
		{
			Com_Printf("%s recv %4i : s=%i\n"
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
			Com_Printf("%s:Out of order packet %i at %i\n"
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
			Com_Printf("%s:Dropped %i packets at %i\n"
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
				Com_Printf("%s:Dropped a message fragment seq: %d, fragmentStart: %d, chan->fragmentLength: %d\n", NET_AdrToString( chan->remoteAddress ), sequence, fragmentStart, chan->fragmentLength);
			}
			// we can still keep the part that we have so far,
			// so we don't need to clear chan->fragmentLength
			return qfalse;
		}

		// copy the fragment to the fragment buffer
		if ( fragmentLength < 0 || msg->readcount + fragmentLength > msg->cursize
		        || chan->fragmentLength + fragmentLength > (int)sizeof( chan->fragmentBuffer ))
		{
			if ( showdrop->current.boolean || showpackets->current.boolean )
			{
				Com_Printf("%s:illegal fragment length: Current %i Fragment length %i Max %i\n",
				           NET_AdrToString( chan->remoteAddress ), chan->fragmentLength,
				           fragmentLength, sizeof( chan->fragmentBuffer ));
			}
			return qfalse;
		}

		memcpy( chan->fragmentBuffer + chan->fragmentLength,	msg->data + msg->readcount, fragmentLength );

		chan->fragmentLength += fragmentLength;

		// if this wasn't the last fragment, don't process anything
		if ( fragmentLength == FRAGMENT_SIZE )
		{
			return qfalse;
		}

		if ( chan->fragmentLength > msg->maxsize )
		{
			Com_Printf("%s:fragmentLength %i > msg->maxsize\n", NET_AdrToString( chan->remoteAddress ), chan->fragmentLength );
			return qfalse;
		}

		// copy the full message over the partial fragment

		// make sure the sequence number is still there
		*(int *)msg->data = LittleLong( sequence );

		memcpy( msg->data + 4, chan->fragmentBuffer, chan->fragmentLength );
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


/*
=================
Netchan_TransmitNextFragment
Send one fragment of the current message
=================
*/
qboolean Netchan_TransmitNextFragment( netchan_t *chan )
{
	msg_t send;
	qboolean sendsucc;
	byte send_buf[MAX_PACKETLEN];
	int fragmentLength;

	// write the packet header
	MSG_Init( &send, send_buf, sizeof( send_buf ) );                // <-- only do the oob here

	MSG_WriteLong( &send, chan->outgoingSequence | FRAGMENT_BIT );

	// send the qport if we are a client
	if ( chan->sock == NS_CLIENT )
	{
		MSG_WriteShort( &send, g_qport->current.integer );
	}

	// copy the reliable message to the packet first
	fragmentLength = FRAGMENT_SIZE;

	if ( chan->unsentFragmentStart  + fragmentLength > chan->unsentLength )
	{
		fragmentLength = chan->unsentLength - chan->unsentFragmentStart;
	}

	MSG_WriteShort( &send, chan->unsentFragmentStart );
	MSG_WriteShort( &send, fragmentLength );
	MSG_WriteData( &send, chan->unsentBuffer + chan->unsentFragmentStart, fragmentLength );

	// send the datagram
	sendsucc = NET_SendPacket( chan->sock, send.cursize, send.data, chan->remoteAddress );

	if ( showpackets->current.boolean )
	{
		Com_Printf("%s send %4i : s=%i fragment=%i,%i\n"
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

	return sendsucc;
}

/*
===============
Netchan_Transmit
Sends a message to a connection, fragmenting if necessary
A 0 length will still generate a packet.
================
*/
qboolean Netchan_Transmit( netchan_t *chan, int length, const byte *data )
{
	msg_t send;
	qboolean sendsucc;
	byte send_buf[MAX_PACKETLEN];

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

	// write the packet header
	MSG_Init( &send, send_buf, sizeof( send_buf ) );

	MSG_WriteLong( &send, chan->outgoingSequence );
	chan->outgoingSequence++;

	// send the qport if we are a client
	if ( chan->sock == NS_CLIENT )
	{
		MSG_WriteShort( &send, g_qport->current.integer );
	}

	MSG_WriteData( &send, data, length );

	// send the datagram
	sendsucc = NET_SendPacket( chan->sock, send.cursize, send.data, chan->remoteAddress );

	if ( showpackets->current.boolean )
	{
		Com_Printf("%s send %4i : s=%i ack=%i\n"
		           , netsrcString[ chan->sock ]
		           , send.cursize
		           , chan->outgoingSequence - 1
		           , chan->incomingSequence );
	}

	return sendsucc;
}