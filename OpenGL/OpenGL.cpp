#include "shader.h"
#include "Skybox.h"
#include "ModelLoader.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Global variables
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float cameraSpeed = 5.0f;

float yaw = -90.0f;   // Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right
float pitch = 0.0f;
float lastX = 2560.0f / 2.0;  // Set to window width divided by 2
float lastY = 1080.0f / 2.0;   // Set to window height divided by 2
float sensitivity = 0.1f;
bool firstMouse = true;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void processInput(GLFWwindow* window) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    float speed = cameraSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        cameraPos += speed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        cameraPos -= speed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;

    //std::cout << "Camera position: "
    //    << cameraPos.x << ", "
    //    << cameraPos.y << ", "
    //    << cameraPos.z << std::endl;
}

int main() {

    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    // Get the primary monitor
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (!monitor) {
        std::cerr << "Failed to get the primary monitor\n";
        glfwTerminate();
        return -1;
    }

    // Get the video mode of the monitor
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    if (!mode) {
        std::cerr << "Failed to get the video mode of the monitor\n";
        glfwTerminate();
        return -1;
    }

    // Create a fullscreen window using the screen resolution
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "OpenGL Skybox", monitor, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW\n";
        return -1;
    }

    // Set the key callback
    glfwSetKeyCallback(window, key_callback);

    // Configure global OpenGL state
    glEnable(GL_DEPTH_TEST);

    // Projection matrix
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)mode->width / (float)mode->height, 0.1f, 100.0f);
    // View matrix
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    // Model matrix
    glm::mat4 model = glm::mat4(1.0f); // Initialize to identity matrix

    // Build and compile our shader program
    Shader SkyboxShader("shaders/skybox.vert", "shaders/skybox.frag");

    // Build and compile our shader program
    Shader PlaneShader("shaders/simple_diffuse.vert", "shaders/simple_diffuse.frag");

    // Check for shader compilation/linking errors
    if (!SkyboxShader.isSuccessfullyCompiled()) {
        std::cerr << "Failed to compile/link skybox shader" << std::endl;
        return -1;
    }

    // Check for shader compilation/linking errors
    if (!PlaneShader.isSuccessfullyCompiled()) {
        std::cerr << "Failed to compile/link simple_diffuse shader" << std::endl;
        return -1;
    }

    GLuint skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, skyboxVerticesSize, skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    // Load cubemap texture for the skybox
    std::vector<std::string> faces{
        "media/skybox/clouds1_east.bmp",   // Right
        "media/skybox/clouds1_west.bmp",   // Left
        "media/skybox/clouds1_up.bmp",     // Top
        "media/skybox/clouds1_down.bmp",   // Bottom
        "media/skybox/clouds1_north.bmp",  // Front
        "media/skybox/clouds1_south.bmp"   // Back
    };

    GLuint cubemapTexture = loadCubemap(faces);

    LevelGeometry plane = ModelLoader::loadModel("media/models/plane.fbx");

    // Define vertices for a simple box
    GLfloat boxVertices[] = {
        // positions          
        -0.5f, -0.5f, -0.5f,  // Bottom-left
         0.5f, -0.5f, -0.5f,  // Bottom-right
         0.5f,  0.5f, -0.5f,  // Top-right
         0.5f,  0.5f, -0.5f,  // Top-right
        -0.5f,  0.5f, -0.5f,  // Top-left
        -0.5f, -0.5f, -0.5f,  // Bottom-left

        -0.5f, -0.5f,  0.5f,  // Bottom-left
         0.5f, -0.5f,  0.5f,  // Bottom-right
         0.5f,  0.5f,  0.5f,  // Top-right
         0.5f,  0.5f,  0.5f,  // Top-right
        -0.5f,  0.5f,  0.5f,  // Top-left
        -0.5f, -0.5f,  0.5f,  // Bottom-left

        -0.5f,  0.5f,  0.5f,  // Top-left
        -0.5f,  0.5f, -0.5f,  // Top-left
        -0.5f, -0.5f, -0.5f,  // Bottom-left
        -0.5f, -0.5f, -0.5f,  // Bottom-left
        -0.5f, -0.5f,  0.5f,  // Bottom-left
        -0.5f,  0.5f,  0.5f,  // Top-left

         0.5f,  0.5f,  0.5f,  // Top-right
         0.5f,  0.5f, -0.5f,  // Top-right
         0.5f, -0.5f, -0.5f,  // Bottom-right
         0.5f, -0.5f, -0.5f,  // Bottom-right
         0.5f, -0.5f,  0.5f,  // Bottom-right
         0.5f,  0.5f,  0.5f,  // Top-right

        -0.5f, -0.5f, -0.5f,  // Bottom-left
         0.5f, -0.5f, -0.5f,  // Bottom-right
         0.5f, -0.5f,  0.5f,  // Bottom-right
         0.5f, -0.5f,  0.5f,  // Bottom-right
        -0.5f, -0.5f,  0.5f,  // Bottom-left
        -0.5f, -0.5f, -0.5f,  // Bottom-left

        -0.5f,  0.5f, -0.5f,  // Top-left
         0.5f,  0.5f, -0.5f,  // Top-right
         0.5f,  0.5f,  0.5f,  // Top-right
         0.5f,  0.5f,  0.5f,  // Top-right
        -0.5f,  0.5f,  0.5f,  // Top-left
        -0.5f,  0.5f, -0.5f   // Top-left
    };

    // Set up VAO and VBO for the box
    GLuint boxVAO, boxVBO;
    glGenVertexArrays(1, &boxVAO);
    glGenBuffers(1, &boxVBO);
    glBindVertexArray(boxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(boxVertices), boxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Texture Coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0); // Unbind VAO

    glDisable(GL_CULL_FACE);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float scale = 1.0f; // Adjust this value as needed
        model = glm::scale(model, glm::vec3(scale, scale, scale)); // Scale the model

        processInput(window);

        // Update the view matrix based on the camera's current position and orientation
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        // Render the plane
        PlaneShader.use();
        PlaneShader.setMat4("view", view);
        PlaneShader.setMat4("projection", projection);
        PlaneShader.setMat4("model", model); // Adjust this matrix as needed
        plane.Draw(PlaneShader);

        // In your render loop, draw the box
        glBindVertexArray(boxVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices for the box
        glBindVertexArray(0);

        // Swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);

    glfwTerminate();
    return 0;
}