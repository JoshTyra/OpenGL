#include "ModelLoader.h"

std::string extractFilename(const std::string& path) {
    // Find the last position of '/' or '\'
    size_t lastSlashPos = path.find_last_of("/\\");
    if (lastSlashPos == std::string::npos) {
        // No slashes found, return the original path
        return path;
    }
    // Extract the filename
    return path.substr(lastSlashPos + 1);
}

LevelGeometry ModelLoader::loadModel(const std::string& path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        throw std::runtime_error("Failed to load model");
    }

    std::vector<Texture> loadedTextures;
    // Assuming single mesh for simplicity
    aiMesh* mesh = scene->mMeshes[0];

    std::cout << "Mesh count: " << scene->mNumMeshes << std::endl;
    std::cout << "Vertex count: " << mesh->mNumVertices << std::endl;
    std::cout << "Face count: " << mesh->mNumFaces << std::endl;

    return processMesh(mesh, scene, loadedTextures);
}

LevelGeometry ModelLoader::processMesh(aiMesh* mesh, const aiScene* scene, std::vector<Texture>& loadedTextures) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;

        // Process vertex positions
        if (mesh->HasPositions()) {
            vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        }

        // Process vertex normals
        if (mesh->HasNormals()) {
            vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        }

        if (mesh->HasTextureCoords(0)) {
            vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            std::cout << "TexCoords[" << i << "]: " << vertex.TexCoords.x << ", " << vertex.TexCoords.y << std::endl;
        }
        else {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    // Process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    LevelGeometry geometry(vertices, indices);

    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", loadedTextures);
        for (auto& texture : diffuseMaps) {
            geometry.addTexture(texture);
        }
        // Repeat for other texture types if needed
    }

    return geometry;
}

std::vector<Texture> ModelLoader::loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName, std::vector<Texture>& loadedTextures) {
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);

        // Extract the filename from the path
        std::string filename = extractFilename(str.C_Str());

        // Construct the new path
        std::string newPath = "media/textures/" + filename;

        // Check if texture was loaded before and if so, continue to next iteration
        bool skip = false;
        for (auto& loadedTexture : loadedTextures) {
            if (std::strcmp(loadedTexture.path.data(), newPath.c_str()) == 0) {
                textures.push_back(loadedTexture);
                skip = true;
                break;
            }
        }
        if (!skip) {
            Texture texture;
            texture.id = loadTexture(newPath.c_str()); // Use the new path
            std::cout << "Loaded texture: " << newPath << ", ID: " << texture.id << std::endl;

            // Check for OpenGL errors after loading the texture
            GLenum err;
            while ((err = glGetError()) != GL_NO_ERROR) {
                std::cerr << "OpenGL error during texture loading: " << err << std::endl;
            }

            texture.type = typeName;
            texture.path = newPath; // Store the new path
            textures.push_back(texture);
            loadedTextures.push_back(texture);  // Add to loaded textures
        }
    }
    return textures;
}
