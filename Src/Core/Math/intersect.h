#pragma once
#include "my_math.h"

struct AABB
{
    Vec3 min;
    Vec3 max;
};

struct Sphere
{
    Vec3 center;
    float radius;
};

bool Intersect(const Sphere& sphere1, const Sphere& sphere2);
bool Intersect(const Sphere& sphere, const AABB& box);
bool Intersect(const AABB& box1, const AABB& box2);
