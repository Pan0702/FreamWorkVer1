/**
 * @brief 頂点シェーダーに入力される頂点データをまとめる構造体。
 */
struct VSInput
{
    float3 position : POSITION; // 位置。
    float4 color : COLOR;    // 色。
};

/**
 * @brief ピクセルシェーダーに渡す補間済みデータをまとめる構造体。
 */
struct PSInput
{
    float4 position : SV_POSITION; // 位置。
    float4 color : COLOR;   // 色。
};

/**
 * @brief デバッグ線分描画に使う行列を保持する定数バッファ。
 */
cbuffer CB : register(b0)
{
    float4x4 view_proj; // ビュー行列と投影行列を掛けた行列。
}

/**
 * @brief 頂点データを変換し、ピクセルシェーダーへ渡す値を作成する関数。
 */
PSInput VSMain(VSInput input)
{
    PSInput output;
    output.position = mul(float4(input.position, 1.0f), view_proj);
    output.color = input.color;
    return output;
}
