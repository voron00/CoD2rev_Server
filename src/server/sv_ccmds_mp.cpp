#include "../qcommon/qcommon.h"

#ifdef TESTING_LIBRARY
#define svs (*((serverStatic_t*)( 0x0841FB00 )))
#define sv (*((server_t*)( 0x0842BC80 )))
#else
extern serverStatic_t svs;
extern server_t sv;
#endif

const char *SV_GetMapBaseName(const char *mapname)
{
	return FS_GetMapBaseName(mapname);
}

void SV_Heartbeat_f(void)
{
	svs.nextHeartbeatTime = 0x80000000;
}

int SV_KickClient(client_s *cl, char *playerName, int maxPlayerNameLen)
{
	int guid;

	if ( cl->netchan.remoteAddress.type == NA_LOOPBACK )
	{
		SV_SendServerCommand(0, 0, "%c \"EXE_CANNOTKICKHOSTPLAYER\"", 101);
		return 0;
	}
	else
	{
		if ( playerName )
		{
			I_strncpyz(playerName, cl->name, maxPlayerNameLen);
			I_CleanStr(playerName);
		}

		guid = cl->guid;
		SV_DropClient(cl, "EXE_PLAYERKICKED");
		cl->lastPacketTime = svs.time;

		return guid;
	}
}

client_s *SV_GetPlayerByName()
{
	char cleanName[64];
	const char *name;
	int i;
	client_s *client;

	if ( !com_sv_running->current.boolean )
		return 0;

	if ( SV_Cmd_Argc() > 1 )
	{
		name = SV_Cmd_Argv(1u);
		i = 0;
		client = svs.clients;

		while ( i < sv_maxclients->current.integer )
		{
			if ( client->state )
			{
				if ( !I_stricmp(client->name, name) )
					return client;

				I_strncpyz(cleanName, client->name, sizeof(cleanName));
				I_CleanStr(cleanName);

				if ( !I_stricmp(cleanName, name) )
					return client;
			}

			++i;
			++client;
		}

		Com_Printf("Player %s is not on the server\n", name);
		return 0;
	}
	else
	{
		Com_Printf("No player specified.\n");
		return 0;
	}
}

int SV_KickUser_f(char *playerName, int maxPlayerNameLen)
{
	int i;
	client_s *client;

	if ( com_sv_running->current.boolean )
	{
		if ( SV_Cmd_Argc() == 2 )
		{
			client = SV_GetPlayerByName();

			if ( client )
			{
				return SV_KickClient(client, playerName, maxPlayerNameLen);
			}
			else
			{
				if ( !I_stricmp(SV_Cmd_Argv(1u), "all") )
				{
					i = 0;
					client = svs.clients;

					while ( i < sv_maxclients->current.integer )
					{
						if ( client->state )
							SV_KickClient(client, 0, 0);

						++i;
						++client;
					}
				}

				return 0;
			}
		}
		else
		{
			Com_Printf("Usage: %s <player name>\n%s all = kick everyone\n", SV_Cmd_Argv(0), SV_Cmd_Argv(0));
			return 0;
		}
	}
	else
	{
		Com_Printf("Server is not running.\n");
		return 0;
	}
}

void SV_Drop_f(void)
{
	SV_KickUser_f(0, 0);
}

void SV_Ban_f(void)
{
	client_s *client;

	if ( com_sv_running->current.boolean )
	{
		if ( SV_Cmd_Argc() == 2 )
		{
			client = SV_GetPlayerByName();

			if ( client )
				SV_BanClient(client);
		}
		else
		{
			Com_Printf("Usage: banUser <player name>\n");
		}
	}
	else
	{
		Com_Printf("Server is not running.\n");
	}
}

client_s *SV_GetPlayerByNum()
{
	const char *num;
	int idnum;
	int i;

	if ( !com_sv_running->current.boolean )
		return 0;

	if ( SV_Cmd_Argc() > 1 )
	{
		num = SV_Cmd_Argv(1u);

		for ( i = 0; num[i]; ++i )
		{
			if ( num[i] <= 47 || num[i] > 57 )
			{
				Com_Printf("Bad slot number: %s\n", num);
				return 0;
			}
		}

		idnum = atoi(num);

		if ( idnum >= 0 && idnum < sv_maxclients->current.integer )
		{
			if ( svs.clients[idnum].state )
			{
				return &svs.clients[idnum];
			}
			else
			{
				Com_Printf("Client %i is not active\n", idnum);
				return 0;
			}
		}
		else
		{
			Com_Printf("Bad client slot: %i\n", idnum);
			return 0;
		}
	}
	else
	{
		Com_Printf("No player specified.\n");
		return 0;
	}
}

void SV_BanNum_f(void)
{
	client_s *client;

	if ( com_sv_running->current.boolean )
	{
		if ( SV_Cmd_Argc() == 2 )
		{
			client = SV_GetPlayerByNum();

			if ( client )
				SV_BanClient(client);
		}
		else
		{
			Com_Printf("Usage: banClient <client number>\n");
		}
	}
	else
	{
		Com_Printf("Server is not running.\n");
	}
}

void SV_TempBan_f(void)
{
	char name[64];
	int guid;

	guid = SV_KickUser_f(name, sizeof(name));

	if ( guid )
	{
		Com_Printf("%s (guid %i) was kicked for cheating\n", name, guid);
		SV_BanGuidBriefly(guid);
	}
}

void SV_Unban_f(void)
{
	if ( SV_Cmd_Argc() == 2 )
	{
		SV_UnbanClient(SV_Cmd_Argv(1u));
	}
	else
	{
		Com_Printf("Usage: unban <client name>\n");
	}
}

int SV_KickClient_f(char *playerName, int maxPlayerNameLen)
{
	client_s *client;

	if ( com_sv_running->current.boolean )
	{
		if ( SV_Cmd_Argc() == 2 )
		{
			client = SV_GetPlayerByNum();

			if ( client )
				return SV_KickClient(client, playerName, maxPlayerNameLen);
			else
				return 0;
		}
		else
		{
			Com_Printf("Usage: %s <client number>\n", SV_Cmd_Argv(0));
			return 0;
		}
	}
	else
	{
		Com_Printf("Server is not running.\n");
		return 0;
	}
}

void SV_DropNum_f(void)
{
	SV_KickClient_f(0, 0);
}

extern dvar_t *sv_mapname;
void SV_Status_f()
{
	int score;
	size_t len;
	int j;
	int k;
	client_t *client;
	int l;
	const char *s;
	int i;

	if ( com_sv_running->current.boolean )
	{
		Com_Printf("map: %s\n", sv_mapname->current.string);
		Com_Printf("num score ping guid   name            lastmsg address               qport rate\n");
		Com_Printf("--- ----- ---- ------ --------------- ------- --------------------- ----- -----\n");

		i = 0;
		client = svs.clients;

		while ( i < sv_maxclients->current.integer )
		{
			if ( client->state )
			{
				Com_Printf("%3i ", i);
				SV_GameClientNum(i);
				score = G_GetClientScore(client - svs.clients);
				Com_Printf("%5i ", score);

				if ( client->state == CS_CONNECTED )
				{
					Com_Printf("CNCT ");
				}
				else if ( client->state == CS_ZOMBIE )
				{
					Com_Printf("ZMBI ");
				}
				else if ( client->ping >= 9999 )
				{
					Com_Printf("%4i ", 9999);
				}
				else
				{
					Com_Printf("%4i ", client->ping);
				}

				Com_Printf("%6i ", client->guid);
				Com_Printf("%s^7", client->name);

				l = 16 - I_DrawStrlen(client->name);

				for ( j = 0; j < l; ++j )
					Com_Printf(" ");

				Com_Printf("%7i ", svs.time - client->lastPacketTime);
				s = NET_AdrToString(client->netchan.remoteAddress);
				Com_Printf("%s", s);

				len = strlen(s);

				for ( k = 0; k < (int)(22 - len); ++k )
					Com_Printf(" ");

				Com_Printf("%6i", client->netchan.qport);
				Com_Printf(" %5i", client->rate);
				Com_Printf("\n");
			}

			++i;
			++client;
		}

		Com_Printf("\n");
	}
	else
	{
		Com_Printf("Server is not running.\n");
	}
}

void SV_Serverinfo_f(void)
{
	Com_Printf("Server info settings:\n");
	Info_Print(Dvar_InfoString(0x404u));
}

void SV_Systeminfo_f(void)
{
	Com_Printf("System info settings:\n");
	Info_Print(Dvar_InfoString(8u));
}

void SV_DumpUser_f(void)
{
	client_s *client;

	if ( com_sv_running->current.boolean )
	{
		if ( SV_Cmd_Argc() == 2 )
		{
			client = SV_GetPlayerByName();

			if ( client )
			{
				Com_Printf("userinfo\n");
				Com_Printf("--------\n");
				Info_Print(client->userinfo);
			}
		}
		else
		{
			Com_Printf("Usage: info <userid>\n");
		}
	}
	else
	{
		Com_Printf("Server is not running.\n");
	}
}