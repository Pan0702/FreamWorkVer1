cbuffer SpriteCB : register(b0)
{
    float2 sprite_pos;
    float2 sprite_size;
    float4 color;
    float2 screen_size;
    float2 _pad;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv       : TEXCOORD;
};

float4 PSMain(PSInput input) : SV_TARGET
{
    return color;
}