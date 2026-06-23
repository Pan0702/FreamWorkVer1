/**
 * @brief 頂点シェーダーに入力される頂点データをまとめる構造体。
 */
struct VSInput
{
    float3 position : POSITION; // 位置。
};

/**
 * @brief ピクセルシェーダーに渡す補間済みデータをまとめる構造体。
 */
struct PSInput
{
    float4 position : SV_POSITION; // 位置。
    float3 dir : TEXCOORD0; // 空テクスチャを参照する方向。
};

/**
 * @brief 空メッシュの変換行列を保持する定数バッファ。
 */
cbuffer SkyCB : register(b0)
{
    float4x4 wvp; // ワールド行列、ビュー行列、投影行列を掛けた行列。
};

/**
 * @brief 頂点データを変換し、ピクセルシェーダーへ渡す値を作成する関数。
 */
PSInput VSMain(VSInput input)
{
    PSInput output;
    output.position = mul(float4(input.position, 1.0f), wvp);
    output.dir = input.position;
    return output;   
}