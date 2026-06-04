#include "test_level.h"
#include "../Engine/actor.h"
#include "../Engine/Components/transform_component.h"
#include "../Engine/Components/static_mesh_component.h"
#include "../Resource/mesh_manager.h"
#include "../Resource/material.h"
#include "../Resource/vertex_types.h"
#include "../Resource/texture_manager.h"
#include "../Engine/render_system.h"
#include "GameMain.h"
#include "../Debug/debug.h"
#include "../Debug/ImGui/imgui.h"

void TestLevel::OnEnter()
{
    // 1) メッシュをロード（引数は std::string！）
    Mesh* mesh = MeshManager::Get().Load("Mesh/box1.mesh");

    // 2) マテリアル作成（triangleシェーダ＝Lambert対応済み）
    material_ = std::make_unique<Material>();
    // 診断テクスチャを1枚（Material::Apply は SRV slot0 を使う実装なので、何か1枚要る）
    material_->SetDiffuse(TextureManager::Get().Load(L"Texture/clock.png"));
    material_->SetBaseColor(Vec4(1,0,0,1));
    // 3) Actor を生成して Mesh コンポーネントを載せる
    Actor* a = SpawnActor();
    auto* t = a->AddComponent<TransformComponent>();
    a->AddComponent<StaticMeshComponent>(mesh, material_.get());
    t->position = Vec3(0, 0, 0);
    LevelBase::OnEnter();
}

void TestLevel::Tick(float dt)
{
    static float num = 0;
    float radius = 5.0f; 
    static Vec3 target = Vec3(0, 0, 0);
    if (game_main->GetInput().CheckKey(Key::kA, KeyState::kDown))
    {
        num += 0.02f;
    }
    if (game_main->GetInput().CheckKey(Key::kD, KeyState::kDown))
    {
        num -= 0.02f;
    }
    Vec3 baseOffset = {0.0f, 0.0f, -radius};   
    Mat  rotY       = RotateY(num);          
    Vec3 offset     = TransformVector(rotY, baseOffset);  

    Vec3 cameraPos = target + offset;
    
    game_main->GetCamera().pos_ = cameraPos;
    game_main->GetCamera().look_ = target ;
    Debug::Get().DrawSphere3D(target, 1.0f, Vec4(1, 0, 0, 1));
 LevelBase::Tick(dt);
}
