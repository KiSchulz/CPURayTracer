//
// Created by kis on 28.09.22.
//

#ifndef CPURAYTRACER_BOX_H
#define CPURAYTRACER_BOX_H

#include "LinearAlgebra.h"

struct Box {
    Vec3f min, max;

    Box() = default;

    explicit Box(const std::vector<Vec3f> &vec) {
        min = Vec3f(vec.front().x(), vec.front().y(), vec.front().z()),
                max = Vec3f(vec.front().x(), vec.front().y(), vec.front().z());

        for (const auto &v: vec) {
            min.x() = std::min(min.x(), v.x());
            min.y() = std::min(min.y(), v.y());
            min.z() = std::min(min.z(), v.z());
            max.x() = std::max(max.x(), v.x());
            max.y() = std::max(max.y(), v.y());
            max.z() = std::max(max.z(), v.z());
        }
    }

    [[nodiscard]] bool isInBox(const Vec3f &value) const {
        return isInInterval(min.x(), max.x(), value.x()) &&
               isInInterval(min.y(), max.y(), value.y()) &&
               isInInterval(min.z(), max.z(), value.z());
    }

    [[nodiscard]] bool rayBoxIntersect(const Vec3f &rayOrig, const Vec3f &rayDir, float &tMin) const {
        tMin = std::numeric_limits<float>::min();
        float tMax = std::numeric_limits<float>::max();

        if (rayDir.x() != 0.0f) {
            float tx1 = (min.x() - rayOrig.x()) / rayDir.x();
            float tx2 = (max.x() - rayOrig.x()) / rayDir.x();

            tMin = std::max(tMin, std::min(tx1, tx2));
            tMax = std::min(tMin, std::max(tx1, tx2));
        }

        if (rayDir.y() != 0.0f) {
            float ty1 = (min.y() - rayOrig.y()) / rayDir.y();
            float ty2 = (max.y() - rayOrig.y()) / rayDir.y();

            tMin = std::max(tMin, std::min(ty1, ty2));
            tMax = std::min(tMax, std::max(ty1, ty2));
        }

        if (rayDir.z() != 0.0f) {
            float tz1 = (min.z() - rayOrig.z()) / rayDir.z();
            float tz2 = (max.z() - rayOrig.z()) / rayDir.z();

            tMin = std::max(tMin, std::min(tz1, tz2));
            tMax = std::min(tMin, std::max(tz1, tz2));
        }

        return tMax >= tMin;
    }
};

#endif //CPURAYTRACER_BOX_H
