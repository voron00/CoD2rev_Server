extern DObjCreateDuplicateParts
extern SL_ConvertToString
extern XModelSetTransArray


global DObjCalcSkel


section .text


VectorAdd123:
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


sub_80B9DC6:
   push    ebp
   mov     ebp, esp
   sub     esp, 58h
   lea     eax, [ebp-48h]
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    XModelSetTransArray
   mov     eax, [ebp+8]
   fld     dword [eax]
   fmul    dword [ebp-48h]
   mov     eax, [ebp+8]
   add     eax, 4
   fld     dword [eax]
   fmul    dword [ebp-3Ch]
   faddp   st1, st0
   mov     eax, [ebp+8]
   add     eax, 8
   fld     dword [eax]
   fmul    dword [ebp-30h]
   faddp   st1, st0
   mov     eax, [ebp+0Ch]
   fadd    dword [eax+10h]
   fstp    dword [ebp-10h]
   mov     eax, [ebp+8]
   fld     dword [eax]
   fmul    dword [ebp-44h]
   mov     eax, [ebp+8]
   add     eax, 4
   fld     dword [eax]
   fmul    dword [ebp-38h]
   faddp   st1, st0
   mov     eax, [ebp+8]
   add     eax, 8
   fld     dword [eax]
   fmul    dword [ebp-2Ch]
   faddp   st1, st0
   mov     eax, [ebp+0Ch]
   fadd    dword [eax+14h]
   fstp    dword [ebp-0Ch]
   mov     edx, [ebp+8]
   add     edx, 8
   mov     eax, [ebp+8]
   fld     dword [eax]
   fmul    dword [ebp-40h]
   mov     eax, [ebp+8]
   add     eax, 4
   fld     dword [eax]
   fmul    dword [ebp-34h]
   faddp   st1, st0
   mov     eax, [ebp+8]
   add     eax, 8
   fld     dword [eax]
   fmul    dword [ebp-28h]
   faddp   st1, st0
   mov     eax, [ebp+0Ch]
   fadd    dword [eax+18h]
   fstp    dword [edx]
   mov     edx, [ebp+8]
   mov     eax, [ebp-10h]
   mov     [edx], eax
   mov     edx, [ebp+8]
   add     edx, 4
   mov     eax, [ebp-0Ch]
   mov     [edx], eax
   leave   
   retn    


sub_80B9FB2:
   push    ebp
   mov     ebp, esp
   sub     esp, 18h
   mov     eax, [ebp+8]
   mov     edx, [ebp+0Ch]
   add     edx, 0Ch
   fld     dword [eax]
   fmul    dword [edx]
   mov     eax, [ebp+8]
   add     eax, 0Ch
   mov     edx, [ebp+0Ch]
   fld     dword [eax]
   fmul    dword [edx]
   faddp   st1, st0
   mov     eax, [ebp+8]
   add     eax, 8
   mov     edx, [ebp+0Ch]
   add     edx, 4
   fld     dword [eax]
   fmul    dword [edx]
   faddp   st1, st0
   mov     eax, [ebp+8]
   add     eax, 4
   mov     edx, [ebp+0Ch]
   add     edx, 8
   fld     dword [eax]
   fmul    dword [edx]
   fsubp   st1, st0
   fstp    dword [ebp-18h]
   mov     eax, [ebp+8]
   add     eax, 4
   mov     edx, [ebp+0Ch]
   add     edx, 0Ch
   fld     dword [eax]
   fmul    dword [edx]
   mov     eax, [ebp+8]
   add     eax, 8
   mov     edx, [ebp+0Ch]
   fld     dword [eax]
   fmul    dword [edx]
   fsubp   st1, st0
   mov     eax, [ebp+8]
   add     eax, 0Ch
   mov     edx, [ebp+0Ch]
   add     edx, 4
   fld     dword [eax]
   fmul    dword [edx]
   faddp   st1, st0
   mov     eax, [ebp+8]
   mov     edx, [ebp+0Ch]
   add     edx, 8
   fld     dword [eax]
   fmul    dword [edx]
   faddp   st1, st0
   fstp    dword [ebp-14h]
   mov     eax, [ebp+8]
   add     eax, 8
   mov     edx, [ebp+0Ch]
   add     edx, 0Ch
   fld     dword [eax]
   fmul    dword [edx]
   mov     eax, [ebp+8]
   add     eax, 4
   mov     edx, [ebp+0Ch]
   fld     dword [eax]
   fmul    dword [edx]
   faddp   st1, st0
   mov     eax, [ebp+8]
   mov     edx, [ebp+0Ch]
   add     edx, 4
   fld     dword [eax]
   fmul    dword [edx]
   fsubp   st1, st0
   mov     eax, [ebp+8]
   add     eax, 0Ch
   mov     edx, [ebp+0Ch]
   add     edx, 8
   fld     dword [eax]
   fmul    dword [edx]
   faddp   st1, st0
   fstp    dword [ebp-10h]
   mov     ecx, [ebp+0Ch]
   add     ecx, 0Ch
   mov     eax, [ebp+8]
   add     eax, 0Ch
   mov     edx, [ebp+0Ch]
   add     edx, 0Ch
   fld     dword [eax]
   fmul    dword [edx]
   mov     eax, [ebp+8]
   mov     edx, [ebp+0Ch]
   fld     dword [eax]
   fmul    dword [edx]
   fsubp   st1, st0
   mov     eax, [ebp+8]
   add     eax, 4
   mov     edx, [ebp+0Ch]
   add     edx, 4
   fld     dword [eax]
   fmul    dword [edx]
   fsubp   st1, st0
   mov     eax, [ebp+8]
   add     eax, 8
   mov     edx, [ebp+0Ch]
   add     edx, 8
   fld     dword [eax]
   fmul    dword [edx]
   fsubp   st1, st0
   fstp    dword [ecx]
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp-18h]
   mov     [edx], eax
   mov     edx, [ebp+0Ch]
   add     edx, 4
   mov     eax, [ebp-14h]
   mov     [edx], eax
   mov     edx, [ebp+0Ch]
   add     edx, 8
   mov     eax, [ebp-10h]
   mov     [edx], eax
   leave   
   retn    


sub_80B9E7A:
   push    ebp
   mov     ebp, esp
   sub     esp, 18h
   mov     eax, [ebp+8]
   mov     edx, [ebp+0Ch]
   add     edx, 0Ch
   fld     dword [eax]
   fmul    dword [edx]
   mov     eax, [ebp+8]
   add     eax, 0Ch
   mov     edx, [ebp+0Ch]
   fld     dword [eax]
   fmul    dword [edx]
   faddp   st1, st0
   mov     eax, [ebp+8]
   add     eax, 8
   mov     edx, [ebp+0Ch]
   add     edx, 4
   fld     dword [eax]
   fmul    dword [edx]
   faddp   st1, st0
   mov     eax, [ebp+8]
   add     eax, 4
   mov     edx, [ebp+0Ch]
   add     edx, 8
   fld     dword [eax]
   fmul    dword [edx]
   fsubp   st1, st0
   fstp    dword [ebp-18h]
   mov     eax, [ebp+8]
   add     eax, 4
   mov     edx, [ebp+0Ch]
   add     edx, 0Ch
   fld     dword [eax]
   fmul    dword [edx]
   mov     eax, [ebp+8]
   add     eax, 8
   mov     edx, [ebp+0Ch]
   fld     dword [eax]
   fmul    dword [edx]
   fsubp   st1, st0
   mov     eax, [ebp+8]
   add     eax, 0Ch
   mov     edx, [ebp+0Ch]
   add     edx, 4
   fld     dword [eax]
   fmul    dword [edx]
   faddp   st1, st0
   mov     eax, [ebp+8]
   mov     edx, [ebp+0Ch]
   add     edx, 8
   fld     dword [eax]
   fmul    dword [edx]
   faddp   st1, st0
   fstp    dword [ebp-14h]
   mov     eax, [ebp+8]
   add     eax, 8
   mov     edx, [ebp+0Ch]
   add     edx, 0Ch
   fld     dword [eax]
   fmul    dword [edx]
   mov     eax, [ebp+8]
   add     eax, 4
   mov     edx, [ebp+0Ch]
   fld     dword [eax]
   fmul    dword [edx]
   faddp   st1, st0
   mov     eax, [ebp+8]
   mov     edx, [ebp+0Ch]
   add     edx, 4
   fld     dword [eax]
   fmul    dword [edx]
   fsubp   st1, st0
   mov     eax, [ebp+8]
   add     eax, 0Ch
   mov     edx, [ebp+0Ch]
   add     edx, 8
   fld     dword [eax]
   fmul    dword [edx]
   faddp   st1, st0
   fstp    dword [ebp-10h]
   mov     ecx, [ebp+8]
   add     ecx, 0Ch
   mov     eax, [ebp+8]
   add     eax, 0Ch
   mov     edx, [ebp+0Ch]
   add     edx, 0Ch
   fld     dword [eax]
   fmul    dword [edx]
   mov     eax, [ebp+8]
   mov     edx, [ebp+0Ch]
   fld     dword [eax]
   fmul    dword [edx]
   fsubp   st1, st0
   mov     eax, [ebp+8]
   add     eax, 4
   mov     edx, [ebp+0Ch]
   add     edx, 4
   fld     dword [eax]
   fmul    dword [edx]
   fsubp   st1, st0
   mov     eax, [ebp+8]
   add     eax, 8
   mov     edx, [ebp+0Ch]
   add     edx, 8
   fld     dword [eax]
   fmul    dword [edx]
   fsubp   st1, st0
   fstp    dword [ecx]
   mov     edx, [ebp+8]
   mov     eax, [ebp-18h]
   mov     [edx], eax
   mov     edx, [ebp+8]
   add     edx, 4
   mov     eax, [ebp-14h]
   mov     [edx], eax
   mov     edx, [ebp+8]
   add     edx, 8
   mov     eax, [ebp-10h]
   mov     [edx], eax
   leave   
   retn    


sub_80B9D30:
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
   mov     eax, [ebp+8]
   add     eax, 0Ch
   mov     edx, [ebp+8]
   add     edx, 0Ch
   fld     dword [eax]
   fmul    dword [edx]
   faddp   st1, st0
   pop     ebp
   retn    


sub_80B9D76:
   push    ebp
   mov     ebp, esp
   sub     esp, 8
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    sub_80B9D30
   fstp    dword [ebp-4]
   fld     dword [ebp-4]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80B9D9C
   jp      loc_80B9D9C
   jmp     loc_80B9DAD
loc_80B9D9C:
   mov     eax, [ebp+8]
   fld     dword [flt_8148E18]
   fdiv    dword [ebp-4]
   fstp    dword [eax+1Ch]
   jmp     locret_80B9DC3
loc_80B9DAD:
   mov     edx, [ebp+8]
   mov     eax, 3F800000h
   mov     [edx+0Ch], eax
   mov     edx, [ebp+8]
   mov     eax, 40000000h
   mov     [edx+1Ch], eax
locret_80B9DC3:
   leave   
   retn    


DObjCalcSkel:
   push    ebp
   mov     ebp, esp
   push    edi
   push    esi
   push    ebx
   sub     esp, 0ACh
   mov     eax, [ebp+8]
   mov     eax, [eax+4]
   mov     [ebp-94h], eax
   mov     byte [ebp-95h], 1
   mov     dword [ebp-20h], 0
loc_80B7848:
   cmp     dword [ebp-20h], 3
   jle     loc_80B7850
   jmp     loc_80B7895
loc_80B7850:
   mov     ebx, [ebp-20h]
   mov     eax, [ebp-20h]
   lea     edx, 0[eax*4]
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+edx]
   mov     ecx, eax
   not     ecx
   mov     edx, [ebp-94h]
   mov     eax, [ebp-20h]
   or      ecx, [edx+eax*4+20h]
   mov     eax, ecx
   mov     [ebp+ebx*4-58h], eax
   mov     eax, [ebp-20h]
   mov     eax, [ebp+eax*4-58h]
   not     eax
   test    eax, eax
   jz      loc_80B788E
   mov     byte [ebp-95h], 0
loc_80B788E:
   lea     eax, [ebp-20h]
   inc     dword [eax]
   jmp     loc_80B7848
loc_80B7895:
   cmp     byte [ebp-95h], 0
   jz      loc_80B78A3
   jmp     loc_80B7D65
loc_80B78A3:
   mov     eax, [ebp+8]
   cmp     word [eax+10h], 0
   jnz     loc_80B78B8
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    DObjCreateDuplicateParts
loc_80B78B8:
   mov     eax, [ebp+8]
   movzx   eax, word [eax+10h]
   mov     [esp], eax
   call    SL_ConvertToString
   mov     [ebp-9Ch], eax
   mov     eax, [ebp-9Ch]
   add     eax, 10h
   mov     [ebp-8Ch], eax
   mov     dword [ebp-20h], 0
loc_80B78E3:
   cmp     dword [ebp-20h], 3
   jle     loc_80B78EB
   jmp     loc_80B7968
loc_80B78EB:
   mov     ebx, [ebp-94h]
   mov     esi, [ebp-20h]
   mov     edi, [ebp-94h]
   mov     ecx, [ebp-20h]
   mov     eax, [ebp-20h]
   lea     edx, 0[eax*4]
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+edx]
   or      eax, [edi+ecx*4+20h]
   mov     [ebx+esi*4+20h], eax
   mov     ebx, [ebp-20h]
   mov     ecx, [ebp-94h]
   mov     edx, [ebp-20h]
   mov     eax, [ebp-20h]
   mov     eax, [ebp+eax*4-58h]
   not     eax
   and     eax, [ecx+edx*4+10h]
   mov     [ebp+ebx*4-48h], eax
   mov     ebx, [ebp-20h]
   mov     edx, [ebp-20h]
   mov     eax, [ebp-20h]
   mov     eax, [ebp+eax*4-48h]
   mov     ecx, [ebp+edx*4-58h]
   or      ecx, eax
   mov     eax, [ebp-20h]
   lea     edx, 0[eax*4]
   mov     eax, [ebp-9Ch]
   or      ecx, [eax+edx]
   mov     eax, ecx
   mov     [ebp+ebx*4-68h], eax
   lea     eax, [ebp-20h]
   inc     dword [eax]
   jmp     loc_80B78E3
loc_80B7968:
   mov     dword [ebp-20h], 0
loc_80B796F:
   cmp     dword [ebp-20h], 3
   jle     loc_80B7977
   jmp     loc_80B7995
loc_80B7977:
   mov     ecx, [ebp-20h]
   mov     edx, [ebp-20h]
   mov     eax, [ebp-20h]
   mov     eax, [ebp+eax*4-68h]
   not     eax
   or      eax, [ebp+edx*4-48h]
   mov     [ebp+ecx*4-48h], eax
   lea     eax, [ebp-20h]
   inc     dword [eax]
   jmp     loc_80B796F
loc_80B7995:
   mov     eax, [ebp+8]
   movzx   eax, byte [eax+18h]
   mov     [ebp-7Ch], eax
   mov     eax, [ebp-94h]
   add     eax, 30h
   mov     [ebp-34h], eax
   mov     eax, [ebp-34h]
   mov     [ebp-1Ch], eax
   mov     dword [ebp-70h], 0
   mov     eax, [ebp-8Ch]
   mov     [ebp-84h], eax
   mov     dword [ebp-24h], 0
loc_80B79CB:
   mov     eax, [ebp-24h]
   cmp     eax, [ebp-7Ch]
   jl      loc_80B79D8
   jmp     loc_80B7D65
loc_80B79D8:
   mov     edx, [ebp+8]
   mov     eax, [ebp-24h]
   mov     eax, [edx+eax*4+1Ch]
   mov     eax, [eax]
   mov     [ebp-28h], eax
   mov     eax, [ebp+8]
   add     eax, [ebp-24h]
   add     eax, 30h
   movzx   eax, byte [eax+0Ch]
   mov     [ebp-2Dh], al
   cmp     byte [ebp-2Dh], 0FFh
   jnz     loc_80B7AE4
   mov     eax, [ebp-28h]
   movsx   eax, word [eax+2]
   mov     [ebp-20h], eax
loc_80B7A0B:
   cmp     dword [ebp-20h], 0
   jnz     loc_80B7A16
   jmp     loc_80B7BA4
loc_80B7A16:
   mov     eax, [ebp-70h]
   sar     eax, 5
   mov     [ebp-74h], eax
   mov     ecx, [ebp-70h]
   and     ecx, 1Fh
   mov     eax, 1
   shl     eax, cl
   mov     [ebp-78h], eax
   mov     edx, [ebp-74h]
   mov     eax, [ebp-78h]
   and     eax, [ebp+edx*4-48h]
   test    eax, eax
   jz      loc_80B7A4D
   mov     eax, [ebp-1Ch]
   mov     [esp], eax
   call    sub_80B9D76
   jmp     loc_80B7ACF
loc_80B7A4D:
   mov     eax, [ebp-84h]
   movzx   eax, byte [eax]
   dec     eax
   cmp     [ebp-70h], eax
   jnz     loc_80B7ACF
   lea     eax, [ebp-84h]
   add     dword [eax], 2
   mov     edx, [ebp-74h]
   mov     eax, [ebp-78h]
   and     eax, [ebp+edx*4-58h]
   test    eax, eax
   jz      loc_80B7A75
   jmp     loc_80B7ACF
loc_80B7A75:
   mov     eax, [ebp-84h]
   dec     eax
   movzx   eax, byte [eax]
   dec     eax
   mov     [ebp-90h], eax
   mov     eax, [ebp-90h]
   shl     eax, 5
   add     eax, [ebp-34h]
   mov     [ebp-88h], eax
   mov     ecx, [ebp-1Ch]
   mov     edx, [ebp-88h]
   mov     eax, [edx]
   mov     [ecx], eax
   mov     eax, [edx+4]
   mov     [ecx+4], eax
   mov     eax, [edx+8]
   mov     [ecx+8], eax
   mov     eax, [edx+0Ch]
   mov     [ecx+0Ch], eax
   mov     eax, [edx+10h]
   mov     [ecx+10h], eax
   mov     eax, [edx+14h]
   mov     [ecx+14h], eax
   mov     eax, [edx+18h]
   mov     [ecx+18h], eax
   mov     eax, [edx+1Ch]
   mov     [ecx+1Ch], eax
loc_80B7ACF:
   lea     eax, [ebp-20h]
   dec     dword [eax]
   lea     eax, [ebp-1Ch]
   add     dword [eax], 20h
   lea     eax, [ebp-70h]
   inc     dword [eax]
   jmp     loc_80B7A0B
loc_80B7AE4:
   movzx   eax, byte [ebp-2Dh]
   shl     eax, 5
   add     eax, [ebp-34h]
   mov     [ebp-88h], eax
   mov     eax, [ebp-28h]
   movsx   eax, word [eax+2]
   mov     [ebp-20h], eax
loc_80B7AFE:
   cmp     dword [ebp-20h], 0
   jnz     loc_80B7B09
   jmp     loc_80B7BA4
loc_80B7B09:
   mov     eax, [ebp-70h]
   sar     eax, 5
   mov     [ebp-74h], eax
   mov     ecx, [ebp-70h]
   and     ecx, 1Fh
   mov     eax, 1
   shl     eax, cl
   mov     [ebp-78h], eax
   mov     edx, [ebp-74h]
   mov     eax, [ebp-78h]
   and     eax, [ebp+edx*4-48h]
   test    eax, eax
   jz      loc_80B7B8F
   mov     eax, [ebp-74h]
   mov     edx, [ebp-78h]
   mov     eax, [ebp+eax*4-68h]
   and     eax, edx
   test    eax, eax
   jnz     loc_80B7B57
   mov     eax, [ebp-88h]
   mov     [esp+4], eax
   mov     eax, [ebp-1Ch]
   mov     [esp], eax
   call    sub_80B9E7A
   jmp     loc_80B7B6C
loc_80B7B57:
   mov     eax, [ebp-1Ch]
   mov     [esp+4], eax
   mov     eax, [ebp-88h]
   mov     [esp], eax
   call    sub_80B9FB2
loc_80B7B6C:
   mov     eax, [ebp-1Ch]
   mov     [esp], eax
   call    sub_80B9D76
   mov     eax, [ebp-88h]
   mov     [esp+4], eax
   mov     eax, [ebp-1Ch]
   add     eax, 10h
   mov     [esp], eax
   call    sub_80B9DC6
loc_80B7B8F:
   lea     eax, [ebp-20h]
   dec     dword [eax]
   lea     eax, [ebp-1Ch]
   add     dword [eax], 20h
   lea     eax, [ebp-70h]
   inc     dword [eax]
   jmp     loc_80B7AFE
loc_80B7BA4:
   mov     eax, [ebp-28h]
   mov     eax, [eax+8]
   mov     [ebp-6Ch], eax
   mov     eax, [ebp-28h]
   mov     eax, [eax+0Ch]
   mov     [ebp-38h], eax
   mov     eax, [ebp-28h]
   mov     eax, [eax+4]
   mov     [ebp-80h], eax
   mov     eax, [ebp-80h]
   add     eax, 4
   mov     [ebp-2Ch], eax
   mov     eax, [ebp-28h]
   movsx   edx, word [eax]
   mov     eax, [ebp-28h]
   movsx   eax, word [eax+2]
   sub     edx, eax
   mov     eax, edx
   mov     [ebp-20h], eax
loc_80B7BDC:
   cmp     dword [ebp-20h], 0
   jnz     loc_80B7BE7
   jmp     loc_80B7D5B
loc_80B7BE7:
   mov     eax, [ebp-70h]
   sar     eax, 5
   mov     [ebp-74h], eax
   mov     ecx, [ebp-70h]
   and     ecx, 1Fh
   mov     eax, 1
   shl     eax, cl
   mov     [ebp-78h], eax
   mov     edx, [ebp-74h]
   mov     eax, [ebp-78h]
   and     eax, [ebp+edx*4-48h]
   test    eax, eax
   jz      loc_80B7CB3
   mov     eax, [ebp-74h]
   mov     edx, [ebp-78h]
   mov     eax, [ebp+eax*4-68h]
   and     eax, edx
   test    eax, eax
   jnz     loc_80B7C43
   mov     eax, [ebp-2Ch]
   movzx   eax, byte [eax]
   mov     edx, eax
   shl     edx, 5
   mov     eax, [ebp-1Ch]
   sub     eax, edx
   mov     [esp+4], eax
   mov     eax, [ebp-1Ch]
   mov     [esp], eax
   call    sub_80B9E7A
   jmp     loc_80B7C62
loc_80B7C43:
   mov     eax, [ebp-1Ch]
   mov     [esp+4], eax
   mov     eax, [ebp-2Ch]
   movzx   eax, byte [eax]
   mov     edx, eax
   shl     edx, 5
   mov     eax, [ebp-1Ch]
   sub     eax, edx
   mov     [esp], eax
   call    sub_80B9FB2
loc_80B7C62:
   mov     eax, [ebp-1Ch]
   mov     [esp], eax
   call    sub_80B9D76
   mov     eax, [ebp-1Ch]
   add     eax, 10h
   mov     [esp+8], eax
   mov     eax, [ebp-38h]
   mov     [esp+4], eax
   mov     eax, [ebp-1Ch]
   add     eax, 10h
   mov     [esp], eax
   call    VectorAdd123
   mov     eax, [ebp-2Ch]
   movzx   eax, byte [eax]
   mov     edx, eax
   shl     edx, 5
   mov     eax, [ebp-1Ch]
   sub     eax, edx
   mov     [esp+4], eax
   mov     eax, [ebp-1Ch]
   add     eax, 10h
   mov     [esp], eax
   call    sub_80B9DC6
   jmp     loc_80B7D35
loc_80B7CB3:
   mov     eax, [ebp-84h]
   movzx   eax, byte [eax]
   dec     eax
   cmp     [ebp-70h], eax
   jnz     loc_80B7D35
   lea     eax, [ebp-84h]
   add     dword [eax], 2
   mov     edx, [ebp-74h]
   mov     eax, [ebp-78h]
   and     eax, [ebp+edx*4-58h]
   test    eax, eax
   jz      loc_80B7CDB
   jmp     loc_80B7D35
loc_80B7CDB:
   mov     eax, [ebp-84h]
   dec     eax
   movzx   eax, byte [eax]
   dec     eax
   mov     [ebp-90h], eax
   mov     eax, [ebp-90h]
   shl     eax, 5
   add     eax, [ebp-34h]
   mov     [ebp-88h], eax
   mov     ecx, [ebp-1Ch]
   mov     edx, [ebp-88h]
   mov     eax, [edx]
   mov     [ecx], eax
   mov     eax, [edx+4]
   mov     [ecx+4], eax
   mov     eax, [edx+8]
   mov     [ecx+8], eax
   mov     eax, [edx+0Ch]
   mov     [ecx+0Ch], eax
   mov     eax, [edx+10h]
   mov     [ecx+10h], eax
   mov     eax, [edx+14h]
   mov     [ecx+14h], eax
   mov     eax, [edx+18h]
   mov     [ecx+18h], eax
   mov     eax, [edx+1Ch]
   mov     [ecx+1Ch], eax
loc_80B7D35:
   lea     eax, [ebp-20h]
   dec     dword [eax]
   lea     eax, [ebp-1Ch]
   add     dword [eax], 20h
   lea     eax, [ebp-6Ch]
   add     dword [eax], 8
   lea     eax, [ebp-38h]
   add     dword [eax], 0Ch
   lea     eax, [ebp-2Ch]
   inc     dword [eax]
   lea     eax, [ebp-70h]
   inc     dword [eax]
   jmp     loc_80B7BDC
loc_80B7D5B:
   lea     eax, [ebp-24h]
   inc     dword [eax]
   jmp     loc_80B79CB
loc_80B7D65:
   add     esp, 0ACh
   pop     ebx
   pop     esi
   pop     edi
   pop     ebp
   retn    


section .rdata
flt_8148E18     dd 2.0