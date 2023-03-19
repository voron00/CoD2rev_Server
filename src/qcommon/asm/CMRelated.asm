extern cm
extern sqrt

extern CM_SightTraceThroughBrush
extern CM_TestBoxInBrush


global VectorSubtract63262
global DotProduct123
global VectorMA712
global CM_Fabs3
global VectorCopy834
global VectorLengthSquared
global VectorScale715
global CM_Sqrt923
global Vec2Multiply271
global Vec3Normalize
global sub_80A8B5A
global Dot2Product824
global Vector2Subtract261
global VectorSet821
global DotProduct584
global sub_805D3EE
global sub_805D34E
global sub_805D434
global CM_Fabs
global CM_VectorCopy2
global Vector4Copy712
global sub_805D3CA
global I_fispos2


; // Recursion in a loop, idk how to compile that with a modern gcc
global CM_TraceThroughLeafBrushNode_r
global CM_PointContentsLeafBrushNode_r
global CM_SightTraceThroughLeafBrushNode_r
global CM_TestInLeafBrushNode_r


; // Way too many differences from CoD4x/black ops code, lots of fpu ops, hard to decompile.
global CM_PositionTestCapsuleInTriangle
global CM_TraceCapsuleThroughTriangle
global CM_TraceCapsuleThroughBorder
global CM_TraceThroughBrush


section .text


CM_TestInLeafBrushNode_r:
   push    ebp
   mov     ebp, esp
   sub     esp, 18h
loc_8059438:
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp+8]
   mov     eax, [eax+80h]
   and     eax, [edx+4]
   test    eax, eax
   jnz     loc_8059450
   jmp     locret_80595C8
loc_8059450:
   mov     eax, [ebp+0Ch]
   cmp     word [eax+2], 0
   jz      loc_8059519
   mov     eax, [ebp+0Ch]
   cmp     word [eax+2], 0
   jle     loc_80594EF
   mov     dword [ebp-4], 0
loc_8059473:
   mov     eax, [ebp+0Ch]
   movsx   eax, word [eax+2]
   cmp     [ebp-4], eax
   jl      loc_8059484
   jmp     locret_80595C8
loc_8059484:
   mov     ecx, [ebp+0Ch]
   mov     eax, [ebp-4]
   lea     edx, [eax+eax]
   mov     eax, [ecx+8]
   movzx   eax, word [eax+edx]
   mov     [ebp-8], eax
   mov     edx, [ebp-8]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 4
   add     eax, [cm+80h]
   mov     [ebp-0Ch], eax
   mov     edx, [ebp-0Ch]
   mov     eax, [ebp+8]
   mov     eax, [eax+80h]
   and     eax, [edx+0Ch]
   test    eax, eax
   jnz     loc_80594C1
   jmp     loc_80594E8
loc_80594C1:
   mov     eax, [ebp+10h]
   mov     [esp+8], eax
   mov     eax, [ebp-0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    CM_TestBoxInBrush
   mov     eax, [ebp+10h]
   cmp     byte [eax+22h], 0
   jz      loc_80594E8
   jmp     locret_80595C8
loc_80594E8:
   lea     eax, [ebp-4]
   inc     dword [eax]
   jmp     loc_8059473
loc_80594EF:
   mov     eax, [ebp+10h]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   add     eax, 14h
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    CM_TestInLeafBrushNode_r
   mov     eax, [ebp+10h]
   cmp     byte [eax+22h], 0
   jz      loc_8059519
   jmp     locret_80595C8
loc_8059519:
   mov     ecx, [ebp+8]
   mov     eax, [ebp+0Ch]
   movzx   eax, byte [eax]
   mov     edx, [ebp+0Ch]
   fld     dword [ecx+eax*4+68h]
   fld     dword [edx+8]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8059537
   jmp     loc_8059556
loc_8059537:
   mov     eax, [ebp+0Ch]
   movzx   edx, word [eax+10h]
   mov     eax, edx
   shl     eax, 2
   add     eax, edx
   lea     edx, 0[eax*4]
   lea     eax, [ebp+0Ch]
   add     [eax], edx
   jmp     loc_8059438
loc_8059556:
   mov     ecx, [ebp+8]
   mov     eax, [ebp+0Ch]
   movzx   eax, byte [eax]
   mov     edx, [ebp+0Ch]
   fld     dword [ecx+eax*4+74h]
   fld     dword [edx+8]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8059574
   jmp     loc_80595A9
loc_8059574:
   mov     eax, [ebp+10h]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   movzx   edx, word [eax+10h]
   mov     eax, edx
   shl     eax, 2
   add     eax, edx
   shl     eax, 2
   add     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    CM_TestInLeafBrushNode_r
   mov     eax, [ebp+10h]
   cmp     byte [eax+22h], 0
   jz      loc_80595A9
   jmp     locret_80595C8
loc_80595A9:
   mov     eax, [ebp+0Ch]
   movzx   edx, word [eax+12h]
   mov     eax, edx
   shl     eax, 2
   add     eax, edx
   lea     edx, 0[eax*4]
   lea     eax, [ebp+0Ch]
   add     [eax], edx
   jmp     loc_8059438
locret_80595C8:
   leave   
   retn    


CM_SightTraceThroughLeafBrushNode_r:
   push    ebp
   mov     ebp, esp
   sub     esp, 98h
   lea     eax, [ebp-48h]
   mov     [esp+4], eax
   mov     eax, [ebp+10h]
   mov     [esp], eax
   call    CM_VectorCopy2
loc_805BD09:
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp+8]
   mov     eax, [eax+80h]
   and     eax, [edx+4]
   test    eax, eax
   jnz     loc_805BD28
   mov     dword [ebp-74h], 0
   jmp     loc_805C0B7
loc_805BD28:
   mov     eax, [ebp+0Ch]
   cmp     word [eax+2], 0
   jz      loc_805BE06
   mov     eax, [ebp+0Ch]
   cmp     word [eax+2], 0
   jle     loc_805BDCF
   mov     dword [ebp-0Ch], 0
loc_805BD4B:
   mov     eax, [ebp+0Ch]
   movsx   eax, word [eax+2]
   cmp     [ebp-0Ch], eax
   jl      loc_805BD59
   jmp     loc_805BDC3
loc_805BD59:
   mov     ecx, [ebp+0Ch]
   mov     eax, [ebp-0Ch]
   lea     edx, [eax+eax]
   mov     eax, [ecx+8]
   movzx   eax, word [eax+edx]
   mov     [ebp-10h], eax
   mov     edx, [ebp-10h]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 4
   add     eax, [cm+80h]
   mov     [ebp-14h], eax
   mov     edx, [ebp-14h]
   mov     eax, [ebp+8]
   mov     eax, [eax+80h]
   and     eax, [edx+0Ch]
   test    eax, eax
   jnz     loc_805BD96
   jmp     loc_805BDBC
loc_805BD96:
   mov     eax, [ebp-14h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    CM_SightTraceThroughBrush
   mov     [ebp-18h], eax
   cmp     dword [ebp-18h], 0
   jz      loc_805BDBC
   mov     eax, [ebp-18h]
   mov     [ebp-74h], eax
   jmp     loc_805C0B7
loc_805BDBC:
   lea     eax, [ebp-0Ch]
   inc     dword [eax]
   jmp     loc_805BD4B
loc_805BDC3:
   mov     dword [ebp-74h], 0
   jmp     loc_805C0B7
loc_805BDCF:
   mov     eax, [ebp+14h]
   mov     [esp+0Ch], eax
   lea     eax, [ebp-48h]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   add     eax, 14h
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    CM_SightTraceThroughLeafBrushNode_r
   mov     [ebp-18h], eax
   cmp     dword [ebp-18h], 0
   jz      loc_805BE06
   mov     eax, [ebp-18h]
   mov     [ebp-74h], eax
   jmp     loc_805C0B7
loc_805BE06:
   mov     eax, [ebp+0Ch]
   movzx   eax, byte [eax]
   mov     edx, [ebp+0Ch]
   fld     dword [ebp+eax*4-48h]
   fsub    dword [edx+8]
   fstp    dword [ebp-20h]
   mov     eax, [ebp+0Ch]
   movzx   eax, byte [eax]
   lea     ecx, 0[eax*4]
   mov     eax, [ebp+14h]
   mov     edx, [ebp+0Ch]
   fld     dword [eax+ecx]
   fsub    dword [edx+8]
   fstp    dword [ebp-24h]
   mov     edx, [ebp+8]
   mov     eax, [ebp+0Ch]
   movzx   eax, byte [eax]
   fld     dword [edx+eax*4+5Ch]
   fld     dword [flt_813BAB8]
   faddp   st1, st0
   mov     eax, [ebp+0Ch]
   fsub    dword [eax+0Ch]
   fstp    dword [ebp-1Ch]
   mov     eax, [ebp-24h]
   mov     [esp+4], eax
   mov     eax, [ebp-20h]
   mov     [esp], eax
   call    sub_805D3CA
   fstp    dword [ebp-30h]
   mov     eax, [ebp-24h]
   mov     [esp+4], eax
   mov     eax, [ebp-20h]
   mov     [esp], eax
   call    sub_805D3EE
   fstp    dword [ebp-2Ch]
   fld     dword [ebp-2Ch]
   fld     dword [ebp-1Ch]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_805BE8E
   jmp     loc_805BED2
loc_805BE8E:
   mov     eax, [ebp-1Ch]
   mov     [ebp-7Ch], eax
   fld     dword [ebp-7Ch]
   fchs    
   fld     dword [ebp-30h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_805BEA7
   jmp     loc_805BEB3
loc_805BEA7:
   mov     dword [ebp-74h], 0
   jmp     loc_805C0B7
loc_805BEB3:
   mov     eax, [ebp+0Ch]
   movzx   edx, word [eax+10h]
   mov     eax, edx
   shl     eax, 2
   add     eax, edx
   lea     edx, 0[eax*4]
   lea     eax, [ebp+0Ch]
   add     [eax], edx
   jmp     loc_805BD09
loc_805BED2:
   mov     eax, [ebp-1Ch]
   mov     [ebp-7Ch], eax
   fld     dword [ebp-7Ch]
   fchs    
   fld     dword [ebp-30h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_805BEEB
   jmp     loc_805BF0A
loc_805BEEB:
   mov     eax, [ebp+0Ch]
   movzx   edx, word [eax+12h]
   mov     eax, edx
   shl     eax, 2
   add     eax, edx
   lea     edx, 0[eax*4]
   lea     eax, [ebp+0Ch]
   add     [eax], edx
   jmp     loc_805BD09
loc_805BF0A:
   fld     dword [ebp-24h]
   fsub    dword [ebp-20h]
   fstp    dword [ebp-6Ch]
   mov     eax, [ebp-6Ch]
   mov     [esp], eax
   call    CM_Fabs
   fstp    dword [ebp-70h]
   fld     dword [ebp-70h]
   fld     dword [flt_813BABC]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_805BF35
   jmp     loc_805BF86
loc_805BF35:
   mov     eax, [ebp-20h]
   mov     [esp+8], eax
   mov     eax, [ebp-20h]
   xor     eax, 80000000h
   mov     [esp+4], eax
   mov     eax, [ebp-6Ch]
   mov     [esp], eax
   call    sub_805D34E
   fstp    dword [ebp-28h]
   fld1    
   fdiv    dword [ebp-70h]
   fstp    dword [ebp-4Ch]
   fld     dword [ebp-28h]
   fsub    dword [ebp-1Ch]
   fmul    dword [ebp-4Ch]
   fstp    dword [ebp-58h]
   fld     dword [ebp-28h]
   fadd    dword [ebp-1Ch]
   fmul    dword [ebp-4Ch]
   fstp    dword [ebp-54h]
   mov     eax, [ebp-6Ch]
   mov     [esp], eax
   call    I_fispos2
   mov     [ebp-50h], eax
   jmp     loc_805BF9D
loc_805BF86:
   mov     dword [ebp-50h], 0
   mov     eax, 3F800000h
   mov     [ebp-54h], eax
   mov     eax, 0
   mov     [ebp-58h], eax
loc_805BF9D:
   mov     eax, 3F800000h
   mov     [esp+4], eax
   mov     eax, [ebp-54h]
   mov     [esp], eax
   call    sub_805D3EE
   fstp    dword [ebp-54h]
   mov     eax, [ebp+14h]
   fld     dword [eax]
   fsub    dword [ebp-48h]
   fmul    dword [ebp-54h]
   fld     dword [ebp-48h]
   faddp   st1, st0
   fstp    dword [ebp-68h]
   mov     eax, [ebp+14h]
   add     eax, 4
   fld     dword [eax]
   fsub    dword [ebp-44h]
   fmul    dword [ebp-54h]
   fld     dword [ebp-44h]
   faddp   st1, st0
   fstp    dword [ebp-64h]
   mov     eax, [ebp+14h]
   add     eax, 8
   fld     dword [eax]
   fsub    dword [ebp-40h]
   fmul    dword [ebp-54h]
   fld     dword [ebp-40h]
   faddp   st1, st0
   fstp    dword [ebp-60h]
   lea     eax, [ebp-68h]
   mov     [esp+0Ch], eax
   lea     eax, [ebp-48h]
   mov     [esp+8], eax
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp-50h]
   movzx   edx, word [edx+eax*2+10h]
   mov     eax, edx
   shl     eax, 2
   add     eax, edx
   shl     eax, 2
   add     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    CM_SightTraceThroughLeafBrushNode_r
   mov     [ebp-18h], eax
   cmp     dword [ebp-18h], 0
   jz      loc_805C039
   mov     eax, [ebp-18h]
   mov     [ebp-74h], eax
   jmp     loc_805C0B7
loc_805C039:
   mov     eax, 0
   mov     [esp+4], eax
   mov     eax, [ebp-58h]
   mov     [esp], eax
   call    sub_805D3CA
   fstp    dword [ebp-58h]
   mov     eax, [ebp+14h]
   fld     dword [eax]
   fsub    dword [ebp-48h]
   fmul    dword [ebp-58h]
   fld     dword [ebp-48h]
   faddp   st1, st0
   fstp    dword [ebp-48h]
   mov     eax, [ebp+14h]
   add     eax, 4
   fld     dword [eax]
   fsub    dword [ebp-44h]
   fmul    dword [ebp-58h]
   fld     dword [ebp-44h]
   faddp   st1, st0
   fstp    dword [ebp-44h]
   mov     eax, [ebp+14h]
   add     eax, 8
   fld     dword [eax]
   fsub    dword [ebp-40h]
   fmul    dword [ebp-58h]
   fld     dword [ebp-40h]
   faddp   st1, st0
   fstp    dword [ebp-40h]
   mov     edx, [ebp+0Ch]
   mov     eax, 1
   sub     eax, [ebp-50h]
   movzx   edx, word [edx+eax*2+10h]
   mov     eax, edx
   shl     eax, 2
   add     eax, edx
   lea     edx, 0[eax*4]
   lea     eax, [ebp+0Ch]
   add     [eax], edx
   jmp     loc_805BD09
loc_805C0B7:
   mov     eax, [ebp-74h]
   leave   
   retn    


I_fispos2:
   push    ebp
   mov     ebp, esp
   fld     dword [ebp+8]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   setnb   al
   movzx   eax, al
   pop     ebp
   retn    


sub_805D3CA:
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
   call    sub_805D34E
   leave   
   retn    


Vector4Copy712:
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
   mov     edx, [ebp+0Ch]
   add     edx, 0Ch
   mov     eax, [ebp+8]
   add     eax, 0Ch
   mov     eax, [eax]
   mov     [edx], eax
   pop     ebp
   retn    


CM_TraceThroughLeafBrushNode_r:
   push    ebp
   mov     ebp, esp
   sub     esp, 0A8h
   lea     eax, [ebp-48h]
   mov     [esp+4], eax
   mov     eax, [ebp+10h]
   mov     [esp], eax
   call    Vector4Copy712
loc_8059FF7:
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp+8]
   mov     eax, [eax+80h]
   and     eax, [edx+4]
   test    eax, eax
   jnz     loc_805A00F
   jmp     locret_805A3A2
loc_805A00F:
   mov     eax, [ebp+0Ch]
   cmp     word [eax+2], 0
   jz      loc_805A0C6
   mov     eax, [ebp+0Ch]
   cmp     word [eax+2], 0
   jle     loc_805A09C
   mov     dword [ebp-0Ch], 0
loc_805A02E:
   mov     eax, [ebp+0Ch]
   movsx   eax, word [eax+2]
   cmp     [ebp-0Ch], eax
   jl      loc_805A03F
   jmp     locret_805A3A2
loc_805A03F:
   mov     ecx, [ebp+0Ch]
   mov     eax, [ebp-0Ch]
   lea     edx, [eax+eax]
   mov     eax, [ecx+8]
   movzx   eax, word [eax+edx]
   mov     [ebp-10h], eax
   mov     edx, [ebp-10h]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 4
   add     eax, [cm+80h]
   mov     [ebp-14h], eax
   mov     edx, [ebp-14h]
   mov     eax, [ebp+8]
   mov     eax, [eax+80h]
   and     eax, [edx+0Ch]
   test    eax, eax
   jnz     loc_805A07C
   jmp     loc_805A095
loc_805A07C:
   mov     eax, [ebp+18h]
   mov     [esp+8], eax
   mov     eax, [ebp-14h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    CM_TraceThroughBrush
loc_805A095:
   lea     eax, [ebp-0Ch]
   inc     dword [eax]
   jmp     loc_805A02E
loc_805A09C:
   mov     eax, [ebp+18h]
   mov     [esp+10h], eax
   mov     eax, [ebp+14h]
   mov     [esp+0Ch], eax
   lea     eax, [ebp-48h]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   add     eax, 14h
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    CM_TraceThroughLeafBrushNode_r
loc_805A0C6:
   mov     eax, [ebp+0Ch]
   movzx   eax, byte [eax]
   mov     edx, [ebp+0Ch]
   fld     dword [ebp+eax*4-48h]
   fsub    dword [edx+8]
   fstp    dword [ebp-1Ch]
   mov     eax, [ebp+0Ch]
   movzx   eax, byte [eax]
   lea     ecx, 0[eax*4]
   mov     eax, [ebp+14h]
   mov     edx, [ebp+0Ch]
   fld     dword [eax+ecx]
   fsub    dword [edx+8]
   fstp    dword [ebp-20h]
   mov     edx, [ebp+8]
   mov     eax, [ebp+0Ch]
   movzx   eax, byte [eax]
   fld     dword [edx+eax*4+5Ch]
   fld     dword [flt_813BA90]
   faddp   st1, st0
   mov     eax, [ebp+0Ch]
   fsub    dword [eax+0Ch]
   fstp    dword [ebp-18h]
   mov     eax, [ebp-20h]
   mov     [esp+4], eax
   mov     eax, [ebp-1Ch]
   mov     [esp], eax
   call    sub_805D3CA
   fstp    dword [ebp-2Ch]
   mov     eax, [ebp-20h]
   mov     [esp+4], eax
   mov     eax, [ebp-1Ch]
   mov     [esp], eax
   call    sub_805D3EE
   fstp    dword [ebp-28h]
   fld     dword [ebp-28h]
   fld     dword [ebp-18h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_805A14E
   jmp     loc_805A188
loc_805A14E:
   mov     eax, [ebp-18h]
   mov     [ebp-7Ch], eax
   fld     dword [ebp-7Ch]
   fchs    
   fld     dword [ebp-2Ch]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     locret_805A3A2
   mov     eax, [ebp+0Ch]
   movzx   edx, word [eax+10h]
   mov     eax, edx
   shl     eax, 2
   add     eax, edx
   lea     edx, 0[eax*4]
   lea     eax, [ebp+0Ch]
   add     [eax], edx
   jmp     loc_8059FF7
loc_805A188:
   mov     eax, [ebp-18h]
   mov     [ebp-7Ch], eax
   fld     dword [ebp-7Ch]
   fchs    
   fld     dword [ebp-2Ch]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_805A1A1
   jmp     loc_805A1C0
loc_805A1A1:
   mov     eax, [ebp+0Ch]
   movzx   edx, word [eax+12h]
   mov     eax, edx
   shl     eax, 2
   add     eax, edx
   lea     edx, 0[eax*4]
   lea     eax, [ebp+0Ch]
   add     [eax], edx
   jmp     loc_8059FF7
loc_805A1C0:
   mov     eax, [ebp+18h]
   fld     dword [eax]
   fld     dword [ebp-3Ch]
   fucompp 
   fnstsw  ax
   sahf    
   jnb     locret_805A3A2
   fld     dword [ebp-20h]
   fsub    dword [ebp-1Ch]
   fstp    dword [ebp-6Ch]
   mov     eax, [ebp-6Ch]
   mov     [esp], eax
   call    CM_Fabs
   fstp    dword [ebp-70h]
   fld     dword [ebp-70h]
   fld     dword [flt_813BA94]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_805A1FE
   jmp     loc_805A24F
loc_805A1FE:
   mov     eax, [ebp-1Ch]
   mov     [esp+8], eax
   mov     eax, [ebp-1Ch]
   xor     eax, 80000000h
   mov     [esp+4], eax
   mov     eax, [ebp-6Ch]
   mov     [esp], eax
   call    sub_805D34E
   fstp    dword [ebp-24h]
   fld1    
   fdiv    dword [ebp-70h]
   fstp    dword [ebp-4Ch]
   fld     dword [ebp-24h]
   fsub    dword [ebp-18h]
   fmul    dword [ebp-4Ch]
   fstp    dword [ebp-58h]
   fld     dword [ebp-24h]
   fadd    dword [ebp-18h]
   fmul    dword [ebp-4Ch]
   fstp    dword [ebp-54h]
   mov     eax, [ebp-6Ch]
   mov     [esp], eax
   call    I_fispos2
   mov     [ebp-50h], eax
   jmp     loc_805A266
loc_805A24F:
   mov     dword [ebp-50h], 0
   mov     eax, 3F800000h
   mov     [ebp-54h], eax
   mov     eax, 0
   mov     [ebp-58h], eax
loc_805A266:
   mov     eax, 3F800000h
   mov     [esp+4], eax
   mov     eax, [ebp-54h]
   mov     [esp], eax
   call    sub_805D3EE
   fstp    dword [ebp-54h]
   mov     eax, [ebp+14h]
   fld     dword [eax]
   fsub    dword [ebp-48h]
   fmul    dword [ebp-54h]
   fld     dword [ebp-48h]
   faddp   st1, st0
   fstp    dword [ebp-68h]
   mov     eax, [ebp+14h]
   add     eax, 4
   fld     dword [eax]
   fsub    dword [ebp-44h]
   fmul    dword [ebp-54h]
   fld     dword [ebp-44h]
   faddp   st1, st0
   fstp    dword [ebp-64h]
   mov     eax, [ebp+14h]
   add     eax, 8
   fld     dword [eax]
   fsub    dword [ebp-40h]
   fmul    dword [ebp-54h]
   fld     dword [ebp-40h]
   faddp   st1, st0
   fstp    dword [ebp-60h]
   mov     eax, [ebp+14h]
   add     eax, 0Ch
   fld     dword [eax]
   fsub    dword [ebp-3Ch]
   fmul    dword [ebp-54h]
   fld     dword [ebp-3Ch]
   faddp   st1, st0
   fstp    dword [ebp-5Ch]
   mov     eax, [ebp+18h]
   mov     [esp+10h], eax
   lea     eax, [ebp-68h]
   mov     [esp+0Ch], eax
   lea     eax, [ebp-48h]
   mov     [esp+8], eax
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp-50h]
   movzx   edx, word [edx+eax*2+10h]
   mov     eax, edx
   shl     eax, 2
   add     eax, edx
   shl     eax, 2
   add     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    CM_TraceThroughLeafBrushNode_r
   mov     eax, 0
   mov     [esp+4], eax
   mov     eax, [ebp-58h]
   mov     [esp], eax
   call    sub_805D3CA
   fstp    dword [ebp-58h]
   mov     eax, [ebp+14h]
   fld     dword [eax]
   fsub    dword [ebp-48h]
   fmul    dword [ebp-58h]
   fld     dword [ebp-48h]
   faddp   st1, st0
   fstp    dword [ebp-48h]
   mov     eax, [ebp+14h]
   add     eax, 4
   fld     dword [eax]
   fsub    dword [ebp-44h]
   fmul    dword [ebp-58h]
   fld     dword [ebp-44h]
   faddp   st1, st0
   fstp    dword [ebp-44h]
   mov     eax, [ebp+14h]
   add     eax, 8
   fld     dword [eax]
   fsub    dword [ebp-40h]
   fmul    dword [ebp-58h]
   fld     dword [ebp-40h]
   faddp   st1, st0
   fstp    dword [ebp-40h]
   mov     eax, [ebp+14h]
   add     eax, 0Ch
   fld     dword [eax]
   fsub    dword [ebp-3Ch]
   fmul    dword [ebp-58h]
   fld     dword [ebp-3Ch]
   faddp   st1, st0
   fstp    dword [ebp-3Ch]
   mov     edx, [ebp+0Ch]
   mov     eax, 1
   sub     eax, [ebp-50h]
   movzx   edx, word [edx+eax*2+10h]
   mov     eax, edx
   shl     eax, 2
   add     eax, edx
   lea     edx, 0[eax*4]
   lea     eax, [ebp+0Ch]
   add     [eax], edx
   jmp     loc_8059FF7
locret_805A3A2:
   leave   
   retn    


CM_VectorCopy2:
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


CM_Fabs:
   push    ebp
   mov     ebp, esp
   sub     esp, 8
   fld     dword [ebp+8]
   fabs    
   fstp    dword [ebp-4]
   mov     eax, [ebp-4]
   mov     [ebp-8], eax
   fld     dword [ebp-8]
   leave   
   retn    


sub_805D434:
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


sub_805D34E:
   push    ebp
   mov     ebp, esp
   sub     esp, 4
   fld     dword [ebp+8]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_805D364
   jmp     loc_805D36C
loc_805D364:
   fld     dword [ebp+0Ch]
   fstp    dword [ebp-4]
   jmp     loc_805D372
loc_805D36C:
   fld     dword [ebp+10h]
   fstp    dword [ebp-4]
loc_805D372:
   fld     dword [ebp-4]
   leave   
   retn    


sub_805D3EE:
   push    ebp
   mov     ebp, esp
   sub     esp, 18h
   mov     eax, [ebp+0Ch]
   mov     [esp+8], eax
   mov     eax, [ebp+8]
   mov     [esp+4], eax
   fld     dword [ebp+0Ch]
   fsub    dword [ebp+8]
   fstp    dword [esp]
   call    sub_805D34E
   leave   
   retn    


CM_TraceThroughBrush:
   push    ebp
   mov     ebp, esp
   push    ebx
   sub     esp, 94h
   mov     eax, 0
   mov     [ebp-18h], eax
   mov     eax, [ebp+10h]
   mov     eax, [eax]
   mov     [ebp-1Ch], eax
   mov     dword [ebp-28h], 1
   mov     dword [ebp-34h], 0
   mov     eax, 0BF800000h
   mov     [ebp-40h], eax
   mov     eax, [ebp+0Ch]
   mov     [ebp-44h], eax
   mov     dword [ebp-7Ch], 0
loc_8059BF9:
   mov     dword [ebp-48h], 0
loc_8059C00:
   cmp     dword [ebp-48h], 2
   jle     loc_8059C0B
   jmp     loc_8059D96
loc_8059C0B:
   mov     ebx, [ebp+8]
   mov     ecx, [ebp-48h]
   mov     eax, [ebp-48h]
   lea     edx, 0[eax*4]
   mov     eax, [ebp-44h]
   fld     dword [ebx+ecx*4]
   fsub    dword [eax+edx]
   fmul    dword [ebp-40h]
   mov     edx, [ebp+8]
   mov     eax, [ebp-48h]
   fsub    dword [edx+eax*4+94h]
   fstp    dword [ebp-20h]
   mov     ebx, [ebp+8]
   mov     ecx, [ebp-48h]
   mov     eax, [ebp-48h]
   lea     edx, 0[eax*4]
   mov     eax, [ebp-44h]
   fld     dword [ebx+ecx*4+0Ch]
   fsub    dword [eax+edx]
   fmul    dword [ebp-40h]
   mov     edx, [ebp+8]
   mov     eax, [ebp-48h]
   fsub    dword [edx+eax*4+94h]
   fstp    dword [ebp-24h]
   fld     dword [ebp-20h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8059C77
   jmp     loc_8059D3C
loc_8059C77:
   mov     eax, 3E000000h
   mov     [esp+4], eax
   mov     eax, [ebp-20h]
   mov     [esp], eax
   call    sub_805D3EE
   fld     dword [ebp-24h]
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8059FD3
   fld     dword [ebp-20h]
   fld     dword [flt_813BA8C]
   fsubp   st1, st0
   mov     edx, [ebp+8]
   mov     eax, [ebp-48h]
   fmul    dword [edx+eax*4+18h]
   fmul    dword [ebp-40h]
   fstp    dword [ebp-80h]
   fld     dword [ebp-80h]
   fld     dword [ebp-1Ch]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8059FD3
   fld     dword [ebp-24h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8059CD7
   jmp     loc_8059CDE
loc_8059CD7:
   mov     dword [ebp-28h], 0
loc_8059CDE:
   fld     dword [ebp-80h]
   fld     dword [ebp-18h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8059CEF
   jmp     loc_8059CF7
loc_8059CEF:
   mov     eax, [ebp-80h]
   mov     [ebp-18h], eax
   jmp     loc_8059D02
loc_8059CF7:
   cmp     dword [ebp-34h], 0
   jz      loc_8059D02
   jmp     loc_8059D8C
loc_8059D02:
   mov     ecx, [ebp+0Ch]
   mov     edx, [ebp-7Ch]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   add     eax, [ebp-48h]
   movsx   eax, word [ecx+eax*2+24h]
   mov     [ebp-4Ch], eax
   lea     eax, [ebp-78h]
   mov     [esp], eax
   call    sub_805D434
   mov     edx, [ebp-48h]
   mov     eax, [ebp-40h]
   mov     [ebp+edx*4-78h], eax
   lea     eax, [ebp-78h]
   mov     [ebp-50h], eax
   lea     eax, [ebp-50h]
   mov     [ebp-34h], eax
   jmp     loc_8059D8C
loc_8059D3C:
   fld     dword [ebp-24h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8059D4C
   jmp     loc_8059D8C
loc_8059D4C:
   mov     edx, [ebp+8]
   mov     eax, [ebp-48h]
   fld     dword [ebp-20h]
   fmul    dword [edx+eax*4+18h]
   fmul    dword [ebp-40h]
   fstp    dword [ebp-80h]
   fld     dword [ebp-80h]
   fld     dword [ebp-18h]
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8059FD3
   mov     dword [ebp-28h], 0
   mov     eax, [ebp-80h]
   mov     [esp+4], eax
   mov     eax, [ebp-1Ch]
   mov     [esp], eax
   call    sub_805D3EE
   fstp    dword [ebp-1Ch]
loc_8059D8C:
   lea     eax, [ebp-48h]
   inc     dword [eax]
   jmp     loc_8059C00
loc_8059D96:
   cmp     dword [ebp-7Ch], 0
   jz      loc_8059D9E
   jmp     loc_8059DBB
loc_8059D9E:
   mov     eax, 3F800000h
   mov     [ebp-40h], eax
   mov     eax, [ebp+0Ch]
   add     eax, 10h
   mov     [ebp-44h], eax
   mov     dword [ebp-7Ch], 1
   jmp     loc_8059BF9
loc_8059DBB:
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+20h]
   mov     [ebp-30h], eax
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+1Ch]
   mov     [ebp-0Ch], eax
loc_8059DCD:
   cmp     dword [ebp-0Ch], 0
   jnz     loc_8059DD8
   jmp     loc_8059F43
loc_8059DD8:
   mov     eax, [ebp-30h]
   mov     eax, [eax]
   mov     [ebp-10h], eax
   mov     eax, [ebp-10h]
   mov     edx, [ebp+8]
   fld     dword [eax+8]
   fmul    dword [edx+90h]
   fstp    dword [esp]
   call    CM_Fabs
   fstp    dword [ebp-38h]
   mov     eax, [ebp-10h]
   mov     edx, [ebp+8]
   fld     dword [eax+0Ch]
   fadd    dword [edx+8Ch]
   fadd    dword [ebp-38h]
   fstp    dword [ebp-14h]
   mov     eax, [ebp-10h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    DotProduct123
   fsub    dword [ebp-14h]
   fstp    dword [ebp-20h]
   mov     eax, [ebp-10h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   add     eax, 0Ch
   mov     [esp], eax
   call    DotProduct123
   fsub    dword [ebp-14h]
   fstp    dword [ebp-24h]
   fld     dword [ebp-20h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8059E55
   jmp     loc_8059EE5
loc_8059E55:
   mov     eax, 3E000000h
   mov     [esp+4], eax
   mov     eax, [ebp-20h]
   mov     [esp], eax
   call    sub_805D3EE
   fld     dword [ebp-24h]
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8059FD3
   fld     dword [ebp-24h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8059E87
   jmp     loc_8059E8E
loc_8059E87:
   mov     dword [ebp-28h], 0
loc_8059E8E:
   fld     dword [ebp-20h]
   fsub    dword [ebp-24h]
   fstp    dword [ebp-3Ch]
   fld     dword [ebp-20h]
   fld     dword [flt_813BA8C]
   fsubp   st1, st0
   fstp    dword [ebp-2Ch]
   fld     dword [ebp-18h]
   fmul    dword [ebp-3Ch]
   fld     dword [ebp-2Ch]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8059EB7
   jmp     loc_8059ED5
loc_8059EB7:
   fld     dword [ebp-2Ch]
   fdiv    dword [ebp-3Ch]
   fstp    dword [ebp-18h]
   fld     dword [ebp-18h]
   fld     dword [ebp-1Ch]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8059FD3
   jmp     loc_8059EDD
loc_8059ED5:
   cmp     dword [ebp-34h], 0
   jz      loc_8059EDD
   jmp     loc_8059F33
loc_8059EDD:
   mov     eax, [ebp-30h]
   mov     [ebp-34h], eax
   jmp     loc_8059F33
loc_8059EE5:
   fld     dword [ebp-24h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8059EF5
   jmp     loc_8059F33
loc_8059EF5:
   fld     dword [ebp-20h]
   fsub    dword [ebp-24h]
   fstp    dword [ebp-3Ch]
   fld     dword [ebp-1Ch]
   fmul    dword [ebp-3Ch]
   fld     dword [ebp-20h]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8059F10
   jmp     loc_8059F2C
loc_8059F10:
   fld     dword [ebp-20h]
   fdiv    dword [ebp-3Ch]
   fstp    dword [ebp-1Ch]
   fld     dword [ebp-18h]
   fld     dword [ebp-1Ch]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8059FD3
loc_8059F2C:
   mov     dword [ebp-28h], 0
loc_8059F33:
   lea     eax, [ebp-0Ch]
   dec     dword [eax]
   lea     eax, [ebp-30h]
   add     dword [eax], 8
   jmp     loc_8059DCD
loc_8059F43:
   mov     eax, [ebp+10h]
   mov     edx, [ebp+0Ch]
   mov     edx, [edx+0Ch]
   mov     [eax+14h], edx
   cmp     dword [ebp-34h], 0
   jnz     loc_8059F75
   mov     eax, [ebp+10h]
   mov     byte [eax+23h], 1
   cmp     dword [ebp-28h], 0
   jz      loc_8059FD3
   mov     eax, [ebp+10h]
   mov     byte [eax+22h], 1
   mov     edx, [ebp+10h]
   mov     eax, 0
   mov     [edx], eax
   jmp     loc_8059FD3
loc_8059F75:
   mov     edx, [ebp+10h]
   mov     eax, [ebp-18h]
   mov     [edx], eax
   mov     eax, [ebp+10h]
   add     eax, 4
   mov     [esp+4], eax
   mov     eax, [ebp-34h]
   mov     eax, [eax]
   mov     [esp], eax
   call    CM_VectorCopy2
   mov     ecx, [ebp+10h]
   mov     eax, [ebp-34h]
   mov     edx, [eax+4]
   mov     eax, edx
   shl     eax, 3
   add     eax, edx
   lea     edx, 0[eax*8]
   mov     eax, [cm+10h]
   mov     eax, [eax+edx+40h]
   mov     [ecx+10h], eax
   mov     ecx, [ebp+10h]
   mov     eax, [ebp-34h]
   mov     edx, [eax+4]
   mov     eax, edx
   shl     eax, 3
   add     eax, edx
   shl     eax, 3
   add     eax, [cm+10h]
   mov     [ecx+18h], eax
loc_8059FD3:
   add     esp, 94h
   pop     ebx
   pop     ebp
   retn    


DotProduct584:
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


CM_PointContentsLeafBrushNode_r:
   push    ebp
   mov     ebp, esp
   push    ebx
   sub     esp, 34h
   mov     dword [ebp-1Ch], 0
loc_8058BE6:
   mov     eax, [ebp+0Ch]
   cmp     word [eax+2], 0
   jz      loc_8058D24
   mov     eax, [ebp+0Ch]
   cmp     word [eax+2], 0
   jle     loc_8058D08
   mov     dword [ebp-8], 0
loc_8058C09:
   mov     eax, [ebp+0Ch]
   movsx   eax, word [eax+2]
   cmp     [ebp-8], eax
   jl      loc_8058C1A
   jmp     loc_8058D00
loc_8058C1A:
   mov     ecx, [ebp+0Ch]
   mov     eax, [ebp-8]
   lea     edx, [eax+eax]
   mov     eax, [ecx+8]
   movzx   eax, word [eax+edx]
   mov     [ebp-0Ch], eax
   mov     edx, [ebp-0Ch]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 4
   add     eax, [cm+80h]
   mov     [ebp-10h], eax
   mov     dword [ebp-14h], 0
loc_8058C49:
   cmp     dword [ebp-14h], 2
   jle     loc_8058C51
   jmp     loc_8058C9F
loc_8058C51:
   mov     eax, [ebp-14h]
   lea     ebx, 0[eax*4]
   mov     edx, [ebp+8]
   mov     ecx, [ebp-10h]
   mov     eax, [ebp-14h]
   fld     dword [edx+ebx]
   fld     dword [ecx+eax*4]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8058CF6
   mov     eax, [ebp-14h]
   lea     ebx, 0[eax*4]
   mov     edx, [ebp+8]
   mov     ecx, [ebp-10h]
   mov     eax, [ebp-14h]
   fld     dword [edx+ebx]
   fld     dword [ecx+eax*4+10h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8058CF6
   lea     eax, [ebp-14h]
   inc     dword [eax]
   jmp     loc_8058C49
loc_8058C9F:
   mov     eax, [ebp-10h]
   mov     eax, [eax+20h]
   mov     [ebp-18h], eax
   mov     eax, [ebp-10h]
   mov     eax, [eax+1Ch]
   mov     [ebp-14h], eax
loc_8058CB1:
   cmp     dword [ebp-14h], 0
   jnz     loc_8058CB9
   jmp     loc_8058CEB
loc_8058CB9:
   mov     eax, [ebp-18h]
   mov     eax, [eax]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    DotProduct584
   mov     eax, [ebp-18h]
   mov     eax, [eax]
   fld     dword [eax+0Ch]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8058CF6
   lea     eax, [ebp-14h]
   dec     dword [eax]
   lea     eax, [ebp-18h]
   add     dword [eax], 8
   jmp     loc_8058CB1
loc_8058CEB:
   mov     eax, [ebp-10h]
   mov     edx, [eax+0Ch]
   lea     eax, [ebp-1Ch]
   or      [eax], edx
loc_8058CF6:
   lea     eax, [ebp-8]
   inc     dword [eax]
   jmp     loc_8058C09
loc_8058D00:
   mov     eax, [ebp-1Ch]
   mov     [ebp-20h], eax
   jmp     loc_8058D7F
loc_8058D08:
   mov     eax, [ebp+0Ch]
   add     eax, 14h
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    CM_PointContentsLeafBrushNode_r
   mov     edx, eax
   lea     eax, [ebp-1Ch]
   or      [eax], edx
loc_8058D24:
   mov     ecx, [ebp+0Ch]
   mov     [ebp-24h], ecx
   mov     eax, [ebp+0Ch]
   movzx   eax, byte [eax]
   lea     ecx, 0[eax*4]
   mov     eax, [ebp+8]
   mov     edx, [ebp+0Ch]
   fld     dword [eax+ecx]
   fld     dword [edx+8]
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8058D4C
   jmp     loc_8058D55
loc_8058D4C:
   mov     dword [ebp-28h], 2
   jmp     loc_8058D5C
loc_8058D55:
   mov     dword [ebp-28h], 0
loc_8058D5C:
   mov     ecx, [ebp-24h]
   mov     eax, [ebp-28h]
   movzx   edx, word [eax+ecx+10h]
   mov     eax, edx
   shl     eax, 2
   add     eax, edx
   lea     edx, 0[eax*4]
   lea     eax, [ebp+0Ch]
   add     [eax], edx
   jmp     loc_8058BE6
loc_8058D7F:
   mov     eax, [ebp-20h]
   add     esp, 34h
   pop     ebx
   pop     ebp
   retn    


VectorSet821:
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


Vector2Subtract261:
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
   pop     ebp
   retn    


CM_TraceCapsuleThroughBorder:
   push    ebp
   mov     ebp, esp
   sub     esp, 98h
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   add     eax, 24h
   mov     [esp], eax
   call    Dot2Product824
   fstp    dword [ebp-14h]
   fld     dword [ebp-14h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     locret_8057399
   mov     eax, [ebp+8]
   fld     dword [eax+8Ch]
   fld     dword [flt_813B858]
   faddp   st1, st0
   fstp    dword [ebp-0Ch]
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    Dot2Product824
   mov     eax, [ebp+0Ch]
   fsub    dword [eax+8]
   fstp    dword [ebp-10h]
   fld     dword [ebp-0Ch]
   fsub    dword [ebp-10h]
   fdiv    dword [ebp-14h]
   fstp    dword [ebp-18h]
   mov     eax, [ebp+10h]
   fld     dword [ebp-18h]
   fld     dword [eax]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     locret_8057399
   mov     eax, [ebp+8]
   fld     dword [ebp-18h]
   fmul    dword [eax+30h]
   mov     eax, [ebp-0Ch]
   mov     [ebp-7Ch], eax
   fld     dword [ebp-7Ch]
   fchs    
   fucompp 
   fnstsw  ax
   sahf    
   ja      locret_8057399
   lea     eax, [ebp-38h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+8]
   add     eax, 24h
   mov     [esp+8], eax
   mov     eax, [ebp-18h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    VectorMA712
   mov     eax, [ebp+0Ch]
   fld     dword [eax+4]
   fmul    dword [ebp-38h]
   mov     eax, [ebp+0Ch]
   fld     dword [eax]
   fmul    dword [ebp-34h]
   fsubp   st1, st0
   mov     eax, [ebp+0Ch]
   fsub    dword [eax+14h]
   fstp    dword [ebp-1Ch]
   fld     dword [ebp-1Ch]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8056F34
   jmp     loc_8057111
loc_8056F34:
   mov     eax, [ebp+0Ch]
   mov     edx, [ebp+0Ch]
   fld     dword [eax+4]
   fmul    dword [edx+14h]
   mov     eax, [ebp+0Ch]
   mov     edx, [ebp+0Ch]
   fld     dword [eax]
   fmul    dword [edx+8]
   faddp   st1, st0
   fstp    dword [ebp-68h]
   mov     eax, [ebp+0Ch]
   mov     edx, [ebp+0Ch]
   fld     dword [eax+4]
   fmul    dword [edx+8]
   mov     eax, [ebp+0Ch]
   mov     edx, [ebp+0Ch]
   fld     dword [eax]
   fmul    dword [edx+14h]
   fsubp   st1, st0
   fstp    dword [ebp-64h]
   lea     eax, [ebp-48h]
   mov     [esp+8], eax
   lea     eax, [ebp-68h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    Vector2Subtract261
   lea     eax, [ebp-48h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   add     eax, 24h
   mov     [esp], eax
   call    Dot2Product824
   fstp    dword [ebp-4Ch]
   fld     dword [ebp-4Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     locret_8057399
   lea     eax, [ebp-48h]
   mov     [esp+4], eax
   lea     eax, [ebp-48h]
   mov     [esp], eax
   call    Dot2Product824
   fstp    dword [ebp-50h]
   fld     dword [ebp-0Ch]
   fmul    dword [ebp-0Ch]
   fld     dword [ebp-50h]
   fsubrp  st1, st0
   fstp    dword [ebp-58h]
   fld     dword [ebp-58h]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8056FE3
   jmp     loc_8057076
loc_8056FE3:
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+0Ch]
   mov     [ebp-60h], eax
   mov     eax, [ebp+8]
   fld     dword [ebp-60h]
   fsub    dword [eax+8]
   fstp    dword [esp]
   call    CM_Fabs3
   mov     eax, [ebp+8]
   fld     dword [eax+90h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      locret_8057399
   mov     eax, 0
   mov     [esp+0Ch], eax
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+4]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     eax, [eax]
   mov     [esp+4], eax
   mov     eax, [ebp+10h]
   add     eax, 4
   mov     [esp], eax
   call    VectorSet821
   mov     edx, [ebp+10h]
   mov     eax, 0
   mov     [edx], eax
   mov     eax, [ebp+8]
   mov     edx, [ebp+8]
   fld     dword [eax+8Ch]
   fmul    dword [edx+8Ch]
   fld     dword [ebp-50h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_805706A
   jmp     locret_8057399
loc_805706A:
   mov     eax, [ebp+10h]
   mov     byte [eax+23h], 1
   jmp     locret_8057399
loc_8057076:
   fld     dword [ebp-4Ch]
   fmul    dword [ebp-4Ch]
   mov     eax, [ebp+8]
   fld     dword [eax+34h]
   fmul    dword [ebp-58h]
   fsubp   st1, st0
   fstp    dword [ebp-54h]
   fld     dword [ebp-54h]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      locret_8057399
   mov     eax, [ebp-4Ch]
   xor     eax, 80000000h
   mov     [ebp-6Ch], eax
   mov     eax, [ebp-54h]
   mov     [esp], eax
   call    CM_Sqrt923
   fsubr   dword [ebp-6Ch]
   mov     eax, [ebp+8]
   fdiv    dword [eax+34h]
   fstp    dword [ebp-18h]
   mov     eax, [ebp+10h]
   fld     dword [ebp-18h]
   fld     dword [eax]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     locret_8057399
   fld     dword [ebp-18h]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   jnb     locret_8057399
   lea     eax, [ebp-38h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+8]
   add     eax, 24h
   mov     [esp+8], eax
   mov     eax, [ebp-18h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    VectorMA712
   mov     eax, 0
   mov     [ebp-1Ch], eax
   jmp     loc_8057333
loc_8057111:
   mov     eax, [ebp+0Ch]
   fld     dword [ebp-1Ch]
   fld     dword [eax+18h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8057128
   jmp     loc_805731D
loc_8057128:
   mov     ecx, [ebp+0Ch]
   mov     eax, [ebp+0Ch]
   mov     edx, [ebp+0Ch]
   fld     dword [eax+14h]
   fadd    dword [edx+18h]
   fmul    dword [ecx+4]
   mov     eax, [ebp+0Ch]
   mov     edx, [ebp+0Ch]
   fld     dword [eax]
   fmul    dword [edx+8]
   faddp   st1, st0
   fstp    dword [ebp-68h]
   mov     eax, [ebp+0Ch]
   mov     edx, [ebp+0Ch]
   fld     dword [eax+4]
   fmul    dword [edx+8]
   mov     ecx, [ebp+0Ch]
   mov     eax, [ebp+0Ch]
   mov     edx, [ebp+0Ch]
   fld     dword [eax+14h]
   fadd    dword [edx+18h]
   fmul    dword [ecx]
   fsubp   st1, st0
   fstp    dword [ebp-64h]
   lea     eax, [ebp-48h]
   mov     [esp+8], eax
   lea     eax, [ebp-68h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    Vector2Subtract261
   lea     eax, [ebp-48h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   add     eax, 24h
   mov     [esp], eax
   call    Dot2Product824
   fstp    dword [ebp-4Ch]
   fld     dword [ebp-4Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     locret_8057399
   lea     eax, [ebp-48h]
   mov     [esp+4], eax
   lea     eax, [ebp-48h]
   mov     [esp], eax
   call    Dot2Product824
   fstp    dword [ebp-50h]
   fld     dword [ebp-0Ch]
   fmul    dword [ebp-0Ch]
   fld     dword [ebp-50h]
   fsubrp  st1, st0
   fstp    dword [ebp-58h]
   fld     dword [ebp-58h]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80571E3
   jmp     loc_8057284
loc_80571E3:
   mov     ecx, [ebp+0Ch]
   mov     eax, [ebp+0Ch]
   mov     edx, [ebp+0Ch]
   fld     dword [eax+10h]
   fmul    dword [edx+18h]
   fld     dword [ecx+0Ch]
   faddp   st1, st0
   fstp    dword [ebp-60h]
   mov     eax, [ebp+8]
   fld     dword [eax+8]
   fsub    dword [ebp-60h]
   fstp    dword [esp]
   call    CM_Fabs3
   mov     eax, [ebp+8]
   fld     dword [eax+90h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      locret_8057399
   mov     eax, 0
   mov     [esp+0Ch], eax
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+4]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     eax, [eax]
   mov     [esp+4], eax
   mov     eax, [ebp+10h]
   add     eax, 4
   mov     [esp], eax
   call    VectorSet821
   mov     edx, [ebp+10h]
   mov     eax, 0
   mov     [edx], eax
   mov     eax, [ebp+8]
   mov     edx, [ebp+8]
   fld     dword [eax+8Ch]
   fmul    dword [edx+8Ch]
   fld     dword [ebp-50h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8057278
   jmp     locret_8057399
loc_8057278:
   mov     eax, [ebp+10h]
   mov     byte [eax+23h], 1
   jmp     locret_8057399
loc_8057284:
   fld     dword [ebp-4Ch]
   fmul    dword [ebp-4Ch]
   mov     eax, [ebp+8]
   fld     dword [eax+34h]
   fmul    dword [ebp-58h]
   fsubp   st1, st0
   fstp    dword [ebp-54h]
   fld     dword [ebp-54h]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      locret_8057399
   mov     eax, [ebp-4Ch]
   xor     eax, 80000000h
   mov     [ebp-70h], eax
   mov     eax, [ebp-54h]
   mov     [esp], eax
   call    CM_Sqrt923
   fsubr   dword [ebp-70h]
   mov     eax, [ebp+8]
   fdiv    dword [eax+34h]
   fstp    dword [ebp-18h]
   mov     eax, [ebp+10h]
   fld     dword [ebp-18h]
   fld     dword [eax]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     locret_8057399
   fld     dword [ebp-18h]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   jnb     locret_8057399
   lea     eax, [ebp-38h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+8]
   add     eax, 24h
   mov     [esp+8], eax
   mov     eax, [ebp-18h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    VectorMA712
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+18h]
   mov     [ebp-1Ch], eax
   jmp     loc_8057333
loc_805731D:
   fld     dword [ebp-18h]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_805732B
   jmp     loc_8057333
loc_805732B:
   mov     eax, 0
   mov     [ebp-18h], eax
loc_8057333:
   mov     edx, [ebp+0Ch]
   mov     eax, [ebp+0Ch]
   fld     dword [ebp-1Ch]
   fmul    dword [eax+10h]
   fld     dword [edx+0Ch]
   faddp   st1, st0
   fstp    dword [ebp-3Ch]
   fld     dword [ebp-30h]
   fsub    dword [ebp-3Ch]
   fstp    dword [esp]
   call    CM_Fabs3
   mov     eax, [ebp+8]
   fld     dword [eax+90h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      locret_8057399
   mov     edx, [ebp+10h]
   mov     eax, [ebp-18h]
   mov     [edx], eax
   mov     eax, 0
   mov     [esp+0Ch], eax
   mov     eax, [ebp+0Ch]
   mov     eax, [eax+4]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     eax, [eax]
   mov     [esp+4], eax
   mov     eax, [ebp+10h]
   add     eax, 4
   mov     [esp], eax
   call    VectorSet821
locret_8057399:
   leave   
   retn    


Dot2Product824:
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


sub_80A8B5A:
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


Vec3Normalize962:
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
   mov     eax, [ebp+8]
   add     eax, 8
   mov     edx, [ebp+8]
   add     edx, 8
   fld     dword [eax]
   fmul    dword [edx]
   faddp   st1, st0
   fstp    dword [ebp-4]
   mov     eax, [ebp-4]
   mov     [esp], eax
   call    sub_80A8B5A
   fstp    dword [ebp-4]
   fld     dword [ebp-4]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnz     loc_80A2117
   jp      loc_80A2117
   jmp     loc_80A2152
loc_80A2117:
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
   mov     edx, [ebp+8]
   add     edx, 8
   mov     eax, [ebp+8]
   add     eax, 8
   fld     dword [eax]
   fmul    dword [ebp-8]
   fstp    dword [edx]
loc_80A2152:
   mov     eax, [ebp-4]
   mov     [ebp-0Ch], eax
   fld     dword [ebp-0Ch]
   leave   
   retn    


Vec2Multiply271:
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


CM_Sqrt923:
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


VectorScale715:
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


CM_TraceCapsuleThroughTriangle:
   push    ebp
   mov     ebp, esp
   sub     esp, 0E8h
   lea     eax, [ebp-0B8h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   add     eax, 0Ch
   mov     [esp], eax
   call    VectorCopy834
   fld     dword [ebp-0B0h]
   fsub    dword [ebp+10h]
   fstp    dword [ebp-0B0h]
   mov     eax, [ebp+8]
   fld     dword [eax+8Ch]
   fld     dword [flt_813B848]
   faddp   st1, st0
   fstp    dword [ebp-14h]
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   lea     eax, [ebp-0B8h]
   mov     [esp], eax
   call    DotProduct123
   mov     eax, [ebp+0Ch]
   fsub    dword [eax+0Ch]
   fstp    dword [ebp-1Ch]
   fld     dword [ebp-1Ch]
   fld     dword [ebp-14h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     locret_8056E40
   lea     eax, [ebp-0A8h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    VectorCopy834
   fld     dword [ebp-0A0h]
   fsub    dword [ebp+10h]
   fstp    dword [ebp-0A0h]
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   lea     eax, [ebp-0A8h]
   mov     [esp], eax
   call    DotProduct123
   mov     eax, [ebp+0Ch]
   fsub    dword [eax+0Ch]
   fstp    dword [ebp-18h]
   fld     dword [ebp-18h]
   fsub    dword [ebp-1Ch]
   fstp    dword [ebp-0CCh]
   fld     dword [ebp-0CCh]
   fld     dword [flt_813B84C]
   fucompp 
   fnstsw  ax
   sahf    
   jnb     locret_8056E40
   mov     eax, [ebp-14h]
   xor     eax, 80000000h
   mov     [ebp-10h], eax
   fld     dword [ebp-18h]
   fld     dword [ebp-10h]
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8056383
   jmp     loc_8056548
loc_8056383:
   fld     dword [ebp+10h]
   fadd    st0, st0
   fstp    dword [ebp-0C0h]
   mov     eax, [ebp+0Ch]
   fld     dword [ebp-0C0h]
   fmul    dword [eax+8]
   fld     dword [ebp-18h]
   faddp   st1, st0
   fstp    dword [ebp-0BCh]
   fld     dword [ebp-0BCh]
   fld     dword [ebp-10h]
   fucompp 
   fnstsw  ax
   sahf    
   jnb     locret_8056E40
   fld     dword [ebp-10h]
   fsub    dword [ebp-18h]
   mov     eax, [ebp+0Ch]
   fdiv    dword [eax+8]
   fstp    dword [ebp-20h]
   mov     eax, [ebp+0Ch]
   add     eax, 10h
   mov     [esp+4], eax
   lea     eax, [ebp-0A8h]
   mov     [esp], eax
   call    DotProduct123
   mov     eax, [ebp+0Ch]
   fsub    dword [eax+1Ch]
   fstp    dword [ebp-0C4h]
   mov     eax, [ebp+0Ch]
   add     eax, 20h
   mov     [esp+4], eax
   lea     eax, [ebp-0A8h]
   mov     [esp], eax
   call    DotProduct123
   mov     eax, [ebp+0Ch]
   fsub    dword [eax+2Ch]
   fstp    dword [ebp-0C8h]
   mov     eax, [ebp+0Ch]
   fld     dword [ebp-20h]
   fmul    dword [eax+18h]
   fld     dword [ebp-0C4h]
   faddp   st1, st0
   fstp    dword [ebp-3Ch]
   fld     dword [ebp-3Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8056434
   jmp     loc_8056494
loc_8056434:
   mov     eax, [ebp+0Ch]
   fld     dword [ebp-20h]
   fmul    dword [eax+28h]
   fld     dword [ebp-0C8h]
   faddp   st1, st0
   fstp    dword [ebp-40h]
   fld     dword [ebp-40h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8056458
   jmp     loc_8056494
loc_8056458:
   fld     dword [ebp-3Ch]
   fadd    dword [ebp-40h]
   fld1    
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8056469
   jmp     loc_8056494
loc_8056469:
   mov     eax, [ebp+14h]
   add     eax, 4
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    VectorCopy834
   mov     edx, [ebp+14h]
   mov     eax, 0
   mov     [edx], eax
   mov     eax, [ebp+14h]
   mov     byte [eax+23h], 1
   jmp     locret_8056E40
loc_8056494:
   fld     dword [ebp-0BCh]
   fld     dword [ebp-14h]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80564A6
   jmp     loc_80564B1
loc_80564A6:
   mov     eax, [ebp-0C0h]
   mov     [ebp-20h], eax
   jmp     loc_80564C0
loc_80564B1:
   fld     dword [ebp-14h]
   fsub    dword [ebp-18h]
   mov     eax, [ebp+0Ch]
   fdiv    dword [eax+8]
   fstp    dword [ebp-20h]
loc_80564C0:
   mov     eax, [ebp+0Ch]
   fld     dword [ebp-20h]
   fmul    dword [eax+18h]
   fld     dword [ebp-0C4h]
   faddp   st1, st0
   fstp    dword [ebp-3Ch]
   fld     dword [ebp-3Ch]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      locret_8056E40
   mov     eax, [ebp+0Ch]
   fld     dword [ebp-20h]
   fmul    dword [eax+28h]
   fld     dword [ebp-0C8h]
   faddp   st1, st0
   fstp    dword [ebp-40h]
   fld     dword [ebp-40h]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      locret_8056E40
   fld     dword [ebp-3Ch]
   fadd    dword [ebp-40h]
   fld1    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      locret_8056E40
   mov     eax, [ebp+14h]
   add     eax, 4
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    VectorCopy834
   mov     edx, [ebp+14h]
   mov     eax, 0
   mov     [edx], eax
   mov     eax, [ebp+14h]
   mov     byte [eax+23h], 1
   jmp     locret_8056E40
loc_8056548:
   fld     dword [ebp-18h]
   fsub    dword [ebp-14h]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8056559
   jmp     loc_8056578
loc_8056559:
   mov     eax, 0
   mov     [ebp-20h], eax
   lea     eax, [ebp-38h]
   mov     [esp+4], eax
   lea     eax, [ebp-0A8h]
   mov     [esp], eax
   call    VectorCopy834
   jmp     loc_80565C2
loc_8056578:
   fld     dword [ebp-18h]
   fsub    dword [ebp-14h]
   fdiv    dword [ebp-0CCh]
   fstp    dword [ebp-20h]
   mov     eax, [ebp+14h]
   fld     dword [ebp-20h]
   fld     dword [eax]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      locret_8056E40
   lea     eax, [ebp-38h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+8]
   add     eax, 24h
   mov     [esp+8], eax
   mov     eax, [ebp-20h]
   mov     [esp+4], eax
   lea     eax, [ebp-0A8h]
   mov     [esp], eax
   call    VectorMA712
loc_80565C2:
   mov     eax, [ebp+0Ch]
   add     eax, 10h
   mov     [esp+4], eax
   lea     eax, [ebp-38h]
   mov     [esp], eax
   call    DotProduct123
   mov     eax, [ebp+0Ch]
   fsub    dword [eax+1Ch]
   fstp    dword [ebp-3Ch]
   mov     eax, [ebp+0Ch]
   add     eax, 20h
   mov     [esp+4], eax
   lea     eax, [ebp-38h]
   mov     [esp], eax
   call    DotProduct123
   mov     eax, [ebp+0Ch]
   fsub    dword [eax+2Ch]
   fstp    dword [ebp-40h]
   fld     dword [ebp-3Ch]
   fadd    dword [ebp-40h]
   fld1    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   setnbe  al
   movzx   eax, al
   mov     [ebp-44h], eax
   mov     eax, [ebp-44h]
   mov     [ebp-0D0h], eax
   fld     dword [ebp-3Ch]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_805662D
   jmp     loc_8056634
loc_805662D:
   or      dword [ebp-0D0h], 2
loc_8056634:
   mov     eax, [ebp-0D0h]
   mov     [ebp-44h], eax
   mov     eax, [ebp-44h]
   mov     [ebp-0D4h], eax
   fld     dword [ebp-40h]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8056654
   jmp     loc_805665B
loc_8056654:
   or      dword [ebp-0D4h], 4
loc_805665B:
   mov     eax, [ebp-0D4h]
   mov     [ebp-44h], eax
   cmp     dword [ebp-44h], 0
   jnz     loc_80566AB
   mov     eax, [ebp+14h]
   add     eax, 4
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    VectorCopy834
   mov     edx, [ebp+14h]
   mov     eax, [ebp-20h]
   mov     [edx], eax
   mov     eax, [ebp+8]
   fld     dword [ebp-18h]
   fld     dword [eax+8Ch]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_805669F
   jmp     locret_8056E40
loc_805669F:
   mov     eax, [ebp+14h]
   mov     byte [eax+23h], 1
   jmp     locret_8056E40
loc_80566AB:
   mov     dword [ebp-0Ch], 0
loc_80566B2:
   cmp     dword [ebp-0Ch], 2
   jle     loc_80566BD
   jmp     locret_8056E40
loc_80566BD:
   movzx   ecx, byte [ebp-0Ch]
   mov     eax, [ebp-44h]
   sar     eax, cl
   and     eax, 1
   test    al, al
   jz      loc_8056B3D
   mov     eax, [ebp+0Ch]
   mov     edx, [ebp-0Ch]
   mov     eax, [eax+edx*4+3Ch]
   mov     [ebp-8Ch], eax
   cmp     dword [ebp-8Ch], 0
   jns     loc_80566EF
   jmp     loc_8056E36
loc_80566EF:
   mov     edx, [ebp+8]
   mov     eax, [ebp-8Ch]
   lea     ecx, 0[eax*4]
   mov     eax, [edx+0A4h]
   mov     edx, [ebp+8]
   mov     eax, [eax+ecx]
   cmp     eax, [edx+0A0h]
   jnz     loc_8056718
   jmp     loc_8056E36
loc_8056718:
   mov     edx, [ebp+8]
   mov     eax, [ebp-8Ch]
   lea     ecx, 0[eax*4]
   mov     edx, [edx+0A4h]
   mov     eax, [ebp+8]
   mov     eax, [eax+0A0h]
   mov     [edx+ecx], eax
   mov     edx, [ebp-8Ch]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 4
   add     eax, [cm+50h]
   mov     [ebp-90h], eax
   lea     eax, [ebp-58h]
   mov     [esp+8], eax
   mov     eax, [ebp-90h]
   mov     [esp+4], eax
   lea     eax, [ebp-0A8h]
   mov     [esp], eax
   call    VectorSubtract63262
   mov     eax, [ebp-90h]
   add     eax, 0Ch
   mov     [esp+4], eax
   lea     eax, [ebp-58h]
   mov     [esp], eax
   call    DotProduct123
   fstp    dword [ebp-78h]
   mov     eax, [ebp-90h]
   add     eax, 18h
   mov     [esp+4], eax
   lea     eax, [ebp-58h]
   mov     [esp], eax
   call    DotProduct123
   fstp    dword [ebp-74h]
   mov     eax, [ebp-90h]
   add     eax, 0Ch
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   add     eax, 24h
   mov     [esp], eax
   call    DotProduct123
   fstp    dword [ebp-68h]
   mov     eax, [ebp-90h]
   add     eax, 18h
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   add     eax, 24h
   mov     [esp], eax
   call    DotProduct123
   fstp    dword [ebp-64h]
   lea     eax, [ebp-78h]
   mov     [esp+4], eax
   lea     eax, [ebp-68h]
   mov     [esp], eax
   call    Dot2Product824
   fstp    dword [ebp-84h]
   fld     dword [ebp-84h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8056E36
   lea     eax, [ebp-78h]
   mov     [esp+4], eax
   lea     eax, [ebp-78h]
   mov     [esp], eax
   call    Dot2Product824
   fstp    dword [ebp-48h]
   fld     dword [ebp-14h]
   fmul    dword [ebp-14h]
   fld     dword [ebp-48h]
   fsubrp  st1, st0
   fstp    dword [ebp-88h]
   fld     dword [ebp-88h]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_805684D
   jmp     loc_8056990
loc_805684D:
   mov     eax, [ebp-90h]
   add     eax, 24h
   mov     [esp+4], eax
   lea     eax, [ebp-58h]
   mov     [esp], eax
   call    DotProduct123
   fstp    dword [ebp-70h]
   fld     dword [ebp-70h]
   fld     dword [flt_813B850]
   fsubp   st1, st0
   fstp    dword [esp]
   call    CM_Fabs3
   fld     dword [flt_813B850]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8056E36
   mov     eax, [ebp+14h]
   add     eax, 4
   mov     [esp+8], eax
   mov     eax, [ebp-78h]
   mov     [esp+4], eax
   mov     eax, [ebp-90h]
   add     eax, 0Ch
   mov     [esp], eax
   call    VectorScale715
   mov     eax, [ebp+14h]
   add     eax, 4
   mov     [esp+0Ch], eax
   mov     eax, [ebp-90h]
   add     eax, 18h
   mov     [esp+8], eax
   mov     eax, [ebp-74h]
   mov     [esp+4], eax
   mov     eax, [ebp+14h]
   add     eax, 4
   mov     [esp], eax
   call    VectorMA712
   mov     eax, [ebp+14h]
   add     eax, 4
   mov     [esp], eax
   call    Vec3Normalize962
   fstp    st0
   mov     eax, [ebp+0Ch]
   fld     dword [eax+8]
   fld     dword [flt_813B854]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8056903
   jmp     loc_8056957
loc_8056903:
   mov     eax, [ebp+14h]
   fld     dword [eax+0Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8056916
   jmp     loc_8056957
loc_8056916:
   mov     eax, [ebp+14h]
   fld     dword [eax+0Ch]
   fld     dword [flt_813B854]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_805692B
   jmp     loc_8056957
loc_805692B:
   fld     dword [ebp-0A0h]
   fld     dword [ebp-0B0h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8056942
   jmp     loc_8056957
loc_8056942:
   mov     eax, [ebp+14h]
   add     eax, 4
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    VectorCopy834
loc_8056957:
   mov     edx, [ebp+14h]
   mov     eax, 0
   mov     [edx], eax
   mov     eax, [ebp+8]
   mov     edx, [ebp+8]
   fld     dword [eax+8Ch]
   fmul    dword [edx+8Ch]
   fld     dword [ebp-48h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8056984
   jmp     locret_8056E40
loc_8056984:
   mov     eax, [ebp+14h]
   mov     byte [eax+23h], 1
   jmp     locret_8056E40
loc_8056990:
   lea     eax, [ebp-68h]
   mov     [esp], eax
   call    Vec2Multiply271
   fstp    dword [ebp-80h]
   fld     dword [ebp-84h]
   fmul    dword [ebp-84h]
   fld     dword [ebp-80h]
   fmul    dword [ebp-88h]
   fsubp   st1, st0
   fstp    dword [ebp-7Ch]
   fld     dword [ebp-7Ch]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8056E36
   mov     eax, [ebp-7Ch]
   mov     [esp], eax
   call    CM_Sqrt923
   fchs    
   fsub    dword [ebp-84h]
   fdiv    dword [ebp-80h]
   fstp    dword [ebp-20h]
   mov     eax, [ebp+14h]
   fld     dword [ebp-20h]
   fld     dword [eax]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80569F5
   jmp     loc_8056E36
loc_80569F5:
   lea     eax, [ebp-58h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+8]
   add     eax, 24h
   mov     [esp+8], eax
   mov     eax, [ebp-20h]
   mov     [esp+4], eax
   lea     eax, [ebp-58h]
   mov     [esp], eax
   call    VectorMA712
   mov     eax, [ebp-90h]
   add     eax, 24h
   mov     [esp+4], eax
   lea     eax, [ebp-58h]
   mov     [esp], eax
   call    DotProduct123
   fstp    dword [ebp-30h]
   fld     dword [ebp-30h]
   fld     dword [flt_813B850]
   fsubp   st1, st0
   fstp    dword [esp]
   call    CM_Fabs3
   fld     dword [flt_813B850]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8056E36
   fld     dword [ebp-20h]
   fmul    dword [ebp-68h]
   fadd    dword [ebp-78h]
   fdiv    dword [ebp-14h]
   fstp    dword [ebp-38h]
   fld     dword [ebp-20h]
   fmul    dword [ebp-64h]
   fadd    dword [ebp-74h]
   fdiv    dword [ebp-14h]
   fstp    dword [ebp-34h]
   mov     eax, [ebp+14h]
   add     eax, 4
   mov     [esp+8], eax
   mov     eax, [ebp-38h]
   mov     [esp+4], eax
   mov     eax, [ebp-90h]
   add     eax, 0Ch
   mov     [esp], eax
   call    VectorScale715
   mov     eax, [ebp+14h]
   add     eax, 4
   mov     [esp+0Ch], eax
   mov     eax, [ebp-90h]
   add     eax, 18h
   mov     [esp+8], eax
   mov     eax, [ebp-34h]
   mov     [esp+4], eax
   mov     eax, [ebp+14h]
   add     eax, 4
   mov     [esp], eax
   call    VectorMA712
   mov     eax, [ebp+0Ch]
   fld     dword [eax+8]
   fld     dword [flt_813B854]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8056ADC
   jmp     loc_8056B30
loc_8056ADC:
   mov     eax, [ebp+14h]
   fld     dword [eax+0Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8056AEF
   jmp     loc_8056B30
loc_8056AEF:
   mov     eax, [ebp+14h]
   fld     dword [eax+0Ch]
   fld     dword [flt_813B854]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8056B04
   jmp     loc_8056B30
loc_8056B04:
   fld     dword [ebp-0A0h]
   fld     dword [ebp-0B0h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8056B1B
   jmp     loc_8056B30
loc_8056B1B:
   mov     eax, [ebp+14h]
   add     eax, 4
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    VectorCopy834
loc_8056B30:
   mov     edx, [ebp+14h]
   mov     eax, [ebp-20h]
   mov     [edx], eax
   jmp     loc_8056E36
loc_8056B3D:
   mov     eax, [ebp+0Ch]
   mov     edx, [ebp-0Ch]
   mov     eax, [eax+edx*4+30h]
   mov     [ebp-94h], eax
   cmp     dword [ebp-94h], 0
   jns     loc_8056B5B
   jmp     loc_8056E36
loc_8056B5B:
   mov     edx, [ebp+8]
   mov     eax, [ebp-94h]
   lea     ecx, 0[eax*4]
   mov     eax, [edx+0A8h]
   mov     edx, [ebp+8]
   mov     eax, [eax+ecx]
   cmp     eax, [edx+0A0h]
   jnz     loc_8056B84
   jmp     loc_8056E36
loc_8056B84:
   mov     edx, [ebp+8]
   mov     eax, [ebp-94h]
   lea     ecx, 0[eax*4]
   mov     edx, [edx+0A8h]
   mov     eax, [ebp+8]
   mov     eax, [eax+0A0h]
   mov     [edx+ecx], eax
   mov     edx, [ebp-94h]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   add     eax, [cm+48h]
   mov     [ebp-98h], eax
   lea     eax, [ebp-58h]
   mov     [esp+8], eax
   mov     eax, [ebp-98h]
   mov     [esp+4], eax
   lea     eax, [ebp-0A8h]
   mov     [esp], eax
   call    VectorSubtract63262
   lea     eax, [ebp-58h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   add     eax, 24h
   mov     [esp], eax
   call    DotProduct123
   fstp    dword [ebp-84h]
   fld     dword [ebp-84h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8056E36
   lea     eax, [ebp-58h]
   mov     [esp+4], eax
   lea     eax, [ebp-58h]
   mov     [esp], eax
   call    DotProduct123
   fstp    dword [ebp-48h]
   fld     dword [ebp-14h]
   fmul    dword [ebp-14h]
   fld     dword [ebp-48h]
   fsubrp  st1, st0
   fstp    dword [ebp-88h]
   fld     dword [ebp-88h]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8056C4A
   jmp     loc_8056D1C
loc_8056C4A:
   mov     eax, [ebp-48h]
   mov     [esp], eax
   call    CM_Sqrt923
   fld1    
   fdivrp  st1, st0
   fstp    dword [ebp-20h]
   mov     eax, [ebp+14h]
   add     eax, 4
   mov     [esp+8], eax
   mov     eax, [ebp-20h]
   mov     [esp+4], eax
   lea     eax, [ebp-58h]
   mov     [esp], eax
   call    VectorScale715
   mov     eax, [ebp+0Ch]
   fld     dword [eax+8]
   fld     dword [flt_813B854]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8056C8F
   jmp     loc_8056CE3
loc_8056C8F:
   mov     eax, [ebp+14h]
   fld     dword [eax+0Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8056CA2
   jmp     loc_8056CE3
loc_8056CA2:
   mov     eax, [ebp+14h]
   fld     dword [eax+0Ch]
   fld     dword [flt_813B854]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8056CB7
   jmp     loc_8056CE3
loc_8056CB7:
   fld     dword [ebp-0A0h]
   fld     dword [ebp-0B0h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8056CCE
   jmp     loc_8056CE3
loc_8056CCE:
   mov     eax, [ebp+14h]
   add     eax, 4
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    VectorCopy834
loc_8056CE3:
   mov     edx, [ebp+14h]
   mov     eax, 0
   mov     [edx], eax
   mov     eax, [ebp+8]
   mov     edx, [ebp+8]
   fld     dword [eax+8Ch]
   fmul    dword [edx+8Ch]
   fld     dword [ebp-48h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8056D10
   jmp     locret_8056E40
loc_8056D10:
   mov     eax, [ebp+14h]
   mov     byte [eax+23h], 1
   jmp     locret_8056E40
loc_8056D1C:
   mov     eax, [ebp+8]
   mov     eax, [eax+34h]
   mov     [ebp-80h], eax
   fld     dword [ebp-84h]
   fmul    dword [ebp-84h]
   fld     dword [ebp-80h]
   fmul    dword [ebp-88h]
   fsubp   st1, st0
   fstp    dword [ebp-7Ch]
   fld     dword [ebp-7Ch]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8056E36
   mov     eax, [ebp-7Ch]
   mov     [esp], eax
   call    CM_Sqrt923
   fchs    
   fsub    dword [ebp-84h]
   fdiv    dword [ebp-80h]
   fstp    dword [ebp-20h]
   mov     eax, [ebp+14h]
   fld     dword [ebp-20h]
   fld     dword [eax]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8056D7C
   jmp     loc_8056E36
loc_8056D7C:
   mov     eax, [ebp+14h]
   add     eax, 4
   mov     [esp+0Ch], eax
   mov     eax, [ebp+8]
   add     eax, 24h
   mov     [esp+8], eax
   mov     eax, [ebp-20h]
   mov     [esp+4], eax
   lea     eax, [ebp-58h]
   mov     [esp], eax
   call    VectorMA712
   mov     eax, [ebp+14h]
   add     eax, 4
   mov     [esp+8], eax
   fld1    
   fdiv    dword [ebp-14h]
   fstp    dword [esp+4]
   mov     eax, [ebp+14h]
   add     eax, 4
   mov     [esp], eax
   call    VectorScale715
   mov     eax, [ebp+0Ch]
   fld     dword [eax+8]
   fld     dword [flt_813B854]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8056DDA
   jmp     loc_8056E2E
loc_8056DDA:
   mov     eax, [ebp+14h]
   fld     dword [eax+0Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8056DED
   jmp     loc_8056E2E
loc_8056DED:
   mov     eax, [ebp+14h]
   fld     dword [eax+0Ch]
   fld     dword [flt_813B854]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8056E02
   jmp     loc_8056E2E
loc_8056E02:
   fld     dword [ebp-0A0h]
   fld     dword [ebp-0B0h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_8056E19
   jmp     loc_8056E2E
loc_8056E19:
   mov     eax, [ebp+14h]
   add     eax, 4
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    VectorCopy834
loc_8056E2E:
   mov     edx, [ebp+14h]
   mov     eax, [ebp-20h]
   mov     [edx], eax
loc_8056E36:
   lea     eax, [ebp-0Ch]
   inc     dword [eax]
   jmp     loc_80566B2
locret_8056E40:
   leave   
   retn    


VectorLengthSquared:
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


VectorCopy834:
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


CM_Fabs3:
   push    ebp
   mov     ebp, esp
   sub     esp, 8
   fld     dword [ebp+8]
   fabs    
   fstp    dword [ebp-4]
   mov     eax, [ebp-4]
   mov     [ebp-8], eax
   fld     dword [ebp-8]
   leave   
   retn    


VectorMA712:
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


DotProduct123:
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


VectorSubtract63262:
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


CM_PositionTestCapsuleInTriangle:
   push    ebp
   mov     ebp, esp
   sub     esp, 0C8h
   mov     eax, [ebp+8]
   mov     eax, [eax+90h]
   mov     [ebp-0A0h], eax
   mov     eax, [ebp+0Ch]
   fld     dword [eax+8]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80553DB
   jmp     loc_80553E5
loc_80553DB:
   lea     eax, [ebp-0A0h]
   xor     byte [eax+3], 80h
loc_80553E5:
   lea     eax, [ebp-88h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    VectorCopy834
   fld     dword [ebp-80h]
   fsub    dword [ebp-0A0h]
   fstp    dword [ebp-80h]
   mov     eax, [ebp+8]
   mov     eax, [eax+8Ch]
   mov     [ebp-64h], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   lea     eax, [ebp-88h]
   mov     [esp], eax
   call    DotProduct123
   mov     eax, [ebp+0Ch]
   fsub    dword [eax+0Ch]
   fstp    dword [ebp-90h]
   fld     dword [ebp-90h]
   fld     dword [ebp-64h]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     locret_80558EA
   mov     eax, [ebp-64h]
   xor     eax, 80000000h
   mov     [ebp-60h], eax
   fld     dword [ebp-90h]
   fld     dword [ebp-60h]
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8055469
   jmp     loc_8055615
loc_8055469:
   fld     dword [ebp-0A0h]
   fadd    st0, st0
   fstp    dword [ebp-8Ch]
   mov     eax, [ebp+0Ch]
   fld     dword [ebp-8Ch]
   fmul    dword [eax+8]
   fld     dword [ebp-90h]
   faddp   st1, st0
   fstp    dword [ebp-6Ch]
   fld     dword [ebp-6Ch]
   fld     dword [ebp-60h]
   fucompp 
   fnstsw  ax
   sahf    
   jnb     locret_80558EA
   fld     dword [ebp-60h]
   fsub    dword [ebp-90h]
   mov     eax, [ebp+0Ch]
   fdiv    dword [eax+8]
   fstp    dword [ebp-68h]
   mov     eax, [ebp+0Ch]
   add     eax, 10h
   mov     [esp+4], eax
   lea     eax, [ebp-88h]
   mov     [esp], eax
   call    DotProduct123
   mov     eax, [ebp+0Ch]
   fsub    dword [eax+1Ch]
   fstp    dword [ebp-94h]
   mov     eax, [ebp+0Ch]
   add     eax, 20h
   mov     [esp+4], eax
   lea     eax, [ebp-88h]
   mov     [esp], eax
   call    DotProduct123
   mov     eax, [ebp+0Ch]
   fsub    dword [eax+2Ch]
   fstp    dword [ebp-98h]
   mov     eax, [ebp+0Ch]
   fld     dword [ebp-68h]
   fmul    dword [eax+18h]
   fld     dword [ebp-94h]
   faddp   st1, st0
   fstp    dword [ebp-2Ch]
   fld     dword [ebp-2Ch]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_805551D
   jmp     loc_805556F
loc_805551D:
   mov     eax, [ebp+0Ch]
   fld     dword [ebp-68h]
   fmul    dword [eax+28h]
   fld     dword [ebp-98h]
   faddp   st1, st0
   fstp    dword [ebp-30h]
   fld     dword [ebp-30h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8055541
   jmp     loc_805556F
loc_8055541:
   fld     dword [ebp-2Ch]
   fadd    dword [ebp-30h]
   fld1    
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_8055552
   jmp     loc_805556F
loc_8055552:
   mov     eax, [ebp+10h]
   mov     byte [eax+23h], 1
   mov     eax, [ebp+10h]
   mov     byte [eax+22h], 1
   mov     edx, [ebp+10h]
   mov     eax, 0
   mov     [edx], eax
   jmp     locret_80558EA
loc_805556F:
   fld     dword [ebp-6Ch]
   fld     dword [ebp-64h]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_805557E
   jmp     loc_8055589
loc_805557E:
   mov     eax, [ebp-8Ch]
   mov     [ebp-68h], eax
   jmp     loc_805559B
loc_8055589:
   fld     dword [ebp-64h]
   fsub    dword [ebp-90h]
   mov     eax, [ebp+0Ch]
   fdiv    dword [eax+8]
   fstp    dword [ebp-68h]
loc_805559B:
   mov     eax, [ebp+0Ch]
   fld     dword [ebp-68h]
   fmul    dword [eax+18h]
   fld     dword [ebp-94h]
   faddp   st1, st0
   fstp    dword [ebp-2Ch]
   fld     dword [ebp-2Ch]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      locret_80558EA
   mov     eax, [ebp+0Ch]
   fld     dword [ebp-68h]
   fmul    dword [eax+28h]
   fld     dword [ebp-98h]
   faddp   st1, st0
   fstp    dword [ebp-30h]
   fld     dword [ebp-30h]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      locret_80558EA
   fld     dword [ebp-2Ch]
   fadd    dword [ebp-30h]
   fld1    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      locret_80558EA
   mov     eax, [ebp+10h]
   mov     byte [eax+23h], 1
   mov     eax, [ebp+10h]
   mov     byte [eax+22h], 1
   mov     edx, [ebp+10h]
   mov     eax, 0
   mov     [edx], eax
   jmp     locret_80558EA
loc_8055615:
   mov     dword [ebp-34h], 0
   lea     eax, [ebp-28h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+8], eax
   mov     eax, [ebp-90h]
   xor     eax, 80000000h
   mov     [esp+4], eax
   lea     eax, [ebp-88h]
   mov     [esp], eax
   call    VectorMA712
   mov     eax, [ebp+0Ch]
   add     eax, 10h
   mov     [esp+4], eax
   lea     eax, [ebp-28h]
   mov     [esp], eax
   call    DotProduct123
   mov     eax, [ebp+0Ch]
   fsub    dword [eax+1Ch]
   fstp    dword [ebp-2Ch]
   mov     eax, [ebp+0Ch]
   add     eax, 20h
   mov     [esp+4], eax
   lea     eax, [ebp-28h]
   mov     [esp], eax
   call    DotProduct123
   mov     eax, [ebp+0Ch]
   fsub    dword [eax+2Ch]
   fstp    dword [ebp-30h]
   mov     eax, [ebp-34h]
   mov     [ebp-0A4h], eax
   fld     dword [ebp-2Ch]
   fadd    dword [ebp-30h]
   fld1    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_805569F
   jmp     loc_80556A6
loc_805569F:
   or      dword [ebp-0A4h], 1
loc_80556A6:
   mov     eax, [ebp-0A4h]
   mov     [ebp-34h], eax
   mov     eax, [ebp-34h]
   mov     [ebp-0A8h], eax
   fld     dword [ebp-2Ch]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80556C6
   jmp     loc_80556CD
loc_80556C6:
   or      dword [ebp-0A8h], 2
loc_80556CD:
   mov     eax, [ebp-0A8h]
   mov     [ebp-34h], eax
   mov     eax, [ebp-34h]
   mov     [ebp-0ACh], eax
   fld     dword [ebp-30h]
   fldz    
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80556ED
   jmp     loc_80556F4
loc_80556ED:
   or      dword [ebp-0ACh], 4
loc_80556F4:
   mov     eax, [ebp-0ACh]
   mov     [ebp-34h], eax
   cmp     dword [ebp-34h], 0
   jnz     loc_8055720
   mov     eax, [ebp+10h]
   mov     byte [eax+23h], 1
   mov     eax, [ebp+10h]
   mov     byte [eax+22h], 1
   mov     edx, [ebp+10h]
   mov     eax, 0
   mov     [edx], eax
   jmp     locret_80558EA
loc_8055720:
   mov     dword [ebp-0Ch], 0
loc_8055727:
   cmp     dword [ebp-0Ch], 2
   jle     loc_8055732
   jmp     locret_80558EA
loc_8055732:
   movzx   ecx, byte [ebp-0Ch]
   mov     eax, [ebp-34h]
   sar     eax, cl
   and     eax, 1
   test    al, al
   jz      loc_805585A
   mov     eax, [ebp+0Ch]
   mov     edx, [ebp-0Ch]
   mov     eax, [eax+edx*4+3Ch]
   mov     [ebp-50h], eax
   cmp     dword [ebp-50h], 0
   jns     loc_805575E
   jmp     loc_80558E0
loc_805575E:
   mov     edx, [ebp-50h]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 4
   add     eax, [cm+50h]
   mov     [ebp-54h], eax
   lea     eax, [ebp-48h]
   mov     [esp+8], eax
   mov     eax, [ebp-54h]
   mov     [esp+4], eax
   lea     eax, [ebp-88h]
   mov     [esp], eax
   call    VectorSubtract63262
   mov     eax, [ebp-54h]
   add     eax, 24h
   mov     [esp+4], eax
   lea     eax, [ebp-48h]
   mov     [esp], eax
   call    DotProduct123
   fstp    dword [ebp-4Ch]
   fld     dword [ebp-4Ch]
   fld     dword [flt_813B834]
   fsubp   st1, st0
   fstp    dword [esp]
   call    CM_Fabs3
   fld     dword [flt_813B834]
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_80557CC
   jmp     loc_80558E0
loc_80557CC:
   mov     eax, [ebp-54h]
   add     eax, 24h
   mov     [esp], eax
   call    VectorLengthSquared
   fstp    dword [ebp-9Ch]
   fld     dword [ebp-4Ch]
   fdiv    dword [ebp-9Ch]
   fstp    dword [ebp-4Ch]
   lea     eax, [ebp-48h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp-54h]
   add     eax, 24h
   mov     [esp+8], eax
   mov     eax, [ebp-4Ch]
   xor     eax, 80000000h
   mov     [esp+4], eax
   lea     eax, [ebp-48h]
   mov     [esp], eax
   call    VectorMA712
   lea     eax, [ebp-48h]
   mov     [esp], eax
   call    VectorLengthSquared
   mov     eax, [ebp+8]
   mov     edx, [ebp+8]
   fld     dword [eax+8Ch]
   fmul    dword [edx+8Ch]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_805583D
   jmp     loc_80558E0
loc_805583D:
   mov     eax, [ebp+10h]
   mov     byte [eax+23h], 1
   mov     eax, [ebp+10h]
   mov     byte [eax+22h], 1
   mov     edx, [ebp+10h]
   mov     eax, 0
   mov     [edx], eax
   jmp     locret_80558EA
loc_805585A:
   mov     eax, [ebp+0Ch]
   mov     edx, [ebp-0Ch]
   mov     eax, [eax+edx*4+30h]
   mov     [ebp-58h], eax
   cmp     dword [ebp-58h], 0
   jns     loc_805586F
   jmp     loc_80558E0
loc_805586F:
   mov     edx, [ebp-58h]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   shl     eax, 2
   add     eax, [cm+48h]
   mov     [ebp-5Ch], eax
   lea     eax, [ebp-48h]
   mov     [esp+8], eax
   mov     eax, [ebp-5Ch]
   mov     [esp+4], eax
   lea     eax, [ebp-88h]
   mov     [esp], eax
   call    VectorSubtract63262
   lea     eax, [ebp-48h]
   mov     [esp], eax
   call    VectorLengthSquared
   mov     eax, [ebp+8]
   mov     edx, [ebp+8]
   fld     dword [eax+8Ch]
   fmul    dword [edx+8Ch]
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_80558C6
   jmp     loc_80558E0
loc_80558C6:
   mov     eax, [ebp+10h]
   mov     byte [eax+23h], 1
   mov     eax, [ebp+10h]
   mov     byte [eax+22h], 1
   mov     edx, [ebp+10h]
   mov     eax, 0
   mov     [edx], eax
   jmp     locret_80558EA
loc_80558E0:
   lea     eax, [ebp-0Ch]
   inc     dword [eax]
   jmp     loc_8055727
locret_80558EA:
   leave   
   retn    


section .rdata
flt_813B834     dd 0.5
flt_813B854     dd 0.69999999
flt_813B848     dd 0.125
flt_813B84C     dd 0.000099999997
flt_813B850     dd 0.5
flt_813B858     dd 0.125
flt_813BA8C     dd 0.125
flt_813BA90     dd 0.125
flt_813BA94     dd 4.7683716e-7
flt_813BAA4     dd 0.125
flt_813BAA8     dd 4.7683716e-7
flt_813BACC     dd 0.125
flt_813BAD0     dd 4.7683716e-7
flt_813BAB8     dd 0.125
flt_813BABC     dd 4.7683716e-7