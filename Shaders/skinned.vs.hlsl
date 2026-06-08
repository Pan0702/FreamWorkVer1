/**
 * @brief 頂点シェーダーに入力される頂点データをまとめる構造体。
 */
struct VSInput
{
    float3 position : POSITION; // 位置。
    float3 normal : NORMAL; // 法線ベクトル。
    float2 uv : TEXCOORD; // テクスチャ座標。
    float3 tangent : TANGENT; // 接線ベクトル。
    float3 bitangent : BINORMAL; // 従接線ベクトル。
    uint4 indices : BLENDINDICES; // 参照するボーン番号。
    float4 weights : BLENDWEIGHT; // 各ボーンの影響度。
};

/**
 * @brief ピクセルシェーダーに渡す補間済みデータをまとめる構造体。
 */
struct PSInput
{
    float4 position : SV_POSITION; // 位置。
    float2 uv : TEXCOORD; // テクスチャ座標。
    float3 normal : NORMAL; // 法線ベクトル。
    float3 world_pos: TEXCOORD1; // ワールド空間の位置。
    float3 tangent : TANGENT; // 接線ベクトル。
    float3 bitangent: BINORMAL; // 従接線ベクトル。
};

/**
 * @brief メッシュ描画で使うシーン行列を保持する定数バッファ。
 */
cbuffer SceneCB : register(b0)
{
    float4x4 wvp; // ワールド行列、ビュー行列、投影行列を掛けた行列。
    float4x4 world; // ワールド行列。
}

/**
 * @brief スキニング用のボーン行列配列を保持する定数バッファ。
 */
cbuffer BoneCB : register(b1)
{
    float4x4 bones[128]; // ボーンごとのスキニング行列。
}

/**
 * @brief 頂点データを変換し、ピクセルシェーダーへ渡す値を作成する関数。
 * @param input シェーダーに入力されるデータ。
 * @return ピクセルシェーダーへ渡す変換済みデータ。
 */
PSInput VSMain(VSInput input)
{
    PSInput output;
    float4x4 skin =
        input.weights.x * bones[input.indices.x] +
        input.weights.y * bones[input.indices.y] +
        input.weights.z * bones[input.indices.z] +
        input.weights.w * bones[input.indices.w];
    
    float4 skinned_pos = mul(float4(input.position, 1.0f), skin);
    output.position = mul(skinned_pos, wvp);
    output.world_pos = mul(skinned_pos, world).xyz;

    float3 skinned_normal = mul(float4(input.normal, 0.0f), skin).xyz;
    float3 skinned_tangent = mul(float4(input.tangent, 0.0f), skin).xyz;
    float3 skinned_bitangent = mul(float4(input.bitangent, 0.0f), skin).xyz;
    output.normal = normalize(mul(float4(skinned_normal,0.0),world).xyz);
    output.tangent = normalize(mul(float4(skinned_tangent,0.0),world).xyz);
    output.bitangent = normalize(mul(float4(skinned_bitangent,0.0),world).xyz);
    output.uv = input.uv;
    return output;
}
