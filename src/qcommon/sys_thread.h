#pragma once

#include <stdint.h>
#include <errno.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

#ifdef _WIN32
typedef DWORD threadid_t;
typedef HANDLE mutex_t;
#else
typedef pthread_t threadid_t;
typedef pthread_mutex_t mutex_t;
#endif

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

enum CriticalSection
{
	CRITSECT_EXEC,
	CRITSECT_MYSQL,
	CRITSECT_SQLITE,
	CRITSECT_COUNT
};

struct va_info_t
{
	char va_string[MAX_VASTRINGS][MAX_STRING_CHARS];
	int index;
};

extern va_info_t va_info[NUMTHREADS];
extern jmp_buf g_com_error[NUMTHREADS];
extern TraceThreadInfo g_traceThreadInfo[NUMTHREADS];

void Com_InitThreadData(int threadContext);
void* Sys_GetValue(int key);
void Sys_SetValue(int key, void* value);
qboolean Sys_IsMainThread(void);
void Sys_InitMainThread();

void Sys_EnterCriticalSection(int section);
int Sys_TryEnterCriticalSection(int section);
void Sys_LeaveCriticalSection(int section);

qboolean Sys_CreateNewThread(void* (*ThreadMain)(void*), threadid_t *tid, void* arg);
void Sys_ExitThread(int code);
void Sys_SleepMSec(int msec);