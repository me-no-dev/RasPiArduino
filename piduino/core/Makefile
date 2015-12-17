#piduino core location
$(info Current directory is $(PWD))
CORE    = .
BUILDDIR = build
INSTALLDIR = ../../build
TESTDIR=/mnt/pi/home/src/arduinoBuilds

#piduino libraries location
LIBRARIES = ../piduino/libraries
TOOLCHAINPREFIX = arm-linux-gnueabihf-

#dir to use for build temp
OBJDIR = build


#location for the core build objects
COREOBJDIR = $(OBJDIR)/core

#name of the output binary (the current folder name is used)
OUTPUT = $(COREOBJDIR)/libarduino.so

#should not need to touch anything bellow
CORESRCS = $(wildcard $(CORE)/*.cpp) $(wildcard $(CORE)/*.c)
COREOBJS = $(patsubst $(CORE)/%, $(COREOBJDIR)/%, $(filter-out %.cpp, $(filter-out %.c, $(CORESRCS:.c=.c.o) $(CORESRCS:.cpp=.cpp.o))))


SRCS = $(CORESRCS)
OBJS = $(COREOBJS) 

DEBUG  = -O3
CC     = $(TOOLCHAINPREFIX)gcc
CPP    = $(TOOLCHAINPREFIX)g++
INCLUDE= $(LOCALINCS) -I$(CORE) 
CFLAGS = $(DEBUG) -fPIC -MD -Wall -Winline -pipe $(INCLUDE)
LDFLAGS= -shared
LDLIBS = -lpthread -lm

all: $(OUTPUT)

$(OUTPUT): $(OBJS)
	@echo "[LD] $(OUTPUT)"
	@$(CPP) -o $(OUTPUT) $(OBJS) $(LDFLAGS) $(LDLIBS)

$(COREOBJDIR)/%.c.o: %.c
	@echo "[CC] $<"
	@$(CC) -c  $(CFLAGS) $< -o $@

$(COREOBJDIR)/%.cpp.o: %.cpp
	@echo "[CP] $<"
	@$(CPP) -c $(CFLAGS) $< -o $@

clean:
	@echo "Cleaning any previous builds"
	@find . -type f \( -name "*.o" -o -name "*.d" \) -delete
	@rm -f $(OUTPUT)
	
run: all

install: all
	@echo "Copying build to install directory $(INSTALLDIR)"
	@mkdir -p $(INSTALLDIR)/lib $(INSTALLDIR)/include
	@cp $(CORE)/*.h $(INSTALLDIR)/include
	@cp $(OUTPUT) $(INSTALLDIR)/lib

test: install
	cp -r $(INSTALLDIR) $(TESTDIR)
