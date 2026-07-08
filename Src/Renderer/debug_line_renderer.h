#pragma once
#include <memory>

#include "frame_snap.h"
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
     * @brief 利用前に必要な参照とリソースを初期化する。
     * @param device 使用する D3D12 デバイス。
     * @return 必要なリソースを作成し、使用可能な状態にできた場合は true。
     */
    bool Initialize(ID3D12Device* device);
    /**
     * @brief 保持しているリソースと登録状態を解放する。
     */
    void Shutdown();
    /**
     * @brief デバッグ描画用の線分を追加する。
     * @param start 線分または範囲の始点。
     * @param end 線分または範囲の終点。
     * @param color 設定する色。
     */
    void AddLine(const Vec3& start, const Vec3& end, const Vec4& color);
    /**
     * @brief デバッグ描画用の三角形を線分として追加する。
     * @param a 1 つ目のベクトルまたは点。
     * @param b 2 つ目のベクトルまたは点。
     * @param c 3 つ目のベクトルまたは点。
     * @param color 設定する色。
     */
    void AddTriangle(const Vec3& a, const Vec3& b, const Vec3& c, const Vec4& color);
    
    /**
     * @brief 収集済みコマンドを GPU コマンドリストへ書き込む。
     * @param write_snap 描画や登録に使う共有コンテキスト。
     */
    void Collect(FrameSnap& write_snap);
    /**
     * @brief 次フレームへ持ち越さない一時描画データを消す。
     */
    void Submit(const RenderContext& context, const FrameSnap& read_snap);

private:

    /**
     * @brief 内部で使用するリソースを作成する。
     * @param device 使用する D3D12 デバイス。
     * @param buffer_size 作成するバッファのバイト数。
     * @param out_buffer 作成したバッファを書き込む値。
     * @param out_mapped マップした CPU アドレスを書き込む値。
     * @param out_gpu_address GPU 仮想アドレスを書き込む値。
     * @return 対象リソースの作成が完了した場合は true。
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
