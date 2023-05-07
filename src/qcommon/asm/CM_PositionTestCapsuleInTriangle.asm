extern cm


global CM_PositionTestCapsuleInTriangle


section .text


DotProduct123:
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


VectorSubtract63262:
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


CM_Fabs3:
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


VectorMA712:
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


VectorCopy834:
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


VectorLengthSquared:
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
   mov     eax, [ebp+8]
   add     eax, 8
   mov     edx, [ebp+8]
   add     edx, 8
   fld     dword [eax]
   fmul    dword [edx]
   faddp   st1, st0
   pop     ebp
   retn  


CM_PositionTestCapsuleInTriangle:
   push    ebp
   mov     ebp, esp
   sub     esp, 0C8h
   mov     eax, [ebp+8]
   mov     eax, [eax+90h]
   mov     [ebp-0A0h], eax
   mov     eax, [ebp+0Ch]
   fld     dword [eax+8]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80553DB
   jmp     loc_80553E5
loc_80553DB:
   lea     eax, [ebp-0A0h]
   xor     byte [eax+3], 80h
loc_80553E5:
   lea     eax, [ebp-88h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    VectorCopy834
   fld     dword [ebp-80h]
   fsub    dword [ebp-0A0h]
   fstp    dword [ebp-80h]
   mov     eax, [ebp+8]
   mov     eax, [eax+8Ch]
   mov     [ebp-64h], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   lea     eax, [ebp-88h]
   mov     [esp], eax
   call    DotProduct123
   mov     eax, [ebp+0Ch]
   fsub    dword [eax+0Ch]
   fstp    dword [ebp-90h]
   fld     dword [ebp-90h]
   fld     dword [ebp-64h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     locret_80558EA
   mov     eax, [ebp-64h]
   xor     eax, 80000000h
   mov     [ebp-60h], eax
   fld     dword [ebp-90h]
   fld     dword [ebp-60h]
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8055469
   jmp     loc_8055615
loc_8055469:
   fld     dword [ebp-0A0h]
   fadd    st0, st0
   fstp    dword [ebp-8Ch]
   mov     eax, [ebp+0Ch]
   fld     dword [ebp-8Ch]
   fmul    dword [eax+8]
   fld     dword [ebp-90h]
   faddp   st1, st0
   fstp    dword [ebp-6Ch]
   fld     dword [ebp-6Ch]
   fld     dword [ebp-60h]
   fucompp 
   fnstsw  ax
   sahf    
   jnb     locret_80558EA
   fld     dword [ebp-60h]
   fsub    dword [ebp-90h]
   mov     eax, [ebp+0Ch]
   fdiv    dword [eax+8]
   fstp    dword [ebp-68h]
   mov     eax, [ebp+0Ch]
   add     eax, 10h
   mov     [esp+4], eax
   lea     eax, [ebp-88h]
   mov     [esp], eax
   call    DotProduct123
   mov     eax, [ebp+0Ch]
   fsub    dword [eax+1Ch]
   fstp    dword [ebp-94h]
   mov     eax, [ebp+0Ch]
   add     eax, 20h
   mov     [esp+4], eax
   lea     eax, [ebp-88h]
   mov     [esp], eax
   call    DotProduct123
   mov     eax, [ebp+0Ch]
   fsub    dword [eax+2Ch]
   fstp    dword [ebp-98h]
   mov     eax, [ebp+0Ch]
   fld     dword [ebp-68h]
   fmul    dword [eax+18h]
   fld     dword [ebp-94h]
   faddp   st1, st0
   fstp    dword [ebp-2Ch]
   fld     dword [ebp-2Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_805551D
   jmp     loc_805556F
loc_805551D:
   mov     eax, [ebp+0Ch]
   fld     dword [ebp-68h]
   fmul    dword [eax+28h]
   fld     dword [ebp-98h]
   faddp   st1, st0
   fstp    dword [ebp-30h]
   fld     dword [ebp-30h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8055541
   jmp     loc_805556F
loc_8055541:
   fld     dword [ebp-2Ch]
   fadd    dword [ebp-30h]
   fld1    
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8055552
   jmp     loc_805556F
loc_8055552:
   mov     eax, [ebp+10h]
   mov     byte [eax+23h], 1
   mov     eax, [ebp+10h]
   mov     byte [eax+22h], 1
   mov     edx, [ebp+10h]
   mov     eax, 0
   mov     [edx], eax
   jmp     locret_80558EA
loc_805556F:
   fld     dword [ebp-6Ch]
   fld     dword [ebp-64h]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_805557E
   jmp     loc_8055589
loc_805557E:
   mov     eax, [ebp-8Ch]
   mov     [ebp-68h], eax
   jmp     loc_805559B
loc_8055589:
   fld     dword [ebp-64h]
   fsub    dword [ebp-90h]
   mov     eax, [ebp+0Ch]
   fdiv    dword [eax+8]
   fstp    dword [ebp-68h]
loc_805559B:
   mov     eax, [ebp+0Ch]
   fld     dword [ebp-68h]
   fmul    dword [eax+18h]
   fld     dword [ebp-94h]
   faddp   st1, st0
   fstp    dword [ebp-2Ch]
   fld     dword [ebp-2Ch]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      locret_80558EA
   mov     eax, [ebp+0Ch]
   fld     dword [ebp-68h]
   fmul    dword [eax+28h]
   fld     dword [ebp-98h]
   faddp   st1, st0
   fstp    dword [ebp-30h]
   fld     dword [ebp-30h]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      locret_80558EA
   fld     dword [ebp-2Ch]
   fadd    dword [ebp-30h]
   fld1    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      locret_80558EA
   mov     eax, [ebp+10h]
   mov     byte [eax+23h], 1
   mov     eax, [ebp+10h]
   mov     byte [eax+22h], 1
   mov     edx, [ebp+10h]
   mov     eax, 0
   mov     [edx], eax
   jmp     locret_80558EA
loc_8055615:
   mov     dword [ebp-34h], 0
   lea     eax, [ebp-28h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+8], eax
   mov     eax, [ebp-90h]
   xor     eax, 80000000h
   mov     [esp+4], eax
   lea     eax, [ebp-88h]
   mov     [esp], eax
   call    VectorMA712
   mov     eax, [ebp+0Ch]
   add     eax, 10h
   mov     [esp+4], eax
   lea     eax, [ebp-28h]
   mov     [esp], eax
   call    DotProduct123
   mov     eax, [ebp+0Ch]
   fsub    dword [eax+1Ch]
   fstp    dword [ebp-2Ch]
   mov     eax, [ebp+0Ch]
   add     eax, 20h
   mov     [esp+4], eax
   lea     eax, [ebp-28h]
   mov     [esp], eax
   call    DotProduct123
   mov     eax, [ebp+0Ch]
   fsub    dword [eax+2Ch]
   fstp    dword [ebp-30h]
   mov     eax, [ebp-34h]
   mov     [ebp-0A4h], eax
   fld     dword [ebp-2Ch]
   fadd    dword [ebp-30h]
   fld1    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_805569F
   jmp     loc_80556A6
loc_805569F:
   or      dword [ebp-0A4h], 1
loc_80556A6:
   mov     eax, [ebp-0A4h]
   mov     [ebp-34h], eax
   mov     eax, [ebp-34h]
   mov     [ebp-0A8h], eax
   fld     dword [ebp-2Ch]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80556C6
   jmp     loc_80556CD
loc_80556C6:
   or      dword [ebp-0A8h], 2
loc_80556CD:
   mov     eax, [ebp-0A8h]
   mov     [ebp-34h], eax
   mov     eax, [ebp-34h]
   mov     [ebp-0ACh], eax
   fld     dword [ebp-30h]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80556ED
   jmp     loc_80556F4
loc_80556ED:
   or      dword [ebp-0ACh], 4
loc_80556F4:
   mov     eax, [ebp-0ACh]
   mov     [ebp-34h], eax
   cmp     dword [ebp-34h], 0
   jnz     loc_8055720
   mov     eax, [ebp+10h]
   mov     byte [eax+23h], 1
   mov     eax, [ebp+10h]
   mov     byte [eax+22h], 1
   mov     edx, [ebp+10h]
   mov     eax, 0
   mov     [edx], eax
   jmp     locret_80558EA
loc_8055720:
   mov     dword [ebp-0Ch], 0
loc_8055727:
   cmp     dword [ebp-0Ch], 2
   jle     loc_8055732
   jmp     locret_80558EA
loc_8055732:
   movzx   ecx, byte [ebp-0Ch]
   mov     eax, [ebp-34h]
   sar     eax, cl
   and     eax, 1
   test    al, al
   jz      loc_805585A
   mov     eax, [ebp+0Ch]
   mov     edx, [ebp-0Ch]
   mov     eax, [eax+edx*4+3Ch]
   mov     [ebp-50h], eax
   cmp     dword [ebp-50h], 0
   jns     loc_805575E
   jmp     loc_80558E0
loc_805575E:
   mov     edx, [ebp-50h]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 4
   add     eax, [cm+50h]
   mov     [ebp-54h], eax
   lea     eax, [ebp-48h]
   mov     [esp+8], eax
   mov     eax, [ebp-54h]
   mov     [esp+4], eax
   lea     eax, [ebp-88h]
   mov     [esp], eax
   call    VectorSubtract63262
   mov     eax, [ebp-54h]
   add     eax, 24h
   mov     [esp+4], eax
   lea     eax, [ebp-48h]
   mov     [esp], eax
   call    DotProduct123
   fstp    dword [ebp-4Ch]
   fld     dword [ebp-4Ch]
   fld     dword [flt_813B834]
   fsubp   st1, st0
   fstp    dword [esp]
   call    CM_Fabs3
   fld     dword [flt_813B834]
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_80557CC
   jmp     loc_80558E0
loc_80557CC:
   mov     eax, [ebp-54h]
   add     eax, 24h
   mov     [esp], eax
   call    VectorLengthSquared
   fstp    dword [ebp-9Ch]
   fld     dword [ebp-4Ch]
   fdiv    dword [ebp-9Ch]
   fstp    dword [ebp-4Ch]
   lea     eax, [ebp-48h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp-54h]
   add     eax, 24h
   mov     [esp+8], eax
   mov     eax, [ebp-4Ch]
   xor     eax, 80000000h
   mov     [esp+4], eax
   lea     eax, [ebp-48h]
   mov     [esp], eax
   call    VectorMA712
   lea     eax, [ebp-48h]
   mov     [esp], eax
   call    VectorLengthSquared
   mov     eax, [ebp+8]
   mov     edx, [ebp+8]
   fld     dword [eax+8Ch]
   fmul    dword [edx+8Ch]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_805583D
   jmp     loc_80558E0
loc_805583D:
   mov     eax, [ebp+10h]
   mov     byte [eax+23h], 1
   mov     eax, [ebp+10h]
   mov     byte [eax+22h], 1
   mov     edx, [ebp+10h]
   mov     eax, 0
   mov     [edx], eax
   jmp     locret_80558EA
loc_805585A:
   mov     eax, [ebp+0Ch]
   mov     edx, [ebp-0Ch]
   mov     eax, [eax+edx*4+30h]
   mov     [ebp-58h], eax
   cmp     dword [ebp-58h], 0
   jns     loc_805586F
   jmp     loc_80558E0
loc_805586F:
   mov     edx, [ebp-58h]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   add     eax, [cm+48h]
   mov     [ebp-5Ch], eax
   lea     eax, [ebp-48h]
   mov     [esp+8], eax
   mov     eax, [ebp-5Ch]
   mov     [esp+4], eax
   lea     eax, [ebp-88h]
   mov     [esp], eax
   call    VectorSubtract63262
   lea     eax, [ebp-48h]
   mov     [esp], eax
   call    VectorLengthSquared
   mov     eax, [ebp+8]
   mov     edx, [ebp+8]
   fld     dword [eax+8Ch]
   fmul    dword [edx+8Ch]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80558C6
   jmp     loc_80558E0
loc_80558C6:
   mov     eax, [ebp+10h]
   mov     byte [eax+23h], 1
   mov     eax, [ebp+10h]
   mov     byte [eax+22h], 1
   mov     edx, [ebp+10h]
   mov     eax, 0
   mov     [edx], eax
   jmp     locret_80558EA
loc_80558E0:
   lea     eax, [ebp-0Ch]
   inc     dword [eax]
   jmp     loc_8055727
locret_80558EA:
   leave   
   retn 

section .rdata
flt_813B834     dd 0.5