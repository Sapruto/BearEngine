#pragma once

#include "../GameObject.h"
#include "World/Hierarchy/HierarchySystem.h"
#include "Engine/Serialization/SerializeField.h"

#include <atomic>
#include <cstdint>
#include <string>
#include <vector>

namespace World::Prefabs {
    class Prefab : public SerializeField::ISerializable {
    private:
        FIELD(std::string, uuid);
        uint32_t id = 0;

        std::vector<std::string> childPrefabUuids;
        std::vector<uint32_t> childPrefabIds;

        std::unique_ptr<GameObject> rootObject;
        std::vector<std::unique_ptr<GameObject>> gameObjects;

        HierarchySystem hierarchy;

        inline static std::atomic<uint32_t> nextId = 0;

    public:
        Prefab() : id(nextId.fetch_add(1)) {}

        
    };
}