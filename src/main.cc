#include <iostream>

#include "app.h"

int main() {
    try {
        tetris3d::Config config{
            .graphics_renderer_type = tetris3d::Config::RendererType::Advanced,
            .window_title = "Tetris 3D",
            .graphics_resolution_width = 1280,
            .graphics_resolution_height = 720,
            .rand_seed = static_cast<unsigned>(42),
        };
        auto app = tetris3d::App(config);
        app.Run();
    } catch (const tetris3d::Error& error) {
        std::cerr << error.what() << std::endl;
        return 1;
    }

    return 0;
}