#pragma once

#include <glm.hpp>

class RenderSettings {
private:
    float globalScale = 1.0f;    
    glm::vec2 referenceResolution = glm::vec2(1920.0f, 1080.0f); 
    glm::vec2 currentResolution = glm::vec2(1920.0f, 1080.0f); 
    
    bool maintainAspectRatio = true; 
    glm::vec2 offset = glm::vec2(0.0f);  
    
    float brightness = 1.0f;              
    float contrast = 1.0f;                
    
public:
    RenderSettings() = default;
    
    void SetGlobalScale(float scale) { globalScale = scale; }
    float GetGlobalScale() const { return globalScale; }
    
    void SetReferenceResolution(float width, float height) { 
        referenceResolution = glm::vec2(width, height); 
    }
    void SetCurrentResolution(float width, float height) { 
        currentResolution = glm::vec2(width, height); 
    }
    
    glm::vec2 GetResolutionScale() const {
        return glm::vec2(
            currentResolution.x / referenceResolution.x,
            currentResolution.y / referenceResolution.y
        );
    }
    
    float GetFinalScale(float objectScale = 1.0f) const {
        glm::vec2 resScale = GetResolutionScale();
        float aspectScale = maintainAspectRatio ? 
            glm::min(resScale.x, resScale.y) : 1.0f;
        return globalScale * aspectScale * objectScale;
    }
    
    glm::vec2 TransformPosition(const glm::vec2& worldPos) const {
        glm::vec2 resScale = GetResolutionScale();
        float uniformScale = maintainAspectRatio ? 
            glm::min(resScale.x, resScale.y) : 1.0f;
        
        return (worldPos * globalScale * uniformScale) + offset;
    }
    
    void SetOffset(const glm::vec2& newOffset) { offset = newOffset; }
    glm::vec2 GetOffset() const { return offset; }
    
    void SetMaintainAspectRatio(bool maintain) { maintainAspectRatio = maintain; }
    bool GetMaintainAspectRatio() const { return maintainAspectRatio; }
    
    void SetBrightness(float value) { brightness = value; }
    float GetBrightness() const { return brightness; }
    
    void SetContrast(float value) { contrast = value; }
    float GetContrast() const { return contrast; }
    
    glm::mat4 GetTransformMatrix() const {
        glm::mat4 matrix = glm::mat4(1.0f);
        glm::vec2 resScale = GetResolutionScale();
        
        if (maintainAspectRatio) {
            float scale = glm::min(resScale.x, resScale.y);
            matrix = glm::scale(matrix, glm::vec3(scale, scale, 1.0f));
        } else {
            matrix = glm::scale(matrix, glm::vec3(resScale.x, resScale.y, 1.0f));
        }
        
        matrix = glm::scale(matrix, glm::vec3(globalScale, globalScale, 1.0f));
        matrix = glm::translate(matrix, glm::vec3(offset, 0.0f));
        
        return matrix;
    }
};