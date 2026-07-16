#pragma once
#include <vector>

#include "draw_command.h"
#include "frame_snap.h"
#include "../Graphics/graphics_device.h"

struct RenderContext;
class InstancedStaticMeshComponent;
class DescriptorHeap;
class Camera;
class RenderObject;

class InstancedMeshRenderer
{
public:
    bool Initialize(const ID3D12Device* device);
    
    void Register(InstancedStaticMeshComponent* object);
    
    void Unregister(InstancedStaticMeshComponent* object);
    
    void Collect(FrameSnap& write_snap) const;
    
    void Submit(const RenderContext& context, const InstancedMeshDrawCommand& command, const FrameSnap& read_snap);
    
    void SubmitDepth(const RenderContext& context, const FrameSnap& read_snap) const;
    
private:
    std::vector<InstancedStaticMeshComponent*> registered_;

};
