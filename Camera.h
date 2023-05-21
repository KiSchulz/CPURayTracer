//
// Created by kis on 04.08.22.
//

#ifndef CPURAYTRACER_CAMERA_H
#define CPURAYTRACER_CAMERA_H


#include "LinearAlgebra.h"
#include <utility>
#include <cmath>

class Camera {
    // direction is in spherical coordinates
    Vec3f pos, direction; // Maybe pull this up to an Entity class
    int screenWidth, screenHeight;
    float fov;

    std::vector<Vec3f> rayBuffer;
    bool rayBufferIsValid;

private:
    static Vec3f cartesianToSpherical(const Vec3f &vec) {
        // x = r
        // y = theta
        // z = phi
        // iso 80000-2:2019
        float r = vec.norm();
        float theta = std::acos(vec.z() / r);
        float phi = std::atan2(vec.y(), vec.x());

        return {r, theta, phi};
    }

    static Vec3f sphericalToCartesian(const Vec3f &vec) {
        // usage of vector according to the comment in cartesianToSpherical
        float x = vec.x() * std::sin(vec.y()) * std::cos(vec.z());
        float y = vec.x() * std::sin(vec.y()) * std::sin(vec.z());
        float z = vec.x() * std::cos(vec.y());

        return {x, y, z};
    }

    void updateRayBuffer() {
        const float stepSize = fov / (float) screenWidth;
        //Initialize vec to top left corner of the Screen
        Vec3f vec{direction.x(),
                  direction.y() - (0.5f * ((float) screenHeight / (float) screenWidth) * fov - 0.5f * stepSize),
                  direction.z() + (0.5f * fov - 0.5f * stepSize)};

        for (int i = 0; i < screenHeight; i++) {
            Vec3f row = vec;
            for (int j = 0; j < screenWidth; j++) {
                rayBuffer[i * screenWidth + j] = sphericalToCartesian(row).normalized();

                // move one column to the right
                row = {row.x(), row.y(), row.z() - stepSize};
            }
            // move one row down
            vec = {vec.x(), vec.y() + stepSize, vec.z()};
        }

        rayBufferIsValid = true;
    }

public:
    Camera(Vec3f pos, const Vec3f &direction, float fov, int screenWidth, int screenHeight)
            : pos(std::move(pos)), direction(cartesianToSpherical(direction)),
              fov(fov * (M_PIf / 180.0f)),
              screenWidth(screenWidth),
              screenHeight(screenHeight) {
        rayBufferIsValid = false;
        rayBuffer.resize(screenWidth * screenHeight);
    }


    const Vec3f &getPos() {
        return pos;
    }

    void setPos(const Vec3f &nPos) {
        rayBufferIsValid = false;
        pos = nPos;
    }

    const Vec3f &getDirection() {
        return direction;
    }

    void setDirection(const Vec3f &nDir) {
        rayBufferIsValid = false;
        direction = nDir;
    }

    const std::vector<Vec3f> &getRays() {
        if (rayBufferIsValid) {
            return rayBuffer;
        } else {
            updateRayBuffer();
            return rayBuffer;
        }
    }

};

#endif //CPURAYTRACER_CAMERA_H
