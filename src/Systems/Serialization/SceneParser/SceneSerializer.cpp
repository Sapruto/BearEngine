#include "Systems/Serialization/SceneParser/SceneSerializer.h"

#include <chrono>
#include <filesystem>
#include <memory>

#include "Systems/Serialization/Chunks/ChunkTokens.h"
#include "Systems/Serialization/Chunks/ParsingContext.h"
#include "Logger/Logger.h"

namespace fs = std::filesystem;
using namespace Serialization;
using namespace Serialization::Chunks;

inline constexpr unsigned int currentVersion = 0;

SceneSerializer::SceneSerializer(const std::vector<std::shared_ptr<IChankParser>>& p) {
    parsers.reserve(p.size());
    for (auto& ptr : p) {
        parsers.push_back(ptr);
    }
}

SceneSerializer::~SceneSerializer() {

}

std::string SceneSerializer::GenerateTextThroughScene(Scene& scene) {
    WriteState state;
    state.scene = &scene;
    state.hierarchy = scene.GetHierarchySystem();

    auto find = [&](std::string_view token) -> std::shared_ptr<IChankParser> {
        for (auto& weak : parsers) {
            if (auto p = weak.lock(); p && p->Token() == token) {
                return p;
            }
        }
        return nullptr;
    };

    state.out += std::string(ChunkTokens::name) + std::string(ChunkTokens::sep) + scene.GetName() + std::string(ChunkTokens::over) + "\n";
    state.out += std::string(ChunkTokens::version) + std::string(ChunkTokens::sep) + std::to_string(currentVersion) + std::string(ChunkTokens::over) + "\n";

    auto resourceParser = find(ChunkTokens::resources_pathes);
    if (resourceParser) {
        resourceParser->OpenWrite(state);
        resourceParser->CloseWrite(state);
    }

    auto objectParser = find(ChunkTokens::object);
    auto componentsParser = find(ChunkTokens::components);
    auto componentParser = find(ChunkTokens::component);

    if (objectParser && componentsParser && componentParser) {
        for (auto* obj : scene.GetGameObjects()) {
            if (!obj) continue;

            state.currentObject = obj;
            objectParser->OpenWrite(state);

            componentsParser->OpenWrite(state);

            for (auto* comp : obj->GetComponents()) {
                if (!comp) continue;
                state.currentComponent = comp;
                componentParser->OpenWrite(state);
                componentParser->CloseWrite(state);
            }
            state.currentComponent = nullptr;

            componentsParser->CloseWrite(state);

            state.currentObject = nullptr;
            objectParser->CloseWrite(state);
        }
    }

    auto hierarchyParser = find(ChunkTokens::Hierarchy::hierarchy);
    if (hierarchyParser) {
        hierarchyParser->OpenWrite(state);
        hierarchyParser->CloseWrite(state);
    }

    return state.out;
}