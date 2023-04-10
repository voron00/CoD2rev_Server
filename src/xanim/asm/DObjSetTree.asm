extern memset


global DObjSetTree


section .text


DObjSetTree:
   push    ebp
   mov     ebp, esp
   sub     esp, 28h
   mov     edx, [ebp+8]
   mov     eax, [ebp+0Ch]
   mov     [edx], eax
   cmp     dword [ebp+0Ch], 0
   jnz     loc_80B7D90
   mov     eax, [ebp+8]
   mov     dword [eax+0Ch], 0
   jmp     locret_80B7DF9
loc_80B7D90:
   mov     eax, [ebp+0Ch]
   mov     eax, [eax]
   mov     eax, [eax+4]
   mov     [ebp-8], eax
   mov     eax, [ebp-8]
   add     eax, eax
   add     eax, 8
   mov     [ebp-4], eax
   mov     edx, [ebp+8]
   mov     eax, [ebp-4]
   add     eax, [ebp+0Ch]
   mov     [edx+0Ch], eax
   mov     edx, [ebp+8]
   mov     eax, [ebp-8]
   add     eax, eax
   add     eax, [edx+0Ch]
   mov     [ebp-0Ch], eax
   mov     eax, [ebp-0Ch]
   movzx   eax, byte [eax]
   inc     al
   mov     [ebp-0Dh], al
   cmp     byte [ebp-0Dh], 0
   jnz     loc_80B7DF0
   mov     byte [ebp-0Dh], 1
   mov     eax, [ebp-8]
   mov     [esp+8], eax
   mov     dword [esp+4], 0
   mov     eax, [ebp-0Ch]
   inc     eax
   mov     [esp], eax
   call    memset
loc_80B7DF0:
   mov     edx, [ebp-0Ch]
   movzx   eax, byte [ebp-0Dh]
   mov     [edx], al
locret_80B7DF9:
   leave   
   retn    


section .rdata