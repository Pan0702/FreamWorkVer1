#pragma once
#include <vector>
#include "draw_command.h"
#include "../Resource/vertex_types.h"
struct ImDrawData;
/**
 * @brief 1フレーム分の描画用カメラデータを保持する。
 */
struct CameraSnap
{
    Mat view;
    Mat projection;
    Vec3 pos;
};

/**
 * @brief 1フレーム分の描画用ライトデータを保持する。
 */
struct LightSnap
{
    Vec3 pos;
    Vec3 color;
    Mat lvp;
};

/**
 * @brief 描画項目を処理するレンダラーの種類を表す。
 */
enum class DrawType
{
    kMesh,
    kSkinnedMesh,
    kSprite,
    kInstancedMesh,
};

/**
 * @brief 描画提出順に並ぶ1つのコマンドを参照する。
 */
struct SceneDrawItem
{
    DrawType type;
    int draw_order;
    uint32 command_index;
    uint64 sequence;
};

/**
 * @brief 1フレーム分の描画データをまとめる。
 */
struct FrameSnap
{
    std::vector<MeshDrawCommand>    mesh_commands;    
    std::vector<SkinnedDrawCommand> skinned_commands;  
    std::vector<SpriteDrawCommand>  sprite_commands; 
    std::vector<SpriteDrawCommand>  ui_commands;
    std::vector<DebugLineVertex> debug_lines;     
    std::vector<DebugLineVertex> debug_triangles; 
    std::vector<GPUInstanceData> gpu_instances;  
    std::vector<InstancedMeshDrawCommand> instanced_mesh_commands;
    std::vector<SceneDrawItem> draw_items;
    
    CameraSnap camera;
    LightSnap light;
    ImDrawData* imgui_draw_data = nullptr;
};
