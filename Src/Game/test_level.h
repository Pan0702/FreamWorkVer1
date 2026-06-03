#pragma once
#include <memory>
#include "../Engine/level_base.h"
#include "../Resource/material.h"
class TestLevel : public LevelBase
{
public:
    void OnEnter() override;
    void Tick(float dt) override;
private:
    std::unique_ptr<Material> material_;  // Levelが所有（Actorが使う間 生存させる）
};
