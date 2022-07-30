#pragma once

#include "Vector3D.h"

#include <ostream>

template <class T>
class BBox3D {
public:
    BBox3D() = default;
    virtual ~BBox3D() = default;

    virtual constexpr inline bool isAxisAligned() const = 0;
    virtual constexpr inline Vector3D<T> getCenter() const = 0;
    virtual constexpr inline Vector3D<T> getMin() const = 0;
    virtual constexpr inline Vector3D<T> getMax() const = 0;
    virtual constexpr inline Vector3D<T> getAxis(int index) const = 0;
    virtual constexpr inline T getHalfSize(int index) const = 0;
    virtual constexpr inline bool isEmpty() const = 0;
    virtual constexpr inline bool isValid() const = 0;
    virtual constexpr inline bool isInside(const Vector3D<T>& p) const = 0;
    virtual bool isInside(const BBox3D<T>& b) const = 0;
    virtual bool intersects(const BBox3D<T>& b) const = 0;

    friend std::ostream& operator<<(std::ostream& os, const BBox3D<T>& b) noexcept
    {
        b.print(os);
        return os;
    }

private:
    virtual void print(std::ostream& os) const = 0;
};
