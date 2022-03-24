#pragma once

#include "Node.h"

template <class T, uint32_t N>
class RootNode : public NodeWithChildren<T, N> {
public:
    RootNode()
    {
        this->id = 0;
        this->subdivide();
    }

    ~RootNode() = default;
};
