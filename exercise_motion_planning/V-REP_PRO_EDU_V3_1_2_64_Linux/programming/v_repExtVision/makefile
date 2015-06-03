CFLAGS = -I../include -Wall -fPIC -static

OS = $(shell uname -s)
ifeq ($(OS), Linux)
	EXT = so
else
	EXT = dylib
endif

all: 
	g++ $(CFLAGS) -c visionTransf.cpp -o visionTransf.o
	g++ $(CFLAGS) -c visionTransfCont.cpp -o visionTransfCont.o
	g++ $(CFLAGS) -c v_repExtVision.cpp -o v_repExtVision.o
	g++ $(CFLAGS) -c ../common/v_repLib.cpp -o ../common/v_repLib.o
	@mkdir -p lib
	g++ visionTransf.o visionTransfCont.o v_repExtVision.o ../common/v_repLib.o -o lib/libv_repExtVision.$(EXT) -lpthread -ldl -shared 