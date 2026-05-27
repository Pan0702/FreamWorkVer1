#pragma once
#include "../Core/common.h"
#include "../Debug/ImGui/imgui_manager.h"
#include "mesh_renderer.h"
class DescriptorHeap;
class CommandQueue;
class Window;
class DepthStencil;
class SwapChain;
class Camera;
class Scene;
class CommandList;
struct RendererData
{
    CommandList* command_list;
    SwapChain* swap_chain;
    DepthStencil* depth_stencil;
    CommandQueue* command_queue;
    DescriptorHeap* srv_heap;
    Window* window;
};
class SceneRenderer
{
public:
    bool Initialize(ID3D12Device* device,HWND hwnd,ID3D12CommandQueue* command_queue,uint32_t frame_count);
    void Render(RendererData& renderer_data,Scene* scene,Camera* camera);
    void Shutdown();
    
private:
    std::unique_ptr<MeshRenderer> mesh_renderer_;
    ImGuiManager imgui_manager_;
    
};
