CFLAGS = -I../../include -I../../remoteApi -I. -I/usr/lib/jvm/java-7-openjdk-i386/include -I/usr/lib/jvm/java-7-openjdk-amd64/include -I/System/Library/Frameworks/javaVM.framework/Headers -Wall -D_Included_extApiJava=1 -DNON_MATLAB_PARSING -DMAX_EXT_API_CONNECTIONS=255
LDFLAGS = -lpthread -ldl

OBJS = ../../remoteApi/extApi.o ../../remoteApi/extApiPlatform.o

OS = $(shell uname -s)
ECHO=@

ifeq ($(OS), Linux)
	CFLAGS += -D__linux -fPIC
	OPTION = -shared
	EXT = so
else
	CFLAGS += -D__APPLE__ -fPIC
	OPTION = -dynamiclib -current_version 1.0
	EXT = dylib
endif

TARGET = lib/libremoteApiJava.$(EXT)

all: remoteApiLib

remoteApiLib: $(OBJS)
		@mkdir -p lib
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
