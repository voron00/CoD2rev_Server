#include "qcommon.h"
#include "sys_thread.h"
#include "../universal/universal_public.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

#ifdef _WIN32
typedef DWORD threadid_t;
#else
typedef pthread_t threadid_t;
#endif

threadid_t threadId[NUMTHREADS];
threadid_t mainthread;

void* g_threadValues[NUMTHREADS + MAX_KEYS];

va_info_t va_info[NUMTHREADS];
jmp_buf g_com_error[NUMTHREADS];
TraceThreadInfo g_traceThreadInfo[NUMTHREADS];

void Com_InitThreadData(int threadContext)
{
	Sys_SetValue(THREAD_VALUE_VA, &va_info[threadContext]);
	Sys_SetValue(THREAD_VALUE_COM_ERROR, &g_com_error[threadContext]);
	Sys_SetValue(THREAD_VALUE_TRACE, &g_traceThreadInfo[threadContext]);
}

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
#ifdef _WIN32
	return GetCurrentThreadId();
#else
	return pthread_self();
#endif
}

qboolean Sys_ThreadisSame(threadid_t threadid)
{
#ifdef _WIN32
	return threadid == GetCurrentThreadId();
#else
	return pthread_equal(threadid, pthread_self()) != 0;
#endif
}

qboolean Sys_IsMainThread(void)
{
	return Sys_ThreadisSame(mainthread);
}

void Sys_InitMainThread()
{
	mainthread = Sys_GetCurrentThreadId();
	threadId[THREAD_CONTEXT_MAIN] = mainthread;
	Com_InitThreadData(THREAD_CONTEXT_MAIN);
}