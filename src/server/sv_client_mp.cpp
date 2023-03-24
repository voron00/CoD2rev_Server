#include "../qcommon/qcommon.h"
#include "../qcommon/netchan.h"

#ifdef TESTING_LIBRARY
#define svs (*((serverStatic_t*)( 0x0841FB00 )))
#define sv (*((server_t*)( 0x0842BC80 )))
#else
extern serverStatic_t svs;
extern server_t sv;
#endif

void SV_DelayDropClient(client_t *client, const char *dropmsg)
{
	if ( client->state != CS_ZOMBIE && client->delayDropMsg == NULL)
	{
		client->delayDropMsg = dropmsg;
	}
}

qboolean SV_IsBannedGuid(int guid)
{
	char *file;
	int banned;
	const char *token;
	const char *text;

	if ( !guid )
		return 0;

	if ( FS_ReadFile("ban.txt", (void **)&file) < 0 )
		return 0;

	text = file;
	banned = 0;

	while ( 1 )
	{
		token = Com_Parse(&text);

		if ( !*token )
			break;

		if ( atoi(token) == guid )
		{
			banned = 1;
			break;
		}

		Com_SkipRestOfLine(&text);
	}

	FS_FreeFile(file);
	return banned;
}

extern dvar_t *sv_kickBanTime;
qboolean SV_IsTempBannedGuid(int guid)
{
	unsigned int banSlot;

	if ( !guid )
		return 0;

	for ( banSlot = 0; banSlot < 16; ++banSlot )
	{
		if ( svs.bans[banSlot].guid == guid
		        && (float)(svs.time - svs.bans[banSlot].time) <= (float)(sv_kickBanTime->current.decimal * 1000.0) )
		{
			return 1;
		}
	}

	return 0;
}

void SV_AuthorizeIpPacket( netadr_t from )
{
	int challenge;
	int i;
	const char    *s;
	const char    *r;
	char ret[1024];

	if ( !NET_CompareBaseAdr( from, svs.authorizeAddress ) )
	{
		Com_Printf( "SV_AuthorizeIpPacket: not from authorize server\n" );
		return;
	}

	challenge = atoi( SV_Cmd_Argv( 1 ) );

	for ( i = 0 ; i < MAX_CHALLENGES ; i++ )
	{
		if ( svs.challenges[i].challenge == challenge )
		{
			break;
		}
	}

	if ( i == MAX_CHALLENGES )
	{
		Com_Printf( "SV_AuthorizeIpPacket: challenge not found\n" );
		return;
	}

	// send a packet back to the original client
	svs.challenges[i].pingTime = svs.time;

	s = SV_Cmd_Argv( 2 );
	r = SV_Cmd_Argv( 3 );

	if ( !Q_stricmp( s, "demo" ) )
	{
		if ( Dvar_GetBool( "fs_restrict" ) )
		{
			// a demo client connecting to a demo server
			NET_OutOfBandPrint( NS_SERVER, svs.challenges[i].adr,
			                    va("challengeResponse %i", svs.challenges[i].challenge) );
			return;
		}
		// they are a demo client trying to connect to a real server
		NET_OutOfBandPrint( NS_SERVER, svs.challenges[i].adr, "error\nEXE_ERR_NOT_A_DEMO_SERVER" );
		// clear the challenge record so it won't timeout and let them through
		memset( &svs.challenges[i], 0, sizeof( svs.challenges[i] ) );
		return;
	}

	if ( !Q_stricmp( s, "accept" ) )
	{
		svs.challenges[i].guid = atoi(SV_Cmd_Argv( 4 ));

		if ( SV_IsBannedGuid(svs.challenges[i].guid) )
		{
			Com_Printf("rejected connection from permanently banned GUID %i\n", svs.challenges[i].guid);
			// they are a demo client trying to connect to a real server
			NET_OutOfBandPrint( NS_SERVER, svs.challenges[i].adr, "error\n\x15You are permanently banned from this server" );
			// clear the challenge record so it won't timeout and let them through
			memset( &svs.challenges[i], 0, sizeof( svs.challenges[i] ) );
			return;
		}

		if ( SV_IsTempBannedGuid(svs.challenges[i].guid) )
		{
			Com_Printf("rejected connection from temporarily banned GUID %i\n", svs.challenges[i].guid);
			// they are a demo client trying to connect to a real server
			NET_OutOfBandPrint( NS_SERVER, svs.challenges[i].adr, "error\n\x15You are temporarily banned from this server" );
			// clear the challenge record so it won't timeout and let them through
			memset( &svs.challenges[i], 0, sizeof( svs.challenges[i] ) );
			return;
		}

		if ( !svs.challenges[i].connected )
		{
			NET_OutOfBandPrint( NS_SERVER, svs.challenges[i].adr,
			                    va("challengeResponse %i", svs.challenges[i].challenge) );
			return;
		}
	}

	if ( !Q_stricmp( s, "deny" ) )
	{
		if ( r && r[0] )
		{
			sprintf(ret, "error\n%s", r);
			NET_OutOfBandPrint(NS_SERVER, svs.challenges[i].adr, ret);
			memset( &svs.challenges[i], 0, sizeof( svs.challenges[i] ) );
			return;
		}
		else
		{
			NET_OutOfBandPrint(NS_SERVER, svs.challenges[i].adr, "error\nEXE_ERR_BAD_CDKEY");
			memset( &svs.challenges[i], 0, sizeof( svs.challenges[i] ) );
			return;
		}
	}

	if ( r && r[0] )
	{
		if ( !Q_stricmp(r, "CLIENT_UNKNOWN_TO_AUTH") || !Q_stricmp(r, "BAD_CDKEY") )
		{
			NET_OutOfBandPrint(NS_SERVER, svs.challenges[i].adr, "needcdkey");
			memset( &svs.challenges[i], 0, sizeof( svs.challenges[i] ) );
			return;
		}

		if ( !Q_stricmp(r, "INVALID_CDKEY") )
		{
			Q_stricmp(r, "BANNED_CDKEY");
			NET_OutOfBandPrint(NS_SERVER, svs.challenges[i].adr, "error\nEXE_ERR_BAD_CDKEY");
			memset( &svs.challenges[i], 0, sizeof( svs.challenges[i] ) );
			return;
		}
	}
	else
	{
		NET_OutOfBandPrint(NS_SERVER, svs.challenges[i].adr, "error\nEXE_ERR_CDKEY_IN_USE");
		memset( &svs.challenges[i], 0, sizeof( svs.challenges[i] ) );
		return;
	}

	// clear the challenge record so it won't timeout and let them through
	memset( &svs.challenges[i], 0, sizeof( svs.challenges[i] ) );
}

void SV_UserinfoChanged( client_t *cl )
{
	const char	*val;
	int		i;

	// name for C code
	Q_strncpyz( cl->name, Info_ValueForKey (cl->userinfo, "name"), sizeof(cl->name) );

	// rate command

	// if the client is on the same subnet as the server and we aren't running an
	// internet public server, assume they don't need a rate choke
	if ( Sys_IsLANAddress( cl->netchan.remoteAddress ) && com_dedicated->current.integer != 2 )
	{
		cl->rate = 99999;	// lans should not rate limit
	}
	else
	{
		val = Info_ValueForKey (cl->userinfo, "rate");
		if (strlen(val))
		{
			i = atoi(val);
			cl->rate = i;
			if (cl->rate < 1000)
			{
				cl->rate = 1000;
			}
			else if (cl->rate > 90000)
			{
				cl->rate = 90000;
			}
		}
		else
		{
			cl->rate = 3000;
		}
	}

	// snaps command
	val = Info_ValueForKey (cl->userinfo, "snaps");
	if (strlen(val))
	{
		i = atoi(val);
		if ( i < 1 )
		{
			i = 1;
		}
		else if ( i > 30 )
		{
			i = 30;
		}
		cl->snapshotMsec = 1000/i;
	}
	else
	{
		cl->snapshotMsec = 50;
	}

	// voice command
	val = Info_ValueForKey (cl->userinfo, "cl_voice");
	cl->hasVoip = atoi(val) > 0;
	if ( cl->rate < 5000 )
		cl->hasVoip = 0;
}