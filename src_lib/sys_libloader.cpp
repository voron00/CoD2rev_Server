#include "sys_patch.h"
#include "../src/sys_thread.h"

void Sys_RedirectFunctions()
{
	// Main function hook
	// SetJump(0x080D2990, (DWORD)Sys_Main);

	// Threads
	SetJump(0x080D4444, (DWORD)Sys_InitMainThread);
	SetJump(0x080D446C, (DWORD)Sys_IsMainThread); // unused in server binary
	SetJump(0x080D44A4, (DWORD)Sys_GetValue);
	SetJump(0x080D4492, (DWORD)Sys_SetValue);
	SetJump(0x08051E9C, (DWORD)CM_InitThreadData);
}

class cCallOfDuty2Pro
{
public:
	cCallOfDuty2Pro()
	{
		// dont inherit lib of parent
		unsetenv("LD_PRELOAD");

		// otherwise the printf()'s are printed at crash/end on older os/compiler versions
		setbuf(stdout, NULL);

		// allow to write in executable memory
		mprotect((void *)0x08048000, 0x135000, PROT_READ | PROT_WRITE | PROT_EXEC);

		Sys_RedirectFunctions();

		printf("> [COD2REV] Compiled %s %s using GCC %s\n", __DATE__, __TIME__, __VERSION__);
	}

	~cCallOfDuty2Pro()
	{
		printf("> [COD2REV] Library unloaded\n");
	}
};

cCallOfDuty2Pro *pro;

void __attribute__ ((constructor)) lib_load(void)
{
	pro = new cCallOfDuty2Pro;
}

void __attribute__ ((destructor)) lib_unload(void)
{
	delete pro;
}