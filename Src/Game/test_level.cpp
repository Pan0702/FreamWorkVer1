#include "test_level.h"
#include "../Engine/actor.h"
#include "../Engine/Components/transform_component.h"
#include "../Engine/Components/static_mesh_component.h"
#include "../Resource/mesh_manager.h"
#include "../Resource/material.h"
#include "../Resource/material_slot.h"
#include "../Resource/texture_manager.h"
#include "GameMain.h"
#include "../Debug/debug.h"
#include "../Debug/ImGui/imgui.h"
#include "../Engine/Components/animation_component.h"
#include "../Resource/skeltal_mesh_manager.h"
#include "../Resource/skeltal_mesh.h"
#include "../Engine/Components/skeletal_mesh.h"
#include "../Resource/animator_manager.h"

TestLevel::TestLevel() = default;
TestLevel::~TestLevel() = default;

void TestLevel::OnEnter()
{
    Mesh* mesh = MeshManager::Get().Load("Assets/Mesh/box1.mesh");
    material_slot_ = std::make_unique<MaterialSlot>(mesh->GetMaterialDecs());
    material_slot_->GetMaterial(0)->SetDiffuse(TextureManager::Get().Load(L"Assets/Texture/NormalMap.png"));
    material_slot_->GetMaterial(0)->SetNormal(TextureManager::Get().Load(L"Assets/Texture/NormalMap.png"));

    Actor* a = SpawnActor();
    auto* t = a->AddComponent<TransformComponent>();
    a->AddComponent<StaticMeshComponent>(mesh, material_slot_.get());
    t->position = Vec3(0, 0, 0);

    Mesh* mesh2 = MeshManager::Get().Load("Assets/Mesh/box1.mesh");
    material_slot_2_ = std::make_unique<MaterialSlot>(mesh2->GetMaterialDecs());
    material_slot_2_->GetMaterial(0)->SetDiffuse(TextureManager::Get().Load(L"Assets/Texture/NormalMap.png"));

    Actor* a2 = SpawnActor();
    auto* t2 = a2->AddComponent<TransformComponent>();
    a2->AddComponent<StaticMeshComponent>(mesh, material_slot_2_.get());
    t2->position = Vec3(3, 0, 0);
    
    SkeletalMesh* sk = SkeletalMeshManager::Get().Load("Assets/Mesh/remy.skmesh");
    if (sk) // Manager 未初期化や読み込み失敗なら nullptr
    {
        skeletal_material_slot_ =
            std::make_unique<MaterialSlot>(sk->GetMaterialDecs());
        Actor* c = SpawnActor();
        auto* ct = c->AddComponent<TransformComponent>();
        c->AddComponent<SkeletalMeshComponent>(sk, skeletal_material_slot_.get());
        ct->rotation = Vec3(0, 180 * kDegToRad, 0);
        ct->scale = Vec3(0.01f, 0.01f, 0.01f); //Mixamoはcm単位＝約180単位の巨人。0.01で縮小
        
        auto* sk_mesh_comp = c->AddComponent<AnimationComponent>();
        const std::string gangnam = "gangnam";
        sk_mesh_comp->AddAnimation(gangnam,AnimatorManager::Get().Load("Assets/Mesh/gangnam.anim"),true);
       sk_mesh_comp->Play(gangnam);
    }
    
    LevelBase::OnEnter();
}

void TestLevel::Tick(float dt)
{
    static float num = 0;
    float radius = 5.0f;
    static Vec3 target = Vec3(0, 0, 0);
    constexpr float kSpeed =0.02f;
    if (game_main->GetInput().CheckKey(InputKey::kA, KeyState::kDown))
    {
        num += kSpeed;
    }
    if (game_main->GetInput().CheckKey(InputKey::kD, KeyState::kDown))
    {
        num -= kSpeed;
    }
    Vec3 baseOffset = {0.0f, 0.0f, -radius};
    Mat rotY = RotateY(num);
    Vec3 offset = TransformVector(rotY, baseOffset);

    Vec3 cameraPos = target + offset;

    game_main->GetCamera().pos_ = cameraPos;
    game_main->GetCamera().look_ = target;
    Debug::Get().DrawSphere3D(target, 1.0f, Vec4(1, 0, 0, 1));
    LevelBase::Tick(dt);
}
