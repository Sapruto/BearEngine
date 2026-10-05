#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Scene.h"
#include "Systems/Serialization/Chunks/IChankParser.h"

namespace Serialization {
    class SceneSerializer {
    private:
        std::vector<std::weak_ptr<Serialization::Chunks::IChankParser>> parsers;

    public:
        SceneSerializer() = default;
        explicit SceneSerializer(const std::vector<std::shared_ptr<Serialization::Chunks::IChankParser>>& parsers);

        ~SceneSerializer();

        SceneSerializer(const SceneSerializer&) = delete;
        SceneSerializer& operator=(const SceneSerializer&) = delete;

        SceneSerializer(SceneSerializer&&) = default;
        SceneSerializer& operator=(SceneSerializer&&) = default;

        std::string GenerateTextThroughScene(Scene& scene);
    };
}