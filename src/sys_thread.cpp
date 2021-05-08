#include "i_shared.h"
#include "sys_thread.h"
#include <pthread.h>

typedef pthread_t threadid_t;

threadid_t threadId[NUMTHREADS];
threadid_t mainthread;

void* g_threadValues[NUMTHREADS + MAX_KEYS];

va_info_t va_info[NUMTHREADS];
jmp_buf g_com_error[NUMTHREADS];
TraceThreadInfo g_traceThreadInfo[NUMTHREADS];

void* Sys_GetValue(int key)
{
	return g_threadValues[key];
}

void Sys_SetValue(int key, void* value)
{
	g_threadValues[key] = value;
}

threadid_t Sys_GetCurrentThreadId()
{
	threadid_t thread = pthread_self();
	return thread;
}

qboolean Sys_ThreadisSame(threadid_t threadid)
{
	threadid_t thread = pthread_self();
	return pthread_equal(threadid, thread) != 0;
}

qboolean Sys_IsMainThread(void)
{
	return Sys_ThreadisSame(mainthread);
}

void Com_InitThreadData(int threadContext)
{
	Sys_SetValue(1, &va_info[threadContext]);
	Sys_SetValue(2, &g_com_error[threadContext]);
	Sys_SetValue(3, &g_traceThreadInfo[threadContext]);
}

void Sys_InitMainThread()
{
	threadId[0] = Sys_GetCurrentThreadId();
	Com_InitThreadData(THREAD_CONTEXT_MAIN);
}