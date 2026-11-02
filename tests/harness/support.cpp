#include "../../src/qcommon/qcommon.h"
#include "../../src/script/script_public.h"
#include "../../src/universal/com_memory.h"
#include "../../src/universal/com_files.h"
#include "../../src/xanim/xanim_public.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

dvar_t *com_developer = NULL;
dvar_t *com_developer_script = NULL;
dvar_t *com_sv_running = NULL;
dvar_t *com_logfile = NULL;
dvar_t *com_dedicated = NULL;
dvar_t *com_viewlog = NULL;
dvar_t *cl_paused = NULL;

void Com_Printf(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
	fflush(stdout);
}

void Com_DPrintf(const char *fmt, ...)
{
	if ( com_developer && com_developer->current.integer )
	{
		va_list ap;
		va_start(ap, fmt);
		vprintf(fmt, ap);
		va_end(ap);
		fflush(stdout);
	}
}

int Com_sprintf(char *dest, int size, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int ret = vsnprintf(dest, size, fmt, ap);
	va_end(ap);
	return ret;
}

int Q_vsnprintf(char *str, size_t size, const char *format, va_list args)
{
	return vsnprintf(str, size, format, args);
}

void Com_PrintMessage(conChannel_t channel, const char *msg)
{
	fputs(msg, stdout);
	fflush(stdout);
}

void Com_Error(errorParm_t code, const char *fmt, ...)
{
	va_list ap;
	fprintf(stderr, "Com_Error(%d): ", (int)code);
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fprintf(stderr, "\n");
	fflush(stderr);
	exit(1);
}

void Com_ErrorCleanup(void) {}

int FS_FOpenFileByMode(const char *qpath, fileHandle_t *f, fsMode_t mode)
{
	FILE *fp = fopen(qpath, mode == FS_READ ? "rb" : "wb");
	if ( !fp )
		return -1;
	*f = (fileHandle_t)(intptr_t)fp;
	fseek(fp, 0, SEEK_END);
	long len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	return (int)len;
}

int FS_Read(void *buffer, int len, fileHandle_t h)
{
	return (int)fread(buffer, 1, len, (FILE *)(intptr_t)h);
}

void FS_FCloseFile(fileHandle_t f)
{
	FILE *fp = (FILE *)(intptr_t)f;
	if ( fp )
		fclose(fp);
}

char **FS_ListFiles(const char *path, const char *extension, FsListBehavior behavior, int *numfiles)
{
	if ( numfiles )
		*numfiles = 0;
	return NULL;
}

void FS_FreeFileList(char **list) {}
int FS_LoadStack() { return 0; }
long FS_HashFileName(const char *fname, int hashSize) { return 0; }

dvar_t *Dvar_RegisterInt(const char *dvarName, int value, int min, int max, unsigned short flags)
{
	static dvar_t d;
	d.name = dvarName;
	d.current.integer = value;
	d.type = DVAR_TYPE_INT;
	return &d;
}

dvar_t *Dvar_RegisterBool(const char *dvarName, bool value, unsigned short flags)
{
	static dvar_t d;
	d.name = dvarName;
	d.current.boolean = value;
	d.type = DVAR_TYPE_BOOL;
	return &d;
}

dvar_t *Dvar_RegisterFloat(const char *dvarName, float value, float min, float max, unsigned short flags)
{
	static dvar_t d;
	d.name = dvarName;
	d.current.decimal = value;
	d.type = DVAR_TYPE_FLOAT;
	return &d;
}

dvar_t *Dvar_RegisterString(const char *dvarName, const char *value, unsigned short flags)
{
	static dvar_t d;
	d.name = dvarName;
	d.current.string = value;
	d.type = DVAR_TYPE_STRING;
	return &d;
}

void Cmd_AddCommand(const char *cmd_name, xcommand_t function) {}

void *Hunk_AllocXAnimTreePrecache(int size) { return Hunk_AllocLowInternal(size); }

XAnim_s *XAnimCreateAnims(const char *filename, int boneCount, void *(*alloc)(int))
{
	return (XAnim_s *)Z_MallocInternal(sizeof(XAnim_s));
}

XAnimParts *XAnimPrecache(const char *filename, void *(*alloc)(int)) { return NULL; }
void XAnimCreate(XAnim_s *anims, unsigned int animIndex, const char *name) {}
void XAnimBlend(XAnim_s *anims, unsigned int animIndex, const char *name, unsigned int children, unsigned int num, unsigned int flags) {}
void XAnimSetupSyncNodes(XAnim_s *anims) {}
const char *XAnimGetAnimDebugName(const XAnim_s *anims, unsigned int index) { return ""; }
const char *XAnimGetAnimTreeDebugName(const XAnim_s *tree) { return ""; }
XAnim_s *XAnimGetAnims(const XAnimTree_s *tree) { return NULL; }
void XAnimFree(XAnimParts *parts) {}
void XAnimFreeList(XAnim *anims) {}
void XModelFree(XModel *model) {}
void XModelPartsFree(XModelParts *modelParts) {}

static void TestPrintln()
{
	printf("PRINT %s\n", Scr_GetString(0));
	Scr_AddUndefined();
}

static void TestPrintInt()
{
	printf("PRINT %ld\n", (long)Scr_GetInt(0));
	Scr_AddUndefined();
}

static void DummyFunction() { Scr_AddUndefined(); }
static void DummyMethod(scr_entref_t) { Scr_AddUndefined(); }

void (*Scr_GetFunction(const char **pName, int *type))()
{
	*type = 0;
	if ( !strcmp(*pName, "println") )
		return TestPrintln;
	if ( !strcmp(*pName, "printint") )
		return TestPrintInt;
	return DummyFunction;
}

void (*Scr_GetMethod(const char **pName, int *type))(scr_entref_t)
{
	*type = 0;
	return DummyMethod;
}

void Scr_GetObjectField(unsigned int classnum, int entnum, int offset) {}
int Scr_SetObjectField(unsigned int classnum, int entnum, int offset) { return 0; }

unsigned int GScr_AllocString(const char *string)
{
	return SL_GetString(string, 0);
}

int Sys_Milliseconds(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (int)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

void Sys_UnimplementedFunctionInternal(const char *function)
{
	Com_Printf("UNIMPLEMENTED: %s\n", function);
}

qboolean Sys_IsMainThread(void) { return qtrue; }

int I_stricmp(const char *s1, const char *s2) { return strcasecmp(s1, s2); }
int I_strncasecmp(const char *s1, const char *s2, int n) { return strncasecmp(s1, s2, n); }

void I_strncpyz(char *dest, const char *src, int destsize)
{
	if ( !destsize )
		return;
	strncpy(dest, src, destsize - 1);
	dest[destsize - 1] = '\0';
}

void I_strncat(char *dest, int size, const char *src)
{
	strncat(dest, src, size - strlen(dest) - 1);
}

char *va(const char *format, ...)
{
	static char buf[8][1024];
	static int index = 0;
	char *string = buf[index % 8];
	index++;
	va_list ap;
	va_start(ap, format);
	vsnprintf(string, 1024, format, ap);
	va_end(ap);
	return string;
}
