# Makefile for SCI Tools
# OPT =-O
#IMGDIR used only for HP.
# Use this on SGI
IMGDIR =/usr/local/contrib/unmoderated
INCLUDE = -I.. -I$(IMGDIR)/include

# INCLUDE = -I..

CFLAGS =$(OPT) $(ARGS) $(INCLUDE)

OBJS = Bmp.o Filter.o Gif.o ImageAlgorithms.o ImageLoadSave.o Quant.o RGBEio.o Targa.o ucImage.o

.SUFFIXES: .cc .cpp
.cc.o:
	$(CC) $(CFLAGS) -c $<

.cpp.o:
	$(CC) $(CFLAGS) -c $<

all: myobjs

myobjs: $(OBJS)
