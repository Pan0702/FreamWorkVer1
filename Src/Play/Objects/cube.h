#pragma once
#include <memory>

#include "../../Engine/actor.h"

class MaterialSlot;

// Cube に関係する状態と振る舞いをまとめる型。
class Cube : public Actor
{
public:
    /**
     * @brief インスタンスの初期状態を整える。
     */
    Cube();
    
private:
    std::unique_ptr<MaterialSlot> materials_;
};
