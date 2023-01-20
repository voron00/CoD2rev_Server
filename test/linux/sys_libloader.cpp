#include "sys_patch.h"

#include "../../src/qcommon/qcommon.h"

void Sys_RedirectFunctions()
{
	// Huffman
	SetJump(0x08066A70, (DWORD)Huff_Init);
	SetJump(0x0806610E, (DWORD)Huff_addRef);
	SetJump(0x080663F6, (DWORD)Huff_offsetReceive);
	SetJump(0x08066550, (DWORD)Huff_offsetTransmit);

	// MSG
	SetJump(0x08067718, (DWORD)MSG_Init);
	SetJump(0x0806ACDE, (DWORD)MSG_initHuffman);
	SetJump(0x0806775C, (DWORD)MSG_BeginReading);
	SetJump(0x080677A2, (DWORD)MSG_WriteBits);
	SetJump(0x08067868, (DWORD)MSG_WriteBit0);
	SetJump(0x080678C6, (DWORD)MSG_WriteBit1);
	SetJump(0x08067960, (DWORD)MSG_ReadBits);
	SetJump(0x08067A0E, (DWORD)MSG_ReadBit);
	SetJump(0x08067A8E, (DWORD)MSG_WriteBitsCompress);
	SetJump(0x08067AE2, (DWORD)MSG_ReadBitsCompress);
	SetJump(0x08067B4C, (DWORD)MSG_WriteByte);
	SetJump(0x08067B84, (DWORD)MSG_WriteData);
	SetJump(0x08067C2A, (DWORD)MSG_WriteLong);
	SetJump(0x08067BDA, (DWORD)MSG_WriteShort);
	SetJump(0x08067C78, (DWORD)MSG_ReadString);
	SetJump(0x08067CE4, (DWORD)MSG_WriteString);
	SetJump(0x08067D96, (DWORD)MSG_WriteBigString);
	SetJump(0x08067EE8, (DWORD)MSG_ReadByte);
	SetJump(0x08067F32, (DWORD)MSG_ReadShort);
	SetJump(0x08067F90, (DWORD)MSG_ReadLong);
	SetJump(0x08067FEC, (DWORD)MSG_ReadLong64);
	SetJump(0x080681EC, (DWORD)MSG_ReadData);
	SetJump(0x08068062, (DWORD)MSG_ReadCommandString);
	SetJump(0x080680CC, (DWORD)MSG_ReadBigString);
	SetJump(0x08068146, (DWORD)MSG_ReadStringLine);
	SetJump(0x080684E6, (DWORD)MSG_SetDefaultUserCmd);
	SetJump(0x08068ADE, (DWORD)MSG_ReadDeltaUsercmdKey);
	SetJump(0x08068E1C, (DWORD)MSG_WriteDeltaField);
	SetJump(0x080691FC, (DWORD)MSG_WriteDeltaStruct);
	SetJump(0x08069142, (DWORD)MSG_WriteDeltaObjective);
	SetJump(0x0806938C, (DWORD)MSG_WriteDeltaEntity);
	SetJump(0x080693DA, (DWORD)MSG_WriteDeltaArchivedEntity);
	SetJump(0x08069428, (DWORD)MSG_WriteDeltaClient);
	SetJump(0x080694A0, (DWORD)MSG_ReadDeltaField);
	SetJump(0x080697D8, (DWORD)MSG_ReadDeltaObjective);
	SetJump(0x0806988A, (DWORD)MSG_ReadDeltaStruct);
	SetJump(0x080699C8, (DWORD)MSG_ReadDeltaEntity);
	SetJump(0x08069A0E, (DWORD)MSG_ReadDeltaArchivedEntity);
	SetJump(0x08069A54, (DWORD)MSG_ReadDeltaClient);
	SetJump(0x08069AC4, (DWORD)MSG_WriteDeltaHudElems);
	SetJump(0x08069C10, (DWORD)MSG_ReadDeltaHudElems);
	SetJump(0x08069D40, (DWORD)MSG_WriteDeltaPlayerstate);
	SetJump(0x0806A608, (DWORD)MSG_ReadDeltaPlayerstate);
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