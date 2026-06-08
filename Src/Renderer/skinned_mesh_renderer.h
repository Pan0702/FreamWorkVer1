#pragma once
#include <vector>
#include "../Core/common.h"
#include "draw_command.h"

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
     * @brief 初期化に必要な参照とリソースを設定する関数。
     * @param device DirectX 12 デバイス。
     * @return 条件を満たす場合は true。
     */
    bool Initialize(ID3D12Device* device);
    /**
     * @brief 登録済みの描画対象を描画する関数。
     * @param command_list DirectX 12 コマンドリスト。
     * @param render_objects 引数。
     * @param camera 引数。
     * @param descriptor_heap 引数。
     */
    void Render(ID3D12GraphicsCommandList* command_list, const std::vector<RenderObject*>& render_objects,
                Camera* camera, DescriptorHeap* descriptor_heap);
    /**
     * @brief Registerを行う関数。
     * @param component 引数。
     */
    void Register(SkeletalMeshComponent* component);
    /**
     * @brief Unregisterを行う関数。
     * @param component 引数。
     */
    void Unregister(SkeletalMeshComponent* component);
    /**
     * @brief Collectを行う関数。
     */
    void Collect();
    /**
     * @brief Sortを行う関数。
     */
    void Sort();
    /**
     * @brief Submitを行う関数。
     * @param context 共有コンテキスト。
     */
    void Submit(RenderContext& context);

private:
    std::vector<SkeletalMeshComponent*> meshes_;
    std::vector<SkinnedDrawCommand> draw_commands_;
    std::unique_ptr<RootSignature> root_signature_;   // ← 追加
    std::unique_ptr<PipelineState> pipeline_state_;   // ← 追加
};
