#pragma once

#include "UIElement.h"
#include "memory"

#include "SerializeField.h"

class RectTransform;
class Texture;
class Vector3;
class UIRenderSettings;

class Image : public UIElement, public ISerializable {
private:
    FIELD(UIRenderSettings, settings);

    void CalculateGeometry() override;

public:
    Image();
    Image(Texture* tex);
    ~Image() = default;

    Texture* GetTexture();
    glm::vec4 GetColor();

    void SetTexture(Texture* texture);
    void SetColor(glm::vec4 color);

    const UIRenderSettings& GetRenderSettings() const override;

    SERIALIZED_FIELDS(&settings)
};