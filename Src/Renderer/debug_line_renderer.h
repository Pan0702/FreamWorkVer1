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
    void AddLine(const Vec3& start, const Vec3& end, const Vec4& color); // Debug から呼ぶ
    void Submit(RenderContext& context);                                // 毎フレーム描画
    void Clear();                                                       // 描画後に貯めた線を捨てる

private:
    struct DebugLineVertex
    {
        float position[3];
        float color[4];
    };

    std::unique_ptr<Shader> vs_;
    std::unique_ptr<Shader> ps_;
    std::unique_ptr<RootSignature> root_signature_;
    std::unique_ptr<PipelineState> pipeline_state_;
    
    ComPtr<ID3D12Resource> vertex_buffer_;
    void* mapped_ = nullptr;                     
    D3D12_GPU_VIRTUAL_ADDRESS gpu_address_ = 0;
    uint32_t capacity_ = 0;                        
    std::vector<DebugLineVertex> vertices_;        
};