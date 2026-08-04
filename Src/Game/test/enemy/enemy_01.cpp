#include "enemy_01.h"

Enemy01::Enemy01() : Character("Assets/Fbx/cupsule.mesh")
{
    transform_.scale = Vec3(0.01f, 0.01f, 0.01f);
    move_speed_ = 8.0f;
}

void Enemy01::Begin()
{
    transform_.position = Vec3(30.0f, 1.0f, 40.0f);
    Character::Begin();
}