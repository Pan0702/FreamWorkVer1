#include "test_level.h"
#include "../Engine/actor.h"
#include "../Engine/Components/transform_component.h"
#include "../Engine/Components/static_mesh_component.h"

#include "../Resource/material.h"
#include "../Resource/material_slot.h"
#include "../Resource/texture_manager.h"
#include "../Resource/mesh_manager.h"
#include "../Resource/skeltal_mesh_manager.h"
#include "GameMain.h"
#include "../Debug/debug.h"
#include "../Debug/ImGui/imgui.h"
#include "../Engine/Components/animation_component.h"
#include "../Engine/level_manager.h"
#include "../Resource/skeltal_mesh.h"
#include "../Engine/Components/skeletal_mesh.h"
#include "../Resource/animator_manager.h"
#include "../Engine/Components/sphere_collider_componet.h"
#include "../Engine/Components/box_collider_component.h"
#include "../Engine/Components/mesh_collider_component.h"
#include "../Play/Objects/goal_flag.h"
#include "../Play/UI/clear.h"
TestLevel::TestLevel() = default;
TestLevel::~TestLevel() = default;

void TestLevel::OnEnter()
{
    Mesh* mesh = MeshManager::Get().Load("Assets/Mesh/box1.mesh");
    material_slot_ = std::make_unique<MaterialSlot>(mesh->GetMaterialDecs());
    material_slot_->GetMaterial(0)->SetDiffuse(TextureManager::Get().Load(L"Assets/Texture/NormalMap.png"));
    material_slot_->GetMaterial(0)->SetNormal(TextureManager::Get().Load(L"Assets/Texture/NormalMap.png"));
    
    Actor* a = SpawnActor();
    auto* t = &a->GetTransform();
    a->AddComponent<StaticMeshComponent>(mesh, material_slot_.get());
    t->position = Vec3(0, 0, 0);
    
    Mesh* mesh2 = MeshManager::Get().Load("Assets/Mesh/box1.mesh");
    material_slot_2_ = std::make_unique<MaterialSlot>(mesh2->GetMaterialDecs());
    material_slot_2_->GetMaterial(0)->SetDiffuse(TextureManager::Get().Load(L"Assets/Texture/NormalMap.png"));
    
    Actor* a2 = SpawnActor();
    auto* t2 = &a2->GetTransform();
    a2->AddComponent<StaticMeshComponent>(mesh2, material_slot_2_.get());
    t2->position = Vec3(3, 2, 0);
    
    SkeletalMesh* sk = SkeletalMeshManager::Get().Load("Assets/Mesh/remy.skmesh");
    if (sk)
    {
        skeletal_material_slot_ =
            std::make_unique<MaterialSlot>(sk->GetMaterialDecs());
        Actor* c = SpawnActor();
        auto* ct = &c->GetTransform();
        c->AddComponent<SkeletalMeshComponent>(sk, skeletal_material_slot_.get());
        ct->rotation = Vec3(0, 180 * kDegToRad, 0);
        ct->scale = Vec3(0.01f, 0.01f, 0.01f); //Mixamo
        
        auto* sk_mesh_comp = c->AddComponent<AnimationComponent>();
        const std::string gangnam = "gangnam";
        sk_mesh_comp->AddAnimation(gangnam,AnimatorManager::Get().Load("Assets/Animation/gangnam.anim"),true);
       sk_mesh_comp->Play(gangnam);
    }

    // --- Collider 検証用（確認できたら消してよい） ---
    // 固定の Box: 原点から離して置く。動く球Bをここに突っ込ませて box-sphere を実走させる
    {
        Actor* sbox = SpawnActor();
        collider_test_transform_box_ = &sbox->GetTransform();
        collider_test_transform_box_->position = Vec3(-3, 2, 0);
        auto* col = sbox->AddComponent<BoxColliderComponent>();
        col->SetHalfSize(collider_test_box_half_);
        col->SetOnBeginOverlap([](const ColliderComponent*, const ColliderComponent*)
        {
            DEBUG_LOG("[Collider] Box BeginOverlap");
        });
        col->SetOnEndOverlap([](const ColliderComponent*, const ColliderComponent*)
        {
            DEBUG_LOG("[Collider] Box EndOverlap");
        });
    }
    // 固定の Mesh(壁quad): x=0 に立てる。動く球Bがここを通過すると Mesh-Sphere が発火
    {
        collider_test_mesh_ = a2->AddComponent<MeshColliderComponent>(mesh2);

        // YZ平面の壁（ローカル）。x=0 の面、y,z が ±2 の 2三角形
        // std::vector<Vec3> verts = {
        //     Vec3(0, -2, -2), Vec3(0, 2, -2), Vec3(0, 2, 2), Vec3(0, -2, 2),
        // };
        // std::vector<uint32> idx = {0, 1, 2, 0, 2, 3};
        // collider_test_mesh_->SetTriangles(std::move(verts), std::move(idx));

        collider_test_mesh_->SetOnBeginOverlap([](const ColliderComponent*, const ColliderComponent*)
        {
            DEBUG_LOG("[Collider] Mesh BeginOverlap");
        });
        collider_test_mesh_->SetOnEndOverlap([](const ColliderComponent*, const ColliderComponent*)
        {
            DEBUG_LOG("[Collider] Mesh EndOverlap");
        });
    }
    // 球B: J/L キーで左右に動かして Box に近づける
    {
        Actor* sb = SpawnActor();
        collider_test_transform_b_ = &sb->GetTransform();
        collider_test_transform_b_->position = Vec3(3, 2, 0);
        auto* col = sb->AddComponent<SphereColliderComponent>();
        col->SetRadius(collider_test_radius_b_);
        col->SetOnBeginOverlap([](const ColliderComponent*, const ColliderComponent*)
        {
            DEBUG_LOG("[Collider] Sphere B BeginOverlap");
        });
        col->SetOnEndOverlap([](const ColliderComponent*, const ColliderComponent*)
        {
            DEBUG_LOG("[Collider] Sphere B EndOverlap");
        });
    }

    SpawnActor<GoalFlag>();
    SpawnActor<Clear>();
    LevelBase::OnEnter();
}

void TestLevel::Tick(float dt)
{
    if (game_main->GetInput().CheckKey(InputKey::kEnter, KeyState::kDown))
    {
        game_main->GetGameInstance().GetLevelManager().OpenLevel("Play");
    }
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

    // --- Collider 検証用: 球B を J/L キーで左右に動かす ---
    if (collider_test_transform_b_)
    {
        constexpr float kMoveSpeed = 3.0f;
        if (game_main->GetInput().CheckKey(InputKey::kJ, KeyState::kDown))
        {
            collider_test_transform_b_->position.x -= kMoveSpeed * dt;
        }
        if (game_main->GetInput().CheckKey(InputKey::kL, KeyState::kDown))
        {
            collider_test_transform_b_->position.x += kMoveSpeed * dt;
        }
    }
    // --- Box を U/O キーで左右に動かす ---
    if (collider_test_transform_box_)
    {
        constexpr float kMoveSpeed = 3.0f;
        if (game_main->GetInput().CheckKey(InputKey::kU, KeyState::kDown))
        {
            collider_test_transform_box_->position.x -= kMoveSpeed * dt;
        }
        if (game_main->GetInput().CheckKey(InputKey::kO, KeyState::kDown))
        {
            collider_test_transform_box_->position.x += kMoveSpeed * dt;
        }
    }
    // Box と球を可視化（重なりを目で確認する）
    // DrawBox3D の size は全幅なので half×2。AABB は回転無視なので rotation は 0 で描く
    if (collider_test_transform_box_)
    {
        Debug::Get().DrawBox3D(collider_test_transform_box_->position,
                               collider_test_box_half_ * 2.0f, Vec4(0, 1, 0, 1),
                               Vec3(0, 0, 0), false);
    }
    if (collider_test_transform_b_)
    {
        Debug::Get().DrawSphere3D(collider_test_transform_b_->position,
                                  collider_test_radius_b_, Vec4(0, 0, 1, 1));
    }
    // Mesh(壁quad)を可視化: ワールド三角形をワイヤーで描く
    if (collider_test_mesh_)
    {
        const Mat w = collider_test_mesh_->GetWorldMatrix();
        const auto& v = collider_test_mesh_->GetVertices();
        const auto& id = collider_test_mesh_->GetIndices();
        for (size_t i = 0; i + 2 < id.size(); i += 3)
        {
            Vec3 a = TransformPoint(w, v[id[i]]);
            Vec3 b = TransformPoint(w, v[id[i + 1]]);
            Vec3 c = TransformPoint(w, v[id[i + 2]]);
            Debug::Get().DrawLine3D(a, b, Vec4(1, 1, 0, 1));
            Debug::Get().DrawLine3D(b, c, Vec4(1, 1, 0, 1));
            Debug::Get().DrawLine3D(c, a, Vec4(1, 1, 0, 1));
        }
    }

    LevelBase::Tick(dt);
}
