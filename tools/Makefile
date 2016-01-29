$(info Current directory is $(PWD))

# The Variant of the build
VARIANT=bplus

CORE    = ../cores/piduino
#dir to use for build temp
BUILDDIR = build

TOOLCHAINPREFIX = arm-linux-gnueabihf-

DEBUG=
#name of the output binary (the current folder name is used)
OUTPUT_DYNAMIC = $(BUILDDIR)/libPiDuino.so
OUTPUT_STATIC = $(BUILDDIR)/libPiDuino.a

#should not need to touch anything bellow
CORESRCS = $(wildcard $(CORE)/*.cpp) $(wildcard $(CORE)/*.c)
OBJS     = $(patsubst $(CORE)/%, $(BUILDDIR)/%, $(filter-out %.cpp, $(filter-out %.c, $(CORESRCS:.c=.c.o) $(CORESRCS:.cpp=.cpp.o))))

CC      = $(TOOLCHAINPREFIX)gcc
CPP     = $(TOOLCHAINPREFIX)g++
INCLUDE = -I$(CORE) -I../variants/$(VARIANT)
CFLAGS  = $(DEBUG) -fPIC -MD -Wall -Winline -pipe $(INCLUDE) -g
LDFLAGS = -shared
LDLIBS  = -lpthread -lm

all: mkBuildDir $(OUTPUT_DYNAMIC)

mkBuildDir:
	mkdir -p $(BUILDDIR)

$(OUTPUT_DYNAMIC): $(OBJS)
	@echo "[LD] $(OUTPUT_DYNAMIC)"
	$(CPP) -g -o $(OUTPUT_DYNAMIC) $(OBJS) $(LDFLAGS) $(LDLIBS)
	ar crv $(OUTPUT_STATIC) $(OBJS)

$(BUILDDIR)/%.c.o: $(CORE)/%.c
	@echo "[CC] $<"
	$(CC) -c  $(CFLAGS) $< -o $@

$(BUILDDIR)/%.cpp.o: $(CORE)/%.cpp
	@echo "[CP] $<"
	$(CPP) -c $(CFLAGS) $< -o $@

clean:
	@echo "Cleaning any previous builds"
	@rm -rf $(BUILDDIR)
