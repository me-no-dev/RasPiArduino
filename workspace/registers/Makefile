#toolchain location
TCPATH     = ../../arm-none-linux-gnueabi
TCINCLUDES = -I$(TCPATH)/arm-none-linux-gnueabi/include
TCLIBS     = -L$(TCPATH)/arm-none-linux-gnueabi/lib
BINPRE     = $(TCPATH)/bin/arm-none-linux-gnueabi-
#piduino core location
CORE    = ../../piduino/core
#piduino libraries location
LIBRARIES = ../../piduino/libraries
#ssh auth for uploading 
SSHUSR = pi
SSHHST = pi2.home
SSHDST = /home/pi

#dir to use for build temp
OBJDIR = .build
#name of the output binary (the current folder name is used)
OUTPUT = $(shell basename `pwd`)
#location for the core build objects
COREOBJDIR = $(OBJDIR)/core

#should not need to touch anything bellow

rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))
get_library_includes = $(if $(and $(wildcard $(1)/src), $(wildcard $(1)/library.properties)), -I$(1)/src, $(addprefix -I,$(1) $(wildcard $(1)/utility)))
get_library_files  = $(if $(and $(wildcard $(1)/src), $(wildcard $(1)/library.properties)), $(call rwildcard,$(1)/src/,*.$(2)), $(wildcard $(1)/*.$(2) $(1)/utility/*.$(2)))
get_local_files  = $(call rwildcard,*/,*.$(1)) $(wildcard *.$(1))

#get local libararies used and the libraries they might use
LOCALALL   = $(call get_local_files,cpp) $(call get_local_files,c) $(call get_local_files,h)
LOCALUSEDLIBS   = $(filter $(notdir $(wildcard $(LIBRARIES)/*)), $(shell sed -ne 's/^ *\# *include *[<\"]\(.*\)\.h[>\"]/\1/p' $(LOCALALL)))
USEDLIBDIRS= $(wildcard $(patsubst %,$(LIBRARIES)/%,$(LOCALUSEDLIBS)))

LIBINCALL  = $(foreach lib, $(USEDLIBDIRS), $(call get_library_files,$(lib),h) $(call get_library_files,$(lib),c) $(call get_library_files,$(lib),cpp))
LIBINCUSD  = $(filter $(notdir $(wildcard $(LIBRARIES)/*)), $(shell sed -ne 's/^ *\# *include *[<\"]\(.*\)\.h[>\"]/\1/p' $(LIBINCALL)))

#all libs that have been found to be used
USEDLIBS   = $(sort $(LIBINCUSD) $(LOCALUSEDLIBS))

#the paths to the lib folders
LIBDIRS    = $(wildcard $(patsubst %,$(LIBRARIES)/%,$(USEDLIBS)))

#all folders in the libs that should be searched for headers
LIBINCLUDES= $(foreach lib, $(LIBDIRS), $(call get_library_includes,$(lib)))


CORESRCS = $(wildcard $(CORE)/*.cpp) $(wildcard $(CORE)/*.c)
COREOBJS = $(patsubst $(CORE)/%, $(COREOBJDIR)/%, $(filter-out %.cpp, $(filter-out %.c, $(CORESRCS:.c=.c.o) $(CORESRCS:.cpp=.cpp.o))))

LIBSRCS   = $(foreach lib, $(LIBDIRS), $(call get_library_files,$(lib),cpp)) $(foreach lib, $(LIBDIRS), $(call get_library_files,$(lib),c))
LIBOBJS   = $(patsubst $(LIBRARIES)/%, $(OBJDIR)/%, $(filter-out %.cpp, $(filter-out %.c, $(LIBSRCS:.c=.c.o) $(LIBSRCS:.cpp=.cpp.o))))
LIBOBJDIRS= $(patsubst %, $(OBJDIR)/%, $(USEDLIBS))

LOCALSRCS = $(call get_local_files,cpp) $(call get_local_files,c)
LOCALDIRS = $(patsubst %, $(OBJDIR)/%, $(dir $(LOCALSRCS)))
LOCALINCS = $(foreach lib, $(dir $(LOCALSRCS)), -I$(lib))
LOCALOBJS = $(patsubst %, $(OBJDIR)/%, $(filter-out %.cpp, $(filter-out %.c, $(LOCALSRCS:.c=.c.o) $(LOCALSRCS:.cpp=.cpp.o))))

SRCS = $(CORESRCS) $(LIBSRCS) $(LOCALSRCS)
OBJS = $(COREOBJS) $(LIBOBJS) $(LOCALOBJS)

DEBUG  = -O3
CC     = $(BINPRE)gcc
CPP    = $(BINPRE)g++
INCLUDE= $(LOCALINCS) $(LIBINCLUDES) -I$(CORE) $(TCINCLUDES)
CFLAGS = $(DEBUG) -MD -Wall -Winline -pipe $(INCLUDE)
LDFLAGS= $(TCLIBS)
LDLIBS = -lpthread -lm

all: info $(OUTPUT) upload

info:
	@echo "USEDLIBS  : $(USEDLIBS)"

$(OUTPUT): $(OBJDIR) $(OBJS)
	@echo "[LD] $(OUTPUT)"
	@$(CPP) -o $(OUTPUT) $(OBJS) $(LDFLAGS) $(LDLIBS)

$(OBJDIR):
	@mkdir -p $(OBJDIR)
	$(foreach lib, $(LOCALDIRS), $(shell mkdir -p $(lib)))
	@mkdir -p $(COREOBJDIR)
	$(foreach lib, $(LIBOBJDIRS), $(shell mkdir -p $(lib)))

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

upload:
	@scp $(OUTPUT) $(SSHUSR)@$(SSHHST):$(SSHDST)
	@ssh $(SSHUSR)@$(SSHHST) "chmod +x $(SSHDST)/$(OUTPUT)"
	
run: all
#	ssh $(SSHUSR)@$(SSHHST) "$(SSHDST)/$(OUTPUT)"

include $(wildcard $(COREOBJDIR)/*.d)
include $(foreach lib, $(LIBOBJDIRS), $(wildcard $(lib)/*.d))
include $(wildcard $(OBJDIR)/*.d)
