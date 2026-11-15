#pragma once

#include <vector>
#include <unordered_map>
#include <algorithm>

#include "../Body/PhysicalBody.h"

namespace Physic {
    class PhysicDomain;
}

namespace Physic::Body {
    class BodyRegistry {
    private:
        std::vector<PhysicalBody*> bodies;
        std::unordered_map<BodyId, PhysicalBody*> idToBody;
        std::unordered_map<std::type_index, std::vector<PhysicalBody*>> featureTypeToBody;

        void Register(PhysicalBody* body) {
            if (!body) return;
            if (!idToBody.emplace(body->GetId(), body).second) return;
            bodies.push_back(body);
        }

        void Unregister(PhysicalBody* body) {
            if (!body) return;
            auto it = idToBody.find(body->GetId());
            if (it == idToBody.end() || it->second != body) return;

            idToBody.erase(it);
            auto vit = std::find(bodies.begin(), bodies.end(), body);
            if (vit != bodies.end()) bodies.erase(vit);
        }

        void Clear() {
            bodies.clear();
            idToBody.clear();
        }

        friend class ::Physic::PhysicDomain;

    public:
        PhysicalBody* GetBody(BodyId id) {
            auto it = idToBody.find(id);
            return it != idToBody.end() ? it->second : nullptr;
        }

        const PhysicalBody* GetBody(BodyId id) const {
            auto it = idToBody.find(id);
            return it != idToBody.end() ? it->second : nullptr;
        }

        bool Contains(BodyId id) const { return idToBody.find(id) != idToBody.end(); }

        template<typename T>
        const std::vector<PhysicalBody*>& GetBodyWithFeature() { return featureTypeToBody[typeid(T)]; }

        template<class... Args>
        std::vector<PhysicalBody*> GetBodyWithFeatures() const {
            std::vector<const std::vector<PhysicalBody*>*> lists;
            lists.reserve(sizeof...(Args));
            (lists.push_back(&featureTypeToBody.at(typeid(Args))), ...);

            if (lists.empty()) return bodies;

            auto* smallest = *std::min_element(lists.begin(), lists.end(),
                [](auto* a, auto* b) { return a->size() < b->size(); });

            std::vector<PhysicalBody*> result;
            for (PhysicalBody* body : *smallest) {
                bool ok = true;
                for (auto* list : lists) {
                    if (list == smallest) continue;
                    if (std::find(list->begin(), list->end(), body) == list->end()) { ok = false; break; }
                }
                if (ok) result.push_back(body);
            }
            return result;
        }


        const std::vector<PhysicalBody*>& GetAllBodies() const { return bodies; }
        size_t GetBodyCount() const { return bodies.size(); }
    };
}