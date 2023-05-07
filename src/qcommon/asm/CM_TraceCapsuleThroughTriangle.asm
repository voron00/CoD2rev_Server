extern cm
extern sqrt


global CM_TraceCapsuleThroughTriangle


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


Vec2Multiply271:
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


Vec3Normalize962:
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


VectorScale715:
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


CM_TraceCapsuleThroughTriangle:
   push    ebp
   mov     ebp, esp
   sub     esp, 0E8h
   lea     eax, [ebp-0B8h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   add     eax, 0Ch
   mov     [esp], eax
   call    VectorCopy834
   fld     dword [ebp-0B0h]
   fsub    dword [ebp+10h]
   fstp    dword [ebp-0B0h]
   mov     eax, [ebp+8]
   fld     dword [eax+8Ch]
   fld     dword [flt_813B848]
   faddp   st1, st0
   fstp    dword [ebp-14h]
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   lea     eax, [ebp-0B8h]
   mov     [esp], eax
   call    DotProduct123
   mov     eax, [ebp+0Ch]
   fsub    dword [eax+0Ch]
   fstp    dword [ebp-1Ch]
   fld     dword [ebp-1Ch]
   fld     dword [ebp-14h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     locret_8056E40
   lea     eax, [ebp-0A8h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    VectorCopy834
   fld     dword [ebp-0A0h]
   fsub    dword [ebp+10h]
   fstp    dword [ebp-0A0h]
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   lea     eax, [ebp-0A8h]
   mov     [esp], eax
   call    DotProduct123
   mov     eax, [ebp+0Ch]
   fsub    dword [eax+0Ch]
   fstp    dword [ebp-18h]
   fld     dword [ebp-18h]
   fsub    dword [ebp-1Ch]
   fstp    dword [ebp-0CCh]
   fld     dword [ebp-0CCh]
   fld     dword [flt_813B84C]
   fucompp 
   fnstsw  ax
   sahf    
   jnb     locret_8056E40
   mov     eax, [ebp-14h]
   xor     eax, 80000000h
   mov     [ebp-10h], eax
   fld     dword [ebp-18h]
   fld     dword [ebp-10h]
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8056383
   jmp     loc_8056548
loc_8056383:
   fld     dword [ebp+10h]
   fadd    st0, st0
   fstp    dword [ebp-0C0h]
   mov     eax, [ebp+0Ch]
   fld     dword [ebp-0C0h]
   fmul    dword [eax+8]
   fld     dword [ebp-18h]
   faddp   st1, st0
   fstp    dword [ebp-0BCh]
   fld     dword [ebp-0BCh]
   fld     dword [ebp-10h]
   fucompp 
   fnstsw  ax
   sahf    
   jnb     locret_8056E40
   fld     dword [ebp-10h]
   fsub    dword [ebp-18h]
   mov     eax, [ebp+0Ch]
   fdiv    dword [eax+8]
   fstp    dword [ebp-20h]
   mov     eax, [ebp+0Ch]
   add     eax, 10h
   mov     [esp+4], eax
   lea     eax, [ebp-0A8h]
   mov     [esp], eax
   call    DotProduct123
   mov     eax, [ebp+0Ch]
   fsub    dword [eax+1Ch]
   fstp    dword [ebp-0C4h]
   mov     eax, [ebp+0Ch]
   add     eax, 20h
   mov     [esp+4], eax
   lea     eax, [ebp-0A8h]
   mov     [esp], eax
   call    DotProduct123
   mov     eax, [ebp+0Ch]
   fsub    dword [eax+2Ch]
   fstp    dword [ebp-0C8h]
   mov     eax, [ebp+0Ch]
   fld     dword [ebp-20h]
   fmul    dword [eax+18h]
   fld     dword [ebp-0C4h]
   faddp   st1, st0
   fstp    dword [ebp-3Ch]
   fld     dword [ebp-3Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8056434
   jmp     loc_8056494
loc_8056434:
   mov     eax, [ebp+0Ch]
   fld     dword [ebp-20h]
   fmul    dword [eax+28h]
   fld     dword [ebp-0C8h]
   faddp   st1, st0
   fstp    dword [ebp-40h]
   fld     dword [ebp-40h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8056458
   jmp     loc_8056494
loc_8056458:
   fld     dword [ebp-3Ch]
   fadd    dword [ebp-40h]
   fld1    
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8056469
   jmp     loc_8056494
loc_8056469:
   mov     eax, [ebp+14h]
   add     eax, 4
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    VectorCopy834
   mov     edx, [ebp+14h]
   mov     eax, 0
   mov     [edx], eax
   mov     eax, [ebp+14h]
   mov     byte [eax+23h], 1
   jmp     locret_8056E40
loc_8056494:
   fld     dword [ebp-0BCh]
   fld     dword [ebp-14h]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80564A6
   jmp     loc_80564B1
loc_80564A6:
   mov     eax, [ebp-0C0h]
   mov     [ebp-20h], eax
   jmp     loc_80564C0
loc_80564B1:
   fld     dword [ebp-14h]
   fsub    dword [ebp-18h]
   mov     eax, [ebp+0Ch]
   fdiv    dword [eax+8]
   fstp    dword [ebp-20h]
loc_80564C0:
   mov     eax, [ebp+0Ch]
   fld     dword [ebp-20h]
   fmul    dword [eax+18h]
   fld     dword [ebp-0C4h]
   faddp   st1, st0
   fstp    dword [ebp-3Ch]
   fld     dword [ebp-3Ch]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      locret_8056E40
   mov     eax, [ebp+0Ch]
   fld     dword [ebp-20h]
   fmul    dword [eax+28h]
   fld     dword [ebp-0C8h]
   faddp   st1, st0
   fstp    dword [ebp-40h]
   fld     dword [ebp-40h]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      locret_8056E40
   fld     dword [ebp-3Ch]
   fadd    dword [ebp-40h]
   fld1    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      locret_8056E40
   mov     eax, [ebp+14h]
   add     eax, 4
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    VectorCopy834
   mov     edx, [ebp+14h]
   mov     eax, 0
   mov     [edx], eax
   mov     eax, [ebp+14h]
   mov     byte [eax+23h], 1
   jmp     locret_8056E40
loc_8056548:
   fld     dword [ebp-18h]
   fsub    dword [ebp-14h]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8056559
   jmp     loc_8056578
loc_8056559:
   mov     eax, 0
   mov     [ebp-20h], eax
   lea     eax, [ebp-38h]
   mov     [esp+4], eax
   lea     eax, [ebp-0A8h]
   mov     [esp], eax
   call    VectorCopy834
   jmp     loc_80565C2
loc_8056578:
   fld     dword [ebp-18h]
   fsub    dword [ebp-14h]
   fdiv    dword [ebp-0CCh]
   fstp    dword [ebp-20h]
   mov     eax, [ebp+14h]
   fld     dword [ebp-20h]
   fld     dword [eax]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      locret_8056E40
   lea     eax, [ebp-38h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+8]
   add     eax, 24h
   mov     [esp+8], eax
   mov     eax, [ebp-20h]
   mov     [esp+4], eax
   lea     eax, [ebp-0A8h]
   mov     [esp], eax
   call    VectorMA712
loc_80565C2:
   mov     eax, [ebp+0Ch]
   add     eax, 10h
   mov     [esp+4], eax
   lea     eax, [ebp-38h]
   mov     [esp], eax
   call    DotProduct123
   mov     eax, [ebp+0Ch]
   fsub    dword [eax+1Ch]
   fstp    dword [ebp-3Ch]
   mov     eax, [ebp+0Ch]
   add     eax, 20h
   mov     [esp+4], eax
   lea     eax, [ebp-38h]
   mov     [esp], eax
   call    DotProduct123
   mov     eax, [ebp+0Ch]
   fsub    dword [eax+2Ch]
   fstp    dword [ebp-40h]
   fld     dword [ebp-3Ch]
   fadd    dword [ebp-40h]
   fld1    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   setnbe  al
   movzx   eax, al
   mov     [ebp-44h], eax
   mov     eax, [ebp-44h]
   mov     [ebp-0D0h], eax
   fld     dword [ebp-3Ch]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_805662D
   jmp     loc_8056634
loc_805662D:
   or      dword [ebp-0D0h], 2
loc_8056634:
   mov     eax, [ebp-0D0h]
   mov     [ebp-44h], eax
   mov     eax, [ebp-44h]
   mov     [ebp-0D4h], eax
   fld     dword [ebp-40h]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8056654
   jmp     loc_805665B
loc_8056654:
   or      dword [ebp-0D4h], 4
loc_805665B:
   mov     eax, [ebp-0D4h]
   mov     [ebp-44h], eax
   cmp     dword [ebp-44h], 0
   jnz     loc_80566AB
   mov     eax, [ebp+14h]
   add     eax, 4
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    VectorCopy834
   mov     edx, [ebp+14h]
   mov     eax, [ebp-20h]
   mov     [edx], eax
   mov     eax, [ebp+8]
   fld     dword [ebp-18h]
   fld     dword [eax+8Ch]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_805669F
   jmp     locret_8056E40
loc_805669F:
   mov     eax, [ebp+14h]
   mov     byte [eax+23h], 1
   jmp     locret_8056E40
loc_80566AB:
   mov     dword [ebp-0Ch], 0
loc_80566B2:
   cmp     dword [ebp-0Ch], 2
   jle     loc_80566BD
   jmp     locret_8056E40
loc_80566BD:
   movzx   ecx, byte [ebp-0Ch]
   mov     eax, [ebp-44h]
   sar     eax, cl
   and     eax, 1
   test    al, al
   jz      loc_8056B3D
   mov     eax, [ebp+0Ch]
   mov     edx, [ebp-0Ch]
   mov     eax, [eax+edx*4+3Ch]
   mov     [ebp-8Ch], eax
   cmp     dword [ebp-8Ch], 0
   jns     loc_80566EF
   jmp     loc_8056E36
loc_80566EF:
   mov     edx, [ebp+8]
   mov     eax, [ebp-8Ch]
   lea     ecx, 0[eax*4]
   mov     eax, [edx+0A4h]
   mov     edx, [ebp+8]
   mov     eax, [eax+ecx]
   cmp     eax, [edx+0A0h]
   jnz     loc_8056718
   jmp     loc_8056E36
loc_8056718:
   mov     edx, [ebp+8]
   mov     eax, [ebp-8Ch]
   lea     ecx, 0[eax*4]
   mov     edx, [edx+0A4h]
   mov     eax, [ebp+8]
   mov     eax, [eax+0A0h]
   mov     [edx+ecx], eax
   mov     edx, [ebp-8Ch]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 4
   add     eax, [cm+50h]
   mov     [ebp-90h], eax
   lea     eax, [ebp-58h]
   mov     [esp+8], eax
   mov     eax, [ebp-90h]
   mov     [esp+4], eax
   lea     eax, [ebp-0A8h]
   mov     [esp], eax
   call    VectorSubtract63262
   mov     eax, [ebp-90h]
   add     eax, 0Ch
   mov     [esp+4], eax
   lea     eax, [ebp-58h]
   mov     [esp], eax
   call    DotProduct123
   fstp    dword [ebp-78h]
   mov     eax, [ebp-90h]
   add     eax, 18h
   mov     [esp+4], eax
   lea     eax, [ebp-58h]
   mov     [esp], eax
   call    DotProduct123
   fstp    dword [ebp-74h]
   mov     eax, [ebp-90h]
   add     eax, 0Ch
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   add     eax, 24h
   mov     [esp], eax
   call    DotProduct123
   fstp    dword [ebp-68h]
   mov     eax, [ebp-90h]
   add     eax, 18h
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   add     eax, 24h
   mov     [esp], eax
   call    DotProduct123
   fstp    dword [ebp-64h]
   lea     eax, [ebp-78h]
   mov     [esp+4], eax
   lea     eax, [ebp-68h]
   mov     [esp], eax
   call    Dot2Product824
   fstp    dword [ebp-84h]
   fld     dword [ebp-84h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8056E36
   lea     eax, [ebp-78h]
   mov     [esp+4], eax
   lea     eax, [ebp-78h]
   mov     [esp], eax
   call    Dot2Product824
   fstp    dword [ebp-48h]
   fld     dword [ebp-14h]
   fmul    dword [ebp-14h]
   fld     dword [ebp-48h]
   fsubrp  st1, st0
   fstp    dword [ebp-88h]
   fld     dword [ebp-88h]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_805684D
   jmp     loc_8056990
loc_805684D:
   mov     eax, [ebp-90h]
   add     eax, 24h
   mov     [esp+4], eax
   lea     eax, [ebp-58h]
   mov     [esp], eax
   call    DotProduct123
   fstp    dword [ebp-70h]
   fld     dword [ebp-70h]
   fld     dword [flt_813B850]
   fsubp   st1, st0
   fstp    dword [esp]
   call    CM_Fabs3
   fld     dword [flt_813B850]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8056E36
   mov     eax, [ebp+14h]
   add     eax, 4
   mov     [esp+8], eax
   mov     eax, [ebp-78h]
   mov     [esp+4], eax
   mov     eax, [ebp-90h]
   add     eax, 0Ch
   mov     [esp], eax
   call    VectorScale715
   mov     eax, [ebp+14h]
   add     eax, 4
   mov     [esp+0Ch], eax
   mov     eax, [ebp-90h]
   add     eax, 18h
   mov     [esp+8], eax
   mov     eax, [ebp-74h]
   mov     [esp+4], eax
   mov     eax, [ebp+14h]
   add     eax, 4
   mov     [esp], eax
   call    VectorMA712
   mov     eax, [ebp+14h]
   add     eax, 4
   mov     [esp], eax
   call    Vec3Normalize962
   fstp    st0
   mov     eax, [ebp+0Ch]
   fld     dword [eax+8]
   fld     dword [flt_813B854]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8056903
   jmp     loc_8056957
loc_8056903:
   mov     eax, [ebp+14h]
   fld     dword [eax+0Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8056916
   jmp     loc_8056957
loc_8056916:
   mov     eax, [ebp+14h]
   fld     dword [eax+0Ch]
   fld     dword [flt_813B854]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_805692B
   jmp     loc_8056957
loc_805692B:
   fld     dword [ebp-0A0h]
   fld     dword [ebp-0B0h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8056942
   jmp     loc_8056957
loc_8056942:
   mov     eax, [ebp+14h]
   add     eax, 4
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    VectorCopy834
loc_8056957:
   mov     edx, [ebp+14h]
   mov     eax, 0
   mov     [edx], eax
   mov     eax, [ebp+8]
   mov     edx, [ebp+8]
   fld     dword [eax+8Ch]
   fmul    dword [edx+8Ch]
   fld     dword [ebp-48h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8056984
   jmp     locret_8056E40
loc_8056984:
   mov     eax, [ebp+14h]
   mov     byte [eax+23h], 1
   jmp     locret_8056E40
loc_8056990:
   lea     eax, [ebp-68h]
   mov     [esp], eax
   call    Vec2Multiply271
   fstp    dword [ebp-80h]
   fld     dword [ebp-84h]
   fmul    dword [ebp-84h]
   fld     dword [ebp-80h]
   fmul    dword [ebp-88h]
   fsubp   st1, st0
   fstp    dword [ebp-7Ch]
   fld     dword [ebp-7Ch]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8056E36
   mov     eax, [ebp-7Ch]
   mov     [esp], eax
   call    CM_Sqrt923
   fchs    
   fsub    dword [ebp-84h]
   fdiv    dword [ebp-80h]
   fstp    dword [ebp-20h]
   mov     eax, [ebp+14h]
   fld     dword [ebp-20h]
   fld     dword [eax]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80569F5
   jmp     loc_8056E36
loc_80569F5:
   lea     eax, [ebp-58h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+8]
   add     eax, 24h
   mov     [esp+8], eax
   mov     eax, [ebp-20h]
   mov     [esp+4], eax
   lea     eax, [ebp-58h]
   mov     [esp], eax
   call    VectorMA712
   mov     eax, [ebp-90h]
   add     eax, 24h
   mov     [esp+4], eax
   lea     eax, [ebp-58h]
   mov     [esp], eax
   call    DotProduct123
   fstp    dword [ebp-30h]
   fld     dword [ebp-30h]
   fld     dword [flt_813B850]
   fsubp   st1, st0
   fstp    dword [esp]
   call    CM_Fabs3
   fld     dword [flt_813B850]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8056E36
   fld     dword [ebp-20h]
   fmul    dword [ebp-68h]
   fadd    dword [ebp-78h]
   fdiv    dword [ebp-14h]
   fstp    dword [ebp-38h]
   fld     dword [ebp-20h]
   fmul    dword [ebp-64h]
   fadd    dword [ebp-74h]
   fdiv    dword [ebp-14h]
   fstp    dword [ebp-34h]
   mov     eax, [ebp+14h]
   add     eax, 4
   mov     [esp+8], eax
   mov     eax, [ebp-38h]
   mov     [esp+4], eax
   mov     eax, [ebp-90h]
   add     eax, 0Ch
   mov     [esp], eax
   call    VectorScale715
   mov     eax, [ebp+14h]
   add     eax, 4
   mov     [esp+0Ch], eax
   mov     eax, [ebp-90h]
   add     eax, 18h
   mov     [esp+8], eax
   mov     eax, [ebp-34h]
   mov     [esp+4], eax
   mov     eax, [ebp+14h]
   add     eax, 4
   mov     [esp], eax
   call    VectorMA712
   mov     eax, [ebp+0Ch]
   fld     dword [eax+8]
   fld     dword [flt_813B854]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8056ADC
   jmp     loc_8056B30
loc_8056ADC:
   mov     eax, [ebp+14h]
   fld     dword [eax+0Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8056AEF
   jmp     loc_8056B30
loc_8056AEF:
   mov     eax, [ebp+14h]
   fld     dword [eax+0Ch]
   fld     dword [flt_813B854]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8056B04
   jmp     loc_8056B30
loc_8056B04:
   fld     dword [ebp-0A0h]
   fld     dword [ebp-0B0h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8056B1B
   jmp     loc_8056B30
loc_8056B1B:
   mov     eax, [ebp+14h]
   add     eax, 4
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    VectorCopy834
loc_8056B30:
   mov     edx, [ebp+14h]
   mov     eax, [ebp-20h]
   mov     [edx], eax
   jmp     loc_8056E36
loc_8056B3D:
   mov     eax, [ebp+0Ch]
   mov     edx, [ebp-0Ch]
   mov     eax, [eax+edx*4+30h]
   mov     [ebp-94h], eax
   cmp     dword [ebp-94h], 0
   jns     loc_8056B5B
   jmp     loc_8056E36
loc_8056B5B:
   mov     edx, [ebp+8]
   mov     eax, [ebp-94h]
   lea     ecx, 0[eax*4]
   mov     eax, [edx+0A8h]
   mov     edx, [ebp+8]
   mov     eax, [eax+ecx]
   cmp     eax, [edx+0A0h]
   jnz     loc_8056B84
   jmp     loc_8056E36
loc_8056B84:
   mov     edx, [ebp+8]
   mov     eax, [ebp-94h]
   lea     ecx, 0[eax*4]
   mov     edx, [edx+0A8h]
   mov     eax, [ebp+8]
   mov     eax, [eax+0A0h]
   mov     [edx+ecx], eax
   mov     edx, [ebp-94h]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   add     eax, [cm+48h]
   mov     [ebp-98h], eax
   lea     eax, [ebp-58h]
   mov     [esp+8], eax
   mov     eax, [ebp-98h]
   mov     [esp+4], eax
   lea     eax, [ebp-0A8h]
   mov     [esp], eax
   call    VectorSubtract63262
   lea     eax, [ebp-58h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   add     eax, 24h
   mov     [esp], eax
   call    DotProduct123
   fstp    dword [ebp-84h]
   fld     dword [ebp-84h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8056E36
   lea     eax, [ebp-58h]
   mov     [esp+4], eax
   lea     eax, [ebp-58h]
   mov     [esp], eax
   call    DotProduct123
   fstp    dword [ebp-48h]
   fld     dword [ebp-14h]
   fmul    dword [ebp-14h]
   fld     dword [ebp-48h]
   fsubrp  st1, st0
   fstp    dword [ebp-88h]
   fld     dword [ebp-88h]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8056C4A
   jmp     loc_8056D1C
loc_8056C4A:
   mov     eax, [ebp-48h]
   mov     [esp], eax
   call    CM_Sqrt923
   fld1    
   fdivrp  st1, st0
   fstp    dword [ebp-20h]
   mov     eax, [ebp+14h]
   add     eax, 4
   mov     [esp+8], eax
   mov     eax, [ebp-20h]
   mov     [esp+4], eax
   lea     eax, [ebp-58h]
   mov     [esp], eax
   call    VectorScale715
   mov     eax, [ebp+0Ch]
   fld     dword [eax+8]
   fld     dword [flt_813B854]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8056C8F
   jmp     loc_8056CE3
loc_8056C8F:
   mov     eax, [ebp+14h]
   fld     dword [eax+0Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8056CA2
   jmp     loc_8056CE3
loc_8056CA2:
   mov     eax, [ebp+14h]
   fld     dword [eax+0Ch]
   fld     dword [flt_813B854]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8056CB7
   jmp     loc_8056CE3
loc_8056CB7:
   fld     dword [ebp-0A0h]
   fld     dword [ebp-0B0h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8056CCE
   jmp     loc_8056CE3
loc_8056CCE:
   mov     eax, [ebp+14h]
   add     eax, 4
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    VectorCopy834
loc_8056CE3:
   mov     edx, [ebp+14h]
   mov     eax, 0
   mov     [edx], eax
   mov     eax, [ebp+8]
   mov     edx, [ebp+8]
   fld     dword [eax+8Ch]
   fmul    dword [edx+8Ch]
   fld     dword [ebp-48h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8056D10
   jmp     locret_8056E40
loc_8056D10:
   mov     eax, [ebp+14h]
   mov     byte [eax+23h], 1
   jmp     locret_8056E40
loc_8056D1C:
   mov     eax, [ebp+8]
   mov     eax, [eax+34h]
   mov     [ebp-80h], eax
   fld     dword [ebp-84h]
   fmul    dword [ebp-84h]
   fld     dword [ebp-80h]
   fmul    dword [ebp-88h]
   fsubp   st1, st0
   fstp    dword [ebp-7Ch]
   fld     dword [ebp-7Ch]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8056E36
   mov     eax, [ebp-7Ch]
   mov     [esp], eax
   call    CM_Sqrt923
   fchs    
   fsub    dword [ebp-84h]
   fdiv    dword [ebp-80h]
   fstp    dword [ebp-20h]
   mov     eax, [ebp+14h]
   fld     dword [ebp-20h]
   fld     dword [eax]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8056D7C
   jmp     loc_8056E36
loc_8056D7C:
   mov     eax, [ebp+14h]
   add     eax, 4
   mov     [esp+0Ch], eax
   mov     eax, [ebp+8]
   add     eax, 24h
   mov     [esp+8], eax
   mov     eax, [ebp-20h]
   mov     [esp+4], eax
   lea     eax, [ebp-58h]
   mov     [esp], eax
   call    VectorMA712
   mov     eax, [ebp+14h]
   add     eax, 4
   mov     [esp+8], eax
   fld1    
   fdiv    dword [ebp-14h]
   fstp    dword [esp+4]
   mov     eax, [ebp+14h]
   add     eax, 4
   mov     [esp], eax
   call    VectorScale715
   mov     eax, [ebp+0Ch]
   fld     dword [eax+8]
   fld     dword [flt_813B854]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8056DDA
   jmp     loc_8056E2E
loc_8056DDA:
   mov     eax, [ebp+14h]
   fld     dword [eax+0Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8056DED
   jmp     loc_8056E2E
loc_8056DED:
   mov     eax, [ebp+14h]
   fld     dword [eax+0Ch]
   fld     dword [flt_813B854]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8056E02
   jmp     loc_8056E2E
loc_8056E02:
   fld     dword [ebp-0A0h]
   fld     dword [ebp-0B0h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8056E19
   jmp     loc_8056E2E
loc_8056E19:
   mov     eax, [ebp+14h]
   add     eax, 4
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    VectorCopy834
loc_8056E2E:
   mov     edx, [ebp+14h]
   mov     eax, [ebp-20h]
   mov     [edx], eax
loc_8056E36:
   lea     eax, [ebp-0Ch]
   inc     dword [eax]
   jmp     loc_80566B2
locret_8056E40:
   leave   
   retn    


section .rdata
flt_813B850     dd 0.5
flt_813B848     dd 0.125
flt_813B854     dd 0.69999999
flt_813B84C     dd 0.000099999997