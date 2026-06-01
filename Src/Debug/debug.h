#pragma once
#include "../Core/common.h"
#include "../Core/Math/my_math.h"

class DebugLineRenderer;
class SpriteRenderer;
class Texture2D;
class UIRenderer;

class Debug
{
public:
    static Debug& Get();

    void Initialize(SpriteRenderer* sprite_renderer, UIRenderer* ui_renderer
        ,DebugLineRenderer* debug_line_renderer);
    void Shutdown();
    void Log(const char* format, ...);
    void Print(const char* format, ...);
    void DrawSprite(Texture2D* texture, const Vec2& position, const Vec2& size, const Vec4& color);
    void DrawBox(const Vec2& position, const Vec2& size, const Vec4& color, float rotation = 0.0f);
    void DrawLine(const Vec2& start, const Vec2& end, const Vec4& color);
    void DrawCircle(const Vec2& center, float radius, const Vec4& color);
    void DrawLine3D(const Vec3& start, const Vec3& end, const Vec4& color);
    void DrawBox3D(const Vec3& center, const Vec3& size, const Vec4& color, const Vec3& rotation, bool fill = true);
    void DrawSphere3D(const Vec3& center, float radius, const Vec4& color);
    void DrawSprite3D(Texture2D* texture, const Vec3& position, const Vec3& rotation, 
        const Vec2& size, const Vec2& src_pos, const Vec2& src_size, float alpha);
    void DrawSprite3D(Texture2D* texture,const Mat& mat,const Vec2& size, const Vec2& src_pos, const Vec2& src_size, float alpha);
    void Watch(const char* name, float value);

private:
    Debug() = default;

    SpriteRenderer* sprite_renderer_ = nullptr;
    UIRenderer* ui_renderer_ = nullptr;
    DebugLineRenderer* line_renderer_ = nullptr;
};

#ifdef _DEBUG
#define DEBUG_LOG(...) Debug::Get().Log(__VA_ARGS__)
#else
#define DEBUG_LOG(...) ((void)0)
#endif
