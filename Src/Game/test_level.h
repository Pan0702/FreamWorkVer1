#pragma once
#include <memory>
#include "../Engine/level_base.h"
#include "../Resource/material.h"

class MaterialSlot;

class TestLevel : public LevelBase
{
public:
    TestLevel();
    ~TestLevel() override;
    void OnEnter() override;
    void Tick(float dt) override;
private:
    std::unique_ptr<MaterialSlot> material_slot_;
};
