

ifeq ($(shell uname -s), Darwin)
CCFLAGS=-std=c++17 \
	-Wall -Wno-deprecated-declarations\
	-g \
	-lglfw -framework CoreVideo -framework OpenGL -framework IOKit -framework Cocoa -framework Carbon
else
CCFLAGS=-std=c++17 \
	-Wall -Wno-deprecated-declarations\
	-g \
	-static-libstdc++ -static-libgcc \
	-lGLU -lGL -lX11 -lpthread -lXrandr -lXi -ldl
endif

.PHONY: game

game: src/main.cc \
	  src/app.h \
	  src/app.cc \
	  src/common.h \
	  src/game.h \
	  src/camera.h \
	  src/input.h \
	  src/settings.h \
	  src/logic.h \
	  src/logic.cc \
	  src/renderer.h \
	  src/renderer.cc \
	  src/settings.h
	mkdir -p bin/
	g++ src/main.cc src/app.cc src/logic.cc src/renderer.cc \
	extern/libglfw3.a $(CCFLAGS) -o bin/tetris3d
