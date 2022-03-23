#pragma once

#include "BBox3D.h"
#include "Vector3D.h"

class Tool {
public:
    Tool() {};
    ~Tool() {};

    float radius = 50.0f;
    Vector3D<float> center = Vector3D<float>(0, 0, 500);

    BBox3D<float> getBBox()
    {
        return BBox3D<float>(center - radius, center + radius);
    }

    bool isInside(const Vector3D<float>& p)
    {
        Vector3D<float> p2 = p - center;
        if (p2.z <= 0 && p2.z >= -radius) {
            return p2.distanceToPoint(Vector3D<float>(0, 0, 0)) <= radius;
        } else if (p2.z > 0) {
            return p2.distanceToLine(Vector3D<float>(0, 0, 0), Vector3D<float>(0, 0, 1.0f)) <= radius;
        }

        return false;
    }
};
