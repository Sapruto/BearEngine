#pragma once

#include "Systems/Serialization/Chunks/IChankParser.h"
#include "Systems/Serialization/Chunks/ParsingContext.h"
#include "Systems/Serialization/Chunks/ChunkTokens.h"

namespace Serialization::Chunks {
    class ResourceParser final : public IChankParser {
    public:
        ResourceParser() {
            layer = ParsingLayers::RESOURCES;
        }
        ~ResourceParser() = default;

        std::string_view Token() const override {
            return ChunkTokens::resources_pathes;
        }

        void OnOpen(const std::string& token, ParserState& state) override {
            state.Push(ParseContext::RESOURCES);
        }

        void OnKeyValue(const std::string& key, const std::string& value, ParserState& state) override {
            if (!state.In(ParseContext::RESOURCES)) return;
            state.resources.insert({value, ResourceTypeUtils::FromString(key)});
        }

        void OnClose(ParserState& state) override {
            state.Pop(ParseContext::RESOURCES);
        }

        void OnFinalize(ParserState& state) override {
            if (!state.scene) return;
            
            state.scene->AddResources(state.resources);
            state.resources.clear();
        }

        void OpenWrite(WriteState& state) const override {
            state.Push(ParseContext::RESOURCES);
            state.out += MakeTabs(state.Offset()) + std::string(ChunkTokens::resources_pathes)
                    + std::string(ChunkTokens::sep) + std::string(ChunkTokens::start_part) + '\n';

            auto* rm = state.scene->GetResourceManager();
            if (rm) {
                for (auto& path : rm->GetLoadedResources()) {
                    state.out += MakeTabs(state.Offset() + 1) + path + std::string(ChunkTokens::over) + '\n';
                }
            }
        }

        void CloseWrite(WriteState& state) const override {
            state.out += MakeTabs(state.Offset()) + std::string(ChunkTokens::end_part) + std::string(ChunkTokens::over) + '\n';
            state.Pop(ParseContext::RESOURCES);
        }
    };
}