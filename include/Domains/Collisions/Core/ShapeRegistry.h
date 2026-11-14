#pragma once

#include "IShape.h"
#include <vector>
#include <unordered_map>

class ShapeRegistry {
private:
    std::vector<IShape*> shapes;
    std::unordered_map<ShapeId, IShape*> idToShape;

    void Register(IShape* shape) {
        if (!shape) return;
        if (!idToShape.emplace(shape->GetId(), shape).second) return;
        shapes.push_back(shape);
    }

    void Unregister(IShape* shape) {
        if (!shape) return;
        auto it = idToShape.find(shape->GetId());
        if (it == idToShape.end() || it->second != shape) return;

        idToShape.erase(it);
        auto vit = std::find(shapes.begin(), shapes.end(), shape);
        if (vit != shapes.end()) shapes.erase(vit);
    }

    void Clear() {
        shapes.clear();
        idToShape.clear();
    }

    friend class CollisionDomain;

public:
    IShape* GetShape(ShapeId id) {
        auto it = idToShape.find(id);
        return it != idToShape.end() ? it->second : nullptr;
    }

    const IShape* GetShape(ShapeId id) const {
        auto it = idToShape.find(id);
        return it != idToShape.end() ? it->second : nullptr;
    }

    template<typename T>
        requires (std::is_base_of_v<IShape, T>)
    T* GetShapeByType(ShapeId id) {
        auto it = idToShape.find(id);
        if (it == idToShape.end()) return nullptr;

        if (typeid(T) != it->second->GetType()) return nullptr;
        return static_cast<T*>(it->second);
    }

    template<typename T>
        requires (std::is_base_of_v<IShape, T>)
    const T* GetShapeByType(ShapeId id) const {
        auto it = idToShape.find(id);
        if (it == idToShape.end()) return nullptr;

        if (typeid(T) != it->second->GetType()) return nullptr;
        return static_cast<T*>(it->second);
    }
    
    const std::vector<IShape*>& GetAll() { return shapes; }
};