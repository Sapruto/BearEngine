#pragma once

#include "IShape.h"
#include "ShapeRegistry.h"

#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <utility>

class CollisionEventBus {
public:
    enum class CollisionEventType { ENTER, STAY, EXIT };
    enum class FilterMode { AnyOf, AllOf };

private:
    using Callback = std::function<void(ShapeId, ShapeId, ShapeRegistry&)>;
    using Pair = std::pair<ShapeId, ShapeId>;

    struct Subscriber {
        CollisionEventType eventType;
        Callback callback;
        std::unordered_set<ShapeId> filter;
        FilterMode filterMode = FilterMode::AnyOf;
    };

    struct PairHash {
        size_t operator()(const Pair& p) const noexcept {
            return std::hash<ShapeId>{}(p.first) ^ (std::hash<ShapeId>{}(p.second) << 1);
        }
    };

    ShapeRegistry& registry;

    std::unordered_map<size_t, Subscriber> subscribers;
    std::unordered_map<CollisionEventType, std::vector<size_t>> byType;
    std::unordered_map<ShapeId, std::vector<size_t>> byShapeId;

    std::unordered_set<Pair, PairHash> currentFrame;
    std::unordered_set<Pair, PairHash> previousFrame;

    size_t nextSubscriberId = 1;

    static Pair MakeKey(ShapeId a, ShapeId b) {
        return a < b ? Pair{a, b} : Pair{b, a};
    }

    bool MatchesFilter(const Subscriber& sub, ShapeId a, ShapeId b) const {
        if (sub.filter.empty()) return true;
        const bool hasA = sub.filter.count(a) > 0;
        const bool hasB = sub.filter.count(b) > 0;
        return sub.filterMode == FilterMode::AllOf ? (hasA && hasB) : (hasA || hasB);
    }

    void DispatchEvent(CollisionEventType type, ShapeId a, ShapeId b) {
        auto typeIt = byType.find(type);
        if (typeIt == byType.end()) return;

        std::unordered_set<size_t> seen;
        const bool exit = (type == CollisionEventType::EXIT);

        auto tryDispatch = [&](size_t subId) {
            if (!seen.insert(subId).second) return;
            auto& sub = subscribers.at(subId);
            if (sub.eventType != type) return;
            if (!MatchesFilter(sub, a, b)) return;
            if (!exit && (!registry.GetShape(a) || !registry.GetShape(b))) return;
            sub.callback(a, b, registry);
        };

        auto universalIt = universalIdsByType.find(type);
        if (universalIt != universalIdsByType.end())
            for (size_t id : universalIt->second) tryDispatch(id);

        if (auto it = byShapeId.find(a); it != byShapeId.end()) {
            for (size_t id : it->second) tryDispatch(id);
        }
        if (a != b) {
            if (auto it = byShapeId.find(b); it != byShapeId.end()) {
                for (size_t id : it->second) tryDispatch(id);
            }
        }
    }

    std::unordered_map<CollisionEventType, std::vector<size_t>> universalIdsByType;

public:
    explicit CollisionEventBus(ShapeRegistry& reg) : registry(reg) {}

    size_t Subscribe(Callback cb,
                        CollisionEventType type = CollisionEventType::ENTER,
                        std::unordered_set<ShapeId> filter = {},
                        FilterMode mode = FilterMode::AnyOf) {
        const size_t id = nextSubscriberId++;
        Subscriber sub{ type, std::move(cb), std::move(filter), mode };

        byType[type].push_back(id);
        if (sub.filter.empty()) {
            universalIdsByType[type].push_back(id);
        }
        else {
            for (ShapeId sid : sub.filter) byShapeId[sid].push_back(id);
        }

        subscribers.emplace(id, std::move(sub));
        return id;
    }

    void Unsubscribe(size_t id) {
        auto it = subscribers.find(id);
        if (it == subscribers.end()) return;

        const auto& sub = it->second;
        auto removeFrom = [&](auto& vec) {
            vec.erase(std::remove(vec.begin(), vec.end(), id), vec.end());
        };

        removeFrom(byType[sub.eventType]);
        if (sub.filter.empty()) {
            removeFrom(universalIdsByType[sub.eventType]);
        }
        else {
            for (ShapeId sid : sub.filter) {
                if (auto it2 = byShapeId.find(sid); it2 != byShapeId.end())
                    removeFrom(it2->second);
            }
        }
        subscribers.erase(it);
    }

    void Report(ShapeId a, ShapeId b) {
        if (a == b) return;
        currentFrame.insert(MakeKey(a, b));
    }

    void Flush() {
        for (const auto& p : currentFrame) {
            const bool existed = previousFrame.count(p) > 0;
            DispatchEvent(existed ? CollisionEventType::STAY
                                    : CollisionEventType::ENTER, p.first, p.second);
        }
        for (const auto& p : previousFrame) {
            if (currentFrame.count(p) == 0) DispatchEvent(CollisionEventType::EXIT, p.first, p.second);
        }
        previousFrame.swap(currentFrame);
        currentFrame.clear();

        std::erase_if(previousFrame, [&](const Pair& p) {
            return !registry.GetShape(p.first) || !registry.GetShape(p.second);
        });
    }
};