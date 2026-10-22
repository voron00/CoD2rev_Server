#include "../qcommon/qcommon.h"

/*
==============
IntAsFloat
==============
*/
float IntAsFloat( int value )
{
	return (float)value;
}

/*
==============
Info_Validate
==============
*/
qboolean Info_Validate( const char *s )
{
	if ( strchr( s, '\"' ) )
	{
		return qfalse;
	}

	if ( strchr( s, ';' ) )
	{
		return qfalse;
	}

	return qtrue;
}

/*
==============
Info_RemoveKey
==============
*/
void Info_RemoveKey( char *s, const char *key )
{
	char	*start;
	char	pkey[MAX_INFO_KEY];
	char	value[MAX_INFO_VALUE];
	char	*o;

	if ( strlen( s ) >= MAX_INFO_STRING )
	{
		Com_Error( ERR_DROP, "Info_RemoveKey: oversize infostring" );
	}

	if (strchr (key, '\\'))
	{
		return;
	}

	while (1)
	{
		start = s;
		if (*s == '\\')
			s++;
		o = pkey;
		while (*s != '\\')
		{
			if (!*s)
				return;
			*o++ = *s++;
		}
		*o = 0;
		s++;

		o = value;
		while (*s != '\\' && *s)
		{
			if (!*s)
				return;
			*o++ = *s++;
		}
		*o = 0;

		if (!strcmp (key, pkey) )
		{
			memmove (start, s, strlen(s) + 1);	// remove this part
			return;
		}

		if (!*s)
			return;
	}

}

/*
==============
Info_NextPair
==============
*/
void Info_NextPair( const char **head, char *key, char *value )
{
	char	*o;
	const char	*s;

	s = *head;

	if ( *s == '\\' )
	{
		s++;
	}
	key[0] = 0;
	value[0] = 0;

	o = key;
	while ( *s != '\\' )
	{
		if ( !*s )
		{
			*o = 0;
			*head = s;
			return;
		}
		*o++ = *s++;
	}
	*o = 0;
	s++;

	o = value;
	while ( *s != '\\' && *s )
	{
		*o++ = *s++;
	}
	*o = 0;

	*head = s;
}

/*
==============
ConvertWcharToChar
==============
*/
void ConvertWcharToChar( const wchar_t* in, char* out, int outLength )
{
	if (outLength == 0)
		return;

	for (int i = 0; i < outLength - 1 && *in; i++)
		*out++ = char(*in++);

	*out = 0;
}

/*
==============
Com_sprintf
==============
*/
int Com_sprintf( char *dest, int size, const char *fmt, ... )
{
	int result;
	va_list va;

	va_start(va, fmt);
	result = I_vsnprintf(dest, size, fmt, va);
	va_end(va);

	dest[size - 1] = '\0';

	return result;
}

/*
==============
I_CleanChar
==============
*/
char I_CleanChar( char character )
{
	if ( (unsigned char)character == 146 )
	{
		return 39;
	}

	return character;
}

/*
==============
I_CleanStr
==============
*/
char *I_CleanStr( char *string )
{
	char*	d;
	char*	s;
	char	c;

	s = string;
	d = string;
	while ((c = *s) != 0 )
	{
		if ( Q_IsColorString( s ) )
		{
			s++;
		}
		else if ( c >= 0x20 && c <= 0x7E )
		{
			*d++ = c;
		}
		s++;
	}
	*d = '\0';

	return string;
}

/*
==============
I_DrawStrlen
==============
*/
int I_DrawStrlen( const char *str )
{
	const char *s = str;
	int count = 0;

	while ( *s )
	{
		if ( Q_IsColorString( s ) )
		{
			s++;
			s++;
		}
		else
		{
			count++;
			s++;
		}
	}

	return count;
}

/*
==============
I_strncpyz
==============
*/
void I_strncpyz( char *dest, const char *src, int destsize )
{
	assert(src);
	assert(dest);
	assert(destsize >= 1);

	strncpy(dest, src, destsize - 1);
	dest[destsize - 1] = '\0';
}

/*
==============
I_isdigit
==============
*/
bool I_isdigit( int c )
{
	return c >= '0' && c <= '9';
}

/*
==============
I_isalpha
==============
*/
bool I_isalpha( int c )
{
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		return ( true );
	return ( false );
}

/*
==============
I_isupper
==============
*/
bool I_isupper( int c )
{
	if (c >= 'A' && c <= 'Z')
		return ( true );
	return ( false );
}

/*
==============
I_islower
==============
*/
bool I_islower( int c )
{
	if (c >= 'a' && c <= 'z')
		return ( true );
	return ( false );
}

/*
==============
I_isprint
==============
*/
int I_isprint( int c )
{
	if ( c >= 0x20 && c <= 0x7E )
		return ( true );
	return ( false );
}

/*
==============
FloatWriteSwap
==============
*/
int FloatWriteSwap( float f )
{
	UNIMPLEMENTED(__FUNCTION__);
	return 0;
}

/*
==============
FloatReadSwap
==============
*/
float FloatReadSwap( int n )
{
	UNIMPLEMENTED(__FUNCTION__);
	return 0.0f;
}

// can't just use function pointers, or dll linkage can
// mess up when qcommon is included in multiple places
static short ( *_BigShort )( short l ) = NULL;
static short ( *_LittleShort )( short l ) = NULL;
static int ( *_BigLong )( int l ) = NULL;
static int ( *_LittleLong )( int l ) = NULL;
static int64_t ( *_BigLong64 )( int64_t l ) = NULL;
static int64_t ( *_LittleLong64 )( int64_t l ) = NULL;
static float ( *_BigFloat )( float l ) = NULL;
static float ( *_LittleFloat )( float l ) = NULL;

short LittleShort( short l )
{
	return _LittleShort( l );
}

int LittleLong( int l )
{
	return _LittleLong( l );
}

int64_t LittleLong64( int64_t l )
{
	return _LittleLong64( l );
}

float LittleFloat( float l )
{
	return _LittleFloat( l );
}

short BigShort( short l )
{
	return _BigShort( l );
}

int BigLong( int l )
{
	return _BigLong( l );
}

int64_t BigLong64( int64_t l )
{
	return _BigLong64( l );
}

float BigFloat( float l )
{
	return _BigFloat( l );
}

short ShortSwap( short l )
{
	byte b1,b2;

	b1 = l & 255;
	b2 = ( l >> 8 ) & 255;

	return ( b1 << 8 ) + b2;
}

short ShortNoSwap( short l )
{
	return l;
}

int LongSwap( int l )
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

int64_t Long64Swap(int64_t i)
{
	byte b1, b2, b3, b4,b5,b6,b7,b8;

	b1 = i & 255;
	b2 = ( i >> 8 ) & 255;
	b3 = ( i>>16 ) & 255;
	b4 = ( i>>24 ) & 255;
	b5 = ( i>>32 ) & 255;
	b6 = ( i>>40 ) & 255;
	b7 = ( i>>48 ) & 255;
	b8 = ( i>>56 ) & 255;

	return ((int64_t)b1 << 56) + ((int64_t)b2 << 48) + ((int64_t)b3 << 40) +((int64_t)b4 << 32) +((int64_t)b5 << 24) +((int64_t)b6 << 16) + ((int64_t)b7 << 8)+ b8;
}

int64_t Long64NoSwap( int64_t ll )
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
==============
LittleFloatWrite_Util
==============
*/
int LittleFloatWrite_Util( float f )
{
	UNIMPLEMENTED(__FUNCTION__);
	return 0;
}

/*
==============
LittleFloatRead_Util
==============
*/
float LittleFloatRead_Util( int n )
{
	UNIMPLEMENTED(__FUNCTION__);
	return 0.0f;
}

/*
==============
Com_DefaultExtension
==============
*/
void Com_DefaultExtension( char *path, int maxSize, const char *extension )
{
	char oldPath[MAX_QPATH];
	char    *src;

	//
	// if path doesn't have a .EXT, append extension
	// (extension should include the .)
	//
	src = path + strlen( path ) - 1;

	while ( *src != '/' && src != path )
	{
		if ( *src == '.' )
		{
			return;                 // it has an extension
		}
		src--;
	}

	I_strncpyz( oldPath, path, sizeof( oldPath ) );
	Com_sprintf( path, maxSize, "%s%s", oldPath, extension );
}

/*
==============
Com_GetExtensionSubString
==============
*/
const char *Com_GetExtensionSubString( const char *filename )
{
	const char* substr;

	assert(filename);

	substr = NULL;
	while (*filename)
	{
		if (*filename == '.')
		{
			substr = filename;
		}
		else if (*filename == '/' || *filename == '\\')
		{
			substr = NULL;
		}
		++filename;
	}
	if (!substr)
	{
		substr = filename;
	}
	return substr;
}

/*
============
Com_SkipPath
============
*/
char *Com_SkipPath( char *pathname )
{
	char	*last;

	last = pathname;
	while (*pathname)
	{
		if (*pathname=='/')
			last = pathname+1;
		pathname++;
	}
	return last;
}

/*
============
ColorIndex
============
*/
unsigned char ColorIndex( unsigned char c )
{
	if ( (unsigned char)( c - 48 ) < 10 )
	{
		return c - 48;
	}

	return 7;
}

/*
============
UnGetLeanFraction
============
*/
float UnGetLeanFraction( float fFrac )
{
	return 1.0f - I_sqrt( 1.0f - fFrac );
}

/*
============
Info_SetValueForKey
============
*/
void Info_SetValueForKey( char *s, const char *key, const char *value )
{
	int i, j;
	char c;
	char cleanValue[MAX_INFO_STRING];
	char newi[MAX_INFO_STRING];

	assert(value);
	if (strlen(s) >= MAX_INFO_STRING)
	{
		Com_Printf("Info_SetValueForKey: oversize infostring");
		return;
	}

	j = 0;
	for (i = 0; i < MAX_INFO_STRING - 1; ++i)
	{
		c = value[i];
		if (!c)
		{
			break;
		}
		if (c != '\\' && c != ';' && c != '\"')
		{
			assert(j < MAX_INFO_STRING);
			cleanValue[j++] = c;
		}
	}

	assert(j < MAX_INFO_STRING);
	cleanValue[j] = 0;
	if (strchr(key, '\\'))
	{
		Com_Printf("Can\'t use keys with a \\\nkey: '%s'\nvalue: '%s'", key, value);
		return;
	}

	if (strchr(key, ';'))
	{
		Com_Printf("Can\'t use keys with a semicolon\nkey: '%s'\nvalue: '%s'", key, value);
		return;
	}

	if (strchr(key, '\"'))
	{
		Com_Printf("Can\'t use keys with a \"\nkey: '%s'\nvalue: '%s'", key, value);
		return;
	}

	Info_RemoveKey(s, key);
	if (!cleanValue[0])
	{
		return;
	}

	if (Com_sprintf(newi, sizeof(newi), "\\%s\\%s", key, cleanValue) <= 0)
	{
		Com_Printf("Info buffer length exceeded, not including key/value pair inresponse\n");
		return;
	}

	if (strlen(newi) + strlen(s) > MAX_INFO_STRING)
	{
		Com_Printf("Info string length exceeded\nkey: '%s'\nvalue: '%s'\nInfo string:\n%s\n", key, value, s);
		return;
	}

	strcat(s, newi);
}

va_info_t va_info[NUMTHREADS];
jmp_buf g_com_error[NUMTHREADS];
TraceThreadInfo g_traceThreadInfo[NUMTHREADS];

/*
============
Com_InitThreadData
============
*/
void Com_InitThreadData( int threadContext )
{
	Sys_SetValue(THREAD_VALUE_VA, &va_info[threadContext]);
	Sys_SetValue(THREAD_VALUE_COM_ERROR, &g_com_error[threadContext]);
	Sys_SetValue(THREAD_VALUE_TRACE, &g_traceThreadInfo[threadContext]);
}

/*
============
I_strncat
============
*/
void I_strncat( char *dest, int size, const char *src )
{
	int l1;

	assert(size != sizeof( char * ));
	l1 = I_strlen( dest );

	if ( l1 >= size )
	{
		Com_Error( ERR_FATAL, "I_strncat: already overflowed" );
	}

	I_strncpyz( dest + l1, src, size - l1 );
}

/*
============
I_strupr
============
*/
char *I_strupr( char *s )
{
	char* iter;

	for (iter = s; *iter; ++iter)
	{
		if (I_islower(*iter))
		{
			*iter -= ' ';
		}
	}
	return s;
}

/*
============
I_strlwr
============
*/
char *I_strlwr( char *s )
{
	char* iter;

	for (iter = s; *iter; ++iter)
	{
		if (I_isupper(*iter))
		{
			*iter += ' ';
		}
	}
	return s;
}

/*
============
I_isalnum
============
*/
bool I_isalnum( int c )
{
	return I_isalpha( c ) || I_isdigit( c );
}

/*
============
FloatWriteNoSwap
============
*/
int FloatWriteNoSwap( float f )
{
	UNIMPLEMENTED(__FUNCTION__);
	return 0;
}

/*
============
FloatReadNoSwap
============
*/
float FloatReadNoSwap( int n )
{
	UNIMPLEMENTED(__FUNCTION__);
	return 0.0f;
}

/*
============
Com_StripFilename
============
*/
void Com_StripFilename( const char *in, char *out )
{
	char *end;
	I_strncpyz( out, in, strlen( in ) + 1 );
	end = Com_SkipPath( out );
	*end = 0;
}

/*
============
Com_StripExtension
============
*/
void Com_StripExtension( const char *in, char *out )
{
	const char* extension;

	extension = Com_GetExtensionSubString(in);
	while (in != extension)
	{
		*out++ = *in++;
	}
	*out = 0;
}

/*
============
I_isforfilename
============
*/
bool I_isforfilename( int c )
{
	return I_isalnum(c) || c == 95 || c == 45;
}

/*
============
GetLeanFraction
============
*/
float GetLeanFraction( const float fFrac )
{
	return ( 2.0f - I_fabs( fFrac ) ) * fFrac;
}

/*
============
Info_ValueForKey
============
*/
const char *Info_ValueForKey( const char *s, const char *key )
{
	char	pkey[BIG_INFO_KEY];
	static	char value[2][BIG_INFO_VALUE];	// use two buffers so compares
	// work without stomping on each other
	static	int	valueindex = 0;
	char	*o;

	if ( !s || !key )
	{
		return "";
	}

	if ( strlen( s ) >= BIG_INFO_STRING )
	{
		Com_Error( ERR_DROP, "Info_ValueForKey: oversize infostring" );
	}

	valueindex ^= 1;
	if (*s == '\\')
		s++;
	while (1)
	{
		o = pkey;
		while (*s != '\\')
		{
			if (!*s)
				return "";
			*o++ = *s++;
		}
		*o = 0;
		s++;

		o = value[valueindex];

		while (*s != '\\' && *s)
		{
			*o++ = *s++;
		}
		*o = 0;

		if (!I_stricmp (key, pkey) )
			return value[valueindex];

		if (!*s)
			break;
		s++;
	}

	return "";
}

/*
============
AddLeanToPosition
============
*/
void AddLeanToPosition( vec3_t position, float fViewYaw, float fLeanFrac, float fViewRoll, float fLeanDist )
{
	vec3_t vRight, vAng;
	float fLean;

	if ( fLeanFrac == 0.0f )
	{
		return;
	}

	fLean = GetLeanFraction(fLeanFrac);

	VectorSet(vAng, 0.0f, fViewYaw, fLean * fViewRoll);
	AngleVectors(vAng, 0, vRight, 0);

	fLean *= fLeanDist;
	VectorMA(position, fLean, vRight, position);
}

/*
============
ParseConfigStringToStruct
============
*/
int ParseConfigStringToStruct( unsigned char *pStruct, const cspField_t *pFieldList,
                               int iNumFields, const char *pszBuffer, int iMaxFieldTypes,
                               int (*parseSpecialFieldType)(unsigned char *, const char *, const int),
                               void (*parseStrCpy)(unsigned char *, const char *) )
{
	const char *pszKeyValue;
	int iField;

	for ( iField = 0; iField < iNumFields; iField++, pFieldList++ )
	{
		pszKeyValue = Info_ValueForKey( pszBuffer, pFieldList->szName );

		if ( !*pszKeyValue )
		{
			continue;
		}

		if ( pFieldList->iFieldType < CSPFT_NUM_BASE_FIELD_TYPES )
		{
			switch ( pFieldList->iFieldType )
			{
			case CSPFT_STRING:
				parseStrCpy(&pStruct[pFieldList->iOffset], pszKeyValue);
				break;

			case CSPFT_STRING_MAX_STRING_CHARS:
				I_strncpyz((char *)&pStruct[pFieldList->iOffset], pszKeyValue, MAX_STRING_CHARS);
				break;

			case CSPFT_STRING_MAX_QPATH:
				I_strncpyz((char *)&pStruct[pFieldList->iOffset], pszKeyValue, MAX_QPATH);
				break;

			case CSPFT_STRING_MAX_OSPATH:
				I_strncpyz((char *)&pStruct[pFieldList->iOffset], pszKeyValue, MAX_OSPATH);
				break;

			case CSPFT_INT:
				*(int *)&pStruct[pFieldList->iOffset] = atoi(pszKeyValue);
				break;

			case CSPFT_QBOOLEAN:
				*(qboolean *)&pStruct[pFieldList->iOffset] = atoi(pszKeyValue) != 0;
				break;

			case CSPFT_FLOAT:
				*(float *)&pStruct[pFieldList->iOffset] = atof(pszKeyValue);
				break;

			case CSPFT_MILLISECONDS:
				*(int *)&pStruct[pFieldList->iOffset] = (int)(atof(pszKeyValue) * 1000.0f);
				break;

			default:
				break;
			}
		}
		else
		{
			if ( iMaxFieldTypes <= 0 || pFieldList->iFieldType >= iMaxFieldTypes )
			{
				Com_Error( ERR_DROP, "Bad field type %i", pFieldList->iFieldType );
			}

			if ( !parseSpecialFieldType( pStruct, pszKeyValue, pFieldList->iFieldType ) )
			{
				return 0;
			}
		}
	}

	return iField == iNumFields;
}

/*
============
Info_RemoveKey_Big
============
*/
void Info_RemoveKey_Big( char *s, const char *key )
{
	char	*start;
	char	pkey[BIG_INFO_KEY];
	char	value[BIG_INFO_VALUE];
	char	*o;

	if ( strlen( s ) >= BIG_INFO_STRING )
	{
		Com_Error( ERR_DROP, "Info_RemoveKey_Big: oversize infostring" );
	}

	if (strchr (key, '\\'))
	{
		return;
	}

	while (1)
	{
		start = s;
		if (*s == '\\')
			s++;
		o = pkey;
		while (*s != '\\')
		{
			if (!*s)
				return;
			*o++ = *s++;
		}
		*o = 0;
		s++;

		o = value;
		while (*s != '\\' && *s)
		{
			if (!*s)
				return;
			*o++ = *s++;
		}
		*o = 0;

		if (!strcmp (key, pkey) )
		{
			memmove (start, s, strlen(s) + 1);	// remove this part
			return;
		}

		if (!*s)
			return;
	}
}

/*
============
Com_AssembleFilepath
============
*/
void Com_AssembleFilepath( const char *folder, const char *name, const char *extension, char *path, int maxCharCount )
{
	int folderLen, nameLen, extLen;

	folderLen = strlen(folder);
	nameLen = strlen(name);
	extLen = strlen(extension) + 1;

	if (folderLen + nameLen + extLen - 1 >= maxCharCount)
	{
		Com_Error( ERR_DROP, "filepath '%s%s%s' is longer than %i characters", folder, name, extension, maxCharCount - 1 );
	}

	memmove(path, folder, folderLen);
	memmove(path + folderLen, name, nameLen);
	memmove(path + folderLen + nameLen, extension, extLen);
}

/*
============
Info_SetValueForKey_Big
============
*/
void Info_SetValueForKey_Big( char *s, const char *key, const char *value )
{
	int i, j;
	char c;
	char cleanValue[BIG_INFO_STRING];
	char newi[BIG_INFO_STRING];

	assert(value);
	if (strlen(s) >= BIG_INFO_STRING)
	{
		Com_Printf("Info_SetValueForKey: oversize infostring");
		return;
	}

	j = 0;
	for (i = 0; i < BIG_INFO_STRING - 1; ++i)
	{
		c = value[i];
		if (!c)
		{
			break;
		}
		if (c != 92 && c != 59 && c != 34)
		{
			assert(j < BIG_INFO_STRING);
			cleanValue[j++] = c;
		}
	}

	assert(j < BIG_INFO_STRING);
	cleanValue[j] = 0;

	if (strchr(key, '\\'))
	{
		Com_Printf("Can\'t use keys with a \\\nkey: '%s'\nvalue: '%s'", key, value);
		return;
	}

	if (strchr(key, ';'))
	{
		Com_Printf("Can\'t use keys with a semicolon\nkey: '%s'\nvalue: '%s'", key, value);
		return;
	}

	if (strchr(key, '\"'))
	{
		Com_Printf("Can\'t use keys with a \"\nkey: '%s'\nvalue: '%s'", key, value);
		return;
	}

	Info_RemoveKey_Big(s, key);
	if (!cleanValue[0])
	{
		return;
	}

	if (Com_sprintf(newi, sizeof(newi), "\\%s\\%s", key, cleanValue) <= 0)
	{
		Com_Printf("Info buffer length exceeded, not including key/value pair in response\n");
		return;
	}

	if (strlen(newi) + strlen(s) > BIG_INFO_STRING)
	{
		Com_Printf("Big info string length exceeded\nkey: '%s'\nvalue: '%s'\nInfo string:\n%s\n",
		           key, value, s);
		return;
	}

	strcat(s, newi);
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

/*
================
I_strnicmp
================
*/
int I_strnicmp( const char *s0, const char *s1, int n )
{
	int c1;
	int c0;

	assert(s0);
	assert(s1);

	do
	{
		c0 = *s0++;
		c1 = *s1++;

		if ( --n == -1 )
		{
			return 0;       // strings are equal until end point
		}

		if ( c0 != c1 )
		{
			if ( I_islower(c0) )
			{
				c0 -= ' ';
			}
			if ( I_islower(c1) )
			{
				c1 -= ' ';
			}
			if ( c0 != c1 )
			{
				return c0 < c1 ? -1 : 1;
			}
		}
	}
	while ( c0 );

	return 0;       // strings are equal
}

/*
================
I_strncmp
================
*/
int I_strncmp( const char *s0, const char *s1, int n )
{
	int c1;
	int c0;

	assert(s0);
	assert(s1);

	do
	{
		c0 = *s0++;
		c1 = *s1++;

		if ( --n == -1 )
		{
			return 0;       // strings are equal until end point
		}

		if ( c0 != c1 )
		{
			return c0 < c1 ? -1 : 1;
		}
	}
	while ( c0 );

	return 0;       // strings are equal
}

/*
================
I_strcmp
================
*/
int I_strcmp( const char *s0, const char *s1 )
{
	assert(s0);
	assert(s1);

	return I_strncmp(s0, s1, INT_MAX);
}

/*
================
I_stricmp
================
*/
int I_stricmp( const char *s0, const char *s1 )
{
	assert(s0);
	assert(s1);

	return I_strnicmp(s0, s1, INT_MAX);
}

/*
================
va
================
*/
char *va( const char *format, ... )
{
	va_info_t *info;
	int index, len;
	va_list va;

	info = (va_info_t*)Sys_GetValue(THREAD_VALUE_VA);

	index = info->index;
	info->index = (info->index + 1) % MAX_VASTRINGS;

	va_start(va, format);
	len = I_vsnprintf(info->va_string[index], sizeof(info->va_string[0]), format, va);
	va_end(va);

	info->va_string[index][MAX_STRING_CHARS - 1] = 0;

#if 0
	if ( len >= MAX_STRING_CHARS )
	{
		Com_Error(ERR_DROP, "Attempted to overrun string in call to va()");
	}
#endif

	return info->va_string[index];
}

/*
==================
Com_BitCheck

  Allows bit-wise checks on arrays with more than one item (> 32 bits)
==================
*/
bool Com_BitCheck( const int array[], int bitNum )
{
	return (array[bitNum >> 5] >> (bitNum & 31)) & 1;
}

/*
==================
Com_BitSet

  Allows bit-wise SETS on arrays with more than one item (> 32 bits)
==================
*/
void Com_BitSet( int array[], int bitNum )
{
	array[bitNum >> 5] |= 1 << (bitNum & 31);
}

/*
==================
Com_BitClear

  Allows bit-wise CLEAR on arrays with more than one item (> 32 bits)
==================
*/
void Com_BitClear( int array[], int bitNum )
{
	array[bitNum >> 5] &= ~(1 << (bitNum & 31));
}

/*
==================
SetConfigString
==================
*/
void SetConfigString( char **ppszConfigString, const char *pszKeyValue )
{
	char *dest;

	if ( !*pszKeyValue )
	{
		*ppszConfigString = "";
		return;
	}

	dest = (char *)Hunk_AllocLowAlignInternal( strlen( pszKeyValue ) + 1, 1 );
	strcpy( dest, pszKeyValue );

	*ppszConfigString = dest;
}

/*
==================
SetConfigString2
==================
*/
void SetConfigString2( unsigned char *pMember, const char *pszKeyValue )
{
	SetConfigString( (char **)pMember, pszKeyValue );
}

/*
==================
Sys_UnimplementedFunctionInternal
==================
*/
void Sys_UnimplementedFunctionInternal(const char *function)
{
	Com_Printf( "FUNCTION NOT IMPLEMENTED!!! (%s)\n", function );
}

/*
==================
Com_FindSoundAlias
==================
*/
snd_alias_list_t* Com_FindSoundAlias( const char *name )
{
	return NULL; // Not supported
}
