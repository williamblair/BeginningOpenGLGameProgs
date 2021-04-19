CC=g++
CFLAGS=-g -std=c++11
INCDIRS=-I.
LIBDIRS=
LIBS=-lSDL2 -lGLEW -lGL
SOURCES=main.cpp \
        Shader.cpp \
        HeightMap.cpp \
        TargaImage.cpp \
		Tree.cpp
TARGET=main

all:
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET) $(INCDIRS) $(LIBDIRS) $(LIBS)

