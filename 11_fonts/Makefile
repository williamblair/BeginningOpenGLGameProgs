CC=g++
CFLAGS=-g -std=c++11
INCDIRS=-I. -I/usr/include/freetype2
LIBDIRS=
LIBS=-lSDL2 -lGLEW -lGL -lfreetype
SOURCES=main.cpp          \
        Shader.cpp        \
        HeightMap.cpp     \
        TargaImage.cpp    \
		Tree.cpp          \
        FreetypeFont.cpp
TARGET=main

all:
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET) $(INCDIRS) $(LIBDIRS) $(LIBS)

