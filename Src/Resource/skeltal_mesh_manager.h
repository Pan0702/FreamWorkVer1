#pragma once
#include <unordered_map>

#include "skeltal_mesh.h"
#include "../Graphics/graphics_device.h"
/**
 * @brief SkeletalMeshManagerのデータと処理をまとめる型。
 */
class SkeletalMeshManager
{
public:
    /**
     * @brief 値を初期化する。
     */
    SkeletalMeshManager();
    /**
     * @brief 保持している登録やリソースを解放する。
     */
    ~SkeletalMeshManager();
    /**
     * @brief 利用前に必要な参照とリソースを初期化する。
     * @param device 使用する D3D12 デバイス。
     */
    void Initialize(ID3D12Device* device);
    /**
     * @brief 保持しているリソースと登録状態を解放する。
     */
    void Shutdown();
    /**
     * @brief ファイルや外部データを読み込んで内部表現へ変換する。
     * @param path 読み書きするファイルパス。
     * @return 指定リソースの読み込みが完了した場合は true。
     */
    SkeletalMesh* Load(const std::string& path);
    
    /**
     * @brief SkeletalMeshManager の共有インスタンスを取得する。
     * @return スケルタルメッシュキャッシュを管理する SkeletalMeshManager への参照。
     */
    static SkeletalMeshManager& Get()
    {
        static SkeletalMeshManager instance;
        return instance;
    }
    
private:
    ID3D12Device* device_ = nullptr;
    std::unordered_map<std::string,std::unique_ptr<SkeletalMesh>> cache_;
};
