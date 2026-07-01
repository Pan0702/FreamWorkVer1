#pragma once
#include "../../Engine/actor.h"

class Explanation : public Actor
{
public:
    Explanation(std::wstring_view path);
    void Begin() override;
private:
    class SpriteComponent* explanation_ = nullptr;
    std::wstring path_;
};
