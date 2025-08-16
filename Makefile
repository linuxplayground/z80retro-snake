CC=/opt/fcc/bin/fcc
AS=/opt/fcc/bin/asz80
LD=/opt/fcc/bin/ldz80


CFLAGS=-mz80 -O -I/opt/fcc/lib/z80/include -I./headers
LDFLAGS=-b -C0x100 -m$@.map

ASMSRCS=$(wildcard asm/*.s)
ASMOBJS=$(ASMSRCS:.s=.o)

CSRCS=$(wildcard libsrc/*.c)
COBJS=$(CSRCS:.c=.o)

CRT0=crt0.o

OBJS=\
		 crt0.o \
		 $(ASMOBJS) \
		 $(COBJS) \
		 main.o

LIBS=\
		 /opt/fcc/lib/z80/libc.a \
		 /opt/fcc/lib/z80/libz80.a


.PHONY: all clean world copy

all: main.com

main.com: main.bin
	dd if=$^ of=$@ skip=1 bs=256

main.bin: $(CRT0) $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

clean:
	rm -fv main.com main.bin
	find . -name "*.o" -exec rm -fv {} \;

world: clean all

copy: main.com
	cpmrm -f z80-retro-8k-8m /dev/loop0p1 0:main.com
	cpmcp -f z80-retro-8k-8m /dev/loop0p1 main.com 0:
