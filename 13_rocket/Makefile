CC=g++
CFLAGS=-g -std=c++11
INCDIRS=-I. -I/usr/include/freetype2
LIBDIRS=
LIBS=-lSDL2 -lGLEW -lGL -lfreetype
OBJS=main.o                 \
     Shader.o               \
     HeightMap.o            \
     TargaImage.o           \
     Tree.o                 \
     FreetypeFont.o         \
     Md2Model.o             \
     Explosion.o            \
     GameWorld.o            \
     Entity.o		        \
	 Enemy.o		        \
	 Ogro.o                 \
     Collider.o             \
     SphereCollider.o       \
     TerrainCollider.o      \
     Camera.o			    \
     MouseInterface.o       \
     KeyboardInterface.o	\
     Player.o               \
     Frustum.o              \
     Rocket.o
TARGET=main

all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(INCDIRS) $(LIBDIRS) $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $< $(INCDIRS)

clean:
	rm -f $(OBJS)

