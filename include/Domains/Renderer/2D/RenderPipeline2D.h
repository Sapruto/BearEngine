#pragma once

#include <vector>
#include <glad/glad.h>
#include "UVManager2D.h"
#include "Camera2D.h"

class Window;
class Shader;
class SpriteComponent;

class RenderPipeline2D {
private:
    Camera2D* camera = nullptr;
    Window* window = nullptr;

    GLuint framebuffer = 0;
    GLuint colorTexture = 0;
    GLuint depthBuffer = 0;

    GLuint quadVAO = 0;
    GLuint quadVBO = 0;
    Shader* presentShader = nullptr;

    UVManager2D* uvManager = nullptr;

    int width = 0, height = 0;

    void CreateFramebuffer(int w, int h);
    void DestroyFramebuffer();
    void InitQuad();
    void RenderQuad();

public:
    RenderPipeline2D(Camera2D* cam, Window* win);
    ~RenderPipeline2D();

    void SetUVManager(UVManager2D* manager) { uvManager = manager; }
    void SetCamera(Camera2D* cam) { camera = cam; }
    void SetWindow(Window* win) { window = win; }
    void SetPresentShader(Shader* shader) { presentShader = shader; }

    UVManager2D* GetUVManager() { return uvManager; }

    void BeginFrame();
    void EndFrame();
    void Present();

    GLuint GetColorTexture() const { return colorTexture; }
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    Camera2D* GetCamera() const { return camera; }
    Window* GetWindow() const { return window; }

    void Resize(int w, int h);
};