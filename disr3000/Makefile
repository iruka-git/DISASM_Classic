#
#
#
#
COPT=
#
.c.o:
	gcc $(COPT) -O2 -c $*.c
#
files	=	disr.o loadfile.o symbol.o strlib.o risc.o	mips-dis.o	mips-opc.o
#
disr3000 :  $(files)
	gcc $(COPT) -o $@ $(files)
#
test:
	./disr3000 PSX.bin
#
run:
	./disr3000 -s80010800 PSX.bin >out.asm
#
debug:
	gdb --args ./disr3000 PSX.bin
#
clean:
	-rm *.o
	-rm *~
	-rm disr3000
#
