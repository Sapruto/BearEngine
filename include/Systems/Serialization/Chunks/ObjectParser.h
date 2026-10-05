#pragma once

#include "Systems/Serialization/Chunks/IChankParser.h"
#include "Systems/Serialization/Chunks/ParsingContext.h"
#include "Systems/Serialization/Chunks/ChunkTokens.h"

namespace Serialization::Chunks {
    class ObjectParser final : public IChankParser {
    public:
        ObjectParser() {
            layer = ParsingLayers::OBJECT;
        }
        ~ObjectParser() = default;

        std::string_view Token() const override {
            return ChunkTokens::object;
        }

        void OnOpen(const std::string& token, ParserState& state) override {
            state.Push(ParseContext::OBJECT);
            state.currentObject = std::make_unique<GameObject>();
        }

        void OnKeyValue(const std::string& key, const std::string& value, ParserState& state) override {
            if (!state.In(ParseContext::OBJECT)) return;

            if (key == ChunkTokens::name && state.currentObject) {
                state.currentObject->SetName(value);
            }
        }

        void OnClose(ParserState& state) override {
            if (!state.currentObject || !state.scene) return;

            state.scene->AddGameObject(std::move(state.currentObject));
            state.currentObject.reset();
            state.Pop(ParseContext::OBJECT);
        }

        void OpenWrite(WriteState& state) const override {
            if (!state.currentObject) return;

            state.out += MakeTabs(state.Offset()) + std::string(ChunkTokens::object)
                    + std::string(ChunkTokens::sep) + std::string(ChunkTokens::start_part) + '\n';
            state.Push(ParseContext::OBJECT);

            state.out += MakeTabs(state.Offset()) + std::string(ChunkTokens::name)
                    + std::string(ChunkTokens::sep) + state.currentObject->GetName() + std::string(ChunkTokens::over) + '\n';
        }

        void CloseWrite(WriteState& state) const override {
            state.out += MakeTabs(state.Offset() - 1) + std::string(ChunkTokens::end_part) + std::string(ChunkTokens::over) + '\n';
            state.Pop(ParseContext::OBJECT);
        }
    };
}