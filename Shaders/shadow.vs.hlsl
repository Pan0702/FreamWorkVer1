cbuffer ObjectCB : register(b0)
{
    float4x4 wvp;
}

float4 VSMain(float3 pos : POSITION) : SV_POSITION
{
    return mul(float4(pos, 1.0f), wvp);
}
