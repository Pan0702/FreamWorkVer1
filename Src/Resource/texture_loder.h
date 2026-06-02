#pragma once
#include <cstdint>
#include <vector>
struct LoadedImage
{
    std::vector<uint8_t> pixels; // RGBA8
    uint32_t width;
    uint32_t height;
};

class TextureLoader
{
public:
    static bool LoadFromFile(const wchar_t* filename, LoadedImage& image);
};