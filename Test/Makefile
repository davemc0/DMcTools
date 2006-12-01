######################################################################
# Makefile for Tools Tests
#
# Copyright David K. McAllister, Mar. 2000.
#
######################################################################

CC =gcc
C++ =g++

TOOLS =..

COPT =-O3

LDFLAGS = $(COPT)

CFLAGS =$(COPT) $(COMPFLAGS) -I$(TOOLS)

DMCILIB =-L$(TOOLS)/Release_i686 -lDMcTools -ltiff -ljpeg -lpng -lz -lm

DMCLIB =-L$(TOOLS)/Release_i686 -lDMcTools -lm

ALL = gaussian ImageRW PullP hashtest imtest TimeTrig
# KDTreeTest

.SUFFIXES	: .cpp

.cpp.o:
	$(C++) $(CFLAGS) -c $<

all: $(ALL)

ImageRW: ImageRW.o
	$(C++) $(LDFLAGS) -o $@ ImageRW.o $(DMCILIB)
PullP: PullP.o
	$(C++) $(LDFLAGS) -o $@ PullP.o $(DMCILIB)
gaussian: gaussian.o
	$(C++) $(LDFLAGS) -o $@ gaussian.o $(DMCLIB)
hashtest: hashtest.o
	$(C++) $(LDFLAGS) -o $@ hashtest.o $(DMCLIB)
imtest: imtest.o
	$(C++) $(LDFLAGS) -o $@ imtest.o $(DMCILIB)
TimeTrig: TimeTrig.o
	$(C++) $(LDFLAGS) -o $@ TimeTrig.o $(DMCLIB)

clean:
	rm -f *.o
	rm -f $(ALL)
