#pragma once

#include "Vec2.hpp"

struct Object {
    Vec2d pos;
    Vec2d size;
    Vec2d delta;
    Object() = default;
    constexpr Object(double x, double y, double w, double h) : pos(x, y), size(w, h), delta(0, 0) {}
    constexpr Object(double x, double y, double w, double h, double dx, double dy): pos(x, y), size(w, h), delta(dx, dy) {}
};

struct Laser : Object {
    double arc;
    Laser() = default;
    constexpr Laser(double x, double y, double w, double h, double arcIn) : Object(x, y, w, h), arc(arcIn) {}
};

struct Player : Object {
    Player() = default;
    constexpr Player(double x, double y, double w, double h) : Object(x, y, 4.0, 4.0) {}
};
