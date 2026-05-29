cbuffer SpriteCB : register(b0)
{
    float2 sprite_pos;
    float2 sprite_size;
    float4 color;
    float2 screen_size;
    float rotation;
    float use_texture;
};

Texture2D sprite_texture : register(t0);
SamplerState sprite_sampler : register(s0);

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv       : TEXCOORD;
};

float4 PSMain(PSInput input) : SV_TARGET
{
    if (use_texture < 0.5f)
    {
        return color;
    }
    return sprite_texture.Sample(sprite_sampler, input.uv) * color;
}
