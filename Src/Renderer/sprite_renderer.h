#pragma once
#include "../Core/common.h"
#include "../Core/Math/my_math.h"

class RootSignature;
class Shader;
class PipelineState;
class IndexBuffer;
class VertexBuffer;
class ConstantBuffer;

struct SpriteCBData
{
    Vec2 sprite_pos;
    Vec2 sprite_size;
    Vec4 color;
    Vec2 screen_size;
    Vec2 _pad;
};
class SpriteRenderer
{
public:
    bool Initialize(ID3D12Device* device);
    void Shutdown();
    
private:
    std::unique_ptr<VertexBuffer> quad_vb_;
    std::unique_ptr<IndexBuffer> quad_ib_;
    std::unique_ptr<Shader> vertex_shader_;
    std::unique_ptr<Shader> pixel_shader_;
    std::unique_ptr<RootSignature> root_signature_;
    std::unique_ptr<PipelineState> pipeline_state_;
    std::unique_ptr<ConstantBuffer> constant_buffer_;
};
