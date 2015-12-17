#piduino core location
CORE    = ../piduino/core
#piduino libraries location
LIBRARIES = ../piduino/libraries

#dir to use for build temp
OBJDIR = .build
#name of the output binary (the current folder name is used)
OUTPUT = $(shell basename `pwd`)
#location for the core build objects
COREOBJDIR = $(OBJDIR)/core

#should not need to touch anything bellow
CORESRCS = $(wildcard $(CORE)/*.cpp) $(wildcard $(CORE)/*.c)
COREOBJS = $(patsubst $(CORE)/%, $(COREOBJDIR)/%, $(filter-out %.cpp, $(filter-out %.c, $(CORESRCS:.c=.c.o) $(CORESRCS:.cpp=.cpp.o))))

LOCALSRCS = $(wildcard *.cpp) $(wildcard *.c)
LOCALDIRS = $(patsubst %, $(OBJDIR)/%, $(dir $(LOCALSRCS)))
LOCALINCS = $(foreach lib, $(dir $(LOCALSRCS)), -I$(lib))
LOCALOBJS = $(patsubst %, $(OBJDIR)/%, $(filter-out %.cpp, $(filter-out %.c, $(LOCALSRCS:.c=.c.o) $(LOCALSRCS:.cpp=.cpp.o))))

SRCS = $(CORESRCS) $(LOCALSRCS)
OBJS = $(COREOBJS) $(LOCALOBJS)

DEBUG  = -O3
CC     = gcc
CPP    = g++
INCLUDE= $(LOCALINCS) -I$(CORE) 
CFLAGS = $(DEBUG) -MD -Wall -Winline -pipe $(INCLUDE)
LDFLAGS= 
LDLIBS = -lpthread -lm

all: $(OUTPUT)

$(OUTPUT): $(OBJDIR) $(OBJS)
	@echo "[LD] $(OUTPUT)"
	@$(CPP) -o $(OUTPUT) $(OBJS) $(LDFLAGS) $(LDLIBS)

$(OBJDIR):
	@mkdir -p $(OBJDIR)
	$(foreach lib, $(LOCALDIRS), $(shell mkdir -p $(lib)))
	@mkdir -p $(COREOBJDIR)

$(OBJDIR)/%.c.o: %.c
	@echo "[CC] $<"
	@$(CC) -c  $(CFLAGS) $< -o $@

$(OBJDIR)/%.cpp.o: %.cpp
	@echo "[CP] $<"
	@$(CPP) -c $(CFLAGS) $< -o $@

$(COREOBJDIR)/%.c.o: $(CORE)/%.c
	@echo "[CC] $<"
	@$(CC) -c  $(CFLAGS) $< -o $@

$(COREOBJDIR)/%.cpp.o: $(CORE)/%.cpp
	@echo "[CP] $<"
	@$(CPP) -c $(CFLAGS) $< -o $@

$(OBJDIR)/%.c.o: $(LIBRARIES)/%.c
	@echo "[CC] $<"
	@$(CC) -c  $(CFLAGS) $< -o $@

$(OBJDIR)/%.cpp.o: $(LIBRARIES)/%.cpp
	@echo "[CP] $<"
	@$(CPP) -c $(CFLAGS) $< -o $@

clean:
	@rm -rf $(OBJDIR) $(OUTPUT)
	
run: all

include $(wildcard $(COREOBJDIR)/*.d)
include $(foreach lib, $(LIBOBJDIRS), $(wildcard $(lib)/*.d))
include $(wildcard $(OBJDIR)/*.d)
