#include "scene.h"

#include <algorithm>

#include "../Resource/mesh.h"

void Scene::AddObject(RenderObject* object)
{
    render_objects_.push_back(object);
}

void Scene::RemoveObject(RenderObject* object)
{
    std::erase(render_objects_, object);
}

void Scene::Sort()
{
    if (needs_sort_)
    {
        std::ranges::sort(render_objects_,
                          [](const RenderObject* a, const RenderObject* b)
                          {
                              return a->GetPriority() < b->GetPriority();
                          });
    }
    
}

const std::vector<RenderObject*>& Scene::GetRenderObjects() const
{
    return render_objects_; 
}
