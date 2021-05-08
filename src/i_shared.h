#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <setjmp.h>

typedef unsigned int long DWORD;
typedef unsigned short WORD;
typedef unsigned char byte;

typedef int	fileHandle_t;
typedef int	clipHandle_t;

#define qboolean int
#define qtrue 1
#define qfalse 0

#ifndef LOWORD
#define LOWORD(a) ((WORD)(a))
#endif

#ifndef HIWORD
#define HIWORD(a) ((WORD)(((DWORD)(a) >> 16) & 0xFFFF))
#endif

#define MAX_STRING_CHARS    1024
#define MAX_STRING_TOKENS   256
#define MAX_TOKEN_CHARS     1024

#define MAX_INFO_STRING     1024
#define MAX_INFO_KEY        1024
#define MAX_INFO_VALUE      1024

#define BIG_INFO_STRING     8192
#define BIG_INFO_KEY        8192
#define BIG_INFO_VALUE      8192

#define MAX_QPATH           64
#define MAX_OSPATH          256

#define HOMEPATH_NAME_UNIX ".callofduty2"
#define PATH_SEP '/'

#define ERR_FATAL 0
#define ERR_DROP 1

#define I_strcmp strcmp
#define I_strcpy strcpy
#define I_vsnprintf vsnprintf

void Com_ErrorMessage(int err, const char* fmt,...);

// Temp
#define Com_Printf printf
#define Com_DPrintf printf
#define Com_Error Com_ErrorMessage

#define PRODUCT_VERSION "1.0"
#define GAME_STRING "CoD2 MP"
#define PLATFORM_STRING "linux-i386"

// 64-bit integers for global rankings interface
// implemented as a struct for qvm compatibility
typedef struct
{
	byte b0;
	byte b1;
	byte b2;
	byte b3;
	byte b4;
	byte b5;
	byte b6;
	byte b7;
} qint64;

//=============================================

short   LittleShort( short l );
int     LittleLong( int l );
qint64  LittleLong64( qint64 l );
float   LittleFloat( float l );

short   BigShort( short l );
int BigLong( int l );
qint64  BigLong64( qint64 l );
float   BigFloat( float l );

int I_isprint( int c );
int I_islower( int c );
int I_isupper( int c );
int I_isalpha( int c );
int I_isalphanum( int c );
qboolean I_isanumber( const char *s );
qboolean I_isintegral( float f );
qboolean I_isprintstring( char* s );
void I_strncpyz( char *dest, const char *src, int destsize );
int I_stricmpn( const char *s1, const char *s2, int n );
int I_strncmp( const char *s1, const char *s2, int n );
int I_stricmp( const char *s1, const char *s2 );
char *I_strlwr( char *s1 );
char *I_strupr( char *s1 );
void I_strncat( char *dest, int size, const char *src );
int Com_sprintf(char *dest, size_t size, const char *format, ...);
qboolean isFloat(const char* string, int size);
qboolean isInteger(const char* string, int size);
qboolean isVector(const char* string, int size, int dim);
qboolean strToVect(const char* string, float *vect, int dim);
char *va(const char *format, ...);
void Swap_Init( void );
