default: encode extract c.tar.gz

CC=gcc
CFLAGS=-Os -m32 -std=gnu99 -Wall -Wno-unused-result
ECFLAGS=-Os -m32 -nostdlib -fwhole-program -std=gnu99  -flto -Wall -Wno-unused-result -ffast-math
DFLAGS=-g -m32 -std=gnu99 -Wall -Wno-unused-result -DDEBUG

data.o: data.asm ex1_data.bin
	nasm $< -f elf

ex1_data.bin: encode
	./encode

read.o: read.asm
	nasm -f elf $<

extract: extract.c data.o data.h model.c read.h
	${CC} ${ECFLAGS} extract.c data.o  -o $@ -lm
	strip extract
	sstrip extract

extract.s: extract.c data.h
	${CC} ${ECFLAGS} extract.c  -S

extract.o: extract.c data.h
	${CC} ${ECFLAGS} extract.c  -c

encode: encode.c model.c model2.c
	${CC} ${CFLAGS} encode.c -o $@ -lm

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

test: extract
	./$<
	diff c/ex1_class.dat ex1_class.dat
	diff c/four-stocks.csv four-stocks.csv

clean:
	rm *.o
	rm extract
	rm encode
