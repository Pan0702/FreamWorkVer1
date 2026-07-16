#include "instance_trees.h"

#include "../../../Engine/Components/instanced_static_mesh_component.h"

InstanceTrees::InstanceTrees()
{
    Mesh* mesh = MeshManager::Get().Load("Assets/Mesh/plane.mesh");
    slot_ = std::make_unique<MaterialSlot>(mesh->GetMaterialDecs());
    
    auto* ism = AddComponent<InstancedStaticMeshComponent>(mesh, slot_.get());

    for (int w = 30; w > -30; --w)
    {
        for (int h = 30; h > -30; --h)
        {
            ism->AddInstance(
                Translate(Vec3(
                    static_cast<float>(w),
                    0.0f,
                    static_cast<float>(h))));
        }
    }
}
