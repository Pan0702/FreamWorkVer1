#pragma once
// .mesh と .skmesh で共有するバイナリエントリ定義。
// 両フォーマットのローダ/コンバータが同じレイアウトを参照するための唯一の出所。
// 依存は最小にする（my_math.h 全体は <numbers> 等を巻き込むため include しない）。
#include <cstdint>
#include "../Core/Math/vec4.h"

/**
 * @brief SubMeshEntryのデータと処理をまとめる型。
 */
struct SubMeshEntry
{
    uint32_t index_start;
    uint32_t index_count;
    uint32_t material_slot;
};

/**
 * @brief MaterialEntryのデータと処理をまとめる型。
 */
struct MaterialEntry
{
    Vec4 base_color;
    uint32_t diffuse_texture_length;
    uint32_t normal_texture_length;    
    uint32_t specular_texture_length;  
    uint32_t height_texture_length;
    uint32_t reserved;
};

// ディスク保存レイアウトを固定する。コンバータ(別プロジェクト)とランタイムで
// パディングが食い違うとバイナリ読み込みが丸ごと崩れるため、サイズを契約として固定。
static_assert(sizeof(SubMeshEntry) == 12, "SubMeshEntry layout changed");
static_assert(sizeof(MaterialEntry) == 36, "MaterialEntry layout changed");
