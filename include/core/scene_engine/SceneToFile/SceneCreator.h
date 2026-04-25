#pragma once

#include <string>
#include <vector>
#include <memory>

#include "SceneDeserializer.h"
#include "SceneSerializer.h"

class Scene;

class SceneCreator {
private:
    SceneSerializer& serializer;
    SceneDeserializer& deserializer;
    
    bool ValidatePath(const std::string& path) const;
    bool ValidateFileName(const std::string& name) const;

public:
    explicit SceneCreator(SceneSerializer& serializer, SceneDeserializer& deserializer);
    
    SceneCreator(const SceneCreator&) = delete;
    SceneCreator& operator=(const SceneCreator&) = delete;
    
    SceneCreator(SceneCreator&&) = default;
    SceneCreator& operator=(SceneCreator&&) = default;

    ~SceneCreator();
    
    bool CreateNewSceneFile(const std::string& path, const std::string& name);
    bool UpdateSceneFile(const std::string& filePath, const Scene& scene);
    bool DeleteSceneFile(const std::string& filePath);
    
    std::vector<std::unique_ptr<Scene>> GetScenes(const std::string& pathDirectory);
    std::unique_ptr<Scene> GetScene(const std::string& pathFile);
};