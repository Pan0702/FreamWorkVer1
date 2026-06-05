#pragma once
#include "../Core/Math/my_math.h"      
#include "mesh_format_common.h"       

struct SkeletonMeshFileHeader
{
    char magic[4];
    uint32 version;
    uint32 vertex_count;
    uint32 index_count;
    uint32 vertex_stride;   // = sizeof(SkinnedVertex)
    uint32 index_stride;    // = sizeof(uint32)
    uint32 submesh_count;
    uint32 material_count;
    uint32 node_count;      // aiNode ツリー全体のノード数
    uint32 skin_count;      // skin_index>=0 のノード数 = ボーンパレット長
    uint32 reserved[2];     // 予備領域
};

// アニメ階層ノード1個分。skin_index<0 のときは inverse_bind_pose は未使用。
// 行列はすべて row-vector 規約（コンバータで assimp 行列を Transpose 済み）。
struct NodeEntry
{
    int32 parent_index;          // ルートは -1。配列は親が必ず子より前（topo 順）
    int32 skin_index;            // -1: 非スキンノード。>=0: ボーンパレットの何番か
    uint32 name_length;          // 続く文字列領域のノード名長（char 数）
    uint32 reserved;             // 8byte 境界そろえ + 予備
    Mat local_bind_transform;    // = aiNode::mTransformation（転置済み）
    Mat inverse_bind_pose;       // = aiBone::mOffsetMatrix（転置済み、skin_index>=0 のみ有効）
};

static constexpr char kSkMeshMagic[4] = {'S', 'K', 'M', 'S'};
static constexpr uint32 kSkMeshVersion = 1;

// ディスク保存レイアウトを固定（コンバータ↔ランタイムのパディング不一致を防ぐ）。
// int(4)+int(4)+uint(4)+uint(4)=16, Mat(64)*2=128 → 計 144。
static_assert(sizeof(Mat) == 64, "Mat must be 16 floats");
static_assert(sizeof(NodeEntry) == 144, "NodeEntry layout changed");
