#include "bmpCreater.hpp"
#include <fstream>

#pragma pack(push,2)

//位图文件头文件定义
//其中不包括文件类型信息（由于结构体的内存结构决定，要是加了的话将不能正确的读取文件信息）
struct BMPFILEHEADER_T {
    uint16_t bfType = 0X4d42; //文件类型，必须是0x424D,即字符“BM”
    uint32_t bfSize; //文件大小
    uint16_t bfReserved1 = 0; //保留字
    uint16_t bfReserved2 = 0; //保留字
    uint32_t bfOffBits; //从文件头到实际位图数据的偏移字节数
};

struct BMPINFOHEADER_T {
    uint32_t biSize; //信息头大小
    int32_t biWidth; //图像宽度
    int32_t biHeight; //图像高度
    uint16_t biPlanes = 1; //位平面数，必须为1
    uint16_t biBitCount = 24; //每像素位数
    uint32_t biCompression = 0; //压缩类型
    uint32_t biSizeImage; //压缩图像大小字节数
    int32_t biXPelsPerMeter = 0; //水平分辨率
    int32_t biYPelsPerMeter = 0; //垂直分辨率
    uint32_t biClrUsed = 0; //位图实际用到的色彩数
    uint32_t biClrImportant = 0; //本位图中重要的色彩数
}; //位图信息头定义

static_assert(alignof(BMPINFOHEADER_T) == 2  && alignof(BMPFILEHEADER_T) == 2, "pragma pack failure");

void generateBmp(void* pData, int width, int height, const char* filename) {
    int size = width * height * 3; // 每个像素点3个字节
    // 位图第一部分，文件信息
    BMPFILEHEADER_T bfh;
    bfh.bfSize = size // data size
        + sizeof(BMPFILEHEADER_T) // first section size
        + sizeof(BMPINFOHEADER_T) // second section size
        ;
    bfh.bfOffBits = bfh.bfSize - size;
    // 位图第二部分，数据信息
    BMPINFOHEADER_T bih;
    bih.biSize = sizeof(BMPINFOHEADER_T);
    bih.biWidth = width;
    bih.biHeight = height;
    bih.biSizeImage = size;
    if (std::ofstream file(filename, std::ios::binary); file.good()) {
        file.write(reinterpret_cast<const char*>(&bfh), sizeof(BMPFILEHEADER_T));
        file.write(reinterpret_cast<const char*>(&bih), sizeof(BMPINFOHEADER_T));
        file.write(reinterpret_cast<const char*>(pData), size);
    }
}

#pragma pack(pop)
