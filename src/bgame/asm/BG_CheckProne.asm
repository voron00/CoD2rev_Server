extern sin
extern cos
extern sqrt
extern atan2
extern pmoveHandlers


global BG_CheckProne


section .text


sub_80DE9D4:
   push    ebp
   mov     ebp, esp
   mov     ecx, [ebp+10h]
   mov     eax, [ebp+8]
   mov     edx, [ebp+0Ch]
   fld     dword [eax]
   fadd    dword [edx]
   fstp    dword [ecx]
   mov     ecx, [ebp+10h]
   add     ecx, 4
   mov     eax, [ebp+8]
   add     eax, 4
   mov     edx, [ebp+0Ch]
   add     edx, 4
   fld     dword [eax]
   fadd    dword [edx]
   fstp    dword [ecx]
   mov     ecx, [ebp+10h]
   add     ecx, 8
   mov     eax, [ebp+8]
   add     eax, 8
   mov     edx, [ebp+0Ch]
   add     edx, 8
   fld     dword [eax]
   fadd    dword [edx]
   fstp    dword [ecx]
   pop     ebp
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


sub_80A8D26:
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


sub_80A8C1C:
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


AngleVectors:
   push    ebp
   mov     ebp, esp
   sub     esp, 38h
   mov     eax, [ebp+8]
   add     eax, 4
   fld     dword [eax]
   fld     qword [dbl_8145F88]
   fmulp   st1, st0
   fstp    dword [ebp-4]
   lea     eax, [ebp-1Ch]
   mov     [esp+8], eax
   lea     eax, [ebp-10h]
   mov     [esp+4], eax
   mov     eax, [ebp-4]
   mov     [esp], eax
   call    sub_80A8C1C
   mov     eax, [ebp+8]
   fld     dword [eax]
   fld     qword [dbl_8145F88]
   fmulp   st1, st0
   fstp    dword [ebp-4]
   lea     eax, [ebp-18h]
   mov     [esp+8], eax
   lea     eax, [ebp-0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp-4]
   mov     [esp], eax
   call    sub_80A8C1C
   cmp     dword [ebp+0Ch], 0
   jz      loc_80A2F4C
   mov     eax, [ebp+0Ch]
   fld     dword [ebp-18h]
   fmul    dword [ebp-1Ch]
   fstp    dword [eax]
   mov     eax, [ebp+0Ch]
   add     eax, 4
   fld     dword [ebp-18h]
   fmul    dword [ebp-10h]
   fstp    dword [eax]
   mov     edx, [ebp+0Ch]
   add     edx, 8
   mov     eax, [ebp-0Ch]
   xor     eax, 80000000h
   mov     [edx], eax
loc_80A2F4C:
   cmp     dword [ebp+10h], 0
   jnz     loc_80A2F5D
   cmp     dword [ebp+14h], 0
   jnz     loc_80A2F5D
   jmp     locret_80A3037
loc_80A2F5D:
   mov     eax, [ebp+8]
   add     eax, 8
   fld     dword [eax]
   fld     qword [dbl_8145F88]
   fmulp   st1, st0
   fstp    dword [ebp-4]
   lea     eax, [ebp-14h]
   mov     [esp+8], eax
   lea     eax, [ebp-8]
   mov     [esp+4], eax
   mov     eax, [ebp-4]
   mov     [esp], eax
   call    sub_80A8C1C
   cmp     dword [ebp+10h], 0
   jz      loc_80A2FEC
   mov     edx, [ebp+10h]
   mov     eax, [ebp-8]
   mov     [ebp-24h], eax
   fld     dword [ebp-24h]
   fchs    
   fmul    dword [ebp-0Ch]
   fmul    dword [ebp-1Ch]
   fld     dword [ebp-14h]
   fmul    dword [ebp-10h]
   faddp   st1, st0
   fstp    dword [edx]
   mov     edx, [ebp+10h]
   add     edx, 4
   mov     eax, [ebp-8]
   mov     [ebp-24h], eax
   fld     dword [ebp-24h]
   fchs    
   fmul    dword [ebp-0Ch]
   fmul    dword [ebp-10h]
   mov     eax, [ebp-14h]
   mov     [ebp-24h], eax
   fld     dword [ebp-24h]
   fchs    
   fmul    dword [ebp-1Ch]
   faddp   st1, st0
   fstp    dword [edx]
   mov     edx, [ebp+10h]
   add     edx, 8
   mov     eax, [ebp-8]
   mov     [ebp-24h], eax
   fld     dword [ebp-24h]
   fchs    
   fmul    dword [ebp-18h]
   fstp    dword [edx]
loc_80A2FEC:
   cmp     dword [ebp+14h], 0
   jz      locret_80A3037
   mov     eax, [ebp+14h]
   fld     dword [ebp-14h]
   fmul    dword [ebp-0Ch]
   fmul    dword [ebp-1Ch]
   fld     dword [ebp-8]
   fmul    dword [ebp-10h]
   faddp   st1, st0
   fstp    dword [eax]
   mov     edx, [ebp+14h]
   add     edx, 4
   fld     dword [ebp-14h]
   fmul    dword [ebp-0Ch]
   fmul    dword [ebp-10h]
   mov     eax, [ebp-8]
   mov     [ebp-24h], eax
   fld     dword [ebp-24h]
   fchs    
   fmul    dword [ebp-1Ch]
   faddp   st1, st0
   fstp    dword [edx]
   mov     eax, [ebp+14h]
   add     eax, 8
   fld     dword [ebp-14h]
   fmul    dword [ebp-18h]
   fstp    dword [eax]
locret_80A3037:
   leave   
   retn    


Vec3NormalizeTo:
   push    ebp
   mov     ebp, esp
   sub     esp, 18h
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
   fstp    dword [ebp-4]
   mov     eax, [ebp-4]
   mov     [esp], eax
   call    sub_80A8B5A
   fstp    dword [ebp-4]
   fld     dword [ebp-4]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80A22EF
   jp      loc_80A22EF
   jmp     loc_80A232C
loc_80A22EF:
   fld1    
   fdiv    dword [ebp-4]
   fstp    dword [ebp-8]
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp+8]
   fld     dword [eax]
   fmul    dword [ebp-8]
   fstp    dword [edx]
   mov     edx, [ebp+0Ch]
   add     edx, 4
   mov     eax, [ebp+8]
   add     eax, 4
   fld     dword [eax]
   fmul    dword [ebp-8]
   fstp    dword [edx]
   mov     edx, [ebp+0Ch]
   add     edx, 8
   mov     eax, [ebp+8]
   add     eax, 8
   fld     dword [eax]
   fmul    dword [ebp-8]
   fstp    dword [edx]
   jmp     loc_80A2337
loc_80A232C:
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    sub_80A8D26
loc_80A2337:
   mov     eax, [ebp-4]
   mov     [ebp-0Ch], eax
   fld     dword [ebp-0Ch]
   leave   
   retn    


sub_80A8B5A:
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


sub_80DEA94:
   push    ebp
   mov     ebp, esp
   mov     ecx, [ebp+14h]
   mov     edx, [ebp+8]
   mov     eax, [ebp+10h]
   fld     dword [ebp+0Ch]
   fmul    dword [eax]
   fld     dword [edx]
   faddp   st1, st0
   fstp    dword [ecx]
   mov     ecx, [ebp+14h]
   add     ecx, 4
   mov     edx, [ebp+8]
   add     edx, 4
   mov     eax, [ebp+10h]
   add     eax, 4
   fld     dword [ebp+0Ch]
   fmul    dword [eax]
   fld     dword [edx]
   faddp   st1, st0
   fstp    dword [ecx]
   mov     ecx, [ebp+14h]
   add     ecx, 8
   mov     edx, [ebp+8]
   add     edx, 8
   mov     eax, [ebp+10h]
   add     eax, 8
   fld     dword [ebp+0Ch]
   fmul    dword [eax]
   fld     dword [edx]
   faddp   st1, st0
   fstp    dword [ecx]
   pop     ebp
   retn    


Vec3Normalize:
   push    ebp
   mov     ebp, esp
   sub     esp, 18h
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
   fstp    dword [ebp-4]
   mov     eax, [ebp-4]
   mov     [esp], eax
   call    sub_80A8B5A
   fstp    dword [ebp-4]
   fld     dword [ebp-4]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80A2117
   jp      loc_80A2117
   jmp     loc_80A2152
loc_80A2117:
   fld1    
   fdiv    dword [ebp-4]
   fstp    dword [ebp-8]
   mov     edx, [ebp+8]
   mov     eax, [ebp+8]
   fld     dword [eax]
   fmul    dword [ebp-8]
   fstp    dword [edx]
   mov     edx, [ebp+8]
   add     edx, 4
   mov     eax, [ebp+8]
   add     eax, 4
   fld     dword [eax]
   fmul    dword [ebp-8]
   fstp    dword [edx]
   mov     edx, [ebp+8]
   add     edx, 8
   mov     eax, [ebp+8]
   add     eax, 8
   fld     dword [eax]
   fmul    dword [ebp-8]
   fstp    dword [edx]
loc_80A2152:
   mov     eax, [ebp-4]
   mov     [ebp-0Ch], eax
   fld     dword [ebp-0Ch]
   leave   
   retn    


Vec3Lerp:
   push    ebp
   mov     ebp, esp
   push    ebx
   mov     ebx, [ebp+14h]
   mov     ecx, [ebp+8]
   mov     eax, [ebp+0Ch]
   mov     edx, [ebp+8]
   fld     dword [eax]
   fsub    dword [edx]
   fmul    dword [ebp+10h]
   fld     dword [ecx]
   faddp   st1, st0
   fstp    dword [ebx]
   mov     ebx, [ebp+14h]
   add     ebx, 4
   mov     ecx, [ebp+8]
   add     ecx, 4
   mov     eax, [ebp+0Ch]
   add     eax, 4
   mov     edx, [ebp+8]
   add     edx, 4
   fld     dword [eax]
   fsub    dword [edx]
   fmul    dword [ebp+10h]
   fld     dword [ecx]
   faddp   st1, st0
   fstp    dword [ebx]
   mov     ebx, [ebp+14h]
   add     ebx, 8
   mov     ecx, [ebp+8]
   add     ecx, 8
   mov     eax, [ebp+0Ch]
   add     eax, 8
   mov     edx, [ebp+8]
   add     edx, 8
   fld     dword [eax]
   fsub    dword [edx]
   fmul    dword [ebp+10h]
   fld     dword [ecx]
   faddp   st1, st0
   fstp    dword [ebx]
   pop     ebx
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


sub_80DE942:
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


VectorCopy428:
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


sub_80A2AF6:
   push    ebp
   mov     ebp, esp
   sub     esp, 28h
   mov     eax, [ebp+8]
   add     eax, 4
   fld     dword [eax]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jp      loc_80A2B11
   jz      loc_80A2B13
loc_80A2B11:
   jmp     loc_80A2B50
loc_80A2B13:
   mov     eax, [ebp+8]
   fld     dword [eax]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jp      loc_80A2B25
   jz      loc_80A2B27
loc_80A2B25:
   jmp     loc_80A2B50
loc_80A2B27:
   mov     eax, [ebp+8]
   add     eax, 8
   fld     dword [eax]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80A2B3C
   jmp     loc_80A2B46
loc_80A2B3C:
   mov     eax, 43870000h
   mov     [ebp-8], eax
   jmp     loc_80A2BBD
loc_80A2B46:
   mov     eax, 42B40000h
   mov     [ebp-8], eax
   jmp     loc_80A2BBD
loc_80A2B50:
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
   fstp    dword [esp]
   call    sub_80A8B5A
   fstp    dword [ebp-4]
   fld     dword [ebp-4]
   fstp    qword [esp+8]
   mov     eax, [ebp+8]
   add     eax, 8
   fld     dword [eax]
   fstp    qword [esp]
   call    atan2
   fld     qword [dbl_8145F28]
   fmulp   st1, st0
   fld     qword [dbl_8145F30]
   fdivp   st1, st0
   fstp    dword [ebp-8]
   fld     dword [ebp-8]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80A2BAF
   jmp     loc_80A2BBD
loc_80A2BAF:
   fld     dword [ebp-8]
   fld     dword [flt_8145F38]
   faddp   st1, st0
   fstp    dword [ebp-8]
loc_80A2BBD:
   mov     eax, [ebp-8]
   mov     [ebp-10h], eax
   fld     dword [ebp-10h]
   leave   
   retn    


sub_80DEA18:
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


BG_CheckProneValid:
   push    ebp
   mov     ebp, esp
   sub     esp, 148h
   mov     eax, [ebp+34h]
   mov     [ebp-9], al
   mov     dword [ebp-114h], 0
   mov     byte [ebp-11Dh], 1
   movzx   edx, byte [ebp-9]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   mov     eax, dword [pmoveHandlers+eax]
   mov     [ebp-11Ch], eax
   mov     eax, 0
   mov     [esp+0Ch], eax
   mov     eax, [ebp+10h]
   xor     eax, 80000000h
   mov     [esp+8], eax
   mov     eax, [ebp+10h]
   xor     eax, 80000000h
   mov     [esp+4], eax
   lea     eax, [ebp-78h]
   mov     [esp], eax
   call    sub_80DE942
   lea     eax, [ebp-78h]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   lea     eax, [ebp-78h]
   mov     [esp], eax
   call    sub_80DE9D4
   mov     eax, [ebp+14h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+10h]
   mov     [esp+8], eax
   mov     eax, [ebp+10h]
   mov     [esp+4], eax
   lea     eax, [ebp-88h]
   mov     [esp], eax
   call    sub_80DE942
   lea     eax, [ebp-88h]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   lea     eax, [ebp-88h]
   mov     [esp], eax
   call    sub_80DE9D4
   cmp     dword [ebp+38h], 0
   jnz     loc_80DDB9F
   mov     dword [ebp-10h], 810011h
   jmp     loc_80DDBA6
loc_80DDB9F:
   mov     dword [ebp-10h], 820011h
loc_80DDBA6:
   cmp     dword [ebp+28h], 0
   jnz     loc_80DDC81
   mov     eax, 0
   mov     [esp+0Ch], eax
   mov     eax, [ebp+10h]
   xor     eax, 80000000h
   mov     [esp+8], eax
   mov     eax, [ebp+10h]
   xor     eax, 80000000h
   mov     [esp+4], eax
   lea     eax, [ebp-78h]
   mov     [esp], eax
   call    sub_80DE942
   mov     eax, [ebp+14h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+10h]
   mov     [esp+8], eax
   mov     eax, [ebp+10h]
   mov     [esp+4], eax
   lea     eax, [ebp-88h]
   mov     [esp], eax
   call    sub_80DE942
   lea     eax, [ebp-58h]
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    VectorCopy428
   lea     eax, [ebp-68h]
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    VectorCopy428
   fld     dword [ebp-60h]
   fld     dword [flt_814DC8C]
   faddp   st1, st0
   fstp    dword [ebp-60h]
   mov     eax, [ebp-10h]
   mov     [esp+18h], eax
   mov     eax, [ebp+8]
   mov     [esp+14h], eax
   lea     eax, [ebp-68h]
   mov     [esp+10h], eax
   lea     eax, [ebp-88h]
   mov     [esp+0Ch], eax
   lea     eax, [ebp-78h]
   mov     [esp+8], eax
   lea     eax, [ebp-58h]
   mov     [esp+4], eax
   lea     eax, [ebp-48h]
   mov     [esp], eax
   mov     eax, [ebp-11Ch]
   call    eax
   cmp     byte [ebp-26h], 0
   jz      loc_80DDC81
   mov     dword [ebp-124h], 0
   jmp     loc_80DE72B
loc_80DDC81:
   cmp     dword [ebp+2Ch], 0
   jz      loc_80DDCB3
   cmp     dword [ebp+30h], 0
   jz      loc_80DDCB3
   mov     eax, [ebp+30h]
   add     eax, 8
   fld     dword [eax]
   fld     dword [flt_814DC90]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80DDCA4
   jmp     loc_80DDCB3
loc_80DDCA4:
   mov     dword [ebp-124h], 0
   jmp     loc_80DE72B
loc_80DDCB3:
   mov     eax, 0C0C00000h
   mov     [esp+0Ch], eax
   mov     eax, 0C0C00000h
   mov     [esp+8], eax
   mov     eax, 0C0C00000h
   mov     [esp+4], eax
   lea     eax, [ebp-78h]
   mov     [esp], eax
   call    sub_80DE942
   mov     eax, 40C00000h
   mov     [esp+0Ch], eax
   mov     eax, 40C00000h
   mov     [esp+8], eax
   mov     eax, 40C00000h
   mov     [esp+4], eax
   lea     eax, [ebp-88h]
   mov     [esp], eax
   call    sub_80DE942
   mov     eax, 0
   mov     [esp+0Ch], eax
   fld     dword [ebp+18h]
   fld     dword [flt_814DC98]
   fsubp   st1, st0
   fstp    dword [esp+8]
   mov     eax, 0
   mov     [esp+4], eax
   lea     eax, [ebp-68h]
   mov     [esp], eax
   call    sub_80DE942
   lea     eax, [ebp-0C8h]
   mov     [esp+0Ch], eax
   lea     eax, [ebp-0B8h]
   mov     [esp+8], eax
   lea     eax, [ebp-0A8h]
   mov     [esp+4], eax
   lea     eax, [ebp-68h]
   mov     [esp], eax
   call    AngleVectors
   fld     dword [ebp+14h]
   fld     dword [flt_814DC94]
   fsubp   st1, st0
   fstp    dword [ebp-108h]
   fld     dword [ebp-108h]
   fld     dword [flt_814DC94]
   fsubp   st1, st0
   fstp    dword [ebp-10Ch]
   lea     eax, [ebp-58h]
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    VectorCopy428
   fld     dword [ebp-50h]
   fadd    dword [ebp-108h]
   fstp    dword [ebp-50h]
   lea     eax, [ebp-68h]
   mov     [esp+0Ch], eax
   lea     eax, [ebp-0A8h]
   mov     [esp+8], eax
   fld     dword [ebp+3Ch]
   fld     dword [flt_814DC94]
   fsubp   st1, st0
   fstp    dword [esp+4]
   lea     eax, [ebp-58h]
   mov     [esp], eax
   call    sub_80DEA94
   mov     eax, [ebp-10h]
   mov     [esp+18h], eax
   mov     eax, [ebp+8]
   mov     [esp+14h], eax
   lea     eax, [ebp-68h]
   mov     [esp+10h], eax
   lea     eax, [ebp-88h]
   mov     [esp+0Ch], eax
   lea     eax, [ebp-78h]
   mov     [esp+8], eax
   lea     eax, [ebp-58h]
   mov     [esp+4], eax
   lea     eax, [ebp-48h]
   mov     [esp], eax
   mov     eax, [ebp-11Ch]
   call    eax
   fld     dword [ebp-48h]
   fld1    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80DDE11
   jmp     loc_80DDF9E
loc_80DDE11:
   cmp     dword [ebp+2Ch], 0
   jnz     loc_80DDE26
   mov     dword [ebp-124h], 0
   jmp     loc_80DE72B
loc_80DDE26:
   mov     dword [ebp-114h], 1
   fld     dword [ebp+3Ch]
   fld     dword [flt_814DC94]
   fsubp   st1, st0
   fmul    dword [ebp-48h]
   fld     dword [flt_814DC94]
   faddp   st1, st0
   fstp    dword [ebp-110h]
   fld     dword [ebp+10h]
   fld     dword [flt_814DC9C]
   faddp   st1, st0
   fld     dword [ebp-110h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80DDE68
   jmp     loc_80DDE77
loc_80DDE68:
   mov     dword [ebp-124h], 0
   jmp     loc_80DE72B
loc_80DDE77:
   fld     dword [ebp-108h]
   fld     dword [flt_814DC90]
   fmulp   st1, st0
   fld     dword [flt_814DCA0]
   faddp   st1, st0
   fld     dword [ebp-110h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80DDEA1
   jmp     loc_80DDFA7
loc_80DDEA1:
   mov     dword [ebp-114h], 0
   fld     dword [ebp-60h]
   fld     dword [flt_814DCA4]
   faddp   st1, st0
   fstp    dword [ebp-60h]
   lea     eax, [ebp-98h]
   mov     [esp+8], eax
   lea     eax, [ebp-58h]
   mov     [esp+4], eax
   lea     eax, [ebp-68h]
   mov     [esp], eax
   call    sub_80DEA18
   lea     eax, [ebp-0A8h]
   mov     [esp+4], eax
   lea     eax, [ebp-98h]
   mov     [esp], eax
   call    Vec3NormalizeTo
   fstp    dword [ebp-104h]
   mov     eax, [ebp-10h]
   mov     [esp+18h], eax
   mov     eax, [ebp+8]
   mov     [esp+14h], eax
   lea     eax, [ebp-68h]
   mov     [esp+10h], eax
   lea     eax, [ebp-88h]
   mov     [esp+0Ch], eax
   lea     eax, [ebp-78h]
   mov     [esp+8], eax
   lea     eax, [ebp-58h]
   mov     [esp+4], eax
   lea     eax, [ebp-48h]
   mov     [esp], eax
   mov     eax, [ebp-11Ch]
   call    eax
   fld     dword [ebp-48h]
   fld1    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80DDF3C
   jmp     loc_80DDF93
loc_80DDF3C:
   mov     dword [ebp-114h], 1
   fld     dword [ebp-48h]
   fmul    dword [ebp-104h]
   fld     dword [flt_814DC94]
   faddp   st1, st0
   fstp    dword [ebp-110h]
   fld     dword [ebp-108h]
   fld     dword [flt_814DC90]
   fmulp   st1, st0
   fld     dword [flt_814DCA0]
   faddp   st1, st0
   fld     dword [ebp-110h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80DDF84
   jmp     loc_80DDFA7
loc_80DDF84:
   mov     dword [ebp-124h], 0
   jmp     loc_80DE72B
loc_80DDF93:
   mov     eax, [ebp+3Ch]
   mov     [ebp-110h], eax
   jmp     loc_80DDFA7
loc_80DDF9E:
   mov     eax, [ebp+3Ch]
   mov     [ebp-110h], eax
loc_80DDFA7:
   lea     eax, [ebp-0F8h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp-48h]
   mov     [esp+8], eax
   lea     eax, [ebp-68h]
   mov     [esp+4], eax
   lea     eax, [ebp-58h]
   mov     [esp], eax
   call    Vec3Lerp
   lea     eax, [ebp-58h]
   mov     [esp+0Ch], eax
   lea     eax, [ebp-0A8h]
   mov     [esp+8], eax
   mov     eax, 42400000h
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    sub_80DEA94
   fld     dword [ebp-50h]
   fadd    dword [ebp-108h]
   fstp    dword [ebp-50h]
   lea     eax, [ebp-68h]
   mov     [esp+4], eax
   lea     eax, [ebp-58h]
   mov     [esp], eax
   call    VectorCopy428
   fld     dword [ebp+10h]
   fld     dword [flt_814DCA8]
   fmulp   st1, st0
   fadd    dword [ebp-108h]
   fld     dword [flt_814DC94]
   fsubp   st1, st0
   fld     dword [ebp-60h]
   fsubrp  st1, st0
   fstp    dword [ebp-60h]
   mov     eax, [ebp-10h]
   mov     [esp+18h], eax
   mov     eax, [ebp+8]
   mov     [esp+14h], eax
   lea     eax, [ebp-68h]
   mov     [esp+10h], eax
   lea     eax, [ebp-88h]
   mov     [esp+0Ch], eax
   lea     eax, [ebp-78h]
   mov     [esp+8], eax
   lea     eax, [ebp-58h]
   mov     [esp+4], eax
   lea     eax, [ebp-48h]
   mov     [esp], eax
   mov     eax, [ebp-11Ch]
   call    eax
   fld     dword [ebp-48h]
   fld1    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jp      loc_80DE07D
   jz      loc_80DE6DF
loc_80DE07D:
   fld     dword [ebp-3Ch]
   fld     dword [flt_814DC90]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80DE08F
   jmp     loc_80DE09E
loc_80DE08F:
   mov     dword [ebp-124h], 0
   jmp     loc_80DE72B
loc_80DE09E:
   fld     dword [ebp+10h]
   fld     dword [flt_814DCA8]
   fmulp   st1, st0
   fadd    dword [ebp-108h]
   fld     dword [flt_814DC94]
   fsubp   st1, st0
   fmul    dword [ebp-48h]
   fld     dword [flt_814DC94]
   faddp   st1, st0
   fstp    dword [ebp-118h]
   lea     eax, [ebp-0E8h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp-48h]
   mov     [esp+8], eax
   lea     eax, [ebp-68h]
   mov     [esp+4], eax
   lea     eax, [ebp-58h]
   mov     [esp], eax
   call    Vec3Lerp
   fld     dword [ebp-0E0h]
   fld     dword [flt_814DC94]
   fsubp   st1, st0
   fstp    dword [ebp-0E0h]
   cmp     dword [ebp-114h], 0
   jz      loc_80DE316
   fld     dword [ebp-110h]
   fsub    dword [ebp-118h]
   fld     dword [ebp-118h]
   fld     dword [flt_814DCAC]
   fmulp   st1, st0
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80DE6DF
   lea     eax, [ebp-98h]
   mov     [esp+8], eax
   lea     eax, [ebp-0E8h]
   mov     [esp+4], eax
   lea     eax, [ebp-0F8h]
   mov     [esp], eax
   call    sub_80DEA18
   lea     eax, [ebp-98h]
   mov     [esp+0Ch], eax
   lea     eax, [ebp-0A8h]
   mov     [esp+8], eax
   mov     eax, 40C00000h
   mov     [esp+4], eax
   lea     eax, [ebp-98h]
   mov     [esp], eax
   call    sub_80DEA94
   fld     dword [ebp-90h]
   fld     dword [flt_814DC94]
   faddp   st1, st0
   fstp    dword [ebp-90h]
   lea     eax, [ebp-98h]
   mov     [esp], eax
   call    Vec3Normalize
   fstp    st0
   lea     eax, [ebp-68h]
   mov     [esp+0Ch], eax
   lea     eax, [ebp-98h]
   mov     [esp+8], eax
   fld     dword [ebp+3Ch]
   fld     dword [flt_814DC94]
   fsubp   st1, st0
   fld     dword [flt_814DCA0]
   fsubp   st1, st0
   fstp    dword [esp+4]
   lea     eax, [ebp-58h]
   mov     [esp], eax
   call    sub_80DEA94
   mov     eax, [ebp+0Ch]
   fld     dword [ebp+3Ch]
   fld     dword [flt_814DC94]
   fsubp   st1, st0
   fmul    dword [ebp-0A8h]
   fadd    dword [eax]
   fadd    dword [ebp-68h]
   fld     dword [flt_814DCB0]
   fmulp   st1, st0
   fstp    dword [ebp-68h]
   mov     eax, [ebp+0Ch]
   add     eax, 4
   fld     dword [ebp+3Ch]
   fld     dword [flt_814DC94]
   fsubp   st1, st0
   fmul    dword [ebp-0A4h]
   fadd    dword [eax]
   fadd    dword [ebp-64h]
   fld     dword [flt_814DCB0]
   fmulp   st1, st0
   fstp    dword [ebp-64h]
   mov     eax, [ebp-10h]
   mov     [esp+18h], eax
   mov     eax, [ebp+8]
   mov     [esp+14h], eax
   lea     eax, [ebp-68h]
   mov     [esp+10h], eax
   lea     eax, [ebp-88h]
   mov     [esp+0Ch], eax
   lea     eax, [ebp-78h]
   mov     [esp+8], eax
   lea     eax, [ebp-58h]
   mov     [esp+4], eax
   lea     eax, [ebp-48h]
   mov     [esp], eax
   mov     eax, [ebp-11Ch]
   call    eax
   fld     dword [ebp-48h]
   fld1    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80DE26C
   jmp     loc_80DE2F3
loc_80DE26C:
   lea     eax, [ebp-58h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp-48h]
   mov     [esp+8], eax
   lea     eax, [ebp-68h]
   mov     [esp+4], eax
   lea     eax, [ebp-58h]
   mov     [esp], eax
   call    Vec3Lerp
   fld     dword [ebp-50h]
   fld     dword [flt_814DCB4]
   faddp   st1, st0
   fstp    dword [ebp-50h]
   fld     dword [ebp-60h]
   fld     dword [flt_814DCB4]
   faddp   st1, st0
   fstp    dword [ebp-60h]
   mov     eax, [ebp-10h]
   mov     [esp+18h], eax
   mov     eax, [ebp+8]
   mov     [esp+14h], eax
   lea     eax, [ebp-68h]
   mov     [esp+10h], eax
   lea     eax, [ebp-88h]
   mov     [esp+0Ch], eax
   lea     eax, [ebp-78h]
   mov     [esp+8], eax
   lea     eax, [ebp-58h]
   mov     [esp+4], eax
   lea     eax, [ebp-48h]
   mov     [esp], eax
   mov     eax, [ebp-11Ch]
   call    eax
   fld     dword [ebp-48h]
   fld1    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80DE6DF
loc_80DE2F3:
   lea     eax, [ebp-0F8h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp-48h]
   mov     [esp+8], eax
   lea     eax, [ebp-68h]
   mov     [esp+4], eax
   lea     eax, [ebp-58h]
   mov     [esp], eax
   call    Vec3Lerp
loc_80DE316:
   lea     eax, [ebp-58h]
   mov     [esp+4], eax
   lea     eax, [ebp-0F8h]
   mov     [esp], eax
   call    VectorCopy428
   lea     eax, [ebp-68h]
   mov     [esp+4], eax
   lea     eax, [ebp-58h]
   mov     [esp], eax
   call    VectorCopy428
   fld     dword [ebp-50h]
   fsub    dword [ebp-0E0h]
   fadd    st0, st0
   fadd    dword [ebp+10h]
   fld     dword [ebp-60h]
   fsubrp  st1, st0
   fstp    dword [ebp-60h]
   mov     eax, [ebp-10h]
   mov     [esp+18h], eax
   mov     eax, [ebp+8]
   mov     [esp+14h], eax
   lea     eax, [ebp-68h]
   mov     [esp+10h], eax
   lea     eax, [ebp-88h]
   mov     [esp+0Ch], eax
   lea     eax, [ebp-78h]
   mov     [esp+8], eax
   lea     eax, [ebp-58h]
   mov     [esp+4], eax
   lea     eax, [ebp-48h]
   mov     [esp], eax
   mov     eax, [ebp-11Ch]
   call    eax
   fld     dword [ebp-48h]
   fld1    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jp      loc_80DE3A2
   jz      loc_80DE6DF
loc_80DE3A2:
   fld     dword [ebp-3Ch]
   fld     dword [flt_814DC90]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80DE3B4
   jmp     loc_80DE3C3
loc_80DE3B4:
   mov     dword [ebp-124h], 0
   jmp     loc_80DE72B
loc_80DE3C3:
   lea     eax, [ebp-0F8h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp-48h]
   mov     [esp+8], eax
   lea     eax, [ebp-68h]
   mov     [esp+4], eax
   lea     eax, [ebp-58h]
   mov     [esp], eax
   call    Vec3Lerp
   fld     dword [ebp-0F0h]
   fld     dword [flt_814DC94]
   fsubp   st1, st0
   fstp    dword [ebp-0F0h]
   lea     eax, [ebp-0D8h]
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    VectorCopy428
   lea     eax, [ebp-98h]
   mov     [esp+8], eax
   lea     eax, [ebp-0D8h]
   mov     [esp+4], eax
   lea     eax, [ebp-0E8h]
   mov     [esp], eax
   call    sub_80DEA18
   lea     eax, [ebp-98h]
   mov     [esp], eax
   call    sub_80A2AF6
   fstp    dword [ebp-0FCh]
   lea     eax, [ebp-98h]
   mov     [esp+8], eax
   lea     eax, [ebp-0E8h]
   mov     [esp+4], eax
   lea     eax, [ebp-0F8h]
   mov     [esp], eax
   call    sub_80DEA18
   lea     eax, [ebp-98h]
   mov     [esp], eax
   call    sub_80A2AF6
   fstp    dword [ebp-100h]
   mov     eax, [ebp-0FCh]
   mov     [esp+4], eax
   mov     eax, [ebp-100h]
   mov     [esp], eax
   call    AngleSubtract
   fstp    dword [ebp-104h]
   fld     dword [ebp-104h]
   fld     dword [flt_814DCB8]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80DE4C3
   fld     dword [ebp-104h]
   fld     dword [flt_814DCBC]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80DE4C3
   jmp     loc_80DE4CA
loc_80DE4C3:
   mov     byte [ebp-11Dh], 0
loc_80DE4CA:
   mov     eax, 80000000h
   mov     [esp+0Ch], eax
   mov     eax, 80000000h
   mov     [esp+8], eax
   mov     eax, 80000000h
   mov     [esp+4], eax
   lea     eax, [ebp-78h]
   mov     [esp], eax
   call    sub_80DE942
   mov     eax, 0
   mov     [esp+0Ch], eax
   mov     eax, 0
   mov     [esp+8], eax
   mov     eax, 0
   mov     [esp+4], eax
   lea     eax, [ebp-88h]
   mov     [esp], eax
   call    sub_80DE942
   lea     eax, [ebp-58h]
   mov     [esp+4], eax
   lea     eax, [ebp-0D8h]
   mov     [esp], eax
   call    VectorCopy428
   fld     dword [ebp-50h]
   fld     dword [flt_814DCC0]
   faddp   st1, st0
   fstp    dword [ebp-50h]
   lea     eax, [ebp-68h]
   mov     [esp+4], eax
   lea     eax, [ebp-0E8h]
   mov     [esp], eax
   call    VectorCopy428
   fld     dword [ebp-60h]
   fld     dword [flt_814DCC0]
   faddp   st1, st0
   fstp    dword [ebp-60h]
   mov     eax, [ebp-10h]
   mov     [esp+18h], eax
   mov     eax, [ebp+8]
   mov     [esp+14h], eax
   lea     eax, [ebp-68h]
   mov     [esp+10h], eax
   lea     eax, [ebp-88h]
   mov     [esp+0Ch], eax
   lea     eax, [ebp-78h]
   mov     [esp+8], eax
   lea     eax, [ebp-58h]
   mov     [esp+4], eax
   lea     eax, [ebp-48h]
   mov     [esp], eax
   mov     eax, [ebp-11Ch]
   call    eax
   fld     dword [ebp-48h]
   fld1    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80DE5A8
   jmp     loc_80DE5AF
loc_80DE5A8:
   mov     byte [ebp-11Dh], 0
loc_80DE5AF:
   lea     eax, [ebp-58h]
   mov     [esp+4], eax
   lea     eax, [ebp-68h]
   mov     [esp], eax
   call    VectorCopy428
   lea     eax, [ebp-68h]
   mov     [esp+4], eax
   lea     eax, [ebp-0F8h]
   mov     [esp], eax
   call    VectorCopy428
   fld     dword [ebp-60h]
   fld     dword [flt_814DCC0]
   faddp   st1, st0
   fstp    dword [ebp-60h]
   mov     eax, [ebp-10h]
   mov     [esp+18h], eax
   mov     eax, [ebp+8]
   mov     [esp+14h], eax
   lea     eax, [ebp-68h]
   mov     [esp+10h], eax
   lea     eax, [ebp-88h]
   mov     [esp+0Ch], eax
   lea     eax, [ebp-78h]
   mov     [esp+8], eax
   lea     eax, [ebp-58h]
   mov     [esp+4], eax
   lea     eax, [ebp-48h]
   mov     [esp], eax
   mov     eax, [ebp-11Ch]
   call    eax
   fld     dword [ebp-48h]
   fld1    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80DE62D
   jmp     loc_80DE634
loc_80DE62D:
   mov     byte [ebp-11Dh], 0
loc_80DE634:
   cmp     dword [ebp+1Ch], 0
   jz      loc_80DE644
   mov     edx, [ebp+1Ch]
   mov     eax, 0
   mov     [edx], eax
loc_80DE644:
   cmp     dword [ebp+20h], 0
   jz      loc_80DE687
   lea     eax, [ebp-98h]
   mov     [esp+8], eax
   lea     eax, [ebp-0E8h]
   mov     [esp+4], eax
   lea     eax, [ebp-0D8h]
   mov     [esp], eax
   call    sub_80DEA18
   lea     eax, [ebp-98h]
   mov     [esp], eax
   call    sub_80A2AF6
   fstp    dword [esp]
   call    AngleNormalize180
   mov     eax, [ebp+20h]
   fstp    dword [eax]
loc_80DE687:
   cmp     dword [ebp+24h], 0
   jz      loc_80DE6CA
   lea     eax, [ebp-98h]
   mov     [esp+8], eax
   lea     eax, [ebp-0F8h]
   mov     [esp+4], eax
   lea     eax, [ebp-0E8h]
   mov     [esp], eax
   call    sub_80DEA18
   lea     eax, [ebp-98h]
   mov     [esp], eax
   call    sub_80A2AF6
   fstp    dword [esp]
   call    AngleNormalize180
   mov     eax, [ebp+24h]
   fstp    dword [eax]
loc_80DE6CA:
   cmp     byte [ebp-11Dh], 0
   jz      loc_80DE6DF
   mov     dword [ebp-124h], 1
   jmp     loc_80DE72B
loc_80DE6DF:
   cmp     dword [ebp+2Ch], 0
   jnz     loc_80DE721
   cmp     dword [ebp+1Ch], 0
   jz      loc_80DE6F5
   mov     edx, [ebp+1Ch]
   mov     eax, 0
   mov     [edx], eax
loc_80DE6F5:
   cmp     dword [ebp+20h], 0
   jz      loc_80DE705
   mov     edx, [ebp+20h]
   mov     eax, 0
   mov     [edx], eax
loc_80DE705:
   cmp     dword [ebp+24h], 0
   jz      loc_80DE715
   mov     edx, [ebp+24h]
   mov     eax, 0
   mov     [edx], eax
loc_80DE715:
   mov     dword [ebp-124h], 1
   jmp     loc_80DE72B
loc_80DE721:
   mov     dword [ebp-124h], 0
loc_80DE72B:
   mov     eax, [ebp-124h]
   leave   
   retn    


BG_CheckProne:
   push    ebp
   mov     ebp, esp
   sub     esp, 48h
   mov     eax, [ebp+34h]
   mov     [ebp-1], al
   mov     eax, [ebp+3Ch]
   mov     [esp+34h], eax
   mov     eax, [ebp+38h]
   mov     [esp+30h], eax
   movzx   eax, byte [ebp-1]
   mov     [esp+2Ch], eax
   mov     eax, [ebp+30h]
   mov     [esp+28h], eax
   mov     eax, [ebp+2Ch]
   mov     [esp+24h], eax
   mov     eax, [ebp+28h]
   mov     [esp+20h], eax
   mov     eax, [ebp+24h]
   mov     [esp+1Ch], eax
   mov     eax, [ebp+20h]
   mov     [esp+18h], eax
   mov     eax, [ebp+1Ch]
   mov     [esp+14h], eax
   mov     eax, [ebp+18h]
   mov     [esp+10h], eax
   mov     eax, [ebp+14h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+10h]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    BG_CheckProneValid
   leave   
   retn    


section .rdata
flt_814DCC0     dd 5.0
flt_814DCBC     dd 70.0
flt_814DCB8     dd -50.0
flt_814DCA0     dd 48.0
flt_814DC94     dd 6.0
flt_814DCB4     dd 18.0
flt_814DC90     dd 0.69999999
flt_814DCB0     dd 0.5
flt_8146088     dd 180.0
flt_814DCAC     dd -0.75
flt_814DCA8     dd 2.5
flt_814DCA4     dd 22.0
flt_814DC9C     dd 2.0
flt_814DC98     dd 180.0
flt_814DC8C     dd 10.0
flt_8146080     dd 182.04445
flt_8146084     dd 0.0054931641
flt_814608C     dd 360.0
flt_8145F38     dd 360.0
flt_8146074     dd 180.0
flt_814607C     dd -180.0
flt_8146078     dd 360.0

dbl_8145F88     dq 0.0174532925199433
dbl_8145F28     dq -180.0
dbl_8145F30     dq 3.141592653589793