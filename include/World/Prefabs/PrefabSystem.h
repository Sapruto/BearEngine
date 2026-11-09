#pragma once

#include "Prefab.h"

namespace World::Prefabs {
    class PrefabSystem {
    private:
        std::vector<std::unique_ptr<Prefab>> prefabs;
        std::unordered_map<size_t, Prefab*> idToPrefab;

    public:
        
    };
}