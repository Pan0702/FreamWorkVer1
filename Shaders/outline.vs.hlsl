struct VSInput
{
    float3 position : POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD;
    float3 tangent  : TANGENT;
    float3 bitangent : BINORMAL;
};

cbuffer SceneCB : register(b0)
{
    float4x4 wvp;
    float4x4 world;
};

static const float kWidth = 0.02;

float4 VSMain(VSInput input) : SV_POSITION
{
    return mul(float4(input.position + input.normal * kWidth, 1.0), wvp);
}