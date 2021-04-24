# Version of the software
VERSION=0.1.0

# Compiler options
INC_XFT=`pkg-config --cflags xft`
INC_FT=`pkg-config --cflags freetype2`
CC=clang
CFLAGS=$(INC_XFT) $(INC_FT)
LIB_X11=`pkg-config --cflags --libs x11`
LIB_FONTCONFIG=`pkg-config --cflags --libs fontconfig`
LIB_UTIL=-lutil
LIB_XFT=`pkg-config --cflags --libs xft`
LIB_FT=`pkg-config --cflags --libs freetype2`
LIB_HARFBUZZ=`pkg-config --cflags --libs harfbuzz`
LIBS=$(LIB_X11)  $(LIB_FONTCONFIG) $(LIB_UTIL) $(LIB_FT) $(LIB_XFT) $(LIB_HARFBUZZ)
LDFLAGS=$(LIBS)

# Installation paths
PREFIX=/usr/local
