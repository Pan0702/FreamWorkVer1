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
     * @brief SkeletalMeshComponentを初期化するコンストラクタ。
     * @param skeletal_mesh 引数。
     * @param material_slot 引数。
     */
    SkeletalMeshComponent(SkeletalMesh* skeletal_mesh, MaterialSlot* material_slot);
    ~SkeletalMeshComponent() override;

    /**
     * @brief OnAttachを行う関数。
     * @param context 共有コンテキスト。
     */
    void OnAttach(const AttachContext& context) override;
    /**
     * @brief OnDetachを行う関数。
     */
    void OnDetach() override;
    /**
     * @brief Animationを取得する関数。
     * @param name 検索または識別に使う名前。
     * @return 戻り値。
     */
    Animation* GetAnimation(const std::string& name);
    /**
     * @brief Animationを設定する関数。
     * @param name 検索または識別に使う名前。
     * @param animation 引数。
     */
    void SetAnimation(const std::string& name, Animation* animation);
    /**
     * @brief SkeltalMeshを取得する関数。
     * @return 戻り値。
     */
    SkeletalMesh* GetSkeltalMesh() const;
    /**
     * @brief MaterialSlotを取得する関数。
     * @return 戻り値。
     */
    MaterialSlot* GetMaterialSlot() const;
private:
    std::unordered_map<std::string, Animation*> animations_;
    SkeletalMesh* skeltal_mesh_ = nullptr;
    MaterialSlot* material_slot_ = nullptr;
    SkinnedMeshRenderer* renderer_ = nullptr;
};
