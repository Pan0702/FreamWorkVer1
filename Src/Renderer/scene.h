#pragma once
#include <vector>
#include "render_object.h"

class Scene
{
public:
    void AddObject(RenderObject* object);
    void RemoveObject(RenderObject* object);
    void Sort();
    const std::vector<RenderObject*>& GetRenderObjects() const;
private:
    std::vector<RenderObject*> render_objects_;
    bool needs_sort_ = false;
};
