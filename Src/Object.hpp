#pragma once

#include "Vec2.hpp"

struct Object {
    Vec2d pos;
    Vec2d size;
    Vec2d delta;
    Object() = default;
    constexpr Object(const Vec2d& iPos, const Vec2d& iSize) noexcept : pos(iPos), size(iSize), delta(0, 0) {}

    constexpr Object(const Vec2d& iPos, const Vec2d& iSize, const Vec2d& iDelta) noexcept :
        pos(iPos), size(iSize), delta(iDelta) {}
};

struct Laser : Object {
    double arc;
    Laser() noexcept = default;
    constexpr Laser(const Vec2d& pos, const Vec2d& size, const double arcIn) noexcept : Object(pos, size), arc(arcIn) {}
};

struct Player : Object {
    Player() noexcept = default;
    constexpr Player(const Vec2d& pos, const Vec2d& size) noexcept : Object(pos, size) {}
};
