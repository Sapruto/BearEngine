#pragma once

#include <vector>
#include <glad/glad.h>
#include "UVManager.h"
#include "Shader.h"

class Camera3D;
class Window;
class Light3D;
class ModelComponent;

class RenderPipeline {
private:
    Camera3D* camera = nullptr;
    Window* window = nullptr;
    std::vector<Light3D*> lights;
    UVManager* uvManager = nullptr;

    GLuint framebuffers[2];
    GLuint colorTextures[2];
    GLuint depthBuffers[2];
    int currentRead = 0;
    int currentWrite = 1;

    GLuint quadVAO = 0;
    GLuint quadVBO = 0;
    Shader presentShader;

    int width = 0, height = 0;

    void CreateFramebuffers(int w, int h);
    void DestroyFramebuffers();
    void InitQuad();
    void RenderQuad();

    void SwapBuffers();

public:
    RenderPipeline(Camera3D* cam, Window* win);
    ~RenderPipeline();

    void SetCamera(Camera3D* cam) { camera = cam; }
    void SetWindow(Window* win) { window = win; }
    void SetUVManager(UVManager* manager) { uvManager = manager; }

    void AddLight(Light3D* light) { lights.push_back(light); }
    void RemoveLight(Light3D* light);

    void BeginCurrentRender();
    void EndCurrentRender();
    void Present();

    GLuint GetReadTexture() const { return colorTextures[currentRead]; }
    GLuint GetWriteTexture() const { return colorTextures[currentWrite]; }
    GLuint GetReadDepth() const { return depthBuffers[currentRead]; }

    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    Camera3D* GetCamera() const { return camera; }
    Window* GetWindow() const { return window; }
    const std::vector<Light3D*>& GetLights() const { return lights; }

    ObjectUVData GetObjectUV(ModelComponent* model);
    void Resize(int w, int h);
};