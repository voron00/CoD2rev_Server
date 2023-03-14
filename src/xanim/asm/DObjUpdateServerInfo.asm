extern XAnimUpdateInfoInternal
extern XAnimFindServerNoteTrack


global DObjUpdateServerInfo


section .text


DObjUpdateServerInfo:
   push    ebp
   mov     ebp, esp
   sub     esp, 28h
   mov     eax, [ebp+8]
   cmp     dword [eax], 0
   jnz     loc_80BEB90
   mov     dword [ebp-0Ch], 0
   jmp     loc_80BEC6F
loc_80BEB90:
   cmp     dword [ebp+10h], 0
   jnz     loc_80BEBC6
   mov     dword [esp+0Ch], 0
   mov     eax, [ebp+0Ch]
   mov     [esp+8], eax
   mov     dword [esp+4], 0
   mov     eax, [ebp+8]
   mov     eax, [eax]
   mov     [esp], eax
   call    XAnimUpdateInfoInternal
   mov     dword [ebp-0Ch], 0
   jmp     loc_80BEC6F
loc_80BEBC6:
   mov     eax, [ebp+0Ch]
   mov     [esp+8], eax
   mov     dword [esp+4], 0
   mov     eax, [ebp+8]
   mov     eax, [eax]
   mov     [esp], eax
   call    XAnimFindServerNoteTrack
   fstp    dword [ebp-4]
   fld     dword [ebp-4]
   fld1    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80BEBF7
   jp      loc_80BEBF7
   jmp     loc_80BEC44
loc_80BEBF7:
   fld     dword [ebp+0Ch]
   fmul    dword [ebp-4]
   fld     dword [flt_81490C8]
   faddp   st1, st0
   fstp    dword [ebp-8]
   fld     dword [ebp-8]
   fld     dword [ebp+0Ch]
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_80BEC17
   jmp     loc_80BEC44
loc_80BEC17:
   mov     dword [esp+0Ch], 1
   mov     eax, [ebp-8]
   mov     [esp+8], eax
   mov     dword [esp+4], 0
   mov     eax, [ebp+8]
   mov     eax, [eax]
   mov     [esp], eax
   call    XAnimUpdateInfoInternal
   mov     dword [ebp-0Ch], 1
   jmp     loc_80BEC6F
loc_80BEC44:
   mov     dword [esp+0Ch], 1
   mov     eax, [ebp+0Ch]
   mov     [esp+8], eax
   mov     dword [esp+4], 0
   mov     eax, [ebp+8]
   mov     eax, [eax]
   mov     [esp], eax
   call    XAnimUpdateInfoInternal
   mov     dword [ebp-0Ch], 0
loc_80BEC6F:
   mov     eax, [ebp-0Ch]
   leave   
   retn    


section .rdata
flt_81490C8     dd 0.001