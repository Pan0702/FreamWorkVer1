#include "material_slot.h"

MaterialSlot::MaterialSlot(const std::vector<MeshMaterialDesc>& materials)
{
    for (const MeshMaterialDesc& material_desc : materials)
    {
        std::unique_ptr<Material> material = std::make_unique<Material>(material_desc);
        materials_.push_back(std::move(material));
    }
}

MaterialSlot::~MaterialSlot() = default;

Material* MaterialSlot::GetMaterial(uint32_t index) const
{
    if (index >= materials_.size())
    {
        return nullptr;
    }
    return materials_[index].get();
}
