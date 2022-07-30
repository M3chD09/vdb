#pragma once

#include <cmath>
#include <cstdint>
#include <ostream>

template <class T>
class Vector3D {
public:
    constexpr inline Vector3D() noexcept
        : x(0)
        , y(0)
        , z(0)
    {
    }
    constexpr inline Vector3D(T _x, T _y, T _z) noexcept
        : x(_x)
        , y(_y)
        , z(_z)
    {
    }
    constexpr inline Vector3D(const Vector3D& v) noexcept
        : x(v.x)
        , y(v.y)
        , z(v.z)
    {
    }
    ~Vector3D() = default;

    constexpr inline bool isZero() const noexcept
    {
        return x == 0 && y == 0 && z == 0;
    }
    constexpr inline T length() const noexcept
    {
        return (T)std::sqrt(x * x + y * y + z * z);
    }
    constexpr inline T dot(const Vector3D& v) const noexcept
    {
        return x * v.x + y * v.y + z * v.z;
    }
    constexpr inline Vector3D<T> cross(const Vector3D& v) const noexcept
    {
        return Vector3D<T>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }
    constexpr inline Vector3D<T> normalize() const noexcept
    {
        return *this / length();
    }
    constexpr inline Vector3D<T> rotate(const Vector3D<T>& axis, const T angle) const noexcept
    {
        const T c = (T)std::cos(angle);
        const T s = (T)std::sin(angle);
        const T t = 1 - c;
        const Vector3D<T> n = axis.normalize();
        return Vector3D<T>(
            c * x + t * (n.x * n.x * x + n.y * n.x * y + n.z * n.x * z) + s * (-n.z * y + n.y * z),
            c * y + t * (n.x * n.y * x + n.y * n.y * y + n.y * n.z * z) + s * (n.z * x - n.x * z),
            c * z + t * (n.x * n.z * x + n.y * n.z * y + n.z * n.z * z) + s * (-n.y * x + n.x * y));
    }
    constexpr inline T angleToLine(const Vector3D& v) const noexcept
    {
        return (T)std::acos(dot(v) / (length() * v.length()));
    }
    constexpr inline T distanceToPoint(const Vector3D<T>& p) const noexcept
    {
        return (*this - p).length();
    }
    constexpr inline T distanceToLine(const Vector3D<T>& p, const Vector3D<T>& q) const noexcept
    {
        Vector3D<T> a = *this - p;
        Vector3D<T> b = (q - p).normalize();
        Vector3D<T> c = b * a.projectionLength(b);
        return (a - c).length();
    }
    constexpr inline T projectionLength(const Vector3D<T>& v) const noexcept
    {
        return std::abs(dot(v) / v.length());
    }

    constexpr inline Vector3D& operator=(const Vector3D& v) noexcept
    {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }
    constexpr inline Vector3D operator+(const Vector3D& v) const noexcept
    {
        return Vector3D(x + v.x, y + v.y, z + v.z);
    }
    constexpr inline Vector3D operator-(const Vector3D& v) const noexcept
    {
        return Vector3D(x - v.x, y - v.y, z - v.z);
    }
    constexpr inline Vector3D operator+(T v) const noexcept
    {
        return Vector3D(x + v, y + v, z + v);
    }
    constexpr inline Vector3D operator-(T v) const noexcept
    {
        return Vector3D(x - v, y - v, z - v);
    }
    constexpr inline Vector3D operator*(T v) const noexcept
    {
        return Vector3D(x * v, y * v, z * v);
    }
    constexpr inline Vector3D operator/(T v) const noexcept
    {
        return Vector3D(x / v, y / v, z / v);
    }
    constexpr inline bool operator==(T v) const noexcept
    {
        return x == v && y == v && z == v;
    }
    constexpr inline bool operator!=(T v) const noexcept
    {
        return x != v || y != v || z != v;
    }
    constexpr inline bool operator==(const Vector3D& v) const noexcept
    {
        return (x == v.x && y == v.y && z == v.z);
    }
    constexpr inline bool operator!=(const Vector3D& v) const noexcept
    {
        return (x != v.x || y != v.y || z != v.z);
    }
    constexpr inline bool operator<(const Vector3D& v) const noexcept
    {
        return (x < v.x && y < v.y && z < v.z);
    }
    constexpr inline bool operator>(const Vector3D& v) const noexcept
    {
        return (x > v.x && y > v.y && z > v.z);
    }
    constexpr inline bool operator<=(const Vector3D& v) const noexcept
    {
        return (x <= v.x && y <= v.y && z <= v.z);
    }
    constexpr inline bool operator>=(const Vector3D& v) const noexcept
    {
        return (x >= v.x && y >= v.y && z >= v.z);
    }
    constexpr inline bool operator<(T v) const noexcept
    {
        return (x < v && y < v && z < v);
    }
    constexpr inline bool operator>(T v) const noexcept
    {
        return (x > v && y > v && z > v);
    }
    constexpr inline bool operator<=(T v) const noexcept
    {
        return (x <= v && y <= v && z <= v);
    }
    constexpr inline bool operator>=(T v) const noexcept
    {
        return (x >= v && y >= v && z >= v);
    }
    constexpr inline Vector3D& operator+=(const Vector3D& v) noexcept
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }
    constexpr inline Vector3D& operator-=(const Vector3D& v) noexcept
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }
    constexpr inline Vector3D& operator*=(const Vector3D& v) noexcept
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        return *this;
    }
    constexpr inline Vector3D& operator/=(const Vector3D& v) noexcept
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        return *this;
    }
    constexpr inline Vector3D& operator+=(T s) noexcept
    {
        x += s;
        y += s;
        z += s;
        return *this;
    }
    constexpr inline Vector3D& operator-=(T s) noexcept
    {
        x -= s;
        y -= s;
        z -= s;
        return *this;
    }
    constexpr inline Vector3D& operator*=(T s) noexcept
    {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }
    constexpr inline Vector3D& operator/=(T s) noexcept
    {
        x /= s;
        y /= s;
        z /= s;
        return *this;
    }
    constexpr inline Vector3D operator-() const noexcept
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
