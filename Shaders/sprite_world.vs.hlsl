/**
 * @brief 頂点シェーダーに入力される頂点データをまとめる構造体。
 */
struct VSInput
{
    float2 position : POSITION; // 位置。
    float2 uv : TEXCOORD; // テクスチャ座標。
};

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
 * @brief 頂点データを変換し、ピクセルシェーダーへ渡す値を作成する関数。
 * @param input シェーダーに入力されるデータ。
 * @return ピクセルシェーダーへ渡す変換済みデータ。
 */
PSInput VSMain(VSInput input)
{
    PSInput output;
    output.position = mul(float4(input.position.xy, 0.0f, 1.0f), wvp);
    output.uv = src_rect.xy + input.uv * src_rect.zw;
    return output;
}
