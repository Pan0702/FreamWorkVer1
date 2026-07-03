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
    const float cos = saturate(1.0f - cosThetam);
    return F0 + cos * pow(cos, 5.0f);
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

/**
 * 
 * @param x 
 * @return 
 */
float3 ACESFilm(float3 x)
{
    const float a = 2.51f;
    const float b = 0.03f;
    const float c = 2.43f;
    const float d = 0.59f;
    const float e = 0.14f;
    return saturate(x * (a * x + b)) / (x * (c * x + d) + e);
}

/**
 * 
 * @param x 
 * @return 
 */
float3 LinearTosRGB(float3 x)
{
    return pow(x,1.0f / 2.2f);
}
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
    float4 sky_color;
    float4 ground_color;
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
    // 法線を用意する。法線マップがある場合は、タンジェント空間からワールド空間へ変換する。
    float3 N = normalize(input.normal);
    if (has_normal_map != 0)
    {
        float3 tn = g_normal_map.Sample(g_sampler, input.uv).xyz * 2.0 - 1.0;
        float3 T = normalize(input.tangent);
        float3 B = normalize(input.bitangent);
        float3x3 TBN = float3x3(T, B, N);
        N = normalize(mul(tn, TBN));
    }

    // ライト方向と視線方向を求める。法線が裏向きなら視線側へ反転する。
    float3 L = normalize(-light_dir.xyz);
    float3 V = normalize(cam_pos.xyz - input.world_pos.xyz);
    if (dot(N, V) < 0.0)
    {
        N = -N;
    }
    
    // マテリアルの基本色を決める。テクスチャがある場合は base_color に掛け合わせる。
    float4 albedo = base_color;
    if (has_texture != 0)
    {
        albedo *= g_texture.Sample(g_sampler, input.uv);
    }

    // ライト方向と視線方向の中間方向を求める。鏡面反射の計算で使う。
    float3 H = normalize(L + V);

    // Fresnel の基準反射率を決める。金属は albedo、非金属は 0.04 を使う。
    const float3 kDielectricF0 = float3(0.04f, 0.04f, 0.04f);
    float3 F0 = lerp(float3(kDielectricF0), albedo.rgb, metallic);

    // Cook-Torrance の D/G/F 項を計算する。
    // D: 法線分布、G: 凹凸による隠れ具合、F: 角度による反射率。
    float D = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    float3 F = FresnelSchlick(saturate(dot(H, V)), F0);

    // 拡散反射を計算する。金属は拡散反射しないので metallic で弱める。
    float3 kd = (1.0f - F) * (1.0f - metallic);
    float3 diffuse = kd * albedo.rgb / PI;

    // 鏡面反射を計算する。分母が 0 に近くなりすぎないように下限を入れる。
    float ndotl = saturate(dot(N, L));
    float ndotv = saturate(dot(N, V));
    float3 spec = (D * G * F) / max(4.0f * ndotl * ndotv, 0.001f);

    // ライト色と入射角を反映して、直接光の明るさを出す。
    float3 radiance = light_color.rgb;
    float3 lit = (diffuse + spec) * radiance * ndotl;

    // 環境光を足して最終色を返す。
    // hemi = hemisphere
    float hemi = N.y * 0.5 + 0.5;
    float3 ambientIrradiance = lerp(ground_color.rgb,sky_color.rgb,hemi);
    lit += kd * albedo.rgb * ambientIrradiance;
    
    // HDR->LDR
    lit = ACESFilm(lit);
    
    // リニア->sRGB
    lit = LinearTosRGB(lit);
    return float4(lit, albedo.a);
}
