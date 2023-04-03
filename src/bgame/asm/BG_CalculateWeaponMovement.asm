extern BG_GetViewmodelWeaponIndex
extern BG_GetWeaponDef
extern BG_IsAimDownSightWeapon
extern BG_GetVerticalBobFactor
extern BG_GetHorizontalBobFactor
extern sin


global BG_CalculateWeaponMovement


section .text


sub_80F0362:
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


sub_80F01B2:
   push    ebp
   mov     ebp, esp
   sub     esp, 18h
   mov     eax, [ebp+0Ch]
   mov     [esp+8], eax
   mov     eax, [ebp+8]
   mov     [esp+4], eax
   fld     dword [ebp+8]
   fsub    dword [ebp+0Ch]
   fstp    dword [esp]
   call    sub_80F03B6
   leave   
   retn    


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


Fabs681:
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


sub_80EE640:
   push    ebp
   mov     ebp, esp
   push    ebx
   sub     esp, 44h
   mov     eax, [ebp+8]
   mov     eax, [eax]
   mov     [ebp-2Ch], eax
   mov     eax, [ebp-2Ch]
   mov     [esp], eax
   call    BG_GetViewmodelWeaponIndex
   mov     [ebp-34h], eax
   mov     eax, [ebp-34h]
   mov     [esp], eax
   call    BG_GetWeaponDef
   mov     [ebp-30h], eax
   mov     eax, [ebp-2Ch]
   mov     eax, [eax+0A0h]
   and     eax, 8
   test    eax, eax
   jz      loc_80EE689
   mov     eax, [ebp-30h]
   mov     eax, [eax+1B0h]
   mov     [ebp-18h], eax
   jmp     loc_80EE6B3
loc_80EE689:
   mov     eax, [ebp-2Ch]
   mov     eax, [eax+0A0h]
   and     eax, 4
   test    eax, eax
   jz      loc_80EE6A7
   mov     eax, [ebp-30h]
   mov     eax, [eax+1ACh]
   mov     [ebp-18h], eax
   jmp     loc_80EE6B3
loc_80EE6A7:
   mov     eax, [ebp-30h]
   mov     eax, [eax+1A8h]
   mov     [ebp-18h], eax
loc_80EE6B3:
   mov     eax, [ebp+8]
   fld     dword [eax+4]
   fld     dword [ebp-18h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80EE6CA
   jmp     loc_80EE799
loc_80EE6CA:
   mov     eax, [ebp-2Ch]
   cmp     dword [eax+0D8h], 5
   jz      loc_80EE799
   mov     eax, [ebp+8]
   fld     dword [eax+4]
   fsub    dword [ebp-18h]
   mov     eax, [ebp-2Ch]
   fild    dword [eax+50h]
   fsub    dword [ebp-18h]
   fdivp   st1, st0
   fstp    dword [ebp-10h]
   mov     eax, [ebp-10h]
   mov     [esp+4], eax
   mov     eax, 0
   mov     [esp], eax
   call    sub_80F01B2
   fstp    dword [esp+4]
   mov     eax, 3F800000h
   mov     [esp], eax
   call    sub_80F01D6
   fstp    dword [ebp-10h]
   mov     eax, [ebp-2Ch]
   mov     eax, [eax+0A0h]
   and     eax, 8
   test    eax, eax
   jz      loc_80EE749
   lea     eax, [ebp-28h]
   mov     [esp+8], eax
   mov     eax, [ebp-10h]
   mov     [esp+4], eax
   mov     eax, [ebp-30h]
   add     eax, 180h
   mov     [esp], eax
   call    VectorScale814
   jmp     loc_80EE7A4
loc_80EE749:
   mov     eax, [ebp-2Ch]
   mov     eax, [eax+0A0h]
   and     eax, 4
   test    eax, eax
   jz      loc_80EE779
   lea     eax, [ebp-28h]
   mov     [esp+8], eax
   mov     eax, [ebp-10h]
   mov     [esp+4], eax
   mov     eax, [ebp-30h]
   add     eax, 15Ch
   mov     [esp], eax
   call    VectorScale814
   jmp     loc_80EE7A4
loc_80EE779:
   lea     eax, [ebp-28h]
   mov     [esp+8], eax
   mov     eax, [ebp-10h]
   mov     [esp+4], eax
   mov     eax, [ebp-30h]
   add     eax, 138h
   mov     [esp], eax
   call    VectorScale814
   jmp     loc_80EE7A4
loc_80EE799:
   lea     eax, [ebp-28h]
   mov     [esp], eax
   call    VectorClear658
loc_80EE7A4:
   mov     eax, [ebp-2Ch]
   fld     dword [eax+0DCh]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80EE7BC
   jp      loc_80EE7BC
   jmp     loc_80EE7E3
loc_80EE7BC:
   mov     eax, [ebp-2Ch]
   fld1    
   fsub    dword [eax+0DCh]
   fstp    dword [ebp-14h]
   lea     eax, [ebp-28h]
   mov     [esp+8], eax
   mov     eax, [ebp-14h]
   mov     [esp+4], eax
   lea     eax, [ebp-28h]
   mov     [esp], eax
   call    VectorScale814
loc_80EE7E3:
   mov     dword [ebp-0Ch], 0
loc_80EE7EA:
   cmp     dword [ebp-0Ch], 2
   jle     loc_80EE7F5
   jmp     loc_80EE980
loc_80EE7F5:
   mov     ecx, [ebp+8]
   mov     eax, [ebp-0Ch]
   mov     edx, [ebp-0Ch]
   fld     dword [ecx+eax*4+0Ch]
   fld     dword [ebp+edx*4-28h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jp      loc_80EE815
   jz      loc_80EE976
loc_80EE815:
   mov     eax, [ebp-2Ch]
   fld     dword [eax+0F8h]
   fld     dword [flt_814E710]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jp      loc_80EE82F
   jz      loc_80EE831
loc_80EE82F:
   jmp     loc_80EE856
loc_80EE831:
   mov     ebx, [ebp+8]
   mov     edx, [ebp-0Ch]
   mov     ecx, [ebp+8]
   mov     eax, [ebp-0Ch]
   fld     dword [ebp+edx*4-28h]
   fsub    dword [ecx+eax*4+0Ch]
   fmul    dword [ebx+8]
   mov     eax, [ebp-30h]
   fmul    dword [eax+1A4h]
   fstp    dword [ebp-14h]
   jmp     loc_80EE879
loc_80EE856:
   mov     ebx, [ebp+8]
   mov     edx, [ebp-0Ch]
   mov     ecx, [ebp+8]
   mov     eax, [ebp-0Ch]
   fld     dword [ebp+edx*4-28h]
   fsub    dword [ecx+eax*4+0Ch]
   fmul    dword [ebx+8]
   mov     eax, [ebp-30h]
   fmul    dword [eax+1A0h]
   fstp    dword [ebp-14h]
loc_80EE879:
   mov     ecx, [ebp+8]
   mov     eax, [ebp-0Ch]
   mov     edx, [ebp-0Ch]
   fld     dword [ecx+eax*4+0Ch]
   fld     dword [ebp+edx*4-28h]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80EE893
   jmp     loc_80EE909
loc_80EE893:
   mov     eax, [ebp+8]
   fld     dword [eax+8]
   fld     dword [flt_814E714]
   fmulp   st1, st0
   fld     dword [ebp-14h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80EE8AF
   jmp     loc_80EE8C0
loc_80EE8AF:
   mov     eax, [ebp+8]
   fld     dword [eax+8]
   fld     dword [flt_814E714]
   fmulp   st1, st0
   fstp    dword [ebp-14h]
loc_80EE8C0:
   mov     ebx, [ebp+8]
   mov     ecx, [ebp-0Ch]
   mov     edx, [ebp+8]
   mov     eax, [ebp-0Ch]
   fld     dword [edx+eax*4+0Ch]
   fadd    dword [ebp-14h]
   fstp    dword [ebx+ecx*4+0Ch]
   mov     ecx, [ebp+8]
   mov     eax, [ebp-0Ch]
   mov     edx, [ebp-0Ch]
   fld     dword [ecx+eax*4+0Ch]
   fld     dword [ebp+edx*4-28h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80EE8F6
   jmp     loc_80EE976
loc_80EE8F6:
   mov     ecx, [ebp+8]
   mov     edx, [ebp-0Ch]
   mov     eax, [ebp-0Ch]
   mov     eax, [ebp+eax*4-28h]
   mov     [ecx+edx*4+0Ch], eax
   jmp     loc_80EE976
loc_80EE909:
   mov     eax, [ebp+8]
   fld     dword [eax+8]
   fld     dword [flt_814E718]
   fmulp   st1, st0
   fld     dword [ebp-14h]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80EE923
   jmp     loc_80EE934
loc_80EE923:
   mov     eax, [ebp+8]
   fld     dword [eax+8]
   fld     dword [flt_814E718]
   fmulp   st1, st0
   fstp    dword [ebp-14h]
loc_80EE934:
   mov     ebx, [ebp+8]
   mov     ecx, [ebp-0Ch]
   mov     edx, [ebp+8]
   mov     eax, [ebp-0Ch]
   fld     dword [edx+eax*4+0Ch]
   fadd    dword [ebp-14h]
   fstp    dword [ebx+ecx*4+0Ch]
   mov     ecx, [ebp+8]
   mov     eax, [ebp-0Ch]
   mov     edx, [ebp-0Ch]
   fld     dword [ecx+eax*4+0Ch]
   fld     dword [ebp+edx*4-28h]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80EE965
   jmp     loc_80EE976
loc_80EE965:
   mov     ecx, [ebp+8]
   mov     edx, [ebp-0Ch]
   mov     eax, [ebp-0Ch]
   mov     eax, [ebp+eax*4-28h]
   mov     [ecx+edx*4+0Ch], eax
loc_80EE976:
   lea     eax, [ebp-0Ch]
   inc     dword [eax]
   jmp     loc_80EE7EA
loc_80EE980:
   mov     eax, [ebp-2Ch]
   fld     dword [eax+0DCh]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80EE998
   jp      loc_80EE998
   jmp     loc_80EE9E9
loc_80EE998:
   mov     eax, [ebp-2Ch]
   fld     dword [eax+0DCh]
   fld     dword [flt_814E71C]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80EE9B0
   jmp     loc_80EEA05
loc_80EE9B0:
   mov     eax, [ebp-2Ch]
   fld     dword [eax+0DCh]
   fld     st0
   faddp   st1, st0
   fld1    
   fsubrp  st1, st0
   fstp    dword [ebp-14h]
   mov     eax, [ebp+0Ch]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+8]
   add     eax, 0Ch
   mov     [esp+8], eax
   mov     eax, [ebp-14h]
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    sub_80F0362
   jmp     loc_80EEA05
loc_80EE9E9:
   mov     eax, [ebp+0Ch]
   mov     [esp+8], eax
   mov     eax, [ebp+8]
   add     eax, 0Ch
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    VectorAdd818
loc_80EEA05:
   add     esp, 44h
   pop     ebx
   pop     ebp
   retn    


sub_80EED76:
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
   mov     eax, [ebp-30h]
   movzx   eax, byte [eax+8]
   push    eax
   fild    dword [esp]
   lea     esp, [esp+4]
   fld     dword [flt_814E738]
   fdivp   st1, st0
   fld     qword [dbl_814E740]
   fmulp   st1, st0
   fld     st0
   faddp   st1, st0
   fld     qword [dbl_814E748]
   faddp   st1, st0
   fstp    dword [ebp-2Ch]
   fld     dword [ebp-2Ch]
   fld     qword [dbl_814E750]
   faddp   st1, st0
   fld     qword [dbl_814E748]
   faddp   st1, st0
   fstp    dword [ebp-10h]
   mov     eax, [ebp+8]
   fld     dword [eax+4]
   fld     dword [flt_814E758]
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
   fld     qword [dbl_814E760]
   fsubp   st1, st0
   fstp    dword [ebp-10h]
   fld     dword [ebp-14h]
   fld     dword [flt_814E768]
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
   jnz     loc_80EEEC5
   jp      loc_80EEEC5
   jmp     loc_80EEEF5
loc_80EEEC5:
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
loc_80EEEF5:
   mov     eax, [ebp-34h]
   cmp     dword [eax+278h], 0
   jz      loc_80EEF1C
   lea     eax, [ebp-28h]
   mov     [esp+8], eax
   fld1    
   fsub    dword [ebp-0Ch]
   fstp    dword [esp+4]
   lea     eax, [ebp-28h]
   mov     [esp], eax
   call    VectorScale814
loc_80EEF1C:
   mov     eax, [ebp+0Ch]
   mov     [esp+8], eax
   lea     eax, [ebp-28h]
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    VectorAdd818
   leave   
   retn    


sub_80EEA7C:
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
   mov     eax, [ebp-20h]
   mov     [esp], eax
   call    BG_IsAimDownSightWeapon
   test    eax, eax
   jz      loc_80EEB12
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
   jmp     loc_80EEB54
loc_80EEB12:
   mov     eax, [ebp-8]
   fld     dword [eax+2CCh]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80EEB2A
   jp      loc_80EEB2A
   jmp     loc_80EEB44
loc_80EEB2A:
   mov     eax, [ebp-8]
   mov     eax, [eax+2CCh]
   mov     [ebp-10h], eax
   mov     eax, [ebp-8]
   mov     eax, [eax+2D4h]
   mov     [ebp-24h], eax
   jmp     loc_80EEB54
loc_80EEB44:
   mov     eax, 42A00000h
   mov     [ebp-10h], eax
   mov     eax, 3F800000h
   mov     [ebp-24h], eax
loc_80EEB54:
   mov     eax, [ebp-1Ch]
   mov     eax, [eax+0A0h]
   and     eax, 8
   test    eax, eax
   jz      loc_80EEB72
   mov     eax, [ebp-8]
   mov     eax, [eax+2DCh]
   mov     [ebp-14h], eax
   jmp     loc_80EEB98
loc_80EEB72:
   mov     eax, [ebp-1Ch]
   mov     eax, [eax+0A0h]
   and     eax, 4
   test    eax, eax
   jz      loc_80EEB90
   mov     eax, [ebp-8]
   mov     eax, [eax+2D8h]
   mov     [ebp-14h], eax
   jmp     loc_80EEB98
loc_80EEB90:
   mov     eax, 3F800000h
   mov     [ebp-14h], eax
loc_80EEB98:
   mov     eax, [ebp-8]
   cmp     dword [eax+278h], 0
   jz      loc_80EEBBF
   mov     eax, [ebp-1Ch]
   fld     dword [eax+0DCh]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jp      loc_80EEBBA
   jz      loc_80EEBBF
loc_80EEBBA:
   jmp     loc_80EEC5E
loc_80EEBBF:
   mov     eax, [ebp+8]
   fld     dword [ebp-14h]
   fld     dword [eax+18h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80EEBD8
   jp      loc_80EEBD8
   jmp     loc_80EEC5E
loc_80EEBD8:
   mov     eax, [ebp+8]
   fld     dword [ebp-14h]
   fld     dword [eax+18h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80EEBEC
   jmp     loc_80EEC27
loc_80EEBEC:
   mov     ecx, [ebp+8]
   mov     edx, [ebp+8]
   mov     eax, [ebp+8]
   fld     dword [eax+8]
   fld     dword [flt_814E720]
   fmulp   st1, st0
   fld     dword [edx+18h]
   faddp   st1, st0
   fstp    dword [ecx+18h]
   mov     eax, [ebp+8]
   fld     dword [eax+18h]
   fld     dword [ebp-14h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80EEC1C
   jmp     loc_80EEC5E
loc_80EEC1C:
   mov     edx, [ebp+8]
   mov     eax, [ebp-14h]
   mov     [edx+18h], eax
   jmp     loc_80EEC5E
loc_80EEC27:
   mov     ecx, [ebp+8]
   mov     edx, [ebp+8]
   mov     eax, [ebp+8]
   fld     dword [eax+8]
   fld     dword [flt_814E720]
   fmulp   st1, st0
   fld     dword [edx+18h]
   fsubrp  st1, st0
   fstp    dword [ecx+18h]
   mov     eax, [ebp+8]
   fld     dword [eax+18h]
   fld     dword [ebp-14h]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80EEC55
   jmp     loc_80EEC5E
loc_80EEC55:
   mov     edx, [ebp+8]
   mov     eax, [ebp-14h]
   mov     [edx+18h], eax
loc_80EEC5E:
   mov     eax, [ebp+8]
   fld     dword [ebp-10h]
   fmul    dword [eax+18h]
   fstp    dword [ebp-10h]
   mov     eax, [ebp-8]
   cmp     dword [eax+278h], 0
   jz      loc_80EEC89
   mov     eax, [ebp-1Ch]
   fld1    
   fsub    dword [eax+0DCh]
   fld     dword [ebp-10h]
   fmulp   st1, st0
   fstp    dword [ebp-10h]
loc_80EEC89:
   mov     eax, [ebp+8]
   mov     edx, [eax+50h]
   mov     eax, [ebp+8]
   mov     ecx, [eax+50h]
   mov     eax, [ebp+8]
   fld     dword [eax+8]
   fld     dword [flt_814E724]
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
   mov     [edx], eax
   mov     eax, [ebp+8]
   mov     eax, [eax+50h]
   fild    dword [eax]
   fld     dword [flt_814E728]
   fmulp   st1, st0
   fstp    qword [esp]
   call    sin
   fstp    dword [ebp-0Ch]
   mov     edx, [ebp+0Ch]
   add     edx, 8
   mov     eax, [ebp+0Ch]
   add     eax, 8
   fld     dword [ebp-10h]
   fmul    dword [ebp-0Ch]
   fld     dword [flt_814E72C]
   fmulp   st1, st0
   fld     dword [eax]
   faddp   st1, st0
   fstp    dword [edx]
   mov     eax, [ebp+8]
   mov     eax, [eax+50h]
   fild    dword [eax]
   fld     dword [flt_814E730]
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
   fld     dword [flt_814E72C]
   fmulp   st1, st0
   fld     dword [eax]
   faddp   st1, st0
   fstp    dword [edx]
   mov     eax, [ebp+8]
   mov     eax, [eax+50h]
   fild    dword [eax]
   fld     dword [flt_814E734]
   fmulp   st1, st0
   fstp    qword [esp]
   call    sin
   fstp    dword [ebp-0Ch]
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp+0Ch]
   fld     dword [ebp-10h]
   fmul    dword [ebp-0Ch]
   fld     dword [flt_814E72C]
   fmulp   st1, st0
   fld     dword [eax]
   faddp   st1, st0
   fstp    dword [edx]
   add     esp, 34h
   pop     ebx
   pop     ebp
   retn    


sub_80EF12E:
   push    ebp
   mov     ebp, esp
   sub     esp, 18h
   mov     dword [ebp-4], 0
   mov     eax, [ebp+8]
   mov     eax, [eax]
   mov     [esp], eax
   call    Fabs681
   fld     dword [flt_814E77C]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80EF157
   jmp     loc_80EF18F
loc_80EF157:
   mov     eax, [ebp+0Ch]
   mov     eax, [eax]
   mov     [esp], eax
   call    Fabs681
   fld1    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80EF16F
   jmp     loc_80EF18F
loc_80EF16F:
   mov     edx, [ebp+8]
   mov     eax, 0
   mov     [edx], eax
   mov     edx, [ebp+0Ch]
   mov     eax, 0
   mov     [edx], eax
   mov     dword [ebp-4], 1
   jmp     loc_80EF331
loc_80EF18F:
   mov     ecx, [ebp+8]
   mov     edx, [ebp+8]
   mov     eax, [ebp+0Ch]
   fld     dword [eax]
   fmul    dword [ebp+10h]
   fld     dword [edx]
   faddp   st1, st0
   fstp    dword [ecx]
   mov     eax, [ebp+8]
   fld     dword [eax]
   fld     dword [ebp+14h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80EF1B6
   jmp     loc_80EF1DC
loc_80EF1B6:
   mov     edx, [ebp+8]
   mov     eax, [ebp+14h]
   mov     [edx], eax
   mov     eax, [ebp+0Ch]
   fld     dword [eax]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80EF1D0
   jmp     loc_80EF21E
loc_80EF1D0:
   mov     edx, [ebp+0Ch]
   mov     eax, 0
   mov     [edx], eax
   jmp     loc_80EF21E
loc_80EF1DC:
   mov     edx, [ebp+8]
   mov     eax, [ebp+14h]
   mov     [ebp-8], eax
   fld     dword [ebp-8]
   fchs    
   fld     dword [edx]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80EF1F7
   jmp     loc_80EF21E
loc_80EF1F7:
   mov     edx, [ebp+8]
   mov     eax, [ebp+14h]
   xor     eax, 80000000h
   mov     [edx], eax
   mov     eax, [ebp+0Ch]
   fld     dword [eax]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80EF214
   jmp     loc_80EF21E
loc_80EF214:
   mov     edx, [ebp+0Ch]
   mov     eax, 0
   mov     [edx], eax
loc_80EF21E:
   mov     eax, [ebp+8]
   fld     dword [eax]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80EF230
   jmp     loc_80EF244
loc_80EF230:
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp+0Ch]
   fld     dword [ebp+18h]
   fmul    dword [ebp+10h]
   fld     dword [eax]
   fsubrp  st1, st0
   fstp    dword [edx]
   jmp     loc_80EF266
loc_80EF244:
   mov     eax, [ebp+8]
   fld     dword [eax]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80EF254
   jmp     loc_80EF266
loc_80EF254:
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp+0Ch]
   fld     dword [ebp+18h]
   fmul    dword [ebp+10h]
   fld     dword [eax]
   faddp   st1, st0
   fstp    dword [edx]
loc_80EF266:
   mov     ecx, [ebp+0Ch]
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp+0Ch]
   fld     dword [eax]
   fmul    dword [ebp+20h]
   fmul    dword [ebp+10h]
   fld     dword [edx]
   fsubrp  st1, st0
   fstp    dword [ecx]
   mov     eax, [ebp+0Ch]
   fld     dword [eax]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80EF28F
   jmp     loc_80EF2BD
loc_80EF28F:
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp+0Ch]
   fld     dword [ebp+24h]
   fmul    dword [ebp+10h]
   fld     dword [eax]
   fsubrp  st1, st0
   fstp    dword [edx]
   mov     eax, [ebp+0Ch]
   fld     dword [eax]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80EF2B1
   jmp     loc_80EF2EB
loc_80EF2B1:
   mov     edx, [ebp+0Ch]
   mov     eax, 0
   mov     [edx], eax
   jmp     loc_80EF2EB
loc_80EF2BD:
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp+0Ch]
   fld     dword [ebp+24h]
   fmul    dword [ebp+10h]
   fld     dword [eax]
   faddp   st1, st0
   fstp    dword [edx]
   mov     eax, [ebp+0Ch]
   fld     dword [eax]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80EF2E1
   jmp     loc_80EF2EB
loc_80EF2E1:
   mov     edx, [ebp+0Ch]
   mov     eax, 0
   mov     [edx], eax
loc_80EF2EB:
   mov     eax, [ebp+0Ch]
   fld     dword [eax]
   fld     dword [ebp+1Ch]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80EF2FE
   jmp     loc_80EF308
loc_80EF2FE:
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp+1Ch]
   mov     [edx], eax
   jmp     loc_80EF331
loc_80EF308:
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp+1Ch]
   mov     [ebp-8], eax
   fld     dword [ebp-8]
   fchs    
   fld     dword [edx]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80EF323
   jmp     loc_80EF331
loc_80EF323:
   mov     eax, [ebp+0Ch]
   mov     edx, [ebp+1Ch]
   xor     edx, 80000000h
   mov     [eax], edx
loc_80EF331:
   mov     eax, [ebp-4]
   leave   
   retn    


sub_80EEF38:
   push    ebp
   mov     ebp, esp
   sub     esp, 28h
   mov     eax, [ebp+8]
   cmp     dword [eax+20h], 0
   jnz     loc_80EEF4C
   jmp     locret_80EF12C
loc_80EEF4C:
   mov     eax, [ebp+8]
   mov     eax, [eax]
   mov     [ebp-14h], eax
   mov     eax, [ebp-14h]
   mov     [esp], eax
   call    BG_GetViewmodelWeaponIndex
   mov     [ebp-1Ch], eax
   mov     eax, [ebp-1Ch]
   mov     [esp], eax
   call    BG_GetWeaponDef
   mov     [ebp-18h], eax
   mov     eax, [ebp-14h]
   fld     dword [eax+0DCh]
   fld     dword [flt_814E76C]
   fmulp   st1, st0
   fld     dword [flt_814E76C]
   faddp   st1, st0
   fstp    dword [ebp-8]
   fld     dword [ebp-8]
   fld     dword [flt_814E770]
   fmulp   st1, st0
   fstp    dword [ebp-0Ch]
   fld     dword [ebp-8]
   fld     dword [flt_814E774]
   fmulp   st1, st0
   fstp    dword [ebp-10h]
   mov     eax, [ebp-14h]
   fld     dword [eax+0DCh]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80EEFC0
   jp      loc_80EEFC0
   jmp     loc_80EEFE9
loc_80EEFC0:
   mov     eax, [ebp-18h]
   cmp     dword [eax+278h], 0
   jz      loc_80EEFE9
   mov     eax, [ebp-14h]
   fld     dword [eax+0DCh]
   fld     dword [flt_814E778]
   fmulp   st1, st0
   fld1    
   fsubrp  st1, st0
   fld     dword [ebp-8]
   fmulp   st1, st0
   fstp    dword [ebp-8]
loc_80EEFE9:
   mov     ecx, [ebp+8]
   mov     eax, [ebp+8]
   mov     edx, [eax+20h]
   mov     eax, [ecx+1Ch]
   sub     eax, edx
   push    eax
   fild    dword [esp]
   lea     esp, [esp+4]
   fstp    dword [ebp-4]
   fld     dword [ebp-4]
   fld     dword [ebp-0Ch]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80EF014
   jmp     loc_80EF094
loc_80EF014:
   fld     dword [ebp-4]
   fdiv    dword [ebp-0Ch]
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
   fmul    dword [eax+24h]
   fld     dword [flt_814E76C]
   fmulp   st1, st0
   fld     dword [edx]
   faddp   st1, st0
   fstp    dword [ecx]
   mov     ecx, [ebp+0Ch]
   add     ecx, 4
   mov     edx, [ebp+0Ch]
   add     edx, 4
   mov     eax, [ebp+8]
   fld     dword [ebp-4]
   fmul    dword [eax+28h]
   fld     dword [edx]
   fsubrp  st1, st0
   fstp    dword [ecx]
   mov     ecx, [ebp+0Ch]
   add     ecx, 8
   mov     edx, [ebp+0Ch]
   add     edx, 8
   mov     eax, [ebp+8]
   fld     dword [ebp-4]
   fmul    dword [eax+28h]
   fld     dword [flt_814E76C]
   fmulp   st1, st0
   fld     dword [edx]
   faddp   st1, st0
   fstp    dword [ecx]
   jmp     locret_80EF12C
loc_80EF094:
   fld     dword [ebp-4]
   fsub    dword [ebp-0Ch]
   fdiv    dword [ebp-10h]
   fld1    
   fsubrp  st1, st0
   fstp    dword [ebp-4]
   fld     dword [ebp-4]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80EF0B4
   jmp     locret_80EF12C
loc_80EF0B4:
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
   fmul    dword [eax+24h]
   fld     dword [flt_814E76C]
   fmulp   st1, st0
   fld     dword [edx]
   faddp   st1, st0
   fstp    dword [ecx]
   mov     ecx, [ebp+0Ch]
   add     ecx, 4
   mov     edx, [ebp+0Ch]
   add     edx, 4
   mov     eax, [ebp+8]
   fld     dword [ebp-4]
   fmul    dword [eax+28h]
   fld     dword [edx]
   fsubrp  st1, st0
   fstp    dword [ecx]
   mov     ecx, [ebp+0Ch]
   add     ecx, 8
   mov     edx, [ebp+0Ch]
   add     edx, 8
   mov     eax, [ebp+8]
   fld     dword [ebp-4]
   fmul    dword [eax+28h]
   fld     dword [flt_814E76C]
   fmulp   st1, st0
   fld     dword [edx]
   faddp   st1, st0
   fstp    dword [ecx]
locret_80EF12C:
   leave   
   retn    


sub_80EF336:
   push    ebp
   mov     ebp, esp
   sub     esp, 58h
   mov     eax, [ebp+8]
   mov     eax, [eax]
   mov     [ebp-24h], eax
   mov     eax, [ebp-24h]
   mov     [esp], eax
   call    BG_GetViewmodelWeaponIndex
   mov     [ebp-2Ch], eax
   mov     eax, [ebp-2Ch]
   mov     [esp], eax
   call    BG_GetWeaponDef
   mov     [ebp-28h], eax
   mov     eax, [ebp-2Ch]
   mov     [esp], eax
   call    BG_IsAimDownSightWeapon
   test    eax, eax
   jnz     loc_80EF374
   jmp     locret_80EF52A
loc_80EF374:
   mov     eax, [ebp-24h]
   mov     eax, [eax+0DCh]
   mov     [ebp-20h], eax
   mov     ecx, [ebp-28h]
   mov     eax, [ebp-28h]
   mov     edx, [ebp-28h]
   fld     dword [eax+490h]
   fsub    dword [edx+4D8h]
   fmul    dword [ebp-20h]
   fld     dword [ecx+4D8h]
   faddp   st1, st0
   fstp    dword [ebp-10h]
   mov     ecx, [ebp-28h]
   mov     eax, [ebp-28h]
   mov     edx, [ebp-28h]
   fld     dword [eax+494h]
   fsub    dword [edx+4DCh]
   fmul    dword [ebp-20h]
   fld     dword [ecx+4DCh]
   faddp   st1, st0
   fstp    dword [ebp-14h]
   mov     ecx, [ebp-28h]
   mov     eax, [ebp-28h]
   mov     edx, [ebp-28h]
   fld     dword [eax+498h]
   fsub    dword [edx+4E0h]
   fmul    dword [ebp-20h]
   fld     dword [ecx+4E0h]
   faddp   st1, st0
   fstp    dword [ebp-18h]
   mov     ecx, [ebp-28h]
   mov     eax, [ebp-28h]
   mov     edx, [ebp-28h]
   fld     dword [eax+49Ch]
   fsub    dword [edx+4E4h]
   fmul    dword [ebp-20h]
   fld     dword [ecx+4E4h]
   faddp   st1, st0
   fstp    dword [ebp-1Ch]
   mov     dword [ebp-0Ch], 0
   mov     eax, [ebp+8]
   mov     eax, [eax+8]
   mov     [ebp-4], eax
loc_80EF41C:
   fld     dword [ebp-4]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80EF42F
   jmp     loc_80EF50E
loc_80EF42F:
   fld     dword [ebp-4]
   fld     dword [dbl_814E780]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80EF443
   jmp     loc_80EF45B
loc_80EF443:
   mov     eax, 3BA3D70Ah
   mov     [ebp-8], eax
   fld     dword [ebp-4]
   fld     dword [dbl_814E780]
   fsubp   st1, st0
   fstp    dword [ebp-4]
   jmp     loc_80EF469
loc_80EF45B:
   mov     eax, [ebp-4]
   mov     [ebp-8], eax
   mov     eax, 0
   mov     [ebp-4], eax
loc_80EF469:
   mov     eax, [ebp-1Ch]
   mov     [esp+1Ch], eax
   mov     eax, [ebp-18h]
   mov     [esp+18h], eax
   mov     eax, [ebp-14h]
   mov     [esp+14h], eax
   mov     eax, [ebp-10h]
   mov     [esp+10h], eax
   mov     eax, [ebp-28h]
   mov     eax, [eax+2E0h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp-8]
   mov     [esp+8], eax
   mov     eax, [ebp+8]
   add     eax, 38h
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   add     eax, 2Ch
   mov     [esp], eax
   call    sub_80EF12E
   mov     [ebp-0Ch], eax
   mov     eax, [ebp-1Ch]
   mov     [esp+1Ch], eax
   mov     eax, [ebp-18h]
   mov     [esp+18h], eax
   mov     eax, [ebp-14h]
   mov     [esp+14h], eax
   mov     eax, [ebp-10h]
   mov     [esp+10h], eax
   mov     eax, [ebp-28h]
   mov     eax, [eax+2E4h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp-8]
   mov     [esp+8], eax
   mov     eax, [ebp+8]
   add     eax, 3Ch
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   add     eax, 30h
   mov     [esp], eax
   call    sub_80EF12E
   test    eax, eax
   jz      loc_80EF41C
   cmp     dword [ebp-0Ch], 0
   jz      loc_80EF41C
loc_80EF50E:
   mov     eax, [ebp+0Ch]
   mov     [esp+8], eax
   mov     eax, [ebp+8]
   add     eax, 2Ch
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    VectorAdd818
locret_80EF52A:
   leave   
   retn    


sub_80EEA0C:
   push    ebp
   mov     ebp, esp
   push    ebx
   sub     esp, 14h
   mov     eax, [ebp+8]
   mov     eax, [eax]
   mov     [ebp-8], eax
   mov     eax, [ebp-8]
   mov     [esp], eax
   call    BG_GetViewmodelWeaponIndex
   mov     [ebp-10h], eax
   mov     eax, [ebp-10h]
   mov     [esp], eax
   call    BG_GetWeaponDef
   mov     [ebp-0Ch], eax
   mov     eax, [ebp-10h]
   mov     [esp], eax
   call    BG_IsAimDownSightWeapon
   test    eax, eax
   jz      loc_80EEA64
   mov     ebx, [ebp+0Ch]
   mov     ecx, [ebp+0Ch]
   mov     eax, [ebp-8]
   mov     edx, [ebp-0Ch]
   fld     dword [eax+0DCh]
   fmul    dword [edx+46Ch]
   fld     dword [ecx]
   faddp   st1, st0
   fstp    dword [ebx]
loc_80EEA64:
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    sub_80EE640
   add     esp, 14h
   pop     ebx
   pop     ebp
   retn    


BG_CalculateWeaponMovement:
   push    ebp
   mov     ebp, esp
   sub     esp, 18h
   mov     eax, [ebp+8]
   mov     eax, [eax]
   mov     [ebp-8], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    VectorClear658
   mov     eax, [ebp-8]
   fld     dword [eax+4Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80EF55A
   jp      loc_80EF55A
   jmp     loc_80EF584
loc_80EF55A:
   mov     eax, [ebp-8]
   mov     eax, [eax+4Ch]
   mov     [esp], eax
   call    GetLeanFraction
   fstp    dword [ebp-4]
   mov     edx, [ebp+0Ch]
   add     edx, 8
   mov     eax, [ebp+0Ch]
   add     eax, 8
   fld     dword [ebp-4]
   fld     st0
   faddp   st1, st0
   fld     dword [eax]
   fsubrp  st1, st0
   fstp    dword [edx]
loc_80EF584:
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    sub_80EEA0C
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    sub_80EEA7C
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    sub_80EED76
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    sub_80EEF38
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    sub_80EF336
   mov     eax, [ebp+8]
   mov     eax, [eax+44h]
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     eax, [eax]
   mov     [esp], eax
   call    AngleSubtract
   mov     eax, [ebp+0Ch]
   fstp    dword [eax]
   mov     eax, [ebp+8]
   mov     eax, [eax+48h]
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   add     eax, 4
   mov     eax, [eax]
   mov     [esp], eax
   call    AngleSubtract
   mov     eax, [ebp+0Ch]
   add     eax, 4
   fstp    dword [eax]
   leave   
   retn    


section .rdata
flt_814E76C     dd 0.5
flt_814E724     dd 1000.0
flt_814E728     dd 0.00050000002
flt_814E720     dd 0.5
flt_81489FC     dd 2.0
flt_814E72C     dd 0.0099999998
flt_8146074     dd 180.0
flt_814E778     dd 0.75
flt_814E714     dd 0.1
flt_8146078     dd 360.0
flt_814607C     dd -180.0
flt_814E718     dd -0.1
flt_814E710     dd 11.0
flt_814E71C     dd 0.5
flt_814E738     dd 255.0
flt_814E770     dd 100.0
flt_814E768     dd 1.5
flt_814E758     dd 0.16
flt_814E730     dd 0.00069999998
flt_814E774     dd 400.0
flt_814E734     dd 0.001
flt_814E77C     dd 0.25

dbl_814E740     dq 3.141592653589793
dbl_814E748     dq 6.283185307179586
dbl_814E750     dq 0.7853981633974483
dbl_814E760     dq 0.4712389167638204
dbl_814E780     dq 0.005