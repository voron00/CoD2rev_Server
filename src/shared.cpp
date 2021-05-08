#include "i_shared.h"
#include "sys_thread.h"

// can't just use function pointers, or dll linkage can
// mess up when qcommon is included in multiple places
static short ( *_BigShort )( short l ) = NULL;
static short ( *_LittleShort )( short l ) = NULL;
static int ( *_BigLong )( int l ) = NULL;
static int ( *_LittleLong )( int l ) = NULL;
static qint64 ( *_BigLong64 )( qint64 l ) = NULL;
static qint64 ( *_LittleLong64 )( qint64 l ) = NULL;
static float ( *_BigFloat )( float l ) = NULL;
static float ( *_LittleFloat )( float l ) = NULL;

short   LittleShort( short l )
{
	return _LittleShort( l );
}
int     LittleLong( int l )
{
	return _LittleLong( l );
}
qint64  LittleLong64( qint64 l )
{
	return _LittleLong64( l );
}
float   LittleFloat( float l )
{
	return _LittleFloat( l );
}

short   BigShort( short l )
{
	return _BigShort( l );
}
int     BigLong( int l )
{
	return _BigLong( l );
}
qint64  BigLong64( qint64 l )
{
	return _BigLong64( l );
}
float   BigFloat( float l )
{
	return _BigFloat( l );
}

short   ShortSwap( short l )
{
	byte b1,b2;

	b1 = l & 255;
	b2 = ( l >> 8 ) & 255;

	return ( b1 << 8 ) + b2;
}

short   ShortNoSwap( short l )
{
	return l;
}

int    LongSwap( int l )
{
	byte b1,b2,b3,b4;

	b1 = l & 255;
	b2 = ( l >> 8 ) & 255;
	b3 = ( l >> 16 ) & 255;
	b4 = ( l >> 24 ) & 255;

	return ( (int)b1 << 24 ) + ( (int)b2 << 16 ) + ( (int)b3 << 8 ) + b4;
}

int LongNoSwap( int l )
{
	return l;
}

qint64 Long64Swap( qint64 ll )
{
	qint64 result;

	result.b0 = ll.b7;
	result.b1 = ll.b6;
	result.b2 = ll.b5;
	result.b3 = ll.b4;
	result.b4 = ll.b3;
	result.b5 = ll.b2;
	result.b6 = ll.b1;
	result.b7 = ll.b0;

	return result;
}

qint64 Long64NoSwap( qint64 ll )
{
	return ll;
}

float FloatSwap( float f )
{
	union
	{
		float f;
		byte b[4];
	} dat1, dat2;


	dat1.f = f;
	dat2.b[0] = dat1.b[3];
	dat2.b[1] = dat1.b[2];
	dat2.b[2] = dat1.b[1];
	dat2.b[3] = dat1.b[0];
	return dat2.f;
}

float FloatNoSwap( float f )
{
	return f;
}

/*
================
Swap_Init
================
*/
void Swap_Init( void )
{
	byte swaptest[2] = {1,0};

// set the byte swapping variables in a portable manner
	if ( *(short *)swaptest == 1 )
	{
		_BigShort = ShortSwap;
		_LittleShort = ShortNoSwap;
		_BigLong = LongSwap;
		_LittleLong = LongNoSwap;
		_BigLong64 = Long64Swap;
		_LittleLong64 = Long64NoSwap;
		_BigFloat = FloatSwap;
		_LittleFloat = FloatNoSwap;
	}
	else
	{
		_BigShort = ShortNoSwap;
		_LittleShort = ShortSwap;
		_BigLong = LongNoSwap;
		_LittleLong = LongSwap;
		_BigLong64 = Long64NoSwap;
		_LittleLong64 = Long64Swap;
		_BigFloat = FloatNoSwap;
		_LittleFloat = FloatSwap;
	}
}

int I_isprint( int c )
{
	if ( c >= 0x20 && c <= 0x7E )
		return ( 1 );
	return ( 0 );
}

int I_islower( int c )
{
	if (c >= 'a' && c <= 'z')
		return ( 1 );
	return ( 0 );
}

int I_isupper( int c )
{
	if (c >= 'A' && c <= 'Z')
		return ( 1 );
	return ( 0 );
}

int I_isalpha( int c )
{
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		return ( 1 );
	return ( 0 );
}

int I_isalphanum( int c )
{
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
		return ( 1 );
	return ( 0 );
}

qboolean I_isanumber( const char *s )
{
	char *p;

	if( *s == '\0' )
		return qfalse;

	strtod( s, &p );

	return *p == '\0';
}

qboolean I_isintegral( float f )
{
	return (int)f == f;
}

qboolean I_isprintstring( char* s )
{
	char* a = s;
	while( *a )
	{
		if ( *a < 0x20 || *a > 0x7E ) return 0;
		a++;
	}
	return 1;
}

void I_strncpyz( char *dest, const char *src, int destsize )
{
	strncpy( dest, src, destsize-1 );
	dest[destsize-1] = 0;
}

int I_stricmpn( const char *s1, const char *s2, int n )
{
	int c1, c2;

	do
	{
		c1 = *s1++;
		c2 = *s2++;

		if ( !n-- )
		{
			return 0;       // strings are equal until end point
		}

		if ( c1 != c2 )
		{
			if ( c1 >= 'a' && c1 <= 'z' )
			{
				c1 -= ( 'a' - 'A' );
			}
			if ( c2 >= 'a' && c2 <= 'z' )
			{
				c2 -= ( 'a' - 'A' );
			}
			if ( c1 != c2 )
			{
				return c1 < c2 ? -1 : 1;
			}
		}
	}
	while ( c1 );

	return 0;       // strings are equal
}

int I_strncmp( const char *s1, const char *s2, int n )
{
	int c1, c2;

	do
	{
		c1 = *s1++;
		c2 = *s2++;

		if ( !n-- )
		{
			return 0;       // strings are equal until end point
		}

		if ( c1 != c2 )
		{
			return c1 < c2 ? -1 : 1;
		}
	}
	while ( c1 );

	return 0;       // strings are equal
}

int I_stricmp( const char *s1, const char *s2 )
{
	return ( s1 && s2 ) ? I_stricmpn( s1, s2, 99999 ) : -1;
}

char *I_strlwr( char *s1 )
{
	char*   s;

	for ( s = s1; *s; ++s )
	{
		if ( ( 'A' <= *s ) && ( *s <= 'Z' ) )
		{
			*s -= 'A' - 'a';
		}
	}

	return s1;
}

char *I_strupr( char *s1 )
{
	char* cp;

	for ( cp = s1 ; *cp ; ++cp )
	{
		if ( ( 'a' <= *cp ) && ( *cp <= 'z' ) )
		{
			*cp += 'A' - 'a';
		}
	}

	return s1;
}

void I_strncat( char *dest, int size, const char *src )
{
	int		l1;

	l1 = strlen( dest );

	if ( l1 >= size )
	{
		Com_Error( ERR_FATAL, "I_strncat: already overflowed" );
	}

	I_strncpyz( dest + l1, src, size - l1 );
}

int Com_sprintf(char *dest, size_t size, const char *format, ...)
{
	int result;
	va_list va;

	va_start(va, format);
	result = vsnprintf(dest, size, format, va);
	va_end(va);

	dest[size - 1] = '\0';

	return result;
}

qboolean isFloat(const char* string, int size)
{
	const char* ptr;
	int i;
	qboolean dot = qfalse;
	qboolean sign = qfalse;
	qboolean whitespaceended = qfalse;

	if(size == 0) //If we have given a length compare the whole string
		size = 0x10000;

	for(i = 0, ptr = string; i < size && *ptr != '\0' && *ptr != '\n'; i++, ptr++)
	{

		if(*ptr == ' ')
		{
			if(whitespaceended == qfalse)
				continue;
			else
				return qtrue;
		}
		whitespaceended = qtrue;

		if(*ptr == '-' && sign ==0)
		{
			sign = qtrue;
			continue;
		}
		if(*ptr == '.' && dot == 0)
		{
			dot = qtrue;
			continue;
		}
		if(*ptr < '0' || *ptr > '9') return qfalse;
	}
	return qtrue;
}

qboolean isInteger(const char* string, int size)
{
	const char* ptr;
	int i;
	qboolean sign = qfalse;
	qboolean whitespaceended = qfalse;

	if(size == 0) //If we have given a length compare the whole string
		size = 0x10000;

	for(i = 0, ptr = string; i < size && *ptr != '\0' && *ptr != '\n' && *ptr != '\r'; i++, ptr++)
	{

		if(*ptr == ' ')
		{
			if(whitespaceended == qfalse)
				continue;
			else
				return qtrue;
		}
		whitespaceended = qtrue;

		if(*ptr == '-' && sign ==0)
		{
			sign = qtrue;
			continue;
		}
		if(*ptr < '0' || *ptr > '9')
		{
			return qfalse;
		}
	}
	return qtrue;
}

qboolean isVector(const char* string, int size, int dim)
{
	const char* ptr;
	int i;

	if(size == 0) //If we have given a length compare the whole string
		size = 0x10000;

	for(i = 0, ptr = string; i < size && *ptr != '\0' && *ptr != '\n' && dim > 0; i++, ptr++)
	{

		if(*ptr == ' ')
		{
			continue;
		}
		dim = dim -1;

		if(isFloat(ptr, size -i) == qfalse)
			return qfalse;

		while(*ptr != ' ' && *ptr != '\0' && *ptr != '\n' && i < size)
		{
			ptr++;
			i++;
		}

		if(*ptr != ' ')
		{
			break;
		}

	}
	if(dim != 0)
		return qfalse;

	return qtrue;
}

qboolean strToVect(const char* string, float *vect, int dim)
{
	const char* ptr;
	int i;

	for(ptr = string, i = 0; *ptr != '\0' && *ptr != '\n' && i < dim; ptr++)
	{

		if(*ptr == ' ')
		{
			continue;
		}

		vect[i] = atof(ptr);

		i++;

		while(*ptr != ' ' && *ptr != '\0' && *ptr != '\n')
		{
			ptr++;
		}


		if(*ptr != ' ')
		{
			break;
		}

	}
	if(i != dim)
		return qfalse;

	return qtrue;
}

char *va(const char *format, ...)
{
	struct va_info_t *info;
	int index;
	va_list va;

	info = (va_info_t*)Sys_GetValue(1);
	index = info->index;
	info->index = (info->index + 1) % MAX_VASTRINGS;

	va_start(va, format);
	vsnprintf(info->va_string[index], sizeof(info->va_string[0]), format, va);
	va_end(va);

	info->va_string[index][1023] = 0;

	return info->va_string[index];
}
