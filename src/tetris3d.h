#pragma once

class Tetris3D {
  public:
    void StartUp() {}

    void Run() {
        while (true) {
            Update();
            Draw();
        }
    }

    void ShutDown() {}

  private:
    void Update() {}

    void Draw() {}
};