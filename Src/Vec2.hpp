#pragma once

#include <cmath>

template <typename T>
struct Vec2 {
    T x, y;

    Vec2() = default;
    constexpr Vec2(T xx, T yy) noexcept : x(xx), y(yy) {}

    Vec2& operator+=(const Vec2& rhs) noexcept {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    Vec2& operator-=(const Vec2& rhs) noexcept {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    Vec2& operator*=(T value) noexcept {
        x *= value;
        y *= value;
        return *this;
    }

    Vec2& operator/=(T value) noexcept {
        x /= value;
        y /= value;
        return *this;
    }

    constexpr Vec2 operator+(const Vec2& rhs) const noexcept { return Vec2(x + rhs.x, y + rhs.y); }
    constexpr Vec2 operator-(const Vec2& rhs) const noexcept { return Vec2(x - rhs.x, y - rhs.y); }
    constexpr Vec2 operator*(T value) const noexcept { return Vec2(x * value, y * value); }
    constexpr Vec2 operator/(T value) const noexcept { return Vec2(x / value, y / value); }
    constexpr bool operator ==(const Vec2& rhs) const noexcept { return (x == rhs.x && y == rhs.y); }
    constexpr auto lengthSqr() const noexcept { return x * x + y * y; }
    auto length() const noexcept { return std::sqrt(lengthSqr()); }
    auto norm() const noexcept { return length(); }
    Vec2 unit() const noexcept { return (*this) / norm(); }
    constexpr T dot(const Vec2& rhs) const noexcept { return x * rhs.x + y * rhs.y; }
};

template <class T>
constexpr auto distanceSqr(const Vec2<T>& lhs, const Vec2<T>& rhs) noexcept { return (rhs - lhs).lengthSqr(); }

template <class T>
auto distance(const Vec2<T>& lhs, const Vec2<T>& rhs) noexcept { return (rhs - lhs).length(); }

using Vec2i = Vec2<int>;
using Vec2l = Vec2<long>;
using Vec2f = Vec2<float>;
using Vec2d = Vec2<double>;

constexpr Vec2i moveDelta[4] = {Vec2i(-1, 0), Vec2i(1, 0), Vec2i(0, -1), Vec2i(0, 1)};
