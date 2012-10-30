
default: encode extract

CFLAGS=-Os -m32
ECFLAGS=-Os -m32 -nostdlib

data.o: ex1_data.bin data.asm encode
	./encode
	nasm data.asm -f elf

ex1_data.bin: encode
	./encode

read.o: read.asm
	nasm -f elf read.asm

extract: extract.c read.o data.o data.h
	c99 ${ECFLAGS} extract.c read.o data.o  -o $@

encode: encode.c read.o
	c99 ${CFLAGS} encode.c read.o -o $@
