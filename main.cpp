#include <iostream>

#include "Engine.h"

int main() {
    auto* engine = new Engine();

    const int width = 800, height = 600;
    const bool fullscreen = false, vsync = true;

    if (engine->Construct(width, height, 1, 1, fullscreen, vsync)) {
        engine->Start();
    }

    delete engine;

    return 0;
}
