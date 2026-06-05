#pragma once
#include <string>
#include <unordered_map>
#include "../component.h"

class SkeltalMesh;
class Animation;

class SkeletalMeshComponent : public Component
{
public:
    explicit SkeletalMeshComponent(SkeltalMesh* skeltal_mesh);
    ~SkeletalMeshComponent() override;

    void OnAttach(const AttachContext& context) override;
    void OnDetach() override;
    Animation* GetAnimation(const std::string& name);
    void SetAnimation(const std::string& name, Animation* animation);
private:
    std::unordered_map<std::string,Animation*> animations_;
    SkeltalMesh* skeltal_mesh_ = nullptr;
};
