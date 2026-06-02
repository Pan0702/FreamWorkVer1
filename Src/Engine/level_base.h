#pragma once
#include <string>

class World;

class LevelBase
{
public:
    LevelBase() : tag_("")
    {}
    ~LevelBase();
    virtual void Tick(float dt);

private:
    std::string tag_;
};
