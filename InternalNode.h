#pragma once

#include "Node.h"

template <class T, uint32_t N>
class InternalNode : public NodeWithChildren<T, N> {
public:
    InternalNode() { }
    ~InternalNode() { }
};
