#include "../qcommon/qcommon.h"
#include "clientscript_public.h"

#define HASH_TABLE_SIZE 16384

#define HASH_NEXT_MASK 0x3FFF
#define HASH_STAT_MASK 0xC000
#define HASH_STAT_HEAD 0x8000
#define HASH_STAT_MOVABLE 0x4000

struct HashEntry
{
	uint16_t status_next;
	union
	{
		uint16_t prev;
		uint16_t str;
	};
};

typedef struct __attribute__((aligned(128))) scrStringGlob_s
{
	HashEntry hashTable[HASH_TABLE_SIZE];
	bool inited;
	HashEntry *nextFreeEntry;
} scrStringGlob_t;
static_assert((sizeof(scrStringGlob_t) == 0x10080), "ERROR: scrStringGlob_t size is invalid!");

scrStringGlob_t scrStringGlob;

RefString *GetRefString(unsigned int stringValue)
{
	return (RefString *)MT_GetRefByIndex(stringValue);
}

RefString *GetRefString(const char* string)
{
	return (RefString *)(string - 4);
}

int SL_GetRefStringLen(RefString *refString)
{
	int len;

	if(!refString->length)
	{
		len = 256 - 1; //Bugfix for 256 % 256 = 0 or 512 % 256 = 0 or... Just promote it to 256
	}
	else
	{
		len = refString->length - 1;
	}

	while(refString->str[len])
	{
		len += 256;
	}

	return len;
}

int SL_GetStringLen(unsigned int stringValue)
{
	RefString *refString;

	refString = GetRefString(stringValue);
	return SL_GetRefStringLen(refString);
}

const char* SL_ConvertToString(unsigned int index)
{
	const char* string;
	RefString* rf;

	if ( index )
	{
		rf = GetRefString(index);
		string = rf->str;
		return string;
	}

	return NULL;
}

unsigned int SL_ConvertToLowercase(unsigned int stringValue, unsigned char user)
{
	const char *cstr;
	char str[8192];
	unsigned int ns;
	unsigned int len;
	unsigned int i;

	len = SL_GetStringLen(stringValue) + 1;

	if ( len > sizeof(str) )
	{
		return stringValue;
	}

	cstr = SL_ConvertToString(stringValue);

	for ( i = 0; i < len; ++i )
	{
		str[i] = tolower(cstr[i]);
	}

	ns = SL_GetStringOfLen(str, user, len);
	SL_RemoveRefToString(stringValue);

	return ns;
}

void SL_RemoveRefToStringOfLen(unsigned int stringValue, unsigned int len)
{
	RefString *refStr;

	refStr = GetRefString(stringValue);

	if ( !--refStr->refCount )
		SL_FreeString(stringValue, refStr, len);
}

void SL_RemoveRefToString(unsigned int stringValue)
{
	int len;
	RefString *refString;

	refString = GetRefString(stringValue);
	len = SL_GetRefStringLen(refString) + 1;
	SL_RemoveRefToStringOfLen(stringValue, len);
}

int SL_ConvertFromRefString(RefString *refString)
{
	return MT_GetIndexByRef((byte*)refString);
}

unsigned int SL_ConvertFromString(const char *str)
{
	return SL_ConvertFromRefString(GetRefString(str));
}

unsigned int SL_GetLowercaseStringOfLen(const char *upperstring, unsigned char user, unsigned int len)
{
	unsigned int i;
	char buffer[8192];

	if ( len > 8192 )
		Com_Error(ERR_DROP, "max string length exceeded: \"%s\"", upperstring);

	for ( i = 0; i < len; ++i )
		buffer[i] = tolower(upperstring[i]);

	return SL_GetStringOfLen(buffer, user, len);
}

unsigned int SL_GetLowercaseString_(const char *str, unsigned char user)
{
	return SL_GetLowercaseStringOfLen(str, user, I_strlen(str) + 1);
}

unsigned int SL_FindLowercaseString(const char *upperstring)
{
	signed int len;
	signed int i;
	char lwrstr[8192];

	len = I_strlen(upperstring) + 1;

	if ( len > 8192 )
		return 0;

	for ( i = 0; i < len; ++i )
		lwrstr[i] = tolower(upperstring[i]);

	return SL_FindStringOfLen(lwrstr, len);
}

unsigned int GetHashCode(const char *str, unsigned int len)
{
	unsigned int i;

	if (len >= 0xFF)
		return (len >> 2) % 0x3FFF + 1;

	for (i = 0; len; --len)
		i = 31 * i + *str++;

	return i % 0x3FFF + 1;
}

void SL_FreeString(unsigned int stringValue, RefString *refStr, unsigned int len)
{
	unsigned int prev;
	unsigned int newNext;
	unsigned int newIndex;
	unsigned int index;
	HashEntry *newEntry;
	HashEntry *entry;

	index = GetHashCode(refStr->str, len);
	entry = &scrStringGlob.hashTable[index];
	MT_FreeIndex(stringValue, len + 4);
	newIndex = entry->status_next & HASH_NEXT_MASK;
	newEntry = &scrStringGlob.hashTable[newIndex];

	if ( entry->prev == stringValue )
	{
		if ( newEntry == entry )
		{
			newEntry = &scrStringGlob.hashTable[index];
			newIndex = index;
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

		while ( newEntry->prev != stringValue )
		{
			assert(newEntry != entry);

			prev = newIndex;
			newIndex = newEntry->status_next & HASH_NEXT_MASK;
			newEntry = &scrStringGlob.hashTable[newIndex];
		}

		scrStringGlob.hashTable[prev].status_next = scrStringGlob.hashTable[prev].status_next & HASH_STAT_MASK | newEntry->status_next & HASH_NEXT_MASK;
	}

	newNext = scrStringGlob.hashTable->status_next;
	newEntry->status_next = scrStringGlob.hashTable->status_next;
	newEntry->prev = 0;
	scrStringGlob.hashTable[newNext].prev = newIndex;
	scrStringGlob.hashTable->status_next = newIndex;
}

void SL_AddUserInternal(RefString *refStr, unsigned char user)
{
	if ( !(user & refStr->user) )
	{
		refStr->user |= user;
		++refStr->refCount;
	}
}

void SL_AddUser(unsigned int stringValue, unsigned char user)
{
	RefString *RefString;

	RefString = GetRefString(stringValue);
	SL_AddUserInternal(RefString, user);
}

unsigned int SL_GetStringOfLen(const char *str, unsigned char user, unsigned int len)
{
	unsigned int prevNode;
	unsigned int stringValue;
	unsigned int newNext;
	unsigned int newIndex;
	unsigned int hash;
	RefString *refStr;
	HashEntry *entry;
	HashEntry *prev;
	HashEntry *newEntry;
	unsigned short next;
	int i;

	assert(str != NULL);

	hash = GetHashCode(str, len);
	entry = &scrStringGlob.hashTable[hash];

	if ( (entry->status_next & HASH_STAT_MASK) != HASH_STAT_HEAD )
	{
		if ( (entry->status_next & HASH_STAT_MASK) != 0 )
		{
			next = entry->status_next & HASH_NEXT_MASK;

			for ( i = next;
			        (scrStringGlob.hashTable[i].status_next & HASH_NEXT_MASK) != hash;
			        i = scrStringGlob.hashTable[i].status_next & HASH_NEXT_MASK )
			{
				;
			}

			assert(i != 0);
			newIndex = scrStringGlob.hashTable->status_next;

			if ( !scrStringGlob.hashTable->status_next )
			{
				Scr_DumpScriptThreads();
				Scr_DumpScriptVariables();
				Com_Error(ERR_DROP, "\x15" "exceeded maximum number of script strings\n");
			}

			stringValue = MT_AllocIndex(len + 4);
			newEntry = &scrStringGlob.hashTable[newIndex];
			scrStringGlob.hashTable->status_next = newEntry->status_next & HASH_NEXT_MASK;
			scrStringGlob.hashTable[scrStringGlob.hashTable->status_next].prev = 0;
			scrStringGlob.hashTable[i].status_next = scrStringGlob.hashTable[i].status_next & HASH_STAT_MASK | newIndex;
			newEntry->status_next = next | HASH_STAT_MOVABLE;
			newEntry->prev = entry->prev;
		}
		else
		{
			stringValue = MT_AllocIndex(len + 4);
			newIndex = entry->status_next & HASH_NEXT_MASK;
			prevNode = entry->prev;
			scrStringGlob.hashTable[prevNode].status_next = scrStringGlob.hashTable[prevNode].status_next & HASH_STAT_MASK | newIndex;
			scrStringGlob.hashTable[newIndex].prev = prevNode;
		}

		assert(stringValue != 0);

		entry->status_next = hash | HASH_STAT_HEAD;
		entry->prev = stringValue;
		refStr = GetRefString(stringValue);
		memcpy(refStr->str, str, len);
		refStr->user = user;
		refStr->refCount = 1;
		refStr->length = len;

		return stringValue;
	}

	refStr = GetRefString(entry->prev);

	if ( refStr->length != len || memcmp(refStr->str, str, len) )
	{
		newIndex = entry->status_next & HASH_NEXT_MASK;

		for ( prev = &scrStringGlob.hashTable[newIndex]; prev != entry; prev = &scrStringGlob.hashTable[newIndex] )
		{
			refStr = GetRefString(prev->prev);

			if ( refStr->length == len && !memcmp(refStr->str, str, len) )
			{
				scrStringGlob.hashTable[hash].status_next = scrStringGlob.hashTable[hash].status_next & HASH_STAT_MASK | prev->status_next & HASH_NEXT_MASK;
				prev->status_next = prev->status_next & HASH_STAT_MASK | entry->status_next & HASH_NEXT_MASK;
				entry->status_next = entry->status_next & HASH_STAT_MASK | newIndex;
				stringValue = prev->prev;
				prev->prev = entry->prev;
				entry->prev = stringValue;
				SL_AddUserInternal(refStr, user);

				return stringValue;
			}

			hash = newIndex;
			newIndex = prev->status_next & HASH_NEXT_MASK;
		}

		newNext = scrStringGlob.hashTable->status_next;

		if ( !scrStringGlob.hashTable->status_next )
		{
			Scr_DumpScriptThreads();
			Scr_DumpScriptVariables();
			Com_Error(ERR_DROP, "\x15" "exceeded maximum number of script strings\n");
		}

		stringValue = MT_AllocIndex(len + 4);
		newEntry = &scrStringGlob.hashTable[newNext];
		scrStringGlob.hashTable->status_next = newEntry->status_next & HASH_NEXT_MASK;
		scrStringGlob.hashTable[scrStringGlob.hashTable->status_next].prev = 0;
		newEntry->status_next = entry->status_next & HASH_NEXT_MASK | HASH_STAT_MOVABLE;
		entry->status_next = entry->status_next & HASH_STAT_MASK | newNext & HASH_NEXT_MASK;
		newEntry->prev = entry->prev;
		entry->prev = stringValue;
		refStr = GetRefString(stringValue);
		memcpy(refStr->str, str, len);
		refStr->user = user;
		refStr->refCount = 1;
		refStr->length = len;

		return stringValue;
	}

	SL_AddUserInternal(refStr, user);
	return entry->prev;
}

unsigned int SL_FindStringOfLen(const char *str, unsigned int len)
{
	unsigned int prev;
	unsigned int newIndex;
	unsigned int hash;
	RefString *refStr;
	HashEntry *newEntry;
	unsigned int index;
	HashEntry *entry;

	hash = GetHashCode(str, len);
	entry = &scrStringGlob.hashTable[hash];

	if ( (entry->status_next & HASH_STAT_MASK) != HASH_STAT_HEAD )
		return 0;

	refStr = GetRefString(entry->prev);

	if ( refStr->length != len || memcmp(refStr->str, str, len) )
	{
		index = hash;
		newIndex = entry->status_next & HASH_NEXT_MASK;

		for ( newEntry = &scrStringGlob.hashTable[newIndex]; newEntry != entry; newEntry = &scrStringGlob.hashTable[newIndex] )
		{
			refStr = GetRefString(newEntry->prev);

			if ( refStr->length == len && !memcmp(refStr->str, str, len) )
			{
				scrStringGlob.hashTable[index].status_next = scrStringGlob.hashTable[index].status_next & HASH_STAT_MASK | newEntry->status_next & HASH_NEXT_MASK;
				newEntry->status_next = newEntry->status_next & HASH_STAT_MASK | entry->status_next & HASH_NEXT_MASK;
				entry->status_next = entry->status_next & HASH_STAT_MASK | newIndex;
				prev = newEntry->prev;
				newEntry->prev = entry->prev;
				entry->prev = prev;

				return prev;
			}

			index = newIndex;
			newIndex = newEntry->status_next & HASH_NEXT_MASK;
		}

		return 0;
	}

	return entry->prev;
}

// VoroN: Wrapper for assembly
unsigned int QDECL SL_GetStringOfSize(const char *str, unsigned char user, unsigned int len, int type)
{
	return SL_GetStringOfLen(str, user, len);
}

unsigned int SL_FindString(const char *string)
{
	return SL_FindStringOfLen(string, I_strlen(string) + 1);
}

unsigned int SL_GetString_(const char *str, unsigned char user)
{
	return SL_GetStringOfLen(str, user, I_strlen(str) + 1);
}

unsigned int SL_GetString(const char *str, unsigned char user)
{
	return SL_GetString_(str, user);
}

unsigned int SL_GetStringForInt(int i)
{
	char s[128];

	sprintf(s, "%i", i);
	return SL_GetString_(s, 0);
}

unsigned int SL_GetStringForFloat(float f)
{
	char s[128];

	sprintf(s, "%g", f);
	return SL_GetString_(s, 0);
}

unsigned int SL_GetStringForVector(const float *v)
{
	char s[256];

	sprintf(s, "(%g, %g, %g)", v[0], v[1], v[2]);
	return SL_GetString_(s, 0);
}

unsigned int Scr_AllocString(const char *string)
{
	return SL_GetString(string, 1);
}

void Scr_SetString(uint16_t *to, unsigned int from)
{
	if ( from )
		SL_AddRefToString(from);

	if ( *to )
		SL_RemoveRefToString(*to);

	*to = from;
}

void SL_AddRefToString(unsigned int stringValue)
{
	RefString *RefString;

	RefString = GetRefString(stringValue);
	++RefString->refCount;
}

void SL_FreeEntry(unsigned int stringValue)
{
	int RefStringLen;
	RefString *refStr;

	refStr = GetRefString(stringValue);

	if ( (refStr->user & 4) != 0 )
	{
		refStr->refCount = 1;
		refStr->user = 4;
	}
	else
	{
		refStr->refCount = 0;
		refStr->user = 0;
		RefStringLen = SL_GetRefStringLen(refStr);
		SL_FreeString(stringValue, refStr, RefStringLen + 1);
	}
}

void SL_TransferRefToUser(unsigned int stringIndex, unsigned char user)
{
	RefString *refStr;

	refStr = GetRefString(stringIndex);

	if ( (user & refStr->user) != 0 )
		--refStr->refCount;
	else
		refStr->user |= user;
}

void SL_ChangeUser(unsigned char from, unsigned char to)
{
	RefString *RefString;
	unsigned int i;

	for ( i = 1; i < HASH_TABLE_SIZE; ++i )
	{
		if ( (scrStringGlob.hashTable[i].status_next & HASH_STAT_MASK) != 0 )
		{
			RefString = GetRefString(scrStringGlob.hashTable[i].prev);

			if ( (from & RefString->user) != 0 )
			{
				RefString->user &= ~from;
				RefString->user |= to;
			}
		}
	}
}

void SL_CreateCanonicalFilename(char *newFilename, const char *filename, int count)
{
	unsigned int c;

	while (1)
	{
		do
		{
			c = *filename++;
		}
		while (c == '\\');

		if (c != '/')
		{
			while (c >= ' ')
			{
				*newFilename++ = tolower(c);
				--count;

				if (!count)
				{
					Com_Error(ERR_DROP, "\x15" "Filename '%s' exceeds maximum length", filename);
				}

				if (c == '/')
				{
					break;
				}

				c = *filename++;

				if (c == '\\')
				{
					c = '/';
				}
			}

			if (!c)
			{
				break;
			}
		}
	}

	*newFilename = 0;
}

unsigned int Scr_CreateCanonicalFilename(const char *name)
{
	char newFilename[MAX_STRING_CHARS];

	SL_CreateCanonicalFilename(newFilename, name, MAX_STRING_CHARS);
	return SL_GetString_(newFilename, 0);
}

void SL_RelocateSystem()
{
	size_t length;
	const char *string;
	byte *ptr;
	HashEntry *entry;
	unsigned int i;

	for ( i = 1; i < HASH_TABLE_SIZE; ++i )
	{
		do
		{
			if ( (scrStringGlob.hashTable[i].status_next & HASH_STAT_MASK) == 0 )
				break;

			scrStringGlob.nextFreeEntry = 0;
			SL_FreeEntry(scrStringGlob.hashTable[i].prev);
		}
		while ( scrStringGlob.nextFreeEntry );
	}

	ptr = (byte *)MT_BeginRelocate();

	for ( i = 1; i < HASH_TABLE_SIZE; ++i )
	{
		entry = &scrStringGlob.hashTable[i];

		if ( (entry->status_next & HASH_STAT_MASK) != 0 && (GetRefString(entry->prev)->user & 4) != 0 )
		{
			string = SL_ConvertToString(entry->prev);
			length = I_strlen(string);
			MT_FreeForLength(ptr, entry->prev, length + 5);
		}
	}

	MT_EndRelocate(ptr);
}

void SL_ShutdownSystem(unsigned char user)
{
	RefString *refStr;
	HashEntry *entry;
	unsigned int i;

	for ( i = 1; i < HASH_TABLE_SIZE; ++i )
	{
		do
		{
			entry = &scrStringGlob.hashTable[i];

			if ( (entry->status_next & HASH_STAT_MASK) == 0 )
				break;

			refStr = GetRefString(entry->prev);

			if ( (user & refStr->user) == 0 )
				break;

			refStr->user &= ~user;
			scrStringGlob.nextFreeEntry = 0;
			SL_RemoveRefToString(entry->prev);
		}
		while ( scrStringGlob.nextFreeEntry );
	}
}

void SL_Shutdown()
{
	if ( scrStringGlob.inited )
		scrStringGlob.inited = 0;
}

void SL_Init()
{
	unsigned int prev;
	unsigned int hash;

	MT_Init();
	scrStringGlob.hashTable->status_next = 0;
	prev = 0;

	for ( hash = 1; hash < HASH_TABLE_SIZE; ++hash )
	{
		scrStringGlob.hashTable[hash].status_next = 0;
		scrStringGlob.hashTable[prev].status_next |= hash;
		scrStringGlob.hashTable[hash].prev = prev;
		prev = hash;
	}

	scrStringGlob.hashTable->prev = prev;
	scrStringGlob.inited = 1;
}

void SL_CheckInit()
{
	if ( scrStringGlob.inited )
		SL_RelocateSystem();
	else
		SL_Init();
}