#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include "com_math.h"

#define PRODUCT_VERSION "1.0"
#define GAME_STRING "CoD2 MP"

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
#define	CPUSTRING	"win-x86-debug"
#elif defined _M_ALPHA
#define	CPUSTRING	"win-AXP-debug"
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
#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

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
#define	MAX_STRING_TOKENS	1024	// max tokens resulting from Cmd_TokenizeString
#define	MAX_TOKEN_CHARS		1024	// max length of an individual token

#define	MAX_INFO_STRING		1024
#define	MAX_INFO_KEY		1024
#define	MAX_INFO_VALUE		1024

#define	BIG_INFO_STRING		8192  // used for system info key only
#define	BIG_INFO_KEY		8192
#define	BIG_INFO_VALUE		8192

#define MAX_QPATH           64
#define MAX_OSPATH          256

#define MAX_EVENTS          4   // max events per frame before we drop events

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

// 64-bit integers for global rankings interface
// implemented as a struct for qvm compatibility
typedef struct
{
	byte	b0;
	byte	b1;
	byte	b2;
	byte	b3;
	byte	b4;
	byte	b5;
	byte	b6;
	byte	b7;
} qint64;

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
static_assert((sizeof(cplane_t) == 20), "ERROR: cplane_t size is invalid!");

typedef struct dmaterial_s
{
	char material[64];
	int surfaceFlags;
	int contentFlags;
} dmaterial_t;
static_assert((sizeof(dmaterial_t) == 72), "ERROR: dmaterial_t size is invalid!");

typedef struct
{
	float fraction;
	vec3_t normal;
	int surfaceFlags;
	int contents;
	dmaterial_t *material;
	unsigned short hitId;
	unsigned short partName;
	unsigned short partGroup;
	byte allsolid;
	byte startsolid;
} trace_t;
static_assert((sizeof(trace_t) == 0x24), "ERROR: trace_t size is invalid!");

struct TraceExtents
{
	vec3_t start;
	vec3_t end;
	vec3_t invDelta;
};

#ifdef __cplusplus
extern "C" {
#endif
void CM_CalcTraceExtents(TraceExtents *extents);
#ifdef __cplusplus
}
#endif

#define SOLID_BMODEL 0xffffff
#define CONTENTS_SOLID          1       	// an eye is never valid in a solid
#define CONTENTS_BODY           0x2000000   // should never be on a brush, only in game

extern qboolean com_fileAccessed;

short   LittleShort( short l );
int     LittleLong( int l );
qint64  LittleLong64( qint64 l );
float   LittleFloat( float l );

short   BigShort( short l );
int BigLong( int l );
qint64  BigLong64( qint64 l );
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
int I_stricmpn( const char *s1, const char *s2, int n );
int I_strncmp( const char *s1, const char *s2, int n );
int I_stricmp( const char *s1, const char *s2 );
char *I_strlwr( char *s1 );
char *I_strupr( char *s1 );
void I_strcat( char *dest, int size, const char *src );
void I_strncat( char *dest, int size, const char *src );

#ifdef __cplusplus
extern "C" {
#endif

int QDECL Com_sprintf(char *dest, size_t size, const char *format, ...);

#ifdef __cplusplus
}
#endif

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
#define Q_stricmpn I_stricmpn
#define Q_strncmp I_strncmp
#define Q_stricmp I_stricmp
#define Q_strlwr I_strlwr
#define Q_strupr I_strupr
#define Q_strcat I_strcat
#define Q_strncat I_strncat
#define Q_strcmp I_strcmp
#define Q_strcpy I_strcpy
#define Q_strlen I_strlen

int Q_vsnprintf(char *str, size_t size, const char *format, va_list args);
#define I_vsnprintf Q_vsnprintf

char *va(const char *format, ...);
void Info_RemoveKey( char *s, const char *key );
void Info_RemoveKey_Big( char *s, const char *key );
void Info_SetValueForKey( char *s, const char *key, const char *value );
void Info_SetValueForKey_Big( char *s, const char *key, const char *value );
const char *Info_ValueForKey( const char *s, const char *key );
void Info_NextPair( const char **head, char *key, char *value );
qboolean I_IsEqualUnitWSpace(char *cmp1, char *cmp2);
char *I_CleanStr(char *string);
char I_CleanChar(char character);
bool Com_ValidXModelName(const char *name);
qboolean Info_Validate( const char *s );
bool COM_BitTest(int array[], int bitNum);
void COM_BitSet(int array[], int bitNum);
void COM_BitClear(int array[], int bitNum);
long double GetLeanFraction(const float fFrac);
long double UnGetLeanFraction(const float fFrac);
void AddLeanToPosition(float *position, const float fViewYaw, const float fLeanFrac, const float fViewRoll, const float fLeanDist);