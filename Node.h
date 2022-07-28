#pragma once

#include "AABB3D.h"
#include "BBox3D.h"
#include "Morton.h"
#include "Vector3D.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <execution>
#include <functional>
#include <memory>
#include <ranges>
#include <vector>

template <class T, uint32_t N>
class Node {
public:
    Node() = default;
    virtual ~Node() = default;

    static constexpr uint32_t edgeLength() { return (1 << N) * T::edgeLength(); }
    static constexpr uint32_t halfEdgeLength() { return edgeLength() >> 1; }
    static constexpr uint32_t maxChildrenCount() { return 1 << (N * 3); }
    static constexpr uint32_t sumN() { return N + T::sumN(); }

    float edgeLengthGL(const uint32_t halfRootEdgeLength)
    {
        return (float)edgeLength() / (float)halfRootEdgeLength;
    }

    Vector3D<uint32_t> getCoord()
    {
        return Morton::decode((uint64_t)id << (sumN() * 3)) + halfEdgeLength();
    }

    Vector3D<float> getCoordGL(const uint32_t halfRootEdgeLength)
    {
        Vector3D<uint32_t> coord = getCoord();
        return {
            (float)coord.x / (float)halfRootEdgeLength - 1.0f,
            (float)coord.y / (float)halfRootEdgeLength - 1.0f,
            (float)coord.z / (float)halfRootEdgeLength - 1.0f
        };
    }

    AABB3D<float> getBBoxGL(const uint32_t halfRootEdgeLength)
    {
        return AABB3D<float>(getCoordGL(halfRootEdgeLength), edgeLengthGL(halfRootEdgeLength) / 2.0f);
    }

    uint64_t calChildId(uint32_t i)
    {
        return (uint64_t)i | (id << (3 * N));
    }

    bool isAllVertexInside(const std::function<bool(const Vector3D<float>&)>& isInside, const uint32_t halfRootEdgeLength)
    {
        for (int8_t i = -1; i <= 1; i += 2) {
            for (int8_t j = -1; j <= 1; j += 2) {
                for (int8_t k = -1; k <= 1; k += 2) {
                    if (!isInside(
                            getCoordGL(halfRootEdgeLength)
                            + Vector3D<float>(i, j, k) * edgeLengthGL(halfRootEdgeLength) / 2.0f)) {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    uint64_t id = 0;
    bool isActive = true;
    bool hasChildren = false;
};

template <class T, uint32_t N>
class NodeWithChildren : public Node<T, N> {
public:
    NodeWithChildren() = default;
    virtual ~NodeWithChildren() = default;

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

        std::for_each(std::execution::par, this->children.begin(), this->children.end(), [&](auto& c) {
            c->initialize(bbox, halfRootEdgeLength);
        });
    }

    void reset()
    {
        for (auto& child : children) {
            child.reset();
        }
        this->isActive = true;
        this->hasChildren = false;
    }

    void subdivide()
    {
        this->hasChildren = true;
        std::for_each(std::execution::par, this->children.begin(), this->children.end(), [&](auto& c) {
            uint32_t i = &c - this->children.data();
            c = std::make_unique<T>();
            c->id = this->calChildId(i);
            c->isActive = true;
            c->hasChildren = false;
        });
    }

    void subtract(const BBox3D<float>& bbox, const std::function<bool(const Vector3D<float>&)>& isInside, const uint32_t halfRootEdgeLength)
    {
        if (!bbox.intersects(this->getBBoxGL(halfRootEdgeLength))) {
            return;
        }
        if (this->isAllVertexInside(isInside, halfRootEdgeLength)) {
            this->isActive = false;
            return;
        }
        if (!this->hasChildren) {
            this->subdivide();
        }
        std::for_each(std::execution::par, this->children.begin(), this->children.end(), [&](auto& c) {
            if (c != nullptr && c->isActive) {
                c->subtract(bbox, isInside, halfRootEdgeLength);
            }
        });
    }

    void calculateVoxels(std::vector<Vector3D<float>>& coords, std::vector<float>& sizes, const uint32_t halfRootEdgeLength)
    {
        if (this->hasChildren) {
            for (uint32_t i = 0; i < Node<T, N>::maxChildrenCount(); ++i) {
                if (children[i] != nullptr) {
                    if (children[i]->isActive) {
                        children[i]->calculateVoxels(coords, sizes, halfRootEdgeLength);
                    } else {
                        children[i].reset();
                    }
                }
            }
        } else {
            coords.push_back(Node<T, N>::getCoordGL(halfRootEdgeLength));
            sizes.push_back(Node<T, N>::edgeLengthGL(halfRootEdgeLength));
        }
    }

    std::array<std::unique_ptr<T>, Node<T, N>::maxChildrenCount()> children = { nullptr };
};
