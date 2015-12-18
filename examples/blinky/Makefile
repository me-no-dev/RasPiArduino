BUILDDIR=../../build
TOOLCHAINPREFIX = arm-linux-gnueabihf-
CXX    = $(TOOLCHAINPREFIX)g++

all: blinky

blinky: blinky.cpp
	$(CXX) -g -MD -L$(BUILDDIR)/lib -I$(BUILDDIR)/include -o blinky blinky.cpp $(BUILDDIR)/lib/libarduino.a -lpthread

clean:
	rm -f blinky

test: clean blinky
	cp blinky /mnt/pi/home/src/arduinoBuilds