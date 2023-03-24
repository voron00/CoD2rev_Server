#pragma once

#include "../qcommon/cmd.h"
#include "../qcommon/netchan.h"
#include "../qcommon/qcommon.h"
#include "../game/g_shared.h"

#define MAX_CHALLENGES  1024
#define MAX_SNAPSHOT_ENTITIES	1024
#define MAX_NAME_LENGTH	32


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
} challenge_t;

enum clientConnectState_t
{
	CS_FREE,
	CS_ZOMBIE,
	CS_CONNECTED,
	CS_PRIMED,
	CS_ACTIVE
};

typedef struct
{
	playerState_t ps;
	int	num_entities;
	int	num_clients;
	int	first_entity;
	int	first_client;
	unsigned int messageSent;
	unsigned int messageAcked;
	int	messageSize;
} clientSnapshot_t;

#pragma pack(push)
#pragma pack(1)
typedef struct
{
	byte num;
	char data[256];
	int dataLen;
} voices_t;
#pragma pack(pop)

typedef struct client_s
{
	int				state;
	int				delayed;
	const char		*delayDropMsg;
	char			userinfo[MAX_STRING_CHARS];
	reliableCommands_t	reliableCommands[MAX_RELIABLE_COMMANDS];
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
	int				deltaMessage;
	int				floodprotect;
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
	unsigned short	clscriptid;
	int				bot;
	int				serverId;
	voices_t		voicedata[40];
	int				unsentVoiceData;
	byte			mutedClients[MAX_CLIENTS];
	byte			hasVoip;
} client_t;
static_assert((sizeof(client_t) == 0x78F14), "ERROR: client_t size is invalid!");

typedef struct archivedSnapshot_s
{
	int start;
	int size;
} archivedSnapshot_t;

typedef struct cachedClient_s
{
	int playerStateExists;
	clientState_t *cs;
	playerState_t *ps;
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

struct tempban_t
{
	int guid;
	int time;
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
	int archivedSnapshotEnabled;
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
	challenge_t challenges[1024];
	netadr_t redirectAddress;
	netadr_t authorizeAddress;
	netProfileInfo_t *netProfilingBuf;
	tempban_t bans[16];
} serverStatic_t;
static_assert((sizeof(serverStatic_t) == 0xC108), "ERROR: serverStatic_t size is invalid!");

typedef enum
{
	SS_DEAD,
	SS_LOADING,
	SS_GAME
} serverState_t;

#define MAX_CONFIGSTRINGS   2048
#define	RESERVED_CONFIGSTRINGS	2
#define MAX_MODELS          256
#define GENTITYNUM_BITS     10
#define MAX_GENTITIES       ( 1 << GENTITYNUM_BITS )
#define MAX_ENT_CLUSTERS    16
#define MAX_BPS_WINDOW 		20

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
static_assert((sizeof(svEntity_t) == 0x174), "ERROR: svEntity_t size is invalid!");

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
	char *entityParsePoint;
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
static_assert((sizeof(server_t) == 0x5F534), "ERROR: server_t size is invalid!");

struct snapshotEntityNumbers_t
{
	int numSnapshotEntities;
	int snapshotEntities[1024];
};

struct moveclip_t
{
	vec3_t mins;
	vec3_t maxs;
	vec3_t outerSize;
	TraceExtents extents;
	int passEntityNum;
	int passOwnerNum;
	int contentmask;
};

extern dvar_t *nextmap;
extern dvar_t *sv_maxclients;

void SV_Init();
void SV_PacketEvent( netadr_t from, msg_t *msg );
void SV_Frame(int msec);
void SV_Shutdown( const char* finalmsg );
void SV_ShutdownGameProgs();
void SV_Netchan_AddOOBProfilePacket(int iLength);
void SV_Netchan_SendOOBPacket(int iLength, const void *pData, netadr_t to);
qboolean SV_Netchan_TransmitNextFragment(netchan_t *chan);
bool SV_Netchan_Transmit(client_t *client, byte *data, int length);
void SV_Netchan_Encode( client_t *client, byte *data, int cursize );
void SV_Netchan_Decode( client_t *client, byte *data, int remaining );
void SV_Netchan_UpdateProfileStats();
void SV_Netchan_PrintProfileStats(qboolean format);
void SV_AddServerCommand(client_t *client, int type, const char *cmd);
void SV_DelayDropClient(client_t *client, const char *dropmsg);
char *SV_ExpandNewlines( char *in );
void SV_SendServerCommand( client_t *cl, int type, const char *fmt, ... );
void SV_SetConfig(int start, int max, unsigned short bit);
void SV_SetConfigValueForKey(int start, int max, const char *key, const char *value);
void SV_SetConfigstring(unsigned int index, const char *val);
void SV_GetConfigstring( int index, char *buffer, int bufferSize );
const char* SV_GetConfigstringConst(int index);
void SV_GetUserinfo( int index, char *buffer, int bufferSize );

void SV_AuthorizeIpPacket( netadr_t from );

qboolean SV_IsLocalClient(int clientNum);
qboolean SV_MapExists(const char *name);
const char *SV_GetMapBaseName(const char *mapname);
XModel* SV_XModelGet(const char *name);
int SV_NumForGentity( gentity_t *ent );
gentity_t *SV_GentityNum( int num );
playerState_t *SV_GameClientNum( int num );
svEntity_t  *SV_SvEntityForGentity( gentity_t *gEnt );
gentity_t *SV_GEntityForSvEntity( svEntity_t *svEnt );
void SV_DObjUpdateServerTime(gentity_s *ent, float dtime, int bNotify);
qboolean SV_inSnapshot(const float *origin, int iEntityNum);

void SV_LinkEntity( gentity_t *gEnt );
void SV_UnlinkEntity( gentity_t *gEnt );
void SV_ClipMoveToEntity(moveclip_t *clip, svEntity_t *entity, trace_t *trace);

void SV_AddCachedEntitiesVisibleFromPoint(int from_num_entities, int from_first_entity, float *origin, signed int clientNum, snapshotEntityNumbers_t *eNums);