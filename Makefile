#
#
# Makefile -  manage the compiling
#
# $RCSfile: makefile,v $
#
# $Id: makefile,v 1.7 2013/12/11 22:09:51 chuck Exp chuck $
#
#
# $Log: makefile,v $
# Revision 1.7  2013/12/11 22:09:51  chuck
# added math.c, math.h for pollard rho
#
# Revision 1.6  2013/11/15 21:27:24  chuck
# added new modules
#
# Revision 1.5  2013/10/22 20:43:11  chuck
# fixed ld/cc
#
# Revision 1.4  2013/10/22 14:09:47  chuck
# *** empty log message ***
#
# Revision 1.3  2013/10/22 14:03:50  chuck
# rules finished and verified.
# filenames added.
#
# Revision 1.2  2013/10/22 12:46:39  chuck
# initialization and depend setup
#
# Revision 1.1  2013/10/21 14:09:36  chuck
# Initial revision
#

# Set DEBUG '-g' for debug mode
DEBUG   = -g

# Suggested Flags & Definitions (adjust for the target processor/architecture)
CC      = gcc

# Compilation time options (default ones)
OPTIONS = -DMULMOD -DINLINE_MULMOD

# -- Normal runtime
CFLAGS  = -O2 -ffast-math -fschedule-insns2 -march=corei7 -m64 -pthread -fomit-frame-pointer $(OPTIONS) # -O2 for later

# -- debug (gdb)  // uncomment for debug (overwrites above options)
#CFLAGS  = $(DEBUG) -g -m64 -pthread $(OPTIONS)



LDFLAGS = $(DEBUG) -m64 -pthread -lm


EXE 	= nrdsieve
SRCS	= askyesno.c powmodtwo.c powmodgeneric.c sieveworker.c mulmod.c pgen.c \
	  math.c powmodtwo128.c powmodgeneric128.c signals.c \
	  writelog.c loadsaveini.c string.c data.c \
	  option.c printmenu.c header.c askint.c \
          threads.c sieve.c sievesetup.c newsieve.c

OBJS	= $(SRCS:.c=.o)

# adjust this as required for your gcc & distro
DEPDIRS = -I /usr/include/linux -I /usr/lib/gcc/x86_64-redhat-linux/4.8.1/include

# Rules

VPATH = SRC:./RCS
        
.SUFFIXES:
.SUFFIXES: .o .c .c,v

.c,v.o:
	co $*
	$(CC) -c $(CFLAGS) $< -o $@

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@


$(EXE):	$(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) primegen.a


# Targets:
all:	$(EXE)


clean:
	$(RM) $(EXE) $(OBJS)

depend:
	makedepend $(DEPDIRS) $(SRCS)

#
#
# --  HAND CRAFTED Dependencies --
#     It is presumed and asserted here that all required includes can be found locally or
#     in directories known to the gcc compiler.
#
# DO NOT DELETE

askyesno.o:	 askyesno.c string.h

askint.o:	 askint.c

data.o:	 	 data.c sieve.h  limits.h

math.o: 	 math.c math.h limits.h

header.o:	 header.c  limits.h

loadsaveini.o:	 loadsaveini.c loadsaveini.h sieve.h string.h  sievenums.h limits.h

mulmod.o:	 mulmod.c

newsieve.o:	 newsieve.c sieve.h option.h limits.h

option.o:	 option.c option.h string.h sieve.h sievenums.h limits.h

pgen.o:		 pgen.c pgen.h primegen.h limits.h sieve.h

powmodgeneric.o: powmodgeneric.c powmod.h

powmodtwo.o:	 powmodtwo.c powmod.h

powmodgeneric128.o: powmodgeneric128.c powmod.h

powmodtwo128.o:	 powmodtwo128.c powmod.h

#powmodtwo-mask.o: powmodtwo-mask.c powmod.h

printmenu.o:	 printmenu.c

sieve.o:     	 sieve.c sieve.h powmod.h option.h loadsaveini.h string.h  primegen.h \
		 loadsaveini.h option.h uint32.h uint64.h sievenums.h limits.h math.h

sievesetup.o:	 sievesetup.c sieve.h powmod.h option.h loadsaveini.h string.h  primegen.h \
		 option.h uint32.h uint64.h sievenums.h limits.h

sieveworker.o:	 sieveworker.c sieve.h powmod.h option.h loadsaveini.h string.h  primegen.h \
		 option.h uint32.h uint64.h sievenums.h limits.h threads.h signals.h math.h

signals.o:       signals.c limits.h sieve.h signals.h

string.o:	 string.c string.h

threads.o:	 threads.c threads.h

writelog.o:	 writelog.c sieve.h

