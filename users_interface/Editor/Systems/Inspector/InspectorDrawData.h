#pragma once

#include "BaseDrawData.h"

#include "GameObject.h"
#include "Vector2.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>

struct ComponentDrawData {
    std::string componentName;
    std::unordered_map<std::string, std::string> params;
    bool isExpanded{true};
};

class InspectorDrawData : public BaseDrawData {
public:
    GameObject* inspectedObject{nullptr};
    std::vector<ComponentDrawData> components;
    
    struct {
        std::string objectName;
        Vector2 position;
        bool isVisible{true};
    } header;
    
    struct {
        Vector2 position;
        bool isVisible{true};
        std::function<void()> onClick;
    } addButton;
    
    float scrollOffset{0};
    float totalHeight{0};
    float viewportHeight{600};
    
    bool showComponentList{false};
    std::vector<std::string> availableComponents;
    std::function<void(const std::string&)> onComponentSelected;
    std::function<void(const std::string&)> onRemoveComponent; 
};