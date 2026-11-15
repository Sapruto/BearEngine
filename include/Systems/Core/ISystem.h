#pragma once

#include <concepts>
#include <string_view>

namespace Systems {
    class ISystem {
    public:
        ISystem() = default;
        virtual ~ISystem() = default;

        virtual void StartSystem() {}
        virtual void BeginFrameSystem() {}
        virtual void EndFrameSystem() {}
        virtual void DestroySystem() {}

        virtual std::string_view GetSystemName() const = 0;
    };

    template<typename T>
    concept NamedSystemType =
        std::derived_from<T, ISystem> &&
        requires{ { T::GetSystemNameStatic() } -> std::convertible_to<std::string_view>; };

    template<NamedSystemType T>
    std::string_view GetNameFromType() {
        return T::GetSystemNameStatic();
    }

    template<typename Derived>
    class BaseSystem : public ISystem {
    public:
        BaseSystem() {
            static_assert(
                requires { { Derived::GetSystemNameStatic() } -> std::convertible_to<std::string_view>; },
                "Derived must define static GetSystemNameStatic() returning string_view-convertible"
            );
        }

        std::string_view GetSystemName() const override { return Derived::GetSystemNameStatic(); }
    };
}