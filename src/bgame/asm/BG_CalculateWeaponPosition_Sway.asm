extern BG_GetViewmodelWeaponIndex
extern BG_GetWeaponDef
extern BG_IsAimDownSightWeapon


global BG_CalculateWeaponPosition_Sway


section .text


VectorCopy932:
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


I_fclamp032:
   push    ebp
   mov     ebp, esp
   sub     esp, 18h
   mov     eax, [ebp+8]
   mov     [esp+8], eax
   mov     eax, [ebp+10h]
   mov     [esp+4], eax
   fld     dword [ebp+8]
   fsub    dword [ebp+10h]
   fstp    dword [esp]
   call    sub_80F03B6
   fstp    dword [esp+8]
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   fld     dword [ebp+0Ch]
   fsub    dword [ebp+8]
   fstp    dword [esp]
   call    sub_80F03B6
   leave   
   retn    


sub_80A8B40:
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


DiffTrack:
   push    ebp
   mov     ebp, esp
   sub     esp, 18h
   fld     dword [ebp+8]
   fsub    dword [ebp+0Ch]
   fstp    dword [ebp-4]
   fld     dword [ebp+10h]
   fmul    dword [ebp-4]
   fmul    dword [ebp+14h]
   fstp    dword [ebp-8]
   mov     eax, [ebp-4]
   mov     [esp], eax
   call    sub_80A8B40
   fld     dword [flt_8145EC4]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80A19BD
   jmp     loc_80A19F7
loc_80A19BD:
   mov     eax, [ebp-4]
   mov     [esp], eax
   call    sub_80A8B40
   fstp    dword [ebp-10h]
   mov     eax, [ebp-8]
   mov     [esp], eax
   call    sub_80A8B40
   fld     dword [ebp-10h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80A19E4
   jmp     loc_80A19EC
loc_80A19E4:
   mov     eax, [ebp+8]
   mov     [ebp-0Ch], eax
   jmp     loc_80A19FD
loc_80A19EC:
   fld     dword [ebp+0Ch]
   fadd    dword [ebp-8]
   fstp    dword [ebp-0Ch]
   jmp     loc_80A19FD
loc_80A19F7:
   mov     eax, [ebp+8]
   mov     [ebp-0Ch], eax
loc_80A19FD:
   fld     dword [ebp-0Ch]
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


AngleNormalize360:
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
   call    AngleNormalize360
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


DiffTrackAngle:
   push    ebp
   mov     ebp, esp
   sub     esp, 18h
   nop     
loc_80A1A09:
   fld     dword [ebp+8]
   fsub    dword [ebp+0Ch]
   fld     dword [flt_8145EC8]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80A1A20
   jmp     loc_80A1A30
loc_80A1A20:
   fld     dword [ebp+8]
   fld     dword [flt_8145ECC]
   fsubp   st1, st0
   fstp    dword [ebp+8]
   jmp     loc_80A1A09
loc_80A1A30:
   nop     
loc_80A1A31:
   fld     dword [ebp+8]
   fsub    dword [ebp+0Ch]
   fld     dword [flt_8145ED0]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80A1A46
   jmp     loc_80A1A56
loc_80A1A46:
   fld     dword [ebp+8]
   fld     dword [flt_8145ECC]
   faddp   st1, st0
   fstp    dword [ebp+8]
   jmp     loc_80A1A31
loc_80A1A56:
   mov     eax, [ebp+14h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+10h]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    DiffTrack
   fstp    dword [ebp-4]
   mov     eax, [ebp-4]
   mov     [esp], eax
   call    AngleNormalize180
   leave   
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


BG_CalculateWeaponPosition_Sway:
   push    ebp
   mov     ebp, esp
   sub     esp, 78h
   mov     eax, [ebp+8]
   mov     eax, [eax+0DCh]
   mov     [ebp-54h], eax
   cmp     dword [ebp+1Ch], 0
   jnz     loc_80EFE0D
   jmp     locret_80F010D
loc_80EFE0D:
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    BG_GetViewmodelWeaponIndex
   mov     [ebp-5Ch], eax
   mov     eax, [ebp-5Ch]
   mov     [esp], eax
   call    BG_GetWeaponDef
   mov     [ebp-0Ch], eax
   fild    dword [ebp+1Ch]
   fld     dword [flt_814E7F0]
   fmulp   st1, st0
   fstp    dword [ebp-58h]
   mov     eax, [ebp-5Ch]
   mov     [esp], eax
   call    BG_IsAimDownSightWeapon
   test    eax, eax
   jz      loc_80EFF33
   fld     dword [ebp-54h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80EFE5A
   jmp     loc_80EFE6B
loc_80EFE5A:
   mov     eax, [ebp-0Ch]
   cmp     dword [eax+278h], 0
   jz      loc_80EFE6B
   jmp     locret_80F010D
loc_80EFE6B:
   mov     eax, [ebp-0Ch]
   mov     edx, [ebp-0Ch]
   fld     dword [eax+304h]
   fsub    dword [edx+2E8h]
   fmul    dword [ebp-54h]
   mov     eax, [ebp-0Ch]
   fadd    dword [eax+2E8h]
   fstp    dword [ebp-3Ch]
   mov     eax, [ebp-0Ch]
   mov     edx, [ebp-0Ch]
   fld     dword [eax+308h]
   fsub    dword [edx+2ECh]
   fmul    dword [ebp-54h]
   mov     eax, [ebp-0Ch]
   fadd    dword [eax+2ECh]
   fstp    dword [ebp-40h]
   mov     eax, [ebp-0Ch]
   mov     edx, [ebp-0Ch]
   fld     dword [eax+30Ch]
   fsub    dword [edx+2F0h]
   fmul    dword [ebp-54h]
   mov     eax, [ebp-0Ch]
   fadd    dword [eax+2F0h]
   fstp    dword [ebp-44h]
   mov     eax, [ebp-0Ch]
   mov     edx, [ebp-0Ch]
   fld     dword [eax+310h]
   fsub    dword [edx+2F4h]
   fmul    dword [ebp-54h]
   mov     eax, [ebp-0Ch]
   fadd    dword [eax+2F4h]
   fstp    dword [ebp-48h]
   mov     eax, [ebp-0Ch]
   mov     edx, [ebp-0Ch]
   fld     dword [eax+314h]
   fsub    dword [edx+2F8h]
   fmul    dword [ebp-54h]
   mov     eax, [ebp-0Ch]
   fadd    dword [eax+2F8h]
   fstp    dword [ebp-4Ch]
   mov     eax, [ebp-0Ch]
   mov     edx, [ebp-0Ch]
   fld     dword [eax+318h]
   fsub    dword [edx+2FCh]
   fmul    dword [ebp-54h]
   mov     eax, [ebp-0Ch]
   fadd    dword [eax+2FCh]
   fstp    dword [ebp-50h]
   jmp     loc_80EFF7B
loc_80EFF33:
   mov     eax, [ebp-0Ch]
   mov     eax, [eax+2E8h]
   mov     [ebp-3Ch], eax
   mov     eax, [ebp-0Ch]
   mov     eax, [eax+2ECh]
   mov     [ebp-40h], eax
   mov     eax, [ebp-0Ch]
   mov     eax, [eax+2F0h]
   mov     [ebp-44h], eax
   mov     eax, [ebp-0Ch]
   mov     eax, [eax+2F4h]
   mov     [ebp-48h], eax
   mov     eax, [ebp-0Ch]
   mov     eax, [eax+2F8h]
   mov     [ebp-4Ch], eax
   mov     eax, [ebp-0Ch]
   mov     eax, [eax+2FCh]
   mov     [ebp-50h], eax
loc_80EFF7B:
   fld     dword [ebp-44h]
   fmul    dword [ebp+18h]
   fstp    dword [ebp-44h]
   fld     dword [ebp-48h]
   fmul    dword [ebp+18h]
   fstp    dword [ebp-48h]
   fld     dword [ebp-4Ch]
   fmul    dword [ebp+18h]
   fstp    dword [ebp-4Ch]
   fld     dword [ebp-50h]
   fmul    dword [ebp+18h]
   fstp    dword [ebp-50h]
   lea     eax, [ebp-28h]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   add     eax, 0E8h
   mov     [esp], eax
   call    sub_80A6062
   lea     eax, [ebp-28h]
   mov     [esp+8], eax
   fld     dword [ebp-58h]
   fld     dword [flt_814E7F4]
   fmulp   st1, st0
   fld1    
   fdivrp  st1, st0
   fstp    dword [esp+4]
   lea     eax, [ebp-28h]
   mov     [esp], eax
   call    VectorScale814
   mov     eax, [ebp-3Ch]
   mov     [esp+8], eax
   mov     eax, [ebp-3Ch]
   xor     eax, 80000000h
   mov     [esp+4], eax
   mov     eax, [ebp-28h]
   mov     [esp], eax
   call    I_fclamp032
   fstp    dword [ebp-28h]
   mov     eax, [ebp-3Ch]
   mov     [esp+8], eax
   mov     eax, [ebp-3Ch]
   xor     eax, 80000000h
   mov     [esp+4], eax
   mov     eax, [ebp-24h]
   mov     [esp], eax
   call    I_fclamp032
   fstp    dword [ebp-24h]
   fld     dword [ebp-24h]
   fmul    dword [ebp-4Ch]
   fstp    dword [ebp-34h]
   fld     dword [ebp-28h]
   fmul    dword [ebp-50h]
   fstp    dword [ebp-30h]
   mov     eax, [ebp-58h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp-40h]
   mov     [esp+8], eax
   mov     eax, [ebp+10h]
   add     eax, 4
   mov     eax, [eax]
   mov     [esp+4], eax
   mov     eax, [ebp-34h]
   mov     [esp], eax
   call    DiffTrack
   mov     eax, [ebp+10h]
   add     eax, 4
   fstp    dword [eax]
   mov     eax, [ebp-58h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp-40h]
   mov     [esp+8], eax
   mov     eax, [ebp+10h]
   add     eax, 8
   mov     eax, [eax]
   mov     [esp+4], eax
   mov     eax, [ebp-30h]
   mov     [esp], eax
   call    DiffTrack
   mov     eax, [ebp+10h]
   add     eax, 8
   fstp    dword [eax]
   fld     dword [ebp-28h]
   fmul    dword [ebp-44h]
   fstp    dword [ebp-28h]
   fld     dword [ebp-24h]
   fmul    dword [ebp-48h]
   fstp    dword [ebp-24h]
   mov     eax, [ebp-58h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp-40h]
   mov     [esp+8], eax
   mov     eax, [ebp+14h]
   mov     eax, [eax]
   mov     [esp+4], eax
   mov     eax, [ebp-28h]
   mov     [esp], eax
   call    DiffTrackAngle
   mov     eax, [ebp+14h]
   fstp    dword [eax]
   mov     eax, [ebp-58h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp-40h]
   mov     [esp+8], eax
   mov     eax, [ebp+14h]
   add     eax, 4
   mov     eax, [eax]
   mov     [esp+4], eax
   mov     eax, [ebp-24h]
   mov     [esp], eax
   call    DiffTrackAngle
   mov     eax, [ebp+14h]
   add     eax, 4
   fstp    dword [eax]
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   add     eax, 0E8h
   mov     [esp], eax
   call    VectorCopy932
locret_80F010D:
   leave   
   retn    


section .rdata
flt_814E7F4     dd 60.0
flt_814E7F0     dd 0.001
flt_8145ECC     dd 360.0
flt_8146088     dd 180.0
flt_8145EC4     dd 0.001
flt_8146074     dd 180.0
flt_8146078     dd 360.0
flt_814607C     dd -180.0
flt_8146080     dd 182.04445
flt_8146084     dd 0.0054931641
flt_814608C     dd 360.0
flt_8145EC8     dd 180.0
flt_8145ED0     dd -180.0