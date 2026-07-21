#pragma once
#include <cstdint>
#include "../Core/Math/my_math.h"

enum class StaticRootParam : uint32
{
    kObjectCB = 0, // b0
    kLightCB, // b1
    kMaterialCB, // b2
    kDiffuse, // t0
    kNormal, // t1
    kShadow, // t2
    kIrradiance, // t3
    kSpecular, // t4
    kHeight, // t5
    kInstanceMatrics,
    kCount
};

enum class SkinnedRootParam : uint32
{
    kObjectCB = 0, // b0 (VS)
    kBoneCB, // b1 (VS)
    kLightCB, // b1 (PS) VS の b1 とは visibility が違うため別スロット
    kMaterialCB, // b2 (PS)
    kDiffuse, // t0
    kNormal, // t1
    kShadow, // t2
    kIrradiance, // t3
    kSpecular, // t4
    kHeight, // t5
    kCount
};

enum class ShadowRootParam : uint32
{
    kObjectCB = 0,
    kInstanceMatrices,
    kCount,
};

enum class SkinnedShadowParam : uint32
{
    kObjectCB = 0,
    kBoneCB,
    kCount,
};

template <typename T>
constexpr UINT ToIndex(T param)
{
    return static_cast<UINT>(param);
}
