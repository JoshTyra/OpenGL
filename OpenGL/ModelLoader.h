#pragma once

#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "LevelGeometry.h" // Your custom geometry class
#include "TextureLoader.h"

class ModelLoader {
public:
    static LevelGeometry loadModel(const std::string& path);

private:
    static LevelGeometry processMesh(aiMesh* mesh, const aiScene* scene, std::vector<Texture>& loadedTextures);
    static std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName, std::vector<Texture>& loadedTextures);
};

#endif // MODEL_LOADER_H