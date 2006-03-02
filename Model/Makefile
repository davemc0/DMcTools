# Makefile for Model stuff:
# VRML 1.0 Object loader
# Wavefront OBJ reader
# Dave McAllister, July 1999.

INCLUDE = -I..

CFLAGS =$(OPT) -OPT:Olimit=0 $(ARGS) $(INCLUDE)

.SUFFIXES: .cc .cpp
.cc.o:
	$(CC) $(CFLAGS) -c $<

.cpp.o:
	$(CC) $(CFLAGS) -c $<

all: BisonMe.o LoadOBJ.o LoadTRI.o Mesh.o SaveOBJ.o SaveTRI.o TriObject.o LoadPLY.o LoadVRML.o Model.o SavePLY.o SaveVRML.o

# Dependency is actually on BisonMe.h, not .cpp
LoadVRML.o: LoadVRML.cpp LoadVRML.h BisonMe.cpp
	$(CC) $(CFLAGS) -c LoadVRML.cpp -o $@

BisonMe.o: BisonMe.cpp FlexMe.cpp LoadVRML.h
	$(CC) $(CFLAGS) -c BisonMe.cpp -o $@

# bison options:
# -v = produce output file w/detailed info on parser
# it will be *.output
# -d = produce *.tab.h for flex to use
BisonMe.cpp: BisonMe.y
	bison -v -d BisonMe.y
	mv -f BisonMe.tab.c BisonMe.cpp
	mv -f BisonMe.tab.h BisonMe.h

# flex options:
# -i = case insensitive (but passes case through)
# -8 = eight bit characters
# actually depends on BisonMe.h, but that's made at same time
FlexMe.cpp: FlexMe.l BisonMe.cpp
	flex -8 FlexMe.l
	mv -f lex.yy.c $@

LoadVRML.h: TriObject.h

clean:
	rm -f *.o LoadVRML *.yy.c *.tab.c *.tab.h
	rm -f BisonMe.h BisonMe.c BisonMe.cpp
	rm -f FlexMe.c FlexMe.cpp
	rm -f *.output
	rm -rf ii_files rii_files
