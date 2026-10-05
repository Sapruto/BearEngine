#pragma once

#include <cstdint>
#include <string_view>

namespace Serialization::Chunks::ChunkTokens {
    inline constexpr std::string_view version = "version";

    inline constexpr std::string_view resources_pathes = "resources_pathes";

    inline constexpr std::string_view name = "name";

    inline constexpr std::string_view object = "object";
    inline constexpr std::string_view components = "components";
    inline constexpr std::string_view component = "component";

    inline constexpr std::string_view start_part = "{";
    inline constexpr std::string_view end_part = "}";

    inline constexpr std::string_view sep = ": ";
    inline constexpr std::string_view over = ",";

    namespace Hierarchy {
        inline constexpr std::string_view hierarchy = "hierarchy";
        inline constexpr char keyValueSep = '=';
        inline constexpr char sectionSep = ';';
        inline constexpr char listSep = ',';
    }
}