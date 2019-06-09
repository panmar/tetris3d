CCFLAGS=-std=c++17 \
	-Wall -Wno-deprecated-declarations\
	-g \
	-lglfw -framework CoreVideo -framework OpenGL -framework IOKit -framework Cocoa -framework Carbon

.PHONY: game

game: src/main.cc src/app.h src/app.cc src/common.h src/game.h \
	 src/camera.h src/settings.h src/logic.h src/logic.cc src/renderer.h
	mkdir -p bin/
	g++ $(CCFLAGS) src/main.cc src/app.cc src/logic.cc -o bin/tetris3d
