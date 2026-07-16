struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
    float3 bitangent : BINORMAL;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 world_pos : TEXCOORD1;
    float3 tangent : TANGENT;
    float3 bitangent : BINORMAL;
};

struct GpuInstanceData
{
    float4x4 world;
};

StructuredBuffer<GpuInstanceData> instance_data : register(t6);

cbuffer CB : register(b0)
{
    float4x4 view_proj;
}

PSInput VSMain(
    VSInput input,
    uint instance_id : SV_InstanceID)
{
    PSInput output;

    const float4x4 world =instance_data[instance_id].world;
    const float4 world_position =mul(float4(input.position, 1.0f), world);
    output.position =mul(world_position, view_proj);
    output.world_pos = world_position.xyz;
    output.normal = normalize(mul(float4(input.normal, 0.0f), world).xyz);
    output.tangent = normalize(mul(float4(input.tangent, 0.0f), world).xyz);
    output.bitangent = normalize(mul(float4(input.bitangent, 0.0f), world).xyz);
    
    output.uv = input.uv;

    return output;
}
