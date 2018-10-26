;brief title of program file name
; Objectives:
; Inputs:
; Outputs:

section .data
    string db 'KANGAROO', 0Dh, 0Ah

section .text
global _start
            ; print msg
_start:     mov eax, 4
            mov ebx, 1
            mov ecx, string
            mov edx, 10
            int 80h

            mov ebx, string
            mov eax, 8
            mov ecx, 8

            ; add 32 to all characters in string
toLower:    add byte[ebx], 32
            inc ebx
            loop toLower

            ; print msg
            mov eax, 4
            mov ebx, 1
            mov ecx, string
            mov edx, 10
            int 80h

            ; return 0
            mov eax, 1
            mov ebx, 0
            int 80h

section .bss
    ; (uninitialized data go here)
