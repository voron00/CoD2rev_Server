#pragma once

#include "../qcommon/cmd.h"
#include "../qcommon/netchan.h"
#include "../qcommon/qcommon.h"
#include "../game/g_shared.h"

// MAX_CHALLENGES is made large to prevent a denial
// of service attack that could cycle all of them
// out before legitimate users connected
#define MAX_CHALLENGES  1024

#define MAX_MASTER_SERVERS  5
#define AUTHORIZE_TIMEOUT 5000

#define BAN_LIST_NAME "ban.txt"

// Snapshot limits. Possibly calculated from max clients/ents somehow?
#define ARCHIVED_SNAPSHOT_BUFFER_SIZE  33554432
#define CACHED_SNAPSHOT_ENTITY_SIZE    16384
#define CACHED_SNAPSHOT_CLIENT_SIZE    4096
#define NUM_ARCHIVED_FRAMES            1200
#define NUM_CACHED_FRAMES              512

#define MAX_TOTAL_ENT_LEAFS            128

extern dvar_t *sv_gametype;
extern dvar_t *sv_mapRotation;
extern dvar_t *sv_mapRotationCurrent;
extern dvar_t *sv_serverid;
extern dvar_t *sv_mapname;
extern dvar_t *sv_reconnectlimit;
extern dvar_t *sv_minPing;
extern dvar_t *sv_maxPing;
extern dvar_t *sv_privatePassword;
extern dvar_t *sv_privateClients;
extern dvar_t *sv_kickBanTime;
extern dvar_t *sv_allowDownload;
extern dvar_t *sv_maxRate;
extern dvar_t *sv_pure;
extern dvar_t *sv_showCommands;
extern dvar_t *sv_floodProtect;
extern dvar_t *rcon_password;
extern dvar_t *sv_timeout;
extern dvar_t *sv_zombietime;
extern dvar_t *sv_hostname;
extern dvar_t *sv_allowAnonymous;
extern dvar_t *sv_disableClientConsole;
extern dvar_t *sv_voice;
extern dvar_t *sv_packet_info;
extern dvar_t *sv_paused;
extern dvar_t *sv_fps;
extern dvar_t *sv_cheats;
extern dvar_t *sv_debugReliableCmds;
extern dvar_t *sv_showAverageBPS;
extern dvar_t *sv_padPackets;

extern dvar_t *sv_wwwDownload;
extern dvar_t *sv_wwwBaseURL;
extern dvar_t *sv_wwwDlDisconnected;

extern qboolean gameInitialized;

enum
{
	CS_FREE,
	CS_ZOMBIE,
	CS_CONNECTED,
	CS_PRIMED,
	CS_ACTIVE
};

typedef struct
{
	netadr_t adr;
	int challenge;
	int time;
	int pingTime;
	int firstTime;
	int firstPing;
	qboolean connected;
	int guid;
	char PBguid[33];
	char clientPBguid[33];
} challenge_t;

typedef struct
{
	playerState_t ps;
	int	num_entities;
	int	num_clients;
	int	first_entity;
	int	first_client;
	int messageSent;
	int messageAcked;
	int	messageSize;
} clientSnapshot_t;

#define MAX_VOICE_PACKET_DATA 256
#define MAX_VOICE_PACKETS 40
#pragma pack(push)
#pragma pack(1)
typedef struct
{
	byte talker;
	byte data[MAX_VOICE_PACKET_DATA];
	int dataSize;
} VoicePacket_t;
#pragma pack(pop)

enum svscmd_type
{
	SV_CMD_CAN_IGNORE,
	SV_CMD_RELIABLE
};

struct svscmd_info_t
{
	char cmd[MAX_STRING_CHARS];
	int time;
	int type;
};

typedef struct client_s
{
	int				state;
	int				sendAsActive;
	const char		*dropReason;
	char			userinfo[MAX_INFO_STRING];
	svscmd_info_t	reliableCommandInfo[MAX_RELIABLE_COMMANDS];
	int				reliableSequence;
	int				reliableAcknowledge;
	int				reliableSent;
	int				messageAcknowledge;
	int				gamestateMessageNum;
	int				challenge;
	usercmd_t  	 	lastUsercmd;
	int				lastClientCommand;
	char			lastClientCommandString[MAX_STRING_CHARS];
	gentity_t 		*gentity;
	char 			name[MAX_NAME_LENGTH];
	char			downloadName[MAX_QPATH];
	fileHandle_t	download;
	int				downloadSize;
	int				downloadCount;
	int				downloadClientBlock;
	int				downloadCurrentBlock;
	int				downloadXmitBlock;
	unsigned char	*downloadBlocks[MAX_DOWNLOAD_WINDOW];
	int				downloadBlockSize[MAX_DOWNLOAD_WINDOW];
	qboolean		downloadEOF;
	int				downloadSendTime;
	char			downloadURL[MAX_OSPATH];
	qboolean		wwwOk;
	qboolean		downloadingWWW;
	qboolean		clientDownloadingWWW;
	qboolean		wwwFallback;
	int				deltaMessage;
	int				nextReliableTime;
	int				lastPacketTime;
	int				lastConnectTime;
	int				nextSnapshotTime;
	qboolean		rateDelayed;
	int				timeoutCount;
	clientSnapshot_t frames[PACKET_BACKUP];
	int				ping;
	int				rate;
	int				snapshotMsec;
	int				pureAuthentic;
	netchan_t		netchan;
	int 			guid;
	unsigned short	scriptId;
	qboolean		bIsTestClient;
	int				serverId;
	VoicePacket_t	voicePackets[MAX_VOICE_PACKETS];
	int				voicePacketCount;
	bool			muteList[MAX_CLIENTS];
	bool			sendVoice;
	char PBguid[33];
	char clientPBguid[33];
} client_t;
/*
#if defined(__i386__)
static_assert((sizeof(client_t) == 0x78F14), "ERROR: client_t size is invalid!");
#endif
*/

typedef struct archivedSnapshot_s
{
	int start;
	int size;
} archivedSnapshot_t;

typedef struct cachedClient_s
{
	int playerStateExists;
	clientState_t cs;
	playerState_t ps;
} cachedClient_t;

typedef struct cachedSnapshot_s
{
	int archivedFrame;
	int time;
	int num_entities;
	int first_entity;
	int num_clients;
	int first_client;
	int usesDelta;
} cachedSnapshot_t;

typedef struct archivedEntityShared_s
{
	int svFlags;
	int clientMask[2];
	vec3_t absmin;
	vec3_t absmax;
} archivedEntityShared_t;

typedef struct archivedEntity_s
{
	entityState_t s;
	archivedEntityShared_t r;
} archivedEntity_t;

#define MAX_TEMPBAN_SLOTS 16

struct tempBanSlot_t
{
	int guid;
	int banTime;
};

typedef struct
{
	qboolean initialized;
	int time;
	int snapFlagServerBit;
	client_t *clients;
	int numSnapshotEntities;
	int numSnapshotClients;
	int nextSnapshotEntities;
	int nextSnapshotClients;
	entityState_t *snapshotEntities;
	clientState_t *snapshotClients;
	int archiveEnabled;
	int nextArchivedSnapshotFrames;
	archivedSnapshot_t *archivedSnapshotFrames;
	byte *archivedSnapshotBuffer;
	int nextArchivedSnapshotBuffer;
	int nextCachedSnapshotEntities;
	int nextCachedSnapshotClients;
	int nextCachedSnapshotFrames;
	archivedEntity_t *cachedSnapshotEntities;
	cachedClient_t *cachedSnapshotClients;
	cachedSnapshot_t *cachedSnapshotFrames;
	int nextHeartbeatTime;
	int nextStatusResponseTime;
	challenge_t challenges[MAX_CHALLENGES];
	netadr_t redirectAddress;
	netadr_t authorizeAddress;
	netProfileInfo_t *pOOBProf;
	tempBanSlot_t tempBans[MAX_TEMPBAN_SLOTS];
} serverStatic_t;
/*
#if defined(__i386__)
static_assert((sizeof(serverStatic_t) == 0xC108), "ERROR: serverStatic_t size is invalid!");
#endif
*/

extern serverStatic_t svs;

typedef enum
{
	SS_DEAD,
	SS_LOADING,
	SS_GAME
} serverState_t;

#define MAX_ENT_CLUSTERS    16

#define MAX_BPS_WINDOW      20          // NERVE - SMF - net debugging

typedef struct svEntity_s
{
	uint16_t worldSector;
	uint16_t nextEntityInWorldSector;
	archivedEntity_t baseline;
	int numClusters;
	int clusternums[MAX_ENT_CLUSTERS];
	int	lastCluster;
	int	linkcontents;
	float linkmin[2];
	float linkmax[2];
} svEntity_t;
#if defined(__i386__)
static_assert((sizeof(svEntity_t) == 0x174), "ERROR: svEntity_t size is invalid!");
#endif

typedef struct
{
	serverState_t state;
	qboolean restarting;
	int start_frameTime;
	int	checksumFeed;
	int timeResidual;
	int nextFrameTime;
	struct cmodel_s *models[MAX_MODELS];
	char *configstrings[MAX_CONFIGSTRINGS];
	svEntity_t svEntities[MAX_GENTITIES];
	const char *entityParsePoint;
	gentity_t *gentities;
	int gentitySize;
	int	num_entities;
	playerState_t *gameClients;
	int gameClientSize;
	int	skelTimeStamp;
	int	skelMemPos;
	int	bpsWindow[MAX_BPS_WINDOW];
	int	bpsWindowSteps;
	int	bpsTotalBytes;
	int	bpsMaxBytes;
	int	ubpsWindow[MAX_BPS_WINDOW];
	int	ubpsTotalBytes;
	int	ubpsMaxBytes;
	float ucompAve;
	int	ucompNum;
	char gametype[MAX_QPATH];
} server_t;
#if defined(__i386__)
static_assert((sizeof(server_t) == 0x5F534), "ERROR: server_t size is invalid!");
#endif

extern server_t sv;

#define MAX_SNAPSHOT_ENTITIES	1024
struct snapshotEntityNumbers_t
{
	int numSnapshotEntities;
	int snapshotEntities[MAX_SNAPSHOT_ENTITIES];
};

typedef struct leakyBucket_s leakyBucket_t;
struct leakyBucket_s
{
	int type;
	unsigned char adr[4];
	uint64_t lastTime;
	signed char	burst;
	long hash;
	leakyBucket_t *prev, *next;
};

extern int sv_serverId_value;

extern dvar_t *nextmap;
extern dvar_t *sv_maxclients;

void SV_Init();
void SV_PacketEvent( netadr_t from, msg_t *msg );
void SV_Frame(int msec);
void SV_Shutdown( const char* finalmsg );
void SV_ShutdownGameProgs();
void SV_Netchan_AddOOBProfilePacket(int iLength);
void SV_Netchan_SendOOBPacket(int iLength, const void *pData, netadr_t to);
bool SV_Netchan_TransmitNextFragment(netchan_t *chan);
bool SV_Netchan_Transmit(client_t *client, byte *data, int length);
void SV_Netchan_Encode( client_t *client, byte *data, int cursize );
void SV_Netchan_Decode( client_t *client, byte *data, int remaining );
void SV_AddServerCommand(client_t *client, int type, const char *cmd);
void SV_DelayDropClient(client_t *client, const char *dropmsg);
char *SV_ExpandNewlines( char *in );
void SV_SendServerCommand( client_t *cl, int type, const char *fmt, ... );
void SV_SetConfig(int start, int max, unsigned short bit);
void SV_SetConfigValueForKey(int start, int max, const char *key, const char *value);
void SV_SetConfigstring(unsigned int index, const char *val);
void SV_GetConfigstring( unsigned int index, char *buffer, int bufferSize );
const char* SV_GetConfigstringConst(unsigned int index);
void SV_GetUserinfo( int index, char *buffer, int bufferSize );
void SV_SetUserinfo( int index, const char *val );
void SV_FreeClients();

void SV_AuthorizeIpPacket( netadr_t from );
void SV_Heartbeat_f(void);
bool SV_Loaded();
void SV_FreeClientScriptId(client_s *cl);
void SV_DropClient(client_s *drop, const char *reason);
void SV_GameDropClient(int clientNum, const char *reason);
void SV_GetUsercmd(int clientNum, usercmd_s *cmd);
int SV_GetGuid(int clientNum);
void SV_BanGuidBriefly(int guid);
void SV_BanClient(client_t *cl);
void SV_UnbanClient(const char *name);

void SV_UserMove( client_t *cl, msg_t *msg, qboolean delta );
void SV_BotUserMove(client_s *cl);
void SV_CalcPings( void );
void SV_CheckTimeouts( void );
void SV_SetGametype();
gentity_t *SV_AddTestClient();
void SV_FreeClient(client_s *cl);

void SV_SendMessageToClient( msg_t *msg, client_t *client );
void SV_UpdateServerCommandsToClient( client_t *client, msg_t *msg );
void SV_SendClientGameState( client_t *client );
void SV_DirectConnect( netadr_t from );
void SVC_RemoteCommand( netadr_t from, msg_t *msg );

bool SVC_RateLimit( leakyBucket_t *bucket, int burst, int period );
bool SVC_RateLimitAddress( netadr_t from, int burst, int period );

void SV_SendClientSnapshot(client_s *client);
void SV_ExecuteClientMessage(client_s *cl, msg_t *msg);
void SV_PacketEvent( netadr_t from, msg_t *msg );
void SV_ArchiveSnapshot();
void SV_EnableArchivedSnapshot(qboolean enabled);

void SV_ResetEntityParsePoint();

qboolean SV_IsLocalClient(int clientNum);
qboolean SV_MapExists(const char *name);
bool hook_SV_MapExists(const char *mapname);
const char *SV_GetMapBaseName(const char *mapname);
XModel* SV_XModelGet(const char *name);
int SV_NumForGentity( gentity_t *ent );
gentity_t *SV_GentityNum( int num );
playerState_t *SV_GameClientNum( int num );
int playerstateToClientNum(playerState_t* ps);
svEntity_t  *SV_SvEntityForGentity( gentity_t *gEnt );
gentity_t *SV_GEntityForSvEntity( svEntity_t *svEnt );
int SV_DObjUpdateServerTime(gentity_s *ent, float dtime, int bNotify);
void SV_DObjDisplayAnim(gentity_s *ent);
void SV_DObjInitServerTime(gentity_s *ent, float dtime);
qboolean SV_DObjExists(gentity_s *ent);
XAnimTree_s* SV_DObjGetTree(gentity_s *ent);

int SV_DObjSetControlRotTransIndex(const gentity_t *ent, int *partBits, int boneIndex);
int SV_DObjSetRotTransIndex(const gentity_t *ent, int *partBits, int boneIndex);
DObjAnimMat* SV_DObjGetRotTransArray(gentity_t *ent);
int SV_DObjNumBones(gentity_t *ent);
qboolean SV_GetEntityToken( char *buffer, int bufferSize );

#include "../qcommon/cm_public.h"
void SV_LinkEntity( gentity_t *gEnt );
void SV_UnlinkEntity( gentity_t *gEnt );
void SV_ClipMoveToEntity(moveclip_t *clip, svEntity_t *entity, trace_t *trace);
void SV_PointTraceToEntity(pointtrace_t *clip, svEntity_t *check, trace_t *trace);
int SV_PointSightTraceToEntity(sightpointtrace_t *clip, svEntity_t *check);
int SV_ClipSightToEntity(sightclip_t *clip, svEntity_t *check);
void SV_Trace(trace_t *results, const float *start, const float *mins, const float *maxs, const float *end, int passEntityNum, int contentmask, int locational, unsigned char *priorityMap, int staticmodels);
int SV_TracePassed(const float *start, const float *mins, const float *maxs, const float *end, int passEntityNum0, int passEntityNum1, int contentmask, int locational, int staticmodels);
void SV_SightTrace(int *hitNum,const float *start, const float *mins, const float *maxs, const float *end, int passEntityNum0, int passEntityNum1, int contentmask);
int SV_PointContents(const vec3_t p, int passEntityNum, int contentmask);
clipHandle_t SV_ClipHandleForEntity( const gentity_t *ent );
qboolean SV_EntityContact( const vec3_t mins, const vec3_t maxs, const gentity_t *gEnt );
int SV_GetArchivedClientInfo(int clientNum, int *pArchiveTime, playerState_t *ps, clientState_t *cs);
int SV_SightTraceToEntity( const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int entityNum, int contentmask );
void SV_LocationalTraceToEntity( trace_t *results, const vec3_t start, const vec3_t end, int entIndex, int contentMask, unsigned char *priorityMap );
void SV_LocateGameData(gentity_s *gEnts, int numGEntities, int sizeofGEntity_t, playerState_s *clients, int sizeofGameClient);
void SV_AddCachedEntitiesVisibleFromPoint(int from_num_entities, int from_first_entity, float *origin, signed int clientNum, snapshotEntityNumbers_t *eNums);
qboolean SV_inSnapshot(const float *origin, int iEntityNum);
cachedSnapshot_t* SV_GetCachedSnapshot(int *pArchiveTime);
void SV_BuildClientSnapshot( client_t *client );
void SV_WriteSnapshotToClient(client_s *client, msg_t *msg);
void SV_InitArchivedSnapshot();
void SV_FreeArchivedSnapshot();
char* SV_AllocSkelMemory(unsigned int size);
void SV_ResetSkeletonCache();
int SV_DObjCreateSkelForBone(gentity_s *ent, int boneIndex);
int SV_DObjCreateSkelForBones(gentity_s *ent, int *partBits);
void SV_DObjGetHierarchyBits(gentity_s *ent, int boneIndex, int *partBits);
void SV_DObjCalcAnim(gentity_s *ent, int *partBits);
void SV_DObjCalcSkel(gentity_s *ent, int *partBits);
int SV_DObjGetBoneIndex(const gentity_s *ent, unsigned int boneName);
void SV_DObjDumpInfo(gentity_s *ent);
DObjAnimMat* SV_DObjGetMatrixArray(gentity_s *ent);
bool SV_GetClientPositionsAtTime(int clientNum, int gametime, float *origin);
void SV_WriteDownloadToClient( client_t *cl, msg_t *msg );
int SV_GetClientPing(int clientNum);
void SV_GameSendServerCommand(int clientnum, int svscmd_type, const char *text);
void SV_GetChallenge( netadr_t from );
void SVC_Info( netadr_t from );
void SVC_Status( netadr_t from );
void SV_ConnectionlessPacket( netadr_t from, msg_t *msg );
void SV_UserinfoChanged( client_t *cl );
void SV_AuthorizeRequest(netadr_t from, int challenge);
void SV_CloseDownload( client_t *cl );

void SV_RestartGameProgs(int savepersist);
void SV_InitGameProgs(int savepersist);

netadr_t *SV_MasterAddress();
void SV_MasterHeartbeat( const char *hbname );
void SV_MasterGameCompleteStatus();
void SV_AddOperatorCommands(void);
void SV_RemoveOperatorCommands();

void SVC_GameCompleteStatus( netadr_t from );

void SV_SpawnServer(char *server);
void SV_MasterShutdown();

void SV_SendClientMessages( void );
void SV_ClientThink(client_t *cl, usercmd_t *cmd);

void SV_ChangeMaxClients( void );
void SV_Startup( void );

void SV_ClientEnterWorld(client_s *client, usercmd_s *cmd);

void SV_RunFrame();
void SV_CreateBaseline( void );

bool SV_ClientHasClientMuted(int listener, int talker);
bool SV_ClientWantsVoiceData(int clientNum);
void SV_QueueVoicePacket(int talkerNum, int clientNum, VoicePacket_t *voicePacket);
void SV_UserVoice(client_s *cl, msg_t *msg);
void SV_PreGameUserVoice(client_s *cl, msg_t *msg);
void SV_VoicePacket(netadr_t from, msg_t *msg);
void SV_SendClientVoiceData(client_s *client);
void SV_SetBrushModel(gentity_s *ent);
void SV_XModelDebugBoxes();
void SV_SetWeaponInfoMemory();
void SV_FreeWeaponInfoMemory();
void SV_GetServerinfo(char *buffer, int bufferSize);
void SV_FreeClientScriptPers();
float SV_FX_GetVisibility(const vec3_t start, const vec3_t end);
int SV_GetCurrentClientInfo(int clientNum, playerState_s *ps, clientState_t *cs);