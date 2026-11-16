#pragma once

#include <cstddef>
#include <cstring>
#include <span>
#include <string_view>
#include <type_traits>
#include <vector>

namespace Renderer::Utilities {
    template<typename T>
        requires (std::is_trivially_copyable_v<T> && (!std::is_pointer_v<T>))
    static std::vector<std::byte> ToBytes(const T& value) {
        static_assert(std::is_trivially_copyable_v<T>,
                        "ToBytes requires a trivially copyable type");
        auto bytes = std::as_bytes(std::span{&value, 1});
        return { bytes.begin(), bytes.end() };
    }

    static std::vector<std::byte> ToBytes(std::string_view str) {
        auto bytes = std::as_bytes(std::span{str});
        return { bytes.begin(), bytes.end() };
    }
}