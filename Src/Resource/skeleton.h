#pragma once
#include <string>
#include <vector>
#include <span>
#include <unordered_map>
#include "../Core/Math/my_math.h"
/**
 * @brief SkeletonNodeのデータと処理をまとめる型。
 */
struct SkeletonNode
{
    std::string name;
    int parent_index = -1;
    Mat local_bind_transform;
    
    int skin_index = -1;
    Mat inverse_bind_pose;
};
/**
 * @brief Skeletonのデータと処理をまとめる型。
 */
class Skeleton
{
public:
    /**
     * @brief 指定された値を内部状態に反映する。
     * @param nodes nodes に設定する値。
     * @param skin_count skin_count に設定する値。
     */
    void SetNodes(std::vector<SkeletonNode> nodes, uint32 skin_count);
    /**
     * @brief スキン数を取得する。
     * @return スケルトンが保持しているスキン数。
     */
    uint32 GetSkinCount() const ;
    /**
     * @brief 名前に対応するノード番号を取得する。
     * @param name name に設定する値。
     * @return 指定名に一致するノード番号。見つからない場合は -1。
     */
    int32 GetNodeIndex(const std::string& name) const;
    /**
     * @brief Nodes を取得する。
     * @return 保持している Nodes への参照。
     */
    const std::vector<SkeletonNode>& GetNodes() const;
private:
    std::vector<SkeletonNode> nodes_;
    std::unordered_map<std::string, int32> name_to_index_; 
    uint32 skin_count_ = 0;
};
