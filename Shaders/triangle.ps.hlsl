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
    return a2 / max(PI * d * d, 1e-6f);
}

/**
 * @brief 視線角度に応じたスペキュラ反射の色・強さを求める関数
 * @param cosThetam 視線方向 V とハーフベクトル H の近さ。
 * @param F0 正面から見たときの反射率
 * @return 角度を考慮した反射率。
 */
float3 FresnelSchlick(float cosThetam, float3 F0)
{
    const float t = saturate(1.0f - cosThetam);
    return F0 + (1.0 - F0) * pow(t, 5.0);
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
    const float ndotl = saturate(dot(N, L));
    const float ndotv = saturate(dot(N, V));
    const float ggxV = GeometrySchlickGGX(ndotv, roughness);
    const float ggxL = GeometrySchlickGGX(ndotl, roughness);
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

    return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}

/**
 * 
 * @param x 
 * @return 
 */
float3 LinearTosRGB(float3 x)
{
    return pow(x, 1.0f / 2.2f);
}

// メッシュの色を取得するテクスチャ。
Texture2D g_texture : register(t0);
// 法線方向を補正する法線マップ。
Texture2D g_normal_map : register(t1);

Texture2D g_shadow_map : register(t2);

Texture2D g_irradiance : register(t3);

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
    float4x4 ligth_view_proj;
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

SamplerComparisonState g_shadow_sampler : register(s1);

float CalcShadow(float3 world_pos)
{
    // ワールド座標をライト視点のクリップ空間へ変換。
    float4 lp = mul(float4(world_pos, 1.0f), ligth_view_proj);
    // 平行投影なので w は 1 だが、一般化のため除算しておく。
    float3 proj = lp.xyz / lp.w;

    // クリップ空間(-1～1)を UV(0～1)へ。Y は上下反転。
    float2 uv = proj.xy * float2(0.5f, -0.5f) + 0.5f;

    // シャドウマップの範囲外、または最遠面より奥は影なし扱い。
    if (uv.x < 0.0f || uv.x > 1.0f || uv.y < 0.0f || uv.y > 1.0f || proj.z > 1.0f)
    {
        return 1.0f;
    }

    // 比較サンプラーで「このピクセルの深度 <= 記録された深度」を判定。
    // SampleCmpLevelZero は 0～1 を返す(比較サンプラーがLINEARなら自動でPCF的に補間)。
    return g_shadow_map.SampleCmpLevelZero(g_shadow_sampler, uv, proj.z);
}

float2 DirToEquirect(float3 d)
{
    float u = atan2(d.z, d.x) / (2.0f * PI) + 0.5f;
    float v = acos(clamp(d.y, -1.0f, 1.0f)) / PI;
    return float2(u, v);
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
        const float3 tn = g_normal_map.Sample(g_sampler, input.uv).xyz * 2.0 - 1.0;
        const float3 T = normalize(input.tangent);
        const float3 B = normalize(input.bitangent);
        const float3x3 TBN = float3x3(T, B, N);
        N = normalize(mul(tn, TBN));
    }

    // ライト方向と視線方向を求める。法線が裏向きなら視線側へ反転する。
    const float3 L = normalize(-light_dir.xyz);
    const float3 V = normalize(cam_pos.xyz - input.world_pos.xyz);
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
    const float3 H = normalize(L + V);

    // Fresnel の基準反射率を決める。金属は albedo、非金属は 0.04 を使う。
    const float3 kDielectricF0 = float3(0.04f, 0.04f, 0.04f);
    const float3 F0 = lerp(float3(kDielectricF0), albedo.rgb, metallic);

    // Cook-Torrance の D/G/F 項を計算する。
    // D: 法線分布、G: 凹凸による隠れ具合、F: 角度による反射率。
    const float D = DistributionGGX(N, H, roughness);
    const float G = GeometrySmith(N, V, L, roughness);
    const float3 F = FresnelSchlick(saturate(dot(H, V)), F0);

    // 拡散反射を計算する。金属は拡散反射しないので metallic で弱める。
    const float3 kd = (1.0f - F) * (1.0f - metallic);
    const float3 diffuse = kd * albedo.rgb / PI;

    // 鏡面反射を計算する。分母が 0 に近くなりすぎないように下限を入れる。
    const float ndotl = saturate(dot(N, L));
    const float ndotv = saturate(dot(N, V));
    const float3 spec = (D * G * F) / max(4.0f * ndotl * ndotv, 0.001f);

    // ライト色と入射角を反映して、直接光の明るさを出す。
    const float3 radiance = light_color.rgb;
    const float shadow = CalcShadow(input.world_pos);
    float3 lit = (diffuse + spec) * radiance * ndotl * shadow;

    //環境光の明るさを出す。
    const float3 F_ambient = FresnelSchlick(ndotv, F0);
    const float3 kd_ambient = (1.0f - F_ambient) * (1.0f - metallic);
    // 環境光を足して最終色を返す。
    const float3 ambientIrradiance = g_irradiance.SampleLevel(g_sampler, DirToEquirect(N), 0).rgb;
    lit = lit + kd_ambient * albedo.rgb * ambientIrradiance;

    // HDR->LDR
    lit = ACESFilm(lit);

    // リニア->sRGB
    lit = LinearTosRGB(lit);
    
    return float4(lit, albedo.a);
}
