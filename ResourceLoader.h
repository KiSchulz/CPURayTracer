//
// Created by kis on 12.08.22.
//

#ifndef CPURAYTRACER_RESOURCELOADER_H
#define CPURAYTRACER_RESOURCELOADER_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// make sure it was not already included by olcPixelGameEngine file
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#endif //STB_IMAGE_IMPLEMENTATION

#include <utility>

#include "Model.h"
#include "Vertex.h"

class ResourceLoader {
private:
    std::string basePath;

public:
    //base path has to end with a '/' and all other paths have to start without one
    ResourceLoader(std::string basePath) : basePath(std::move(basePath)) {}

    Model LoadModel(const std::string &relPathMesh, const std::string &relPathTexture) {
        //Load the texture
        int width, height, channels, desiredChannels = 3;

        stbi_set_flip_vertically_on_load(true);
        uint8_t *img = stbi_load((basePath + relPathTexture).c_str(), &width, &height, &channels, desiredChannels);
        stbi_set_flip_vertically_on_load(false);
        if (img == nullptr) {
            throw std::runtime_error("Failed to load Model: Could not load texture!");
        }

        Texture texture(width, height);
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                texture.SetPixel(j, i,
                                 Pixel{img[(i * width + j) * desiredChannels],
                                       img[(i * width + j) * desiredChannels + 1],
                                       img[(i * width + j) * desiredChannels + 2]});
            }
        }

        stbi_image_free(img);

        //load the mesh and uv-coordinates
        Assimp::Importer importer;
        const aiScene *scene;
        std::vector<Vertex> vertexBuffer;
        std::vector<Triangle> triangleBuffer;

        std::string meshPath = basePath + relPathMesh;
        scene = importer.ReadFile(meshPath, aiProcess_CalcTangentSpace |
                                            aiProcess_Triangulate |
                                            aiProcess_JoinIdenticalVertices |
                                            aiProcess_SortByPType);

        if (scene == nullptr) {
            throw std::runtime_error(
                    "Failed to load Model: Could not load file from " + meshPath + "\n" + importer.GetErrorString());
        }

        if (!scene->HasMeshes()) {
            throw std::runtime_error("Fail to load Model: File does not contain a mesh!");
        }

        if (scene->mNumMeshes != 1) {
            throw std::runtime_error("Failed to load Model: Expected only one mesh!");
        }

        if (scene->mMeshes[0]->mTextureCoords[0] == nullptr) {
            throw std::runtime_error("Failed to load Model: Model has no uv-coordinates!");
        }

        const aiVector3D *vertBuff = scene->mMeshes[0]->mVertices;
        const aiVector3D *texBuff = scene->mMeshes[0]->mTextureCoords[0];
        const unsigned int vertBuffSize = scene->mMeshes[0]->mNumVertices;
        for (unsigned int i = 0; i < vertBuffSize; i++) {
            vertexBuffer.emplace_back(Vertex{{vertBuff[i].x,                vertBuff[i].y, vertBuff[i].z},
                                             {texBuff[i].x * (float) width, texBuff[i].y * (float) height}});

        }

        const aiFace *faceBuff = scene->mMeshes[0]->mFaces;
        const unsigned int faceBuffSize = scene->mMeshes[0]->mNumFaces;
        for (unsigned int i = 0; i < faceBuffSize; i++) {
            triangleBuffer.emplace_back(Triangle{faceBuff[i].mIndices[0],
                                                 faceBuff[i].mIndices[1],
                                                 faceBuff[i].mIndices[2]});
        }

        return Model{vertexBuffer, triangleBuffer, texture};
    }

};

#endif //CPURAYTRACER_RESOURCELOADER_H
