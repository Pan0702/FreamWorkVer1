struct VSInput
{
    float3 position : POSITION;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float3 dir : TEXCOORD0;
};

cbuffer SkyCB : register(b0)
{
    float4x4 wvp;
};

PSInput VSMain(VSInput input)
{
    PSInput output;
    output.position = mul(float4(input.position, 1.0f), wvp);
    output.dir = input.position;
    return output;   
}