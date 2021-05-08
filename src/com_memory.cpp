#include "com_memory.h"
#include "sys_main.h"

void *Z_TryMallocInternal( int size )
{
	void *buf;

	buf = malloc(size);
	if (buf)
		Com_Memset(buf, 0, size);

	return buf;
}

void *Z_MallocInternal( int size )
{
	void *buf;

	buf = malloc(size);
	if (!buf)
		Sys_OutOfMemErrorInternal("src/com_memory.cpp", 15);
	Com_Memset(buf, 0, size);

	return buf;
}

void Z_FreeInternal( void *ptr )
{
	free(ptr);
}

char* CopyStringInternal(const char *in)
{
	char *copy = (char *)malloc(strlen(in) + 1);
	strcpy(copy, in);

	return copy;
}

void FreeStringInternal(char *str)
{
	if (str)
		Z_FreeInternal(str);
}
