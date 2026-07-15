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
     * @brief 保持している登録やリソースを解放する。
     */
    ~TextureManager();
    /**
     * @brief 利用前に必要な参照とリソースを初期化する。
     * @param device 使用する D3D12 デバイス。
     * @param srv_heap SRV/CBV/UAV などを保持するディスクリプタヒープ。
     * @param queue GPU コマンドを実行するキュー。
     * @param cmd テクスチャ転送に使うコマンドリスト。
     */
    void Initialize(ID3D12Device* device, DescriptorHeap* srv_heap,
                    CommandQueue* queue, CommandList* cmd);
    /**
     * @brief 保持しているリソースと登録状態を解放する。
     */
    void Shutdown();
    /**
     * @brief ファイルや外部データを読み込んで内部表現へ変換する。
     * @param path 読み書きするファイルパス。
     * @param is_srgb 画像データがsrgbかどうか
     * @return 指定リソースの読み込みが完了した場合は true。
     */
    Texture2D* Load(const wchar_t* path, bool is_srgb = true);
    /**
     * @brief 読み込み済み画像からテクスチャを作成または取得する。
     * @param cache_key キャッシュに使用する識別子。`r`n     * @param image テクスチャ化する画像データ。`r`n     * @param is_srgb sRGB テクスチャとして扱う場合は true。
     * @return 作成または取得したテクスチャ。失敗時は nullptr。
     */
    Texture2D* CreateFromImage(const wchar_t* cache_key,const LoadedImage& image,bool is_srgb = false);
    /**
     * @brief TextureManager の共有インスタンスを取得する。
     * @return テクスチャキャッシュを管理する TextureManager への参照。
     */
    static TextureManager& Get()
    {
        static TextureManager instance;
        return instance;
    }

private:
    
    /**
     * @brief 読み込んだ画像からテクスチャを作成し、キャッシュへ登録する。
     */
    Texture2D* LoadTexture(const wchar_t* path, const LoadedImage& image, bool is_srgb);
    
    ComPtr<ID3D12CommandAllocator> upload_allocator_;
    ID3D12Device* device_ = nullptr;
    DescriptorHeap* srv_heap_ = nullptr;
    CommandQueue* queue_ = nullptr;
    CommandList* cmd_ = nullptr;
    std::unordered_map<std::wstring, std::unique_ptr<Texture2D>> cache_;
};
