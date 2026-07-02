static const float PI = 3.1415926535897932384626433832795;

/**
 * @brief 表面の細かい凹凸によって、光や視線がどれくらい隠れるか  
 * @param N 表面の法線方向
 * @param H ライト方向と視線方向の中間方向
 * @param roughness ざらつき
 * @return D項
 */
float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    // 0 ~ 1の間にclamp //
    float doth = saturate(dot(N, H));
    float d = doth * doth * (a2 - 1.0f) + 1.0f;
    return a2 / (PI * d * d);
}

/**
 * @brief 視線角度に応じたスペキュラ反射の色・強さを求める関数
 * @param cosThetam 視線方向 V とハーフベクトル H の近さ。
 * @param F0 正面から見たときの反射率
 * @return 角度を考慮した反射率。
 */
float3 FresnelSchlick(float cosThetam, float3 F0)
{
    float cos = saturate(1.0f - cosThetam);
    return F0 + (1.0f - F0) * pow(1.0f - cos, 5.0f);
}

/**
* @brief 表面のざらつきによって、光や視線がどれくらい隠れるかを求める
* @param ndotv 表面法線 N と視線方向 V の近さ
 * @param roughness ざらつき
* @return Schlick-GGX による Geometry 項の片方向分。
 */
float GeometrySchlickGGX(float ndotv, float roughness)
{
    float r = (roughness + 1.0f);
    float k = (r * r) / 8.0f;
    return ndotv / (ndotv * (1.0f - k) + k);
}

/**
 * @brief　Smith GGXのGeometry項を計算する
 * @param N 規化済みの表面法線方向。
 * @param V 正規化済みの視線方向
 * @param L 正規化済みのライト方向
 * @param roughness 表面のざらつき
* @return Geometry項。0.0から1.0程度の遮蔽係数。
 */
float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float3 ndotl = saturate(dot(N, L));
    float3 ndotv = saturate(dot(N, V));
    float ggxV = GeometrySchlickGGX(ndotv, roughness);
    float ggxL = GeometrySchlickGGX(ndotl, roughness);
    return ggxV * ggxL;
};
/**
 * @brief ピクセルシェーダーに渡す補間済みデータをまとめる構造体。
 */
struct PSInput
{
    float4 position : SV_POSITION; // 位置。
    float2 uv : TEXCOORD; // テクスチャ座標。
    float3 normal : NORMAL; // 法線ベクトル。
    float3 world_pos : TEXCOORD1; // ワールド空間の位置。
    float3 tangent : TANGENT; // 接線ベクトル。
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
    int has_normal_map; // 法線マップを持つかどうか。
    float metallic; // 0=誘電体, 1=金属              
    float roughness; // 0=つるつる, 1=ざらざら
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
    
    float4 albedo = base_color;
    if (has_texture != 0)
    {
        albedo *= g_texture.Sample(g_sampler, input.uv);
    }
    float3 H = normalize(L + V);

    // 金属はalbedoを反射色にして金属以外は0.04に固定
    const float3 kDielectricF0 = float3(0.04f, 0.04f, 0.04f);
    float3 F0 = lerp(float3(kDielectricF0), albedo.rgb, metallic);
    float D = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    float3 F = FresnelSchlick(saturate(dot(H, V)), F0);

    //金属は拡散反射しない
    float3 kd = (1.0f - F) * (1.0f - metallic);
    float3 diffuse = kd * albedo.rgb / PI;
    float ndotl = saturate(dot(N, L));
    float ndotv = saturate(dot(N, V));
    float3 spec = (D * G * F) / max(4.0f * ndotl * ndotv, 0.001f);
    float3 radiance = light_color.rgb;
    float3 lit = (diffuse + spec) * radiance * ndotl;

    lit += ambient.rgb * albedo.rgb;
    return float4(lit, albedo.a);
}
