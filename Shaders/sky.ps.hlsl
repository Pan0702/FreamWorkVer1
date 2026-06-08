// 空の色を取得するテクスチャ。
Texture2D g_sky : register(t0);
// 空テクスチャのサンプラ。
SamplerState g_sky_sampler : register(s0);

/**
 * @brief ピクセルシェーダーに渡す補間済みデータをまとめる構造体。
 */
struct PSInput
{
    float4 pos : SV_POSITION; // クリップ空間位置。
    float3 dir : TEXCOORD0; // 空テクスチャを参照する方向。
};

static const float PI = 3.14159265;

/**
 * @brief 入力された補間済みデータから最終カラーを計算する関数。
 * @param input シェーダーに入力されるデータ。
 * @return 出力するピクセルカラー。
 */
float4 PSMain(PSInput input) : SV_TARGET
{
    float3 d = normalize(input.dir);
    float u = atan2(d.z, d.x) / (PI * 2.0f) + 0.5f;
    float v = acos(clamp(d.y, -1.0f, 1.0f)) / PI;
    return g_sky.Sample(g_sky_sampler, float2(u, v));
}
