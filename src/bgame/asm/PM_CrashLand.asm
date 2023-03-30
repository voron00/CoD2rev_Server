extern BG_AddPredictableEventToPlayerstate
extern Com_Printf
extern sqrt
extern BG_AnimScriptEvent
extern PM_AddEvent
extern PM_GroundSurfaceType
extern bg_fallDamageMinHeight
extern bg_fallDamageMaxHeight


global PM_CrashLand


section .text


sub_80E731C:
   push    ebp
   mov     ebp, esp
   sub     esp, 4
   cmp     dword [ebp+8], 0
   js      loc_80E7330
   mov     eax, [ebp+0Ch]
   mov     [ebp-4], eax
   jmp     loc_80E7336
loc_80E7330:
   mov     eax, [ebp+10h]
   mov     [ebp-4], eax
loc_80E7336:
   mov     eax, [ebp-4]
   leave   
   retn    


sub_80E6EC4:
   push    ebp
   mov     ebp, esp
   sub     esp, 18h
   mov     eax, [ebp+8]
   mov     [esp+8], eax
   mov     eax, [ebp+10h]
   mov     [esp+4], eax
   mov     edx, [ebp+10h]
   mov     eax, [ebp+8]
   sub     eax, edx
   mov     [esp], eax
   call    sub_80E731C
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     edx, [ebp+8]
   mov     eax, [ebp+0Ch]
   sub     eax, edx
   mov     [esp], eax
   call    sub_80E731C
   leave   
   retn    


sub_80E0968:
   push    ebp
   mov     ebp, esp
   sub     esp, 8
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    PM_GroundSurfaceType
   mov     [ebp-4], eax
   mov     eax, [ebp-4]
   add     eax, 5Dh
   leave   
   retn    


sub_80E0938:
   push    ebp
   mov     ebp, esp
   sub     esp, 18h
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    PM_GroundSurfaceType
   mov     [ebp-4], eax
   cmp     dword [ebp-4], 0
   jnz     loc_80E095B
   mov     dword [ebp-8], 0
   jmp     loc_80E0962
loc_80E095B:
   mov     eax, [ebp-4]
   inc     eax
   mov     [ebp-8], eax
loc_80E0962:
   mov     eax, [ebp-8]
   leave   
   retn    


sub_80E0906:
   push    ebp
   mov     ebp, esp
   sub     esp, 18h
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    PM_GroundSurfaceType
   mov     [ebp-4], eax
   cmp     dword [ebp-4], 0
   jnz     loc_80E0929
   mov     dword [ebp-8], 0
   jmp     loc_80E0932
loc_80E0929:
   mov     eax, [ebp-4]
   add     eax, 18h
   mov     [ebp-8], eax
loc_80E0932:
   mov     eax, [ebp-8]
   leave   
   retn    


sub_80E0984:
   push    ebp
   mov     ebp, esp
   sub     esp, 8
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    PM_GroundSurfaceType
   mov     [ebp-4], eax
   mov     eax, [ebp-4]
   add     eax, 74h
   leave   
   retn    


VectorScale707:
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


PM_CrashLand:
   push    ebp
   mov     ebp, esp
   sub     esp, 68h
   mov     eax, [ebp+0Ch]
   mov     edx, [ebp+8]
   fld     dword [eax+68h]
   fsub    dword [edx+1Ch]
   fstp    dword [ebp-0Ch]
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+74h]
   mov     [ebp-10h], eax
   mov     eax, [ebp+8]
   fild    dword [eax+48h]
   fstp    dword [ebp-4Ch]
   mov     eax, [ebp-4Ch]
   xor     eax, 80000000h
   mov     [ebp-14h], eax
   fld     dword [ebp-14h]
   fld     dword [flt_814DEC4]
   fmulp   st1, st0
   fstp    dword [ebp-1Ch]
   mov     eax, [ebp-10h]
   mov     [ebp-20h], eax
   mov     eax, [ebp-0Ch]
   mov     [ebp-24h], eax
   fld     dword [ebp-20h]
   fmul    dword [ebp-20h]
   fld     dword [ebp-1Ch]
   fld     dword [flt_814DEC8]
   fmulp   st1, st0
   fmul    dword [ebp-24h]
   fsubp   st1, st0
   fstp    dword [ebp-28h]
   fld     dword [ebp-28h]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      locret_80E0DA0
   mov     eax, [ebp-20h]
   mov     [ebp-4Ch], eax
   fld     dword [ebp-4Ch]
   fchs    
   fstp    qword [ebp-48h]
   fld     dword [ebp-28h]
   fstp    qword [esp]
   call    sqrt
   fld     qword [ebp-48h]
   fsubrp  st1, st0
   fld     dword [ebp-1Ch]
   fadd    st0, st0
   fdivp   st1, st0
   fstp    dword [ebp-18h]
   fld     dword [ebp-18h]
   fmul    dword [ebp-14h]
   fadd    dword [ebp-10h]
   fstp    dword [ebp-4Ch]
   mov     eax, [ebp-4Ch]
   xor     eax, 80000000h
   mov     [ebp-4], eax
   fld     dword [ebp-4]
   fmul    dword [ebp-4]
   mov     eax, [ebp+8]
   fild    dword [eax+48h]
   fadd    st0, st0
   fdivp   st1, st0
   fstp    dword [ebp-8]
   mov     eax, [bg_fallDamageMaxHeight]
   mov     edx, [bg_fallDamageMinHeight]
   fld     dword [eax+8]
   fld     dword [edx+8]
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_80E0A81
   jmp     loc_80E0A99
loc_80E0A81:
   mov     dword [esp], aBgFalldamagema_0
   call    Com_Printf
   mov     dword [ebp-34h], 0
   jmp     loc_80E0B47
loc_80E0A99:
   mov     eax, [bg_fallDamageMinHeight]
   fld     dword [ebp-8]
   fld     dword [eax+8]
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_80E0AC3
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+48h]
   and     eax, 1
   test    al, al
   jnz     loc_80E0AC3
   mov     eax, [ebp+8]
   cmp     dword [eax+4], 5
   jg      loc_80E0AC3
   jmp     loc_80E0ACC
loc_80E0AC3:
   mov     dword [ebp-34h], 0
   jmp     loc_80E0B47
loc_80E0ACC:
   mov     eax, [bg_fallDamageMaxHeight]
   fld     dword [ebp-8]
   fld     dword [eax+8]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_80E0AE2
   jmp     loc_80E0AEB
loc_80E0AE2:
   mov     dword [ebp-34h], 64h
   jmp     loc_80E0B47
loc_80E0AEB:
   mov     eax, [bg_fallDamageMinHeight]
   fld     dword [ebp-8]
   fsub    dword [eax+8]
   mov     eax, [bg_fallDamageMaxHeight]
   mov     edx, [bg_fallDamageMinHeight]
   fld     dword [eax+8]
   fsub    dword [edx+8]
   fdivp   st1, st0
   fld     dword [flt_814DECC]
   fmulp   st1, st0
   fnstcw  word [ebp-3Eh]
   movzx   eax, word [ebp-3Eh]
   or      ax, 0C00h
   mov     [ebp-40h], ax
   fldcw   word [ebp-40h]
   fistp   dword [ebp-34h]
   fldcw   word [ebp-3Eh]
   mov     dword [esp+8], 64h
   mov     dword [esp+4], 0
   mov     eax, [ebp-34h]
   mov     [esp], eax
   call    sub_80E6EC4
   mov     [ebp-34h], eax
loc_80E0B47:
   fld     dword [ebp-8]
   fld     dword [flt_814DED0]
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_80E0B59
   jmp     loc_80E0B62
loc_80E0B59:
   mov     dword [ebp-30h], 0
   jmp     loc_80E0BCD
loc_80E0B62:
   fld     dword [ebp-8]
   fld     dword [flt_814DED0]
   fsubp   st1, st0
   fld     dword [flt_814DED4]
   fdivp   st1, st0
   fld     dword [flt_814DEC8]
   fmulp   st1, st0
   fld     dword [flt_814DEC8]
   faddp   st1, st0
   fnstcw  word [ebp-3Eh]
   movzx   eax, word [ebp-3Eh]
   or      ax, 0C00h
   mov     [ebp-40h], ax
   fldcw   word [ebp-40h]
   fistp   dword [ebp-30h]
   fldcw   word [ebp-3Eh]
   cmp     dword [ebp-30h], 18h
   jle     loc_80E0BAA
   mov     dword [ebp-30h], 18h
loc_80E0BAA:
   mov     dword [esp+0Ch], 1
   mov     dword [esp+8], 0
   mov     dword [esp+4], 5
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    BG_AnimScriptEvent
loc_80E0BCD:
   cmp     dword [ebp-34h], 0
   jz      loc_80E0CE9
   cmp     dword [ebp-34h], 63h
   jg      loc_80E0CA0
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+48h]
   and     eax, 2
   test    eax, eax
   jnz     loc_80E0CA0
   mov     edx, [ebp-34h]
   mov     eax, edx
   shl     eax, 3
   add     eax, edx
   shl     eax, 2
   sub     eax, edx
   add     eax, 1F4h
   mov     [ebp-38h], eax
   cmp     dword [ebp-38h], 7D0h
   jle     loc_80E0C19
   mov     dword [ebp-38h], 7D0h
loc_80E0C19:
   cmp     dword [ebp-38h], 1F4h
   jg      loc_80E0C2C
   mov     eax, 3F000000h
   mov     [ebp-2Ch], eax
   jmp     loc_80E0C65
loc_80E0C2C:
   cmp     dword [ebp-38h], 5DBh
   jle     loc_80E0C3F
   mov     eax, 3E4CCCCDh
   mov     [ebp-2Ch], eax
   jmp     loc_80E0C65
loc_80E0C3F:
   fild    dword [ebp-38h]
   fld     dword [flt_814DED8]
   fsubp   st1, st0
   fld     dword [flt_814DEDC]
   fdivp   st1, st0
   fld     dword [flt_814DEE0]
   fmulp   st1, st0
   fld     dword [flt_814DEC4]
   fsubrp  st1, st0
   fstp    dword [ebp-2Ch]
loc_80E0C65:
   mov     edx, [ebp+8]
   mov     eax, [ebp-38h]
   mov     [edx+10h], eax
   mov     edx, [ebp+8]
   mov     eax, [ebp+8]
   mov     eax, [eax+0Ch]
   or      eax, 200h
   mov     [edx+0Ch], eax
   mov     eax, [ebp+8]
   add     eax, 20h
   mov     [esp+8], eax
   mov     eax, [ebp-2Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   add     eax, 20h
   mov     [esp], eax
   call    VectorScale707
   jmp     loc_80E0CC1
loc_80E0CA0:
   mov     eax, [ebp+8]
   add     eax, 20h
   mov     [esp+8], eax
   mov     eax, 3F2B851Fh
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   add     eax, 20h
   mov     [esp], eax
   call    VectorScale707
loc_80E0CC1:
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    sub_80E0984
   mov     edx, eax
   mov     eax, [ebp+8]
   mov     [esp+8], eax
   mov     eax, [ebp-34h]
   mov     [esp+4], eax
   mov     [esp], edx
   call    BG_AddPredictableEventToPlayerstate
   jmp     locret_80E0DA0
loc_80E0CE9:
   fld     dword [ebp-8]
   fld     dword [flt_814DEC8]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E0D00
   jmp     locret_80E0DA0
loc_80E0D00:
   fld     dword [ebp-8]
   fld     dword [flt_814DEE4]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E0D12
   jmp     loc_80E0D2E
loc_80E0D12:
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    sub_80E0906
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    PM_AddEvent
   jmp     locret_80E0DA0
loc_80E0D2E:
   fld     dword [ebp-8]
   fld     dword [flt_814DED0]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80E0D40
   jmp     loc_80E0D5C
loc_80E0D40:
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    sub_80E0938
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    PM_AddEvent
   jmp     locret_80E0DA0
loc_80E0D5C:
   mov     eax, [ebp+8]
   add     eax, 20h
   mov     [esp+8], eax
   mov     eax, 3F2B851Fh
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   add     eax, 20h
   mov     [esp], eax
   call    VectorScale707
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    sub_80E0968
   mov     edx, eax
   mov     eax, [ebp+8]
   mov     [esp+8], eax
   mov     eax, [ebp-30h]
   mov     [esp+4], eax
   mov     [esp], edx
   call    BG_AddPredictableEventToPlayerstate
locret_80E0DA0:
   leave   
   retn    


section .rdata
flt_814DED0     dd 12.0
flt_814DEC8     dd 4.0
flt_814DEC4     dd 0.5
flt_814DEE4     dd 8.0
flt_814DEE0     dd 0.30000001
flt_814DECC     dd 100.0
flt_814DEDC     dd 1000.0
flt_814DED4     dd 26.0
flt_814DED8     dd 500.0

aBgFalldamagema_0 db 'bg_fallDamageMaxHeight must be greater than bg_fallDamageMinHeigh'