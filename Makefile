########################################################
# This makefile makes a library and/or an executable
#


#########################################################
# DEFINES/PATH
#

INSTALL_LIB_DIR =
INSTALL_EXE_DIR = ./bin
INSTALL_HEADER_DIR =
GCCPATH = /home/utils/gcc-4.3.2

#########################################################
# FILES

LIB	= Release_i686/libDMcTools.a
LIBSRCS	= Half/half.cpp Image/Bmp.cpp Image/Filter.cpp Image/Gif.cpp Image/ImageAlgorithms.cpp Image/ImageLoadSave.cpp Image/tLoadSave.cpp Image/Quant.cpp Image/RGBEio.cpp Image/Targa.cpp Math/CatmullRomSpline.cpp Math/DownSimplex.cpp Math/HVector.cpp Math/HermiteSpline.cpp Math/Matrix44.cpp Math/Perlin.cpp Math/Quadric.cpp Model/BisonMe.cpp Model/Camera.cpp Model/LoadOBJ.cpp Model/LoadVRML.cpp Model/Mesh.cpp Model/Model.cpp Model/RenderObject.cpp Model/SaveOBJ.cpp Model/SaveVRML.cpp Model/TextureDB.cpp Model/TriObject.cpp Util/Timer.cpp Util/Utils.cpp
LIBOBJS = $(LIBSRCS:.cpp=.o)

EXE	= 
EXESRCS	= 

EXEOBJS = $(EXESRCS:.cpp=.o)
EXEDEPENDS = -lz
# targa.h

SRCS	= $(LIBSRCS) $(EXESRCS)

EXPORT_HEADERS = 

#########################################################
# C COMPILATION

CC = $(GCCPATH)/bin/g++
CWARNFLAGS = -Wall
CDEFS = -DANSI
COMPINCLUDES =
#COMPINCLUDES = -I$(GCCPATH)/include/c++/4.3.1 -I$(GCCPATH)/include/c++/4.3.1/i686-pc-linux-gnu
CINCLUDES = $(COMPINCLUDES) -I. -I.. -IHalf -I../Goodies/include
CDEBUGFLAGS = -O3
CFLAGS = $(CWARNFLAGS) $(CDEBUGFLAGS) $(CDEFS) $(CINCLUDES)

################################################################
# TARGETS
ALL = $(EXE) $(LIB)
all: $(ALL)

Version:
	$(GCCPATH)/g++ --version

################################################################
# BUILD

Build build install:
	@echo "Starting Build @ `date '+%T %d%h%y'`"
	@$(MAKE) install_exe
	@echo "Done @ `date '+%T %d%h%y'`"

################################################################
include Makefile.std


# DO NOT DELETE
