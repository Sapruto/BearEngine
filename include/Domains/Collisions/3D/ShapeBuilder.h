#pragma once

#include "Math/Vector/Vector3.h"
#include "Polyhedron3D.h"
#include <memory>

class ShapeBuilder {
public:
    ShapeBuilder() = default;
    ~ShapeBuilder() = default;

    std::unique_ptr<Polyhedron3D> CreateCube(float edgeSize) {
        return CreateBox(Vector3f(edgeSize, edgeSize, edgeSize));
    }

    std::unique_ptr<Polyhedron3D> CreateBox(const Vector3f& size) {
        const float x = size.x * 0.5f;
        const float y = size.y * 0.5f;
        const float z = size.z * 0.5f;

        auto box = std::make_unique<Polyhedron3D>();

        box->AddVertex(Vector3f(-x, -y, -z));
        box->AddVertex(Vector3f( x, -y, -z), {0});
        box->AddVertex(Vector3f( x, -y,  z), {1});
        box->AddVertex(Vector3f(-x, -y,  z), {2, 0});
        box->AddVertex(Vector3f(-x,  y, -z), {0});
        box->AddVertex(Vector3f( x,  y, -z), {4, 1});
        box->AddVertex(Vector3f( x,  y,  z), {5, 2});
        box->AddVertex(Vector3f(-x,  y,  z), {6, 3, 4});

        return box;
    }
};