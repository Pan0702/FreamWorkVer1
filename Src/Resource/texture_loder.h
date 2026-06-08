#pragma once
#include <cstdint>
#include <vector>
/**
 * @brief LoadedImageのデータと処理をまとめる型。
 */
struct LoadedImage
{
    std::vector<uint8_t> pixels; // RGBA8
    uint32_t width;
    uint32_t height;
};
/**
 * @brief TextureLoaderのデータと処理をまとめる型。
 */
class TextureLoader
{
public:
    /**
     * @brief LoadFromFileを行う関数。
     * @param filename 読み込むファイル名。
     * @param image テクスチャ化する読み込み済み画像。
     * @return 条件を満たす場合は true。
     */
    static bool LoadFromFile(const wchar_t* filename, LoadedImage& image);
};