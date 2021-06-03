#pragma once

#include "i_shared.h"
#include "i_math.h"
#include "com_memory.h"
#include "dvar.h"

#define	MAX_RELIABLE_COMMANDS 128
#define MAX_DOWNLOAD_WINDOW	8
#define MAX_DOWNLOAD_BLKSIZE 1024
#define MAX_PACKET_USERCMDS	32
#define MAX_NAME_LENGTH	32

#define MAX_CLIENTS 64

#define	PACKET_BACKUP 32
#define PACKET_MASK ( PACKET_BACKUP - 1 )

#define MAX_QPATH 64

#define MAX_DOWNLOAD_WINDOW         8
#define MAX_DOWNLOAD_BLKSIZE        1024

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

typedef enum
{
	MSG_DEFAULT,
	MSG_NA,
	MSG_WARNING,
	MSG_ERROR,
	MSG_NORDPRINT
} msgtype_t;

extern dvar_t *com_dedicated;
extern dvar_t *com_maxfps;
extern dvar_t *com_developer;
extern dvar_t *com_developer_script;
extern dvar_t *com_logfile;
extern dvar_t *com_timescale;
extern dvar_t *com_fixedtime;
extern dvar_t *com_viewlog;
extern dvar_t *sv_paused;
extern dvar_t *cl_paused;
extern dvar_t *sv_running;
extern dvar_t *com_introPlayed;
extern dvar_t *com_animCheck;
extern dvar_t *com_sv_running;

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

void    Huff_Compress( msg_t *buf, int offset );
void    Huff_Decompress( msg_t *buf, int offset );
void    Huff_Init( huffman_t *huff );
void    Huff_addRef( huff_t* huff, byte ch );
int     Huff_Receive( node_t *node, int *ch, byte *fin );
void    Huff_transmit( huff_t *huff, int ch, byte *fout );
void    Huff_offsetReceive( node_t *node, int *ch, byte *fin, int *offset );
void    Huff_offsetTransmit( huff_t *huff, int ch, byte *fout, int *offset );
void    Huff_putBit( int bit, byte *fout, int *offset );
int     Huff_getBit( byte *fout, int *offset );

void MSG_Init( msg_t *buf, byte *data, int length );
void MSG_BeginReading( msg_t *msg );
int MSG_ReadBits( msg_t *msg, int bits );
int MSG_ReadBitsCompress(const byte* input, int readsize, byte* outputBuf, int outputBufSize);
int MSG_WriteBitsCompress( const byte *datasrc, byte *buffdest, int bytecount);
int MSG_ReadByte( msg_t *msg );
int MSG_ReadShort( msg_t *msg );
int MSG_ReadLong( msg_t *msg );
char *MSG_ReadString( msg_t *msg );
char *MSG_ReadBigString( msg_t *msg );
void MSG_WriteByte( msg_t *sb, int c );
void MSG_WriteShort( msg_t *sb, int c );
void MSG_WriteLong( msg_t *sb, int c );
void MSG_WriteString( msg_t *sb, const char *s );
void MSG_WriteBigString( msg_t *sb, const char *s );
void MSG_WriteData( msg_t *buf, const void *data, int length );

void Com_StartupVariable( const char *match );
void Com_ParseCommandLine( char *commandLine );

const char *Com_StringContains( const char *str1, const char *str2, int casesensitive);
int Com_Filter( const char *filter, const char *name, int casesensitive);
int Com_FilterPath(const char *filter, const char *name, int casesensitive);

void Com_Init(char* commandLine);
void Com_Frame(void);
void COM_DefaultExtension( char *path, int maxSize, const char *extension );

void *Hunk_AllocInternal( int size );
void *Hunk_AllocLowInternal( int size );

typedef void (*xcommand_t)(void);
