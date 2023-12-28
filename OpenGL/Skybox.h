#pragma once

#ifndef SKYBOX_H
#define SKYBOX_H

#include <GL/glew.h>
#include <vector>
#include <string>
#include "shader.h" // Include your Shader class header
#include <glm/glm.hpp> // Include GLM if you're using it in the header

extern const int skyboxVerticesSize;
extern GLfloat skyboxVertices[];

// Function declarations
GLuint loadCubemap(std::vector<std::string> faces);
void drawSkybox(GLuint skyboxVAO, GLuint skyboxTexture, Shader& skyboxShader, const glm::mat4& view, const glm::mat4& projection);

#endif // SKYBOX_H