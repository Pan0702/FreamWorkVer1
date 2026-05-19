#include "application.h"
#include "../Platform/window.h"
#include "../Graphics/graphics_device.h"
#include "../Graphics/swap_chain.h"
#include "../Graphics/command_queue.h"
#include "../Graphics/command_list.h"
#include "../Graphics/shader.h"
#include "../Graphics/root_signature.h"
#include "../Graphics/pipeline_state.h"
#include "../Graphics/vertex_buffer.h"
#include "../Graphics/index_buffer.h"

struct Vertex
{
    float position[3];
    float color[3];
};

Application::Application() = default;

Application::~Application() = default;

bool Application::Initialize(const wchar_t* title, uint32_t width, uint32_t height)
{
    window_ = std::make_unique<Window>();
    if (!window_->Create(title, width, height))
    {
        MessageBox(nullptr, L"Failed to create window", L"Error", MB_OK);
        return false;
    }

    graphics_device_ = std::make_unique<GraphicsDevice>();
#ifdef _DEBUG
    constexpr bool enable_debug = true;
#else
    constexpr bool enable_debug = false;
#endif
    if (!graphics_device_->Initialize(enable_debug))
    {
        MessageBox(nullptr, L"Failed to create graphics device", L"Error", MB_OK);
        return false;
    }

    command_queue_ = std::make_unique<CommandQueue>();
    if (!command_queue_->Initialize(graphics_device_->GetDevice()))
    {
        MessageBox(nullptr, L"Failed to create command queue", L"Error", MB_OK);
        return false;
    }

    swap_chain_ = std::make_unique<SwapChain>();
    if (!swap_chain_->Initialize(graphics_device_->GetFactory(), graphics_device_->GetDevice(),
                                 command_queue_->GetCommandQueue(), window_->GetHwnd(), width, height))
    {
        MessageBox(nullptr, L"Failed to create swap chain", L"Error", MB_OK);
        return false;
    }

    command_list_ = std::make_unique<CommandList>();
    if (!command_list_->Initialize(graphics_device_->GetDevice()))
    {
        MessageBox(nullptr, L"Failed to create command list", L"Error", MB_OK);
        return false;
    }

    root_signature_ = std::make_unique<RootSignature>();
    if
    (!root_signature_->Initialize(graphics_device_->GetDevice()))
    {
        MessageBox(nullptr, L"Failed to create root signature", L"Error", MB_OK);
        return false;
    }

    vertex_shader_ = std::make_unique<Shader>();
    if (!vertex_shader_->LoadFromFile(L"Shaders/triangle.vs.hlsl", "VSMain", "vs_5_0"))
    {
        MessageBox(nullptr, L"Failed to load vertex shader", L"Error", MB_OK);
        return false;
    }

    pixel_shader_ = std::make_unique<Shader>();
    if (!pixel_shader_->LoadFromFile(L"Shaders/triangle.ps.hlsl", "PSMain", "ps_5_0"))
    {
        MessageBox(nullptr, L"Failed to load pixel shader", L"Error", MB_OK);
        return false;
    }

    pipeline_state_ = std::make_unique<PipelineState>();
    if (!pipeline_state_->Initialize(graphics_device_->GetDevice(), root_signature_->GetRootSignature(),
                                     *vertex_shader_, *pixel_shader_))
    {
        MessageBox(nullptr, L"Failed to create pipeline state", L"Error", MB_OK);
        return false;
    }

    //三角形用のやつ
    Vertex vertices[] = {
        {{0.0f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}}, // 上-赤
        {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // 右下-緑
        {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}, // 左下-青
    };
    
    
    //立方体の描画
    Vertex cube_vertices[8] = {
        {{-0.3f, -0.3f, -0.3f}, {1, 0, 0}},
        {{+0.3f, -0.3f, -0.3f}, {0, 1, 0}},
        {{+0.3f, +0.3f, -0.3f}, {0, 0, 1}}, 
        {{-0.3f, +0.3f, -0.3f}, {1, 1, 0}}, 
        {{-0.3f, -0.3f, +0.3f}, {1, 0, 1}},
        {{+0.3f, -0.3f, +0.3f}, {0, 1, 1}}, 
        {{+0.3f, +0.3f, +0.3f}, {0.5, 0.5, 0.5}}, 
        {{-0.3f, +0.3f, +0.3f}, {1, 1, 1}}
    };
    
    vertex_buffer_ = std::make_unique<VertexBuffer>();
    if (!vertex_buffer_->Initialize(graphics_device_->GetDevice(), cube_vertices, sizeof(cube_vertices), sizeof(cube_vertices)))
    {
        MessageBox(nullptr, L"Failed to create vertex buffer", L"Error", MB_OK);
        return false;
    }
    
    uint16_t cube_indices[36] = {
        0, 3, 2, 0, 2, 1, // 前面 (-Z)
        4, 5, 6, 4, 6, 7, // 背面 (+Z)
        0, 4, 7, 0, 7, 3, // 左面 (-X)
        1, 2, 6, 1, 6, 5, // 右面 (+X)
        0, 1, 5, 0, 5, 4, // 下面 (-Y)
        3, 7, 6, 3, 6, 2, // 上面 (+Y)
    };
    index_buffer_ = std::make_unique<IndexBuffer>();
    DXGI_FORMAT format = DXGI_FORMAT_R32_UINT;
    if (!index_buffer_->Initialize(graphics_device_->GetDevice(), cube_indices, sizeof(cube_indices), format))
    {
        MessageBox(nullptr, L"Failed to create index buffer", L"Error", MB_OK);
        return false;
    }
    
    window_->Show();
    return true;
}

void Application::Run()
{
    while (window_->ProcessMessages())
    {
        Update();
        Render();
    }
    WaitForGPU();
}

void Application::Shutdown()
{
}

void Application::Update()
{
}

void Application::Render()
{
    if (!command_list_->Reset())
    {
        MessageBox(nullptr, L"Failed to reset command list", L"Error", MB_OK);
        return;
    }

    auto command_list = command_list_->GetCommandList();
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = swap_chain_->GetCurrentBackBuffer();
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    command_list->ResourceBarrier(1, &barrier);

    //背景の色を変える
    D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle = swap_chain_->GetCurrentRtvHandle();
    command_list->OMSetRenderTargets(1, &rtv_handle, false, nullptr);
    //decided color
    constexpr float clear_color[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    command_list->ClearRenderTargetView(rtv_handle, clear_color, 0, nullptr);

    //NDC の三角形を、ウィンドウの 0,0 〜 width,heightのピクセル領域に引き伸ばす」設定
    D3D12_VIEWPORT viewport = {};
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<float>(window_->GetWidth());
    viewport.Height = static_cast<float>(window_->GetHeight());
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    constexpr UINT Viewport_Num = 1;
    command_list->RSSetViewports(Viewport_Num, &viewport);

    //描画に必要な状態を全部 Set してから、Drawする
    constexpr UINT num_rects = 1;
    const D3D12_RECT rects[num_rects] = {
        {0, 0, static_cast<LONG>(window_->GetWidth()), static_cast<LONG>(window_->GetHeight())}
    };
    command_list->RSSetScissorRects(num_rects, rects);
    command_list->SetGraphicsRootSignature(root_signature_->GetRootSignature());
    command_list->SetPipelineState(pipeline_state_->GetPipelineState());
    command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    D3D12_VERTEX_BUFFER_VIEW vbv = vertex_buffer_->GetVertexBufferView();
    command_list->IASetVertexBuffers(0, 1, &vbv);
    //三角形描画
    //command_list->DrawInstanced(3, 1, 0, 0);

    //立方体描画
    command_list->DrawIndexedInstanced(36, 1, 0, 0, 0);


    
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    command_list->ResourceBarrier(1, &barrier);

    if (!command_list_->Close())
    {
        MessageBox(nullptr, L"Failed to close command list", L"Error", MB_OK);
        return;
    }

    ID3D12CommandList* command_lists[] = {command_list};
    command_queue_->GetCommandQueue()->ExecuteCommandLists(1, command_lists);
    swap_chain_->Present();
    command_queue_->WaitIdle();
}

void Application::WaitForGPU() const
{
    command_queue_->WaitIdle();
}
