#pragma once

#ifndef LEVEL_GEOMETRY_H
#define LEVEL_GEOMETRY_H

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "shader.h"
#include "Texture.h"

// Vertex structure
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec2 LightMapTexCoords;
};

// LevelGeometry class
class LevelGeometry {
public:
    LevelGeometry();
    LevelGeometry(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
    void Draw(Shader& shader); // Ensure Shader class is included or declared
    void addTexture(const Texture& texture);

private:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures; // Store textures
    GLuint VAO, VBO, EBO;

    void setupMesh();
};

#endif // LEVEL_GEOMETRY_H