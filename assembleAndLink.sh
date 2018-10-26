#!/bin/bash

nasm -f elf -o hello.o hello.asm && \
ld -m elf_i386 -o hello.e hello.o
