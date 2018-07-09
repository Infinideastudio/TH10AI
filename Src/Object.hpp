#pragma once

#include "Vec2.hpp"

struct Object {
    Vec2d pos;
    Vec2d size;
    Vec2d delta;
    //double dx, dy;
    Object() {}
    Object(double x, double y, double w, double h) {
        pos.x = x;
        pos.y = y;
        size.x = w;
        size.y = h;
        delta.x = 0;
        delta.y = 0;
    }
    Object(double x, double y, double w, double h, double dx, double dy) {
        pos.x = x;
        pos.y = y;
        size.x = w;
        size.y = h;
        delta.x = dx;
        delta.y = dy;
    }
};

struct Laser : Object {
    double arc;
    Laser() {}
    Laser(double x, double y, double w, double h, double arc) : Object(x, y, w, h) {
        arc = arc;
    }
};

struct Player : Object {
    Player() {}
    Player(double x, double y, double w, double h) : Object(x, y, w, h) {
        size.x = 2;
        size.y = 2;
    }
};