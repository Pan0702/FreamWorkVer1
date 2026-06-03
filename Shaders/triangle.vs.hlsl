//シェーダー間でのデータ共有は構造体でやる。
//また：セマンティクスをつけてGPUに認識させる
struct VSInput
{
    float3 position : POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD;
    float3 tangent  : TANGENT;
    float3 bitangent : BINORMAL;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv       : TEXCOORD;
    float3 normal   : NORMAL;
};
cbuffer SceneCB : register(b0)
{
    float4x4 wvp;
    float4x4 world;
};
PSInput VSMain(VSInput input)
{
    PSInput output;
    //同次座標のw成分
    output.position = mul(float4(input.position, 1.0),wvp);
    output.normal = normalize(mul(float4(input.normal, 0.0),world).xyz);
    output.uv = input.uv;
    return output;
}
//３3x3行列だと回転と拡大しか表現できない。が4x4行列だと移動まで表せるから。
