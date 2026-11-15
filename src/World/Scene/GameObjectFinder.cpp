#include "World/Scene/GameObjectFinder.h"

void GameObjectFinder::Invalidate() {
    for (auto& cache : queryCache) {
        cache.valid = false;
    }
}

void GameObjectFinder::Clear() {
    queryCache.clear();
}