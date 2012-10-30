
default: encode extract run

CC=gcc
CFLAGS=-Os -m32 -std=gnu99
ECFLAGS=-Os -m32 -nostdlib -fwhole-program -flto  -std=gnu99 -s

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

encode: encode.c read.o
	${CC} ${CFLAGS} encode.c read.o -o $@

run: extract unpack.header
	7z a -tGZip -mx=9 $<.gz $<
	cat unpack.header $<.gz > $@
	rm $<.gz
	chmod +rx $@
	du -b $@

clean:
	rm *.o
	rm extract
	rm encode
