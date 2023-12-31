#include "LevelGeometry.h"
#include "shader.h" // Include your Shader class header

LevelGeometry::LevelGeometry() {
    // Initialize with empty data or default values
}

LevelGeometry::LevelGeometry(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
    : vertices(vertices), indices(indices) {
    // Setup mesh (VAO, VBO, EBO)
    setupMesh();
}

void LevelGeometry::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // Vertex normals (if needed in shader)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    // Vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    // Assuming lightmap UVs are the fourth attribute in the Vertex structure
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, LightMapTexCoords));

    glBindVertexArray(0);
}

void LevelGeometry::Draw(Shader& shader) {
    shader.use();

    // Assuming the first texture is always the diffuse texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0].id); // Bind diffuse texture
    shader.setInt("diffuseTexture", 0); // Set uniform for diffuse texture

    // Assuming the second texture is always the lightmap
    if (textures.size() > 1) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textures[1].id); // Bind lightmap texture
        shader.setInt("lightMapTexture", 1); // Set uniform for lightmap texture
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void LevelGeometry::addTexture(const Texture& texture) {
    textures.push_back(texture);
}