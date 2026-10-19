#include "Systems/Serialization/SceneParser/SceneDeserializer.h"

#include <sstream>

#include "Systems/Serialization/Chunks/ParsingContext.h"
#include "Systems/Serialization/Chunks/ChunkTokens.h"
#include "Systems/Serialization/Chunks/IChankParser.h"

#include "World/Scene/Scene.h"
#include "World/GameObject.h"

#include "Logger/Logger.h"

using namespace Serialization::Chunks;
using namespace Serialization;

static std::string Trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

SceneDeserializer::SceneDeserializer(const std::vector<std::shared_ptr<IChankParser>>& p) {
    parsers.reserve(p.size());
    for (auto& ptr : p) {
        parsers.push_back(ptr);
    }
}

void SceneDeserializer::HandleOpenToken(const std::string& token, ParserState& state) {
    for (auto& weak : parsers) {
        auto parser = weak.lock();
        if (!parser) continue;
        if (parser->MatchesOpen(token, state)) {
            parser->OnOpen(token, state);
            return;
        }
    }
    Logging::LoggerFacade::Warn("Unknown token: " + token);
}

void SceneDeserializer::HandleCloseToken(ParserState& state) {
    auto current = state.Current();
    for (auto& weak : parsers) {
        auto parser = weak.lock();
        if (!parser) continue;
        if (parser->Token() == ChunkTokens::resources_pathes && current == ParseContext::RESOURCES) {
            parser->OnClose(state);
            return;
        }
        if (parser->Token() == ChunkTokens::object && current == ParseContext::OBJECT) {
            parser->OnClose(state);
            return;
        }
        if (parser->Token() == ChunkTokens::components && current == ParseContext::COMPONENTS) {
            parser->OnClose(state);
            return;
        }
        if (parser->Token() == ChunkTokens::component && current == ParseContext::COMPONENT) {
            parser->OnClose(state);
            return;
        }
        if (parser->Token() == ChunkTokens::Hierarchy::hierarchy && current == ParseContext::HIERARCHY) {
            parser->OnClose(state);
            return;
        }
    }
}

void SceneDeserializer::HandleKeyValue(const std::string& key, const std::string& value, ParserState& state) {
    auto current = state.Current();

    if (key == ChunkTokens::name) {
        if (current == ParseContext::ROOT && state.scene) {
            state.scene->SetName(value);
        } else if (current == ParseContext::OBJECT && state.currentObject) {
            state.currentObject->SetName(value);
        }
        return;
    }

    for (auto& weak : parsers) {
        auto parser = weak.lock();
        if (!parser) continue;

        if (parser->Token() == ChunkTokens::resources_pathes && current == ParseContext::RESOURCES) {
            parser->OnKeyValue(key, value, state);
            return;
        }
        if (parser->Token() == ChunkTokens::object && current == ParseContext::OBJECT) {
            parser->OnKeyValue(key, value, state);
            return;
        }
        if (parser->Token() == ChunkTokens::component && current == ParseContext::COMPONENT) {
            parser->OnKeyValue(key, value, state);
            return;
        }
        if (parser->Token() == ChunkTokens::Hierarchy::hierarchy && current == ParseContext::HIERARCHY) {
            parser->OnKeyValue(key, value, state);
            return;
        }
    }
    state.Pop(current);
}

void SceneDeserializer::ProcessLine(const std::string& line, ParserState& state) {
    std::string working = line;

    if (!working.empty() && working.back() == ChunkTokens::over[0]) {
        working.pop_back();
    }

    std::string trimmed = Trim(working);
    if (trimmed.empty()) return;

    if (trimmed == ChunkTokens::end_part) {
        HandleCloseToken(state);
        return;
    }

    if (trimmed.back() == ChunkTokens::start_part[0]) {
        std::string raw = trimmed.substr(0, trimmed.size() - 1);

        auto sepPos = raw.find(ChunkTokens::sep);
        std::string token;
        if (sepPos != std::string::npos) {
            token = Trim(raw.substr(0, sepPos));
        }
        else {
            token = Trim(raw);
        }

        HandleOpenToken(token, state);
        return;
    }

    auto sepPos = trimmed.find(ChunkTokens::sep);
    if (sepPos == std::string::npos) {
        Logging::LoggerFacade::Warn("Malformed line: " + trimmed);
        return;
    }
    HandleKeyValue(Trim(trimmed.substr(0, sepPos)),
                Trim(trimmed.substr(sepPos + ChunkTokens::sep.size())),
                state);
}

std::unique_ptr<Scene> SceneDeserializer::GenerateSceneThroughFile(const std::string& sceneText) {
    auto scene = std::make_unique<Scene>();

    std::istringstream stream(sceneText);
    std::string line;

    ParserState state;
    state.scene = scene.get();

    while (std::getline(stream, line)) {
        ProcessLine(line, state);
    }

    for (auto& weak : parsers) {
        if (auto parser = weak.lock()) {
            parser->OnFinalize(state);
        }
    }

    scene->AddResources(state.resources);
    scene->RegisterAllComponents();

    return scene;
}