#pragma once

#include "cm_local.h"

#define NUMTHREADS 2
#define MAX_KEYS 3
#define MAX_VASTRINGS 2

#define THREAD_CONTEXT_MAIN 0
#define THREAD_CONTEXT_DATABASE 1

struct va_info_t
{
	char va_string[MAX_VASTRINGS][MAX_STRING_CHARS];
	int index;
};

struct TraceCheckCount
{
	int global;
	int *partitions;
};

typedef struct TraceThreadInfo
{
	TraceCheckCount checkcount;
	cbrush_t *box_brush;
	cmodel_t *box_model;
} TraceThreadInfo;

extern TraceThreadInfo g_traceThreadInfo[NUMTHREADS];

void* Sys_GetValue(int key);
void Sys_SetValue(int key, void* value);
void Com_InitThreadData(int threadContext);
qboolean Sys_IsMainThread(void);
void Sys_InitMainThread();
