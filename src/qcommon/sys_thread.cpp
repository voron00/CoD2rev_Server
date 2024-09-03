#include "qcommon.h"
#include "sys_thread.h"
#include "../universal/universal_public.h"

#ifdef _WIN32
static CRITICAL_SECTION crit_sections[CRITSECT_COUNT];
#else
static pthread_mutex_t crit_sections[CRITSECT_COUNT];
#endif

threadid_t threadId[NUMTHREADS];
threadid_t mainthread;
void* g_threadValues[NUMTHREADS];
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

#ifdef _WIN32
void Sys_InitializeCriticalSections( void )
{
	int i;

	for (i = 0; i < CRITSECT_COUNT; i++)
	{
		InitializeCriticalSection( &crit_sections[i] );
	}
}
#else
void Sys_InitializeCriticalSections( void )
{
	int i;
	pthread_mutexattr_t muxattr;

	pthread_mutexattr_init(&muxattr);
	pthread_mutexattr_settype(&muxattr, PTHREAD_MUTEX_RECURSIVE);

	for (i = 0; i < CRITSECT_COUNT; i++)
	{
		pthread_mutex_init( &crit_sections[i], &muxattr );

	}

	pthread_mutexattr_destroy(&muxattr);

}
#endif

#ifdef _WIN32
void Sys_EnterCriticalSection(int section)
{
	EnterCriticalSection(&crit_sections[section]);
}

int Sys_TryEnterCriticalSection(int section)
{
	return TryEnterCriticalSection(&crit_sections[section]) == 0;
}

void Sys_LeaveCriticalSection(int section)
{
	LeaveCriticalSection(&crit_sections[section]);
}
#else
void Sys_EnterCriticalSection(int section)
{
	pthread_mutex_lock(&crit_sections[section]);
}

int Sys_TryEnterCriticalSection(int section)
{
	return pthread_mutex_trylock(&crit_sections[section]);
}

void Sys_LeaveCriticalSection(int section)
{
	pthread_mutex_unlock(&crit_sections[section]);
}
#endif

qboolean Sys_IsMainThread(void)
{
	return Sys_ThreadisSame(mainthread);
}

void Sys_InitMainThread()
{
	Sys_InitializeCriticalSections();
	mainthread = Sys_GetCurrentThreadId();
	threadId[THREAD_CONTEXT_MAIN] = mainthread;
	Com_InitThreadData(THREAD_CONTEXT_MAIN);
}

#ifdef _WIN32
HANDLE Sys_CreateThreadWithHandle(void* (*ThreadMain)(void*), threadid_t *tid, void* arg)
{
	char errMessageBuf[512];
	DWORD lastError;


	HANDLE thid = CreateThread(	NULL, // LPSECURITY_ATTRIBUTES lpsa,
	                            0, // DWORD cbStack,
	                            (LPTHREAD_START_ROUTINE)ThreadMain, // LPTHREAD_START_ROUTINE lpStartAddr,
	                            arg, // LPVOID lpvThreadParm,
	                            0, // DWORD fdwCreate,
	                            tid );

	if(thid == NULL)
	{
		lastError = GetLastError();

		if(lastError != 0)
		{
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, lastError, MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT), (LPSTR)errMessageBuf, sizeof(errMessageBuf) -1, NULL);
			Com_Printf("Failed to start thread with error: %s\n", errMessageBuf);

		}
		else
		{
			Com_Printf("Failed to start thread!\n");
		}
		return NULL;
	}
	return thid;
}

qboolean Sys_CreateNewThread(void* (*ThreadMain)(void*), threadid_t *tid, void* arg)
{
	HANDLE thid = Sys_CreateThreadWithHandle(ThreadMain, tid, arg);

	if(thid == NULL)
	{
		return qfalse;
	}

	CloseHandle(thid);
	return qtrue;
}
#else
#define MIN_STACKSIZE 256*1024
qboolean Sys_CreateNewThread(void* (*ThreadMain)(void*), threadid_t *tid, void* arg)
{
	int err;
	pthread_attr_t tattr;

	err = pthread_attr_init(&tattr);
	if(err != 0)
	{
		Com_Printf("pthread_attr_init(): Thread creation failed with the following error: %s\n", strerror(errno));
		return qfalse;
	}

	err = pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
	if(err != 0)
	{
		pthread_attr_destroy(&tattr);
		Com_Printf("pthread_attr_setdetachstate(): Thread creation failed with the following error: %s\n", strerror(errno));
		return qfalse;
	}

	err = pthread_attr_setstacksize(&tattr, MIN_STACKSIZE);
	if(err != 0)
	{
		pthread_attr_destroy(&tattr);
		Com_Printf("pthread_attr_setstacksize(): Thread creation failed with the following error: %s\n", strerror(errno));
		return qfalse;
	}

	err = pthread_create(tid, &tattr, ThreadMain, arg);
	if(err != 0)
	{
		pthread_attr_destroy(&tattr);
		Com_Printf("pthread_create(): Thread creation failed with the following error: %s\n", strerror(errno));
		return qfalse;
	}
	pthread_attr_destroy(&tattr);
	return qtrue;
}
#endif

#ifdef _WIN32
void Sys_ExitThread(int code)
{
	ExitThread( code );

}
#else
void Sys_ExitThread(int code)
{
	pthread_exit(&code);

}
#endif

#ifdef _WIN32
void Sys_SleepMSec(int msec)
{
	Sleep(msec);
}
#else
void Sys_SleepMSec(int msec)
{
	struct timespec ts;
	ts.tv_sec = msec / 1000;
	ts.tv_nsec = (msec % 1000) * 1000000;
	nanosleep(&ts, NULL);
}
#endif