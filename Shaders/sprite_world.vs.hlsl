struct VSInput
{
    float2 position : POSITION;
    float2 uv : TEXCOORD;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

cbuffer SpriteWorldCB : register(b0)
{
    float4x4 wvp;
    float4 color;
    float4 src_rect;
    float4 options;
}

PSInput VSMain(VSInput input)
{
    PSInput output;
    output.position = mul(float4(input.position.xy, 0.0f, 1.0f), wvp);
    output.uv = src_rect.xy + input.uv * src_rect.zw;
    return output;
}
