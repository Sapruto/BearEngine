#pragma once

#include "InspectorUI.h"
#include "UtilData/DrawComponentData.h"

#include "GameObject.h"
#include "ComponentLibrary.h"

class Scene;

class InspectorLogic {
private:
    GameObject* currentGameObject{nullptr};

    Scene* scene{nullptr};
    std::unordered_map<GameObject*, DrawComponentData> inspectorObjects;

    std::vector<std::string> componentNames;

    bool isShowAllComps{false};

    InspectorUI* ui{nullptr};

    void ClearInspector();

public:
    InspectorLogic(Scene* scene, InspectorUI* ui) : scene(scene), ui(ui) {}

    void Start();

    void Update();

    void SetObject(GameObject* newCurrentObject);
    GameObject* GetCurrentObject();
};