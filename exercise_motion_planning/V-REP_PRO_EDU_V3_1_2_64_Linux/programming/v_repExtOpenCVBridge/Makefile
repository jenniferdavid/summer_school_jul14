CFLAGS = -I./include -I./v_repExtOpenCVBridge -Wall -fPIC `pkg-config --cflags opencv`
LDFLAGS = -lpthread -ldl `pkg-config --libs opencv`

OBJS = v_repExtOpenCVBridge/v_repExtOpenCVBridge.o common/v_repLib.o v_repExtOpenCVBridge/myOpenCVElab.o

OS = $(shell uname -s)
ECHO=@

ifeq ($(OS), Linux)
	CFLAGS += -D__linux
	OPTION = -shared
	EXT = so
else
	CFLAGS += -D__APPLE__
	OPTION = -dynamiclib -current_version 1.0
	EXT = dylib
endif

TARGET = libv_repExtOpenCVBridge.$(EXT)

all: v_repExtOpenCVBridgeLib

v_repExtOpenCVBridgeLib: $(OBJS)
		@echo "Linking $(OBJS) to $(TARGET)"
		$(ECHO)$(CXX) $(CFLAGS) $(OBJS) $(OPTION) -o $(TARGET) $(LDFLAGS)

%.o: %.c
		@echo "Compiling $< to $@"
		$(ECHO)$(CXX) $(CFLAGS) -c $< -o $@

%.o: %.cpp
		@echo "Compiling $< to $@"
		$(ECHO)$(CXX) $(CFLAGS) -c $< -o $@

clean:
		@echo "Cleaning $(OBJS) $(TARGET)"
		$(ECHO)rm -rf $(OBJS) $(TARGET)
