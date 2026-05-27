#include "application.h"
#include "camera.h"
#include "../Platform/window.h"
#include "../Graphics/graphics_device.h"
#include "../Graphics/swap_chain.h"
#include "../Graphics/command_queue.h"
#include "../Graphics/command_list.h"
#include "../Renderer/scene.h"
#include "../Graphics/depth_stencil.h"
#include "../Core/Math/my_math.h"
#include "../Platform/input.h"
#include "../Renderer/scene_renderer.h"
#include  "../Resource/texture2D.h"
#include "../Resource/mesh.h"
#include "../Resource/material.h"
#include "../Resource/vertex_types.h"

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


    StaticVertex cube_vertices[24] = {
        // ===== 前面 (-Z) =====  法線は (0, 0, -1)
        {{-0.3f, -0.3f, -0.3f}, {0, 0, -1}, {0.0f, 1.0f}, {0, 0, 0}, {0, 0, 0}},
        {{-0.3f, +0.3f, -0.3f}, {0, 0, -1}, {0.0f, 0.0f}, {0, 0, 0}, {0, 0, 0}},
        {{+0.3f, +0.3f, -0.3f}, {0, 0, -1}, {1.0f, 0.0f}, {0, 0, 0}, {0, 0, 0}},
        {{+0.3f, -0.3f, -0.3f}, {0, 0, -1}, {1.0f, 1.0f}, {0, 0, 0}, {0, 0, 0}},

        // ===== 背面 (+Z) =====  法線は (0, 0, +1)
        {{+0.3f, -0.3f, +0.3f}, {0, 0, +1}, {0.0f, 1.0f}, {0, 0, 0}, {0, 0, 0}},
        {{+0.3f, +0.3f, +0.3f}, {0, 0, +1}, {0.0f, 0.0f}, {0, 0, 0}, {0, 0, 0}},
        {{-0.3f, +0.3f, +0.3f}, {0, 0, +1}, {1.0f, 0.0f}, {0, 0, 0}, {0, 0, 0}},
        {{-0.3f, -0.3f, +0.3f}, {0, 0, +1}, {1.0f, 1.0f}, {0, 0, 0}, {0, 0, 0}},

        // ===== 左面 (-X) =====  法線は (-1, 0, 0)
        {{-0.3f, -0.3f, +0.3f}, {-1, 0, 0}, {0.0f, 1.0f}, {0, 0, 0}, {0, 0, 0}},
        {{-0.3f, +0.3f, +0.3f}, {-1, 0, 0}, {0.0f, 0.0f}, {0, 0, 0}, {0, 0, 0}},
        {{-0.3f, +0.3f, -0.3f}, {-1, 0, 0}, {1.0f, 0.0f}, {0, 0, 0}, {0, 0, 0}},
        {{-0.3f, -0.3f, -0.3f}, {-1, 0, 0}, {1.0f, 1.0f}, {0, 0, 0}, {0, 0, 0}},

        // ===== 右面 (+X) =====  法線は (+1, 0, 0)
        {{+0.3f, -0.3f, -0.3f}, {+1, 0, 0}, {0.0f, 1.0f}, {0, 0, 0}, {0, 0, 0}},
        {{+0.3f, +0.3f, -0.3f}, {+1, 0, 0}, {0.0f, 0.0f}, {0, 0, 0}, {0, 0, 0}},
        {{+0.3f, +0.3f, +0.3f}, {+1, 0, 0}, {1.0f, 0.0f}, {0, 0, 0}, {0, 0, 0}},
        {{+0.3f, -0.3f, +0.3f}, {+1, 0, 0}, {1.0f, 1.0f}, {0, 0, 0}, {0, 0, 0}},

        // ===== 下面 (-Y) =====  法線は (0, -1, 0)
        {{-0.3f, -0.3f, +0.3f}, {0, -1, 0}, {0.0f, 1.0f}, {0, 0, 0}, {0, 0, 0}},
        {{-0.3f, -0.3f, -0.3f}, {0, -1, 0}, {0.0f, 0.0f}, {0, 0, 0}, {0, 0, 0}},
        {{+0.3f, -0.3f, -0.3f}, {0, -1, 0}, {1.0f, 0.0f}, {0, 0, 0}, {0, 0, 0}},
        {{+0.3f, -0.3f, +0.3f}, {0, -1, 0}, {1.0f, 1.0f}, {0, 0, 0}, {0, 0, 0}},

        // ===== 上面 (+Y) =====  法線は (0, +1, 0)
        {{-0.3f, +0.3f, -0.3f}, {0, +1, 0}, {0.0f, 1.0f}, {0, 0, 0}, {0, 0, 0}},
        {{-0.3f, +0.3f, +0.3f}, {0, +1, 0}, {0.0f, 0.0f}, {0, 0, 0}, {0, 0, 0}},
        {{+0.3f, +0.3f, +0.3f}, {0, +1, 0}, {1.0f, 0.0f}, {0, 0, 0}, {0, 0, 0}},
        {{+0.3f, +0.3f, -0.3f}, {0, +1, 0}, {1.0f, 1.0f}, {0, 0, 0}, {0, 0, 0}},
    };

    VertexData vertex_data = {};
    vertex_data.data = cube_vertices;
    vertex_data.total_size = sizeof(cube_vertices);
    vertex_data.stride = sizeof(cube_vertices[0]);

    uint16_t cube_indices[36] = {
        0, 1, 2, 0, 2, 3, // 前面
        4, 5, 6, 4, 6, 7, // 背面 
        8, 9, 10, 8, 10, 11, // 左面 
        12, 13, 14, 12, 14, 15, // 右面 
        16, 17, 18, 16, 18, 19, // 下面 
        20, 21, 22, 20, 22, 23, // 上面 
    };

    IndexData index_data = {};
    index_data.data = cube_indices;
    index_data.total_size = sizeof(cube_indices);
    index_data.format = DXGI_FORMAT_R16_UINT;
    

    mesh_ = std::make_unique<Mesh>();
    if (!mesh_->Create(graphics_device_->GetDevice(), vertex_data, index_data, kStaticVertexLayout))
    {
        MessageBox(nullptr, L"Failed to create mesh", L"Error", MB_OK);
        return false;
    }
    
    depth_stencil_ = std::make_unique<DepthStencil>();
    if (!depth_stencil_->Initialize(graphics_device_->GetDevice(), window_->GetWidth(), window_->GetHeight()))
    {
        MessageBox(nullptr, L"Failed to create depth stencil", L"Error", MB_OK);
        return false;
    }
    srv_heap_ = std::make_unique<DescriptorHeap>();
    if (!srv_heap_->Initialize(graphics_device_->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1, true))
    {
        MessageBox(nullptr, L"Failed to create descriptor heap", L"Error", MB_OK);
        return false;
    }
    LoadedImage image;
    if (!TextureLoader::LoadFromFile(L"Texture/test.png", image))
    {
        MessageBox(nullptr, L"Failed to load texture", L"Error", MB_OK);
        return false;
    }
    if (!command_list_->Reset())
    {
        MessageBox(nullptr, L"Failed to reset command list", L"Error", MB_OK);
        return false;
    }
    texture_ = std::make_unique<Texture2D>();
    if (!texture_->Initialize(graphics_device_->GetDevice(), command_queue_->GetCommandQueue(),
                              command_list_->GetCommandList(), nullptr, image))
    {
        MessageBox(nullptr, L"Failed to create texture", L"Error", MB_OK);
        return false;
    }
    material_ = std::make_unique<Material>();
    material_->Create(graphics_device_->GetDevice(),L"Shaders/triangle.vs.hlsl", L"Shaders/triangle.ps.hlsl",mesh_->GetInputLayout());
    material_->SetDiffuse(texture_.get());
    texture_->CreateSrv(graphics_device_->GetDevice(), srv_heap_->GetCpuHandle(0));
    input_ = std::make_unique<Input>();
    input_->Initialize(window_->GetHwnd());
    camera_ = std::make_unique<Camera>();
    const float aspect = static_cast<float>(width) / static_cast<float>(height);
    camera_->Initialize(kHalfPi, aspect, 0.1f, 100.0f);

    window_->SetResizeCallback([this](uint32_t w, uint32_t h)
    {
        WaitForGPU();
        swap_chain_->Resize(w, h);
        depth_stencil_->Resize(w, h);
        camera_->SetAspect(static_cast<float>(w) / static_cast<float>(h));
    });
    scene_ = std::make_unique<Scene>();
    scene_renderer_ = std::make_unique<SceneRenderer>();
    if (!scene_renderer_->Initialize(graphics_device_->GetDevice(), window_->GetHwnd(),command_queue_->GetCommandQueue(),kFrameCount))
    {
        MessageBox(nullptr, L"Failed to create scene renderer", L"Error", MB_OK);
        return false;
    }
    render_object_ = std::make_unique<RenderObject>();
    render_object_->SetMesh(mesh_.get());
    render_object_->SetMaterial(material_.get());
    scene_->AddObject(render_object_.get());
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
    scene_renderer_->Shutdown();
}

void Application::Update()
{
    input_->Update();
    camera_->pos_ = Vec3(0, 0, 5.0f);
    camera_->look_ = Vec3(0, 0, -1.0f);
}

void Application::Render()
{
    if (!command_list_->Reset())
    {
        MessageBox(nullptr, L"Failed to reset command list", L"Error", MB_OK);
        return;
    }
    auto command_list = command_list_->GetCommandList();
    
    RendererData renderer_data = {};
    renderer_data.command_list = command_list_.get();
    renderer_data.command_queue = command_queue_.get();
    renderer_data.depth_stencil = depth_stencil_.get();
    renderer_data.srv_heap = srv_heap_.get();
    renderer_data.swap_chain = swap_chain_.get();
    renderer_data.window = window_.get();
    scene_renderer_->Render(renderer_data, scene_.get(), camera_.get());
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
