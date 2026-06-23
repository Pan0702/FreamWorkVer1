#pragma once
#include <vector>

#include "../Core/common.h"
#include "draw_command.h"

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
     * @param render_objects render_objects に設定する値。
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
    void Collect();
    /**
     * @brief 描画順が安定するようにコマンドを並べ替える。
     */
    void Sort();
    /**
     * @brief 収集済みコマンドを GPU コマンドリストへ書き込む。
     * @param context 描画や登録に使う共有コンテキスト。
     */
    void Submit(RenderContext& context) const;

private:
    std::vector<StaticMeshComponent*> registered_;
    std::vector<DrawCommand> draw_commands_;
};
