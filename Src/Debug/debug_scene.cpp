#include "debug_scene.h"
#include "debug.h"
void DebugComponent::Tick(float dt)
{
    time_ += dt;
    Render();
}

void DebugComponent::Render()
{
    Debug& debug = Debug::Get();

    // 1. 静止した箱：そもそも DrawBox が出るか
    debug.DrawBox(Vec2(100.0f, 100.0f), Vec2(200.0f, 150.0f),
                  Vec4(1.0f, 0.0f, 0.0f, 1.0f), time_);   // 赤

    // 2. 動く箱：毎フレーム呼べているか（time_ で揺らす）
    const float x = 400.0f + std::sin(time_) * 100.0f;
    debug.DrawBox(Vec2(x, 300.0f), Vec2(50.0f, 50.0f),
                  Vec4(0.0f, 1.0f, 0.0f, 1.0f));    // 緑

    // 3. 線と円：他の Debug API も機能するか
    debug.DrawLine(Vec2(0, 0), Vec2(500, 500), Vec4(1, 1, 0, 1));
    debug.DrawCircle(Vec2(640, 360), 80.0f, Vec4(0, 1, 1, 1));
}
