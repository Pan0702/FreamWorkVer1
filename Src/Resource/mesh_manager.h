#pragma once
#include <unordered_map>

#include "mesh.h"
#include "../Graphics/graphics_device.h"
/**
 * @brief MeshManagerのデータと処理をまとめる型。
 */
class MeshManager
{
public:
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
    Mesh* Load(const std::string& path);
    
    static MeshManager& Get()
    {
        static MeshManager instance;
        return instance;
    }
    
private:
    ID3D12Device* device_ = nullptr;
    std::unordered_map<std::string,std::unique_ptr<Mesh>> cache_;
    
};
