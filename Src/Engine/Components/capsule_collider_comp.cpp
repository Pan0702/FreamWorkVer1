#include "capsule_collider_comp.h"

#include <algorithm>
#include "../../Core/Math/intersect.h"
#include "../../Resource/skeltal_mesh.h"

CapsuleColliderComponent::CapsuleColliderComponent(SkeletalMesh* mesh)
{
    if (not mesh) return;
    AABB bounds = mesh->GetBounds();
    half_size = (bounds.max - bounds.min) * 0.5f;
    // 腕(x)ではなく薄い方(z)を半径に使う
    float radius = (std::min)(half_size.x, half_size.z);
    half_size.x = radius;
    half_size.z = radius;
    center_offset_ = (bounds.max + bounds.min) * 0.5f;
}

void CapsuleColliderComponent::SetHalfSize(const Vec3& size)
{
    half_size = size;
}

const Vec3& CapsuleColliderComponent::GetHalfSize() const
{
    return half_size;
}

ColliderShape CapsuleColliderComponent::GetColliderShape() const
{
    return ColliderShape::kCapsule;
}

Capsule CapsuleColliderComponent::GetColliderCapsuleData() const
{
    Vec3 center, scale;
    TryGetColliderTransform(&center, &scale); 
    Capsule c;
    c.center = center + center_offset_ * scale;
    c.dir = Vec3(0,1,0);
    
    // 半径
    float radius_scale = (std::max)(scale.x, scale.z);
    c.radius = half_size.x * radius_scale;

    float height = half_size.y * scale.y;
    c.height = height - c.radius;
    
    //半径より低かったら球にする
    c.height = (std::max)(c.height, 0.0f);
    return c;
}

void CapsuleColliderComponent::DrawDebug() const
{
    Capsule cap = GetColliderCapsuleData();                              
                                                                           
    // 両端の球の中心（GetColliderCapsuleData と同じ式）                 
    const Vec3 A = cap.center - cap.dir * cap.height; // 下              
    const Vec3 B = cap.center + cap.dir * cap.height; // 上              
                                                                           
    // 両端の半球（球で代用）                                            
    Debug::Get().DrawSphere3D(A, cap.radius, color_);                    
    Debug::Get().DrawSphere3D(B, cap.radius, color_);                    
                                                                           
    // 円柱部分の側面ライン：軸に直交する2方向に ±radius ずらした4本     
    const Vec3 up = cap.dir;                                             
    const Vec3 side = (std::fabs(up.x) < 0.99f)                          
                        ? Cross(up, Vec3(1, 0, 0)).Normalized()          
                        : Cross(up, Vec3(0, 0, 1)).Normalized();         
    const Vec3 fwd = Cross(up, side).Normalized();                       
                                                                           
    const Vec3 offsets[4] = {                                            
        side * cap.radius, -side * cap.radius,                           
        fwd  * cap.radius, -fwd  * cap.radius,                           
    };                                                                   
    for (const Vec3& o : offsets)                                        
    {                                                                    
        Debug::Get().DrawLine3D(A + o, B + o, color_);                   
    }
}
