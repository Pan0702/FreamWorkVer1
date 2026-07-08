#pragma once
#include <vector>
#include "draw_command.h"
#include "../Resource/vertex_types.h"

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

struct FrameSnap
{
    std::vector<MeshDrawCommand>    mesh_commands;    
    std::vector<SkinnedDrawCommand> skinned_commands;  
    std::vector<SpriteDrawCommand>  sprite_commands; 
    std::vector<SpriteDrawCommand>  ui_commands;
    std::vector<DebugLineVertex> debug_lines;     
    std::vector<DebugLineVertex> debug_triangles; 
    CameraSnap camera;
    LightSnap light;
};
