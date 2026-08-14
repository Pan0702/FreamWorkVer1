#include "enemy_01.h"

Enemy01::Enemy01() : Character("Assets/Fbx/cupsule.mesh")
{
    // メッシュがcm単位で作られているため、ワールド単位へ縮める。
    transform_.scale = Vec3(0.01f, 0.01f, 0.01f);
    // プレイヤー(10.0)より遅くして、追いかけっこが成立するようにする。
    move_speed_ = 8.0f;
}

void Enemy01::Begin()
{
    // プレイヤーの初期位置(原点)から離れた地点に出し、追跡の様子を見えるようにする。
    // Yを少し上げているのは、床にめり込んだ状態で始まらないようにするため。
    transform_.position = Vec3(30.0f, 1.0f, 40.0f);
    Character::Begin();
}
