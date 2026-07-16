struct GpuInstanceData
{
    float4x4 world;
};

StructuredBuffer<GpuInstanceData> instance_data : register(t6);

cbuffer ShadowCB : register(b0)
{
    float4x4 light_view_proj;
};

float4 VSMain(
    float3 position : POSITION,
    uint instance_id : SV_InstanceID) : SV_POSITION
{
    const float4x4 world = instance_data[instance_id].world;
    const float4 world_position = mul(float4(position, 1.0f), world);

    return mul(world_position, light_view_proj);
}
