#pragma once

#include <vector>
#include "Scene.h"
#include "ComponentLibrary.h"
#include "SerializeField.h"

class SceneSerializer {
public:
    SceneSerializer(const SceneSerializer&) = delete;
    ~SceneSerializer();
    
    SceneSerializer& operator=(const SceneSerializer&) = delete;
    
    SceneSerializer(SceneSerializer&&) = default;
    SceneSerializer& operator=(SceneSerializer&&) = default;

    static std::string GenerateTextThroughScene(const Scene& scene);
};