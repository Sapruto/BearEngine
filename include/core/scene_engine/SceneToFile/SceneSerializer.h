#pragma once

#include <vector>
#include "Scene.h"
#include "ComponentLibrary.h"
#include "SerializeField.h"

class SceneSerializer {
public:
    SceneSerializer() = default;
    ~SceneSerializer();
    
    SceneSerializer& operator=(const SceneSerializer&) = delete;
    
    SceneSerializer(SceneSerializer&&) = default;
    SceneSerializer& operator=(SceneSerializer&&) = default;

    std::string GenerateTextThroughScene(const Scene& scene);
};