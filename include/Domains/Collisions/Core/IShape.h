#pragma once

#include "World/Components/Component.h"
#include <atomic>
#include <cstdint>
#include <type_traits>
#include <typeindex>

using ShapeId = uint32_t;

class IShape : public Component {
private:
    ShapeId id = 0;
    inline static std::atomic<ShapeId> nextId{1};

public:
    IShape() : id(nextId.fetch_add(1)) {}
    virtual ~IShape() = default;
    
    virtual float GetVolume() const = 0;
    virtual std::type_index GetType() const = 0;

    ShapeId GetId() const { return id; }
};

template<typename Derived, typename BaseShape = IShape>
    requires (std::is_base_of_v<IShape, BaseShape>)
class TypedShape : public BaseShape {
public:
    using BaseShape::BaseShape;
    std::type_index GetType() const override { return typeid(Derived); }
};