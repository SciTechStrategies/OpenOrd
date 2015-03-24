#***************************************************************************
#                               Makefile
#                          -------------------
#
#  _________________________________________________________________________
#
#  Configuration for Makefiles
#
#  _________________________________________________________________________
#
#    begin                : Thu Oct 22 2003
#    copyright            : (C) 2003 by W. Michael Brown
#    email                : wmbrown@sandia.gov
# ***************************************************************************/

#Locations of outside objects relative to a source directory
HOBJ_DIR = ../obj

ALL_DIR	= ./
ALL_LIB = $(HOBJ_DIR)/liball.a

MATH_DIR = ./
MATH_LIB = $(HOBJ_DIR)/libmath.a

EVERY_LIB = $(MATH_LIB) $(ALL_LIB)

BIN_DIR = ../bin/


# Compiler Setup
CPP         = mpiCC
CC          = mpicc
AR          = ar
INC         = -I$(ALL_DIR) -I$(MATH_DIR)
DBUG        = #-g -Wall -pedantic -g #-DDEBUG #-ansi
OPT         = -DMUSE_MPI -O3 
CFLAGS      = $(OPT) $(MOVIE) $(DBUG) $(INC) $(GSLC) $(LIBGAC) -c
LFLAGS      = $(OPT) $(GSLL) $(LIBGAL)

# Distribution Directories
DIST_BIN		= /home/wmbrown/distbin/
DIST_MAN    = /home/wmbrown/cpp/manpages/man1/
DIST_DOC		= /home/wmbrown/cpp/doc/
