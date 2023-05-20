//
// Created by kis on 11.08.22.
//

#ifndef CPURAYTRACER_MODEL_H
#define CPURAYTRACER_MODEL_H

#include <memory>
#include <utility>
#include <vector>

#include "LinearAlgebra.h"
#include "Vertex.h"
#include "Triangle.h"
#include "Texture.h"
#include "Box.h"

struct Model {
    //bounding Box
    Box boundingBox;

    std::vector<Vertex> vertexBuffer;
    std::vector<Triangle> triangleBuffer;
    Texture texture;

    Model(std::vector<Vertex> _vertexBuffer, std::vector<Triangle> _triangleBuffer, Texture _texture)
            : vertexBuffer(std::move(_vertexBuffer)), triangleBuffer(std::move(_triangleBuffer)),
              texture(std::move(_texture)) {
        boundingBox.min = Vec3f(vertexBuffer[0].pos.x(), vertexBuffer[0].pos.y(), vertexBuffer[0].pos.z()),
                boundingBox.max = Vec3f(vertexBuffer[0].pos.x(), vertexBuffer[0].pos.y(), vertexBuffer[0].pos.z());

        for (const auto &v: vertexBuffer) {
            boundingBox.min.x() = std::min(boundingBox.min.x(), v.pos.x());
            boundingBox.min.y() = std::min(boundingBox.min.y(), v.pos.y());
            boundingBox.min.z() = std::min(boundingBox.min.z(), v.pos.z());
            boundingBox.max.x() = std::max(boundingBox.max.x(), v.pos.x());
            boundingBox.max.y() = std::max(boundingBox.max.y(), v.pos.y());
            boundingBox.max.z() = std::max(boundingBox.max.z(), v.pos.z());
        }
    }
};

class ModelInstance {
private:
    Vec3f position, rotation; //rotation are Euler angles in radians with x = alpha, y = beta, z = gamma
    Vec3f scale;

    std::vector<Vertex> vertexBuffer;

public:
    const std::shared_ptr<Model> model;

private:
    void updateVertexBuffer() {
        Mat3f m = calcRotationMatrix(rotation) * calcScaleMatrix(scale);

        for (int i = 0; i < vertexBuffer.size(); i++) {
            vertexBuffer[i].pos = position + m * model->vertexBuffer[i].pos;
        }
    }

public:
    explicit ModelInstance(std::shared_ptr<Model> _model) : model(std::move(_model)) {
        position = Vec3f{0, 0, 0};
        rotation = Vec3f{0, 0, 0};
        scale = Vec3f{1, 1, 1};
        vertexBuffer = model->vertexBuffer;
    }

    [[nodiscard]] const Vec3f &getPosition() const {
        return position;
    }

    [[nodiscard]] const Vec3f &getRotation() const {
        return rotation;
    }

    [[nodiscard]] Vec3f getScale() const {
        return scale;
    }

    [[nodiscard]] const std::vector<Vertex> &getVertexBuffer() const {
        return vertexBuffer;
    }

    [[nodiscard]] const std::vector<Triangle> &getTriangleBuffer() const {
        return model->triangleBuffer;
    }

    [[nodiscard]] const Texture &getTexture() const {
        return model->texture;
    }

    void setPosition(const Vec3f &_position) {
        Vec3f deltaPos = position - _position;
        ModelInstance::position = _position;

        for (auto &v: vertexBuffer) {
            v.pos += deltaPos;
        }
    }

    void setRotation(const Vec3f &_rotation) {
        ModelInstance::rotation = _rotation;
        updateVertexBuffer();
    }

    void setScale(const Vec3f &_scale) {
        ModelInstance::scale = _scale;
        updateVertexBuffer();
    }

    void setScale(float _scale) {
        ModelInstance::scale = _scale * Vec3f{1, 1, 1};
        updateVertexBuffer();
    }
};

#endif //CPURAYTRACER_MODEL_H
