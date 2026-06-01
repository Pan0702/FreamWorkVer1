struct VSInput
{
    float3 position : POSITION;
    float4 color : COLOR;   
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;  
};

cbuffer CB : register(b0)
{
    float4x4 view_proj;
}

PSInput VSMain(VSInput input)
{
    PSInput output;
    output.position = mul(float4(input.position, 1.0f), view_proj);
    output.color = input.color;
    return output;
}
