#!/bin/bash

ASM_NAME='hello'
nasm -f elf -o $ASM_NAME.o $ASM_NAME.asm && \
ld -m elf_i386 -o $ASM_NAME.e $ASM_NAME.o && \
rm $ASM_NAME.o

ASM_NAME='toLower'
nasm -f elf -o $ASM_NAME.o $ASM_NAME.asm && \
ld -m elf_i386 -o $ASM_NAME.e $ASM_NAME.o && \
rm $ASM_NAME.o
