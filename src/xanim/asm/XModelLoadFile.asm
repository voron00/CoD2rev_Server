extern SL_GetString_
extern FS_FreeFile
extern FS_ReadFile
extern XModelFree
extern strlen
extern strcpy
extern memcpy
extern strlen
extern Com_sprintf
extern Com_Printf
extern XModelPartsLoad


global XModelLoadFile
global XModelLoadCollision
global XModelIsValidModelFile
global XModelDataReadInt
global XModelDataReadShort
global XModelDataReadFloat


section .text


XModelLoadCollision:
   push    ebp
   mov     ebp, esp
   push    ebx
   sub     esp, 14h
   mov     ebx, [ebp+0Ch]
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XModelDataReadInt
   mov     [ebx+58h], eax
   mov     eax, [ebp+0Ch]
   cmp     dword [eax+58h], 0
   jnz     loc_80C441A
   jmp     loc_80C4684
loc_80C441A:
   mov     ebx, [ebp+0Ch]
   mov     eax, [ebp+0Ch]
   mov     edx, [eax+58h]
   mov     eax, edx
   shl     eax, 2
   add     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   mov     [esp], eax
   mov     eax, [ebp+10h]
   call    eax
   mov     [ebx+54h], eax
   mov     dword [ebp-10h], 0
loc_80C4443:
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp-10h]
   cmp     eax, [edx+58h]
   jl      loc_80C4453
   jmp     loc_80C4684
loc_80C4453:
   mov     ecx, [ebp+0Ch]
   mov     edx, [ebp-10h]
   mov     eax, edx
   shl     eax, 2
   add     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   add     eax, [ecx+54h]
   mov     [ebp-8], eax
   mov     ebx, [ebp-8]
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XModelDataReadInt
   mov     [ebx+4], eax
   mov     ebx, [ebp-8]
   mov     eax, [ebp-8]
   mov     edx, [eax+4]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 4
   mov     [esp], eax
   mov     eax, [ebp+10h]
   call    eax
   mov     [ebx], eax
   mov     dword [ebp-14h], 0
loc_80C44A1:
   mov     edx, [ebp-8]
   mov     eax, [ebp-14h]
   cmp     eax, [edx+4]
   jl      loc_80C44B1
   jmp     loc_80C459A
loc_80C44B1:
   mov     ecx, [ebp-8]
   mov     edx, [ebp-14h]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 4
   add     eax, [ecx]
   mov     [ebp-0Ch], eax
   mov     ebx, [ebp-0Ch]
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XModelDataReadFloat
   fstp    dword [ebx]
   mov     ebx, [ebp-0Ch]
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XModelDataReadFloat
   fstp    dword [ebx+4]
   mov     ebx, [ebp-0Ch]
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XModelDataReadFloat
   fstp    dword [ebx+8]
   mov     ebx, [ebp-0Ch]
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XModelDataReadFloat
   fstp    dword [ebx+0Ch]
   mov     ebx, [ebp-0Ch]
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XModelDataReadFloat
   fstp    dword [ebx+10h]
   mov     ebx, [ebp-0Ch]
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XModelDataReadFloat
   fstp    dword [ebx+14h]
   mov     ebx, [ebp-0Ch]
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XModelDataReadFloat
   fstp    dword [ebx+18h]
   mov     ebx, [ebp-0Ch]
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XModelDataReadFloat
   fstp    dword [ebx+1Ch]
   mov     ebx, [ebp-0Ch]
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XModelDataReadFloat
   fstp    dword [ebx+20h]
   mov     ebx, [ebp-0Ch]
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XModelDataReadFloat
   fstp    dword [ebx+24h]
   mov     ebx, [ebp-0Ch]
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XModelDataReadFloat
   fstp    dword [ebx+28h]
   mov     ebx, [ebp-0Ch]
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XModelDataReadFloat
   fstp    dword [ebx+2Ch]
   lea     eax, [ebp-14h]
   inc     dword [eax]
   jmp     loc_80C44A1
loc_80C459A:
   mov     ebx, [ebp-8]
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XModelDataReadFloat
   fld     dword [flt_8149640]
   fsubp   st1, st0
   fstp    dword [ebx+8]
   mov     ebx, [ebp-8]
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XModelDataReadFloat
   fld     dword [flt_8149640]
   fsubp   st1, st0
   fstp    dword [ebx+0Ch]
   mov     ebx, [ebp-8]
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XModelDataReadFloat
   fld     dword [flt_8149640]
   fsubp   st1, st0
   fstp    dword [ebx+10h]
   mov     ebx, [ebp-8]
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XModelDataReadFloat
   fld     dword [flt_8149640]
   faddp   st1, st0
   fstp    dword [ebx+14h]
   mov     ebx, [ebp-8]
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XModelDataReadFloat
   fld     dword [flt_8149640]
   faddp   st1, st0
   fstp    dword [ebx+18h]
   mov     ebx, [ebp-8]
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XModelDataReadFloat
   fld     dword [flt_8149640]
   faddp   st1, st0
   fstp    dword [ebx+1Ch]
   mov     ebx, [ebp-8]
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XModelDataReadInt
   mov     [ebx+20h], eax
   mov     ebx, [ebp-8]
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XModelDataReadInt
   and     eax, 0DFFFFFFBh
   mov     [ebx+24h], eax
   mov     ebx, [ebp-8]
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XModelDataReadInt
   mov     [ebx+28h], eax
   mov     ecx, [ebp+0Ch]
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp-8]
   mov     eax, [eax+24h]
   or      eax, [edx+5Ch]
   mov     [ecx+5Ch], eax
   lea     eax, [ebp-10h]
   inc     dword [eax]
   jmp     loc_80C4443
loc_80C4684:
   add     esp, 14h
   pop     ebx
   pop     ebp
   retn    


XModelIsValidModelFile:
   push    ebp
   mov     ebp, esp
   push    esi
   push    ebx
   sub     esp, 20h
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    XModelDataReadShort
   mov     [ebp-0Eh], ax
   cmp     word [ebp-0Eh], 14h
   jz      loc_80C42CD
   mov     dword [esp+0Ch], 14h
   movsx   eax, word [ebp-0Eh]
   mov     [esp+8], eax
   mov     eax, [ebp+8]
   mov     [esp+4], eax
   mov     dword [esp], a1errorXmodelSO
   call    Com_Printf
   mov     dword [ebp-14h], 0
   jmp     loc_80C43EA
loc_80C42CD:
   mov     edx, [ebp+10h]
   mov     eax, [ebp+0Ch]
   mov     eax, [eax]
   movzx   eax, byte [eax]
   mov     [edx+102Ch], al
   mov     eax, [ebp+0Ch]
   inc     dword [eax]
   mov     ebx, [ebp+10h]
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    XModelDataReadFloat
   fstp    dword [ebx+1010h]
   mov     ebx, [ebp+10h]
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    XModelDataReadFloat
   fstp    dword [ebx+1014h]
   mov     ebx, [ebp+10h]
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    XModelDataReadFloat
   fstp    dword [ebx+1018h]
   mov     ebx, [ebp+10h]
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    XModelDataReadFloat
   fstp    dword [ebx+101Ch]
   mov     ebx, [ebp+10h]
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    XModelDataReadFloat
   fstp    dword [ebx+1020h]
   mov     ebx, [ebp+10h]
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    XModelDataReadFloat
   fstp    dword [ebx+1024h]
   mov     dword [ebp-0Ch], 0
loc_80C4362:
   cmp     dword [ebp-0Ch], 3
   jle     loc_80C436A
   jmp     loc_80C43CF
loc_80C436A:
   mov     ecx, [ebp+10h]
   mov     edx, [ebp-0Ch]
   mov     eax, edx
   shl     eax, 8
   add     eax, edx
   shl     eax, 2
   add     eax, ecx
   lea     ebx, [eax+400h]
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    XModelDataReadFloat
   fstp    dword [ebx]
   mov     eax, [ebp+0Ch]
   mov     eax, [eax]
   mov     [esp+4], eax
   mov     edx, [ebp-0Ch]
   mov     eax, edx
   shl     eax, 8
   add     eax, edx
   shl     eax, 2
   add     eax, [ebp+10h]
   mov     [esp], eax
   call    strcpy
   mov     esi, [ebp+0Ch]
   mov     ebx, [ebp+0Ch]
   mov     eax, [ebp+0Ch]
   mov     eax, [eax]
   mov     [esp], eax
   call    strlen
   add     eax, [ebx]
   inc     eax
   mov     [esi], eax
   lea     eax, [ebp-0Ch]
   inc     dword [eax]
   jmp     loc_80C4362
loc_80C43CF:
   mov     ebx, [ebp+10h]
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    XModelDataReadInt
   mov     [ebx+1028h], eax
   mov     dword [ebp-14h], 1
loc_80C43EA:
   mov     eax, [ebp-14h]
   add     esp, 20h
   pop     ebx
   pop     esi
   pop     ebp
   retn    


XModelDataReadInt:
   push    ebp
   mov     ebp, esp
   sub     esp, 18h
   mov     dword [esp+8], 4
   mov     eax, [ebp+8]
   mov     eax, [eax]
   mov     [esp+4], eax
   lea     eax, [ebp-4]
   mov     [esp], eax
   call    memcpy
   mov     edx, [ebp+8]
   mov     eax, [ebp+8]
   mov     eax, [eax]
   add     eax, 4
   mov     [edx], eax
   mov     eax, [ebp-4]
   leave   
   retn    


XModelDataReadShort:
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
   movsx   eax, word [ebp-2]
   leave   
   retn    


XModelDataReadFloat:
   push    ebp
   mov     ebp, esp
   sub     esp, 18h
   mov     dword [esp+8], 4
   mov     eax, [ebp+8]
   mov     eax, [eax]
   mov     [esp+4], eax
   lea     eax, [ebp-4]
   mov     [esp], eax
   call    memcpy
   mov     edx, [ebp+8]
   mov     eax, [ebp+8]
   mov     eax, [eax]
   add     eax, 4
   mov     [edx], eax
   mov     eax, [ebp-4]
   mov     [ebp-8], eax
   fld     dword [ebp-8]
   leave   
   retn    


sub_80C525E:
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


sub_80C528E:
   push    ebp
   mov     ebp, esp
   mov     ecx, [ebp+10h]
   mov     eax, [ebp+8]
   mov     edx, [ebp+0Ch]
   fld     dword [eax]
   fsub    dword [edx]
   fstp    dword [ecx]
   mov     ecx, [ebp+10h]
   add     ecx, 4
   mov     eax, [ebp+8]
   add     eax, 4
   mov     edx, [ebp+0Ch]
   add     edx, 4
   fld     dword [eax]
   fsub    dword [edx]
   fstp    dword [ecx]
   mov     ecx, [ebp+10h]
   add     ecx, 8
   mov     eax, [ebp+8]
   add     eax, 8
   mov     edx, [ebp+0Ch]
   add     edx, 8
   fld     dword [eax]
   fsub    dword [edx]
   fstp    dword [ecx]
   pop     ebp
   retn    


sub_80C52D2:
   push    ebp
   mov     ebp, esp
   mov     ecx, [ebp+10h]
   mov     eax, [ebp+8]
   mov     edx, [ebp+0Ch]
   fld     dword [eax]
   fadd    dword [edx]
   fld     dword [flt_814972C]
   fmulp   st1, st0
   fstp    dword [ecx]
   mov     ecx, [ebp+10h]
   add     ecx, 4
   mov     eax, [ebp+8]
   add     eax, 4
   mov     edx, [ebp+0Ch]
   add     edx, 4
   fld     dword [eax]
   fadd    dword [edx]
   fld     dword [flt_814972C]
   fmulp   st1, st0
   fstp    dword [ecx]
   mov     ecx, [ebp+10h]
   add     ecx, 8
   mov     eax, [ebp+8]
   add     eax, 8
   mov     edx, [ebp+0Ch]
   add     edx, 8
   fld     dword [eax]
   fadd    dword [edx]
   fld     dword [flt_814972C]
   fmulp   st1, st0
   fstp    dword [ecx]
   pop     ebp
   retn    


sub_80C532E:
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
   pop     ebp
   retn    


XModelLoadFile:
   push    ebp
   mov     ebp, esp
   push    esi
   push    ebx
   sub     esp, 1100h
   mov     eax, [ebp+8]
   mov     [esp+0Ch], eax
   mov     dword [esp+8], aXmodelS
   mov     dword [esp+4], 40h
   lea     eax, [ebp-10B8h]
   mov     [esp], eax
   call    Com_sprintf
   test    eax, eax
   jns     loc_80C4767
   lea     eax, [ebp-10B8h]
   mov     [esp+4], eax
   mov     dword [esp], a1errorFilename_0
   call    Com_Printf
   mov     dword [ebp-10F8h], 0
   jmp     loc_80C4E2C
loc_80C4767:
   lea     eax, [ebp-10BCh]
   mov     [esp+4], eax
   lea     eax, [ebp-10B8h]
   mov     [esp], eax
   call    FS_ReadFile
   mov     [ebp-10F4h], eax
   cmp     dword [ebp-10F4h], 0
   jns     loc_80C47B0
   mov     eax, [ebp+8]
   mov     [esp+4], eax
   mov     dword [esp], a1errorXmodelSN
   call    Com_Printf
   mov     dword [ebp-10F8h], 0
   jmp     loc_80C4E2C
loc_80C47B0:
   cmp     dword [ebp-10F4h], 0
   jnz     loc_80C47E9
   mov     eax, [ebp+8]
   mov     [esp+4], eax
   mov     dword [esp], a1errorXmodelSH_0
   call    Com_Printf
   mov     eax, [ebp-10BCh]
   mov     [esp], eax
   call    FS_FreeFile
   mov     dword [ebp-10F8h], 0
   jmp     loc_80C4E2C
loc_80C47E9:
   mov     eax, [ebp-10BCh]
   mov     [ebp-105Ch], eax
   lea     eax, [ebp-1038h]
   mov     [esp+8], eax
   lea     eax, [ebp-105Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XModelIsValidModelFile
   test    al, al
   jnz     loc_80C4835
   mov     eax, [ebp-10BCh]
   mov     [esp], eax
   call    FS_FreeFile
   mov     dword [ebp-10F8h], 0
   jmp     loc_80C4E2C
loc_80C4835:
   mov     dword [ebp-1064h], 0
   mov     dword [ebp-1068h], 0
loc_80C4849:
   cmp     dword [ebp-1068h], 3
   jle     loc_80C4854
   jmp     loc_80C48A2
loc_80C4854:
   mov     ebx, [ebp-1068h]
   lea     ecx, [ebp-1038h]
   mov     edx, [ebp-1068h]
   mov     eax, edx
   shl     eax, 8
   add     eax, edx
   shl     eax, 2
   lea     eax, [eax+ecx]
   mov     [esp], eax
   call    strlen
   inc     eax
   mov     [ebp+ebx*4-1058h], eax
   mov     eax, [ebp-1068h]
   mov     edx, [ebp+eax*4-1058h]
   lea     eax, [ebp-1064h]
   add     [eax], edx
   lea     eax, [ebp-1068h]
   inc     dword [eax]
   jmp     loc_80C4849
loc_80C48A2:
   mov     eax, [ebp-1064h]
   add     eax, 90h
   mov     [ebp-10F0h], eax
   mov     eax, [ebp-10F0h]
   mov     [esp], eax
   mov     eax, [ebp+0Ch]
   call    eax
   mov     [ebp-103Ch], eax
   mov     edx, [ebp-103Ch]
   mov     eax, [ebp-10F0h]
   mov     [edx+84h], eax
   mov     eax, [ebp+8]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+10h]
   mov     [esp+8], eax
   mov     eax, [ebp-103Ch]
   mov     [esp+4], eax
   lea     eax, [ebp-105Ch]
   mov     [esp], eax
   call    XModelLoadCollision
   mov     eax, [ebp-103Ch]
   add     eax, 90h
   mov     [ebp-1060h], eax
   mov     eax, [ebp-103Ch]
   mov     word [eax+7Ch], 0
   mov     dword [ebp-1068h], 0
loc_80C4926:
   cmp     dword [ebp-1068h], 3
   jle     loc_80C4934
   jmp     loc_80C4B69
loc_80C4934:
   lea     ecx, [ebp-1038h]
   mov     edx, [ebp-1068h]
   mov     eax, edx
   shl     eax, 8
   add     eax, edx
   shl     eax, 2
   lea     eax, [eax+ecx]
   mov     [esp+4], eax
   mov     eax, [ebp-1060h]
   mov     [esp], eax
   call    strcpy
   mov     ecx, [ebp-103Ch]
   mov     edx, [ebp-1068h]
   mov     eax, edx
   shl     eax, 2
   add     eax, edx
   shl     eax, 2
   add     eax, ecx
   lea     edx, [eax+8]
   mov     eax, [ebp-1060h]
   mov     [edx], eax
   mov     eax, [ebp-1060h]
   cmp     byte [eax], 0
   jz      loc_80C4AED
   mov     edx, [ebp-103Ch]
   mov     eax, [ebp-103Ch]
   movzx   eax, word [eax+7Ch]
   inc     eax
   mov     [edx+7Ch], ax
   mov     esi, [ebp-103Ch]
   mov     edx, [ebp-1068h]
   mov     eax, edx
   shl     eax, 2
   add     eax, edx
   lea     ebx, 0[eax*4]
   lea     eax, [ebp-105Ch]
   mov     [esp], eax
   call    XModelDataReadShort
   mov     [esi+ebx+0Ch], ax
   mov     ecx, [ebp-103Ch]
   mov     edx, [ebp-1068h]
   mov     eax, edx
   shl     eax, 2
   add     eax, edx
   shl     eax, 2
   movsx   eax, word [ecx+eax+0Ch]
   add     eax, eax
   mov     [ebp-10F0h], eax
   mov     ecx, [ebp-103Ch]
   mov     edx, [ebp-1068h]
   mov     eax, edx
   shl     eax, 2
   add     eax, edx
   shl     eax, 2
   add     eax, ecx
   lea     ebx, [eax+10h]
   mov     eax, [ebp-10F0h]
   mov     [esp], eax
   mov     eax, [ebp+0Ch]
   call    eax
   mov     [ebx], eax
   mov     ecx, [ebp-103Ch]
   mov     edx, [ebp-103Ch]
   mov     eax, [ebp-10F0h]
   add     eax, [edx+84h]
   mov     [ecx+84h], eax
   mov     dword [ebp-106Ch], 0
loc_80C4A49:
   mov     ecx, [ebp-103Ch]
   mov     edx, [ebp-1068h]
   mov     eax, edx
   shl     eax, 2
   add     eax, edx
   shl     eax, 2
   movsx   eax, word [ecx+eax+0Ch]
   cmp     [ebp-106Ch], eax
   jl      loc_80C4A71
   jmp     loc_80C4B0E
loc_80C4A71:
   mov     eax, [ebp-105Ch]
   mov     [ebp-10C0h], eax
   mov     eax, [ebp-10C0h]
   mov     [esp], eax
   call    strlen
   add     eax, [ebp-105Ch]
   inc     eax
   mov     [ebp-105Ch], eax
   mov     ecx, [ebp-103Ch]
   mov     edx, [ebp-1068h]
   mov     eax, edx
   shl     eax, 2
   add     eax, edx
   shl     eax, 2
   add     eax, ecx
   lea     edx, [eax+10h]
   mov     eax, [ebp-106Ch]
   lea     ebx, [eax+eax]
   mov     esi, [edx]
   mov     dword [esp+8], 8
   mov     dword [esp+4], 0
   mov     eax, [ebp-10C0h]
   mov     [esp], eax
   call    SL_GetString_
   mov     [esi+ebx], ax
   lea     eax, [ebp-106Ch]
   inc     dword [eax]
   jmp     loc_80C4A49
loc_80C4AED:
   mov     ecx, [ebp-103Ch]
   mov     edx, [ebp-1068h]
   mov     eax, edx
   shl     eax, 2
   add     eax, edx
   shl     eax, 2
   add     eax, ecx
   add     eax, 10h
   mov     dword [eax], 0
loc_80C4B0E:
   mov     ecx, [ebp-103Ch]
   mov     edx, [ebp-1068h]
   mov     eax, edx
   shl     eax, 2
   add     eax, edx
   shl     eax, 2
   add     eax, ecx
   lea     ecx, [eax+4]
   mov     edx, [ebp-1068h]
   mov     eax, edx
   shl     eax, 8
   add     eax, edx
   shl     eax, 2
   lea     edx, [ebp-8]
   add     eax, edx
   sub     eax, 0C30h
   mov     eax, [eax]
   mov     [ecx], eax
   mov     eax, [ebp-1068h]
   mov     edx, [ebp+eax*4-1058h]
   lea     eax, [ebp-1060h]
   add     [eax], edx
   lea     eax, [ebp-1068h]
   inc     dword [eax]
   jmp     loc_80C4926
loc_80C4B69:
   mov     ebx, [ebp-103Ch]
   mov     eax, [ebp+0Ch]
   mov     [esp+8], eax
   mov     eax, [ebp-103Ch]
   mov     eax, [eax+8]
   mov     [esp+4], eax
   mov     eax, [ebp-103Ch]
   mov     [esp], eax
   call    XModelPartsLoad
   mov     [ebx], eax
   mov     eax, [ebp-103Ch]
   cmp     dword [eax], 0
   jnz     loc_80C4BC9
   mov     eax, [ebp-10BCh]
   mov     [esp], eax
   call    FS_FreeFile
   mov     eax, [ebp-103Ch]
   mov     [esp], eax
   call    XModelFree
   mov     dword [ebp-10F8h], 0
   jmp     loc_80C4E2C
loc_80C4BC9:
   mov     eax, [ebp-103Ch]
   mov     eax, [eax]
   movsx   eax, word [eax]
   mov     [ebp-10C4h], eax
   mov     edx, [ebp-10C4h]
   mov     eax, edx
   shl     eax, 2
   add     eax, edx
   shl     eax, 3
   mov     [ebp-10F0h], eax
   mov     eax, [ebp-10F0h]
   mov     [esp], eax
   mov     eax, [ebp+0Ch]
   call    eax
   mov     [ebp-10ECh], eax
   mov     ecx, [ebp-103Ch]
   mov     edx, [ebp-103Ch]
   mov     eax, [ebp-10F0h]
   add     eax, [edx+84h]
   mov     [ecx+84h], eax
   mov     dword [ebp-1068h], 0
loc_80C4C2C:
   mov     eax, [ebp-1068h]
   cmp     eax, [ebp-10C4h]
   jl      loc_80C4C3F
   jmp     loc_80C4DA6
loc_80C4C3F:
   mov     edx, [ebp-1068h]
   mov     eax, edx
   shl     eax, 2
   add     eax, edx
   shl     eax, 3
   add     eax, [ebp-10ECh]
   mov     [ebp-10C8h], eax
   lea     eax, [ebp-105Ch]
   mov     [esp], eax
   call    XModelDataReadFloat
   mov     eax, [ebp-10C8h]
   fstp    dword [eax]
   lea     eax, [ebp-105Ch]
   mov     [esp], eax
   call    XModelDataReadFloat
   mov     eax, [ebp-10C8h]
   add     eax, 4
   fstp    dword [eax]
   lea     eax, [ebp-105Ch]
   mov     [esp], eax
   call    XModelDataReadFloat
   mov     eax, [ebp-10C8h]
   add     eax, 8
   fstp    dword [eax]
   mov     edx, [ebp-1068h]
   mov     eax, edx
   shl     eax, 2
   add     eax, edx
   shl     eax, 3
   add     eax, [ebp-10ECh]
   add     eax, 0Ch
   mov     [ebp-10CCh], eax
   lea     eax, [ebp-105Ch]
   mov     [esp], eax
   call    XModelDataReadFloat
   mov     eax, [ebp-10CCh]
   fstp    dword [eax]
   lea     eax, [ebp-105Ch]
   mov     [esp], eax
   call    XModelDataReadFloat
   mov     eax, [ebp-10CCh]
   add     eax, 4
   fstp    dword [eax]
   lea     eax, [ebp-105Ch]
   mov     [esp], eax
   call    XModelDataReadFloat
   mov     eax, [ebp-10CCh]
   add     eax, 8
   fstp    dword [eax]
   mov     edx, [ebp-1068h]
   mov     eax, edx
   shl     eax, 2
   add     eax, edx
   shl     eax, 3
   add     eax, [ebp-10ECh]
   add     eax, 18h
   mov     [ebp-10D0h], eax
   mov     eax, [ebp-10D0h]
   mov     [esp+8], eax
   mov     eax, [ebp-10CCh]
   mov     [esp+4], eax
   mov     eax, [ebp-10C8h]
   mov     [esp], eax
   call    sub_80C52D2
   lea     eax, [ebp-10E8h]
   mov     [esp+8], eax
   mov     eax, [ebp-10D0h]
   mov     [esp+4], eax
   mov     eax, [ebp-10CCh]
   mov     [esp], eax
   call    sub_80C528E
   mov     edx, [ebp-1068h]
   mov     eax, edx
   shl     eax, 2
   add     eax, edx
   lea     esi, 0[eax*8]
   mov     ebx, [ebp-10ECh]
   lea     eax, [ebp-10E8h]
   mov     [esp], eax
   call    sub_80C532E
   fstp    dword [ebx+esi+24h]
   lea     eax, [ebp-1068h]
   inc     dword [eax]
   jmp     loc_80C4C2C
loc_80C4DA6:
   mov     edx, [ebp-103Ch]
   mov     eax, [ebp-10ECh]
   mov     [edx+60h], eax
   mov     eax, [ebp-10BCh]
   mov     [esp], eax
   call    FS_FreeFile
   mov     eax, [ebp-103Ch]
   add     eax, 64h
   mov     [esp+4], eax
   lea     eax, [ebp-1038h]
   add     eax, 1010h
   mov     [esp], eax
   call    sub_80C525E
   mov     eax, [ebp-103Ch]
   add     eax, 70h
   mov     [esp+4], eax
   lea     eax, [ebp-1038h]
   add     eax, 101Ch
   mov     [esp], eax
   call    sub_80C525E
   mov     edx, [ebp-103Ch]
   mov     eax, [ebp-10h]
   mov     [edx+7Eh], ax
   mov     edx, [ebp-103Ch]
   movzx   eax, byte [ebp-0Ch]
   mov     [edx+8Ch], al
   mov     eax, [ebp-103Ch]
   mov     [ebp-10F8h], eax
loc_80C4E2C:
   mov     eax, [ebp-10F8h]
   add     esp, 1100h
   pop     ebx
   pop     esi
   pop     ebp
   retn    

section .rdata
flt_814972C       dd 0.5
flt_8149640       dd 0.001

aXmodelS          db 'xmodel/%s',0
a1errorXmodelSH_0 db '^1ERROR: xmodel ',27h,'%s',27h,' has 0 length',0Ah,0
a1errorFilename_0 db '^1ERROR: filename ',27h,'%s',27h,' too long',0Ah,0
a1errorXmodelSN   db '^1ERROR: xmodel ',27h,'%s',27h,' not found',0Ah,0
a1errorXmodelSO   db '^1ERROR: xmodel ',27h,'%s',27h,' out of date (version %d, expecting %d).',0Ah,0