#pragma once
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
     * @brief SkeletalMeshManagerを初期化するコンストラクタ。
     */
    SkeletalMeshManager();
    /**
     * @brief SkeletalMeshManagerの終了処理を行うデストラクタ。
     */
    ~SkeletalMeshManager();
    /**
     * @brief 初期化に必要な参照とリソースを設定する関数。
     * @param device DirectX 12 デバイス。
     */
    void Initialize(ID3D12Device* device);
    /**
     * @brief 保持しているリソースを解放する関数。
     */
    void Shutdown();
    /**
     * @brief 指定されたリソースを読み込む関数。
     * @param path 読み込むファイルパスまたは検索キー。
     * @return 戻り値。
     */
    SkeletalMesh* Load(const std::string& path);
    
    static SkeletalMeshManager& Get()
    {
        static SkeletalMeshManager instance;
        return instance;
    }
    
private:
    ID3D12Device* device_ = nullptr;
    std::unordered_map<std::string,std::unique_ptr<SkeletalMesh>> cache_;
};

