#include "debug_scene.h"
#include "debug.h"
#include "../Resource/texture_manager.h"

void DebugComponent::Tick(float dt)
{
    time_ += dt;
    Render();
}

void DebugComponent::Render()
{
    Debug& debug = Debug::Get();

    // 1. Static box: verifies DrawBox output.
    debug.DrawBox(Vec2(100.0f, 100.0f), Vec2(200.0f, 150.0f),
                  Vec4(1.0f, 0.0f, 0.0f, 1.0f)); // Red

    // 2. Animated box: verifies per-frame rendering.
    const float x = 400.0f + std::sin(time_) * 100.0f;
    debug.DrawBox(Vec2(x, 300.0f), Vec2(50.0f, 50.0f),
                  Vec4(0.0f, 1.0f, 0.0f, 1.0f)); // Green

    // 3. Line and circle: verifies other Debug APIs.
    debug.DrawLine(Vec2(0, 0), Vec2(500, 500), Vec4(1, 1, 0, 1));
    debug.DrawCircle(Vec2(100, 100), 10.0f, Vec4(0, 1, 1, 1));
    
    Texture2D* tex = TextureManager::Get().Load(L"Texture/clock.png");
    debug.DrawSprite3D(tex,Vec3(0.0f, 0.0f, 0.0f),Vec3(0.0f, time_, time_),
                       Vec2(1.0f, 1.0f),Vec2(0.0f, 0.0f),Vec2(1.0f, 1.0f),1.0f);
    
    debug.DrawBox3D(Vec3(0, 0, -5), Vec3(2, 2, 2), Vec4(1, 1, 1, 1), Vec3(0, 45.0f * kDegToRad * time_, 0),true);
}