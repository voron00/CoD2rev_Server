extern FS_FreeFile
extern SL_GetStringOfLen
extern Com_Printf
extern Com_sprintf
extern memcpy
extern strlen
extern floor
extern sqrt
extern memset
extern FS_ReadFile
extern XModelDataReadShort
extern XModelDataReadFloat


global XModelPartsLoadFile
global sub_80C3B60
global sub_80C51F0
global sub_80C3C42
global sub_80C5210
global sub_80A5372
global sub_80B9D76
global sub_80B9D30
global sub_80A9094
global sub_80A913C
global sub_80A8E2C


section .text


sub_80A8E2C:
   push    ebp
   mov     ebp, esp
   mov     edx, [ebp+10h]
   mov     eax, [ebp+8]
   fld     dword [ebp+0Ch]
   fmul    dword [eax]
   fstp    dword [edx]
   mov     edx, [ebp+10h]
   add     edx, 4
   mov     eax, [ebp+8]
   add     eax, 4
   fld     dword [ebp+0Ch]
   fmul    dword [eax]
   fstp    dword [edx]
   mov     edx, [ebp+10h]
   add     edx, 8
   mov     eax, [ebp+8]
   add     eax, 8
   fld     dword [ebp+0Ch]
   fmul    dword [eax]
   fstp    dword [edx]
   pop     ebp
   retn    


sub_80A913C:
   push    ebp
   mov     ebp, esp
   sub     esp, 58h
   lea     eax, [ebp-48h]
   mov     [esp+8], eax
   mov     eax, [ebp+8]
   mov     eax, [eax+1Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    sub_80A8E2C
   mov     eax, [ebp+8]
   fld     dword [ebp-48h]
   fmul    dword [eax]
   fstp    dword [ebp-0Ch]
   mov     eax, [ebp+8]
   fld     dword [ebp-48h]
   fmul    dword [eax+4]
   fstp    dword [ebp-10h]
   mov     eax, [ebp+8]
   fld     dword [ebp-48h]
   fmul    dword [eax+8]
   fstp    dword [ebp-14h]
   mov     eax, [ebp+8]
   fld     dword [ebp-48h]
   fmul    dword [eax+0Ch]
   fstp    dword [ebp-18h]
   mov     eax, [ebp+8]
   fld     dword [ebp-44h]
   fmul    dword [eax+4]
   fstp    dword [ebp-1Ch]
   mov     eax, [ebp+8]
   fld     dword [ebp-44h]
   fmul    dword [eax+8]
   fstp    dword [ebp-20h]
   mov     eax, [ebp+8]
   fld     dword [ebp-44h]
   fmul    dword [eax+0Ch]
   fstp    dword [ebp-24h]
   mov     eax, [ebp+8]
   fld     dword [ebp-40h]
   fmul    dword [eax+8]
   fstp    dword [ebp-28h]
   mov     eax, [ebp+8]
   fld     dword [ebp-40h]
   fmul    dword [eax+0Ch]
   fstp    dword [ebp-2Ch]
   mov     eax, [ebp+0Ch]
   fld     dword [ebp-1Ch]
   fadd    dword [ebp-28h]
   fld1    
   fsubrp  st1, st0
   fstp    dword [eax]
   mov     eax, [ebp+0Ch]
   add     eax, 4
   fld     dword [ebp-10h]
   fadd    dword [ebp-2Ch]
   fstp    dword [eax]
   mov     eax, [ebp+0Ch]
   add     eax, 8
   fld     dword [ebp-14h]
   fsub    dword [ebp-24h]
   fstp    dword [eax]
   mov     eax, [ebp+0Ch]
   add     eax, 0Ch
   fld     dword [ebp-10h]
   fsub    dword [ebp-2Ch]
   fstp    dword [eax]
   mov     eax, [ebp+0Ch]
   add     eax, 10h
   fld     dword [ebp-0Ch]
   fadd    dword [ebp-28h]
   fld1    
   fsubrp  st1, st0
   fstp    dword [eax]
   mov     eax, [ebp+0Ch]
   add     eax, 14h
   fld     dword [ebp-20h]
   fadd    dword [ebp-18h]
   fstp    dword [eax]
   mov     eax, [ebp+0Ch]
   add     eax, 18h
   fld     dword [ebp-14h]
   fadd    dword [ebp-24h]
   fstp    dword [eax]
   mov     eax, [ebp+0Ch]
   add     eax, 1Ch
   fld     dword [ebp-20h]
   fsub    dword [ebp-18h]
   fstp    dword [eax]
   mov     eax, [ebp+0Ch]
   add     eax, 20h
   fld     dword [ebp-0Ch]
   fadd    dword [ebp-1Ch]
   fld1    
   fsubrp  st1, st0
   fstp    dword [eax]
   leave   
   retn    


sub_80A9094:
   push    ebp
   mov     ebp, esp
   sub     esp, 48h
   lea     eax, [ebp-38h]
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    sub_80A913C
   mov     edx, [ebp+10h]
   mov     eax, [ebp+8]
   fld     dword [eax]
   fmul    dword [ebp-38h]
   mov     eax, [ebp+8]
   add     eax, 4
   fld     dword [eax]
   fmul    dword [ebp-2Ch]
   faddp   st1, st0
   mov     eax, [ebp+8]
   add     eax, 8
   fld     dword [eax]
   fmul    dword [ebp-20h]
   faddp   st1, st0
   mov     eax, [ebp+0Ch]
   fadd    dword [eax+10h]
   fstp    dword [edx]
   mov     edx, [ebp+10h]
   add     edx, 4
   mov     eax, [ebp+8]
   fld     dword [eax]
   fmul    dword [ebp-34h]
   mov     eax, [ebp+8]
   add     eax, 4
   fld     dword [eax]
   fmul    dword [ebp-28h]
   faddp   st1, st0
   mov     eax, [ebp+8]
   add     eax, 8
   fld     dword [eax]
   fmul    dword [ebp-1Ch]
   faddp   st1, st0
   mov     eax, [ebp+0Ch]
   fadd    dword [eax+14h]
   fstp    dword [edx]
   mov     edx, [ebp+10h]
   add     edx, 8
   mov     eax, [ebp+8]
   fld     dword [eax]
   fmul    dword [ebp-30h]
   mov     eax, [ebp+8]
   add     eax, 4
   fld     dword [eax]
   fmul    dword [ebp-24h]
   faddp   st1, st0
   mov     eax, [ebp+8]
   add     eax, 8
   fld     dword [eax]
   fmul    dword [ebp-18h]
   faddp   st1, st0
   mov     eax, [ebp+0Ch]
   fadd    dword [eax+18h]
   fstp    dword [edx]
   leave   
   retn    


sub_80B9D30:
   push    ebp
   mov     ebp, esp
   mov     eax, [ebp+8]
   mov     edx, [ebp+8]
   fld     dword [eax]
   fmul    dword [edx]
   mov     eax, [ebp+8]
   add     eax, 4
   mov     edx, [ebp+8]
   add     edx, 4
   fld     dword [eax]
   fmul    dword [edx]
   faddp   st1, st0
   mov     eax, [ebp+8]
   add     eax, 8
   mov     edx, [ebp+8]
   add     edx, 8
   fld     dword [eax]
   fmul    dword [edx]
   faddp   st1, st0
   mov     eax, [ebp+8]
   add     eax, 0Ch
   mov     edx, [ebp+8]
   add     edx, 0Ch
   fld     dword [eax]
   fmul    dword [edx]
   faddp   st1, st0
   pop     ebp
   retn    


sub_80B9D76:
   push    ebp
   mov     ebp, esp
   sub     esp, 8
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    sub_80B9D30
   fstp    dword [ebp-4]
   fld     dword [ebp-4]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80B9D9C
   jp      loc_80B9D9C
   jmp     loc_80B9DAD
loc_80B9D9C:
   mov     eax, [ebp+8]
   fld     dword [flt_8148E18]
   fdiv    dword [ebp-4]
   fstp    dword [eax+1Ch]
   jmp     locret_80B9DC3
loc_80B9DAD:
   mov     edx, [ebp+8]
   mov     eax, 3F800000h
   mov     [edx+0Ch], eax
   mov     edx, [ebp+8]
   mov     eax, 40000000h
   mov     [edx+1Ch], eax
locret_80B9DC3:
   leave   
   retn    


sub_80A5372:
   push    ebp
   mov     ebp, esp
   mov     ecx, [ebp+10h]
   mov     eax, [ebp+8]
   mov     edx, [ebp+0Ch]
   add     edx, 0Ch
   fld     dword [eax]
   fmul    dword [edx]
   mov     eax, [ebp+8]
   add     eax, 0Ch
   mov     edx, [ebp+0Ch]
   fld     dword [eax]
   fmul    dword [edx]
   faddp   st1, st0
   mov     eax, [ebp+8]
   add     eax, 8
   mov     edx, [ebp+0Ch]
   add     edx, 4
   fld     dword [eax]
   fmul    dword [edx]
   faddp   st1, st0
   mov     eax, [ebp+8]
   add     eax, 4
   mov     edx, [ebp+0Ch]
   add     edx, 8
   fld     dword [eax]
   fmul    dword [edx]
   fsubp   st1, st0
   fstp    dword [ecx]
   mov     ecx, [ebp+10h]
   add     ecx, 4
   mov     eax, [ebp+8]
   add     eax, 4
   mov     edx, [ebp+0Ch]
   add     edx, 0Ch
   fld     dword [eax]
   fmul    dword [edx]
   mov     eax, [ebp+8]
   add     eax, 8
   mov     edx, [ebp+0Ch]
   fld     dword [eax]
   fmul    dword [edx]
   fsubp   st1, st0
   mov     eax, [ebp+8]
   add     eax, 0Ch
   mov     edx, [ebp+0Ch]
   add     edx, 4
   fld     dword [eax]
   fmul    dword [edx]
   faddp   st1, st0
   mov     eax, [ebp+8]
   mov     edx, [ebp+0Ch]
   add     edx, 8
   fld     dword [eax]
   fmul    dword [edx]
   faddp   st1, st0
   fstp    dword [ecx]
   mov     ecx, [ebp+10h]
   add     ecx, 8
   mov     eax, [ebp+8]
   add     eax, 8
   mov     edx, [ebp+0Ch]
   add     edx, 0Ch
   fld     dword [eax]
   fmul    dword [edx]
   mov     eax, [ebp+8]
   add     eax, 4
   mov     edx, [ebp+0Ch]
   fld     dword [eax]
   fmul    dword [edx]
   faddp   st1, st0
   mov     eax, [ebp+8]
   mov     edx, [ebp+0Ch]
   add     edx, 4
   fld     dword [eax]
   fmul    dword [edx]
   fsubp   st1, st0
   mov     eax, [ebp+8]
   add     eax, 0Ch
   mov     edx, [ebp+0Ch]
   add     edx, 8
   fld     dword [eax]
   fmul    dword [edx]
   faddp   st1, st0
   fstp    dword [ecx]
   mov     ecx, [ebp+10h]
   add     ecx, 0Ch
   mov     eax, [ebp+8]
   add     eax, 0Ch
   mov     edx, [ebp+0Ch]
   add     edx, 0Ch
   fld     dword [eax]
   fmul    dword [edx]
   mov     eax, [ebp+8]
   mov     edx, [ebp+0Ch]
   fld     dword [eax]
   fmul    dword [edx]
   fsubp   st1, st0
   mov     eax, [ebp+8]
   add     eax, 4
   mov     edx, [ebp+0Ch]
   add     edx, 4
   fld     dword [eax]
   fmul    dword [edx]
   fsubp   st1, st0
   mov     eax, [ebp+8]
   add     eax, 8
   mov     edx, [ebp+0Ch]
   add     edx, 8
   fld     dword [eax]
   fmul    dword [edx]
   fsubp   st1, st0
   fstp    dword [ecx]
   pop     ebp
   retn    


sub_80C5210:
   push    ebp
   mov     ebp, esp
   mov     edx, [ebp+8]
   mov     eax, 0
   mov     [edx], eax
   mov     edx, [ebp+8]
   add     edx, 4
   mov     eax, 0
   mov     [edx], eax
   mov     edx, [ebp+8]
   add     edx, 8
   mov     eax, 0
   mov     [edx], eax
   pop     ebp
   retn    


sub_80C3C42:
   push    ebp
   mov     ebp, esp
   sub     esp, 48h
   mov     eax, [ebp+8]
   mov     eax, [eax+4]
   mov     [ebp-18h], eax
   mov     eax, [ebp-18h]
   add     eax, 4
   mov     [ebp-24h], eax
   mov     eax, [ebp+8]
   movsx   eax, word [eax]
   mov     [ebp-1Ch], eax
   mov     eax, [ebp+8]
   mov     eax, [eax+8]
   mov     [ebp-10h], eax
   mov     eax, [ebp+8]
   mov     eax, [eax+0Ch]
   mov     [ebp-14h], eax
   mov     eax, [ebp+8]
   add     eax, 44h
   mov     [ebp-20h], eax
   mov     eax, [ebp+8]
   movsx   eax, word [eax+2]
   mov     [ebp-0Ch], eax
loc_80C3C88:
   cmp     dword [ebp-0Ch], 0
   jnz     loc_80C3C90
   jmp     loc_80C3CCC
loc_80C3C90:
   mov     eax, [ebp-20h]
   mov     [esp], eax
   call    sub_80C5210
   mov     edx, [ebp-20h]
   mov     eax, 3F800000h
   mov     [edx+0Ch], eax
   mov     eax, [ebp-20h]
   add     eax, 10h
   mov     [esp], eax
   call    sub_80C5210
   mov     edx, [ebp-20h]
   mov     eax, 40000000h
   mov     [edx+1Ch], eax
   lea     eax, [ebp-0Ch]
   dec     dword [eax]
   lea     eax, [ebp-20h]
   add     dword [eax], 20h
   jmp     loc_80C3C88
loc_80C3CCC:
   mov     eax, [ebp+8]
   movsx   edx, word [eax+2]
   mov     eax, [ebp-1Ch]
   sub     eax, edx
   mov     [ebp-0Ch], eax
loc_80C3CDB:
   cmp     dword [ebp-0Ch], 0
   jnz     loc_80C3CE6
   jmp     loc_80C3DAA
loc_80C3CE6:
   mov     eax, [ebp-10h]
   fild    word [eax]
   fld     dword [flt_8149484]
   fmulp   st1, st0
   fstp    dword [ebp-38h]
   mov     eax, [ebp-10h]
   add     eax, 2
   fild    word [eax]
   fld     dword [flt_8149484]
   fmulp   st1, st0
   fstp    dword [ebp-34h]
   mov     eax, [ebp-10h]
   add     eax, 4
   fild    word [eax]
   fld     dword [flt_8149484]
   fmulp   st1, st0
   fstp    dword [ebp-30h]
   mov     eax, [ebp-10h]
   add     eax, 6
   fild    word [eax]
   fld     dword [flt_8149484]
   fmulp   st1, st0
   fstp    dword [ebp-2Ch]
   mov     eax, [ebp-20h]
   mov     [esp+8], eax
   mov     eax, [ebp-24h]
   movzx   eax, byte [eax]
   mov     edx, eax
   shl     edx, 5
   mov     eax, [ebp-20h]
   sub     eax, edx
   mov     [esp+4], eax
   lea     eax, [ebp-38h]
   mov     [esp], eax
   call    sub_80A5372
   mov     eax, [ebp-20h]
   mov     [esp], eax
   call    sub_80B9D76
   mov     eax, [ebp-20h]
   add     eax, 10h
   mov     [esp+8], eax
   mov     eax, [ebp-24h]
   movzx   eax, byte [eax]
   mov     edx, eax
   shl     edx, 5
   mov     eax, [ebp-20h]
   sub     eax, edx
   mov     [esp+4], eax
   mov     eax, [ebp-14h]
   mov     [esp], eax
   call    sub_80A9094
   lea     eax, [ebp-0Ch]
   dec     dword [eax]
   lea     eax, [ebp-10h]
   add     dword [eax], 8
   lea     eax, [ebp-14h]
   add     dword [eax], 0Ch
   lea     eax, [ebp-20h]
   add     dword [eax], 20h
   lea     eax, [ebp-24h]
   inc     dword [eax]
   jmp     loc_80C3CDB
loc_80C3DAA:
   mov     dword [esp+8], 10h
   mov     dword [esp+4], 0FFh
   mov     eax, [ebp+8]
   add     eax, 14h
   mov     [esp], eax
   call    memset
   mov     dword [esp+8], 10h
   mov     dword [esp+4], 0FFh
   mov     eax, [ebp+8]
   add     eax, 34h
   mov     [esp], eax
   call    memset
   leave   
   retn    


sub_80C51F0:
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


sub_80C3B60:
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
   jle     loc_80C3C26
   fild    dword [ebp-1Ch]
   fstp    dword [esp]
   call    sub_80C51F0
   fld     dword [flt_8149480]
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
   jmp     loc_80C3C2D
loc_80C3C26:
   mov     dword [ebp-28h], 0
loc_80C3C2D:
   mov     eax, [ebp-28h]
   mov     [ebp-0Ch], eax
   mov     edx, [ebp+0Ch]
   add     edx, 6
   mov     eax, [ebp-0Ch]
   mov     [edx], ax
   leave   
   retn    


XModelPartsLoadFile:
   push    ebp
   mov     ebp, esp
   push    esi
   push    ebx
   sub     esp, 0A0h
   mov     eax, [ebp+0Ch]
   mov     [esp+0Ch], eax
   mov     dword [esp+8], aXmodelpartsS
   mov     dword [esp+4], 40h
   lea     eax, [ebp-58h]
   mov     [esp], eax
   call    Com_sprintf
   test    eax, eax
   jns     loc_80C3E3B
   lea     eax, [ebp-58h]
   mov     [esp+4], eax
   mov     dword [esp], a1errorFilename_0
   call    Com_Printf
   mov     dword [ebp-98h], 0
   jmp     loc_80C426F
loc_80C3E3B:
   lea     eax, [ebp-5Ch]
   mov     [esp+4], eax
   lea     eax, [ebp-58h]
   mov     [esp], eax
   call    FS_ReadFile
   mov     [ebp-94h], eax
   cmp     dword [ebp-94h], 0
   jns     loc_80C3E7E
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     dword [esp], a1errorXmodelpa
   call    Com_Printf
   mov     dword [ebp-98h], 0
   jmp     loc_80C426F
loc_80C3E7E:
   cmp     dword [ebp-94h], 0
   jnz     loc_80C3EB4
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     dword [esp], a1errorXmodelpa_0
   call    Com_Printf
   mov     eax, [ebp-5Ch]
   mov     [esp], eax
   call    FS_FreeFile
   mov     dword [ebp-98h], 0
   jmp     loc_80C426F
loc_80C3EB4:
   mov     eax, [ebp-5Ch]
   mov     [ebp-60h], eax
   lea     eax, [ebp-60h]
   mov     [esp], eax
   call    XModelDataReadShort
   mov     [ebp-86h], ax
   cmp     word [ebp-86h], 14h
   jz      loc_80C3F16
   mov     eax, [ebp-5Ch]
   mov     [esp], eax
   call    FS_FreeFile
   mov     dword [esp+0Ch], 14h
   movsx   eax, word [ebp-86h]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     dword [esp], a1errorXmodelpa_1
   call    Com_Printf
   mov     dword [ebp-98h], 0
   jmp     loc_80C426F
loc_80C3F16:
   lea     eax, [ebp-60h]
   mov     [esp], eax
   call    XModelDataReadShort
   mov     [ebp-72h], ax
   lea     eax, [ebp-60h]
   mov     [esp], eax
   call    XModelDataReadShort
   mov     [ebp-70h], ax
   movzx   edx, word [ebp-72h]
   mov     eax, [ebp-70h]
   lea     eax, [eax+edx]
   mov     [ebp-6Eh], ax
   movsx   eax, word [ebp-6Eh]
   add     eax, eax
   mov     [ebp-90h], eax
   mov     eax, [ebp-90h]
   mov     [esp], eax
   mov     eax, [ebp+10h]
   call    eax
   mov     [ebp-68h], eax
   mov     ecx, [ebp+8]
   mov     edx, [ebp+8]
   mov     eax, [ebp-90h]
   add     eax, [edx+84h]
   mov     [ecx+84h], eax
   cmp     word [ebp-6Eh], 7Fh
   jle     loc_80C3FB3
   mov     eax, [ebp-5Ch]
   mov     [esp], eax
   call    FS_FreeFile
   mov     dword [esp+8], 7Fh
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     dword [esp], a1errorXmodelSH
   call    Com_Printf
   mov     dword [ebp-98h], 0
   jmp     loc_80C426F
loc_80C3FB3:
   movsx   eax, word [ebp-72h]
   add     eax, 7
   mov     [ebp-90h], eax
   mov     eax, [ebp-90h]
   mov     [esp], eax
   mov     eax, [ebp+10h]
   call    eax
   mov     [ebp-64h], eax
   mov     ecx, [ebp+8]
   mov     edx, [ebp+8]
   mov     eax, [ebp-90h]
   add     eax, [edx+84h]
   mov     [ecx+84h], eax
   mov     edx, [ebp-64h]
   mov     eax, [ebp-68h]
   mov     [edx], eax
   mov     eax, [ebp-64h]
   add     eax, 4
   mov     [ebp-6Ch], eax
   movsx   eax, word [ebp-6Eh]
   shl     eax, 5
   add     eax, 44h
   mov     [ebp-90h], eax
   mov     eax, [ebp-90h]
   mov     [esp], eax
   mov     eax, [ebp+10h]
   call    eax
   mov     [ebp-0Ch], eax
   mov     ecx, [ebp+8]
   mov     edx, [ebp+8]
   mov     eax, [ebp-90h]
   add     eax, [edx+84h]
   mov     [ecx+84h], eax
   mov     edx, [ebp-0Ch]
   mov     eax, [ebp-64h]
   mov     [edx+4], eax
   cmp     word [ebp-72h], 0
   jz      loc_80C40B7
   movsx   eax, word [ebp-72h]
   shl     eax, 3
   mov     [ebp-90h], eax
   mov     ebx, [ebp-0Ch]
   mov     eax, [ebp-90h]
   mov     [esp], eax
   mov     eax, [ebp+10h]
   call    eax
   mov     [ebx+8], eax
   mov     ecx, [ebp+8]
   mov     edx, [ebp+8]
   mov     eax, [ebp-90h]
   add     eax, [edx+84h]
   mov     [ecx+84h], eax
   movsx   eax, word [ebp-72h]
   shl     eax, 4
   mov     [ebp-90h], eax
   mov     ebx, [ebp-0Ch]
   mov     eax, [ebp-90h]
   mov     [esp], eax
   mov     eax, [ebp+10h]
   call    eax
   mov     [ebx+0Ch], eax
   mov     ecx, [ebp+8]
   mov     edx, [ebp+8]
   mov     eax, [ebp-90h]
   add     eax, [edx+84h]
   mov     [ecx+84h], eax
   jmp     loc_80C40CB
loc_80C40B7:
   mov     eax, [ebp-0Ch]
   mov     dword [eax+8], 0
   mov     eax, [ebp-0Ch]
   mov     dword [eax+0Ch], 0
loc_80C40CB:
   movsx   eax, word [ebp-6Eh]
   mov     [ebp-90h], eax
   mov     ebx, [ebp-0Ch]
   mov     eax, [ebp-90h]
   mov     [esp], eax
   mov     eax, [ebp+10h]
   call    eax
   mov     [ebx+10h], eax
   mov     ecx, [ebp+8]
   mov     edx, [ebp+8]
   mov     eax, [ebp-90h]
   add     eax, [edx+84h]
   mov     [ecx+84h], eax
   mov     edx, [ebp-0Ch]
   movzx   eax, word [ebp-6Eh]
   mov     [edx], ax
   mov     edx, [ebp-0Ch]
   mov     eax, [ebp-70h]
   mov     [edx+2], ax
   mov     eax, [ebp-0Ch]
   mov     eax, [eax+8]
   mov     [ebp-8Ch], eax
   mov     eax, [ebp-0Ch]
   mov     eax, [eax+0Ch]
   mov     [ebp-7Ch], eax
   movsx   eax, word [ebp-70h]
   mov     [ebp-78h], eax
loc_80C4131:
   movsx   eax, word [ebp-6Eh]
   cmp     [ebp-78h], eax
   jl      loc_80C413F
   jmp     loc_80C41CB
loc_80C413F:
   mov     eax, [ebp-60h]
   movzx   eax, byte [eax]
   mov     [ebp-84h], eax
   lea     eax, [ebp-60h]
   inc     dword [eax]
   mov     ecx, [ebp-6Ch]
   movzx   edx, byte [ebp-84h]
   movzx   eax, byte [ebp-78h]
   sub     al, dl
   mov     [ecx], al
   lea     eax, [ebp-60h]
   mov     [esp], eax
   call    XModelDataReadFloat
   mov     eax, [ebp-7Ch]
   fstp    dword [eax]
   lea     eax, [ebp-60h]
   mov     [esp], eax
   call    XModelDataReadFloat
   mov     eax, [ebp-7Ch]
   add     eax, 4
   fstp    dword [eax]
   lea     eax, [ebp-60h]
   mov     [esp], eax
   call    XModelDataReadFloat
   mov     eax, [ebp-7Ch]
   add     eax, 8
   fstp    dword [eax]
   mov     eax, [ebp-8Ch]
   mov     [esp+4], eax
   lea     eax, [ebp-60h]
   mov     [esp], eax
   call    sub_80C3B60
   lea     eax, [ebp-78h]
   inc     dword [eax]
   lea     eax, [ebp-8Ch]
   add     dword [eax], 8
   lea     eax, [ebp-7Ch]
   add     dword [eax], 0Ch
   lea     eax, [ebp-6Ch]
   inc     dword [eax]
   jmp     loc_80C4131
loc_80C41CB:
   mov     dword [ebp-78h], 0
loc_80C41D2:
   movsx   eax, word [ebp-6Eh]
   cmp     [ebp-78h], eax
   jl      loc_80C41DD
   jmp     loc_80C422A
loc_80C41DD:
   mov     eax, [ebp-60h]
   mov     [esp], eax
   call    strlen
   inc     eax
   mov     [ebp-80h], eax
   mov     eax, [ebp-78h]
   lea     ebx, [eax+eax]
   mov     esi, [ebp-68h]
   mov     dword [esp+0Ch], 0Ah
   mov     eax, [ebp-80h]
   mov     [esp+8], eax
   mov     dword [esp+4], 0
   mov     eax, [ebp-60h]
   mov     [esp], eax
   call    SL_GetStringOfLen
   mov     [esi+ebx], ax
   mov     edx, [ebp-80h]
   lea     eax, [ebp-60h]
   add     [eax], edx
   lea     eax, [ebp-78h]
   inc     dword [eax]
   jmp     loc_80C41D2
loc_80C422A:
   movsx   eax, word [ebp-6Eh]
   mov     [esp+8], eax
   mov     eax, [ebp-60h]
   mov     [esp+4], eax
   mov     eax, [ebp-0Ch]
   mov     eax, [eax+10h]
   mov     [esp], eax
   call    memcpy
   movsx   edx, word [ebp-6Eh]
   lea     eax, [ebp-60h]
   add     [eax], edx
   mov     eax, [ebp-5Ch]
   mov     [esp], eax
   call    FS_FreeFile
   mov     eax, [ebp-0Ch]
   mov     [esp], eax
   call    sub_80C3C42
   mov     eax, [ebp-0Ch]
   mov     [ebp-98h], eax
loc_80C426F:
   mov     eax, [ebp-98h]
   add     esp, 0A0h
   pop     ebx
   pop     esi
   pop     ebp
   retn    


section .rdata
flt_8149480       dd 0.5
flt_8149484       dd 0.000030518509
flt_8148E18       dd 2.0

aXmodelpartsS     db 'xmodelparts/%s',0
a1errorXmodelSH   db '^1ERROR: xmodel ',27h,'%s',27h,' has more than %d bones',0Ah,0
a1errorXmodelpa_1 db '^1ERROR: xmodelparts ',27h,'%s',27h,' out of date (version %d, expecting %d).',0Ah,0
a1errorXmodelpa_0 db '^1ERROR: xmodelparts ',27h,'%s',27h,' has 0 length',0Ah,0
a1errorFilename_0 db '^1ERROR: filename ',27h,'%s',27h,' too long',0Ah,0
a1errorXmodelpa   db '^1ERROR: xmodelparts ',27h,'%s',27h,' not found',0Ah,0