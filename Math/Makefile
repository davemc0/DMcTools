# Makefile for SCI Tools

INCLUDE = -I..
CFLAGS =$(OPT) $(ARGS) $(INCLUDE)

OBJS = CatmullRomSpline.o DownSimplex.o HVector.o HermiteSpline.o Matrix44.o Perlin.o Quadric.o

.SUFFIXES: .cc .cpp
.cc.o:
	$(CC) $(CFLAGS) -c $<

.cpp.o:
	$(CC) $(CFLAGS) -c $<

all: myobjs

myobjs: $(OBJS)
