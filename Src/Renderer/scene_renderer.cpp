#include "scene_renderer.h"

#include "debug_line_renderer.h"
#include "ibl_baker.h"
#include "../Engine/camera.h"
#include "../Engine/world.h"
#include "../Graphics/command_list.h"
#include "../Graphics/depth_stencil.h"
#include "../Graphics/swap_chain.h"
#include "../Platform/window.h"
#include "render_context.h"
#include "shadow_renderer.h"
#include "../Resource/texture_manager.h"

namespace 
{
    // 平行光には実際の位置がないため、LookAt 用の仮想カメラ距離として使う。
    constexpr float kShadowLightViewDistance = 30.0f;
    // ライト視点で影を描く正射影範囲。値を大きくすると広範囲を覆えるが、影の解像度は粗くなる。
    constexpr float kShadowOrthoWidth = 50.0f;
    constexpr float kShadowOrthoHeight = 50.0f;
    // ライト視点で深度を記録する奥行き範囲。
    constexpr float kShadowNearZ = 0.1f;
    constexpr float kShadowFarZ = 100.0f;
}
SceneRenderer::SceneRenderer() = default;
SceneRenderer::~SceneRenderer() = default;

bool SceneRenderer::Initialize(ID3D12Device* device, HWND hwnd, ID3D12CommandQueue* command_queue,
                               uint32_t frame_count, DescriptorHeap* srv_heap)
{
    shadow_renderer_ = std::make_unique<ShadowRenderer>();
    if (!shadow_renderer_->Initialize(device, srv_heap))
    {
        return false;
    }

    mesh_renderer_ = std::make_unique<MeshRenderer>();
    if (!mesh_renderer_->Initialize(device))
    {
        return false;
    }

    sky_renderer_ = std::make_unique<SkyRenderer>();
    if (!sky_renderer_->Initialize(device))
    {
        return false;
    }

    // SkySphereのテクスチャをセット
    LoadedImage sky_image;
    const wchar_t* sky_texture_path = L"Assets/Texture/SkyImage.png";
    TextureLoader::LoadFromFile(sky_texture_path, sky_image);
    LoadedImage irr = IBLBaker::BakeIrradianceMap(sky_image);
    irradiance_texture_ = TextureManager::Get().CreateFromImage(L"__sky_irradiance", irr, false);
    sky_renderer_->SetTexture(TextureManager::Get().Load(sky_texture_path));
    sprite_renderer_ = std::make_unique<SpriteRenderer>();
    if (!sprite_renderer_->Initialize(device))
    {
        return false;
    }

    ui_renderer_ = std::make_unique<UIRenderer>();
    if (!ui_renderer_->Initialize(device))
    {
        return false;
    }
    debug_renderer_ = std::make_unique<DebugLineRenderer>();
    if (!debug_renderer_->Initialize(device))
    {
        return false;
    }
    skinned_mesh_renderer_ = std::make_unique<SkinnedMeshRenderer>();
    if (!skinned_mesh_renderer_->Initialize(device))
    {
        return false;
    }

    return imgui_manager_.Initialize(hwnd, device, command_queue, static_cast<int>(frame_count));
}

void SceneRenderer::Render(RendererData& renderer_data)
{
    const int index =  1 - write_index_;
    RenderContext context = {};
    context.command_list = renderer_data.command_list->GetCommandList();
    context.srv_heap = renderer_data.srv_heap;
    context.cb_allocator = renderer_data.cb_allocator;
    context.screen_size = Vec2(static_cast<float>(renderer_data.window->GetWidth()),
                               static_cast<float>(renderer_data.window->GetHeight()));

    
    context.irradiance_srv_index = irradiance_texture_->GetSrvIndex();
    
    //影はバックバッファに書くため、メインバッファに書き込む前に書き込む//
    shadow_renderer_->RenderShadowPass(context, mesh_renderer_.get(),
                                       skinned_mesh_renderer_.get(), frame_snaps_[index]);
    context.shadow_srv_index = shadow_renderer_->GetShadowMapIndex();
    
    // ImGui::ShowDemoWindow();
    BeginRenderTarget(renderer_data);
    sky_renderer_->Render(context, frame_snaps_[index]);
    
    mesh_renderer_->Submit(context, frame_snaps_[index]);
    debug_renderer_->Submit(context, frame_snaps_[index]);
    sprite_renderer_->Submit(context, frame_snaps_[index]);
    skinned_mesh_renderer_->Submit(context, frame_snaps_[index]);
    ui_renderer_->Submit(context, frame_snaps_[index]);
    
    imgui_manager_.RenderCloneData(frame_snaps_[index].imgui_draw_data,context.command_list);
    EndRenderTarget(renderer_data);
}

void SceneRenderer::AllCollect(Camera& c)
{
    FrameSnap& snap = frame_snaps_[write_index_];
    snap.camera.pos = c.pos_;
    snap.camera.view = c.GetViewMatrix();
    snap.camera.projection = c.GetProjectionMatrix();
    snap.light.pos = Vec3(0.3f,-1.0f,0.5f).Normalized();
    snap.light.color = Vec3(3.0f,3.0f,3.0f);
    
    const Vec3 eye = Vec3(0, 0, 0) - snap.light.pos * kShadowLightViewDistance;
    const Mat light_view = LookAtLH(eye, Vec3(0, 0, 0), Vec3(0, 1, 0));
    // 平行光の影なので Perspective ではなく Orthographicを使う。
    // 幅・高さはシャドウを描きたい範囲に合わせて調整する。
    const Mat light_proj = OrthographicLH(kShadowOrthoWidth, kShadowOrthoHeight, kShadowNearZ, kShadowFarZ);
    snap.light.lvp = light_view * light_proj;
    
    mesh_renderer_->Collect(snap);
    debug_renderer_->Collect(snap);
    sprite_renderer_->Collect(snap);
    skinned_mesh_renderer_->Collect(snap);
    ui_renderer_->Collect(snap);
    
    imgui_manager_.FreeDrawData(snap.imgui_draw_data);
    snap.imgui_draw_data = imgui_manager_.CloneCurrentData();
}

void SceneRenderer::Shutdown()
{
    if (ui_renderer_)
    {
        ui_renderer_->Shutdown();
    }
    if (sprite_renderer_)
    {
        sprite_renderer_->Shutdown();
    }
    imgui_manager_.FreeDrawData(frame_snaps_[0].imgui_draw_data);
    imgui_manager_.FreeDrawData(frame_snaps_[1].imgui_draw_data);
    imgui_manager_.Shutdown();
}

void SceneRenderer::SwapFrame()
{
    write_index_ ^= 1;
}

MeshRenderer* SceneRenderer::GetMeshRenderer() const
{
    return mesh_renderer_.get();
}

SpriteRenderer* SceneRenderer::GetSpriteRenderer() const
{
    return sprite_renderer_.get();
}

UIRenderer* SceneRenderer::GetUIRenderer() const
{
    return ui_renderer_.get();
}

SkinnedMeshRenderer* SceneRenderer::GetSkinnedMeshRenderer() const
{
    return skinned_mesh_renderer_.get();
}

DebugLineRenderer* SceneRenderer::GetDebugLineRenderer() const
{
    return debug_renderer_.get();
}

ImGuiManager& SceneRenderer::GetImGuiManager()
{
    return imgui_manager_;
}

void SceneRenderer::BeginRenderTarget(const RendererData& renderer_data)
{
    auto command_list = renderer_data.command_list->GetCommandList();
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = renderer_data.swap_chain->GetCurrentBackBuffer();
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    command_list->ResourceBarrier(1, &barrier);

    D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle = renderer_data.swap_chain->GetCurrentRtvHandle();
    D3D12_CPU_DESCRIPTOR_HANDLE dsv_handle = renderer_data.depth_stencil->GetCpuHandle();
    command_list->OMSetRenderTargets(1, &rtv_handle, false, &dsv_handle);

    constexpr float clear_color[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    command_list->ClearRenderTargetView(rtv_handle, clear_color, 0, nullptr);
    command_list->ClearDepthStencilView(renderer_data.depth_stencil->GetCpuHandle(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0,
                                        nullptr);

    D3D12_VIEWPORT viewport = {};
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<float>(renderer_data.window->GetWidth());
    viewport.Height = static_cast<float>(renderer_data.window->GetHeight());
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    command_list->RSSetViewports(1, &viewport);

    const D3D12_RECT rect = {
        0, 0, static_cast<LONG>(renderer_data.window->GetWidth()), static_cast<LONG>(renderer_data.window->GetHeight())
    };
    command_list->RSSetScissorRects(1, &rect);
}

void SceneRenderer::EndRenderTarget(const RendererData& renderer_data)
{
    auto command_list = renderer_data.command_list->GetCommandList();

    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = renderer_data.swap_chain->GetCurrentBackBuffer();
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    command_list->ResourceBarrier(1, &barrier);
}
