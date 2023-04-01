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

/*
==================
SV_WriteDownloadToClient
Check to see if the client wants a file, open it if needed and start pumping the client
Fill up msg with data
==================
*/
extern dvar_t *sv_allowDownload;
extern dvar_t *sv_maxRate;
extern dvar_t *sv_pure;
void SV_WriteDownloadToClient( client_t *cl, msg_t *msg )
{
	int curindex;
	int rate;
	int blockspersnap;
	int iwdFile;
	char errorMessage[1024];

	if (!*cl->downloadName)
		return;	// Nothing being downloaded

	if (!cl->download)
	{
		// We open the file here

		Com_Printf( "clientDownload: %d : begining \"%s\"\n", cl - svs.clients, cl->downloadName );

		iwdFile = FS_iwIwd(cl->downloadName, "main");

		if ( !sv_allowDownload->current.integer || iwdFile ||
		        ( cl->downloadSize = FS_SV_FOpenFileRead( cl->downloadName, &cl->download ) ) <= 0 )
		{
			// cannot auto-download file
			if (iwdFile)
			{
				Com_Printf("clientDownload: %d : \"%s\" cannot download iwd files\n", cl - svs.clients, cl->downloadName);
				Com_sprintf(errorMessage, sizeof(errorMessage), "EXE_CANTAUTODLGAMEIWD\x15%s", cl->downloadName);
			}
			else if ( !sv_allowDownload->current.integer )
			{
				Com_Printf("clientDownload: %d : \"%s\" download disabled", cl - svs.clients, cl->downloadName);
				if (sv_pure->current.integer)
				{
					Com_sprintf(errorMessage, sizeof(errorMessage), "EXE_AUTODL_SERVERDISABLED_PURE\x15%s", cl->downloadName);
				}
				else
				{
					Com_sprintf(errorMessage, sizeof(errorMessage), "EXE_AUTODL_SERVERDISABLED\x15%s", cl->downloadName);
				}
			}
			else
			{
				// NOTE TTimo this is NOT supposed to happen unless bug in our filesystem scheme?
				//   if the iwd is referenced, it must have been found somewhere in the filesystem
				Com_Printf("clientDownload: %d : \"%s\" file not found on server\n", cl - svs.clients, cl->downloadName);
				Com_sprintf(errorMessage, sizeof(errorMessage), "EXE_AUTODL_FILENOTONSERVER\x15%s", cl->downloadName);
			}
			MSG_WriteByte( msg, svc_download );
			MSG_WriteShort( msg, 0 ); // client is expecting block zero
			MSG_WriteLong( msg, -1 ); // illegal file size
			MSG_WriteString( msg, errorMessage );

			*cl->downloadName = 0;
			return;
		}

		// Init
		cl->downloadCurrentBlock = cl->downloadClientBlock = cl->downloadXmitBlock = 0;
		cl->downloadCount = 0;
		cl->downloadEOF = qfalse;
	}

	// Perform any reads that we need to
	while (cl->downloadCurrentBlock - cl->downloadClientBlock < MAX_DOWNLOAD_WINDOW &&
	        cl->downloadSize != cl->downloadCount)
	{

		curindex = (cl->downloadCurrentBlock % MAX_DOWNLOAD_WINDOW);

		if (!cl->downloadBlocks[curindex])
			cl->downloadBlocks[curindex] = (unsigned char *)Z_Malloc( MAX_DOWNLOAD_BLKSIZE );

		cl->downloadBlockSize[curindex] = FS_Read( cl->downloadBlocks[curindex], MAX_DOWNLOAD_BLKSIZE, cl->download );

		if (cl->downloadBlockSize[curindex] < 0)
		{
			// EOF right now
			cl->downloadCount = cl->downloadSize;
			break;
		}

		cl->downloadCount += cl->downloadBlockSize[curindex];

		// Load in next block
		cl->downloadCurrentBlock++;
	}

	// Check to see if we have eof condition and add the EOF block
	if (cl->downloadCount == cl->downloadSize &&
	        !cl->downloadEOF &&
	        cl->downloadCurrentBlock - cl->downloadClientBlock < MAX_DOWNLOAD_WINDOW)
	{

		cl->downloadBlockSize[cl->downloadCurrentBlock % MAX_DOWNLOAD_WINDOW] = 0;
		cl->downloadCurrentBlock++;

		cl->downloadEOF = qtrue;  // We have added the EOF block
	}

	// Loop up to window size times based on how many blocks we can fit in the
	// client snapMsec and rate

	// based on the rate, how many bytes can we fit in the snapMsec time of the client
	// normal rate / snapshotMsec calculation
	rate = cl->rate;
	if ( sv_maxRate->current.integer )
	{
		if ( sv_maxRate->current.integer < 1000 )
		{
			Dvar_SetInt( sv_maxRate, 1000 );
		}
		if ( sv_maxRate->current.integer < rate )
		{
			rate = sv_maxRate->current.integer;
		}
	}

	/*
	if (!rate)
	{
		blockspersnap = 1;
	}
	else
	{
		blockspersnap = ( (rate * cl->snapshotMsec) / 1000 + MAX_DOWNLOAD_BLKSIZE ) /
		                MAX_DOWNLOAD_BLKSIZE;
	}

	if (blockspersnap < 0)
		blockspersnap = 1;
	*/

	blockspersnap = 1;

	while (blockspersnap--)
	{

		// Write out the next section of the file, if we have already reached our window,
		// automatically start retransmitting

		if (cl->downloadClientBlock == cl->downloadCurrentBlock)
			return; // Nothing to transmit

		if (cl->downloadXmitBlock == cl->downloadCurrentBlock)
		{
			// We have transmitted the complete window, should we start resending?

			//FIXME:  This uses a hardcoded one second timeout for lost blocks
			//the timeout should be based on client rate somehow
			if (svs.time - cl->downloadSendTime > 1000)
				cl->downloadXmitBlock = cl->downloadClientBlock;
			else
				return;
		}

		// Send current block
		curindex = (cl->downloadXmitBlock % MAX_DOWNLOAD_WINDOW);

		MSG_WriteByte( msg, svc_download );
		MSG_WriteShort( msg, cl->downloadXmitBlock );

		// block zero is special, contains file size
		if ( cl->downloadXmitBlock == 0 )
			MSG_WriteLong( msg, cl->downloadSize );

		MSG_WriteShort( msg, cl->downloadBlockSize[curindex] );

		// Write the block
		if ( cl->downloadBlockSize[curindex] )
		{
			MSG_WriteData( msg, cl->downloadBlocks[curindex], cl->downloadBlockSize[curindex] );
		}

		Com_DPrintf( "clientDownload: %d : writing block %d\n", cl - svs.clients, cl->downloadXmitBlock );

		// Move on to the next block
		// It will get sent with next snap shot.  The rate will keep us in line.
		cl->downloadXmitBlock++;

		cl->downloadSendTime = svs.time;
	}
}