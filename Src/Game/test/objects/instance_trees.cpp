#include "instance_trees.h"

#include "../../../Engine/Components/instanced_static_mesh_component.h"
#include "constant.h"
InstanceTrees::InstanceTrees()
{
    Mesh* mesh = MeshManager::Get().Load("Assets/Mesh/tree.mesh");
    slot_ = std::make_unique<MaterialSlot>(mesh->GetMaterialDecs());
    
    auto* ism = AddComponent<InstancedStaticMeshComponent>(mesh, slot_.get());

    for (int w = kRange; w > -kRange; --w)
    {
        for (int h = kRange; h > -kRange; --h)
        {
            ism->AddInstance(
                Translate(Vec3(
                    static_cast<float>(w),
                    0.0f,
                    static_cast<float>(h))));
        }
    }
}
