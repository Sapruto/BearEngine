#pragma once

#include "Math/Vector/Vector3.h"
#include "Material.h"
#include "Engine/Serialization/SerializeField.h"

namespace {
    using namespace SerializeField;
}

struct DefaultMaterial : public Material {
private:
    FIELD(Vector3f, color);

public:
    DefaultMaterial() : Material(MaterialType::DEFAULT) {}

    void SetColor(const Vector3f& newColor) { color = newColor; }
    Vector3f GetColor() { return color.GetValue(); }

    SERIALIZED_FIELDS(&color)
};