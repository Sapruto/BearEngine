#pragma once

#include <src\dependencies\glm\GLM\glm.hpp>
#include <src\dependencies\glm\GLM\gtc\matrix_transform.hpp>

class Camera2D {
private:
    glm::vec2 position;    
    float rotation;         
    float zoom;              
    
    float screenWidth;      
    float screenHeight;    
    
    bool needsUpdate;     
    glm::mat4 viewMatrix; 
    glm::mat4 projMatrix;   
    
    void UpdateMatrices();  
    
public:
    Camera2D(float width, float height);
    
    void SetPosition(const glm::vec2& pos);
    void Move(const glm::vec2& delta);
    glm::vec2 GetPosition() const { return position; }
    
    void SetZoom(float z);
    void AddZoom(float delta);
    float GetZoom() const { return zoom; }
    
    const glm::mat4& GetViewMatrix();
    const glm::mat4& GetProjectionMatrix();
    
    glm::vec2 ScreenToWorld(const glm::vec2& screenPoint);
    glm::vec2 WorldToScreen(const glm::vec2& worldPoint);
    
    void SetScreenSize(float width, float height);
};