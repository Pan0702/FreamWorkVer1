#pragma once
#include "../Resource/texture_loder.h"
class IBLBaker
{
public:
    static LoadedImage BakeIrradianceMap(const LoadedImage& sky_srgb);
};
