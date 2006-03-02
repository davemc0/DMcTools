# Makefile for Tools

INCLUDE = -I..

CFLAGS =$(OPT) $(ARGS) $(INCLUDE)

OBJS = Timer.o Utils.o

.SUFFIXES: .cc .cpp
.cc.o:
	$(CC) $(CFLAGS) -c $<

.cpp.o:
	$(CC) $(CFLAGS) -c $<

all: myobjs

myobjs: $(OBJS)
