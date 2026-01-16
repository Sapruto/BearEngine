#include "include/core/graphics_engine/Render.h"

#include <dependencies/glad/include/glad/glad.h>
#include <dependencies/GLFW/include/GLFW/glfw3.h>
#include <iostream> 
#include <cmath>
#include "include/core/Component.h"
#include "include/core/GameObject.h"
#include "include/core/math/Transform2D.h"

TriangleRenderer::~TriangleRenderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
}
TriangleRenderer::TriangleRenderer(GLFWwindow* targetWindow) : window(targetWindow) {
    if (!window) {
        std::cout << "ERROR: Window is null!" << std::endl;
        return;
    }
    
    glfwMakeContextCurrent(window);
    
    createShaderProgram();
    setupBuffers();
    updateVertexColors();
}
    
unsigned int TriangleRenderer::compileShader(unsigned int type, const std::string& source) {
    unsigned int shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "SHADER COMPILATION FAILED\n" << infoLog << std::endl;
    }
    return shader;
}

void TriangleRenderer::createShaderProgram() {
    unsigned int vertex = compileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fragment = compileShader(GL_FRAGMENT_SHADER, fragmentShader);
    
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertex);
    glAttachShader(shaderProgram, fragment);
    glLinkProgram(shaderProgram);
    
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "SHADER LINKING FAILED\n" << infoLog << std::endl;
    }
    
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void TriangleRenderer::setupBuffers() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Transform2D* TriangleRenderer::GetTransformComponent() {
    if (!gameObject) return nullptr;
    return gameObject->GetComponentOfType<Transform2D>();
}

void TriangleRenderer::ApplyTransformToShader() {
    Transform2D* transform = GetTransformComponent();
    if (!transform) return;
    
    float modelMatrix[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    
    modelMatrix[0] *= transform->scale.x; 
    modelMatrix[5] *= transform->scale.y;
    
    float cosA = cosf(transform->rotation);
    float sinA = sinf(transform->rotation);
    
    float rotationMatrix[4] = {
        cosA, -sinA,
        sinA,  cosA
    };
    
    float m00 = modelMatrix[0];
    float m01 = modelMatrix[1];
    float m10 = modelMatrix[4];
    float m11 = modelMatrix[5];
    
    modelMatrix[0] = m00 * rotationMatrix[0] + m01 * rotationMatrix[2];
    modelMatrix[1] = m00 * rotationMatrix[1] + m01 * rotationMatrix[3];
    modelMatrix[4] = m10 * rotationMatrix[0] + m11 * rotationMatrix[2];
    modelMatrix[5] = m10 * rotationMatrix[1] + m11 * rotationMatrix[3];
    
    modelMatrix[12] = transform->position.x;
    modelMatrix[13] = transform->position.y;
    
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    float aspect = (float)width / (float)height;
    
    float projectionMatrix[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    
    if (aspect > 1.0f) {
        projectionMatrix[0] = 1.0f / aspect;
    } else {
        projectionMatrix[5] = aspect;
    }
    
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, projectionMatrix);
}

void TriangleRenderer::updateVertexColors() {
    for (int i = 0; i < 3; i++) {
        vertices[i * 6 + 3] = colorR;
        vertices[i * 6 + 4] = colorG;
        vertices[i * 6 + 5] = colorB;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void TriangleRenderer::Start() {
    Component::Start();
    std::cout << "TriangleRenderer started" << std::endl;
}

void TriangleRenderer::Update(float deltaTime) {
    Component::Update(deltaTime);
    if (!window) return;
    
    glfwMakeContextCurrent(window);
    glUseProgram(shaderProgram);
    
    ApplyTransformToShader();
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

void TriangleRenderer::Destroy() {
    Component::Destroy();
    std::cout << "TriangleRenderer destroyed" << std::endl;
}

void TriangleRenderer::SetColor(float r, float g, float b) {
    colorR = r;
    colorG = g;
    colorB = b;
    updateVertexColors();
}