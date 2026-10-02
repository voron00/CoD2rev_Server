#include "../qcommon/qcommon.h"
#include "script_public.h"

scrStringGlob_t scrStringGlob;

/*
==============
SL_Shutdown
==============
*/
void SL_Shutdown()
{
	assert(scrStringGlob.inited);
	scrStringGlob.inited = false;
}

/*
==============
SL_TransferSystem
==============
*/
void SL_TransferSystem( unsigned int from, unsigned int to )
{
	RefString *refStr;
	unsigned int hash;
	HashEntry *entry;

	assert(from);
	assert(to);

	for ( hash = 1; hash < HASH_TABLE_SIZE; hash++ )
	{
		entry = &scrStringGlob.hashTable[hash];

		if ( !( entry->status_next & HASH_STAT_MASK ) )
		{
			continue;
		}

		refStr = GetRefString(entry->str);

		if ( from & refStr->user )
		{
			refStr->user &= ~from;
			refStr->user |= to;
		}
	}
}

/*
==============
SL_AddRefToString
==============
*/
void SL_AddRefToString( unsigned int stringValue )
{
	RefString *refStr = GetRefString(stringValue);

	refStr->refCount++;
	assert(refStr->refCount);
}

/*
==============
SL_FindStringOfLen
==============
*/
unsigned int SL_FindStringOfLen( const char *str, unsigned int len )
{
	unsigned int stringValue, newIndex, hash, prev;
	RefString *refStr;
	HashEntry *newEntry, *entry;

	assert(str);
	hash = GetHashCode(str, len);
	entry = &scrStringGlob.hashTable[hash];

	if ( (entry->status_next & HASH_STAT_MASK) != HASH_STAT_HEAD )
	{
		return 0;
	}

	refStr = GetRefString(entry->prev);

	if ( refStr->byteLen == (unsigned char)len && !memcmp(refStr->str, str, len) )
	{
		return entry->str;
	}

	prev = hash;
	newIndex = entry->status_next & HASH_NEXT_MASK;

	for ( newEntry = &scrStringGlob.hashTable[newIndex]; newEntry != entry; newEntry = &scrStringGlob.hashTable[newIndex] )
	{
		assert((newEntry->status_next & HASH_STAT_MASK) == HASH_STAT_MOVABLE);
		refStr = GetRefString(newEntry->prev);

		if ( refStr->byteLen == (unsigned char)len && !memcmp(refStr->str, str, len) )
		{
			scrStringGlob.hashTable[prev].status_next = scrStringGlob.hashTable[prev].status_next & HASH_STAT_MASK | newEntry->status_next & HASH_NEXT_MASK;
			newEntry->status_next = newEntry->status_next & HASH_STAT_MASK | entry->status_next & HASH_NEXT_MASK;

			entry->status_next = entry->status_next & HASH_STAT_MASK | newIndex;
			stringValue = newEntry->str;

			newEntry->prev = entry->prev;
			entry->str = stringValue;

			assert((newEntry->status_next & HASH_STAT_MASK) != HASH_STAT_FREE);
			assert((entry->status_next & HASH_STAT_MASK) != HASH_STAT_FREE);
			assert(refStr->str == SL_ConvertToString(stringValue));

			return stringValue;
		}

		prev = newIndex;
		newIndex = newEntry->status_next & HASH_NEXT_MASK;
	}

	return 0;
}

/*
==============
SL_ConvertFromString
==============
*/
unsigned int SL_ConvertFromString( const char *str )
{
	assert( str );
	return SL_ConvertFromRefString( GetRefString( str ) );
}

/*
==============
SL_GetStringLen
==============
*/
int SL_GetStringLen( unsigned int stringValue )
{
	assert( stringValue );
	return SL_GetRefStringLen( GetRefString( stringValue ) );
}

/*
==============
SL_ConvertToString
==============
*/
const char* SL_ConvertToString( unsigned int stringValue )
{
	if ( !stringValue )
	{
		return NULL;
	}

	return GetRefString( stringValue )->str;
}

/*
==============
SL_FindLowercaseString
==============
*/
unsigned int SL_FindLowercaseString( const char *str )
{
	int len, i;
	char buf[8192];

	len = I_strlen(str) + 1;

	if ( len > sizeof(buf) )
	{
		return 0;
	}

	for ( i = 0; i < len; i++ )
	{
		buf[i] = tolower(str[i]);
	}

	return SL_FindStringOfLen(buf, len);
}

/*
==============
SL_FindString
==============
*/
unsigned int SL_FindString( const char *str )
{
	return SL_FindStringOfLen( str, I_strlen( str ) + 1 );
}

/*
==============
SL_RemoveRefToStringOfLen
==============
*/
void SL_RemoveRefToStringOfLen( unsigned int stringValue, unsigned int len )
{
	RefString *refStr = GetRefString(stringValue);

	refStr->refCount--;

	if ( refStr->refCount )
	{
		return;
	}

	SL_FreeString(stringValue, refStr, len);
}

/*
==============
SL_RemoveRefToString
==============
*/
void SL_RemoveRefToString( unsigned int stringValue )
{
	SL_RemoveRefToStringOfLen( stringValue, SL_GetRefStringLen( GetRefString( stringValue) ) + 1 );
}

/*
==============
SL_ShutdownSystem
==============
*/
void SL_ShutdownSystem( unsigned int user )
{
	RefString *refStr;
	HashEntry *entry;
	unsigned int hash;

	assert(user);

	for ( hash = 1; hash < HASH_TABLE_SIZE; hash++ )
	{
		do
		{
			entry = &scrStringGlob.hashTable[hash];

			if ( !( entry->status_next & HASH_STAT_MASK ) )
			{
				break;
			}

			refStr = GetRefString(entry->str);

			if ( !( user & refStr->user ) )
			{
				break;
			}

			refStr->user &= ~user;
			scrStringGlob.nextFreeEntry = NULL;
			SL_RemoveRefToString(entry->str);
		}
		while ( scrStringGlob.nextFreeEntry );
	}
}

/*
==============
SL_GetStringForVector
==============
*/
unsigned int SL_GetStringForVector( const vec3_t v )
{
	char tempString[128];

	sprintf( tempString, "(%g, %g, %g)", v[0], v[1], v[2] );
	return SL_GetString_( tempString, 0 );
}

/*
==============
SL_GetStringForInt
==============
*/
unsigned int SL_GetStringForInt( long int i )
{
	char tempString[128];

	sprintf( tempString, "%li", i );
	return SL_GetString_( tempString, 0 );
}

/*
==============
SL_GetStringForFloat
==============
*/
unsigned int SL_GetStringForFloat( float f )
{
	char tempString[128];

	sprintf( tempString, "%g", f );
	return SL_GetString_( tempString, 0 );
}

/*
==============
Scr_SetString
==============
*/
void Scr_SetString( unsigned short *to, unsigned int from )
{
	if ( from )
	{
		SL_AddRefToString(from);
	}

	if ( *to )
	{
		SL_RemoveRefToString(*to);
	}

	*to = from;
}

/*
==============
SL_GetString
==============
*/
unsigned int SL_GetString( const char *str, unsigned int user )
{
	return SL_GetString_( str, user );
}

/*
==============
Scr_ShutdownGameStrings
==============
*/
void Scr_ShutdownGameStrings()
{
	SL_ShutdownSystem(1);
}

/*
==============
Scr_AllocString
==============
*/
unsigned int Scr_AllocString( const char *s )
{
	return SL_GetString( s, 1 );
}

/*
==============
Scr_SetStringFromCharString
==============
*/
void Scr_SetStringFromCharString( unsigned short *to, const char *from )
{
	if ( *to )
	{
		SL_RemoveRefToString(*to);
	}

	*to = SL_GetString(from, 0);
}

/*
==============
Scr_CreateCanonicalFilename
==============
*/
unsigned int Scr_CreateCanonicalFilename( const char *filename )
{
	char newFilename[MAX_STRING_CHARS];

	CreateCanonicalFilename(newFilename, filename, sizeof(newFilename));
	return SL_GetString_(newFilename, 0);
}

/*
==============
SL_GetLowercaseString_
==============
*/
unsigned int SL_GetLowercaseString_( const char *str, unsigned int user )
{
	return SL_GetLowercaseStringOfLen( str, user, I_strlen( str ) + 1 );
}

/*
==============
SL_GetLowercaseString
==============
*/
unsigned int SL_GetLowercaseString( const char *str, unsigned int user )
{
	return SL_GetLowercaseString_( str, user );
}

/*
==============
SL_Init
==============
*/
void SL_Init()
{
	unsigned int prev, hash;
	HashEntry *entry;

	assert(!scrStringGlob.inited);
	MT_Init();

	scrStringGlob.hashTable[0].status_next = 0;
	prev = 0;

	for ( hash = 1; hash < HASH_TABLE_SIZE; hash++ )
	{
		assert(!(hash & HASH_STAT_MASK));

		entry = &scrStringGlob.hashTable[hash];
		entry->status_next = 0;

		scrStringGlob.hashTable[prev].status_next |= hash;

		entry->prev = prev;
		prev = hash;
	}

	assert(!(scrStringGlob.hashTable[prev].status_next & HASH_NEXT_MASK));
	scrStringGlob.hashTable[0].prev = prev;
	scrStringGlob.inited = true;
}

/*
==============
SL_AddUserInternal
==============
*/
void SL_AddUserInternal( RefString *refStr, unsigned int user )
{
	if ( user & refStr->user )
	{
		return;
	}

	refStr->user |= user;
	refStr->refCount++;
}

/*
==============
GetHashCode
==============
*/
unsigned int GetHashCode( const char *str, unsigned int len )
{
	unsigned int hash;

	if ( len >= 256 )
	{
		return ( len >> 2 ) % ( HASH_TABLE_SIZE - 1 ) + 1;
	}

	hash = 0;

	while ( len )
	{
		hash = *str + 31 * hash;

		*str++;
		len--;
	}

	return hash % ( HASH_TABLE_SIZE - 1 ) + 1;
}

/*
==============
SL_ConvertFromRefString
==============
*/
int SL_ConvertFromRefString( RefString *refString )
{
	return MT_GetIndexByRef( (byte *)refString );
}

/*
==============
SL_GetRefStringLen
==============
*/
int SL_GetRefStringLen( RefString *refString )
{
	int len = (unsigned char)( refString->byteLen - 1 );

	while ( 1 )
	{
		if ( !refString->str[len] )
		{
			break;
		}

		len += 256;
	}

	return len;
}

/*
==============
GetRefString
==============
*/
RefString *GetRefString( unsigned int stringValue )
{
	assert(stringValue);
	assert(stringValue * MT_NODE_SIZE < MT_SIZE);
	return (RefString *)MT_GetRefByIndex( stringValue );
}

/*
==============
GetRefString
==============
*/
RefString *GetRefString( const char* str )
{
	//assert( str >= scrMemTreePub.mt_buffer && str < scrMemTreePub.mt_buffer + MT_SIZE );
	return (RefString *)( str - REFSTRING_STRING_OFFSET );
}

/*
==============
SL_Clear
==============
*/
void SL_Clear()
{
	byte *allocBits;
	HashEntry *entry;
	unsigned int hash;

	for ( hash = 1; hash < HASH_TABLE_SIZE; hash++ )
	{
		do
		{
			if ( !( scrStringGlob.hashTable[hash].status_next & HASH_STAT_MASK ) )
			{
				break;
			}

			scrStringGlob.nextFreeEntry = NULL;
			SL_RemoveAllRefToString(scrStringGlob.hashTable[hash].str);
		}
		while ( scrStringGlob.nextFreeEntry );
	}

	allocBits = MT_InitForceAlloc();

	for ( hash = 1; hash < HASH_TABLE_SIZE; hash++ )
	{
		entry = &scrStringGlob.hashTable[hash];

		if ( entry->status_next & HASH_STAT_MASK && GetRefString(entry->str)->user & 4 )
		{
			MT_ForceAllocIndex( allocBits, entry->str, I_strlen( SL_ConvertToString( entry->str ) ) + REFSTRING_STRING_OFFSET + 1 );
		}
	}

	MT_FinishForceAlloc(allocBits);
}

/*
==============
SL_FreeString
==============
*/
void SL_FreeString( unsigned int stringValue, RefString *refStr, unsigned int len )
{
	unsigned int prev, newNext, newIndex, index;
	HashEntry *newEntry, *entry;

	index = GetHashCode(refStr->str, len);
	entry = &scrStringGlob.hashTable[index];
	assert(!refStr->user);

	MT_FreeIndex(stringValue, len + REFSTRING_STRING_OFFSET);

	assert(((entry->status_next & HASH_STAT_MASK) == HASH_STAT_HEAD));
	newIndex = entry->status_next & HASH_NEXT_MASK;
	newEntry = &scrStringGlob.hashTable[newIndex];

	if ( entry->str == stringValue )
	{
		if ( newEntry == entry )
		{
			newEntry = &scrStringGlob.hashTable[index];
			newIndex = (unsigned short)index;
		}
		else
		{
			entry->status_next = newEntry->status_next & HASH_NEXT_MASK | HASH_STAT_HEAD;
			entry->prev = newEntry->prev;

			scrStringGlob.nextFreeEntry = &scrStringGlob.hashTable[index];
		}
	}
	else
	{
		prev = index;

		while ( 1 )
		{
			assert(newEntry != entry);
			assert((newEntry->status_next & HASH_STAT_MASK) == HASH_STAT_MOVABLE);

			if ( newEntry->str == stringValue )
			{
				break;
			}

			prev = newIndex;

			newIndex = newEntry->status_next & HASH_NEXT_MASK;
			newEntry = &scrStringGlob.hashTable[newIndex];
		}

		scrStringGlob.hashTable[prev].status_next = scrStringGlob.hashTable[prev].status_next & HASH_STAT_MASK | newEntry->status_next & HASH_NEXT_MASK;
	}

	assert((newEntry->status_next & HASH_STAT_MASK) != HASH_STAT_FREE);
	newNext = scrStringGlob.hashTable[0].status_next;
	assert((newNext & HASH_STAT_MASK) == HASH_STAT_FREE);

	newEntry->status_next = scrStringGlob.hashTable[0].status_next;
	newEntry->prev = 0;

	scrStringGlob.hashTable[newNext].prev = newIndex;
	scrStringGlob.hashTable[0].status_next = newIndex;
}

/*
==============
SL_RemoveAllRefToString
==============
*/
void SL_RemoveAllRefToString( unsigned int stringValue )
{
	RefString *refStr = GetRefString( stringValue );

	if ( refStr->user & 4 )
	{
		refStr->refCount = 1;
		refStr->user = 4;
		return;
	}

	refStr->refCount = 0;
	refStr->user = 0;

	SL_FreeString( stringValue, refStr, SL_GetRefStringLen( refStr ) + 1 );
}

/*
==============
CreateCanonicalFilename
==============
*/
void CreateCanonicalFilename( char *newFilename, const char *filename, int count )
{
	unsigned int c;

	assert(count);

	do
	{
		do
		{
			do
			{
				c = *filename;
				*filename++;
			}
			while ( c == '\\' );
		}
		while ( c == '/' );
		while ( c >= ' ' )
		{
			*newFilename = tolower(c);
			*newFilename++;

			--count;

			if ( !count )
			{
				Com_Error(ERR_DROP, "Filename %s exceeds maximum length of %d", filename, 0);
			}

			if ( c == '/' )
			{
				break;
			}

			c = *filename;
			*filename++;

			if ( c == '\\' )
			{
				c = '/';
			}
		}
	}
	while ( c );

	*newFilename = 0;
}

/*
==============
SL_GetLowercaseStringOfLen
==============
*/
unsigned int SL_GetLowercaseStringOfLen( const char *str, unsigned int user, unsigned int len )
{
	char buf[8192];

	if ( len > sizeof(buf) )
	{
		Com_Error(ERR_DROP, "max string length exceeded: \"%s\"", str);
	}

	for ( int i = 0; i < len; i++ )
	{
		buf[i] = tolower(str[i]);
	}

	return SL_GetStringOfLen(buf, user, len);
}

/*
==============
SL_Restart
==============
*/
void SL_Restart()
{
	if ( scrStringGlob.inited )
	{
		SL_Clear();
		return;
	}

	SL_Init();
}

/*
==============
SL_ConvertToLowercase
==============
*/
unsigned int SL_ConvertToLowercase( unsigned int stringValue, unsigned int user )
{
	unsigned int ns, len, i;
	const char *str;
	char buf[8192];

	len = SL_GetStringLen(stringValue) + 1;

	if ( len > sizeof(buf) )
	{
		return stringValue;
	}

	str = SL_ConvertToString(stringValue);

	for ( i = 0; i < len; i++ )
	{
		buf[i] = tolower(str[i]);
	}

	ns = SL_GetStringOfLen(buf, user, len);
	SL_RemoveRefToString(stringValue);

	return ns;
}

/*
==============
SL_AddUser
==============
*/
void SL_AddUser( unsigned int stringValue, unsigned int user )
{
	SL_AddUserInternal( GetRefString( stringValue ), user );
}

/*
==============
SL_GetStringOfLen
==============
*/
unsigned int SL_GetStringOfLen( const char *str, unsigned int user, unsigned int len )
{
	unsigned int hash, next, prev, newIndex, stringValue;
	HashEntry *entry, *newEntry;
	RefString *refStr;

	assert(str);
	hash = GetHashCode(str, len);
	entry = &scrStringGlob.hashTable[hash];

	if ( ( entry->status_next & HASH_STAT_MASK ) == HASH_STAT_HEAD )
	{
		refStr = GetRefString(entry->str);

		if ( refStr->byteLen == (unsigned char)len && !memcmp(refStr->str, str, len) )
		{
			SL_AddUserInternal(refStr, user);

			assert((entry->status_next & HASH_STAT_MASK) != HASH_STAT_FREE);
			assert(refStr->str == SL_ConvertToString( entry->str ));

			return entry->str;
		}

		prev = hash;
		newIndex = entry->status_next & HASH_NEXT_MASK;
		newEntry = &scrStringGlob.hashTable[newIndex];

		while ( 1 )
		{
			if ( newEntry == entry )
			{
				break;
			}

			assert((newEntry->status_next & HASH_STAT_MASK) == HASH_STAT_MOVABLE);
			refStr = GetRefString(newEntry->prev);

			if ( refStr->byteLen == (unsigned char)len && !memcmp(refStr->str, str, len) )
			{
				scrStringGlob.hashTable[prev].status_next = scrStringGlob.hashTable[prev].status_next & HASH_STAT_MASK | newEntry->status_next & HASH_NEXT_MASK;

				newEntry->status_next = newEntry->status_next & HASH_STAT_MASK | entry->status_next & HASH_NEXT_MASK;
				entry->status_next = entry->status_next & HASH_STAT_MASK | newIndex;

				stringValue = newEntry->str;
				newEntry->prev = entry->prev;
				entry->str = stringValue;

				SL_AddUserInternal(refStr, user);

				assert((newEntry->status_next & HASH_STAT_MASK) != HASH_STAT_FREE);
				assert((entry->status_next & HASH_STAT_MASK) != HASH_STAT_FREE);
				assert(refStr->str == SL_ConvertToString(stringValue));

				return stringValue;
			}

			prev = newIndex;
			newIndex = newEntry->status_next & HASH_NEXT_MASK;
			newEntry = &scrStringGlob.hashTable[newIndex];
		}

		newIndex = scrStringGlob.hashTable[0].status_next;

		if ( !newIndex )
		{
			Scr_DumpScriptThreads();
			Scr_DumpScriptVariablesDefault();
			Com_Error(ERR_DROP, "exceeded maximum number of script strings");
		}

		stringValue = MT_AllocIndex( len + REFSTRING_STRING_OFFSET );

		newEntry = &scrStringGlob.hashTable[newIndex];
		assert((newEntry->status_next & HASH_STAT_MASK) == HASH_STAT_FREE);

		scrStringGlob.hashTable[0].status_next = newEntry->status_next & HASH_NEXT_MASK;
		scrStringGlob.hashTable[scrStringGlob.hashTable[0].status_next].prev = 0;

		newEntry->status_next = entry->status_next & HASH_NEXT_MASK | HASH_STAT_MOVABLE;
		entry->status_next = entry->status_next & HASH_STAT_MASK | newIndex & HASH_NEXT_MASK;
		newEntry->prev = entry->prev;

		assert(stringValue);
		entry->str = stringValue;

		refStr = GetRefString(stringValue);
		memcpy(refStr->str, str, len);

		refStr->user = user;
		assert(refStr->user == user);

		refStr->refCount = 1;
		refStr->byteLen = len;

		assert((entry->status_next & HASH_STAT_MASK) != HASH_STAT_FREE);
		assert(refStr->str == SL_ConvertToString(stringValue));

		return stringValue;
	}

	if ( entry->status_next & HASH_STAT_MASK )
	{
		assert((entry->status_next & HASH_STAT_MASK) == HASH_STAT_MOVABLE);
		next = (unsigned short)entry->status_next & HASH_NEXT_MASK;
		prev = (unsigned short)next;

		while ( 1 )
		{
			if ( ( scrStringGlob.hashTable[prev].status_next & HASH_NEXT_MASK ) == hash )
			{
				break;
			}

			prev = scrStringGlob.hashTable[prev].status_next & HASH_NEXT_MASK;
		}

		assert(prev);
		newIndex = scrStringGlob.hashTable[0].status_next;

		if ( !newIndex )
		{
			Scr_DumpScriptThreads();
			Scr_DumpScriptVariablesDefault();
			Com_Error(ERR_DROP, "exceeded maximum number of script strings");
		}

		stringValue = MT_AllocIndex( len + REFSTRING_STRING_OFFSET );

		newEntry = &scrStringGlob.hashTable[newIndex];
		assert((newEntry->status_next & HASH_STAT_MASK) == HASH_STAT_FREE);

		scrStringGlob.hashTable[0].status_next = newEntry->status_next & HASH_NEXT_MASK;
		scrStringGlob.hashTable[scrStringGlob.hashTable[0].status_next].prev = 0;
		scrStringGlob.hashTable[prev].status_next = scrStringGlob.hashTable[prev].status_next & HASH_STAT_MASK | newIndex;

		newEntry->status_next = next | HASH_STAT_MOVABLE;
		newEntry->prev = entry->prev;
	}
	else
	{
		stringValue = MT_AllocIndex( len + REFSTRING_STRING_OFFSET );

		next = entry->status_next & HASH_NEXT_MASK;
		prev = entry->prev;

		scrStringGlob.hashTable[prev].status_next = scrStringGlob.hashTable[prev].status_next & HASH_STAT_MASK | next;
		scrStringGlob.hashTable[next].prev = prev;
	}

	assert(!(hash & HASH_STAT_MASK));
	entry->status_next = hash | HASH_STAT_HEAD;

	assert(stringValue);
	entry->str = stringValue;

	refStr = GetRefString(stringValue);
	memcpy(refStr->str, str, len);

	refStr->user = user;
	assert(refStr->user == user);

	refStr->refCount = 1;
	refStr->byteLen = len;

	assert((entry->status_next & HASH_STAT_MASK) != HASH_STAT_FREE);
	assert(refStr->str == SL_ConvertToString(stringValue));

	return stringValue;
}

/*
==============
SL_GetString_
==============
*/
unsigned int SL_GetString_( const char *str, unsigned int user )
{
	return SL_GetStringOfLen( str, user, I_strlen( str ) + 1 );
}

/*
==============
SL_TransferRefToUser
==============
*/
void SL_TransferRefToUser( unsigned int stringValue, unsigned int user )
{
	RefString *refStr = GetRefString( stringValue );

	if ( user & refStr->user )
	{
		refStr->refCount--;
		return;
	}

	refStr->user |= user;
}
