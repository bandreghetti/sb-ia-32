;brief title of program file name
; Objectives:
; Inputs:
; Outputs:

section .data
    msg db 'Hello World!', 0Ah

section .text
global _start
_start:     mov eax, 4
            mov ebx, 1
            mov ecx, msg
            mov edx, 13
            int 80h
            mov eax, 1
            mov ebx, 0
            int 80h

section .bss
    ; (uninitialized data go here)
