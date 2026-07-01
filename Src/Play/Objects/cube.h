#pragma once
#include <memory>

#include "../../Engine/actor.h"

class MaterialSlot;

// Cube ‚ÉŠÖŒW‚·‚éó‘Ô‚ÆU‚é•‘‚¢‚ğ‚Ü‚Æ‚ß‚éŒ^B
class Cube : public Actor
{
public:
    /**
     * @brief ’l‚ğ‰Šú‰»‚·‚éB
     */
    Cube();
    
    Cube(const Vec3& pos, const Vec3& scale);
    explicit Cube(const std::string& name);
private:
    std::unique_ptr<MaterialSlot> materials_;
};
