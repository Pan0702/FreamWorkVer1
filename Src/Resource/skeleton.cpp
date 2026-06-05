#include "skeleton.h"

void Skeleton::SetNodes(const std::vector<SkeletonNode>& nodes, uint32 skin_count)
{
    nodes_ = std::move(nodes);
    skin_count_ = skin_count;
    name_to_index_.clear();
    for (int32 i = 0; i < static_cast<int32>(nodes_.size()); ++i)
    {
        name_to_index_[nodes_[i].name] = i;
    }
}

uint32 Skeleton::GetSkinCount() const
{
    return skin_count_;
}

int32 Skeleton::GetNodeIndex(const std::string& name) const
{
    auto it = name_to_index_.find(name);
    if (it != name_to_index_.end())
    {
        return it->second;
    }
    return -1;
}

const std::vector<SkeletonNode>& Skeleton::GetNodes() const
{
    return nodes_;
}
