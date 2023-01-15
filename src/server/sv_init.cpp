#include "server.h"

serverStatic_t svs;
server_t sv;

dvar_t *sv_gametype;
dvar_t *sv_mapname;
dvar_t *sv_privateClients;
dvar_t *sv_maxclients;
dvar_t *sv_hostname;
dvar_t *sv_maxRate;
dvar_t *sv_minPing;
dvar_t *sv_maxPing;
dvar_t *sv_floodProtect;
dvar_t *sv_allowAnonymous;
dvar_t *sv_showCommands;
dvar_t *sv_disableClientConsole;
dvar_t *sv_voice;
dvar_t *sv_voiceQuality;
dvar_t *sv_cheats;
dvar_t *sv_serverid;
dvar_t *sv_pure;
dvar_t *sv_iwds;
dvar_t *sv_iwdNames;
dvar_t *sv_referencedIwds;
dvar_t *sv_referencedIwdNames;
dvar_t *rcon_password;
dvar_t *sv_privatePassword;
dvar_t *sv_fps;
dvar_t *sv_timeout;
dvar_t *sv_zombietime;
dvar_t *sv_allowDownload;
dvar_t *sv_reconnectlimit;
dvar_t *sv_padPackets;
dvar_t *sv_allowedClan1;
dvar_t *sv_allowedClan2;
dvar_t *sv_packet_info;
dvar_t *sv_showAverageBPS;
dvar_t *sv_kickBanTime;
dvar_t *sv_mapRotation;
dvar_t *sv_mapRotationCurrent;
dvar_t *sv_debugRate;
dvar_t *sv_debugReliableCmds;
dvar_t *nextmap;
dvar_t *com_expectedHunkUsage;

void SV_AddOperatorCommands()
{
	UNIMPLEMENTED(__FUNCTION__);
}

void SV_Init()
{
	SV_AddOperatorCommands();
	sv_gametype = Dvar_RegisterString("g_gametype", "dm", 0x1024u);
	Dvar_RegisterString("sv_keywords", "", 0x1004u);
	Dvar_RegisterInt("protocol", 115, 115, 115, 0x1044u);
	sv_mapname = Dvar_RegisterString("mapname", "", 0x1044u);
	sv_privateClients = Dvar_RegisterInt("sv_privateClients", 0, 0, 64, 0x1004u);
	sv_maxclients = Dvar_RegisterInt("sv_maxclients", 20, 1, 64, 0x1025u);
	sv_hostname = Dvar_RegisterString("sv_hostname", "CoD2Host", 0x1005u);
	sv_maxRate = Dvar_RegisterInt("sv_maxRate", 0, 0, 25000, 0x1005u);
	sv_minPing = Dvar_RegisterInt("sv_minPing", 0, 0, 999, 0x1005u);
	sv_maxPing = Dvar_RegisterInt("sv_maxPing", 0, 0, 999, 0x1005u);
	sv_floodProtect = Dvar_RegisterBool("sv_floodProtect", 1, 4101);
	sv_allowAnonymous = Dvar_RegisterBool("sv_allowAnonymous", 0, 4100);
	sv_showCommands = Dvar_RegisterBool("sv_showCommands", 0, 4096);
	sv_disableClientConsole = Dvar_RegisterBool("sv_disableClientConsole", 0, 4104);
	sv_voice = Dvar_RegisterBool("sv_voice", 0, 4109);
	sv_voiceQuality = Dvar_RegisterInt("sv_voiceQuality", 1, 0, 9, 0x1008u);
	sv_cheats = Dvar_RegisterBool("sv_cheats", 0, 4120);
	sv_serverid = Dvar_RegisterInt("sv_serverid", 0, 0x80000000, 0x7FFFFFFF, 0x1048u);
	sv_pure = Dvar_RegisterBool("sv_pure", 1, 4108);
	sv_iwds = Dvar_RegisterString("sv_iwds", "", 0x1048u);
	sv_iwdNames = Dvar_RegisterString("sv_iwdNames", "", 0x1048u);
	sv_referencedIwds = Dvar_RegisterString("sv_referencedIwds", "", 0x1048u);
	sv_referencedIwdNames = Dvar_RegisterString("sv_referencedIwdNames", "", 0x1048u);
	rcon_password = Dvar_RegisterString("rcon_password", "", 0x1000u);
	sv_privatePassword = Dvar_RegisterString("sv_privatePassword", "", 0x1000u);
	sv_fps = Dvar_RegisterInt("sv_fps", 20, 10, 1000, 0x1000u);
	sv_timeout = Dvar_RegisterInt("sv_timeout", 240, 0, 1800, 0x1000u);
	sv_zombietime = Dvar_RegisterInt("sv_zombietime", 2, 0, 1800, 0x1000u);
	sv_allowDownload = Dvar_RegisterBool("sv_allowDownload", 1, 4097);
	sv_reconnectlimit = Dvar_RegisterInt("sv_reconnectlimit", 3, 0, 1800, 0x1001u);
	sv_padPackets = Dvar_RegisterInt("sv_padPackets", 0, 0, 0x7FFFFFFF, 0x1000u);
	//*(_BYTE *)(legacyHacks + 221) = 0;
	sv_allowedClan1 = Dvar_RegisterString("sv_allowedClan1", "", 0x1000u);
	sv_allowedClan2 = Dvar_RegisterString("sv_allowedClan2", "", 0x1000u);
	sv_packet_info = Dvar_RegisterBool("sv_packet_info", 0, 4096);
	sv_showAverageBPS = Dvar_RegisterBool("sv_showAverageBPS", 0, 4096);
	sv_kickBanTime = Dvar_RegisterFloat("sv_kickBanTime", 300.0, 0.0, 3600.0, 0x1000u);
	sv_mapRotation = Dvar_RegisterString("sv_mapRotation", "", 0x1000u);
	sv_mapRotationCurrent = Dvar_RegisterString("sv_mapRotationCurrent", "", 0x1000u);
	sv_debugRate = Dvar_RegisterBool("sv_debugRate", 0, 4096);
	sv_debugReliableCmds = Dvar_RegisterBool("sv_debugReliableCmds", 0, 4096);
	nextmap = Dvar_RegisterString("nextmap", "", 0x1000u);
	com_expectedHunkUsage = Dvar_RegisterInt("com_expectedHunkUsage", 0, 0, 0x7FFFFFFF, 0x1040u);
}