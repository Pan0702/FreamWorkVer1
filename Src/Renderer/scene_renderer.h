#pragma once
#include "../Core/common.h"
#include "../Debug/ImGui/imgui_manager.h"
#include "mesh_renderer.h"
#include "skinned_mesh_renderer.h"
#include "sprite_renderer.h"
#include "ui_renderer.h"

class DebugLineRenderer;
class Camera;
class CommandList;
class CommandQueue;
class ConstantBufferAllocator;
class DepthStencil;
class DescriptorHeap;
class Scene;
class SwapChain;
class Window;
class World;

struct RendererData
{
    CommandList* command_list = nullptr;
    SwapChain* swap_chain = nullptr;
    DepthStencil* depth_stencil = nullptr;
    CommandQueue* command_queue = nullptr;
    DescriptorHeap* srv_heap = nullptr;
    ConstantBufferAllocator* cb_allocator = nullptr;
    Window* window = nullptr;
};

class SceneRenderer
{
public:
    bool Initialize(ID3D12Device* device, HWND hwnd, ID3D12CommandQueue* command_queue, uint32_t frame_count);
    void Render(const RendererData& renderer_data, Scene* scene, Camera* camera);
    void Render(RendererData& renderer_data, World* world, Camera* camera);
    void Shutdown();
    MeshRenderer* GetMeshRenderer() const;
    SpriteRenderer* GetSpriteRenderer() const;
    UIRenderer* GetUIRenderer() const;
    SkinnedMeshRenderer* GetSkinnedMeshRenderer() const;
    DebugLineRenderer* GetDebugLineRenderer() const;

private:
    void BeginRenderTarget(const RendererData& renderer_data);
    void EndRenderTarget(const RendererData& renderer_data);

    std::unique_ptr<MeshRenderer> mesh_renderer_;
    std::unique_ptr<SpriteRenderer> sprite_renderer_;
    std::unique_ptr<UIRenderer> ui_renderer_;
    std::unique_ptr<SkinnedMeshRenderer> skinned_mesh_renderer_;
    std::unique_ptr<DebugLineRenderer> debug_renderer_;
    ImGuiManager imgui_manager_;
};
