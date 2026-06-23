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
     * @brief ファイルや外部データを読み込んで内部表現へ変換する。
     * @param filename 読み書きするファイルパス。
     * @param image image に設定する値。
     * @return 指定データの読み込みが完了した場合は true。
     */
    static bool LoadFromFile(const wchar_t* filename, LoadedImage& image);
};
