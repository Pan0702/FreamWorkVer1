
struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv       : TEXCOORD;
    float3 normal   : NORMAL;
};
Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);
float4 PSMain(PSInput input) : SV_TARGET
{
    float4 color = g_texture.Sample(g_sampler, input.uv);
    return color;
}