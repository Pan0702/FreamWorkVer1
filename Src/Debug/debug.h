#pragma once
#include "../Core/common.h"
#include "../Core/Math/my_math.h"

class SpriteRenderer;
class Texture2D;
class UIRenderer;

class Debug
{
public:
    static Debug& Get();

    void Initialize(SpriteRenderer* sprite_renderer, UIRenderer* ui_renderer);
    void Shutdown();
    void Log(const char* format, ...);
    void Print(const char* format, ...);
    void DrawSprite(Texture2D* texture, const Vec2& position, const Vec2& size, const Vec4& color);
    void DrawBox(const Vec2& position, const Vec2& size, const Vec4& color);
    void DrawLine(const Vec2& start, const Vec2& end, const Vec4& color);
    void DrawCircle(const Vec2& center, float radius, const Vec4& color);
    void Watch(const char* name, float value);

private:
    Debug() = default;

    SpriteRenderer* sprite_renderer_ = nullptr;
    UIRenderer* ui_renderer_ = nullptr;
};

#ifdef _DEBUG
#define DEBUG_LOG(...) Debug::Get().Log(__VA_ARGS__)
#else
#define DEBUG_LOG(...) ((void)0)
#endif
