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

cbuffer SpriteCB : register(b0)
{
    float2 sprite_pos;
    float2 sprite_size;
    float4 color;
    float2 screen_size;
    float rotation;
    float use_texture;
}

PSInput VSMain(VSInput input)
{
    float2 local = input.position * sprite_size;
    float s = sin(rotation);
    float c = cos(rotation);
    float2 rotated = float2(local.x * c - local.y * s, local.x * s + local.y * c);
    float2 pixel = sprite_pos + rotated;
    float2 ndc;
    ndc.x = pixel.x / screen_size.x * 2.0 - 1.0;
    ndc.y = 1.0 - pixel.y / screen_size.y * 2.0;
    PSInput output;
    output.position = float4(ndc, 0.0f, 1.0f);
    output.uv = input.uv;
    return output;
}
