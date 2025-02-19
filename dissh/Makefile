#
#	SH-2 Disassembler
#
COPT= -g
#
.c.o:
	gcc $(COPT) -O2 -c $*.c
#
files	=  dissh.o dis.o opcode.o mark.o symbol.o
#
dissh:	$(files)
	gcc $(COPT) -o $@ $(files)
#
test:
	./dissh a.out
#
run:
	./dissh -s0x6004000 sl.bin sl.asm
#
debug:
	gdb --args ./dissh -s0x6004000 sl.bin sl.asm
#
clean:
	-rm *.o
	-rm *~
	-rm dissh
#
#
