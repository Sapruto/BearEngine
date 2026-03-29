#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Transform2D.h"
#include "Vector2.h"

#include "Camera.h"

class Camera2D : public Camera {
private:
    Transform2D* transform;    
    float zoom{1.0f};       
    
    float orthoSize;
    
    float screenWidth;      
    float screenHeight;    
    
    bool isDirty;     
    glm::mat4 viewMatrix; 
    glm::mat4 projMatrix;   
    
    void UpdateMatrices();  
    
public:
    Camera2D(float width, float height, float orthoSize = 5.0f);

    void Start() override{ 
        transform = gameObject->GetComponentOfType<Transform2D>(); 
        if(!transform) {
            transform = gameObject->AddComponent<Transform2D>();
        }
        isDirty = true;
    }
    void Update() override{}
    void Destroy() override{}
    
    void SetPosition(const Vector2& pos);
    void Move(const Vector2& delta);
    Vector2 GetPosition() const { 
        return transform ? transform->position : Vector2(0, 0); 
    }
    
    void SetZoom(float z);
    void AddZoom(float delta);
    float GetZoom() const { return zoom; }
    
    const glm::mat4& GetViewMatrix();
    const glm::mat4& GetProjectionMatrix();
    
    Vector2 ScreenToWorld(const Vector2& screenPoint);
    Vector2 WorldToScreen(const Vector2& worldPoint);
    
    void SetScreenSize(float width, float height);

    float GetOrthoSize() const { return orthoSize; }
    void SetOrthoSize(float size) { 
        orthoSize = size; 
        isDirty = true; 
    }
};