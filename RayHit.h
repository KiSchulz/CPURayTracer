//
// Created by kis on 28.09.22.
//

#ifndef CPURAYTRACER_RAYHIT_H
#define CPURAYTRACER_RAYHIT_H

#include "Triangle.h"
#include "Model.h"

#include <limits>

struct RayHit {
    const ModelInstance *modelInstance = nullptr;
    const Triangle *triangle = nullptr;
    float t = std::numeric_limits<float>::max(), u = 0, v = 0;
};

#endif //CPURAYTRACER_RAYHIT_H
