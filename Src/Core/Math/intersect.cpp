#include "intersect.h"

#include <algorithm>

bool Intersect(const Sphere& sphere1, const Sphere& sphere2)
{
    float r = sphere1.radius + sphere2.radius;
    return (DistanceSquared(sphere1.center, sphere2.center) <= r * r);
}

bool Intersect(const Sphere& sphere, const AABB& box)
{
    Vec3 closest;
    closest.x = std::clamp(sphere.center.x,box.min.x,box.max.x);
    closest.y = std::clamp(sphere.center.y,box.min.y,box.max.y);
    closest.z = std::clamp(sphere.center.z,box.min.z,box.max.z);
    return DistanceSquared(closest, sphere.center) <= sphere.radius * sphere.radius;
}

bool Intersect(const AABB& box1, const AABB& box2)
{
    if (box1.max.x < box2.min.x || box1.min.x > box2.max.x) return false;
    if (box1.max.y < box2.min.y || box1.min.y > box2.max.y) return false;
    if (box1.max.z < box2.min.z || box1.min.z > box2.max.z) return false;
    return true;
}
