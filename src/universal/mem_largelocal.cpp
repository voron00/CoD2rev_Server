#include "com_memory.h"
#include "../qcommon/sys_thread.h"

char g_largeLocalBuf[524288];
int g_largeLocalPos;

/*
==============
LargeLocalEnd
==============
*/
void LargeLocalEnd(int startPos)
{
	g_largeLocalPos = startPos;
}

/*
==============
LargeLocal::~LargeLocal
==============
*/
LargeLocal::~LargeLocal()
{
	LargeLocalEnd(startPos);
}

/*
==============
LargeLocalReset
==============
*/
void LargeLocalReset()
{
	g_largeLocalPos = 0;
}

/*
==============
LargeLocalBegin
==============
*/
int LargeLocalBegin(int size)
{
	int startPos;

	size = LargeLocalRoundSize(size);
	startPos = g_largeLocalPos;
	g_largeLocalPos += size;

	return startPos;
}

/*
==============
LargeLocalRoundSize

==============
*/
unsigned int LargeLocalRoundSize(int size)
{
	return (size + 3) & ~3;
}

/*
==============
LargeLocalGetBuf
==============
*/
void *LargeLocalGetBuf(int startPos, int size)
{
	int startIndex;

	if (Sys_IsMainThread())
	{
		return &g_largeLocalBuf[startPos];
	}

	startIndex = startPos - LargeLocalRoundSize(size);

	return &g_largeLocalBuf[startIndex];
}

/*
==============
LargeLocal::LargeLocal
==============
*/
LargeLocal::LargeLocal(int sizeParam)
{
	startPos = LargeLocalBegin(sizeParam);
	size = sizeParam;
}

/*
==============
LargeLocal::GetBuf
==============
*/
void *LargeLocal::GetBuf()
{
	return LargeLocalGetBuf(startPos, size);
}