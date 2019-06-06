CCFLAGS=-std=c++17 \
	-Wall \
	-g \
	-lglfw -framework CoreVideo -framework OpenGL -framework IOKit -framework Cocoa -framework Carbon

.PHONY: game

game: src/main.cc src/tetris3d.h
	mkdir -p bin/
	g++ $(CCFLAGS) src/main.cc -o bin/tetris3d
