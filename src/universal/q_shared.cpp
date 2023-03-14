#include "../qcommon/qcommon.h"
#include "../qcommon/sys_thread.h"

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

qboolean com_fileAccessed;

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

void Swap_Init( void )
{
	byte swaptest[2] = {1,0};

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

int I_strnicmp(const char *s0, const char *s1, int n)
{
	int c1;
	int c0;

	if (!s0 || !s1)
	{
		return s1 - s0;
	}

	do
	{
		c0 = *s0++;
		c1 = *s1++;

		if (!n--)
		{
			return 0;       // strings are equal until end point
		}

		if (c0 != c1)
		{
			if (I_isupper(c0))
			{
				c0 += 32;
			}
			if (I_isupper(c1))
			{
				c1 += 32;
			}
			if (c0 != c1)
			{
				return c0 < c1 ? -1 : 1;
			}
		}
	}
	while (c0);

	return 0;
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

void I_strcat( char *dest, int size, const char *src )
{
	int l1;

	l1 = strlen( dest );

	if ( l1 >= size )
	{
		Com_Error( ERR_FATAL, "I_strcat: already overflowed" );
	}

	Q_strncpyz( dest + l1, src, size - l1 );
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

int QDECL Com_sprintf(char *dest, size_t size, const char *format, ...)
{
	int result;
	va_list va;

	va_start(va, format);
	result = I_vsnprintf(dest, size, format, va);
	va_end(va);

	dest[size - 1] = '\0';

	return result;
}

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

const char *Com_StringContains( const char *str1, const char *str2, int casesensitive)
{
	int len, i, j;

	len = strlen(str1) - strlen(str2);
	for (i = 0; i <= len; i++, str1++)
	{
		for (j = 0; str2[j]; j++)
		{
			if (casesensitive)
			{
				if (str1[j] != str2[j])
				{
					break;
				}
			}
			else
			{
				if (toupper(str1[j]) != toupper(str2[j]))
				{
					break;
				}
			}
		}
		if (!str2[j])
		{
			return str1;
		}
	}
	return NULL;
}

int Com_Filter( const char *filter, const char *name, int casesensitive)
{
	char buf[MAX_TOKEN_CHARS];
	const char *ptr;
	int i, found;

	while(*filter)
	{
		if (*filter == '*')
		{
			filter++;
			for (i = 0; *filter; i++)
			{
				if (*filter == '*' || *filter == '?') break;
				buf[i] = *filter;
				filter++;
			}
			buf[i] = '\0';
			if (strlen(buf))
			{
				ptr = Com_StringContains(name, buf, casesensitive);
				if (!ptr) return qfalse;
				name = ptr + strlen(buf);
			}
		}
		else if (*filter == '?')
		{
			filter++;
			name++;
		}
		else if (*filter == '[' && *(filter+1) == '[')
		{
			filter++;
		}
		else if (*filter == '[')
		{
			filter++;
			found = qfalse;
			while(*filter && !found)
			{
				if (*filter == ']' && *(filter+1) != ']') break;
				if (*(filter+1) == '-' && *(filter+2) && (*(filter+2) != ']' || *(filter+3) == ']'))
				{
					if (casesensitive)
					{
						if (*name >= *filter && *name <= *(filter+2)) found = qtrue;
					}
					else
					{
						if (toupper(*name) >= toupper(*filter) &&
						        toupper(*name) <= toupper(*(filter+2))) found = qtrue;
					}
					filter += 3;
				}
				else
				{
					if (casesensitive)
					{
						if (*filter == *name) found = qtrue;
					}
					else
					{
						if (toupper(*filter) == toupper(*name)) found = qtrue;
					}
					filter++;
				}
			}
			if (!found) return qfalse;
			while(*filter)
			{
				if (*filter == ']' && *(filter+1) != ']') break;
				filter++;
			}
			filter++;
			name++;
		}
		else
		{
			if (casesensitive)
			{
				if (*filter != *name) return qfalse;
			}
			else
			{
				if (toupper(*filter) != toupper(*name)) return qfalse;
			}
			filter++;
			name++;
		}
	}
	return qtrue;
}

int Com_FilterPath(const char *filter, const char *name, int casesensitive)
{
	int i;
	char new_filter[MAX_QPATH];
	char new_name[MAX_QPATH];

	for (i = 0; i < MAX_QPATH-1 && filter[i]; i++)
	{
		if ( filter[i] == '\\' || filter[i] == ':' )
		{
			new_filter[i] = '/';
		}
		else
		{
			new_filter[i] = filter[i];
		}
	}
	new_filter[i] = '\0';
	for (i = 0; i < MAX_QPATH-1 && name[i]; i++)
	{
		if ( name[i] == '\\' || name[i] == ':' )
		{
			new_name[i] = '/';
		}
		else
		{
			new_name[i] = name[i];
		}
	}
	new_name[i] = '\0';
	return Com_Filter(new_filter, new_name, casesensitive);
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

char ColorIndex(unsigned char c)
{
	if ((int)(c - 48) >= 17)
	{
		return 7;
	}
	else
	{
		return c - 48;
	}
}

const char *Com_GetFilenameSubString(const char *pathname)
{
	const char* last;

	last = pathname;
	while (*pathname)
	{
		if (*pathname == '/' || *pathname == '\\')
		{
			last = pathname + 1;
		}
		++pathname;
	}
	return last;
}

const char *Com_GetExtensionSubString(const char *filename)
{
	const char* substr;

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

void Com_StripExtension(const char *in, char *out)
{
	const char* extension;

	extension = Com_GetExtensionSubString(in);
	while (in != extension)
	{
		*out++ = *in++;
	}
	*out = 0;
}

char *va(const char *format, ...)
{
	struct va_info_t *info;
	int index;
	va_list va;

	info = (va_info_t*)Sys_GetValue(THREAD_VALUE_VA);
	index = info->index;
	info->index = (info->index + 1) % MAX_VASTRINGS;

	va_start(va, format);
	I_vsnprintf(info->va_string[index], sizeof(info->va_string[0]), format, va);
	va_end(va);

	info->va_string[index][MAX_STRING_CHARS - 1] = 0;

	return info->va_string[index];
}

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
			strcpy (start, s);	// remove this part
			return;
		}

		if (!*s)
			return;
	}

}

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
			strcpy (start, s);	// remove this part
			return;
		}

		if (!*s)
			return;
	}

}

void Info_SetValueForKey( char *s, const char *key, const char *value )
{
	char	newi[MAX_INFO_STRING];

	if ( strlen( s ) >= MAX_INFO_STRING )
	{
		Com_Error( ERR_DROP, "Info_SetValueForKey: oversize infostring" );
	}

	if (strchr (key, '\\') || strchr (value, '\\'))
	{
		Com_Printf ("Can't use keys or values with a \\\n");
		return;
	}

	if (strchr (key, ';') || strchr (value, ';'))
	{
		Com_Printf ("Can't use keys or values with a semicolon\n");
		return;
	}

	if (strchr (key, '\"') || strchr (value, '\"'))
	{
		Com_Printf ("Can't use keys or values with a \"\n");
		return;
	}

	Info_RemoveKey (s, key);
	if (!value || !strlen(value))
		return;

	Com_sprintf (newi, sizeof(newi), "\\%s\\%s", key, value);

	if (strlen(newi) + strlen(s) > MAX_INFO_STRING)
	{
		Com_Printf ("Info string length exceeded\n");
		return;
	}

	strcat (newi, s);
	strcpy (s, newi);
}

void Info_SetValueForKey_Big( char *s, const char *key, const char *value )
{
	char	newi[BIG_INFO_STRING];

	if ( strlen( s ) >= BIG_INFO_STRING )
	{
		Com_Error( ERR_DROP, "Info_SetValueForKey: oversize infostring" );
	}

	if (strchr (key, '\\') || strchr (value, '\\'))
	{
		Com_Printf ("Can't use keys or values with a \\\n");
		return;
	}

	if (strchr (key, ';') || strchr (value, ';'))
	{
		Com_Printf ("Can't use keys or values with a semicolon\n");
		return;
	}

	if (strchr (key, '\"') || strchr (value, '\"'))
	{
		Com_Printf ("Can't use keys or values with a \"\n");
		return;
	}

	Info_RemoveKey_Big (s, key);
	if (!value || !strlen(value))
		return;

	Com_sprintf (newi, sizeof(newi), "\\%s\\%s", key, value);

	if (strlen(newi) + strlen(s) > BIG_INFO_STRING)
	{
		Com_Printf ("BIG Info string length exceeded\n");
		return;
	}

	strcat (s, newi);
}

qboolean I_IsEqualUnitWSpace(char *cmp1, char *cmp2)
{
	while ( 1 )
	{
		if ( !(*cmp1) || !(*cmp2) )
			break;

		if ( *cmp1 == ' ' || *cmp2 == ' ' )
			break;

		if ( *cmp1 != *cmp2 )
			return qfalse;

		cmp1++;
		cmp2++;
	}

	if ( *cmp1 && *cmp1 != ' ')
	{
		return qfalse;
	}
	if ( *cmp2 && *cmp2 != ' ')
	{
		return qfalse;
	}

	return qtrue;
}

char *I_CleanStr(char *string)
{
	char* d;
	char c;
	char* s;
	int keep_cleaning;

	do
	{
		s = string;
		d = string;
		keep_cleaning = 0;

		while (1)
		{
			c = *s;
			if (!*s)
			{
				break;
			}
			if (s && *s == '^' && s[1] && s[1] != '^' && s[1] >= '0' && s[1] <= '@')
			{
				++s;
				keep_cleaning = 1;
			}
			else if (c >= 0x20 && c != 0x7F)
			{
				*d++ = c;
			}
			++s;
		}
		*d = '\0';
	}
	while (keep_cleaning);

	return string;
}

char I_CleanChar(char character)
{
	if ((unsigned char)character == 146)
	{
		return 39;
	}
	else
	{
		return character;
	}
}

bool Com_ValidXModelName(const char *name)
{
	return !I_strnicmp(name, "xmodel", 6) && name[6] == 47;
}

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

bool COM_BitTest(int array[], int bitNum)
{
	return (array[bitNum >> 5] >> (bitNum & 0x1F)) & 1;
}

void COM_BitSet(int array[], int bitNum)
{
	array[bitNum >> 5] |= 1 << (bitNum & 0x1F);
}

long double GetLeanFraction(const float fFrac)
{
	return (2.0f - fabs(fFrac)) * fFrac;
}

long double UnGetLeanFraction(const float fFrac)
{
	assert(fFrac >= 0);
	assert(fFrac <= 1.f);

	return 1.0f - sqrtf(1.0f - fFrac);
}