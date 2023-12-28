#pragma once
#include <GL/glew.h>

// Cube.h
#ifndef CUBE_H
#define CUBE_H

// Define vertices for the cube
GLfloat vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f
};

// Define colors for the vertices
GLfloat colors[] = {
    1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,  1.0f, 1.0f, 0.0f,
    1.0f, 0.0f, 1.0f,  0.0f, 1.0f, 1.0f
};

// Define indices for drawing the cube
GLuint indices[] = {
    0, 1, 2, 2, 3, 0,  1, 5, 6, 6, 2, 1,
    7, 6, 5, 5, 4, 7,  4, 0, 3, 3, 7, 4,
    4, 5, 1, 1, 0, 4,  3, 2, 6, 6, 7, 3
};

#endif
