CFLAGS = -I../include -Wall -fPIC -static

OS = $(shell uname -s)
ifeq ($(OS), Linux)
	EXT = so
else
	EXT = dylib
endif

all: 
	g++ $(CFLAGS) -c confReader.cpp -o confReader.o
	g++ $(CFLAGS) -c inConnection.cpp -o inConnection.o
	g++ $(CFLAGS) -c porting.cpp -o porting.o
	g++ $(CFLAGS) -c simxCmd.cpp -o simxCmd.o
	g++ $(CFLAGS) -c simxConnections.cpp -o simxConnections.o
	g++ $(CFLAGS) -c simxUtils.cpp -o simxUtils.o
	g++ $(CFLAGS) -c simxSocket.cpp -o simxSocket.o
	g++ $(CFLAGS) -c simxContainer.cpp -o simxContainer.o
	g++ $(CFLAGS) -c v_repExtRemoteApi.cpp -o v_repExtRemoteApi.o
	g++ $(CFLAGS) -c ../common/v_repLib.cpp -o ../common/v_repLib.o
	@mkdir -p lib
	g++ confReader.o inConnection.o porting.o simxCmd.o simxConnections.o simxUtils.o simxSocket.o simxContainer.o v_repExtRemoteApi.o ../common/v_repLib.o -o lib/libv_repExtRemoteApi.$(EXT) -lpthread -ldl -shared 