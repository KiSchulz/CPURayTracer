//
// Created by kis on 08.08.22.
//

#ifndef CPURAYTRACER_LINEARALGEBRA_H
#define CPURAYTRACER_LINEARALGEBRA_H

#undef Success

#include <Eigen/Dense>
#include <cmath>
#include <cassert>
#include <concepts>

using Vec3f = Eigen::Vector3f;
using Vec2f = Eigen::Vector2f;
using Mat3f = Eigen::Matrix3f;

Mat3f calcRotationMatrix(const Vec3f &rotation) {
    float a = rotation.x(), b = rotation.y(), c = rotation.z();
    Mat3f ret;

    ret(0, 0) = std::cos(b) * std::cos(c);
    ret(0, 1) = std::sin(a) * std::sin(b) * std::cos(c) - std::cos(a) * std::sin(c);
    ret(0, 2) = std::cos(a) * std::sin(b) * std::cos(c) + std::sin(a) * std::sin(c);

    ret(1, 0) = std::cos(b) * std::sin(c);
    ret(1, 1) = std::sin(a) * std::sin(b) * std::sin(c) + std::cos(a) * std::cos(c);
    ret(1, 2) = std::cos(a) * std::sin(b) * std::sin(c) - std::sin(a) * std::cos(c);

    ret(2, 0) = -std::sin(b);
    ret(2, 1) = std::sin(a) * std::cos(b);
    ret(2, 2) = std::cos(a) * std::cos(b);

    return ret;
}

Mat3f calcScaleMatrix(const Vec3f &scale) {
    return Mat3f{
            {scale.x(), 0,         0},
            {0,         scale.y(), 0},
            {0,         0,         scale.z()}
    };
}

bool rayTriangleIntersect(const Vec3f &orig,
                          const Vec3f &dir,
                          const Vec3f &v0, const Vec3f &v1, const Vec3f &v2,
                          float &t, float &u, float &v) {
    //Moeller-Trumbore algorithm
    static constexpr float kEpsilon = 0.0000001f;

    Vec3f v0v1 = v1 - v0;
    Vec3f v0v2 = v2 - v0;
    Vec3f pVec = dir.cross(v0v2);
    float det = v0v1.dot(pVec);

    if (det < kEpsilon) return false;

    float invDet = 1 / det;

    Vec3f tVec = orig - v0;
    u = tVec.dot(pVec) * invDet;
    if (u < 0 || u > 1) return false;

    Vec3f qVec = tVec.cross(v0v1);
    v = dir.dot(qVec) * invDet;
    if (v < 0 || u + v > 1) return false;

    t = v0v2.dot(qVec) * invDet;

    return true;
}

bool raySphereIntersect(const Vec3f &rayOrig, const Vec3f &rayDir, const Vec3f &sphereOrig, float radius) {
    Vec3f dist = rayOrig - sphereOrig;
    float dNorm = dist.norm();

    if (dNorm <= radius) return true;

    float d = dist.dot(rayDir);

    return d * d >= dNorm * dNorm - radius * radius;
}

enum class IntervalType {
    open, close, leftOpen, rightOpen
};

template<std::totally_ordered T = float, IntervalType rangeType = IntervalType::rightOpen>
bool isInInterval(T min, T max, T value) {
    if constexpr (rangeType == IntervalType::open) {
        return min < value && value < max;
    } else if constexpr (rangeType == IntervalType::close) {
        return min <= value && value <= max;
    } else if constexpr (rangeType == IntervalType::leftOpen) {
        return min < value && value <= max;
    } else if constexpr (rangeType == IntervalType::rightOpen) {
        return min <= value && value < max;
    }
}

#endif //CPURAYTRACER_LINEARALGEBRA_H
