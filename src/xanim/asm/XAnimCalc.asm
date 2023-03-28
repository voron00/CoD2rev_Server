extern g_xAnimInfo
extern Com_Printf
extern SL_ConvertToString
extern SL_RemoveRefToStringOfLen
extern SL_GetStringOfLen


global XAnimCalc
global XAnimGetRelDelta
global XAnimCalcDeltaTree
global XAnim_CalcDeltaForTime
global XAnimCalcDelta
global XAnimGetAbsDelta


section .text


XAnimGetAbsDelta:
   push    ebp
   mov     ebp, esp
   sub     esp, 48h
   mov     eax, [ebp+0Ch]
   shl     eax, 3
   add     eax, [ebp+8]
   add     eax, 0Ch
   mov     [ebp-2Ch], eax
   mov     eax, [ebp-2Ch]
   movzx   eax, word [eax]
   mov     [ebp-30h], eax
   cmp     dword [ebp-30h], 0
   jnz     loc_80BF289
   mov     eax, [ebp-2Ch]
   mov     eax, [eax+4]
   mov     [ebp-34h], eax
   mov     eax, [ebp-34h]
   cmp     byte [eax+3], 0
   jz      loc_80BF289
   lea     eax, [ebp-28h]
   mov     [esp], eax
   call    Vector2Clear651
   mov     eax, 0
   mov     [ebp-20h], eax
   lea     eax, [ebp-28h]
   add     eax, 0Ch
   mov     [esp], eax
   call    VectorClear875
   lea     eax, [ebp-28h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+18h]
   mov     [esp+8], eax
   mov     eax, 3F800000h
   mov     [esp+4], eax
   mov     eax, [ebp-34h]
   mov     [esp], eax
   call    XAnimCalcAbsDeltaParts
   fld     dword [ebp-28h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80BF247
   jp      loc_80BF247
   fld     dword [ebp-24h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80BF247
   jp      loc_80BF247
   jmp     loc_80BF25B
loc_80BF247:
   mov     eax, [ebp+10h]
   mov     [esp+4], eax
   lea     eax, [ebp-28h]
   mov     [esp], eax
   call    Vector2Copy713
   jmp     loc_80BF272
loc_80BF25B:
   mov     edx, [ebp+10h]
   mov     eax, 0
   mov     [edx], eax
   mov     edx, [ebp+10h]
   add     edx, 4
   mov     eax, 3F800000h
   mov     [edx], eax
loc_80BF272:
   mov     eax, [ebp+14h]
   mov     [esp+4], eax
   lea     eax, [ebp-28h]
   add     eax, 0Ch
   mov     [esp], eax
   call    VectorCopy925
   jmp     locret_80BF2AB
loc_80BF289:
   mov     edx, [ebp+10h]
   mov     eax, 0
   mov     [edx], eax
   mov     edx, [ebp+10h]
   add     edx, 4
   mov     eax, 3F800000h
   mov     [edx], eax
   mov     eax, [ebp+14h]
   mov     [esp], eax
   call    VectorClear875
locret_80BF2AB:
   leave   
   retn    


XAnimCalcDelta:
   push    ebp
   mov     ebp, esp
   sub     esp, 58h
   mov     eax, [ebp+18h]
   mov     [ebp-9], al
   mov     eax, [ebp+8]
   mov     byte [eax+6], 0
   mov     edx, [ebp+8]
   movzx   eax, byte [ebp-9]
   mov     [edx+7], al
   lea     eax, [ebp-38h]
   mov     [esp+14h], eax
   mov     dword [esp+10h], 0
   mov     dword [esp+0Ch], 1
   mov     eax, 3F800000h
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XAnimCalcDeltaTree
   fld     dword [ebp-38h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80BEF8D
   jp      loc_80BEF8D
   jmp     loc_80BEFB3
loc_80BEF8D:
   fld     dword [ebp-34h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80BEF9F
   jp      loc_80BEF9F
   jmp     loc_80BEFB3
loc_80BEF9F:
   mov     eax, [ebp+10h]
   mov     [esp+4], eax
   lea     eax, [ebp-38h]
   mov     [esp], eax
   call    Vector2Copy713
   jmp     loc_80BEFCA
loc_80BEFB3:
   mov     edx, [ebp+10h]
   mov     eax, 0
   mov     [edx], eax
   mov     edx, [ebp+10h]
   add     edx, 4
   mov     eax, 3F800000h
   mov     [edx], eax
loc_80BEFCA:
   mov     eax, [ebp+14h]
   mov     [esp+4], eax
   lea     eax, [ebp-38h]
   add     eax, 0Ch
   mov     [esp], eax
   call    VectorCopy925
   leave   
   retn    


sub_80C0FC2:
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
   pop     ebp
   retn    


sub_80C0FE8:
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
   pop     ebp
   retn    


XAnimCalcAbsDeltaParts:
   push    ebp
   mov     ebp, esp
   sub     esp, 38h
   lea     eax, [ebp-28h]
   mov     [esp+0Ch], eax
   lea     eax, [ebp-10h]
   mov     [esp+8], eax
   mov     eax, [ebp+10h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XAnim_CalcDeltaForTime
   mov     eax, [ebp+14h]
   mov     [esp+0Ch], eax
   lea     eax, [ebp-10h]
   mov     [esp+8], eax
   fld     dword [ebp+0Ch]
   fld     dword [flt_8148EC4]
   fmulp   st1, st0
   fstp    dword [esp+4]
   mov     eax, [ebp+14h]
   mov     [esp], eax
   call    sub_80C0FE8
   mov     edx, [ebp+14h]
   mov     eax, [ebp+14h]
   fld     dword [eax+8]
   fadd    dword [ebp+0Ch]
   fstp    dword [edx+8]
   mov     eax, [ebp+14h]
   add     eax, 0Ch
   mov     [esp+0Ch], eax
   lea     eax, [ebp-28h]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+14h]
   add     eax, 0Ch
   mov     [esp], eax
   call    VectorMA561
   leave   
   retn    


sub_80C101E:
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


XAnimCalcDeltaTree:
   push    ebp
   mov     ebp, esp
   sub     esp, 98h
   mov     eax, [ebp+14h]
   mov     edx, [ebp+18h]
   mov     [ebp-9], al
   mov     [ebp-0Ah], dl
   mov     edx, [ebp+8]
   mov     eax, [ebp+0Ch]
   shl     eax, 3
   add     eax, [edx]
   add     eax, 0Ch
   mov     [ebp-50h], eax
   mov     eax, [ebp-50h]
   movzx   eax, word [eax]
   mov     [ebp-20h], eax
   cmp     dword [ebp-20h], 0
   jnz     loc_80BE317
   cmp     byte [ebp-9], 0
   jz      loc_80BE257
   mov     eax, [ebp+1Ch]
   mov     [esp], eax
   call    Vector2Clear651
   mov     edx, [ebp+1Ch]
   mov     eax, 0
   mov     [edx+8], eax
   mov     eax, [ebp+1Ch]
   add     eax, 0Ch
   mov     [esp], eax
   call    VectorClear875
loc_80BE257:
   mov     eax, [ebp-50h]
   mov     eax, [eax+4]
   mov     [ebp-54h], eax
   mov     eax, [ebp-54h]
   cmp     byte [eax+3], 0
   jnz     loc_80BE26E
   jmp     locret_80BE75D
loc_80BE26E:
   mov     eax, [ebp+8]
   mov     edx, [ebp+0Ch]
   movzx   eax, word [eax+edx*2+8]
   mov     [ebp-4Ah], ax
   cmp     word [ebp-4Ah], 0
   jnz     loc_80BE289
   jmp     locret_80BE75D
loc_80BE289:
   movzx   edx, word [ebp-4Ah]
   mov     eax, edx
   shl     eax, 2
   add     eax, edx
   lea     edx, 0[eax*8]
   mov     eax, g_xAnimInfo
   add     eax, edx
   mov     [ebp-58h], eax
   mov     eax, [ebp-58h]
   add     eax, 0Ch
   mov     [ebp-5Ch], eax
   mov     eax, [ebp+8]
   cmp     byte [eax+6], 0
   jz      loc_80BE2DE
   mov     eax, [ebp+1Ch]
   mov     [esp+0Ch], eax
   mov     eax, [ebp-5Ch]
   mov     eax, [eax]
   mov     [esp+8], eax
   mov     eax, [ebp+10h]
   mov     [esp+4], eax
   mov     eax, [ebp-54h]
   mov     [esp], eax
   call    XAnimCalcAbsDeltaParts
   jmp     locret_80BE75D
loc_80BE2DE:
   mov     dword [esp+14h], 1
   mov     eax, [ebp+1Ch]
   mov     [esp+10h], eax
   mov     eax, [ebp-5Ch]
   mov     eax, [eax]
   mov     [esp+0Ch], eax
   mov     eax, [ebp-5Ch]
   mov     eax, [eax+4]
   mov     [esp+8], eax
   mov     eax, [ebp+10h]
   mov     [esp+4], eax
   mov     eax, [ebp-54h]
   mov     [esp], eax
   call    XAnimCalcRelDeltaParts
   jmp     locret_80BE75D
loc_80BE317:
   mov     dword [ebp-18h], 0
loc_80BE31E:
   mov     eax, [ebp-18h]
   cmp     eax, [ebp-20h]
   jl      loc_80BE32B
   jmp     loc_80BE733
loc_80BE32B:
   mov     edx, [ebp+8]
   mov     eax, [ebp-50h]
   movzx   eax, word [eax+6]
   add     eax, [ebp-18h]
   movzx   eax, word [edx+eax*2+8]
   mov     [ebp-4Ah], ax
   cmp     word [ebp-4Ah], 0
   jnz     loc_80BE34D
   jmp     loc_80BE729
loc_80BE34D:
   movzx   edx, word [ebp-4Ah]
   mov     eax, edx
   shl     eax, 2
   add     eax, edx
   shl     eax, 3
   add     eax, 8527A8Ch
   mov     [ebp-5Ch], eax
   mov     eax, [ebp+8]
   cmp     byte [eax+7], 0
   jnz     loc_80BE377
   mov     eax, [ebp-5Ch]
   fld     dword [eax+14h]
   fstp    dword [ebp-60h]
   jmp     loc_80BE380
loc_80BE377:
   mov     eax, [ebp-5Ch]
   fld     dword [eax+10h]
   fstp    dword [ebp-60h]
loc_80BE380:
   fld     dword [ebp-60h]
   fstp    dword [ebp-14h]
   fld     dword [ebp-14h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jp      loc_80BE39A
   jz      loc_80BE729
loc_80BE39A:
   mov     eax, [ebp-18h]
   inc     eax
   mov     [ebp-1Ch], eax
loc_80BE3A1:
   mov     eax, [ebp-1Ch]
   cmp     eax, [ebp-20h]
   jl      loc_80BE3AE
   jmp     loc_80BE6F0
loc_80BE3AE:
   mov     edx, [ebp+8]
   mov     eax, [ebp-50h]
   movzx   eax, word [eax+6]
   add     eax, [ebp-1Ch]
   movzx   eax, word [edx+eax*2+8]
   mov     [ebp-4Ah], ax
   cmp     word [ebp-4Ah], 0
   jnz     loc_80BE3D0
   jmp     loc_80BE6E6
loc_80BE3D0:
   movzx   edx, word [ebp-4Ah]
   mov     eax, edx
   shl     eax, 2
   add     eax, edx
   shl     eax, 3
   add     eax, 8527A8Ch
   mov     [ebp-5Ch], eax
   mov     eax, [ebp+8]
   cmp     byte [eax+7], 0
   jnz     loc_80BE3FA
   mov     eax, [ebp-5Ch]
   fld     dword [eax+14h]
   fstp    dword [ebp-64h]
   jmp     loc_80BE403
loc_80BE3FA:
   mov     eax, [ebp-5Ch]
   fld     dword [eax+10h]
   fstp    dword [ebp-64h]
loc_80BE403:
   fld     dword [ebp-64h]
   fstp    dword [ebp-10h]
   fld     dword [ebp-10h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jp      loc_80BE41D
   jz      loc_80BE6E6
loc_80BE41D:
   cmp     byte [ebp-9], 0
   jz      loc_80BE42B
   mov     eax, [ebp+1Ch]
   mov     [ebp-68h], eax
   jmp     loc_80BE431
loc_80BE42B:
   lea     eax, [ebp-48h]
   mov     [ebp-68h], eax
loc_80BE431:
   mov     eax, [ebp-68h]
   mov     [ebp-24h], eax
   mov     eax, [ebp-24h]
   mov     [esp+14h], eax
   mov     dword [esp+10h], 1
   mov     dword [esp+0Ch], 1
   mov     eax, [ebp-14h]
   mov     [esp+8], eax
   mov     eax, [ebp-50h]
   movzx   eax, word [eax+6]
   add     eax, [ebp-18h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XAnimCalcDeltaTree
   mov     eax, [ebp-24h]
   mov     [esp+14h], eax
   mov     dword [esp+10h], 1
   mov     dword [esp+0Ch], 0
   mov     eax, [ebp-10h]
   mov     [esp+8], eax
   mov     eax, [ebp-50h]
   movzx   eax, word [eax+6]
   add     eax, [ebp-1Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XAnimCalcDeltaTree
   lea     eax, [ebp-1Ch]
   inc     dword [eax]
loc_80BE4AA:
   mov     eax, [ebp-1Ch]
   cmp     eax, [ebp-20h]
   jl      loc_80BE4B7
   jmp     loc_80BE563
loc_80BE4B7:
   mov     edx, [ebp+8]
   mov     eax, [ebp-50h]
   movzx   eax, word [eax+6]
   add     eax, [ebp-1Ch]
   movzx   eax, word [edx+eax*2+8]
   mov     [ebp-4Ah], ax
   cmp     word [ebp-4Ah], 0
   jnz     loc_80BE4D9
   jmp     loc_80BE559
loc_80BE4D9:
   movzx   edx, word [ebp-4Ah]
   mov     eax, edx
   shl     eax, 2
   add     eax, edx
   shl     eax, 3
   add     eax, 8527A8Ch
   mov     [ebp-5Ch], eax
   mov     eax, [ebp+8]
   cmp     byte [eax+7], 0
   jnz     loc_80BE503
   mov     eax, [ebp-5Ch]
   fld     dword [eax+14h]
   fstp    dword [ebp-6Ch]
   jmp     loc_80BE50C
loc_80BE503:
   mov     eax, [ebp-5Ch]
   fld     dword [eax+10h]
   fstp    dword [ebp-6Ch]
loc_80BE50C:
   fld     dword [ebp-6Ch]
   fstp    dword [ebp-10h]
   fld     dword [ebp-10h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jp      loc_80BE522
   jz      loc_80BE559
loc_80BE522:
   mov     eax, [ebp-24h]
   mov     [esp+14h], eax
   mov     dword [esp+10h], 1
   mov     dword [esp+0Ch], 0
   mov     eax, [ebp-10h]
   mov     [esp+8], eax
   mov     eax, [ebp-50h]
   movzx   eax, word [eax+6]
   add     eax, [ebp-1Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XAnimCalcDeltaTree
loc_80BE559:
   lea     eax, [ebp-1Ch]
   inc     dword [eax]
   jmp     loc_80BE4AA
loc_80BE563:
   cmp     byte [ebp-0Ah], 0
   jnz     loc_80BE5AA
   mov     eax, [ebp+1Ch]
   fld     dword [eax+8]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80BE581
   jp      loc_80BE581
   jmp     locret_80BE75D
loc_80BE581:
   mov     eax, [ebp+1Ch]
   add     eax, 0Ch
   mov     [esp+8], eax
   mov     eax, [ebp+1Ch]
   fld1    
   fdiv    dword [eax+8]
   fstp    dword [esp+4]
   mov     eax, [ebp+1Ch]
   add     eax, 0Ch
   mov     [esp], eax
   call    VectorScale519
   jmp     locret_80BE75D
loc_80BE5AA:
   cmp     byte [ebp-9], 0
   jz      loc_80BE643
   mov     eax, [ebp+1Ch]
   mov     [esp], eax
   call    sub_80C101E
   fstp    dword [ebp-28h]
   fld     dword [ebp-28h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80BE5D4
   jp      loc_80BE5D4
   jmp     loc_80BE5F8
loc_80BE5D4:
   mov     eax, [ebp-28h]
   mov     [esp], eax
   call    XAnimCalc_GetLowestElement
   fmul    dword [ebp+10h]
   mov     eax, [ebp+1Ch]
   mov     [esp+8], eax
   fstp    dword [esp+4]
   mov     eax, [ebp+1Ch]
   mov     [esp], eax
   call    sub_80C0FC2
loc_80BE5F8:
   mov     eax, [ebp+1Ch]
   fld     dword [eax+8]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80BE610
   jp      loc_80BE610
   jmp     locret_80BE75D
loc_80BE610:
   mov     eax, [ebp+1Ch]
   add     eax, 0Ch
   mov     [esp+8], eax
   mov     eax, [ebp+1Ch]
   fld     dword [ebp+10h]
   fdiv    dword [eax+8]
   fstp    dword [esp+4]
   mov     eax, [ebp+1Ch]
   add     eax, 0Ch
   mov     [esp], eax
   call    VectorScale519
   mov     edx, [ebp+1Ch]
   mov     eax, [ebp+10h]
   mov     [edx+8], eax
   jmp     locret_80BE75D
loc_80BE643:
   mov     eax, [ebp-24h]
   mov     [esp], eax
   call    sub_80C101E
   fstp    dword [ebp-28h]
   fld     dword [ebp-28h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80BE663
   jp      loc_80BE663
   jmp     loc_80BE68E
loc_80BE663:
   mov     eax, [ebp-28h]
   mov     [esp], eax
   call    XAnimCalc_GetLowestElement
   fmul    dword [ebp+10h]
   mov     eax, [ebp+1Ch]
   mov     [esp+0Ch], eax
   mov     eax, [ebp-24h]
   mov     [esp+8], eax
   fstp    dword [esp+4]
   mov     eax, [ebp+1Ch]
   mov     [esp], eax
   call    sub_80C0FE8
loc_80BE68E:
   mov     eax, [ebp-24h]
   fld     dword [eax+8]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80BE6A6
   jp      loc_80BE6A6
   jmp     locret_80BE75D
loc_80BE6A6:
   mov     eax, [ebp+1Ch]
   add     eax, 0Ch
   mov     [esp+0Ch], eax
   mov     eax, [ebp-24h]
   add     eax, 0Ch
   mov     [esp+8], eax
   mov     eax, [ebp-24h]
   fld     dword [ebp+10h]
   fdiv    dword [eax+8]
   fstp    dword [esp+4]
   mov     eax, [ebp+1Ch]
   add     eax, 0Ch
   mov     [esp], eax
   call    VectorMA561
   mov     edx, [ebp+1Ch]
   mov     eax, [ebp+1Ch]
   fld     dword [eax+8]
   fadd    dword [ebp+10h]
   fstp    dword [edx+8]
   jmp     locret_80BE75D
loc_80BE6E6:
   lea     eax, [ebp-1Ch]
   inc     dword [eax]
   jmp     loc_80BE3A1
loc_80BE6F0:
   mov     eax, [ebp+1Ch]
   mov     [esp+14h], eax
   movzx   eax, byte [ebp-0Ah]
   mov     [esp+10h], eax
   movzx   eax, byte [ebp-9]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+10h]
   mov     [esp+8], eax
   mov     eax, [ebp-50h]
   movzx   eax, word [eax+6]
   add     eax, [ebp-18h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XAnimCalcDeltaTree
   jmp     locret_80BE75D
loc_80BE729:
   lea     eax, [ebp-18h]
   inc     dword [eax]
   jmp     loc_80BE31E
loc_80BE733:
   cmp     byte [ebp-9], 0
   jz      locret_80BE75D
   mov     eax, [ebp+1Ch]
   mov     [esp], eax
   call    Vector2Clear651
   mov     edx, [ebp+1Ch]
   mov     eax, 0
   mov     [edx+8], eax
   mov     eax, [ebp+1Ch]
   add     eax, 0Ch
   mov     [esp], eax
   call    VectorClear875
locret_80BE75D:
   leave   
   retn    


XAnimCalcAbsDelta:
   push    ebp
   mov     ebp, esp
   sub     esp, 48h
   mov     eax, [ebp+8]
   mov     byte [eax+6], 1
   mov     eax, [ebp+8]
   mov     byte [eax+7], 1
   lea     eax, [ebp-28h]
   mov     [esp+14h], eax
   mov     dword [esp+10h], 0
   mov     dword [esp+0Ch], 1
   mov     eax, 3F800000h
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XAnimCalcDeltaTree
   fld     dword [ebp-28h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80BF04A
   jp      loc_80BF04A
   fld     dword [ebp-24h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80BF04A
   jp      loc_80BF04A
   jmp     loc_80BF05E
loc_80BF04A:
   mov     eax, [ebp+10h]
   mov     [esp+4], eax
   lea     eax, [ebp-28h]
   mov     [esp], eax
   call    Vector2Copy713
   jmp     loc_80BF075
loc_80BF05E:
   mov     edx, [ebp+10h]
   mov     eax, 0
   mov     [edx], eax
   mov     edx, [ebp+10h]
   add     edx, 4
   mov     eax, 3F800000h
   mov     [edx], eax
loc_80BF075:
   mov     eax, [ebp+14h]
   mov     [esp+4], eax
   lea     eax, [ebp-28h]
   add     eax, 0Ch
   mov     [esp], eax
   call    VectorCopy925
   leave   
   retn    


sub_80BB77C:
   push    ebp
   mov     ebp, esp
   sub     esp, 10h
   mov     eax, [ebp+8]
   mov     edx, [ebp+8]
   fld     dword [eax]
   fmul    dword [edx]
   fstp    dword [ebp-4]
   mov     eax, [ebp+8]
   add     eax, 4
   mov     edx, [ebp+8]
   add     edx, 4
   fld     dword [eax]
   fmul    dword [edx]
   fld     dword [ebp-4]
   faddp   st1, st0
   fstp    dword [ebp-0Ch]
   fld     dword [ebp-0Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jp      loc_80BB7B7
   jz      locret_80BB828
loc_80BB7B7:
   fld     dword [flt_8148EBC]
   fdiv    dword [ebp-0Ch]
   fstp    dword [ebp-0Ch]
   fld     dword [ebp-4]
   fmul    dword [ebp-0Ch]
   fstp    dword [ebp-4]
   mov     eax, [ebp+8]
   mov     edx, [ebp+8]
   add     edx, 4
   fld     dword [eax]
   fmul    dword [edx]
   fmul    dword [ebp-0Ch]
   fstp    dword [ebp-8]
   fld1    
   fsub    dword [ebp-4]
   mov     eax, [ebp+0Ch]
   fmul    dword [eax]
   mov     eax, [ebp+0Ch]
   add     eax, 4
   fld     dword [ebp-8]
   fmul    dword [eax]
   faddp   st1, st0
   fstp    dword [ebp-10h]
   mov     ecx, [ebp+0Ch]
   add     ecx, 4
   mov     edx, [ebp+0Ch]
   add     edx, 4
   mov     eax, [ebp+0Ch]
   fld     dword [ebp-8]
   fmul    dword [eax]
   mov     eax, [ebp+0Ch]
   add     eax, 4
   fld     dword [ebp-4]
   fmul    dword [eax]
   faddp   st1, st0
   fld     dword [edx]
   fsubrp  st1, st0
   fstp    dword [ecx]
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp-10h]
   mov     [edx], eax
locret_80BB828:
   leave   
   retn    


sub_80C10DE:
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


sub_80C109A:
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


XAnim_CalcDynamicIndexForTrans_s:
   push    ebp
   mov     ebp, esp
   sub     esp, 48h
   mov     eax, [ebp+8]
   cmp     word [eax], 0
   jnz     loc_80BB5B9
   mov     eax, [ebp+14h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   add     eax, 4
   mov     [esp], eax
   call    VectorCopy925
   jmp     locret_80BB640
loc_80BB5B9:
   lea     eax, [ebp-18h]
   mov     [esp+8], eax
   mov     eax, [ebp+10h]
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    XAnim_SetTime
   lea     eax, [ebp-20h]
   mov     [esp+10h], eax
   lea     eax, [ebp-1Ch]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+8]
   movzx   eax, word [eax]
   mov     [esp+8], eax
   mov     eax, [ebp+8]
   add     eax, 8
   mov     [esp+4], eax
   lea     eax, [ebp-18h]
   mov     [esp], eax
   call    XAnimCalcDynamicFrameIndices_s
   mov     eax, [ebp+14h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp-20h]
   mov     [esp+8], eax
   mov     ecx, [ebp+8]
   mov     edx, [ebp-1Ch]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   add     eax, [ecx+4]
   add     eax, 0Ch
   mov     [esp+4], eax
   mov     ecx, [ebp+8]
   mov     edx, [ebp-1Ch]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   add     eax, [ecx+4]
   mov     [esp], eax
   call    XAnimSetDeltaTransFrames
locret_80BB640:
   leave   
   retn    


XAnim_CalcDynamicIndicesForTrans_s:
   push    ebp
   mov     ebp, esp
   sub     esp, 18h
   mov     eax, [ebp+8]
   cmp     dword [eax], 0
   jz      loc_80BB674
   mov     eax, [ebp+14h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+10h]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     eax, [eax]
   mov     [esp], eax
   call    XAnim_CalcDynamicIndexForTrans_s
   jmp     locret_80BB67F
loc_80BB674:
   mov     eax, [ebp+14h]
   mov     [esp], eax
   call    VectorClear875
locret_80BB67F:
   leave   
   retn    


XAnim_CalcDynamicIndexForQuat_s:
   push    ebp
   mov     ebp, esp
   sub     esp, 48h
   mov     eax, [ebp+8]
   cmp     word [eax], 0
   jnz     loc_80BB4C4
   mov     eax, [ebp+14h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   add     eax, 4
   mov     [esp], eax
   call    XAnimSetDeltaQuatFrames
   jmp     locret_80BB53F
loc_80BB4C4:
   lea     eax, [ebp-18h]
   mov     [esp+8], eax
   mov     eax, [ebp+10h]
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    XAnim_SetTime
   lea     eax, [ebp-20h]
   mov     [esp+10h], eax
   lea     eax, [ebp-1Ch]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+8]
   movzx   eax, word [eax]
   mov     [esp+8], eax
   mov     eax, [ebp+8]
   add     eax, 8
   mov     [esp+4], eax
   lea     eax, [ebp-18h]
   mov     [esp], eax
   call    XAnimCalcDynamicFrameIndices_s
   mov     eax, [ebp+14h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp-20h]
   mov     [esp+8], eax
   mov     edx, [ebp+8]
   mov     eax, [ebp-1Ch]
   shl     eax, 2
   add     eax, [edx+4]
   add     eax, 4
   mov     [esp+4], eax
   mov     edx, [ebp+8]
   mov     eax, [ebp-1Ch]
   shl     eax, 2
   add     eax, [edx+4]
   mov     [esp], eax
   call    XAnimSetRotDeltaFramesForQuat
locret_80BB53F:
   leave   
   retn    


XAnim_CalcDynamicIndicesForQuats_s:
   push    ebp
   mov     ebp, esp
   sub     esp, 18h
   mov     eax, [ebp+8]
   cmp     dword [eax+4], 0
   jz      loc_80BB576
   mov     eax, [ebp+14h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+10h]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     eax, [eax+4]
   mov     [esp], eax
   call    XAnim_CalcDynamicIndexForQuat_s
   jmp     locret_80BB58D
loc_80BB576:
   mov     edx, [ebp+14h]
   mov     eax, 0
   mov     [edx], eax
   mov     edx, [ebp+14h]
   add     edx, 4
   mov     eax, 46FFFE00h
   mov     [edx], eax
locret_80BB58D:
   leave   
   retn    


XAnimSetDeltaTransFrames:
   push    ebp
   mov     ebp, esp
   push    ebx
   mov     ebx, [ebp+14h]
   mov     ecx, [ebp+8]
   mov     eax, [ebp+0Ch]
   mov     edx, [ebp+8]
   fld     dword [eax]
   fsub    dword [edx]
   fmul    dword [ebp+10h]
   fld     dword [ecx]
   faddp   st1, st0
   fstp    dword [ebx]
   mov     ebx, [ebp+14h]
   add     ebx, 4
   mov     ecx, [ebp+8]
   add     ecx, 4
   mov     eax, [ebp+0Ch]
   add     eax, 4
   mov     edx, [ebp+8]
   add     edx, 4
   fld     dword [eax]
   fsub    dword [edx]
   fmul    dword [ebp+10h]
   fld     dword [ecx]
   faddp   st1, st0
   fstp    dword [ebx]
   mov     ebx, [ebp+14h]
   add     ebx, 8
   mov     ecx, [ebp+8]
   add     ecx, 8
   mov     eax, [ebp+0Ch]
   add     eax, 8
   mov     edx, [ebp+8]
   add     edx, 8
   fld     dword [eax]
   fsub    dword [edx]
   fmul    dword [ebp+10h]
   fld     dword [ecx]
   faddp   st1, st0
   fstp    dword [ebx]
   pop     ebx
   pop     ebp
   retn    


XAnim_CalcDynamicIndexForTrans_b:
   push    ebp
   mov     ebp, esp
   sub     esp, 48h
   mov     eax, [ebp+8]
   cmp     word [eax], 0
   jnz     loc_80BB1D5
   mov     eax, [ebp+14h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   add     eax, 4
   mov     [esp], eax
   call    VectorCopy925
   jmp     locret_80BB25C
loc_80BB1D5:
   lea     eax, [ebp-18h]
   mov     [esp+8], eax
   mov     eax, [ebp+10h]
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    XAnim_SetTime
   lea     eax, [ebp-20h]
   mov     [esp+10h], eax
   lea     eax, [ebp-1Ch]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+8]
   movzx   eax, word [eax]
   mov     [esp+8], eax
   mov     eax, [ebp+8]
   add     eax, 8
   mov     [esp+4], eax
   lea     eax, [ebp-18h]
   mov     [esp], eax
   call    XAnimCalcDynamicFrameIndices_b
   mov     eax, [ebp+14h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp-20h]
   mov     [esp+8], eax
   mov     ecx, [ebp+8]
   mov     edx, [ebp-1Ch]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   add     eax, [ecx+4]
   add     eax, 0Ch
   mov     [esp+4], eax
   mov     ecx, [ebp+8]
   mov     edx, [ebp-1Ch]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   add     eax, [ecx+4]
   mov     [esp], eax
   call    XAnimSetDeltaTransFrames
locret_80BB25C:
   leave   
   retn    


XAnim_CalcDynamicIndicesForTrans_b:
   push    ebp
   mov     ebp, esp
   sub     esp, 18h
   mov     eax, [ebp+8]
   cmp     dword [eax], 0
   jz      loc_80BB290
   mov     eax, [ebp+14h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+10h]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     eax, [eax]
   mov     [esp], eax
   call    XAnim_CalcDynamicIndexForTrans_b
   jmp     locret_80BB29B
loc_80BB290:
   mov     eax, [ebp+14h]
   mov     [esp], eax
   call    VectorClear875
locret_80BB29B:
   leave   
   retn    


XAnimSetRotDeltaFramesForQuat:
   push    ebp
   mov     ebp, esp
   sub     esp, 18h
   mov     eax, [ebp+10h]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   movsx   eax, word [eax]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   movsx   eax, word [eax]
   mov     [esp], eax
   call    XAnimGetFrameTime
   mov     eax, [ebp+14h]
   fstp    dword [eax]
   mov     eax, [ebp+10h]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   add     eax, 2
   movsx   eax, word [eax]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   add     eax, 2
   movsx   eax, word [eax]
   mov     [esp], eax
   call    XAnimGetFrameTime
   mov     eax, [ebp+14h]
   add     eax, 4
   fstp    dword [eax]
   leave   
   retn    


XAnim_CalcDynamicIndexForQuat_b:
   push    ebp
   mov     ebp, esp
   sub     esp, 48h
   mov     eax, [ebp+8]
   cmp     word [eax], 0
   jnz     loc_80BB0E0
   mov     eax, [ebp+14h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   add     eax, 4
   mov     [esp], eax
   call    XAnimSetDeltaQuatFrames
   jmp     locret_80BB15B
loc_80BB0E0:
   lea     eax, [ebp-18h]
   mov     [esp+8], eax
   mov     eax, [ebp+10h]
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    XAnim_SetTime
   lea     eax, [ebp-20h]
   mov     [esp+10h], eax
   lea     eax, [ebp-1Ch]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+8]
   movzx   eax, word [eax]
   mov     [esp+8], eax
   mov     eax, [ebp+8]
   add     eax, 8
   mov     [esp+4], eax
   lea     eax, [ebp-18h]
   mov     [esp], eax
   call    XAnimCalcDynamicFrameIndices_b
   mov     eax, [ebp+14h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp-20h]
   mov     [esp+8], eax
   mov     edx, [ebp+8]
   mov     eax, [ebp-1Ch]
   shl     eax, 2
   add     eax, [edx+4]
   add     eax, 4
   mov     [esp+4], eax
   mov     edx, [ebp+8]
   mov     eax, [ebp-1Ch]
   shl     eax, 2
   add     eax, [edx+4]
   mov     [esp], eax
   call    XAnimSetRotDeltaFramesForQuat
locret_80BB15B:
   leave   
   retn    


XAnim_CalcDynamicIndicesForQuats_b:
   push    ebp
   mov     ebp, esp
   sub     esp, 18h
   mov     eax, [ebp+8]
   cmp     dword [eax+4], 0
   jz      loc_80BB192
   mov     eax, [ebp+14h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+10h]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     eax, [eax+4]
   mov     [esp], eax
   call    XAnim_CalcDynamicIndexForQuat_b
   jmp     locret_80BB1A9
loc_80BB192:
   mov     edx, [ebp+14h]
   mov     eax, 0
   mov     [edx], eax
   mov     edx, [ebp+14h]
   add     edx, 4
   mov     eax, 46FFFE00h
   mov     [edx], eax
locret_80BB1A9:
   leave   
   retn    


XAnim_CalcPosDeltaTrans:
   push    ebp
   mov     ebp, esp
   sub     esp, 8
   mov     eax, [ebp+8]
   cmp     word [eax], 0
   jnz     loc_80BAE94
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   add     eax, 4
   mov     [esp], eax
   call    VectorCopy925
   jmp     locret_80BAEB8
loc_80BAE94:
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     ecx, [ebp+8]
   mov     eax, [ebp+8]
   movzx   edx, word [eax]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   add     eax, [ecx+4]
   mov     [esp], eax
   call    VectorCopy925
locret_80BAEB8:
   leave   
   retn    


XAnim_CalcPosDeltaForTrans:
   push    ebp
   mov     ebp, esp
   sub     esp, 8
   mov     eax, [ebp+8]
   cmp     dword [eax], 0
   jz      loc_80BAEDE
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     eax, [eax]
   mov     [esp], eax
   call    XAnim_CalcPosDeltaTrans
   jmp     locret_80BAEE9
loc_80BAEDE:
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    VectorClear875
locret_80BAEE9:
   leave   
   retn    


XAnimSetDeltaQuatFrames:
   push    ebp
   mov     ebp, esp
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp+8]
   fild    word [eax]
   fstp    dword [edx]
   mov     edx, [ebp+0Ch]
   add     edx, 4
   mov     eax, [ebp+8]
   add     eax, 2
   fild    word [eax]
   fstp    dword [edx]
   pop     ebp
   retn    


XAnim_CalcRotDeltaQuat:
   push    ebp
   mov     ebp, esp
   sub     esp, 18h
   mov     eax, [ebp+8]
   cmp     word [eax], 0
   jnz     loc_80BAE00
   mov     eax, [ebp+8]
   add     eax, 4
   mov     [ebp-8], eax
   jmp     loc_80BAE14
loc_80BAE00:
   mov     edx, [ebp+8]
   mov     eax, [ebp+8]
   movzx   eax, word [eax]
   shl     eax, 2
   mov     edx, [edx+4]
   add     edx, eax
   mov     [ebp-8], edx
loc_80BAE14:
   mov     eax, [ebp-8]
   mov     [ebp-4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp-4]
   mov     [esp], eax
   call    XAnimSetDeltaQuatFrames
   leave   
   retn    


XAnim_CalcRotDeltaForQuats:
   push    ebp
   mov     ebp, esp
   sub     esp, 8
   mov     eax, [ebp+8]
   cmp     dword [eax+4], 0
   jz      loc_80BAE54
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     eax, [eax+4]
   mov     [esp], eax
   call    XAnim_CalcRotDeltaQuat
   jmp     locret_80BAE6B
loc_80BAE54:
   mov     edx, [ebp+0Ch]
   mov     eax, 0
   mov     [edx], eax
   mov     edx, [ebp+0Ch]
   add     edx, 4
   mov     eax, 46FFFE00h
   mov     [edx], eax
locret_80BAE6B:
   leave   
   retn    


XAnim_CalcDeltaForTime:
   push    ebp
   mov     ebp, esp
   sub     esp, 28h
   mov     eax, [ebp+8]
   mov     eax, [eax+20h]
   mov     [ebp-0Ch], eax
   mov     eax, [ebp+8]
   movzx   eax, word [eax]
   mov     [ebp-4], eax
   mov     byte [ebp-0Dh], 0
   fld     dword [ebp+0Ch]
   fld1    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jp      loc_80BB6AE
   jz      loc_80BB6B4
loc_80BB6AE:
   cmp     dword [ebp-4], 0
   jnz     loc_80BB6B8
loc_80BB6B4:
   mov     byte [ebp-0Dh], 1
loc_80BB6B8:
   movzx   eax, byte [ebp-0Dh]
   mov     [ebp-5], al
   cmp     byte [ebp-5], 0
   jz      loc_80BB6EE
   mov     eax, [ebp+10h]
   mov     [esp+4], eax
   mov     eax, [ebp-0Ch]
   mov     [esp], eax
   call    XAnim_CalcRotDeltaForQuats
   mov     eax, [ebp+14h]
   mov     [esp+4], eax
   mov     eax, [ebp-0Ch]
   mov     [esp], eax
   call    XAnim_CalcPosDeltaForTrans
   jmp     locret_80BB779
loc_80BB6EE:
   cmp     dword [ebp-4], 0FFh
   jg      loc_80BB739
   mov     eax, [ebp+10h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp-4]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp-0Ch]
   mov     [esp], eax
   call    XAnim_CalcDynamicIndicesForQuats_b
   mov     eax, [ebp+14h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp-4]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp-0Ch]
   mov     [esp], eax
   call    XAnim_CalcDynamicIndicesForTrans_b
   jmp     locret_80BB779
loc_80BB739:
   mov     eax, [ebp+10h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp-4]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp-0Ch]
   mov     [esp], eax
   call    XAnim_CalcDynamicIndicesForQuats_s
   mov     eax, [ebp+14h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp-4]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp-0Ch]
   mov     [esp], eax
   call    XAnim_CalcDynamicIndicesForTrans_s
locret_80BB779:
   leave   
   retn    


XAnimCalcRelDeltaParts:
   push    ebp
   mov     ebp, esp
   sub     esp, 68h
   lea     eax, [ebp-28h]
   mov     [esp+0Ch], eax
   lea     eax, [ebp-18h]
   mov     [esp+8], eax
   mov     eax, [ebp+10h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XAnim_CalcDeltaForTime
   lea     eax, [ebp-38h]
   mov     [esp+0Ch], eax
   lea     eax, [ebp-18h]
   add     eax, 8
   mov     [esp+8], eax
   mov     eax, [ebp+14h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XAnim_CalcDeltaForTime
   mov     eax, [ebp+8]
   cmp     byte [eax+2], 0
   jz      loc_80BB8F2
   fld     dword [ebp+14h]
   fld     dword [ebp+10h]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80BB88B
   jmp     loc_80BB8F2
loc_80BB88B:
   mov     eax, [ebp+8]
   mov     eax, [eax+20h]
   mov     [ebp-50h], eax
   mov     eax, [ebp-50h]
   mov     eax, [eax]
   mov     [ebp-54h], eax
   cmp     dword [ebp-54h], 0
   jz      loc_80BB8F2
   mov     eax, [ebp-54h]
   cmp     word [eax], 0
   jz      loc_80BB8F2
   lea     eax, [ebp-38h]
   mov     [esp+8], eax
   mov     ecx, [ebp-54h]
   mov     eax, [ebp-54h]
   movzx   edx, word [eax]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   add     eax, [ecx+4]
   mov     [esp+4], eax
   lea     eax, [ebp-38h]
   mov     [esp], eax
   call    sub_80C109A
   lea     eax, [ebp-38h]
   mov     [esp+8], eax
   mov     eax, [ebp-54h]
   mov     eax, [eax+4]
   mov     [esp+4], eax
   lea     eax, [ebp-38h]
   mov     [esp], eax
   call    sub_80C10DE
loc_80BB8F2:
   fld     dword [ebp+0Ch]
   fld     dword [flt_8148EC0]
   fmulp   st1, st0
   fstp    dword [ebp-4Ch]
   mov     eax, [ebp+18h]
   mov     edx, [ebp+18h]
   fld     dword [ebp-10h]
   fmul    dword [ebp-14h]
   fld     dword [ebp-0Ch]
   fmul    dword [ebp-18h]
   fsubp   st1, st0
   fmul    dword [ebp-4Ch]
   fld     dword [edx]
   faddp   st1, st0
   fstp    dword [eax]
   mov     eax, [ebp+18h]
   mov     edx, [ebp+18h]
   fld     dword [ebp-10h]
   fmul    dword [ebp-18h]
   fld     dword [ebp-0Ch]
   fmul    dword [ebp-14h]
   faddp   st1, st0
   fmul    dword [ebp-4Ch]
   fld     dword [edx+4]
   faddp   st1, st0
   fstp    dword [eax+4]
   lea     eax, [ebp-48h]
   mov     [esp+8], eax
   lea     eax, [ebp-28h]
   mov     [esp+4], eax
   lea     eax, [ebp-38h]
   mov     [esp], eax
   call    sub_80C10DE
   lea     eax, [ebp-48h]
   mov     [esp+4], eax
   lea     edx, [ebp-18h]
   mov     eax, [ebp+1Ch]
   shl     eax, 3
   lea     eax, [eax+edx]
   mov     [esp], eax
   call    sub_80BB77C
   mov     edx, [ebp+18h]
   mov     eax, [ebp+18h]
   fld     dword [eax+8]
   fadd    dword [ebp+0Ch]
   fstp    dword [edx+8]
   mov     eax, [ebp+18h]
   add     eax, 0Ch
   mov     [esp+0Ch], eax
   lea     eax, [ebp-48h]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+18h]
   add     eax, 0Ch
   mov     [esp], eax
   call    VectorMA561
   leave   
   retn    


VectorCopy925:
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


Vector2Copy713:
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


Vector2Clear651:
   push    ebp
   mov     ebp, esp
   mov     edx, [ebp+8]
   mov     eax, 0
   mov     [edx], eax
   mov     edx, [ebp+8]
   add     edx, 4
   mov     eax, 0
   mov     [edx], eax
   pop     ebp
   retn    


XAnimGetRelDelta:
   push    ebp
   mov     ebp, esp
   sub     esp, 58h
   mov     eax, [ebp+0Ch]
   shl     eax, 3
   add     eax, [ebp+8]
   add     eax, 0Ch
   mov     [ebp-2Ch], eax
   mov     eax, [ebp-2Ch]
   movzx   eax, word [eax]
   mov     [ebp-30h], eax
   cmp     dword [ebp-30h], 0
   jnz     loc_80BF180
   mov     eax, [ebp-2Ch]
   mov     eax, [eax+4]
   mov     [ebp-34h], eax
   mov     eax, [ebp-34h]
   cmp     byte [eax+3], 0
   jz      loc_80BF180
   lea     eax, [ebp-28h]
   mov     [esp], eax
   call    Vector2Clear651
   mov     eax, 0
   mov     [ebp-20h], eax
   lea     eax, [ebp-28h]
   add     eax, 0Ch
   mov     [esp], eax
   call    VectorClear875
   mov     dword [esp+14h], 0
   lea     eax, [ebp-28h]
   mov     [esp+10h], eax
   mov     eax, [ebp+1Ch]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+18h]
   mov     [esp+8], eax
   mov     eax, 3F800000h
   mov     [esp+4], eax
   mov     eax, [ebp-34h]
   mov     [esp], eax
   call    XAnimCalcRelDeltaParts
   fld     dword [ebp-28h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80BF13E
   jp      loc_80BF13E
   fld     dword [ebp-24h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80BF13E
   jp      loc_80BF13E
   jmp     loc_80BF152
loc_80BF13E:
   mov     eax, [ebp+10h]
   mov     [esp+4], eax
   lea     eax, [ebp-28h]
   mov     [esp], eax
   call    Vector2Copy713
   jmp     loc_80BF169
loc_80BF152:
   mov     edx, [ebp+10h]
   mov     eax, 0
   mov     [edx], eax
   mov     edx, [ebp+10h]
   add     edx, 4
   mov     eax, 3F800000h
   mov     [edx], eax
loc_80BF169:
   mov     eax, [ebp+14h]
   mov     [esp+4], eax
   lea     eax, [ebp-28h]
   add     eax, 0Ch
   mov     [esp], eax
   call    VectorCopy925
   jmp     locret_80BF1A2
loc_80BF180:
   mov     edx, [ebp+10h]
   mov     eax, 0
   mov     [edx], eax
   mov     edx, [ebp+10h]
   add     edx, 4
   mov     eax, 3F800000h
   mov     [edx], eax
   mov     eax, [ebp+14h]
   mov     [esp], eax
   call    VectorClear875
locret_80BF1A2:
   leave   
   retn    


sub_80C0CE0:
   push    ebp
   mov     ebp, esp
   sub     esp, 28h
   mov     eax, [ebp+0Ch]
   cmp     word [eax], 0
   jnz     loc_80C0D14
   mov     eax, [ebp+14h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+0Ch]
   add     eax, 4
   mov     [esp+8], eax
   mov     eax, [ebp+10h]
   mov     [esp+4], eax
   mov     eax, [ebp+14h]
   mov     [esp], eax
   call    VectorMA561
   jmp     locret_80C0D90
loc_80C0D14:
   lea     eax, [ebp-8]
   mov     [esp+10h], eax
   lea     eax, [ebp-4]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+0Ch]
   movzx   eax, word [eax]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   add     eax, 8
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XAnimCalcDynamicFrameIndices_s
   mov     eax, [ebp+14h]
   mov     [esp+14h], eax
   mov     eax, [ebp-8]
   mov     [esp+10h], eax
   mov     ecx, [ebp+0Ch]
   mov     edx, [ebp-4]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   add     eax, [ecx+4]
   add     eax, 0Ch
   mov     [esp+0Ch], eax
   mov     ecx, [ebp+0Ch]
   mov     edx, [ebp-4]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   add     eax, [ecx+4]
   mov     [esp+8], eax
   mov     eax, [ebp+10h]
   mov     [esp+4], eax
   mov     eax, [ebp+14h]
   mov     [esp], eax
   call    sub_80C1608
locret_80C0D90:
   leave   
   retn    


sub_80C0C3A:
   push    ebp
   mov     ebp, esp
   sub     esp, 28h
   mov     eax, [ebp+0Ch]
   cmp     word [eax], 0
   jnz     loc_80C0C6E
   mov     eax, [ebp+14h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+0Ch]
   add     eax, 4
   mov     [esp+8], eax
   mov     eax, [ebp+10h]
   mov     [esp+4], eax
   mov     eax, [ebp+14h]
   mov     [esp], eax
   call    sub_80C1544
   jmp     locret_80C0CDE
loc_80C0C6E:
   lea     eax, [ebp-8]
   mov     [esp+10h], eax
   lea     eax, [ebp-4]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+0Ch]
   movzx   eax, word [eax]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   add     eax, 8
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XAnimCalcDynamicFrameIndices_s
   mov     eax, [ebp+14h]
   mov     [esp+14h], eax
   mov     eax, [ebp-8]
   mov     [esp+10h], eax
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp-4]
   shl     eax, 3
   add     eax, [edx+4]
   add     eax, 8
   mov     [esp+0Ch], eax
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp-4]
   shl     eax, 3
   add     eax, [edx+4]
   mov     [esp+8], eax
   mov     eax, [ebp+10h]
   mov     [esp+4], eax
   mov     eax, [ebp+14h]
   mov     [esp], eax
   call    sub_80C1422
locret_80C0CDE:
   leave   
   retn    


XAnim_GetTimeIndex:
   push    ebp
   mov     ebp, esp
   push    ebx
   sub     esp, 14h
   mov     eax, [ebp+8]
   fild    dword [ebp+10h]
   fmul    dword [eax]
   fnstcw  word [ebp-16h]
   movzx   eax, word [ebp-16h]
   or      ax, 0C00h
   mov     [ebp-18h], ax
   fldcw   word [ebp-18h]
   fistp   dword [ebp-0Ch]
   fldcw   word [ebp-16h]
   mov     eax, [ebp+8]
   mov     eax, [eax+0Ch]
   mov     [ebp-10h], eax
   mov     eax, [ebp-0Ch]
   lea     edx, [eax+eax]
   mov     eax, [ebp+0Ch]
   movzx   eax, word [eax+edx]
   cmp     [ebp-10h], eax
   jge     loc_80BB35F
   mov     dword [ebp-8], 0
   mov     eax, [ebp-0Ch]
   mov     [ebp-14h], eax
loc_80BB2ED:
   mov     eax, [ebp-14h]
   mov     edx, [ebp-8]
   add     edx, eax
   mov     eax, edx
   sar     eax, 1Fh
   shr     eax, 1Fh
   lea     eax, [eax+edx]
   sar     eax, 1
   mov     [ebp-0Ch], eax
   mov     eax, [ebp-0Ch]
   lea     edx, [eax+eax]
   mov     eax, [ebp+0Ch]
   movzx   eax, word [eax+edx]
   cmp     [ebp-10h], eax
   jge     loc_80BB31F
   mov     eax, [ebp-0Ch]
   mov     [ebp-14h], eax
   jmp     loc_80BB2ED
loc_80BB31F:
   mov     eax, [ebp-0Ch]
   add     eax, eax
   add     eax, [ebp+0Ch]
   add     eax, 2
   movzx   eax, word [eax]
   cmp     [ebp-10h], eax
   jl      loc_80BB3EB
   mov     eax, [ebp-0Ch]
   inc     eax
   mov     [ebp-8], eax
   lea     eax, [ebp-14h]
   dec     dword [eax]
   mov     eax, [ebp-14h]
   lea     edx, [eax+eax]
   mov     eax, [ebp+0Ch]
   movzx   eax, word [eax+edx]
   cmp     [ebp-10h], eax
   jl      loc_80BB2ED
   mov     eax, [ebp-14h]
   mov     [ebp-0Ch], eax
   jmp     loc_80BB3EB
loc_80BB35F:
   mov     eax, [ebp-0Ch]
   add     eax, eax
   add     eax, [ebp+0Ch]
   add     eax, 2
   movzx   eax, word [eax]
   cmp     [ebp-10h], eax
   jl      loc_80BB3EB
   mov     eax, [ebp-0Ch]
   inc     eax
   mov     [ebp-8], eax
   mov     eax, [ebp+10h]
   mov     [ebp-14h], eax
loc_80BB37F:
   mov     eax, [ebp-14h]
   mov     edx, [ebp-8]
   add     edx, eax
   mov     eax, edx
   sar     eax, 1Fh
   shr     eax, 1Fh
   lea     eax, [eax+edx]
   sar     eax, 1
   mov     [ebp-0Ch], eax
   mov     eax, [ebp-0Ch]
   lea     edx, [eax+eax]
   mov     eax, [ebp+0Ch]
   movzx   eax, word [eax+edx]
   cmp     [ebp-10h], eax
   jge     loc_80BB3CF
   mov     eax, [ebp-0Ch]
   mov     [ebp-14h], eax
   lea     eax, [ebp-8]
   inc     dword [eax]
   mov     eax, [ebp-8]
   lea     edx, [eax+eax]
   mov     eax, [ebp+0Ch]
   movzx   eax, word [eax+edx]
   cmp     [ebp-10h], eax
   jge     loc_80BB37F
   mov     eax, [ebp-8]
   dec     eax
   mov     [ebp-0Ch], eax
   jmp     loc_80BB3EB
loc_80BB3CF:
   mov     eax, [ebp-0Ch]
   add     eax, eax
   add     eax, [ebp+0Ch]
   add     eax, 2
   movzx   eax, word [eax]
   cmp     [ebp-10h], eax
   jl      loc_80BB3EB
   mov     eax, [ebp-0Ch]
   inc     eax
   mov     [ebp-8], eax
   jmp     loc_80BB37F
loc_80BB3EB:
   mov     ebx, [ebp+18h]
   mov     ecx, [ebp+8]
   mov     eax, [ebp-0Ch]
   lea     edx, [eax+eax]
   mov     eax, [ebp+0Ch]
   movzx   eax, word [eax+edx]
   push    eax
   fild    dword [esp]
   lea     esp, [esp+4]
   fld     dword [ecx+8]
   fsubrp  st1, st0
   mov     eax, [ebp-0Ch]
   add     eax, eax
   add     eax, [ebp+0Ch]
   add     eax, 2
   movzx   ecx, word [eax]
   mov     eax, [ebp-0Ch]
   lea     edx, [eax+eax]
   mov     eax, [ebp+0Ch]
   movzx   eax, word [eax+edx]
   sub     ecx, eax
   mov     eax, ecx
   push    eax
   fild    dword [esp]
   lea     esp, [esp+4]
   fdivp   st1, st0
   fstp    dword [ebx]
   mov     edx, [ebp+14h]
   mov     eax, [ebp-0Ch]
   mov     [edx], eax
   add     esp, 14h
   pop     ebx
   pop     ebp
   retn    


XAnimCalcDynamicFrameIndices_s:
   push    ebp
   mov     ebp, esp
   sub     esp, 18h
   mov     edx, [ebp+8]
   mov     eax, [ebp+10h]
   cmp     eax, [edx+4]
   jge     loc_80BB47E
   mov     eax, [ebp+18h]
   mov     [esp+10h], eax
   mov     eax, [ebp+14h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+10h]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XAnim_GetTimeIndex
   jmp     locret_80BB49C
loc_80BB47E:
   mov     ecx, [ebp+18h]
   mov     edx, [ebp+8]
   mov     eax, [ebp+8]
   fild    dword [eax+0Ch]
   fld     dword [edx+8]
   fsubrp  st1, st0
   fstp    dword [ecx]
   mov     edx, [ebp+14h]
   mov     eax, [ebp+8]
   mov     eax, [eax+0Ch]
   mov     [edx], eax
locret_80BB49C:
   leave   
   retn    


sub_80C0B94:
   push    ebp
   mov     ebp, esp
   sub     esp, 28h
   mov     eax, [ebp+0Ch]
   cmp     word [eax], 0
   jnz     loc_80C0BC8
   mov     eax, [ebp+14h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+0Ch]
   add     eax, 4
   mov     [esp+8], eax
   mov     eax, [ebp+10h]
   mov     [esp+4], eax
   mov     eax, [ebp+14h]
   mov     [esp], eax
   call    sub_80C150E
   jmp     locret_80C0C38
loc_80C0BC8:
   lea     eax, [ebp-8]
   mov     [esp+10h], eax
   lea     eax, [ebp-4]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+0Ch]
   movzx   eax, word [eax]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   add     eax, 8
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XAnimCalcDynamicFrameIndices_s
   mov     eax, [ebp+14h]
   mov     [esp+14h], eax
   mov     eax, [ebp-8]
   mov     [esp+10h], eax
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp-4]
   shl     eax, 2
   add     eax, [edx+4]
   add     eax, 4
   mov     [esp+0Ch], eax
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp-4]
   shl     eax, 2
   add     eax, [edx+4]
   mov     [esp+8], eax
   mov     eax, [ebp+10h]
   mov     [esp+4], eax
   mov     eax, [ebp+14h]
   mov     [esp], eax
   call    sub_80C13AA
locret_80C0C38:
   leave   
   retn    


XAnimNormalizeRotScaleTransArray:
   push    ebp
   mov     ebp, esp
   sub     esp, 48h
   mov     eax, [ebp+8]
   movzx   eax, word [eax]
   mov     [ebp-24h], eax
   lea     eax, [ebp-38h]
   mov     [esp+8], eax
   mov     eax, [ebp-24h]
   mov     [esp+4], eax
   mov     eax, [ebp+10h]
   mov     [esp], eax
   call    XAnim_SetTime
   fld     dword [ebp+14h]
   fld     dword [flt_81492C4]
   fmulp   st1, st0
   fstp    dword [ebp-18h]
   mov     eax, [ebp+8]
   mov     eax, [eax+14h]
   mov     [ebp-28h], eax
   mov     eax, [ebp+8]
   movsx   eax, word [eax+0Eh]
   mov     [ebp-20h], eax
   mov     dword [ebp-10h], 0
loc_80C0DE2:
   mov     eax, [ebp-10h]
   cmp     eax, [ebp-20h]
   jl      loc_80C0DEF
   jmp     locret_80C0F06
loc_80C0DEF:
   mov     eax, [ebp-10h]
   add     eax, [ebp+0Ch]
   movzx   eax, byte [eax]
   mov     [ebp-1Ch], eax
   mov     eax, [ebp-1Ch]
   sar     eax, 5
   lea     edx, 0[eax*4]
   mov     eax, [ebp+1Ch]
   mov     ecx, [ebp-1Ch]
   and     ecx, 1Fh
   mov     eax, [eax+edx]
   sar     eax, cl
   and     eax, 1
   test    al, al
   jz      loc_80C0E22
   jmp     loc_80C0EFC
loc_80C0E22:
   mov     eax, [ebp-1Ch]
   shl     eax, 5
   add     eax, [ebp+18h]
   mov     [ebp-0Ch], eax
   mov     edx, [ebp+8]
   mov     eax, [ebp-10h]
   shl     eax, 3
   add     eax, [edx+18h]
   mov     [ebp-14h], eax
   mov     eax, [ebp-10h]
   mov     edx, eax
   sar     edx, 3
   mov     eax, [ebp-28h]
   movsx   eax, byte [edx+eax]
   mov     ecx, [ebp-10h]
   and     ecx, 7
   sar     eax, cl
   and     eax, 1
   test    al, al
   jz      loc_80C0E9D
   mov     eax, [ebp-14h]
   cmp     dword [eax+4], 0
   jz      loc_80C0E8C
   mov     eax, [ebp-0Ch]
   add     eax, 8
   mov     [esp+0Ch], eax
   mov     eax, [ebp-18h]
   mov     [esp+8], eax
   mov     eax, [ebp-14h]
   mov     eax, [eax+4]
   mov     [esp+4], eax
   lea     eax, [ebp-38h]
   mov     [esp], eax
   call    sub_80C0B94
   jmp     loc_80C0EC0
loc_80C0E8C:
   mov     edx, [ebp-0Ch]
   mov     eax, [ebp-0Ch]
   fld     dword [eax+0Ch]
   fadd    dword [ebp+14h]
   fstp    dword [edx+0Ch]
   jmp     loc_80C0EC0
loc_80C0E9D:
   mov     eax, [ebp-0Ch]
   mov     [esp+0Ch], eax
   mov     eax, [ebp-18h]
   mov     [esp+8], eax
   mov     eax, [ebp-14h]
   mov     eax, [eax+4]
   mov     [esp+4], eax
   lea     eax, [ebp-38h]
   mov     [esp], eax
   call    sub_80C0C3A
loc_80C0EC0:
   mov     eax, [ebp-14h]
   cmp     dword [eax], 0
   jz      loc_80C0EED
   mov     eax, [ebp-0Ch]
   add     eax, 10h
   mov     [esp+0Ch], eax
   mov     eax, [ebp+14h]
   mov     [esp+8], eax
   mov     eax, [ebp-14h]
   mov     eax, [eax]
   mov     [esp+4], eax
   lea     eax, [ebp-38h]
   mov     [esp], eax
   call    sub_80C0CE0
loc_80C0EED:
   mov     edx, [ebp-0Ch]
   mov     eax, [ebp-0Ch]
   fld     dword [eax+1Ch]
   fadd    dword [ebp+14h]
   fstp    dword [edx+1Ch]
loc_80C0EFC:
   lea     eax, [ebp-10h]
   inc     dword [eax]
   jmp     loc_80C0DE2
locret_80C0F06:
   leave   
   retn    


sub_80C1706:
   push    ebp
   mov     ebp, esp
   fld     dword [ebp+0Ch]
   fsub    dword [ebp+8]
   fmul    dword [ebp+10h]
   fadd    dword [ebp+8]
   pop     ebp
   retn    


sub_80C1608:
   push    ebp
   mov     ebp, esp
   push    esi
   push    ebx
   sub     esp, 10h
   mov     ebx, [ebp+1Ch]
   mov     esi, [ebp+8]
   mov     eax, [ebp+18h]
   mov     [esp+8], eax
   mov     eax, [ebp+14h]
   mov     eax, [eax]
   mov     [esp+4], eax
   mov     eax, [ebp+10h]
   mov     eax, [eax]
   mov     [esp], eax
   call    sub_80C1706
   fmul    dword [ebp+0Ch]
   fld     dword [esi]
   faddp   st1, st0
   fstp    dword [ebx]
   mov     ebx, [ebp+1Ch]
   add     ebx, 4
   mov     esi, [ebp+8]
   add     esi, 4
   mov     eax, [ebp+18h]
   mov     [esp+8], eax
   mov     eax, [ebp+14h]
   add     eax, 4
   mov     eax, [eax]
   mov     [esp+4], eax
   mov     eax, [ebp+10h]
   add     eax, 4
   mov     eax, [eax]
   mov     [esp], eax
   call    sub_80C1706
   fmul    dword [ebp+0Ch]
   fld     dword [esi]
   faddp   st1, st0
   fstp    dword [ebx]
   mov     ebx, [ebp+1Ch]
   add     ebx, 8
   mov     esi, [ebp+8]
   add     esi, 8
   mov     eax, [ebp+18h]
   mov     [esp+8], eax
   mov     eax, [ebp+14h]
   add     eax, 8
   mov     eax, [eax]
   mov     [esp+4], eax
   mov     eax, [ebp+10h]
   add     eax, 8
   mov     eax, [eax]
   mov     [esp], eax
   call    sub_80C1706
   fmul    dword [ebp+0Ch]
   fld     dword [esi]
   faddp   st1, st0
   fstp    dword [ebx]
   add     esp, 10h
   pop     ebx
   pop     esi
   pop     ebp
   retn    


sub_80C096C:
   push    ebp
   mov     ebp, esp
   sub     esp, 28h
   mov     eax, [ebp+0Ch]
   cmp     word [eax], 0
   jnz     loc_80C09A0
   mov     eax, [ebp+14h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+0Ch]
   add     eax, 4
   mov     [esp+8], eax
   mov     eax, [ebp+10h]
   mov     [esp+4], eax
   mov     eax, [ebp+14h]
   mov     [esp], eax
   call    VectorMA561
   jmp     locret_80C0A1C
loc_80C09A0:
   lea     eax, [ebp-8]
   mov     [esp+10h], eax
   lea     eax, [ebp-4]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+0Ch]
   movzx   eax, word [eax]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   add     eax, 8
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XAnimCalcDynamicFrameIndices_b
   mov     eax, [ebp+14h]
   mov     [esp+14h], eax
   mov     eax, [ebp-8]
   mov     [esp+10h], eax
   mov     ecx, [ebp+0Ch]
   mov     edx, [ebp-4]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   add     eax, [ecx+4]
   add     eax, 0Ch
   mov     [esp+0Ch], eax
   mov     ecx, [ebp+0Ch]
   mov     edx, [ebp-4]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   add     eax, [ecx+4]
   mov     [esp+8], eax
   mov     eax, [ebp+10h]
   mov     [esp+4], eax
   mov     eax, [ebp+14h]
   mov     [esp], eax
   call    sub_80C1608
locret_80C0A1C:
   leave   
   retn    


sub_80C1422:
   push    ebp
   mov     ebp, esp
   push    esi
   push    ebx
   sub     esp, 10h
   mov     ebx, [ebp+1Ch]
   mov     esi, [ebp+8]
   mov     eax, [ebp+18h]
   mov     [esp+8], eax
   mov     eax, [ebp+14h]
   movsx   eax, word [eax]
   mov     [esp+4], eax
   mov     eax, [ebp+10h]
   movsx   eax, word [eax]
   mov     [esp], eax
   call    XAnimGetFrameTime
   fmul    dword [ebp+0Ch]
   fld     dword [esi]
   faddp   st1, st0
   fstp    dword [ebx]
   mov     ebx, [ebp+1Ch]
   add     ebx, 4
   mov     esi, [ebp+8]
   add     esi, 4
   mov     eax, [ebp+18h]
   mov     [esp+8], eax
   mov     eax, [ebp+14h]
   add     eax, 2
   movsx   eax, word [eax]
   mov     [esp+4], eax
   mov     eax, [ebp+10h]
   add     eax, 2
   movsx   eax, word [eax]
   mov     [esp], eax
   call    XAnimGetFrameTime
   fmul    dword [ebp+0Ch]
   fld     dword [esi]
   faddp   st1, st0
   fstp    dword [ebx]
   mov     ebx, [ebp+1Ch]
   add     ebx, 8
   mov     esi, [ebp+8]
   add     esi, 8
   mov     eax, [ebp+18h]
   mov     [esp+8], eax
   mov     eax, [ebp+14h]
   add     eax, 4
   movsx   eax, word [eax]
   mov     [esp+4], eax
   mov     eax, [ebp+10h]
   add     eax, 4
   movsx   eax, word [eax]
   mov     [esp], eax
   call    XAnimGetFrameTime
   fmul    dword [ebp+0Ch]
   fld     dword [esi]
   faddp   st1, st0
   fstp    dword [ebx]
   mov     ebx, [ebp+1Ch]
   add     ebx, 0Ch
   mov     esi, [ebp+8]
   add     esi, 0Ch
   mov     eax, [ebp+18h]
   mov     [esp+8], eax
   mov     eax, [ebp+14h]
   add     eax, 6
   movsx   eax, word [eax]
   mov     [esp+4], eax
   mov     eax, [ebp+10h]
   add     eax, 6
   movsx   eax, word [eax]
   mov     [esp], eax
   call    XAnimGetFrameTime
   fmul    dword [ebp+0Ch]
   fld     dword [esi]
   faddp   st1, st0
   fstp    dword [ebx]
   add     esp, 10h
   pop     ebx
   pop     esi
   pop     ebp
   retn    


sub_80C08C6:
   push    ebp
   mov     ebp, esp
   sub     esp, 28h
   mov     eax, [ebp+0Ch]
   cmp     word [eax], 0
   jnz     loc_80C08FA
   mov     eax, [ebp+14h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+0Ch]
   add     eax, 4
   mov     [esp+8], eax
   mov     eax, [ebp+10h]
   mov     [esp+4], eax
   mov     eax, [ebp+14h]
   mov     [esp], eax
   call    sub_80C1544
   jmp     locret_80C096A
loc_80C08FA:
   lea     eax, [ebp-8]
   mov     [esp+10h], eax
   lea     eax, [ebp-4]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+0Ch]
   movzx   eax, word [eax]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   add     eax, 8
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XAnimCalcDynamicFrameIndices_b
   mov     eax, [ebp+14h]
   mov     [esp+14h], eax
   mov     eax, [ebp-8]
   mov     [esp+10h], eax
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp-4]
   shl     eax, 3
   add     eax, [edx+4]
   add     eax, 8
   mov     [esp+0Ch], eax
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp-4]
   shl     eax, 3
   add     eax, [edx+4]
   mov     [esp+8], eax
   mov     eax, [ebp+10h]
   mov     [esp+4], eax
   mov     eax, [ebp+14h]
   mov     [esp], eax
   call    sub_80C1422
locret_80C096A:
   leave   
   retn    


XAnimGetFrameTime:
   push    ebp
   mov     ebp, esp
   sub     esp, 4
   mov     eax, [ebp+8]
   mov     edx, [ebp+0Ch]
   mov     [ebp-2], ax
   mov     [ebp-4], dx
   fild    word [ebp-2]
   movsx   edx, word [ebp-4]
   movsx   eax, word [ebp-2]
   sub     edx, eax
   mov     eax, edx
   push    eax
   fild    dword [esp]
   lea     esp, [esp+4]
   fmul    dword [ebp+10h]
   faddp   st1, st0
   leave   
   retn    


sub_80C13AA:
   push    ebp
   mov     ebp, esp
   push    esi
   push    ebx
   sub     esp, 10h
   mov     ebx, [ebp+1Ch]
   mov     esi, [ebp+8]
   mov     eax, [ebp+18h]
   mov     [esp+8], eax
   mov     eax, [ebp+14h]
   movsx   eax, word [eax]
   mov     [esp+4], eax
   mov     eax, [ebp+10h]
   movsx   eax, word [eax]
   mov     [esp], eax
   call    XAnimGetFrameTime
   fmul    dword [ebp+0Ch]
   fld     dword [esi]
   faddp   st1, st0
   fstp    dword [ebx]
   mov     ebx, [ebp+1Ch]
   add     ebx, 4
   mov     esi, [ebp+8]
   add     esi, 4
   mov     eax, [ebp+18h]
   mov     [esp+8], eax
   mov     eax, [ebp+14h]
   add     eax, 2
   movsx   eax, word [eax]
   mov     [esp+4], eax
   mov     eax, [ebp+10h]
   add     eax, 2
   movsx   eax, word [eax]
   mov     [esp], eax
   call    XAnimGetFrameTime
   fmul    dword [ebp+0Ch]
   fld     dword [esi]
   faddp   st1, st0
   fstp    dword [ebx]
   add     esp, 10h
   pop     ebx
   pop     esi
   pop     ebp
   retn    


XAnim_GetTimeIndex_b:
   push    ebp
   mov     ebp, esp
   sub     esp, 14h
   mov     eax, [ebp+8]
   fild    dword [ebp+10h]
   fmul    dword [eax]
   fnstcw  word [ebp-12h]
   movzx   eax, word [ebp-12h]
   or      ax, 0C00h
   mov     [ebp-14h], ax
   fldcw   word [ebp-14h]
   fistp   dword [ebp-8]
   fldcw   word [ebp-12h]
   mov     eax, [ebp+8]
   mov     eax, [eax+0Ch]
   mov     [ebp-0Ch], eax
   mov     eax, [ebp-8]
   add     eax, [ebp+0Ch]
   movzx   eax, byte [eax]
   cmp     [ebp-0Ch], eax
   jge     loc_80BAF99
   mov     dword [ebp-4], 0
   mov     eax, [ebp-8]
   mov     [ebp-10h], eax
loc_80BAF36:
   mov     eax, [ebp-10h]
   mov     edx, [ebp-4]
   add     edx, eax
   mov     eax, edx
   sar     eax, 1Fh
   shr     eax, 1Fh
   lea     eax, [eax+edx]
   sar     eax, 1
   mov     [ebp-8], eax
   mov     eax, [ebp-8]
   add     eax, [ebp+0Ch]
   movzx   eax, byte [eax]
   cmp     [ebp-0Ch], eax
   jge     loc_80BAF64
   mov     eax, [ebp-8]
   mov     [ebp-10h], eax
   jmp     loc_80BAF36
loc_80BAF64:
   mov     eax, [ebp-8]
   add     eax, [ebp+0Ch]
   inc     eax
   movzx   eax, byte [eax]
   cmp     [ebp-0Ch], eax
   jl      loc_80BB015
   mov     eax, [ebp-8]
   inc     eax
   mov     [ebp-4], eax
   lea     eax, [ebp-10h]
   dec     dword [eax]
   mov     eax, [ebp-10h]
   add     eax, [ebp+0Ch]
   movzx   eax, byte [eax]
   cmp     [ebp-0Ch], eax
   jl      loc_80BAF36
   mov     eax, [ebp-10h]
   mov     [ebp-8], eax
   jmp     loc_80BB015
loc_80BAF99:
   mov     eax, [ebp-8]
   add     eax, [ebp+0Ch]
   inc     eax
   movzx   eax, byte [eax]
   cmp     [ebp-0Ch], eax
   jl      loc_80BB015
   mov     eax, [ebp-8]
   inc     eax
   mov     [ebp-4], eax
   mov     eax, [ebp+10h]
   mov     [ebp-10h], eax
loc_80BAFB5:
   mov     eax, [ebp-10h]
   mov     edx, [ebp-4]
   add     edx, eax
   mov     eax, edx
   sar     eax, 1Fh
   shr     eax, 1Fh
   lea     eax, [eax+edx]
   sar     eax, 1
   mov     [ebp-8], eax
   mov     eax, [ebp-8]
   add     eax, [ebp+0Ch]
   movzx   eax, byte [eax]
   cmp     [ebp-0Ch], eax
   jge     loc_80BAFFD
   mov     eax, [ebp-8]
   mov     [ebp-10h], eax
   lea     eax, [ebp-4]
   inc     dword [eax]
   mov     eax, [ebp-4]
   add     eax, [ebp+0Ch]
   movzx   eax, byte [eax]
   cmp     [ebp-0Ch], eax
   jge     loc_80BAFB5
   mov     eax, [ebp-4]
   dec     eax
   mov     [ebp-8], eax
   jmp     loc_80BB015
loc_80BAFFD:
   mov     eax, [ebp-8]
   add     eax, [ebp+0Ch]
   inc     eax
   movzx   eax, byte [eax]
   cmp     [ebp-0Ch], eax
   jl      loc_80BB015
   mov     eax, [ebp-8]
   inc     eax
   mov     [ebp-4], eax
   jmp     loc_80BAFB5
loc_80BB015:
   mov     ecx, [ebp+18h]
   mov     edx, [ebp+8]
   mov     eax, [ebp-8]
   add     eax, [ebp+0Ch]
   movzx   ax, byte [eax]
   push    ax
   fild    word [esp]
   lea     esp, [esp+2]
   fld     dword [edx+8]
   fsubrp  st1, st0
   mov     eax, [ebp-8]
   add     eax, [ebp+0Ch]
   inc     eax
   movzx   edx, byte [eax]
   mov     eax, [ebp-8]
   add     eax, [ebp+0Ch]
   movzx   eax, byte [eax]
   sub     edx, eax
   mov     eax, edx
   push    eax
   fild    dword [esp]
   lea     esp, [esp+4]
   fdivp   st1, st0
   fstp    dword [ecx]
   mov     edx, [ebp+14h]
   mov     eax, [ebp-8]
   mov     [edx], eax
   leave   
   retn    


XAnimCalcDynamicFrameIndices_b:
   push    ebp
   mov     ebp, esp
   sub     esp, 18h
   mov     edx, [ebp+8]
   mov     eax, [ebp+10h]
   cmp     eax, [edx+4]
   jge     loc_80BB09A
   mov     eax, [ebp+18h]
   mov     [esp+10h], eax
   mov     eax, [ebp+14h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+10h]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XAnim_GetTimeIndex_b
   jmp     locret_80BB0B8
loc_80BB09A:
   mov     ecx, [ebp+18h]
   mov     edx, [ebp+8]
   mov     eax, [ebp+8]
   fild    dword [eax+0Ch]
   fld     dword [edx+8]
   fsubrp  st1, st0
   fstp    dword [ecx]
   mov     edx, [ebp+14h]
   mov     eax, [ebp+8]
   mov     eax, [eax+0Ch]
   mov     [edx], eax
locret_80BB0B8:
   leave   
   retn    


sub_80C0820:
   push    ebp
   mov     ebp, esp
   sub     esp, 28h
   mov     eax, [ebp+0Ch]
   cmp     word [eax], 0
   jnz     loc_80C0854
   mov     eax, [ebp+14h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+0Ch]
   add     eax, 4
   mov     [esp+8], eax
   mov     eax, [ebp+10h]
   mov     [esp+4], eax
   mov     eax, [ebp+14h]
   mov     [esp], eax
   call    sub_80C150E
   jmp     locret_80C08C4
loc_80C0854:
   lea     eax, [ebp-8]
   mov     [esp+10h], eax
   lea     eax, [ebp-4]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+0Ch]
   movzx   eax, word [eax]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   add     eax, 8
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XAnimCalcDynamicFrameIndices_b
   mov     eax, [ebp+14h]
   mov     [esp+14h], eax
   mov     eax, [ebp-8]
   mov     [esp+10h], eax
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp-4]
   shl     eax, 2
   add     eax, [edx+4]
   add     eax, 4
   mov     [esp+0Ch], eax
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp-4]
   shl     eax, 2
   add     eax, [edx+4]
   mov     [esp+8], eax
   mov     eax, [ebp+10h]
   mov     [esp+4], eax
   mov     eax, [ebp+14h]
   mov     [esp], eax
   call    sub_80C13AA
locret_80C08C4:
   leave   
   retn    


XAnim_SetTime:
   push    ebp
   mov     ebp, esp
   sub     esp, 4
   mov     edx, [ebp+10h]
   mov     eax, [ebp+8]
   mov     [edx], eax
   mov     edx, [ebp+10h]
   mov     eax, [ebp+0Ch]
   mov     [edx+4], eax
   mov     eax, [ebp+10h]
   fild    dword [ebp+0Ch]
   fld     dword [ebp+8]
   fmulp   st1, st0
   fstp    dword [eax+8]
   mov     edx, [ebp+10h]
   mov     eax, [ebp+10h]
   fld     dword [eax+8]
   fnstcw  word [ebp-2]
   movzx   eax, word [ebp-2]
   or      ax, 0C00h
   mov     [ebp-4], ax
   fldcw   word [ebp-4]
   fistp   dword [edx+0Ch]
   fldcw   word [ebp-2]
   leave   
   retn    


XAnimMadRotTransArray:
   push    ebp
   mov     ebp, esp
   sub     esp, 48h
   mov     eax, [ebp+8]
   movzx   eax, word [eax]
   mov     [ebp-24h], eax
   lea     eax, [ebp-38h]
   mov     [esp+8], eax
   mov     eax, [ebp-24h]
   mov     [esp+4], eax
   mov     eax, [ebp+10h]
   mov     [esp], eax
   call    XAnim_SetTime
   fld     dword [ebp+14h]
   fld     dword [flt_81492C0]
   fmulp   st1, st0
   fstp    dword [ebp-18h]
   mov     eax, [ebp+8]
   mov     eax, [eax+14h]
   mov     [ebp-28h], eax
   mov     eax, [ebp+8]
   movsx   eax, word [eax+0Eh]
   mov     [ebp-20h], eax
   mov     dword [ebp-10h], 0
loc_80C0A6E:
   mov     eax, [ebp-10h]
   cmp     eax, [ebp-20h]
   jl      loc_80C0A7B
   jmp     locret_80C0B92
loc_80C0A7B:
   mov     eax, [ebp-10h]
   add     eax, [ebp+0Ch]
   movzx   eax, byte [eax]
   mov     [ebp-1Ch], eax
   mov     eax, [ebp-1Ch]
   sar     eax, 5
   lea     edx, 0[eax*4]
   mov     eax, [ebp+1Ch]
   mov     ecx, [ebp-1Ch]
   and     ecx, 1Fh
   mov     eax, [eax+edx]
   sar     eax, cl
   and     eax, 1
   test    al, al
   jz      loc_80C0AAE
   jmp     loc_80C0B88
loc_80C0AAE:
   mov     eax, [ebp-1Ch]
   shl     eax, 5
   add     eax, [ebp+18h]
   mov     [ebp-0Ch], eax
   mov     edx, [ebp+8]
   mov     eax, [ebp-10h]
   shl     eax, 3
   add     eax, [edx+18h]
   mov     [ebp-14h], eax
   mov     eax, [ebp-10h]
   mov     edx, eax
   sar     edx, 3
   mov     eax, [ebp-28h]
   movsx   eax, byte [edx+eax]
   mov     ecx, [ebp-10h]
   and     ecx, 7
   sar     eax, cl
   and     eax, 1
   test    al, al
   jz      loc_80C0B29
   mov     eax, [ebp-14h]
   cmp     dword [eax+4], 0
   jz      loc_80C0B18
   mov     eax, [ebp-0Ch]
   add     eax, 8
   mov     [esp+0Ch], eax
   mov     eax, [ebp-18h]
   mov     [esp+8], eax
   mov     eax, [ebp-14h]
   mov     eax, [eax+4]
   mov     [esp+4], eax
   lea     eax, [ebp-38h]
   mov     [esp], eax
   call    sub_80C0820
   jmp     loc_80C0B4C
loc_80C0B18:
   mov     edx, [ebp-0Ch]
   mov     eax, [ebp-0Ch]
   fld     dword [eax+0Ch]
   fadd    dword [ebp+14h]
   fstp    dword [edx+0Ch]
   jmp     loc_80C0B4C
loc_80C0B29:
   mov     eax, [ebp-0Ch]
   mov     [esp+0Ch], eax
   mov     eax, [ebp-18h]
   mov     [esp+8], eax
   mov     eax, [ebp-14h]
   mov     eax, [eax+4]
   mov     [esp+4], eax
   lea     eax, [ebp-38h]
   mov     [esp], eax
   call    sub_80C08C6
loc_80C0B4C:
   mov     eax, [ebp-14h]
   cmp     dword [eax], 0
   jz      loc_80C0B79
   mov     eax, [ebp-0Ch]
   add     eax, 10h
   mov     [esp+0Ch], eax
   mov     eax, [ebp+14h]
   mov     [esp+8], eax
   mov     eax, [ebp-14h]
   mov     eax, [eax]
   mov     [esp+4], eax
   lea     eax, [ebp-38h]
   mov     [esp], eax
   call    sub_80C096C
loc_80C0B79:
   mov     edx, [ebp-0Ch]
   mov     eax, [ebp-0Ch]
   fld     dword [eax+1Ch]
   fadd    dword [ebp+14h]
   fstp    dword [edx+1Ch]
loc_80C0B88:
   lea     eax, [ebp-10h]
   inc     dword [eax]
   jmp     loc_80C0A6E
locret_80C0B92:
   leave   
   retn    


sub_80BABCE:
   push    ebp
   mov     ebp, esp
   sub     esp, 18h
   mov     eax, [ebp+8]
   cmp     word [eax], 0
   jnz     loc_80BAC02
   mov     eax, [ebp+10h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+8]
   add     eax, 4
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+10h]
   mov     [esp], eax
   call    VectorMA561
   jmp     locret_80BAC34
loc_80BAC02:
   mov     eax, [ebp+10h]
   mov     [esp+0Ch], eax
   mov     ecx, [ebp+8]
   mov     eax, [ebp+8]
   movzx   edx, word [eax]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   add     eax, [ecx+4]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+10h]
   mov     [esp], eax
   call    VectorMA561
locret_80BAC34:
   leave   
   retn    


sub_80C1544:
   push    ebp
   mov     ebp, esp
   mov     ecx, [ebp+14h]
   mov     edx, [ebp+8]
   mov     eax, [ebp+10h]
   fild    word [eax]
   fmul    dword [ebp+0Ch]
   fld     dword [edx]
   faddp   st1, st0
   fstp    dword [ecx]
   mov     ecx, [ebp+14h]
   add     ecx, 4
   mov     edx, [ebp+8]
   add     edx, 4
   mov     eax, [ebp+10h]
   add     eax, 2
   fild    word [eax]
   fmul    dword [ebp+0Ch]
   fld     dword [edx]
   faddp   st1, st0
   fstp    dword [ecx]
   mov     ecx, [ebp+14h]
   add     ecx, 8
   mov     edx, [ebp+8]
   add     edx, 8
   mov     eax, [ebp+10h]
   add     eax, 4
   fild    word [eax]
   fmul    dword [ebp+0Ch]
   fld     dword [edx]
   faddp   st1, st0
   fstp    dword [ecx]
   mov     ecx, [ebp+14h]
   add     ecx, 0Ch
   mov     edx, [ebp+8]
   add     edx, 0Ch
   mov     eax, [ebp+10h]
   add     eax, 6
   fild    word [eax]
   fmul    dword [ebp+0Ch]
   fld     dword [edx]
   faddp   st1, st0
   fstp    dword [ecx]
   pop     ebp
   retn    


sub_80BAB6C:
   push    ebp
   mov     ebp, esp
   sub     esp, 18h
   mov     eax, [ebp+8]
   cmp     word [eax], 0
   jnz     loc_80BABA0
   mov     eax, [ebp+10h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+8]
   add     eax, 4
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+10h]
   mov     [esp], eax
   call    sub_80C1544
   jmp     locret_80BABCC
loc_80BABA0:
   mov     eax, [ebp+10h]
   mov     [esp+0Ch], eax
   mov     edx, [ebp+8]
   mov     eax, [ebp+8]
   movzx   eax, word [eax]
   shl     eax, 3
   add     eax, [edx+4]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+10h]
   mov     [esp], eax
   call    sub_80C1544
locret_80BABCC:
   leave   
   retn    


sub_80C150E:
   push    ebp
   mov     ebp, esp
   mov     ecx, [ebp+14h]
   mov     edx, [ebp+8]
   mov     eax, [ebp+10h]
   fild    word [eax]
   fmul    dword [ebp+0Ch]
   fld     dword [edx]
   faddp   st1, st0
   fstp    dword [ecx]
   mov     ecx, [ebp+14h]
   add     ecx, 4
   mov     edx, [ebp+8]
   add     edx, 4
   mov     eax, [ebp+10h]
   add     eax, 2
   fild    word [eax]
   fmul    dword [ebp+0Ch]
   fld     dword [edx]
   faddp   st1, st0
   fstp    dword [ecx]
   pop     ebp
   retn    


sub_80BAB16:
   push    ebp
   mov     ebp, esp
   sub     esp, 18h
   mov     eax, [ebp+8]
   cmp     word [eax], 0
   jnz     loc_80BAB30
   mov     eax, [ebp+8]
   add     eax, 4
   mov     [ebp-8], eax
   jmp     loc_80BAB44
loc_80BAB30:
   mov     edx, [ebp+8]
   mov     eax, [ebp+8]
   movzx   eax, word [eax]
   shl     eax, 2
   mov     edx, [edx+4]
   add     edx, eax
   mov     [ebp-8], edx
loc_80BAB44:
   mov     eax, [ebp-8]
   mov     [ebp-4], eax
   mov     eax, [ebp+10h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp-4]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+10h]
   mov     [esp], eax
   call    sub_80C150E
   leave   
   retn    


XAnimApplyAdditives:
   push    ebp
   mov     ebp, esp
   sub     esp, 28h
   fld     dword [ebp+10h]
   fld     dword [flt_8148EB8]
   fmulp   st1, st0
   fstp    dword [ebp-10h]
   mov     eax, [ebp+8]
   mov     eax, [eax+14h]
   mov     [ebp-1Ch], eax
   mov     eax, [ebp+8]
   movsx   eax, word [eax+0Eh]
   mov     [ebp-18h], eax
   mov     dword [ebp-8], 0
loc_80BAC64:
   mov     eax, [ebp-8]
   cmp     eax, [ebp-18h]
   jl      loc_80BAC71
   jmp     locret_80BAD73
loc_80BAC71:
   mov     eax, [ebp-8]
   add     eax, [ebp+0Ch]
   movzx   eax, byte [eax]
   mov     [ebp-14h], eax
   mov     eax, [ebp-14h]
   sar     eax, 5
   lea     edx, 0[eax*4]
   mov     eax, [ebp+18h]
   mov     ecx, [ebp-14h]
   and     ecx, 1Fh
   mov     eax, [eax+edx]
   sar     eax, cl
   and     eax, 1
   test    al, al
   jz      loc_80BACA4
   jmp     loc_80BAD69
loc_80BACA4:
   mov     eax, [ebp-14h]
   shl     eax, 5
   add     eax, [ebp+14h]
   mov     [ebp-4], eax
   mov     edx, [ebp+8]
   mov     eax, [ebp-8]
   shl     eax, 3
   add     eax, [edx+18h]
   mov     [ebp-0Ch], eax
   mov     eax, [ebp-8]
   mov     edx, eax
   sar     edx, 3
   mov     eax, [ebp-1Ch]
   movsx   eax, byte [edx+eax]
   mov     ecx, [ebp-8]
   and     ecx, 7
   sar     eax, cl
   and     eax, 1
   test    al, al
   jz      loc_80BAD18
   mov     eax, [ebp-0Ch]
   cmp     dword [eax+4], 0
   jz      loc_80BAD07
   mov     eax, [ebp-4]
   add     eax, 8
   mov     [esp+8], eax
   mov     eax, [ebp-10h]
   mov     [esp+4], eax
   mov     eax, [ebp-0Ch]
   mov     eax, [eax+4]
   mov     [esp], eax
   call    sub_80BAB16
   jmp     loc_80BAD34
loc_80BAD07:
   mov     edx, [ebp-4]
   mov     eax, [ebp-4]
   fld     dword [eax+0Ch]
   fadd    dword [ebp+10h]
   fstp    dword [edx+0Ch]
   jmp     loc_80BAD34
loc_80BAD18:
   mov     eax, [ebp-4]
   mov     [esp+8], eax
   mov     eax, [ebp-10h]
   mov     [esp+4], eax
   mov     eax, [ebp-0Ch]
   mov     eax, [eax+4]
   mov     [esp], eax
   call    sub_80BAB6C
loc_80BAD34:
   mov     eax, [ebp-0Ch]
   cmp     dword [eax], 0
   jz      loc_80BAD5A
   mov     eax, [ebp-4]
   add     eax, 10h
   mov     [esp+8], eax
   mov     eax, [ebp+10h]
   mov     [esp+4], eax
   mov     eax, [ebp-0Ch]
   mov     eax, [eax]
   mov     [esp], eax
   call    sub_80BABCE
loc_80BAD5A:
   mov     edx, [ebp-4]
   mov     eax, [ebp-4]
   fld     dword [eax+1Ch]
   fadd    dword [ebp+10h]
   fstp    dword [edx+1Ch]
loc_80BAD69:
   lea     eax, [ebp-8]
   inc     dword [eax]
   jmp     loc_80BAC64
locret_80BAD73:
   leave   
   retn    


XAnimCalc_GetLowestElement:
   push    ebp
   mov     ebp, esp
   sub     esp, 14h
   mov     eax, 3FC00000h
   mov     [ebp-10h], eax
   fld     dword [ebp+8]
   fld     dword [flt_81492CC]
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
   fld     dword [flt_81492C8]
   fsubrp  st1, st0
   fld     dword [ebp-0Ch]
   fmulp   st1, st0
   fstp    dword [ebp-0Ch]
   mov     eax, [ebp-0Ch]
   mov     [ebp-14h], eax
   fld     dword [ebp-14h]
   leave   
   retn    


XAnimSetModel:
   push    ebp
   mov     ebp, esp
   push    ebx
   sub     esp, 0E4h
   mov     eax, [ebp+8]
   mov     eax, [eax+4]
   mov     [ebp-0A8h], eax
   mov     eax, [ebp-0A8h]
   mov     eax, [eax+10h]
   mov     [ebp-0B4h], eax
   mov     eax, [ebp-0A8h]
   movsx   eax, word [eax+0Eh]
   mov     [ebp-0ACh], eax
   mov     eax, [ebp-0ACh]
   add     eax, 10h
   mov     [ebp-0C0h], eax
   mov     dword [ebp-0C4h], 0
loc_80BA8D4:
   cmp     dword [ebp-0C4h], 3
   jle     loc_80BA8DF
   jmp     loc_80BA8FA
loc_80BA8DF:
   mov     eax, [ebp-0C4h]
   mov     dword [ebp+eax*4-98h], 0
   lea     eax, [ebp-0C4h]
   inc     dword [eax]
   jmp     loc_80BA8D4
loc_80BA8FA:
   mov     eax, [ebp-0ACh]
   dec     eax
   mov     [ebp-9Ch], eax
loc_80BA907:
   cmp     dword [ebp-9Ch], 0
   jns     loc_80BA912
   jmp     loc_80BA92B
loc_80BA912:
   lea     eax, [ebp-88h]
   add     eax, [ebp-9Ch]
   mov     byte [eax], 7Fh
   lea     eax, [ebp-9Ch]
   dec     dword [eax]
   jmp     loc_80BA907
loc_80BA92B:
   mov     dword [ebp-0A0h], 0
   mov     dword [ebp-0C4h], 0
loc_80BA93F:
   mov     eax, [ebp-0C4h]
   cmp     eax, [ebp+10h]
   jl      loc_80BA94F
   jmp     loc_80BAA91
loc_80BA94F:
   mov     eax, [ebp-0C4h]
   lea     edx, 0[eax*4]
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+edx]
   mov     eax, [eax]
   mov     [ebp-0D0h], eax
   mov     eax, [ebp-0D0h]
   mov     eax, [eax+4]
   mov     [ebp-0CCh], eax
   mov     eax, [ebp-0CCh]
   mov     eax, [eax]
   mov     [ebp-0B8h], eax
   mov     eax, [ebp-0D0h]
   movsx   eax, word [eax]
   mov     [ebp-0B0h], eax
   mov     dword [ebp-0A4h], 0
loc_80BA9A0:
   mov     eax, [ebp-0A4h]
   cmp     eax, [ebp-0B0h]
   jl      loc_80BA9B3
   jmp     loc_80BAA84
loc_80BA9B3:
   mov     eax, [ebp-0A4h]
   lea     edx, [eax+eax]
   mov     eax, [ebp-0B8h]
   movzx   eax, word [eax+edx]
   mov     [ebp-0BCh], eax
   mov     eax, [ebp-0ACh]
   dec     eax
   mov     [ebp-9Ch], eax
loc_80BA9D9:
   cmp     dword [ebp-9Ch], 0
   jns     loc_80BA9E7
   jmp     loc_80BAA6F
loc_80BA9E7:
   mov     eax, [ebp-9Ch]
   lea     edx, [eax+eax]
   mov     eax, [ebp-0B4h]
   movzx   eax, word [eax+edx]
   cmp     [ebp-0BCh], eax
   jnz     loc_80BAA62
   lea     eax, [ebp-88h]
   add     eax, [ebp-9Ch]
   cmp     byte [eax], 7Fh
   jz      loc_80BAA15
   jmp     loc_80BAA6F
loc_80BAA15:
   lea     eax, [ebp-88h]
   mov     edx, eax
   add     edx, [ebp-9Ch]
   movzx   eax, byte [ebp-0A0h]
   mov     [edx], al
   mov     eax, [ebp-0A0h]
   mov     ebx, eax
   sar     ebx, 5
   mov     eax, [ebp-0A0h]
   mov     edx, eax
   sar     edx, 5
   mov     ecx, [ebp-0A0h]
   and     ecx, 1Fh
   mov     eax, 1
   shl     eax, cl
   or      eax, [ebp+edx*4-98h]
   mov     [ebp+ebx*4-98h], eax
   jmp     loc_80BAA6F
loc_80BAA62:
   lea     eax, [ebp-9Ch]
   dec     dword [eax]
   jmp     loc_80BA9D9
loc_80BAA6F:
   lea     eax, [ebp-0A4h]
   inc     dword [eax]
   lea     eax, [ebp-0A0h]
   inc     dword [eax]
   jmp     loc_80BA9A0
loc_80BAA84:
   lea     eax, [ebp-0C4h]
   inc     dword [eax]
   jmp     loc_80BA93F
loc_80BAA91:
   mov     dword [esp+0Ch], 0Bh
   mov     eax, [ebp-0C0h]
   mov     [esp+8], eax
   mov     dword [esp+4], 0
   lea     eax, [ebp-98h]
   mov     [esp], eax
   call    SL_GetStringOfLen
   mov     [ebp-0C8h], eax
   mov     eax, [ebp-0C8h]
   add     esp, 0E4h
   pop     ebx
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


XAnimClearQuats:
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
   mov     edx, [ebp+8]
   add     edx, 0Ch
   mov     eax, 0
   mov     [edx], eax
   pop     ebp
   retn    


XanimCalcClearInfo:
   push    ebp
   mov     ebp, esp
   sub     esp, 8
   mov     dword [ebp-4], 0
loc_80BAD83:
   mov     eax, [ebp+8]
   movzx   eax, byte [eax+19h]
   cmp     [ebp-4], eax
   jl      loc_80BAD91
   jmp     locret_80BADE3
loc_80BAD91:
   mov     edx, [ebp+10h]
   mov     eax, [ebp-4]
   sar     eax, 5
   mov     ecx, [ebp-4]
   and     ecx, 1Fh
   mov     eax, [edx+eax*4+4010h]
   sar     eax, cl
   and     eax, 1
   test    al, al
   jz      loc_80BADB2
   jmp     loc_80BADD6
loc_80BADB2:
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    XAnimClearQuats
   mov     edx, [ebp+0Ch]
   mov     eax, 0
   mov     [edx+1Ch], eax
   mov     eax, [ebp+0Ch]
   add     eax, 10h
   mov     [esp], eax
   call    VectorClear875
loc_80BADD6:
   lea     eax, [ebp-4]
   inc     dword [eax]
   lea     eax, [ebp+0Ch]
   add     dword [eax], 20h
   jmp     loc_80BAD83
locret_80BADE3:
   leave   
   retn    


Vec4LengthSq612:
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


VectorScale519:
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


Vector4Scale715:
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
   mov     edx, [ebp+10h]
   add     edx, 0Ch
   mov     eax, [ebp+8]
   add     eax, 0Ch
   fld     dword [ebp+0Ch]
   fmul    dword [eax]
   fstp    dword [edx]
   pop     ebp
   retn    


VectorMA561:
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


VectorMA4812:
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
   mov     ecx, [ebp+14h]
   add     ecx, 0Ch
   mov     edx, [ebp+8]
   add     edx, 0Ch
   mov     eax, [ebp+10h]
   add     eax, 0Ch
   fld     dword [ebp+0Ch]
   fmul    dword [eax]
   fld     dword [edx]
   faddp   st1, st0
   fstp    dword [ecx]
   pop     ebp
   retn    


XAnimCalc:
   push    ebp
   mov     ebp, esp
   push    edi
   push    esi
   push    ebx
   sub     esp, 6Ch
   mov     eax, [ebp+18h]
   mov     edx, [ebp+1Ch]
   mov     [ebp-0Dh], al
   mov     [ebp-0Eh], dl
   mov     eax, [ebp+8]
   mov     eax, [eax]
   mov     [ebp-3Ch], eax
   mov     edx, [ebp-3Ch]
   mov     eax, [ebp+0Ch]
   shl     eax, 3
   add     eax, [edx]
   add     eax, 0Ch
   mov     [ebp-34h], eax
   mov     eax, [ebp-34h]
   movzx   eax, word [eax]
   mov     [ebp-24h], eax
   cmp     dword [ebp-24h], 0
   jnz     loc_80BD8D3
   cmp     byte [ebp-0Dh], 0
   jz      loc_80BD68C
   mov     eax, [ebp+20h]
   mov     [esp+8], eax
   mov     eax, [ebp+14h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XanimCalcClearInfo
loc_80BD68C:
   mov     edx, [ebp+8]
   mov     eax, [ebp-3Ch]
   mov     eax, [eax]
   mov     eax, [eax+4]
   add     eax, eax
   add     eax, [edx+0Ch]
   mov     [ebp-38h], eax
   mov     ecx, [ebp+8]
   mov     eax, [ebp+0Ch]
   lea     edx, [eax+eax]
   mov     eax, [ecx+0Ch]
   cmp     word [eax+edx], 0
   jnz     loc_80BD6F5
   mov     eax, [ebp+0Ch]
   add     eax, [ebp-38h]
   lea     edx, [eax+1]
   mov     eax, [ebp-38h]
   movzx   eax, byte [eax]
   mov     [edx], al
   mov     edx, [ebp+8]
   mov     eax, [ebp+0Ch]
   lea     esi, [eax+eax]
   mov     ebx, [edx+0Ch]
   mov     eax, [ebp+8]
   movzx   eax, byte [eax+18h]
   mov     [esp+8], eax
   mov     eax, [ebp+8]
   add     eax, 1Ch
   mov     [esp+4], eax
   mov     eax, [ebp-34h]
   mov     [esp], eax
   call    XAnimSetModel
   mov     [ebx+esi], ax
   jmp     loc_80BD770
loc_80BD6F5:
   mov     eax, [ebp+0Ch]
   add     eax, [ebp-38h]
   inc     eax
   mov     edx, [ebp-38h]
   movzx   eax, byte [eax]
   cmp     al, [edx]
   jz      loc_80BD770
   mov     eax, [ebp+0Ch]
   add     eax, [ebp-38h]
   lea     edx, [eax+1]
   mov     eax, [ebp-38h]
   movzx   eax, byte [eax]
   mov     [edx], al
   mov     eax, [ebp-34h]
   mov     eax, [eax+4]
   movsx   eax, word [eax+0Eh]
   add     eax, 10h
   mov     [esp+4], eax
   mov     ecx, [ebp+8]
   mov     eax, [ebp+0Ch]
   lea     edx, [eax+eax]
   mov     eax, [ecx+0Ch]
   movzx   eax, word [eax+edx]
   mov     [esp], eax
   call    SL_RemoveRefToStringOfLen
   mov     edx, [ebp+8]
   mov     eax, [ebp+0Ch]
   lea     esi, [eax+eax]
   mov     ebx, [edx+0Ch]
   mov     eax, [ebp+8]
   movzx   eax, byte [eax+18h]
   mov     [esp+8], eax
   mov     eax, [ebp+8]
   add     eax, 1Ch
   mov     [esp+4], eax
   mov     eax, [ebp-34h]
   mov     [esp], eax
   call    XAnimSetModel
   mov     [ebx+esi], ax
loc_80BD770:
   mov     ecx, [ebp+8]
   mov     eax, [ebp+0Ch]
   lea     edx, [eax+eax]
   mov     eax, [ecx+0Ch]
   movzx   eax, word [eax+edx]
   mov     [esp], eax
   call    SL_ConvertToString
   mov     [ebp-40h], eax
   mov     dword [ebp-1Ch], 0
loc_80BD792:
   cmp     dword [ebp-1Ch], 3
   jle     loc_80BD79A
   jmp     loc_80BD7DF
loc_80BD79A:
   mov     eax, [ebp+20h]
   mov     [ebp-4Ch], eax
   mov     edx, [ebp-1Ch]
   mov     [ebp-50h], edx
   mov     esi, [ebp+20h]
   mov     edi, [ebp-1Ch]
   mov     ebx, [ebp-40h]
   mov     ecx, [ebp-1Ch]
   mov     edx, [ebp+20h]
   mov     eax, [ebp-1Ch]
   mov     eax, [edx+eax*4+4010h]
   not     eax
   and     eax, [ebx+ecx*4]
   or      eax, [esi+edi*4+4000h]
   mov     ecx, [ebp-50h]
   mov     edx, [ebp-4Ch]
   mov     [edx+ecx*4+4000h], eax
   lea     eax, [ebp-1Ch]
   inc     dword [eax]
   jmp     loc_80BD792
loc_80BD7DF:
   mov     eax, [ebp-34h]
   mov     eax, [eax+4]
   mov     [ebp-44h], eax
   mov     edx, [ebp-3Ch]
   mov     eax, [ebp+0Ch]
   movzx   edx, word [edx+eax*2+8]
   mov     eax, edx
   shl     eax, 2
   add     eax, edx
   shl     eax, 3
   mov     eax, [(g_xAnimInfo+0Ch)+eax]
   mov     [ebp-48h], eax
   fld     dword [ebp-48h]
   fld1    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jp      loc_80BD816
   jz      loc_80BD81F
loc_80BD816:
   mov     eax, [ebp-44h]
   cmp     word [eax], 0
   jnz     loc_80BD853
loc_80BD81F:
   mov     eax, [ebp+20h]
   add     eax, 4010h
   mov     [esp+10h], eax
   mov     eax, [ebp+14h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+10h]
   mov     [esp+8], eax
   mov     eax, [ebp-40h]
   add     eax, 10h
   mov     [esp+4], eax
   mov     eax, [ebp-44h]
   mov     [esp], eax
   call    XAnimApplyAdditives
   jmp     loc_80BDDF8
loc_80BD853:
   mov     eax, [ebp-44h]
   cmp     word [eax], 0FFh
   ja      loc_80BD898
   mov     eax, [ebp+20h]
   add     eax, 4010h
   mov     [esp+14h], eax
   mov     eax, [ebp+14h]
   mov     [esp+10h], eax
   mov     eax, [ebp+10h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp-48h]
   mov     [esp+8], eax
   mov     eax, [ebp-40h]
   add     eax, 10h
   mov     [esp+4], eax
   mov     eax, [ebp-44h]
   mov     [esp], eax
   call    XAnimMadRotTransArray
   jmp     loc_80BDDF8
loc_80BD898:
   mov     eax, [ebp+20h]
   add     eax, 4010h
   mov     [esp+14h], eax
   mov     eax, [ebp+14h]
   mov     [esp+10h], eax
   mov     eax, [ebp+10h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp-48h]
   mov     [esp+8], eax
   mov     eax, [ebp-40h]
   add     eax, 10h
   mov     [esp+4], eax
   mov     eax, [ebp-44h]
   mov     [esp], eax
   call    XAnimNormalizeRotScaleTransArray
   jmp     loc_80BDDF8
loc_80BD8D3:
   mov     dword [ebp-1Ch], 0
loc_80BD8DA:
   mov     eax, [ebp-1Ch]
   cmp     eax, [ebp-24h]
   jl      loc_80BD8E7
   jmp     loc_80BDDD9
loc_80BD8E7:
   mov     edx, [ebp-3Ch]
   mov     eax, [ebp-34h]
   movzx   eax, word [eax+6]
   add     eax, [ebp-1Ch]
   movzx   eax, word [edx+eax*2+8]
   mov     [ebp-2Eh], ax
   cmp     word [ebp-2Eh], 0
   jnz     loc_80BD909
   jmp     loc_80BDDCF
loc_80BD909:
   movzx   edx, word [ebp-2Eh]
   mov     eax, edx
   shl     eax, 2
   add     eax, edx
   shl     eax, 3
   mov     eax, [(g_xAnimInfo+20h)+eax]
   mov     [ebp-18h], eax
   fld     dword [ebp-18h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jp      loc_80BD934
   jz      loc_80BDDCF
loc_80BD934:
   mov     eax, [ebp-1Ch]
   inc     eax
   mov     [ebp-20h], eax
loc_80BD93B:
   mov     eax, [ebp-20h]
   cmp     eax, [ebp-24h]
   jl      loc_80BD948
   jmp     loc_80BDD88
loc_80BD948:
   mov     edx, [ebp-3Ch]
   mov     eax, [ebp-34h]
   movzx   eax, word [eax+6]
   add     eax, [ebp-20h]
   movzx   eax, word [edx+eax*2+8]
   mov     [ebp-2Eh], ax
   cmp     word [ebp-2Eh], 0
   jnz     loc_80BD96A
   jmp     loc_80BDD7E
loc_80BD96A:
   movzx   edx, word [ebp-2Eh]
   mov     eax, edx
   shl     eax, 2
   add     eax, edx
   shl     eax, 3
   mov     eax, [(g_xAnimInfo+20h)+eax]
   mov     [ebp-14h], eax
   fld     dword [ebp-14h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jp      loc_80BD995
   jz      loc_80BDD7E
loc_80BD995:
   cmp     byte [ebp-0Dh], 0
   jz      loc_80BD9A3
   mov     eax, [ebp+14h]
   mov     [ebp-28h], eax
   jmp     loc_80BD9D5
loc_80BD9A3:
   mov     eax, [ebp+24h]
   shl     eax, 5
   add     eax, [ebp+20h]
   mov     [ebp-28h], eax
   mov     eax, [ebp+8]
   movzx   edx, byte [eax+19h]
   lea     eax, [ebp+24h]
   add     [eax], edx
   cmp     dword [ebp+24h], 200h
   jle     loc_80BD9D5
   mov     dword [esp], aMaxCalcAnimBuf
   call    Com_Printf
   jmp     loc_80BDDF8
loc_80BD9D5:
   mov     eax, [ebp+24h]
   mov     [esp+1Ch], eax
   mov     eax, [ebp+20h]
   mov     [esp+18h], eax
   mov     dword [esp+14h], 1
   mov     dword [esp+10h], 1
   mov     eax, [ebp-28h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp-18h]
   mov     [esp+8], eax
   mov     eax, [ebp-34h]
   movzx   eax, word [eax+6]
   add     eax, [ebp-1Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XAnimCalc
   mov     eax, [ebp+24h]
   mov     [esp+1Ch], eax
   mov     eax, [ebp+20h]
   mov     [esp+18h], eax
   mov     dword [esp+14h], 1
   mov     dword [esp+10h], 0
   mov     eax, [ebp-28h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp-14h]
   mov     [esp+8], eax
   mov     eax, [ebp-34h]
   movzx   eax, word [eax+6]
   add     eax, [ebp-20h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XAnimCalc
   lea     eax, [ebp-20h]
   inc     dword [eax]
loc_80BDA64:
   mov     eax, [ebp-20h]
   cmp     eax, [ebp-24h]
   jl      loc_80BDA71
   jmp     loc_80BDB06
loc_80BDA71:
   mov     edx, [ebp-3Ch]
   mov     eax, [ebp-34h]
   movzx   eax, word [eax+6]
   add     eax, [ebp-20h]
   movzx   eax, word [edx+eax*2+8]
   mov     [ebp-2Eh], ax
   cmp     word [ebp-2Eh], 0
   jnz     loc_80BDA90
   jmp     loc_80BDAFC
loc_80BDA90:
   movzx   edx, word [ebp-2Eh]
   mov     eax, edx
   shl     eax, 2
   add     eax, edx
   shl     eax, 3
   mov     eax, [(g_xAnimInfo+20h)+eax]
   mov     [ebp-14h], eax
   fld     dword [ebp-14h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jp      loc_80BDAB7
   jz      loc_80BDAFC
loc_80BDAB7:
   mov     eax, [ebp+24h]
   mov     [esp+1Ch], eax
   mov     eax, [ebp+20h]
   mov     [esp+18h], eax
   mov     dword [esp+14h], 1
   mov     dword [esp+10h], 0
   mov     eax, [ebp-28h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp-14h]
   mov     [esp+8], eax
   mov     eax, [ebp-34h]
   movzx   eax, word [eax+6]
   add     eax, [ebp-20h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XAnimCalc
loc_80BDAFC:
   lea     eax, [ebp-20h]
   inc     dword [eax]
   jmp     loc_80BDA64
loc_80BDB06:
   cmp     byte [ebp-0Eh], 0
   jnz     loc_80BDBB1
   mov     dword [ebp-1Ch], 0
loc_80BDB17:
   mov     eax, [ebp+8]
   movzx   eax, byte [eax+19h]
   cmp     [ebp-1Ch], eax
   jl      loc_80BDB28
   jmp     loc_80BDDF8
loc_80BDB28:
   mov     edx, [ebp+20h]
   mov     eax, [ebp-1Ch]
   sar     eax, 5
   mov     ecx, [ebp-1Ch]
   and     ecx, 1Fh
   mov     eax, [edx+eax*4+4010h]
   sar     eax, cl
   and     eax, 1
   test    al, al
   jz      loc_80BDB49
   jmp     loc_80BDBA1
loc_80BDB49:
   mov     eax, [ebp+14h]
   fld     dword [eax+1Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80BDB5E
   jp      loc_80BDB5E
   jmp     loc_80BDBA1
loc_80BDB5E:
   mov     eax, [ebp+14h]
   fld1    
   fdiv    dword [eax+1Ch]
   fstp    dword [ebp-2Ch]
   mov     eax, [ebp+14h]
   mov     [esp+8], eax
   mov     eax, [ebp-2Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+14h]
   mov     [esp], eax
   call    Vector4Scale715
   mov     eax, [ebp+14h]
   add     eax, 10h
   mov     [esp+8], eax
   mov     eax, [ebp-2Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+14h]
   add     eax, 10h
   mov     [esp], eax
   call    VectorScale519
loc_80BDBA1:
   lea     eax, [ebp-1Ch]
   inc     dword [eax]
   lea     eax, [ebp+14h]
   add     dword [eax], 20h
   jmp     loc_80BDB17
loc_80BDBB1:
   cmp     byte [ebp-0Dh], 0
   jz      loc_80BDC8E
   mov     dword [ebp-1Ch], 0
loc_80BDBC2:
   mov     eax, [ebp+8]
   movzx   eax, byte [eax+19h]
   cmp     [ebp-1Ch], eax
   jl      loc_80BDBD3
   jmp     loc_80BDDF8
loc_80BDBD3:
   mov     edx, [ebp+20h]
   mov     eax, [ebp-1Ch]
   sar     eax, 5
   mov     ecx, [ebp-1Ch]
   and     ecx, 1Fh
   mov     eax, [edx+eax*4+4010h]
   sar     eax, cl
   and     eax, 1
   test    al, al
   jz      loc_80BDBF7
   jmp     loc_80BDC7E
loc_80BDBF7:
   mov     eax, [ebp+14h]
   mov     [esp], eax
   call    Vec4LengthSq612
   fstp    dword [ebp-2Ch]
   fld     dword [ebp-2Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80BDC17
   jp      loc_80BDC17
   jmp     loc_80BDC3B
loc_80BDC17:
   mov     eax, [ebp-2Ch]
   mov     [esp], eax
   call    XAnimCalc_GetLowestElement
   fmul    dword [ebp+10h]
   mov     eax, [ebp+14h]
   mov     [esp+8], eax
   fstp    dword [esp+4]
   mov     eax, [ebp+14h]
   mov     [esp], eax
   call    Vector4Scale715
loc_80BDC3B:
   mov     eax, [ebp+14h]
   fld     dword [eax+1Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80BDC50
   jp      loc_80BDC50
   jmp     loc_80BDC7E
loc_80BDC50:
   mov     eax, [ebp+14h]
   add     eax, 10h
   mov     [esp+8], eax
   mov     eax, [ebp+14h]
   fld     dword [ebp+10h]
   fdiv    dword [eax+1Ch]
   fstp    dword [esp+4]
   mov     eax, [ebp+14h]
   add     eax, 10h
   mov     [esp], eax
   call    VectorScale519
   mov     edx, [ebp+14h]
   mov     eax, [ebp+10h]
   mov     [edx+1Ch], eax
loc_80BDC7E:
   lea     eax, [ebp-1Ch]
   inc     dword [eax]
   lea     eax, [ebp+14h]
   add     dword [eax], 20h
   jmp     loc_80BDBC2
loc_80BDC8E:
   mov     dword [ebp-1Ch], 0
loc_80BDC95:
   mov     eax, [ebp+8]
   movzx   eax, byte [eax+19h]
   cmp     [ebp-1Ch], eax
   jl      loc_80BDCA6
   jmp     loc_80BDDF8
loc_80BDCA6:
   mov     edx, [ebp+20h]
   mov     eax, [ebp-1Ch]
   sar     eax, 5
   mov     ecx, [ebp-1Ch]
   and     ecx, 1Fh
   mov     eax, [edx+eax*4+4010h]
   sar     eax, cl
   and     eax, 1
   test    al, al
   jz      loc_80BDCCA
   jmp     loc_80BDD68
loc_80BDCCA:
   mov     eax, [ebp-28h]
   mov     [esp], eax
   call    Vec4LengthSq612
   fstp    dword [ebp-2Ch]
   fld     dword [ebp-2Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80BDCEA
   jp      loc_80BDCEA
   jmp     loc_80BDD15
loc_80BDCEA:
   mov     eax, [ebp-2Ch]
   mov     [esp], eax
   call    XAnimCalc_GetLowestElement
   fmul    dword [ebp+10h]
   mov     eax, [ebp+14h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp-28h]
   mov     [esp+8], eax
   fstp    dword [esp+4]
   mov     eax, [ebp+14h]
   mov     [esp], eax
   call    VectorMA4812
loc_80BDD15:
   mov     eax, [ebp-28h]
   fld     dword [eax+1Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80BDD2A
   jp      loc_80BDD2A
   jmp     loc_80BDD68
loc_80BDD2A:
   mov     eax, [ebp+14h]
   add     eax, 10h
   mov     [esp+0Ch], eax
   mov     eax, [ebp-28h]
   add     eax, 10h
   mov     [esp+8], eax
   mov     eax, [ebp-28h]
   fld     dword [ebp+10h]
   fdiv    dword [eax+1Ch]
   fstp    dword [esp+4]
   mov     eax, [ebp+14h]
   add     eax, 10h
   mov     [esp], eax
   call    VectorMA561
   mov     edx, [ebp+14h]
   mov     eax, [ebp+14h]
   fld     dword [eax+1Ch]
   fadd    dword [ebp+10h]
   fstp    dword [edx+1Ch]
loc_80BDD68:
   lea     eax, [ebp-1Ch]
   inc     dword [eax]
   lea     eax, [ebp+14h]
   add     dword [eax], 20h
   lea     eax, [ebp-28h]
   add     dword [eax], 20h
   jmp     loc_80BDC95
loc_80BDD7E:
   lea     eax, [ebp-20h]
   inc     dword [eax]
   jmp     loc_80BD93B
loc_80BDD88:
   mov     eax, [ebp+24h]
   mov     [esp+1Ch], eax
   mov     eax, [ebp+20h]
   mov     [esp+18h], eax
   movzx   eax, byte [ebp-0Eh]
   mov     [esp+14h], eax
   movzx   eax, byte [ebp-0Dh]
   mov     [esp+10h], eax
   mov     eax, [ebp+14h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+10h]
   mov     [esp+8], eax
   mov     eax, [ebp-34h]
   movzx   eax, word [eax+6]
   add     eax, [ebp-1Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XAnimCalc
   jmp     loc_80BDDF8
loc_80BDDCF:
   lea     eax, [ebp-1Ch]
   inc     dword [eax]
   jmp     loc_80BD8DA
loc_80BDDD9:
   cmp     byte [ebp-0Dh], 0
   jz      loc_80BDDF8
   mov     eax, [ebp+20h]
   mov     [esp+8], eax
   mov     eax, [ebp+14h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    XanimCalcClearInfo
loc_80BDDF8:
   add     esp, 6Ch
   pop     ebx
   pop     esi
   pop     edi
   pop     ebp
   retn    


section .rdata
aMaxCalcAnimBuf db 'MAX_CALC_ANIM_BUFFER exceeded',0Ah,0
flt_81492CC     dd 0.5
flt_81492C8     dd 1.5
flt_8148EB8     dd 0.000030518509
flt_81492C0     dd 0.000030518509
flt_81492C4     dd 0.000030518509
flt_8148EBC     dd 2.0
flt_8148EC0     dd 9.3137942e-10
flt_8148EC4     dd 0.000030518509