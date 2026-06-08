/**
 * @brief ピクセルシェーダーに渡す補間済みデータをまとめる構造体。
 */
struct PSInput
{
    float4 position : SV_POSITION; // 位置。
    float4 color : COLOR0; // 色。
};
/**
 * @brief 入力された補間済みデータから最終カラーを計算する関数。
 * @param input シェーダーに入力されるデータ。
 * @return 出力するピクセルカラー。
 */
float4 PSMain(PSInput input) : SV_Target
{
    return input.color;
}
