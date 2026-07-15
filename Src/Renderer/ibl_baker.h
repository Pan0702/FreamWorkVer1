#pragma once
#include "../Resource/texture_loder.h"
/**
 * @brief 空画像からイメージベースドライティング用テクスチャを生成する。
 */
class IBLBaker
{
public:
    /**
     * @brief sRGBの空画像から拡散反射用のイラディアンスマップを生成する。
     * @param sky_srgb sRGBカラースペースの入力空画像。
     * @return 生成したイラディアンス画像。
     */
    static LoadedImage BakeIrradianceMap(const LoadedImage& sky_srgb);
};
