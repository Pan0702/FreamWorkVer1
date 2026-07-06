
cbuffer ObjectCB : register(b0)
{
    float4x4 wvp;
}

cbuffer BoneCB : register(b1)
{
    float4x4 bones[128];
}

struct VSInput
{
    float3 position : POSITION;
    uint4 bone_ids : BLENDINDICES;
    float4 weights : BLENDWEIGHT;
};

float4 VSMain(VSInput input) : SV_POSITION
{
    float4x4 skin =
        bones[input.bone_ids.x] * input.weights.x +
        bones[input.bone_ids.y] * input.weights.y +
        bones[input.bone_ids.z] * input.weights.z +
        bones[input.bone_ids.w] * input.weights.w;
    float4 pos = mul(float4(input.position, 1.0f), skin);
    return mul(pos,wvp);
}
