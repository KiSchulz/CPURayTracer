//
// Created by kis on 11.08.22.
//

#ifndef CPURAYTRACER_TEXTURE_H
#define CPURAYTRACER_TEXTURE_H

#include <cstdint>
#include <vector>
#include <olcPixelGameEngine/olcPixelGameEngine.h>

struct Pixel {
    uint8_t r, g, b;

    Pixel() = default;

    Pixel(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
};


struct Texture {
    int width, height;
    std::vector<Pixel> data;

    Texture() : width(0), height(0), data() {
    }

    Texture(int width, int height) : width(width), height(height), data(width * height) {}

    [[nodiscard]] const Pixel &GetPixel(int x, int y) const {
        return data[y * width + x];
    }

    void SetPixel(int x, int y, const Pixel &pixel) {
        data[y * width + x] = pixel;
    }
};

#endif //CPURAYTRACER_TEXTURE_H
