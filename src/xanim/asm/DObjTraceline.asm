extern sqrt
extern sqrtf
extern DObjGetRotTransArray
extern XModelSetTransArray
extern SL_ConvertToString


global DObjTraceline


section .text


sub_80A8E2D:
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


sub_80A913D:
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
   call    sub_80A8E2D
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


sub_80A9095:
   push    ebp
   mov     ebp, esp
   sub     esp, 48h
   lea     eax, [ebp-38h]
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    sub_80A913D
   mov     edx, [ebp+10h]
   mov     eax, [ebp+8]
   fld     dword [eax]
   fmul    dword [ebp-38h]
   mov     eax, [ebp+8]
   add     eax, 4
   fld     dword [eax]
   fmul    dword [ebp-2Ch]
   faddp   st1, st0
   mov     eax, [ebp+8]
   add     eax, 8
   fld     dword [eax]
   fmul    dword [ebp-20h]
   faddp   st1, st0
   mov     eax, [ebp+0Ch]
   fadd    dword [eax+10h]
   fstp    dword [edx]
   mov     edx, [ebp+10h]
   add     edx, 4
   mov     eax, [ebp+8]
   fld     dword [eax]
   fmul    dword [ebp-34h]
   mov     eax, [ebp+8]
   add     eax, 4
   fld     dword [eax]
   fmul    dword [ebp-28h]
   faddp   st1, st0
   mov     eax, [ebp+8]
   add     eax, 8
   fld     dword [eax]
   fmul    dword [ebp-1Ch]
   faddp   st1, st0
   mov     eax, [ebp+0Ch]
   fadd    dword [eax+14h]
   fstp    dword [edx]
   mov     edx, [ebp+10h]
   add     edx, 8
   mov     eax, [ebp+8]
   fld     dword [eax]
   fmul    dword [ebp-30h]
   mov     eax, [ebp+8]
   add     eax, 4
   fld     dword [eax]
   fmul    dword [ebp-24h]
   faddp   st1, st0
   mov     eax, [ebp+8]
   add     eax, 8
   fld     dword [eax]
   fmul    dword [ebp-18h]
   faddp   st1, st0
   mov     eax, [ebp+0Ch]
   fadd    dword [eax+18h]
   fstp    dword [edx]
   leave   
   retn  


sub_80A8B5B:
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
   call    sub_80A8B5B
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


sub_80B9BE4:
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


sub_80B9D06:
   push    ebp
   mov     ebp, esp
   sub     esp, 4
   fld     dword [ebp+8]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_80B9D1C
   jmp     loc_80B9D24
loc_80B9D1C:
   fld     dword [ebp+0Ch]
   fstp    dword [ebp-4]
   jmp     loc_80B9D2A
loc_80B9D24:
   fld     dword [ebp+10h]
   fstp    dword [ebp-4]
loc_80B9D2A:
   fld     dword [ebp-4]
   leave   
   retn    


sub_80B9A48:
   push    ebp
   mov     ebp, esp
   sub     esp, 18h
   mov     eax, 0BF800000h
   mov     [esp+8], eax
   mov     eax, 3F800000h
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    sub_80B9D06
   leave   
   retn    


sub_80B9A9C:
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


sub_80B9ABC:
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


sub_80B8C18:
   push    ebp
   mov     ebp, esp
   sub     esp, 58h
   lea     eax, [ebp-18h]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   add     eax, 10h
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    sub_80B9BA0
   lea     eax, [ebp-48h]
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    XModelSetTransArray
   mov     eax, [ebp+10h]
   fld     dword [ebp-18h]
   fmul    dword [ebp-48h]
   fld     dword [ebp-14h]
   fmul    dword [ebp-44h]
   faddp   st1, st0
   fld     dword [ebp-10h]
   fmul    dword [ebp-40h]
   faddp   st1, st0
   fstp    dword [eax]
   mov     eax, [ebp+10h]
   add     eax, 4
   fld     dword [ebp-18h]
   fmul    dword [ebp-3Ch]
   fld     dword [ebp-14h]
   fmul    dword [ebp-38h]
   faddp   st1, st0
   fld     dword [ebp-10h]
   fmul    dword [ebp-34h]
   faddp   st1, st0
   fstp    dword [eax]
   mov     eax, [ebp+10h]
   add     eax, 8
   fld     dword [ebp-18h]
   fmul    dword [ebp-30h]
   fld     dword [ebp-14h]
   fmul    dword [ebp-2Ch]
   faddp   st1, st0
   fld     dword [ebp-10h]
   fmul    dword [ebp-28h]
   faddp   st1, st0
   fstp    dword [eax]
   leave   
   retn    


sub_80B9C1C:
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


sub_80B9C70:
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
  

sub_80B9CA4:
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


sub_80B9BA0:
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


VectorClear188:
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


DObjTraceline:
   push    ebp
   mov     ebp, esp
   push    ebx
   sub     esp, 284h
   mov     eax, [ebp+18h]
   mov     dword [eax+4], 0
   mov     eax, [ebp+18h]
   mov     word [eax+14h], 0
   mov     eax, [ebp+18h]
   mov     word [eax+16h], 0
   mov     eax, [ebp+18h]
   add     eax, 8
   mov     [esp], eax
   call    VectorClear188
   lea     eax, [ebp-18h]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+10h]
   mov     [esp], eax
   call    sub_80B9BA0
   lea     eax, [ebp-18h]
   mov     [esp], eax
   call    sub_80B9CA4
   fstp    dword [ebp-154h]
   fld     dword [ebp-154h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jp      loc_80B8D1B
   jz      loc_80B9551
loc_80B8D1B:
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    DObjGetRotTransArray
   mov     [ebp-30h], eax
   cmp     dword [ebp-30h], 0
   jnz     loc_80B8D34
   jmp     loc_80B9551
loc_80B8D34:
   fld1    
   fdiv    dword [ebp-154h]
   fstp    dword [ebp-1Ch]
   mov     dword [ebp-130h], 2
   mov     eax, [ebp+8]
   mov     eax, [eax+4]
   mov     [ebp-128h], eax
   mov     eax, [ebp+8]
   movzx   eax, word [eax+10h]
   mov     [esp], eax
   call    SL_ConvertToString
   add     eax, 10h
   mov     [ebp-13Ch], eax
   mov     dword [ebp-124h], 0
   mov     dword [ebp-11Ch], 0FFFFFFFFh
   mov     dword [ebp-120h], 0FFFFFFFFh
   mov     eax, 0
   mov     [ebp-118h], eax
   mov     dword [ebp-34h], 0
   mov     eax, [ebp+18h]
   mov     eax, [eax]
   mov     [ebp-0F4h], eax
   mov     dword [ebp-12Ch], 0
loc_80B8DB2:
   mov     eax, [ebp+8]
   movzx   eax, byte [eax+18h]
   cmp     [ebp-12Ch], eax
   jl      loc_80B8DC6
   jmp     loc_80B9508
loc_80B8DC6:
   mov     edx, [ebp+8]
   mov     eax, [ebp-12Ch]
   mov     eax, [edx+eax*4+1Ch]
   mov     [ebp-20h], eax
   mov     eax, [ebp-20h]
   mov     eax, [eax]
   mov     [ebp-24h], eax
   mov     eax, [ebp-24h]
   mov     eax, [eax+4]
   mov     [ebp-14Ch], eax
   mov     eax, [ebp-14Ch]
   mov     eax, [eax]
   mov     [ebp-138h], eax
   mov     eax, [ebp-24h]
   movsx   eax, word [eax]
   mov     [ebp-2Ch], eax
   mov     eax, [ebp+8]
   movzx   edx, byte [eax+1Ah]
   mov     eax, 1
   movzx   ecx, byte [ebp-12Ch]
   shl     eax, cl
   and     eax, edx
   mov     [ebp-144h], eax
   mov     dword [ebp-114h], 0
loc_80B8E28:
   mov     eax, [ebp-114h]
   cmp     eax, [ebp-2Ch]
   jl      loc_80B8E38
   jmp     loc_80B94FB
loc_80B8E38:
   mov     edx, [ebp-24h]
   mov     eax, [ebp-114h]
   add     eax, [edx+10h]
   movzx   ax, byte [eax]
   mov     [ebp-14Eh], ax
   movzx   edx, word [ebp-14Eh]
   mov     eax, [ebp+14h]
   movzx   eax, byte [edx+eax]
   mov     [ebp-134h], eax
   mov     eax, [ebp-13Ch]
   movzx   eax, byte [eax]
   dec     eax
   cmp     [ebp-124h], eax
   jnz     loc_80B8EBD
   lea     eax, [ebp-13Ch]
   add     dword [eax], 2
   cmp     dword [ebp-134h], 1
   jnz     loc_80B8F6E
   mov     eax, [ebp-13Ch]
   dec     eax
   movzx   eax, byte [eax]
   movzx   eax, word [ebp+eax*2-25Ah]
   mov     [ebp-14Eh], ax
   movzx   edx, word [ebp-14Eh]
   mov     eax, [ebp+14h]
   movzx   eax, byte [edx+eax]
   mov     [ebp-134h], eax
   jmp     loc_80B8F6E
loc_80B8EBD:
   cmp     dword [ebp-134h], 1
   jnz     loc_80B8F6E
   mov     eax, [ebp-24h]
   movsx   eax, word [eax+2]
   cmp     [ebp-114h], eax
   jge     loc_80B8F29
   mov     eax, [ebp+8]
   add     eax, [ebp-12Ch]
   add     eax, 30h
   movzx   eax, byte [eax+0Ch]
   mov     [ebp-145h], al
   cmp     byte [ebp-145h], 0FFh
   jz      loc_80B8F10
   movzx   eax, byte [ebp-145h]
   movzx   eax, word [ebp+eax*2-258h]
   mov     [ebp-25Ah], ax
   jmp     loc_80B8F19
loc_80B8F10:
   mov     word [ebp-25Ah], 0
loc_80B8F19:
   movzx   eax, word [ebp-25Ah]
   mov     [ebp-14Eh], ax
   jmp     loc_80B8F5A
loc_80B8F29:
   mov     ecx, [ebp-14Ch]
   mov     eax, [ebp-24h]
   movsx   edx, word [eax+2]
   mov     eax, [ebp-114h]
   sub     eax, edx
   movzx   edx, byte [eax+ecx+4]
   mov     eax, [ebp-124h]
   sub     eax, edx
   movzx   eax, word [ebp+eax*2-258h]
   mov     [ebp-14Eh], ax
loc_80B8F5A:
   movzx   eax, word [ebp-14Eh]
   mov     edx, [ebp+14h]
   movzx   eax, byte [eax+edx]
   mov     [ebp-134h], eax
loc_80B8F6E:
   mov     edx, [ebp-124h]
   movzx   eax, word [ebp-14Eh]
   mov     [ebp+edx*2-258h], ax
   cmp     dword [ebp-144h], 0
   jz      loc_80B8F91
   jmp     loc_80B94E0
loc_80B8F91:
   mov     ecx, [ebp-20h]
   mov     edx, [ebp-114h]
   mov     eax, edx
   shl     eax, 2
   add     eax, edx
   shl     eax, 3
   add     eax, [ecx+60h]
   mov     [ebp-140h], eax
   mov     eax, [ebp-140h]
   fld     dword [eax+24h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jp      loc_80B8FC7
   jz      loc_80B94E0
loc_80B8FC7:
   mov     eax, [ebp-130h]
   cmp     eax, [ebp-134h]
   jbe     loc_80B8FDA
   jmp     loc_80B94E0
loc_80B8FDA:
   lea     eax, [ebp-78h]
   mov     [esp+8], eax
   mov     eax, [ebp-30h]
   mov     [esp+4], eax
   mov     eax, [ebp-140h]
   add     eax, 18h
   mov     [esp], eax
   call    sub_80A9095
   lea     eax, [ebp-88h]
   mov     [esp+8], eax
   lea     eax, [ebp-78h]
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    sub_80B9BA0
   lea     eax, [ebp-18h]
   mov     [esp+4], eax
   lea     eax, [ebp-88h]
   mov     [esp], eax
   call    sub_80B9C70
   fchs    
   fmul    dword [ebp-1Ch]
   fstp    dword [ebp-9Ch]
   fld     dword [ebp-9Ch]
   fld1    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80B9046
   jmp     loc_80B90A8
loc_80B9046:
   fld     dword [ebp-9Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80B9059
   jmp     loc_80B9095
loc_80B9059:
   lea     eax, [ebp-0B8h]
   mov     [esp+0Ch], eax
   lea     eax, [ebp-18h]
   mov     [esp+8], eax
   mov     eax, [ebp-9Ch]
   mov     [esp+4], eax
   lea     eax, [ebp-88h]
   mov     [esp], eax
   call    sub_80B9C1C
   lea     eax, [ebp-0B8h]
   mov     [esp], eax
   call    sub_80B9CA4
   fstp    dword [ebp-28h]
   jmp     loc_80B90D5
loc_80B9095:
   lea     eax, [ebp-88h]
   mov     [esp], eax
   call    sub_80B9CA4
   fstp    dword [ebp-28h]
   jmp     loc_80B90D5
loc_80B90A8:
   lea     eax, [ebp-98h]
   mov     [esp+8], eax
   lea     eax, [ebp-78h]
   mov     [esp+4], eax
   mov     eax, [ebp+10h]
   mov     [esp], eax
   call    sub_80B9BA0
   lea     eax, [ebp-98h]
   mov     [esp], eax
   call    sub_80B9CA4
   fstp    dword [ebp-28h]
loc_80B90D5:
   mov     eax, [ebp-140h]
   fld     dword [eax+24h]
   fsub    dword [ebp-28h]
   fstp    dword [ebp-0BCh]
   fld     dword [ebp-0BCh]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_80B94E0
   mov     eax, [ebp-130h]
   cmp     eax, [ebp-134h]
   jnz     loc_80B9133
   fld     dword [ebp-0BCh]
   fmul    dword [ebp-1Ch]
   fstp    dword [esp]
   call    sqrtf
   fld     dword [ebp-9Ch]
   fsubrp  st1, st0
   mov     eax, [ebp+18h]
   fld     dword [eax]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_80B94E0
loc_80B9133:
   lea     eax, [ebp-0D8h]
   mov     [esp+8], eax
   mov     eax, [ebp-30h]
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    sub_80B8C18
   lea     eax, [ebp-0E8h]
   mov     [esp+8], eax
   mov     eax, [ebp-30h]
   mov     [esp+4], eax
   mov     eax, [ebp+10h]
   mov     [esp], eax
   call    sub_80B8C18
   mov     eax, 0
   mov     [ebp-0ECh], eax
   mov     eax, [ebp-130h]
   cmp     eax, [ebp-134h]
   jnz     loc_80B9191
   mov     eax, [ebp+18h]
   mov     eax, [eax]
   mov     [ebp-0F0h], eax
   jmp     loc_80B919D
loc_80B9191:
   mov     eax, [ebp-0F4h]
   mov     [ebp-0F0h], eax
loc_80B919D:
   mov     byte [ebp-109h], 1
   mov     byte [ebp-10Ah], 1
   mov     eax, 0BF800000h
   mov     [ebp-110h], eax
   mov     eax, [ebp-140h]
   mov     [ebp-108h], eax
loc_80B91C2:
   mov     dword [ebp-104h], 0
loc_80B91CC:
   cmp     dword [ebp-104h], 2
   jle     loc_80B91DA
   jmp     loc_80B935E
loc_80B91DA:
   mov     ecx, [ebp-104h]
   mov     eax, [ebp-104h]
   lea     edx, 0[eax*4]
   mov     eax, [ebp-108h]
   fld     dword [ebp+ecx*4-0D8h]
   fsub    dword [eax+edx]
   fmul    dword [ebp-110h]
   fstp    dword [ebp-0F8h]
   mov     ecx, [ebp-104h]
   mov     eax, [ebp-104h]
   lea     edx, 0[eax*4]
   mov     eax, [ebp-108h]
   fld     dword [ebp+ecx*4-0E8h]
   fsub    dword [eax+edx]
   fmul    dword [ebp-110h]
   fstp    dword [ebp-0FCh]
   fld     dword [ebp-0F8h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80B924E
   jmp     loc_80B92DF
loc_80B924E:
   fld     dword [ebp-0FCh]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80B94E0
   mov     byte [ebp-109h], 0
   fld     dword [ebp-0F8h]
   fsub    dword [ebp-0FCh]
   fstp    dword [ebp-100h]
   fld     dword [ebp-0ECh]
   fmul    dword [ebp-100h]
   fld     dword [ebp-0F8h]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80B929A
   jmp     loc_80B9351
loc_80B929A:
   fld     dword [ebp-0F8h]
   fdiv    dword [ebp-100h]
   fstp    dword [ebp-0ECh]
   fld     dword [ebp-0ECh]
   fld     dword [ebp-0F0h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_80B94E0
   mov     eax, [ebp-110h]
   mov     [ebp-118h], eax
   mov     eax, [ebp-104h]
   mov     [ebp-11Ch], eax
   jmp     loc_80B9351
loc_80B92DF:
   fld     dword [ebp-0FCh]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80B92F2
   jmp     loc_80B9351
loc_80B92F2:
   mov     byte [ebp-10Ah], 0
   fld     dword [ebp-0F8h]
   fsub    dword [ebp-0FCh]
   fstp    dword [ebp-100h]
   fld     dword [ebp-0F0h]
   fmul    dword [ebp-100h]
   fld     dword [ebp-0F8h]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80B9326
   jmp     loc_80B9351
loc_80B9326:
   fld     dword [ebp-0F8h]
   fdiv    dword [ebp-100h]
   fstp    dword [ebp-0F0h]
   fld     dword [ebp-0ECh]
   fld     dword [ebp-0F0h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_80B94E0
loc_80B9351:
   lea     eax, [ebp-104h]
   inc     dword [eax]
   jmp     loc_80B91CC
loc_80B935E:
   fld     dword [ebp-110h]
   fld1    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jp      loc_80B9371
   jz      loc_80B938A
loc_80B9371:
   mov     eax, 3F800000h
   mov     [ebp-110h], eax
   lea     eax, [ebp-108h]
   add     dword [eax], 0Ch
   jmp     loc_80B91C2
loc_80B938A:
   cmp     byte [ebp-109h], 0
   jz      loc_80B946B
   cmp     byte [ebp-10Ah], 0
   jnz     loc_80B93A5
   jmp     loc_80B94E0
loc_80B93A5:
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   lea     eax, [ebp-18h]
   mov     [esp], eax
   call    sub_80B9ABC
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80B94E0
   mov     edx, [ebp+18h]
   mov     eax, 0
   mov     [edx], eax
   mov     ecx, [ebp+18h]
   mov     eax, [ebp-114h]
   lea     edx, [eax+eax]
   mov     eax, [ebp-138h]
   movzx   eax, word [eax+edx]
   mov     [ecx+14h], ax
   mov     edx, [ebp+18h]
   movzx   eax, word [ebp-14Eh]
   mov     [edx+16h], ax
   fld     dword [ebp-18h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80B941A
   jp      loc_80B941A
   fld     dword [ebp-14h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80B941A
   jp      loc_80B941A
   jmp     loc_80B9444
loc_80B941A:
   mov     eax, [ebp+18h]
   add     eax, 8
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    sub_80B9A9C
   mov     eax, [ebp+18h]
   add     eax, 8
   mov     [esp], eax
   call    Vec2Normalize962
   fstp    st0
   jmp     loc_80B9551
loc_80B9444:
   mov     ebx, [ebp+18h]
   mov     eax, [ebp-10h]
   mov     [esp], eax
   call    sub_80B9A48
   fstp    dword [ebp-26Ch]
   mov     eax, [ebp-26Ch]
   xor     eax, 80000000h
   mov     [ebx+10h], eax
   jmp     loc_80B9551
loc_80B946B:
   mov     eax, [ebp-130h]
   cmp     eax, [ebp-134h]
   jnz     loc_80B948F
   mov     eax, [ebp+18h]
   fld     dword [ebp-0ECh]
   fld     dword [eax]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_80B94E0
   jmp     loc_80B949B
loc_80B948F:
   mov     eax, [ebp-134h]
   mov     [ebp-130h], eax
loc_80B949B:
   mov     edx, [ebp+18h]
   mov     eax, [ebp-0ECh]
   mov     [edx], eax
   mov     ecx, [ebp+18h]
   mov     eax, [ebp-114h]
   lea     edx, [eax+eax]
   mov     eax, [ebp-138h]
   movzx   eax, word [eax+edx]
   mov     [ecx+14h], ax
   mov     edx, [ebp+18h]
   movzx   eax, word [ebp-14Eh]
   mov     [edx+16h], ax
   mov     eax, [ebp-11Ch]
   mov     [ebp-120h], eax
   mov     eax, [ebp-30h]
   mov     [ebp-34h], eax
loc_80B94E0:
   lea     eax, [ebp-114h]
   inc     dword [eax]
   lea     eax, [ebp-30h]
   add     dword [eax], 20h
   lea     eax, [ebp-124h]
   inc     dword [eax]
   jmp     loc_80B8E28
loc_80B94FB:
   lea     eax, [ebp-12Ch]
   inc     dword [eax]
   jmp     loc_80B8DB2
loc_80B9508:
   cmp     dword [ebp-34h], 0
   jz      loc_80B9551
   lea     eax, [ebp-68h]
   mov     [esp+4], eax
   mov     eax, [ebp-34h]
   mov     [esp], eax
   call    XModelSetTransArray
   mov     eax, [ebp+18h]
   add     eax, 8
   mov     [esp+8], eax
   mov     eax, [ebp-118h]
   mov     [esp+4], eax
   lea     ecx, [ebp-68h]
   mov     edx, [ebp-120h]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   lea     eax, [eax+ecx]
   mov     [esp], eax
   call    sub_80B9BE4
loc_80B9551:
   add     esp, 284h
   pop     ebx
   pop     ebp
   retn    


section .rdata