;brief title of program file name
; Objectives:
; Inputs:
; Outputs:

section .data
    msg db 'Hello World!', 0Dh, 0Ah

section .text
global _start
            ; print msg
_start:     mov eax, 4
            mov ebx, 1
            mov ecx, msg
            mov edx, 14
            int 80h

            ; return 0
            mov eax, 1
            mov ebx, 0
            int 80h

section .bss
    ; (uninitialized data go here)
