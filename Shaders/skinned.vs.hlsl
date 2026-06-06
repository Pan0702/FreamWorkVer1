struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
    float3 bitangent : BINORMAL;
    uint4 indices : BLENDINDICES;
    float4 weights : BLENDWEIGHT;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 world_pos: TEXCOORD1;
    float3 tangent : TANGENT;
    float3 bitangent: BINORMAL;
};

cbuffer SceneCB : register(b0)
{
    float4x4 wvp;
    float4x4 world;
}

cbuffer BoneCB : register(b1)
{
    float4x4 bones[128];
}

PSInput VSMain(VSInput input)
{
    PSInput output;
    float4x4 skin =
        input.weights.x * bones[input.indices.x] +
        input.weights.y * bones[input.indices.y] +
        input.weights.z * bones[input.indices.z] +
        input.weights.w * bones[input.indices.w];
    
    float4 skinned_pos = mul(float4(input.position, 1.0f), skin);
    output.position = mul(skinned_pos, wvp);
    output.world_pos = mul(skinned_pos, world).xyz;

    float3 skinned_normal = mul(float4(input.normal, 0.0f), skin).xyz;
    float3 skinned_tangent = mul(float4(input.tangent, 0.0f), skin).xyz;
    float3 skinned_bitangent = mul(float4(input.bitangent, 0.0f), skin).xyz;
    output.normal = normalize(mul(float4(skinned_normal,0.0),world).xyz);
    output.tangent = normalize(mul(float4(skinned_tangent,0.0),world).xyz);
    output.bitangent = normalize(mul(float4(skinned_bitangent,0.0),world).xyz);
    output.uv = input.uv;
    return output;
}
