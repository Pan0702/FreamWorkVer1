#include "debug.h"

#include "../Renderer/sprite_draw_command.h"
#include "../Renderer/sprite_renderer.h"
#include "../Renderer/ui_renderer.h"

#include <cmath>

Debug& Debug::Get()
{
    static Debug instance;
    return instance;
}

void Debug::Initialize(SpriteRenderer* sprite_renderer, UIRenderer* ui_renderer)
{
    sprite_renderer_ = sprite_renderer;
    ui_renderer_ = ui_renderer;
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
    command.position = Vec2(position.x + size.x * 0.5f, position.y + size.y * 0.5f);
    command.size = size;
    command.color = color;
    command.use_texture = true;
    ui_renderer_->DrawImmediate(command);
}

void Debug::DrawBox(const Vec2& position, const Vec2& size, const Vec4& color)
{
    if (ui_renderer_ == nullptr)
    {
        return;
    }

    SpriteDrawCommand command = {};
    command.position = Vec2(position.x + size.x * 0.5f, position.y + size.y * 0.5f);
    command.size = size;
    command.color = color;
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
    command.position = Vec2((start.x + end.x) * 0.5f, (start.y + end.y) * 0.5f);
    command.size = Vec2(length, 2.0f);
    command.color = color;
    command.rotation = std::atan2(dy, dx);
    command.use_texture = false;
    ui_renderer_->DrawImmediate(command);
}

void Debug::DrawCircle(const Vec2& center, float radius, const Vec4& color)
{
    constexpr int kSegments = 32;
    constexpr float kTwoPi = 6.28318530718f;
    for (int i = 0; i < kSegments; ++i)
    {
        const float a0 = kTwoPi * static_cast<float>(i) / static_cast<float>(kSegments);
        const float a1 = kTwoPi * static_cast<float>(i + 1) / static_cast<float>(kSegments);
        const Vec2 p0(center.x + std::cos(a0) * radius, center.y + std::sin(a0) * radius);
        const Vec2 p1(center.x + std::cos(a1) * radius, center.y + std::sin(a1) * radius);
        DrawLine(p0, p1, color);
    }
}

void Debug::Watch(const char* name, float value)
{
    Log("%s: %.3f", name, value);
}
