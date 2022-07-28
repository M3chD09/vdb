#pragma once

#include "AABB3D.h"
#include "BBox3D.h"
#include "Brick.h"
#include "InternalNode.h"
#include "Morton.h"
#include "OBB3D.h"
#include "RootNode.h"
#include "Vector3D.h"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <functional>
#include <iostream>
#include <vector>

template <uint32_t N1 = 2, uint32_t N2 = 3, uint32_t N3 = 4>
class Topology {
public:
    Topology() = default;
    ~Topology()
    {
        fout.close();
    }
    explicit Topology(float _length)
        : Length(_length)
        , Width(_length)
        , Height(_length)
        , MaxEdge(_length)
    {
        initialize();
    }

    explicit Topology(float _length, float _width, float _height)
        : Length(_length)
        , Width(_width)
        , Height(_height)
        , MaxEdge(std::max(_length, std::max(_width, _height)))
    {
        initialize();
    }

    void initialize()
    {
        AABB3D<float> bbox(Vector3D<float>(0, 0, 0), Length / 2.0f, Width / 2.0f, Height / 2.0f);
        AABB3D<float> bboxGL(coordToGL(bbox.getMin()), coordToGL(bbox.getMax()));
        root.initialize(bboxGL, root.halfEdgeLength());
    }

    void calculateVoxels(std::vector<Vector3D<float>>& coords, std::vector<float>& sizes)
    {
        coords.clear();
        sizes.clear();
        coords.reserve(1 << (N1 + N2));
        sizes.reserve(1 << (N1 + N2));
        root.calculateVoxels(coords, sizes, root.halfEdgeLength());
    }

    void subtract(const BBox3D<float>& bbox, const std::function<bool(const Vector3D<float>&)>& isInside)
    {
        auto startTime = std::chrono::high_resolution_clock::now();
        auto bboxGL = OBB3D<float>(coordToGL(bbox.getCenter()), coordToGL(bbox.getAxis(0)), coordToGL(bbox.getAxis(1)), coordToGL(bbox.getAxis(2)));
        auto isInsideGL = [&](const Vector3D<float>& coord) {
            return isInside(this->coordFromGL(coord));
        };
        root.subtract(bboxGL, isInsideGL, root.halfEdgeLength());
        auto endTime = std::chrono::high_resolution_clock::now();
        fout << "Subtract time: " << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << " ms" << std::endl;
    }

private:
    constexpr inline Vector3D<float> coordToGL(const Vector3D<float>& coord)
    {
        return coord / (MaxEdge / 2.0f);
    }

    constexpr inline Vector3D<float> coordFromGL(const Vector3D<float>& coord)
    {
        return coord * (MaxEdge / 2.0f);
    }

    const float MaxEdge = 1000.0f;
    const float Length = 1000.0f;
    const float Width = 1000.0f;
    const float Height = 1000.0f;
    RootNode<InternalNode<Brick<N3>, N2>, N1> root;
    std::fstream fout = std::fstream("subtract_time.txt", std::ios::out);
};
