#pragma once
#include <memory>
#include <vector>
#include "../Core/common.h"
#include "../Core/Math/my_math.h"
#include "../Graphics/pipeline_state.h"
#include "../Graphics/root_signature.h"
#include "../Graphics/shader.h"

struct RenderContext;
/**
 * @brief DebugLineRendererのデータと処理をまとめる型。
 */
class DebugLineRenderer
{
public:
    /**
     * @brief 初期化に必要な参照とリソースを設定する関数。
     * @param device DirectX 12 デバイス。
     * @return 条件を満たす場合は true。
     */
    bool Initialize(ID3D12Device* device);
    /**
     * @brief 保持しているリソースを解放する関数。
     */
    void Shutdown();
    /**
     * @brief AddLineを行う関数。
     * @param start 引数。
     * @param end 引数。
     * @param color 色。
     */
    void AddLine(const Vec3& start, const Vec3& end, const Vec4& color);
    /**
     * @brief AddTriangleを行う関数。
     * @param a 引数。
     * @param b 引数。
     * @param c 引数。
     * @param color 色。
     */
    void AddTriangle(const Vec3& a, const Vec3& b, const Vec3& c, const Vec4& color);
    /**
     * @brief Submitを行う関数。
     * @param context 共有コンテキスト。
     */
    void Submit(RenderContext& context);
    /**
     * @brief Clearを行う関数。
     */
    void Clear();                                                                 

private:
/**
 * @brief DebugLineVertexのデータと処理をまとめる型。
 */
    struct DebugLineVertex
    {
        float position[3];
        float color[4];
    };
    /**
     * @brief CreateDynamicVertexBufferを行う関数。
     * @param device DirectX 12 デバイス。
     * @param buffer_size 引数。
     * @param out_buffer 引数。
     * @param out_mapped 引数。
     * @param out_gpu_address 引数。
     * @return 条件を満たす場合は true。
     */
    bool CreateDynamicVertexBuffer(ID3D12Device* device, uint32_t buffer_size,
                                   ComPtr<ID3D12Resource>& out_buffer, void*& out_mapped,
                                   D3D12_GPU_VIRTUAL_ADDRESS& out_gpu_address);

    std::unique_ptr<Shader> vs_;
    std::unique_ptr<Shader> ps_;
    std::unique_ptr<RootSignature> root_signature_;
    std::unique_ptr<PipelineState> pipeline_state_;    
    std::unique_ptr<PipelineState> tri_pipeline_state_;

    uint32_t capacity_ = 0;

    //線//
    ComPtr<ID3D12Resource> vertex_buffer_;
    void* mapped_ = nullptr;
    D3D12_GPU_VIRTUAL_ADDRESS gpu_address_ = 0;
    std::vector<DebugLineVertex> vertices_;

    //塗りつぶし三角形//
    ComPtr<ID3D12Resource> tri_vertex_buffer_;
    void* tri_mapped_ = nullptr;
    D3D12_GPU_VIRTUAL_ADDRESS tri_gpu_address_ = 0;
    std::vector<DebugLineVertex> tri_vertices_;
};