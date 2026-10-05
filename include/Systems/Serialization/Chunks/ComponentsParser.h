#pragma once

#include "Systems/Serialization/Chunks/IChankParser.h"
#include "Systems/Serialization/Chunks/ParsingContext.h"
#include "Systems/Serialization/Chunks/ChunkTokens.h"

namespace Serialization::Chunks {
    class ComponentsParser final : public IChankParser {
    public:
        ComponentsParser() {
            layer = ParsingLayers::COMPONENTS;
        }
        ~ComponentsParser() = default;

        std::string_view Token() const override {
            return ChunkTokens::components;
        }

        void OnOpen(const std::string& token, ParserState& state) override {
            state.Push(ParseContext::COMPONENTS);
        }

        void OnClose(ParserState& state) override {
            state.Pop(ParseContext::COMPONENTS);
        }

        void OpenWrite(WriteState& state) const override {
            state.out += MakeTabs(state.Offset()) + std::string(ChunkTokens::components) 
                    + std::string(ChunkTokens::sep) + std::string(ChunkTokens::start_part) + '\n';
            state.Push(ParseContext::COMPONENTS);
        }

        void CloseWrite(WriteState& state) const override {
            state.out += MakeTabs(state.Offset()) + std::string(ChunkTokens::end_part) + std::string(ChunkTokens::over) + '\n';
            state.Pop(ParseContext::COMPONENTS);
        }
    };
}