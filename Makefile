default: encode extract c.tar.gz

CC=gcc
CFLAGS=-Os -m32 -std=gnu99
ECFLAGS=-Os -m32 -nostdlib -fwhole-program -std=gnu99  -flto

data.o: data.asm ex1_data.bin
	nasm $< -f elf

ex1_data.bin: encode
	./encode

read.o: read.asm
	nasm -f elf $<

extract: extract.c data.o data.h model.c read.h
	${CC} ${ECFLAGS} extract.c data.o  -o $@
	strip extract

extract.s: extract.c data.h
	${CC} ${ECFLAGS} extract.c  -S

encode: encode.c
	${CC} ${CFLAGS} encode.c -o $@

#run: extract unpack.header
#	7z a -tGZip -mx=9 $<.gz $<
#	cat unpack.header $<.gz > $@
#	rm $<.gz
#	chmod +rx $@
#	du -b $@
c.tar.gz: extract
	rm -rf c
	mkdir -p c
	cp $< c/run
	tar -cvvf c.tar c
	7z a -tGZip -mx=9 c.tar.gz c.tar
	du -b $@

clean:
	rm *.o
	rm extract
	rm encode
