#include "World/Scene/ComponentCatalog.h"

#include "World/Components/Component.h"

#include <algorithm>

void ComponentCatalog::Clear() {
    uuidToComponent.clear();
    components.clear();
    allRequirers.clear();
    indecesNewRequirers.clear();
}

void ComponentCatalog::Register(Component* comp) {
    if (!comp) return;

    const std::string& uuid = comp->GetUUID();
    if (!uuid.empty()) {
        uuidToComponent[uuid] = comp;
    }

    components.push_back(comp);

    if (auto* req = dynamic_cast<Systems::HasRequired*>(comp)) {
        allRequirers.push_back(req);
        indecesNewRequirers.push_back(allRequirers.size() - 1);
    }
}

void ComponentCatalog::Unregister(Component* comp) {
    if (!comp) return;

    const std::string& uuid = comp->GetUUID();
    if (!uuid.empty()) {
        uuidToComponent.erase(uuid);
    }

    auto itComp = std::find(components.begin(), components.end(), comp);
    if (itComp != components.end()) {
        std::iter_swap(itComp, components.end() - 1);
        components.pop_back();
    }

    auto* req = dynamic_cast<Systems::HasRequired*>(comp);
    if (!req) return;

    auto it = std::find(allRequirers.begin(), allRequirers.end(), req);
    if (it == allRequirers.end()) return;

    const size_t removedIdx = static_cast<size_t>(std::distance(allRequirers.begin(), it));
    const size_t lastIdx    = allRequirers.size() - 1;

    indecesNewRequirers.erase(
        std::remove(indecesNewRequirers.begin(), indecesNewRequirers.end(), removedIdx),
        indecesNewRequirers.end());

    if (removedIdx != lastIdx) {
        std::swap(allRequirers[removedIdx], allRequirers[lastIdx]);

        for (size_t& idx : indecesNewRequirers) {
            if (idx == lastIdx) {
                idx = removedIdx;
                break;
            }
        }
    }
    allRequirers.pop_back();
}

Component* ComponentCatalog::FindByUUID(const std::string& uuid) const {
    auto it = uuidToComponent.find(uuid);
    return it != uuidToComponent.end() ? it->second : nullptr;
}

std::vector<Systems::HasRequired*> ComponentCatalog::ConsumeNewRequirers() {
    std::vector<Systems::HasRequired*> result;
    result.reserve(indecesNewRequirers.size());

    for (size_t idx : indecesNewRequirers) {
        if (idx < allRequirers.size()) {
            result.push_back(allRequirers[idx]);
        }
    }

    indecesNewRequirers.clear();
    return result;
}