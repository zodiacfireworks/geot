#*****************************************************************************80
#
#   PROGRAM    :
#                GeoT
#
#   PURPOSE    :
#                Program to show Goemetric Transformations as an applied
#                exploration SFML multimedia library.
#
#   PROGRAMMER :
#                Martín Josemaría <martin.vuelta@gmail.com>
#
#                * Software Development and Research
#                  SoftButterfly
#                  Lima 	Peru
#
#                * Faculty of Physical Science
#                  Universidad Nacional Mayor de San Marcos
#                  Lima 	Peru
#
#   FILE       :
#                makefile
#
#   DESCRIPTION:
#                This file contains instructions to compile the program.
#
#*****************************************************************************80
# Directories
BUILDDIR    = build
SRCDIR      = src

# Resources
BUILDRESDIR = $(BUILDDIR)/resources
BUILDRES    = $(BUILDRESDIR)/ball.wav $(BUILDRESDIR)/sansation.ttf
SRCRESDIR   = $(SRCDIR)/resources
SRCRES      = $(SRCRESDIR)/ball.wav $(SRCRESDIR)/sansation.ttf $(SRCRESDIR)/graffiti.ttf $(SRCRESDIR)/sphere.png $(SRCRESDIR)/brick.png $(SRCRESDIR)/grass.png

# C++ Compiler options
CXX     = g++
SRCCXX  = $(SRCDIR)/main.cpp
OBJCXX  = $(BUILDDIR)/geot.o
FLAGSCXX= -g -W -Wall -Werror -Wextra -Wshadow -Wconversion -Wpointer-arith -Wcast-qual -Wcast-align -Wwrite-strings -Wunused -Wunused-function -Wunused-label -Wunused-parameter -Wunused-value -Wunused-variable -Wmissing-braces -Wswitch -Wswitch-default -Wswitch-enum

# Linker options
LINKER  = g++
OBJL    = $(BUILDDIR)/geot.o
LIBL    = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
FLAGSL  = -g -O4

# Resources

ifeq ($(OS),Windows_NT)
    BINL    = $(BUILDDIR)/geot.exe
    RM      = del /Q
    COPY    = COPY
    MKDIR   = MKDIR
    FixPath = $(subst /,\,$1)
    ClrCsl  = CLS
    STDCXX  = --std=c++11
else
    BINL    = $(BUILDDIR)/geot
    RM      = rm -rf
    COPY    = cp
    MKDIR   = mkdir
    FixPath = $1
    ClrCsl  = clear
endif


.PHONY: all all-before all-after clean clean-custom

all: all-before $(OBJL) $(BINL) clean-custom all-after

all-before:
ifeq ($(OS),Windows_NT)
ifneq ($(wildcard $(BUILDRESDIR)),)
	$(foreach res,$(call FixPath,$(SRCRES)),COPY $(res) $(call FixPath,$(BUILDRESDIR)) &)
else
	$(MKDIR) $(call FixPath,$(BUILDRESDIR)) & $(foreach res,$(call FixPath,$(SRCRES)),COPY $(res) $(call FixPath,$(BUILDRESDIR)) &)
endif
else
	$(MKDIR) $(BUILDRESDIR) 2>/dev/null; $(COPY) $(call FixPath,$(SRCRES) $(BUILDRESDIR))
endif

clean:
	$(RM) $(call FixPath, $(OBJL) $(BINL) $(BUILDRESDIR))

clean-custom:
	$(RM) $(call FixPath, $(OBJL) $(OBJCXX))

$(OBJCXX): $(GLOBALDEPS)
ifeq ($(OS),Windows_NT)
	$(CXX) -c $(call FixPath,$(SRCCXX)) -o $(call FixPath,$(OBJCXX)) $(FLAGSCXX) $(STDCXX)
else
	$(CXX) -c $(call FixPath,$(SRCCXX)) -o $(call FixPath,$(OBJCXX)) $(FLAGSCXX)
endif

$(BINL): $(OBJCXX)
	$(LINKER) -o $(call FixPath,$(BINL) $(OBJL)) $(LIBL) $(FLAGSL)
