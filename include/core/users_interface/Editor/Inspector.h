#pragma once

#include "GraphicsUtilits/UI/InspectorUI.h"
#include "UtilData/DrawComponentData.h"

#include "GameObject.h"
#include "ComponentLibrary.h"

class Scene;

class Inspector{
private:
    GameObject* currentGameObject{nullptr};

    Scene* scene{nullptr};
    std::unordered_map<GameObject*, DrawComponentData> inspectorObjects;

    std::vector<std::string> componentNames;

    bool isShowAllComps{false};

    UtilitUI::InspectorUI* ui{nullptr};

    void ClearInspector();

public:
    Inspector(Scene* scene, UtilitUI::InspectorUI* ui) : scene(scene), ui(ui) {}

    void StartInspectOfObject();

    void UpdateInspectOfObject();

    void SetObject(GameObject* newCurrentObject);
    GameObject* GetCurrentObject();
};