#pragma once

#include "UIElement.h"
#include "memory"

#include "SerializeField.h"

class RectTransform;
class Texture;
class UIRenderSettings;

class Image : public UIElement, public ISerializable {
private:
    FIELD(UIRenderSettings, settings);
    
    void CalculateGeometry() override;

public:
    Image();
    explicit Image(Texture* tex);
    ~Image() = default;

    Texture* GetTexture();
    const float* GetColor() const;
    void GetColor(float out[4]) const;
    
    void SetTexture(Texture* texture);
    void SetColor(const float color[4]);

    const UIRenderSettings& GetRenderSettings() const override;

    SERIALIZED_FIELDS(&settings)
};