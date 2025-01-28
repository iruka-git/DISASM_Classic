#
#	makefile for Mingw32(Windows) / Linux
#	=====================================
#
#
# If you'd like to compile on linux, please uncomment below.
#linux	= BETTER
# Enjoy.
#
ifdef	linux
OPT	= -DLINUX
else
OPT	= -DMINGW
EXE	= .exe
endif
#
#  ===== rule =====
#
.c.o:
	gcc ${OPT} -O2 -c $*.c
#
#  ===== source files =====
#
files	= diswin.o dis.o dissub.o mmx.o kni.o le.o pe.o hash.o strlib.o
#
mips	= risc.o mips-dis.o mips-opc.o
sh	= sh-dis.o
arm	= arm-dis.o
#
#  ===== target =====
#
diswin${EXE}: $(files) $(mips) $(sh) $(arm)
	gcc -s -o $@  $(files) $(mips) $(sh) $(arm)
#
# w/map
#	gcc -s -o $@  $(files) $(mips) $(sh) $(arm) -Xlinker -Map=diswin.map
#
#  ===== other stuff =====
#
clean:
	-rm *.o
	-rm *.obj
	-rm *.map
	-rm *.bak
#
