CCFLAGS=-std=c++17 -Wall -g

.PHONY: game

game: src/main.cc
	mkdir -p bin/
	g++ $(CCFLAGS) src/main.cc -o bin/tetris3d
