#include "UIRendering.h"

#include "Renderer.h"
#include "Shader.h"
#include "Canvas.h"
#include "UIVertex.h"

UIRendering::UIRendering() 
    : shader("include/core/graphics_engine/UI/UIRendering/UIShaders/UIVertexShader.glsl", 
               "include/core/graphics_engine/UI/UIRendering/UIShaders/UIFragmentShader.glsl") {
    CreateBuffers();
    CacheUniformLocations();
}

UIRendering::~UIRendering() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

UIRendering::UIRendering(UIRendering&& other) noexcept
    : VAO(other.VAO), VBO(other.VBO), EBO(other.EBO),
      shader(std::move(other.shader)),
      uniforms(other.uniforms) {
    other.VAO = other.VBO = other.EBO = 0;
}

UIRendering& UIRendering::operator=(UIRendering&& other) noexcept {
    if (this != &other) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        
        VAO = other.VAO;
        VBO = other.VBO;
        EBO = other.EBO;
        shader = std::move(other.shader);
        uniforms = other.uniforms;
        
        other.VAO = other.VBO = other.EBO = 0;
    }
    return *this;
}

void UIRendering::CreateBuffers() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (void*)(4 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
}

void UIRendering::CacheUniformLocations() {
    shader.Bind();
    uniforms.projection = glGetUniformLocation(shader.GetID(), "uProjection");
    uniforms.texture_atlas = glGetUniformLocation(shader.GetID(), "uTextureAtlas");
}

void UIRendering::Update(){
    shader.Bind();
    glBindVertexArray(VAO); 

    for(const auto& component : rendererComponents){
        Canvas* canvas = dynamic_cast<Canvas*>(component);
        if(!canvas) continue;

        canvas->Update();
        RenderCanvas(canvas);
    }
}

void UIRendering::RenderCanvas(Canvas* canvas) {
    float screenWidth = canvas->GetScreenWidth();
    float screenHeight = canvas->GetScreenHeight();

    glm::mat4 projection = glm::ortho(0.0f, screenWidth, screenHeight, 0.0f, -1.0f, 1.0f);
    glUniformMatrix4fv(uniforms.projection, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform1i(uniforms.texture_atlas, 0);
    
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(uniforms.texture_atlas, 0);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    for (const auto& [layer, batches] : canvas->GetGroups()){
        for (const auto& batch : batches) {
            if (batch.vertices.empty() || batch.indices.empty()){
                continue;
            }
            
            if (batch.texture){
                glBindTexture(GL_TEXTURE_2D, batch.texture->GetOpenGLTextureID());
            } 
            else{
                continue;
            }
            
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, 
                        batch.vertices.size() * sizeof(UIVertex), 
                        batch.vertices.data(), 
                        GL_DYNAMIC_DRAW);
            
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                        batch.indices.size() * sizeof(unsigned int),
                        batch.indices.data(), 
                        GL_DYNAMIC_DRAW);
            
            glDrawElements(GL_TRIANGLES, 
                          static_cast<GLsizei>(batch.indices.size()), 
                          GL_UNSIGNED_INT, 
                          nullptr);
        }
    }
}