#pragma once

#include "BBox3D.h"
#include "Brick.h"
#include "InternalNode.h"
#include "Morton.h"
#include "RootNode.h"
#include "Vector3D.h"

#include <chrono>
#include <fstream>
#include <functional>
#include <iostream>
#include <vector>

template <uint32_t N1 = 2, uint32_t N2 = 3, uint32_t N3 = 4>
class Topology {
public:
    Topology() {

    };
    ~Topology()
    {
        fout.close();
    };
    Topology(uint32_t _length)
        : Length(_length)
    {
    }

    void calculateVoxels(std::vector<Vector3D<float>>& coords, std::vector<float>& sizes)
    {
        coords.clear();
        sizes.clear();
        coords.reserve(1 << (N1 + N2));
        sizes.reserve(1 << (N1 + N2));
        root.calculateVoxels(coords, sizes, root.halfEdgeLength());
    }

    Vector3D<float> coordToGL(const Vector3D<float>& coord)
    {
        return coord / (float)(Length >> 1);
    }

    Vector3D<float> coordFromGL(const Vector3D<float>& coord)
    {
        return coord * (float)(Length >> 1);
    }

    void subtract(const BBox3D<float>& bbox, std::function<bool(const Vector3D<float>&)> isInside)
    {
        auto startTime = std::chrono::high_resolution_clock::now();
        BBox3D<float> bboxGL = BBox3D<float>(coordToGL(bbox.min), coordToGL(bbox.max));
        auto isInsideGL = [&](const Vector3D<float>& coord) {
            return isInside(this->coordFromGL(coord));
        };
        root.subtract(bboxGL, isInsideGL, root.halfEdgeLength());
        auto endTime = std::chrono::high_resolution_clock::now();
        fout << "Subtract time: " << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << " ms" << std::endl;
    }

    RootNode<InternalNode<Brick<N3>, N2>, N1> root;
    const uint32_t Length = 1000;
    std::fstream fout = std::fstream("subtract_time.txt", std::ios::out);
};
