#pragma once

#include "BBox3D.h"
#include "Node.h"
#include "Vector3D.h"

#include <algorithm>
#include <array>
#include <bitset>
#include <cmath>
#include <cstdint>
#include <execution>
#include <functional>
#include <memory>
#include <vector>

class Voxel {
public:
    Voxel() = default;
    ~Voxel() = default;
    static constexpr uint32_t edgeLength() { return 1; }
    static constexpr uint32_t sumN() { return 0; }
    static float edgeLengthGL(const uint32_t halfRootEdgeLength)
    {
        return (float)edgeLength() / (float)halfRootEdgeLength;
    }
    static Vector3D<uint32_t> getCoord(uint32_t index)
    {
        return Morton::decode((uint64_t)(index));
    }
    static Vector3D<float> getCoordGL(uint32_t index, uint32_t halfRootEdgeLength)
    {
        Vector3D<uint32_t> coord = getCoord(index);
        return {
            ((float)coord.x + 0.5f) / (float)halfRootEdgeLength - 1.0f,
            ((float)coord.y + 0.5f) / (float)halfRootEdgeLength - 1.0f,
            ((float)coord.z + 0.5f) / (float)halfRootEdgeLength - 1.0f
        };
    }
};

template <uint32_t N>
class Brick : public Node<Voxel, N> {
public:
    Brick() = default;
    ~Brick() = default;

    void initialize(const BBox3D<float>& bbox, const uint32_t halfRootEdgeLength)
    {
        this->reset();
        if (bbox.isInside(this->getBBoxGL(halfRootEdgeLength))) {
            return;
        }
        if (!bbox.intersects(this->getBBoxGL(halfRootEdgeLength))) {
            this->isActive = false;
            return;
        }
        this->subdivide();
        std::for_each(std::execution::par, this->voxels.begin(), this->voxels.end(), [&](auto& v) {
            uint32_t i = &v - voxels.data();
            v = std::make_unique<std::bitset<bitLength>>(0xffff'ffff'ffff'ffff);
            for (uint32_t j = 0; j < bitLength; ++j) {
                if (v->test(j)) {
                    uint64_t index = this->calChildId(i * bitLength + j);
                    if (!bbox.isInside(Voxel::getCoordGL(index, halfRootEdgeLength))) {
                        v->reset(j);
                    }
                }
            }
        });
    }

    void reset()
    {
        for (auto& voxel : voxels) {
            voxel.reset();
        }
        this->isActive = true;
        this->hasChildren = false;
    }

    void subdivide()
    {
        this->hasChildren = true;
        std::for_each(std::execution::par, voxels.begin(), voxels.end(), [&](auto& v) {
            v = std::make_unique<std::bitset<bitLength>>(0xffff'ffff'ffff'ffff);
        });
    }

    void subtract(const BBox3D<float>& bbox, const std::function<bool(const Vector3D<float>&)>& isInside, const uint32_t halfRootEdgeLength)
    {
        if (!bbox.intersects(this->getBBoxGL(halfRootEdgeLength))) {
            return;
        }
        if (!this->hasChildren) {
            this->subdivide();
        }
        std::for_each(std::execution::par, voxels.begin(), voxels.end(), [&](auto& v) {
            uint32_t i = &v - voxels.data();
            if (v == nullptr) {
                return;
            }
            if (v->none()) {
                v.reset();
                return;
            }
            for (uint32_t j = 0; j < bitLength; ++j) {
                if (voxels[i]->test(j)) {
                    uint64_t index = this->calChildId(i * bitLength + j);
                    if (isInside(Voxel::getCoordGL(index, halfRootEdgeLength))) {
                        voxels[i]->reset(j);
                    }
                }
            }
        });
    }

    void calculateVoxels(std::vector<Vector3D<float>>& coords, std::vector<float>& sizes, const uint32_t halfRootEdgeLength)
    {
        if (this->hasChildren) {
            for (uint32_t i = 0; i < Node<Voxel, N>::maxChildrenCount() / bitLength; ++i) {
                if (voxels[i] == nullptr) {
                    continue;
                }
                if (voxels[i]->none()) {
                    voxels[i].reset();
                    continue;
                }
                for (uint32_t j = 0; j < bitLength; ++j) {
                    if (voxels[i]->test(j)) {
                        uint64_t index = this->calChildId(i * bitLength + j);
                        coords.push_back(Voxel::getCoordGL(index, halfRootEdgeLength));
                        sizes.push_back(Voxel::edgeLengthGL(halfRootEdgeLength));
                    }
                }
            }
        } else {
            coords.push_back(Node<Voxel, N>::getCoordGL(halfRootEdgeLength));
            sizes.push_back(Node<Voxel, N>::edgeLengthGL(halfRootEdgeLength));
        }
    }

private:
    static const size_t bitLength = 64;
    std::array<std::unique_ptr<std::bitset<bitLength>>, Node<Voxel, N>::maxChildrenCount() / bitLength> voxels;
};
