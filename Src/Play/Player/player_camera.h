#pragma once
#include "../../Game/GameMain.h"

// PlayerCamera に関係する状態と振る舞いをまとめる型。
class PlayerCamera : public Actor
{
public:
    float GetYaw() const;
private:
    /**
     * @brief 生成または遷移直後に必要な初期処理を行う。
     */
    void Begin() override;
    /**
     * @brief 1 フレーム分の状態更新を進める。
     * @param dt 前フレームからの経過秒数。
     */
    void Tick(float dt) override;
    void Input();
    Camera* camera_ = nullptr;
    class Player* player_ = nullptr;
    Vec2 cam_rad_;
};
