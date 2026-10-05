#pragma once

#include <cstdint>

namespace Serialization::Chunks {
    enum class ParsingLayers : uint8_t {
        RESOURCES = 0,
        OBJECT = 1,
        COMPONENTS = 2,
        COMPONENT = 3,
        HIERARCHY = 4,
    };
}