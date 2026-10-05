#pragma once

#include <string>
#include <vector>
#include <memory>

#include "Systems/Serialization/SceneParser/SceneDeserializer.h"
#include "Systems/Serialization/SceneParser/SceneSerializer.h"

#include "Systems/Serialization/Chunks/IChankParser.h"

class Scene;

namespace Serialization {
    class SceneCreator {
    private:
        std::vector<std::shared_ptr<Serialization::Chunks::IChankParser>> parsers;

        SceneSerializer serializer;
        SceneDeserializer deserializer;
        
        std::vector<std::shared_ptr<Serialization::Chunks::IChankParser>> MakeDefaultParsers();
        bool ValidatePath(const std::string& path) const;
        bool ValidateFileName(const std::string& name) const;

    public:
        SceneCreator()
            : parsers(MakeDefaultParsers()),
            serializer(parsers),
            deserializer(parsers)
        {}
        SceneCreator(std::vector<std::shared_ptr<Serialization::Chunks::IChankParser>> p);
        
        SceneCreator(const SceneCreator&) = delete;
        SceneCreator& operator=(const SceneCreator&) = delete;
        
        SceneCreator(SceneCreator&&) = default;
        SceneCreator& operator=(SceneCreator&&) = default;

        ~SceneCreator();
        
        bool CreateNewSceneFile(const std::string& path, const std::string& name);
        bool UpdateSceneFile(const std::string& filePath, Scene& scene);
        bool DeleteSceneFile(const std::string& filePath);
        
        std::vector<std::unique_ptr<Scene>> GetScenes(const std::string& pathDirectory);
        std::unique_ptr<Scene> GetScene(const std::string& pathFile);

        std::string SerializeToString(Scene& scene);
        std::unique_ptr<Scene> DeserializeFromString(const std::string& text);
    };
}