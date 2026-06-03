
struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv       : TEXCOORD;
    float3 normal   : NORMAL;
};
Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);
cbuffer LightCB : register(b1)
{
    float4 light_dir;
    float4 light_color;
    float4 ambient;
}
float4 PSMain(PSInput input) : SV_TARGET
{
    float3 N = normalize(input.normal);
    float3 L = normalize(-light_dir.xyz);
    float ndotl = saturate(dot(N,L));
    float4 tex = g_texture.Sample(g_sampler, input.uv);
    float3 lit = tex.rgb * (light_color.rgb * ndotl + ambient.rgb);
    return  float4(lit, tex.a);
}