#pragma once

#include "i_common.h"
#include "i_shared.h"
#include "i_math.h"
#include "netchan.h"
#include "sys_net.h"
#include "entity.h"
#include "player.h"

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
	char num;
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
	short			clscriptid;
	int				bot;
	int				serverId;
	voices_t		voicedata[40];
	int				unsentVoiceData;
	byte			mutedClients[MAX_CLIENTS];
	byte			hasVoip;
} client_t;

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

typedef struct
{
	qboolean	initialized;
	int			time;
	int			snapFlagServerBit;
	client_t	*clients;
	int			numSnapshotEntities;
	int			numSnapshotClients;
	int			nextSnapshotEntities;
	int			nextSnapshotClients;
	entityState_t *snapshotEntities;
	clientState_t *snapshotClients;
	int 		archivedSnapshotEnabled;
	int 		nextArchivedSnapshotFrames;
	archivedSnapshot_t *archivedSnapshotFrames;
	int 		*archivedSnapshotBuffer;
	int 		nextArchivedSnapshotBuffer;
	int			nextCachedSnapshotEntities;
	int 		nextCachedSnapshotClients;
	int 		nextCachedSnapshotFrames;
	cachedClient_t cachedSnapshotClients;
	int			nextHeartbeatTime;
	int 		nextStatusResponseTime;
	challenge_t	challenges[1024];
	netadr_t	redirectAddress;
	netadr_t	authorizeAddress;
	char 		netProfilingBuf[1504];
} serverStatic_t; // verified

typedef struct
{
	const char *key;
	const char *value;
} keyValueStr_t;

typedef struct
{
	byte spawnVarsValid;
	int numSpawnVars;
	keyValueStr_t spawnVars[64];
	int numSpawnVarChars;
	char spawnVarChars[2048];
} SpawnVar;

typedef struct
{
	u_int16_t entnum;
	u_int16_t otherEntnum;
	int useCount;
	int otherUseCount;
} trigger_info_t;

typedef struct
{
	struct gclient_s *clients;
	struct gentity_s *gentities;
	int gentitySize;
	int num_entities;
	struct gentity_s *firstFreeEnt;
	struct gentity_s *lastFreeEnt;
	fileHandle_t logFile;
	int initializing;
	int clientIsSpawning;
	objective_t objectives[16];
	int maxclients;
	int framenum;
	int time;
	int previousTime;
	int frameTime;
	int startTime;
	int teamScores[TEAM_NUM_TEAMS];
	int lastTeammateHealthTime;
	qboolean bUpdateScoresForIntermission;
	int manualNameChange;
	int numConnectedClients;
	int sortedClients[MAX_CLIENTS];
	char voteString[1024];
	char voteDisplayString[1024];
	int voteTime; // 711
	int voteExecuteTime;
	int voteYes;
	int voteNo;
	int numVotingClients;
	byte gap[2072];
	SpawnVar spawnVars;
	int savePersist;
	struct gentity_s *droppedWeaponCue[32];
	float fFogOpaqueDist;
	float fFogOpaqueDistSqrd;
	int remapCount;
	int currentPlayerClone;
	trigger_info_t pendingTriggerList[256];
	trigger_info_t currentTriggerList[256];
	int pendingTriggerListSize;
	int currentTriggerListSize;
	int finished;
	int bPlayerIgnoreRadiusDamage;
	int bPlayerIgnoreRadiusDamageLatched;
	int registerWeapons;
	int bRegisterItems;
	int currentEntityThink;
	void *openScriptIOFileHandles[1];
	char *openScriptIOFileBuffers[1];
} level_locals_t; // possibly more stuff here

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

typedef struct
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

typedef struct svEntity_s
{
	u_int16_t worldSector;
	u_int16_t nextEntityInWorldSector;
	archivedEntity_t baseline;
	int numClusters;
	int clusternums[MAX_ENT_CLUSTERS];
	int	lastCluster;
	int	linkcontents;
	float linkmin[2];
	float linkmax[2];
} svEntity_t;

typedef struct
{
	serverState_t state;
	qboolean restarting;
	int start_frameTime;
	int	checksumFeed;
	int timeResidual;
	int unk; // ?
	struct cmodel_s *models[MAX_MODELS]; // ?
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
} server_t; // verified

qboolean SV_GameCommand(void);
