extern XAnimCalc


global DObjCalcAnim


section .text


DObjScaleQuats:
   push    ebp
   mov     ebp, esp
   mov     edx, [ebp+10h]
   mov     eax, [ebp+8]
   fild    word [eax]
   fld     dword [ebp+0Ch]
   fmulp   st1, st0
   fstp    dword [edx]
   mov     edx, [ebp+10h]
   add     edx, 4
   mov     eax, [ebp+8]
   add     eax, 2
   fild    word [eax]
   fld     dword [ebp+0Ch]
   fmulp   st1, st0
   fstp    dword [edx]
   mov     edx, [ebp+10h]
   add     edx, 8
   mov     eax, [ebp+8]
   add     eax, 4
   fild    word [eax]
   fld     dword [ebp+0Ch]
   fmulp   st1, st0
   fstp    dword [edx]
   mov     edx, [ebp+10h]
   add     edx, 0Ch
   mov     eax, [ebp+8]
   add     eax, 6
   fild    word [eax]
   fld     dword [ebp+0Ch]
   fmulp   st1, st0
   fstp    dword [edx]
   pop     ebp
   retn    


VectorClear875:
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


DObjCalcAnim:
   push    ebp
   mov     ebp, esp
   push    edi
   push    esi
   push    ebx
   sub     esp, 407Ch
   mov     eax, [ebp+8]
   mov     eax, [eax+4]
   mov     [ebp-20h], eax
   mov     byte [ebp-39h], 1
   mov     dword [ebp-1Ch], 0
loc_80BECA8:
   cmp     dword [ebp-1Ch], 3
   jle     loc_80BECB0
   jmp     loc_80BECEE
loc_80BECB0:
   mov     ebx, [ebp-1Ch]
   mov     eax, [ebp-1Ch]
   lea     edx, 0[eax*4]
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+edx]
   mov     ecx, eax
   not     ecx
   mov     edx, [ebp-20h]
   mov     eax, [ebp-1Ch]
   or      ecx, [edx+eax*4]
   mov     eax, ecx
   mov     [ebp+ebx*4-68h], eax
   mov     eax, [ebp-1Ch]
   mov     eax, [ebp+eax*4-68h]
   not     eax
   test    eax, eax
   jz      loc_80BECE7
   mov     byte [ebp-39h], 0
loc_80BECE7:
   lea     eax, [ebp-1Ch]
   inc     dword [eax]
   jmp     loc_80BECA8
loc_80BECEE:
   cmp     byte [ebp-39h], 0
   jz      loc_80BECF9
   jmp     loc_80BEED8
loc_80BECF9:
   mov     dword [ebp-1Ch], 0
loc_80BED00:
   cmp     dword [ebp-1Ch], 3
   jle     loc_80BED08
   jmp     loc_80BED3F
loc_80BED08:
   mov     ebx, [ebp-20h]
   mov     esi, [ebp-1Ch]
   mov     edi, [ebp-20h]
   mov     ecx, [ebp-1Ch]
   mov     eax, [ebp-1Ch]
   lea     edx, 0[eax*4]
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+edx]
   or      eax, [edi+ecx*4]
   mov     [ebx+esi*4], eax
   mov     edx, [ebp-1Ch]
   mov     eax, [ebp-1Ch]
   mov     eax, [ebp+eax*4-68h]
   mov     [ebp+edx*4-58h], eax
   lea     eax, [ebp-1Ch]
   inc     dword [eax]
   jmp     loc_80BED00
loc_80BED3F:
   mov     eax, [ebp-20h]
   add     eax, 30h
   mov     [ebp-24h], eax
   mov     eax, [ebp+8]
   cmp     dword [eax], 0
   jz      loc_80BED9E
   lea     eax, [ebp-4Ch]
   or      dword [eax], 80000000h
   mov     dword [esp+1Ch], 0
   lea     eax, [ebp-4068h]
   mov     [esp+18h], eax
   mov     dword [esp+14h], 0
   mov     dword [esp+10h], 1
   mov     eax, [ebp-24h]
   mov     [esp+0Ch], eax
   mov     eax, 3F800000h
   mov     [esp+8], eax
   mov     dword [esp+4], 0
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XAnimCalc
loc_80BED9E:
   mov     dword [ebp-34h], 0
   mov     dword [ebp-28h], 0
loc_80BEDAC:
   mov     eax, [ebp+8]
   movzx   eax, byte [eax+18h]
   cmp     [ebp-28h], eax
   jl      loc_80BEDBD
   jmp     loc_80BEED8
loc_80BEDBD:
   mov     edx, [ebp+8]
   mov     eax, [ebp-28h]
   mov     eax, [edx+eax*4+1Ch]
   mov     eax, [eax]
   mov     [ebp-2Ch], eax
   mov     eax, [ebp-2Ch]
   movsx   eax, word [eax+2]
   mov     [ebp-1Ch], eax
loc_80BEDD6:
   cmp     dword [ebp-1Ch], 0
   jnz     loc_80BEDDE
   jmp     loc_80BEE44
loc_80BEDDE:
   mov     eax, [ebp-34h]
   sar     eax, 5
   mov     ecx, [ebp-34h]
   and     ecx, 1Fh
   mov     eax, [ebp+eax*4-68h]
   sar     eax, cl
   and     eax, 1
   test    al, al
   jz      loc_80BEDF9
   jmp     loc_80BEE32
loc_80BEDF9:
   mov     edx, [ebp-24h]
   mov     eax, 0
   mov     [edx], eax
   mov     edx, [ebp-24h]
   mov     eax, 0
   mov     [edx+4], eax
   mov     edx, [ebp-24h]
   mov     eax, 0
   mov     [edx+8], eax
   mov     edx, [ebp-24h]
   mov     eax, 3F800000h
   mov     [edx+0Ch], eax
   mov     eax, [ebp-24h]
   add     eax, 10h
   mov     [esp], eax
   call    VectorClear875
loc_80BEE32:
   lea     eax, [ebp-1Ch]
   dec     dword [eax]
   lea     eax, [ebp-24h]
   add     dword [eax], 20h
   lea     eax, [ebp-34h]
   inc     dword [eax]
   jmp     loc_80BEDD6
loc_80BEE44:
   mov     eax, [ebp-2Ch]
   mov     eax, [eax+8]
   mov     [ebp-30h], eax
   mov     eax, [ebp-2Ch]
   mov     eax, [eax+4]
   mov     [ebp-38h], eax
   mov     eax, [ebp-2Ch]
   movsx   edx, word [eax]
   mov     eax, [ebp-2Ch]
   movsx   eax, word [eax+2]
   sub     edx, eax
   mov     eax, edx
   mov     [ebp-1Ch], eax
loc_80BEE6A:
   cmp     dword [ebp-1Ch], 0
   jnz     loc_80BEE72
   jmp     loc_80BEECE
loc_80BEE72:
   mov     eax, [ebp-34h]
   sar     eax, 5
   mov     ecx, [ebp-34h]
   and     ecx, 1Fh
   mov     eax, [ebp+eax*4-68h]
   sar     eax, cl
   and     eax, 1
   test    al, al
   jz      loc_80BEE8D
   jmp     loc_80BEEB6
loc_80BEE8D:
   mov     eax, [ebp-24h]
   mov     [esp+8], eax
   mov     eax, 38000100h
   mov     [esp+4], eax
   mov     eax, [ebp-30h]
   mov     [esp], eax
   call    DObjScaleQuats
   mov     eax, [ebp-24h]
   add     eax, 10h
   mov     [esp], eax
   call    VectorClear875
loc_80BEEB6:
   lea     eax, [ebp-1Ch]
   dec     dword [eax]
   lea     eax, [ebp-24h]
   add     dword [eax], 20h
   lea     eax, [ebp-34h]
   inc     dword [eax]
   lea     eax, [ebp-30h]
   add     dword [eax], 8
   jmp     loc_80BEE6A
loc_80BEECE:
   lea     eax, [ebp-28h]
   inc     dword [eax]
   jmp     loc_80BEDAC
loc_80BEED8:
   add     esp, 407Ch
   pop     ebx
   pop     esi
   pop     edi
   pop     ebp
   retn    


section .rdata