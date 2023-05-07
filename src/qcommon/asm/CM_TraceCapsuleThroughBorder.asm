extern sqrt


global CM_TraceCapsuleThroughBorder


section .text


VectorSet821:
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


Vector2Subtract261:
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


Dot2Product824:
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


CM_Sqrt923:
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


CM_TraceCapsuleThroughBorder:
   push    ebp
   mov     ebp, esp
   sub     esp, 98h
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   add     eax, 24h
   mov     [esp], eax
   call    Dot2Product824
   fstp    dword [ebp-14h]
   fld     dword [ebp-14h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     locret_8057399
   mov     eax, [ebp+8]
   fld     dword [eax+8Ch]
   fld     dword [flt_813B858]
   faddp   st1, st0
   fstp    dword [ebp-0Ch]
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    Dot2Product824
   mov     eax, [ebp+0Ch]
   fsub    dword [eax+8]
   fstp    dword [ebp-10h]
   fld     dword [ebp-0Ch]
   fsub    dword [ebp-10h]
   fdiv    dword [ebp-14h]
   fstp    dword [ebp-18h]
   mov     eax, [ebp+10h]
   fld     dword [ebp-18h]
   fld     dword [eax]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     locret_8057399
   mov     eax, [ebp+8]
   fld     dword [ebp-18h]
   fmul    dword [eax+30h]
   mov     eax, [ebp-0Ch]
   mov     [ebp-7Ch], eax
   fld     dword [ebp-7Ch]
   fchs    
   fucompp 
   fnstsw  ax
   sahf    
   ja      locret_8057399
   lea     eax, [ebp-38h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+8]
   add     eax, 24h
   mov     [esp+8], eax
   mov     eax, [ebp-18h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    VectorMA712
   mov     eax, [ebp+0Ch]
   fld     dword [eax+4]
   fmul    dword [ebp-38h]
   mov     eax, [ebp+0Ch]
   fld     dword [eax]
   fmul    dword [ebp-34h]
   fsubp   st1, st0
   mov     eax, [ebp+0Ch]
   fsub    dword [eax+14h]
   fstp    dword [ebp-1Ch]
   fld     dword [ebp-1Ch]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8056F34
   jmp     loc_8057111
loc_8056F34:
   mov     eax, [ebp+0Ch]
   mov     edx, [ebp+0Ch]
   fld     dword [eax+4]
   fmul    dword [edx+14h]
   mov     eax, [ebp+0Ch]
   mov     edx, [ebp+0Ch]
   fld     dword [eax]
   fmul    dword [edx+8]
   faddp   st1, st0
   fstp    dword [ebp-68h]
   mov     eax, [ebp+0Ch]
   mov     edx, [ebp+0Ch]
   fld     dword [eax+4]
   fmul    dword [edx+8]
   mov     eax, [ebp+0Ch]
   mov     edx, [ebp+0Ch]
   fld     dword [eax]
   fmul    dword [edx+14h]
   fsubp   st1, st0
   fstp    dword [ebp-64h]
   lea     eax, [ebp-48h]
   mov     [esp+8], eax
   lea     eax, [ebp-68h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    Vector2Subtract261
   lea     eax, [ebp-48h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   add     eax, 24h
   mov     [esp], eax
   call    Dot2Product824
   fstp    dword [ebp-4Ch]
   fld     dword [ebp-4Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     locret_8057399
   lea     eax, [ebp-48h]
   mov     [esp+4], eax
   lea     eax, [ebp-48h]
   mov     [esp], eax
   call    Dot2Product824
   fstp    dword [ebp-50h]
   fld     dword [ebp-0Ch]
   fmul    dword [ebp-0Ch]
   fld     dword [ebp-50h]
   fsubrp  st1, st0
   fstp    dword [ebp-58h]
   fld     dword [ebp-58h]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8056FE3
   jmp     loc_8057076
loc_8056FE3:
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+0Ch]
   mov     [ebp-60h], eax
   mov     eax, [ebp+8]
   fld     dword [ebp-60h]
   fsub    dword [eax+8]
   fstp    dword [esp]
   call    CM_Fabs3
   mov     eax, [ebp+8]
   fld     dword [eax+90h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      locret_8057399
   mov     eax, 0
   mov     [esp+0Ch], eax
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+4]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     eax, [eax]
   mov     [esp+4], eax
   mov     eax, [ebp+10h]
   add     eax, 4
   mov     [esp], eax
   call    VectorSet821
   mov     edx, [ebp+10h]
   mov     eax, 0
   mov     [edx], eax
   mov     eax, [ebp+8]
   mov     edx, [ebp+8]
   fld     dword [eax+8Ch]
   fmul    dword [edx+8Ch]
   fld     dword [ebp-50h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_805706A
   jmp     locret_8057399
loc_805706A:
   mov     eax, [ebp+10h]
   mov     byte [eax+23h], 1
   jmp     locret_8057399
loc_8057076:
   fld     dword [ebp-4Ch]
   fmul    dword [ebp-4Ch]
   mov     eax, [ebp+8]
   fld     dword [eax+34h]
   fmul    dword [ebp-58h]
   fsubp   st1, st0
   fstp    dword [ebp-54h]
   fld     dword [ebp-54h]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      locret_8057399
   mov     eax, [ebp-4Ch]
   xor     eax, 80000000h
   mov     [ebp-6Ch], eax
   mov     eax, [ebp-54h]
   mov     [esp], eax
   call    CM_Sqrt923
   fsubr   dword [ebp-6Ch]
   mov     eax, [ebp+8]
   fdiv    dword [eax+34h]
   fstp    dword [ebp-18h]
   mov     eax, [ebp+10h]
   fld     dword [ebp-18h]
   fld     dword [eax]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     locret_8057399
   fld     dword [ebp-18h]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   jnb     locret_8057399
   lea     eax, [ebp-38h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+8]
   add     eax, 24h
   mov     [esp+8], eax
   mov     eax, [ebp-18h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    VectorMA712
   mov     eax, 0
   mov     [ebp-1Ch], eax
   jmp     loc_8057333
loc_8057111:
   mov     eax, [ebp+0Ch]
   fld     dword [ebp-1Ch]
   fld     dword [eax+18h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8057128
   jmp     loc_805731D
loc_8057128:
   mov     ecx, [ebp+0Ch]
   mov     eax, [ebp+0Ch]
   mov     edx, [ebp+0Ch]
   fld     dword [eax+14h]
   fadd    dword [edx+18h]
   fmul    dword [ecx+4]
   mov     eax, [ebp+0Ch]
   mov     edx, [ebp+0Ch]
   fld     dword [eax]
   fmul    dword [edx+8]
   faddp   st1, st0
   fstp    dword [ebp-68h]
   mov     eax, [ebp+0Ch]
   mov     edx, [ebp+0Ch]
   fld     dword [eax+4]
   fmul    dword [edx+8]
   mov     ecx, [ebp+0Ch]
   mov     eax, [ebp+0Ch]
   mov     edx, [ebp+0Ch]
   fld     dword [eax+14h]
   fadd    dword [edx+18h]
   fmul    dword [ecx]
   fsubp   st1, st0
   fstp    dword [ebp-64h]
   lea     eax, [ebp-48h]
   mov     [esp+8], eax
   lea     eax, [ebp-68h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    Vector2Subtract261
   lea     eax, [ebp-48h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   add     eax, 24h
   mov     [esp], eax
   call    Dot2Product824
   fstp    dword [ebp-4Ch]
   fld     dword [ebp-4Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     locret_8057399
   lea     eax, [ebp-48h]
   mov     [esp+4], eax
   lea     eax, [ebp-48h]
   mov     [esp], eax
   call    Dot2Product824
   fstp    dword [ebp-50h]
   fld     dword [ebp-0Ch]
   fmul    dword [ebp-0Ch]
   fld     dword [ebp-50h]
   fsubrp  st1, st0
   fstp    dword [ebp-58h]
   fld     dword [ebp-58h]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80571E3
   jmp     loc_8057284
loc_80571E3:
   mov     ecx, [ebp+0Ch]
   mov     eax, [ebp+0Ch]
   mov     edx, [ebp+0Ch]
   fld     dword [eax+10h]
   fmul    dword [edx+18h]
   fld     dword [ecx+0Ch]
   faddp   st1, st0
   fstp    dword [ebp-60h]
   mov     eax, [ebp+8]
   fld     dword [eax+8]
   fsub    dword [ebp-60h]
   fstp    dword [esp]
   call    CM_Fabs3
   mov     eax, [ebp+8]
   fld     dword [eax+90h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      locret_8057399
   mov     eax, 0
   mov     [esp+0Ch], eax
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+4]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     eax, [eax]
   mov     [esp+4], eax
   mov     eax, [ebp+10h]
   add     eax, 4
   mov     [esp], eax
   call    VectorSet821
   mov     edx, [ebp+10h]
   mov     eax, 0
   mov     [edx], eax
   mov     eax, [ebp+8]
   mov     edx, [ebp+8]
   fld     dword [eax+8Ch]
   fmul    dword [edx+8Ch]
   fld     dword [ebp-50h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8057278
   jmp     locret_8057399
loc_8057278:
   mov     eax, [ebp+10h]
   mov     byte [eax+23h], 1
   jmp     locret_8057399
loc_8057284:
   fld     dword [ebp-4Ch]
   fmul    dword [ebp-4Ch]
   mov     eax, [ebp+8]
   fld     dword [eax+34h]
   fmul    dword [ebp-58h]
   fsubp   st1, st0
   fstp    dword [ebp-54h]
   fld     dword [ebp-54h]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      locret_8057399
   mov     eax, [ebp-4Ch]
   xor     eax, 80000000h
   mov     [ebp-70h], eax
   mov     eax, [ebp-54h]
   mov     [esp], eax
   call    CM_Sqrt923
   fsubr   dword [ebp-70h]
   mov     eax, [ebp+8]
   fdiv    dword [eax+34h]
   fstp    dword [ebp-18h]
   mov     eax, [ebp+10h]
   fld     dword [ebp-18h]
   fld     dword [eax]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     locret_8057399
   fld     dword [ebp-18h]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   jnb     locret_8057399
   lea     eax, [ebp-38h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+8]
   add     eax, 24h
   mov     [esp+8], eax
   mov     eax, [ebp-18h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    VectorMA712
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+18h]
   mov     [ebp-1Ch], eax
   jmp     loc_8057333
loc_805731D:
   fld     dword [ebp-18h]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_805732B
   jmp     loc_8057333
loc_805732B:
   mov     eax, 0
   mov     [ebp-18h], eax
loc_8057333:
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp+0Ch]
   fld     dword [ebp-1Ch]
   fmul    dword [eax+10h]
   fld     dword [edx+0Ch]
   faddp   st1, st0
   fstp    dword [ebp-3Ch]
   fld     dword [ebp-30h]
   fsub    dword [ebp-3Ch]
   fstp    dword [esp]
   call    CM_Fabs3
   mov     eax, [ebp+8]
   fld     dword [eax+90h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      locret_8057399
   mov     edx, [ebp+10h]
   mov     eax, [ebp-18h]
   mov     [edx], eax
   mov     eax, 0
   mov     [esp+0Ch], eax
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+4]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     eax, [eax]
   mov     [esp+4], eax
   mov     eax, [ebp+10h]
   add     eax, 4
   mov     [esp], eax
   call    VectorSet821
locret_8057399:
   leave   
   retn 


section .rdata
flt_813B858     dd 0.125