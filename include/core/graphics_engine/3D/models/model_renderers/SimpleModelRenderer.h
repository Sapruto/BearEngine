#pragma once

#include "ModelFeatureRenderer.h"
#include "Shader.h"

#include <string>
#include <vector>
#include <memory>
#include <algorithm> 

class ModelComponent;
class ModelRenderer;
class Transform3D;

class SimpleModelRenderer : public ModelFeatureRenderer {
private:
    Shader shader;

    struct UniformsName {
        static const std::string projection;
        static const std::string view;
        static const std::string viewPos;
        static const std::string color;
        static const std::string lightCount;
        static const std::string dirLightDirection;
        static const std::string dirLightColor;
        static const std::string useDirLight;
        static const std::string lightPosPrefix;
        static const std::string lightColorPrefix;
        static const std::string lightSpaceMatrixPrefix;
        static const std::string model;
    };

    GLuint instanceVBO;
    std::vector<Matrix4x4f> instanceMatrices;

    void RenderModel(ModelComponent* modelComp, Transform3D* transform, const float* colorRGB);

public:
    SimpleModelRenderer();
    ~SimpleModelRenderer() override {}

    void Init() override;
    void Shutdown() override;

    void RenderGroup(std::vector<ModelComponent*>& models, RenderPipeline* pipeline) override;

    Shader& GetShader() { return shader; }
};