/**
 * @brief 頂点シェーダーに入力される頂点データをまとめる構造体。
 */
struct VSInput
{
    float2 position : POSITION; // 位置。
    float2 uv : TEXCOORD; // テクスチャ座標。
};

/**
 * @brief ピクセルシェーダーに渡す補間済みデータをまとめる構造体。
 */
struct PSInput
{
    float4 position : SV_POSITION; // 位置。
    float2 uv : TEXCOORD; // テクスチャ座標。
};

/**
 * @brief 画面空間スプライトの表示情報を保持する定数バッファ。
 */
cbuffer SpriteCB : register(b0)
{
    float2 sprite_pos; // 画面上の表示位置。
    float2 sprite_size; // スプライトの表示サイズ。
    float4 color; // 色。
    float2 screen_size; // 画面サイズ。
    float rotation; // 回転角度。
    float use_texture; // テクスチャを使うかどうかの値。
}

/**
 * @brief 頂点データを変換し、ピクセルシェーダーへ渡す値を作成する関数。
 */
PSInput VSMain(VSInput input)
{
    float2 local = input.position * sprite_size;
    float s = sin(rotation);
    float c = cos(rotation);
    float2 rotated = float2(local.x * c - local.y * s, local.x * s + local.y * c);
    float2 pixel = sprite_pos + rotated;
    float2 ndc;
    ndc.x = pixel.x / screen_size.x * 2.0 - 1.0;
    ndc.y = 1.0 - pixel.y / screen_size.y * 2.0;
    PSInput output;
    output.position = float4(ndc, 0.0f, 1.0f);
    output.uv = input.uv;
    return output;
}
