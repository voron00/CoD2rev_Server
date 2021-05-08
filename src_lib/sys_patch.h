#ifndef __SYS_PATCH_H__
#define __SYS_PATCH_H__

DWORD SetCall(DWORD addr, DWORD destination);
DWORD SetJump(DWORD addr, DWORD destination);
DWORD SetDword(DWORD addr, DWORD replacement);

#endif
