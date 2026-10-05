#pragma once

#include <memory>
#include <string>
#include <vector>

#include "World/Scene/Scene.h"
#include "Systems/Serialization/Chunks/IChankParser.h"

namespace Serialization {
    class SceneDeserializer {
    private:
        std::vector<std::weak_ptr<Serialization::Chunks::IChankParser>> parsers;

        void HandleOpenToken(const std::string& token, Serialization::Chunks::ParserState& state);
        void HandleCloseToken(Serialization::Chunks::ParserState& state);
        void HandleKeyValue(const std::string& key, const std::string& value,
                            Serialization::Chunks::ParserState& state);
        void ProcessLine(const std::string& line, Serialization::Chunks::ParserState& state);

    public:
        SceneDeserializer() = default;
        explicit SceneDeserializer(const std::vector<std::shared_ptr<Serialization::Chunks::IChankParser>>& parsers);

        SceneDeserializer(const SceneDeserializer&) = delete;
        SceneDeserializer& operator=(const SceneDeserializer&) = delete;

        SceneDeserializer(SceneDeserializer&&) = default;
        SceneDeserializer& operator=(SceneDeserializer&&) = default;

        std::unique_ptr<Scene> GenerateSceneThroughFile(const std::string& sceneText);
    };
}