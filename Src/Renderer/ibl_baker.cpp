#include "ibl_baker.h"
#include "../Core/Math/my_math.h"
#include <algorithm>

namespace
{
    void TexelToDir(int x, int y, int width, int height, Vec3& d, float& sin_theta)
    {
        const float u = (static_cast<float>(x) + 0.5f) / static_cast<float>(width);
        const float v = (static_cast<float>(y) + 0.5f) / static_cast<float>(height);
        const float phi = (u - 0.5f) * 2.0f * kPI;
        float theta = kPI * v;
        sin_theta = std::sinf(theta);
        d.x = std::cos(phi) * sin_theta;
        d.y = std::cos(theta);             
        d.z = std::sin(phi) * sin_theta;  
    }
}

LoadedImage IBLBaker::BakeIrradianceMap(const LoadedImage& sky_srgb)
{
    constexpr int src_w = 64;
    constexpr int src_h = 32;
    constexpr float kMaxRGB = 255.0f;
    constexpr float kGamma = 2.2f;
    std::vector<Vec3> src(src_h * src_w);
    for (int y = 0; y < src_h; ++y)
    {
        for (int x = 0; x < src_w; ++x)
        {
            const int sx = x * static_cast<int>(sky_srgb.width) / src_w;
            const int sy = y * static_cast<int>(sky_srgb.height) / src_h;
            const size_t a = static_cast<size_t>(sy * sky_srgb.width + sx) * 4;
            const uint8* p = &sky_srgb.pixels[a];
            
            src[y * src_w + x].x = powf(static_cast<float>(p[0]) / kMaxRGB, kGamma);
            src[y * src_w + x].y = powf(static_cast<float>(p[1]) / kMaxRGB, kGamma);
            src[y * src_w + x].z = powf(static_cast<float>(p[2]) / kMaxRGB, kGamma);
        }
    }

    constexpr int dst_w = 32;
    constexpr int dst_h = 16;
    LoadedImage result;
    result.width = dst_w;
    result.height = dst_h;
    constexpr size_t kB = static_cast<size_t>(dst_w * dst_h) * 4;
    result.pixels.resize(kB);

    for (int oy = 0; oy < dst_h; ++oy)
    {
        for (int ox = 0; ox < dst_w; ++ox)
        {
            Vec3 n = {};
            float n_sin;
            TexelToDir(ox, oy, dst_w, dst_h, n, n_sin);

            Vec3 sum = {};
            float weight_sum = 0.0f;
            for (int iy = 0; iy < src_h; ++iy)
            {
                for (int ix = 0; ix < src_w; ++ix)
                {
                    Vec3 l;
                    float l_sin;
                    TexelToDir(ix, iy, src_w, src_h, l, l_sin);
                    const float ndotl = n.x * l.x + n.y * l.y + n.z * l.z;
                    if (ndotl <= 0.0f)
                    {
                        continue;
                    }
                    const float weight = ndotl * l_sin;
                    const Vec3 c = src[iy * src_w + ix];
                    sum.x += c.x * weight;
                    sum.y += c.y * weight;
                    sum.z += c.z * weight;
                    weight_sum += weight;
                }
            }

            Vec3 irr = {};
            if (weight_sum > 0.0f)
            {
                irr = sum / weight_sum;
            }
            const size_t c = static_cast<size_t>(oy * dst_w + ox) * 4;
            uint8* out = &result.pixels[c];
            constexpr float kInvGamma = 1.0f/kGamma;
            out[0] = static_cast<uint8_t>(std::powf(std::clamp(irr.x, 0.0f, 1.0f),kInvGamma)* kMaxRGB + 0.5f);
            out[1] = static_cast<uint8_t>(std::powf(std::clamp(irr.y, 0.0f, 1.0f),kInvGamma) * kMaxRGB + 0.5f);
            out[2] = static_cast<uint8_t>(std::powf(std::clamp(irr.z, 0.0f, 1.0f),kInvGamma) * kMaxRGB + 0.5f);
            out[3] = static_cast<uint8>(kMaxRGB);
        }
    }
    return result;
}
