
/**
 * @brief 頂点シェーダーに入力される頂点データをまとめる構造体。
 */
struct VSInput
{
    float3 position : POSITION; // 位置。
    float3 normal   : NORMAL; // 法線ベクトル。
    float2 uv       : TEXCOORD; // テクスチャ座標。
    float3 tangent  : TANGENT; // 接線ベクトル。
    float3 bitangent : BINORMAL; // 従接線ベクトル。
};

/**
 * @brief ピクセルシェーダーに渡す補間済みデータをまとめる構造体。
 */
struct PSInput
{
    float4 position : SV_POSITION; // 位置。
    float2 uv       : TEXCOORD; // テクスチャ座標。
    float3 normal   : NORMAL; // 法線ベクトル。
    float3 world_pos : TEXCOORD1; // ワールド空間の位置。
    float3 tangent   : TANGENT; // 接線ベクトル。
    float3 bitangent : BINORMAL; // 従接線ベクトル。
};
/**
 * @brief メッシュ描画で使うシーン行列を保持する定数バッファ。
 */
cbuffer SceneCB : register(b0)
{
    float4x4 wvp; // ワールド行列、ビュー行列、投影行列を掛けた行列。
    float4x4 world; // ワールド行列。
};
/**
 * @brief 頂点データを変換し、ピクセルシェーダーへ渡す値を作成する関数。
 * @param input シェーダーに入力されるデータ。
 * @return ピクセルシェーダーへ渡す変換済みデータ。
 */
PSInput VSMain(VSInput input)
{
    PSInput output;

    output.position = mul(float4(input.position, 1.0),wvp);
    output.normal = normalize(mul(float4(input.normal, 0.0),world).xyz);
    output.world_pos = mul(float4(input.position, 1.0),world).xyz;
    output.tangent = normalize(mul(float4(input.tangent, 0.0),world).xyz);
    output.bitangent = normalize(mul(float4(input.bitangent, 0.0),world).xyz);
    output.uv = input.uv;
    return output;
}

