#pragma once

#include "Vector3D.h"

#include <algorithm>
#include <ostream>

template <class T>
class BBox3D {
public:
    BBox3D() = default;
    ~BBox3D() = default;

    BBox3D(const Vector3D<T>& min, const Vector3D<T>& max)
        : min(min)
        , max(max)
    {
    }
    BBox3D(const Vector3D<T>& center, const T halfSize)
        : min(center - Vector3D<T>(halfSize, halfSize, halfSize))
        , max(center + Vector3D<T>(halfSize, halfSize, halfSize))
    {
    }
    BBox3D(const Vector3D<T>& center, const T halfSizeX, const T halfSizeY, const T halfSizeZ)
        : min(center - Vector3D<T>(halfSizeX, halfSizeY, halfSizeZ))
        , max(center + Vector3D<T>(halfSizeX, halfSizeY, halfSizeZ))
    {
    }

    [[nodiscard]] bool IsEmpty() const
    {
        return min == max;
    }

    [[nodiscard]] bool IsValid() const
    {
        return min < max;
    }

    bool IsInside(const Vector3D<T>& p) const
    {
        return min <= p && p <= max;
    }

    bool Intersects(const BBox3D& b) const
    {
        return min <= b.max && b.min <= max;
    }

    bool Intersects(const Vector3D<T>& p, const Vector3D<T>& q) const
    {
        return min <= p && p <= max && min <= q && q <= max;
    }

    bool Intersects(const Vector3D<T>& p, const Vector3D<T>& q, Vector3D<T>& intersection) const
    {
        if (min <= p && p <= max && min <= q && q <= max) {
            intersection = std::max(p, q);
            return true;
        }
        return false;
    }

    bool operator==(const BBox3D& b) const
    {
        return min == b.min && max == b.max;
    }

    Vector3D<T> min, max;
};

template <class T>
std::ostream& operator<<(std::ostream& os, const BBox3D<T>& b)
{
    os << "BBox3D(" << b.min << ", " << b.max << ")";
    return os;
}
