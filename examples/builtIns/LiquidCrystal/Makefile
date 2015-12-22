BUILDDIR=../../../build
TOOLCHAINPREFIX = arm-linux-gnueabihf-
CXX    = $(TOOLCHAINPREFIX)g++
HELLO_WORLD_OBJS= HelloWorld.o
COMMON_OBJS=LiquidCrystal.o
HELLO_WORLD_APP=helloworld
INCLUDES=-I$(BUILDDIR)/include
CFLAGS=-c -g -MD

all: $(HELLO_WORLD_APP)

$(HELLO_WORLD_APP): $(COMMON_OBJS) $(HELLO_WORLD_OBJS)
	$(CXX)  -L$(BUILDDIR)/lib -I$(BUILDDIR)/include -o $(HELLO_WORLD_APP) $(COMMON_OBJS) $(HELLO_WORLD_OBJS) $(BUILDDIR)/lib/libarduino.a -lpthread

.cpp.o:
	$(CXX) $(CFLAGS) $(INCLUDES) $< -o $@

clean:
	rm -f *.o $(HELLO_WORLD_APP)

test: clean $(HELLO_WORLD_APP)
	cp $(HELLO_WORLD_APP) /mnt/pi/home/src/arduinoBuilds