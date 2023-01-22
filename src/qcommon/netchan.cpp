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

void Net_DumpProfile_f(void)
{
	UNIMPLEMENTED(__FUNCTION__);
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
	Cmd_AddCommand("net_dumpprofile", Net_DumpProfile_f);
}