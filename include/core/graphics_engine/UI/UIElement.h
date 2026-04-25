#pragma once

#include "Component.h"
#include "RectTransform.h"
#include "UIRenderSettings.h"
#include "UIVertex.h"
#include "SerializeField.h"

class RenderSettings;
class Canvas;

class UIElement : public Component {
protected:
    Canvas* canvas{nullptr};

    std::vector<UIVertex> cachedVertices;
    std::vector<unsigned int> cachedIndices;

    int layer{0};

    bool isVisible{true};

    virtual void CalculateGeometry() = 0;

public:
    RectTransform* rectTransform{nullptr};

    virtual ~UIElement() = default;

    void Initialize(){
        rectTransform = gameObject->GetComponentOfType<RectTransform>();
        if (!rectTransform) {
            rectTransform = gameObject->AddComponent<RectTransform>();
        }
    }

    void Start() override { Initialize(); }

    void UpdateGeometry(){
        cachedVertices.clear();
        cachedIndices.clear();
        CalculateGeometry();
    }

    virtual const UIRenderSettings& GetRenderSettings() const = 0;

    bool GetVisible() const { return isVisible; }
    int GetLayer() const { return layer; }
    Canvas* GetCanvas() const { return canvas; }

    const std::vector<UIVertex>& GetUIVertices() const { return cachedVertices; }
    const std::vector<unsigned int>& GetUIIndices() const { return cachedIndices; }

    void SetVisible(bool newVisible) { isVisible = newVisible; }
    void SetLayer(int newLayer) { layer = newLayer; }
    void SetCanvas(Canvas* newCanvas) { canvas = newCanvas; }
};