#include "../qcommon/qcommon.h"
#include "clientscript_public.h"

#ifdef TESTING_LIBRARY
#define scrVarPub (*((scrVarPub_t*)( 0x08394000 )))
#else
extern scrVarPub_t scrVarPub;
#endif

unsigned int Scr_GetCanonicalStringIndex(unsigned int index)
{
	SL_TransferRefToUser(index, 2);

	if ( scrCompilePub.canonicalStrings[index] )
		return scrCompilePub.canonicalStrings[index];

	scrCompilePub.canonicalStrings[index] = ++scrVarPub.canonicalStrCount;
	return scrVarPub.canonicalStrCount;
}

unsigned int SL_GetCanonicalString(const char *str)
{
	unsigned int stringValue;
	unsigned int index;

	stringValue = SL_FindString(str);

	if ( scrCompilePub.canonicalStrings[stringValue] )
		return scrCompilePub.canonicalStrings[stringValue];

	index = SL_GetString_(str, 0);
	return Scr_GetCanonicalStringIndex(index);
}