#ifndef __SYS_PATCH_H__
#define __SYS_PATCH_H__

#include "../src/i_common.h"
#include "../src/i_shared.h"

DWORD SetCall(DWORD addr, DWORD destination);
DWORD SetJump(DWORD addr, DWORD destination);
DWORD SetDword(DWORD addr, DWORD replacement);

#endif
