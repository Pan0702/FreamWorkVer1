#include "debug.h"

#include "../Renderer/sprite_draw_command.h"
#include "../Renderer/sprite_renderer.h"
#include "../Renderer/ui_renderer.h"
#include "../Renderer/debug_line_renderer.h"
#include <cmath>

namespace
{
    constexpr float kHalf = 0.5f;
}

Debug& Debug::Get()
{
    static Debug instance;
    return instance;
}

void Debug::Initialize(SpriteRenderer* sprite_renderer, UIRenderer* ui_renderer
                       , DebugLineRenderer* debug_line_renderer)
{
    sprite_renderer_ = sprite_renderer;
    ui_renderer_ = ui_renderer;
    line_renderer_ = debug_line_renderer;
}

void Debug::Shutdown()
{
    sprite_renderer_ = nullptr;
    ui_renderer_ = nullptr;
}

void Debug::Log(const char* format, ...)
{
    char buffer[2048] = {};

    va_list args;
    va_start(args, format);
    vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, format, args);
    va_end(args);

    OutputDebugStringA(buffer);
    OutputDebugStringA("\n");
}

void Debug::Print(const char* format, ...)
{
    char buffer[2048] = {};

    va_list args;
    va_start(args, format);
    vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, format, args);
    va_end(args);

    Log("%s", buffer);
}

void Debug::DrawSprite(Texture2D* texture, const Vec2& position, const Vec2& size, const Vec4& color)
{
    if (ui_renderer_ == nullptr || texture == nullptr)
    {
        return;
    }

    SpriteDrawCommand command = {};
    command.texture = texture;
    command.position = Vec2(position.x + size.x * kHalf, position.y + size.y * kHalf);
    command.size = size;
    command.color = color;
    command.use_texture = true;
    ui_renderer_->DrawImmediate(command);
}

void Debug::DrawBox(const Vec2& position, const Vec2& size, const Vec4& color, float rotation)
{
    if (ui_renderer_ == nullptr)
    {
        return;
    }

    SpriteDrawCommand command = {};
    command.position = Vec2(position.x + size.x * kHalf, position.y + size.y * kHalf);
    command.size = size;
    command.color = color;
    command.rotation = rotation;
    command.use_texture = false;
    ui_renderer_->DrawImmediate(command);
}

void Debug::DrawLine(const Vec2& start, const Vec2& end, const Vec4& color)
{
    if (ui_renderer_ == nullptr)
    {
        return;
    }

    const float dx = end.x - start.x;
    const float dy = end.y - start.y;
    const float length = std::sqrt(dx * dx + dy * dy);
    SpriteDrawCommand command = {};
    command.position = Vec2((start.x + end.x) * kHalf, (start.y + end.y) * kHalf);
    command.size = Vec2(length, 2.0f);
    command.color = color;
    command.rotation = std::atan2(dy, dx);
    command.use_texture = false;
    ui_renderer_->DrawImmediate(command);
}

void Debug::DrawCircle(const Vec2& center, float radius, const Vec4& color)
{
    constexpr int kSegments = 32;
    for (int i = 0; i < kSegments; ++i)
    {
        const float a0 = k2PI * static_cast<float>(i) / static_cast<float>(kSegments);
        const float a1 = k2PI * static_cast<float>(i + 1) / static_cast<float>(kSegments);
        const Vec2 p0(center.x + std::cos(a0) * radius, center.y + std::sin(a0) * radius);
        const Vec2 p1(center.x + std::cos(a1) * radius, center.y + std::sin(a1) * radius);
        DrawLine(p0, p1, color);
    }
}

void Debug::DrawLine3D(const Vec3& start, const Vec3& end, const Vec4& color)
{
    if (line_renderer_ == nullptr)
        return;

    line_renderer_->AddLine(start, end, color);
}

void Debug::DrawBox3D(const Vec3& center, const Vec3& size, const Vec4& color, const Vec3& rotation, bool fill)
{
    if (line_renderer_ == nullptr)
        return;

    Mat rot_mat = RotateX(rotation.x) * RotateY(rotation.y) * RotateZ(rotation.z);
    const Vec3 h = size * 0.5f;
    Vec3 corners[8] = {};
    for (int xi = 0; xi < 2; ++xi)
    {
        for (int yi = 0; yi < 2; ++yi)
        {
            for (int yz = 0; yz < 2; ++yz)
            {
                Vec3 local(xi ? -h.x : h.x, yi ? -h.y : h.y, yz ? -h.z : h.z);
                const int index = xi * 4 + yi * 2 + yz;
                corners[index] = Vec3(XMVector3Transform(local, rot_mat)) + center;
            }
        }
    }

    constexpr int edges[12][2] = {
        {0, 1}, {2, 3}, {4, 5}, {6, 7}, 
        {0, 2}, {1, 3}, {4, 6}, {5, 7}, 
        {0, 4}, {1, 5}, {2, 6}, {3, 7}, 
    };

    for (auto& e : edges)
    {
        line_renderer_->AddLine(corners[e[0]], corners[e[1]], color);
    }

    if (fill)
    {
        constexpr int faces[6][4] = {
            {0, 1, 3, 2}, 
            {4, 5, 7, 6}, 
            {0, 1, 5, 4}, 
            {2, 3, 7, 6}, 
            {0, 2, 6, 4}, 
            {1, 3, 7, 5}, 
        };
        for (auto& f : faces)
        {
            line_renderer_->AddTriangle(corners[f[0]], corners[f[1]], corners[f[2]], color);
            line_renderer_->AddTriangle(corners[f[0]], corners[f[2]], corners[f[3]], color);
        }
    }
}

void Debug::DrawSphere3D(const Vec3& center, float radius, const Vec4& color)
{
    if (sprite_renderer_ == nullptr)
        return;
    
    constexpr int kSegments = 32;
    for (int i = 0; i < kSegments; ++i)
    {
        const float a0 = k2PI * static_cast<float>(i) / static_cast<float>(kSegments);
        const float a1 = k2PI * static_cast<float>(i + 1) / static_cast<float>(kSegments);
        const float c0 = std::cos(a0), s0 = std::sin(a0);
        const float c1 = std::cos(a1), s1 = std::sin(a1);
        line_renderer_->AddLine(center + Vec3(c0,s0,0.0f) * radius,
            center + Vec3(c1,s1,0.0f) * radius, color);
        line_renderer_->AddLine(center + Vec3(c0,0.0f,s0) * radius,
            center + Vec3(c1,0.0f,s1) * radius, color);
        line_renderer_->AddLine(center + Vec3(0.0f,c0,s0) * radius,
            center + Vec3(0.0f,c1,s1) * radius, color);
    }
}

void Debug::DrawSprite3D(Texture2D* texture, const Mat& mat, const Vec2& size, const Vec2& src_pos,
    const Vec2& src_size, float alpha)
{
    if (sprite_renderer_ == nullptr)
    {
        return;
    }

    SpriteDrawCommand command = {};
    command.texture = texture;
    command.world = mat;
    command.size = size;
    command.src_rect = Vec4(src_pos.x, src_pos.y, src_size.x, src_size.y);
    command.color = Vec4(1.0f, 1.0f, 1.0f, alpha);
    command.use_texture = texture != nullptr;
    sprite_renderer_->DrawImmediate(command);
}

void Debug::DrawSprite3D( Texture2D* texture, const Vec3& position, const Vec3& rotation,const Vec2& size,
                    const Vec2& src_pos, const Vec2& src_size, float alpha)
{
    const Mat world = RotateX(rotation.x) * RotateY(rotation.y) * RotateZ(rotation.z) * Translate(position);
    DrawSprite3D(texture, world, size, src_pos, src_size, alpha);
}



void Debug::Watch(const char* name, float value)
{
    Log("%s: %.3f", name, value);
}
