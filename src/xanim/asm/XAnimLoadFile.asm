extern Com_Printf
extern Com_sprintf
extern FS_FreeFile
extern FS_ReadFile
extern SL_GetStringOfLen
extern SL_GetString_
extern memcpy
extern strlen
extern floor
extern sqrt
extern XModelDataReadShort
extern XModelDataReadFloat


global XAnimLoadNotifyInfo
global XAnimLoadFile


section .text


sub_80C1718:
   push    ebp
   mov     ebp, esp
   sub     esp, 38h
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XModelDataReadShort
   mov     edx, eax
   mov     eax, [ebp+0Ch]
   mov     [eax], dx
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XModelDataReadShort
   mov     edx, eax
   mov     eax, [ebp+0Ch]
   add     eax, 2
   mov     [eax], dx
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XModelDataReadShort
   mov     edx, eax
   mov     eax, [ebp+0Ch]
   add     eax, 4
   mov     [eax], dx
   mov     eax, [ebp+0Ch]
   movsx   eax, word [eax]
   mov     [ebp-18h], eax
   mov     eax, [ebp+0Ch]
   add     eax, 2
   movsx   eax, word [eax]
   mov     [ebp-14h], eax
   mov     eax, [ebp+0Ch]
   add     eax, 4
   movsx   eax, word [eax]
   mov     [ebp-10h], eax
   mov     eax, [ebp-18h]
   mov     edx, eax
   imul    edx, [ebp-18h]
   mov     eax, [ebp-14h]
   imul    eax, [ebp-14h]
   add     edx, eax
   mov     eax, [ebp-10h]
   imul    eax, [ebp-10h]
   add     edx, eax
   mov     eax, 3FFF0001h
   sub     eax, edx
   mov     [ebp-1Ch], eax
   cmp     dword [ebp-1Ch], 0
   jle     loc_80C17DE
   fild    dword [ebp-1Ch]
   fstp    dword [esp]
   call    sub_80C2C4E
   fld     dword [flt_81492E4]
   faddp   st1, st0
   fstp    qword [esp]
   call    floor
   fnstcw  word [ebp-1Eh]
   movzx   eax, word [ebp-1Eh]
   or      ax, 0C00h
   mov     [ebp-20h], ax
   fldcw   word [ebp-20h]
   fistp   dword [ebp-28h]
   fldcw   word [ebp-1Eh]
   jmp     loc_80C17E5
loc_80C17DE:
   mov     dword [ebp-28h], 0
loc_80C17E5:
   mov     eax, [ebp-28h]
   mov     [ebp-0Ch], eax
   mov     edx, [ebp+0Ch]
   add     edx, 6
   mov     eax, [ebp-0Ch]
   mov     [edx], ax
   leave   
   retn    


sub_80C2C6E:
   push    ebp
   mov     ebp, esp
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp+8]
   mov     eax, [eax]
   mov     [edx], eax
   mov     edx, [ebp+0Ch]
   add     edx, 4
   mov     eax, [ebp+8]
   add     eax, 4
   mov     eax, [eax]
   mov     [edx], eax
   mov     edx, [ebp+0Ch]
   add     edx, 8
   mov     eax, [ebp+8]
   add     eax, 8
   mov     eax, [eax]
   mov     [edx], eax
   pop     ebp
   retn    


section .text


sub_80C2C4E:
   push    ebp
   mov     ebp, esp
   sub     esp, 18h
   fld     dword [ebp+8]
   fstp    qword [esp]
   call    sqrt
   fstp    dword [ebp-4]
   mov     eax, [ebp-4]
   mov     [ebp-8], eax
   fld     dword [ebp-8]
   leave   
   retn    


sub_80C17FA:
   push    ebp
   mov     ebp, esp
   sub     esp, 28h
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XModelDataReadShort
   mov     edx, eax
   mov     eax, [ebp+0Ch]
   mov     [eax], dx
   mov     eax, [ebp+0Ch]
   movsx   eax, word [eax]
   mov     [ebp-8], eax
   mov     eax, [ebp-8]
   mov     edx, eax
   imul    edx, [ebp-8]
   mov     eax, 3FFF0001h
   sub     eax, edx
   mov     [ebp-0Ch], eax
   cmp     dword [ebp-0Ch], 0
   jle     loc_80C186A
   fild    dword [ebp-0Ch]
   fstp    dword [esp]
   call    sub_80C2C4E
   fld     dword [flt_81492E4]
   faddp   st1, st0
   fstp    qword [esp]
   call    floor
   fnstcw  word [ebp-0Eh]
   movzx   eax, word [ebp-0Eh]
   or      ax, 0C00h
   mov     [ebp-10h], ax
   fldcw   word [ebp-10h]
   fistp   dword [ebp-18h]
   fldcw   word [ebp-0Eh]
   jmp     loc_80C1871
loc_80C186A:
   mov     dword [ebp-18h], 0
loc_80C1871:
   mov     eax, [ebp-18h]
   mov     [ebp-4], eax
   mov     edx, [ebp+0Ch]
   add     edx, 2
   mov     eax, [ebp-4]
   mov     [edx], ax
   leave   
   retn    


sub_80C2CD4:
   push    ebp
   mov     ebp, esp
   sub     esp, 18h
   mov     dword [esp+8], 2
   mov     eax, [ebp+8]
   mov     eax, [eax]
   mov     [esp+4], eax
   lea     eax, [ebp-2]
   mov     [esp], eax
   call    memcpy
   mov     edx, [ebp+8]
   mov     eax, [ebp+8]
   mov     eax, [eax]
   add     eax, 2
   mov     [edx], eax
   movzx   eax, word [ebp-2]
   leave   
   retn    


XAnimLoadNotifyInfo:
   push    ebp
   mov     ebp, esp
   push    esi
   push    ebx
   sub     esp, 30h
   mov     eax, [ebp+0Ch]
   mov     eax, [eax]
   movzx   eax, byte [eax]
   mov     [ebp-0Ch], eax
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp+0Ch]
   mov     eax, [eax]
   inc     eax
   mov     [edx], eax
   mov     edx, [ebp+10h]
   movzx   eax, byte [ebp-0Ch]
   inc     al
   mov     [edx+0Ch], al
   mov     eax, [ebp+10h]
   movzx   eax, byte [eax+0Ch]
   shl     eax, 3
   mov     [esp], eax
   mov     eax, [ebp+14h]
   call    eax
   mov     [ebp-18h], eax
   mov     edx, [ebp+10h]
   mov     eax, [ebp-18h]
   mov     [edx+1Ch], eax
   mov     dword [ebp-10h], 0
loc_80C18D5:
   mov     eax, [ebp-10h]
   cmp     eax, [ebp-0Ch]
   jl      loc_80C18E2
   jmp     loc_80C1979
loc_80C18E2:
   mov     ebx, [ebp-18h]
   mov     dword [esp+8], 3
   mov     dword [esp+4], 0
   mov     eax, [ebp+0Ch]
   mov     eax, [eax]
   mov     [esp], eax
   call    SL_GetString_
   mov     [ebx], ax
   mov     esi, [ebp+0Ch]
   mov     ebx, [ebp+0Ch]
   mov     eax, [ebp+0Ch]
   mov     eax, [eax]
   mov     [esp], eax
   call    strlen
   add     eax, [ebx]
   inc     eax
   mov     [esi], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    sub_80C2CD4
   movzx   eax, ax
   mov     [ebp-14h], eax
   mov     eax, [ebp-18h]
   mov     [ebp-1Ch], eax
   mov     eax, [ebp+10h]
   cmp     word [eax], 0
   jz      loc_80C195B
   fild    dword [ebp-14h]
   fstp    dword [ebp-20h]
   mov     eax, [ebp+10h]
   movzx   eax, word [eax]
   push    eax
   fild    dword [esp]
   lea     esp, [esp+4]
   fld     dword [ebp-20h]
   fdivrp  st1, st0
   fstp    dword [ebp-20h]
   jmp     loc_80C1960
loc_80C195B:
   fldz    
   fstp    dword [ebp-20h]
loc_80C1960:
   fld     dword [ebp-20h]
   mov     eax, [ebp-1Ch]
   fstp    dword [eax+4]
   lea     eax, [ebp-10h]
   inc     dword [eax]
   lea     eax, [ebp-18h]
   add     dword [eax], 8
   jmp     loc_80C18D5
loc_80C1979:
   mov     ebx, [ebp-18h]
   mov     dword [esp+8], 3
   mov     dword [esp+4], 0
   mov     dword [esp], aEnd_1
   call    SL_GetString_
   mov     [ebx], ax
   mov     edx, [ebp-18h]
   mov     eax, 3F800000h
   mov     [edx+4], eax
   add     esp, 30h
   pop     ebx
   pop     esi
   pop     ebp
   retn    


XAnimLoadFile:
   push    ebp
   mov     ebp, esp
   push    esi
   push    ebx
   sub     esp, 0E0h
   mov     eax, [ebp+8]
   mov     [esp+0Ch], eax
   mov     dword [esp+8], aXanimS
   mov     dword [esp+4], 40h
   lea     eax, [ebp-78h]
   mov     [esp], eax
   call    Com_sprintf
   test    eax, eax
   jns     loc_80C1A01
   lea     eax, [ebp-78h]
   mov     [esp+4], eax
   mov     dword [esp], a1errorFilename
   call    Com_Printf
   mov     dword [ebp-0C8h], 0
   jmp     loc_80C2C3E
loc_80C1A01:
   lea     eax, [ebp-28h]
   mov     [esp+4], eax
   lea     eax, [ebp-78h]
   mov     [esp], eax
   call    FS_ReadFile
   mov     [ebp-0Ch], eax
   cmp     dword [ebp-0Ch], 0
   jns     loc_80C1A3E
   mov     eax, [ebp+8]
   mov     [esp+4], eax
   mov     dword [esp], a1errorXanimSNo
   call    Com_Printf
   mov     dword [ebp-0C8h], 0
   jmp     loc_80C2C3E
loc_80C1A3E:
   cmp     dword [ebp-0Ch], 0
   jnz     loc_80C1A71
   mov     eax, [ebp+8]
   mov     [esp+4], eax
   mov     dword [esp], a1errorXanimSHa
   call    Com_Printf
   mov     eax, [ebp-28h]
   mov     [esp], eax
   call    FS_FreeFile
   mov     dword [ebp-0C8h], 0
   jmp     loc_80C2C3E
loc_80C1A71:
   mov     eax, [ebp-28h]
   mov     [ebp-2Ch], eax
   lea     eax, [ebp-2Ch]
   mov     [esp], eax
   call    XModelDataReadShort
   mov     [ebp-80h], ax
   cmp     word [ebp-80h], 0Eh
   jz      loc_80C1ACA
   mov     eax, [ebp-28h]
   mov     [esp], eax
   call    FS_FreeFile
   mov     dword [esp+0Ch], 0Eh
   movsx   eax, word [ebp-80h]
   mov     [esp+8], eax
   mov     eax, [ebp+8]
   mov     [esp+4], eax
   mov     dword [esp], a1errorXanimSOu
   call    Com_Printf
   mov     dword [ebp-0C8h], 0
   jmp     loc_80C2C3E
loc_80C1ACA:
   lea     eax, [ebp-2Ch]
   mov     [esp], eax
   call    XModelDataReadShort
   mov     [ebp-18h], ax
   lea     eax, [ebp-2Ch]
   mov     [esp], eax
   call    XModelDataReadShort
   mov     [ebp-16h], ax
   cmp     word [ebp-16h], 0
   jz      loc_80C1B05
   movsx   eax, word [ebp-16h]
   add     eax, eax
   mov     [esp], eax
   mov     eax, [ebp+0Ch]
   call    eax
   mov     [ebp-0CCh], eax
   jmp     loc_80C1B0F
loc_80C1B05:
   mov     dword [ebp-0CCh], 0
loc_80C1B0F:
   mov     eax, [ebp-0CCh]
   mov     [ebp-10h], eax
   mov     eax, [ebp-2Ch]
   movzx   eax, byte [eax]
   mov     [ebp-81h], al
   lea     eax, [ebp-2Ch]
   inc     dword [eax]
   movzx   eax, byte [ebp-81h]
   and     eax, 1
   mov     [ebp-82h], al
   movzx   eax, byte [ebp-81h]
   shr     eax, 1
   and     al, 1
   mov     [ebp-83h], al
   lea     eax, [ebp-2Ch]
   mov     [esp], eax
   call    XModelDataReadShort
   mov     [ebp-7Eh], ax
   mov     dword [esp], 2Ch
   mov     eax, [ebp+0Ch]
   call    eax
   mov     [ebp-14h], eax
   mov     edx, [ebp-14h]
   movzx   eax, word [ebp-16h]
   mov     [edx+0Eh], ax
   mov     edx, [ebp-14h]
   mov     eax, [ebp-10h]
   mov     [edx+10h], eax
   mov     eax, [ebp-14h]
   fild    word [ebp-7Eh]
   fstp    dword [eax+4]
   mov     edx, [ebp-14h]
   movzx   eax, byte [ebp-82h]
   mov     [edx+2], al
   mov     edx, [ebp-14h]
   movzx   eax, byte [ebp-83h]
   mov     [edx+3], al
   cmp     byte [ebp-82h], 0
   jz      loc_80C1BB5
   mov     eax, [ebp-18h]
   inc     eax
   mov     [ebp-0CEh], ax
   jmp     loc_80C1BBF
loc_80C1BB5:
   mov     eax, [ebp-18h]
   mov     [ebp-0CEh], ax
loc_80C1BBF:
   movzx   eax, word [ebp-0CEh]
   mov     [ebp-1Ah], ax
   cmp     word [ebp-1Ah], 100h
   setbe   al
   mov     [ebp-89h], al
   mov     edx, [ebp-14h]
   movzx   eax, word [ebp-1Ah]
   dec     eax
   mov     [edx], ax
   mov     eax, [ebp-14h]
   mov     [ebp-0D4h], eax
   mov     eax, [ebp-14h]
   cmp     word [eax], 0
   jz      loc_80C1C14
   mov     edx, [ebp-14h]
   mov     eax, [ebp-14h]
   movzx   eax, word [eax]
   push    eax
   fild    dword [esp]
   lea     esp, [esp+4]
   fld     dword [edx+4]
   fdivrp  st1, st0
   fstp    dword [ebp-0D8h]
   jmp     loc_80C1C1C
loc_80C1C14:
   fldz    
   fstp    dword [ebp-0D8h]
loc_80C1C1C:
   fld     dword [ebp-0D8h]
   mov     eax, [ebp-0D4h]
   fstp    dword [eax+8]
   cmp     byte [ebp-83h], 0
   jz      loc_80C216E
   mov     dword [esp], 8
   mov     eax, [ebp+0Ch]
   call    eax
   mov     [ebp-24h], eax
   mov     edx, [ebp-14h]
   mov     eax, [ebp-24h]
   mov     [edx+20h], eax
   lea     eax, [ebp-2Ch]
   mov     [esp], eax
   call    sub_80C2CD4
   mov     [ebp-86h], ax
   cmp     word [ebp-86h], 0
   jnz     loc_80C1C7B
   mov     eax, [ebp-24h]
   mov     dword [eax+4], 0
   jmp     loc_80C1F2A
loc_80C1C7B:
   cmp     word [ebp-86h], 1
   jnz     loc_80C1CDD
   lea     eax, [ebp-98h]
   mov     [esp+4], eax
   lea     eax, [ebp-2Ch]
   mov     [esp], eax
   call    sub_80C17FA
   mov     ebx, [ebp-24h]
   mov     dword [esp], 8
   mov     eax, [ebp+0Ch]
   call    eax
   mov     [ebx+4], eax
   mov     eax, [ebp-24h]
   mov     eax, [eax+4]
   mov     word [eax], 0
   mov     eax, [ebp-24h]
   mov     edx, [eax+4]
   mov     eax, [ebp-98h]
   mov     [edx+4], ax
   mov     eax, [ebp-24h]
   mov     edx, [eax+4]
   movzx   eax, word [ebp-96h]
   mov     [edx+6], ax
   jmp     loc_80C1F2A
loc_80C1CDD:
   movzx   eax, word [ebp-86h]
   cmp     ax, [ebp-1Ah]
   jnb     loc_80C1DA3
   cmp     byte [ebp-89h], 0
   jz      loc_80C1D4B
   mov     ebx, [ebp-24h]
   movzx   eax, word [ebp-86h]
   add     eax, 8
   mov     [esp], eax
   mov     eax, [ebp+0Ch]
   call    eax
   mov     [ebx+4], eax
   movzx   eax, word [ebp-86h]
   mov     [ebp-0ACh], eax
   mov     eax, [ebp-0ACh]
   mov     [esp+8], eax
   mov     eax, [ebp-2Ch]
   mov     [esp+4], eax
   mov     eax, [ebp-24h]
   mov     eax, [eax+4]
   add     eax, 8
   mov     [esp], eax
   call    memcpy
   mov     edx, [ebp-0ACh]
   lea     eax, [ebp-2Ch]
   add     [eax], edx
   jmp     loc_80C1DB5
loc_80C1D4B:
   mov     ebx, [ebp-24h]
   movzx   eax, word [ebp-86h]
   add     eax, eax
   add     eax, 8
   mov     [esp], eax
   mov     eax, [ebp+0Ch]
   call    eax
   mov     [ebx+4], eax
   movzx   eax, word [ebp-86h]
   add     eax, eax
   mov     [ebp-0ACh], eax
   mov     eax, [ebp-0ACh]
   mov     [esp+8], eax
   mov     eax, [ebp-2Ch]
   mov     [esp+4], eax
   mov     eax, [ebp-24h]
   mov     eax, [eax+4]
   add     eax, 8
   mov     [esp], eax
   call    memcpy
   mov     edx, [ebp-0ACh]
   lea     eax, [ebp-2Ch]
   add     [eax], edx
   jmp     loc_80C1DB5
loc_80C1DA3:
   mov     ebx, [ebp-24h]
   mov     dword [esp], 8
   mov     eax, [ebp+0Ch]
   call    eax
   mov     [ebx+4], eax
loc_80C1DB5:
   mov     eax, [ebp-24h]
   mov     edx, [eax+4]
   movzx   eax, word [ebp-86h]
   dec     eax
   mov     [edx], ax
   mov     eax, [ebp-24h]
   mov     ebx, [eax+4]
   movzx   eax, word [ebp-86h]
   shl     eax, 2
   mov     [esp], eax
   mov     eax, [ebp+0Ch]
   call    eax
   mov     [ebx+4], eax
   mov     dword [ebp-0B0h], 0
loc_80C1DEB:
   movzx   eax, word [ebp-86h]
   cmp     [ebp-0B0h], eax
   jl      loc_80C1DFC
   jmp     loc_80C1E5A
loc_80C1DFC:
   lea     eax, [ebp-98h]
   mov     [esp+4], eax
   lea     eax, [ebp-2Ch]
   mov     [esp], eax
   call    sub_80C17FA
   mov     eax, [ebp-24h]
   mov     edx, [eax+4]
   mov     eax, [ebp-0B0h]
   lea     ecx, 0[eax*4]
   mov     edx, [edx+4]
   mov     eax, [ebp-98h]
   mov     [edx+ecx], ax
   mov     eax, [ebp-24h]
   mov     edx, [eax+4]
   mov     eax, [ebp-0B0h]
   shl     eax, 2
   add     eax, [edx+4]
   lea     edx, [eax+2]
   movzx   eax, word [ebp-96h]
   mov     [edx], ax
   lea     eax, [ebp-0B0h]
   inc     dword [eax]
   jmp     loc_80C1DEB
loc_80C1E5A:
   mov     dword [ebp-0B0h], 1
loc_80C1E64:
   movzx   eax, word [ebp-86h]
   cmp     [ebp-0B0h], eax
   jl      loc_80C1E78
   jmp     loc_80C1F2A
loc_80C1E78:
   mov     eax, [ebp-24h]
   mov     edx, [eax+4]
   mov     eax, [ebp-0B0h]
   shl     eax, 2
   add     eax, [edx+4]
   mov     [ebp-0B4h], eax
   mov     eax, [ebp-24h]
   mov     edx, [eax+4]
   mov     eax, [ebp-0B0h]
   shl     eax, 2
   add     eax, [edx+4]
   sub     eax, 4
   mov     [ebp-0B8h], eax
   mov     eax, [ebp-0B4h]
   movsx   edx, word [eax]
   mov     eax, [ebp-0B8h]
   movsx   eax, word [eax]
   mov     ecx, edx
   imul    ecx, eax
   mov     eax, [ebp-0B4h]
   add     eax, 2
   movsx   edx, word [eax]
   mov     eax, [ebp-0B8h]
   add     eax, 2
   movsx   eax, word [eax]
   imul    eax, edx
   lea     eax, [eax+ecx]
   mov     [ebp-0BCh], eax
   cmp     dword [ebp-0BCh], 0
   jns     loc_80C1F1D
   mov     edx, [ebp-0B4h]
   mov     eax, [ebp-0B4h]
   movzx   eax, word [eax]
   neg     eax
   mov     [edx], ax
   mov     edx, [ebp-0B4h]
   add     edx, 2
   mov     eax, [ebp-0B4h]
   add     eax, 2
   movzx   eax, word [eax]
   neg     eax
   mov     [edx], ax
loc_80C1F1D:
   lea     eax, [ebp-0B0h]
   inc     dword [eax]
   jmp     loc_80C1E64
loc_80C1F2A:
   lea     eax, [ebp-2Ch]
   mov     [esp], eax
   call    sub_80C2CD4
   mov     [ebp-88h], ax
   cmp     word [ebp-88h], 0
   jnz     loc_80C1F54
   mov     eax, [ebp-24h]
   mov     dword [eax], 0
   jmp     loc_80C216E
loc_80C1F54:
   cmp     word [ebp-88h], 1
   jnz     loc_80C1FCB
   lea     eax, [ebp-2Ch]
   mov     [esp], eax
   call    XModelDataReadFloat
   fstp    dword [ebp-0A8h]
   lea     eax, [ebp-2Ch]
   mov     [esp], eax
   call    XModelDataReadFloat
   fstp    dword [ebp-0A4h]
   lea     eax, [ebp-2Ch]
   mov     [esp], eax
   call    XModelDataReadFloat
   fstp    dword [ebp-0A0h]
   mov     ebx, [ebp-24h]
   mov     dword [esp], 10h
   mov     eax, [ebp+0Ch]
   call    eax
   mov     [ebx], eax
   mov     eax, [ebp-24h]
   mov     eax, [eax]
   mov     word [eax], 0
   mov     eax, [ebp-24h]
   mov     eax, [eax]
   add     eax, 4
   mov     [esp+4], eax
   lea     eax, [ebp-0A8h]
   mov     [esp], eax
   call    sub_80C2C6E
   jmp     loc_80C216E
loc_80C1FCB:
   mov     eax, [ebp-88h]
   cmp     ax, [ebp-1Ah]
   jnb     loc_80C208C
   cmp     byte [ebp-89h], 0
   jz      loc_80C2036
   mov     ebx, [ebp-24h]
   movzx   eax, word [ebp-88h]
   add     eax, 8
   mov     [esp], eax
   mov     eax, [ebp+0Ch]
   call    eax
   mov     [ebx], eax
   movzx   eax, word [ebp-88h]
   mov     [ebp-0ACh], eax
   mov     eax, [ebp-0ACh]
   mov     [esp+8], eax
   mov     eax, [ebp-2Ch]
   mov     [esp+4], eax
   mov     eax, [ebp-24h]
   mov     eax, [eax]
   add     eax, 8
   mov     [esp], eax
   call    memcpy
   mov     edx, [ebp-0ACh]
   lea     eax, [ebp-2Ch]
   add     [eax], edx
   jmp     loc_80C209D
loc_80C2036:
   mov     ebx, [ebp-24h]
   movzx   eax, word [ebp-88h]
   add     eax, eax
   add     eax, 8
   mov     [esp], eax
   mov     eax, [ebp+0Ch]
   call    eax
   mov     [ebx], eax
   movzx   eax, word [ebp-88h]
   add     eax, eax
   mov     [ebp-0ACh], eax
   mov     eax, [ebp-0ACh]
   mov     [esp+8], eax
   mov     eax, [ebp-2Ch]
   mov     [esp+4], eax
   mov     eax, [ebp-24h]
   mov     eax, [eax]
   add     eax, 8
   mov     [esp], eax
   call    memcpy
   mov     edx, [ebp-0ACh]
   lea     eax, [ebp-2Ch]
   add     [eax], edx
   jmp     loc_80C209D
loc_80C208C:
   mov     ebx, [ebp-24h]
   mov     dword [esp], 8
   mov     eax, [ebp+0Ch]
   call    eax
   mov     [ebx], eax
loc_80C209D:
   mov     eax, [ebp-24h]
   mov     edx, [eax]
   mov     eax, [ebp-88h]
   dec     eax
   mov     [edx], ax
   mov     eax, [ebp-24h]
   mov     ebx, [eax]
   movzx   edx, word [ebp-88h]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   mov     [esp], eax
   mov     eax, [ebp+0Ch]
   call    eax
   mov     [ebx+4], eax
   mov     dword [ebp-0B0h], 0
loc_80C20D6:
   movzx   eax, word [ebp-88h]
   cmp     [ebp-0B0h], eax
   jl      loc_80C20EA
   jmp     loc_80C216E
loc_80C20EA:
   lea     eax, [ebp-2Ch]
   mov     [esp], eax
   call    XModelDataReadFloat
   mov     eax, [ebp-24h]
   mov     ecx, [eax]
   mov     edx, [ebp-0B0h]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   lea     edx, 0[eax*4]
   mov     eax, [ecx+4]
   fstp    dword [eax+edx]
   lea     eax, [ebp-2Ch]
   mov     [esp], eax
   call    XModelDataReadFloat
   mov     eax, [ebp-24h]
   mov     ecx, [eax]
   mov     edx, [ebp-0B0h]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   add     eax, [ecx+4]
   add     eax, 4
   fstp    dword [eax]
   lea     eax, [ebp-2Ch]
   mov     [esp], eax
   call    XModelDataReadFloat
   mov     eax, [ebp-24h]
   mov     ecx, [eax]
   mov     edx, [ebp-0B0h]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   add     eax, [ecx+4]
   add     eax, 8
   fstp    dword [eax]
   lea     eax, [ebp-0B0h]
   inc     dword [eax]
   jmp     loc_80C20D6
loc_80C216E:
   cmp     word [ebp-16h], 0
   jz      loc_80C21E7
   movsx   eax, word [ebp-16h]
   dec     eax
   sar     eax, 3
   inc     eax
   mov     [ebp-7Ch], eax
   mov     eax, [ebp-2Ch]
   mov     [ebp-30h], eax
   mov     edx, [ebp-7Ch]
   lea     eax, [ebp-2Ch]
   add     [eax], edx
   mov     eax, [ebp-7Ch]
   mov     [esp], eax
   mov     eax, [ebp+0Ch]
   call    eax
   mov     [ebp-0C0h], eax
   mov     eax, [ebp-7Ch]
   mov     [esp+8], eax
   mov     eax, [ebp-2Ch]
   mov     [esp+4], eax
   mov     eax, [ebp-0C0h]
   mov     [esp], eax
   call    memcpy
   mov     edx, [ebp-7Ch]
   lea     eax, [ebp-2Ch]
   add     [eax], edx
   mov     edx, [ebp-14h]
   mov     eax, [ebp-0C0h]
   mov     [edx+14h], eax
   mov     ebx, [ebp-14h]
   movsx   eax, word [ebp-16h]
   shl     eax, 3
   mov     [esp], eax
   mov     eax, [ebp+0Ch]
   call    eax
   mov     [ebx+18h], eax
   jmp     loc_80C21F8
loc_80C21E7:
   mov     dword [ebp-30h], 0
   mov     dword [ebp-0C0h], 0
loc_80C21F8:
   mov     dword [ebp-34h], 0
loc_80C21FF:
   movsx   eax, word [ebp-16h]
   cmp     [ebp-34h], eax
   jl      loc_80C220A
   jmp     loc_80C2257
loc_80C220A:
   mov     eax, [ebp-2Ch]
   mov     [esp], eax
   call    strlen
   inc     eax
   mov     [ebp-7Ch], eax
   mov     eax, [ebp-34h]
   lea     ebx, [eax+eax]
   mov     esi, [ebp-10h]
   mov     dword [esp+0Ch], 9
   mov     eax, [ebp-7Ch]
   mov     [esp+8], eax
   mov     dword [esp+4], 0
   mov     eax, [ebp-2Ch]
   mov     [esp], eax
   call    SL_GetStringOfLen
   mov     [esi+ebx], ax
   mov     edx, [ebp-7Ch]
   lea     eax, [ebp-2Ch]
   add     [eax], edx
   lea     eax, [ebp-34h]
   inc     dword [eax]
   jmp     loc_80C21FF
loc_80C2257:
   mov     dword [ebp-34h], 0
loc_80C225E:
   movsx   eax, word [ebp-16h]
   cmp     [ebp-34h], eax
   jl      loc_80C226C
   jmp     loc_80C2C0A
loc_80C226C:
   mov     eax, [ebp-34h]
   mov     edx, eax
   sar     edx, 3
   mov     eax, [ebp-30h]
   movzx   eax, byte [edx+eax]
   mov     ecx, [ebp-34h]
   and     ecx, 7
   sar     eax, cl
   and     eax, 1
   mov     [ebp-0C1h], al
   mov     eax, [ebp-34h]
   mov     edx, eax
   sar     edx, 3
   mov     eax, [ebp-0C0h]
   movsx   eax, byte [edx+eax]
   mov     ecx, [ebp-34h]
   and     ecx, 7
   sar     eax, cl
   and     eax, 1
   mov     [ebp-0C2h], al
   mov     edx, [ebp-14h]
   mov     eax, [ebp-34h]
   shl     eax, 3
   add     eax, [edx+18h]
   mov     [ebp-20h], eax
   lea     eax, [ebp-2Ch]
   mov     [esp], eax
   call    sub_80C2CD4
   mov     [ebp-86h], ax
   cmp     word [ebp-86h], 0
   jnz     loc_80C22E9
   mov     eax, [ebp-20h]
   mov     dword [eax+4], 0
   jmp     loc_80C29BC
loc_80C22E9:
   cmp     word [ebp-86h], 1
   jnz     loc_80C2435
   cmp     byte [ebp-0C2h], 0
   jz      loc_80C2375
   lea     eax, [ebp-98h]
   mov     [esp+4], eax
   lea     eax, [ebp-2Ch]
   mov     [esp], eax
   call    sub_80C17FA
   cmp     byte [ebp-0C1h], 0
   jz      loc_80C233D
   mov     eax, [ebp-98h]
   neg     eax
   mov     [ebp-98h], ax
   movzx   eax, word [ebp-96h]
   neg     eax
   mov     [ebp-96h], ax
loc_80C233D:
   mov     ebx, [ebp-20h]
   mov     dword [esp], 8
   mov     eax, [ebp+0Ch]
   call    eax
   mov     [ebx+4], eax
   mov     eax, [ebp-20h]
   mov     edx, [eax+4]
   mov     eax, [ebp-98h]
   mov     [edx+4], ax
   mov     eax, [ebp-20h]
   mov     edx, [eax+4]
   movzx   eax, word [ebp-96h]
   mov     [edx+6], ax
   jmp     loc_80C2425
loc_80C2375:
   lea     eax, [ebp-98h]
   mov     [esp+4], eax
   lea     eax, [ebp-2Ch]
   mov     [esp], eax
   call    sub_80C1718
   cmp     byte [ebp-0C1h], 0
   jz      loc_80C23D1
   mov     eax, [ebp-98h]
   neg     eax
   mov     [ebp-98h], ax
   movzx   eax, word [ebp-96h]
   neg     eax
   mov     [ebp-96h], ax
   mov     eax, [ebp-94h]
   neg     eax
   mov     [ebp-94h], ax
   movzx   eax, word [ebp-92h]
   neg     eax
   mov     [ebp-92h], ax
loc_80C23D1:
   mov     ebx, [ebp-20h]
   mov     dword [esp], 0Ch
   mov     eax, [ebp+0Ch]
   call    eax
   mov     [ebx+4], eax
   mov     eax, [ebp-20h]
   mov     edx, [eax+4]
   mov     eax, [ebp-98h]
   mov     [edx+4], ax
   mov     eax, [ebp-20h]
   mov     edx, [eax+4]
   movzx   eax, word [ebp-96h]
   mov     [edx+6], ax
   mov     eax, [ebp-20h]
   mov     edx, [eax+4]
   mov     eax, [ebp-94h]
   mov     [edx+8], ax
   mov     eax, [ebp-20h]
   mov     edx, [eax+4]
   movzx   eax, word [ebp-92h]
   mov     [edx+0Ah], ax
loc_80C2425:
   mov     eax, [ebp-20h]
   mov     eax, [eax+4]
   mov     word [eax], 0
   jmp     loc_80C29BC
loc_80C2435:
   movzx   eax, word [ebp-86h]
   cmp     ax, [ebp-1Ah]
   jnb     loc_80C24FB
   cmp     byte [ebp-89h], 0
   jz      loc_80C24A3
   mov     ebx, [ebp-20h]
   movzx   eax, word [ebp-86h]
   add     eax, 8
   mov     [esp], eax
   mov     eax, [ebp+0Ch]
   call    eax
   mov     [ebx+4], eax
   movzx   eax, word [ebp-86h]
   mov     [ebp-0ACh], eax
   mov     eax, [ebp-0ACh]
   mov     [esp+8], eax
   mov     eax, [ebp-2Ch]
   mov     [esp+4], eax
   mov     eax, [ebp-20h]
   mov     eax, [eax+4]
   add     eax, 8
   mov     [esp], eax
   call    memcpy
   mov     edx, [ebp-0ACh]
   lea     eax, [ebp-2Ch]
   add     [eax], edx
   jmp     loc_80C250D
loc_80C24A3:
   mov     ebx, [ebp-20h]
   movzx   eax, word [ebp-86h]
   add     eax, eax
   add     eax, 8
   mov     [esp], eax
   mov     eax, [ebp+0Ch]
   call    eax
   mov     [ebx+4], eax
   movzx   eax, word [ebp-86h]
   add     eax, eax
   mov     [ebp-0ACh], eax
   mov     eax, [ebp-0ACh]
   mov     [esp+8], eax
   mov     eax, [ebp-2Ch]
   mov     [esp+4], eax
   mov     eax, [ebp-20h]
   mov     eax, [eax+4]
   add     eax, 8
   mov     [esp], eax
   call    memcpy
   mov     edx, [ebp-0ACh]
   lea     eax, [ebp-2Ch]
   add     [eax], edx
   jmp     loc_80C250D
loc_80C24FB:
   mov     ebx, [ebp-20h]
   mov     dword [esp], 8
   mov     eax, [ebp+0Ch]
   call    eax
   mov     [ebx+4], eax
loc_80C250D:
   cmp     byte [ebp-0C2h], 0
   jz      loc_80C26E3
   mov     eax, [ebp-20h]
   mov     ebx, [eax+4]
   movzx   eax, word [ebp-86h]
   shl     eax, 2
   mov     [esp], eax
   mov     eax, [ebp+0Ch]
   call    eax
   mov     [ebx+4], eax
   lea     eax, [ebp-98h]
   mov     [esp+4], eax
   lea     eax, [ebp-2Ch]
   mov     [esp], eax
   call    sub_80C17FA
   cmp     byte [ebp-0C1h], 0
   jz      loc_80C2572
   mov     eax, [ebp-98h]
   neg     eax
   mov     [ebp-98h], ax
   movzx   eax, word [ebp-96h]
   neg     eax
   mov     [ebp-96h], ax
loc_80C2572:
   mov     eax, [ebp-20h]
   mov     eax, [eax+4]
   mov     edx, [eax+4]
   mov     eax, [ebp-98h]
   mov     [edx], ax
   mov     eax, [ebp-20h]
   mov     eax, [eax+4]
   mov     edx, [eax+4]
   add     edx, 2
   movzx   eax, word [ebp-96h]
   mov     [edx], ax
   mov     dword [ebp-0B0h], 1
loc_80C25A4:
   movzx   eax, word [ebp-86h]
   cmp     [ebp-0B0h], eax
   jl      loc_80C25B5
   jmp     loc_80C2613
loc_80C25B5:
   lea     eax, [ebp-98h]
   mov     [esp+4], eax
   lea     eax, [ebp-2Ch]
   mov     [esp], eax
   call    sub_80C17FA
   mov     eax, [ebp-20h]
   mov     edx, [eax+4]
   mov     eax, [ebp-0B0h]
   lea     ecx, 0[eax*4]
   mov     edx, [edx+4]
   mov     eax, [ebp-98h]
   mov     [edx+ecx], ax
   mov     eax, [ebp-20h]
   mov     edx, [eax+4]
   mov     eax, [ebp-0B0h]
   shl     eax, 2
   add     eax, [edx+4]
   lea     edx, [eax+2]
   movzx   eax, word [ebp-96h]
   mov     [edx], ax
   lea     eax, [ebp-0B0h]
   inc     dword [eax]
   jmp     loc_80C25A4
loc_80C2613:
   mov     dword [ebp-0B0h], 1
loc_80C261D:
   movzx   eax, word [ebp-86h]
   cmp     [ebp-0B0h], eax
   jl      loc_80C2631
   jmp     loc_80C29AB
loc_80C2631:
   mov     eax, [ebp-20h]
   mov     edx, [eax+4]
   mov     eax, [ebp-0B0h]
   shl     eax, 2
   add     eax, [edx+4]
   mov     [ebp-0B4h], eax
   mov     eax, [ebp-20h]
   mov     edx, [eax+4]
   mov     eax, [ebp-0B0h]
   shl     eax, 2
   add     eax, [edx+4]
   sub     eax, 4
   mov     [ebp-0B8h], eax
   mov     eax, [ebp-0B4h]
   movsx   edx, word [eax]
   mov     eax, [ebp-0B8h]
   movsx   eax, word [eax]
   mov     ecx, edx
   imul    ecx, eax
   mov     eax, [ebp-0B4h]
   add     eax, 2
   movsx   edx, word [eax]
   mov     eax, [ebp-0B8h]
   add     eax, 2
   movsx   eax, word [eax]
   imul    eax, edx
   lea     eax, [eax+ecx]
   mov     [ebp-0BCh], eax
   cmp     dword [ebp-0BCh], 0
   jns     loc_80C26D6
   mov     edx, [ebp-0B4h]
   mov     eax, [ebp-0B4h]
   movzx   eax, word [eax]
   neg     eax
   mov     [edx], ax
   mov     edx, [ebp-0B4h]
   add     edx, 2
   mov     eax, [ebp-0B4h]
   add     eax, 2
   movzx   eax, word [eax]
   neg     eax
   mov     [edx], ax
loc_80C26D6:
   lea     eax, [ebp-0B0h]
   inc     dword [eax]
   jmp     loc_80C261D
loc_80C26E3:
   mov     eax, [ebp-20h]
   mov     ebx, [eax+4]
   movzx   eax, word [ebp-86h]
   shl     eax, 3
   mov     [esp], eax
   mov     eax, [ebp+0Ch]
   call    eax
   mov     [ebx+4], eax
   lea     eax, [ebp-98h]
   mov     [esp+4], eax
   lea     eax, [ebp-2Ch]
   mov     [esp], eax
   call    sub_80C1718
   cmp     byte [ebp-0C1h], 0
   jz      loc_80C275A
   mov     eax, [ebp-98h]
   neg     eax
   mov     [ebp-98h], ax
   movzx   eax, word [ebp-96h]
   neg     eax
   mov     [ebp-96h], ax
   mov     eax, [ebp-94h]
   neg     eax
   mov     [ebp-94h], ax
   movzx   eax, word [ebp-92h]
   neg     eax
   mov     [ebp-92h], ax
loc_80C275A:
   mov     eax, [ebp-20h]
   mov     eax, [eax+4]
   mov     edx, [eax+4]
   mov     eax, [ebp-98h]
   mov     [edx], ax
   mov     eax, [ebp-20h]
   mov     eax, [eax+4]
   mov     edx, [eax+4]
   add     edx, 2
   movzx   eax, word [ebp-96h]
   mov     [edx], ax
   mov     eax, [ebp-20h]
   mov     eax, [eax+4]
   mov     edx, [eax+4]
   add     edx, 4
   mov     eax, [ebp-94h]
   mov     [edx], ax
   mov     eax, [ebp-20h]
   mov     eax, [eax+4]
   mov     edx, [eax+4]
   add     edx, 6
   movzx   eax, word [ebp-92h]
   mov     [edx], ax
   mov     dword [ebp-0B0h], 1
loc_80C27B7:
   movzx   eax, word [ebp-86h]
   cmp     [ebp-0B0h], eax
   jl      loc_80C27CB
   jmp     loc_80C2869
loc_80C27CB:
   lea     eax, [ebp-98h]
   mov     [esp+4], eax
   lea     eax, [ebp-2Ch]
   mov     [esp], eax
   call    sub_80C1718
   mov     eax, [ebp-20h]
   mov     edx, [eax+4]
   mov     eax, [ebp-0B0h]
   lea     ecx, 0[eax*8]
   mov     edx, [edx+4]
   mov     eax, [ebp-98h]
   mov     [edx+ecx], ax
   mov     eax, [ebp-20h]
   mov     edx, [eax+4]
   mov     eax, [ebp-0B0h]
   shl     eax, 3
   add     eax, [edx+4]
   lea     edx, [eax+2]
   movzx   eax, word [ebp-96h]
   mov     [edx], ax
   mov     eax, [ebp-20h]
   mov     edx, [eax+4]
   mov     eax, [ebp-0B0h]
   shl     eax, 3
   add     eax, [edx+4]
   lea     edx, [eax+4]
   mov     eax, [ebp-94h]
   mov     [edx], ax
   mov     eax, [ebp-20h]
   mov     edx, [eax+4]
   mov     eax, [ebp-0B0h]
   shl     eax, 3
   add     eax, [edx+4]
   lea     edx, [eax+6]
   movzx   eax, word [ebp-92h]
   mov     [edx], ax
   lea     eax, [ebp-0B0h]
   inc     dword [eax]
   jmp     loc_80C27B7
loc_80C2869:
   mov     dword [ebp-0B0h], 1
loc_80C2873:
   movzx   eax, word [ebp-86h]
   cmp     [ebp-0B0h], eax
   jl      loc_80C2887
   jmp     loc_80C29AB
loc_80C2887:
   mov     eax, [ebp-20h]
   mov     edx, [eax+4]
   mov     eax, [ebp-0B0h]
   shl     eax, 3
   add     eax, [edx+4]
   mov     [ebp-0B4h], eax
   mov     eax, [ebp-20h]
   mov     edx, [eax+4]
   mov     eax, [ebp-0B0h]
   shl     eax, 3
   add     eax, [edx+4]
   sub     eax, 8
   mov     [ebp-0B8h], eax
   mov     eax, [ebp-0B4h]
   add     eax, 4
   movsx   edx, word [eax]
   mov     eax, [ebp-0B8h]
   add     eax, 4
   movsx   eax, word [eax]
   mov     ecx, edx
   imul    ecx, eax
   mov     eax, [ebp-0B4h]
   add     eax, 6
   movsx   edx, word [eax]
   mov     eax, [ebp-0B8h]
   add     eax, 6
   movsx   eax, word [eax]
   imul    eax, edx
   lea     ebx, [eax+ecx]
   mov     eax, [ebp-0B4h]
   movsx   edx, word [eax]
   mov     eax, [ebp-0B8h]
   movsx   eax, word [eax]
   mov     ecx, edx
   imul    ecx, eax
   mov     eax, [ebp-0B4h]
   add     eax, 2
   movsx   edx, word [eax]
   mov     eax, [ebp-0B8h]
   add     eax, 2
   movsx   eax, word [eax]
   imul    eax, edx
   lea     eax, [eax+ecx]
   lea     eax, [eax+ebx]
   mov     [ebp-0BCh], eax
   cmp     dword [ebp-0BCh], 0
   jns     loc_80C299E
   mov     edx, [ebp-0B4h]
   mov     eax, [ebp-0B4h]
   movzx   eax, word [eax]
   neg     eax
   mov     [edx], ax
   mov     edx, [ebp-0B4h]
   add     edx, 2
   mov     eax, [ebp-0B4h]
   add     eax, 2
   movzx   eax, word [eax]
   neg     eax
   mov     [edx], ax
   mov     edx, [ebp-0B4h]
   add     edx, 4
   mov     eax, [ebp-0B4h]
   add     eax, 4
   movzx   eax, word [eax]
   neg     eax
   mov     [edx], ax
   mov     edx, [ebp-0B4h]
   add     edx, 6
   mov     eax, [ebp-0B4h]
   add     eax, 6
   movzx   eax, word [eax]
   neg     eax
   mov     [edx], ax
loc_80C299E:
   lea     eax, [ebp-0B0h]
   inc     dword [eax]
   jmp     loc_80C2873
loc_80C29AB:
   mov     eax, [ebp-20h]
   mov     edx, [eax+4]
   movzx   eax, word [ebp-86h]
   dec     eax
   mov     [edx], ax
loc_80C29BC:
   lea     eax, [ebp-2Ch]
   mov     [esp], eax
   call    sub_80C2CD4
   mov     [ebp-88h], ax
   cmp     word [ebp-88h], 0
   jnz     loc_80C29E6
   mov     eax, [ebp-20h]
   mov     dword [eax], 0
   jmp     loc_80C2C00
loc_80C29E6:
   cmp     word [ebp-88h], 1
   jnz     loc_80C2A5D
   lea     eax, [ebp-2Ch]
   mov     [esp], eax
   call    XModelDataReadFloat
   fstp    dword [ebp-0A8h]
   lea     eax, [ebp-2Ch]
   mov     [esp], eax
   call    XModelDataReadFloat
   fstp    dword [ebp-0A4h]
   lea     eax, [ebp-2Ch]
   mov     [esp], eax
   call    XModelDataReadFloat
   fstp    dword [ebp-0A0h]
   mov     ebx, [ebp-20h]
   mov     dword [esp], 10h
   mov     eax, [ebp+0Ch]
   call    eax
   mov     [ebx], eax
   mov     eax, [ebp-20h]
   mov     eax, [eax]
   mov     word [eax], 0
   mov     eax, [ebp-20h]
   mov     eax, [eax]
   add     eax, 4
   mov     [esp+4], eax
   lea     eax, [ebp-0A8h]
   mov     [esp], eax
   call    sub_80C2C6E
   jmp     loc_80C2C00
loc_80C2A5D:
   mov     eax, [ebp-88h]
   cmp     ax, [ebp-1Ah]
   jnb     loc_80C2B1E
   cmp     byte [ebp-89h], 0
   jz      loc_80C2AC8
   mov     ebx, [ebp-20h]
   movzx   eax, word [ebp-88h]
   add     eax, 8
   mov     [esp], eax
   mov     eax, [ebp+0Ch]
   call    eax
   mov     [ebx], eax
   movzx   eax, word [ebp-88h]
   mov     [ebp-0ACh], eax
   mov     eax, [ebp-0ACh]
   mov     [esp+8], eax
   mov     eax, [ebp-2Ch]
   mov     [esp+4], eax
   mov     eax, [ebp-20h]
   mov     eax, [eax]
   add     eax, 8
   mov     [esp], eax
   call    memcpy
   mov     edx, [ebp-0ACh]
   lea     eax, [ebp-2Ch]
   add     [eax], edx
   jmp     loc_80C2B2F
loc_80C2AC8:
   mov     ebx, [ebp-20h]
   movzx   eax, word [ebp-88h]
   add     eax, eax
   add     eax, 8
   mov     [esp], eax
   mov     eax, [ebp+0Ch]
   call    eax
   mov     [ebx], eax
   movzx   eax, word [ebp-88h]
   add     eax, eax
   mov     [ebp-0ACh], eax
   mov     eax, [ebp-0ACh]
   mov     [esp+8], eax
   mov     eax, [ebp-2Ch]
   mov     [esp+4], eax
   mov     eax, [ebp-20h]
   mov     eax, [eax]
   add     eax, 8
   mov     [esp], eax
   call    memcpy
   mov     edx, [ebp-0ACh]
   lea     eax, [ebp-2Ch]
   add     [eax], edx
   jmp     loc_80C2B2F
loc_80C2B1E:
   mov     ebx, [ebp-20h]
   mov     dword [esp], 8
   mov     eax, [ebp+0Ch]
   call    eax
   mov     [ebx], eax
loc_80C2B2F:
   mov     eax, [ebp-20h]
   mov     edx, [eax]
   mov     eax, [ebp-88h]
   dec     eax
   mov     [edx], ax
   mov     eax, [ebp-20h]
   mov     ebx, [eax]
   movzx   edx, word [ebp-88h]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   mov     [esp], eax
   mov     eax, [ebp+0Ch]
   call    eax
   mov     [ebx+4], eax
   mov     dword [ebp-0B0h], 0
loc_80C2B68:
   movzx   eax, word [ebp-88h]
   cmp     [ebp-0B0h], eax
   jl      loc_80C2B7C
   jmp     loc_80C2C00
loc_80C2B7C:
   lea     eax, [ebp-2Ch]
   mov     [esp], eax
   call    XModelDataReadFloat
   mov     eax, [ebp-20h]
   mov     ecx, [eax]
   mov     edx, [ebp-0B0h]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   lea     edx, 0[eax*4]
   mov     eax, [ecx+4]
   fstp    dword [eax+edx]
   lea     eax, [ebp-2Ch]
   mov     [esp], eax
   call    XModelDataReadFloat
   mov     eax, [ebp-20h]
   mov     ecx, [eax]
   mov     edx, [ebp-0B0h]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   add     eax, [ecx+4]
   add     eax, 4
   fstp    dword [eax]
   lea     eax, [ebp-2Ch]
   mov     [esp], eax
   call    XModelDataReadFloat
   mov     eax, [ebp-20h]
   mov     ecx, [eax]
   mov     edx, [ebp-0B0h]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   add     eax, [ecx+4]
   add     eax, 8
   fstp    dword [eax]
   lea     eax, [ebp-0B0h]
   inc     dword [eax]
   jmp     loc_80C2B68
loc_80C2C00:
   lea     eax, [ebp-34h]
   inc     dword [eax]
   jmp     loc_80C225E
loc_80C2C0A:
   mov     eax, [ebp+0Ch]
   mov     [esp+0Ch], eax
   mov     eax, [ebp-14h]
   mov     [esp+8], eax
   lea     eax, [ebp-2Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XAnimLoadNotifyInfo
   mov     eax, [ebp-28h]
   mov     [esp], eax
   call    FS_FreeFile
   mov     eax, [ebp-14h]
   mov     [ebp-0C8h], eax
loc_80C2C3E:
   mov     eax, [ebp-0C8h]
   add     esp, 0E0h
   pop     ebx
   pop     esi
   pop     ebp
   retn    


section .rdata
flt_81492E4     dd 0.5

aEnd_1          db 'end',0
aXanimS         db 'xanim/%s',0
a1errorXanimSOu db '^1ERROR: xanim ',27h,'%s',27h,' out of date (version %d, expecting %d)',0Ah,0
a1errorXanimSHa db '^1ERROR: xanim ',27h,'%s',27h,' has 0 length',0Ah,0
a1errorXanimSNo db '^1ERROR: xanim ',27h,'%s',27h,' not found',0Ah,0
a1errorFilename db '^1ERROR: filename ',27h,'%s',27h,' too long',0Ah,0