#pragma once
#include <string>
#include <unordered_map>
#include "../component.h"

class MaterialSlot;
class SkeletalMesh;
class Animation;
class SkinnedMeshRenderer;

/**
 * @brief SkeletalMeshComponentのデータと処理をまとめる型。
 */
class SkeletalMeshComponent : public Component
{
public:
    /**
     * @brief 値を初期化する。
     * @param skeletal_mesh 描画に使うスケルタルメッシュ。
     * @param material_slot 描画に使用するマテリアル。
     */
    SkeletalMeshComponent(SkeletalMesh* skeletal_mesh, MaterialSlot* material_slot);
    /**
     * @brief 保持している登録やリソースを解放する。
     */
    ~SkeletalMeshComponent() override;
    
    /**
     * @brief スケルタルメッシュコンポーネントを更新する。
     * @param dt 前フレームからの経過秒数。
     */
    void Tick(float dt) override;

    /**
     * @brief 共有コンテキストへ登録し、システム側で扱える状態にする。
     * @param context 描画や登録に使う共有コンテキスト。
     */
    void OnAttach(const AttachContext& context) override;
    /**
     * @brief 共有コンテキストから登録を外し、システム側の参照を切る。
     */
    void OnDetach() override;
    /**
     * @brief アニメーション を取得する。
     * @param name 対象の名前。
     * @return アニメーション。見つからない、または未作成の場合は nullptr。
     */
    Animation* GetAnimation(const std::string& name);
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param name 対象の名前。
     * @param animation 追加または再生するアニメーション。
     */
    void SetAnimation(const std::string& name, Animation* animation);
    
    /**
     * @brief スケルタルメッシュのデバッグ情報を描画する。
     */
    void DebugDraw() const;
    /**
     * @brief Skeltal Mesh を取得する。
     * @return Skeltal Mesh。見つからない、または未作成の場合は nullptr。
     */
    SkeletalMesh* GetSkeletalMesh() const;
    /**
     * @brief マテリアルスロット を取得する。
     * @return マテリアルスロット。見つからない、または未作成の場合は nullptr。
     */
    MaterialSlot* GetMaterialSlot() const;

    /**
     * @brief スケルタルメッシュを描画するかどうかを設定する。
     * @param is_draw 描画を有効にする場合は true。
     */
    void SetDraw(bool is_draw);
    
    int sort_key = 0;
private:
    std::unordered_map<std::string, Animation*> animations_;
    SkeletalMesh* skeltal_mesh_ = nullptr;
    MaterialSlot* material_slot_ = nullptr;
    SkinnedMeshRenderer* renderer_ = nullptr;
    bool is_draw_ = false;
};
