#include "render_object.h"

void RenderObject::SetMesh(Mesh* mesh)
{
    mesh_ = mesh;
}

void RenderObject::SetMaterial(Material* material)
{
    material_ = material;  
}

void RenderObject::SetTransform(const Mat& transform)
{
    transform_ = transform; 
}

void RenderObject::SetPriority(int priority)
{
    priority_ = priority;
}

int RenderObject::GetPriority() const
{
    return priority_;
}

Mat RenderObject::GetTransform() const
{
    return transform_;
}

Mesh* RenderObject::GetMesh() const
{
    return mesh_; 
}

Material* RenderObject::GetMaterial() const
{
    return material_;
}
