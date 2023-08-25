#ifndef _GSC_HPP_
#define _GSC_HPP_

#include "../qcommon/qcommon.h"
#include "../game/g_shared.h"

#define COD2_MAX_STRINGLENGTH 1024

/* default stuff */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdarg.h> // va_args
#include <unistd.h> //link, unlink, usleep
#include <dirent.h> // dir stuff
#include <stddef.h> // offsetof
#include <sys/stat.h> // fsize
#include <time.h>  // getsystemtime
#include <sys/time.h> // milliseconds
#include <ctype.h> // isdigit

#include "config.hpp"

#if COMPILE_BOTS == 1
#include "gsc_bots.hpp"
#endif

#if COMPILE_ENTITY == 1
#include "gsc_entity.hpp"
#endif

#if COMPILE_EXEC == 1
#include "gsc_exec.hpp"
#endif

#if COMPILE_LEVEL == 1
#include "gsc_level.hpp"
#endif

#if COMPILE_MEMORY == 1
#include "gsc_memory.hpp"
#endif

#if COMPILE_MYSQL == 1
#include "gsc_mysql.hpp"
#endif

#if COMPILE_PLAYER == 1
#include "gsc_player.hpp"
#endif

#if COMPILE_SQLITE == 1
#include "gsc_sqlite.hpp"
#endif

#if COMPILE_UTILS == 1
#include "gsc_utils.hpp"
#endif

#if COMPILE_WEAPONS == 1
#include "gsc_weapons.hpp"
#endif

#ifdef EXTRA_INCLUDES_INC
#include "extra/includes.hpp"
#endif

#define STACK_UNDEFINED 0
#define STACK_OBJECT 1
#define STACK_STRING 2
#define STACK_LOCALIZED_STRING 3
#define STACK_VECTOR 4
#define STACK_FLOAT 5
#define STACK_INT 6
#define STACK_CODEPOS 7
#define STACK_PRECODEPOS 8
#define STACK_FUNCTION 9
#define STACK_STACK 10
#define STACK_ANIMATION 11
#define STACK_DEVELOPER_CODEPOS 12
#define STACK_INCLUDE_CODEPOS 13
#define STACK_THREAD_LIST 14
#define STACK_THREAD_1 15
#define STACK_THREAD_2 16
#define STACK_THREAD_3 17
#define STACK_THREAD_4 18
#define STACK_STRUCT 19
#define STACK_REMOVED_ENTITY 20
#define STACK_ENTITY 21
#define STACK_ARRAY 22
#define STACK_REMOVED_THREAD 23

#define stackPushUndefined Scr_AddUndefined
#define stackPushBool Scr_AddBool
#define stackPushInt Scr_AddInt
#define stackPushFloat Scr_AddFloat
#define stackPushString Scr_AddString
#define stackPushVector Scr_AddVector
#define stackPushEntity Scr_AddEntity
#define stackPushArray Scr_MakeArray
#define stackPushArrayLast Scr_AddArray
#define stackPushObject Scr_AddObject

#ifndef Q_vsnprintf
int Q_vsnprintf(char *s0, size_t size, const char *fmt, va_list args);
#endif

int stackGetParamType(int param);
const char *stackGetParamTypeAsString(int param);

int stackGetParams(const char *params, ...);
void stackError(const char *format, ...);

int stackGetParamInt(int param, int *value);
int stackGetParamFunction(int param, int *value);
int stackGetParamString(int param, const char **value);
int stackGetParamConstString(int param, unsigned int *value);
int stackGetParamVector(int param, vec3_t value);
int stackGetParamFloat(int param, float *value);
int stackGetParamObject(int param, unsigned int *value);

xfunction_t Scr_GetCustomFunction(const char **fname, qboolean *fdev);
xmethod_t Scr_GetCustomMethod(const char **fname, qboolean *fdev);

uint64_t Sys_Milliseconds64(void);

#endif
