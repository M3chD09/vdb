#pragma once

#include "BBox3D.h"
#include "Vector3D.h"

#include <vector>

class Tool {
public:
    Tool()
    {
        loadPaths();
    }
    ~Tool() { }
    Tool(float _radius)
        : radius(_radius)
    {
        loadPaths();
    }

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

    void loadPaths()
    {
        paths.clear();
        paths.push_back(std::vector<Vector3D<float>> {
            Vector3D<float>(0, 400, 500),
            Vector3D<float>(-300, -400, 500),
            Vector3D<float>(400, 100, 500),
            Vector3D<float>(-400, 100, 500),
            Vector3D<float>(300, -400, 500),
            Vector3D<float>(0, 400, 500),
        });
        center = paths[0][0];
    }

    bool moveToNextPos()
    {
        if (paths.size() <= currentPathListIndex) {
            return false;
        }
        Vector3D<float> currentPos = paths[currentPathListIndex][currentPathIndex];
        float distance = currentPos.distanceToPoint(center);
        if (distance <= pathStep) {
            center = currentPos;
            currentPathIndex++;
            if (currentPathIndex >= paths[currentPathListIndex].size()) {
                currentPathListIndex++;
                currentPathIndex = 0;
            }
        } else {
            center += (currentPos - center).normalize() * pathStep;
        }
        return true;
    }

    const float pathStep = 10.0f;
    const float radius = 50.0f;
    Vector3D<float> center;
    size_t currentPathIndex = 0;
    size_t currentPathListIndex = 0;
    std::vector<std::vector<Vector3D<float>>> paths;
};
