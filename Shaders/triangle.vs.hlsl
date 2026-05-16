//シェーダー間でのデータ共有は構造体でやる。
//また：セマンティクスをつけてGPUに認識させる
struct VSInput
{
    float3 position : POSITION;
    float3 color : COLOR;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PSInput VSMain(VSInput input)
{
    PSInput output;
    //同次座標のw成分
    output.position = float4(input.position, 1.0);
    output.color = float4(input.color, 1.0);
    return output;
}
//３3x3行列だと回転と拡大しか表現できない。が4x4行列だと移動まで表せるから。
