#pragma once

#include "UIElement.h"
#include "memory"

class RectTransform;
class Texture;
class Vector3;
class UIRenderSettings;

class Image : public UIElement{
private:
    UIRenderSettings settings;

    void CalculateGeometry() override;

public:
    Image() = default;
    Image(Texture* tex);
    ~Image() = default;

    Texture* GetTexture();
    glm::vec4 GetColor();

    void SetTexture(Texture* texture);
    void SetColor(glm::vec4 color);

    const UIRenderSettings& GetRenderSettings() const override;
};