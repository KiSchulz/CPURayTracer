//
// Created by kis on 04.08.22.
//

#ifndef CPURAYTRACER_RENDERER_H
#define CPURAYTRACER_RENDERER_H

#include <olcPixelGameEngine/olcPixelGameEngine.h>
#include <vector>
#include <limits>
#include <thread>

#include "Camera.h"
#include "Vertex.h"
#include "Triangle.h"
#include "LinearAlgebra.h"
#include "Octree.h"
#include "Texture.h"
#include "Model.h"
#include "RayHit.h"

class Renderer {
private:
    olc::Sprite *backBuffer;

public:
    Camera camera;
    std::vector<ModelInstance> modelBuffer;

private:

    static inline Pixel
    calcPixelColor(const RayHit &hit, const Vec3f &dir) {
        //get the right pixel form the texture of the model
        const std::vector<Vertex> &vertexBuffer = hit.modelInstance->getVertexBuffer();
        Vec2f v0 = vertexBuffer[hit.triangle->v0].posTex,
                v0V1 = vertexBuffer[hit.triangle->v1].posTex - v0,
                v0V2 = vertexBuffer[hit.triangle->v2].posTex - v0;

        Vec2f texCoords = v0 + hit.u * v0V1 + hit.v * v0V2;

        Pixel texPixel = hit.modelInstance->getTexture().GetPixel((int) texCoords.x(), (int) texCoords.y());

        //calculate the angle between the normal of the triangle and the ray
        Vec3f v0v1 = hit.modelInstance->getVertexBuffer()[hit.triangle->v1].pos -
                     hit.modelInstance->getVertexBuffer()[hit.triangle->v0].pos;
        Vec3f v0v2 = hit.modelInstance->getVertexBuffer()[hit.triangle->v2].pos -
                     hit.modelInstance->getVertexBuffer()[hit.triangle->v0].pos;;
        Vec3f normal = v0v1.cross(v0v2).normalized();
        float factor = -normal.dot(dir);

        return Pixel{(uint8_t) (factor * (float) texPixel.r),
                     (uint8_t) (factor * (float) texPixel.g),
                     (uint8_t) (factor * (float) texPixel.b)};
    }

    static void applyToneMap(Pixel &p) {
        auto ACESFilm = [](float x) {
            x /= 255;

            float a = 2.51f;
            float b = 0.03f;
            float c = 2.43f;
            float d = 0.59f;
            float e = 0.14f;
            x = ((x * (a * x + b)) / (x * (c * x + d) + e));

            return (uint8_t) (x * 255);
        };
        p.r = ACESFilm(p.r);
        p.g = ACESFilm(p.g);
        p.b = ACESFilm(p.b);
    }

    Pixel renderPixel(const Vec3f &ray, const std::vector<Octree> &octrees) {
        RayHit hit;

        //check against all the geometry
        for (const auto &el: octrees) {
            el.checkForHit(camera.getPos(), ray, hit);
        }

        //calculate which color the pixel has if the ray did hit something
        Pixel p{0, 0, 0};
        if (hit.t < std::numeric_limits<float>::max()) {
            p = calcPixelColor(hit, ray);
        }

        return p;
    }

    void runThread(int threadIdx, int numThreads, const std::vector<Octree> *octrees) {
        const std::vector<Vec3f> &rayBuffer = camera.getRays();
        for (int i = threadIdx; i < rayBuffer.size(); i += numThreads) {
            Pixel p = renderPixel(rayBuffer[i], *octrees);
            applyToneMap(p);
            backBuffer->GetData()[i] = olc::Pixel{p.r, p.g, p.b};
        }
    }

public:
    explicit Renderer(olc::Sprite *backBuffer) : backBuffer(backBuffer),
                                                 camera({0, 0, 0}, {1, 0, 0}, 90, backBuffer->width,
                                                        backBuffer->height) {
    }

    void renderFrame() {
        std::vector<Octree> octrees;
        for (const auto &modelInstance: modelBuffer) {
            octrees.emplace_back(modelInstance);
        }


        unsigned int size = std::thread::hardware_concurrency();
        std::vector<std::thread> threads;
        for (unsigned int i = 0; i < size; i++) {
            std::thread t(&Renderer::runThread, this, i, size, &octrees);
            threads.push_back(std::move(t));
        }

        for (auto &thread: threads) {
            thread.join();
        }
    }
};

#endif //CPURAYTRACER_RENDERER_H
