#pragma once
#include "Vec2.hpp"

class Obb2D {
public:
    Obb2D(const Vec2d& center, const Vec2d& size, const double angle) noexcept {
        const auto x = Vec2d(cos(angle), sin(angle)) * size.x / 2, y = Vec2d(-sin(angle), cos(angle)) * size.y / 2;
        corner[0] = center - x - y;
        corner[1] = center + x - y;
        corner[2] = center + x + y;
        corner[3] = center - x + y;
        computeAxes();
    }

    void moveTo(const Vec2d& center) noexcept {
        const auto centroid = (corner[0] + corner[1] + corner[2] + corner[3]) / 4;
        const auto translation = center - centroid;
        for (auto c = 0; c < 4; ++c) { corner[c] += translation; }
        computeAxes();
    }

    bool overlaps(const Obb2D& other) const noexcept { return overlaps1Way(other) && other.overlaps1Way(*this); }
private:
    Vec2d corner[4]{}, axis[2]{};
    double origin[2]{};

    bool overlaps1Way(const Obb2D& other) const noexcept {
        for (auto a = 0; a < 2; ++a) {
            auto t = other.corner[0].dot(axis[a]);
            auto tMin = t, tMax = t;
            for (auto c = 1; c < 4; ++c) {
                t = other.corner[c].dot(axis[a]);
                if (t < tMin) tMin = t;
                else if (t > tMax) tMax = t;
            }
            if ((tMin > 1 + origin[a]) || (tMax < origin[a])) return false;
        }
        return true;
    }

    void computeAxes() noexcept {
        axis[0] = corner[1] - corner[0];
        axis[1] = corner[3] - corner[0];
        for (auto a = 0; a < 2; ++a) {
            axis[a] /= axis[a].lengthSqr();
            origin[a] = corner[0].dot(axis[a]);
        }
    }

};
