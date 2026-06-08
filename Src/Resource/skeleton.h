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
     * @brief Nodesを設定する関数。
     * @param nodes 引数。
     * @param skin_count 引数。
     */
    void SetNodes(std::vector<SkeletonNode> nodes, uint32 skin_count);
    /**
     * @brief SkinCountを取得する関数。
     * @return 戻り値。
     */
    uint32 GetSkinCount() const ;
    /**
     * @brief NodeIndexを取得する関数。
     * @param name 検索または識別に使う名前。
     * @return 戻り値。
     */
    int32 GetNodeIndex(const std::string& name) const;
    /**
     * @brief Nodesを取得する関数。
     * @return 戻り値。
     */
    const std::vector<SkeletonNode>& GetNodes() const;
private:
    std::vector<SkeletonNode> nodes_;
    std::unordered_map<std::string, int32> name_to_index_; 
    uint32 skin_count_ = 0;
};


