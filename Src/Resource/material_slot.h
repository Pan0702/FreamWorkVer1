#pragma once
#include <vector>

#include "material.h"

class MaterialSlot
{
public:
    MaterialSlot(const std::vector<MeshMaterialDesc>& materials);
    ~MaterialSlot();
    Material* GetMaterial(uint32_t index) const;
private:
    std::vector<std::unique_ptr<Material>> materials_;
};
