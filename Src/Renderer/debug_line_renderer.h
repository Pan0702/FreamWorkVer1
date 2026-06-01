#pragma once
#include <memory>
#include <vector>
#include "../Core/common.h"
#include "../Core/Math/my_math.h"
#include "../Graphics/pipeline_state.h"
#include "../Graphics/root_signature.h"
#include "../Graphics/shader.h"

struct RenderContext;

class DebugLineRenderer
{
public:
    bool Initialize(ID3D12Device* device);
    void Shutdown();
    void AddLine(const Vec3& start, const Vec3& end, const Vec4& color);            
    void AddTriangle(const Vec3& a, const Vec3& b, const Vec3& c, const Vec4& color); 
    void Submit(RenderContext& context);                                            
    void Clear();                                                                 

private:
    struct DebugLineVertex
    {
        float position[3];
        float color[4];
    };


    bool CreateDynamicVertexBuffer(ID3D12Device* device, uint32_t buffer_size,
                                   ComPtr<ID3D12Resource>& out_buffer, void*& out_mapped,
                                   D3D12_GPU_VIRTUAL_ADDRESS& out_gpu_address);

    std::unique_ptr<Shader> vs_;
    std::unique_ptr<Shader> ps_;
    std::unique_ptr<RootSignature> root_signature_;
    std::unique_ptr<PipelineState> pipeline_state_;    
    std::unique_ptr<PipelineState> tri_pipeline_state_;

    uint32_t capacity_ = 0;

    //ê¸//
    ComPtr<ID3D12Resource> vertex_buffer_;
    void* mapped_ = nullptr;
    D3D12_GPU_VIRTUAL_ADDRESS gpu_address_ = 0;
    std::vector<DebugLineVertex> vertices_;

    //ìhÇËÇ¬Ç‘ÇµéOäpå`//
    ComPtr<ID3D12Resource> tri_vertex_buffer_;
    void* tri_mapped_ = nullptr;
    D3D12_GPU_VIRTUAL_ADDRESS tri_gpu_address_ = 0;
    std::vector<DebugLineVertex> tri_vertices_;
};