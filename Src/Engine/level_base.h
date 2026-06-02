#pragma once
#include <string>

class LevelBase
{
public:
    LevelBase();
    virtual ~LevelBase();
    virtual void Tick(float dt){}

};
