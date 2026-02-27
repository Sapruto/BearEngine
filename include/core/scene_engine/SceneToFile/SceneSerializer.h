#pragma once

#include <vector>
#include <filesystem> 
#include "include/core/scene_engine/Scene.h"
#include "include/core/system_engine/component_system/ComponentLibrary.h"
#include "include/core/system_engine/atributs/SerializeField.h"

class SceneSerializer {
public:
    SceneSerializer(const SceneSerializer&) = delete;
    ~SceneSerializer();
    
    SceneSerializer& operator=(const SceneSerializer&) = delete;
    
    SceneSerializer(SceneSerializer&&) = default;
    SceneSerializer& operator=(SceneSerializer&&) = default;

    static std::string GenerateTextThroughScene(const Scene& scene);
};