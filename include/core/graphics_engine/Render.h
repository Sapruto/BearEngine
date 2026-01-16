#pragma once

#include <dependencies/glad/include/glad/glad.h>
#include <dependencies/GLFW/include/GLFW/glfw3.h>
#include <iostream> 
#include <cmath>
#include "include/core/Component.h"
#include "include/core/math/Transform2D.h"

class TriangleRenderer : public Component {
private:
    unsigned int VAO, VBO, shaderProgram;
    GLFWwindow* window;
    
    const std::string vertexShader = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aColor;
        
        uniform mat4 model;
        uniform mat4 projection;
        
        out vec3 ourColor;
        
        void main() {
            vec4 worldPos = model * vec4(aPos, 1.0);
            vec4 screenPos = projection * worldPos;
            
            gl_Position = screenPos;
            ourColor = aColor;
        }
    )";
    
    const std::string fragmentShader = R"(
        #version 330 core
        out vec4 FragColor;
        in vec3 ourColor;
        void main() {
            FragColor = vec4(ourColor, 1.0);
        }
    )";
    
    float vertices[18] = {
        -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f
    };
    
    float colorR = 1.0f, colorG = 0.0f, colorB = 0.0f;
    
    unsigned int compileShader(unsigned int type, const std::string& source);
    
    void createShaderProgram();
    
    void setupBuffers();
    
    Transform2D* GetTransformComponent();
    
    void ApplyTransformToShader();
    
    void updateVertexColors();
        
public:
    TriangleRenderer(GLFWwindow* targetWindow);
    
    ~TriangleRenderer();
    
    void Start() override;
    
    void Update(float deltaTime) override;
    
    void Destroy() override;

    void SetColor(float r, float g, float b);
};