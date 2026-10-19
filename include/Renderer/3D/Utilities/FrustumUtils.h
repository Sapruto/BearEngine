#pragma once

#include "Math/Matrix/Matrix4x4.h"
#include "Math/Vector/Vector4.h"
#include <cmath>

inline void ExtractFrustumPlanes(const Matrix4x4f& vp, Vector4f out[6]) {
    const float* m = vp.GetData();

    auto at = [&](int row, int col) -> float {
        return m[col * 4 + row];
    };

    out[0] = Vector4f(
        at(0, 3) + at(0, 0),
        at(1, 3) + at(1, 0),
        at(2, 3) + at(2, 0),
        at(3, 3) + at(3, 0)
    );
    out[1] = Vector4f(
        at(0, 3) - at(0, 0),
        at(1, 3) - at(1, 0),
        at(2, 3) - at(2, 0),
        at(3, 3) - at(3, 0)
    );
    out[2] = Vector4f(
        at(0, 3) + at(0, 1),
        at(1, 3) + at(1, 1),
        at(2, 3) + at(2, 1),
        at(3, 3) + at(3, 1)
    );
    out[3] = Vector4f(
        at(0, 3) - at(0, 1),
        at(1, 3) - at(1, 1),
        at(2, 3) - at(2, 1),
        at(3, 3) - at(3, 1)
    );
    out[4] = Vector4f(
        at(0, 3) + at(0, 2),
        at(1, 3) + at(1, 2),
        at(2, 3) + at(2, 2),
        at(3, 3) + at(3, 2)
    );
    out[5] = Vector4f(
        at(0, 3) - at(0, 2),
        at(1, 3) - at(1, 2),
        at(2, 3) - at(2, 2),
        at(3, 3) - at(3, 2)
    );

    for (int i = 0; i < 6; ++i) {
        Vector3f n(out[i].x, out[i].y, out[i].z);
        float len = std::sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
        if (len > 1e-6f) {
            out[i].x /= len;
            out[i].y /= len;
            out[i].z /= len;
            out[i].w /= len;
        }
    }
}

bool IsSphereInFrustum(const Vector4f planes[6], const Vector3f& center, float radius) {
    for (int i = 0; i < 6; ++i) {
        float dist = planes[i].x * center.x
                    + planes[i].y * center.y
                    + planes[i].z * center.z
                    + planes[i].w;
        if (dist < -radius) return false;
    }
    return true;
}