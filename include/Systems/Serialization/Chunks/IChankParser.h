#pragma once

#include <string>
#include <string_view>

#include "Systems/Serialization/Chunks/ParsingLayers.h"

namespace Serialization::Chunks {
    struct ParserState;
    struct WriteState;

    class IChankParser {
    protected:
        ParsingLayers layer = ParsingLayers::OBJECT;

        std::string MakeTabs(int count) const {
            if (count <= 0) {
                return std::string();
            }
            return std::string(static_cast<size_t>(count), '\t');
        }

    public:
        virtual ~IChankParser() = default;

        virtual std::string_view Token() const = 0;

        virtual void OnOpen([[maybe_unused]] const std::string& token, [[maybe_unused]] ParserState& state) {}

        virtual void OnKeyValue([[maybe_unused]] const std::string& key,
                                [[maybe_unused]] const std::string& value,
                                [[maybe_unused]] ParserState& state) {}

        virtual void OnClose([[maybe_unused]] ParserState& state) {}
        virtual void OnFinalize([[maybe_unused]] ParserState& state) {}

        virtual void OpenWrite(WriteState& state) const {}
        virtual void CloseWrite(WriteState& state) const {}

        virtual bool MatchesOpen(const std::string& token,
                                [[maybe_unused]] const ParserState& state) const { return token == Token(); }
    };
}