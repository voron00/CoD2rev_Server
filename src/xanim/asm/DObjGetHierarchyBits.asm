extern DObjCreateDuplicateParts
extern SL_ConvertToString


global DObjGetHierarchyBits


section .text


DObjGetHierarchyBits:
   push    ebp
   mov     ebp, esp
   push    edi
   push    esi
   push    ebx
   sub     esp, 6Ch
   mov     dword [ebp-20h], 0
loc_80B73D0:
   cmp     dword [ebp-20h], 3
   jle     loc_80B73D8
   jmp     loc_80B73F3
loc_80B73D8:
   mov     eax, [ebp-20h]
   lea     edx, 0[eax*4]
   mov     eax, [ebp+10h]
   mov     dword [eax+edx], 0
   lea     eax, [ebp-20h]
   inc     dword [eax]
   jmp     loc_80B73D0
loc_80B73F3:
   mov     eax, [ebp+8]
   movzx   eax, byte [eax+18h]
   mov     [ebp-1Ch], eax
   mov     eax, [ebp+8]
   cmp     word [eax+10h], 0
   jnz     loc_80B7412
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    DObjCreateDuplicateParts
loc_80B7412:
   mov     eax, [ebp+8]
   movzx   eax, word [eax+10h]
   mov     [esp], eax
   call    SL_ConvertToString
   mov     [ebp-5Ch], eax
   mov     eax, [ebp-5Ch]
   add     eax, 10h
   mov     [ebp-28h], eax
   mov     eax, [ebp-28h]
   mov     [ebp-34h], eax
   mov     dword [ebp-58h], 0
   mov     dword [ebp-20h], 0
loc_80B7441:
   mov     edx, [ebp+8]
   mov     eax, [ebp-20h]
   mov     eax, [edx+eax*4+1Ch]
   mov     eax, [eax]
   mov     [ebp-24h], eax
   mov     edx, [ebp-20h]
   mov     eax, [ebp-24h]
   movsx   eax, word [eax]
   add     eax, [ebp+edx*4-58h]
   mov     [ebp-38h], eax
   mov     eax, [ebp-38h]
   cmp     eax, [ebp+0Ch]
   jle     loc_80B746A
   jmp     loc_80B7488
loc_80B746A:
   lea     eax, [ebp-20h]
   inc     dword [eax]
   mov     eax, [ebp-20h]
   cmp     eax, [ebp-1Ch]
   jnz     loc_80B747C
   jmp     loc_80B759F
loc_80B747C:
   mov     edx, [ebp-20h]
   mov     eax, [ebp-38h]
   mov     [ebp+edx*4-58h], eax
   jmp     loc_80B7441
loc_80B7488:
   mov     eax, [ebp-24h]
   mov     eax, [eax+4]
   add     eax, 4
   mov     [ebp-2Ch], eax
loc_80B7494:
   mov     eax, [ebp-20h]
   mov     edx, [ebp+eax*4-58h]
   mov     eax, [ebp+0Ch]
   sub     eax, edx
   mov     [ebp-30h], eax
loc_80B74A3:
   mov     eax, [ebp+0Ch]
   sar     eax, 5
   lea     edi, 0[eax*4]
   mov     esi, [ebp+10h]
   mov     eax, [ebp+0Ch]
   sar     eax, 5
   lea     ebx, 0[eax*4]
   mov     edx, [ebp+10h]
   mov     ecx, [ebp+0Ch]
   and     ecx, 1Fh
   mov     eax, 1
   shl     eax, cl
   or      eax, [edx+ebx]
   mov     [esi+edi], eax
   mov     eax, [ebp+0Ch]
   sar     eax, 5
   lea     edx, 0[eax*4]
   mov     eax, [ebp-5Ch]
   mov     ecx, [ebp+0Ch]
   and     ecx, 1Fh
   mov     eax, [eax+edx]
   sar     eax, cl
   xor     al, 1
   and     al, 1
   test    al, al
   jz      loc_80B753C
   mov     eax, [ebp-24h]
   movsx   edx, word [eax+2]
   mov     eax, [ebp-30h]
   sub     eax, edx
   mov     [ebp-38h], eax
   cmp     dword [ebp-38h], 0
   js      loc_80B7521
   mov     eax, [ebp-38h]
   add     eax, [ebp-2Ch]
   movzx   edx, byte [eax]
   lea     eax, [ebp+0Ch]
   sub     [eax], edx
   jmp     loc_80B7494
loc_80B7521:
   mov     eax, [ebp+8]
   add     eax, [ebp-20h]
   add     eax, 30h
   movzx   eax, byte [eax+0Ch]
   mov     [ebp+0Ch], eax
   cmp     dword [ebp+0Ch], 0FFh
   jnz     loc_80B7563
   jmp     loc_80B759F
loc_80B753C:
   mov     eax, [ebp-28h]
   mov     [ebp-34h], eax
loc_80B7542:
   mov     eax, [ebp-34h]
   movzx   eax, byte [eax]
   dec     eax
   cmp     [ebp+0Ch], eax
   jnz     loc_80B755B
   mov     eax, [ebp-34h]
   inc     eax
   movzx   eax, byte [eax]
   dec     eax
   mov     [ebp+0Ch], eax
   jmp     loc_80B7563
loc_80B755B:
   lea     eax, [ebp-34h]
   add     dword [eax], 2
   jmp     loc_80B7542
loc_80B7563:
   lea     eax, [ebp-20h]
   dec     dword [eax]
   mov     eax, [ebp-20h]
   mov     edx, [ebp+eax*4-58h]
   mov     eax, [ebp+0Ch]
   sub     eax, edx
   mov     [ebp-30h], eax
   cmp     dword [ebp-30h], 0
   jns     loc_80B757F
   jmp     loc_80B7563
loc_80B757F:
   mov     edx, [ebp+8]
   mov     eax, [ebp-20h]
   mov     eax, [edx+eax*4+1Ch]
   mov     eax, [eax]
   mov     [ebp-24h], eax
   mov     eax, [ebp-24h]
   mov     eax, [eax+4]
   add     eax, 4
   mov     [ebp-2Ch], eax
   jmp     loc_80B74A3
loc_80B759F:
   add     esp, 6Ch
   pop     ebx
   pop     esi
   pop     edi
   pop     ebp
   retn    


section .rdata