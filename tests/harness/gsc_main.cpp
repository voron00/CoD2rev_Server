#include "../../src/qcommon/qcommon.h"
#include "../../src/script/script_public.h"
#include "../../src/universal/com_memory.h"
#include "../../src/universal/com_files.h"

extern dvar_t *com_developer;
extern dvar_t *com_developer_script;

int main(int argc, char **argv)
{
	if ( argc < 2 )
	{
		fprintf(stderr, "Usage: %s <script> [function]\n", argv[0]);
		return 2;
	}

	const char *scriptName = argv[1];
	const char *functionName = argc >= 3 ? argv[2] : "main";

	Com_InitParse();
	Com_InitHunkMemory();

	com_developer = Dvar_RegisterInt("developer", 0, 0, 2, 0);
	com_developer_script = Dvar_RegisterBool("developer_script", false, 0);

	Scr_Init();
	Scr_InitSystem();

	Scr_BeginLoadScripts();

	unsigned int scriptId = Scr_LoadScript(scriptName);
	if ( !scriptId )
	{
		fprintf(stderr, "Failed to load script: %s\n", scriptName);
		Scr_EndLoadScripts();
		Scr_FreeScripts();
		return 1;
	}

	Scr_PostCompileScripts();

	int handle = Scr_GetFunctionHandle(scriptName, functionName);
	if ( !handle )
	{
		fprintf(stderr, "Function not found: %s::%s\n", scriptName, functionName);
		Scr_EndLoadScripts();
		Scr_FreeScripts();
		return 1;
	}

	Scr_EndLoadScripts();

	Scr_ExecThread(handle, 0);

	Scr_FreeScripts();
	return 0;
}
