#include "include/core/graphics_engine/Sprite/SpriteRenderer.h"
#include "include/core/graphics_engine/Sprite/Sprite.h"
#include <iostream>

SpriteRenderer::SpriteRenderer() 
    : m_Shader("include/core/graphics_engine/Shaders/SpriteShaders/SpriteVertexShader.glsl", 
               "include/core/graphics_engine/Shaders/SpriteShaders/SpriteFragmentShader.glsl") {
    CreateBuffers();
    CacheUniformLocations();
}

SpriteRenderer::~SpriteRenderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

SpriteRenderer::SpriteRenderer(SpriteRenderer&& other) noexcept
    : VAO(other.VAO), VBO(other.VBO), EBO(other.EBO),
      m_Shader(std::move(other.m_Shader)),
      m_Uniforms(other.m_Uniforms) {
    other.VAO = other.VBO = other.EBO = 0;
}

SpriteRenderer& SpriteRenderer::operator=(SpriteRenderer&& other) noexcept {
    if (this != &other) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        
        VAO = other.VAO;
        VBO = other.VBO;
        EBO = other.EBO;
        m_Shader = std::move(other.m_Shader);
        m_Uniforms = other.m_Uniforms;
        
        other.VAO = other.VBO = other.EBO = 0;
    }
    return *this;
}

void SpriteRenderer::CacheUniformLocations() {
    m_Shader.Bind();
    m_Uniforms.projection = glGetUniformLocation(m_Shader.GetID(), "projection");
    m_Uniforms.view = glGetUniformLocation(m_Shader.GetID(), "view");
    m_Uniforms.model = glGetUniformLocation(m_Shader.GetID(), "model");
    m_Uniforms.texture = glGetUniformLocation(m_Shader.GetID(), "texture1");
    m_Uniforms.color = glGetUniformLocation(m_Shader.GetID(), "spriteColor");
}

void SpriteRenderer::CreateBuffers() {
    float vertices[] = {
         0.5f,  0.5f,   1.0f, 0.0f, 
         0.5f, -0.5f,   1.0f, 1.0f,  
        -0.5f, -0.5f,   0.0f, 1.0f, 
        -0.5f,  0.5f,   0.0f, 0.0f 
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

void SpriteRenderer::Update() {
    if (!manager) return;
    
    Camera2D* camera = manager->GetCamera();
    if (!camera) return;
    
    m_Shader.Bind();
    
    if (m_Uniforms.projection != -1) {
        glUniformMatrix4fv(m_Uniforms.projection, 1, GL_FALSE, 
                          glm::value_ptr(camera->GetProjectionMatrix()));
    }
    
    if (m_Uniforms.view != -1) {
        glUniformMatrix4fv(m_Uniforms.view, 1, GL_FALSE, 
                          glm::value_ptr(camera->GetViewMatrix()));
    }
    
    glActiveTexture(GL_TEXTURE0);
    if (m_Uniforms.texture != -1) {
        glUniform1i(m_Uniforms.texture, 0);
    }
    
    for (auto* component : rendererComponents) {
        Sprite* sprite = dynamic_cast<Sprite*>(component);
        if (!sprite || !sprite->IsActive()) continue;
        
        GameObject* gameObject = sprite->GetGameObject();
        if (!gameObject) continue;

        if(gameObject->IsDestroyed()) continue;
        
        Transform2D* transform = gameObject->GetComponentOfType<Transform2D>();
        if (!transform) continue;
        
        Texture* texture = sprite->GetTexture();
        if (!texture || !texture->IsLoaded()) continue;
        
        GLuint textureID = texture->GetOpenGLTextureID();
        if (textureID == 0) continue;
        
        RenderSprite(
            textureID,
            transform->position.x, 
            transform->position.y,
            sprite->GetWidth(),
            sprite->GetHeight(),
            transform->rotation,
            sprite->GetColor().r,
            sprite->GetColor().g,
            sprite->GetColor().b,
            sprite->GetColor().a
        );
    }
}

void SpriteRenderer::RenderSprite(GLuint textureID, float x, float y,
                                  float width, float height, float rotation,
                                  float r, float g, float b, float a) {
    if (m_Uniforms.model == -1 || m_Uniforms.texture == -1) return;
    
    if (m_Uniforms.color != -1) {
        glUniform4f(m_Uniforms.color, r, g, b, a);
    }
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(m_Uniforms.texture, 0);

    float normalizedWidth = width / 100.0f;
    float normalizedHeight = height / 100.0f;
    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, y, 0.0f));
    model = glm::rotate(model, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(normalizedWidth, normalizedHeight, 1.0f));
    glUniformMatrix4fv(m_Uniforms.model, 1, GL_FALSE, glm::value_ptr(model));
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}