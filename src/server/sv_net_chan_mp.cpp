#include "../qcommon/qcommon.h"
#include "../qcommon/netchan.h"

#ifdef TESTING_LIBRARY
#define svs (*((serverStatic_t*)( 0x0841FB00 )))
#define sv (*((server_t*)( 0x0842BC80 )))
#else
extern server_t sv;
extern serverStatic_t svs;
#endif

void SV_Netchan_AddOOBProfilePacket(int iLength)
{
	if ( net_profile->current.integer )
	{
		NetProf_PrepProfiling(&svs.netProfilingBuf);
		NetProf_AddPacket(&svs.netProfilingBuf->send, iLength, 0);
	}
}

void SV_Netchan_SendOOBPacket(int iLength, const void *pData, netadr_t to)
{
	if ( *(int *)pData != -1 )
		Com_Printf("SV_Netchan_SendOOBPacket used to send non-OOB packet.\n");

	NetProf_PrepProfiling(&svs.netProfilingBuf);
	NET_SendPacket(NS_SERVER, iLength, pData, to);
	SV_Netchan_AddOOBProfilePacket(iLength);
}

qboolean SV_Netchan_TransmitNextFragment(netchan_t *chan)
{
	return Netchan_TransmitNextFragment(chan);
}

void SV_Netchan_UpdateProfileStats()
{
	UNIMPLEMENTED(__FUNCTION__);
}

void SV_Netchan_PrintProfileStats(qboolean format)
{
	UNIMPLEMENTED(__FUNCTION__);
}