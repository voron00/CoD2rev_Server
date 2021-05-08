#include "i_shared.h"
#include "sys_patch.h"

DWORD SetCall(DWORD addr, DWORD destination)
{
	DWORD restore = *(DWORD *)addr;

	DWORD relative = destination - (addr + 5); // +5 is the position of next opcode
	memcpy((void *)(addr + 1), &relative, 4); // set relative address with endian

	return restore;
}

DWORD SetJump(DWORD addr, DWORD destination)
{
	DWORD restore = *(DWORD *)addr;

	DWORD relative = destination - (addr + 5); // +5 is the position of next opcode
	memset((void *)addr, 0xE9, 1); // JMP-OPCODE
	memcpy((void *)(addr + 1), &relative, 4); // set relative address with endian

	return restore;
}

DWORD SetDword(DWORD addr, DWORD replacement)
{
	DWORD restore = *(DWORD *)addr;
	*(DWORD *)addr = replacement;

	return restore;
}
