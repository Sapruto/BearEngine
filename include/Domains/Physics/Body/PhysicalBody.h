#pragma once

#include <unordered_map>
#include <memory>
#include <typeindex>
#include <utility>

#include "World/Components/Component.h"
#include "IPhysicFeature.h"

namespace Physic::Body {
    class PhysicalBody : public Component {
    private:
        std::unordered_map<std::type_index, std::unique_ptr<IPhysicFeature>> typeToFeature;

    public:
        PhysicalBody() = default;
        ~PhysicalBody() override = default;

        template<typename T, typename... Args>
        T* UpsertFeature(Args&&... args) {
            static_assert(std::is_base_of_v<IPhysicFeature, T>,
                            "T must be derived from IPhysicFeature");

            auto feature = std::make_unique<T>(std::forward<Args>(args)...);
            T* ptr = feature.get();

            typeToFeature[std::type_index(typeid(T))] = std::move(feature);
            return ptr;
        }

        template<typename T>
        bool RemoveFeature() {
            return typeToFeature.erase(std::type_index(typeid(T))) > 0;
        }

        template<typename T>
        T* GetFeatureOfType() {
            auto it = typeToFeature.find(std::type_index(typeid(T)));
            return it != typeToFeature.end() ? static_cast<T*>(it->second.get()) : nullptr;
        }

        template<typename T>
        const T* GetFeatureOfType() const {
            auto it = typeToFeature.find(std::type_index(typeid(T)));
            return it != typeToFeature.end() ? static_cast<const T*>(it->second.get()) : nullptr;
        }

        template<typename Base>
        Base* FindFeatureByBase() {
            for (auto& [_, feature] : typeToFeature) {
                if (auto* p = dynamic_cast<Base*>(feature.get()))
                    return p;
            }
            return nullptr;
        } //! THIS METHOD IS VARY LAZY AND SLOW. YOU DON'T WANT TO USE THIS IN UPDATE OR OTHER.

        template<typename T>
        bool HasFeature() const {
            return typeToFeature.find(std::type_index(typeid(T))) != typeToFeature.end();
        }

        const std::unordered_map<std::type_index, std::unique_ptr<IPhysicFeature>>& GetTypeToFeatures() const { return typeToFeature; }
    };
}