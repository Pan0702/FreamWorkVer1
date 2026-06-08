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
    float2 uv : TEXCOORD; // テクスチャ座標。
};

/**
 * @brief ワールド空間スプライトの描画情報を保持する定数バッファ。
 */
cbuffer SpriteWorldCB : register(b0)
{
    float4x4 wvp; // ワールド行列、ビュー行列、投影行列を掛けた行列。
    float4 color; // 色。
    float4 src_rect; // テクスチャから切り出す範囲。
    float4 options; // テクスチャ使用などのオプション。
}

/**
 * @brief 入力された補間済みデータから最終カラーを計算する関数。
 * @param input シェーダーに入力されるデータ。
 * @return 出力するピクセルカラー。
 */
float4 PSMain(PSInput input) : SV_Target
{
    if (options.x < 0.5f)
    {
        return color;
    }

    return sprite_texture.Sample(sprite_sampler, input.uv) * color;
}
