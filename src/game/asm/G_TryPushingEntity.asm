extern G_TestEntityPosition
extern pushed_p
extern sin
extern cos


global G_TryPushingEntity


section .text


sub_810CA2C:
   push    ebp
   mov     ebp, esp
   push    ebx
   sub     esp, 8
   mov     dword [ebp-8], 0
loc_810CA3A:
   cmp     dword [ebp-8], 2
   jle     loc_810CA42
   jmp     loc_810CA91
loc_810CA42:
   mov     dword [ebp-0Ch], 0
loc_810CA49:
   cmp     dword [ebp-0Ch], 2
   jle     loc_810CA51
   jmp     loc_810CA8A
loc_810CA51:
   mov     edx, [ebp-8]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   add     eax, [ebp-0Ch]
   lea     ebx, 0[eax*4]
   mov     ecx, [ebp+0Ch]
   mov     edx, [ebp-0Ch]
   mov     eax, edx
   add     eax, eax
   add     eax, edx
   add     eax, [ebp-8]
   lea     edx, 0[eax*4]
   mov     eax, [ebp+8]
   mov     eax, [eax+edx]
   mov     [ecx+ebx], eax
   lea     eax, [ebp-0Ch]
   inc     dword [eax]
   jmp     loc_810CA49
loc_810CA8A:
   lea     eax, [ebp-8]
   inc     dword [eax]
   jmp     loc_810CA3A
loc_810CA91:
   add     esp, 8
   pop     ebx
   pop     ebp
   retn    


sub_810DC22:
   push    ebp
   mov     ebp, esp
   mov     edx, [ebp+8]
   mov     eax, [ebp+8]
   mov     eax, [eax]
   xor     eax, 80000000h
   mov     [edx], eax
   mov     edx, [ebp+8]
   add     edx, 4
   mov     eax, [ebp+8]
   add     eax, 4
   mov     eax, [eax]
   xor     eax, 80000000h
   mov     [edx], eax
   mov     edx, [ebp+8]
   add     edx, 8
   mov     eax, [ebp+8]
   add     eax, 8
   mov     eax, [eax]
   xor     eax, 80000000h
   mov     [edx], eax
   pop     ebp
   retn    


sub_80A8C1C:
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


AngleVectors:
   push    ebp
   mov     ebp, esp
   sub     esp, 38h
   mov     eax, [ebp+8]
   add     eax, 4
   fld     dword [eax]
   fld     qword [dbl_8145F88]
   fmulp   st1, st0
   fstp    dword [ebp-4]
   lea     eax, [ebp-1Ch]
   mov     [esp+8], eax
   lea     eax, [ebp-10h]
   mov     [esp+4], eax
   mov     eax, [ebp-4]
   mov     [esp], eax
   call    sub_80A8C1C
   mov     eax, [ebp+8]
   fld     dword [eax]
   fld     qword [dbl_8145F88]
   fmulp   st1, st0
   fstp    dword [ebp-4]
   lea     eax, [ebp-18h]
   mov     [esp+8], eax
   lea     eax, [ebp-0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp-4]
   mov     [esp], eax
   call    sub_80A8C1C
   cmp     dword [ebp+0Ch], 0
   jz      loc_80A2F4C
   mov     eax, [ebp+0Ch]
   fld     dword [ebp-18h]
   fmul    dword [ebp-1Ch]
   fstp    dword [eax]
   mov     eax, [ebp+0Ch]
   add     eax, 4
   fld     dword [ebp-18h]
   fmul    dword [ebp-10h]
   fstp    dword [eax]
   mov     edx, [ebp+0Ch]
   add     edx, 8
   mov     eax, [ebp-0Ch]
   xor     eax, 80000000h
   mov     [edx], eax
loc_80A2F4C:
   cmp     dword [ebp+10h], 0
   jnz     loc_80A2F5D
   cmp     dword [ebp+14h], 0
   jnz     loc_80A2F5D
   jmp     locret_80A3037
loc_80A2F5D:
   mov     eax, [ebp+8]
   add     eax, 8
   fld     dword [eax]
   fld     qword [dbl_8145F88]
   fmulp   st1, st0
   fstp    dword [ebp-4]
   lea     eax, [ebp-14h]
   mov     [esp+8], eax
   lea     eax, [ebp-8]
   mov     [esp+4], eax
   mov     eax, [ebp-4]
   mov     [esp], eax
   call    sub_80A8C1C
   cmp     dword [ebp+10h], 0
   jz      loc_80A2FEC
   mov     edx, [ebp+10h]
   mov     eax, [ebp-8]
   mov     [ebp-24h], eax
   fld     dword [ebp-24h]
   fchs    
   fmul    dword [ebp-0Ch]
   fmul    dword [ebp-1Ch]
   fld     dword [ebp-14h]
   fmul    dword [ebp-10h]
   faddp   st1, st0
   fstp    dword [edx]
   mov     edx, [ebp+10h]
   add     edx, 4
   mov     eax, [ebp-8]
   mov     [ebp-24h], eax
   fld     dword [ebp-24h]
   fchs    
   fmul    dword [ebp-0Ch]
   fmul    dword [ebp-10h]
   mov     eax, [ebp-14h]
   mov     [ebp-24h], eax
   fld     dword [ebp-24h]
   fchs    
   fmul    dword [ebp-1Ch]
   faddp   st1, st0
   fstp    dword [edx]
   mov     edx, [ebp+10h]
   add     edx, 8
   mov     eax, [ebp-8]
   mov     [ebp-24h], eax
   fld     dword [ebp-24h]
   fchs    
   fmul    dword [ebp-18h]
   fstp    dword [edx]
loc_80A2FEC:
   cmp     dword [ebp+14h], 0
   jz      locret_80A3037
   mov     eax, [ebp+14h]
   fld     dword [ebp-14h]
   fmul    dword [ebp-0Ch]
   fmul    dword [ebp-1Ch]
   fld     dword [ebp-8]
   fmul    dword [ebp-10h]
   faddp   st1, st0
   fstp    dword [eax]
   mov     edx, [ebp+14h]
   add     edx, 4
   fld     dword [ebp-14h]
   fmul    dword [ebp-0Ch]
   fmul    dword [ebp-10h]
   mov     eax, [ebp-8]
   mov     [ebp-24h], eax
   fld     dword [ebp-24h]
   fchs    
   fmul    dword [ebp-1Ch]
   faddp   st1, st0
   fstp    dword [edx]
   mov     eax, [ebp+14h]
   add     eax, 8
   fld     dword [ebp-14h]
   fmul    dword [ebp-18h]
   fstp    dword [eax]
locret_80A3037:
   leave   
   retn    


VectorSubtract412:
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


sub_810DBCE:
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


sub_810DCE8:
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


sub_810CA98:
   push    ebp
   mov     ebp, esp
   sub     esp, 28h
   lea     eax, [ebp-18h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    VectorCopy478
   lea     eax, [ebp-18h]
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   mov     [esp], eax
   call    sub_810DCE8
   mov     eax, [ebp+8]
   fstp    dword [eax]
   lea     eax, [ebp-18h]
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   add     eax, 0Ch
   mov     [esp], eax
   call    sub_810DCE8
   mov     eax, [ebp+8]
   add     eax, 4
   fstp    dword [eax]
   lea     eax, [ebp-18h]
   mov     [esp+4], eax
   mov     eax, [ebp+0Ch]
   add     eax, 18h
   mov     [esp], eax
   call    sub_810DCE8
   mov     eax, [ebp+8]
   add     eax, 8
   fstp    dword [eax]
   leave   
   retn    


VectorCopy478:
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


sub_810C9F0:
   push    ebp
   mov     ebp, esp
   sub     esp, 18h
   mov     eax, [ebp+0Ch]
   add     eax, 18h
   mov     [esp+0Ch], eax
   mov     eax, [ebp+0Ch]
   add     eax, 0Ch
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    AngleVectors
   mov     eax, [ebp+0Ch]
   add     eax, 0Ch
   mov     [esp], eax
   call    sub_810DC22
   leave   
   retn    


sub_810DC60:
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


G_TryPushingEntity:
   push    ebp
   mov     ebp, esp
   sub     esp, 0F8h
   lea     eax, [ebp-0D8h]
   mov     [esp+8], eax
   mov     eax, [ebp+10h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   add     eax, 138h
   mov     [esp], eax
   call    sub_810DC60
   lea     eax, [ebp-0A8h]
   mov     [esp+4], eax
   mov     eax, [ebp+14h]
   mov     [esp], eax
   call    sub_810C9F0
   lea     eax, [ebp-78h]
   mov     [esp+4], eax
   lea     eax, [ebp-0A8h]
   mov     [esp], eax
   call    sub_810CA2C
   lea     eax, [ebp-18h]
   mov     [esp+8], eax
   mov     eax, [ebp+0Ch]
   add     eax, 138h
   mov     [esp+4], eax
   lea     eax, [ebp-0D8h]
   mov     [esp], eax
   call    VectorSubtract412
   lea     eax, [ebp-28h]
   mov     [esp+4], eax
   lea     eax, [ebp-18h]
   mov     [esp], eax
   call    VectorCopy478
   lea     eax, [ebp-78h]
   mov     [esp+4], eax
   lea     eax, [ebp-28h]
   mov     [esp], eax
   call    sub_810CA98
   lea     eax, [ebp-38h]
   mov     [esp+8], eax
   lea     eax, [ebp-18h]
   mov     [esp+4], eax
   lea     eax, [ebp-28h]
   mov     [esp], eax
   call    VectorSubtract412
   lea     eax, [ebp-0D8h]
   mov     [esp+8], eax
   lea     eax, [ebp-38h]
   mov     [esp+4], eax
   lea     eax, [ebp-0D8h]
   mov     [esp], eax
   call    sub_810DC60
   lea     eax, [ebp-0D8h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    G_TestEntityPosition
   mov     [ebp-3Ch], eax
   cmp     dword [ebp-3Ch], 0
   jnz     loc_810CCDD
   mov     eax, [ebp+8]
   mov     edx, [ebp+0Ch]
   mov     eax, [eax+7Ch]
   cmp     eax, [edx]
   jz      loc_810CC0E
   mov     eax, [ebp+8]
   mov     dword [eax+7Ch], 3FFh
loc_810CC0E:
   mov     eax, [ebp+8]
   add     eax, 138h
   mov     [esp+4], eax
   lea     eax, [ebp-0D8h]
   mov     [esp], eax
   call    VectorCopy478
   mov     eax, [ebp+8]
   add     eax, 18h
   mov     [esp+4], eax
   lea     eax, [ebp-0D8h]
   mov     [esp], eax
   call    VectorCopy478
   mov     eax, [ebp+8]
   cmp     dword [eax+158h], 0
   jz      loc_810CCC7
   mov     eax, [ebp+8]
   mov     edx, [eax+158h]
   mov     eax, [ebp+8]
   mov     ecx, [eax+158h]
   mov     eax, [ebp+14h]
   add     eax, 4
   fld     dword [eax]
   fld     dword [flt_8153E14]
   fmulp   st1, st0
   fnstcw  word [ebp-0DAh]
   movzx   eax, word [ebp-0DAh]
   or      ax, 0C00h
   mov     [ebp-0DCh], ax
   fldcw   word [ebp-0DCh]
   fistp   dword [ebp-0E0h]
   fldcw   word [ebp-0DAh]
   mov     eax, [ebp-0E0h]
   and     eax, 0FFFFh
   add     eax, [ecx+58h]
   mov     [edx+58h], eax
   mov     eax, [ebp+8]
   mov     eax, [eax+158h]
   add     eax, 14h
   mov     [esp+4], eax
   lea     eax, [ebp-0D8h]
   mov     [esp], eax
   call    VectorCopy478
loc_810CCC7:
   add     dword [pushed_p], 20h
   mov     dword [ebp-0E4h], 1
   jmp     loc_810D04F
loc_810CCDD:
   mov     eax, [ebp+8]
   fld     dword [eax+110h]
   fld     qword [dbl_8153E18]
   fdivp   st1, st0
   fld     qword [dbl_8153E20]
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_810CD02
   jmp     loc_810D00F
loc_810CD02:
   lea     eax, [ebp-18h]
   mov     [esp+4], eax
   lea     eax, [ebp-0D8h]
   mov     [esp], eax
   call    VectorCopy478
   mov     eax, 0
   mov     [ebp-0BCh], eax
loc_810CD22:
   fld     dword [ebp-0BCh]
   mov     eax, [ebp+8]
   fld     dword [eax+110h]
   fld     qword [dbl_8153E18]
   fdivp   st1, st0
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_810CD45
   jmp     loc_810D00F
loc_810CD45:
   mov     eax, [ebp-0BCh]
   xor     eax, 80000000h
   mov     [ebp-0C0h], eax
loc_810CD56:
   fld     dword [ebp-0C0h]
   fld     dword [ebp-0BCh]
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_810CD6E
   jmp     loc_810CFF6
loc_810CD6E:
   mov     eax, 40800000h
   mov     [ebp-0ACh], eax
loc_810CD79:
   fld     dword [ebp-0ACh]
   mov     eax, [ebp+8]
   fld     dword [eax+110h]
   fld     qword [dbl_8153E18]
   fdivp   st1, st0
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_810CD9C
   jmp     loc_810CFC6
loc_810CD9C:
   mov     eax, [ebp-0ACh]
   xor     eax, 80000000h
   mov     [ebp-0B0h], eax
loc_810CDAD:
   fld     dword [ebp-0B0h]
   fld     dword [ebp-0ACh]
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_810CDC5
   jmp     loc_810CFAD
loc_810CDC5:
   mov     eax, 40800000h
   mov     [ebp-0B4h], eax
loc_810CDD0:
   fld     dword [ebp-0B4h]
   mov     eax, [ebp+8]
   fld     dword [eax+110h]
   fld     qword [dbl_8153E18]
   fdivp   st1, st0
   fucompp 
   fnstsw  ax
   sahf    
   ja      loc_810CDF3
   jmp     loc_810CF90
loc_810CDF3:
   mov     eax, [ebp-0B4h]
   xor     eax, 80000000h
   mov     [ebp-0B8h], eax
loc_810CE04:
   fld     dword [ebp-0B8h]
   fld     dword [ebp-0B4h]
   fucompp 
   fnstsw  ax
   sahf    
   jnb     loc_810CE1C
   jmp     loc_810CF77
loc_810CE1C:
   mov     eax, [ebp-0C0h]
   mov     [esp+0Ch], eax
   mov     eax, [ebp-0B8h]
   mov     [esp+8], eax
   mov     eax, [ebp-0B0h]
   mov     [esp+4], eax
   lea     eax, [ebp-38h]
   mov     [esp], eax
   call    sub_810DBCE
   lea     eax, [ebp-28h]
   mov     [esp+8], eax
   lea     eax, [ebp-38h]
   mov     [esp+4], eax
   lea     eax, [ebp-18h]
   mov     [esp], eax
   call    sub_810DC60
   lea     eax, [ebp-28h]
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    G_TestEntityPosition
   mov     [ebp-3Ch], eax
   cmp     dword [ebp-3Ch], 0
   jnz     loc_810CF5A
   mov     eax, [ebp+8]
   mov     edx, [ebp+0Ch]
   mov     eax, [eax+7Ch]
   cmp     eax, [edx]
   jz      loc_810CE94
   mov     eax, [ebp+8]
   mov     dword [eax+7Ch], 3FFh
loc_810CE94:
   mov     eax, [ebp+8]
   add     eax, 138h
   mov     [esp+4], eax
   lea     eax, [ebp-28h]
   mov     [esp], eax
   call    VectorCopy478
   mov     eax, [ebp+8]
   add     eax, 18h
   mov     [esp+4], eax
   lea     eax, [ebp-28h]
   mov     [esp], eax
   call    VectorCopy478
   mov     eax, [ebp+8]
   cmp     dword [eax+158h], 0
   jz      loc_810CF44
   mov     eax, [ebp+8]
   mov     edx, [eax+158h]
   mov     eax, [ebp+8]
   mov     ecx, [eax+158h]
   mov     eax, [ebp+14h]
   add     eax, 4
   fld     dword [eax]
   fld     dword [flt_8153E14]
   fmulp   st1, st0
   fnstcw  word [ebp-0DAh]
   movzx   eax, word [ebp-0DAh]
   or      ax, 0C00h
   mov     [ebp-0DCh], ax
   fldcw   word [ebp-0DCh]
   fistp   dword [ebp-0E0h]
   fldcw   word [ebp-0DAh]
   mov     eax, [ebp-0E0h]
   and     eax, 0FFFFh
   add     eax, [ecx+58h]
   mov     [edx+58h], eax
   mov     eax, [ebp+8]
   mov     eax, [eax+158h]
   add     eax, 14h
   mov     [esp+4], eax
   lea     eax, [ebp-28h]
   mov     [esp], eax
   call    VectorCopy478
loc_810CF44:
   add     dword [pushed_p], 20h
   mov     dword [ebp-0E4h], 1
   jmp     loc_810D04F
loc_810CF5A:
   fld     dword [ebp-0B4h]
   fld     st0
   faddp   st1, st0
   fld     dword [ebp-0B8h]
   faddp   st1, st0
   fstp    dword [ebp-0B8h]
   jmp     loc_810CE04
loc_810CF77:
   fld     dword [ebp-0B4h]
   fld     dword [flt_8153E28]
   faddp   st1, st0
   fstp    dword [ebp-0B4h]
   jmp     loc_810CDD0
loc_810CF90:
   fld     dword [ebp-0ACh]
   fld     st0
   faddp   st1, st0
   fld     dword [ebp-0B0h]
   faddp   st1, st0
   fstp    dword [ebp-0B0h]
   jmp     loc_810CDAD
loc_810CFAD:
   fld     dword [ebp-0ACh]
   fld     dword [flt_8153E28]
   faddp   st1, st0
   fstp    dword [ebp-0ACh]
   jmp     loc_810CD79
loc_810CFC6:
   fld     dword [ebp-0C0h]
   fldz    
   fxch    st1
   fucompp 
   fnstsw  ax
   sahf    
   jp      loc_810CFD9
   jz      loc_810CFF6
loc_810CFD9:
   fld     dword [ebp-0BCh]
   fld     st0
   faddp   st1, st0
   fld     dword [ebp-0C0h]
   faddp   st1, st0
   fstp    dword [ebp-0C0h]
   jmp     loc_810CD56
loc_810CFF6:
   fld     dword [ebp-0BCh]
   fld     dword [flt_8153E28]
   faddp   st1, st0
   fstp    dword [ebp-0BCh]
   jmp     loc_810CD22
loc_810D00F:
   mov     eax, [ebp+8]
   add     eax, 138h
   mov     [esp+4], eax
   mov     eax, [ebp+8]
   mov     [esp], eax
   call    G_TestEntityPosition
   mov     [ebp-3Ch], eax
   cmp     dword [ebp-3Ch], 0
   jnz     loc_810D045
   mov     eax, [ebp+8]
   mov     dword [eax+7Ch], 3FFh
   mov     dword [ebp-0E4h], 1
   jmp     loc_810D04F
loc_810D045:
   mov     dword [ebp-0E4h], 0
loc_810D04F:
   mov     eax, [ebp-0E4h]
   leave   
   retn    


section .rdata
flt_8153E28     dd 4.0
flt_8153E14     dd 182.04445

dbl_8145F88     dq 0.0174532925199433
dbl_8153E18     dq 2.0
dbl_8153E20     dq 4.0