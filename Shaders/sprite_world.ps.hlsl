Texture2D sprite_texture : register(t0);
SamplerState sprite_sampler : register(s0);

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

float4 PSMain(PSInput input) : SV_Target
{
    if (options.x < 0.5f)
    {
        return color;
    }

    return sprite_texture.Sample(sprite_sampler, input.uv) * color;
}
