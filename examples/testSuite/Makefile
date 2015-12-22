APP=testSuite
BUILDDIR=../../build
TOOLCHAINPREFIX = arm-linux-gnueabihf-
CXX    = $(TOOLCHAINPREFIX)g++
CFLAGS=-g -MD
INCLUDEDIRS=-I$(BUILDDIR)/include
OBJS=testSuite.o time.o


all: $(APP)

$(APP): $(OBJS)
	$(CXX) -L$(BUILDDIR)/lib  -o $(APP) $(OBJS) $(BUILDDIR)/lib/libarduino.a -lpthread

.cpp.o:
	$(CXX) $(CFLAGS) $(INCLUDEDIRS) -c $< -o $@
	
clean:
	rm -f $(APP) $(OBJS)

test: clean $(APP)
	cp $(APP) /mnt/pi/home/src/arduinoBuilds