#pragma once

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <ctype.h>
#include <limits.h>
#include <setjmp.h>

#include "../universal/universal_public.h"

// We only care about dedicated server
#define DEDICATED

#ifndef MAXPRINTMSG
#define MAXPRINTMSG 4096
#endif

extern dvar_t *com_sv_running;
extern dvar_t *com_logfile;
extern dvar_t *com_dedicated;
extern dvar_t *com_viewlog;

// returnbed by Sys_GetProcessorId
#define CPUID_GENERIC			0				// any unrecognized processor
#define CPUID_AXP				0x10
#define CPUID_INTEL_UNSUPPORTED	0x20			// Intel 386/486
#define CPUID_INTEL_PENTIUM		0x21			// Intel Pentium or PPro
#define CPUID_INTEL_MMX			0x22			// Intel Pentium/MMX or P2/MMX
#define CPUID_INTEL_KATMAI		0x23			// Intel Katmai
#define CPUID_AMD_3DNOW			0x30			// AMD K6 3DNOW!

int Sys_GetProcessorId( void );

typedef enum
{
	// bk001129 - make sure SE_NONE is zero
	SE_NONE = 0,	// evTime is still valid
	SE_KEY,		// evValue is a key code, evValue2 is the down flag
	SE_CHAR,	// evValue is an ascii char
	SE_MOUSE,	// evValue and evValue2 are reletive signed x / y moves
	SE_JOYSTICK_AXIS,	// evValue is an axis number and evValue2 is the current state (-127 to 127)
	SE_CONSOLE,	// evPtr is a char*
	SE_PACKET	// evPtr is a netadr_t followed by data bytes to evPtrLength
} sysEventType_t;

typedef struct
{
	int evTime;
	sysEventType_t evType;
	int evValue, evValue2;
	int evPtrLength;                // bytes of data pointed to by evPtr, for journaling
	void            *evPtr;         // this must be manually freed if not NULL
} sysEvent_t;

void Sys_Init (void);
int Sys_Milliseconds (void);

#define Sys_MilliSeconds Sys_Milliseconds

sysEvent_t Sys_GetEvent( void );

void Sys_Print( const char *msg );
const char *Sys_GetCurrentUser( void );

void Sys_Error( const char *error, ...);
void Sys_Quit(void);
char *Sys_GetClipboardData( void );

void Sys_ShowConsole( int level, qboolean quitOnClose );

char **Sys_ListFiles( const char *directory, const char *extension, const char *filter, int *numfiles, qboolean wantsubs );
void Sys_FreeFileList( char **list );
char *Sys_Cwd( void );
void Sys_StreamSeek( fileHandle_t f, int offset, int origin );
void Sys_EndStreamedFile( fileHandle_t f );
qboolean Sys_DirectoryHasContents(const char *dir);
void Sys_Mkdir( const char *path );
char *Sys_Cwd( void );
void Sys_SetDefaultCDPath(const char *path);
const char *Sys_DefaultCDPath(void);
void Sys_SetDefaultInstallPath(const char *path);
char *Sys_DefaultInstallPath(void);
void Sys_SetDefaultHomePath(const char *path);
const char *Sys_DefaultHomePath(void);

enum errorParm_t
{
	ERR_FATAL = 0x0,
	ERR_DROP = 0x1,
	ERR_SERVERDISCONNECT = 0x2,
	ERR_DISCONNECT = 0x3,
	ERR_SCRIPT = 0x4,
	ERR_SCRIPT_DROP = 0x5,
	ERR_LOCALIZATION = 0x6,
	ERR_MAPLOADERRORSUMMARY = 0x7,
};

typedef enum
{
	NA_BOT = 0,
	NA_BAD = 1,
	NA_LOOPBACK = 2,
	NA_BROADCAST = 3,
	NA_IP = 4,
	NA_IPX = 5,
	NA_BROADCAST_IPX = 6
} netadrtype_t;

typedef enum
{
	NS_CLIENT,
	NS_SERVER
} netsrc_t;

typedef struct
{
	int type;
	byte ip[4];
	unsigned short port;
	byte ipx[10];
} netadr_t;

void NET_Init( void );
void NET_Shutdown( void );
void NET_Restart( void );
void NET_Config( qboolean enableNetworking );
void NET_Sleep(int msec);

enum conChannel_t
{
	CON_CHANNEL_DONT_FILTER = 0x0,
	CON_CHANNEL_ERROR = 0x1,
	CON_CHANNEL_GAMENOTIFY = 0x2,
	CON_CHANNEL_BOLDGAME = 0x3,
	CON_CHANNEL_LOGFILEONLY = 0x4,
};

// Edit fields and command line history/completion
#define	MAX_EDIT_LINE	256
typedef struct
{
	int		cursor;
	int		scroll;
	int		widthInChars;
	char	buffer[MAX_EDIT_LINE];
} field_t;

void Field_Clear( field_t *edit );
void Field_CompleteCommand( field_t *edit );

typedef struct
{
	qboolean overflowed;
	byte *data;
	int maxsize;
	int cursize;
	int readcount;
	int bit;
} msg_t;

enum svc_ops_e
{
	svc_nop,
	svc_gamestate,
	svc_configstring,
	svc_baseline,
	svc_serverCommand,
	svc_download,
	svc_snapshot,
	svc_EOF
};

enum clc_ops_e
{
	clc_move,
	clc_moveNoDelta,
	clc_clientCommand,
	clc_EOF
};

typedef enum
{
	MSG_DEFAULT,
	MSG_NA,
	MSG_WARNING,
	MSG_ERROR,
	MSG_NORDPRINT
} msgtype_t;

typedef void (*xcommand_t)(void);

#define PORT_SERVER 28960
#define	PORT_ANY -1

#define MASTER_SERVER_NAME "cod2master.activision.com"
#define MAX_MASTER_SERVERS  5
#define PORT_MASTER 20710

#define AUTHORIZE_SERVER_NAME "cod2master.activision.com"
#define PORT_AUTHORIZE 20700
#define AUTHORIZE_TIMEOUT 5000

#define	MAX_RELIABLE_COMMANDS 128
#define MAX_DOWNLOAD_WINDOW	8
#define MAX_DOWNLOAD_BLKSIZE 1024
#define MAX_CLIENTS 64
#define	PACKET_BACKUP 32
#define PACKET_MASK ( PACKET_BACKUP - 1 )
#define MAX_QPATH 64
#define MAX_PACKET_USERCMDS     32      // max number of usercmd_t in a packet

/* This is based on the Adaptive Huffman algorithm described in Sayood's Data
 * Compression book.  The ranks are not actually stored, but implicitly defined
 * by the location of a node within a doubly-linked list */
#define NYT HMAX                    /* NYT = Not Yet Transmitted */
#define INTERNAL_NODE ( HMAX + 1 )

typedef struct nodetype
{
	struct  nodetype *left, *right, *parent; /* tree structure */
	struct  nodetype *next, *prev; /* doubly-linked list */
	struct  nodetype **head; /* highest ranked node in block */
	int weight;
	int symbol;
} node_t;

#define HMAX 256 /* Maximum symbol */

typedef struct
{
	int blocNode;
	int blocPtrs;

	node_t*     tree;
	node_t*     lhead;
	node_t*     ltail;
	node_t*     loc[HMAX + 1];
	node_t**    freelist;

	node_t nodeList[768];
	node_t*     nodePtrs[768];
} huff_t;

typedef struct
{
	huff_t compressor;
	huff_t decompressor;
} huffman_t;

typedef struct NetField
{
	const char *name;
	int offset;
	int bits;
} netField_t;

#include "../game/g_shared.h"
#include "../server/server.h"

qboolean Sys_SendPacket( int length, const void *data, netadr_t to );
qboolean Sys_GetPacket ( netadr_t *net_from, msg_t *net_message );
qboolean Sys_StringToAdr( const char *s, netadr_t *a );
qboolean Sys_IsLANAddress (netadr_t adr);

void Huff_Compress( msg_t *buf, int offset );
void Huff_Decompress( msg_t *buf, int offset );
void Huff_Init( huffman_t *huff );
void MSG_initHuffman();
void Huff_addRef( huff_t* huff, byte ch );
int  Huff_Receive( node_t *node, int *ch, byte *fin );
void Huff_transmit( huff_t *huff, int ch, byte *fout );
void Huff_offsetReceive( node_t *node, int *ch, byte *fin, int *offset );
void Huff_offsetTransmit( huff_t *huff, int ch, byte *fout, int *offset );
void Huff_putBit( int bit, byte *fout, int *offset );
int  Huff_getBit( byte *fout, int *offset );

void MSG_Init( msg_t *buf, byte *data, int length );
void MSG_BeginReading( msg_t *msg );
void MSG_WriteBits(msg_t *msg, int bits, int bitcount);
void MSG_WriteBit0( msg_t* msg );
void MSG_WriteBit1(msg_t *msg);
int MSG_ReadBits(msg_t *msg, int bits);
int MSG_ReadBit(msg_t *msg);
int MSG_WriteBitsCompress( const byte *datasrc, byte *buffdest, int bytecount);
int MSG_ReadBitsCompress(const byte* input, byte* outputBuf, int readsize);
void MSG_WriteByte( msg_t *msg, int c );
void MSG_WriteData(msg_t *buf, const void *data, int length);
void MSG_WriteShort( msg_t *msg, int c );
void MSG_WriteLong( msg_t *msg, int c );
void MSG_WriteLong64( msg_t *msg, int c );
char *MSG_ReadString( msg_t *msg );
void MSG_WriteString( msg_t *sb, const char *s );
void MSG_WriteBigString( msg_t *sb, const char *s );
int MSG_ReadByte( msg_t *msg );
int MSG_ReadShort( msg_t *msg );
int MSG_ReadLong( msg_t *msg );
int MSG_ReadLong64( msg_t *msg );
void MSG_ReadData(msg_t *msg, void *data, int len);
char *MSG_ReadCommandString( msg_t *msg );
char *MSG_ReadCommandString( msg_t *msg );
char *MSG_ReadBigString( msg_t *msg );
char *MSG_ReadStringLine( msg_t *msg );
void MSG_WriteReliableCommandToBuffer(const char *source, char *destination, int length);
void MSG_SetDefaultUserCmd(playerState_t *ps, usercmd_t *ucmd);
void MSG_ReadDeltaUsercmdKey(msg_t *msg, int key, usercmd_t *from, usercmd_t *to);
void MSG_WriteDeltaField(msg_t *msg, const byte *from, const byte *to, const netField_t *field);
void MSG_WriteDeltaStruct(msg_t *msg, const byte *from, const byte *to, qboolean force, int numFields, int indexBits, netField_t *stateFields, qboolean bChangeBit);
void MSG_WriteDeltaObjective(msg_t *msg,objective_t *from,objective_t *to, int lcfield, int numFields, netField_t *objFields);
void MSG_WriteDeltaEntity(msg_t *msg, entityState_t *from, entityState_t *to, qboolean force);
void MSG_WriteDeltaArchivedEntity(msg_t *msg, archivedEntity_t *from, archivedEntity_t *to, int flags);
void MSG_WriteDeltaClient(msg_t *msg, clientState_t *from, clientState_t *to, qboolean force);
void MSG_ReadDeltaField(msg_t *msg, const void *from, const void *to, netField_t *field, qboolean print);
void MSG_ReadDeltaObjective(msg_t *msg, objective_t *from, objective_t *to, int numFields, netField_t *objFields);
void MSG_ReadDeltaStruct(msg_t *msg, const void *from, void *to, unsigned int number, int numFields, int indexBits, netField_t *stateFields);
void MSG_ReadDeltaEntity(msg_t *msg, entityState_t *from, entityState_t *to, int number);
void MSG_ReadDeltaArchivedEntity(msg_t *msg, archivedEntity_t *from, archivedEntity_t *to, int number);
void MSG_ReadDeltaClient(msg_t *msg, clientState_t *from, clientState_t *to, int number);
void MSG_WriteDeltaHudElems(msg_t *msg, hudelem_t *from, hudelem_t *to, int count);
void MSG_ReadDeltaHudElems(msg_t *msg, hudelem_t *from, hudelem_t *to, int count);
void MSG_WriteDeltaPlayerstate(msg_t *msg, playerState_t *from, playerState_t *to);
void MSG_ReadDeltaPlayerstate(msg_t *msg, playerState_t *from, playerState_t *to);

unsigned int Com_BlockChecksum( void *buffer, int length );
unsigned int Com_BlockChecksumKey( void *buffer, int length, int key );

int Com_HashKey( const char *string, int maxlen );

#ifdef __cplusplus
extern "C" {
#endif

void Com_InitDvars();
void Com_StartupVariable( const char *match );
void Com_PrintMessage( conChannel_t channel, const char *fmt, ... );
void QDECL Com_Printf( const char *fmt, ...);
void QDECL Com_DPrintf( const char *fmt, ...);
void Com_Error(errorParm_t code, const char *fmt, ...);
void Com_WriteConfigToFile(const char *contents);

#ifdef __cplusplus
}
#endif

qboolean Com_SafeMode( void );
int Com_Milliseconds( void );

void Com_DvarDump(conChannel_t channel);

#define Sys_MillisecondsRaw Com_Milliseconds

void Com_BeginRedirect (char *buffer, int buffersize, void (*flush)( char *) );
void Com_EndRedirect (void);

void Com_SetWeaponInfoMemory(int source);
void Com_FreeWeaponInfoMemory(int source);

void Info_Print( const char *s );

const char *GetBspExtension();
void Com_LoadBsp(const char *filename);
void Com_UnloadBsp();
void Com_CleanupBsp();
qboolean Com_IsBspLoaded();

void Com_ServerDObjCreate(DObjModel_s *dobjModels, unsigned short numModels, XAnimTree_s *tree, int handle);
void Com_SafeServerDObjFree(int handle);
DObj* Com_GetServerDObj(int handle);
void Com_InitDObj();
void Com_ShutdownDObj();
void Com_AbortDObj();

void Sys_OutOfMemErrorInternal(const char *filename, int line);
void Sys_UnimplementedFunctionInternal(const char *function);

#define UNIMPLEMENTED Sys_UnimplementedFunctionInternal

void Com_Init(char* commandLine);
void Com_Restart();
void Com_Shutdown(const char *reason);
void Com_Frame(void);
void Com_Quit_f( void );

extern int com_frameTime;
extern int com_fixedConsolePosition;

qboolean SV_GameCommand();

#include "cm_public.h"

void CM_InitThreadData(int threadContext);
void CM_LoadMapFromBsp(const char *name, bool usePvs);
void* CM_Hunk_Alloc(int size);
void CM_Hunk_CheckTempMemoryHighClear();
void CM_Hunk_ClearTempMemoryHigh();
void* CM_Hunk_AllocateTempMemoryHigh(int size);
byte* Com_GetBspLump(int type, unsigned int elemSize, unsigned int *count);
bool Com_BspHasLump(int type);
void CM_LoadMapFromBsp(const char *name, bool usePvs);
void CM_LoadMap(const char *name, int *checksum);
void CM_LoadStaticModels();
void CM_Cleanup(void);
void CM_Shutdown();

void CM_LinkEntity(svEntity_t *ent, float *absmin, float *absmax, clipHandle_t clipHandle);
void CM_UnlinkEntity(svEntity_t *ent);

clipHandle_t CM_TempBoxModel(const vec3_t mins, const vec3_t maxs, int capsule);
int CM_LeafCluster( int leafnum );
void CM_ModelBounds( clipHandle_t model, vec3_t mins, vec3_t maxs );
int CM_BoxLeafnums( const vec3_t mins, const vec3_t maxs, int *list, int listsize, int *lastLeaf );
byte *CM_ClusterPVS( int cluster );
int CM_PointLeafnum( const vec3_t p );

#include "../xanim/xanim_public.h"
XModel* CM_XModelPrecache(const char *name);