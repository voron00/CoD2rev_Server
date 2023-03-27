extern memset
extern sin
extern cos
extern BG_GetConditionValue
extern GetLeanFraction


global BG_Player_DoControllersInternal


section .text


sub_80D9D06:
   push    ebp
   mov     ebp, esp
   mov     edx, [ebp+8]
   mov     eax, [ebp+0Ch]
   mov     [edx], eax
   mov     edx, [ebp+8]
   add     edx, 4
   mov     eax, [ebp+10h]
   mov     [edx], eax
   mov     edx, [ebp+8]
   add     edx, 8
   mov     eax, [ebp+14h]
   mov     [edx], eax
   pop     ebp
   retn    


sub_80A6062:
   push    ebp
   mov     ebp, esp
   sub     esp, 8
   mov     eax, [ebp+0Ch]
   mov     eax, [eax]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     eax, [eax]
   mov     [esp], eax
   call    AngleSubtract
   mov     eax, [ebp+10h]
   fstp    dword [eax]
   mov     eax, [ebp+0Ch]
   add     eax, 4
   mov     eax, [eax]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   add     eax, 4
   mov     eax, [eax]
   mov     [esp], eax
   call    AngleSubtract
   mov     eax, [ebp+10h]
   add     eax, 4
   fstp    dword [eax]
   mov     eax, [ebp+0Ch]
   add     eax, 8
   mov     eax, [eax]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   add     eax, 8
   mov     eax, [eax]
   mov     [esp], eax
   call    AngleSubtract
   mov     eax, [ebp+10h]
   add     eax, 8
   fstp    dword [eax]
   leave   
   retn    


sub_80A60CE:
   push    ebp
   mov     ebp, esp
   sub     esp, 8
   fld     dword [ebp+8]
   fld     dword [flt_8146080]
   fmulp   st1, st0
   fnstcw  word [ebp-2]
   movzx   eax, word [ebp-2]
   or      ax, 0C00h
   mov     [ebp-4], ax
   fldcw   word [ebp-4]
   fistp   dword [ebp-8]
   fldcw   word [ebp-2]
   mov     eax, [ebp-8]
   and     eax, 0FFFFh
   push    eax
   fild    dword [esp]
   lea     esp, [esp+4]
   fld     dword [flt_8146084]
   fmulp   st1, st0
   leave   
   retn    


AngleNormalize180:
   push    ebp
   mov     ebp, esp
   sub     esp, 18h
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    sub_80A60CE
   fstp    dword [ebp-4]
   fld     dword [ebp-4]
   fld     dword [flt_8146088]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80A613A
   jmp     loc_80A6148
loc_80A613A:
   fld     dword [ebp-4]
   fld     dword [flt_814608C]
   fsubp   st1, st0
   fstp    dword [ebp-4]
loc_80A6148:
   mov     eax, [ebp-4]
   mov     [ebp-8], eax
   fld     dword [ebp-8]
   leave   
   retn    


sub_80D9CAE:
   push    ebp
   mov     ebp, esp
   push    ebx
   sub     esp, 14h
   mov     ebx, [ebp+0Ch]
   fld     dword [ebp+8]
   fstp    qword [esp]
   call    sin
   fstp    dword [ebx]
   mov     ebx, [ebp+10h]
   fld     dword [ebp+8]
   fstp    qword [esp]
   call    cos
   fstp    dword [ebx]
   add     esp, 14h
   pop     ebx
   pop     ebp
   retn    


VectorCopy439:
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


AngleSubtract:
   push    ebp
   mov     ebp, esp
   sub     esp, 8
   fld     dword [ebp+8]
   fsub    dword [ebp+0Ch]
   fstp    dword [ebp-4]
loc_80A600F:
   fld     dword [ebp-4]
   fld     dword [flt_8146074]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80A6023
   jmp     loc_80A6033
loc_80A6023:
   fld     dword [ebp-4]
   fld     dword [flt_8146078]
   fsubp   st1, st0
   fstp    dword [ebp-4]
   jmp     loc_80A600F
loc_80A6033:
   nop     
loc_80A6034:
   fld     dword [ebp-4]
   fld     dword [flt_814607C]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80A6046
   jmp     loc_80A6056
loc_80A6046:
   fld     dword [ebp-4]
   fld     dword [flt_8146078]
   faddp   st1, st0
   fstp    dword [ebp-4]
   jmp     loc_80A6034
loc_80A6056:
   mov     eax, [ebp-4]
   mov     [ebp-8], eax
   fld     dword [ebp-8]
   leave   
   retn    


VectorClear871:
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


BG_Player_DoControllersInternal:
   push    ebp
   mov     ebp, esp
   sub     esp, 0E8h
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+8]
   and     eax, 300h
   test    eax, eax
   jz      loc_80D8C76
   mov     dword [esp+8], 60h
   mov     dword [esp+4], 0
   mov     eax, [ebp+18h]
   mov     [esp], eax
   call    memset
   jmp     locret_80D93D6
loc_80D8C76:
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+90h]
   mov     [ebp-0ACh], eax
   lea     eax, [ebp-68h]
   mov     [esp], eax
   call    VectorClear871
   lea     eax, [ebp-98h]
   mov     [esp], eax
   call    VectorClear871
   lea     eax, [ebp-0A8h]
   mov     [esp+4], eax
   mov     eax, [ebp+14h]
   add     eax, 3E8h
   mov     [esp], eax
   call    VectorCopy439
   mov     eax, [ebp+14h]
   mov     eax, [eax+380h]
   mov     [ebp-64h], eax
   mov     eax, [ebp+14h]
   mov     eax, [eax+3B0h]
   mov     [ebp-94h], eax
   mov     dword [esp+8], 0
   mov     dword [esp+4], 3
   mov     eax, [ebp+14h]
   mov     [esp], eax
   call    BG_GetConditionValue
   and     eax, 0C0000h
   test    eax, eax
   jnz     loc_80D8D64
   mov     eax, [ebp+14h]
   mov     eax, [eax+3B8h]
   mov     [ebp-98h], eax
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+8]
   and     eax, 8
   test    eax, eax
   jz      loc_80D8D64
   mov     eax, [ebp-98h]
   mov     [esp], eax
   call    AngleNormalize180
   fstp    dword [ebp-98h]
   fld     dword [ebp-98h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80D8D3A
   jmp     loc_80D8D50
loc_80D8D3A:
   fld     dword [ebp-98h]
   fld     dword [flt_814C0C4]
   fmulp   st1, st0
   fstp    dword [ebp-98h]
   jmp     loc_80D8D64
loc_80D8D50:
   fld     dword [ebp-98h]
   fld     dword [flt_814C0C8]
   fmulp   st1, st0
   fstp    dword [ebp-98h]
loc_80D8D64:
   lea     eax, [ebp-0A8h]
   mov     [esp+8], eax
   lea     eax, [ebp-98h]
   mov     [esp+4], eax
   lea     eax, [ebp-0A8h]
   mov     [esp], eax
   call    sub_80A6062
   lea     eax, [ebp-98h]
   mov     [esp+8], eax
   lea     eax, [ebp-68h]
   mov     [esp+4], eax
   lea     eax, [ebp-98h]
   mov     [esp], eax
   call    sub_80A6062
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+0E4h]
   mov     [esp+0Ch], eax
   mov     eax, 0
   mov     [esp+8], eax
   mov     eax, 0
   mov     [esp+4], eax
   lea     eax, [ebp-78h]
   mov     [esp], eax
   call    sub_80D9D06
   mov     eax, [ebp+14h]
   mov     eax, [eax+3E4h]
   mov     [esp], eax
   call    GetLeanFraction
   fstp    dword [ebp-7Ch]
   fld     dword [ebp-7Ch]
   fld     dword [flt_814C0CC]
   fmulp   st1, st0
   fld     dword [flt_814C0D0]
   fmulp   st1, st0
   fstp    dword [ebp-90h]
   fld     dword [ebp-7Ch]
   fld     dword [flt_814C0CC]
   fmulp   st1, st0
   fld     dword [flt_814C0D0]
   fmulp   st1, st0
   fstp    dword [ebp-0A0h]
   fld     dword [ebp-7Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80D8E2A
   jp      loc_80D8E2A
   jmp     loc_80D8EE1
loc_80D8E2A:
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+8]
   and     eax, 4
   test    eax, eax
   jz      loc_80D8E8D
   fld     dword [ebp-7Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80D8E47
   jmp     loc_80D8E6A
loc_80D8E47:
   mov     eax, [ebp-7Ch]
   mov     [ebp-0CCh], eax
   fld     dword [ebp-0CCh]
   fchs    
   fld     dword [flt_814C0D4]
   fmulp   st1, st0
   fld     dword [ebp-74h]
   faddp   st1, st0
   fstp    dword [ebp-74h]
   jmp     loc_80D8EE1
loc_80D8E6A:
   mov     eax, [ebp-7Ch]
   mov     [ebp-0CCh], eax
   fld     dword [ebp-0CCh]
   fchs    
   fld     dword [flt_814C0D8]
   fmulp   st1, st0
   fld     dword [ebp-74h]
   faddp   st1, st0
   fstp    dword [ebp-74h]
   jmp     loc_80D8EE1
loc_80D8E8D:
   fld     dword [ebp-7Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80D8E9D
   jmp     loc_80D8EC0
loc_80D8E9D:
   mov     eax, [ebp-7Ch]
   mov     [ebp-0CCh], eax
   fld     dword [ebp-0CCh]
   fchs    
   fld     dword [flt_814C0D4]
   fmulp   st1, st0
   fld     dword [ebp-74h]
   faddp   st1, st0
   fstp    dword [ebp-74h]
   jmp     loc_80D8EE1
loc_80D8EC0:
   mov     eax, [ebp-7Ch]
   mov     [ebp-0CCh], eax
   fld     dword [ebp-0CCh]
   fchs    
   fld     dword [flt_814C0DC]
   fmulp   st1, st0
   fld     dword [ebp-74h]
   faddp   st1, st0
   fstp    dword [ebp-74h]
loc_80D8EE1:
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+8]
   and     eax, 20000h
   test    eax, eax
   jnz     loc_80D8F0B
   mov     eax, [ebp+14h]
   mov     eax, [eax+3ECh]
   mov     [esp+4], eax
   mov     eax, [ebp-64h]
   mov     [esp], eax
   call    AngleSubtract
   fstp    dword [ebp-64h]
loc_80D8F0B:
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+8]
   and     eax, 8
   test    eax, eax
   jz      loc_80D90D8
   fld     dword [ebp-7Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80D8F2E
   jp      loc_80D8F2E
   jmp     loc_80D8F42
loc_80D8F2E:
   fld     dword [ebp-0A0h]
   fld     dword [flt_814C0C4]
   fmulp   st1, st0
   fstp    dword [ebp-0A0h]
loc_80D8F42:
   mov     eax, [ebp+0Ch]
   fld     dword [ebp-68h]
   fadd    dword [eax+0E8h]
   fstp    dword [ebp-68h]
   lea     eax, [ebp-0B4h]
   mov     [esp+8], eax
   lea     eax, [ebp-0B0h]
   mov     [esp+4], eax
   fld     dword [ebp-94h]
   fld     qword [dbl_814C0E0]
   fmulp   st1, st0
   fstp    dword [esp]
   call    sub_80D9CAE
   fld1    
   fsub    dword [ebp-0B4h]
   fld     dword [flt_814C0E8]
   fmulp   st1, st0
   fld     dword [ebp-78h]
   faddp   st1, st0
   fstp    dword [ebp-78h]
   fld     dword [ebp-0B0h]
   fld     dword [flt_814C0EC]
   fmulp   st1, st0
   fld     dword [ebp-74h]
   faddp   st1, st0
   fstp    dword [ebp-74h]
   fld     dword [ebp-7Ch]
   fmul    dword [ebp-0B0h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80D8FBF
   jmp     loc_80D8FEA
loc_80D8FBF:
   mov     eax, [ebp-7Ch]
   mov     [ebp-0CCh], eax
   fld     dword [ebp-0CCh]
   fchs    
   fld1    
   fsub    dword [ebp-0B4h]
   fmulp   st1, st0
   fld     dword [flt_814C0F0]
   fmulp   st1, st0
   fld     dword [ebp-74h]
   faddp   st1, st0
   fstp    dword [ebp-74h]
loc_80D8FEA:
   mov     eax, 0
   mov     [ebp-58h], eax
   fld     dword [ebp-90h]
   fld     dword [flt_814C0F4]
   fmulp   st1, st0
   fstp    dword [ebp-54h]
   fld     dword [ebp-90h]
   fld     dword [flt_814C0F8]
   fmulp   st1, st0
   fstp    dword [ebp-50h]
   mov     eax, [ebp+0Ch]
   fld     dword [eax+0E8h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80D9042
   jp      loc_80D9042
   mov     eax, [ebp+0Ch]
   fld     dword [eax+0ECh]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80D9042
   jp      loc_80D9042
   jmp     loc_80D9068
loc_80D9042:
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+0ECh]
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+0E8h]
   mov     [esp], eax
   call    AngleSubtract
   fld     dword [ebp-58h]
   faddp   st1, st0
   fstp    dword [ebp-58h]
loc_80D9068:
   mov     eax, 0
   mov     [ebp-4Ch], eax
   fld     dword [ebp-94h]
   fld     dword [flt_814C0FC]
   fmulp   st1, st0
   fld     dword [ebp-90h]
   fld     dword [flt_814C100]
   fmulp   st1, st0
   fsubp   st1, st0
   fstp    dword [ebp-48h]
   fld     dword [ebp-90h]
   fld     dword [flt_814C100]
   fmulp   st1, st0
   fstp    dword [ebp-44h]
   mov     eax, [ebp-98h]
   mov     [ebp-40h], eax
   fld     dword [ebp-94h]
   fld     dword [flt_814C104]
   fmulp   st1, st0
   fadd    dword [ebp-90h]
   fstp    dword [ebp-3Ch]
   fld     dword [ebp-90h]
   fld     dword [flt_814C108]
   fmulp   st1, st0
   fstp    dword [ebp-38h]
   jmp     loc_80D92A0
loc_80D90D8:
   fld     dword [ebp-7Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80D90ED
   jp      loc_80D90ED
   jmp     loc_80D9198
loc_80D90ED:
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+8]
   and     eax, 4
   test    eax, eax
   jz      loc_80D9136
   fld     dword [ebp-7Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80D9198
   fld     dword [ebp-90h]
   fld     dword [flt_814C10C]
   fmulp   st1, st0
   fstp    dword [ebp-90h]
   fld     dword [ebp-0A0h]
   fld     dword [flt_814C10C]
   fmulp   st1, st0
   fstp    dword [ebp-0A0h]
   jmp     loc_80D9198
loc_80D9136:
   fld     dword [ebp-7Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80D9146
   jmp     loc_80D9170
loc_80D9146:
   fld     dword [ebp-90h]
   fld     dword [flt_814C10C]
   fmulp   st1, st0
   fstp    dword [ebp-90h]
   fld     dword [ebp-0A0h]
   fld     dword [flt_814C10C]
   fmulp   st1, st0
   fstp    dword [ebp-0A0h]
   jmp     loc_80D9198
loc_80D9170:
   fld     dword [ebp-90h]
   fld     dword [flt_814C10C]
   fmulp   st1, st0
   fstp    dword [ebp-90h]
   fld     dword [ebp-0A0h]
   fld     dword [flt_814C10C]
   fmulp   st1, st0
   fstp    dword [ebp-0A0h]
loc_80D9198:
   fld     dword [ebp-7Ch]
   fld     dword [flt_814C0CC]
   fmulp   st1, st0
   fld     dword [flt_814C110]
   fmulp   st1, st0
   fld     dword [ebp-60h]
   faddp   st1, st0
   fstp    dword [ebp-60h]
   fld     dword [ebp-98h]
   fld     dword [flt_814C100]
   fmulp   st1, st0
   fstp    dword [ebp-58h]
   fld     dword [ebp-94h]
   fld     dword [flt_814C114]
   fmulp   st1, st0
   fstp    dword [ebp-54h]
   fld     dword [ebp-90h]
   fld     dword [flt_814C0C4]
   fmulp   st1, st0
   fstp    dword [ebp-50h]
   mov     eax, [ebp+0Ch]
   fld     dword [eax+0E8h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80D9214
   jp      loc_80D9214
   mov     eax, [ebp+0Ch]
   fld     dword [eax+0ECh]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80D9214
   jp      loc_80D9214
   jmp     loc_80D923A
loc_80D9214:
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+0ECh]
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+0E8h]
   mov     [esp], eax
   call    AngleSubtract
   fld     dword [ebp-58h]
   faddp   st1, st0
   fstp    dword [ebp-58h]
loc_80D923A:
   fld     dword [ebp-98h]
   fld     dword [flt_814C0F8]
   fmulp   st1, st0
   fstp    dword [ebp-4Ch]
   fld     dword [ebp-94h]
   fld     dword [flt_814C114]
   fmulp   st1, st0
   fstp    dword [ebp-48h]
   fld     dword [ebp-90h]
   fld     dword [flt_814C0C4]
   fmulp   st1, st0
   fstp    dword [ebp-44h]
   fld     dword [ebp-98h]
   fld     dword [flt_814C0C4]
   fmulp   st1, st0
   fstp    dword [ebp-40h]
   fld     dword [ebp-94h]
   fld     dword [flt_814C100]
   fmulp   st1, st0
   fstp    dword [ebp-3Ch]
   fld     dword [ebp-90h]
   fld     dword [flt_814C118]
   fmulp   st1, st0
   fstp    dword [ebp-38h]
loc_80D92A0:
   fld     dword [ebp-0A8h]
   fld     dword [flt_814C0F8]
   fmulp   st1, st0
   fstp    dword [ebp-34h]
   fld     dword [ebp-0A4h]
   fld     dword [flt_814C0F8]
   fmulp   st1, st0
   fstp    dword [ebp-30h]
   mov     eax, 0
   mov     [ebp-2Ch], eax
   fld     dword [ebp-0A8h]
   fld     dword [flt_814C11C]
   fmulp   st1, st0
   fstp    dword [ebp-28h]
   fld     dword [ebp-0A4h]
   fld     dword [flt_814C11C]
   fmulp   st1, st0
   fstp    dword [ebp-24h]
   fld     dword [ebp-0A0h]
   fld     dword [flt_814C120]
   fmulp   st1, st0
   fstp    dword [ebp-20h]
   lea     eax, [ebp-58h]
   add     eax, 3Ch
   mov     [esp], eax
   call    VectorClear871
   mov     eax, [ebp+0Ch]
   fld     dword [eax+0ECh]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80D9339
   jp      loc_80D9339
   mov     eax, [ebp+0Ch]
   fld     dword [eax+0E8h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80D9339
   jp      loc_80D9339
   jmp     loc_80D935A
loc_80D9339:
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+0E8h]
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+0ECh]
   mov     [esp], eax
   call    AngleSubtract
   fstp    dword [ebp-1Ch]
loc_80D935A:
   mov     dword [ebp-0B8h], 0
loc_80D9364:
   cmp     dword [ebp-0B8h], 5
   jle     loc_80D936F
   jmp     loc_80D93AC
loc_80D936F:
   mov     edx, [ebp-0B8h]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   add     eax, [ebp+18h]
   mov     [esp+4], eax
   lea     ecx, [ebp-58h]
   mov     edx, [ebp-0B8h]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   lea     eax, [eax+ecx]
   mov     [esp], eax
   call    VectorCopy439
   lea     eax, [ebp-0B8h]
   inc     dword [eax]
   jmp     loc_80D9364
loc_80D93AC:
   mov     eax, [ebp+18h]
   add     eax, 48h
   mov     [esp+4], eax
   lea     eax, [ebp-68h]
   mov     [esp], eax
   call    VectorCopy439
   mov     eax, [ebp+18h]
   add     eax, 54h
   mov     [esp+4], eax
   lea     eax, [ebp-78h]
   mov     [esp], eax
   call    VectorCopy439
locret_80D93D6:
   leave   
   retn    


section .rdata
flt_814C120     dd -0.30000001
flt_814C0F8     dd 0.30000001
flt_814C11C     dd 0.69999999
flt_814C118     dd -0.60000002
flt_814C114     dd 0.40000001
flt_814C0C4     dd 0.5
flt_814C100     dd 0.2
flt_814C10C     dd 1.25
flt_8146080     dd 182.04445
flt_814C108     dd -0.2
flt_814C0E8     dd -24.0
flt_8146084     dd 0.0054931641
flt_814C0EC     dd -12.0
flt_814C0D4     dd 2.5
flt_814607C     dd -180.0
flt_814C110     dd 0.075000003
flt_814C0CC     dd 50.0
flt_814C0F0     dd 16.0
flt_8146088     dd 180.0
flt_814C0FC     dd 0.1
flt_814C0C8     dd 0.25
flt_814608C     dd 360.0
flt_8146074     dd 180.0
flt_814C0D8     dd 12.5
flt_814C0F4     dd -1.2
flt_8146078     dd 360.0
flt_814C104     dd 0.80000001
flt_814C0D0     dd 0.92500001
flt_814C0DC     dd 5.0

dbl_814C0E0     dq 0.0174532925199433