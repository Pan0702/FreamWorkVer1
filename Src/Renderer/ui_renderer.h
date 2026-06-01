#pragma once
#include <memory>
#include <vector>
#include <memory>
#include <vector>
#include "../Core/common.h"
#include "../Core/Math/my_math.h"
#include "../Graphics/index_buffer.h"
#include "../Graphics/pipeline_state.h"
#include "../Graphics/root_signature.h"
#include "../Graphics/shader.h"
#include "../Graphics/vertex_buffer.h"
#include "sprite_draw_command.h"
#include "../Engine/Components/sprite_component.h"

struct RenderContext;

struct UISpriteCBData
{
    Vec2 sprite_pos;
    Vec2 sprite_size;
    Vec4 color;
    Vec2 screen_size;   
    float rotation;
    float use_texture;
};
class UIRenderer
{
public:
    bool Initialize(ID3D12Device* device);
    void Shutdown();
    void Register(SpriteComponent* component);
    void Unregister(SpriteComponent* component);
    void DrawImmediate(const SpriteDrawCommand& command);
    void Collect();
    void Sort();
    void Submit(RenderContext& context);

private:
    void SubmitCommand(RenderContext& context, const SpriteDrawCommand& command);

    std::unique_ptr<VertexBuffer> quad_vb_;
    std::unique_ptr<IndexBuffer> quad_ib_;
    std::unique_ptr<Shader> vertex_shader_;
    std::unique_ptr<Shader> pixel_shader_;
    std::unique_ptr<RootSignature> root_signature_;
    std::unique_ptr<PipelineState> pipeline_state_;
    std::vector<SpriteComponent*> registered_;
    std::vector<SpriteDrawCommand> draw_commands_;
    std::vector<SpriteDrawCommand> immediate_commands_;
};
