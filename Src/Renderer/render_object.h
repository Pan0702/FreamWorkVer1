#pragma once
#include "../Core/Math/my_math.h"
class Material;
class Mesh;

class RenderObject
{
public:
    void SetMesh(Mesh* mesh);
    void SetMaterial(Material* material);
    void SetTransform(const Mat& transform);
    void SetPriority(int priority);
    int GetPriority() const;
    Mat GetTransform() const;
    Mesh* GetMesh() const;
    Material* GetMaterial() const;
    
private:
    Mat transform_ = Identity();
    Mesh* mesh_ = nullptr;
    Material* material_ = nullptr;
    int priority_ = 0;
    
};
