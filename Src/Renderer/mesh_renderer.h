#pragma once
#include <vector>

#include "../Core/common.h"
#include "frame_snap.h"

class Camera;
class DescriptorHeap;
struct RenderContext;
class RenderObject;
class StaticMeshComponent;
/**
 * @brief MeshRendererのデータと処理をまとめる型。
 */
class MeshRenderer
{
public:
    /**
     * @brief 利用前に必要な参照とリソースを初期化する。
     * @param device 使用する D3D12 デバイス。
     * @return 必要なリソースを作成し、使用可能な状態にできた場合は true。
     */
    bool Initialize(const ID3D12Device* device);
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
    void Register(StaticMeshComponent* component);
    /**
     * @brief 対象を管理リストから外す。
     * @param component 登録または解除する Component。
     */
    void Unregister(StaticMeshComponent* component);
    /**
     * @brief 登録済みの対象から今回処理する要素を集める。
     */
    void Collect(FrameSnap& write_snap);
    
    /**
     * @brief 静的メッシュの描画コマンドを GPU コマンドリストへ記録する。
     * @param context 描画に使用するコンテキスト。`r`n     * @param command 実行するメッシュ描画コマンド。`r`n     * @param cam 描画に使用するカメラ情報。
     */
    void Submit(RenderContext& context, const MeshDrawCommand& command, const CameraSnap& cam);


    /**
     * @brief 収集済みコマンドを GPU コマンドリストへ書き込む。
     * @param context 描画や登録に使う共有コンテキスト。
     */
    void SubmitDepth( RenderContext& context, const FrameSnap& read_snap);
private:
    std::vector<StaticMeshComponent*> registered_;

};
