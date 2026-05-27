#include "scene_renderer.h"

#include "mesh_renderer.h"
#include "../Engine/camera.h"
#include "../Graphics/command_list.h"
#include "../Graphics/swap_chain.h"
#include "../Graphics/depth_stencil.h"
#include "scene.h"

bool SceneRenderer::Initialize(ID3D12Device* device, HWND hwnd, ID3D12CommandQueue* command_queue, uint32_t frame_count)
{
    mesh_renderer_ = std::make_unique<MeshRenderer>();
    if (!mesh_renderer_->Initialize(device))
    {
        return false;
    }
    
    return imgui_manager_.Initialize(hwnd, device, command_queue, static_cast<int>(frame_count));
}

void SceneRenderer::Render(RendererData& renderer_data, Scene* scene, Camera* camera)
{
    imgui_manager_.BeginFrame();
    ImGui::ShowDemoWindow();
    
    auto command_list = renderer_data.command_list->GetCommandList();
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = renderer_data.swap_chain->GetCurrentBackBuffer();
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    command_list->ResourceBarrier(1, &barrier);

    //背景の色を変える
    D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle = renderer_data.swap_chain->GetCurrentRtvHandle();
    D3D12_CPU_DESCRIPTOR_HANDLE dsv_handle = renderer_data.depth_stencil->GetCpuHandle();
    command_list->OMSetRenderTargets(1, &rtv_handle, false, &dsv_handle);

    //decided color
    constexpr float clear_color[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    command_list->ClearRenderTargetView(rtv_handle, clear_color, 0, nullptr);
    command_list->ClearDepthStencilView(renderer_data.depth_stencil->GetCpuHandle(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0,
                                        nullptr);
    //NDC の三角形を、ウィンドウの 0,0 〜 width,heightのピクセル領域に引き伸ばす」設定
    D3D12_VIEWPORT viewport = {};
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<float>(renderer_data.window->GetWidth());
    viewport.Height = static_cast<float>(renderer_data.window->GetHeight());
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    constexpr UINT Viewport_Num = 1;
    command_list->RSSetViewports(Viewport_Num, &viewport);

    //描画に必要な状態を全部 Set してから、Drawする
    constexpr UINT num_rects = 1;
    const D3D12_RECT rects[num_rects] = {
        {0, 0, static_cast<LONG>(renderer_data.window->GetWidth()), static_cast<LONG>(renderer_data.window->GetHeight())}
    };
    command_list->RSSetScissorRects(num_rects, rects);
    
    command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    scene->Sort();
    mesh_renderer_->Render(command_list,scene->GetRenderObjects(),camera, renderer_data.srv_heap);
    imgui_manager_.EndFrame(renderer_data.command_list->GetCommandList());

    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    command_list->ResourceBarrier(1, &barrier);
    
}

void SceneRenderer::Shutdown()
{
    imgui_manager_.Shutdown(); 
}
