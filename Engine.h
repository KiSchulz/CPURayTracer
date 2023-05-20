//
// Created by kis on 04.08.22.
//

#ifndef CPURAYTRACER_ENGINE_H
#define CPURAYTRACER_ENGINE_H

#define OLC_IMAGE_STB
#define OLC_PGE_APPLICATION

#include <olcPixelGameEngine/olcPixelGameEngine.h>

#include "Renderer.h"
#include "ResourceLoader.h"

class Engine : public olc::PixelGameEngine {
    Renderer *renderer;
public:
    bool OnUserCreate() override {
        sAppName = "RAYTRACER";

        renderer = new Renderer(GetDrawTarget());

        ResourceLoader r{"../resources/"};

        Model m = r.LoadModel("suzanne.fbx", "textures/granite.png");
        renderer->modelBuffer.emplace_back(std::make_shared<Model>(m));
        renderer->modelBuffer[0].setPosition({3, 0, 0});

        // Model m = r.LoadModel("suzanne.fbx", "textures/granite.png");
       // renderer->modelBuffer.emplace_back(ModelInstance{std::make_shared<Model>(m)});
       // renderer->modelBuffer[0].setPosition({6, 2, 0});

       // Model m1 = r.LoadModel("face.fbx", "textures/granite.png");
       // renderer->modelBuffer.emplace_back(ModelInstance{std::make_shared<Model>(m1)});
       // renderer->modelBuffer[1].setPosition({6, -2, 0});
       // renderer->modelBuffer[1].setScale(0.0003);

        return true;
    }

    bool OnUserDestroy() override {
        delete renderer;
        return true;
    }

    bool OnUserUpdate(float dt) override {
        renderer->renderFrame();

        for (auto &el: renderer->modelBuffer) {
            Vec3f rotation = el.getRotation();
            rotation.z() -= dt * 1;
            el.setRotation(rotation);
        }

        return true;
    }
};

#endif //CPURAYTRACER_ENGINE_H
