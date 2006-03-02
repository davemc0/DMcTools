######################################################################
# Makefile for Tools Tests
#
# Copyright David K. McAllister, Mar. 2000.
#
######################################################################

CC =cc
C++ =CC

TOOLS =/afs/cs.unc.edu/home/davemc/Work/Tools

64 =-64
MP =-mp
LNO =-LNO:opt=1:fusion=2:fission=2:fusion_peeling_limit=2048:cs1=32K:cs2=8M
#STRIP =-FE:template_in_elf_section -FE:eliminate_duplicate_inline_copies

COPT =-LANG:std $(64) $(STRIP) -O3 -Ofast=IP27 $(MP) $(LNO)

LDFLAGS =-IPA $(COPT)

CFLAGS =$(COPT) $(COMPFLAGS) -I$(TOOLS)

DMCILIB =-L$(TOOLS)/SGI -lTools$(64) -ltiff -ljpeg -lm

DMCLIB =-L$(TOOLS)/SGI -lTools$(64) -lm

ALL = ImageRW gaussian hfgen imtest KDTreeTest myseriesauint pfmtowrl testmesh testpull timetrig

# following line needed for c++ .cc files
.SUFFIXES	: .cc .cpp

.cc.o:
	$(C++) $(CFLAGS) -c $<

.cpp.o:
	$(C++) $(CFLAGS) -c $<

all: $(ALL)

gaussian: gaussian.o
	$(C++) $(LDFLAGS) -o $@ gaussian.o $(DMCLIB)

ImageRW: ImageRW.o
	$(C++) $(LDFLAGS) -o $@ ImageRW.o $(DMCILIB)

testpull: PullP.o
	$(C++) $(LDFLAGS) -o $@ PullP.o $(DMCILIB)

testmesh: TestMesh.o
	$(C++) $(LDFLAGS) -o $@ TestMesh.o $(DMCLIB)

imtest: imtest.o
	CC $(LDFLAGS) -o $@ imtest.o $(DMCILIB)

KDTreeTest: KDTreeTest.o
	CC $(LDFLAGS) -o $@ KDTreeTest.o $(DMCLIB)

myseries: myseries.o
	CC $(LDFLAGS) -o $@ myseries.o $(DMCLIB)

timetrig: TimeTrig.o
	CC $(LDFLAGS) -o $@ TimeTrig.o $(DMCLIB)

myseriesauint: myseriesauint.o
	CC $(LDFLAGS) -o $@ myseriesauint.o $(DMCLIB)

hfgen: HFGen.o
	CC $(LDFLAGS) -o $@ HFGen.o $(DMCLIB)

pfmtowrl: PFMtoWRL.o
	CC $(LDFLAGS) -o $@ PFMtoWRL.o $(DMCLIB)

clean:
	rm -rf ii_files rii_files
	rm -f *.o
	rm -f $(ALL)
