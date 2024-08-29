#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <math.h>

#include "com_math.h"

#define GAME_STRING "CoD2 MP"

#define PRODUCT_VERSION "1.0"
#define PROTOCOL_VERSION 115

// for windows fastcall option
#define	QDECL

//======================= WIN32 DEFINES =================================

#ifdef WIN32

#define	MAC_STATIC

#undef QDECL
#define	QDECL	__cdecl

// buildstring will be incorporated into the version string
#ifdef NDEBUG
#ifdef _M_IX86
#define	CPUSTRING	"win-x86"
#elif defined _M_ALPHA
#define	CPUSTRING	"win-AXP"
#endif
#else
#ifdef _M_IX86
#define	CPUSTRING	"win-x86"
#elif defined _M_ALPHA
#define	CPUSTRING	"win-AXP"
#endif
#endif

#define	PATH_SEP '\\'

#endif

//======================= MAC OS X SERVER DEFINES =====================

#if defined(__MACH__) && defined(__APPLE__)

#define MAC_STATIC

#ifdef __ppc__
#define CPUSTRING	"MacOSXS-ppc"
#elif defined __i386__
#define CPUSTRING	"MacOSXS-i386"
#else
#define CPUSTRING	"MacOSXS-other"
#endif

#define	PATH_SEP	'/'

#define	GAME_HARD_LINKED
#define	CGAME_HARD_LINKED
#define	UI_HARD_LINKED
#define _alloca alloca

#undef ALIGN_ON
#undef ALIGN_OFF
#define	ALIGN_ON		#pragma align(16)
#define	ALIGN_OFF		#pragma align()

#endif

//======================= MAC DEFINES =================================

#ifdef __MACOS__

#define	MAC_STATIC static

#define	CPUSTRING	"MacOS-PPC"

#define	PATH_SEP ':'

#endif

#ifdef __MRC__

#define	MAC_STATIC

#define	CPUSTRING	"MacOS-PPC"

#define	PATH_SEP ':'

#undef QDECL
#define	QDECL	__cdecl

#define _alloca alloca
#endif

//======================= LINUX DEFINES =================================

// the mac compiler can't handle >32k of locals, so we
// just waste space and make big arrays static...
#ifdef __linux__

// bk001205 - from Makefile
#define stricmp strcasecmp

#define	MAC_STATIC // bk: FIXME

#ifdef __i386__
#define	CPUSTRING	"linux-i386"
#elif defined __axp__
#define	CPUSTRING	"linux-alpha"
#else
#define	CPUSTRING	"linux-other"
#endif

#define	PATH_SEP '/'

// bk001205 - try
#ifdef Q3_STATIC
#define	GAME_HARD_LINKED
#define	CGAME_HARD_LINKED
#define	UI_HARD_LINKED
#define	BOTLIB_HARD_LINKED
#endif

#endif

//=============================================================

#define qboolean int
#define qtrue 1
#define qfalse 0

#include "../misc/hexrays_defs.h"
#define ARRAY_COUNT(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

typedef unsigned char byte;
typedef int	fileHandle_t;
typedef int	clipHandle_t;

// in order from highest priority to lowest
// if none of the catchers are active, bound key strings will be executed
#define KEYCATCH_CONSOLE		0x0001
#define	KEYCATCH_UI				0x0002
#define	KEYCATCH_MESSAGE		0x0004
#define	KEYCATCH_CGAME

#define Q_COLOR_ESCAPE	'^'
#define Q_IsColorString(p)	( p && *(p) == Q_COLOR_ESCAPE && *((p)+1) && *((p)+1) != Q_COLOR_ESCAPE )

// the game guarantees that no string from the network will ever
// exceed MAX_STRING_CHARS
#define	MAX_STRING_CHARS	1024	// max length of a string passed to Cmd_TokenizeString
#define	MAX_STRING_TOKENS	512		// max tokens resulting from Cmd_TokenizeString
#define	MAX_TOKEN_CHARS		1024	// max length of an individual token

#define	MAX_INFO_STRING		1024
#define	MAX_INFO_KEY		1024
#define	MAX_INFO_VALUE		1024

#define	BIG_INFO_STRING		8192  // used for system info key only
#define	BIG_INFO_KEY		8192
#define	BIG_INFO_VALUE		8192

#define MAX_QPATH           64      // max length of a quake game pathname
#define MAX_OSPATH          256     // max length of a filesystem pathname

#define MAX_EVENTS          4   // max events per frame before we drop events

// these are the only configstrings that the system reserves, all the
// other ones are strictly for servergame to clientgame communication
#define CS_SERVERINFO       0       // an info string with all the serverinfo cvars
#define CS_SYSTEMINFO       1       // an info string for server system to client system configuration (timescale, etc)

#define SNAPFLAG_RATE_DELAYED   1
#define SNAPFLAG_NOT_ACTIVE     2   // snapshot used during connection and for zombies
#define SNAPFLAG_SERVERCOUNT    4   // toggled every map_restart so transitions can be detected

#define MAX_NAME_LENGTH	32 // max length of a client name

#define MAX_CONFIGSTRINGS   2048

#define RESERVED_CONFIGSTRINGS  2   // game can't modify below this, only the system can

#define MAX_MODELS          256     // these are sent over the net as 8 bits (Gordon: upped to 9 bits, erm actually it was already at 9 bits, wtf? NEVAR TRUST GAMECODE COMMENTS, comments are evil :E, lets hope it doesnt horribly break anything....)
#define MAX_SOUNDS          256     // so they cannot be blindly increased

#define GENTITYNUM_BITS     10  // JPW NERVE put q3ta default back for testing	// don't need to send any more
#define MAX_GENTITIES       ( 1 << GENTITYNUM_BITS )

#define GCLIENTNUM_BITS    6
#define MAX_CLIENTS       ( 1 << GCLIENTNUM_BITS ) // JPW NERVE back to q3ta default was 128		// absolute limit

// entitynums are communicated with GENTITY_BITS, so any reserved
// values thatare going to be communcated over the net need to
// also be in this range
#define ENTITYNUM_NONE          ( MAX_GENTITIES - 1 )
#define ENTITYNUM_WORLD         ( MAX_GENTITIES - 2 )
#define ENTITYNUM_MAX_NORMAL    ( MAX_GENTITIES - 2 )

#define SNAPFLAG_RATE_DELAYED   1
#define SNAPFLAG_NOT_ACTIVE     2   // snapshot used during connection and for zombies
#define SNAPFLAG_SERVERCOUNT    4   // toggled every map_restart so transitions can be detected

#define GAME_INIT_FRAMES    3
#define FRAMETIME           100                 // msec

typedef struct usercmd_s
{
	int serverTime;
	int buttons;
	byte weapon;
	byte offHandIndex;
	int angles[3];
	char forwardmove;
	char rightmove;
} usercmd_t;

//
// usercmd_t->button bits, many of which are generated by the client system,
// so they aren't game/cgame only definitions
//
#define BUTTON_NONE        	0

#define BUTTON_FORWARD        127
#define BUTTON_BACK           -127
#define BUTTON_MOVERIGHT      127
#define BUTTON_MOVELEFT       -127

#define BUTTON_ATTACK         1
#define BUTTON_MELEE          4
#define BUTTON_USE            8
#define BUTTON_RELOAD         16
#define BUTTON_USERELOAD      32
#define BUTTON_LEANLEFT       64
#define BUTTON_LEANRIGHT      128
#define BUTTON_PRONE          256
#define BUTTON_CROUCH         512
#define BUTTON_JUMP           1024
#define BUTTON_ADS_LEGACY     2048 // unused?
#define BUTTON_ADS            4096
#define BUTTON_CANNOT_PRONE   8192
#define BUTTON_BINOCULARS     16384
#define BUTTON_HOLDBREATH     32768
#define BUTTON_FRAG           65536
#define BUTTON_SMOKE          131072
#define BUTTON_TALK           262144

// if entityState->solid == SOLID_BMODEL, modelindex is an inline model number
#define SOLID_BMODEL    0xffffff

#define MAX_TOKENLENGTH     1024
typedef struct pc_token_s
{
	int type;
	int subtype;
	int intvalue;
	float floatvalue;
	char string[MAX_TOKENLENGTH];
	int line;
	int linescrossed;
} pc_token_t;

typedef struct cplane_s
{
	vec3_t normal;
	float dist;
	byte type;
	byte signbits;
	byte pad[2];
} cplane_t;
#if defined(__i386__)
static_assert((sizeof(cplane_t) == 20), "ERROR: cplane_t size is invalid!");
#endif

typedef struct dmaterial_s
{
	char material[64];
	int surfaceFlags;
	int contentFlags;
} dmaterial_t;
#if defined(__i386__)
static_assert((sizeof(dmaterial_t) == 72), "ERROR: dmaterial_t size is invalid!");
#endif

typedef struct
{
	float fraction;
	vec3_t normal;
	int surfaceFlags;
	int contents;
	dmaterial_t *material;
	unsigned short entityNum;
	unsigned short partName;
	unsigned short partGroup;
	byte allsolid;
	byte startsolid;
} trace_t;
#if defined(__i386__)
static_assert((sizeof(trace_t) == 0x24), "ERROR: trace_t size is invalid!");
#endif

struct TraceExtents
{
	vec3_t start;
	vec3_t end;
	vec3_t invDelta;
};

void CM_CalcTraceEntents(TraceExtents *extents);

short   LittleShort( short l );
int     LittleLong( int l );
int64_t LittleLong64( int64_t l );
float   LittleFloat( float l );

short   BigShort( short l );
int BigLong( int l );
int64_t  BigLong64( int64_t l );
float   BigFloat( float l );

void Swap_Init( void );

int I_isprint( int c );
int I_islower( int c );
int I_isupper( int c );
int I_isalpha( int c );
int I_isalphanum( int c );
qboolean I_isanumber( const char *s );
qboolean I_isintegral( float f );
qboolean I_isprintstring( char* s );
void I_strncpyz( char *dest, const char *src, int destsize );
int I_strnicmp(const char *s0, const char *s1, int n);
int I_strncmp( const char *s1, const char *s2, int n );
int I_stricmp( const char *s1, const char *s2 );
char *I_strlwr( char *s1 );
char *I_strupr( char *s1 );
void I_strncat( char *dest, int size, const char *src );

int Com_sprintf(char *dest, size_t size, const char *format, ...);

float GetLeanFraction(const float fFrac);
float UnGetLeanFraction(const float fFrac);

void Com_DefaultExtension( char *path, int maxSize, const char *extension );
const char *Com_StringContains( const char *str1, const char *str2, int casesensitive);
int Com_Filter( const char *filter, const char *name, int casesensitive);
int Com_FilterPath(const char *filter, const char *name, int casesensitive);
qboolean isFloat(const char* string, int size);
qboolean isInteger(const char* string, int size);
qboolean isVector(const char* string, int size, int dim);
qboolean strToVect(const char* string, float *vect, int dim);
char ColorIndex(unsigned char c);
const char *Com_GetFilenameSubString(const char *pathname);
const char *Com_GetExtensionSubString(const char *filename);
void Com_StripExtension(const char *in, char *out);
char *I_CleanStr(char *string);

#define I_strcmp strcmp
#define I_strcpy strcpy
#define I_strlen strlen

#define Q_isprint I_isprint
#define Q_islower I_islower
#define Q_isupper I_isupper
#define Q_isalpha I_isalpha
#define Q_isalphanum I_isalphanum
#define Q_isanumber I_isanumber
#define Q_isintegral I_isintegral
#define Q_isprintstring I_isprintstring
#define Q_strncpyz I_strncpyz
#define Q_strncmp I_strncmp
#define Q_stricmp I_stricmp
#define Q_strlwr I_strlwr
#define Q_strupr I_strupr
#define Q_strncat I_strncat
#define Q_strcmp I_strcmp
#define Q_strcpy I_strcpy
#define Q_strlen I_strlen

#define Q_CleanStr I_CleanStr

int Q_vsnprintf(char *str, size_t size, const char *format, va_list args);
#define I_vsnprintf Q_vsnprintf

struct infoParm_t
{
	const char *name;
	qboolean clearSolid;
	unsigned int surfaceFlags;
	unsigned int contents;
	unsigned int toolFlags;
};

struct cspField_t
{
	const char *szName;
	int iOffset;
	int iFieldType;
};

enum csParseFieldType_t
{
	CSPFT_STRING = 0x0,
	CSPFT_STRING_MAX_STRING_CHARS = 0x1,
	CSPFT_STRING_MAX_QPATH = 0x2,
	CSPFT_STRING_MAX_OSPATH = 0x3,
	CSPFT_INT = 0x4,
	CSPFT_QBOOLEAN = 0x5,
	CSPFT_FLOAT = 0x6,
	CSPFT_MILLISECONDS = 0x7,
	CSPFT_NUM_BASE_FIELD_TYPES = 0x8,
};

const char* Com_SurfaceTypeToName(int iTypeIndex);

char *va(const char *format, ...);
void Info_RemoveKey( char *s, const char *key );
void Info_RemoveKey_Big( char *s, const char *key );
void Info_SetValueForKey( char *s, const char *key, const char *value );
void Info_SetValueForKey_Big( char *s, const char *key, const char *value );
const char *Info_ValueForKey( const char *s, const char *key );
void Info_NextPair( const char **head, char *key, char *value );
char I_CleanChar(char character);
bool Com_ValidXModelName(const char *name);
qboolean Info_Validate( const char *s );
bool Com_BitCheck(const int array[], int bitNum);
void Com_BitSet(int array[], int bitNum);
void Com_BitClear(int array[], int bitNum);
void AddLeanToPosition(float *position, float fViewYaw, float fLeanFrac, float fViewRoll, float fLeanDist);
qboolean ParseConfigStringToStruct(unsigned char *pStruct, const cspField_t *pFieldList, int iNumFields, const char *pszBuffer, int iMaxFieldTypes, int (*parseSpecialFieldType)(unsigned char *, const char *, const int), void (*parseStrCpy)(unsigned char *, const char *));
void SetConfigString(char **ppszConfigString, const char *pszKeyValue);
void SetConfigString2(unsigned char *pMember, const char *pszKeyValue);
int I_DrawStrlen(const char *str);
int Com_AddToString(const char *add, char *msg, int len, int maxlen, int mayAddQuotes);