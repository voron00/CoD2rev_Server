#include "../qcommon/qcommon.h"

/*
================
Com_Memcpy
================
*/
void Com_Memcpy( unsigned char *dest, unsigned char *src, int count )
{
	memcpy( dest, src, count );
}

/*
================
Com_Memset
================
*/
void Com_Memset( void *dest, int val, int count )
{
	memset( dest, val, count );
}

/*
==================
Com_HashKey
==================
*/
int Com_HashKey( const char *string, int maxlen )
{
	int hash, i;

	hash = 0;

	for ( i = 0; i < maxlen && string[i] != '\0'; i++ )
	{
		hash += string[i] * ( 119 + i );
	}

	hash = ( hash ^ ( hash >> 10 ) ^ ( hash >> 20 ) );
	return hash;
}

/*
================
Com_RealTime
================
*/
int Com_RealTime( qtime_t *qtime ) {
	time_t t;
	struct tm *tms;

	t = time( NULL );
	if ( !qtime ) {
		return t;
	}
	tms = localtime( &t );
	if ( tms ) {
		qtime->tm_sec = tms->tm_sec;
		qtime->tm_min = tms->tm_min;
		qtime->tm_hour = tms->tm_hour;
		qtime->tm_mday = tms->tm_mday;
		qtime->tm_mon = tms->tm_mon;
		qtime->tm_year = tms->tm_year;
		qtime->tm_wday = tms->tm_wday;
		qtime->tm_yday = tms->tm_yday;
		qtime->tm_isdst = tms->tm_isdst;
	}
	return t;
}

/*
================
Com_Filter
================
*/
bool Com_Filter( const char *filter, const char *name, int casesensitive )
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
				if ( casesensitive )
					ptr = Com_StringContains(name, buf, I_strcmp);
				else
					ptr = Com_StringContains(name, buf, I_stricmp);
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

/*
================
Com_FilterPath
================
*/
bool Com_FilterPath( const char *filter, const char *name, int casesensitive )
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

/*
================
Com_StringContains
================
*/
const char *Com_StringContains( const char *str1, const char *str2, int (*cmpFunc)(const char *, const char *) )
{
	int len, i;

	len = I_strlen(str1) - I_strlen(str2);
	for ( i = 0; i <= len; i++, str1++ )
	{
		if ( !cmpFunc(str1, str2) )
			return str1;
	}
	return NULL;
}
