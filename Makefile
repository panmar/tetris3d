

ifeq ($(shell uname -s), Darwin)
CCFLAGS=-std=c++17 \
	-Wall -Wno-deprecated-declarations\
	-g \
	-I\src\
	-lglfw -framework CoreVideo -framework OpenGL -framework IOKit -framework Cocoa -framework Carbon
else
CCFLAGS=-std=c++17 \
	-Wall -Wno-deprecated-declarations\
	-g \
	-Isrc\ \
	-static-libstdc++ -static-libgcc \
	-lGLU -lGL -lX11 -lpthread -ldl
endif

FILES=src/main.cc \
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

.PHONY: game

ifeq ($(shell uname -s), Darwin)
game: $(FILES)
	mkdir -p bin/
	g++ src/main.cc src/app.cc src/logic.cc src/renderer.cc src/glad.cc \
	$(CCFLAGS) -o bin/tetris3d
else
game: $(FILES)
	mkdir -p bin/
	g++ src/main.cc src/app.cc src/logic.cc src/renderer.cc src/glad.cc \
	extern/libglfw3.a $(CCFLAGS) -o bin/tetris3d
endif
