CFLAGS = -I../../include -I../../remoteApi -Wall -DNON_MATLAB_PARSING -DMAX_EXT_API_CONNECTIONS=255
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

TARGET = lib/remoteApi.$(EXT)

all: remoteApiLib

remoteApiLib: $(OBJS)
		@mkdir -p lib
		@echo "Linking $(OBJS) to $(TARGET)"
		$(ECHO)$(CC) $(CFLAGS) $(OBJS) $(OPTION) -o $(TARGET) $(LDFLAGS)

%.o: %.c
		@echo "Compiling $< to $@"
		$(ECHO)$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
		@echo "Compiling $< to $@"
		$(ECHO)$(CC) $(CFLAGS) -c $< -o $@

clean:
		@echo "Cleaning $(OBJS) $(TARGET)"
		$(ECHO)rm -rf $(OBJS) $(TARGET)
