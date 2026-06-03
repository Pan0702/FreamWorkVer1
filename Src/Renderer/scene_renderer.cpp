#include "scene_renderer.h"

#include "debug_line_renderer.h"
#include "../Engine/camera.h"
#include "../Engine/world.h"
#include "../Graphics/command_list.h"
#include "../Graphics/depth_stencil.h"
#include "../Graphics/swap_chain.h"
#include "../Platform/window.h"
#include "render_context.h"
#include "scene.h"

bool SceneRenderer::Initialize(ID3D12Device* device, HWND hwnd, ID3D12CommandQueue* command_queue, uint32_t frame_count)
{
    mesh_renderer_ = std::make_unique<MeshRenderer>();
    if (!mesh_renderer_->Initialize(device))
    {
        return false;
    }

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

    return imgui_manager_.Initialize(hwnd, device, command_queue, static_cast<int>(frame_count));
}

void SceneRenderer::Render(RendererData& renderer_data, Scene* scene, Camera* camera)
{
    imgui_manager_.BeginFrame();
    ImGui::ShowDemoWindow();
    BeginRenderTarget(renderer_data);

    auto command_list = renderer_data.command_list->GetCommandList();
    command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    scene->Sort();
    mesh_renderer_->Render(command_list, scene->GetRenderObjects(), camera, renderer_data.srv_heap);

    imgui_manager_.EndFrame(command_list);
    EndRenderTarget(renderer_data);
}

void SceneRenderer::Render(RendererData& renderer_data, World* world, Camera* camera)
{
    (void)world;
    imgui_manager_.BeginFrame();
    ImGui::ShowDemoWindow();
    BeginRenderTarget(renderer_data);

    RenderContext context = {};
    context.command_list = renderer_data.command_list->GetCommandList();
    context.view = camera->GetViewMatrix();
    context.projection = camera->GetProjectionMatrix();
    context.srv_heap = renderer_data.srv_heap;
    context.cb_allocator = renderer_data.cb_allocator;
    context.screen_size = Vec2(static_cast<float>(renderer_data.window->GetWidth()),
                               static_cast<float>(renderer_data.window->GetHeight()));
    
    context.light_dir = Vec3(0.3f,-1.0f,0.5f).Normalized();
    context.light_color = Vec3(1.0f,1.0f,1.0f);
    context.ambient = Vec3(0.15f,0.15f,0.15f);

    mesh_renderer_->Collect();
    mesh_renderer_->Sort();
    mesh_renderer_->Submit(context);

    // �f�o�b�O�`��i���E���E���Ȃǁj�B���b�V���̌�ɕ`���Đ[�x�ŉB���悤�ɂ���
    debug_renderer_->Submit(context);

    sprite_renderer_->Collect();
    sprite_renderer_->Sort();
    sprite_renderer_->Submit(context);

    ui_renderer_->Collect();
    ui_renderer_->Sort();
    ui_renderer_->Submit(context);

    imgui_manager_.EndFrame(context.command_list);
    EndRenderTarget(renderer_data);
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
    imgui_manager_.Shutdown();
}

MeshRenderer* SceneRenderer::GetMeshRenderer()
{
    return mesh_renderer_.get();
}

SpriteRenderer* SceneRenderer::GetSpriteRenderer()
{
    return sprite_renderer_.get();
}

UIRenderer* SceneRenderer::GetUIRenderer()
{
    return ui_renderer_.get();
}

DebugLineRenderer* SceneRenderer::GetDebugLineRenderer()
{
    return debug_renderer_.get();
}

void SceneRenderer::BeginRenderTarget(RendererData& renderer_data)
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

void SceneRenderer::EndRenderTarget(RendererData& renderer_data)
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
