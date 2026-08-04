#pragma once
#include "../../Engine/character.h"
class StateComponentBase;
class Player : public Character
{
public:
    Player();                    // Character("Assets/Mesh/remy.skmesh") + // アニメ5本登録 + scale 0.01
private:
    void Begin() override;       // 位置初期化 + camera_ 取得 (現行のまま)
    void Tick(float dt) override;
    class PlayerCamera* camera_ = nullptr;
};

