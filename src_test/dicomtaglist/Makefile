GXX=g++

TARGET=dcmtlv

CFLAGS += -std=c++11
CFLAGS += -DDEBUG
CFLAGS += -g3

# Automatic detecting architecture.
KRNL := $(shell uname -s)
KVER := $(shell uname -r | cut -d . -f1)
ARCH := $(shell uname -m)

ifeq ($(KRNL),Darwin)
    GCC = llvm-gcc
    GPP = llvm-g++
    ifeq ($(shell test $(KVER) -gt 19; echo $$?),0)
        ## macOS 11~ may built as universal binary.
        OPTARCH += -arch x86_64 -arch arm64
        LIBARCH = macos11
    else
        LIBARCH = macos10
    endif
else
    SUBSYS := $(shell uname -s | cut -d _ -f1)
    ifeq ($(SUBSYS),MINGW64)
        CFLAGS  += -mconsole
        OPTARCH +=  -static 
        LDFLAGS += -static
        LIBARCH = mingw64
    else
        LIBARCH = linux_$(ARCH)
    endif
    OPTARCH += -fopenmp
endif


TINYDICOM_PATH = ../../lib

CFLAGS += -DWIN_32_LEAN_AND_MEAN
CFLAGS += -I$(TINYDICOM_PATH)

LDFLAGS += -L$(TINYDICOM_PATH)
LDFLAGS += -ltinydicomd
LDFLAGS += $(OPTLIBS)

SRCS += src/main.cpp

OBJS = $(SRCS:src/%.cpp=obj/%.o)

all: prepare continue
continue: bin/$(TARGET)

clean:
	@rm -rf bin/$(TARGET)
	@rm -rf obj/*.o

prepare:
	@mkdir -p obj
	@mkdir -p bin

$(OBJS): obj/%.o: src/%.cpp
	@echo "compiling $< ..."
	@$(GXX) $(CFLAGS) -c $< -o $@

bin/$(TARGET): $(OBJS)
	@echo "Linking $@ ..."
	@$(GXX) $(CFLAGS) $^ $(LDFLAGS) -o $@
