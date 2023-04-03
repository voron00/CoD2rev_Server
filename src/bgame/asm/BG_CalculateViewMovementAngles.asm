extern BG_GetViewmodelWeaponIndex
extern BG_GetWeaponDef
extern BG_IsAimDownSightWeapon
extern BG_GetVerticalBobFactor
extern BG_GetHorizontalBobFactor
extern sin


global BG_CalculateViewMovementAngles


section .text


sub_80B6D1E:
   push    ebp
   mov     ebp, esp
   sub     esp, 8
   fld     dword [ebp+8]
   fabs    
   fstp    dword [ebp-4]
   mov     eax, [ebp-4]
   mov     [ebp-8], eax
   fld     dword [ebp-8]
   leave   
   retn    


sub_80F03B6:
   push    ebp
   mov     ebp, esp
   sub     esp, 4
   fld     dword [ebp+8]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_80F03CC
   jmp     loc_80F03D4
loc_80F03CC:
   fld     dword [ebp+0Ch]
   fstp    dword [ebp-4]
   jmp     loc_80F03DA
loc_80F03D4:
   fld     dword [ebp+10h]
   fstp    dword [ebp-4]
loc_80F03DA:
   fld     dword [ebp-4]
   leave   
   retn    


sub_80F01D6:
   push    ebp
   mov     ebp, esp
   sub     esp, 18h
   mov     eax, [ebp+0Ch]
   mov     [esp+8], eax
   mov     eax, [ebp+8]
   mov     [esp+4], eax
   fld     dword [ebp+0Ch]
   fsub    dword [ebp+8]
   fstp    dword [esp]
   call    sub_80F03B6
   leave   
   retn    


VectorAdd818:
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


VectorScale814:
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


GetLeanFraction:
   push    ebp
   mov     ebp, esp
   sub     esp, 8
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    sub_80B6D1E
   fld     dword [flt_81489FC]
   fsubrp  st1, st0
   fmul    dword [ebp+8]
   leave   
   retn    


BG_CalculateViewMovement_AdsBob:
   push    ebp
   mov     ebp, esp
   sub     esp, 38h
   mov     eax, [ebp+8]
   mov     eax, [eax]
   mov     [ebp-10h], eax
   mov     eax, [ebp-10h]
   mov     [esp], eax
   call    BG_GetViewmodelWeaponIndex
   mov     [ebp-18h], eax
   mov     eax, [ebp-18h]
   mov     [esp], eax
   call    BG_GetWeaponDef
   mov     [ebp-14h], eax
   mov     eax, [ebp-10h]
   mov     eax, [eax+0A0h]
   and     eax, 300h
   test    eax, eax
   jnz     locret_80EFD92
   mov     eax, [ebp-10h]
   fld     dword [eax+0DCh]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80EFCA5
   jp      loc_80EFCA5
   jmp     locret_80EFD92
loc_80EFCA5:
   mov     eax, [ebp-14h]
   fld     dword [eax+288h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80EFCC0
   jp      loc_80EFCC0
   jmp     locret_80EFD92
loc_80EFCC0:
   mov     eax, [ebp-10h]
   movzx   eax, byte [eax+8]
   push    eax
   fild    dword [esp]
   lea     esp, [esp+4]
   fld     dword [flt_814E7DC]
   fdivp   st1, st0
   fld     qword [dbl_814E7E0]
   fmulp   st1, st0
   fld     st0
   faddp   st1, st0
   fld     qword [dbl_814E7E8]
   faddp   st1, st0
   fstp    dword [ebp-0Ch]
   mov     eax, 42340000h
   mov     [esp+0Ch], eax
   mov     eax, [ebp+8]
   mov     eax, [eax+14h]
   mov     [esp+8], eax
   mov     eax, [ebp-0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp-10h]
   mov     [esp], eax
   call    BG_GetVerticalBobFactor
   fstp    dword [ebp-4]
   mov     eax, [ebp-10h]
   mov     edx, [ebp-14h]
   fld     dword [eax+0DCh]
   fmul    dword [edx+288h]
   fld     dword [ebp-4]
   fmulp   st1, st0
   fstp    dword [ebp-4]
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp+0Ch]
   fld     dword [eax]
   fsub    dword [ebp-4]
   fstp    dword [edx]
   mov     eax, 42340000h
   mov     [esp+0Ch], eax
   mov     eax, [ebp+8]
   mov     eax, [eax+14h]
   mov     [esp+8], eax
   mov     eax, [ebp-0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp-10h]
   mov     [esp], eax
   call    BG_GetHorizontalBobFactor
   fstp    dword [ebp-8]
   mov     eax, [ebp-10h]
   mov     edx, [ebp-14h]
   fld     dword [eax+0DCh]
   fmul    dword [edx+288h]
   fld     dword [ebp-8]
   fmulp   st1, st0
   fstp    dword [ebp-8]
   mov     edx, [ebp+0Ch]
   add     edx, 4
   mov     eax, [ebp+0Ch]
   add     eax, 4
   fld     dword [eax]
   fsub    dword [ebp-8]
   fstp    dword [edx]
locret_80EFD92:
   leave   
   retn    


BG_CalculateViewMovement_BobAngles:
   push    ebp
   mov     ebp, esp
   sub     esp, 68h
   mov     eax, [ebp+8]
   mov     eax, [eax]
   mov     [ebp-30h], eax
   mov     eax, [ebp-30h]
   mov     [esp], eax
   call    BG_GetViewmodelWeaponIndex
   mov     [ebp-38h], eax
   mov     eax, [ebp-38h]
   mov     [esp], eax
   call    BG_GetWeaponDef
   mov     [ebp-34h], eax
   mov     eax, [ebp-34h]
   cmp     dword [eax+278h], 0
   jnz     loc_80EFAC1
   jmp     locret_80EFC48
loc_80EFAC1:
   mov     eax, [ebp-30h]
   movzx   eax, byte [eax+8]
   push    eax
   fild    dword [esp]
   lea     esp, [esp+4]
   fld     dword [flt_814E7A8]
   fdivp   st1, st0
   fld     qword [dbl_814E7B0]
   fmulp   st1, st0
   fld     st0
   faddp   st1, st0
   fld     qword [dbl_814E7B8]
   faddp   st1, st0
   fstp    dword [ebp-2Ch]
   fld     dword [ebp-2Ch]
   fld     qword [dbl_814E7C0]
   faddp   st1, st0
   fld     qword [dbl_814E7B8]
   faddp   st1, st0
   fstp    dword [ebp-10h]
   mov     eax, [ebp+8]
   fld     dword [eax+14h]
   fld     dword [flt_814E7C8]
   fmulp   st1, st0
   fstp    dword [ebp-14h]
   mov     eax, 41200000h
   mov     [esp+0Ch], eax
   mov     eax, [ebp-14h]
   mov     [esp+8], eax
   mov     eax, [ebp-10h]
   mov     [esp+4], eax
   mov     eax, [ebp-30h]
   mov     [esp], eax
   call    BG_GetVerticalBobFactor
   fstp    dword [ebp-4Ch]
   mov     eax, [ebp-4Ch]
   xor     eax, 80000000h
   mov     [ebp-28h], eax
   mov     eax, 41200000h
   mov     [esp+0Ch], eax
   mov     eax, [ebp-14h]
   mov     [esp+8], eax
   mov     eax, [ebp-10h]
   mov     [esp+4], eax
   mov     eax, [ebp-30h]
   mov     [esp], eax
   call    BG_GetHorizontalBobFactor
   fstp    dword [ebp-4Ch]
   mov     eax, [ebp-4Ch]
   xor     eax, 80000000h
   mov     [ebp-24h], eax
   fld     dword [ebp-10h]
   fld     qword [dbl_814E7D0]
   fsubp   st1, st0
   fstp    dword [ebp-10h]
   fld     dword [ebp-14h]
   fld     dword [flt_814E7D8]
   fmulp   st1, st0
   fstp    dword [ebp-14h]
   mov     eax, 41200000h
   mov     [esp+0Ch], eax
   mov     eax, [ebp-14h]
   mov     [esp+8], eax
   mov     eax, [ebp-10h]
   mov     [esp+4], eax
   mov     eax, [ebp-30h]
   mov     [esp], eax
   call    BG_GetHorizontalBobFactor
   mov     eax, 0
   mov     [esp+4], eax
   fstp    dword [esp]
   call    sub_80F01D6
   fstp    dword [ebp-20h]
   mov     eax, [ebp-30h]
   mov     eax, [eax+0DCh]
   mov     [ebp-0Ch], eax
   fld     dword [ebp-0Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80EFBE6
   jp      loc_80EFBE6
   jmp     loc_80EFC16
loc_80EFBE6:
   mov     eax, [ebp-34h]
   fld1    
   fsub    dword [eax+284h]
   fmul    dword [ebp-0Ch]
   fld1    
   fsubrp  st1, st0
   fstp    dword [ebp-14h]
   fld     dword [ebp-28h]
   fmul    dword [ebp-14h]
   fstp    dword [ebp-28h]
   fld     dword [ebp-24h]
   fmul    dword [ebp-14h]
   fstp    dword [ebp-24h]
   fld     dword [ebp-20h]
   fmul    dword [ebp-14h]
   fstp    dword [ebp-20h]
loc_80EFC16:
   lea     eax, [ebp-28h]
   mov     [esp+8], eax
   mov     eax, [ebp-0Ch]
   mov     [esp+4], eax
   lea     eax, [ebp-28h]
   mov     [esp], eax
   call    VectorScale814
   mov     eax, [ebp+0Ch]
   mov     [esp+8], eax
   lea     eax, [ebp-28h]
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    VectorAdd818
locret_80EFC48:
   leave   
   retn    


BG_CalculateViewMovement_IdleAngles:
   push    ebp
   mov     ebp, esp
   push    ebx
   sub     esp, 34h
   mov     eax, 3F000000h
   mov     [ebp-18h], eax
   mov     eax, [ebp+8]
   mov     eax, [eax]
   mov     [ebp-1Ch], eax
   mov     eax, [ebp-1Ch]
   mov     [esp], eax
   call    BG_GetViewmodelWeaponIndex
   mov     [ebp-20h], eax
   mov     eax, [ebp-20h]
   mov     [esp], eax
   call    BG_GetWeaponDef
   mov     [ebp-8], eax
   mov     eax, [ebp-8]
   cmp     dword [eax+278h], 0
   jnz     loc_80EF7EE
   jmp     loc_80EFA80
loc_80EF7EE:
   mov     eax, [ebp-20h]
   mov     [esp], eax
   call    BG_IsAimDownSightWeapon
   test    eax, eax
   jz      loc_80EF851
   mov     ebx, [ebp-8]
   mov     ecx, [ebp-1Ch]
   mov     eax, [ebp-8]
   mov     edx, [ebp-8]
   fld     dword [eax+2C8h]
   fsub    dword [edx+2CCh]
   fmul    dword [ecx+0DCh]
   fld     dword [ebx+2CCh]
   faddp   st1, st0
   fstp    dword [ebp-10h]
   mov     ebx, [ebp-8]
   mov     ecx, [ebp-1Ch]
   mov     eax, [ebp-8]
   mov     edx, [ebp-8]
   fld     dword [eax+2D0h]
   fsub    dword [edx+2D4h]
   fmul    dword [ecx+0DCh]
   fld     dword [ebx+2D4h]
   faddp   st1, st0
   fstp    dword [ebp-24h]
   jmp     loc_80EF893
loc_80EF851:
   mov     eax, [ebp-8]
   fld     dword [eax+2CCh]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80EF869
   jp      loc_80EF869
   jmp     loc_80EF883
loc_80EF869:
   mov     eax, [ebp-8]
   mov     eax, [eax+2CCh]
   mov     [ebp-10h], eax
   mov     eax, [ebp-8]
   mov     eax, [eax+2D4h]
   mov     [ebp-24h], eax
   jmp     loc_80EF893
loc_80EF883:
   mov     eax, 42A00000h
   mov     [ebp-10h], eax
   mov     eax, 3F800000h
   mov     [ebp-24h], eax
loc_80EF893:
   mov     eax, [ebp-1Ch]
   mov     eax, [eax+0A0h]
   and     eax, 8
   test    eax, eax
   jz      loc_80EF8B1
   mov     eax, [ebp-8]
   mov     eax, [eax+2DCh]
   mov     [ebp-14h], eax
   jmp     loc_80EF8D7
loc_80EF8B1:
   mov     eax, [ebp-1Ch]
   mov     eax, [eax+0A0h]
   and     eax, 4
   test    eax, eax
   jz      loc_80EF8CF
   mov     eax, [ebp-8]
   mov     eax, [eax+2D8h]
   mov     [ebp-14h], eax
   jmp     loc_80EF8D7
loc_80EF8CF:
   mov     eax, 3F800000h
   mov     [ebp-14h], eax
loc_80EF8D7:
   mov     eax, [ebp-8]
   cmp     dword [eax+278h], 0
   jz      loc_80EF9A1
   mov     eax, [ebp-1Ch]
   fld     dword [eax+0DCh]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80EF902
   jp      loc_80EF902
   jmp     loc_80EF9A1
loc_80EF902:
   mov     eax, [ebp+8]
   fld     dword [ebp-14h]
   fld     dword [eax+1Ch]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80EF91B
   jp      loc_80EF91B
   jmp     loc_80EF9A1
loc_80EF91B:
   mov     eax, [ebp+8]
   fld     dword [ebp-14h]
   fld     dword [eax+1Ch]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80EF92F
   jmp     loc_80EF96A
loc_80EF92F:
   mov     ecx, [ebp+8]
   mov     edx, [ebp+8]
   mov     eax, [ebp+8]
   fld     dword [eax+18h]
   fld     dword [flt_814E794]
   fmulp   st1, st0
   fld     dword [edx+1Ch]
   faddp   st1, st0
   fstp    dword [ecx+1Ch]
   mov     eax, [ebp+8]
   fld     dword [eax+1Ch]
   fld     dword [ebp-14h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80EF95F
   jmp     loc_80EF9A1
loc_80EF95F:
   mov     edx, [ebp+8]
   mov     eax, [ebp-14h]
   mov     [edx+1Ch], eax
   jmp     loc_80EF9A1
loc_80EF96A:
   mov     ecx, [ebp+8]
   mov     edx, [ebp+8]
   mov     eax, [ebp+8]
   fld     dword [eax+18h]
   fld     dword [flt_814E794]
   fmulp   st1, st0
   fld     dword [edx+1Ch]
   fsubrp  st1, st0
   fstp    dword [ecx+1Ch]
   mov     eax, [ebp+8]
   fld     dword [eax+1Ch]
   fld     dword [ebp-14h]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80EF998
   jmp     loc_80EF9A1
loc_80EF998:
   mov     edx, [ebp+8]
   mov     eax, [ebp-14h]
   mov     [edx+1Ch], eax
loc_80EF9A1:
   mov     eax, [ebp+8]
   fld     dword [ebp-10h]
   fmul    dword [eax+1Ch]
   fstp    dword [ebp-10h]
   mov     eax, [ebp-1Ch]
   fld     dword [ebp-10h]
   fmul    dword [eax+0DCh]
   fstp    dword [ebp-10h]
   mov     eax, [ebp-1Ch]
   fld     dword [ebp-10h]
   fmul    dword [eax+5B4h]
   fstp    dword [ebp-10h]
   mov     eax, [ebp+8]
   mov     ebx, [eax+20h]
   mov     eax, [ebp+8]
   mov     ecx, [eax+20h]
   mov     eax, [ebp-1Ch]
   mov     edx, [ebp+8]
   fld     dword [eax+5B4h]
   fmul    dword [edx+18h]
   fld     dword [flt_814E798]
   fmulp   st1, st0
   fmul    dword [ebp-24h]
   fnstcw  word [ebp-2Ah]
   movzx   eax, word [ebp-2Ah]
   or      ax, 0C00h
   mov     [ebp-2Ch], ax
   fldcw   word [ebp-2Ch]
   fistp   dword [ebp-30h]
   fldcw   word [ebp-2Ah]
   mov     eax, [ebp-30h]
   add     eax, [ecx]
   mov     [ebx], eax
   mov     eax, [ebp+8]
   mov     eax, [eax+20h]
   fild    dword [eax]
   fld     dword [flt_814E79C]
   fmulp   st1, st0
   fstp    qword [esp]
   call    sin
   fstp    dword [ebp-0Ch]
   mov     edx, [ebp+0Ch]
   add     edx, 4
   mov     eax, [ebp+0Ch]
   add     eax, 4
   fld     dword [ebp-10h]
   fmul    dword [ebp-0Ch]
   fld     dword [flt_814E7A0]
   fmulp   st1, st0
   fld     dword [eax]
   faddp   st1, st0
   fstp    dword [edx]
   mov     eax, [ebp+8]
   mov     eax, [eax+20h]
   fild    dword [eax]
   fld     dword [flt_814E7A4]
   fmulp   st1, st0
   fstp    qword [esp]
   call    sin
   fstp    dword [ebp-0Ch]
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp+0Ch]
   fld     dword [ebp-10h]
   fmul    dword [ebp-0Ch]
   fld     dword [flt_814E7A0]
   fmulp   st1, st0
   fld     dword [eax]
   faddp   st1, st0
   fstp    dword [edx]
loc_80EFA80:
   add     esp, 34h
   pop     ebx
   pop     ebp
   retn    


BG_CalculateViewMovement_DamageKick:
   push    ebp
   mov     ebp, esp
   sub     esp, 18h
   mov     eax, [ebp+8]
   cmp     dword [eax+4], 0
   jnz     loc_80EF632
   jmp     locret_80EF7A8
loc_80EF632:
   mov     eax, [ebp+8]
   mov     eax, [eax]
   mov     [ebp-0Ch], eax
   mov     eax, [ebp-0Ch]
   mov     [esp], eax
   call    BG_GetViewmodelWeaponIndex
   mov     [ebp-14h], eax
   mov     eax, [ebp-14h]
   mov     [esp], eax
   call    BG_GetWeaponDef
   mov     [ebp-10h], eax
   mov     eax, [ebp-0Ch]
   fld     dword [eax+0DCh]
   fld     dword [flt_814E788]
   fmulp   st1, st0
   fld1    
   fsubrp  st1, st0
   fstp    dword [ebp-8]
   mov     eax, [ebp-0Ch]
   fld     dword [eax+0DCh]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80EF686
   jp      loc_80EF686
   jmp     loc_80EF6AF
loc_80EF686:
   mov     eax, [ebp-10h]
   cmp     dword [eax+278h], 0
   jz      loc_80EF6AF
   mov     eax, [ebp-0Ch]
   fld     dword [eax+0DCh]
   fld     dword [flt_814E788]
   fmulp   st1, st0
   fld1    
   faddp   st1, st0
   fld     dword [ebp-8]
   fmulp   st1, st0
   fstp    dword [ebp-8]
loc_80EF6AF:
   mov     ecx, [ebp+8]
   mov     eax, [ebp+8]
   mov     edx, [eax+4]
   mov     eax, [ecx+8]
   sub     eax, edx
   push    eax
   fild    dword [esp]
   lea     esp, [esp+4]
   fstp    dword [ebp-4]
   fld     dword [ebp-4]
   fld     dword [flt_814E78C]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80EF6DA
   jmp     loc_80EF731
loc_80EF6DA:
   fld     dword [ebp-4]
   fld     dword [flt_814E78C]
   fdivp   st1, st0
   fstp    dword [ebp-4]
   mov     eax, [ebp-4]
   mov     [esp], eax
   call    GetLeanFraction
   fstp    dword [ebp-4]
   fld     dword [ebp-4]
   fmul    dword [ebp-8]
   fstp    dword [ebp-4]
   mov     ecx, [ebp+0Ch]
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp+8]
   fld     dword [ebp-4]
   fmul    dword [eax+0Ch]
   fld     dword [edx]
   faddp   st1, st0
   fstp    dword [ecx]
   mov     ecx, [ebp+0Ch]
   add     ecx, 8
   mov     edx, [ebp+0Ch]
   add     edx, 8
   mov     eax, [ebp+8]
   fld     dword [ebp-4]
   fmul    dword [eax+10h]
   fld     dword [edx]
   faddp   st1, st0
   fstp    dword [ecx]
   jmp     locret_80EF7A8
loc_80EF731:
   fld     dword [ebp-4]
   fld     dword [flt_814E78C]
   fsubp   st1, st0
   fld     dword [flt_814E790]
   fdivp   st1, st0
   fld1    
   fsubrp  st1, st0
   fstp    dword [ebp-4]
   fld     dword [ebp-4]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80EF75B
   jmp     locret_80EF7A8
loc_80EF75B:
   fld1    
   fsub    dword [ebp-4]
   fstp    dword [esp]
   call    GetLeanFraction
   fld1    
   fsubrp  st1, st0
   fstp    dword [ebp-4]
   fld     dword [ebp-4]
   fmul    dword [ebp-8]
   fstp    dword [ebp-4]
   mov     ecx, [ebp+0Ch]
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp+8]
   fld     dword [ebp-4]
   fmul    dword [eax+0Ch]
   fld     dword [edx]
   faddp   st1, st0
   fstp    dword [ecx]
   mov     ecx, [ebp+0Ch]
   add     ecx, 8
   mov     edx, [ebp+0Ch]
   add     edx, 8
   mov     eax, [ebp+8]
   fld     dword [ebp-4]
   fmul    dword [eax+10h]
   fld     dword [edx]
   faddp   st1, st0
   fstp    dword [ecx]
locret_80EF7A8:
   leave   
   retn    


VectorClear658:
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


BG_CalculateViewMovementAngles:
   push    ebp
   mov     ebp, esp
   sub     esp, 8
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    VectorClear658
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    BG_CalculateViewMovement_DamageKick
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    BG_CalculateViewMovement_IdleAngles
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    BG_CalculateViewMovement_BobAngles
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    BG_CalculateViewMovement_AdsBob
   leave   
   retn    


section .rdata
flt_814E790     dd 400.0
flt_814E78C     dd 100.0
flt_814E788     dd 0.5
flt_814E7A0     dd 0.0099999998
flt_814E7A4     dd 0.001
flt_814E79C     dd 0.00069999998
flt_81489FC     dd 2.0
flt_814E798     dd 1000.0
flt_814E794     dd 0.5
flt_814E7DC     dd 255.0
flt_814E7C8     dd 0.16
flt_814E7D8     dd 1.5
flt_814E7A8     dd 255.0

dbl_814E7E0     dq 3.141592653589793
dbl_814E7E8     dq 6.283185307179586
dbl_814E7B0     dq 3.141592653589793
dbl_814E7D0     dq 0.4712389167638204
dbl_814E7B8     dq 6.283185307179586
dbl_814E7C0     dq 0.7853981633974483