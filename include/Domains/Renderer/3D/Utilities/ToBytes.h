#pragma once

#include <cstddef>
#include <cstring>
#include <span>
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

    template<size_t N>
    static std::vector<std::byte> ToBytes(const char (&str)[N]) {
        std::vector<std::byte> data(N - 1);
        std::memcpy(data.data(), str, N - 1);
        return data;
    }
}