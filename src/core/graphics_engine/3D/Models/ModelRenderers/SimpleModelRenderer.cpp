#include "SimpleModelRenderer.h"

#include "Model.h"
#include "ModelComponent.h"
#include "ModelRenderer.h"
#include "ModelFeatureRenderer.h"

#include "Shader.h"
#include "ModelFeatureType.h"

#include "Light3D.h"
#include "DirectionalLight3D.h"

#include "Transform3D.h"
#include "Camera3D.h"

#include "GraphicsManager.h"

#include "ModelFeatureType.h"

#include <vector>
#include <memory>
#include <algorithm> 

#include <glad/glad.h>
#include <GLFW/glfw3.h>

SimpleModelRenderer::SimpleModelRenderer() 
    : m_Shader("include/core/graphics_engine/Shaders/ModelsShaders/Vertex.glsl", "include/core/graphics_engine/Shaders/ModelsShaders/Fragment.glsl")
{
    type = ModelFeatureType::Simple;
    CacheUniformLocations();
}

void SimpleModelRenderer::CacheUniformLocations() {
    m_Shader.Bind();
    m_Uniforms.projection = glGetUniformLocation(m_Shader.GetID(), "projection");
    m_Uniforms.view = glGetUniformLocation(m_Shader.GetID(), "view");
    m_Uniforms.model = glGetUniformLocation(m_Shader.GetID(), "model");
    m_Uniforms.color = glGetUniformLocation(m_Shader.GetID(), "color");
}

void SimpleModelRenderer::RenderGroup(std::vector<ModelComponent*> models, ModelRenderer* baseRenderer){
    if (!baseRenderer) return;

    GraphicsManager* manager = baseRenderer->GetManager();

    glEnable(GL_DEPTH_TEST);
    
    Camera3D* camera = dynamic_cast<Camera3D*>(manager->GetCamera());
    if (!camera) return;

    std::vector<Light3D*> lights = baseRenderer->GetLights();
    
    m_Shader.Bind();
    
    int width, height;
    glfwGetWindowSize(manager->GetWindow()->GetWindow(), &width, &height);
    float aspect = (float)width / (float)height;
    
    if (m_Uniforms.projection != -1) {
        glUniformMatrix4fv(m_Uniforms.projection, 1, GL_FALSE, 
                        glm::value_ptr(camera->GetProjectionMatrix(aspect)));
    }
    
    if (m_Uniforms.view != -1) {
        glUniformMatrix4fv(m_Uniforms.view, 1, GL_FALSE, 
                        glm::value_ptr(camera->GetViewMatrix()));
    }

    GLint lightCountLoc = glGetUniformLocation(m_Shader.GetID(), "lightCount");
    if (lightCountLoc != -1) {
        glUniform1i(lightCountLoc, lights.size());
    }

    bool hasDirLight = false;
    for (auto* light : lights) {
        DirectionalLight3D* dirLight = dynamic_cast<DirectionalLight3D*>(light);
        if (dirLight) {
            GLint dirDirLoc = glGetUniformLocation(m_Shader.GetID(), "dirLightDirection");
            if (dirDirLoc != -1) {
                Vector3 dir = dirLight->GetDirection();
                glUniform3f(dirDirLoc, dir.x, dir.y, dir.z);
            }
            
            GLint dirColLoc = glGetUniformLocation(m_Shader.GetID(), "dirLightColor");
            if (dirColLoc != -1) {
                Vector3 col = light->GetColor();
                glUniform3f(dirColLoc, col.x, col.y, col.z);
            }
            
            hasDirLight = true;
            break;
        }
    }
    
    GLint useDirLightLoc = glGetUniformLocation(m_Shader.GetID(), "useDirLight");
    if (useDirLightLoc != -1) {
        glUniform1i(useDirLightLoc, hasDirLight ? 1 : 0);
    }

    for (size_t i = 0; i < lights.size(); ++i) {
        std::string posName = "lightPos[" + std::to_string(i) + "]";
        Vector3 vecPos = lights[i]->GetPosition();
        glm::vec3 pos(vecPos.x, vecPos.y, vecPos.z);
        GLint loc = glGetUniformLocation(m_Shader.GetID(), posName.c_str());
        if (loc != -1) {
            glUniform3f(loc, pos.x, pos.y, pos.z);
        }
        
        std::string colName = "lightColor[" + std::to_string(i) + "]";
        Vector3 col = lights[i]->GetColor();
        loc = glGetUniformLocation(m_Shader.GetID(), colName.c_str());
        if (loc != -1) {
            glUniform3f(loc, col.x, col.y, col.z);
        }
        
        std::string matrixName = "lightSpaceMatrix[" + std::to_string(i) + "]";
        glm::mat4 lightMatrix = lights[i]->GetLightSpaceMatrix(aspect);
        loc = glGetUniformLocation(m_Shader.GetID(), matrixName.c_str());
        if (loc != -1) {
            glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(lightMatrix));
        }
    }

    GLint viewPosLoc = glGetUniformLocation(m_Shader.GetID(), "viewPos");
    if (viewPosLoc != -1) {
        glUniform3f(viewPosLoc, camera->position.x, camera->position.y, camera->position.z);
    }
    
    glActiveTexture(GL_TEXTURE0);

    for(auto* model : models){
        GameObject* object = model->gameObject;
        if(!object){
            continue;
        }

        Transform3D* transform = object->GetComponentOfType<Transform3D>();
        if(!transform){
            continue;
        }

        Model* modelRefernce = model->GetModel();
        if(!modelRefernce){
            continue;
        }

        const float* colorRGB = model->GetColor();

        RenderModel(model, transform, colorRGB);
    }
}

void SimpleModelRenderer::RenderModel(ModelComponent* modelComp, Transform3D* transform, const float* colorRGB) {
    if (m_Uniforms.model != -1) {
        glm::mat4 modelMatrix = transform->GetMatrix();
        glUniformMatrix4fv(m_Uniforms.model, 1, GL_FALSE, 
                        glm::value_ptr(modelMatrix));
    }
    
    if (m_Uniforms.color != -1) {
        glUniform3f(m_Uniforms.color, colorRGB[0], colorRGB[1], colorRGB[2]);
    }
    
    glBindVertexArray(modelComp->GetVAO());
    glDrawElements(GL_TRIANGLES, modelComp->GetModel()->GetIndices().size(), 
                GL_UNSIGNED_INT, 0);
}