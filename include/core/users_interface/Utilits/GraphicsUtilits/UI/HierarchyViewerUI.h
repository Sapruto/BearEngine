#pragma once

#include "UIRendering.h"
#include "Canvas.h"
#include "ResourceManager.h"
#include "Font.h"
#include "Texture.h"
#include "Image.h"
#include "Button.h"
#include "GameObject.h"
#include "Text.h"
#include "Scene.h"

#include "UtilData/DrawComponentData.h"

#include "Tag.h"
#include "EditorTags.h"

#include "GraphicsUtilits/UI/UtilitUI.h"

#include "Inspector.h"

#include "UILayers.h"

namespace UtilitUI {
    class HierarchyViewerUI {
    private:
        Font* objectFont{nullptr};
        Texture* objectTextureButton{nullptr};

        Canvas* canvas{nullptr};
        ResourceManager* resources{nullptr};

        Vector2 anchoredMin = Vector2(0.8f, 0.0f);
        Vector2 anchoredMax = Vector2(0.8f, 0.0f);
        float x = -250.0f;
        float yOffset = 50.0f;
        float ySpacing = 35.0f;

    public:
        HierarchyViewerUI(Canvas* cv, ResourceManager* resMgr);

        void LoadResources();

        Canvas* GetCanvas();

        GameObject* CreateUIObject(GameObject* gameObject, int index, Inspector* inspector);
        
        void UpdateUIObjectPosition(GameObject* obj, int index);
        
        void UpdateUIObjectText(GameObject* obj, const std::string& name);
        
        void DestroyUIObject(GameObject* obj, Scene* scene);
    };
}