#pragma once

#include "Math/Vector/Vector4.h"
#include "../../Base/IRendererScene.h"
#include "../Materials/MaterialType.h"
#include "../Passes/BasePass3D.h"
#include "../Settings/RenderContext3D.h"
#include "../Utilities/UV.h"

#include "RHI/Base/Handles.h"
#include "Systems/Core/HasRequired.h"
#include "Systems/Time/TimeSystem.h"

#include <vector>
#include <memory>

class Transform3D;
class Camera3D;
class Renderable3D;
class Window;
class ILight3D;

class RendererScene3D : public IRendererScene, public Systems::HasRequired {
private:
    RHI::Handles::RenderTargetHandle renderTarget{};

    std::unordered_map<MaterialType, std::vector<Renderable3D*>> groups;
    std::unordered_map<MaterialType, std::unique_ptr<BasePass3D>> passes;

    bool isInitialized = false;

    std::unique_ptr<RenderContext3D> context;

    std::vector<RenderMeshDesc> GetDescsByGroup(MaterialType type);

    std::vector<ILight3D*> lights;
    Camera3D* currentCamera;

    Vector4f clearColor{ 0.05f, 0.05f, 0.1f, 1.0f };

    void InitDefault();
    void CreateRenderTarget();
    void BuildGroups();

    void GenerateNewContext();

    void BeginRender();
    void Present();

public:
    RendererScene3D() {
        context = std::make_unique<RenderContext3D>();
        GetRequire()->AddSystemByType<TimeSystem>();
    }
    ~RendererScene3D() = default;

    void StartDomain() override;
    void EndFrameDomain() override;
    void DestroyDomain() override {
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

    void SetClearColor(Vector4f color) {
        clearColor = color;
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

    void AddByObject(GameObject* gameObject) override {
        Renderable3D* renderable = gameObject->GetComponentOfType<Renderable3D>();
        if (renderable) RegisterRenderable<Renderable3D>(renderable);
    }

    void RemoveByObject(GameObject* gameObject) override {
        Renderable3D* renderable = gameObject->GetComponentOfType<Renderable3D>();
        if (renderable) UnregisterRenderable<Renderable3D>(renderable);
    }
};