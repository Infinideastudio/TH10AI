#pragma once
#include <cstdint>

//生成Bmp图片，传递RGB值，传递图片像素大小，传递图片存储路径
void generateBmp(void* pData, int width, int height, const char* filename); 

struct Pixel {
    uint8_t b, g, r;
    constexpr Pixel(): b(0), g(0), r(0) {}
    constexpr Pixel(uint8_t r_, uint8_t g_, uint8_t b_) : b(b_), g(g_), r(r_) {}
};
