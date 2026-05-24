#pragma once
#include <cstdint>

enum class KeState : uint8_t
{
    Down,
    Pressed,
    Released
};
class Input
{
public:
    bool IsPushKey(uint8_t keycode,int key) const;
};
