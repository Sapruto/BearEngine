#pragma once

#include <dependencies/glad/include/glad/glad.h>
#include <dependencies/GLFW/include/GLFW/glfw3.h>

#include <string>
#include <iostream>
#include <cmath>
#include <src\dependencies\glm\GLM\glm.hpp>
#include <src\dependencies\glm\GLM\gtc\matrix_transform.hpp>
#include <src\dependencies\glm\GLM\gtc\type_ptr.hpp>

#include "include/core/graphics_engine/Shader.h"
#include "include/core/graphics_engine/BaseGraphic/Renderer.h"
#include "include/core/graphics_engine/Camera.h"
#include "include/core/math/Transform2D.h"
#include "include/core/graphics_engine/GraphicsManager.h"
#include "include/core/GameObject.h"

class Sprite;

class SpriteRenderer : public Renderer {
private:
    GLuint VAO, VBO, EBO;
    Shader m_Shader;

    struct UniformLocations {
        int projection;
        int view;
        int model;
        int texture;
        int color;
    } m_Uniforms;
    
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
};
