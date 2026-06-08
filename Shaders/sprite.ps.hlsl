/**
 * @brief 画面空間スプライトの表示情報を保持する定数バッファ。
 */
cbuffer SpriteCB : register(b0)
{
    float2 sprite_pos; // 画面上の表示位置。
    float2 sprite_size; // スプライトの表示サイズ。
    float4 color; // 色。
    float2 screen_size; // 画面サイズ。
    float rotation; // 回転角度。
    float use_texture; // テクスチャを使うかどうかの値。
};

// スプライト表示に使うテクスチャ。
Texture2D sprite_texture : register(t0);
// スプライトテクスチャのサンプラ。
SamplerState sprite_sampler : register(s0);

/**
 * @brief ピクセルシェーダーに渡す補間済みデータをまとめる構造体。
 */
struct PSInput
{
    float4 position : SV_POSITION; // 位置。
    float2 uv       : TEXCOORD; // テクスチャ座標。
};

/**
 * @brief 入力された補間済みデータから最終カラーを計算する関数。
 * @param input シェーダーに入力されるデータ。
 * @return 出力するピクセルカラー。
 */
float4 PSMain(PSInput input) : SV_TARGET
{
    if (use_texture < 0.5f)
    {
        return color;
    }
    return sprite_texture.Sample(sprite_sampler, input.uv) * color;
}
