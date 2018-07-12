#pragma once
#include <cstdint>
#include "Vec2.hpp"

void generateBmp(void* pData, const Vec2i& size, const char* filename);

struct Pixel {
    uint8_t b, g, r;
    constexpr Pixel() noexcept : b(0), g(0), r(0) {}
    constexpr Pixel(const uint8_t iR, const uint8_t iG, const uint8_t iB) noexcept : b(iB), g(iG), r(iR) {}
};
