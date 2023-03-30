extern PM_StepSlideMove
extern PM_Accelerate
extern PM_CmdScale
extern PM_AirMove
extern Jump_Check
extern sqrt
extern atan2


global PM_LadderMove


global Fabs961


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


VectorMA468:
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


VectorClear187:
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


Vector2Copy781:
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
   pop     ebp
   retn    


Vec2Normalize962:
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
   jnz     loc_80A21A3
   jp      loc_80A21A3
   jmp     loc_80A21CB
loc_80A21A3:
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
loc_80A21CB:
   mov     eax, [ebp-4]
   mov     [ebp-0Ch], eax
   fld     dword [ebp-0Ch]
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


Vec2LengthSq145:
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
   pop     ebp
   retn    


sub_80E702E:
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


sub_80E6F06:
   push    ebp
   mov     ebp, esp
   fld     dword [ebp+8]
   fmul    dword [ebp+8]
   pop     ebp
   retn    


sub_80E6FD6:
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


PM_LadderMove:
   push    ebp
   mov     ebp, esp
   sub     esp, 98h
   mov     eax, [ebp+8]
   mov     eax, [eax]
   mov     [ebp-68h], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    Jump_Check
   test    al, al
   jz      loc_80E5D88
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    PM_AirMove
   jmp     locret_80E61FD
loc_80E5D88:
   mov     eax, [ebp+0Ch]
   fld     dword [eax+8]
   fld     dword [flt_814DFF0]
   faddp   st1, st0
   fld     dword [flt_814DFF4]
   fmulp   st1, st0
   fstp    dword [ebp-4Ch]
   fld     dword [ebp-4Ch]
   fld1    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E5DB1
   jmp     loc_80E5DBB
loc_80E5DB1:
   mov     eax, 3F800000h
   mov     [ebp-4Ch], eax
   jmp     loc_80E5DD5
loc_80E5DBB:
   fld     dword [ebp-4Ch]
   fld     dword [flt_814DFF8]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E5DCD
   jmp     loc_80E5DD5
loc_80E5DCD:
   mov     eax, 0BF800000h
   mov     [ebp-4Ch], eax
loc_80E5DD5:
   mov     edx, [ebp+0Ch]
   mov     eax, 0
   mov     [edx+8], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    Vec3Normalize
   fstp    st0
   mov     edx, [ebp+0Ch]
   mov     eax, 0
   mov     [edx+14h], eax
   lea     eax, [ebp-48h]
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   add     eax, 0Ch
   mov     [esp], eax
   call    Vec3NormalizeTo
   fstp    st0
   mov     eax, [ebp+0Ch]
   add     eax, 0Ch
   mov     [esp+8], eax
   mov     eax, [ebp-68h]
   add     eax, 64h
   mov     [esp+4], eax
   lea     eax, [ebp-48h]
   mov     [esp], eax
   call    ProjectPointOnPlane
   mov     eax, [ebp+8]
   add     eax, 4
   mov     [esp+4], eax
   mov     eax, [ebp-68h]
   mov     [esp], eax
   call    PM_CmdScale
   fstp    dword [ebp-10h]
   lea     eax, [ebp-38h]
   mov     [esp], eax
   call    VectorClear187
   mov     eax, [ebp+8]
   cmp     byte [eax+1Ch], 0
   jz      loc_80E5E7E
   fld     dword [ebp-4Ch]
   fld     dword [flt_814DFFC]
   fmulp   st1, st0
   fmul    dword [ebp-10h]
   mov     eax, [ebp+8]
   movsx   ax, byte [eax+1Ch]
   push    ax
   fild    word [esp]
   lea     esp, [esp+2]
   fmulp   st1, st0
   fstp    dword [ebp-30h]
loc_80E5E7E:
   mov     eax, [ebp+8]
   cmp     byte [eax+1Dh], 0
   jz      loc_80E5EC5
   lea     eax, [ebp-38h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+0Ch]
   add     eax, 0Ch
   mov     [esp+8], eax
   fld     dword [ebp-10h]
   fld     dword [flt_814E000]
   fmulp   st1, st0
   mov     eax, [ebp+8]
   movsx   ax, byte [eax+1Dh]
   push    ax
   fild    word [esp]
   lea     esp, [esp+2]
   fmulp   st1, st0
   fstp    dword [esp+4]
   lea     eax, [ebp-38h]
   mov     [esp], eax
   call    VectorMA468
loc_80E5EC5:
   lea     eax, [ebp-28h]
   mov     [esp+4], eax
   lea     eax, [ebp-38h]
   mov     [esp], eax
   call    Vec3NormalizeTo
   fstp    dword [ebp-0Ch]
   mov     eax, 41100000h
   mov     [esp+10h], eax
   mov     eax, [ebp-0Ch]
   mov     [esp+0Ch], eax
   lea     eax, [ebp-28h]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp-68h]
   mov     [esp], eax
   call    PM_Accelerate
   mov     eax, [ebp+8]
   cmp     byte [eax+1Ch], 0
   jnz     loc_80E5F93
   mov     eax, [ebp-68h]
   fld     dword [eax+28h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E5F23
   jmp     loc_80E5F5B
loc_80E5F23:
   mov     ecx, [ebp-68h]
   mov     edx, [ebp-68h]
   mov     eax, [ebp-68h]
   fild    dword [eax+48h]
   mov     eax, [ebp+0Ch]
   fmul    dword [eax+24h]
   fld     dword [edx+28h]
   fsubrp  st1, st0
   fstp    dword [ecx+28h]
   mov     eax, [ebp-68h]
   fld     dword [eax+28h]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E5F4E
   jmp     loc_80E5F93
loc_80E5F4E:
   mov     edx, [ebp-68h]
   mov     eax, 0
   mov     [edx+28h], eax
   jmp     loc_80E5F93
loc_80E5F5B:
   mov     ecx, [ebp-68h]
   mov     edx, [ebp-68h]
   mov     eax, [ebp-68h]
   fild    dword [eax+48h]
   mov     eax, [ebp+0Ch]
   fmul    dword [eax+24h]
   fld     dword [edx+28h]
   faddp   st1, st0
   fstp    dword [ecx+28h]
   mov     eax, [ebp-68h]
   fld     dword [eax+28h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E5F88
   jmp     loc_80E5F93
loc_80E5F88:
   mov     edx, [ebp-68h]
   mov     eax, 0
   mov     [edx+28h], eax
loc_80E5F93:
   mov     eax, [ebp+8]
   cmp     byte [eax+1Dh], 0
   jnz     loc_80E60AE
   lea     eax, [ebp-58h]
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   add     eax, 0Ch
   mov     [esp], eax
   call    Vector2Copy781
   lea     eax, [ebp-58h]
   mov     [esp], eax
   call    Vec2Normalize962
   fstp    st0
   mov     eax, [ebp-68h]
   add     eax, 20h
   mov     [esp+4], eax
   lea     eax, [ebp-58h]
   mov     [esp], eax
   call    Vec2LengthSq145
   fstp    dword [ebp-5Ch]
   fld     dword [ebp-5Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80E5FEF
   jp      loc_80E5FEF
   jmp     loc_80E60AE
loc_80E5FEF:
   mov     eax, [ebp-68h]
   add     eax, 20h
   mov     [esp+0Ch], eax
   lea     eax, [ebp-58h]
   mov     [esp+8], eax
   mov     eax, [ebp-5Ch]
   xor     eax, 80000000h
   mov     [esp+4], eax
   mov     eax, [ebp-68h]
   add     eax, 20h
   mov     [esp], eax
   call    sub_80E6FD6
   mov     eax, [ebp+0Ch]
   fld     dword [ebp-5Ch]
   fmul    dword [eax+24h]
   fld     dword [flt_814E004]
   fmulp   st1, st0
   fstp    dword [ebp-60h]
   mov     eax, [ebp-5Ch]
   mov     [esp], eax
   call    Fabs961
   fstp    dword [ebp-70h]
   mov     eax, [ebp-60h]
   mov     [esp], eax
   call    Fabs961
   fld     dword [ebp-70h]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E6053
   jmp     loc_80E60AE
loc_80E6053:
   mov     eax, [ebp-60h]
   mov     [esp], eax
   call    Fabs961
   fld1    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E6069
   jmp     loc_80E607F
loc_80E6069:
   mov     eax, [ebp-60h]
   mov     [esp], eax
   call    sub_80E6F12
   push    eax
   fild    dword [esp]
   lea     esp, [esp+4]
   fstp    dword [ebp-60h]
loc_80E607F:
   fld     dword [ebp-5Ch]
   fsub    dword [ebp-60h]
   fstp    dword [ebp-5Ch]
   mov     eax, [ebp-68h]
   add     eax, 20h
   mov     [esp+0Ch], eax
   lea     eax, [ebp-58h]
   mov     [esp+8], eax
   mov     eax, [ebp-5Ch]
   mov     [esp+4], eax
   mov     eax, [ebp-68h]
   add     eax, 20h
   mov     [esp], eax
   call    sub_80E6FD6
loc_80E60AE:
   mov     eax, [ebp+0Ch]
   cmp     dword [eax+2Ch], 0
   jnz     loc_80E6162
   mov     eax, [ebp-68h]
   add     eax, 20h
   mov     [esp+4], eax
   mov     eax, [ebp-68h]
   add     eax, 64h
   mov     [esp], eax
   call    Vec2LengthSq145
   fstp    dword [ebp-5Ch]
   mov     eax, [ebp-68h]
   add     eax, 20h
   mov     [esp+0Ch], eax
   mov     eax, [ebp-68h]
   add     eax, 64h
   mov     [esp+8], eax
   mov     eax, [ebp-5Ch]
   xor     eax, 80000000h
   mov     [esp+4], eax
   mov     eax, [ebp-68h]
   add     eax, 20h
   mov     [esp], eax
   call    sub_80E6FD6
   mov     eax, [ebp-68h]
   add     eax, 20h
   mov     [esp], eax
   call    sub_80E702E
   fstp    dword [ebp-74h]
   mov     eax, [ebp-68h]
   mov     eax, [eax+28h]
   mov     [esp], eax
   call    sub_80E6F06
   fld     dword [ebp-74h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_80E6131
   jmp     loc_80E6162
loc_80E6131:
   mov     eax, 0C2480000h
   mov     [ebp-5Ch], eax
   mov     eax, [ebp-68h]
   add     eax, 20h
   mov     [esp+0Ch], eax
   mov     eax, [ebp-68h]
   add     eax, 64h
   mov     [esp+8], eax
   mov     eax, [ebp-5Ch]
   mov     [esp+4], eax
   mov     eax, [ebp-68h]
   add     eax, 20h
   mov     [esp], eax
   call    sub_80E6FD6
loc_80E6162:
   mov     dword [esp+8], 0
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    PM_StepSlideMove
   mov     eax, [ebp-68h]
   add     eax, 64h
   mov     [esp], eax
   call    vectoyaw
   fld     dword [flt_814E008]
   faddp   st1, st0
   fstp    dword [ebp-10h]
   mov     eax, [ebp-68h]
   mov     eax, [eax+0ECh]
   mov     [esp+4], eax
   mov     eax, [ebp-10h]
   mov     [esp], eax
   call    AngleDelta
   fnstcw  word [ebp-6Ah]
   movzx   eax, word [ebp-6Ah]
   or      ax, 0C00h
   mov     [ebp-6Ch], ax
   fldcw   word [ebp-6Ch]
   fistp   dword [ebp-64h]
   fldcw   word [ebp-6Ah]
   mov     eax, [ebp-64h]
   mov     [ebp-78h], eax
   cmp     dword [ebp-78h], 0
   jns     loc_80E61D4
   neg     dword [ebp-78h]
loc_80E61D4:
   cmp     dword [ebp-78h], 4Bh
   jle     loc_80E61F0
   cmp     dword [ebp-64h], 0
   jle     loc_80E61E9
   mov     dword [ebp-64h], 4Bh
   jmp     loc_80E61F0
loc_80E61E9:
   mov     dword [ebp-64h], 0FFFFFFB5h
loc_80E61F0:
   mov     edx, [ebp-68h]
   movsx   eax, byte [ebp-64h]
   mov     [edx+9Ch], eax
locret_80E61FD:
   leave   
   retn    


section .rdata
flt_8146080     dd 182.04445
flt_8146084     dd 0.0054931641
flt_8146088     dd 180.0
flt_814608C     dd 360.0
flt_8145F10     dd 360.0
flt_814DFF0     dd 0.25
flt_814DFFC     dd 0.5
flt_814DFF4     dd 2.5
flt_814DFF8     dd -1.0
flt_814E008     dd 180.0
flt_814E000     dd 0.2
flt_814E004     dd 16.0

dbl_8145F00     dq 180.0
dbl_8145F08     dq 3.141592653589793