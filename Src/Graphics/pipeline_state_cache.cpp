#include "pipeline_state_cache.h"

#include "pipeline_state.h"
#include "../Resource/vertex_types.h"
#include "root_signature.h"
#include "shader.h"

bool PipelineStateCache::Initialize(ID3D12Device* device)
{
    if (!device)
    {
        return false;
    }
    // RS は VertexFactory だけで決まるので、PSO より先に 1 回だけ作る。
    if (!BuidRootSignature(device))
    {
        return false;   
    }
    //ShaderID×VertexFactoryIDの全組み合わせのPSOを事前作成
    for (uint32 i = 0; i < ToIndex(ShaderId::kCount); ++i)
    {
        for (uint32 j = 0; j < ToIndex(VertexFactoryId::kCount); ++j)
        {
            if (!BuildPipelineState(device, static_cast<ShaderId>(i), static_cast<VertexFactoryId>(j)))
            {
                return false;
            }
        }
    }
    return true;
}

ID3D12RootSignature* PipelineStateCache::GetRootSignature(VertexFactoryId factory)
{
    switch (factory)
    {
    case VertexFactoryId::kStatic:
    case VertexFactoryId::kInstanced:
        return static_rs_->GetRootSignature();
    case VertexFactoryId::kSkinned:
        return skinned_rs_->GetRootSignature();
    default:
        return nullptr;
    }
}

PipelineState* PipelineStateCache::Get(ShaderId shader, VertexFactoryId factory)
{
    return pso_[ToIndex(shader)][ToIndex(factory)].get();
}

bool PipelineStateCache::BuidRootSignature(ID3D12Device* device)
{
    // static 系（Static/Instanced 共有）。t6 はインスタンス行列用の root SRV。
    static_rs_ = std::make_unique<RootSignature>();
    RootSignatureBuilder builder_static;
    builder_static
        .AddCbv(0, D3D12_SHADER_VISIBILITY_VERTEX) //b0
        .AddCbv(1, D3D12_SHADER_VISIBILITY_PIXEL) // b1
        .AddCbv(2, D3D12_SHADER_VISIBILITY_PIXEL) // b2 
        .AddSrvTable(0, 1, D3D12_SHADER_VISIBILITY_PIXEL) // t0 : diffus
        .AddSrvTable(1, 1, D3D12_SHADER_VISIBILITY_PIXEL) // t1 :  normal
        .AddSrvTable(2, 1, D3D12_SHADER_VISIBILITY_PIXEL) // t2 : shadow    
        .AddSrvTable(3, 1, D3D12_SHADER_VISIBILITY_PIXEL) // t3 : irradiance
        .AddSrvTable(4, 1, D3D12_SHADER_VISIBILITY_PIXEL) // t4 : specular
        .AddSrvTable(5, 1, D3D12_SHADER_VISIBILITY_PIXEL) // t5 : height
        .AddSrv(6, D3D12_SHADER_VISIBILITY_VERTEX)
        .AddStaticSampler(0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_TEXTURE_ADDRESS_MODE_WRAP)
        .AddComparisonSampler(1, D3D12_SHADER_VISIBILITY_PIXEL); // s1

    if (!builder_static.Build(device, static_rs_.get()))
    {
        MessageBox(nullptr, L"Failed to create root signature", L"Error", MB_OK);
        return false;
    }

    // skinned 用。b1(VS) の BoneCB が増える分だけ static 系と並びがずれる。
    skinned_rs_ = std::make_unique<RootSignature>();
    RootSignatureBuilder builder_skinned;
    builder_skinned
        .AddCbv(0, D3D12_SHADER_VISIBILITY_VERTEX)
        .AddCbv(1, D3D12_SHADER_VISIBILITY_VERTEX)
        .AddCbv(1, D3D12_SHADER_VISIBILITY_PIXEL)
        .AddCbv(2, D3D12_SHADER_VISIBILITY_PIXEL)
        .AddSrvTable(0, 1, D3D12_SHADER_VISIBILITY_PIXEL)
        .AddSrvTable(1, 1, D3D12_SHADER_VISIBILITY_PIXEL)
        .AddSrvTable(2, 1, D3D12_SHADER_VISIBILITY_PIXEL)
        .AddSrvTable(3, 1, D3D12_SHADER_VISIBILITY_PIXEL)
        .AddSrvTable(4, 1, D3D12_SHADER_VISIBILITY_PIXEL)
        .AddSrvTable(5, 1, D3D12_SHADER_VISIBILITY_PIXEL)
        .AddStaticSampler(0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_TEXTURE_ADDRESS_MODE_WRAP)
        .AddComparisonSampler(1, D3D12_SHADER_VISIBILITY_PIXEL);

    if (!builder_skinned.Build(device, skinned_rs_.get()))
    {
        return false;
    }

    return true;
}

bool PipelineStateCache::BuildPipelineState(ID3D12Device* device, ShaderId shader, VertexFactoryId factory)
{
    //Factoryごとの差分
    struct FactoryDecs
    {
        const wchar_t* vs_path;
        const char* vs_model;
        const D3D12_INPUT_ELEMENT_DESC* input_layout;
        UINT layout_count;
    };

    static const FactoryDecs kFactories[] = {
        {.vs_path = L"Shaders/triangle.vs.hlsl", .vs_model = "vs_5_0", .input_layout = kStaticVertexLayout,
            .layout_count = _countof(kStaticVertexLayout)},
        {.vs_path = L"Shaders/instanced_mesh.vs.hlsl", .vs_model = "vs_5_1", .input_layout = kStaticVertexLayout,
            .layout_count = _countof(kStaticVertexLayout)},
        {.vs_path = L"Shaders/skinned.vs.hlsl", .vs_model = "vs_5_0", .input_layout = kSkinnedVertexLayout,
            .layout_count = _countof(kSkinnedVertexLayout)},
    };
    
    static const wchar_t* kPixelShader[] = {
        L"Shaders/triangle.ps.hlsl",
    };
    
    const FactoryDecs& fd = kFactories[ToIndex(factory)];

    //VSはFactory、PSはShaderが決める
    Shader vs;
    if (!vs.LoadFromFile(fd.vs_path, "VSMain", fd.vs_model))
    {
        printf("PSO cache : Failed to load VS %ls\n", fd.vs_path);
        return false;
    }
    Shader ps;
    if (!ps.LoadFromFile(kPixelShader[ToIndex(shader)], "PSMain", "ps_5_0"))
    {
        printf("PSO cache : Failed to load PS %ls\n", kPixelShader[ToIndex(shader)]);
        return false;
    }
    
    D3D12_INPUT_LAYOUT_DESC layout_desc = {};
    layout_desc.pInputElementDescs = fd.input_layout;
    layout_desc.NumElements = fd.layout_count;
    auto pso = std::make_unique<PipelineState>();
    PipelineStateBuilder ps_builder;
    ps_builder
        .SetInputLayout(layout_desc)
        .SetRootSignature(GetRootSignature(factory))
        .SetVertexShader(vs.GetBytecode())
        .SetPixelShader(ps.GetBytecode());
    
    if (factory == VertexFactoryId::kSkinned)
    {
        ps_builder
        .SetDepthEnabled(true)
        .SetAlphaBlendEnabled(false);
    }
    
    if (!ps_builder.Build(device, pso.get()))
    {
        printf("PSO cache : failed to build PSO (shader %u factory %u)", ToIndex(shader), ToIndex(factory));
        return false;
    }
    
    pso_[ToIndex(shader)][ToIndex(factory)] = std::move(pso);
    return true;   
}