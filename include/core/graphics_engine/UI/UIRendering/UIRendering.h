#pragma once

#include "Renderer.h"
#include "Shader.h"
#include "Canvas.h"
#include "UIVertex.h"

class UIRendering : public Renderer{
private:
    GLuint VAO, VBO, EBO;
    Shader shader;

    struct UniformLocations {
        int projection;
        int texture_atlas;
    } uniforms;

    void CreateBuffers();
    void CacheUniformLocations();

public:
    UIRendering();
    ~UIRendering();
    
    UIRendering(const UIRendering&) = delete;
    UIRendering& operator=(const UIRendering&) = delete;
    
    UIRendering(UIRendering&& other) noexcept;
    UIRendering& operator=(UIRendering&& other) noexcept;

    void Update() override;
    
    void RenderCanvas(Canvas* canvas);
};