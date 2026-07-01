/**
 * @brief ピクセルシェーダーに渡す補間済みデータをまとめる構造体。
 */
struct PSInput
{
    float4 position : SV_POSITION; // 位置。
    float2 uv : TEXCOORD; // テクスチャ座標。
    float3 normal : NORMAL; // 法線ベクトル。
    float3 world_pos : TEXCOORD1; // ワールド空間の位置。
    float3 tangent   : TANGENT; // 接線ベクトル。
    float3 bitangent : BINORMAL; // 従接線ベクトル。
};

// メッシュの色を取得するテクスチャ。
Texture2D g_texture : register(t0);
// 法線方向を補正する法線マップ。
Texture2D g_normal_map : register(t1);
// メッシュテクスチャのサンプラ。
SamplerState g_sampler : register(s0);

/**
 * @brief ライトとカメラ位置を保持する定数バッファ。
 */
cbuffer LightCB : register(b1)
{
    float4 light_dir; // ライトの向き。
    float4 light_color; // ライトの色。
    float4 ambient; // 環境光の色。
    float4 cam_pos; // カメラのワールド位置。
}

/**
 * @brief マテリアル色とテクスチャ使用情報を保持する定数バッファ。
 */
cbuffer MaterialCB : register(b2)
{
    float4 base_color; // マテリアルの基本色。
    int has_texture; // テクスチャを持つかどうか。
    float specular_intensity; // 鏡面反射の強度。
    float specular_power; // 鏡面反射の鋭さ。
    int has_normal_map; // 法線マップを持つかどうか。
}

/**
 * @brief 入力された補間済みデータから最終カラーを計算する関数。
 */
float4 PSMain(PSInput input) : SV_TARGET
{
    float3 N = normalize(input.normal);
    if (has_normal_map != 0)
    {
        float3 tn = g_normal_map.Sample(g_sampler, input.uv).xyz * 2.0 - 1.0;
        float3 T = normalize(input.tangent);
        float3 B = normalize(input.bitangent);
        float3x3 TBN = float3x3(T, B, N);
        N = normalize(mul(tn, TBN));
    }
    float3 L = normalize(-light_dir.xyz);
    float3 V = normalize(cam_pos.xyz - input.world_pos.xyz);
    if (dot(N, V) < 0.0)
    {
        N = -N;
    }
    float3 H = normalize(L + V);
    float ndotl = saturate(dot(N, L));
    float spec = pow(saturate(dot(N, H)), specular_power) * specular_intensity * ndotl;
    float4 albedo = base_color;
    if (has_texture != 0)
    {
        albedo *= g_texture.Sample(g_sampler, input.uv);
    }
    float3 lit = albedo.rgb * (light_color.rgb * ndotl + ambient.rgb) + light_color.rgb * spec;
    return float4(lit, albedo.a);
}
