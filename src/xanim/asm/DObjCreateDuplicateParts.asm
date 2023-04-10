extern memset
extern DObjGetBoneIndex
extern SL_ConvertToString
extern Com_Printf
extern SL_GetStringOfLen
extern g_empty


global DObjCreateDuplicateParts


section .text


DObjCreateDuplicateParts:
   push    ebp
   mov     ebp, esp
   push    ebx
   sub     esp, 4A4h
   lea     eax, [ebp-498h]
   add     eax, 10h
   mov     [ebp-2Ch], eax
   mov     dword [esp+8], 10h
   mov     dword [esp+4], 0
   lea     eax, [ebp-498h]
   mov     [esp], eax
   call    memset
   mov     dword [ebp-30h], 0
   mov     eax, [ebp+8]
   mov     eax, [eax+1Ch]
   mov     eax, [eax]
   movsx   eax, word [eax]
   mov     [ebp-10h], eax
   mov     dword [ebp-0Ch], 1
loc_80B71F8:
   mov     eax, [ebp+8]
   movzx   eax, byte [eax+18h]
   cmp     [ebp-0Ch], eax
   jl      loc_80B7209
   jmp     loc_80B7363
loc_80B7209:
   mov     edx, [ebp+8]
   mov     eax, [ebp-0Ch]
   mov     eax, [edx+eax*4+1Ch]
   mov     [ebp-14h], eax
   mov     eax, [ebp+8]
   add     eax, [ebp-0Ch]
   add     eax, 30h
   cmp     byte [eax+0Ch], 0FFh
   jz      loc_80B722A
   jmp     loc_80B734C
loc_80B722A:
   mov     eax, [ebp-14h]
   mov     eax, [eax]
   mov     [ebp-3Ch], eax
   mov     eax, [ebp-3Ch]
   mov     eax, [eax+4]
   mov     [ebp-24h], eax
   mov     eax, [ebp-24h]
   mov     eax, [eax]
   mov     [ebp-28h], eax
   mov     eax, [ebp-3Ch]
   movsx   eax, word [eax]
   mov     [ebp-20h], eax
   mov     byte [ebp-31h], 0
   mov     dword [ebp-18h], 0FFFFFFFFh
   mov     dword [ebp-1Ch], 0
loc_80B725E:
   mov     eax, [ebp-1Ch]
   cmp     eax, [ebp-20h]
   jl      loc_80B726B
   jmp     loc_80B7309
loc_80B726B:
   mov     eax, [ebp-1Ch]
   lea     edx, [eax+eax]
   mov     eax, [ebp-28h]
   movzx   eax, word [eax+edx]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    DObjGetBoneIndex
   mov     [ebp-18h], eax
   mov     eax, [ebp-1Ch]
   add     eax, [ebp-10h]
   cmp     [ebp-18h], eax
   jnz     loc_80B7297
   jmp     loc_80B72FF
loc_80B7297:
   cmp     dword [ebp-1Ch], 0
   jnz     loc_80B72A1
   mov     byte [ebp-31h], 1
loc_80B72A1:
   mov     eax, [ebp-1Ch]
   add     eax, [ebp-10h]
   mov     [ebp-38h], eax
   mov     eax, [ebp-30h]
   mov     edx, [ebp-2Ch]
   add     edx, eax
   movzx   eax, byte [ebp-38h]
   inc     al
   mov     [edx], al
   mov     eax, [ebp-38h]
   mov     ebx, eax
   sar     ebx, 5
   mov     eax, [ebp-38h]
   mov     edx, eax
   sar     edx, 5
   mov     ecx, [ebp-38h]
   and     ecx, 1Fh
   mov     eax, 1
   shl     eax, cl
   or      eax, [ebp+edx*4-498h]
   mov     [ebp+ebx*4-498h], eax
   lea     eax, [ebp-30h]
   inc     dword [eax]
   mov     eax, [ebp-30h]
   mov     edx, [ebp-2Ch]
   add     edx, eax
   movzx   eax, byte [ebp-18h]
   inc     al
   mov     [edx], al
   lea     eax, [ebp-30h]
   inc     dword [eax]
loc_80B72FF:
   lea     eax, [ebp-1Ch]
   inc     dword [eax]
   jmp     loc_80B725E
loc_80B7309:
   cmp     byte [ebp-31h], 0
   jnz     loc_80B734C
   mov     eax, [ebp-28h]
   movzx   eax, word [eax]
   mov     [esp], eax
   call    SL_ConvertToString
   mov     edx, eax
   mov     eax, [ebp+8]
   mov     eax, [eax+1Ch]
   mov     eax, [eax+88h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp-14h]
   mov     eax, [eax+88h]
   mov     [esp+8], eax
   mov     [esp+4], edx
   mov     dword [esp], aWarningAttempt
   call    Com_Printf
loc_80B734C:
   lea     eax, [ebp-0Ch]
   inc     dword [eax]
   mov     eax, [ebp-14h]
   mov     eax, [eax]
   movsx   edx, word [eax]
   lea     eax, [ebp-10h]
   add     [eax], edx
   jmp     loc_80B71F8
loc_80B7363:
   cmp     dword [ebp-30h], 0
   jz      loc_80B73A8
   mov     eax, [ebp-30h]
   add     eax, [ebp-2Ch]
   mov     byte [eax], 0
   lea     eax, [ebp-30h]
   inc     dword [eax]
   mov     ebx, [ebp+8]
   mov     dword [esp+0Ch], 0Ch
   mov     eax, [ebp-30h]
   add     eax, 10h
   mov     [esp+8], eax
   mov     dword [esp+4], 0
   lea     eax, [ebp-498h]
   mov     [esp], eax
   call    SL_GetStringOfLen
   mov     [ebx+10h], ax
   jmp     loc_80B73B6
loc_80B73A8:
   mov     edx, [ebp+8]
   movzx   eax, word [g_empty]
   mov     [edx+10h], ax
loc_80B73B6:
   add     esp, 4A4h
   pop     ebx
   pop     ebp
   retn    


section .rdata
aWarningAttempt db 'WARNING: Attempting to meld model, but root part ',27h,'%s',27h,' '
db 'of model ',27h,'%s',27h,' not found in model ',27h,'%s',27h,' or '
db 'any of its descendants',0Ah,0