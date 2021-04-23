# Version of the software
VERSION=0.1.0

# Compiler options
CC=clang
CFLAGS=

LIB_X11=`pkg-config --cflags --libs x11`
LIB_UTIL=-lutil
LIBS=$(LIB_X11) $(LIB_UTIL)
LDFLAGS=$(LIBS)

# Installation paths
PREFIX=/usr/local
