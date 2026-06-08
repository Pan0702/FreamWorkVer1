Texture2D g_sky : register(t0);
SamplerState g_sky_sampler : register(s0);

struct PSInput
{
    float4 pos : SV_POSITION;
    float3 dir : TEXCOORD0;
};

static const float PI = 3.14159265;

float4 PSMain(PSInput input) : SV_TARGET
{
    float3 d = normalize(input.dir);
    float u = atan2(d.z, d.x) / (PI * 2.0f) + 0.5f;
    float v = acos(clamp(d.y, -1.0f, 1.0f)) / PI;
    return g_sky.Sample(g_sky_sampler, float2(u, v));
}
