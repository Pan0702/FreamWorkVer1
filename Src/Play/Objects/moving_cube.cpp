#include "moving_cube.h"

MovingCube::MovingCube()
{
    moving_range_ = 10;
}

MovingCube::MovingCube(const Vec3& pos, int range)
{
    moving_range_ = range;
    base_pos_ = pos;
}



void MovingCube::Tick(float dt)
{
    waiting_time_+= dt;
    
    
    if (waiting_time_ < kQaitingTimeMax)
    {
        transform_.position.y += speed_ * dt;
        if (transform_.position.y > base_pos_.y + moving_range_)
        {
            waiting_time_ = 0;
            speed_ = -kMovingSpeed;
            transform_.position.y =  base_pos_.y + moving_range_;
        }
        else if (transform_.position.y < base_pos_.y)
        {
            waiting_time_ = 0;
            speed_ = kMovingSpeed;
            transform_.position.y = base_pos_.y;
        }
    }
    Actor::Tick(dt);
}
