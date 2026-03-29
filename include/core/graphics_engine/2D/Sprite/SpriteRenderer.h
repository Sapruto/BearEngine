#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Renderer.h"
#include "Camera2D.h"
#include "Transform2D.h"
#include "GraphicsManager.h"
#include "GameObject.h"

class Sprite;

class SpriteRenderer : public Renderer {
private:
    GLuint VAO, VBO, EBO;
    Shader shader;

    struct UniformLocations {
        int projection;
        int view;
        int model;
        int texture;
        int color;
    } uniforms;
    
    void CreateShader();
    void CreateBuffers();
    void CacheUniformLocations();

public:
    SpriteRenderer();
    ~SpriteRenderer();
    
    SpriteRenderer(const SpriteRenderer&) = delete;
    SpriteRenderer& operator=(const SpriteRenderer&) = delete;
    
    SpriteRenderer(SpriteRenderer&& other) noexcept;
    SpriteRenderer& operator=(SpriteRenderer&& other) noexcept;

    void Update() override;
    
    void SetProjection(const glm::mat4& projection);
    void RenderSprite(GLuint textureID, float x, float y, float width, float height, 
                     float rotation = 0.0f, 
                     float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);

    void UnregisterRenderComponent(RenderComponent* component); 
};
