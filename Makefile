# Makefile for libtinydicom
# (C)2016 Raphael Kim / rageworx
#

# To enable build for embedded linux, you may encomment next 2 lines.
# CCPREPATH = ${ARM_LINUX_GCC_PATH}
# CCPREFIX  = arm-linux-

# To enable build for embedded linux, change following line.
# CCPATH    = ${CCPREPATH}/${CCPREFIX}
CCPATH =

# Compiler configure.
GCC = ${CCPATH}gcc
GPP = ${CCPATH}g++
AR  = ${CCPATH}ar

SOURCEDIR = ./src
LIBSRCDIR = ./src_lib
OUTDIR    = ./lib

ifeq (debug,$(firstword $(MAKECMDGOALS)))
	OBJDIR := ./obj/Debug
	OUTBIN := ./libtinydicomd.a
else
	OBJDIR := ./obj/Release
	OUTBIN := libtinydicom.a
endif

# Make all source code be compiled ..
SRCS = $(wildcard $(SOURCEDIR)/*.cpp)
OBJS = $(SRCS:$(SOURCEDIR)/%.cpp=$(OBJDIR)/%.o)
LOBJ = $(OBJDIR)/libdcm.o

DEFINEOPT = -DRAWPROCESSOR_USE_LOCALTCHAR

ifeq (debug,$(firstword $(MAKECMDGOALS)))
	OPTIMIZEOPT :=
	MOREOPT := -g
else
	OPTIMIZEOPT := -O3 -s
	MOREOPT :=
endif

CFLAGS    = -I$(SOURCEDIR) -I$(LIBSRCDIR) $(DEFINEOPT) $(OPTIMIZEOPT) $(MOREOPT)

all: prepare clean ${OUTDIR}/${OUTBIN}

debug: all

prepare:
	@mkdir -p ${OBJDIR}
	@mkdir -p ${OUTDIR}

clean:
	@rm -rf ${OBJDIR}/*
	@rm -rf ${OUTDIR}/${OUTBIN}

$(OBJS): $(OBJDIR)/%.o: $(SOURCEDIR)/%.cpp
	@$(GPP) $(CFLAGS) -c $< -o $@

$(LOBJ):
	@$(GPP) -I$(LIBSRCDIR) $(CFLAGS) -c $(LIBSRCDIR)/libdcm.cpp -o $@

$(OUTDIR)/$(OUTBIN): $(OBJS) $(LOBJ)
	@$(AR) -q $@ $(OBJDIR)/*.o
