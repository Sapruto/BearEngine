#pragma once

#include "Systems/Serialization/Chunks/IChankParser.h"
#include "Systems/Serialization/Chunks/ParsingContext.h"
#include "Systems/Serialization/Chunks/ChunkTokens.h"

#include "World/Components/Component.h"
#include "World/Components/ComponentLibrary.h"

namespace Serialization::Chunks {
    class ComponentParser final : public IChankParser {
    public:
        ComponentParser() {
            layer = ParsingLayers::COMPONENT;
        }
        ~ComponentParser() = default;

        std::string_view Token() const override {
            return ChunkTokens::component;
        }

        void OnOpen(const std::string& token, ParserState& state) override {
            state.currentCompName = token;
            auto comp = ComponentRegistry::Create(state.currentCompName);
            if (comp && state.currentObject) {
                state.currentComponent = comp.get();
                state.currentObject->AddComponent(comp.release());
            }
            state.Push(ParseContext::COMPONENT);
        }

        void OnKeyValue(const std::string& key, const std::string& value, ParserState& state) override {
            if (!state.In(ParseContext::COMPONENT)) return;
            if (!state.currentComponent) return;

            for (auto* field : state.currentComponent->GetSerializedFields()) {
                if (field->GetName() == key) {
                    field->FromString(value);
                    break;
                }
            }
        }

        void OnClose(ParserState& state) override {
            state.currentComponent = nullptr;
            state.currentCompName = "";
            state.Pop(ParseContext::COMPONENT);
        }

        void OnFinalize(ParserState& state) override {
            if (!state.scene) return;
            
            for (auto* gameObject : state.scene->GetGameObjects()) {
                if (!gameObject) continue;
                
                auto components = gameObject->GetComponents();
                
                for (auto* component : components) {
                    if (!component) continue;
                    
                    for (auto* field : component->GetSerializedFields()) {
                        field->Resolve(state.scene);
                    }
                }
            }
        }

        void OpenWrite(WriteState& state) const override {
            if (!state.currentComponent) return;

            state.Push(ParseContext::COMPONENT);

            std::string compName = ComponentRegistry::GetNameByComponent(state.currentComponent);
            state.out += MakeTabs(state.Offset()) + compName + std::string(ChunkTokens::sep) + std::string(ChunkTokens::start_part) + '\n';
            
            for (const auto* field : state.currentComponent->GetSerializedFields()) {
                state.out += MakeTabs(state.Offset() + 1) + field->GetName() + std::string(ChunkTokens::sep) + field->ToString() + std::string(ChunkTokens::over) + '\n';
            }
        }

        void CloseWrite(WriteState& state) const override {
            state.out += MakeTabs(state.Offset()) + std::string(ChunkTokens::end_part) + std::string(ChunkTokens::over) + '\n';
            state.Pop(ParseContext::COMPONENT);
        }

        bool MatchesOpen(const std::string& token, const ParserState& state) const override {
            return state.In(ParseContext::COMPONENTS);
        }
    };
}