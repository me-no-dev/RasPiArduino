BUILDDIR=../../build
TOOLCHAINPREFIX = arm-linux-gnueabihf-
CXX    = $(TOOLCHAINPREFIX)g++

all: blinky

blinky: blinky.cpp
	$(CXX) -L$(BUILDDIR)/lib -I$(BUILDDIR)/include -o blinky blinky.cpp -larduino

test: blinky
	cp blinky /mnt/pi/home/src/arduinoBuilds