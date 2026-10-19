#pragma once

#include "Renderer/Base/IRendererScene.h"
#include "Renderer/3D/Materials/MaterialType.h"
#include "Renderer/3D/Passes/BasePass3D.h"
#include "Renderer/3D/Settings/RenderContext3D.h"
#include "Renderer/3D/Utilities/UV.h"

#include "RHI/Base/Handles.h"

#include <vector>
#include <memory>

class Transform3D;
class Camera3D;
class Renderable3D;
class Window;
class ILight3D;

class RendererScene3D : public IRendererScene {
private:
    RHI::Handles::RenderTargetHandle renderTarget{};

    std::unordered_map<MaterialType, std::vector<Renderable3D*>> groups;
    std::unordered_map<MaterialType, std::unique_ptr<BasePass3D>> passes;

    bool isDirty = true;
    bool isInitialized = false;

    std::unique_ptr<RenderContext3D> context;

    std::vector<RenderMeshDesc> GetDescsByGroup(MaterialType type);

    std::vector<ILight3D*> lights;
    Camera3D* currentCamera;

    float clearColor[4] = { 0.05f, 0.05f, 0.1f, 1.0f };

    void InitDefault();
    void CreateRenderTarget();
    void BuildGroups();

    void GenerateNewContext();

    void BeginRender();
    void Present();

public:
    RendererScene3D() {
        context = std::make_unique<RenderContext3D>();
    }
    ~RendererScene3D() = default;

    void Start() override;
    void Update() override;
    void Destroy() override {
        for(auto& [type, pass] : passes){
            if(pass) pass->Shutdown();
        }
    }

    template<typename T>
    T* RegisterRenderer(std::unique_ptr<T> pass) {
        static_assert(std::is_base_of_v<BasePass3D, T>, "T must derive from BasePass3D");
        if (!pass) return nullptr;

        auto type = pass->GetType();
        auto it = passes.find(type);
        if (it != passes.end()) {
            it->second->Shutdown();
        }

        T* raw = pass.get();
        passes[type] = std::move(pass);
        if (isInitialized) raw->Init();
        isDirty = true;
        return raw;
    }

    template<typename T>
    void UnregisterRenderer(T* pass) {
        auto it = passes.find(pass->type);
        if(it != passes.end() && it->second == pass){
            passes.erase(it);
        }
        isDirty = true;
    }

    std::vector<ILight3D*>& GetLights() { return lights; }
    const std::vector<ILight3D*>& GetLights() const { return lights; }

    Camera3D* GetCurrentCamera() { return currentCamera; }
    const Camera3D* GetCurrentCamera() const { return currentCamera; }

    RenderContext3D* GetContext() { return context.get(); }
    const RenderContext3D* GetContext() const { return context.get(); }

    bool IsDirty() const { return isDirty; }
    bool IsInitialized() const { return isInitialized; }

    void SetCurrentCamera(Camera3D* camera) {
        currentCamera = camera;
        isDirty = true;
    }

    void SetLights(const std::vector<ILight3D*>& newLights) {
        lights = newLights;
        isDirty = true;
    }

    void SetClearColor(float r, float g, float b, float a = 1.0f) {
        clearColor[0] = r;
        clearColor[1] = g;
        clearColor[2] = b;
        clearColor[3] = a;
    }

    void AddLight(ILight3D* light) {
        if (!light) return;
        lights.push_back(light);
        isDirty = true;
    }

    void RemoveLight(ILight3D* light) {
        auto it = std::find(lights.begin(), lights.end(), light);
        if (it != lights.end()) {
            lights.erase(it);
            isDirty = true;
        }
    }

    void ClearLights() {
        lights.clear();
        isDirty = true;
    }
};