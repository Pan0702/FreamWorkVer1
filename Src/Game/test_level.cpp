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
    ID3D12Device* device = game_main->GetRenderSystem()->GetDevice();
    material_ = std::make_unique<Material>();
    if (!material_->Create(device, L"Shaders/triangle.vs.hlsl", L"Shaders/triangle.ps.hlsl",
                      kStaticVertexLayout))
    {
        MessageBox(nullptr, L"Failed to create material", L"Error", MB_OK);
    }
    // 診断テクスチャを1枚（Material::Apply は SRV slot0 を使う実装なので、何か1枚要る）
    material_->SetDiffuse(TextureManager::Get().Load(L"Texture/clock.png"));

    // 3) Actor を生成して Mesh コンポーネントを載せる
    Actor* a = SpawnActor();
    auto* t = a->AddComponent<TransformComponent>();
    a->AddComponent<StaticMeshComponent>(mesh, material_.get());
    t->position = Vec3(0, 0, 0);
    LevelBase::OnEnter();
}

void TestLevel::Tick(float dt)
{
    static float num = 3;
    if (game_main->GetInput().CheckKey(Key::kA, KeyState::kDown))
    {
        num += 0.1f;
    }
    if (game_main->GetInput().CheckKey(Key::kD, KeyState::kDown))
    {
        num -= 0.1f;
    }
    game_main->GetCamera().pos_ = Vec3(0, 0, num);
    
    //Debug::Get().DrawBox3D(Vec3(0, 0, -5), Vec3(2, 2, 2), Vec4(1, 1, 1, 1), Vec3(0, 0, 0),true);
    LevelBase::Tick(dt);
}
