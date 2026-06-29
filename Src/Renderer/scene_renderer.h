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
     * @brief 利用前に必要な参照とリソースを初期化する。
     * @param device 使用する D3D12 デバイス。
     * @param hwnd 描画対象の Win32 ウィンドウハンドル。
     * @param command_queue GPU コマンドを実行するキュー。
     * @param frame_count 用意するフレームリソース数。
     * @return 必要なリソースを作成し、使用可能な状態にできた場合は true。
     */
    bool Initialize(ID3D12Device* device, HWND hwnd, ID3D12CommandQueue* command_queue, uint32_t frame_count);
 /**
  * @brief 現在の状態をもとに描画コマンドを積む。
  * @param renderer_data レンダラー初期化に使う共有データ。
  * @param world Actor や描画対象を管理する World。
  * @param camera ビュー行列と射影行列を提供するカメラ。
  */
 void Render(RendererData& renderer_data, World* world, Camera* camera);
    /**
     * @brief 保持しているリソースと登録状態を解放する。
     */
    void Shutdown();
    /**
     * @brief MeshRenderer を取得する。
     * @return MeshRenderer。見つからない、または未作成の場合は nullptr。
     */
    MeshRenderer* GetMeshRenderer() const;
    /**
     * @brief SpriteRenderer を取得する。
     * @return SpriteRenderer。見つからない、または未作成の場合は nullptr。
     */
    SpriteRenderer* GetSpriteRenderer() const;
    /**
     * @brief UiRenderer を取得する。
     * @return UiRenderer。見つからない、または未作成の場合は nullptr。
     */
    UIRenderer* GetUIRenderer() const;
    /**
     * @brief SkinnedMeshRenderer を取得する。
     * @return SkinnedMeshRenderer。見つからない、または未作成の場合は nullptr。
     */
    SkinnedMeshRenderer* GetSkinnedMeshRenderer() const;
    /**
     * @brief DebugLineRenderer を取得する。
     * @return DebugLineRenderer。見つからない、または未作成の場合は nullptr。
     */
    DebugLineRenderer* GetDebugLineRenderer() const;
    
    /**
     * @brief Im Gui Manager を取得する。
     * @return 保持している Im Gui Manager への参照。
     */
    ImGuiManager& GetImGuiManager();

private:
    /**
     * @brief 描画先をバックバッファと深度バッファへ切り替える。
     * @param renderer_data レンダラー初期化に使う共有データ。
     */
    void BeginRenderTarget(const RendererData& renderer_data);
    /**
     * @brief 描画先のバックバッファを Present 可能な状態へ戻す。
     * @param renderer_data レンダラー初期化に使う共有データ。
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
