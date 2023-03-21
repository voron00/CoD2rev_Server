extern SL_GetStringOfLen


global XAnim_CalcDeltaForTime
global XAnim_CalcDelta3DForTime
global XAnim_CalcRotDeltaForQuats
global XAnim_CalcRotDeltaQuat
global XAnimSetDeltaQuatFrames
global XAnim_CalcPosDeltaForTrans
global XAnim_CalcPosDeltaTrans
global XAnim_CalcDynamicIndicesForQuats_s
global XAnim_CalcDynamicIndexForQuat_s
global XAnim_SetTime
global XAnimSetRotDeltaFramesForQuat
global XAnimGetFrameTime
global XAnimCalcDynamicFrameIndices_s
global XAnim_GetTimeIndex_s
global XAnim_CalcDynamicIndicesForTrans_s
global XAnim_CalcDynamicIndexForTrans_s
global XAnimSetDeltaTransFrames
global XAnim_CalcDynamicIndicesForQuats_b
global XAnim_CalcDynamicIndexForQuat_b
global XAnimCalcDynamicFrameIndices_b
global XAnim_GetTimeIndex_b
global XAnim_CalcDynamicIndicesForTrans_b
global XAnim_CalcDynamicIndexForTrans_b
global XAnimGetAbsDelta
global XAnimCalcParts
global XAnimCalcParts2
global XAnimCalcNonLoopEnd
global XAnimCalc_GetLowestElement
global XAnimSetModel


section .text


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
   call    sub_80C11C4
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
   call    sub_80C11C4
locret_80BAC34:
   leave   
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


XAnimCalcNonLoopEnd:
   push    ebp
   mov     ebp, esp
   sub     esp, 28h
   fld     dword [ebp+10h]
   fld     dword [flt_8148EC4]
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
   call    sub_80C11C4
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


XAnimCalcParts2:
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
   fld     dword [flt_8148EC4]
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
   call    sub_80C11C4
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


XAnimCalcParts:
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
   fld     dword [flt_8148EC4]
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
   call    XAnim_CalcDeltaForTime
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


XAnim_GetTimeIndex_s:
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
   call    XAnim_GetTimeIndex_s
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


XAnim_CalcDelta3DForTime:
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


sub_80C11C4:
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


XAnim_CalcDeltaForTime:
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
   call    XAnim_CalcDelta3DForTime
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
   call    sub_80C11C4
   leave   
   retn    


section .rdata
flt_8148EC4     dd 0.000030518509
flt_81492CC     dd 0.5
flt_81492C8     dd 1.5