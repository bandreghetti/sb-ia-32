; Auto-generated from test-files/triangulo.asm
section .text
global _start
_start:
mov eax, dword[B]
mov [R], eax
mov eax, 1
mov ebx, 0
int 80h
section .bss
B resd 1
H resd 1
R resd 1
section .data
DOIS dd 2
