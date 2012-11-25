default: encode extract c.tar.gz

CC=gcc
CFLAGS=-Os -m32 -std=gnu99 -Wall -Wno-unused-result
ECFLAGS=-Os -m32 -nostdlib -fwhole-program -std=gnu99  -flto -Wall -Wno-unused-result -ffast-math -Wl,--build-id=none -fomit-frame-pointer
DCFLAGS=-g -m32 -std=gnu99 -Wall -Wno-unused-result -DDEBUG -O0

data.o: data.asm ex1_data.bin ex2_data.bin ex4_data.bin
	nasm $< -f elf

ex1_data.bin ex2_data.bin ex4_data.bin: encode
	./$<

read.o: read.asm
	nasm -f elf $<

extract: extract.c data.o data.h model.c read.h model2.c model4.c pmath.h
	${CC} ${ECFLAGS} extract.c data.o  -o $@ -lm
	strip extract
	sstrip extract

extract.s: extract.c data.h
	${CC} ${ECFLAGS} extract.c  -S

extract.o: extract.c data.h
	${CC} ${ECFLAGS} extract.c  -c

encode: encode.c model.c model2.c model4.c pmath.h
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
	diff -q c/ex1_class.dat ex1_class.dat
	diff -q c/four-stocks.csv four-stocks.csv
	diff -q c/shuttle.class shuttle.class
	diff -q c/kiel.arr kiel.arr
	./a2p c/kd.arr kd.ppm

clean:
	rm *.o
	rm extract
	rm encode
