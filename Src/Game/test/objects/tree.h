#pragma once
#include "../../../Engine/actor.h"

class Tree :public Actor
{
public:
    Tree();
    Tree(const Vec3& position);

private:
    std::unique_ptr<MaterialSlot> materials_;
};
