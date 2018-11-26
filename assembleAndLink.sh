#!/bin/bash

if [ $# -lt 1 ] ; then
    echo "Usage: ./assembleAndLink <file-name-without-.s-extension>"
    exit
fi

SRC_FILE=$1
FILENAME=${SRC_FILE%%.*}
nasm -f elf -o $FILENAME.o $SRC_FILE && \
ld -m elf_i386 -o $FILENAME.e $FILENAME.o && \
rm $FILENAME.o
