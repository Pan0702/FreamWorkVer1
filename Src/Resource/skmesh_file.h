#pragma once
#include "../Core/Math/my_math.h"      
#include "mesh_format_common.h"       

struct SkMeshFileHeader
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

struct NodeEntry
{
    int32 parent_index;          // ルートは -1。配列は親が必ず子より前（topo 順）
    int32 skin_index;            // -1: 非スキンノード。>=0: ボーンパレットの何番か
    uint32 name_length;          // 続く文字列領域のノード名長（char 数）
    uint32 reserved;             // 8byte 境界そろえ + 予備
    Mat local_bind_transform;    // = aiNode::mTransformation（転置済み）
    Mat inverse_bind_pose;       // = aiBone::mOffsetMatrix（転置済み、skin_index>=0 のみ有効）
};

///////////////////////////////////////////////////////////////////////
///AnimationInfomation
///////////////////////////////////////////////////////////////////////
struct SkAnimFileHeader
{
    char magic[4];
    uint32 version;
    float duration;
    float ticks_per_second;
    uint32 channel_count;
};

struct SkAnimChannel
{
    uint32 node_name_length;    // 続くノード名文字列のバイト長（char 数）
    uint32 pos_key_count;
    uint32 rot_key_count;
    uint32 scale_key_count;
    // この直後にファイル上で可変長領域が続く:
    //   char        node_name[node_name_length]
    //   PositionKey position_keys[pos_key_count]
    //   RotationKey rotation_keys[rot_key_count]
    //   ScaleKey    scale_keys[scale_key_count]
};

struct PositionKey
{
    float time;   
    Vec3  value;  
};

struct ScaleKey
{
    float time;
    Vec3  value;
};

struct RotationKey
{
    float time;
    Quat  value;
};

static constexpr char kSkMeshMagic[4] = {'S', 'K', 'M', 'S'};
static constexpr char kSkAnimMagic[4] = {'A', 'N', 'I', 'M'};
static constexpr uint32 kSkMeshVersion = 1;
static constexpr uint32 kSkAnimVersion = 1;

// ディスク保存レイアウトを固定（コンバータ?ランタイムのパディング不一致を防ぐ）。
// int(4)+int(4)+uint(4)+uint(4)=16, Mat(64)*2=128 → 計 144。
static_assert(sizeof(Mat) == 64, "Mat must be 16 floats");
static_assert(sizeof(NodeEntry) == 144, "NodeEntry layout changed");

// .anim 側も同様にディスクレイアウトを固定。
// char[4]+uint(4)+float(4)+float(4)+uint(4)=20。
static_assert(sizeof(SkAnimFileHeader) == 20, "SkAnimFileHeader layout changed");
// uint(4)*4=16。
static_assert(sizeof(SkAnimChannel) == 16, "SkAnimChannel layout changed");
// float(4)+Vec3(12)=16。
static_assert(sizeof(PositionKey) == 16, "PositionKey layout changed");
static_assert(sizeof(ScaleKey) == 16, "ScaleKey layout changed");
// float(4)+Quat(16)=20。
static_assert(sizeof(RotationKey) == 20, "RotationKey layout changed");
