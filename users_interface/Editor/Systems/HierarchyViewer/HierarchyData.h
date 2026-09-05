#pragma once

#include "BaseDrawData.h"
#include "GameObject.h"
#include <functional>
#include <string>

class InspectorLogic;

class HierarchyData : public BaseDrawData {
public:
    int index{0};
    std::string objectName;
    GameObject* gameObject{nullptr};
    InspectorLogic* inspector{nullptr};
    std::function<void()> onClick;
    
    HierarchyData() = default;
    ~HierarchyData() = default;
};