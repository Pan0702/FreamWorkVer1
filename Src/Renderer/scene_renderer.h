#pragma once
#include "../Core/common.h"
#include "../Debug/ImGui/imgui_manager.h"
#include "mesh_renderer.h"
#include "skinned_mesh_renderer.h"
#include "sky_renderer.h"
#include "sprite_renderer.h"
#include "ui_renderer.h"

class DebugLineRenderer;
class Camera;
class CommandList;
class CommandQueue;
class ConstantBufferAllocator;
class DepthStencil;
class DescriptorHeap;
class SwapChain;
class Window;
class World;
/**
 * @brief RendererDataのデータと処理をまとめる型。
 */
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
/**
 * @brief SceneRendererのデータと処理をまとめる型。
 */
class SceneRenderer
{
public:
    /**
     * @brief 初期化に必要な参照とリソースを設定する関数。
     * @param device DirectX 12 デバイス。
     * @param hwnd Win32 ウィンドウハンドル。
     * @param command_queue コマンド実行に使うキュー。
     * @param frame_count 引数。
     * @return 条件を満たす場合は true。
     */
    bool Initialize(ID3D12Device* device, HWND hwnd, ID3D12CommandQueue* command_queue, uint32_t frame_count);
 /**
  * @brief 登録済みの描画対象を描画する関数。
  * @param renderer_data 引数。
  * @param world 引数。
  * @param camera 引数。
  */
 void Render(RendererData& renderer_data, World* world, Camera* camera);
    /**
     * @brief 保持しているリソースを解放する関数。
     */
    void Shutdown();
    /**
     * @brief MeshRendererを取得する関数。
     * @return 戻り値。
     */
    MeshRenderer* GetMeshRenderer() const;
    /**
     * @brief SpriteRendererを取得する関数。
     * @return 戻り値。
     */
    SpriteRenderer* GetSpriteRenderer() const;
    /**
     * @brief UIRendererを取得する関数。
     * @return 戻り値。
     */
    UIRenderer* GetUIRenderer() const;
    /**
     * @brief SkinnedMeshRendererを取得する関数。
     * @return 戻り値。
     */
    SkinnedMeshRenderer* GetSkinnedMeshRenderer() const;
    /**
     * @brief DebugLineRendererを取得する関数。
     * @return 戻り値。
     */
    DebugLineRenderer* GetDebugLineRenderer() const;

private:
    /**
     * @brief BeginRenderTargetを行う関数。
     * @param renderer_data 引数。
     */
    void BeginRenderTarget(const RendererData& renderer_data);
    /**
     * @brief EndRenderTargetを行う関数。
     * @param renderer_data 引数。
     */
    void EndRenderTarget(const RendererData& renderer_data);

    std::unique_ptr<MeshRenderer> mesh_renderer_;
    std::unique_ptr<SpriteRenderer> sprite_renderer_;
    std::unique_ptr<UIRenderer> ui_renderer_;
    std::unique_ptr<SkinnedMeshRenderer> skinned_mesh_renderer_;
    std::unique_ptr<DebugLineRenderer> debug_renderer_;
    std::unique_ptr<SkyRenderer> sky_renderer_;
    ImGuiManager imgui_manager_;
};
