extern Jump_ClearState
extern Jump_ClampVelocity
extern BG_AddPredictableEventToPlayerstate
extern PM_ShouldMakeFootsteps
extern PM_FootstepEvent
extern PM_VerifyPronePosition
extern Jump_IsPlayerAboveMax
extern PM_playerTrace
extern Jump_GetStepHeight
extern floor
extern sqrt
extern PM_AddTouchEnt
extern PM_ClipVelocity


global PM_StepSlideMove


section .text


sub_80E7448:
   push    ebp
   mov     ebp, esp
   push    ebx
   sub     esp, 64h
   mov     dword [ebp-0Ch], 0
loc_80E7456:
   mov     eax, [ebp-0Ch]
   cmp     eax, [ebp+0Ch]
   jl      loc_80E7463
   jmp     loc_80E74F8
loc_80E7463:
   mov     ebx, [ebp-0Ch]
   mov     edx, [ebp-0Ch]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   add     eax, [ebp+10h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    sub_80E88AA
   fstp    dword [ebp+ebx*4-38h]
   mov     eax, [ebp-0Ch]
   mov     [ebp-3Ch], eax
loc_80E748E:
   cmp     dword [ebp-3Ch], 0
   jnz     loc_80E7496
   jmp     loc_80E74DB
loc_80E7496:
   mov     eax, [ebp-3Ch]
   shl     eax, 2
   add     eax, [ebp+14h]
   sub     eax, 4
   mov     eax, [eax]
   mov     edx, [ebp-0Ch]
   fld     dword [ebp+eax*4-38h]
   fld     dword [ebp+edx*4-38h]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E74DB
   mov     eax, [ebp-3Ch]
   lea     edx, 0[eax*4]
   mov     ecx, [ebp+14h]
   mov     eax, [ebp-3Ch]
   shl     eax, 2
   add     eax, [ebp+14h]
   sub     eax, 4
   mov     eax, [eax]
   mov     [ecx+edx], eax
   lea     eax, [ebp-3Ch]
   dec     dword [eax]
   jmp     loc_80E748E
loc_80E74DB:
   mov     eax, [ebp-3Ch]
   lea     ecx, 0[eax*4]
   mov     edx, [ebp+14h]
   mov     eax, [ebp-0Ch]
   mov     [edx+ecx], eax
   lea     eax, [ebp-0Ch]
   inc     dword [eax]
   jmp     loc_80E7456
loc_80E74F8:
   mov     eax, [ebp+14h]
   mov     eax, [eax]
   mov     eax, [ebp+eax*4-38h]
   mov     [ebp-4Ch], eax
   fld     dword [ebp-4Ch]
   add     esp, 64h
   pop     ebx
   pop     ebp
   retn    


sub_80E87DA:
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


sub_80E875C:
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


sub_80E8856:
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


sub_80E88AA:
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


Vec3Cross:
   push    ebp
   mov     ebp, esp
   mov     ecx, [ebp+10h]
   mov     eax, [ebp+8]
   add     eax, 4
   mov     edx, [ebp+0Ch]
   add     edx, 8
   fld     dword [eax]
   fmul    dword [edx]
   mov     eax, [ebp+8]
   add     eax, 8
   mov     edx, [ebp+0Ch]
   add     edx, 4
   fld     dword [eax]
   fmul    dword [edx]
   fsubp   st1, st0
   fstp    dword [ecx]
   mov     ecx, [ebp+10h]
   add     ecx, 4
   mov     eax, [ebp+8]
   add     eax, 8
   mov     edx, [ebp+0Ch]
   fld     dword [eax]
   fmul    dword [edx]
   mov     eax, [ebp+8]
   mov     edx, [ebp+0Ch]
   add     edx, 8
   fld     dword [eax]
   fmul    dword [edx]
   fsubp   st1, st0
   fstp    dword [ecx]
   mov     ecx, [ebp+10h]
   add     ecx, 8
   mov     eax, [ebp+8]
   mov     edx, [ebp+0Ch]
   add     edx, 4
   fld     dword [eax]
   fmul    dword [edx]
   mov     eax, [ebp+8]
   add     eax, 4
   mov     edx, [ebp+0Ch]
   fld     dword [eax]
   fmul    dword [edx]
   fsubp   st1, st0
   fstp    dword [ecx]
   pop     ebp
   retn    


sub_80E724C:
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


PM_SlideMove:
   push    ebp
   mov     ebp, esp
   sub     esp, 198h
   mov     eax, [ebp+8]
   mov     eax, [eax]
   mov     [ebp-15Ch], eax
   mov     dword [ebp-10h], 4
   lea     eax, [ebp-0C8h]
   mov     [esp+4], eax
   mov     eax, [ebp-15Ch]
   add     eax, 20h
   mov     [esp], eax
   call    VectorCopy805
   lea     eax, [ebp-148h]
   mov     [esp+4], eax
   mov     eax, [ebp-15Ch]
   add     eax, 20h
   mov     [esp], eax
   call    VectorCopy805
   cmp     dword [ebp+10h], 0
   jz      loc_80E75DF
   mov     eax, [ebp-15Ch]
   fild    dword [eax+48h]
   mov     eax, [ebp+0Ch]
   fmul    dword [eax+24h]
   fld     dword [ebp-140h]
   fsubrp  st1, st0
   fstp    dword [ebp-140h]
   mov     edx, [ebp-15Ch]
   mov     eax, [ebp-15Ch]
   fld     dword [eax+28h]
   fadd    dword [ebp-140h]
   fld     dword [flt_814E1B8]
   fmulp   st1, st0
   fstp    dword [edx+28h]
   mov     eax, [ebp-140h]
   mov     [ebp-0C0h], eax
   mov     eax, [ebp+0Ch]
   cmp     dword [eax+30h], 0
   jz      loc_80E75DF
   mov     eax, [ebp-15Ch]
   add     eax, 20h
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   add     eax, 3Ch
   mov     [esp+4], eax
   mov     eax, [ebp-15Ch]
   add     eax, 20h
   mov     [esp], eax
   call    PM_ClipVelocity
loc_80E75DF:
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+24h]
   mov     [ebp-12Ch], eax
   mov     eax, [ebp+0Ch]
   cmp     dword [eax+30h], 0
   jz      loc_80E7615
   lea     eax, [ebp-98h]
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   add     eax, 3Ch
   mov     [esp], eax
   call    VectorCopy805
   mov     dword [ebp-30h], 1
   jmp     loc_80E761C
loc_80E7615:
   mov     dword [ebp-30h], 0
loc_80E761C:
   lea     ecx, [ebp-98h]
   mov     edx, [ebp-30h]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   lea     eax, [eax+ecx]
   mov     [esp+4], eax
   mov     eax, [ebp-15Ch]
   add     eax, 20h
   mov     [esp], eax
   call    Vec3NormalizeTo
   fstp    st0
   lea     eax, [ebp-30h]
   inc     dword [eax]
   mov     dword [ebp-0Ch], 0
loc_80E7654:
   mov     eax, [ebp-0Ch]
   cmp     eax, [ebp-10h]
   jl      loc_80E7661
   jmp     loc_80E7C5F
loc_80E7661:
   lea     eax, [ebp-128h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp-15Ch]
   add     eax, 20h
   mov     [esp+8], eax
   mov     eax, [ebp-12Ch]
   mov     [esp+4], eax
   mov     eax, [ebp-15Ch]
   add     eax, 14h
   mov     [esp], eax
   call    sub_80E8856
   mov     eax, [ebp+8]
   mov     eax, [eax+3Ch]
   mov     [esp+1Ch], eax
   mov     eax, [ebp-15Ch]
   mov     eax, [eax+0CCh]
   mov     [esp+18h], eax
   lea     eax, [ebp-128h]
   mov     [esp+14h], eax
   mov     eax, [ebp+8]
   add     eax, 0D0h
   mov     [esp+10h], eax
   mov     eax, [ebp+8]
   add     eax, 0C4h
   mov     [esp+0Ch], eax
   mov     eax, [ebp-15Ch]
   add     eax, 14h
   mov     [esp+8], eax
   lea     eax, [ebp-118h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    PM_playerTrace
   cmp     byte [ebp-0F6h], 0
   jz      loc_80E7717
   mov     edx, [ebp-15Ch]
   mov     eax, 0
   mov     [edx+28h], eax
   mov     dword [ebp-160h], 1
   jmp     loc_80E7CB7
loc_80E7717:
   fld     dword [ebp-118h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E772A
   jmp     loc_80E775C
loc_80E772A:
   mov     eax, [ebp-15Ch]
   add     eax, 14h
   mov     [esp+0Ch], eax
   mov     eax, [ebp-118h]
   mov     [esp+8], eax
   lea     eax, [ebp-128h]
   mov     [esp+4], eax
   mov     eax, [ebp-15Ch]
   add     eax, 14h
   mov     [esp], eax
   call    Vec3Lerp
loc_80E775C:
   fld     dword [ebp-118h]
   fld1    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jp      loc_80E7773
   jz      loc_80E7C5F
loc_80E7773:
   movzx   eax, word [ebp-0FCh]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    PM_AddTouchEnt
   fld     dword [ebp-12Ch]
   fmul    dword [ebp-118h]
   fld     dword [ebp-12Ch]
   fsubrp  st1, st0
   fstp    dword [ebp-12Ch]
   cmp     dword [ebp-30h], 7
   jle     loc_80E77C9
   mov     eax, [ebp-15Ch]
   add     eax, 20h
   mov     [esp], eax
   call    sub_80E875C
   mov     dword [ebp-160h], 1
   jmp     loc_80E7CB7
loc_80E77C9:
   mov     dword [ebp-0DCh], 0
loc_80E77D3:
   mov     eax, [ebp-0DCh]
   cmp     eax, [ebp-30h]
   jl      loc_80E77E3
   jmp     loc_80E7886
loc_80E77E3:
   lea     ecx, [ebp-98h]
   mov     edx, [ebp-0DCh]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   lea     eax, [eax+ecx]
   mov     [esp+4], eax
   lea     eax, [ebp-118h]
   add     eax, 4
   mov     [esp], eax
   call    sub_80E88AA
   fld     dword [flt_814E1BC]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E7821
   jmp     loc_80E7879
loc_80E7821:
   mov     eax, [ebp-15Ch]
   add     eax, 20h
   mov     [esp+8], eax
   lea     eax, [ebp-118h]
   add     eax, 4
   mov     [esp+4], eax
   mov     eax, [ebp-15Ch]
   add     eax, 20h
   mov     [esp], eax
   call    PM_ClipVelocity
   mov     eax, [ebp-15Ch]
   add     eax, 20h
   mov     [esp+8], eax
   mov     eax, [ebp-15Ch]
   add     eax, 20h
   mov     [esp+4], eax
   lea     eax, [ebp-118h]
   add     eax, 4
   mov     [esp], eax
   call    sub_80E87DA
   jmp     loc_80E7886
loc_80E7879:
   lea     eax, [ebp-0DCh]
   inc     dword [eax]
   jmp     loc_80E77D3
loc_80E7886:
   mov     eax, [ebp-0DCh]
   cmp     eax, [ebp-30h]
   jge     loc_80E7896
   jmp     loc_80E7C55
loc_80E7896:
   lea     ecx, [ebp-98h]
   mov     edx, [ebp-30h]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   lea     eax, [eax+ecx]
   mov     [esp+4], eax
   lea     eax, [ebp-118h]
   add     eax, 4
   mov     [esp], eax
   call    VectorCopy805
   lea     eax, [ebp-30h]
   inc     dword [eax]
   lea     eax, [ebp-0B8h]
   mov     [esp+0Ch], eax
   lea     eax, [ebp-98h]
   mov     [esp+8], eax
   mov     eax, [ebp-30h]
   mov     [esp+4], eax
   mov     eax, [ebp-15Ch]
   add     eax, 20h
   mov     [esp], eax
   call    sub_80E7448
   fstp    dword [ebp-130h]
   fld     dword [ebp-130h]
   fld     dword [flt_814E1C0]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_80E7C55
   mov     eax, [ebp-130h]
   mov     [ebp-16Ch], eax
   fld     dword [ebp-16Ch]
   fchs    
   mov     eax, [ebp+0Ch]
   fld     dword [eax+5Ch]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E7935
   jmp     loc_80E7947
loc_80E7935:
   mov     eax, [ebp+0Ch]
   mov     edx, [ebp-130h]
   xor     edx, 80000000h
   mov     [eax+5Ch], edx
loc_80E7947:
   lea     eax, [ebp-0D8h]
   mov     [esp+8], eax
   lea     ecx, [ebp-98h]
   mov     edx, [ebp-0B8h]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   lea     eax, [eax+ecx]
   mov     [esp+4], eax
   mov     eax, [ebp-15Ch]
   add     eax, 20h
   mov     [esp], eax
   call    PM_ClipVelocity
   lea     eax, [ebp-158h]
   mov     [esp+8], eax
   lea     ecx, [ebp-98h]
   mov     edx, [ebp-0B8h]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   lea     eax, [eax+ecx]
   mov     [esp+4], eax
   lea     eax, [ebp-148h]
   mov     [esp], eax
   call    PM_ClipVelocity
   mov     dword [ebp-0E0h], 1
loc_80E79BC:
   mov     eax, [ebp-0E0h]
   cmp     eax, [ebp-30h]
   jl      loc_80E79CC
   jmp     loc_80E7C22
loc_80E79CC:
   lea     ecx, [ebp-98h]
   mov     eax, [ebp-0E0h]
   mov     edx, [ebp+eax*4-0B8h]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   lea     eax, [eax+ecx]
   mov     [esp+4], eax
   lea     eax, [ebp-0D8h]
   mov     [esp], eax
   call    sub_80E88AA
   fld     dword [flt_814E1C0]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_80E7C15
   lea     eax, [ebp-0D8h]
   mov     [esp+8], eax
   lea     ecx, [ebp-98h]
   mov     eax, [ebp-0E0h]
   mov     edx, [ebp+eax*4-0B8h]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   lea     eax, [eax+ecx]
   mov     [esp+4], eax
   lea     eax, [ebp-0D8h]
   mov     [esp], eax
   call    PM_ClipVelocity
   lea     eax, [ebp-158h]
   mov     [esp+8], eax
   lea     ecx, [ebp-98h]
   mov     eax, [ebp-0E0h]
   mov     edx, [ebp+eax*4-0B8h]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   lea     eax, [eax+ecx]
   mov     [esp+4], eax
   lea     eax, [ebp-158h]
   mov     [esp], eax
   call    PM_ClipVelocity
   lea     ecx, [ebp-98h]
   mov     edx, [ebp-0B8h]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   lea     eax, [eax+ecx]
   mov     [esp+4], eax
   lea     eax, [ebp-0D8h]
   mov     [esp], eax
   call    sub_80E88AA
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_80E7C15
   lea     eax, [ebp-28h]
   mov     [esp+8], eax
   lea     ecx, [ebp-98h]
   mov     eax, [ebp-0E0h]
   mov     edx, [ebp+eax*4-0B8h]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   lea     eax, [eax+ecx]
   mov     [esp+4], eax
   lea     ecx, [ebp-98h]
   mov     edx, [ebp-0B8h]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   lea     eax, [eax+ecx]
   mov     [esp], eax
   call    Vec3Cross
   lea     eax, [ebp-28h]
   mov     [esp], eax
   call    Vec3Normalize
   fstp    st0
   mov     eax, [ebp-15Ch]
   add     eax, 20h
   mov     [esp+4], eax
   lea     eax, [ebp-28h]
   mov     [esp], eax
   call    sub_80E88AA
   fstp    dword [ebp-2Ch]
   lea     eax, [ebp-0D8h]
   mov     [esp+8], eax
   mov     eax, [ebp-2Ch]
   mov     [esp+4], eax
   lea     eax, [ebp-28h]
   mov     [esp], eax
   call    VectorScale818
   lea     eax, [ebp-148h]
   mov     [esp+4], eax
   lea     eax, [ebp-28h]
   mov     [esp], eax
   call    sub_80E88AA
   fstp    dword [ebp-2Ch]
   lea     eax, [ebp-158h]
   mov     [esp+8], eax
   mov     eax, [ebp-2Ch]
   mov     [esp+4], eax
   lea     eax, [ebp-28h]
   mov     [esp], eax
   call    VectorScale818
   mov     dword [ebp-0E4h], 1
loc_80E7B8B:
   mov     eax, [ebp-0E4h]
   cmp     eax, [ebp-30h]
   jl      loc_80E7B98
   jmp     loc_80E7C15
loc_80E7B98:
   mov     eax, [ebp-0E4h]
   cmp     eax, [ebp-0E0h]
   jnz     loc_80E7BA8
   jmp     loc_80E7C08
loc_80E7BA8:
   lea     ecx, [ebp-98h]
   mov     eax, [ebp-0E4h]
   mov     edx, [ebp+eax*4-0B8h]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   lea     eax, [eax+ecx]
   mov     [esp+4], eax
   lea     eax, [ebp-0D8h]
   mov     [esp], eax
   call    sub_80E88AA
   fld     dword [flt_814E1C0]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_80E7C08
   mov     eax, [ebp-15Ch]
   add     eax, 20h
   mov     [esp], eax
   call    sub_80E875C
   mov     dword [ebp-160h], 1
   jmp     loc_80E7CB7
loc_80E7C08:
   lea     eax, [ebp-0E4h]
   inc     dword [eax]
   jmp     loc_80E7B8B
loc_80E7C15:
   lea     eax, [ebp-0E0h]
   inc     dword [eax]
   jmp     loc_80E79BC
loc_80E7C22:
   mov     eax, [ebp-15Ch]
   add     eax, 20h
   mov     [esp+4], eax
   lea     eax, [ebp-0D8h]
   mov     [esp], eax
   call    VectorCopy805
   lea     eax, [ebp-148h]
   mov     [esp+4], eax
   lea     eax, [ebp-158h]
   mov     [esp], eax
   call    VectorCopy805
loc_80E7C55:
   lea     eax, [ebp-0Ch]
   inc     dword [eax]
   jmp     loc_80E7654
loc_80E7C5F:
   cmp     dword [ebp+10h], 0
   jz      loc_80E7C80
   mov     eax, [ebp-15Ch]
   add     eax, 20h
   mov     [esp+4], eax
   lea     eax, [ebp-148h]
   mov     [esp], eax
   call    VectorCopy805
loc_80E7C80:
   mov     eax, [ebp-15Ch]
   cmp     dword [eax+10h], 0
   jz      loc_80E7CA7
   mov     eax, [ebp-15Ch]
   add     eax, 20h
   mov     [esp+4], eax
   lea     eax, [ebp-0C8h]
   mov     [esp], eax
   call    VectorCopy805
loc_80E7CA7:
   cmp     dword [ebp-0Ch], 0
   setnz   al
   movzx   eax, al
   mov     [ebp-160h], eax
loc_80E7CB7:
   mov     eax, [ebp-160h]
   leave   
   retn    


sub_80E873A:
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


sub_80E8786:
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


sub_80E870E:
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
   pop     ebp
   retn    


VectorCopy805:
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


Fabs589:
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


Floor790:
   push    ebp
   mov     ebp, esp
   sub     esp, 18h
   fld     dword [ebp+8]
   fld     dword [flt_814E1E4]
   faddp   st1, st0
   fstp    qword [esp]
   call    floor
   fnstcw  word [ebp-2]
   movzx   eax, word [ebp-2]
   or      ax, 0C00h
   mov     [ebp-4], ax
   fldcw   word [ebp-4]
   fistp   dword [ebp-8]
   fldcw   word [ebp-2]
   mov     eax, [ebp-8]
   leave   
   retn    


VectorScale818:
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


PM_StepSlideMove:
   push    ebp
   mov     ebp, esp
   sub     esp, 138h
   mov     eax, 0
   mov     [ebp-0B8h], eax
   mov     eax, [ebp+8]
   mov     eax, [eax]
   mov     [ebp-0E0h], eax
   mov     dword [ebp-0DCh], 0
   mov     eax, [ebp-0E0h]
   mov     eax, [eax+0Ch]
   and     eax, 20h
   test    eax, eax
   jz      loc_80E7D13
   mov     dword [ebp-0BCh], 0
   mov     eax, [ebp-0E0h]
   mov     [esp], eax
   call    Jump_ClearState
   jmp     loc_80E7D5E
loc_80E7D13:
   mov     eax, [ebp+0Ch]
   cmp     dword [eax+30h], 0
   jz      loc_80E7D28
   mov     dword [ebp-0BCh], 1
   jmp     loc_80E7D5E
loc_80E7D28:
   mov     dword [ebp-0BCh], 0
   mov     eax, [ebp-0E0h]
   mov     eax, [eax+0Ch]
   and     eax, 80000h
   test    eax, eax
   jz      loc_80E7D5E
   mov     eax, [ebp-0E0h]
   cmp     dword [eax+10h], 0
   jz      loc_80E7D5E
   mov     eax, [ebp-0E0h]
   mov     [esp], eax
   call    Jump_ClearState
loc_80E7D5E:
   lea     eax, [ebp-18h]
   mov     [esp+4], eax
   mov     eax, [ebp-0E0h]
   add     eax, 14h
   mov     [esp], eax
   call    VectorCopy805
   lea     eax, [ebp-28h]
   mov     [esp+4], eax
   mov     eax, [ebp-0E0h]
   add     eax, 20h
   mov     [esp], eax
   call    VectorCopy805
   mov     eax, [ebp+10h]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    PM_SlideMove
   mov     [ebp-0B4h], eax
   mov     eax, [ebp-0E0h]
   mov     eax, [eax+0Ch]
   and     eax, 1
   test    al, al
   jz      loc_80E7DCA
   mov     eax, 41200000h
   mov     [ebp-0B0h], eax
   jmp     loc_80E7DD5
loc_80E7DCA:
   mov     eax, 41900000h
   mov     [ebp-0B0h], eax
loc_80E7DD5:
   mov     eax, [ebp-0E0h]
   cmp     dword [eax+60h], 3FFh
   jnz     loc_80E7EA5
   mov     eax, [ebp-0E0h]
   mov     eax, [eax+0Ch]
   and     eax, 80000h
   test    eax, eax
   jz      loc_80E7E14
   mov     eax, [ebp-0E0h]
   cmp     dword [eax+10h], 0
   jz      loc_80E7E14
   mov     eax, [ebp-0E0h]
   mov     [esp], eax
   call    Jump_ClearState
loc_80E7E14:
   cmp     dword [ebp-0B4h], 0
   jz      loc_80E7E6F
   mov     eax, [ebp-0E0h]
   mov     eax, [eax+0Ch]
   and     eax, 80000h
   test    eax, eax
   jz      loc_80E7E6F
   lea     eax, [ebp-0B0h]
   mov     [esp+8], eax
   lea     eax, [ebp-18h]
   mov     [esp+4], eax
   mov     eax, [ebp-0E0h]
   mov     [esp], eax
   call    Jump_GetStepHeight
   test    al, al
   jz      loc_80E7E6F
   fld     dword [ebp-0B0h]
   fld1    
   fucompp 
   fnstsw  ax
   sahf    
   ja      locret_80E86BB
   mov     dword [ebp-0DCh], 1
loc_80E7E6F:
   cmp     dword [ebp-0DCh], 0
   jnz     loc_80E7EA5
   mov     eax, [ebp-0E0h]
   mov     eax, [eax+0Ch]
   and     eax, 20h
   test    eax, eax
   jz      locret_80E86BB
   mov     eax, [ebp-0E0h]
   fld     dword [eax+28h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E7EA5
   jmp     locret_80E86BB
loc_80E7EA5:
   lea     eax, [ebp-38h]
   mov     [esp+4], eax
   mov     eax, [ebp-0E0h]
   add     eax, 14h
   mov     [esp], eax
   call    VectorCopy805
   lea     eax, [ebp-48h]
   mov     [esp+4], eax
   mov     eax, [ebp-0E0h]
   add     eax, 20h
   mov     [esp], eax
   call    VectorCopy805
   lea     eax, [ebp-0A0h]
   mov     [esp+8], eax
   lea     eax, [ebp-18h]
   mov     [esp+4], eax
   lea     eax, [ebp-38h]
   mov     [esp], eax
   call    sub_80E870E
   cmp     dword [ebp-0B4h], 0
   jz      loc_80E8012
   lea     eax, [ebp-88h]
   mov     [esp+4], eax
   lea     eax, [ebp-18h]
   mov     [esp], eax
   call    VectorCopy805
   fld     dword [ebp-0B0h]
   fld1    
   faddp   st1, st0
   fld     dword [ebp-80h]
   faddp   st1, st0
   fstp    dword [ebp-80h]
   mov     eax, [ebp+8]
   mov     eax, [eax+3Ch]
   mov     [esp+1Ch], eax
   mov     eax, [ebp-0E0h]
   mov     eax, [eax+0CCh]
   mov     [esp+18h], eax
   lea     eax, [ebp-88h]
   mov     [esp+14h], eax
   mov     eax, [ebp+8]
   add     eax, 0D0h
   mov     [esp+10h], eax
   mov     eax, [ebp+8]
   add     eax, 0C4h
   mov     [esp+0Ch], eax
   lea     eax, [ebp-18h]
   mov     [esp+8], eax
   lea     eax, [ebp-78h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    PM_playerTrace
   fld     dword [ebp-0B0h]
   fld1    
   faddp   st1, st0
   fmul    dword [ebp-78h]
   fld1    
   fsubp   st1, st0
   fstp    dword [ebp-0B8h]
   fld     dword [ebp-0B8h]
   fld1    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E7FA2
   jmp     loc_80E7FAF
loc_80E7FA2:
   mov     eax, 0
   mov     [ebp-0B8h], eax
   jmp     loc_80E8012
loc_80E7FAF:
   fld     dword [ebp-10h]
   fadd    dword [ebp-0B8h]
   fstp    dword [esp+0Ch]
   mov     eax, [ebp-84h]
   mov     [esp+8], eax
   mov     eax, [ebp-88h]
   mov     [esp+4], eax
   mov     eax, [ebp-0E0h]
   add     eax, 14h
   mov     [esp], eax
   call    sub_80E8786
   mov     eax, [ebp-0E0h]
   add     eax, 20h
   mov     [esp+4], eax
   lea     eax, [ebp-28h]
   mov     [esp], eax
   call    VectorCopy805
   mov     eax, [ebp+10h]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    PM_SlideMove
loc_80E8012:
   cmp     dword [ebp-0BCh], 0
   jnz     loc_80E8033
   fld     dword [ebp-0B8h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80E8033
   jp      loc_80E8033
   jmp     loc_80E81F2
loc_80E8033:
   lea     eax, [ebp-98h]
   mov     [esp+4], eax
   mov     eax, [ebp-0E0h]
   add     eax, 14h
   mov     [esp], eax
   call    VectorCopy805
   fld     dword [ebp-90h]
   fsub    dword [ebp-0B8h]
   fstp    dword [ebp-90h]
   cmp     dword [ebp-0BCh], 0
   jz      loc_80E807D
   fld     dword [ebp-90h]
   fld     dword [flt_814E1C4]
   fsubp   st1, st0
   fstp    dword [ebp-90h]
loc_80E807D:
   mov     eax, [ebp+8]
   mov     eax, [eax+3Ch]
   mov     [esp+1Ch], eax
   mov     eax, [ebp-0E0h]
   mov     eax, [eax+0CCh]
   mov     [esp+18h], eax
   lea     eax, [ebp-98h]
   mov     [esp+14h], eax
   mov     eax, [ebp+8]
   add     eax, 0D0h
   mov     [esp+10h], eax
   mov     eax, [ebp+8]
   add     eax, 0C4h
   mov     [esp+0Ch], eax
   mov     eax, [ebp-0E0h]
   add     eax, 14h
   mov     [esp+8], eax
   lea     eax, [ebp-78h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    PM_playerTrace
   cmp     word [ebp-5Ch], 3Fh
   ja      loc_80E8114
   mov     eax, [ebp-0E0h]
   add     eax, 14h
   mov     [esp+4], eax
   lea     eax, [ebp-38h]
   mov     [esp], eax
   call    VectorCopy805
   mov     eax, [ebp-0E0h]
   add     eax, 20h
   mov     [esp+4], eax
   lea     eax, [ebp-48h]
   mov     [esp], eax
   call    VectorCopy805
   jmp     locret_80E86BB
loc_80E8114:
   fld     dword [ebp-78h]
   fld1    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E8125
   jmp     loc_80E81C5
loc_80E8125:
   fld     dword [ebp-6Ch]
   fld     dword [flt_814E1C8]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E8137
   jmp     loc_80E816C
loc_80E8137:
   mov     eax, [ebp-0E0h]
   add     eax, 14h
   mov     [esp+4], eax
   lea     eax, [ebp-38h]
   mov     [esp], eax
   call    VectorCopy805
   mov     eax, [ebp-0E0h]
   add     eax, 20h
   mov     [esp+4], eax
   lea     eax, [ebp-48h]
   mov     [esp], eax
   call    VectorCopy805
   jmp     locret_80E86BB
loc_80E816C:
   mov     eax, [ebp-0E0h]
   add     eax, 14h
   mov     [esp+0Ch], eax
   mov     eax, [ebp-78h]
   mov     [esp+8], eax
   lea     eax, [ebp-98h]
   mov     [esp+4], eax
   mov     eax, [ebp-0E0h]
   add     eax, 14h
   mov     [esp], eax
   call    Vec3Lerp
   mov     eax, [ebp-0E0h]
   add     eax, 20h
   mov     [esp+8], eax
   lea     eax, [ebp-78h]
   add     eax, 4
   mov     [esp+4], eax
   mov     eax, [ebp-0E0h]
   add     eax, 20h
   mov     [esp], eax
   call    PM_ClipVelocity
   jmp     loc_80E81F2
loc_80E81C5:
   fld     dword [ebp-0B8h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80E81DA
   jp      loc_80E81DA
   jmp     loc_80E81F2
loc_80E81DA:
   mov     edx, [ebp-0E0h]
   mov     eax, [ebp-0E0h]
   fld     dword [eax+1Ch]
   fsub    dword [ebp-0B8h]
   fstp    dword [edx+1Ch]
loc_80E81F2:
   lea     eax, [ebp-0A8h]
   mov     [esp+8], eax
   lea     eax, [ebp-18h]
   mov     [esp+4], eax
   mov     eax, [ebp-0E0h]
   add     eax, 14h
   mov     [esp], eax
   call    sub_80E870E
   mov     eax, [ebp-0E0h]
   add     eax, 20h
   mov     [esp+4], eax
   lea     eax, [ebp-0A8h]
   mov     [esp], eax
   call    sub_80E873A
   fstp    dword [ebp-104h]
   mov     eax, [ebp-0E0h]
   add     eax, 20h
   mov     [esp+4], eax
   lea     eax, [ebp-0A0h]
   mov     [esp], eax
   call    sub_80E873A
   fld     dword [flt_814E1CC]
   faddp   st1, st0
   fld     dword [ebp-104h]
   fxch    st1
   fucompp 
   fnstsw  ax
   test    ah, 5
   setz    al
   movzx   eax, al
   mov     [ebp-0ACh], eax
   cmp     dword [ebp-0ACh], 0
   jnz     loc_80E82A0
   cmp     dword [ebp-0DCh], 0
   jz      loc_80E8407
   mov     eax, [ebp-0E0h]
   mov     [esp], eax
   call    Jump_IsPlayerAboveMax
   test    al, al
   jnz     loc_80E82A0
   jmp     loc_80E8407
loc_80E82A0:
   mov     eax, [ebp-0E0h]
   add     eax, 14h
   mov     [esp+4], eax
   lea     eax, [ebp-38h]
   mov     [esp], eax
   call    VectorCopy805
   mov     eax, [ebp-0E0h]
   add     eax, 20h
   mov     [esp+4], eax
   lea     eax, [ebp-48h]
   mov     [esp], eax
   call    VectorCopy805
   mov     eax, 0
   mov     [ebp-0B8h], eax
   cmp     dword [ebp-0BCh], 0
   jz      loc_80E8407
   lea     eax, [ebp-98h]
   mov     [esp+4], eax
   mov     eax, [ebp-0E0h]
   add     eax, 14h
   mov     [esp], eax
   call    VectorCopy805
   fld     dword [ebp-90h]
   fld     dword [flt_814E1C4]
   fsubp   st1, st0
   fstp    dword [ebp-90h]
   mov     eax, [ebp+8]
   mov     eax, [eax+3Ch]
   mov     [esp+1Ch], eax
   mov     eax, [ebp-0E0h]
   mov     eax, [eax+0CCh]
   mov     [esp+18h], eax
   lea     eax, [ebp-98h]
   mov     [esp+14h], eax
   mov     eax, [ebp+8]
   add     eax, 0D0h
   mov     [esp+10h], eax
   mov     eax, [ebp+8]
   add     eax, 0C4h
   mov     [esp+0Ch], eax
   mov     eax, [ebp-0E0h]
   add     eax, 14h
   mov     [esp+8], eax
   lea     eax, [ebp-78h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    PM_playerTrace
   fld     dword [ebp-78h]
   fld1    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E8383
   jmp     loc_80E8407
loc_80E8383:
   lea     eax, [ebp-0D8h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp-78h]
   mov     [esp+8], eax
   lea     eax, [ebp-98h]
   mov     [esp+4], eax
   mov     eax, [ebp-0E0h]
   add     eax, 14h
   mov     [esp], eax
   call    Vec3Lerp
   mov     eax, [ebp-0E0h]
   fld     dword [ebp-0D0h]
   fsub    dword [eax+1Ch]
   fstp    dword [ebp-0B8h]
   mov     eax, [ebp-0E0h]
   add     eax, 14h
   mov     [esp+4], eax
   lea     eax, [ebp-0D8h]
   mov     [esp], eax
   call    VectorCopy805
   mov     eax, [ebp-0E0h]
   add     eax, 20h
   mov     [esp+8], eax
   lea     eax, [ebp-78h]
   add     eax, 4
   mov     [esp+4], eax
   mov     eax, [ebp-0E0h]
   add     eax, 20h
   mov     [esp], eax
   call    PM_ClipVelocity
loc_80E8407:
   cmp     dword [ebp-0DCh], 0
   jz      loc_80E8425
   lea     eax, [ebp-38h]
   mov     [esp+4], eax
   mov     eax, [ebp-0E0h]
   mov     [esp], eax
   call    Jump_ClampVelocity
loc_80E8425:
   cmp     dword [ebp-0BCh], 0
   jz      locret_80E86BB
   mov     eax, [ebp-0E0h]
   cmp     dword [eax+4], 5
   jg      locret_80E86BB
   lea     eax, [ebp-28h]
   mov     [esp+8], eax
   lea     eax, [ebp-18h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    PM_VerifyPronePosition
   test    eax, eax
   jz      locret_80E86BB
   mov     eax, [ebp-0E0h]
   fld     dword [eax+1Ch]
   fsub    dword [ebp-30h]
   fstp    dword [esp]
   call    Fabs589
   fld     dword [flt_814E1D0]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E848B
   jmp     locret_80E86BB
loc_80E848B:
   mov     eax, [ebp-0E0h]
   fld     dword [eax+1Ch]
   fsub    dword [ebp-30h]
   fstp    dword [esp]
   call    Floor790
   mov     [ebp-0E4h], eax
   cmp     dword [ebp-0E4h], 0
   jz      locret_80E86BB
   mov     eax, 3F4CCCCDh
   mov     [ebp-0E8h], eax
   cmp     dword [ebp-0E4h], 0FFFFFFF0h
   jge     loc_80E84D2
   mov     dword [ebp-0E4h], 0FFFFFFF0h
   jmp     loc_80E84E5
loc_80E84D2:
   cmp     dword [ebp-0E4h], 18h
   jle     loc_80E84E5
   mov     dword [ebp-0E4h], 18h
loc_80E84E5:
   lea     eax, [ebp-0E4h]
   sub     dword [eax], 0FFFFFF80h
   mov     eax, [ebp-0E0h]
   mov     [esp+8], eax
   mov     eax, [ebp-0E4h]
   mov     [esp+4], eax
   mov     dword [esp], 8Fh
   call    BG_AddPredictableEventToPlayerstate
   mov     eax, [ebp-0E0h]
   fld     dword [eax+1Ch]
   fsub    dword [ebp-10h]
   fstp    dword [esp]
   call    Fabs589
   fstp    dword [ebp-0ECh]
   fld     dword [ebp-0ECh]
   fdiv    dword [ebp-0B0h]
   fld1    
   fsubrp  st1, st0
   fld     dword [flt_814E1D4]
   fmulp   st1, st0
   fld     dword [flt_814E1D8]
   faddp   st1, st0
   fstp    dword [ebp-0F0h]
   mov     eax, [ebp-0E0h]
   add     eax, 20h
   mov     [esp+8], eax
   mov     eax, [ebp-0F0h]
   mov     [esp+4], eax
   mov     eax, [ebp-0E0h]
   add     eax, 20h
   mov     [esp], eax
   call    VectorScale818
   lea     eax, [ebp-0E4h]
   add     dword [eax], 0FFFFFF80h
   mov     eax, [ebp-0E4h]
   mov     [ebp-108h], eax
   cmp     dword [ebp-108h], 0
   jns     loc_80E859A
   neg     dword [ebp-108h]
loc_80E859A:
   cmp     dword [ebp-108h], 3
   jle     locret_80E86BB
   mov     eax, [ebp-0E0h]
   cmp     dword [eax+60h], 3FFh
   jz      locret_80E86BB
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    PM_ShouldMakeFootsteps
   test    eax, eax
   jz      locret_80E86BB
   mov     eax, [ebp-0E4h]
   mov     [ebp-10Ch], eax
   cmp     dword [ebp-10Ch], 0
   jns     loc_80E85E8
   neg     dword [ebp-10Ch]
loc_80E85E8:
   mov     eax, [ebp-10Ch]
   sar     eax, 1Fh
   shr     eax, 1Fh
   add     eax, [ebp-10Ch]
   sar     eax, 1
   mov     [ebp-0E4h], eax
   cmp     dword [ebp-0E4h], 4
   jle     loc_80E8615
   mov     dword [ebp-0E4h], 4
loc_80E8615:
   fild    dword [ebp-0E4h]
   fld     dword [flt_814E1DC]
   fmulp   st1, st0
   fld     dword [flt_814E1E0]
   faddp   st1, st0
   fstp    dword [ebp-0F8h]
   mov     eax, [ebp-0E0h]
   mov     eax, [eax+8]
   mov     [ebp-0F4h], eax
   mov     edx, [ebp-0E0h]
   fild    dword [ebp-0F4h]
   fadd    dword [ebp-0F8h]
   fnstcw  word [ebp-0FAh]
   movzx   eax, word [ebp-0FAh]
   or      ax, 0C00h
   mov     [ebp-0FCh], ax
   fldcw   word [ebp-0FCh]
   fistp   dword [ebp-100h]
   fldcw   word [ebp-0FAh]
   mov     eax, [ebp-100h]
   and     eax, 0FFh
   mov     [edx+8], eax
   mov     dword [esp+10h], 1
   mov     eax, [ebp-0E0h]
   mov     eax, [eax+8]
   mov     [esp+0Ch], eax
   mov     eax, [ebp-0F4h]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    PM_FootstepEvent
locret_80E86BB:
   leave   
   retn    


section .rdata
flt_814E1E0     dd 7.0
flt_814E1DC     dd 1.25
flt_814E1D8     dd 0.19999999
flt_814DD80     dd 0.001
flt_814E1B8     dd 0.5
flt_814E1BC     dd 0.99900001
flt_814E1C0     dd 0.1
flt_814E1CC     dd 0.001
flt_814E1E4     dd 0.5
flt_814E1D4     dd 0.80000001
flt_814E1D0     dd 0.5
flt_814E1C4     dd 9.0
flt_814E1C8     dd 0.30000001