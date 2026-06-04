struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 world_pos : TEXCOORD1;
    float3 tangent   : TANGENT;
    float3 bitangent : BINORMAL;
};

Texture2D g_texture : register(t0);
Texture2D g_normal_map : register(t1);
SamplerState g_sampler : register(s0);

cbuffer LightCB : register(b1)
{
    float4 light_dir;
    float4 light_color;
    float4 ambient;
    float4 cam_pos;
}

cbuffer MaterialCB : register(b2)
{
    float4 base_color;
    int has_texture;
    float specular_intensity;
    float specular_power;
    int has_normal_map;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float3 N = normalize(input.normal);
    if (has_normal_map != 0)
    {
        float3 tn = g_normal_map.Sample(g_sampler, input.uv).xyz * 2.0 - 1.0;
        float3 T = normalize(input.tangent);
        float3 B = normalize(input.bitangent);
        float3x3 TBN = float3x3(T, B, N);
        N = normalize(mul(tn, TBN));
    }
    float3 L = normalize(-light_dir.xyz);
    float3 V = normalize(cam_pos.xyz - input.world_pos.xyz);
    float3 H = normalize(L + V);
    float ndotl = saturate(dot(N, L));
    float spec = pow(saturate(dot(N, H)), specular_power) * specular_intensity * ndotl;
    float4 albedo = base_color;
    if (has_texture != 0)
    {
        albedo *= g_texture.Sample(g_sampler, input.uv);
    }
    float3 lit = albedo.rgb * (light_color.rgb * ndotl + ambient.rgb) + light_color.rgb * spec;
    return float4(lit, albedo.a);
}
