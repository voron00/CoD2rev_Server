#include "i_shared.h"
#include "i_functions.h"
#include "sys_patch.h"
#include "sys_thread.h"
#include "server.h"
#include "dvar.h"
#include "cm_local.h"
#include "netchan.h"
#include "filesystem.h"
#include "sys_main.h"

extern void SV_CalcPings();
extern void SV_WriteDownloadToClient(client_t *cl, msg_t *msg);

void Sys_RedirectFunctions()
{
	// Hooks to get certain values from the binary
	//SetCall(0x08061FE7, (DWORD)hook_sv_init);

	// Reversed Threads
	//SetJump(0x080D4444, (DWORD)Sys_InitMainThread);
	//SetJump(0x08051E9C, (DWORD)CM_InitThreadData);
	//SetJump(0x080D44A4, (DWORD)Sys_GetValue);
	//SetJump(0x080D4492, (DWORD)Sys_SetValue);

	// Reversed functions
	//SetJump(0x0809443E, (DWORD)SV_CalcPings);
	//SetJump(0x0808E544, (DWORD)SV_WriteDownloadToClient);
	//SetJump(0x0806B510, (DWORD)Netchan_Setup);
	//SetJump(0x0806B9CA, (DWORD)Netchan_Process);
	//SetJump(0x0806B58C, (DWORD)Netchan_TransmitNextFragment);
	//SetJump(0x0806B7AC, (DWORD)Netchan_Transmit);

	// Other testing stuff
	//Cmd_AddCommand("dvarlist_t", Dvar_List_f);
	//Cmd_AddCommand("set_t", Dvar_Set_f);

	//Sys_InitMainThread();
	//FS_InitFilesystem();
	//Netchan_Init(28962);

	// Main function hook (testing)
	SetJump(0x080D2990, (DWORD)Sys_Main);
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