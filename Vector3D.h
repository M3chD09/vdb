#pragma once

#include <cmath>
#include <cstdint>
#include <ostream>

template <class T>
class Vector3D {
public:
    Vector3D()
        : x(0)
        , y(0)
        , z(0)
    {
    }
    Vector3D(T _x, T _y, T _z)
        : x(_x)
        , y(_y)
        , z(_z)
    {
    }
    Vector3D(const Vector3D& v)
        : x(v.x)
        , y(v.y)
        , z(v.z)
    {
    }
    ~Vector3D() { }

    T length() const
    {
        return std::sqrt(x * x + y * y + z * z);
    }
    T dot(const Vector3D& v) const
    {
        return x * v.x + y * v.y + z * v.z;
    }
    Vector3D<T> cross(const Vector3D& v) const
    {
        return Vector3D<T>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }
    T distanceToPoint(const Vector3D<T>& p) const
    {
        return (p - *this).length();
    }
    T distanceToLine(const Vector3D<T>& p, const Vector3D<T>& q)
    {
        Vector3D<T> v = q - p;
        Vector3D<T> w = *this - p;
        T c1 = w.dot(v);
        if (c1 <= 0)
            return distanceToPoint(p);
        T c2 = v.dot(v);
        if (c2 <= c1)
            return distanceToPoint(q);
        return (v * (c1 / c2)).length();
    }
    Vector3D<T> normalize() const
    {
        T len = length();
        return Vector3D<T>(x / len, y / len, z / len);
    }

    Vector3D& operator=(const Vector3D& v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }
    Vector3D operator+(const Vector3D& v) const
    {
        return Vector3D(x + v.x, y + v.y, z + v.z);
    }
    Vector3D operator-(const Vector3D& v) const
    {
        return Vector3D(x - v.x, y - v.y, z - v.z);
    }
    Vector3D operator+(T v) const
    {
        return Vector3D(x + v, y + v, z + v);
    }
    Vector3D operator-(T v) const
    {
        return Vector3D(x - v, y - v, z - v);
    }
    Vector3D operator*(T v) const
    {
        return Vector3D(x * v, y * v, z * v);
    }
    Vector3D operator/(T v) const
    {
        return Vector3D(x / v, y / v, z / v);
    }
    bool operator==(const Vector3D& v) const
    {
        return (x == v.x && y == v.y && z == v.z);
    }
    bool operator!=(const Vector3D& v) const
    {
        return (x != v.x || y != v.y || z != v.z);
    }
    bool operator<(const Vector3D& v) const
    {
        return (x < v.x && y < v.y && z < v.z);
    }
    bool operator>(const Vector3D& v) const
    {
        return (x > v.x && y > v.y && z > v.z);
    }
    bool operator<=(const Vector3D& v) const
    {
        return (x <= v.x && y <= v.y && z <= v.z);
    }
    bool operator>=(const Vector3D& v) const
    {
        return (x >= v.x && y >= v.y && z >= v.z);
    }
    bool operator<(T v) const
    {
        return (x < v && y < v && z < v);
    }
    bool operator>(T v) const
    {
        return (x > v && y > v && z > v);
    }
    bool operator<=(T v) const
    {
        return (x <= v && y <= v && z <= v);
    }
    bool operator>=(T v) const
    {
        return (x >= v && y >= v && z >= v);
    }
    Vector3D& operator+=(const Vector3D& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }
    Vector3D& operator-=(const Vector3D& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }
    Vector3D& operator*=(const Vector3D& v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        return *this;
    }
    Vector3D& operator/=(const Vector3D& v)
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        return *this;
    }
    Vector3D& operator+=(T s)
    {
        x += s;
        y += s;
        z += s;
        return *this;
    }
    Vector3D& operator-=(T s)
    {
        x -= s;
        y -= s;
        z -= s;
        return *this;
    }
    Vector3D& operator*=(T s)
    {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }
    Vector3D& operator/=(T s)
    {
        x /= s;
        y /= s;
        z /= s;
        return *this;
    }
    Vector3D operator-() const
    {
        return Vector3D(-x, -y, -z);
    }

    T x, y, z;
};

template <class T>
std::ostream& operator<<(std::ostream& os, const Vector3D<T>& v)
{
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}
