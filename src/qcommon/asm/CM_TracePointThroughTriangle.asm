global CM_TracePointThroughTriangle


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


I_fmax123:
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
   call    sub_80574AC
   leave   
   retn  


sub_80574AC:
   push    ebp
   mov     ebp, esp
   sub     esp, 4
   fld     dword [ebp+8]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_80574C2
   jmp     loc_80574CA
loc_80574C2:
   fld     dword [ebp+0Ch]
   fstp    dword [ebp-4]
   jmp     loc_80574D0
loc_80574CA:
   fld     dword [ebp+10h]
   fstp    dword [ebp-4]
loc_80574D0:
   fld     dword [ebp-4]
   leave   
   retn  


CM_TracePointThroughTriangle:
   push    ebp
   mov     ebp, esp
   sub     esp, 48h
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   add     eax, 0Ch
   mov     [esp], eax
   call    DotProduct123
   mov     eax, [ebp+0Ch]
   fsub    dword [eax+0Ch]
   fstp    dword [ebp-10h]
   fld     dword [ebp-10h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     locret_805628A
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    DotProduct123
   mov     eax, [ebp+0Ch]
   fsub    dword [eax+0Ch]
   fstp    dword [ebp-0Ch]
   fld     dword [ebp-0Ch]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   jnb     locret_805628A
   fld     dword [ebp-0Ch]
   fld     dword [flt_813B83C]
   fsubp   st1, st0
   fld     dword [ebp-0Ch]
   fsub    dword [ebp-10h]
   fdivp   st1, st0
   fstp    dword [ebp-14h]
   mov     eax, 0
   mov     [esp+4], eax
   mov     eax, [ebp-14h]
   mov     [esp], eax
   call    I_fmax123
   fstp    dword [ebp-14h]
   mov     eax, [ebp+10h]
   fld     dword [ebp-14h]
   fld     dword [eax]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     locret_805628A
   fld     dword [ebp-0Ch]
   fsub    dword [ebp-10h]
   fld     dword [ebp-0Ch]
   fdivrp  st1, st0
   fstp    dword [ebp-18h]
   lea     eax, [ebp-28h]
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
   add     eax, 10h
   mov     [esp+4], eax
   lea     eax, [ebp-28h]
   mov     [esp], eax
   call    DotProduct123
   mov     eax, [ebp+0Ch]
   fsub    dword [eax+1Ch]
   fstp    dword [ebp-2Ch]
   fld     dword [ebp-2Ch]
   fld     dword [flt_813B840]
   fucompp 
   fnstsw  ax
   sahf    
   ja      locret_805628A
   fld     dword [ebp-2Ch]
   fld     dword [flt_813B844]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      locret_805628A
   mov     eax, [ebp+0Ch]
   add     eax, 20h
   mov     [esp+4], eax
   lea     eax, [ebp-28h]
   mov     [esp], eax
   call    DotProduct123
   mov     eax, [ebp+0Ch]
   fsub    dword [eax+2Ch]
   fstp    dword [ebp-30h]
   fld     dword [ebp-30h]
   fld     dword [flt_813B840]
   fucompp 
   fnstsw  ax
   sahf    
   ja      locret_805628A
   fld     dword [ebp-2Ch]
   fadd    dword [ebp-30h]
   fld     dword [flt_813B844]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      locret_805628A
   mov     edx, [ebp+10h]
   mov     eax, [ebp-14h]
   mov     [edx], eax
   mov     eax, [ebp+10h]
   add     eax, 4
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    VectorCopy834
locret_805628A:
   leave   
   retn    

section .rdata
flt_813B83C     dd 0.125
flt_813B844     dd 1.001
flt_813B840     dd -0.001