#pragma once

#include "Math/Vector/Vector3.h"
#include "Renderer/3D/Materials/Material.h"
#include "Serialization/SerializeField.h"

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