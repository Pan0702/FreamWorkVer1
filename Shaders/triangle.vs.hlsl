
struct VSInput
{
    float3 position : POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD;
    float3 tangent  : TANGENT;
    float3 bitangent : BINORMAL;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv       : TEXCOORD;
    float3 normal   : NORMAL;
    float3 world_pos : TEXCOORD1;
    float3 tangent   : TANGENT;
    float3 bitangent : BINORMAL;
};
cbuffer SceneCB : register(b0)
{
    float4x4 wvp;
    float4x4 world;
};
PSInput VSMain(VSInput input)
{
    PSInput output;

    output.position = mul(float4(input.position, 1.0),wvp);
    output.normal = normalize(mul(float4(input.normal, 0.0),world).xyz);
    output.world_pos = mul(float4(input.position, 1.0),world).xyz;
    output.tangent = normalize(mul(float4(input.tangent, 0.0),world).xyz);
    output.bitangent = normalize(mul(float4(input.bitangent, 0.0),world).xyz);
    output.uv = input.uv;
    return output;
}

