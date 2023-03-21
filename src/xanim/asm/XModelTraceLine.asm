extern XModelNumBones
extern CM_CalcTraceExtents
extern CM_TraceBox


global XModelTraceLine
global XModelSetTransArray


section .text


sub_80C3A0A:
   push    ebp
   mov     ebp, esp
   mov     ecx, [ebp+10h]
   mov     eax, [ebp+8]
   mov     edx, [ebp+0Ch]
   fld     dword [eax]
   fmul    dword [edx]
   mov     eax, [ebp+8]
   add     eax, 4
   mov     edx, [ebp+0Ch]
   add     edx, 0Ch
   fld     dword [eax]
   fmul    dword [edx]
   faddp   st1, st0
   mov     eax, [ebp+8]
   add     eax, 8
   mov     edx, [ebp+0Ch]
   add     edx, 18h
   fld     dword [eax]
   fmul    dword [edx]
   faddp   st1, st0
   fstp    dword [ecx]
   mov     ecx, [ebp+10h]
   add     ecx, 4
   mov     eax, [ebp+8]
   mov     edx, [ebp+0Ch]
   add     edx, 4
   fld     dword [eax]
   fmul    dword [edx]
   mov     eax, [ebp+8]
   add     eax, 4
   mov     edx, [ebp+0Ch]
   add     edx, 10h
   fld     dword [eax]
   fmul    dword [edx]
   faddp   st1, st0
   mov     eax, [ebp+8]
   add     eax, 8
   mov     edx, [ebp+0Ch]
   add     edx, 1Ch
   fld     dword [eax]
   fmul    dword [edx]
   faddp   st1, st0
   fstp    dword [ecx]
   mov     ecx, [ebp+10h]
   add     ecx, 8
   mov     eax, [ebp+8]
   mov     edx, [ebp+0Ch]
   add     edx, 8
   fld     dword [eax]
   fmul    dword [edx]
   mov     eax, [ebp+8]
   add     eax, 4
   mov     edx, [ebp+0Ch]
   add     edx, 14h
   fld     dword [eax]
   fmul    dword [edx]
   faddp   st1, st0
   mov     eax, [ebp+8]
   add     eax, 8
   mov     edx, [ebp+0Ch]
   add     edx, 20h
   fld     dword [eax]
   fmul    dword [edx]
   faddp   st1, st0
   fstp    dword [ecx]
   pop     ebp
   retn    


VectorCopy434:
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


sub_80C3920:
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


sub_80C39A8:
   push    ebp
   mov     ebp, esp
   sub     esp, 4
   fld     dword [ebp+8]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_80C39BE
   jmp     loc_80C39C6
loc_80C39BE:
   fld     dword [ebp+0Ch]
   fstp    dword [ebp-4]
   jmp     loc_80C39CC
loc_80C39C6:
   fld     dword [ebp+10h]
   fstp    dword [ebp-4]
loc_80C39CC:
   fld     dword [ebp-4]
   leave   
   retn    


I_fmax7:
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
   call    sub_80C39A8
   leave   
   retn    


sub_80C3974:
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


sub_80C3AB4:
   push    ebp
   mov     ebp, esp
   mov     ecx, [ebp+10h]
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
   fstp    dword [ecx]
   mov     ecx, [ebp+10h]
   add     ecx, 4
   mov     eax, [ebp+8]
   mov     edx, [ebp+0Ch]
   add     edx, 0Ch
   fld     dword [eax]
   fmul    dword [edx]
   mov     eax, [ebp+8]
   add     eax, 4
   mov     edx, [ebp+0Ch]
   add     edx, 10h
   fld     dword [eax]
   fmul    dword [edx]
   faddp   st1, st0
   mov     eax, [ebp+8]
   add     eax, 8
   mov     edx, [ebp+0Ch]
   add     edx, 14h
   fld     dword [eax]
   fmul    dword [edx]
   faddp   st1, st0
   fstp    dword [ecx]
   mov     ecx, [ebp+10h]
   add     ecx, 8
   mov     eax, [ebp+8]
   mov     edx, [ebp+0Ch]
   add     edx, 18h
   fld     dword [eax]
   fmul    dword [edx]
   mov     eax, [ebp+8]
   add     eax, 4
   mov     edx, [ebp+0Ch]
   add     edx, 1Ch
   fld     dword [eax]
   fmul    dword [edx]
   faddp   st1, st0
   mov     eax, [ebp+8]
   add     eax, 8
   mov     edx, [ebp+0Ch]
   add     edx, 20h
   fld     dword [eax]
   fmul    dword [edx]
   faddp   st1, st0
   fstp    dword [ecx]
   pop     ebp
   retn    


VectorSubtract173:
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


VectorScale714:
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


XModelSetTransArray:
   push    ebp
   mov     ebp, esp
   sub     esp, 58h
   lea     eax, [ebp-48h]
   mov     [esp+8], eax
   mov     eax, [ebp+8]
   mov     eax, [eax+1Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    VectorScale714
   mov     eax, [ebp+8]
   fld     dword [ebp-48h]
   fmul    dword [eax]
   fstp    dword [ebp-0Ch]
   mov     eax, [ebp+8]
   fld     dword [ebp-48h]
   fmul    dword [eax+4]
   fstp    dword [ebp-10h]
   mov     eax, [ebp+8]
   fld     dword [ebp-48h]
   fmul    dword [eax+8]
   fstp    dword [ebp-14h]
   mov     eax, [ebp+8]
   fld     dword [ebp-48h]
   fmul    dword [eax+0Ch]
   fstp    dword [ebp-18h]
   mov     eax, [ebp+8]
   fld     dword [ebp-44h]
   fmul    dword [eax+4]
   fstp    dword [ebp-1Ch]
   mov     eax, [ebp+8]
   fld     dword [ebp-44h]
   fmul    dword [eax+8]
   fstp    dword [ebp-20h]
   mov     eax, [ebp+8]
   fld     dword [ebp-44h]
   fmul    dword [eax+0Ch]
   fstp    dword [ebp-24h]
   mov     eax, [ebp+8]
   fld     dword [ebp-40h]
   fmul    dword [eax+8]
   fstp    dword [ebp-28h]
   mov     eax, [ebp+8]
   fld     dword [ebp-40h]
   fmul    dword [eax+0Ch]
   fstp    dword [ebp-2Ch]
   mov     eax, [ebp+0Ch]
   fld     dword [ebp-1Ch]
   fadd    dword [ebp-28h]
   fld1    
   fsubrp  st1, st0
   fstp    dword [eax]
   mov     eax, [ebp+0Ch]
   add     eax, 4
   fld     dword [ebp-10h]
   fadd    dword [ebp-2Ch]
   fstp    dword [eax]
   mov     eax, [ebp+0Ch]
   add     eax, 8
   fld     dword [ebp-14h]
   fsub    dword [ebp-24h]
   fstp    dword [eax]
   mov     eax, [ebp+0Ch]
   add     eax, 0Ch
   fld     dword [ebp-10h]
   fsub    dword [ebp-2Ch]
   fstp    dword [eax]
   mov     eax, [ebp+0Ch]
   add     eax, 10h
   fld     dword [ebp-0Ch]
   fadd    dword [ebp-28h]
   fld1    
   fsubrp  st1, st0
   fstp    dword [eax]
   mov     eax, [ebp+0Ch]
   add     eax, 14h
   fld     dword [ebp-20h]
   fadd    dword [ebp-18h]
   fstp    dword [eax]
   mov     eax, [ebp+0Ch]
   add     eax, 18h
   fld     dword [ebp-14h]
   fadd    dword [ebp-24h]
   fstp    dword [eax]
   mov     eax, [ebp+0Ch]
   add     eax, 1Ch
   fld     dword [ebp-20h]
   fsub    dword [ebp-18h]
   fstp    dword [eax]
   mov     eax, [ebp+0Ch]
   add     eax, 20h
   fld     dword [ebp-0Ch]
   fadd    dword [ebp-1Ch]
   fld1    
   fsubrp  st1, st0
   fstp    dword [eax]
   leave   
   retn    


XModelTraceLine:
   push    ebp
   mov     ebp, esp
   sub     esp, 118h
   mov     dword [ebp-6Ch], 0FFFFFFFFh
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XModelNumBones
   mov     [ebp-10h], eax
   mov     dword [ebp-0B4h], 0
loc_80C3426:
   mov     edx, [ebp+8]
   mov     eax, [ebp-0B4h]
   cmp     eax, [edx+58h]
   jl      loc_80C3439
   jmp     loc_80C37A1
loc_80C3439:
   mov     ecx, [ebp+8]
   mov     edx, [ebp-0B4h]
   mov     eax, edx
   shl     eax, 2
   add     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   add     eax, [ecx+54h]
   mov     [ebp-70h], eax
   mov     edx, [ebp-70h]
   mov     eax, [ebp+1Ch]
   and     eax, [edx+24h]
   test    eax, eax
   jnz     loc_80C3468
   jmp     loc_80C3794
loc_80C3468:
   mov     eax, [ebp-70h]
   mov     eax, [eax+20h]
   mov     [ebp-0Ch], eax
   mov     eax, [ebp-0Ch]
   shl     eax, 5
   add     eax, [ebp+10h]
   mov     [ebp-14h], eax
   lea     eax, [ebp-28h]
   mov     [esp+8], eax
   mov     eax, [ebp-14h]
   add     eax, 10h
   mov     [esp+4], eax
   mov     eax, [ebp+14h]
   mov     [esp], eax
   call    VectorSubtract173
   lea     eax, [ebp-38h]
   mov     [esp+8], eax
   mov     eax, [ebp-14h]
   add     eax, 10h
   mov     [esp+4], eax
   mov     eax, [ebp+18h]
   mov     [esp], eax
   call    VectorSubtract173
   lea     eax, [ebp-0F8h]
   mov     [esp+4], eax
   mov     eax, [ebp-14h]
   mov     [esp], eax
   call    XModelSetTransArray
   lea     eax, [ebp-68h]
   mov     [esp+8], eax
   lea     eax, [ebp-0F8h]
   mov     [esp+4], eax
   lea     eax, [ebp-28h]
   mov     [esp], eax
   call    sub_80C3AB4
   lea     eax, [ebp-68h]
   add     eax, 0Ch
   mov     [esp+8], eax
   lea     eax, [ebp-0F8h]
   mov     [esp+4], eax
   lea     eax, [ebp-38h]
   mov     [esp], eax
   call    sub_80C3AB4
   lea     eax, [ebp-68h]
   mov     [esp], eax
   call    CM_CalcTraceExtents
   mov     eax, [ebp+0Ch]
   mov     eax, [eax]
   mov     [esp+0Ch], eax
   mov     eax, [ebp-70h]
   add     eax, 14h
   mov     [esp+8], eax
   mov     eax, [ebp-70h]
   add     eax, 8
   mov     [esp+4], eax
   lea     eax, [ebp-68h]
   mov     [esp], eax
   call    CM_TraceBox
   test    eax, eax
   jz      loc_80C3541
   jmp     loc_80C3794
loc_80C3541:
   lea     eax, [ebp-88h]
   mov     [esp+8], eax
   lea     eax, [ebp-68h]
   mov     [esp+4], eax
   lea     eax, [ebp-68h]
   add     eax, 0Ch
   mov     [esp], eax
   call    VectorSubtract173
   mov     dword [ebp-0B8h], 0
loc_80C356A:
   mov     edx, [ebp-70h]
   mov     eax, [ebp-0B8h]
   cmp     eax, [edx+4]
   jl      loc_80C357D
   jmp     loc_80C3794
loc_80C357D:
   mov     ecx, [ebp-70h]
   mov     edx, [ebp-0B8h]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 4
   add     eax, [ecx]
   mov     [ebp-74h], eax
   mov     eax, [ebp-74h]
   mov     [esp+4], eax
   lea     eax, [ebp-68h]
   add     eax, 0Ch
   mov     [esp], eax
   call    sub_80C3974
   mov     eax, [ebp-74h]
   fsub    dword [eax+0Ch]
   fstp    dword [ebp-9Ch]
   fld     dword [ebp-9Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_80C3787
   mov     eax, [ebp-74h]
   mov     [esp+4], eax
   lea     eax, [ebp-68h]
   mov     [esp], eax
   call    sub_80C3974
   mov     eax, [ebp-74h]
   fsub    dword [eax+0Ch]
   fstp    dword [ebp-0A0h]
   fld     dword [ebp-0A0h]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_80C3787
   fld     dword [ebp-0A0h]
   fld     dword [flt_8149464]
   fsubp   st1, st0
   fld     dword [ebp-0A0h]
   fsub    dword [ebp-9Ch]
   fdivp   st1, st0
   fstp    dword [ebp-0A4h]
   mov     eax, 0
   mov     [esp+4], eax
   mov     eax, [ebp-0A4h]
   mov     [esp], eax
   call    I_fmax7
   fstp    dword [ebp-0A4h]
   mov     eax, [ebp+0Ch]
   fld     dword [ebp-0A4h]
   fld     dword [eax]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_80C3787
   fld     dword [ebp-0A0h]
   fsub    dword [ebp-9Ch]
   fld     dword [ebp-0A0h]
   fdivrp  st1, st0
   fstp    dword [ebp-0A8h]
   lea     eax, [ebp-98h]
   mov     [esp+0Ch], eax
   lea     eax, [ebp-88h]
   mov     [esp+8], eax
   mov     eax, [ebp-0A8h]
   mov     [esp+4], eax
   lea     eax, [ebp-68h]
   mov     [esp], eax
   call    sub_80C3920
   mov     eax, [ebp-74h]
   add     eax, 10h
   mov     [esp+4], eax
   lea     eax, [ebp-98h]
   mov     [esp], eax
   call    sub_80C3974
   mov     eax, [ebp-74h]
   fsub    dword [eax+1Ch]
   fstp    dword [ebp-0ACh]
   fld     dword [ebp-0ACh]
   fld     dword [flt_8149468]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80C3787
   fld     dword [ebp-0ACh]
   fld     dword [flt_814946C]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80C3787
   mov     eax, [ebp-74h]
   add     eax, 20h
   mov     [esp+4], eax
   lea     eax, [ebp-98h]
   mov     [esp], eax
   call    sub_80C3974
   mov     eax, [ebp-74h]
   fsub    dword [eax+2Ch]
   fstp    dword [ebp-0B0h]
   fld     dword [ebp-0B0h]
   fld     dword [flt_8149468]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80C3787
   fld     dword [ebp-0ACh]
   fadd    dword [ebp-0B0h]
   fld     dword [flt_814946C]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80C3787
   mov     eax, [ebp-0Ch]
   mov     [ebp-6Ch], eax
   mov     eax, [ebp+0Ch]
   mov     byte [eax+23h], 0
   mov     eax, [ebp+0Ch]
   mov     byte [eax+22h], 0
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp-0A4h]
   mov     [edx], eax
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp-70h]
   mov     eax, [eax+28h]
   mov     [edx+10h], eax
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp-70h]
   mov     eax, [eax+24h]
   mov     [edx+14h], eax
   mov     eax, [ebp+0Ch]
   add     eax, 4
   mov     [esp+4], eax
   mov     eax, [ebp-74h]
   mov     [esp], eax
   call    VectorCopy434
loc_80C3787:
   lea     eax, [ebp-0B8h]
   inc     dword [eax]
   jmp     loc_80C356A
loc_80C3794:
   lea     eax, [ebp-0B4h]
   inc     dword [eax]
   jmp     loc_80C3426
loc_80C37A1:
   cmp     dword [ebp-6Ch], 0
   jns     loc_80C37B3
   mov     dword [ebp-0FCh], 0FFFFFFFFh
   jmp     loc_80C3817
loc_80C37B3:
   mov     eax, [ebp-6Ch]
   shl     eax, 5
   add     eax, [ebp+10h]
   mov     [ebp-14h], eax
   lea     eax, [ebp-0F8h]
   mov     [esp+4], eax
   mov     eax, [ebp-14h]
   mov     [esp], eax
   call    XModelSetTransArray
   lea     eax, [ebp-0C8h]
   mov     [esp+8], eax
   lea     eax, [ebp-0F8h]
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   add     eax, 4
   mov     [esp], eax
   call    sub_80C3A0A
   mov     eax, [ebp+0Ch]
   add     eax, 4
   mov     [esp+4], eax
   lea     eax, [ebp-0C8h]
   mov     [esp], eax
   call    VectorCopy434
   mov     eax, [ebp-6Ch]
   mov     [ebp-0FCh], eax
loc_80C3817:
   mov     eax, [ebp-0FCh]
   leave   
   retn    


section .rdata
flt_8149464     dd 0.125
flt_8149468     dd -0.001
flt_814946C     dd 1.001