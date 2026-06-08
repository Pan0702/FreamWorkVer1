#pragma once
// .mesh と .skmesh で共有するバイナリエントリ定義。
// 両フォーマットのローダ/コンバータが同じレイアウトを参照するための唯一の出所。
// 依存は最小にする（my_math.h 全体は <numbers> 等を巻き込むため include しない）。
#include <cstdint>
#include "../Core/Math/vec4.h"

// サブメッシュ1個分（インデックス範囲 + 使用マテリアルスロット）
struct SubMeshEntry
{
    uint32_t index_start;
    uint32_t index_count;
    uint32_t material_slot;
};

// マテリアル1個分。base_color のあとに diffuse_texture_length 文字分の
// ワイド文字列が（別領域に）続く。
struct MaterialEntry
{
    Vec4 base_color;
    uint32_t diffuse_texture_length;
    uint32_t reserved;
};

// ディスク保存レイアウトを固定する。コンバータ(別プロジェクト)とランタイムで
// パディングが食い違うとバイナリ読み込みが丸ごと崩れるため、サイズを契約として固定。
static_assert(sizeof(SubMeshEntry) == 12, "SubMeshEntry layout changed");
static_assert(sizeof(MaterialEntry) == 24, "MaterialEntry layout changed");
