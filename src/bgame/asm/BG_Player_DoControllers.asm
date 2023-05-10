extern memset
extern sin
extern cos
extern BG_GetConditionValue
extern GetLeanFraction
extern DObjGetBoneIndex
extern vec3_origin
extern controller_names
extern scr_const


global BG_Player_DoControllers


section .text


sub_80D9C4A:
   push    ebp
   mov     ebp, esp
   sub     esp, 14h
   mov     eax, 3FC00000h
   mov     [ebp-10h], eax
   fld     dword [ebp+8]
   fld     dword [flt_814C168]
   fmulp   st1, st0
   fstp    dword [ebp-8]
   mov     eax, [ebp+8]
   mov     [ebp-0Ch], eax
   mov     eax, [ebp-0Ch]
   mov     [ebp-4], eax
   mov     eax, [ebp-4]
   mov     edx, eax
   sar     edx, 1
   mov     eax, 5F3759DFh
   sub     eax, edx
   mov     [ebp-4], eax
   mov     eax, [ebp-4]
   mov     [ebp-0Ch], eax
   fld     dword [ebp-8]
   fmul    dword [ebp-0Ch]
   fmul    dword [ebp-0Ch]
   fld     dword [flt_814C164]
   fsubrp  st1, st0
   fld     dword [ebp-0Ch]
   fmulp   st1, st0
   fstp    dword [ebp-0Ch]
   mov     eax, [ebp-0Ch]
   mov     [ebp-14h], eax
   fld     dword [ebp-14h]
   leave   
   retn    


sub_80D9DF2:
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


sub_80B9A6E:
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


DObjGetRotTransArray:
   push    ebp
   mov     ebp, esp
   sub     esp, 4
   mov     eax, [ebp+8]
   cmp     dword [eax+4], 0
   jz      loc_80B84EF
   mov     eax, [ebp+8]
   mov     eax, [eax+4]
   add     eax, 30h
   mov     [ebp-4], eax
   jmp     loc_80B84F6
loc_80B84EF:
   mov     dword [ebp-4], 0
loc_80B84F6:
   mov     eax, [ebp-4]
   leave   
   retn    


VectorCopy817:
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


DObjClearAngles:
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
   mov     edx, [ebp+8]
   add     edx, 0Ch
   mov     eax, [ebp+18h]
   mov     [edx], eax
   pop     ebp
   retn    


DObjSetRotTransIndexControl:
   push    ebp
   mov     ebp, esp
   push    esi
   push    ebx
   sub     esp, 10h
   mov     eax, [ebp+10h]
   sar     eax, 5
   mov     [ebp-0Ch], eax
   mov     ecx, [ebp+10h]
   and     ecx, 1Fh
   mov     eax, 1
   shl     eax, cl
   mov     [ebp-10h], eax
   mov     eax, [ebp-0Ch]
   lea     ecx, 0[eax*4]
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp-10h]
   and     eax, [edx+ecx]
   test    eax, eax
   jnz     loc_80B85CD
   mov     dword [ebp-18h], 0
   jmp     loc_80B860C
loc_80B85CD:
   mov     eax, [ebp+8]
   mov     eax, [eax+4]
   mov     [ebp-14h], eax
   mov     ecx, [ebp-14h]
   mov     edx, [ebp-0Ch]
   mov     eax, [ebp-10h]
   and     eax, [ecx+edx*4+20h]
   test    eax, eax
   jz      loc_80B85F0
   mov     dword [ebp-18h], 0
   jmp     loc_80B860C
loc_80B85F0:
   mov     esi, [ebp-14h]
   mov     ebx, [ebp-0Ch]
   mov     ecx, [ebp-14h]
   mov     edx, [ebp-0Ch]
   mov     eax, [ebp-10h]
   or      eax, [ecx+edx*4]
   mov     [esi+ebx*4], eax
   mov     dword [ebp-18h], 1
loc_80B860C:
   mov     eax, [ebp-18h]
   add     esp, 10h
   pop     ebx
   pop     esi
   pop     ebp
   retn    


sub_80D9D9E:
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


sub_80D9D5A:
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


DObjSetLocalTagInternal:
   push    ebp
   mov     ebp, esp
   sub     esp, 78h
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    DObjGetRotTransArray
   mov     [ebp-0Ch], eax
   cmp     dword [ebp-0Ch], 0
   jnz     loc_80B9767
   jmp     locret_80B98CA
loc_80B9767:
   mov     eax, [ebp+14h]
   mov     edx, eax
   shl     edx, 5
   lea     eax, [ebp-0Ch]
   add     [eax], edx
   cmp     dword [ebp+10h], 0
   jz      loc_80B987B
   lea     eax, [ebp-18h]
   add     eax, 4
   mov     [esp+8], eax
   lea     eax, [ebp-18h]
   mov     [esp+4], eax
   mov     eax, [ebp+10h]
   add     eax, 4
   fld     dword [eax]
   fld     qword [dbl_8148E10]
   fmulp   st1, st0
   fstp    dword [esp]
   call    sub_80B9A6E
   lea     eax, [ebp-28h]
   add     eax, 4
   mov     [esp+8], eax
   lea     eax, [ebp-28h]
   mov     [esp+4], eax
   mov     eax, [ebp+10h]
   fld     dword [eax]
   fld     qword [dbl_8148E10]
   fmulp   st1, st0
   fstp    dword [esp]
   call    sub_80B9A6E
   lea     eax, [ebp-20h]
   add     eax, 4
   mov     [esp+8], eax
   lea     eax, [ebp-20h]
   mov     [esp+4], eax
   mov     eax, [ebp+10h]
   add     eax, 8
   fld     dword [eax]
   fld     qword [dbl_8148E10]
   fmulp   st1, st0
   fstp    dword [esp]
   call    sub_80B9A6E
   mov     eax, [ebp-28h]
   mov     [ebp-4Ch], eax
   fld     dword [ebp-4Ch]
   fchs    
   fmul    dword [ebp-18h]
   fstp    dword [ebp-38h]
   fld     dword [ebp-28h]
   fmul    dword [ebp-14h]
   fstp    dword [ebp-34h]
   fld     dword [ebp-24h]
   fmul    dword [ebp-18h]
   fstp    dword [ebp-30h]
   fld     dword [ebp-24h]
   fmul    dword [ebp-14h]
   fstp    dword [ebp-2Ch]
   mov     eax, [ebp-0Ch]
   fld     dword [ebp-20h]
   fmul    dword [ebp-2Ch]
   fld     dword [ebp-1Ch]
   fmul    dword [ebp-38h]
   faddp   st1, st0
   fstp    dword [eax]
   mov     eax, [ebp-0Ch]
   fld     dword [ebp-1Ch]
   fmul    dword [ebp-34h]
   fld     dword [ebp-20h]
   fmul    dword [ebp-30h]
   faddp   st1, st0
   fstp    dword [eax+4]
   mov     edx, [ebp-0Ch]
   mov     eax, [ebp-20h]
   mov     [ebp-4Ch], eax
   fld     dword [ebp-4Ch]
   fchs    
   fmul    dword [ebp-34h]
   fld     dword [ebp-1Ch]
   fmul    dword [ebp-30h]
   faddp   st1, st0
   fstp    dword [edx+8]
   mov     eax, [ebp-0Ch]
   fld     dword [ebp-1Ch]
   fmul    dword [ebp-2Ch]
   fld     dword [ebp-20h]
   fmul    dword [ebp-38h]
   fsubp   st1, st0
   fstp    dword [eax+0Ch]
   jmp     loc_80B98AA
loc_80B987B:
   mov     eax, 3F800000h
   mov     [esp+10h], eax
   mov     eax, 0
   mov     [esp+0Ch], eax
   mov     eax, 0
   mov     [esp+8], eax
   mov     eax, 0
   mov     [esp+4], eax
   mov     eax, [ebp-0Ch]
   mov     [esp], eax
   call    DObjClearAngles
loc_80B98AA:
   mov     edx, [ebp-0Ch]
   mov     eax, 0
   mov     [edx+1Ch], eax
   mov     eax, [ebp-0Ch]
   add     eax, 10h
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    VectorCopy817
locret_80B98CA:
   leave   
   retn    


DObjSetRotTransIndex:
   push    ebp
   mov     ebp, esp
   push    esi
   push    ebx
   sub     esp, 10h
   mov     eax, [ebp+10h]
   sar     eax, 5
   mov     [ebp-0Ch], eax
   mov     ecx, [ebp+10h]
   and     ecx, 1Fh
   mov     eax, 1
   shl     eax, cl
   mov     [ebp-10h], eax
   mov     eax, [ebp-0Ch]
   lea     ecx, 0[eax*4]
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp-10h]
   and     eax, [edx+ecx]
   test    eax, eax
   jnz     loc_80B8657
   mov     dword [ebp-18h], 0
   jmp     loc_80B86AD
loc_80B8657:
   mov     eax, [ebp+8]
   mov     eax, [eax+4]
   mov     [ebp-14h], eax
   mov     ecx, [ebp-14h]
   mov     edx, [ebp-0Ch]
   mov     eax, [ebp-10h]
   and     eax, [ecx+edx*4+20h]
   test    eax, eax
   jz      loc_80B867A
   mov     dword [ebp-18h], 0
   jmp     loc_80B86AD
loc_80B867A:
   mov     esi, [ebp-14h]
   mov     ebx, [ebp-0Ch]
   mov     ecx, [ebp-14h]
   mov     edx, [ebp-0Ch]
   mov     eax, [ebp-10h]
   or      eax, [ecx+edx*4+10h]
   mov     [esi+ebx*4+10h], eax
   mov     esi, [ebp-14h]
   mov     ebx, [ebp-0Ch]
   mov     ecx, [ebp-14h]
   mov     edx, [ebp-0Ch]
   mov     eax, [ebp-10h]
   or      eax, [ecx+edx*4]
   mov     [esi+ebx*4], eax
   mov     dword [ebp-18h], 1
loc_80B86AD:
   mov     eax, [ebp-18h]
   add     esp, 10h
   pop     ebx
   pop     esi
   pop     ebp
   retn    


DObjSetControlTagAngles:
   push    ebp
   mov     ebp, esp
   sub     esp, 18h
   mov     eax, [ebp+10h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    DObjGetBoneIndex
   mov     [ebp-4], eax
   cmp     dword [ebp-4], 0
   jns     loc_80B98F6
   mov     dword [ebp-8], 0
   jmp     loc_80B9944
loc_80B98F6:
   mov     eax, [ebp-4]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    DObjSetRotTransIndex
   test    eax, eax
   jnz     loc_80B991C
   mov     dword [ebp-8], 0
   jmp     loc_80B9944
loc_80B991C:
   mov     eax, [ebp-4]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+14h]
   mov     [esp+8], eax
   mov     dword [esp+4], vec3_origin
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    DObjSetLocalTagInternal
   mov     dword [ebp-8], 1
loc_80B9944:
   mov     eax, [ebp-8]
   leave   
   retn    


BG_LerpOffset:
   push    ebp
   mov     ebp, esp
   sub     esp, 38h
   lea     eax, [ebp-18h]
   mov     [esp+8], eax
   mov     eax, [ebp+10h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    sub_80D9D5A
   lea     eax, [ebp-18h]
   mov     [esp], eax
   call    sub_80D9DF2
   fstp    dword [ebp-1Ch]
   fld     dword [ebp-1Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jp      loc_80D94F5
   jz      locret_80D954A
loc_80D94F5:
   mov     eax, [ebp-1Ch]
   mov     [esp], eax
   call    sub_80D9C4A
   fld     dword [ebp+0Ch]
   fmulp   st1, st0
   fstp    dword [ebp-1Ch]
   fld     dword [ebp-1Ch]
   fld1    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80D9516
   jmp     loc_80D9538
loc_80D9516:
   mov     eax, [ebp+10h]
   mov     [esp+0Ch], eax
   lea     eax, [ebp-18h]
   mov     [esp+8], eax
   mov     eax, [ebp-1Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+10h]
   mov     [esp], eax
   call    sub_80D9D9E
   jmp     locret_80D954A
loc_80D9538:
   mov     eax, [ebp+10h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    VectorCopy439
locret_80D954A:
   leave   
   retn    


DObjSetLocalTag:
   push    ebp
   mov     ebp, esp
   sub     esp, 18h
   mov     eax, [ebp+10h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    DObjGetBoneIndex
   mov     [ebp-4], eax
   cmp     dword [ebp-4], 0
   jns     loc_80B9974
   mov     dword [ebp-8], 0
   jmp     loc_80B99C1
loc_80B9974:
   mov     eax, [ebp-4]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    DObjSetRotTransIndexControl
   test    eax, eax
   jnz     loc_80B999A
   mov     dword [ebp-8], 0
   jmp     loc_80B99C1
loc_80B999A:
   mov     eax, [ebp-4]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+18h]
   mov     [esp+8], eax
   mov     eax, [ebp+14h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    DObjSetLocalTagInternal
   mov     dword [ebp-8], 1
loc_80B99C1:
   mov     eax, [ebp-8]
   leave   
   retn    


BG_LerpAngles:
   push    ebp
   mov     ebp, esp
   push    ebx
   sub     esp, 0Ch
   mov     dword [ebp-0Ch], 0
loc_80D93E6:
   cmp     dword [ebp-0Ch], 2
   jle     loc_80D93F1
   jmp     loc_80D94B2
loc_80D93F1:
   mov     eax, [ebp-0Ch]
   lea     ebx, 0[eax*4]
   mov     ecx, [ebp+8]
   mov     eax, [ebp-0Ch]
   lea     edx, 0[eax*4]
   mov     eax, [ebp+10h]
   fld     dword [ecx+ebx]
   fsub    dword [eax+edx]
   fstp    dword [ebp-8]
   fld     dword [ebp-8]
   fld     dword [ebp+0Ch]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80D9425
   jmp     loc_80D944A
loc_80D9425:
   mov     eax, [ebp-0Ch]
   lea     ecx, 0[eax*4]
   mov     ebx, [ebp+10h]
   mov     eax, [ebp-0Ch]
   lea     edx, 0[eax*4]
   mov     eax, [ebp+10h]
   fld     dword [eax+edx]
   fadd    dword [ebp+0Ch]
   fstp    dword [ebx+ecx]
   jmp     loc_80D94A8
loc_80D944A:
   mov     eax, [ebp+0Ch]
   mov     [ebp-10h], eax
   fld     dword [ebp-10h]
   fchs    
   fld     dword [ebp-8]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80D9463
   jmp     loc_80D9488
loc_80D9463:
   mov     eax, [ebp-0Ch]
   lea     ecx, 0[eax*4]
   mov     ebx, [ebp+10h]
   mov     eax, [ebp-0Ch]
   lea     edx, 0[eax*4]
   mov     eax, [ebp+10h]
   fld     dword [eax+edx]
   fsub    dword [ebp+0Ch]
   fstp    dword [ebx+ecx]
   jmp     loc_80D94A8
loc_80D9488:
   mov     eax, [ebp-0Ch]
   lea     ebx, 0[eax*4]
   mov     ecx, [ebp+10h]
   mov     eax, [ebp-0Ch]
   lea     edx, 0[eax*4]
   mov     eax, [ebp+8]
   mov     eax, [eax+edx]
   mov     [ecx+ebx], eax
loc_80D94A8:
   lea     eax, [ebp-0Ch]
   inc     dword [eax]
   jmp     loc_80D93E6
loc_80D94B2:
   add     esp, 0Ch
   pop     ebx
   pop     ebp
   retn    


BG_Player_DoControllers:
   push    ebp
   mov     ebp, esp
   sub     esp, 98h
   lea     eax, [ebp-78h]
   mov     [esp+10h], eax
   mov     eax, [ebp+14h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+10h]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    BG_Player_DoControllersInternal
   fild    dword [ebp+18h]
   fld     dword [flt_814C124]
   fmulp   st1, st0
   fstp    dword [ebp-10h]
   mov     dword [ebp-0Ch], 0
loc_80D9591:
   cmp     dword [ebp-0Ch], 5
   jle     loc_80D9599
   jmp     loc_80D9617
loc_80D9599:
   mov     edx, [ebp-0Ch]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   add     eax, [ebp+14h]
   add     eax, 3FCh
   mov     [esp+8], eax
   mov     eax, [ebp-10h]
   mov     [esp+4], eax
   lea     ecx, [ebp-78h]
   mov     edx, [ebp-0Ch]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   lea     eax, [eax+ecx]
   mov     [esp], eax
   call    BG_LerpAngles
   mov     edx, [ebp-0Ch]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   add     eax, [ebp+14h]
   add     eax, 3FCh
   mov     [esp+0Ch], eax
   mov     eax, [ebp-0Ch]
   mov     eax, controller_names[eax*4]
   movzx   eax, word [eax]
   mov     [esp+8], eax
   mov     eax, [ebp+10h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    DObjSetControlTagAngles
   lea     eax, [ebp-0Ch]
   inc     dword [eax]
   jmp     loc_80D9591
loc_80D9617:
   mov     eax, [ebp+14h]
   add     eax, 444h
   mov     [esp+8], eax
   mov     eax, [ebp-10h]
   mov     [esp+4], eax
   lea     eax, [ebp-78h]
   add     eax, 48h
   mov     [esp], eax
   call    BG_LerpAngles
   mov     eax, [ebp+14h]
   add     eax, 450h
   mov     [esp+8], eax
   fild    dword [ebp+18h]
   fld     dword [flt_814C128]
   fmulp   st1, st0
   fstp    dword [esp+4]
   lea     eax, [ebp-78h]
   add     eax, 54h
   mov     [esp], eax
   call    BG_LerpOffset
   mov     eax, [ebp+14h]
   add     eax, 444h
   mov     [esp+10h], eax
   mov     eax, [ebp+14h]
   add     eax, 450h
   mov     [esp+0Ch], eax
   movzx   eax, word [scr_const+0A2h]
   mov     [esp+8], eax
   mov     eax, [ebp+10h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    DObjSetLocalTag
   leave   
   retn    


sub_80D9D06:
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


sub_80A6062:
   push    ebp
   mov     ebp, esp
   sub     esp, 8
   mov     eax, [ebp+0Ch]
   mov     eax, [eax]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     eax, [eax]
   mov     [esp], eax
   call    AngleSubtract
   mov     eax, [ebp+10h]
   fstp    dword [eax]
   mov     eax, [ebp+0Ch]
   add     eax, 4
   mov     eax, [eax]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   add     eax, 4
   mov     eax, [eax]
   mov     [esp], eax
   call    AngleSubtract
   mov     eax, [ebp+10h]
   add     eax, 4
   fstp    dword [eax]
   mov     eax, [ebp+0Ch]
   add     eax, 8
   mov     eax, [eax]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   add     eax, 8
   mov     eax, [eax]
   mov     [esp], eax
   call    AngleSubtract
   mov     eax, [ebp+10h]
   add     eax, 8
   fstp    dword [eax]
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


sub_80D9CAE:
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


VectorCopy439:
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


AngleSubtract:
   push    ebp
   mov     ebp, esp
   sub     esp, 8
   fld     dword [ebp+8]
   fsub    dword [ebp+0Ch]
   fstp    dword [ebp-4]
loc_80A600F:
   fld     dword [ebp-4]
   fld     dword [flt_8146074]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80A6023
   jmp     loc_80A6033
loc_80A6023:
   fld     dword [ebp-4]
   fld     dword [flt_8146078]
   fsubp   st1, st0
   fstp    dword [ebp-4]
   jmp     loc_80A600F
loc_80A6033:
   nop     
loc_80A6034:
   fld     dword [ebp-4]
   fld     dword [flt_814607C]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80A6046
   jmp     loc_80A6056
loc_80A6046:
   fld     dword [ebp-4]
   fld     dword [flt_8146078]
   faddp   st1, st0
   fstp    dword [ebp-4]
   jmp     loc_80A6034
loc_80A6056:
   mov     eax, [ebp-4]
   mov     [ebp-8], eax
   fld     dword [ebp-8]
   leave   
   retn    


VectorClear871:
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


BG_Player_DoControllersInternal:
   push    ebp
   mov     ebp, esp
   sub     esp, 0E8h
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+8]
   and     eax, 300h
   test    eax, eax
   jz      loc_80D8C76
   mov     dword [esp+8], 60h
   mov     dword [esp+4], 0
   mov     eax, [ebp+18h]
   mov     [esp], eax
   call    memset
   jmp     locret_80D93D6
loc_80D8C76:
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+90h]
   mov     [ebp-0ACh], eax
   lea     eax, [ebp-68h]
   mov     [esp], eax
   call    VectorClear871
   lea     eax, [ebp-98h]
   mov     [esp], eax
   call    VectorClear871
   lea     eax, [ebp-0A8h]
   mov     [esp+4], eax
   mov     eax, [ebp+14h]
   add     eax, 3E8h
   mov     [esp], eax
   call    VectorCopy439
   mov     eax, [ebp+14h]
   mov     eax, [eax+380h]
   mov     [ebp-64h], eax
   mov     eax, [ebp+14h]
   mov     eax, [eax+3B0h]
   mov     [ebp-94h], eax
   mov     dword [esp+8], 0
   mov     dword [esp+4], 3
   mov     eax, [ebp+14h]
   mov     [esp], eax
   call    BG_GetConditionValue
   and     eax, 0C0000h
   test    eax, eax
   jnz     loc_80D8D64
   mov     eax, [ebp+14h]
   mov     eax, [eax+3B8h]
   mov     [ebp-98h], eax
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+8]
   and     eax, 8
   test    eax, eax
   jz      loc_80D8D64
   mov     eax, [ebp-98h]
   mov     [esp], eax
   call    AngleNormalize180
   fstp    dword [ebp-98h]
   fld     dword [ebp-98h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80D8D3A
   jmp     loc_80D8D50
loc_80D8D3A:
   fld     dword [ebp-98h]
   fld     dword [flt_814C0C4]
   fmulp   st1, st0
   fstp    dword [ebp-98h]
   jmp     loc_80D8D64
loc_80D8D50:
   fld     dword [ebp-98h]
   fld     dword [flt_814C0C8]
   fmulp   st1, st0
   fstp    dword [ebp-98h]
loc_80D8D64:
   lea     eax, [ebp-0A8h]
   mov     [esp+8], eax
   lea     eax, [ebp-98h]
   mov     [esp+4], eax
   lea     eax, [ebp-0A8h]
   mov     [esp], eax
   call    sub_80A6062
   lea     eax, [ebp-98h]
   mov     [esp+8], eax
   lea     eax, [ebp-68h]
   mov     [esp+4], eax
   lea     eax, [ebp-98h]
   mov     [esp], eax
   call    sub_80A6062
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+0E4h]
   mov     [esp+0Ch], eax
   mov     eax, 0
   mov     [esp+8], eax
   mov     eax, 0
   mov     [esp+4], eax
   lea     eax, [ebp-78h]
   mov     [esp], eax
   call    sub_80D9D06
   mov     eax, [ebp+14h]
   mov     eax, [eax+3E4h]
   mov     [esp], eax
   call    GetLeanFraction
   fstp    dword [ebp-7Ch]
   fld     dword [ebp-7Ch]
   fld     dword [flt_814C0CC]
   fmulp   st1, st0
   fld     dword [flt_814C0D0]
   fmulp   st1, st0
   fstp    dword [ebp-90h]
   fld     dword [ebp-7Ch]
   fld     dword [flt_814C0CC]
   fmulp   st1, st0
   fld     dword [flt_814C0D0]
   fmulp   st1, st0
   fstp    dword [ebp-0A0h]
   fld     dword [ebp-7Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80D8E2A
   jp      loc_80D8E2A
   jmp     loc_80D8EE1
loc_80D8E2A:
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+8]
   and     eax, 4
   test    eax, eax
   jz      loc_80D8E8D
   fld     dword [ebp-7Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80D8E47
   jmp     loc_80D8E6A
loc_80D8E47:
   mov     eax, [ebp-7Ch]
   mov     [ebp-0CCh], eax
   fld     dword [ebp-0CCh]
   fchs    
   fld     dword [flt_814C0D4]
   fmulp   st1, st0
   fld     dword [ebp-74h]
   faddp   st1, st0
   fstp    dword [ebp-74h]
   jmp     loc_80D8EE1
loc_80D8E6A:
   mov     eax, [ebp-7Ch]
   mov     [ebp-0CCh], eax
   fld     dword [ebp-0CCh]
   fchs    
   fld     dword [flt_814C0D8]
   fmulp   st1, st0
   fld     dword [ebp-74h]
   faddp   st1, st0
   fstp    dword [ebp-74h]
   jmp     loc_80D8EE1
loc_80D8E8D:
   fld     dword [ebp-7Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80D8E9D
   jmp     loc_80D8EC0
loc_80D8E9D:
   mov     eax, [ebp-7Ch]
   mov     [ebp-0CCh], eax
   fld     dword [ebp-0CCh]
   fchs    
   fld     dword [flt_814C0D4]
   fmulp   st1, st0
   fld     dword [ebp-74h]
   faddp   st1, st0
   fstp    dword [ebp-74h]
   jmp     loc_80D8EE1
loc_80D8EC0:
   mov     eax, [ebp-7Ch]
   mov     [ebp-0CCh], eax
   fld     dword [ebp-0CCh]
   fchs    
   fld     dword [flt_814C0DC]
   fmulp   st1, st0
   fld     dword [ebp-74h]
   faddp   st1, st0
   fstp    dword [ebp-74h]
loc_80D8EE1:
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+8]
   and     eax, 20000h
   test    eax, eax
   jnz     loc_80D8F0B
   mov     eax, [ebp+14h]
   mov     eax, [eax+3ECh]
   mov     [esp+4], eax
   mov     eax, [ebp-64h]
   mov     [esp], eax
   call    AngleSubtract
   fstp    dword [ebp-64h]
loc_80D8F0B:
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+8]
   and     eax, 8
   test    eax, eax
   jz      loc_80D90D8
   fld     dword [ebp-7Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80D8F2E
   jp      loc_80D8F2E
   jmp     loc_80D8F42
loc_80D8F2E:
   fld     dword [ebp-0A0h]
   fld     dword [flt_814C0C4]
   fmulp   st1, st0
   fstp    dword [ebp-0A0h]
loc_80D8F42:
   mov     eax, [ebp+0Ch]
   fld     dword [ebp-68h]
   fadd    dword [eax+0E8h]
   fstp    dword [ebp-68h]
   lea     eax, [ebp-0B4h]
   mov     [esp+8], eax
   lea     eax, [ebp-0B0h]
   mov     [esp+4], eax
   fld     dword [ebp-94h]
   fld     qword [dbl_814C0E0]
   fmulp   st1, st0
   fstp    dword [esp]
   call    sub_80D9CAE
   fld1    
   fsub    dword [ebp-0B4h]
   fld     dword [flt_814C0E8]
   fmulp   st1, st0
   fld     dword [ebp-78h]
   faddp   st1, st0
   fstp    dword [ebp-78h]
   fld     dword [ebp-0B0h]
   fld     dword [flt_814C0EC]
   fmulp   st1, st0
   fld     dword [ebp-74h]
   faddp   st1, st0
   fstp    dword [ebp-74h]
   fld     dword [ebp-7Ch]
   fmul    dword [ebp-0B0h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80D8FBF
   jmp     loc_80D8FEA
loc_80D8FBF:
   mov     eax, [ebp-7Ch]
   mov     [ebp-0CCh], eax
   fld     dword [ebp-0CCh]
   fchs    
   fld1    
   fsub    dword [ebp-0B4h]
   fmulp   st1, st0
   fld     dword [flt_814C0F0]
   fmulp   st1, st0
   fld     dword [ebp-74h]
   faddp   st1, st0
   fstp    dword [ebp-74h]
loc_80D8FEA:
   mov     eax, 0
   mov     [ebp-58h], eax
   fld     dword [ebp-90h]
   fld     dword [flt_814C0F4]
   fmulp   st1, st0
   fstp    dword [ebp-54h]
   fld     dword [ebp-90h]
   fld     dword [flt_814C0F8]
   fmulp   st1, st0
   fstp    dword [ebp-50h]
   mov     eax, [ebp+0Ch]
   fld     dword [eax+0E8h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80D9042
   jp      loc_80D9042
   mov     eax, [ebp+0Ch]
   fld     dword [eax+0ECh]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80D9042
   jp      loc_80D9042
   jmp     loc_80D9068
loc_80D9042:
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+0ECh]
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+0E8h]
   mov     [esp], eax
   call    AngleSubtract
   fld     dword [ebp-58h]
   faddp   st1, st0
   fstp    dword [ebp-58h]
loc_80D9068:
   mov     eax, 0
   mov     [ebp-4Ch], eax
   fld     dword [ebp-94h]
   fld     dword [flt_814C0FC]
   fmulp   st1, st0
   fld     dword [ebp-90h]
   fld     dword [flt_814C100]
   fmulp   st1, st0
   fsubp   st1, st0
   fstp    dword [ebp-48h]
   fld     dword [ebp-90h]
   fld     dword [flt_814C100]
   fmulp   st1, st0
   fstp    dword [ebp-44h]
   mov     eax, [ebp-98h]
   mov     [ebp-40h], eax
   fld     dword [ebp-94h]
   fld     dword [flt_814C104]
   fmulp   st1, st0
   fadd    dword [ebp-90h]
   fstp    dword [ebp-3Ch]
   fld     dword [ebp-90h]
   fld     dword [flt_814C108]
   fmulp   st1, st0
   fstp    dword [ebp-38h]
   jmp     loc_80D92A0
loc_80D90D8:
   fld     dword [ebp-7Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80D90ED
   jp      loc_80D90ED
   jmp     loc_80D9198
loc_80D90ED:
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+8]
   and     eax, 4
   test    eax, eax
   jz      loc_80D9136
   fld     dword [ebp-7Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80D9198
   fld     dword [ebp-90h]
   fld     dword [flt_814C10C]
   fmulp   st1, st0
   fstp    dword [ebp-90h]
   fld     dword [ebp-0A0h]
   fld     dword [flt_814C10C]
   fmulp   st1, st0
   fstp    dword [ebp-0A0h]
   jmp     loc_80D9198
loc_80D9136:
   fld     dword [ebp-7Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80D9146
   jmp     loc_80D9170
loc_80D9146:
   fld     dword [ebp-90h]
   fld     dword [flt_814C10C]
   fmulp   st1, st0
   fstp    dword [ebp-90h]
   fld     dword [ebp-0A0h]
   fld     dword [flt_814C10C]
   fmulp   st1, st0
   fstp    dword [ebp-0A0h]
   jmp     loc_80D9198
loc_80D9170:
   fld     dword [ebp-90h]
   fld     dword [flt_814C10C]
   fmulp   st1, st0
   fstp    dword [ebp-90h]
   fld     dword [ebp-0A0h]
   fld     dword [flt_814C10C]
   fmulp   st1, st0
   fstp    dword [ebp-0A0h]
loc_80D9198:
   fld     dword [ebp-7Ch]
   fld     dword [flt_814C0CC]
   fmulp   st1, st0
   fld     dword [flt_814C110]
   fmulp   st1, st0
   fld     dword [ebp-60h]
   faddp   st1, st0
   fstp    dword [ebp-60h]
   fld     dword [ebp-98h]
   fld     dword [flt_814C100]
   fmulp   st1, st0
   fstp    dword [ebp-58h]
   fld     dword [ebp-94h]
   fld     dword [flt_814C114]
   fmulp   st1, st0
   fstp    dword [ebp-54h]
   fld     dword [ebp-90h]
   fld     dword [flt_814C0C4]
   fmulp   st1, st0
   fstp    dword [ebp-50h]
   mov     eax, [ebp+0Ch]
   fld     dword [eax+0E8h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80D9214
   jp      loc_80D9214
   mov     eax, [ebp+0Ch]
   fld     dword [eax+0ECh]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80D9214
   jp      loc_80D9214
   jmp     loc_80D923A
loc_80D9214:
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+0ECh]
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+0E8h]
   mov     [esp], eax
   call    AngleSubtract
   fld     dword [ebp-58h]
   faddp   st1, st0
   fstp    dword [ebp-58h]
loc_80D923A:
   fld     dword [ebp-98h]
   fld     dword [flt_814C0F8]
   fmulp   st1, st0
   fstp    dword [ebp-4Ch]
   fld     dword [ebp-94h]
   fld     dword [flt_814C114]
   fmulp   st1, st0
   fstp    dword [ebp-48h]
   fld     dword [ebp-90h]
   fld     dword [flt_814C0C4]
   fmulp   st1, st0
   fstp    dword [ebp-44h]
   fld     dword [ebp-98h]
   fld     dword [flt_814C0C4]
   fmulp   st1, st0
   fstp    dword [ebp-40h]
   fld     dword [ebp-94h]
   fld     dword [flt_814C100]
   fmulp   st1, st0
   fstp    dword [ebp-3Ch]
   fld     dword [ebp-90h]
   fld     dword [flt_814C118]
   fmulp   st1, st0
   fstp    dword [ebp-38h]
loc_80D92A0:
   fld     dword [ebp-0A8h]
   fld     dword [flt_814C0F8]
   fmulp   st1, st0
   fstp    dword [ebp-34h]
   fld     dword [ebp-0A4h]
   fld     dword [flt_814C0F8]
   fmulp   st1, st0
   fstp    dword [ebp-30h]
   mov     eax, 0
   mov     [ebp-2Ch], eax
   fld     dword [ebp-0A8h]
   fld     dword [flt_814C11C]
   fmulp   st1, st0
   fstp    dword [ebp-28h]
   fld     dword [ebp-0A4h]
   fld     dword [flt_814C11C]
   fmulp   st1, st0
   fstp    dword [ebp-24h]
   fld     dword [ebp-0A0h]
   fld     dword [flt_814C120]
   fmulp   st1, st0
   fstp    dword [ebp-20h]
   lea     eax, [ebp-58h]
   add     eax, 3Ch
   mov     [esp], eax
   call    VectorClear871
   mov     eax, [ebp+0Ch]
   fld     dword [eax+0ECh]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80D9339
   jp      loc_80D9339
   mov     eax, [ebp+0Ch]
   fld     dword [eax+0E8h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80D9339
   jp      loc_80D9339
   jmp     loc_80D935A
loc_80D9339:
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+0E8h]
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+0ECh]
   mov     [esp], eax
   call    AngleSubtract
   fstp    dword [ebp-1Ch]
loc_80D935A:
   mov     dword [ebp-0B8h], 0
loc_80D9364:
   cmp     dword [ebp-0B8h], 5
   jle     loc_80D936F
   jmp     loc_80D93AC
loc_80D936F:
   mov     edx, [ebp-0B8h]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   add     eax, [ebp+18h]
   mov     [esp+4], eax
   lea     ecx, [ebp-58h]
   mov     edx, [ebp-0B8h]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   lea     eax, [eax+ecx]
   mov     [esp], eax
   call    VectorCopy439
   lea     eax, [ebp-0B8h]
   inc     dword [eax]
   jmp     loc_80D9364
loc_80D93AC:
   mov     eax, [ebp+18h]
   add     eax, 48h
   mov     [esp+4], eax
   lea     eax, [ebp-68h]
   mov     [esp], eax
   call    VectorCopy439
   mov     eax, [ebp+18h]
   add     eax, 54h
   mov     [esp+4], eax
   lea     eax, [ebp-78h]
   mov     [esp], eax
   call    VectorCopy439
locret_80D93D6:
   leave   
   retn    


section .rdata
flt_814C120     dd -0.30000001
flt_814C0F8     dd 0.30000001
flt_814C11C     dd 0.69999999
flt_814C118     dd -0.60000002
flt_814C114     dd 0.40000001
flt_814C0C4     dd 0.5
flt_814C100     dd 0.2
flt_814C10C     dd 1.25
flt_8146080     dd 182.04445
flt_814C108     dd -0.2
flt_814C0E8     dd -24.0
flt_8146084     dd 0.0054931641
flt_814C0EC     dd -12.0
flt_814C0D4     dd 2.5
flt_814607C     dd -180.0
flt_814C110     dd 0.075000003
flt_814C0CC     dd 50.0
flt_814C0F0     dd 16.0
flt_8146088     dd 180.0
flt_814C0FC     dd 0.1
flt_814C0C8     dd 0.25
flt_814608C     dd 360.0
flt_8146074     dd 180.0
flt_814C0D8     dd 12.5
flt_814C0F4     dd -1.2
flt_8146078     dd 360.0
flt_814C104     dd 0.80000001
flt_814C0D0     dd 0.92500001
flt_814C0DC     dd 5.0

flt_814C168     dd 0.5
flt_814C164     dd 1.5
flt_814C124     dd 0.36000001
flt_814C128     dd 0.1

dbl_814C0E0     dq 0.0174532925199433
dbl_8148E10     dq 0.008726646259971648