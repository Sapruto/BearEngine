#pragma once

#include <string>
#include <vector>
#include <utility>

class GameObject;

class GameObjectFinder {
public:
    using TypeId = const void*;

    template<typename T>
    static TypeId TypeTag() {
        static const char tag = 0;
        return &tag;
    }

    struct CachedQuery {
        TypeId type = nullptr;
        std::vector<GameObject*> results;
        bool valid = false;

        CachedQuery(TypeId t, std::vector<GameObject*> r)
            : type(t), results(std::move(r)), valid(true) {}
    };

private:
    std::vector<CachedQuery> queryCache;

public:
    GameObjectFinder() = default;

    template<typename T, typename Provider>
    std::vector<GameObject*> Find(Provider&& provider) {
        const TypeId type = TypeTag<T>();

        for (auto& cache : queryCache) {
            if (cache.type == type && cache.valid) {
                return cache.results;
            }
        }

        std::vector<GameObject*> result = provider();

        for (auto& cache : queryCache) {
            if (cache.type == type) {
                cache.results = result;
                cache.valid = true;
                return cache.results;
            }
        }
        queryCache.emplace_back(type, std::move(result));
        return queryCache.back().results;
    }

    void Invalidate();
    void Clear();
};