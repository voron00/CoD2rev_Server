extern BG_CheckProne
extern BG_AddPredictableEventToPlayerstate
extern sqrt
extern atan2
extern sin
extern cos


global PM_UpdatePronePitch


section .text


Fabs961:
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


sub_80A303A:
   push    ebp
   mov     ebp, esp
   sub     esp, 28h
   fld     dword [ebp+8]
   fld     qword [dbl_8145F90]
   fmulp   st1, st0
   fstp    dword [ebp-4]
   lea     eax, [ebp-0Ch]
   mov     [esp+8], eax
   lea     eax, [ebp-8]
   mov     [esp+4], eax
   mov     eax, [ebp-4]
   mov     [esp], eax
   call    sub_80A8C1C
   cmp     dword [ebp+0Ch], 0
   jz      loc_80A308D
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp-0Ch]
   mov     [edx], eax
   mov     edx, [ebp+0Ch]
   add     edx, 4
   mov     eax, [ebp-8]
   mov     [edx], eax
   mov     edx, [ebp+0Ch]
   add     edx, 8
   mov     eax, 0
   mov     [edx], eax
loc_80A308D:
   cmp     dword [ebp+10h], 0
   jz      locret_80A30B8
   mov     edx, [ebp+10h]
   mov     eax, [ebp-8]
   mov     [edx], eax
   mov     edx, [ebp+10h]
   add     edx, 4
   mov     eax, [ebp-0Ch]
   xor     eax, 80000000h
   mov     [edx], eax
   mov     edx, [ebp+10h]
   add     edx, 8
   mov     eax, 0
   mov     [edx], eax
locret_80A30B8:
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


sub_80A8EFC:
   push    ebp
   mov     ebp, esp
   mov     eax, [ebp+8]
   mov     edx, [ebp+0Ch]
   fld     dword [eax]
   fmul    dword [edx]
   mov     eax, [ebp+8]
   add     eax, 4
   mov     edx, [ebp+0Ch]
   add     edx, 4
   fld     dword [eax]
   fmul    dword [edx]
   faddp   st1, st0
   mov     eax, [ebp+8]
   add     eax, 8
   mov     edx, [ebp+0Ch]
   add     edx, 8
   fld     dword [eax]
   fmul    dword [edx]
   faddp   st1, st0
   pop     ebp
   retn    


sub_80E733C:
   push    ebp
   mov     ebp, esp
   mov     eax, [ebp+8]
   shr     eax, 1Fh
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


sub_80A8EA8:
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


AngleDelta:
   push    ebp
   mov     ebp, esp
   sub     esp, 8
   fld     dword [ebp+8]
   fsub    dword [ebp+0Ch]
   fstp    dword [esp]
   call    AngleNormalize180
   leave   
   retn    


sub_80E6F12:
   push    ebp
   mov     ebp, esp
   sub     esp, 8
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    sub_80E733C
   mov     [ebp-4], eax
   mov     eax, [ebp-4]
   add     eax, eax
   neg     eax
   mov     [ebp-4], eax
   inc     dword [ebp-4]
   mov     eax, [ebp-4]
   leave   
   retn    


ProjectPointOnPlane:
   push    ebp
   mov     ebp, esp
   sub     esp, 18h
   mov     eax, [ebp+8]
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    sub_80A8EFC
   fstp    dword [ebp-8]
   mov     eax, [ebp-8]
   xor     eax, 80000000h
   mov     [ebp-4], eax
   mov     eax, [ebp+10h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+8], eax
   mov     eax, [ebp-4]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    sub_80A8EA8
   leave   
   retn    


AngleNormalize180Accurate:
   push    ebp
   mov     ebp, esp
   sub     esp, 4
   fld     dword [ebp+8]
   fld     dword [flt_8146094]
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_80A61EA
   jmp     loc_80A6210
loc_80A61EA:
   fld     dword [ebp+8]
   fld     dword [flt_8146098]
   faddp   st1, st0
   fstp    dword [ebp+8]
   fld     dword [ebp+8]
   fld     dword [flt_8146094]
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_80A61EA
   mov     eax, [ebp+8]
   mov     [ebp-4], eax
   jmp     loc_80A6252
loc_80A6210:
   fld     dword [ebp+8]
   fld     dword [flt_814609C]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80A6224
   jmp     loc_80A624C
loc_80A6224:
   fld     dword [ebp+8]
   fld     dword [flt_8146098]
   fsubp   st1, st0
   fstp    dword [ebp+8]
   fld     dword [ebp+8]
   fld     dword [flt_814609C]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80A6224
   mov     eax, [ebp+8]
   mov     [ebp-4], eax
   jmp     loc_80A6252
loc_80A624C:
   mov     eax, [ebp+8]
   mov     [ebp-4], eax
loc_80A6252:
   fld     dword [ebp-4]
   leave   
   retn    


PitchForYawOnNormal:
   push    ebp
   mov     ebp, esp
   sub     esp, 38h
   mov     dword [esp+8], 0
   lea     eax, [ebp-18h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    sub_80A303A
   lea     eax, [ebp-28h]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   lea     eax, [ebp-18h]
   mov     [esp], eax
   call    ProjectPointOnPlane
   lea     eax, [ebp-28h]
   mov     [esp], eax
   call    sub_80A2AF6
   leave   
   retn    


PM_UpdatePronePitch:
   push    ebp
   mov     ebp, esp
   push    esi
   push    ebx
   sub     esp, 50h
   mov     eax, [ebp+8]
   mov     eax, [eax]
   mov     [ebp-14h], eax
   mov     eax, [ebp-14h]
   movzx   eax, byte [eax+0Ch]
   xor     al, 1
   and     al, 1
   test    al, al
   jz      loc_80E5500
   jmp     loc_80E5819
loc_80E5500:
   mov     eax, [ebp-14h]
   cmp     dword [eax+60h], 3FFh
   jnz     loc_80E5606
   mov     eax, 42840000h
   mov     [esp+34h], eax
   mov     dword [esp+30h], 0
   mov     eax, [ebp+8]
   movzx   eax, byte [eax+0E4h]
   mov     [esp+2Ch], eax
   mov     eax, [ebp+0Ch]
   cmp     dword [eax+30h], 0
   jz      loc_80E5543
   mov     eax, [ebp+0Ch]
   add     eax, 3Ch
   mov     [ebp-1Ch], eax
   jmp     loc_80E554A
loc_80E5543:
   mov     dword [ebp-1Ch], 0
loc_80E554A:
   mov     eax, [ebp-1Ch]
   mov     [esp+28h], eax
   mov     eax, [ebp-14h]
   cmp     dword [eax+60h], 3FFh
   setnz   al
   movzx   eax, al
   mov     [esp+24h], eax
   mov     dword [esp+20h], 1
   mov     eax, [ebp-14h]
   add     eax, 5B0h
   mov     [esp+1Ch], eax
   mov     eax, [ebp-14h]
   add     eax, 5ACh
   mov     [esp+18h], eax
   mov     eax, [ebp-14h]
   add     eax, 5A8h
   mov     [esp+14h], eax
   mov     eax, [ebp-14h]
   mov     eax, [eax+584h]
   mov     [esp+10h], eax
   mov     eax, 41F00000h
   mov     [esp+0Ch], eax
   mov     eax, [ebp-14h]
   mov     eax, [eax+578h]
   mov     [esp+8], eax
   mov     eax, [ebp-14h]
   add     eax, 14h
   mov     [esp+4], eax
   mov     eax, [ebp-14h]
   mov     eax, [eax+0CCh]
   mov     [esp], eax
   call    BG_CheckProne
   mov     [ebp-18h], eax
   cmp     dword [ebp-18h], 0
   jnz     loc_80E563F
   mov     eax, [ebp-14h]
   mov     [esp+8], eax
   mov     dword [esp+4], 0
   mov     dword [esp], 8Dh
   call    BG_AddPredictableEventToPlayerstate
   mov     edx, [ebp-14h]
   mov     eax, [ebp-14h]
   mov     eax, [eax+0Ch]
   or      eax, 10000h
   mov     [edx+0Ch], eax
   jmp     loc_80E563F
loc_80E5606:
   mov     eax, [ebp+0Ch]
   cmp     dword [eax+30h], 0
   jz      loc_80E563F
   mov     eax, [ebp+0Ch]
   fld     dword [eax+44h]
   fld     dword [flt_814DFE4]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E5624
   jmp     loc_80E563F
loc_80E5624:
   mov     eax, [ebp-14h]
   mov     [esp+8], eax
   mov     dword [esp+4], 0
   mov     dword [esp], 8Dh
   call    BG_AddPredictableEventToPlayerstate
loc_80E563F:
   mov     eax, [ebp+0Ch]
   cmp     dword [eax+30h], 0
   jz      loc_80E5668
   mov     eax, [ebp+0Ch]
   add     eax, 3Ch
   mov     [esp+4], eax
   mov     eax, [ebp-14h]
   mov     eax, [eax+584h]
   mov     [esp], eax
   call    PitchForYawOnNormal
   fstp    dword [ebp-0Ch]
   jmp     loc_80E5670
loc_80E5668:
   mov     eax, 0
   mov     [ebp-0Ch], eax
loc_80E5670:
   mov     eax, [ebp-14h]
   mov     eax, [eax+588h]
   mov     [esp+4], eax
   mov     eax, [ebp-0Ch]
   mov     [esp], eax
   call    AngleDelta
   fstp    dword [ebp-10h]
   fld     dword [ebp-10h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80E56A0
   jp      loc_80E56A0
   jmp     loc_80E572C
loc_80E56A0:
   mov     eax, [ebp-10h]
   mov     [esp], eax
   call    Fabs961
   mov     eax, [ebp+0Ch]
   fld     dword [eax+24h]
   fld     dword [flt_814DFE8]
   fmulp   st1, st0
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E56C4
   jmp     loc_80E56FD
loc_80E56C4:
   mov     ebx, [ebp-14h]
   mov     esi, [ebp-14h]
   mov     eax, [ebp-10h]
   mov     [esp], eax
   call    sub_80E6F12
   push    eax
   fild    dword [esp]
   lea     esp, [esp+4]
   mov     eax, [ebp+0Ch]
   fld     dword [eax+24h]
   fld     dword [flt_814DFE8]
   fmulp   st1, st0
   fmulp   st1, st0
   fld     dword [esi+588h]
   faddp   st1, st0
   fstp    dword [ebx+588h]
   jmp     loc_80E5712
loc_80E56FD:
   mov     edx, [ebp-14h]
   mov     eax, [ebp-14h]
   fld     dword [eax+588h]
   fadd    dword [ebp-10h]
   fstp    dword [edx+588h]
loc_80E5712:
   mov     ebx, [ebp-14h]
   mov     eax, [ebp-14h]
   mov     eax, [eax+588h]
   mov     [esp], eax
   call    AngleNormalize180Accurate
   fstp    dword [ebx+588h]
loc_80E572C:
   mov     eax, [ebp+0Ch]
   cmp     dword [eax+30h], 0
   jz      loc_80E5755
   mov     eax, [ebp+0Ch]
   add     eax, 3Ch
   mov     [esp+4], eax
   mov     eax, [ebp-14h]
   mov     eax, [eax+0ECh]
   mov     [esp], eax
   call    PitchForYawOnNormal
   fstp    dword [ebp-0Ch]
   jmp     loc_80E575D
loc_80E5755:
   mov     eax, 0
   mov     [ebp-0Ch], eax
loc_80E575D:
   mov     eax, [ebp-14h]
   mov     eax, [eax+58Ch]
   mov     [esp+4], eax
   mov     eax, [ebp-0Ch]
   mov     [esp], eax
   call    AngleDelta
   fstp    dword [ebp-10h]
   fld     dword [ebp-10h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80E578D
   jp      loc_80E578D
   jmp     loc_80E5819
loc_80E578D:
   mov     eax, [ebp-10h]
   mov     [esp], eax
   call    Fabs961
   mov     eax, [ebp+0Ch]
   fld     dword [eax+24h]
   fld     dword [flt_814DFE8]
   fmulp   st1, st0
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E57B1
   jmp     loc_80E57EA
loc_80E57B1:
   mov     ebx, [ebp-14h]
   mov     esi, [ebp-14h]
   mov     eax, [ebp-10h]
   mov     [esp], eax
   call    sub_80E6F12
   push    eax
   fild    dword [esp]
   lea     esp, [esp+4]
   mov     eax, [ebp+0Ch]
   fld     dword [eax+24h]
   fld     dword [flt_814DFE8]
   fmulp   st1, st0
   fmulp   st1, st0
   fld     dword [esi+58Ch]
   faddp   st1, st0
   fstp    dword [ebx+58Ch]
   jmp     loc_80E57FF
loc_80E57EA:
   mov     edx, [ebp-14h]
   mov     eax, [ebp-14h]
   fld     dword [eax+58Ch]
   fadd    dword [ebp-10h]
   fstp    dword [edx+58Ch]
loc_80E57FF:
   mov     ebx, [ebp-14h]
   mov     eax, [ebp-14h]
   mov     eax, [eax+58Ch]
   mov     [esp], eax
   call    AngleNormalize180Accurate
   fstp    dword [ebx+58Ch]
loc_80E5819:
   add     esp, 50h
   pop     ebx
   pop     esi
   pop     ebp
   retn    


section .rdata
flt_814DFE8     dd 70.0
flt_814DFE4     dd 0.69999999
flt_814609C     dd 180.0
flt_8146088     dd 180.0
flt_8146098     dd 360.0
flt_8146094     dd -180.0
flt_814608C     dd 360.0
flt_8146084     dd 0.0054931641
flt_8145F38     dd 360.0
flt_8146080     dd 182.04445

dbl_8145F30     dq 3.141592653589793
dbl_8145F90     dq 0.0174532925199433
dbl_8145F28     dq -180.0