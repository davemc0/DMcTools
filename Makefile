######################################################################
# Makefile for Tools
#

CC =CC

# This line gives good loop nest optimization settings for evans (SGI Origin2000).
LNO =-LNO:opt=1:fusion=2:fission=2:fusion_peeling_limit=2048:cs1=32K:cs2=8M

# This line gives other good optimization settings for evans (SGI Origin2000).
TEMPL =-ptv -ptnone
OPT =-LANG:std $(TEMPL) -O3 -Ofast=IP27 $(LNO) -IPA
# -DDMC_DEBUG

INCLUDE = -I..

CFLAGS =$(OPT) $(ARGS) $(INCLUDE)

all: SGI/libTools-64.a

32: SGI/libTools.a

SGI/libTools.a: UtilDir MathDir ImageDir ModelDir
	rm -f $@
	mkdir -p SGI
	ar clq $@ Util/*.o Math/*.o Image/*.o Model/*.o

SGI/libTools-64.a: UtilDir64 MathDir64 ImageDir64 ModelDir64
	rm -f $@
	mkdir -p SGI
	CC -ar -o $@ $(OPT) Util/*.o Math/*.o Image/*.o Model/*.o

UtilDir:
	(cd Util ; smake OPT="$(OPT)" CC="$(CC)")

MathDir:
	(cd Math ; smake OPT="$(OPT)" CC="$(CC)")

ImageDir:
	(cd Image ; smake OPT="$(OPT)" CC="$(CC)")

ModelDir:
	(cd Model ; smake OPT="$(OPT)" CC="$(CC)")

UtilDir64:
	(cd Util ; smake OPT="$(OPT) -64" CC="$(CC)")

MathDir64:
	(cd Math ; smake OPT="$(OPT) -64" CC="$(CC)")

ImageDir64:
	(cd Image ; smake OPT="$(OPT) -64" CC="$(CC)")

ModelDir64:
	(cd Model ; smake OPT="$(OPT) -64" CC="$(CC)")

clean:
	(cd Model ; make clean)
	(cd Test ; make clean)
	rm -f ./unistd.h
	rm -f */*.o
	rm -f */*~
	rm -rf */ii_files
	rm -rf */ptrepository
	chmod 644 *.h */*.cpp */*.h
	crfilter *.h */*.cpp */*.h

tgz:
	mkdir DMcTools
	mkdir DMcTools/Util
	mkdir DMcTools/Math
	mkdir DMcTools/Image
	mkdir DMcTools/Model
	mkdir DMcTools/Test
	cp *.h Makefile Makefile.hp DMcTools.ds? README.txt DMcTools
	cp Util/*.h Util/*.cpp Util/Makefile DMcTools/Util
	cp Math/*.h Math/*.cpp Math/Makefile DMcTools/Math
	cp Test/*.h Test/*.cpp Test/Makefile DMcTools/Test
	cp Image/*.h Image/*.cpp Image/Makefile DMcTools/Image
	cp Model/*.h Model/*.cpp Model/Makefile Model/*.y Model/*.l DMcTools/Model
	tar cvf DMcTools.tar DMcTools
	gzip DMcTools.tar
	rm -rf DMcTools
	mv DMcTools.tar.gz ..
