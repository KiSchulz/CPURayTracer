//
// Created by kis on 22.08.22.
//

#ifndef CPURAYTRACER_OCTREE_H
#define CPURAYTRACER_OCTREE_H

#include "LinearAlgebra.h"
#include "Model.h"
#include "RayHit.h"

#include <utility>

class Octree {
    struct Node {
        const Vec3f pos;
        const Box box;

        std::array<std::unique_ptr<Node>, 8> children;
        std::vector<std::size_t> triangleBuffer;

        explicit Node(Vec3f pos, Box box) : pos(std::move(pos)), box(std::move(box)) {}

        [[nodiscard]] int getOctant(const Vec3f &_pos) const {
            int idx = 0;

            if (isInInterval(box.min.x(), box.min.x() + (box.max.x() - box.min.x()) / 2, _pos.x())) {
                idx += 4;
            }

            if (isInInterval(box.min.y(), box.min.y() + (box.max.y() - box.min.y()) / 2, _pos.y())) {
                idx += 2;
            }

            if (isInInterval(box.min.z(), box.min.z() + (box.max.z() - box.min.z()) / 2, _pos.z())) {
                idx += 1;
            }

            return idx;
        }

        Box calcSubBoxByIdx(int octIdx) {
            Box subBox;
            if (octIdx >= 4) {
                octIdx -= 4;

                subBox.min.x() = box.min.x();
                subBox.max.x() = box.min.x() + (box.max.x() - box.min.x()) / 2;
            } else {
                subBox.min.x() = box.min.x() + (box.max.x() - box.min.x()) / 2;
                subBox.max.x() = box.max.x();
            }

            if (octIdx >= 2) {
                octIdx -= 2;

                subBox.min.y() = box.min.y();
                subBox.max.y() = box.min.y() + (box.max.y() - box.min.y()) / 2;
            } else {
                subBox.min.y() = box.min.y() + (box.max.y() - box.min.y()) / 2;
                subBox.max.y() = box.max.y();
            }

            if (octIdx >= 1) {
                octIdx -= 1;

                subBox.min.z() = box.min.z();
                subBox.max.z() = box.min.z() + (box.max.z() - box.min.z()) / 2;
            } else {
                subBox.min.z() = box.min.z() + (box.max.z() - box.min.z()) / 2;
                subBox.max.z() = box.max.z();
            }

            assert(octIdx == 0);

            return subBox;
        }

        bool insert(const Vec3f &elPos, std::size_t triIdx) {
            if (elPos == pos) {
                triangleBuffer.push_back(triIdx);
                return true;
            }

            int idx = getOctant(elPos);

            if (!children[idx]) {
                children[idx] = std::make_unique<Node>(elPos, calcSubBoxByIdx(idx));
            }

            return children[idx]->insert(elPos, triIdx);
        }
    };

    const ModelInstance &modelInstance;
    std::unique_ptr<Node> root;


    void
    checkForHit(const std::unique_ptr<Node> &node, const Vec3f &rayOrig, const Vec3f &rayDir, RayHit &rayHit) const {
        for (const auto &child: node->children) {
            if (!child) continue;

            float t;
            bool didHit = child->box.rayBoxIntersect(rayOrig, rayDir, t);

            if (didHit && isInInterval(0.0f, rayHit.t, t)) {
                checkForHit(child, rayOrig, rayDir, rayHit);
            }
        }

        const auto &vertBuf = modelInstance.getVertexBuffer();
        const auto &triBuf = modelInstance.getTriangleBuffer();
        for (const auto &i: node->triangleBuffer) {
            float t, u, v;
            bool ditHit = rayTriangleIntersect(rayOrig, rayDir,
                                               vertBuf[triBuf[i].v0].pos,
                                               vertBuf[triBuf[i].v1].pos,
                                               vertBuf[triBuf[i].v2].pos,
                                               t, u, v);

            if (ditHit && isInInterval(0.0f, rayHit.t, t)) {
                rayHit.t = t;
                rayHit.u = u;
                rayHit.v = v;
                rayHit.triangle = &triBuf[i];
                rayHit.modelInstance = &modelInstance;
            }
        }
    }

public:
    explicit Octree(const ModelInstance &_modelInstance) : modelInstance(_modelInstance) {
        const auto &vertBuf = modelInstance.getVertexBuffer();
        const auto &triBuf = modelInstance.getTriangleBuffer();

        root = std::make_unique<Node>(vertBuf[triBuf[0].v0].pos, modelInstance.model->boundingBox);
        for (std::size_t i = 0; i < triBuf.size(); i++) {
            root->insert(vertBuf[triBuf[i].v0].pos, i);
            root->insert(vertBuf[triBuf[i].v1].pos, i);
            root->insert(vertBuf[triBuf[i].v2].pos, i);
        }
    }

    void checkForHit(const Vec3f &rayOrig, const Vec3f &rayDir, RayHit &rayHit) const {
        float t;
        if (root->box.rayBoxIntersect(rayOrig, rayDir, t) && isInInterval(0.0f, rayHit.t, t)) {
            checkForHit(root, rayOrig, rayDir, rayHit);
        }
    }
};

#endif //CPURAYTRACER_OCTREE_H
