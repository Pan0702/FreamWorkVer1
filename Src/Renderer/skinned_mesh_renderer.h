#pragma once
#include <vector>
#include "../Core/common.h"
#include "frame_snap.h"

class PipelineState;
class RootSignature;
struct RenderContext;
class SkeletalMeshComponent;
class Camera;
class DescriptorHeap;
class RenderObject;
/**
 * @brief SkinnedMeshRendererのデータと処理をまとめる型。
 */
class SkinnedMeshRenderer
{
public:
    /**
     * @brief 利用前に必要な参照とリソースを初期化する。
     * @param device 使用する D3D12 デバイス。
     * @return 必要なリソースを作成し、使用可能な状態にできた場合は true。
     */
    bool Initialize(ID3D12Device* device);
    /**
     * @brief 現在の状態をもとに描画コマンドを積む。
     * @param command_list 描画コマンドを書き込むコマンドリスト。
     * @param render_objects 描画対象のオブジェクト配列。
     * @param camera ビュー行列と射影行列を提供するカメラ。
     * @param descriptor_heap SRV/CBV/UAV などを保持するディスクリプタヒープ。
     */
    void Render(ID3D12GraphicsCommandList* command_list, const std::vector<RenderObject*>& render_objects,
                Camera* camera, DescriptorHeap* descriptor_heap);
    /**
     * @brief 対象を管理リストへ登録する。
     * @param component 登録または解除する Component。
     */
    void Register(SkeletalMeshComponent* component);
    /**
     * @brief 対象を管理リストから外す。
     * @param component 登録または解除する Component。
     */
    void Unregister(SkeletalMeshComponent* component);
    /**
     * @brief 登録済みの対象から今回処理する要素を集める。
     */
    void Collect(FrameSnap& write_snap);
    /**
     * @brief 収集済みコマンドを GPU コマンドリストへ書き込む。
     * @param context 描画や登録に使う共有コンテキスト。
     * @param command
     * @param cam
     */
    void Submit(RenderContext& context, const SkinnedDrawCommand& command, const CameraSnap& cam) const;

    /**
     * @brief 収集済みのメッシュをライト視点から描画する
     * @param context 描画や登録に使う共有コンテキスト
     */
    void SubmitDepth( RenderContext& context,const FrameSnap& read_snap) const;
private:
    std::vector<SkeletalMeshComponent*> meshes_;
    std::unique_ptr<RootSignature> root_signature_;   
    std::unique_ptr<PipelineState> pipeline_state_; 
    
};
