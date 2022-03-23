#pragma once

#include "BBox3D.h"
#include "Morton.h"
#include "Vector3D.h"

#ifdef USE_TBB
#include <tbb/parallel_for.h>
#endif

#include <cmath>
#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

template <class T, uint32_t N>
class Node {
public:
    Node() {};
    virtual ~Node() {};

    static constexpr uint32_t edgeLength() { return (1 << N) * T::edgeLength(); };
    static constexpr uint32_t halfEdgeLength() { return edgeLength() >> 1; };
    static constexpr uint32_t maxChildrenCount() { return 1 << (N * 3); };
    static constexpr uint32_t sumN() { return N + T::sumN(); };

    float edgeLengthGL(const uint32_t halfRootEdgeLength)
    {
        return (float)edgeLength() / (float)halfRootEdgeLength;
    };

    Vector3D<uint32_t> getCoord(void)
    {
        return Morton::decode((uint64_t)id << (sumN() * 3)) + halfEdgeLength();
    }

    Vector3D<float> getCoordGL(const uint32_t halfRootEdgeLength)
    {
        Vector3D<uint32_t> coord = getCoord();
        return Vector3D<float>((float)coord.x / (float)halfRootEdgeLength - 1.0f,
            (float)coord.y / (float)halfRootEdgeLength - 1.0f,
            (float)coord.z / (float)halfRootEdgeLength - 1.0f);
    }

    BBox3D<uint32_t> getBBox(void)
    {
        return BBox3D<uint32_t>(getCoord(), halfEdgeLength());
    }

    BBox3D<float> getBBoxGL(const uint32_t halfRootEdgeLength)
    {
        return BBox3D<float>(getCoordGL(halfRootEdgeLength), edgeLengthGL(halfRootEdgeLength) / 2.0f);
    }

    uint64_t calChildId(uint32_t i)
    {
        return (uint64_t)i | (id << (3 * N));
    };

    bool isAllVertexInside(std::function<bool(const Vector3D<float>&)> isInside, const uint32_t halfRootEdgeLength)
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

    uint64_t id;
    bool isActive = true;
    bool hasChildren = false;
};

template <class T, uint32_t N>
class NodeWithChildren : public Node<T, N> {
public:
    NodeWithChildren() {};
    ~NodeWithChildren() {};

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
#if USE_TBB
        tbb::parallel_for(tbb::blocked_range<uint32_t>(0, Node<T, N>::maxChildrenCount()), [&](const tbb::blocked_range<uint32_t>& r) {
            for (uint32_t i = r.begin(); i != r.end(); ++i) {
                this->children[i] = std::make_unique<T>();
                this->children[i]->id = this->calChildId(i);
                this->children[i]->isActive = true;
                this->children[i]->hasChildren = false;
            }
        });
#else
        for (uint32_t i = 0; i < Node<T, N>::maxChildrenCount(); i++) {
            this->children[i] = std::make_unique<T>();
            this->children[i]->id = this->calChildId(i);
            this->children[i]->isActive = true;
            this->children[i]->hasChildren = false;
        }

#endif
    }

    void subtract(const BBox3D<float>& bbox, std::function<bool(const Vector3D<float>&)> isInside, const uint32_t halfRootEdgeLength)
    {
        if (!bbox.Intersects(this->getBBoxGL(halfRootEdgeLength))) {
            return;
        }
        if (this->isAllVertexInside(isInside, halfRootEdgeLength)) {
            this->isActive = false;
            return;
        }
        if (!this->hasChildren) {
            this->subdivide();
        }
#if USE_TBB
        tbb::parallel_for(tbb::blocked_range<uint32_t>(0, Node<T, N>::maxChildrenCount()), [&](const tbb::blocked_range<uint32_t>& r) {
            for (uint32_t i = r.begin(); i != r.end(); ++i) {
                if (this->children[i] != nullptr && this->children[i]->isActive) {
                    this->children[i]->subtract(bbox, isInside, halfRootEdgeLength);
                }
            }
        });
#else
        for (uint32_t i = 0; i < Node<T, N>::maxChildrenCount(); i++) {
            if (this->children[i] != nullptr && this->children[i]->isActive) {
                this->children[i]->subtract(bbox, isInside, halfRootEdgeLength);
            }
        }
#endif
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

    std::unique_ptr<T> children[Node<T, N>::maxChildrenCount()] = { nullptr };
};
