#pragma once

#include "MeshShadingType.h"
#include "RenderPipeline.h"
#include "Mesh.h"

#include <vector> 
#include <glad/glad.h>

class RendererScene;

class IRenderer {
protected:
    MeshShadingType type;
    bool isInitialized = false;

    void ClearUVRegion(const std::vector<Mesh*>& models, RenderPipeline* pipeline) {
        if (!pipeline || models.empty()) return;
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_ZERO, GL_ZERO);
        glDisable(GL_DEPTH_TEST);
        
        for (auto* model : models) {
            if (!model) continue;
            
            const ObjectUVData& uvData = pipeline->GetObjectUV(model);
            if (!uvData.isVisible || uvData.screenVertices.empty()) continue;
            
            glBindVertexArray(model->GetVAO());
            glDrawElements(GL_TRIANGLES, uvData.indices.size(), GL_UNSIGNED_INT, 0);
        }
        
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
    }

public:
    virtual ~IRenderer() = default;
    
    MeshShadingType GetType() const { return type; }
    bool IsInitialized() const { return isInitialized; }
    
    virtual void Init() = 0;
    virtual void Shutdown() = 0;
    
    virtual void RenderGroup(std::vector<Mesh*>& models, RenderPipeline* pipeline) = 0;
};