#pragma once
#include <string>
#include <unordered_map>
#include "../component.h"

class MaterialSlot;
class SkeletalMesh;
class Animation;
class SkinnedMeshRenderer;

class SkeletalMeshComponent : public Component
{
public:
    SkeletalMeshComponent(SkeletalMesh* skeletal_mesh, MaterialSlot* material_slot);
    ~SkeletalMeshComponent() override;

    void OnAttach(const AttachContext& context) override;
    void OnDetach() override;
    Animation* GetAnimation(const std::string& name);
    void SetAnimation(const std::string& name, Animation* animation);
    SkeletalMesh* GetSkeltalMesh() const;
    MaterialSlot* GetMaterialSlot() const;
private:
    std::unordered_map<std::string,Animation*> animations_;
    SkeletalMesh* skeltal_mesh_ = nullptr;
    MaterialSlot* material_slot_ = nullptr;
    SkinnedMeshRenderer* renderer_ = nullptr;
};
