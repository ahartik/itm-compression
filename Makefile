
default: encode extract

CC=gcc
CFLAGS=-Os -m32 -std=gnu99
ECFLAGS=-Os -m32 -nostdlib -fwhole-program  -std=gnu99 

data.o: ex1_data.bin data.asm encode
	./encode
	nasm data.asm -f elf

ex1_data.bin: encode
	./encode

read.o: read.asm
	nasm -f elf read.asm

extract: extract.c read.o data.o data.h
	${CC} ${ECFLAGS} extract.c read.o data.o  -o $@
	strip extract

extract.s: extract.c read.o data.o data.h
	${CC} ${CFLAGS} extract.c  -S 

encode: encode.c read.o
	${CC} ${CFLAGS} encode.c read.o -o $@

clean:
	rm *.o
	rm extract
	rm encode
