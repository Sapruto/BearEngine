#pragma once

#include <vector>
#include <glad/glad.h>
#include "UVManager.h"
#include "Shader.h"

class Camera3D;
class Window;
class Light3D;
class UVManager;
class ModelComponent;

class RenderPipeline {
private:
    Camera3D* camera = nullptr;
    Window* window = nullptr;
    std::vector<Light3D*> lights;
    
    GLuint framebuffer = 0;
    GLuint colorTexture = 0;
    GLuint depthBuffer = 0;
    
    GLuint quadVAO = 0;
    GLuint quadVBO = 0;
    Shader presentShader;

    UVManager* uvManager;
    
    int width = 0, height = 0;
    
    void CreateFramebuffer(int w, int h);
    void DestroyFramebuffer();
    void InitQuad();
    void RenderQuad();
    
public:
    RenderPipeline(Camera3D* cam, Window* win);
    ~RenderPipeline();
    
    void SetCamera(Camera3D* cam) { camera = cam; }
    void SetWindow(Window* win) { window = win; }
    void SetUVManager(UVManager* manager) { uvManager = manager; }
    
    void AddLight(Light3D* light) { lights.push_back(light); }
    void RemoveLight(Light3D* light);
    
    void BeginFrame();
    void EndFrame();
    
    void Present();
    
    GLuint GetColorTexture() const { return colorTexture; }
    GLuint GetDepthTexture() const { return depthBuffer; }
    
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    Camera3D* GetCamera() const { return camera; }
    Window* GetWindow() const { return window; }
    const std::vector<Light3D*>& GetLights() const { return lights; }

    ObjectUVData GetObjectUV(ModelComponent* model);
    
    void Resize(int w, int h);
};