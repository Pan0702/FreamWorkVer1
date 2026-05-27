#include "mesh_renderer.h"
#include "../Core/Math/my_math.h"
#include "../Graphics/constant_buffer.h"
#include "render_object.h"
#include "../Engine/camera.h"
#include "../Resource/material.h"
#include "../Resource/mesh.h"

bool MeshRenderer::Initialize(ID3D12Device* device)
{
    constant_buffer_ = std::make_unique<ConstantBuffer>();
    if (!constant_buffer_->Initialize(device, sizeof(Mat)))
    {
        return false;
    }
    
    return true;
}

void MeshRenderer::Render(ID3D12GraphicsCommandList* command_list, const std::vector<RenderObject*>& render_objects,
    Camera* camera,DescriptorHeap* descriptor_heap)
{
    for (auto& object : render_objects)
    {
        Mat wvp = Transpose(object->GetTransform() * camera->GetViewMatrix() * camera->GetProjectionMatrix());
        constant_buffer_->Update(&wvp, sizeof(wvp));
        object->GetMaterial()->Apply(command_list,descriptor_heap);
        command_list->SetGraphicsRootConstantBufferView(0,  constant_buffer_->GetGpuAddress());
        D3D12_VERTEX_BUFFER_VIEW vbv = object->GetMesh()->GetVertexBufferView();
        command_list->IASetVertexBuffers(0, 1, &vbv);
        D3D12_INDEX_BUFFER_VIEW ibv = object->GetMesh()->GetIndexBufferView();
        command_list->IASetIndexBuffer(&ibv);
        command_list->DrawIndexedInstanced(object->GetMesh()->GetIndexCount(), 1, 0, 0, 0);
    }
}
