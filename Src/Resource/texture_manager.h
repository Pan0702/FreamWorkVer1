#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include "../Core/common.h"
#include "texture2D.h"

class CommandList;
class CommandQueue;
class DescriptorHeap;
/**
 * @brief TextureManagerのデータと処理をまとめる型。
 */
class TextureManager
{
public:
    /**
     * @brief TextureManagerの終了処理を行うデストラクタ。
     */
    ~TextureManager();
    /**
     * @brief 初期化に必要な参照とリソースを設定する関数。
     * @param device DirectX 12 デバイス。
     * @param srv_heap 引数。
     * @param queue コマンド実行に使うキュー。
     * @param cmd コマンドリスト管理オブジェクト。
     */
    void Initialize(ID3D12Device* device, DescriptorHeap* srv_heap,
                    CommandQueue* queue, CommandList* cmd);
    /**
     * @brief 保持しているリソースを解放する関数。
     */
    void Shutdown();
    /**
     * @brief 指定されたリソースを読み込む関数。
     * @param path 読み込むファイルパスまたは検索キー。
     * @return 戻り値。
     */
    Texture2D* Load(const wchar_t* path);

    static TextureManager& Get()
    {
        static TextureManager instance;
        return instance;
    }

private:
    ID3D12Device* device_ = nullptr;
    DescriptorHeap* srv_heap_ = nullptr;
    CommandQueue* queue_ = nullptr;
    CommandList* cmd_ = nullptr;
    std::unordered_map<std::wstring, std::unique_ptr<Texture2D>> cache_;
};
