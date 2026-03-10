#pragma once

#include "Renderer.h"
#include "ModelComponent.h"
#include "Shader.h"
#include "Light3D.h"

#include <vector>
#include <memory>
#include <algorithm> 

class Transform3D;
class Camera3D;

class ModelRenderer : public Renderer{
private:
    Shader m_Shader;

    struct UniformLocations {
        int projection = -1;
        int view = -1;
        int model = -1;
        int color = -1;
    } m_Uniforms;

    std::vector<std::unique_ptr<Light3D>> lights; 

    void CacheUniformLocations();

public:
    ModelRenderer();
    ~ModelRenderer();

    void Update() override;

    void RenderModel(ModelComponent* modelComp, Transform3D* transform, const float* colorRGB);

    template<typename T, typename... Args>
    T* AddLight(Args&&... args) {
        static_assert(std::is_base_of<Light3D, T>::value, "T must be derived from Light3D");
        
        auto light = std::make_unique<T>(std::forward<Args>(args)...);
        
        T* ptr = light.get();
        
        lights.push_back(std::move(light));
        
        return ptr;
    }
    
    void RemoveLight(Light3D* light);
};