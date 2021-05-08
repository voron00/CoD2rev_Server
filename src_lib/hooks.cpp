#include "i_shared.h"
#include "i_functions.h"
#include "i_declarations.h"
#include "dvar.h"
#include "server.h"
#include "sys_thread.h"

cvar_t *sv_maxclients;
cvar_t *sv_allowDownload;
cvar_t *sv_pure;

void hook_sv_init(const char *format, ...)
{
	char s[MAX_STRING_CHARS];
	va_list va;

	va_start(va, format);
	vsnprintf(s, sizeof(s), format, va);
	va_end(va);

	Com_Printf("%s", s);

	sv_maxclients = Cvar_FindVar("sv_maxclients");
	sv_allowDownload = Cvar_FindVar("sv_allowDownload");
	sv_pure = Cvar_FindVar("sv_pure");
}