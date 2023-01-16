#pragma once

#include <stdint.h>

#include "cm_local.h"

#define NUMTHREADS 2
#define MAX_KEYS 3
#define MAX_VASTRINGS 2

#define THREAD_CONTEXT_MAIN 0
#define THREAD_CONTEXT_DATABASE 1

enum ThreadValue
{
	THREAD_VALUE_PROF_STACK = 0x0,
	THREAD_VALUE_VA = 0x1,
	THREAD_VALUE_COM_ERROR = 0x2,
	THREAD_VALUE_TRACE = 0x3,
	THREAD_VALUE_COUNT = 0x4,
};

struct va_info_t
{
	char va_string[MAX_VASTRINGS][MAX_STRING_CHARS];
	int index;
};

typedef struct TraceThreadInfo
{
	int global;
	int *edges;
	int *verts;
	int *partitions;
	cbrush_t *box_brush;
	cmodel_t *box_model;
} TraceThreadInfo;

extern va_info_t va_info[NUMTHREADS];
extern jmp_buf g_com_error[NUMTHREADS];
extern TraceThreadInfo g_traceThreadInfo[NUMTHREADS];

void Com_InitThreadData(int threadContext);
void* Sys_GetValue(int key);
void Sys_SetValue(int key, void* value);
qboolean Sys_IsMainThread(void);
void Sys_InitMainThread();
