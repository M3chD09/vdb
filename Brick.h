#pragma once

#include "BBox3D.h"
#include "Node.h"
#include "Vector3D.h"

#ifdef USE_TBB
#include <tbb/parallel_for.h>
#endif

#include <bitset>
#include <cmath>
#include <cstdint>
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
#ifdef USE_TBB
        tbb::parallel_for(tbb::blocked_range<uint32_t>(0, Node<Voxel, N>::maxChildrenCount() / bitLength), [&](const tbb::blocked_range<uint32_t>& r) {
            for (uint32_t i = r.begin(); i != r.end(); ++i) {
                voxels[i] = std::make_unique<std::bitset<bitLength>>(0xffff'ffff'ffff'ffff);
                for (uint32_t j = 0; j < bitLength; ++j) {
                    if (voxels[i]->test(j)) {
                        uint64_t index = this->calChildId(i * bitLength + j);
                        if (!bbox.isInside(Voxel::getCoordGL(index, halfRootEdgeLength))) {
                            voxels[i]->reset(j);
                        }
                    }
                }
            }
        });
#else
        for (uint32_t i = 0; i < Node<Voxel, N>::maxChildrenCount() / bitLength; ++i) {
            voxels[i] = std::make_unique<std::bitset<bitLength>>(0xffff'ffff'ffff'ffff);
            for (uint32_t j = 0; j < bitLength; ++j) {
                if (voxels[i]->test(j)) {
                    uint64_t index = this->calChildId(i * bitLength + j);
                    if (!bbox.isInside(Voxel::getCoordGL(index, halfRootEdgeLength))) {
                        voxels[i]->reset(j);
                    }
                }
            }
        }
#endif
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
#ifdef USE_TBB
        tbb::parallel_for(tbb::blocked_range<uint32_t>(0, Node<Voxel, N>::maxChildrenCount() / bitLength), [&](const tbb::blocked_range<uint32_t>& r) {
            for (uint32_t i = r.begin(); i != r.end(); ++i) {
                voxels[i] = std::make_unique<std::bitset<bitLength>>(0xffff'ffff'ffff'ffff);
            }
        });
#else
        for (uint32_t i = 0; i < Node<Voxel, N>::maxChildrenCount() / bitLength; ++i) {
            voxels[i] = std::make_unique<std::bitset<bitLength>>(0xffff'ffff'ffff'ffff);
        }
#endif
    }

    void subtract(const BBox3D<float>& bbox, const std::function<bool(const Vector3D<float>&)>& isInside, const uint32_t halfRootEdgeLength)
    {
        if (!bbox.intersects(this->getBBoxGL(halfRootEdgeLength))) {
            return;
        }
        if (!this->hasChildren) {
            this->subdivide();
        }
#ifdef USE_TBB
        tbb::parallel_for(tbb::blocked_range<uint32_t>(0, Node<Voxel, N>::maxChildrenCount() / bitLength), [&](const tbb::blocked_range<uint32_t>& r) {
            for (uint32_t i = r.begin(); i != r.end(); ++i) {
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
                        if (isInside(Voxel::getCoordGL(index, halfRootEdgeLength))) {
                            voxels[i]->reset(j);
                        }
                    }
                }
            }
        });
#else
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
                    if (isInside(Voxel::getCoordGL(index, halfRootEdgeLength))) {
                        voxels[i]->reset(j);
                    }
                }
            }
        }
#endif
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
    std::unique_ptr<std::bitset<bitLength>> voxels[Node<Voxel, N>::maxChildrenCount() / bitLength];
};
