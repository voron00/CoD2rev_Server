extern pmoveHandlers
extern bg_prone_yawcap
extern bg_ladder_yawcap
extern player_view_pitch_up
extern player_view_pitch_down
extern mantle_enable
extern mantle_view_yawcap
extern BG_CheckProne
extern PM_GetEffectiveStance
extern sin
extern cos
extern atan2
extern sqrt


global PM_UpdateViewAngles


section .text


sub_80B6D74:
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


sub_80E733C:
   push    ebp
   mov     ebp, esp
   mov     eax, [ebp+8]
   shr     eax, 1Fh
   pop     ebp
   retn    


sub_80B6D38:
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


sub_80B6D98:
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


sub_80B66A6:
   push    ebp
   mov     ebp, esp
   sub     esp, 8
   fld1    
   fsub    dword [ebp+8]
   fstp    dword [esp]
   call    sub_80B6D38
   fld1    
   fsubrp  st1, st0
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


VectorSet761:
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


VectorCopy856:
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


AddLeanToPosition:
   push    ebp
   mov     ebp, esp
   sub     esp, 48h
   fld     dword [ebp+10h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80B66D8
   jp      loc_80B66D8
   jmp     locret_80B6756
loc_80B66D8:
   mov     eax, [ebp+10h]
   mov     [esp], eax
   call    GetLeanFraction
   fstp    dword [ebp-0Ch]
   fld     dword [ebp-0Ch]
   fmul    dword [ebp+14h]
   fstp    dword [esp+0Ch]
   mov     eax, [ebp+0Ch]
   mov     [esp+8], eax
   mov     eax, 0
   mov     [esp+4], eax
   lea     eax, [ebp-38h]
   mov     [esp], eax
   call    sub_80B6D74
   mov     dword [esp+0Ch], 0
   lea     eax, [ebp-28h]
   mov     [esp+8], eax
   mov     dword [esp+4], 0
   lea     eax, [ebp-38h]
   mov     [esp], eax
   call    AngleVectors
   fld     dword [ebp-0Ch]
   fmul    dword [ebp+18h]
   fstp    dword [ebp-0Ch]
   mov     eax, [ebp+8]
   mov     [esp+0Ch], eax
   lea     eax, [ebp-28h]
   mov     [esp+8], eax
   mov     eax, [ebp-0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    sub_80B6D98
locret_80B6756:
   leave   
   retn    


sub_80DBD56:
   push    ebp
   mov     ebp, esp
   push    ebx
   sub     esp, 24h
   mov     eax, [mantle_enable]
   cmp     byte [eax+8], 0
   jnz     loc_80DBD6D
   jmp     loc_80DBE8F
loc_80DBD6D:
   mov     eax, [ebp+8]
   mov     eax, [eax+0ECh]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     eax, [eax+5BCh]
   mov     [esp], eax
   call    AngleDelta
   fstp    dword [ebp-8]
   mov     eax, [mantle_view_yawcap]
   mov     eax, [eax+8]
   mov     [ebp-1Ch], eax
   fld     dword [ebp-1Ch]
   fchs    
   fld     dword [ebp-8]
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_80DBDAA
   jmp     loc_80DBDC0
loc_80DBDAA:
   mov     eax, [mantle_view_yawcap]
   fld     dword [ebp-8]
   fld     dword [eax+8]
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_80DBE8F
loc_80DBDC0:
   nop     
loc_80DBDC1:
   mov     eax, [mantle_view_yawcap]
   mov     eax, [eax+8]
   mov     [ebp-1Ch], eax
   fld     dword [ebp-1Ch]
   fchs    
   fld     dword [ebp-8]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80DBDDF
   jmp     loc_80DBDEF
loc_80DBDDF:
   mov     eax, [mantle_view_yawcap]
   fld     dword [ebp-8]
   fadd    dword [eax+8]
   fstp    dword [ebp-8]
   jmp     loc_80DBDC1
loc_80DBDEF:
   nop     
loc_80DBDF0:
   mov     eax, [mantle_view_yawcap]
   fld     dword [ebp-8]
   fld     dword [eax+8]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80DBE06
   jmp     loc_80DBE16
loc_80DBE06:
   mov     eax, [mantle_view_yawcap]
   fld     dword [ebp-8]
   fsub    dword [eax+8]
   fstp    dword [ebp-8]
   jmp     loc_80DBDF0
loc_80DBE16:
   mov     eax, [mantle_view_yawcap]
   fld     dword [eax+8]
   fstp    dword [ebp-18h]
   fld     dword [ebp-8]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80DBE31
   jmp     loc_80DBE35
loc_80DBE31:
   xor     byte [ebp-15h], 80h
loc_80DBE35:
   fld     dword [ebp-18h]
   fstp    dword [ebp-0Ch]
   mov     ecx, [ebp+8]
   mov     edx, [ebp+8]
   fld     dword [ebp-8]
   fld     dword [flt_814C7CC]
   fmulp   st1, st0
   fnstcw  word [ebp-0Eh]
   movzx   eax, word [ebp-0Eh]
   or      ax, 0C00h
   mov     [ebp-10h], ax
   fldcw   word [ebp-10h]
   fistp   dword [ebp-14h]
   fldcw   word [ebp-0Eh]
   mov     eax, [ebp-14h]
   and     eax, 0FFFFh
   add     eax, [edx+58h]
   mov     [ecx+58h], eax
   mov     ebx, [ebp+8]
   mov     eax, [ebp+8]
   fld     dword [eax+5BCh]
   fadd    dword [ebp-0Ch]
   fstp    dword [esp]
   call    AngleNormalize360Accurate
   fstp    dword [ebx+0ECh]
loc_80DBE8F:
   add     esp, 24h
   pop     ebx
   pop     ebp
   retn    


vectoyaw:
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
   jp      loc_80A2A11
   jz      loc_80A2A13
loc_80A2A11:
   jmp     loc_80A2A31
loc_80A2A13:
   mov     eax, [ebp+8]
   fld     dword [eax]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jp      loc_80A2A25
   jz      loc_80A2A27
loc_80A2A25:
   jmp     loc_80A2A31
loc_80A2A27:
   mov     eax, 0
   mov     [ebp-4], eax
   jmp     loc_80A2A79
loc_80A2A31:
   mov     eax, [ebp+8]
   fld     dword [eax]
   fstp    qword [esp+8]
   mov     eax, [ebp+8]
   add     eax, 4
   fld     dword [eax]
   fstp    qword [esp]
   call    atan2
   fld     qword [dbl_8145F00]
   fmulp   st1, st0
   fld     qword [dbl_8145F08]
   fdivp   st1, st0
   fstp    dword [ebp-4]
   fld     dword [ebp-4]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80A2A6B
   jmp     loc_80A2A79
loc_80A2A6B:
   fld     dword [ebp-4]
   fld     dword [flt_8145F10]
   faddp   st1, st0
   fstp    dword [ebp-4]
loc_80A2A79:
   mov     eax, [ebp-4]
   mov     [ebp-0Ch], eax
   fld     dword [ebp-0Ch]
   leave   
   retn    


sub_80E4688:
   push    ebp
   mov     ebp, esp
   sub     esp, 58h
   mov     eax, [ebp+10h]
   mov     [ebp-1], al
   mov     eax, [ebp+8]
   mov     eax, [eax+0ECh]
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    AngleDelta
   fstp    dword [ebp-8]
   mov     eax, [ebp-8]
   mov     [esp], eax
   call    Fabs961
   fld     dword [flt_814DFB4]
   fdivp   st1, st0
   fstp    dword [ebp-10h]
   fld1    
   fsub    dword [ebp-10h]
   fmul    dword [ebp-8]
   fld     dword [ebp+0Ch]
   fsubrp  st1, st0
   fstp    dword [esp]
   call    AngleNormalize360Accurate
   fstp    dword [ebp-0Ch]
   fld     dword [ebp-10h]
   fld     dword [flt_814DFB8]
   fmulp   st1, st0
   fld1    
   fsub    dword [ebp-10h]
   fld     dword [flt_814DFBC]
   fmulp   st1, st0
   faddp   st1, st0
   fstp    dword [ebp-14h]
   mov     eax, [ebp-14h]
   mov     [esp+34h], eax
   mov     dword [esp+30h], 0
   movzx   eax, byte [ebp-1]
   mov     [esp+2Ch], eax
   mov     dword [esp+28h], 0
   mov     eax, [ebp+8]
   cmp     dword [eax+60h], 3FFh
   setnz   al
   movzx   eax, al
   mov     [esp+24h], eax
   mov     dword [esp+20h], 1
   mov     eax, [ebp+8]
   add     eax, 5B0h
   mov     [esp+1Ch], eax
   mov     eax, [ebp+8]
   add     eax, 5ACh
   mov     [esp+18h], eax
   mov     eax, [ebp+8]
   add     eax, 5A8h
   mov     [esp+14h], eax
   mov     eax, [ebp-0Ch]
   mov     [esp+10h], eax
   mov     eax, 41F00000h
   mov     [esp+0Ch], eax
   mov     eax, [ebp+8]
   mov     eax, [eax+578h]
   mov     [esp+8], eax
   mov     eax, [ebp+8]
   add     eax, 14h
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     eax, [eax+0CCh]
   mov     [esp], eax
   call    BG_CheckProne
   leave   
   retn    


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


AngleNormalize360Accurate:
   push    ebp
   mov     ebp, esp
   sub     esp, 4
   fld     dword [ebp+8]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80A6168
   jmp     loc_80A618A
loc_80A6168:
   fld     dword [ebp+8]
   fld     dword [flt_8146090]
   faddp   st1, st0
   fstp    dword [ebp+8]
   fld     dword [ebp+8]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80A6168
   mov     eax, [ebp+8]
   mov     [ebp-4], eax
   jmp     loc_80A61CC
loc_80A618A:
   fld     dword [ebp+8]
   fld     dword [flt_8146090]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_80A619E
   jmp     loc_80A61C6
loc_80A619E:
   fld     dword [ebp+8]
   fld     dword [flt_8146090]
   fsubp   st1, st0
   fstp    dword [ebp+8]
   fld     dword [ebp+8]
   fld     dword [flt_8146090]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_80A619E
   mov     eax, [ebp+8]
   mov     [ebp-4], eax
   jmp     loc_80A61CC
loc_80A61C6:
   mov     eax, [ebp+8]
   mov     [ebp-4], eax
loc_80A61CC:
   fld     dword [ebp-4]
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


sub_80E42D0:
   push    ebp
   mov     ebp, esp
   push    ebx
   sub     esp, 0C4h
   mov     dword [ebp-4Ch], 0
   mov     eax, 0
   mov     [ebp-50h], eax
   mov     edx, [ebp+10h]
   mov     al, 0C0h
   and     al, [edx+4]
   test    al, al
   jz      loc_80E434D
   mov     eax, [ebp+8]
   mov     edx, 0FFFF8000h
   movzx   eax, word [eax+0Ch]
   and     eax, edx
   test    ax, ax
   jnz     loc_80E434D
   mov     eax, [ebp+8]
   cmp     dword [eax+4], 5
   jg      loc_80E434D
   mov     eax, [ebp+8]
   cmp     dword [eax+60h], 3FFh
   jnz     loc_80E4328
   mov     eax, [ebp+8]
   cmp     dword [eax+4], 1
   jz      loc_80E4328
   jmp     loc_80E434D
loc_80E4328:
   mov     eax, [ebp+10h]
   mov     eax, [eax+4]
   and     eax, 40h
   test    eax, eax
   jz      loc_80E433A
   lea     eax, [ebp-4Ch]
   dec     dword [eax]
loc_80E433A:
   mov     eax, [ebp+10h]
   mov     dl, 80h
   and     dl, [eax+4]
   mov     al, dl
   test    al, al
   jz      loc_80E434D
   lea     eax, [ebp-4Ch]
   inc     dword [eax]
loc_80E434D:
   mov     eax, [ebp+8]
   mov     eax, [eax+0A0h]
   and     eax, 300h
   test    eax, eax
   jz      loc_80E4366
   mov     dword [ebp-4Ch], 0
loc_80E4366:
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    PM_GetEffectiveStance
   mov     [ebp-90h], eax
   cmp     dword [ebp-90h], 1
   jnz     loc_80E438D
   mov     eax, 3E800000h
   mov     [ebp-8Ch], eax
   jmp     loc_80E4398
loc_80E438D:
   mov     eax, 3F000000h
   mov     [ebp-8Ch], eax
loc_80E4398:
   mov     eax, [ebp+8]
   mov     eax, [eax+4Ch]
   mov     [ebp-50h], eax
   cmp     dword [ebp-4Ch], 0
   jnz     loc_80E443C
   fld     dword [ebp-50h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E43BB
   jmp     loc_80E43F2
loc_80E43BB:
   fld     dword [ebp+0Ch]
   fld     dword [flt_814DFAC]
   fdivp   st1, st0
   fmul    dword [ebp-8Ch]
   fld     dword [ebp-50h]
   fsubrp  st1, st0
   fstp    dword [ebp-50h]
   fld     dword [ebp-50h]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E43E5
   jmp     loc_80E44F5
loc_80E43E5:
   mov     eax, 0
   mov     [ebp-50h], eax
   jmp     loc_80E44F5
loc_80E43F2:
   fld     dword [ebp-50h]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E4403
   jmp     loc_80E44F5
loc_80E4403:
   fld     dword [ebp+0Ch]
   fld     dword [flt_814DFAC]
   fdivp   st1, st0
   fmul    dword [ebp-8Ch]
   fld     dword [ebp-50h]
   faddp   st1, st0
   fstp    dword [ebp-50h]
   fld     dword [ebp-50h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E442F
   jmp     loc_80E44F5
loc_80E442F:
   mov     eax, 0
   mov     [ebp-50h], eax
   jmp     loc_80E44F5
loc_80E443C:
   cmp     dword [ebp-4Ch], 0
   jle     loc_80E448C
   fld     dword [ebp-50h]
   fld     dword [ebp-8Ch]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E4454
   jmp     loc_80E446D
loc_80E4454:
   fld     dword [ebp+0Ch]
   fld     dword [flt_814DFB0]
   fdivp   st1, st0
   fmul    dword [ebp-8Ch]
   fld     dword [ebp-50h]
   faddp   st1, st0
   fstp    dword [ebp-50h]
loc_80E446D:
   fld     dword [ebp-50h]
   fld     dword [ebp-8Ch]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E4481
   jmp     loc_80E44F5
loc_80E4481:
   mov     eax, [ebp-8Ch]
   mov     [ebp-50h], eax
   jmp     loc_80E44F5
loc_80E448C:
   mov     eax, [ebp-8Ch]
   mov     [ebp-9Ch], eax
   fld     dword [ebp-9Ch]
   fchs    
   fld     dword [ebp-50h]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E44AC
   jmp     loc_80E44C5
loc_80E44AC:
   fld     dword [ebp+0Ch]
   fld     dword [flt_814DFB0]
   fdivp   st1, st0
   fmul    dword [ebp-8Ch]
   fld     dword [ebp-50h]
   fsubrp  st1, st0
   fstp    dword [ebp-50h]
loc_80E44C5:
   mov     eax, [ebp-8Ch]
   mov     [ebp-9Ch], eax
   fld     dword [ebp-9Ch]
   fchs    
   fld     dword [ebp-50h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E44E7
   jmp     loc_80E44F5
loc_80E44E7:
   mov     eax, [ebp-8Ch]
   xor     eax, 80000000h
   mov     [ebp-50h], eax
loc_80E44F5:
   mov     edx, [ebp+8]
   mov     eax, [ebp-50h]
   mov     [edx+4Ch], eax
   mov     eax, [ebp+8]
   fld     dword [eax+4Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80E4516
   jp      loc_80E4516
   jmp     loc_80E467E
loc_80E4516:
   mov     eax, [ebp+8]
   mov     eax, [eax+4Ch]
   mov     [esp], eax
   call    sub_80E6F12
   push    eax
   fild    dword [esp]
   lea     esp, [esp+4]
   fstp    dword [ebp-94h]
   lea     eax, [ebp-18h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   add     eax, 14h
   mov     [esp], eax
   call    VectorCopy856
   mov     eax, [ebp+8]
   fld     dword [ebp-10h]
   fadd    dword [eax+0F8h]
   fstp    dword [ebp-10h]
   lea     eax, [ebp-28h]
   mov     [esp+4], eax
   lea     eax, [ebp-18h]
   mov     [esp], eax
   call    VectorCopy856
   mov     eax, 41A00000h
   mov     [esp+10h], eax
   mov     eax, 41800000h
   mov     [esp+0Ch], eax
   mov     eax, [ebp-94h]
   mov     [esp+8], eax
   mov     eax, [ebp+8]
   mov     eax, [eax+0ECh]
   mov     [esp+4], eax
   lea     eax, [ebp-28h]
   mov     [esp], eax
   call    AddLeanToPosition
   mov     eax, 0C1000000h
   mov     [esp+0Ch], eax
   mov     eax, 0C1000000h
   mov     [esp+8], eax
   mov     eax, 0C1000000h
   mov     [esp+4], eax
   lea     eax, [ebp-38h]
   mov     [esp], eax
   call    VectorSet761
   mov     eax, 41000000h
   mov     [esp+0Ch], eax
   mov     eax, 41000000h
   mov     [esp+8], eax
   mov     eax, 41000000h
   mov     [esp+4], eax
   lea     eax, [ebp-48h]
   mov     [esp], eax
   call    VectorSet761
   mov     dword [esp+18h], 2810011h
   mov     eax, [ebp+8]
   mov     eax, [eax+0CCh]
   mov     [esp+14h], eax
   lea     eax, [ebp-28h]
   mov     [esp+10h], eax
   lea     eax, [ebp-48h]
   mov     [esp+0Ch], eax
   lea     eax, [ebp-38h]
   mov     [esp+8], eax
   lea     eax, [ebp-18h]
   mov     [esp+4], eax
   lea     eax, [ebp-88h]
   mov     [esp], eax
   mov     eax, [ebp+14h]
   call    eax
   mov     eax, [ebp-88h]
   mov     [esp], eax
   call    sub_80B66A6
   fstp    dword [ebp-94h]
   mov     eax, [ebp+8]
   mov     eax, [eax+4Ch]
   mov     [esp], eax
   call    Fabs961
   fld     dword [ebp-94h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E465A
   jmp     loc_80E467E
loc_80E465A:
   mov     ebx, [ebp+8]
   mov     eax, [ebp+8]
   mov     eax, [eax+4Ch]
   mov     [esp], eax
   call    sub_80E6F12
   push    eax
   fild    dword [esp]
   lea     esp, [esp+4]
   fld     dword [ebp-94h]
   fmulp   st1, st0
   fstp    dword [ebx+4Ch]
loc_80E467E:
   add     esp, 0C4h
   pop     ebx
   pop     ebp
   retn    


PM_UpdateViewAngles:
   push    ebp
   mov     ebp, esp
   push    edi
   push    esi
   push    ebx
   sub     esp, 7Ch
   mov     eax, [ebp+14h]
   mov     [ebp-0Dh], al
   mov     eax, [ebp+8]
   cmp     dword [eax+4], 5
   jnz     loc_80E47B1
   jmp     loc_80E54D3
loc_80E47B1:
   mov     eax, [ebp+8]
   cmp     dword [eax+4], 5
   jle     loc_80E4840
   mov     eax, [ebp+10h]
   mov     edx, [ebp+8]
   movzx   ecx, word [eax+10h]
   movzx   eax, word [edx+58h]
   lea     eax, [eax+ecx]
   mov     [ebp-10h], ax
   mov     eax, [ebp+8]
   cmp     dword [eax+130h], 3E7h
   jnz     loc_80E480B
   mov     edx, [ebp+8]
   fild    word [ebp-10h]
   fld     dword [flt_814DFC0]
   fmulp   st1, st0
   fnstcw  word [ebp-46h]
   movzx   eax, word [ebp-46h]
   or      ax, 0C00h
   mov     [ebp-48h], ax
   fldcw   word [ebp-48h]
   fistp   dword [edx+130h]
   fldcw   word [ebp-46h]
loc_80E480B:
   movzx   edx, byte [ebp-0Dh]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   mov     eax, dword [pmoveHandlers+eax]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+10h]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    sub_80E42D0
   jmp     loc_80E54D3
loc_80E4840:
   mov     eax, [ebp+8]
   mov     eax, [eax+0ECh]
   mov     [ebp-28h], eax
   mov     eax, [player_view_pitch_up]
   fld     dword [eax+8]
   fld     dword [flt_814DFC4]
   fmulp   st1, st0
   fnstcw  word [ebp-46h]
   movzx   eax, word [ebp-46h]
   or      ax, 0C00h
   mov     [ebp-48h], ax
   fldcw   word [ebp-48h]
   fistp   dword [ebp-4Ch]
   fldcw   word [ebp-46h]
   mov     eax, [ebp-4Ch]
   and     eax, 0FFFFh
   mov     [ebp-40h], eax
   mov     eax, [player_view_pitch_down]
   fld     dword [eax+8]
   fld     dword [flt_814DFC4]
   fmulp   st1, st0
   fldcw   word [ebp-48h]
   fistp   dword [ebp-4Ch]
   fldcw   word [ebp-46h]
   mov     eax, [ebp-4Ch]
   and     eax, 0FFFFh
   mov     [ebp-44h], eax
   mov     dword [ebp-14h], 0
loc_80E48AA:
   cmp     dword [ebp-14h], 2
   jle     loc_80E48B5
   jmp     loc_80E4958
loc_80E48B5:
   mov     ebx, [ebp+10h]
   mov     edx, [ebp-14h]
   mov     ecx, [ebp+8]
   mov     eax, [ebp-14h]
   movzx   edx, word [ebx+edx*4+0Ch]
   movzx   eax, word [ecx+eax*4+54h]
   lea     eax, [eax+edx]
   mov     [ebp-10h], ax
   cmp     dword [ebp-14h], 0
   jnz     loc_80E4936
   movsx   eax, word [ebp-10h]
   cmp     eax, [ebp-44h]
   jle     loc_80E4903
   mov     ebx, [ebp+8]
   mov     ecx, [ebp-14h]
   mov     edx, [ebp+10h]
   mov     eax, [ebp-14h]
   mov     edx, [edx+eax*4+0Ch]
   mov     eax, [ebp-44h]
   sub     eax, edx
   mov     [ebx+ecx*4+54h], eax
   mov     eax, [ebp-44h]
   mov     [ebp-10h], ax
   jmp     loc_80E4936
loc_80E4903:
   movsx   eax, word [ebp-10h]
   mov     edx, [ebp-40h]
   neg     edx
   cmp     eax, edx
   jge     loc_80E4936
   mov     esi, [ebp+8]
   mov     ebx, [ebp-14h]
   mov     eax, [ebp-40h]
   mov     ecx, eax
   neg     ecx
   mov     edx, [ebp+10h]
   mov     eax, [ebp-14h]
   sub     ecx, [edx+eax*4+0Ch]
   mov     eax, ecx
   mov     [esi+ebx*4+54h], eax
   mov     eax, [ebp-40h]
   neg     eax
   mov     [ebp-10h], ax
loc_80E4936:
   mov     edx, [ebp+8]
   mov     eax, [ebp-14h]
   fild    word [ebp-10h]
   fld     dword [flt_814DFC0]
   fmulp   st1, st0
   fstp    dword [edx+eax*4+0E8h]
   lea     eax, [ebp-14h]
   inc     dword [eax]
   jmp     loc_80E48AA
loc_80E4958:
   mov     eax, [ebp+8]
   mov     eax, [eax+0ECh]
   mov     [ebp-2Ch], eax
   mov     eax, [ebp+8]
   mov     eax, [eax+0A0h]
   and     eax, 300h
   test    eax, eax
   jz      loc_80E4AF0
   mov     dword [ebp-14h], 0
loc_80E4981:
   cmp     dword [ebp-14h], 1
   jle     loc_80E498C
   jmp     loc_80E54D3
loc_80E498C:
   mov     edx, [ebp+8]
   mov     eax, [ebp-14h]
   mov     eax, [edx+eax*4+0E8h]
   mov     [esp+4], eax
   mov     edx, [ebp+8]
   mov     eax, [ebp-14h]
   mov     eax, [edx+eax*4+10Ch]
   mov     [esp], eax
   call    AngleDelta
   fstp    dword [ebp-18h]
   mov     edx, [ebp+8]
   mov     eax, [ebp-14h]
   fld     dword [ebp-18h]
   fld     dword [edx+eax*4+114h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E49F4
   mov     edx, [ebp+8]
   mov     eax, [ebp-14h]
   mov     eax, [edx+eax*4+114h]
   mov     [ebp-50h], eax
   fld     dword [ebp-50h]
   fchs    
   fld     dword [ebp-18h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E49F4
   jmp     loc_80E4AE6
loc_80E49F4:
   mov     edx, [ebp+8]
   mov     eax, [ebp-14h]
   fld     dword [ebp-18h]
   fld     dword [edx+eax*4+114h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E4A0F
   jmp     loc_80E4A24
loc_80E4A0F:
   mov     edx, [ebp+8]
   mov     eax, [ebp-14h]
   fld     dword [ebp-18h]
   fsub    dword [edx+eax*4+114h]
   fstp    dword [ebp-18h]
   jmp     loc_80E4A37
loc_80E4A24:
   mov     edx, [ebp+8]
   mov     eax, [ebp-14h]
   fld     dword [ebp-18h]
   fadd    dword [edx+eax*4+114h]
   fstp    dword [ebp-18h]
loc_80E4A37:
   mov     ecx, [ebp+8]
   mov     ebx, [ebp-14h]
   mov     esi, [ebp+8]
   mov     edx, [ebp-14h]
   fld     dword [ebp-18h]
   fld     dword [flt_814DFC4]
   fmulp   st1, st0
   fnstcw  word [ebp-46h]
   movzx   eax, word [ebp-46h]
   or      ax, 0C00h
   mov     [ebp-48h], ax
   fldcw   word [ebp-48h]
   fistp   dword [ebp-4Ch]
   fldcw   word [ebp-46h]
   mov     eax, [ebp-4Ch]
   and     eax, 0FFFFh
   add     eax, [esi+edx*4+54h]
   mov     [ecx+ebx*4+54h], eax
   fld     dword [ebp-18h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E4A86
   jmp     loc_80E4AB7
loc_80E4A86:
   mov     esi, [ebp+8]
   mov     edi, [ebp-14h]
   mov     ebx, [ebp+8]
   mov     edx, [ebp-14h]
   mov     ecx, [ebp+8]
   mov     eax, [ebp-14h]
   fld     dword [ebx+edx*4+10Ch]
   fsub    dword [ecx+eax*4+114h]
   fstp    dword [esp]
   call    AngleNormalize360Accurate
   fstp    dword [esi+edi*4+0E8h]
   jmp     loc_80E4AE6
loc_80E4AB7:
   mov     esi, [ebp+8]
   mov     edi, [ebp-14h]
   mov     ebx, [ebp+8]
   mov     edx, [ebp-14h]
   mov     ecx, [ebp+8]
   mov     eax, [ebp-14h]
   fld     dword [ebx+edx*4+10Ch]
   fadd    dword [ecx+eax*4+114h]
   fstp    dword [esp]
   call    AngleNormalize360Accurate
   fstp    dword [esi+edi*4+0E8h]
loc_80E4AE6:
   lea     eax, [ebp-14h]
   inc     dword [eax]
   jmp     loc_80E4981
loc_80E4AF0:
   mov     eax, [ebp+8]
   mov     eax, [eax+0Ch]
   and     eax, 4
   test    eax, eax
   jz      loc_80E4B0D
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    sub_80DBD56
   jmp     loc_80E54D3
loc_80E4B0D:
   mov     eax, [ebp+8]
   mov     eax, [eax+0Ch]
   and     eax, 20h
   test    eax, eax
   jz      loc_80E4C66
   mov     eax, [ebp+8]
   cmp     dword [eax+60h], 3FFh
   jnz     loc_80E4C66
   mov     eax, [bg_ladder_yawcap]
   fld     dword [eax+8]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80E4B48
   jp      loc_80E4B48
   jmp     loc_80E4C66
loc_80E4B48:
   mov     eax, [ebp+8]
   add     eax, 64h
   mov     [esp], eax
   call    vectoyaw
   fld     dword [flt_814DFC8]
   faddp   st1, st0
   fstp    dword [ebp-3Ch]
   mov     eax, [ebp+8]
   mov     eax, [eax+0ECh]
   mov     [esp+4], eax
   mov     eax, [ebp-3Ch]
   mov     [esp], eax
   call    AngleDelta
   fstp    dword [ebp-18h]
   mov     eax, [bg_ladder_yawcap]
   fld     dword [ebp-18h]
   fld     dword [eax+8]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E4BB1
   mov     eax, [bg_ladder_yawcap]
   mov     eax, [eax+8]
   mov     [ebp-50h], eax
   fld     dword [ebp-50h]
   fchs    
   fld     dword [ebp-18h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E4BB1
   jmp     loc_80E4C66
loc_80E4BB1:
   mov     eax, [bg_ladder_yawcap]
   fld     dword [ebp-18h]
   fld     dword [eax+8]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E4BC7
   jmp     loc_80E4BD7
loc_80E4BC7:
   mov     eax, [bg_ladder_yawcap]
   fld     dword [ebp-18h]
   fsub    dword [eax+8]
   fstp    dword [ebp-18h]
   jmp     loc_80E4BE5
loc_80E4BD7:
   mov     eax, [bg_ladder_yawcap]
   fld     dword [ebp-18h]
   fadd    dword [eax+8]
   fstp    dword [ebp-18h]
loc_80E4BE5:
   mov     ecx, [ebp+8]
   mov     edx, [ebp+8]
   fld     dword [ebp-18h]
   fld     dword [flt_814DFC4]
   fmulp   st1, st0
   fnstcw  word [ebp-46h]
   movzx   eax, word [ebp-46h]
   or      ax, 0C00h
   mov     [ebp-48h], ax
   fldcw   word [ebp-48h]
   fistp   dword [ebp-4Ch]
   fldcw   word [ebp-46h]
   mov     eax, [ebp-4Ch]
   and     eax, 0FFFFh
   add     eax, [edx+58h]
   mov     [ecx+58h], eax
   fld     dword [ebp-18h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E4C2C
   jmp     loc_80E4C4A
loc_80E4C2C:
   mov     ebx, [ebp+8]
   mov     eax, [bg_ladder_yawcap]
   fld     dword [ebp-3Ch]
   fsub    dword [eax+8]
   fstp    dword [esp]
   call    AngleNormalize360Accurate
   fstp    dword [ebx+0ECh]
   jmp     loc_80E4C66
loc_80E4C4A:
   mov     ebx, [ebp+8]
   mov     eax, [bg_ladder_yawcap]
   fld     dword [ebp-3Ch]
   fadd    dword [eax+8]
   fstp    dword [esp]
   call    AngleNormalize360Accurate
   fstp    dword [ebx+0ECh]
loc_80E4C66:
   mov     eax, [ebp+8]
   mov     eax, [eax+0Ch]
   and     eax, 1
   test    al, al
   jz      loc_80E5482
   mov     eax, [ebp+8]
   mov     eax, [eax+0A0h]
   and     eax, 300h
   test    eax, eax
   jnz     loc_80E5482
   mov     dword [ebp-30h], 0
   mov     eax, [ebp+8]
   mov     eax, [eax+0ECh]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     eax, [eax+584h]
   mov     [esp], eax
   call    AngleDelta
   fstp    dword [ebp-18h]
   mov     eax, [bg_prone_yawcap]
   fld     dword [eax+8]
   fld     dword [flt_814DFCC]
   fsubp   st1, st0
   fld     dword [ebp-18h]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E4D19
   mov     eax, [bg_prone_yawcap]
   fld     dword [eax+8]
   fld     dword [flt_814DFCC]
   fsubp   st1, st0
   fchs    
   fld     dword [ebp-18h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E4D19
   mov     eax, [ebp+10h]
   cmp     byte [eax+18h], 0
   jnz     loc_80E4D04
   mov     eax, [ebp+10h]
   cmp     byte [eax+19h], 0
   jnz     loc_80E4D04
   jmp     loc_80E4FAA
loc_80E4D04:
   fld     dword [ebp-18h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80E4D19
   jp      loc_80E4D19
   jmp     loc_80E4FAA
loc_80E4D19:
   mov     eax, [ebp-18h]
   mov     [esp], eax
   call    Fabs961
   fld     dword [ebp+0Ch]
   fld     dword [flt_814DFD0]
   fmulp   st1, st0
   fld     dword [flt_814DFD4]
   fmulp   st1, st0
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E4D40
   jmp     loc_80E4D4E
loc_80E4D40:
   mov     eax, [ebp+8]
   mov     eax, [eax+0ECh]
   mov     [ebp-1Ch], eax
   jmp     loc_80E4DA2
loc_80E4D4E:
   fld     dword [ebp-18h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E4D5E
   jmp     loc_80E4D81
loc_80E4D5E:
   mov     eax, [ebp+8]
   fld     dword [ebp+0Ch]
   fld     dword [flt_814DFD0]
   fmulp   st1, st0
   fld     dword [flt_814DFD4]
   fmulp   st1, st0
   fld     dword [eax+584h]
   fsubrp  st1, st0
   fstp    dword [ebp-1Ch]
   jmp     loc_80E4DA2
loc_80E4D81:
   mov     eax, [ebp+8]
   fld     dword [ebp+0Ch]
   fld     dword [flt_814DFD0]
   fmulp   st1, st0
   fld     dword [flt_814DFD4]
   fmulp   st1, st0
   fld     dword [eax+584h]
   faddp   st1, st0
   fstp    dword [ebp-1Ch]
loc_80E4DA2:
   mov     dword [ebp-24h], 1
loc_80E4DA9:
   movzx   eax, byte [ebp-0Dh]
   mov     [esp+8], eax
   mov     eax, [ebp-1Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    sub_80E4688
   test    eax, eax
   jz      loc_80E4F23
   mov     eax, 42340000h
   mov     [esp+34h], eax
   mov     dword [esp+30h], 0
   movzx   eax, byte [ebp-0Dh]
   mov     [esp+2Ch], eax
   mov     dword [esp+28h], 0
   mov     eax, [ebp+8]
   cmp     dword [eax+60h], 3FFh
   setnz   al
   movzx   eax, al
   mov     [esp+24h], eax
   mov     dword [esp+20h], 1
   mov     dword [esp+1Ch], 0
   mov     dword [esp+18h], 0
   mov     dword [esp+14h], 0
   mov     eax, [ebp+8]
   mov     eax, [eax+0ECh]
   mov     [esp+10h], eax
   mov     eax, 41F00000h
   mov     [esp+0Ch], eax
   mov     eax, [ebp+8]
   mov     eax, [eax+578h]
   mov     [esp+8], eax
   mov     eax, [ebp+8]
   add     eax, 14h
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     eax, [eax+0CCh]
   mov     [esp], eax
   call    BG_CheckProne
   mov     [ebp-20h], eax
   cmp     dword [ebp-20h], 0
   jz      loc_80E4F0D
   mov     eax, 42340000h
   mov     [esp+34h], eax
   mov     dword [esp+30h], 0
   movzx   eax, byte [ebp-0Dh]
   mov     [esp+2Ch], eax
   mov     dword [esp+28h], 0
   mov     eax, [ebp+8]
   cmp     dword [eax+60h], 3FFh
   setnz   al
   movzx   eax, al
   mov     [esp+24h], eax
   mov     dword [esp+20h], 1
   mov     dword [esp+1Ch], 0
   mov     dword [esp+18h], 0
   mov     dword [esp+14h], 0
   mov     eax, [ebp-1Ch]
   mov     [esp+10h], eax
   mov     eax, 41F00000h
   mov     [esp+0Ch], eax
   mov     eax, [ebp+8]
   mov     eax, [eax+578h]
   mov     [esp+8], eax
   mov     eax, [ebp+8]
   add     eax, 14h
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     eax, [eax+0CCh]
   mov     [esp], eax
   call    BG_CheckProne
   mov     [ebp-20h], eax
   cmp     dword [ebp-20h], 0
   jz      loc_80E4F0D
   mov     edx, [ebp+8]
   mov     eax, [ebp-1Ch]
   mov     [edx+584h], eax
loc_80E4F0D:
   cmp     dword [ebp-20h], 0
   jnz     loc_80E4FAA
   mov     dword [ebp-30h], 1
   jmp     loc_80E4FAA
loc_80E4F23:
   cmp     dword [ebp-24h], 0
   jnz     loc_80E4F2B
   jmp     loc_80E4FAA
loc_80E4F2B:
   mov     eax, [ebp-1Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     eax, [eax+584h]
   mov     [esp], eax
   call    AngleDelta
   fstp    dword [ebp-18h]
   mov     eax, [ebp-18h]
   mov     [esp], eax
   call    Fabs961
   fld1    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   setnbe  al
   movzx   eax, al
   mov     [ebp-24h], eax
   cmp     dword [ebp-24h], 0
   jz      loc_80E4F8D
   fld     dword [ebp-18h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E4F79
   jmp     loc_80E4F83
loc_80E4F79:
   mov     eax, 3F800000h
   mov     [ebp-18h], eax
   jmp     loc_80E4F94
loc_80E4F83:
   mov     eax, 0BF800000h
   mov     [ebp-18h], eax
   jmp     loc_80E4F94
loc_80E4F8D:
   mov     dword [ebp-30h], 1
loc_80E4F94:
   fld     dword [ebp-1Ch]
   fadd    dword [ebp-18h]
   fstp    dword [esp]
   call    AngleNormalize360Accurate
   fstp    dword [ebp-1Ch]
   jmp     loc_80E4DA9
loc_80E4FAA:
   mov     eax, [ebp+8]
   mov     eax, [eax+0ECh]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     eax, [eax+584h]
   mov     [esp], eax
   call    AngleDelta
   fstp    dword [ebp-18h]
   fld     dword [ebp-18h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80E4FE0
   jp      loc_80E4FE0
   jmp     loc_80E51A4
loc_80E4FE0:
   mov     eax, [ebp+8]
   mov     eax, [eax+584h]
   mov     [ebp-1Ch], eax
   mov     dword [ebp-24h], 1
loc_80E4FF3:
   mov     eax, 42340000h
   mov     [esp+34h], eax
   mov     dword [esp+30h], 0
   movzx   eax, byte [ebp-0Dh]
   mov     [esp+2Ch], eax
   mov     dword [esp+28h], 0
   mov     eax, [ebp+8]
   cmp     dword [eax+60h], 3FFh
   setnz   al
   movzx   eax, al
   mov     [esp+24h], eax
   mov     dword [esp+20h], 1
   mov     dword [esp+1Ch], 0
   mov     dword [esp+18h], 0
   mov     dword [esp+14h], 0
   mov     eax, [ebp-1Ch]
   mov     [esp+10h], eax
   mov     eax, 41F00000h
   mov     [esp+0Ch], eax
   mov     eax, [ebp+8]
   mov     eax, [eax+578h]
   mov     [esp+8], eax
   mov     eax, [ebp+8]
   add     eax, 14h
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     eax, [eax+0CCh]
   mov     [esp], eax
   call    BG_CheckProne
   mov     [ebp-20h], eax
   cmp     dword [ebp-20h], 0
   jz      loc_80E50B8
   movzx   eax, byte [ebp-0Dh]
   mov     [esp+8], eax
   mov     eax, [ebp-1Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    sub_80E4688
   test    eax, eax
   jz      loc_80E50B8
   mov     edx, [ebp+8]
   mov     eax, [ebp-1Ch]
   mov     [edx+584h], eax
   jmp     loc_80E51A4
loc_80E50B8:
   cmp     dword [ebp-24h], 0
   jnz     loc_80E50C3
   jmp     loc_80E51A4
loc_80E50C3:
   mov     eax, [ebp-18h]
   mov     [esp], eax
   call    Fabs961
   fld1    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   setnbe  al
   movzx   eax, al
   mov     [ebp-24h], eax
   cmp     dword [ebp-24h], 0
   jz      loc_80E5108
   fld     dword [ebp-18h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E50F6
   jmp     loc_80E5100
loc_80E50F6:
   mov     eax, 3F800000h
   mov     [ebp-18h], eax
   jmp     loc_80E5108
loc_80E5100:
   mov     eax, 0BF800000h
   mov     [ebp-18h], eax
loc_80E5108:
   mov     dword [ebp-30h], 1
   mov     ecx, [ebp+8]
   mov     edx, [ebp+8]
   fld     dword [ebp-18h]
   fld     dword [flt_814DFC4]
   fmulp   st1, st0
   fnstcw  word [ebp-46h]
   movzx   eax, word [ebp-46h]
   or      ax, 0C00h
   mov     [ebp-48h], ax
   fldcw   word [ebp-48h]
   fistp   dword [ebp-4Ch]
   fldcw   word [ebp-46h]
   mov     eax, [ebp-4Ch]
   and     eax, 0FFFFh
   add     eax, [edx+58h]
   mov     [ecx+58h], eax
   mov     ebx, [ebp+8]
   mov     eax, [ebp+8]
   fld     dword [eax+0ECh]
   fadd    dword [ebp-18h]
   fstp    dword [esp]
   call    AngleNormalize360Accurate
   fstp    dword [ebx+0ECh]
   mov     eax, [ebp+8]
   mov     eax, [eax+0ECh]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     eax, [eax+584h]
   mov     [esp], eax
   call    AngleDelta
   fstp    dword [ebp-18h]
   cmp     dword [ebp-20h], 0
   jnz     loc_80E4FF3
   fld     dword [ebp-1Ch]
   fadd    dword [ebp-18h]
   fstp    dword [esp]
   call    AngleNormalize360Accurate
   fstp    dword [ebp-1Ch]
   jmp     loc_80E4FF3
loc_80E51A4:
   mov     eax, [bg_prone_yawcap]
   fld     dword [ebp-18h]
   fld     dword [eax+8]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E51D9
   mov     eax, [bg_prone_yawcap]
   mov     eax, [eax+8]
   mov     [ebp-50h], eax
   fld     dword [ebp-50h]
   fchs    
   fld     dword [ebp-18h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E51D9
   jmp     loc_80E529C
loc_80E51D9:
   mov     eax, [bg_prone_yawcap]
   fld     dword [ebp-18h]
   fld     dword [eax+8]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E51EF
   jmp     loc_80E51FF
loc_80E51EF:
   mov     eax, [bg_prone_yawcap]
   fld     dword [ebp-18h]
   fsub    dword [eax+8]
   fstp    dword [ebp-18h]
   jmp     loc_80E520D
loc_80E51FF:
   mov     eax, [bg_prone_yawcap]
   fld     dword [ebp-18h]
   fadd    dword [eax+8]
   fstp    dword [ebp-18h]
loc_80E520D:
   mov     ecx, [ebp+8]
   mov     edx, [ebp+8]
   fld     dword [ebp-18h]
   fld     dword [flt_814DFC4]
   fmulp   st1, st0
   fnstcw  word [ebp-46h]
   movzx   eax, word [ebp-46h]
   or      ax, 0C00h
   mov     [ebp-48h], ax
   fldcw   word [ebp-48h]
   fistp   dword [ebp-4Ch]
   fldcw   word [ebp-46h]
   mov     eax, [ebp-4Ch]
   and     eax, 0FFFFh
   add     eax, [edx+58h]
   mov     [ecx+58h], eax
   fld     dword [ebp-18h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E5254
   jmp     loc_80E5279
loc_80E5254:
   mov     ebx, [ebp+8]
   mov     eax, [ebp+8]
   mov     edx, [bg_prone_yawcap]
   fld     dword [eax+584h]
   fsub    dword [edx+8]
   fstp    dword [esp]
   call    AngleNormalize360Accurate
   fstp    dword [ebx+0ECh]
   jmp     loc_80E529C
loc_80E5279:
   mov     ebx, [ebp+8]
   mov     eax, [ebp+8]
   mov     edx, [bg_prone_yawcap]
   fld     dword [eax+584h]
   fadd    dword [edx+8]
   fstp    dword [esp]
   call    AngleNormalize360Accurate
   fstp    dword [ebx+0ECh]
loc_80E529C:
   cmp     dword [ebp-30h], 0
   jz      loc_80E537B
   mov     edx, [ebp+8]
   mov     eax, [ebp+8]
   mov     eax, [eax+0Ch]
   or      eax, 10000h
   mov     [edx+0Ch], eax
   mov     eax, [ebp+8]
   mov     eax, [eax+0ECh]
   mov     [esp+4], eax
   mov     eax, [ebp-28h]
   mov     [esp], eax
   call    AngleDelta
   fstp    dword [ebp-34h]
   mov     eax, [ebp-34h]
   mov     [esp], eax
   call    Fabs961
   fld1    
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_80E52EB
   jmp     loc_80E537B
loc_80E52EB:
   mov     eax, [ebp+8]
   mov     eax, [eax+0ECh]
   mov     [esp+4], eax
   mov     eax, [ebp-2Ch]
   mov     [esp], eax
   call    AngleDelta
   fstp    dword [ebp-38h]
   fld     dword [ebp-34h]
   fmul    dword [ebp-38h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E5319
   jmp     loc_80E537B
loc_80E5319:
   fld     dword [ebp-34h]
   fld     dword [flt_814DFDC]
   fmulp   st1, st0
   fstp    dword [ebp-34h]
   mov     ebx, [ebp+8]
   mov     eax, [ebp+8]
   fld     dword [eax+0ECh]
   fadd    dword [ebp-34h]
   fstp    dword [esp]
   call    AngleNormalize360Accurate
   fstp    dword [ebx+0ECh]
   mov     ecx, [ebp+8]
   mov     edx, [ebp+8]
   fld     dword [ebp-34h]
   fld     dword [flt_814DFC4]
   fmulp   st1, st0
   fnstcw  word [ebp-46h]
   movzx   eax, word [ebp-46h]
   or      ax, 0C00h
   mov     [ebp-48h], ax
   fldcw   word [ebp-48h]
   fistp   dword [ebp-4Ch]
   fldcw   word [ebp-46h]
   mov     eax, [ebp-4Ch]
   and     eax, 0FFFFh
   add     eax, [edx+58h]
   mov     [ecx+58h], eax
loc_80E537B:
   mov     eax, [ebp+8]
   mov     eax, [eax+0E8h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     eax, [eax+58Ch]
   mov     [esp], eax
   call    AngleDelta
   fstp    dword [ebp-18h]
   fld     dword [ebp-18h]
   fld     dword [flt_814DFD8]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E53C3
   fld     dword [ebp-18h]
   fld     dword [flt_814DFE0]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E53C3
   jmp     loc_80E5482
loc_80E53C3:
   fld     dword [ebp-18h]
   fld     dword [flt_814DFD8]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E53D7
   jmp     loc_80E53E7
loc_80E53D7:
   fld     dword [ebp-18h]
   fld     dword [flt_814DFD8]
   fsubp   st1, st0
   fstp    dword [ebp-18h]
   jmp     loc_80E53F5
loc_80E53E7:
   fld     dword [ebp-18h]
   fld     dword [flt_814DFD8]
   faddp   st1, st0
   fstp    dword [ebp-18h]
loc_80E53F5:
   mov     ecx, [ebp+8]
   mov     edx, [ebp+8]
   fld     dword [ebp-18h]
   fld     dword [flt_814DFC4]
   fmulp   st1, st0
   fnstcw  word [ebp-46h]
   movzx   eax, word [ebp-46h]
   or      ax, 0C00h
   mov     [ebp-48h], ax
   fldcw   word [ebp-48h]
   fistp   dword [ebp-4Ch]
   fldcw   word [ebp-46h]
   mov     eax, [ebp-4Ch]
   and     eax, 0FFFFh
   add     eax, [edx+54h]
   mov     [ecx+54h], eax
   fld     dword [ebp-18h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E543C
   jmp     loc_80E5460
loc_80E543C:
   mov     ebx, [ebp+8]
   mov     eax, [ebp+8]
   fld     dword [eax+58Ch]
   fld     dword [flt_814DFD8]
   fsubp   st1, st0
   fstp    dword [esp]
   call    AngleNormalize180Accurate
   fstp    dword [ebx+0E8h]
   jmp     loc_80E5482
loc_80E5460:
   mov     ebx, [ebp+8]
   mov     eax, [ebp+8]
   fld     dword [eax+58Ch]
   fld     dword [flt_814DFD8]
   faddp   st1, st0
   fstp    dword [esp]
   call    AngleNormalize180Accurate
   fstp    dword [ebx+0E8h]
loc_80E5482:
   mov     eax, [ebp+8]
   cmp     dword [eax+4], 3
   jnz     loc_80E548D
   jmp     loc_80E54D3
loc_80E548D:
   mov     eax, [ebp+8]
   cmp     dword [eax+4], 2
   jnz     loc_80E5498
   jmp     loc_80E54D3
loc_80E5498:
   mov     eax, [ebp+8]
   cmp     dword [eax+4], 4
   jnz     loc_80E54A3
   jmp     loc_80E54D3
loc_80E54A3:
   movzx   edx, byte [ebp-0Dh]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   mov     eax, dword [pmoveHandlers+eax]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+10h]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    sub_80E42D0
loc_80E54D3:
   add     esp, 7Ch
   pop     ebx
   pop     esi
   pop     edi
   pop     ebp
   retn    


section .rdata
flt_814DFD8     dd 45.0
flt_814DFD4     dd 0.001
flt_814DFC4     dd 182.04445
flt_814DFE0     dd -45.0
flt_814DFDC     dd 0.98000002
flt_814DFD0     dd 55.0
flt_814DFCC     dd 5.0
flt_814DFC8     dd 180.0
flt_814DFAC     dd 280.0
flt_814DFC0     dd 0.0054931641
flt_814DFB0     dd 350.0
flt_814609C     dd 180.0
flt_8146094     dd -180.0
flt_8146098     dd 360.0
flt_8146090     dd 360.0
flt_8146080     dd 182.04445
flt_8146084     dd 0.0054931641
flt_814DFBC     dd 66.0
flt_814DFB8     dd 45.0
flt_814DFB4     dd 240.0
flt_8145F10     dd 360.0
flt_814C7CC     dd 182.04445
flt_81489FC     dd 2.0
flt_814608C     dd 360.0
flt_8146088     dd 180.0

dbl_8145F88     dq 0.0174532925199433
dbl_8145F00     dq 180.0
dbl_8145F08     dq 3.141592653589793