#pragma once

#include "Renderer/3D/Materials/MaterialType.h"
#include "Systems/Serialization/SerializeField.h"

class Material : SerializeField::ISerializable {
private:
    MaterialType type;

public:
    explicit Material(MaterialType newType) : type(newType) {}
    virtual ~Material() = default;

    MaterialType GetType() { return type; }
};