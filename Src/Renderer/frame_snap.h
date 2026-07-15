#pragma once
#include <vector>
#include "draw_command.h"
#include "../Resource/vertex_types.h"
struct ImDrawData;
struct CameraSnap
{
    Mat view;
    Mat projection;
    Vec3 pos;
};

struct LightSnap
{
    Vec3 pos;
    Vec3 color;
    Mat lvp;
};

enum class DrawType
{
    kMesh,
    kSkinnedMesh,
    kSprite,
    kUI,
    kDebugLine,
    kDebugTriangle,
};

struct SceneDrawItem
{
    DrawType type;
    int draw_order;
    uint32 command_index;
    uint64 sequence;
};

struct FrameSnap
{
    std::vector<MeshDrawCommand>    mesh_commands;    
    std::vector<SkinnedDrawCommand> skinned_commands;  
    std::vector<SpriteDrawCommand>  sprite_commands; 
    std::vector<SpriteDrawCommand>  ui_commands;
    std::vector<DebugLineVertex> debug_lines;     
    std::vector<DebugLineVertex> debug_triangles; 
    
    std::vector<SceneDrawItem> draw_items;
    
    CameraSnap camera;
    LightSnap light;
    ImDrawData* imgui_draw_data = nullptr;
};
