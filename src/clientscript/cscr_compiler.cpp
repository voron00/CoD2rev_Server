#include "../qcommon/qcommon.h"
#include "clientscript_public.h"

#ifdef TESTING_LIBRARY
#define scrCompilePub (*((scrCompilePub_t*)( 0x08202A40 )))
#else
scrCompilePub_t scrCompilePub;
#endif



int QDECL Scr_ScanFile(char *buf, int max_size)
{
	char c;
	int i;
	char ptr;

	ptr = 42;

	for ( i = 0; i < max_size; ++i )
	{
		c = *scrCompilePub.in_ptr++;
		ptr = c;

		if ( !c || c == 10 )
			break;

		buf[i] = c;
	}

	if ( ptr == 10 )
	{
		buf[i++] = 10;
	}
	else if ( !ptr )
	{
		if ( scrCompilePub.parseBuf )
		{
			scrCompilePub.in_ptr = scrCompilePub.parseBuf;
			scrCompilePub.parseBuf = 0;
		}
		else
		{
			--scrCompilePub.in_ptr;
		}
	}

	return i;
}

void QDECL Scr_YYACError(const char* fmt, ...)
{
	va_list argptr;
	char msg[MAXPRINTMSG];

	va_start(argptr, fmt);
	Q_vsnprintf(msg, sizeof(msg), fmt, argptr);
	va_end(argptr);

	Com_Error(ERR_SCRIPT, "%s", msg);
}