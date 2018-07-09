#ifndef VEC2_HPP
#define VEC2_HPP

#include<cmath>

template<typename T>
class Vec2 {
public:
    T x, y;
    Vec2() {}
    ~Vec2() {}
    Vec2(T xx, T yy) : x(xx), y(yy) {}
    Vec2 &operator+=(const Vec2 &rhs) {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }
    Vec2 &operator-=(const Vec2 &rhs) {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }
    Vec2 &operator*=(T value) {
        x *= value;
        y *= value;
        return *this;
    }
    Vec2 &operator/=(T value) {
        x /= value;
        y /= value;
        return *this;
    }
    Vec2 operator+(const Vec2 &rhs) const {
        return Vec2(x + rhs.x, y + rhs.y);
    }
    Vec2 operator-(const Vec2 &rhs) const {
        return Vec2(x - rhs.x, y - rhs.y);
    }
    Vec2 operator*(T value) const {
        return Vec2(x * value, y * value);
    }
    Vec2 operator/(T value) const {
        return Vec2(x / value, y / value);
    }
    bool operator==(const Vec2 &rhs) const {
        return (x == rhs.x && y == rhs.y);
    }
    T norm() const {
        return sqrt(x * x + y * y);
    }
    Vec2 unit() const {
        return (*this) / norm();
    }
    T dot(const Vec2 &rhs) {
        return x * rhs.x + y * rhs.y;
    }
};

using Vec2i = Vec2<int>;
using Vec2l = Vec2<long>;
using Vec2f = Vec2<float>;
using Vec2d = Vec2<double>;
const Vec2i moveDelta[4] = {Vec2i(-1, 0), Vec2i(1, 0), Vec2i(0, -1), Vec2i(0, 1)};
#endif
